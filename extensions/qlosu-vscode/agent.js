/*
 * QLosu Agent view.
 *
 * The model is deliberately kept behind the local QLosu HTTP proxy. The
 * extension owns every workspace and terminal operation so a provider cannot
 * access the filesystem directly. Mutating tools always require an explicit
 * confirmation in VS Code.
 */

const { renderMarkdown } = require('./markdown');
const fs = require('node:fs/promises');
const path = require('node:path');

const TOOL_DEFINITIONS = [
  {
    name: 'read_file',
    description: '读取工作区内一个文本文件。只能使用相对工作区路径。',
    parameters: {
      type: 'object',
      properties: { path: { type: 'string', description: '相对工作区路径' } },
      required: ['path'],
      additionalProperties: false,
    },
  },
  {
    name: 'list_files',
    description: '列出工作区内的文件和目录，用于定位项目结构。',
    parameters: {
      type: 'object',
      properties: { glob: { type: 'string', description: '可选的 glob，例如 **/*.losu' } },
      additionalProperties: false,
    },
  },
  {
    name: 'get_diagnostics',
    description: '读取当前编辑器和工作区的诊断信息。',
    parameters: { type: 'object', properties: {}, additionalProperties: false },
  },
  {
    name: 'run_file',
    description: '运行当前打开的洛书文件并返回输出。需要用户确认。',
    parameters: { type: 'object', properties: {}, additionalProperties: false },
  },
  {
    name: 'generate_flowchart',
    description: '为当前洛书文件生成程序流程图并打开图表标签页。',
    parameters: { type: 'object', properties: {}, additionalProperties: false },
  },
  {
    name: 'generate_circuit',
    description: '为当前洛书文件生成量子线路并打开图表标签页。',
    parameters: { type: 'object', properties: {}, additionalProperties: false },
  },
  {
    name: 'write_file',
    description: '提出对工作区文本文件的完整修改。VS Code 会先显示预览，用户确认后才写入。',
    parameters: {
      type: 'object',
      properties: {
        path: { type: 'string', description: '相对工作区路径' },
        content: { type: 'string', description: '写入的完整文本内容' },
      },
      required: ['path', 'content'],
      additionalProperties: false,
    },
  },
  {
    name: 'run_terminal',
    description: '在工作区目录执行一条本地命令。每次执行都会要求用户确认。',
    parameters: {
      type: 'object',
      properties: {
        command: { type: 'string', description: '要执行的 shell 命令' },
        cwd: { type: 'string', description: '可选的相对工作区目录' },
      },
      required: ['command'],
      additionalProperties: false,
    },
  },
];

const AGENT_SYSTEM = [
  '你是 QLosu IDE 内置的洛书编程 Agent。',
  '你可以使用工具读取项目、检查诊断、运行当前洛书文件、生成图表、提出文件修改和运行终端命令。',
  '所有文件修改和终端命令都会交给用户确认；不要声称已经执行了未返回结果的操作。',
  '优先使用工具获取事实，再给出简洁中文答复。修改文件时解释目的，并保持现有项目风格。',
  '洛书源文件扩展名为 .losu，版本和运行时由 qlosu.json 或工作台设置决定。',
].join('\n');

function json(value) {
  try { return JSON.stringify(value); } catch { return String(value); }
}

function asText(value) {
  if (typeof value === 'string') return value;
  return json(value);
}

function isPlainObject(value) {
  return value && typeof value === 'object' && !Array.isArray(value);
}

function safeToolArguments(value) {
  return isPlainObject(value) ? value : {};
}

function parseNdjsonLine(line) {
  if (!line.trim()) return null;
  try { return JSON.parse(line); } catch { return { type: 'error', message: `模型返回了无效事件：${line.slice(0, 240)}` }; }
}

/**
 * Read the server's newline-delimited stream and yield normalized events.
 * This is exported separately so it can be tested without starting VS Code.
 */
