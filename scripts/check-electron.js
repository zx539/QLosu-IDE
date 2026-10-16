#!/usr/bin/env node
const { spawnSync } = require('node:child_process');
const path = require('node:path');
const {
  findElectronBinary,
  installHint,
} = require('../electron/launcher');

const root = path.resolve(__dirname, '..');
const binary = findElectronBinary({ root });
if (!binary) {
  console.error(installHint({ root }));
  process.exitCode = 1;
} else {
  const result = spawnSync(binary, ['-p', 'process.versions.electron'], {
    cwd: root,
    encoding: 'utf8',
    timeout: 10_000,
    env: { ...process.env, ELECTRON_RUN_AS_NODE: '1' },
  });
  if (result.error || result.status !== 0) {
    console.error(`Electron 二进制无法执行：${binary}`);
    if (result.error) console.error(result.error.message);
    if (result.stderr) console.error(result.stderr.trim());
    process.exitCode = 1;
  } else {
    console.log(`Electron 可用：${binary}`);
    console.log((result.stdout || '').trim());
  }
}
