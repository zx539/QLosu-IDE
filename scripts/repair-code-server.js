#!/usr/bin/env node
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { config } = require('../apps/code-server/manager');

function rebuild(vscodeRoot) {
  const manifest = path.join(vscodeRoot, 'package.json');
  const value = JSON.parse(fs.readFileSync(manifest, 'utf8'));
  // npm 12 requires explicit permission for dependency lifecycle scripts.
  value.allowScripts = { ...value.allowScripts, 'node-pty': true, 'native-watchdog': true, '@parcel/watcher': true, '@vscode/spdlog': true };
  fs.writeFileSync(manifest, JSON.stringify(value, null, 2) + '\n');
  const headers = path.join(os.homedir(), '.cache', 'node-gyp', process.versions.node);
  const args = ['rebuild', 'node-pty', 'native-watchdog', '@parcel/watcher', '@vscode/spdlog'];
  const env = { ...process.env, npm_config_build_from_source: 'true', ...(fs.existsSync(path.join(headers, 'include', 'node', 'node.h')) ? { npm_config_nodedir: headers } : {}) };
  const result = spawnSync(process.platform === 'win32' ? 'npm.cmd' : 'npm', args, { cwd: vscodeRoot, env, stdio: 'inherit' });
  if (result.error || result.status !== 0) throw new Error(result.error?.message || `npm rebuild exited ${result.status}`);
}
if (require.main === module) {
  try {
    const vscodeRoot = path.join(config.installRoot(), 'lib', 'vscode');
    rebuild(vscodeRoot);
    const check = spawnSync(process.execPath, [path.join(__dirname, 'check-code-server-native.js')], { stdio: 'inherit' });
    if (check.error || check.status !== 0) throw new Error(check.error?.message || '原生模块交互检查失败');
  } catch (error) { console.error(error.message); process.exitCode = 1; }
}
module.exports = { rebuild };