async function* readNdjson(body) {
  const decoder = new TextDecoder();
  let buffer = '';
  for await (const chunk of body) {
    buffer += typeof chunk === 'string' ? chunk : decoder.decode(chunk, { stream: true });
    const lines = buffer.split(/\r?\n/);
    buffer = lines.pop() || '';
    for (const line of lines) {
      const event = parseNdjsonLine(line);
      if (event) yield event;
    }
  }
  buffer += decoder.decode();
  const last = parseNdjsonLine(buffer);
  if (last) yield last;
}

function relativePath(vscode, uri, workspaceFolder) {
  if (!uri) return '';
  return workspaceFolder ? vscode.workspace.asRelativePath(uri, false) : uri.fsPath;
}

function trimForContext(value, max = 100000) {
  const text = String(value || '');
  return text.length > max ? `${text.slice(0, max)}\n…(内容已截断)` : text;
}

function defaultProvider() {
  return { type: 'openai', baseUrl: 'http://127.0.0.1:11434/v1', model: 'qwen2.5-coder:7b', apiKey: '' };
}

function normalizeProvider(input) {
  const value = isPlainObject(input) ? input : {};
  const defaults = defaultProvider();
  return {
    type: value.type === 'anthropic' ? 'anthropic' : 'openai',
    baseUrl: String(value.baseUrl || defaults.baseUrl).trim(),
    model: String(value.model || defaults.model).trim(),
    apiKey: String(value.apiKey || ''),
  };
}

