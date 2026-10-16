const assert = require('node:assert/strict');
const test = require('node:test');
const { startServer } = require('../../../server/app');
const { LosuAgentProvider } = require('../agent');

test('Agent streams through HTTP proxy, executes tools, sends results back and renders Markdown', async () => {
  const requests = [], calls = [], messages = [];
  const api = await startServer({ port: 0, host: '127.0.0.1', fetchImpl: async (url, options) => {
    requests.push(JSON.parse(options.body));
    const delta = requests.length === 1
      ? { tool_calls: [{ index: 0, id: 'read-1', type: 'function', function: { name: 'read_file', arguments: '{"path":"hello.losu"}' } }] }
      : { content: '**结果**\n\n|项目|值|\n|---|---|\n|答案|42|\n\n$x^2$' };
    const sse = `data: ${JSON.stringify({ choices: [{ delta, finish_reason: requests.length === 1 ? 'tool_calls' : 'stop' }] })}\n\ndata: [DONE]\n\n`;
    const encoder = new TextEncoder();
    return new Response(new ReadableStream({ start(controller) { const bytes = encoder.encode(sse); controller.enqueue(bytes.slice(0, 35)); controller.enqueue(bytes.slice(35)); controller.close(); } }), { status: 200, headers: { 'content-type': 'text/event-stream' } });
  } });
  const provider = new LosuAgentProvider({
    vscode: {}, context: {}, serverBase: () => api.url,
    getProvider: () => ({ type: 'openai', baseUrl: 'http://fixture.invalid/v1', model: 'fixture', apiKey: '' }),
    getActiveContext: async () => ({ file: 'hello.losu', code: 'print(42)' }),
    executeTool: async (name, args) => { calls.push({ name, args }); return { ok: true, content: 'print(42)' }; },
  });
  provider.view = { webview: { postMessage: (message) => { messages.push(message); } } };
  try {
    await provider.run('解释这个程序', {});
    assert.deepEqual(calls, [{ name: 'read_file', args: { path: 'hello.losu' } }]);
    assert.equal(requests.length, 2);
    assert.ok(requests[1].messages.some((message) => message.role === 'tool' && message.content.includes('print(42)')));
    assert.match(messages.findLast((message) => message.type === 'text').html, /<strong>结果<\/strong>/);
    assert.match(messages.findLast((message) => message.type === 'text').html, /class="katex/);
    assert.equal(messages.some((message) => message.type === 'error'), false);
    assert.equal(provider.abort, null);
  } finally { await api.close(); }
});
