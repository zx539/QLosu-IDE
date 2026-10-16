const assert = require('node:assert/strict');
const { spawn } = require('node:child_process');
const path = require('node:path');
const test = require('node:test');

const server = path.join(__dirname, '..', 'language-server', 'server.js');

function startServer() {
  const child = spawn(process.execPath, [server], { stdio: ['pipe', 'pipe', 'pipe'] });
  child.stderr.on('data', (chunk) => process.stderr.write(`[losu-lsp] ${chunk}`));
  let buffer = Buffer.alloc(0);
  const responses = new Map();
  const notifications = [];
  const waiters = new Map();
  child.stdout.on('data', (chunk) => {
    buffer = Buffer.concat([buffer, chunk]);
    while (true) {
      const separator = buffer.indexOf('\r\n\r\n');
      if (separator < 0) break;
      const header = buffer.subarray(0, separator).toString();
      const length = Number(header.match(/Content-Length:\s*(\d+)/i)?.[1]);
      if (!Number.isFinite(length)) { buffer = buffer.subarray(separator + 4); continue; }
      const start = separator + 4;
      if (buffer.length < start + length) break;
      const message = JSON.parse(buffer.subarray(start, start + length).toString());
      buffer = buffer.subarray(start + length);
      if (message.id !== undefined) {
        responses.set(message.id, message);
        waiters.get(message.id)?.();
      } else notifications.push(message);
    }
  });
  let nextId = 1;
  const send = (message) => {
    const body = Buffer.from(JSON.stringify(message));
    child.stdin.write(`Content-Length: ${body.length}\r\n\r\n`);
    child.stdin.write(body);
  };
  const request = (method, params) => {
    const id = nextId++;
    send({ jsonrpc: '2.0', id, method, params });
    return new Promise((resolve, reject) => {
      const timer = setTimeout(() => { waiters.delete(id); reject(new Error(`Timed out waiting for ${method}`)); }, 2000);
      const poll = () => {
        const response = responses.get(id);
        if (!response) return;
        clearTimeout(timer); waiters.delete(id); responses.delete(id); resolve(response);
      };
      waiters.set(id, poll);
      poll();
    });
  };
  const notify = (method, params) => send({ jsonrpc: '2.0', method, params });
  const close = () => { child.kill(); };
  return { request, notify, notifications, close };
}

async function openFixture() {
  const client = startServer();
  const uri = 'file:///tmp/qlosu-language-test.losu';
  const text = [
    '# print(abs(-1)) should remain a comment',
    '函数 量子演示(q: QuReg)：',
    '  H(q[0])',
    '  CNOT(q[0], q[1])',
    '  返回 print("完成")',
  ].join('\n');
  await client.request('initialize', { rootUri: 'file:///tmp', capabilities: {} });
  client.notify('initialized', {});
  client.notify('textDocument/didOpen', { textDocument: { uri, languageId: 'losu', version: 1, text } });
  return { client, uri, text };
}

test('language server advertises rich semantic token and hover capabilities', async (t) => {
  const client = startServer(); t.after(client.close);
  const response = await client.request('initialize', { rootUri: 'file:///tmp', capabilities: {} });
  assert.equal(response.result.serverInfo.version, '0.2.0');
  assert.deepEqual(response.result.capabilities.semanticTokensProvider.legend.tokenTypes.slice(0, 10), [
    'function', 'variable', 'keyword', 'type', 'number', 'string', 'comment', 'operator', 'constant', 'quantum',
  ]);
  assert.equal(response.result.capabilities.hoverProvider, true);
});

test('completion and hover document reference built-in and quantum APIs', async (t) => {
  const { client, uri, text } = await openFixture(); t.after(client.close);
  const completion = await client.request('textDocument/completion', { textDocument: { uri }, position: { line: 2, character: 3 } });
  const labels = completion.result.items.map((item) => item.label);
  assert.ok(labels.includes('CNOT'));
  assert.ok(labels.includes('abs'));
  assert.ok(completion.result.items.find((item) => item.label === 'CNOT').documentation.value.includes('受控非门'));

  const hoverOffset = text.indexOf('CNOT');
  const before = text.slice(0, hoverOffset).split('\n');
  const hover = await client.request('textDocument/hover', { textDocument: { uri }, position: { line: before.length - 1, character: 3 } });
  assert.match(hover.result.contents.value, /受控非门/);
  assert.match(hover.result.contents.value, /CNOT\(control, target\)/);
});

test('comments are ignored by declarations, diagnostics, and semantic tokens', async (t) => {
  const { client, uri } = await openFixture(); t.after(client.close);
  const symbols = await client.request('textDocument/documentSymbol', { textDocument: { uri } });
  assert.deepEqual(symbols.result.map((item) => item.name), ['量子演示']);
  const tokens = await client.request('textDocument/semanticTokens/full', { textDocument: { uri } });
  assert.ok(tokens.result.data.length > 0);
  assert.equal(tokens.result.data.length % 5, 0);
  const diagnostics = client.notifications.find((message) => message.method === 'textDocument/publishDiagnostics');
  assert.ok(!diagnostics || diagnostics.params.diagnostics.every((item) => !/print|abs/.test(item.message)));
});