function renderAgentHtml(cspSource, assets = {}) {
  const nonce = Math.random().toString(36).slice(2);
  const csp = `default-src 'none'; style-src ${cspSource} 'unsafe-inline'; script-src ${cspSource} 'nonce-${nonce}'; connect-src ${cspSource}; img-src ${cspSource} data: https:; font-src ${cspSource} data:;`;
  return `<!doctype html><html lang="zh-CN"><head><meta charset="utf-8"><meta http-equiv="Content-Security-Policy" content="${csp}"><link rel="stylesheet" href="${assets.style || ''}"><link rel="stylesheet" href="${assets.katex || ''}"><style>
  :root{color-scheme:light dark}*{box-sizing:border-box}body{margin:0;padding:10px;color:var(--vscode-foreground);background:var(--vscode-sideBar-background);font:13px var(--vscode-font-family);height:100vh;display:flex;flex-direction:column;gap:8px}.toolbar{display:flex;gap:5px;align-items:center}.toolbar strong{flex:1;font-size:14px}.toolbar button,button{border:1px solid var(--vscode-panel-border);background:var(--vscode-button-secondaryBackground);color:inherit;border-radius:3px;padding:4px 8px;cursor:pointer}.toolbar button:hover,button:hover{background:var(--vscode-button-secondaryHoverBackground)}.config{display:grid;grid-template-columns:82px 1fr;gap:5px;align-items:center}.config input,.config select,.composer textarea{min-width:0;border:1px solid var(--vscode-input-border);background:var(--vscode-input-background);color:var(--vscode-input-foreground);padding:5px;border-radius:2px}.config input[type=password]{font-family:var(--vscode-editor-font-family)}.config .wide{grid-column:1/-1}.hint{color:var(--vscode-descriptionForeground);font-size:11px}.chat{flex:1;min-height:110px;overflow:auto;border:1px solid var(--vscode-panel-border);padding:8px;display:flex;flex-direction:column;gap:8px}.message{white-space:pre-wrap;overflow-wrap:anywhere;line-height:1.45}.message.user{background:var(--vscode-textBlockQuote-background);padding:6px;border-left:3px solid var(--vscode-textLink-foreground)}.message.assistant{padding:2px;white-space:normal}.message.tool{color:var(--vscode-descriptionForeground);font-size:11px;border-left:2px solid var(--vscode-panel-border);padding-left:6px}.message.error{color:var(--vscode-errorForeground)}.composer{display:flex;gap:6px}.composer textarea{resize:vertical;min-height:58px;flex:1;font:inherit}.composer button{align-self:stretch;min-width:50px}.status{min-height:15px;color:var(--vscode-descriptionForeground);font-size:11px}.context{display:flex;gap:9px;flex-wrap:wrap;color:var(--vscode-descriptionForeground);font-size:11px}.context label{display:flex;gap:3px;align-items:center}
  </style></head><body><div class="toolbar"><strong>洛书 Agent</strong><button id="stop" title="停止当前请求">停止</button><button id="clear" title="清空对话">清空</button><button id="save" title="保存模型配置">保存配置</button></div><div class="config"><label for="provider">提供商</label><select id="provider"><option value="openai">OpenAI 兼容 / Ollama</option><option value="anthropic">Anthropic</option></select><label for="baseUrl">接口</label><input id="baseUrl" spellcheck="false"><label for="model">模型</label><input id="model" spellcheck="false"><label for="apiKey">密钥</label><input id="apiKey" type="password" autocomplete="off"><span class="hint wide">默认连接本机 Ollama；密钥仅保存在 VS Code SecretStorage，不会写入工作区。</span></div><div class="context"><label><input id="includeCode" type="checkbox" checked>当前代码</label><label><input id="includeSelection" type="checkbox" checked>选中内容</label><label><input id="includeDiagnostics" type="checkbox" checked>诊断</label><label><input id="includeFiles" type="checkbox" checked>文件列表</label></div><div id="chat" class="chat" role="log" aria-live="polite"><div class="hint">可以询问语法、请求修复当前文件、运行测试或生成量子线路。需要修改和命令时，Agent 会先请求确认。</div></div><div id="status" class="status"></div><div class="composer"><textarea id="input" placeholder="询问洛书问题，或描述要完成的任务…"></textarea><button id="send">发送</button></div><script nonce="${nonce}" src="${assets.mermaid || ''}"></script><script nonce="${nonce}" src="${assets.script || ''}"></script><script nonce="${nonce}">
  const vscode=acquireVsCodeApi();const $=id=>document.getElementById(id);let active=null;function esc(s){return String(s).replace(/[&<>\"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','\\':'&#92;','\"':'&quot;'}[c]||c))}function add(kind,text){const el=document.createElement('div');el.className='message '+kind+(kind==='assistant'?' markdown-body':'');el.dataset.message=kind;el.textContent=text||'';$('chat').appendChild(el);$('chat').scrollTop=$('chat').scrollHeight;return el}function status(s){$('status').textContent=s||''}function send(){const text=$('input').value.trim();if(!text||active)return;$('input').value='';active=add('user',text);vscode.postMessage({type:'send',text,options:{includeCode:$('includeCode').checked,includeSelection:$('includeSelection').checked,includeDiagnostics:$('includeDiagnostics').checked,includeFiles:$('includeFiles').checked}});status('正在连接 Agent…')}function config(){return{type:$('provider').value,baseUrl:$('baseUrl').value,model:$('model').value,apiKey:$('apiKey').value}}$('provider').onchange=()=>{if($('provider').value==='anthropic'&&(!$('baseUrl').value||$('baseUrl').value.includes('11434'))) $('baseUrl').value='https://api.anthropic.com/v1';else if($('provider').value==='openai'&&$('baseUrl').value.includes('anthropic.com')) $('baseUrl').value='http://127.0.0.1:11434/v1')};$('send').onclick=send;$('stop').onclick=()=>vscode.postMessage({type:'stop'});$('input').addEventListener('keydown',e=>{if(e.key==='Enter'&&!e.shiftKey){e.preventDefault();send()}});$('clear').onclick=()=>{if(active)return;$('chat').innerHTML='';vscode.postMessage({type:'clear'})};$('save').onclick=()=>vscode.postMessage({type:'configure',config:config()});window.addEventListener('message',e=>{const m=e.data||{};if(m.type==='config'){ $('provider').value=m.config.type;$('baseUrl').value=m.config.baseUrl;$('model').value=m.config.model;$('apiKey').value=m.config.apiKey||''}else if(m.type==='assistant-start'){active=add('assistant','');status(m.status||'正在思考…')}else if(m.type==='text'){if(!active||active.dataset.message!=='assistant')active=add('assistant','');if(m.html!==undefined)active.innerHTML=m.html;else active.textContent+=m.delta||'';$('chat').scrollTop=$('chat').scrollHeight}else if(m.type==='reasoning'){status('正在分析…')}else if(m.type==='tool'){add('tool','工具 '+m.name+'：'+(m.status||'处理中'))}else if(m.type==='done'){window.QLosuMarkdown?.render($('chat'));active=null;status(m.status||'')}else if(m.type==='error'){if(!active||active.dataset.message!=='assistant')active=add('assistant','');active.classList.add('error');active.textContent+=m.message||'未知错误';active=null;status('') }else if(m.type==='cleared'){$('chat').innerHTML='<div class="hint">对话已清空。</div>'}});vscode.postMessage({type:'ready'});
  </script></body></html>`;
}

