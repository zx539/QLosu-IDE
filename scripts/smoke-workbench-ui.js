#!/usr/bin/env node
// Drive the real browser workbench through keyboard commands using Chrome CDP.
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { startServer } = require('../server/app');
const { Workspace } = require('../server/workspace');
const manager = require('../apps/code-server/manager');

const pause = (ms) => new Promise((resolve) => setTimeout(resolve, ms));
async function until(probe, label, timeout = 20000) {
  const start = Date.now();
  while (Date.now() - start < timeout) { const result = await probe(); if (result) return result; await pause(150); }
  throw new Error(`Timed out: ${label}`);
}
async function connect(url) {
  const ws = new WebSocket(url), pending = new Map(); let next = 0;
  await new Promise((resolve, reject) => { ws.addEventListener('open', resolve, { once: true }); ws.addEventListener('error', reject, { once: true }); });
  ws.addEventListener('message', ({ data }) => {
    const message = JSON.parse(data); if (!message.id) return;
    const entry = pending.get(message.id); if (!entry) return;
    pending.delete(message.id); clearTimeout(entry.timer);
    if (message.error) entry.reject(new Error(message.error.message)); else entry.resolve(message.result);
  });
  return {
    send(method, params = {}, sessionId) {
      return new Promise((resolve, reject) => {
        const id = ++next, timer = setTimeout(() => { pending.delete(id); reject(new Error(`CDP timeout: ${method}`)); }, 10000);
        pending.set(id, { resolve, reject, timer }); ws.send(JSON.stringify({ id, method, params, ...(sessionId ? { sessionId } : {}) }));
      });
    }, close() { ws.close(); },
  };
}

