#!/usr/bin/env node
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { Workspace } = require('./workspace');

const root = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-workspace-'));
const outside = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-outside-'));
try {
  fs.writeFileSync(path.join(outside, 'secret.txt'), 'outside');
  fs.symlinkSync(outside, path.join(root, 'linked'), 'dir');
  const workspace = new Workspace(root);

  assert.throws(() => workspace.resolve('linked/secret.txt'), /非法路径/);
  assert.throws(() => workspace.read('linked/secret.txt'), /非法路径/);
  assert.throws(() => workspace.write('linked/new.txt', 'blocked'), /非法路径/);
  workspace.write('nested/new.txt', 'ok');
  assert.equal(workspace.read('nested/new.txt'), 'ok');
  console.log('PASS workspace symlink boundary checks');
} finally {
  fs.rmSync(root, { recursive: true, force: true });
  fs.rmSync(outside, { recursive: true, force: true });
}
