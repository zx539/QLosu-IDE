const fs = require('fs');
const os = require('os');
const path = require('path');
const net = require('net');
const { execFileSync } = require('child_process');
const { spawn } = require('child_process');
const config = require('./config');
const { ensureLosuWorkbench } = require('./workbench-profile');
const { nodeExecutable } = require('../../runtime/node');

function binaryName(platform = process.platform) {
  return platform === 'win32' ? 'code-server.exe' : 'code-server';
}
function candidates() {
  const installed = config.installRoot();
  return [
    process.env.QLOSU_CODE_SERVER_BIN,
    path.join(installed, 'out', 'node', 'entry.js'),
    path.join(installed, 'code-server', 'bin', binaryName()),
    path.join(installed, 'bin', binaryName()),
    path.join(installed, binaryName()),
    path.join(config.root, 'vendor', 'code-server', 'bin', binaryName()),
    path.join(config.root, 'vendor', 'code-server', 'code-server'),
    path.join(config.root, 'node_modules', '.bin', 'code-server'),
    'code-server',
  ].filter(Boolean);
}
function findBinary() {
  for (const candidate of candidates()) {
    if (candidate === 'code-server') {
      try {
        execFileSync(process.platform === 'win32' ? 'where' : 'sh',
          process.platform === 'win32' ? ['code-server'] : ['-c', 'command -v code-server'], { stdio: 'ignore' });
        return candidate;
      } catch { continue; }
    }
    try {
      if (fs.existsSync(candidate) && fs.statSync(candidate).isFile()) {
        // A TypeScript-only source build has out/node/entry.js but no VS Code
        // server bundle; accepting it causes a browser HTTP 500 later.
        if (path.basename(candidate) === 'entry.js') {
          const root = path.resolve(path.dirname(candidate), '..', '..');
          if (!fs.existsSync(path.join(root, 'lib', 'vscode', 'out', 'server-main.js'))) continue;
        }
        return candidate;
      }
    } catch { /* try the next candidate */ }
  }
  return null;
}
function freePort(host) {
  return new Promise((resolve, reject) => {
    const server = net.createServer();
    server.once('error', reject);
    server.listen(0, host, () => {
      const port = server.address().port;
      server.close(() => resolve(port));
    });
  });
}
function waitForPort(port, host, child, timeoutMs = 15000) {
  const started = Date.now();
  return new Promise((resolve, reject) => {
    let settled = false;
    let timer;
    const finish = (error) => {
      if (settled) return;
      settled = true;
      clearTimeout(timer);
      child.removeListener('exit', onExit);
      error ? reject(error) : resolve();
    };
    const onExit = (code, signal) => finish(new Error(`Code-Server 在启动时退出（${code == null ? signal : `退出码 ${code}`}）`));
    const probe = () => {
      const socket = net.connect({ host, port });
      socket.once('connect', () => { socket.destroy(); finish(); });
      socket.once('error', () => {
        socket.destroy();
        if (!settled && Date.now() - started < timeoutMs) setTimeout(probe, 100);
        else finish(new Error(`Code-Server 启动超时（${timeoutMs}ms）`));
      });
    };
    timer = setTimeout(() => finish(new Error(`Code-Server 启动超时（${timeoutMs}ms）`)), timeoutMs);
    child.once('exit', onExit);
    probe();
  });
}
function ensureDir(dir) { fs.mkdirSync(dir, { recursive: true }); }
function ensureExtensionLink() {
  ensureDir(config.extensionRoot);
  const target = path.join(config.extensionRoot, 'qlosu-vscode');
  if (!fs.existsSync(target)) {
    try { fs.symlinkSync(config.qlosuExtension, target, os.platform() === 'win32' ? 'junction' : 'dir'); }
    catch { fs.cpSync(config.qlosuExtension, target, { recursive: true }); }
  }
  if (!fs.existsSync(path.join(target, 'package.json'))) throw new Error(`洛书扩展未完整安装：${target}`);
  return target;
}
async function stopChild(child, timeoutMs = 5000) {
  if (!child || child.exitCode !== null) return;
  await new Promise((resolve) => {
    let done = false;
    let timer;
    const finish = () => { if (!done) { done = true; clearTimeout(timer); resolve(); } };
    timer = setTimeout(() => { try { child.kill('SIGKILL'); } catch {} finish(); }, timeoutMs);
    child.once('exit', finish);
    try { child.kill('SIGTERM'); } catch { finish(); }
  });
}
async function start({ workspace, extraArgs = [], readyTimeoutMs = 15000 } = {}) {
  const binary = findBinary();
  if (!binary) {
    const error = new Error(`未找到 Code-Server ${config.version}。Fedora x86_64 请运行 npm run code-server:build；也可运行 npm run code-server:download 或设置 QLOSU_CODE_SERVER_BIN。`);
    error.code = 'CODE_SERVER_NOT_FOUND';
    error.installHint = `https://github.com/coder/code-server/releases/tag/v${config.version}`;
    throw error;
  }
  const port = await freePort(config.bindHost);
  ensureDir(config.dataRoot);
  ensureDir(config.userDataRoot);
  ensureLosuWorkbench(config.userDataRoot);
  ensureExtensionLink();
  let resolvedWorkspace;
  if (workspace) {
    resolvedWorkspace = path.resolve(workspace);
    if (!fs.existsSync(resolvedWorkspace) || !fs.statSync(resolvedWorkspace).isDirectory()) {
      const error = new Error(`工作区目录不存在：${resolvedWorkspace}`);
      error.code = 'WORKSPACE_NOT_FOUND';
      throw error;
    }
  }
  const args = [
    '--bind-addr', `${config.bindHost}:${port}`,
    '--auth', 'none',
    '--disable-telemetry',
    '--disable-update-check',
    '--config', path.join(config.dataRoot, 'config.yaml'),
    '--extensions-dir', config.extensionRoot,
    '--user-data-dir', config.userDataRoot,
    ...extraArgs,
    ...(resolvedWorkspace ? [resolvedWorkspace] : []),
  ];
  const javascript = path.extname(binary) === '.js';
  const child = spawn(javascript ? nodeExecutable() : binary, javascript ? [binary, ...args] : args, {
    cwd: config.root,
    env: { ...process.env, QLOSU_ROOT: config.root, QLOSU_SERVER_URL: process.env.QLOSU_SERVER_URL || '' },
    stdio: ['ignore', 'pipe', 'pipe'],
    windowsHide: true,
  });
  let stderr = '';
  child.stderr.on('data', (chunk) => { stderr += String(chunk); });
  child.on('error', (error) => { stderr += `\n${error.message}`; });
  try {
    await waitForPort(port, config.bindHost, child, readyTimeoutMs);
  } catch (error) {
    await stopChild(child);
    error.stderr = stderr;
    throw error;
  }
  let closed = false;
  const close = async () => {
    if (closed) return;
    closed = true;
    await stopChild(child);
  };
  return { child, binary, port, url: `http://${config.bindHost}:${port}/`, stderr: () => stderr, close };
}
function continueInstallArgs() {
  if (process.env.QLOSU_INSTALL_CONTINUE !== '1') return [];
  const version = String(process.env.QLOSU_CONTINUE_VERSION || '').trim();
  const extension = version ? `Continue.continue@${version}` : 'Continue.continue';
  return ['--install-extension', extension];
}
module.exports = {
  config, candidates, findBinary, freePort, waitForPort, start, stopChild, continueInstallArgs,
};

