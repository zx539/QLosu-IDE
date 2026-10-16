const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const test = require('node:test');
const { parseChecksumManifest, validateArchiveEntries, replaceDownload } = require('./fetch-code-server');

test('parses SHA-256 checksum manifests by artifact filename', () => {
  assert.equal(parseChecksumManifest('deadbeef  file.tar.gz\n0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef  *code-server-4.95.3-linux-amd64.tar.gz', 'code-server-4.95.3-linux-amd64.tar.gz'), '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef');
});

test('ignores checksum entries for another artifact', () => {
  assert.equal(parseChecksumManifest('0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef  other.tar.gz', 'code-server.tar.gz'), null);
});

test('tar archive entries with parent paths are rejected before extraction', () => {
  assert.throws(() => validateArchiveEntries(['code-server/bin/code-server', '../escape', '/tmp/outside']), /非法路径/);
  assert.doesNotThrow(() => validateArchiveEntries(['code-server/bin/code-server', 'code-server/lib/vscode/']));
});

test('download replacement leaves the complete file at the destination', () => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-download-'));
  const source = path.join(root, 'artifact.part');
  const destination = path.join(root, 'artifact.tar.gz');
  try {
    fs.writeFileSync(source, 'complete');
    replaceDownload(source, destination);
    assert.equal(fs.readFileSync(destination, 'utf8'), 'complete');
    assert.equal(fs.existsSync(source), false);
  } finally {
    fs.rmSync(root, { recursive: true, force: true });
  }
});
