#!/usr/bin/env node
const { spawn } = require('node:child_process');
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
  const child = spawn(binary, [root, ...process.argv.slice(2)], {
    cwd: root,
    env: { ...process.env, QLOSU_ROOT: root },
    stdio: 'inherit',
    windowsHide: false,
  });
  let forwarding = false;
  const forward = (signal) => {
    if (forwarding) return;
    forwarding = true;
    child.kill(signal);
  };
  process.once('SIGINT', () => forward('SIGINT'));
  process.once('SIGTERM', () => forward('SIGTERM'));
  child.once('error', (error) => {
    console.error(`Electron 启动失败：${error.message}`);
    process.exitCode = 1;
  });
  child.once('exit', (code, signal) => {
    if (signal) process.exitCode = 1;
    else if (Number.isInteger(code)) process.exitCode = code;
  });
}
