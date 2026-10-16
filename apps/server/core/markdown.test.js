const assert = require('node:assert/strict');
const test = require('node:test');
const { renderMarkdown } = require('./markdown');
test('Markdown supports tables, tasks, details, alerts, math and highlighted source', () => {
  const html = renderMarkdown('| a | b |\n|---|---|\n| 1 | 2 |\n\n- [x] done\n\n<details><summary>展开</summary>\n\n**内容**\n\n</details>\n\n> [!WARNING]\n> 注意\n\n$x^2$\n\n$$\\frac{1}{2}$$\n\n```losu\nvar x = 42;\n```\n\n```mermaid\ngraph LR\nA-->B\n```');
  for (const expected of [/<table>/, /type="checkbox"/, /checked/, /<details>/, /<strong>内容/, /markdown-alert-warning/, /class="katex/, /hljs-keyword/, /class="mermaid"/]) assert.match(html, expected);
});
test('math and HTML remain literal inside code fences and code spans', () => {
  const html = renderMarkdown('`$x$`\n\n```text\n$$x$$\n<script>alert(1)</script>\n```');
  assert.doesNotMatch(html, /class="katex|<script>/);
  assert.match(html, /\$\$x\$\$/);
});
test('safe HTML renders while script, events, unsafe links and SVG are removed', () => {
  const html = renderMarkdown('<details onclick="alert(1)"><summary>查看</summary></details>\n\n<script>alert(1)</script>\n\n<a href="javascript:alert(1)">x</a><img src="x" onerror="alert(1)"><svg onload="alert(1)"></svg>');
  assert.match(html, /<details>/);
  assert.doesNotMatch(html, /onclick|onerror|onload|javascript:|<script|<svg/);
});
test('heading anchors are unique and formulas retain required layout styles', () => {
  const html = renderMarkdown('# 标题\n\n# 标题\n\n$$\\frac{1}{2}$$\n');
  assert.match(html, /id="标题"/); assert.match(html, /id="标题-1"/);
  assert.match(html, /style="[^"]*(height|top|vertical-align)/);
});
