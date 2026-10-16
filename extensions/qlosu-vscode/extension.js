const vscode = require('vscode');
const path = require('path');
const fs = require('fs');
const { diagramHtml: renderDiagram } = require('./diagram');
const { apiEndpoint } = require('./api');
const { openManual } = require('./manual');
const {
  LosuAgentProvider,
  createAgentTools,
  defaultProvider,
  normalizeProvider,
  trimForContext,
} = require('./agent');
const { LanguageClient, TransportKind } = require('vscode-languageclient/node');

const decoder = new TextDecoder();

let client;
let extensionContext;
let agentProvider;
let agentApiKey = '';
let lastLosuDocument;
let outputChannel;
const diagramPanels = new Map();
const diagramRefreshTimers = new Map();

function getOutputChannel() {
  if (!outputChannel) {
    outputChannel = vscode.window.createOutputChannel('QLosu');
    extensionContext?.subscriptions.push(outputChannel);
  }
  return outputChannel;
}

function logError(message, error) {
  const output = getOutputChannel();
  output.appendLine(`[${new Date().toISOString()}] ${message}`);
  if (error) output.appendLine(error.stack || error.message || String(error));
  output.show(true);
}

function serverPath() {
  const configured = vscode.workspace.getConfiguration('qlosu').get('languageServer');
  const projectRoot = process.env.QLOSU_ROOT || path.join(__dirname, '..', '..');
  const bundled = path.join(__dirname, 'language-server', 'server.js');
  return configured || (fs.existsSync(bundled) ? bundled : path.join(projectRoot, 'language-server', 'losu-lsp', 'server.js'));
}

function rememberEditor(editor) {
  const document = editor?.document;
  if (!document || document.languageId !== 'losu') return;
  lastLosuDocument = document;
  if (document.uri.scheme === 'file') extensionContext?.workspaceState.update('qlosu.lastFile', document.uri.toString());
}

async function resolveDocument() {
  let document = getActiveDocument();
  if (!document) {
    const stored = extensionContext?.workspaceState.get('qlosu.lastFile');
    if (stored) {
      try { document = await vscode.workspace.openTextDocument(vscode.Uri.parse(stored)); lastLosuDocument = document; }
      catch { await extensionContext.workspaceState.update('qlosu.lastFile', undefined); }
    }
  }
  if (!document) vscode.window.showWarningMessage('请先打开一个洛书文件，之后工作台会默认使用上次文件。');
  return document;
}

async function selectedVersion(document) {
  const folder = document && vscode.workspace.getWorkspaceFolder(document.uri) || vscode.workspace.workspaceFolders?.[0];
  const settings = vscode.workspace.getConfiguration('qlosu', document?.uri);
  const inspection = settings.inspect?.('runtimeVersion');
  const explicit = inspection?.workspaceFolderValue ?? inspection?.workspaceValue ?? inspection?.globalValue;
  if (explicit) return explicit;
  try {
    const config = JSON.parse(decoder.decode(await vscode.workspace.fs.readFile(vscode.Uri.joinPath(folder.uri, 'qlosu.json'))));
    if (config.losuVersion) return config.losuVersion;
  } catch { /* Project configuration is optional. */ }
  return settings.get('runtimeVersion', '2.0.0');
}

async function selectRuntime() {
  try {
    const response = await fetch(apiEndpoint(serverBase(), '/api/runtime/versions'));
    const data = await response.json();
    if (!response.ok) throw new Error(data.error || '无法读取运行时列表');
    const current = await selectedVersion(getActiveDocument());
    const items = data.versions.map((runtime) => ({
      id: runtime.id, label: runtime.label,
      description: `${runtime.id === current || runtime.id === `losu-${current}` ? '当前 · ' : ''}${runtime.runnable ?? runtime.ok ? '可运行' : '未就绪'}`,
      detail: runtime.message, runnable: runtime.runnable ?? runtime.ok,
    }));
    const chosen = await vscode.window.showQuickPick(items, { title: '选择洛书运行时', placeHolder: '选择已安装的运行时版本', matchOnDetail: true });
    if (!chosen) return;
    if (!chosen.runnable) return vscode.window.showWarningMessage(chosen.detail || '此运行时尚未安装。');
    await vscode.workspace.getConfiguration('qlosu').update('runtimeVersion', chosen.id, vscode.workspace.workspaceFolders?.length ? vscode.ConfigurationTarget.Workspace : vscode.ConfigurationTarget.Global);
    vscode.window.showInformationMessage(`已选择 ${chosen.label}`);
  } catch (error) { vscode.window.showErrorMessage(`读取运行时失败：${error.message}`); }
}

