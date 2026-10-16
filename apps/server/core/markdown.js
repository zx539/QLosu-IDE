// Shared by the web IDE and the packaged VS Code extension (copied at prepare).
const { Marked, Renderer } = require('marked');
const katex = require('katex');
const hljs = require('highlight.js');
const sanitizeHtml = require('sanitize-html');
const escapeHtml = (value) => String(value).replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));

hljs.registerLanguage('losu', () => ({
  name: 'Losu', keywords: { keyword: 'var def return if elif else for loop while break continue import block class this 令 方法 回 如果 另外 否则 计次 列举 重复 当 跳出 引 构造 此', literal: 'true false null 真 假 空' },
  contains: [hljs.C_LINE_COMMENT_MODE, hljs.HASH_COMMENT_MODE, hljs.QUOTE_STRING_MODE, hljs.APOS_STRING_MODE, hljs.C_NUMBER_MODE],
}));

function mathHtml(source, displayMode) {
  return `<span class="math${displayMode ? ' math-display' : ''}" role="math" aria-label="${escapeHtml(source)}">${katex.renderToString(source, { displayMode, throwOnError: false, trust: false, strict: 'ignore', output: 'html', maxExpand: 200 })}</span>`;
}
function renderMarkdown(markdown, { link, image } = {}) {
  const renderer = new Renderer();
  const headings = new Map();
  renderer.heading = (text, level, raw) => {
    const base = raw.toLowerCase().replace(/[^\p{L}\p{N}_-]+/gu, '-');
    const count = headings.get(base) || 0; headings.set(base, count + 1);
    return `<h${level} id="${escapeHtml(base + (count ? `-${count}` : ''))}">${text}</h${level}>\n`;
  };
  renderer.code = (code, language) => {
    const name = String(language || '').split(/\s+/)[0].toLowerCase();
    if (name === 'mermaid') return `<pre class="mermaid">${escapeHtml(code)}</pre>\n`;
    const highlighted = hljs.getLanguage(name) ? hljs.highlight(code, { language: name, ignoreIllegals: true }).value : escapeHtml(code);
    return `<pre><code class="hljs language-${escapeHtml(name)}">${highlighted}</code></pre>\n`;
  };
  renderer.blockquote = (quote) => {
    const alert = quote.match(/^<p>\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\](?:\n|<br\s*\/?>)?/i);
    if (!alert) return `<blockquote>${quote}</blockquote>\n`;
    const labels = { NOTE: '说明', TIP: '提示', IMPORTANT: '重要', WARNING: '注意', CAUTION: '谨慎操作' };
    return `<aside class="markdown-alert markdown-alert-${alert[1].toLowerCase()}"><p class="markdown-alert-title">${labels[alert[1].toUpperCase()]}</p><p>${quote.slice(alert[0].length)}</aside>\n`;
  };
  if (link) renderer.link = link;
  if (image) renderer.image = image;
  const parser = new Marked({ gfm: true, breaks: false, renderer });
  parser.use({ extensions: [
    {
      name: 'displayMath', level: 'block', start: (src) => src.indexOf('$$'),
      tokenizer(src) {
        const match = /^\$\$([\s\S]+?)\$\$(?:\n|$)/.exec(src);
        if (match) return { type: 'displayMath', raw: match[0], text: match[1].trim() };
      },
      renderer: (token) => mathHtml(token.text, true),
    },
    {
      name: 'inlineMath', level: 'inline', start: (src) => src.indexOf('$'),
      tokenizer(src) {
        const match = /^\$(?!\$)((?:\\.|[^$\n\\])+?)\$(?!\$)/.exec(src);
        if (match && match[1].trim()) return { type: 'inlineMath', raw: match[0], text: match[1] };
      },
      renderer: (token) => mathHtml(token.text, false),
    },
  ] });
  const html = parser.parse(String(markdown || '').replace(/^#![^\n]*\n/, ''));
  return sanitizeHtml(html, {
    allowedTags: [...sanitizeHtml.defaults.allowedTags, 'details', 'summary', 'img', 'input', 'span', 'aside'],
    allowedAttributes: {
      '*': ['class', 'id', 'title', 'role', 'aria-*'], a: ['href', 'title', 'data-doc', 'data-anchor'],
      img: ['src', 'alt', 'title', 'width', 'height'], input: ['type', 'checked', 'disabled'], details: ['open'],
      span: ['class', 'style', 'aria-hidden', 'aria-label', 'role'], th: ['align'], td: ['align'],
    },
    allowedStyles: { span: Object.fromEntries(['height', 'width', 'min-width', 'top', 'bottom', 'left', 'right', 'vertical-align', 'margin-left', 'margin-right', 'margin-top', 'padding-left', 'padding-right', 'position', 'border-bottom-width', 'border-top-width', 'border-right-width', 'font-size', 'color'].map((key) => [key, [/^(?!.*(?:url|expression|javascript|@import))[\w\s.#%,()+\-]*$/i]])) },
    allowedSchemes: ['http', 'https', 'mailto', 'vscode-resource', 'vscode-webview-resource', 'resource'],
    allowProtocolRelative: false,
    transformTags: { input: (_, attributes) => ({ tagName: 'input', attribs: { type: 'checkbox', disabled: '', ...(Object.hasOwn(attributes, 'checked') ? { checked: '' } : {}) } }) },
  });
}
module.exports = { renderMarkdown, escapeHtml };
