const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const { selectAdapter, runtimeId, listVersions } = require('./manager');
const { resolveRuntimeVersion } = require('./version');
const { startServer } = require('../server/app');
const { Workspace } = require('../server/workspace');

const root = path.resolve(__dirname, '..');
test('runtime aliases resolve to the native lpk adapter', () => {
  assert.equal(runtimeId('1.0'), 'losu-1.0');
  assert.equal(selectAdapter({ workspace: root, version: '1.0.0' }).id, 'losu-1.0');
  assert.throws(() => selectAdapter({ workspace: root, version: 'missing' }), /未找到/);
});

test('runtime selection honors a source version declaration', () => {
  assert.equal(resolveRuntimeVersion({ workspace: root, code: '// losu-version: 1.0\n输出 1' }), '1.0');
  assert.equal(resolveRuntimeVersion({ workspace: root, version: '2.0.0', code: '// losu-version: 1.0\n' }), '2.0.0');
});

test('runtime API lists both versions, runs unsaved native source and reports errors', async (t) => {
  const adapter = selectAdapter({ workspace: root, version: '1.0' });
  if (!fs.existsSync(adapter.command)) return t.skip('Run npm run losu1:build to verify the native interpreter');
  const versions = await listVersions({ workspace: root });
  assert.equal(versions.find((v) => v.id === 'losu-1.0').runnable, true);
  const api = await startServer({ port: 0, host: '127.0.0.1', workspace: new Workspace(root) });
  const post = async (route, body) => {
    const response = await fetch(new URL(route, api.url), { method: 'POST', headers: { 'content-type': 'application/json' }, body: JSON.stringify(body) });
    return { status: response.status, data: await response.json() };
  };
  try {
    const native = await post('/api/runtime/run', { version: 'losu-1.0', code: "打印('你好，洛书')\nprint(6 * 7)\n" });
    assert.equal(native.data.code, 0, JSON.stringify(native));
    assert.equal(native.data.backend, 'native');
    assert.match(native.data.stdout, /你好，洛书/);
    assert.match(native.data.stdout, /42/);
    const invalid = await post('/api/runtime/run', { version: 'losu-1.0', code: 'if (\n' });
    assert.notEqual(invalid.data.code, 0);
    assert.ok(invalid.data.stdout || invalid.data.stderr);
    const quantum = await post('/api/trace', { version: 'losu-1.0', code: 'print(1)' });
    assert.equal(quantum.status, 422);
    assert.match(quantum.data.error, /不支持/);
    const wasm = await post('/api/runtime/run', { version: 'losu-2.0.0', code: fs.readFileSync(path.join(root, 'examples', 'hello.losu'), 'utf8') });
    assert.equal(wasm.data.code, 0, JSON.stringify(wasm));
    assert.equal(wasm.data.backend, 'wasm');
  } finally { await api.close(); }
});