async function runFile() {
  const document = await resolveDocument();
  if (!document) return;
  try {
    const result = await callServer('/api/runtime/run', { code: document.getText(), version: await selectedVersion(document) });
    const channel = getOutputChannel();
    channel.appendLine(`\n▶ ${path.basename(document.fileName)} · ${result.version}`);
    if (result.stdout) channel.appendLine(result.stdout);
    if (result.stderr) channel.appendLine(result.stderr);
    channel.appendLine(result.timedOut ? '运行超时' : `退出码：${result.code ?? result.ret ?? 0}`);
    channel.show(true);
  } catch (error) { vscode.window.showErrorMessage(error.message); }
}

function openTerminal() {
  const folder = vscode.workspace.workspaceFolders?.[0];
  const terminal = vscode.window.activeTerminal || vscode.window.createTerminal({ name: '洛书终端', cwd: folder?.uri });
  terminal.show();
}

async function toggleWorkbenchMode() {
  const configuration = vscode.workspace.getConfiguration('qlosu');
  const enabled = !configuration.get('workbenchMode', true);
  await configuration.update('workbenchMode', enabled, vscode.ConfigurationTarget.Global);
  const updates = enabled
    ? {
      'window.title': '洛书 IDE — ${activeEditorShort}',
      'workbench.startupEditor': 'none',
      'window.commandCenter': false,
      'workbench.layoutControl.enabled': false,
      'workbench.tips.enabled': false,
      'extensions.ignoreRecommendations': true,
      'telemetry.telemetryLevel': 'off',
    }
    : {
      'window.title': '${activeEditorShort} - ${rootName}',
      'workbench.startupEditor': 'welcomePage',
      'window.commandCenter': true,
      'workbench.layoutControl.enabled': true,
      'workbench.tips.enabled': true,
      'extensions.ignoreRecommendations': false,
      'telemetry.telemetryLevel': 'all',
    };
  for (const [key, value] of Object.entries(updates)) {
    await vscode.workspace.getConfiguration().update(key, value, vscode.ConfigurationTarget.Global);
  }
  const mode = enabled ? '洛书工作台' : '标准 VS Code 工作台';
  vscode.window.showInformationMessage(`已切换到${mode}，重新加载窗口后生效。`);
}

function focusLosuWorkbench() {
  const configuration = vscode.workspace.getConfiguration('qlosu');
  if (!configuration.get('workbenchMode', true) || !configuration.get('autoFocusWorkbench', true)) return;
  setTimeout(() => {
    vscode.commands.executeCommand('workbench.view.extension.qlosu').then(undefined, (error) => logError('无法聚焦洛书工作台视图', error));
  }, 0);
}

async function callServer(route, body) {
  const base = serverBase();
  const response = await fetch(apiEndpoint(base, route), { method: 'POST', headers: { 'content-type': 'application/json' }, body: JSON.stringify(body) });
  const data = await response.json();
    if (!response.ok && !(route === '/api/runtime/run' && typeof data.code === 'number')) throw new Error(data.error || `QLosu 服务错误（${response.status}）`);
  return data;
}

function serverBase() {
  return process.env.QLOSU_SERVER_URL || vscode.workspace.getConfiguration('qlosu').get('serverUrl') || 'http://127.0.0.1:8080';
}

function diagramKey(kind, document) {
  return `${kind}:${document.uri.toString()}`;
}

async function diagramRequest(kind, code, document) {
  return kind === 'circuit'
    ? callServer('/api/trace', { code, version: await selectedVersion(document), theme: vscode.window.activeColorTheme?.kind === 1 ? 'light' : 'dark' })
    : callServer('/api/flowchart', { code, title: document.fileName });
}

