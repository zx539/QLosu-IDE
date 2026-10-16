const path = require('node:path');
const crypto = require('node:crypto');
const { renderMarkdown: renderRichMarkdown } = require('./markdown');

const decode = new TextDecoder();
function escapeHtml(value) { return String(value).replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c])); }
const scriptJson = (value) => JSON.stringify(value).replace(/</g, '\\u003c');

async function findManualRoot({ vscode, context, version }) {
  const folder = vscode.workspace.workspaceFolders?.[0];
  const normalized = String(version).replace(/^losu-/, '');
  const legacy = normalized === '1.0' || normalized === '1.0.0';
  let project = {};
  if (folder) {
    try { project = JSON.parse(decode.decode(await vscode.workspace.fs.readFile(vscode.Uri.joinPath(folder.uri, 'qlosu.json')))); }
    catch { /* The manual works without a project configuration. */ }
  }
  const configured = project.syntaxRoots?.[version] || project.syntaxRoots?.[normalized] || project.syntaxRoot || vscode.workspace.getConfiguration('qlosu').get('syntaxRoot');
  const candidates = [];
  if (configured) {
    if (path.isAbsolute(configured)) candidates.push(vscode.Uri.file(configured));
    else {
      if (folder) candidates.push(vscode.Uri.joinPath(folder.uri, configured));
      if (process.env.QLOSU_ROOT) candidates.push(vscode.Uri.file(path.resolve(process.env.QLOSU_ROOT, configured)));
    }
  }
  candidates.push(vscode.Uri.joinPath(context.extensionUri, 'manual', legacy ? '1.0' : '2.0.0'));
  // Source checkouts work before the first VSIX packaging step as well.
  const root = process.env.QLOSU_ROOT || path.resolve(__dirname, '../..');
  candidates.push(vscode.Uri.file(path.join(root, legacy ? 'src/losu-1.0/document' : 'src/QLosu/洛书语法')));
  for (const candidate of candidates) {
    try { const entries = await vscode.workspace.fs.readDirectory(candidate); if (entries.length) return candidate; }
    catch { /* Try the next installed source of documentation. */ }
  }
  throw new Error(`找不到语法手册，已检查：${candidates.map((u) => u.fsPath).join('、')}`);
}

function renderMarkdown(markdown, { current, files, root, webview, vscode }) {
  const link = (href, title, text) => {
    if (/^(https?:|mailto:)/i.test(href)) return `<a href="${escapeHtml(href)}">${text}</a>`;
    if (/^[a-z][\w+.-]*:/i.test(href) || href.startsWith('//')) return text;
    let name, anchor;
    try {
      const url = new URL(href, `https://manual.local/${current.split('/').map(encodeURIComponent).join('/')}`);
      name = decodeURIComponent(url.pathname.slice(1)); anchor = decodeURIComponent(url.hash.slice(1));
    } catch { return text; }
    if (!files.some((file) => file.relative === name)) return `<span title="文档未包含此链接">${text}</span>`;
    return `<a href="#${escapeHtml(anchor)}" data-doc="${escapeHtml(name)}" data-anchor="${escapeHtml(anchor)}">${text}</a>`;
  };
  const image = (href, title, text) => {
    if (/^https?:/i.test(href)) return `<img src="${escapeHtml(href)}" alt="${escapeHtml(text)}">`;
    if (/^[a-z][\w+.-]*:/i.test(href) || href.startsWith('/')) return escapeHtml(text);
    let relative;
    try { relative = path.posix.normalize(path.posix.join(path.posix.dirname(current), decodeURIComponent(href))); }
    catch { return escapeHtml(text); }
    if (relative.startsWith('../')) return escapeHtml(text);
    return `<img src="${escapeHtml(webview.asWebviewUri(vscode.Uri.joinPath(root, relative)))}" alt="${escapeHtml(text)}">`;
  };
  return renderRichMarkdown(markdown, { link, image });
}

async function openManual({ vscode, context, version, guide = false }) {
  try {
    const root = guide ? vscode.Uri.joinPath(context.extensionUri, 'manual', 'guide') : await findManualRoot({ vscode, context, version });
    const files = [];
    async function walk(uri, relative = '') {
      for (const [name, type] of await vscode.workspace.fs.readDirectory(uri)) {
        if (name === '.git' || name === 'LosuBook-syntaxs') continue;
        const child = vscode.Uri.joinPath(uri, name), rel = path.posix.join(relative, name);
        if (type === vscode.FileType.Directory) await walk(child, rel);
        else if (name.toLowerCase().endsWith('.md')) files.push({ relative: rel, uri: child });
      }
    }
    await walk(root);
    files.sort((a, b) => a.relative.localeCompare(b.relative, 'zh-CN'));
    if (!files.length) throw new Error(`语法目录没有 Markdown 文档：${root.fsPath}`);
    const panel = vscode.window.createWebviewPanel('qlosu.syntaxManual', guide ? 'QLosu 使用说明书' : `洛书语法手册 · ${version}`, vscode.ViewColumn.Active, { enableScripts: true, retainContextWhenHidden: true, localResourceRoots: [root, context.extensionUri] });
    const logo = panel.webview.asWebviewUri(vscode.Uri.joinPath(context.extensionUri, 'assets', 'losu-logo.png'));
    const resource = (...parts) => panel.webview.asWebviewUri(vscode.Uri.joinPath(context.extensionUri, ...parts));
    const assets = { mermaid: resource('vendor', 'mermaid', 'mermaid.min.js'), katex: resource('vendor', 'katex', 'katex.min.css'), style: resource('assets', 'markdown.css'), script: resource('assets', 'markdown-view.js') };
    let current = files.find((file) => /^readme\.md$/i.test(file.relative)) || files[0];
    const render = async (file) => renderMarkdown(decode.decode(await vscode.workspace.fs.readFile(file.uri)), { current: file.relative, files, root, webview: panel.webview, vscode });
    panel.webview.html = manualHtml({ files, active: current.relative, content: await render(current), logo, assets, cspSource: panel.webview.cspSource });
    panel.webview.onDidReceiveMessage(async (message) => {
      try {
        if (!['open', 'refresh'].includes(message?.type)) return;
        const file = message.type === 'refresh' ? current : files.find((item) => item.relative === message.name);
        if (!file) return;
        current = file;
        await panel.webview.postMessage({ type: 'content', name: file.relative, html: await render(file), anchor: String(message.anchor || '') });
      } catch (error) { vscode.window.showErrorMessage(`读取语法手册失败：${error.message}`); }
    });
  } catch (error) { vscode.window.showErrorMessage(error.message); }
}

