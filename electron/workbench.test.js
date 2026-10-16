const assert = require('node:assert/strict');
const test = require('node:test');
const { startWorkbench } = require('./workbench');

test('desktop API and editor receive the same selected workspace and close once', async () => {
  const closed = [], selected = [];
  const session = await startWorkbench({
    workspace: '/tmp/洛书 workspace',
    startServer: async ({ workspace }) => { selected.push(workspace.root); return { url: 'http://127.0.0.1:1/', close: async () => closed.push('api') }; },
    startCodeServer: async ({ workspace }) => { selected.push(workspace); return { url: 'http://127.0.0.1:2/', close: async () => closed.push('editor') }; },
  });
  assert.deepEqual(selected, ['/tmp/洛书 workspace', '/tmp/洛书 workspace']);
  assert.equal(new URL(session.url).searchParams.get('folder'), selected[0]);
  await Promise.all([session.close(), session.close()]);
  assert.deepEqual(closed, ['editor', 'api']);
});
test('desktop fallback preserves the workspace and still closes the API', async () => {
  let closed = false;
  const session = await startWorkbench({ workspace: '/tmp', startServer: async () => ({ url: 'http://127.0.0.1:1/', close: async () => { closed = true; } }), startCodeServer: async () => { throw new Error('missing binary'); } });
  assert.match(session.startupError.message, /missing/);
  assert.equal(session.url, session.api.url);
  await session.close(); assert.equal(closed, true);
});
test('API cleanup runs even when Code-Server cleanup fails', async () => {
  let closed = false;
  const session = await startWorkbench({ workspace: '/tmp', startServer: async () => ({ url: 'http://127.0.0.1:1/', close: async () => { closed = true; } }), startCodeServer: async () => ({ url: 'http://127.0.0.1:2/', close: async () => { throw new Error('close failed'); } }) });
  await assert.rejects(session.close(), /close failed/); assert.equal(closed, true);
});
