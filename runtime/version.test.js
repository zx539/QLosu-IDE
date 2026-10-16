const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const test = require('node:test');
const { resolveRuntimeVersion } = require('./version');

test('runtime version resolution follows explicit, source, project, default priority', () => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-version-'));
  try {
    fs.writeFileSync(path.join(root, 'qlosu.json'), JSON.stringify({ losuVersion: '1.0' }));
    const file = path.join(root, 'source.losu');
    fs.writeFileSync(file, '# losu-version: 2.0.0\n输出 1\n');
    assert.equal(resolveRuntimeVersion({ workspace: root, file }), '2.0.0');
    assert.equal(resolveRuntimeVersion({ workspace: root, file, version: '1.0' }), '1.0');
    assert.equal(resolveRuntimeVersion({ workspace: root, code: '// losu-version=1.0\n' }), '1.0');
    fs.rmSync(file);
    assert.equal(resolveRuntimeVersion({ workspace: root }), '1.0');
    fs.rmSync(path.join(root, 'qlosu.json'));
    assert.equal(resolveRuntimeVersion({ workspace: root }), '2.0.0');
  } finally { fs.rmSync(root, { recursive: true, force: true }); }
});
