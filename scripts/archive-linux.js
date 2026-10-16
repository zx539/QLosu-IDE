#!/usr/bin/env node
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const root = path.resolve(__dirname, '..');
const name = `QLosu-IDE-${require('../package.json').version}-linux-${process.arch}.tar.gz`;
const output = path.join(root, 'dist', name), partial = `${output}.part`;
const prefix = name.replace(/\.tar\.gz$/, '');
function checkContents(dir) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    if (['.qlosu', '.git', '.aws', '.ssh', '.env', '.env.local'].includes(entry.name)) throw new Error(`发布目录包含不应分发的文件：${path.join(dir, entry.name)}`);
    if (entry.isDirectory()) checkContents(path.join(dir, entry.name));
  }
}
checkContents(path.join(root, 'dist', 'linux-unpacked'));
const result = spawnSync('tar', ['--use-compress-program=gzip -1', '--transform', `s,^linux-unpacked,${prefix},`, '-cf', partial, '-C', path.join(root, 'dist'), 'linux-unpacked'], { stdio: 'inherit' });
if (result.error || result.status !== 0) {
  fs.rmSync(partial, { force: true });
  console.error(result.error?.message || `tar exited ${result.status}`); process.exitCode = 1;
} else {
  fs.renameSync(partial, output);
  require('./release-checksums');
  console.log(`PASS Linux 便携包：${output}`);
}
