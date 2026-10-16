'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const test = require('node:test');
const { diagnostics } = require('./losu2-cli');

const ROOT = path.resolve(__dirname, '..');
test('Losu v2 CLI checks a valid source file', () => {
  const source = fs.readFileSync(path.join(ROOT, 'examples', 'hello.losu'), 'utf8');
  assert.deepEqual(diagnostics(source), []);
});

test('Losu v2 CLI reports unbalanced source delimiters', () => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-cli-'));
  const file = path.join(dir, 'broken.losu');
  try {
    fs.writeFileSync(file, 'var x = (1 + 2;\n', 'utf8');
    const items = diagnostics(fs.readFileSync(file, 'utf8'));
    assert.equal(items[0].severity, 'error');
    assert.match(items[0].message, /缺少闭合符号/);
  } finally {
    fs.rmSync(dir, { recursive: true, force: true });
  }
});