class LosuAgentProvider {
  constructor({ vscode, context, callServer, serverBase, getActiveContext, executeTool, getProvider, saveProvider }) {
    this.vscode = vscode;
    this.context = context;
    this.callServer = callServer;
    this.serverBase = serverBase;
    this.getActiveContext = getActiveContext;
    this.executeTool = executeTool;
    this.getProvider = getProvider;
    this.saveProvider = saveProvider;
    this.view = null;
    this.messages = [];
    this.abort = null;
  }

  resolveWebviewView(view) {
    this.view = view;
    view.webview.options = { enableScripts: true };
    const resource = (...parts) => view.webview.asWebviewUri(this.vscode.Uri.joinPath(this.context.extensionUri, ...parts));
    view.webview.html = renderAgentHtml(view.webview.cspSource, { style: resource('assets', 'markdown.css'), katex: resource('vendor', 'katex', 'katex.min.css'), mermaid: resource('vendor', 'mermaid', 'mermaid.min.js'), script: resource('assets', 'markdown-view.js') });
    view.webview.onDidReceiveMessage((message) => this.onMessage(message));
    view.onDidDispose(() => { this.abort?.abort(); this.abort = null; this.view = null; });
  }

  show() {
    this.view?.show?.(true);
  }

  post(message) {
    this.view?.webview.postMessage(message);
  }

  async onMessage(message) {
    if (!message || typeof message.type !== 'string') return;
    if (message.type === 'ready') {
      this.post({ type: 'config', config: this.getProvider() });
      return;
    }
    if (message.type === 'configure') {
      await this.saveProvider(normalizeProvider(message.config));
      this.post({ type: 'config', config: this.getProvider() });
      this.post({ type: 'done', status: '模型配置已保存' });
      return;
    }
    if (message.type === 'clear') {
      this.messages = [];
      this.post({ type: 'cleared' });
      return;
    }
    if (message.type === 'stop') {
      this.abort?.abort();
      this.abort = null;
      this.post({ type: 'done', status: '已停止' });
      return;
    }
    if (message.type === 'send') await this.run(String(message.text || ''), message.options || {});
  }

  async streamRequest(request) {
    const provider = normalizeProvider(this.getProvider());
    const base = String(this.serverBase()).replace(/\/+$/, '');
    const endpoint = `${base}/api/llm/chat`;
    const response = await fetch(endpoint, {
      method: 'POST',
      headers: { 'content-type': 'application/json' },
      body: JSON.stringify({ ...request, provider }),
      signal: this.abort?.signal,
    });
    if (!response.ok) throw new Error(`QLosu Agent 服务错误（${response.status}）`);
    if (!response.body) throw new Error('Agent 服务未返回数据流');
    return readNdjson(response.body);
  }

