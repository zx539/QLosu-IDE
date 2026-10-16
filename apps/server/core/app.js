/**
 * QLosu IDE HTTP 服务（Express）
 *
 * 供命令行版（index.js）与桌面版（electron/main.js）共用。
 * 服务默认只监听 127.0.0.1；由于开放了工作区读写与大模型代理接口，
 * 这里额外校验 Host / Origin，阻止其他网站通过浏览器（DNS rebinding / CSRF）访问。
 */
const express = require('express');
const fs = require('fs');
const path = require('path');
const os = require('os');
const { exec } = require('child_process');

const { Runtime } = require('./runtime');
const { instrument } = require('./instrument');
const { Workspace } = require('./workspace');
const { listVersions, selectAdapter } = require('../../../runtime/manager');
const { resolveRuntimeVersion } = require('../../../runtime/version');
const { parse: parseTrace } = require('../../../legacy/web-ide/www/js/core/trace');
const { parseProgram } = require('../../../legacy/web-ide/www/js/core/losu-parse');
const { circuitXml, flowchartXml } = require('../../../legacy/web-ide/www/js/core/drawio');
const llm = require('./llm');
const { renderMarkdown } = require('./markdown');

const ROOT = path.resolve(__dirname, '..', '..', '..');
const LOCAL_HOSTS = new Set(['localhost', '127.0.0.1', '[::1]']);

function hostOnly(h) {
  if (!h) return '';
  return h.startsWith('[') ? h.slice(0, h.indexOf(']') + 1) : h.split(':')[0];
}

