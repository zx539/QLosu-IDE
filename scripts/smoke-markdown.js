#!/usr/bin/env node
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { startServer } = require('../server/app');
const { Workspace } = require('../server/workspace');
const { renderMarkdown } = require('../server/markdown');
const { until, connect, driver } = require('./lib/cdp');
const { stopChild } = require('../apps/code-server/manager');
async function main() {
  const root = path.resolve(__dirname, '..'), temp = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-markdown-'));
  const api = await startServer({ port: 0, workspace: new Workspace(temp) });
  let chrome, cdp;
  try {
    const profile = path.join(temp, 'chrome');
    chrome = spawn(process.env.QLOSU_CHROME_BIN || '/usr/bin/google-chrome', ['--headless', '--no-sandbox', '--disable-gpu', '--remote-debugging-port=0', `--user-data-dir=${profile}`, 'about:blank'], { stdio: 'ignore' });
    const file = path.join(profile, 'DevToolsActivePort'); await until(() => fs.existsSync(file), 'Chrome startup');
    const port = fs.readFileSync(file, 'utf8').split('\n')[0];
    const pages = await (await fetch(`http://127.0.0.1:${port}/json/list`)).json();
    cdp = await connect(pages.find((page) => page.type === 'page').webSocketDebuggerUrl);
    await cdp.send('Page.navigate', { url: api.url });
    const ui = driver(cdp); await until(() => ui.evaluate('document.readyState===\'complete\''), 'page loaded');
    const markdown = fs.readFileSync(path.join(root, '使用说明书.md'), 'utf8');
    const response = await fetch(new URL('/api/markdown', api.url), { method: 'POST', headers: { 'content-type': 'application/json' }, body: JSON.stringify({ markdown }) });
    const result = await response.json(); assert.equal(response.status, 200);
    await ui.evaluate(`document.body.innerHTML='<main class="markdown-body">'+${JSON.stringify(result.html)}+'</main>';window.QLosuMarkdown.render(document.body);`);
    await until(() => ui.evaluate("!!document.querySelector('.mermaid svg')"), 'Mermaid SVG rendered');
    const counts = await ui.evaluate("({tables:document.querySelectorAll('table').length,details:document.querySelectorAll('details').length,math:document.querySelectorAll('.katex').length,code:document.querySelectorAll('.hljs-keyword').length,alerts:document.querySelectorAll('.markdown-alert').length})");
    for (const [key, value] of Object.entries(counts)) assert.ok(value > 0, key);
    await cdp.send('Page.captureScreenshot', { format: 'png' }).then((shot) => fs.writeFileSync('/tmp/qlosu-markdown.png', Buffer.from(shot.data, 'base64')));
    console.log('PASS 使用说明书浏览器渲染：表格、折叠、公式、提示、高亮与 Mermaid SVG');
    // The same generated HTML is used in extension Webviews and Agent messages.
    assert.match(renderMarkdown('**Agent 回复**\n\n$x^2$'), /<strong>Agent 回复<\/strong>/);
  } finally { cdp?.close(); await stopChild(chrome); await api.close(); }
}
main().catch((error) => { console.error(error.stack || error); process.exitCode = 1; });