  async run(text, options) {
    if (!text.trim() || this.abort) return;
    this.abort = new AbortController();
    this.post({ type: 'assistant-start', status: '正在收集工作区上下文…' });
    try {
      const context = await this.getActiveContext(options);
      const prompt = `${text}\n\n工作区上下文：\n${trimForContext(json(context), 120000)}`;
      this.messages.push({ role: 'user', content: prompt });
      let rounds = 0;
      while (rounds++ < 8) {
        let answer = '';
        const calls = [];
        this.post({ type: 'assistant-start', status: rounds === 1 ? '正在思考…' : '正在处理工具结果…' });
        for await (const event of await this.streamRequest({ system: AGENT_SYSTEM, messages: this.messages, tools: TOOL_DEFINITIONS, maxTokens: 8192 })) {
          if (event.type === 'text') { answer += event.delta || ''; this.post({ type: 'text', delta: event.delta || '', html: renderMarkdown(answer) }); }
          else if (event.type === 'reasoning') this.post({ type: 'reasoning', delta: event.delta || '' });
          else if (event.type === 'tool_call') calls.push({ ...event, arguments: safeToolArguments(event.arguments) });
          else if (event.type === 'error') throw new Error(event.message || '模型请求失败');
        }
        if (calls.length === 0) {
          if (answer) this.messages.push({ role: 'assistant', content: answer });
          break;
        }
        this.messages.push({ role: 'assistant', content: answer, tool_calls: calls.map((call) => ({ id: call.id, name: call.name, arguments: call.arguments })) });
        for (const call of calls) {
          this.post({ type: 'tool', name: call.name, status: '处理中' });
          let result;
          try { result = await this.executeTool(call.name, call.arguments); }
          catch (error) { result = { ok: false, error: error.message || String(error) }; }
          this.post({ type: 'tool', name: call.name, status: result?.ok === false ? `失败：${result.error}` : '完成' });
          this.messages.push({ role: 'tool', tool_call_id: call.id, name: call.name, content: trimForContext(asText(result), 50000) });
        }
      }
      if (rounds > 8) this.post({ type: 'error', message: 'Agent 工具调用超过 8 轮，已停止。' });
      this.post({ type: 'done', status: '' });
    } catch (error) {
      if (error.name !== 'AbortError') this.post({ type: 'error', message: error.message || String(error) });
    } finally {
      this.abort = null;
    }
  }
}