async function refreshDiagramPanel(entry, document = entry.document) {
  if (!entry || entry.panel?.visible === false && entry.disposed) return;
  const generation = ++entry.generation;
  entry.document = document;
  entry.panel?.webview.postMessage({ type: 'refreshing' });
  try {
    const code = document.getText();
    const result = await diagramRequest(entry.kind, code, document);
    if (entry.disposed || generation !== entry.generation) return;
    entry.panel.webview.html = diagramHtml(entry.kind, code, document.fileName, result, entry.panel.webview);
  } catch (error) {
    if (entry.disposed || generation !== entry.generation) return;
    entry.panel?.webview.postMessage({ type: 'refreshError', message: error.message });
    logError(`刷新${entry.kind === 'circuit' ? '量子线路' : '程序流程图'}失败`, error);
  }
}

function registerDiagramPanel(kind, document, result, code) {
  const key = diagramKey(kind, document);
  const existing = diagramPanels.get(key);
  if (existing && !existing.disposed) {
    existing.panel.reveal(vscode.ViewColumn.Beside);
    existing.document = document;
    existing.panel.webview.html = diagramHtml(kind, code, document.fileName, result, existing.panel.webview);
    return existing;
  }
  const panel = vscode.window.createWebviewPanel(`qlosu.${kind}`, `${document.fileName.split(/[\\/]/).pop()} · ${kind === 'circuit' ? '量子线路' : '程序流程'}`, vscode.ViewColumn.Beside, { enableScripts: true, retainContextWhenHidden: true, localResourceRoots: [vscode.Uri.joinPath(extensionContext.extensionUri, 'vendor')] });
  const entry = { key, kind, document, panel, generation: 0, disposed: false };
  diagramPanels.set(key, entry);
  panel.onDidDispose(() => {
    entry.disposed = true;
    if (diagramPanels.get(key) === entry) diagramPanels.delete(key);
  });
  panel.webview.onDidReceiveMessage((message) => {
    if (message?.type === 'refresh') refreshDiagramPanel(entry);
  });
  panel.webview.html = diagramHtml(kind, code, document.fileName, result, panel.webview);
  return entry;
}

async function openDiagram(kind) {
  const document = await resolveDocument();
  if (!document) return;
  const code = document.getText();
  const key = diagramKey(kind, document);
  const existing = diagramPanels.get(key);
  if (existing && !existing.disposed) {
    existing.panel.reveal(vscode.ViewColumn.Beside);
    await refreshDiagramPanel(existing, document);
    return;
  }
  try {
    const result = await diagramRequest(kind, code, document);
    registerDiagramPanel(kind, document, result, code);
  } catch (error) { vscode.window.showErrorMessage(`生成${kind === 'circuit' ? '量子线路' : '程序流程图'}失败：${error.message}`); }
}

function scheduleDiagramRefresh(document) {
  if (!document || document.languageId !== 'losu') return;
  for (const kind of ['circuit', 'flowchart']) {
    const key = diagramKey(kind, document);
    if (!diagramPanels.has(key)) continue;
    clearTimeout(diagramRefreshTimers.get(key));
    diagramRefreshTimers.set(key, setTimeout(() => {
      diagramRefreshTimers.delete(key);
      const entry = diagramPanels.get(key);
      if (entry && !entry.disposed) refreshDiagramPanel(entry, document);
    }, 350));
  }
}

function diagramHtml(kind, code, fileName, result, webview) {
  const viewer = webview.asWebviewUri(vscode.Uri.joinPath(extensionContext.extensionUri, 'vendor', 'drawio', 'viewer-static.min.js'));
  return renderDiagram(kind, fileName, result, { viewer, cspSource: webview.cspSource });
}

async function openSyntaxManual() {
  return openManual({ vscode, context: extensionContext, version: await selectedVersion(getActiveDocument()) });
}

