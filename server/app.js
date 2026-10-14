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

const { Runtime } = require('./runtime');
const { instrument } = require('./instrument');
const { Workspace } = require('./workspace');
const llm = require('./llm');

const ROOT = path.join(__dirname, '..');
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
  app.use(express.static(path.join(ROOT, 'www')));
  app.use('/wasm', express.static(path.join(ROOT, 'wasm')));
  app.use('/vendor/monaco', express.static(path.join(ROOT, 'node_modules', 'monaco-editor', 'min')));

  const wrap = (fn) => async (req, res) => {
    try {
      await fn(req, res);
    } catch (e) {
      res.status(e.status || 500).json({ error: String((e && e.message) || e) });
    }
  };

  // ---------- 运行时 ----------

  app.get('/api/version', wrap(async (req, res) => {
    res.json({ version: await runtime.version() });
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
  app.post('/api/fs/delete', wrap(async (req, res) => {
    workspace.remove(req.body.path);
    res.json({ ok: true });
  }));
  app.post('/api/fs/rename', wrap(async (req, res) => {
    workspace.rename(req.body.from, req.body.to);
    res.json({ ok: true });
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
