const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const test = require('node:test');

function renderer() {
  const filename = path.join(__dirname, '..', 'extension.js');
  const sandbox = {
    require(name) {
      if (name === 'vscode') return { Uri: { joinPath: (...parts) => parts.join('/') } };
      if (name === 'vscode-languageclient/node') return {};
      if (name.startsWith('./')) return require(path.join(path.dirname(filename), name));
      return require(name);
    },
    TextDecoder, process, __dirname: path.dirname(filename), module: { exports: {} },
  };
  vm.runInNewContext(`${fs.readFileSync(filename, 'utf8')}
extensionContext={extensionUri:'extension-root'};module.exports.renderDiagram=diagramHtml;`, sandbox, { filename });
  return sandbox.module.exports.renderDiagram;
}
const webview = { cspSource: 'vscode-resource:', asWebviewUri: (uri) => uri };
const render = (kind, result, name = 'example.losu') => renderer()(kind, '', name, result, webview);

test('no quantum operations show an explicit empty state', () => {
  const html = render('circuit', { circuitXml: null, trace: { registers: [], ops: [] } });
  assert.match(html, /当前程序没有量子操作/);
});
test('source filename cannot inject HTML into a diagram', () => {
  const html = render('flowchart', { xml: '<mxfile><diagram/></mxfile>' }, '<img src=x onerror=alert(1)>.losu');
  assert.doesNotMatch(html, /<img src=x/);
  assert.match(html, /&lt;img/);
});
test('diagram scripts and resource access use a restrictive CSP', () => {
  const html = render('flowchart', { xml: '<mxfile><diagram/></mxfile>' });
  assert.match(html, /Content-Security-Policy/);
  assert.match(html, /default-src 'none'/);
  assert.match(html, /nonce-/);
});
test('viewer failure is visible instead of a blank graph', () => {
  const html = render('flowchart', { xml: '<mxfile><diagram/></mxfile>' });
  const elements = new Map();
  const element = (id) => { if (!elements.has(id)) elements.set(id, { hidden: false, textContent: '', setAttribute() {}, addEventListener() {}, querySelector() { return {}; } }); return elements.get(id); };
  const listeners = {};
  const window = { GraphViewer: { createViewerForElement() { throw new Error('invalid graph'); } }, addEventListener: (name, cb) => { listeners[name] = cb; } };
  const sandbox = { DOMParser: class { parseFromString() { return { querySelector() { return null; } }; } }, acquireVsCodeApi: () => ({ postMessage() {} }), window, document: { getElementById: element, querySelector: () => element('diagram') }, console, setTimeout: () => 1, clearTimeout() {} };
  for (const match of html.matchAll(/<script\b[^>]*>([\s\S]*?)<\/script>/g)) if (match[1].trim()) vm.runInNewContext(match[1], sandbox);
  window.onDrawioViewerLoad?.();
  assert.match([...elements.values()].map((el) => el.textContent).join(' '), /invalid graph/);
});
test('diagram XML is safely serialized without changing entities', () => {
  const html = render('flowchart', { xml: '<mxfile><diagram name="A &amp; B"/></mxfile>' });
  assert.doesNotMatch(html, /data-mxgraph='[^']*"A &amp; B/);
});

test('diagram tabs expose refresh messaging for source synchronization', () => {
  const html = render('flowchart', { xml: '<mxfile><diagram/></mxfile>' });
  assert.match(html, /id="refresh-diagram"/);
  assert.match(html, /postMessage\(\{type:'refresh'\}\)/);
  assert.match(html, /type==='refreshing'/);
  assert.match(html, /type==='refreshError'/);
});