async function main() {
  const root = path.resolve(__dirname, '..'), temp = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-workbench-ui-'));
  const workspace = path.join(temp, 'workspace'); fs.mkdirSync(workspace);
  for (const name of ['hello.losu', 'bell.losu']) fs.copyFileSync(path.join(root, 'examples', name), path.join(workspace, name));
  fs.copyFileSync(path.join(root, 'examples/losu1/hello.els'), path.join(workspace, 'hello.els'));
  manager.config.userDataRoot = path.join(temp, 'user-data');
  manager.config.extensionRoot = path.join(temp, 'extensions');
  const api = await startServer({ port: 0, host: '127.0.0.1', workspace: new Workspace(workspace) });
  process.env.QLOSU_SERVER_URL = api.url;
  let codeServer, chrome, cdp;
  try {
    codeServer = await manager.start({ workspace });
    const profile = path.join(temp, 'chrome');
    chrome = spawn(process.env.QLOSU_CHROME_BIN || '/usr/bin/google-chrome', ['--headless', '--no-sandbox', '--disable-gpu', '--disable-dev-shm-usage', '--no-first-run', '--remote-debugging-port=0', `--user-data-dir=${profile}`, 'about:blank'], { stdio: 'ignore' });
    chrome.on('error', () => {});
    const devtools = path.join(profile, 'DevToolsActivePort');
    await until(() => fs.existsSync(devtools), 'Chrome startup');
    const port = fs.readFileSync(devtools, 'utf8').split('\n')[0];
    const pages = await (await fetch(`http://127.0.0.1:${port}/json/list`)).json();
    cdp = await connect(pages.find((page) => page.type === 'page').webSocketDebuggerUrl);
    await cdp.send('Emulation.setDeviceMetricsOverride', { width: 1440, height: 1000, deviceScaleFactor: 1, mobile: false });
    const evaluate = async (expression) => {
      const result = await cdp.send('Runtime.evaluate', { expression, returnByValue: true });
      if (result.exceptionDetails) throw new Error(result.exceptionDetails.text);
      return result.result.value;
    };
    const key = async (key, code, virtualKey, modifiers = 0) => {
      await cdp.send('Input.dispatchKeyEvent', { type: 'keyDown', key, code, windowsVirtualKeyCode: virtualKey, modifiers, ...(key === 'Enter' ? { text: '\r' } : {}) });
      await cdp.send('Input.dispatchKeyEvent', { type: 'keyUp', key, code, windowsVirtualKeyCode: virtualKey, modifiers });
    };
    const enter = () => key('Enter', 'Enter', 13);
    const command = async (name) => {
      await key('F1', 'F1', 112);
      await until(() => evaluate("!!document.querySelector('.quick-input-widget input') && document.querySelector('.quick-input-widget').offsetHeight > 0"), 'command palette');
      await key('a', 'KeyA', 65, 2);
      await cdp.send('Input.insertText', { text: `>${name}` });
      await until(() => evaluate(`document.querySelector('.quick-input-list')?.innerText.includes(${JSON.stringify(name)})`), `command ${name}`);
      await enter(); await pause(500);
    };
    const openFile = async (name) => {
      await key('p', 'KeyP', 80, 2); await pause(150);
      await cdp.send('Input.insertText', { text: path.join(workspace, name) });
      await pause(500); await enter();
      await until(() => evaluate(`!!document.querySelector('.tab[aria-label*=${JSON.stringify(name)}]')`), `open ${name}`);
    };
    await cdp.send('Page.navigate', { url: `${codeServer.url}?folder=${encodeURIComponent(workspace)}` });
    await until(() => evaluate("!!document.querySelector('.monaco-workbench')"), 'workbench loaded', 30000);
    // Wait for extension commands to register, not just the HTTP document.
    await pause(3000);
    // Trust only this generated fixture workspace; leave the user's trust settings intact.
    await evaluate("Array.from(document.querySelectorAll('.monaco-button')).find(b=>b.textContent.includes(\"Yes, I trust the authors\"))?.click()");
    await pause(2000);
    await openFile('hello.losu');
    await until(() => evaluate("!!Array.from(document.querySelectorAll('button,a,[role=button]')).find(b=>b.textContent.includes(\"Yes, I trust the authors\"))"), 'fixture trust dialog');
    await evaluate("Array.from(document.querySelectorAll('button,a,[role=button]')).find(b=>b.textContent.includes(\"Yes, I trust the authors\"))?.click()");
    await pause(2000);
    await command('QLosu: Open Losu Syntax Manual');
    await until(() => evaluate("document.body.innerText.includes('洛书语法手册')"), 'manual opened outside repository');
    await command('QLosu: Open User Guide');
    await until(() => evaluate("document.body.innerText.includes('QLosu 使用说明书')"), 'bundled user guide');
    let renderedManual;
    async function inspectFrame(tree) {
      const world = await cdp.send('Page.createIsolatedWorld', { frameId: tree.frame.id, worldName: 'qlosu-markdown-check' });
      const result = await cdp.send('Runtime.evaluate', { contextId: world.executionContextId, returnByValue: true, expression: "({guide:document.getElementById('markdown')?.innerText.includes('QLosu IDE 使用说明书'),svg:!!document.querySelector('.mermaid svg'),math:!!document.querySelector('.katex'),details:!!document.querySelector('details')})" });
      if(result.result.value?.guide) renderedManual = result.result.value;
      for(const child of tree.childFrames || []) await inspectFrame(child);
    }
    await until(async () => { const tree = await cdp.send('Page.getFrameTree'); await inspectFrame(tree.frameTree); return renderedManual?.svg; }, 'actual Webview Mermaid SVG');
    assert.equal(renderedManual.math, true); assert.equal(renderedManual.details, true);
    console.log('PASS 实际手册 Webview：使用说明书、Mermaid、公式与折叠');
    await command('QLosu: Run Current File');
    await until(() => evaluate("document.body.innerText.includes('圆周率') && document.body.innerText.includes('退出码：0')"), 'remembered file output after manual focus');
    console.log('PASS 手册跨工作区路径、打开手册后直接运行上次文件');

    await command('QLosu: Open Terminal');
    await until(() => evaluate("!!document.querySelector('.xterm')"), 'terminal created');
    await pause(500);
    await evaluate("document.querySelector('.xterm-helper-textarea')?.focus()");
    const terminalResult = path.join(workspace, 'terminal-result.txt');
    await cdp.send('Input.insertText', { text: `printf '%s' "$((21 * 2))" > '${terminalResult}'` });
    await enter();
    await until(() => fs.existsSync(terminalResult) && fs.readFileSync(terminalResult, 'utf8') === '42', 'terminal keyboard command result');
    console.log('PASS 浏览器终端键盘输入与实际 shell 命令执行');

    await command('QLosu: Select Runtime Version');
    await until(() => evaluate("document.querySelector('.quick-input-list')?.innerText.includes('洛书 1.0')"), 'runtime dropdown');
    await cdp.send('Input.insertText', { text: '洛书 1.0' }); await pause(200); await enter();
    await openFile('hello.els');
    await command('QLosu: Run Current File');
    await until(() => evaluate("document.body.innerText.replace(/\\u00a0/g,' ').includes('你好，洛书 1.0！') && document.body.innerText.includes('退出码：0')"), 'native runtime output');
    console.log('PASS 下拉选择洛书 1.0 并通过原生解释器运行 .els');

    await command('QLosu: Select Runtime Version');
    await cdp.send('Input.insertText', { text: '洛书 2.0' }); await pause(200); await enter();
    await openFile('bell.losu');
    await command('QLosu: Generate Quantum Circuit');
    await until(() => evaluate("document.body.innerText.replace(/\\u00a0/g,' ').includes('bell.losu · 量子线路')"), 'Bell circuit panel');
    console.log('PASS Bell 量子线路标签页');
    await key('j', 'KeyJ', 74, 2); // Hide the output panel for the visual artifact.
    await evaluate("document.querySelectorAll('.notifications-toasts [aria-label=\"Clear Notification\"]').forEach(button=>button.click())");
    await pause(800);
    await cdp.send('Page.captureScreenshot', { format: 'png' }).then((result) => fs.writeFileSync('/tmp/qlosu-workbench.png', Buffer.from(result.data, 'base64')));
    console.log('PASS browser workbench end-to-end');
  } catch (error) {
    console.error(`Browser artifacts: ${temp}`);
    if (cdp) { try { const result = await cdp.send('Runtime.evaluate', { expression: 'document.body.innerText', returnByValue: true }); console.error(String(result.result.value).slice(-5000)); const shot=await cdp.send('Page.captureScreenshot',{format:'png'}); fs.writeFileSync('/tmp/qlosu-workbench-failure.png',Buffer.from(shot.data,'base64')); } catch {} }
    throw error;
  } finally {
    cdp?.close(); if (chrome) await manager.stopChild(chrome);
    await codeServer?.close(); await api.close();
    // Keep the temporary browser logs for diagnostics; no user profile is changed.
  }
}
main().catch((error) => { console.error(error.stack || error); process.exitCode = 1; });
