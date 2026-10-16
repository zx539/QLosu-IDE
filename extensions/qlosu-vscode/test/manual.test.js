const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const { renderMarkdown, findManualRoot } = require('../manual');
const { marked } = require('../vendor/marked/marked.min.js');

const uri = (fsPath) => ({ fsPath });
const vscode = {
  Uri: { file: uri, joinPath: (base, ...parts) => uri(path.join(base.fsPath, ...parts)) },
  workspace: { getConfiguration: () => ({ get: () => '' }), fs: { readDirectory: (u) => fs.promises.readdir(u.fsPath) } },
};
const render = (source) => renderMarkdown(source, { current: 'README.md', files: [{ relative: 'README.md' }], root: uri('/manual'), vscode, webview: { asWebviewUri: (u) => `resource:${u.fsPath}` } });
test('manual renders expandable examples and removes unsafe HTML attributes', () => {
  const html = render('<details onclick="alert(1)"><summary>查看答案</summary>\n\n**答案**\n\n</details>');
  assert.match(html, /<details>/);
  assert.match(html, /<summary>查看答案<\/summary>/);
  assert.match(html, /<strong>答案<\/strong>/);
  assert.doesNotMatch(html, /onclick=/);
});
test('manual renders math, syntax highlighting, Mermaid blocks and GFM alerts', () => {
  const html = render('> [!TIP]\n> 提示正文\n\n公式 $x^2$。\n\n$$\\frac{1}{2}$$\n\n```javascript\nconst answer = 42;\n```\n\n```mermaid\ngraph LR\nA-->B\n```');
  assert.match(html, /markdown-alert-tip/);
  assert.match(html, /class="katex/);
  assert.match(html, /hljs-keyword/);
  assert.match(html, /class="mermaid"/);
});
test('bundled manual resolves without a workspace for both runtime IDs', async () => {
  const context = { extensionUri: uri(path.resolve(__dirname, '..')) };
  assert.match((await findManualRoot({ vscode, context, version: 'losu-1.0' })).fsPath, /manual\/1.0$/);
  assert.match((await findManualRoot({ vscode, context, version: 'losu-2.0.0' })).fsPath, /manual\/2.0.0$/);
});

test('Markdown line breaks, relative links, anchors and local images render safely', () => {
  const html = renderMarkdown('# 标题\n\n[下一章](../二/说明.md#示例)\n\n![图](../img/example.png)\n\n<script>alert(1)</script>\n\n[攻击](javascript:alert(1))\n', {
    current: '一/README.md', files: [{ relative: '二/说明.md' }], root: uri('/manual'), vscode,
    webview: { asWebviewUri: (u) => `resource:${u.fsPath}` },
  });
  assert.match(html, /<h1 id="标题">标题<\/h1>/);
  assert.match(html, /data-doc="二\/说明.md" data-anchor="示例"/);
  assert.match(html, /resource:\/manual\/img\/example.png/);
  assert.doesNotMatch(html, /<script>|href="javascript:/);
});

test('every packaged manual link and image points to an existing local file', () => {
  const root = path.resolve(__dirname, '..', 'manual');
  const missing = [];
  function walk(dir) {
    for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
      const file = path.join(dir, entry.name);
      if (entry.isDirectory()) walk(file);
      else if (/\.md$/i.test(entry.name)) {
        marked.walkTokens(marked.lexer(fs.readFileSync(file, 'utf8')), (token) => {
          if (!['link', 'image'].includes(token.type)) return;
          const href = token.href;
          if (/^[a-z][\w+.-]*:/i.test(href) || href.startsWith('#') || href.startsWith('//')) return;
          const target = decodeURIComponent(href.split('#')[0]);
          if (target && !fs.existsSync(path.resolve(dir, target))) missing.push(`${path.relative(root, file)} → ${href}`);
        });
      }
    }
  }
  walk(root);
  assert.deepEqual(missing, []);
});
