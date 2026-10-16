#!/usr/bin/env node
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { config } = require('../apps/code-server/manager');

async function probe(vscodeRoot) {
  const resolve = (name) => require.resolve(name, { paths: [vscodeRoot] });
  require(resolve('native-watchdog'));
  const pty = require(resolve('node-pty'));
  await new Promise((resolveProbe, reject) => {
    const terminal = pty.spawn(process.platform === 'win32' ? 'cmd.exe' : '/bin/sh', [], {
      name: 'xterm-256color', cols: 80, rows: 24, cwd: vscodeRoot, env: process.env,
    });
    let output = '';
    const timer = setTimeout(() => { terminal.kill(); reject(new Error(`PTY 未返回命令输出：${output}`)); }, 5000);
    terminal.onData((chunk) => { output += chunk; });
    terminal.onExit(({ exitCode }) => {
      clearTimeout(timer);
      if (exitCode !== 0 || !output.includes('QLosu-PTY-42')) reject(new Error(`PTY 交互失败：${output}`));
      else resolveProbe();
    });
    // The expected result does not appear in the input, so shell echo cannot pass this check.
    terminal.write(process.platform === 'win32' ? 'set /a 21*2\r\necho QLosu-PTY-42\r\nexit\r\n' : "printf 'QLosu-PTY-%s\\n' \"$((21 * 2))\"\nexit\n");
  });
  const fs = require('node:fs');
  const os = require('node:os');
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-watch-'));
  try {
    const watcher = require(resolve('@parcel/watcher'));
    let subscription, timer;
    try {
      let resolveWatch, rejectWatch;
      const event = new Promise((resolve, reject) => { resolveWatch = resolve; rejectWatch = reject; });
      subscription = await watcher.subscribe(dir, (error, events) => {
        if (error) rejectWatch(error);
        else if (events.some((item) => item.path.endsWith('probe.txt'))) resolveWatch();
      });
      timer = setTimeout(() => rejectWatch(new Error('文件监听未返回事件')), 5000);
      fs.writeFileSync(path.join(dir, 'probe.txt'), 'watch-ok');
      await event;
    } finally { clearTimeout(timer); await subscription?.unsubscribe(); }
  } finally { fs.rmSync(dir, { recursive: true, force: true }); }
  console.log('PASS Code-Server 原生终端输入、命令输出、进程退出和文件监听');
}

if (require.main === module) {
  if (process.argv[2] === '--probe') {
    probe(process.argv[3]).catch((error) => { console.error(error.stack || error); process.exitCode = 1; });
  } else {
    const result = spawnSync(process.execPath, [__filename, '--probe', path.join(config.installRoot(), 'lib', 'vscode')], { stdio: 'inherit', timeout: 15000 });
    if (result.status !== 0 || result.error) {
      console.error(`Code-Server 原生模块检查失败（${result.error?.message || result.signal || result.status}），请运行 npm run code-server:repair。`);
      process.exitCode = 1;
    }
  }
}
module.exports = { probe };