function escapeHtml(value) { return String(value).replace(/[&<>"']/g, (char) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[char])); }

class QLosuViewProvider {
  constructor(context) { this.context = context; }

  resolveWebviewView(view) {
    view.webview.options = { enableScripts: true, localResourceRoots: [this.context.extensionUri] };
    const logo = view.webview.asWebviewUri(vscode.Uri.joinPath(this.context.extensionUri, 'assets', 'losu-logo.png'));
    view.webview.html = `<!doctype html><html><meta charset="utf-8"><style>body{font:13px var(--vscode-font-family);color:var(--vscode-foreground);padding:12px}.brand{display:flex;gap:10px;align-items:center;margin-bottom:16px}.brand img{width:36px;height:36px;border-radius:50%;background:#fff}.brand strong{font-size:15px}.actions{display:grid;gap:6px}button{padding:7px 9px;text-align:left;color:inherit;background:var(--vscode-button-secondaryBackground);border:1px solid var(--vscode-panel-border);cursor:pointer}button:hover{background:var(--vscode-button-secondaryHoverBackground)}</style><div class="brand"><img src="${logo}" alt="洛书"><strong>洛书工作台</strong></div><div class="actions"><button data-command="qlosu.openAgent">打开 AI Agent</button><button data-command="qlosu.runFile">运行上次文件</button><button data-command="qlosu.openTerminal">打开终端</button><button data-command="qlosu.generateCircuit">打开量子线路</button><button data-command="qlosu.generateFlowchart">打开程序流程图</button><button data-command="qlosu.openSyntaxManual">打开语法手册</button><button data-command="qlosu.openUserGuide">使用说明书</button><button data-command="qlosu.selectRuntime">选择运行时版本</button></div><script>const vscode=acquireVsCodeApi();document.querySelectorAll('[data-command]').forEach(b=>b.onclick=()=>vscode.postMessage({command:b.dataset.command}));</script></html>`;
    const commands = new Set(['qlosu.openUserGuide', 'qlosu.openTerminal', 'qlosu.openAgent', 'qlosu.runFile', 'qlosu.generateCircuit', 'qlosu.generateFlowchart', 'qlosu.openSyntaxManual', 'qlosu.selectRuntime']);
    this.context.subscriptions.push(view.webview.onDidReceiveMessage((message) => {
      if (message && commands.has(message.command)) vscode.commands.executeCommand(message.command);
    }));
  }
}

function agentProviderConfig() {
  const configured = extensionContext?.globalState.get('qlosu.agent.provider', {});
  const settings = vscode.workspace.getConfiguration('qlosu.agent');
  const setting = (key) => {
    const inspection = settings.inspect(key);
    return inspection?.workspaceFolderValue ?? inspection?.workspaceValue ?? inspection?.globalValue;
  };
  return normalizeProvider({
    ...defaultProvider(),
    ...configured,
    type: setting('provider') ?? configured?.type,
    baseUrl: setting('baseUrl') ?? configured?.baseUrl,
    model: setting('model') ?? configured?.model,
    apiKey: agentApiKey,
  });
}

async function saveAgentProvider(provider) {
  const value = normalizeProvider(provider);
  agentApiKey = value.apiKey || '';
  if (extensionContext?.secrets) {
    if (agentApiKey) await extensionContext.secrets.store('qlosu.agent.apiKey', agentApiKey);
    else await extensionContext.secrets.delete('qlosu.agent.apiKey');
  }
  await extensionContext.globalState.update('qlosu.agent.provider', {
    type: value.type,
    baseUrl: value.baseUrl,
    model: value.model,
  });
}

async function activeAgentContext(options = {}) {
  const editor = vscode.window.activeTextEditor;
  const document = getActiveDocument();
  const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
  const context = {
    workspace: workspaceFolder ? workspaceFolder.uri.fsPath : '',
    file: document ? vscode.workspace.asRelativePath(document.uri, false) : '',
  };
  if (options.includeCode && document) context.code = trimForContext(document.getText(), 100000);
  if (options.includeSelection && editor && editor.document === document) {
    const selection = editor.document.getText(editor.selection);
    if (selection) context.selection = trimForContext(selection, 30000);
  }
  if (options.includeDiagnostics) {
    const diagnostics = document ? vscode.languages.getDiagnostics(document.uri).map((item) => ({
      line: item.range.start.line + 1,
      severity: item.severity,
      message: item.message,
      code: item.code,
    })) : [];
    context.diagnostics = diagnostics.slice(0, 200);
  }
  if (options.includeFiles && workspaceFolder) {
    const files = await vscode.workspace.findFiles(new vscode.RelativePattern(workspaceFolder, '**/*'), '{**/node_modules/**,**/.git/**,**/.qlosu/**}', 300);
    context.files = files.map((uri) => vscode.workspace.asRelativePath(uri, false)).sort();
  }
  return context;
}

function getActiveDocument() {
  rememberEditor(vscode.window.activeTextEditor);
  return lastLosuDocument && !lastLosuDocument.isClosed ? lastLosuDocument : null;
}

function activate(context) {
  extensionContext = context;
  rememberEditor(vscode.window.activeTextEditor);
  context.secrets.get('qlosu.agent.apiKey').then((value) => {
    agentApiKey = value || '';
    agentProvider?.post({ type: 'config', config: agentProviderConfig() });
  }).catch(() => {});
  const executeAgentTool = createAgentTools({
    vscode,
    callServer,
    getDocument: resolveDocument,
    getVersion: selectedVersion,
    openDiagram,
  });
  agentProvider = new LosuAgentProvider({
    vscode,
    context,
    callServer,
    serverBase,
    getActiveContext: activeAgentContext,
    executeTool: executeAgentTool,
    getProvider: agentProviderConfig,
    saveProvider: saveAgentProvider,
  });
  const documentSelector = [{ scheme: 'file', language: 'losu' }, { scheme: 'untitled', language: 'losu' }];
  const serverOptions = { run: { command: process.execPath, args: [serverPath()], transport: TransportKind.stdio }, debug: { command: process.execPath, args: [serverPath()], transport: TransportKind.stdio } };
  client = new LanguageClient('losuLanguageServer', 'Losu Language Server', serverOptions, { documentSelector });
  client.start().then(() => getOutputChannel().appendLine('Losu Language Server 已启动')).catch((error) => { logError('Losu Language Server 启动失败，请检查 qlosu.languageServer 配置和 Node.js 路径。', error); vscode.window.showErrorMessage('QLosu 语言服务启动失败，详情请查看 QLosu 输出面板。'); });
  context.subscriptions.push(
    vscode.window.registerWebviewViewProvider('qlosu.workspace', new QLosuViewProvider(context)),
    vscode.window.registerWebviewViewProvider('qlosu.agent', agentProvider),
    vscode.commands.registerCommand('qlosu.runFile', runFile),
    vscode.commands.registerCommand('qlosu.generateCircuit', () => openDiagram('circuit')),
    vscode.commands.registerCommand('qlosu.generateFlowchart', () => openDiagram('flowchart')),
    vscode.commands.registerCommand('qlosu.openSyntaxManual', openSyntaxManual),
    vscode.commands.registerCommand('qlosu.openUserGuide', () => openManual({ vscode, context: extensionContext, guide: true })),
    vscode.commands.registerCommand('qlosu.selectRuntime', selectRuntime),
    vscode.commands.registerCommand('qlosu.openTerminal', openTerminal),
    vscode.commands.registerCommand('qlosu.toggleWorkbenchMode', toggleWorkbenchMode),
    vscode.commands.registerCommand('qlosu.openAgent', () => {
      agentProvider?.show();
      return vscode.commands.executeCommand('workbench.view.extension.qlosu');
    }),
    vscode.workspace.onDidChangeTextDocument((event) => scheduleDiagramRefresh(event.document)),
    vscode.window.onDidChangeActiveTextEditor(rememberEditor),
  );
  focusLosuWorkbench();
}

function deactivate() {
  for (const timer of diagramRefreshTimers.values()) clearTimeout(timer);
  diagramRefreshTimers.clear();
  for (const entry of diagramPanels.values()) entry.disposed = true;
  diagramPanels.clear();
  return client?.stop();
}

module.exports = { activate, deactivate };