function manualHtml({ files, active, content, logo, assets, cspSource }) {
  const nonce = crypto.randomBytes(16).toString('base64');
  return `<!doctype html><html><head><meta charset="utf-8"><meta http-equiv="Content-Security-Policy" content="default-src 'none'; img-src ${cspSource} https: http: data:; style-src ${cspSource} 'unsafe-inline'; font-src ${cspSource} data:; script-src ${cspSource} 'nonce-${nonce}';"><link rel="stylesheet" href="${escapeHtml(assets.katex)}"><link rel="stylesheet" href="${escapeHtml(assets.style)}"><style>
body{display:flex;height:100vh;margin:0;font:14px var(--vscode-font-family);color:var(--vscode-foreground);background:var(--vscode-editor-background)}body>aside{width:270px;flex-shrink:0;padding:14px;overflow:auto;border-right:1px solid var(--vscode-panel-border)}button,input{font:inherit;color:inherit;background:var(--vscode-input-background);border:1px solid var(--vscode-panel-border);padding:7px;cursor:pointer}nav button{display:block;width:100%;text-align:left;border:0;overflow-wrap:anywhere}button.active,button:hover{background:var(--vscode-list-hoverBackground)}.brand{display:flex;gap:10px;align-items:center;margin-bottom:16px}.brand img{width:38px;height:38px}.tools{display:flex;gap:6px;margin-bottom:12px}input{min-width:0;width:100%}main{flex:1;padding:24px;overflow:auto;line-height:1.7}article{max-width:1000px}pre{padding:14px;background:var(--vscode-textCodeBlock-background);overflow:auto}code{font-family:var(--vscode-editor-font-family)}table{border-collapse:collapse}td,th{border:1px solid var(--vscode-panel-border);padding:6px 10px}a{color:var(--vscode-textLink-foreground)}article img{max-width:100%}.markdown-alert{padding:10px 14px;margin:12px 0;border-left:3px solid var(--vscode-textLink-foreground);background:var(--vscode-textBlockQuote-background)}.markdown-alert-tip{border-color:#3fb950}.markdown-alert-warning,.markdown-alert-caution{border-color:#d29922}.markdown-alert-important{border-color:#a78bfa}.markdown-alert-note{border-color:#6aa7ff}.mermaid{overflow:auto;background:var(--vscode-textCodeBlock-background);padding:12px}[hidden]{display:none!important}@media(max-width:600px){aside{width:160px;padding:8px}main{padding:12px}}
</style></head><body><aside><div class="brand"><img src="${escapeHtml(logo)}" alt="洛书"><strong>洛书语法手册</strong></div><div class="tools"><input id="search" placeholder="搜索文档"><button id="refresh" title="刷新当前文档">↻</button></div><nav>${files.map((f) => `<button data-doc="${escapeHtml(f.relative)}" class="${f.relative === active ? 'active' : ''}">${escapeHtml(f.relative)}</button>`).join('')}</nav></aside><main><div class="tools"><button id="prev">‹ 上一篇</button><button id="next">下一篇 ›</button></div><article id="markdown" class="markdown-body">${content}</article></main><script nonce="${nonce}" src="${escapeHtml(assets.mermaid)}"></script><script nonce="${nonce}" src="${escapeHtml(assets.script)}"></script><script nonce="${nonce}">
const vscode=acquireVsCodeApi(),docs=${scriptJson(files.map((f) => f.relative))};let active=${scriptJson(active)};
function openDoc(name,anchor=''){vscode.postMessage({type:'open',name,anchor});}
document.addEventListener('click',e=>{const link=e.target.closest('[data-doc]');if(link){e.preventDefault();openDoc(link.dataset.doc,link.dataset.anchor||'');}});
document.getElementById('prev').onclick=()=>{const i=docs.indexOf(active);if(i>0)openDoc(docs[i-1]);};document.getElementById('next').onclick=()=>{const i=docs.indexOf(active);if(i<docs.length-1)openDoc(docs[i+1]);};
document.getElementById('search').oninput=e=>document.querySelectorAll('nav [data-doc]').forEach(b=>b.hidden=!b.dataset.doc.toLowerCase().includes(e.target.value.toLowerCase()));document.getElementById('refresh').onclick=()=>vscode.postMessage({type:'refresh'});
window.addEventListener('message',async e=>{if(e.data.type!=='content')return;active=e.data.name;document.getElementById('markdown').innerHTML=e.data.html;document.querySelectorAll('nav [data-doc]').forEach(b=>b.classList.toggle('active',b.dataset.doc===active));const target=document.getElementById(e.data.anchor);if(target)target.scrollIntoView();else document.querySelector('main').scrollTop=0;await window.QLosuMarkdown?.render(document.getElementById('markdown'));});
</script></body></html>`;
}

module.exports = { findManualRoot, renderMarkdown, openManual };
