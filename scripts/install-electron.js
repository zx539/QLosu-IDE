#!/usr/bin/env node
// A resumable download path for hosts where @electron/get cannot use the proxy.
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const { spawn } = require('node:child_process');
const root = path.resolve(__dirname, '..');
const version = require('electron/package.json').version;
const artifact = `electron-v${version}-${process.platform}-${process.arch}.zip`;
const checksum = require('electron/checksums.json')[artifact];
const cache = path.join(root, '.cache', 'electron-download', version);
const archive = path.join(cache, artifact);
const base = process.env.QLOSU_ELECTRON_MIRROR || 'https://github.com/electron/electron/releases/download';
const url = `${base.replace(/\/$/, '')}/${base.includes('github.com') ? 'v' : ''}${version}/${artifact}`;

function curl(args) {
  return new Promise((resolve, reject) => {
    const child = spawn('curl', ['--location', '--fail', '--silent', '--show-error', ...args, url], { stdio: ['ignore', 'pipe', 'pipe'] });
    let output = '', error = '';
    child.stdout.on('data', (data) => { output += data; }); child.stderr.on('data', (data) => { error += data; });
    child.once('error', reject); child.once('close', (code) => code === 0 ? resolve(output) : reject(new Error(`curl exited ${code}: ${error.slice(0, 300)}`)));
  });
}
function digest(file) { return crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex'); }
async function main() {
  if (!checksum) throw new Error(`官方校验表没有此平台产物：${artifact}`);
  fs.mkdirSync(cache, { recursive: true });
  if (!fs.existsSync(archive) || digest(archive) !== checksum) {
    const headers = await curl(['--head', '--max-time', '20']);
    const lengths = [...headers.matchAll(/^content-length:\s*(\d+)/gim)];
    const length = Number(lengths.at(-1)?.[1]);
    if (!Number.isSafeInteger(length) || length < 1000000) throw new Error('下载源没有提供有效产物大小');
    const size = 2 * 1024 * 1024, total = Math.ceil(length / size);
    let next = 0, completed = 0;
    const parts = Array.from({ length: total }, (_, index) => path.join(cache, `${index}.part`));
    await Promise.all(Array.from({ length: 12 }, async () => {
      while (next < total) {
        const index = next++, from = index * size, to = Math.min(length - 1, from + size - 1), part = parts[index];
        if (!fs.existsSync(part) || fs.statSync(part).size !== to - from + 1) {
          await curl(['--range', `${from}-${to}`, '--retry', '2', '--max-time', '100', '--output', part]);
          if (fs.statSync(part).size !== to - from + 1) throw new Error('下载源没有按 Range 返回指定分段');
        }
        completed++; if (completed % 10 === 0 || completed === total) console.log(`Electron 下载：${completed}/${total} 段`);
      }
    }));
    const fd = fs.openSync(archive, 'w');
    try { for (const part of parts) fs.writeSync(fd, fs.readFileSync(part)); } finally { fs.closeSync(fd); }
    if (digest(archive) !== checksum) throw new Error('Electron SHA-256 与官方校验表不一致，拒绝解压');
  }
  const dist = path.join(root, 'node_modules', 'electron', 'dist');
  fs.mkdirSync(dist, { recursive: true });
  await require('@electron-internal/extract-zip').extract(archive, { dir: dist });
  const executable = process.platform === 'darwin' ? 'Electron.app/Contents/MacOS/Electron' : process.platform === 'win32' ? 'electron.exe' : 'electron';
  fs.writeFileSync(path.join(root, 'node_modules', 'electron', 'path.txt'), executable);
  console.log(`PASS Electron ${version} 下载、官方 SHA-256 校验与安装`);
}
main().catch((error) => { console.error(error.message); process.exitCode = 1; });
