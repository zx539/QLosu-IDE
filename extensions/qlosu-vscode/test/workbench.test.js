const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const vm = require('node:vm');
const test = require('node:test');

const extensionRoot = path.resolve(__dirname, '..');
function harness(workspaceRoot) {
  const commands = new Map(), panels = [], requests = [], errors = [];
  const state = new Map(), settings = { runtimeVersion: '2.0.0' };
  let editorListener;
  const uri = (file) => ({ fsPath: file, path: file, scheme: 'file', toString: () => `file://${file}` });
  const document = {
    uri: uri(path.join(workspaceRoot, 'hello.losu')), fileName: path.join(workspaceRoot, 'hello.losu'),
    languageId: 'losu', isClosed: false, isUntitled: false, getText: () => 'print("remembered")', save: async () => true,
  };
  const disposable = { dispose() {} };
  const vscode = {
    Uri: { file: uri, parse: (value) => uri(value.replace(/^file:\/\//, '')), joinPath: (base, ...parts) => uri(path.join(base.fsPath, ...parts)) },
    FileType: { File: 1, Directory: 2 }, ViewColumn: { Active: 1, Beside: 2 }, ConfigurationTarget: { Workspace: 2, Global: 1 },
    window: {
      activeTextEditor: { document }, visibleTextEditors: [],
      onDidChangeActiveTextEditor(fn) { editorListener = fn; return disposable; },
      registerWebviewViewProvider: () => disposable,
      createOutputChannel: () => ({ appendLine() {}, show() {} }),
      showWarningMessage: (s) => errors.push(s), showErrorMessage: (s) => errors.push(s), showInformationMessage() {},
      showInputBox: async () => { throw new Error('Runtime selection must not ask for free text'); },
      showQuickPick: async (items) => items.find((item) => item.id === 'losu-1.0'),
      createWebviewPanel() {
        const panel = { webview: { cspSource: 'test:', asWebviewUri: (u) => u, html: '', postMessage() {}, onDidReceiveMessage(fn) { panel.message = fn; return disposable; } }, onDidDispose: () => disposable, reveal() {} };
        panels.push(panel); return panel;
      },
    },
    workspace: {
      workspaceFolders: [{ uri: uri(workspaceRoot) }], textDocuments: [document],
      getWorkspaceFolder: () => ({ uri: uri(workspaceRoot) }),
      getConfiguration: () => ({ get: (key, fallback) => settings[key] ?? fallback, update: async (key, value) => { settings[key] = value; } }),
      onDidChangeTextDocument: () => disposable,
      openTextDocument: async () => document,
      fs: {
        readFile: async (u) => fs.promises.readFile(u.fsPath),
        stat: async (u) => fs.promises.stat(u.fsPath),
        readDirectory: async (u) => (await fs.promises.readdir(u.fsPath, { withFileTypes: true })).map((d) => [d.name, d.isDirectory() ? 2 : 1]),
        writeFile: async (u, bytes) => fs.promises.writeFile(u.fsPath, bytes),
      },
    },
    commands: { registerCommand(name, fn) { commands.set(name, fn); return disposable; }, executeCommand: async (name, ...args) => commands.get(name)?.(...args) },
  };
  const context = { extensionUri: uri(extensionRoot), subscriptions: [], secrets: { get: async () => '' }, globalState: { get: () => ({}) }, workspaceState: { get: (k) => state.get(k), update: async (k, v) => state.set(k, v) } };
  const sandbox = {
    TextDecoder, TextEncoder, Buffer, console, URL, process: { ...process, env: { ...process.env, QLOSU_ROOT: path.resolve(extensionRoot, '../..') } },
    __dirname: extensionRoot, module: { exports: {} }, setTimeout: () => 1, clearTimeout() {},
    require(name) {
      if (name === 'vscode') return vscode;
      if (name === 'vscode-languageclient/node') return { LanguageClient: class { start() { return Promise.resolve(); } }, TransportKind: { stdio: 0 } };
      return name.startsWith('./') ? require(path.join(extensionRoot, name)) : require(name);
    },
    fetch: async (url, options) => {
      requests.push({ url: String(url), body: options?.body && JSON.parse(options.body) });
      return { ok: true, json: async () => String(url).includes('/versions') ? { versions: [{ id: 'losu-1.0', label: '洛书 1.0', ok: true, runnable: true }, { id: 'losu-2.0.0', label: '洛书 2.0', ok: true, runnable: true }] } : { stdout: 'remembered', code: 0, ret: 0, circuitXml: null } };
    },
  };
  vm.runInNewContext(fs.readFileSync(path.join(extensionRoot, 'extension.js'), 'utf8'), sandbox);
  sandbox.module.exports.activate(context);
  return { commands, panels, requests, errors, document, settings, loseFocus() { editorListener?.({ document }); vscode.window.activeTextEditor = undefined; editorListener?.(undefined); } };
}

test('workbench actions keep the last Losu document after focus moves to a webview', async () => {
  const h = harness(os.tmpdir()); h.loseFocus();
  await h.commands.get('qlosu.runFile')();
  assert.equal(h.requests.length, 1, 'run must execute the remembered document');
  assert.equal(h.requests[0].body.code, h.document.getText());
  await h.commands.get('qlosu.generateCircuit')();
  assert.equal(h.panels.length, 1, h.errors.join('\n'));
});

test('syntax manual opens outside the IDE repository and renders Markdown', async () => {
  const h = harness(os.tmpdir());
  await h.commands.get('qlosu.openSyntaxManual')();
  assert.equal(h.panels.length, 1, h.errors.join('\n'));
  assert.match(h.panels[0].webview.html, /<h[1-6][ >]/);
});

test('runtime picker offers installed versions and run uses the selected version', async () => {
  const h = harness(os.tmpdir());
  await h.commands.get('qlosu.selectRuntime')();
  assert.equal(h.settings.runtimeVersion, 'losu-1.0');
  await h.commands.get('qlosu.runFile')();
  const run = h.requests.find((r) => r.url.endsWith('/api/runtime/run'));
  assert.ok(run, 'selected native runtime must reach runtime API');
  assert.equal(run.body.version, 'losu-1.0');
});