function createAgentTools({ vscode, callServer, getDocument, openDiagram, getVersion = async () => '2.0.0' }) {
  function workspaceFolder() { return vscode.workspace.workspaceFolders?.[0] || null; }
  async function uriFor(relative) {
    const folder = workspaceFolder();
    if (!folder) throw new Error('请先打开工作区');
    const clean = String(relative || '').replace(/\\/g, '/');
    if (path.isAbsolute(clean) || /^[a-z]:/i.test(clean)) throw new Error('路径必须是工作区内的相对路径');
    if (!clean || clean.split('/').some((part) => part === '..' || part === '.')) throw new Error('路径必须是工作区内的相对路径');
    const uri = vscode.Uri.joinPath(folder.uri, ...clean.split('/'));
    if (folder.uri.scheme === 'file' && uri.scheme === 'file') {
      const root = await fs.realpath(folder.uri.fsPath);
      let candidate = uri.fsPath;
      while (true) {
        try { candidate = await fs.realpath(candidate); break; }
        catch (error) {
          if (error.code !== 'ENOENT') throw error;
          const entry = await fs.lstat(candidate).catch(() => null);
          if (entry?.isSymbolicLink()) throw new Error('无法解析的符号链接，已拒绝访问');
          const parent = path.dirname(candidate); if (parent === candidate) throw error; candidate = parent;
        }
      }
      const rel = path.relative(root, candidate);
      if (rel === '..' || rel.startsWith(`..${path.sep}`) || path.isAbsolute(rel)) throw new Error('符号链接指向工作区以外，已拒绝访问');
    }
    return uri;
  }
  async function confirm(message, detail) {
    const choice = await vscode.window.showWarningMessage(message, { modal: true, detail }, '确认', '取消');
    return choice === '确认';
  }
  async function readFile(args) {
    const uri = await uriFor(args.path);
    const bytes = await vscode.workspace.fs.readFile(uri);
    return { ok: true, path: args.path, content: trimForContext(new TextDecoder().decode(bytes), 120000) };
  }
  async function listFiles(args) {
    const folder = workspaceFolder();
    if (!folder) throw new Error('请先打开工作区');
    const pattern = String(args.glob || '**/*');
    const files = await vscode.workspace.findFiles(new vscode.RelativePattern(folder, pattern), '{**/node_modules/**,**/.git/**,**/.qlosu/**}', 500);
    return { ok: true, files: files.map((uri) => relativePath(vscode, uri, folder)).sort() };
  }
  async function currentFile() {
    const document = await getDocument();
    if (!document) throw new Error('请先打开洛书文件');
    return document;
  }
  return async function execute(name, args) {
    if (name === 'read_file') return readFile(args);
    if (name === 'list_files') return listFiles(args);
    if (name === 'get_diagnostics') {
      const diagnostics = vscode.languages.getDiagnostics().flatMap(([uri, items]) => items.map((item) => ({ path: relativePath(vscode, uri, workspaceFolder()), line: item.range.start.line + 1, severity: item.severity, message: item.message, code: item.code }))).slice(0, 500);
      return { ok: true, diagnostics };
    }
    if (name === 'run_file') {
      const document = await currentFile();
      if (document.languageId !== 'losu') throw new Error('当前文件不是 .losu 文件');
      if (!await confirm('洛书 Agent 请求运行当前文件', document.fileName)) return { ok: false, cancelled: true };
      const result = await callServer('/api/runtime/run', { code: document.getText(), version: await getVersion(document) });
      return { ...result, ok: (result.code ?? result.ret ?? 0) === 0 && !result.timedOut };
    }
    if (name === 'generate_flowchart' || name === 'generate_circuit') {
      const document = await currentFile();
      if (document.languageId !== 'losu') throw new Error('当前文件不是 .losu 文件');
      await openDiagram(name === 'generate_circuit' ? 'circuit' : 'flowchart');
      return { ok: true, opened: name === 'generate_circuit' ? 'quantum-circuit' : 'program-flowchart', file: document.fileName };
    }
    if (name === 'write_file') {
      const uri = await uriFor(args.path);
      const content = String(args.content ?? '');
      const existing = await vscode.workspace.fs.readFile(uri).catch((error) => { if (error.code === 'ENOENT' || error.code === 'FileNotFound') return new Uint8Array(); throw error; });
      const before = new TextDecoder().decode(existing);
      if (before === content) return { ok: true, unchanged: true, path: args.path };
      const beforeDocument = await vscode.workspace.openTextDocument({ content: before });
      const afterDocument = await vscode.workspace.openTextDocument({ content });
      await vscode.commands.executeCommand('vscode.diff', beforeDocument.uri, afterDocument.uri, `Agent 修改预览：${args.path}`, { preview: true });
      const approved = await confirm(`洛书 Agent 请求修改 ${args.path}`, `已打开差异预览，将写入 ${content.length} 个字符。确认后才会保存到文件。`);
      if (!approved) return { ok: false, cancelled: true, path: args.path };
      await uriFor(args.path);
      const latest = await vscode.workspace.fs.readFile(uri).catch((error) => { if (error.code === 'ENOENT' || error.code === 'FileNotFound') return new Uint8Array(); throw error; });
      if (new TextDecoder().decode(latest) !== before) throw new Error('文件在预览期间发生变化，请重新生成修改。');
      await vscode.workspace.fs.writeFile(uri, new TextEncoder().encode(content));
      return { ok: true, path: args.path, bytes: Buffer.byteLength(content) };
    }
    if (name === 'run_terminal') {
      const command = String(args.command || '').trim();
      if (!command) throw new Error('命令不能为空');
      const cwd = String(args.cwd || '.');
      if (!await confirm('洛书 Agent 请求执行终端命令', `${command}\n工作目录：${cwd}`)) return { ok: false, cancelled: true };
      return { ok: true, ...(await callServer('/api/terminal/exec', { command, cwd })) };
    }
    throw new Error(`未知 Agent 工具：${name}`);
  };
}

module.exports = {
  AGENT_SYSTEM,
  TOOL_DEFINITIONS,
  LosuAgentProvider,
  createAgentTools,
  defaultProvider,
  normalizeProvider,
  parseNdjsonLine,
  readNdjson,
  renderAgentHtml,
  trimForContext,
};
