#!/usr/bin/env node
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const { spawnSync } = require('node:child_process');
const root = path.resolve(__dirname, '..');
function run(command, args) {
  const result = spawnSync(command, args, { cwd: root, stdio: 'inherit' });
  if (result.error || result.status !== 0) throw new Error(result.error?.message || `${command} exited ${result.status}`);
}
function prepare() {
  if (process.platform !== 'linux') throw new Error('当前发布验收入口针对 Linux；其他平台尚未验收。');
  const nodeRoot = path.join(root, 'vendor', 'node', `${process.platform}-${process.arch}`);
  fs.mkdirSync(nodeRoot, { recursive: true });
  const nodeTarget = path.join(nodeRoot, 'node');
  const hash = (file) => crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex');
  if (!fs.existsSync(nodeTarget) || hash(nodeTarget) !== hash(process.execPath)) {
    const temporary = path.join(nodeRoot, `node.${process.pid}.tmp`);
    fs.copyFileSync(process.execPath, temporary); fs.chmodSync(temporary, 0o755); fs.renameSync(temporary, nodeTarget);
  }
  const license = path.resolve(process.execPath, '../../LICENSE');
  if (!fs.existsSync(license)) throw new Error('找不到当前 Node 发行版的 LICENSE，不能生成发布包。');
  fs.copyFileSync(license, path.join(nodeRoot, 'LICENSE'));
  run(process.execPath, ['extensions/qlosu-vscode/scripts/prepare.js']);
  run(process.execPath, ['scripts/check-code-server-native.js']);
  const packageRoot = path.join(root, 'extensions', 'qlosu-vscode');
  const result = spawnSync('npm', ['ls', '--omit=dev', '--parseable', '--all'], { cwd: packageRoot, encoding: 'utf8' });
  if (result.status !== 0) throw new Error(result.stderr || '无法枚举扩展生产依赖');
  const production = result.stdout.trim().split('\n').filter((file) => file.startsWith(path.join(packageRoot, 'node_modules') + path.sep)).map((file) => `${path.relative(root, file).split(path.sep).join('/')}/**/*`);
  fs.mkdirSync(path.join(root, '.cache'), { recursive: true });
  fs.writeFileSync(path.join(root, '.cache', 'extension-production-files.json'), JSON.stringify(production, null, 2) + '\n');
  const required = ['使用说明书.md', 'wasm/qlosu.m.js', 'wasm/qlosu.m.wasm', 'extensions/qlosu-vscode/markdown.js', 'extensions/qlosu-vscode/vendor/mermaid/mermaid.min.js', `runtime/losu1/${process.platform}-${process.arch}/losu`, `runtime/losu1/${process.platform}-${process.arch}/libeasylosu.so`, `vendor/node/${process.platform}-${process.arch}/node`];
  const files = {};
  for (const relative of required) {
    const file = path.join(root, relative);
    if (!fs.existsSync(file)) throw new Error(`发布资源缺失：${relative}`);
    files[relative] = crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex');
  }
  fs.writeFileSync(path.join(root, 'release-manifest.json'), JSON.stringify({ version: require('../package.json').version, platform: process.platform, arch: process.arch, node: process.versions.node, nodeAbi: process.versions.modules, electron: require('electron/package.json').version, codeServer: require('../apps/code-server/config').version, glibc: process.report.getReport().header.glibcVersionRuntime, files }, null, 2) + '\n');
  console.log(`PASS 发布资源、Node ${process.versions.node} / ABI ${process.versions.modules} 与原生终端检查`);
}
if (require.main === module) { try { prepare(); } catch (error) { console.error(error.message); process.exitCode = 1; } }
module.exports = { prepare };
