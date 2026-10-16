#!/usr/bin/env node
const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { until } = require('./lib/cdp');
const { stopChild } = require('../apps/code-server/manager');
async function main() {
  const local = path.resolve(__dirname, '../.cache/xvfb/root/usr/bin/Xvfb');
  const executable = process.env.QLOSU_XVFB_BIN || (fs.existsSync(local) ? local : 'Xvfb');
  const display = spawn(executable, ['-displayfd', '1', '-screen', '0', '1440x1000x24', '-nolisten', 'tcp'], { stdio: ['ignore', 'pipe', 'pipe'] });
  let number = '', error = '';
  display.stdout.on('data', (data) => { number = (number + data).slice(-100); }); display.stderr.on('data', (data) => { error = (error + data).slice(-2000); }); display.on('error', (e) => { error += e.message; });
  let child;
  try {
    await until(() => { if (display.exitCode !== null) throw new Error(error || 'Xvfb unavailable'); return /^\d+\n/.test(number); }, 'virtual display', 10000);
    child = spawn(process.execPath, process.argv.slice(2), { stdio: 'inherit', env: { ...process.env, DISPLAY: `:${number.trim()}` } });
    const result = await new Promise((resolve, reject) => { child.once('error', reject); child.once('exit', (code, signal) => resolve(code ?? (signal ? 1 : 0))); });
    process.exitCode = result;
  } finally { await stopChild(child); await stopChild(display); }
}
main().catch((error) => { console.error(error.message); process.exitCode = 1; });
