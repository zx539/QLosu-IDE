const assert = require('node:assert/strict');
const { Readable } = require('node:stream');
const test = require('node:test');
const {
  TOOL_DEFINITIONS,
  defaultProvider,
  normalizeProvider,
  parseNdjsonLine,
  readNdjson,
  renderAgentHtml,
} = require('../agent');

test('Agent exposes read, diagnose, diagram, mutation and terminal tools', () => {
  const names = TOOL_DEFINITIONS.map((tool) => tool.name);
  assert.deepEqual(names, [
    'read_file', 'list_files', 'get_diagnostics', 'run_file',
    'generate_flowchart', 'generate_circuit', 'write_file', 'run_terminal',
  ]);
  assert.deepEqual(TOOL_DEFINITIONS.find((tool) => tool.name === 'write_file').parameters.required, ['path', 'content']);
});

test('Agent provider configuration has a local Ollama default and normalizes protocols', () => {
  assert.equal(defaultProvider().baseUrl, 'http://127.0.0.1:11434/v1');
  assert.equal(normalizeProvider({ type: 'unexpected', model: 'local' }).type, 'openai');
  assert.equal(normalizeProvider({ type: 'anthropic', baseUrl: '', model: '' }).type, 'anthropic');
  assert.equal(normalizeProvider({ type: 'anthropic', baseUrl: '', model: '' }).model, 'qwen2.5-coder:7b');
});

test('Agent stream parser handles complete, split and malformed events', async () => {
  assert.deepEqual(parseNdjsonLine('{"type":"text","delta":"ok"}'), { type: 'text', delta: 'ok' });
  assert.equal(parseNdjsonLine('not json').type, 'error');
  const chunks = Readable.from(['{"type":"text","delta":"a"}\n{"type":"text"', ',"delta":"b"}\n']);
  const events = [];
  for await (const event of readNdjson(chunks)) events.push(event);
  assert.deepEqual(events, [{ type: 'text', delta: 'a' }, { type: 'text', delta: 'b' }]);
});

test('Agent webview uses a restrictive CSP and exposes configuration controls', () => {
  const html = renderAgentHtml('vscode-resource:');
  assert.match(html, /default-src 'none'/);
  assert.match(html, /洛书 Agent/);
  assert.match(html, /id="includeDiagnostics"/);
  assert.match(html, /id="apiKey"/);
  assert.match(html, /postMessage\(\{type:'send'/);
  assert.match(html, /postMessage\(\{type:'stop'\}\)/);
});