function createApp({
  runtime = new Runtime(),
  workspace = new Workspace(),
  examplesDir = path.join(ROOT, 'examples'),
  allowRemote = process.env.QLOSU_ALLOW_REMOTE === '1',
  fetchImpl = fetch,
} = {}) {
  const app = express();
  app.disable('x-powered-by');

  if (!allowRemote) {
    app.use((req, res, next) => {
      if (!LOCAL_HOSTS.has(hostOnly(req.headers.host))) return res.status(403).send('forbidden host');
      const origin = req.headers.origin;
      if (origin && origin !== 'null') {
        try {
          if (!LOCAL_HOSTS.has(hostOnly(new URL(origin).host))) return res.status(403).send('forbidden origin');
        } catch {
          return res.status(403).send('forbidden origin');
        }
      }
      next();
    });
  }

  app.use(express.json({ limit: '8mb' }));
  app.use(express.static(path.join(ROOT, 'legacy', 'web-ide', 'www')));
  app.use('/img', express.static(path.join(ROOT, 'img')));
  app.use('/wasm', express.static(path.join(ROOT, 'wasm')));
  app.use('/vendor/monaco', express.static(path.join(ROOT, 'node_modules', 'monaco-editor', 'min')));
  app.use('/vendor/xterm', express.static(path.join(ROOT, 'node_modules', '@xterm', 'xterm')));
  // 保留旧版 GUI 使用的根路径别名；新版使用 /lib/xterm.js。
  app.get('/vendor/xterm/xterm.js', (req, res) => res.sendFile(path.join(ROOT, 'node_modules', '@xterm', 'xterm', 'lib', 'xterm.js')));
  app.get('/vendor/xterm/xterm.css', (req, res) => res.sendFile(path.join(ROOT, 'node_modules', '@xterm', 'xterm', 'css', 'xterm.css')));
  app.use('/vendor/xterm-fit', express.static(path.join(ROOT, 'node_modules', '@xterm', 'addon-fit', 'lib')));
  app.use('/vendor/katex', express.static(path.join(ROOT, 'node_modules', 'katex', 'dist')));
  app.use('/vendor/marked', express.static(path.join(ROOT, 'node_modules', 'marked')));
  app.use('/vendor/mermaid', express.static(path.join(ROOT, 'extensions', 'qlosu-vscode', 'vendor', 'mermaid')));
  app.get('/docs/losu-syntax.md', (req, res) => res.sendFile(path.join(ROOT, 'docs', 'syntax.md')));

  const wrap = (fn) => async (req, res) => {
    try {
      await fn(req, res);
    } catch (e) {
      res.status(e.status || 500).json({ error: String((e && e.message) || e) });
    }
  };

  // ---------- 运行时 ----------
  app.post('/api/markdown', wrap(async (req, res) => {
    if (typeof req.body?.markdown !== 'string') return res.status(400).json({ error: 'missing markdown' });
    res.json({ html: renderMarkdown(req.body.markdown) });
  }));

  app.get('/api/version', wrap(async (req, res) => {
    res.json({ version: await runtime.version() });
  }));

  app.get('/api/runtime/versions', wrap(async (req, res) => {
    const workspaceRoot = workspace.root;
    res.json({ versions: await listVersions({ workspace: workspaceRoot }) });
  }));

  // Versioned runtimes are intentionally separate from the built-in WASM
  // executor. This endpoint is used by the extension for configured CLI
  // adapters and never silently falls back to another version.
  app.post('/api/runtime/run', wrap(async (req, res) => {
    const file = req.body && req.body.file;
    const version = req.body && (req.body.version || req.body.losuVersion);
    const code = req.body && req.body.code;
    if ((typeof file !== 'string' || !file) && typeof code !== 'string') return res.status(400).json({ error: 'missing file or code' });
    const cwd = req.body.cwd ? workspace.resolve(req.body.cwd) : workspace.root;
    let temporary;
    try {
      let input;
      if (typeof code === 'string') {
        temporary = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-source-'));
        input = path.join(temporary, 'main.losu');
        fs.writeFileSync(input, code, 'utf8');
      } else input = workspace.resolve(file);
      const selectedVersion = resolveRuntimeVersion({ workspace: workspace.root, version, file: input, code });
      const adapter = selectAdapter({ workspace: workspace.root, version: selectedVersion });
      const timeoutMs = Math.min(Math.max(Number(req.body.timeoutMs) || 15000, 1000), 120000);
      const result = await adapter.run({ file: input, cwd, timeoutMs });
      res.status(result.code === 0 ? 200 : 422).json({ version: adapter.id, ...result, diagnostics: result.code === 0 ? [] : adapter.diagnosticsFromResult(result) });
    } finally { if (temporary) fs.rmSync(temporary, { recursive: true, force: true }); }
  }));

  app.post('/api/runtime/diagnose', wrap(async (req, res) => {
    const file = req.body && req.body.file;
    if (typeof file !== 'string' || !file) return res.status(400).json({ error: 'missing file' });
    const input = workspace.resolve(file);
    const selectedVersion = resolveRuntimeVersion({ workspace: workspace.root, version: req.body.version || req.body.losuVersion, file: input });
    const adapter = selectAdapter({ workspace: workspace.root, version: selectedVersion });
    const result = await adapter.diagnose({ file: input, cwd: workspace.root, timeoutMs: req.body.timeoutMs });
    res.json({ version: adapter.id, ...result });
  }));

  app.post('/api/runtime/trace', wrap(async (req, res) => {
    const file = req.body && req.body.file;
    if (typeof file !== 'string' || !file) return res.status(400).json({ error: 'missing file' });
    const input = workspace.resolve(file);
    const selectedVersion = resolveRuntimeVersion({ workspace: workspace.root, version: req.body.version || req.body.losuVersion, file: input });
    const adapter = selectAdapter({ workspace: workspace.root, version: selectedVersion });
    const result = await adapter.trace({ file: input, cwd: workspace.root, timeoutMs: req.body.timeoutMs });
    res.status(result.code === 0 ? 200 : 422).json({ version: adapter.id, ...result, diagnostics: result.code === 0 ? [] : adapter.diagnosticsFromResult(result) });
  }));

  app.post('/api/run', wrap(async (req, res) => {
    const code = req.body && req.body.code;
    if (!code || typeof code !== 'string') return res.status(400).json({ error: 'missing code' });
    // trace=true 时插桩记录量子门调用，用于生成量子线路图
    const wantTrace = req.body.trace !== false;
    const prepared = wantTrace ? instrument(code) : { code, instrumented: false };
    const timeoutMs = Math.min(Math.max(Number(req.body.timeoutMs) || 15000, 1000), 120000);
    const r = await runtime.run(prepared.code, { timeoutMs });
    res.json({
      ret: r.ret,
      stdout: r.stdout,
      truncated: !!r.truncated,
      timedOut: !!r.timedOut,
      instrumented: prepared.instrumented,
    });
  }));

  app.post('/api/trace', wrap(async (req, res) => {
    const code = req.body && req.body.code;
    if (typeof code !== 'string') return res.status(400).json({ error: 'missing code' });
    const selectedVersion = resolveRuntimeVersion({ workspace: workspace.root, version: req.body.version, code });
    const adapter = selectAdapter({ workspace: workspace.root, version: selectedVersion });
    if (adapter.id !== 'losu-2.0.0') return res.status(422).json({ error: '当前运行时不支持 QLosu 量子门追踪。量子示例请选择洛书 2.0（QLosu WASM）。' });
    const prepared = instrument(code);
    const result = await runtime.run(prepared.code, { timeoutMs: Math.min(Math.max(Number(req.body.timeoutMs) || 15000, 1000), 120000) });
    const trace = parseTrace(result.stdout || '');
    res.json({ ...result, trace, circuitXml: trace.registers?.length ? circuitXml(trace, 0, { theme: req.body.theme || 'dark' }) : null });
  }));

  app.post('/api/flowchart', wrap(async (req, res) => {
    const code = req.body && req.body.code;
    if (typeof code !== 'string') return res.status(400).json({ error: 'missing code' });
    res.json({ xml: flowchartXml(parseProgram(code), { theme: req.body.theme || 'dark', title: req.body.title || '程序流程图' }) });
  }));

  // ---------- 示例 ----------

  app.get('/api/examples', wrap(async (req, res) => {
    const files = fs.readdirSync(examplesDir)
      .filter((f) => f.endsWith('.losu'))
      .sort()
      .map((f) => {
        const src = fs.readFileSync(path.join(examplesDir, f), 'utf8');
        const first = src.split('\n').find((l) => l.trim().startsWith('// '));
        return { name: f, description: first ? first.replace(/^\/\/\s*/, '') : '' };
      });
    res.json({ examples: files });
  }));

  app.get('/api/examples/:name', wrap(async (req, res) => {
    const name = path.basename(req.params.name);
    const file = path.join(examplesDir, name);
    if (!name.endsWith('.losu') || !fs.existsSync(file)) {
      return res.status(404).json({ error: 'example not found' });
    }
    res.json({ name, code: fs.readFileSync(file, 'utf8') });
  }));

  // ---------- 洛书语法文档 ----------
  const syntaxRoot = path.join(ROOT, 'src', 'QLosu', '洛书语法');
  app.get('/api/syntax/list', wrap(async (req, res) => {
    const files = [];
    const walk = (dir) => {
      if (!fs.existsSync(dir)) return;
      for (const entry of fs.readdirSync(dir, { withFileTypes: true }).sort((a, b) => a.name.localeCompare(b.name))) {
        const abs = path.join(dir, entry.name);
        if (entry.isDirectory()) walk(abs);
        else if (entry.isFile() && entry.name.toLowerCase().endsWith('.md')) {
          files.push({ path: path.relative(syntaxRoot, abs).split(path.sep).join('/'), name: entry.name });
        }
      }
    };
    walk(syntaxRoot);
    res.json({ root: syntaxRoot, files });
  }));
  app.get('/api/syntax/read', wrap(async (req, res) => {
    const rel = String(req.query.path || '').replace(/\\/g, '/');
    if (!rel || rel.split('/').some((part) => part === '..' || part === '') || !rel.toLowerCase().endsWith('.md')) {
      return res.status(400).json({ error: '非法文档路径' });
    }
    const file = path.resolve(syntaxRoot, rel);
    const check = path.relative(syntaxRoot, file);
    if (check.startsWith('..') || path.isAbsolute(check) || !fs.existsSync(file)) return res.status(404).json({ error: '文档不存在' });
    res.json({ path: rel, content: fs.readFileSync(file, 'utf8') });
  }));

  // ---------- 工作区 ----------

  app.get('/api/fs/info', wrap(async (req, res) => {
    res.json({ root: workspace.root });
  }));
  app.get('/api/fs/list', wrap(async (req, res) => {
    res.json({ root: workspace.root, entries: workspace.list() });
  }));
  app.get('/api/fs/read', wrap(async (req, res) => {
    res.json({ path: req.query.path, content: workspace.read(req.query.path) });
  }));
  app.post('/api/fs/write', wrap(async (req, res) => {
    workspace.write(req.body.path, req.body.content);
    res.json({ ok: true });
  }));
  app.post('/api/fs/mkdir', wrap(async (req, res) => {
    const rel = req.body && req.body.path;
    if (typeof rel !== 'string' || !rel.trim()) return res.status(400).json({ error: '缺少文件夹名称' });
    const abs = workspace.resolve(rel);
    if (abs === workspace.root) return res.status(400).json({ error: '非法路径' });
    if (fs.existsSync(abs)) return res.status(409).json({ error: '目标已存在' });
    fs.mkdirSync(abs, { recursive: true });
    res.json({ ok: true });
  }));
  app.post('/api/fs/delete', wrap(async (req, res) => {
    workspace.remove(req.body.path);
    res.json({ ok: true });
  }));
  app.post('/api/fs/rename', wrap(async (req, res) => {
    workspace.rename(req.body.from, req.body.to);
    res.json({ ok: true });
  }));
  app.post('/api/fs/root', wrap(async (req, res) => {
    if (typeof req.body.root !== 'string' || !path.isAbsolute(req.body.root)) {
      return res.status(400).json({ error: '工作区路径必须是绝对路径' });
    }
    workspace.setRoot(req.body.root);
    res.json({ root: workspace.root, entries: workspace.list() });
  }));

  // ---------- 本地集成终端 ----------
  app.post('/api/terminal/exec', wrap(async (req, res) => {
    const command = req.body && req.body.command;
    if (typeof command !== 'string' || !command.trim()) return res.status(400).json({ error: '请输入命令' });
    const cwd = path.resolve(workspace.root, String(req.body.cwd || '.'));
    const relative = path.relative(workspace.root, cwd);
    if (relative.startsWith('..') || path.isAbsolute(relative)) return res.status(400).json({ error: '终端目录必须位于工作区内' });
    fs.mkdirSync(cwd, { recursive: true });
    exec(command, { cwd, timeout: 30000, maxBuffer: 1024 * 1024, windowsHide: true }, (error, stdout, stderr) => {
      res.json({ stdout, stderr, exitCode: error ? (Number.isInteger(error.code) ? error.code : 1) : 0, timedOut: !!(error && error.killed) });
    });
  }));

  // ---------- 大模型代理（流式 NDJSON） ----------

  app.post('/api/llm/chat', async (req, res) => {
    const controller = new AbortController();
    res.on('close', () => {
      if (!res.writableFinished) controller.abort();
    });
    res.setHeader('content-type', 'application/x-ndjson; charset=utf-8');
    res.setHeader('cache-control', 'no-cache');
    res.flushHeaders();
    const send = (obj) => res.write(JSON.stringify(obj) + '\n');
    try {
      for await (const ev of llm.chat(req.body || {}, { fetchImpl, signal: controller.signal })) send(ev);
    } catch (e) {
      if (!controller.signal.aborted) send({ type: 'error', message: String((e && e.message) || e) });
    }
    res.end();
  });

  return app;
}

/** 启动服务，返回 { server, url, close } */
function startServer({ port = 8080, host = '127.0.0.1', ...opts } = {}) {
  const runtime = opts.runtime || new Runtime();
  const app = createApp({ ...opts, runtime });
  return new Promise((resolve, reject) => {
    const server = app.listen(port, host, () => {
      const addr = server.address();
      const shown = host === '0.0.0.0' || host === '::' ? 'localhost' : host;
      resolve({
        server,
        url: `http://${shown}:${addr.port}/`,
        close: () => new Promise((r) => { runtime.close(); server.close(() => r()); }),
      });
    });
    server.on('error', reject);
  });
}

module.exports = { createApp, startServer };
