#!/usr/bin/env node
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { findElectronBinary } = require('../electron/launcher');
const { freePort, stopChild } = require('../apps/code-server/manager');
const { until, connect, driver, pause } = require('./lib/cdp');

async function main() {
  const root = path.resolve(__dirname, '..'), temp = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-desktop-'));
  const workspace = path.join(temp, '工作区 with spaces'); fs.mkdirSync(workspace);
  const settingsRoot = path.join(temp, 'data', 'user-data', 'User'); fs.mkdirSync(settingsRoot, { recursive: true });
  fs.writeFileSync(path.join(settingsRoot, 'settings.json'), JSON.stringify({ 'window.confirmBeforeClose': 'never', 'terminal.integrated.confirmOnExit': 'never' }));
  fs.copyFileSync(path.join(root, 'examples/hello.losu'), path.join(workspace, 'hello.losu'));
  const binary = process.env.QLOSU_DESKTOP_APP || findElectronBinary({ root });
  if (!binary) throw new Error('请先运行 npm run desktop:install');
  const port = await freePort('127.0.0.1');
  const env = { ...process.env, QLOSU_DATA_DIR: path.join(temp, 'data'), XDG_CACHE_HOME: path.join(temp, 'cache') };
  delete env.ELECTRON_RUN_AS_NODE; delete env.QLOSU_NODE_BIN; delete env.QLOSU_WORKSPACE;
  const args = [...(process.env.QLOSU_DESKTOP_APP ? [] : [root]), '--no-sandbox', ...(env.DISPLAY ? ['--ozone-platform=x11'] : ['--ozone-platform=headless']), '--disable-gpu', `--remote-debugging-port=${port}`, '--remote-debugging-address=127.0.0.1', `--workspace=${workspace}`, '--qlosu-smoke'];
  const child = spawn(binary, args, { cwd: temp, env, stdio: ['ignore', 'pipe', 'pipe'] });
  let output = '', errors = '', cdp, ready;
  child.stdout.on('data', (data) => { output += data; }); child.stderr.on('data', (data) => { errors = (errors + data).slice(-8000); });
  child.on('error', (error) => { errors += error.message; });
  try {
    ready = await until(() => {
      if (child.exitCode !== null || child.signalCode) throw new Error(`Electron exited ${child.exitCode || child.signalCode}: ${errors}`);
      const line = output.split('\n').find((item) => item.startsWith('QLOSU_DESKTOP_READY '));
      return line ? JSON.parse(line.slice('QLOSU_DESKTOP_READY '.length)) : null;
    }, 'desktop window ready', 45000);
    assert.equal(ready.fallback, false, output);
    assert.equal((await (await fetch(new URL('/api/fs/info', ready.apiUrl))).json()).root, workspace);
    assert.equal(new URL(ready.url).searchParams.get('folder'), workspace);
    const post = async (body) => (await fetch(new URL('/api/runtime/run', ready.apiUrl), { method: 'POST', headers: { 'content-type': 'application/json' }, body: JSON.stringify(body) })).json();
    const native = await post({ version: 'losu-1.0', code: 'print(21 * 2)\n' });
    assert.equal(native.code, 0, JSON.stringify(native)); assert.match(native.stdout, /42/);
    const wasm = await post({ version: '2.0.0', code: fs.readFileSync(path.join(root, 'examples/hello.losu'), 'utf8') });
    assert.equal(wasm.code, 0, JSON.stringify(wasm)); assert.match(wasm.stdout, /Quantum/);
    console.log('PASS Electron 工作区绑定、原生 1.0 与 WASM 2.0 执行');
    const pages = await (await fetch(`http://127.0.0.1:${port}/json/list`)).json();
    cdp = await connect(pages.find((page) => page.type === 'page' && page.url.includes(new URL(ready.url).host)).webSocketDebuggerUrl);
    const ui = driver(cdp);
    await until(() => ui.evaluate("!!document.querySelector('.monaco-workbench')"), 'Electron workbench DOM');
    await cdp.send('Page.bringToFront'); await pause(2500);
    await until(() => ui.evaluate("!!Array.from(document.querySelectorAll('button,a,[role=button]')).find(b=>b.textContent.includes(\"Yes, I trust the authors\"))"), 'fixture trust');
    await ui.evaluate("Array.from(document.querySelectorAll('button,a,[role=button]')).find(b=>b.textContent.includes(\"Yes, I trust the authors\"))?.click()");
    await pause(1200);
    await ui.evaluate("document.querySelector('.monaco-workbench').focus()");
    await ui.key('p', 'KeyP', 80, 2); await pause(200);
    await until(() => ui.evaluate("document.querySelector('.quick-input-widget')?.offsetHeight > 0"), 'Electron quick open');
    await cdp.send('Input.insertText', { text: path.join(workspace, 'hello.losu') }); await pause(500); await ui.enter();
    await until(() => ui.evaluate("!!document.querySelector('.tab[aria-label*=\"hello.losu\"]')"), 'desktop source file opened');
    await pause(1500); await ui.command('QLosu: Open Terminal');
    await until(() => ui.evaluate("!!document.querySelector('.xterm')"), 'Electron terminal');
    await ui.evaluate("document.querySelector('.xterm-helper-textarea')?.focus()");
    const resultFile = path.join(workspace, 'terminal-result');
    await cdp.send('Input.insertText', { text: `printf '%s' "$((6 * 7))" > '${resultFile}'` }); await ui.enter();
    await until(() => fs.existsSync(resultFile) && fs.readFileSync(resultFile, 'utf8') === '42', 'Electron terminal keyboard execution');
    console.log('PASS Electron 工作台加载与终端键盘交互');
    await cdp.send('Input.insertText', { text: 'exit' }); await ui.enter();
    await pause(500);
    await cdp.send('Page.close').catch(() => {});
    await until(() => child.exitCode !== null || child.signalCode, 'Electron window close and exit', 20000);
    assert.equal(child.exitCode, 0, errors);
    for (const url of [ready.apiUrl, ready.url]) await assert.rejects(fetch(url, { signal: AbortSignal.timeout(2000) }));
    await assert.rejects(async () => { process.kill(ready.codeServerPid, 0); }, /ESRCH/);
    console.log('PASS 关闭 Electron 窗口后 API / Code-Server 与子进程全部退出');
    console.log(`PASS ${process.env.QLOSU_DESKTOP_APP ? 'Linux 发布包' : '源码版'} desktop smoke`);
  } catch (error) {
    console.error(`Desktop test logs: ${temp}\n${errors}`);
    if (cdp) { try { const dom = await cdp.send('Runtime.evaluate', { expression: 'document.body.innerText', returnByValue: true }); console.error(String(dom.result.value).slice(-4000)); } catch {} }
    if (cdp) { try { const shot = await cdp.send('Page.captureScreenshot', { format: 'png' }); fs.writeFileSync('/tmp/qlosu-desktop-failure.png', Buffer.from(shot.data, 'base64')); } catch {} }
    throw error;
  }
  finally { cdp?.close(); await stopChild(child); }
}
main().catch((error) => { console.error(error.stack || error); process.exitCode = 1; });
