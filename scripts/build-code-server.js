#!/usr/bin/env node
/**
 * Build Code-Server from the pinned upstream source for the current host.
 * Fedora x86_64 is the primary supported development target.
 *
 * The source checkout lives in .cache/code-server-source and the runnable
 * result is copied to vendor/code-server/<version>/<platform>-<arch>/.
 */
const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');
const { config } = require('../apps/code-server/manager');

const root = config.root;
const sourceDir = path.resolve(process.env.QLOSU_CODE_SERVER_SOURCE || path.join(root, '.cache', 'code-server-source'));
const ref = process.env.QLOSU_CODE_SERVER_REF || `v${config.version}`;
const repo = process.env.QLOSU_CODE_SERVER_REPO || 'https://github.com/coder/code-server.git';
const installDir = config.installRoot();
// v4.95.3 is maintained and tested with npm; pnpm remains available as an
// opt-in override for newer source revisions.
const packageManager = process.env.QLOSU_CODE_SERVER_PM || 'npm';

function run(command, args, options = {}) {
  console.log(`$ ${command} ${args.join(' ')}`);
  const result = spawnSync(command, args, { stdio: 'inherit', cwd: sourceDir, env: process.env, ...options });
  if (result.error) throw result.error;
  if (result.status !== 0) throw new Error(`${command} 退出码 ${result.status}`);
}

function commandExists(command) {
  const probe = spawnSync('sh', ['-c', `command -v ${command}`], { stdio: 'ignore' });
  return probe.status === 0;
}

function ensureSource() {
  if (!fs.existsSync(path.join(sourceDir, '.git'))) {
    fs.mkdirSync(path.dirname(sourceDir), { recursive: true });
    run('git', ['clone', '--filter=blob:none', '--branch', ref, '--depth', '1', repo, sourceDir], { cwd: root });
    run('git', ['submodule', 'update', '--init', '--depth', '1']);
    return;
  }
  run('git', ['fetch', '--tags', '--force', 'origin', ref]);
  run('git', ['checkout', '--force', ref]);
  run('git', ['submodule', 'update', '--init', '--depth', '1']);
}

function ensurePackageManager() {
  if (packageManager === 'pnpm' && !commandExists('pnpm')) {
    if (!commandExists('corepack')) throw new Error('缺少 pnpm/corepack；Fedora 可执行 corepack enable pnpm，或设置 QLOSU_CODE_SERVER_PM=yarn');
    run('corepack', ['pnpm', '--version'], { cwd: root });
  } else if (!commandExists(packageManager)) {
    throw new Error(`找不到 ${packageManager}，请先安装包管理器`);
  }
}

function build() {
  ensurePackageManager();
  const hasLockfile = packageManager === 'pnpm'
    ? fs.existsSync(path.join(sourceDir, 'pnpm-lock.yaml'))
    : packageManager === 'yarn'
      ? fs.existsSync(path.join(sourceDir, 'yarn.lock'))
      : fs.existsSync(path.join(sourceDir, 'package-lock.json'));
  const args = packageManager === 'npm' && hasLockfile && process.env.CI && process.env.QLOSU_CODE_SERVER_FROZEN !== '0'
    ? ['ci']
    : ['install'];
  const command = packageManager === 'pnpm' && !commandExists('pnpm') ? 'corepack' : packageManager;
  const prefix = command === 'corepack' ? ['pnpm'] : [];
  // v4.95.3's VS Code remote package contains an optional Kerberos native
  // module. SKIP_SUBMODULE_DEPS lets the code-server TypeScript build run
  // without that system package; a full release build can unset it after
  // installing Fedora's krb5-devel package.
  const buildEnv = { ...process.env, SKIP_SUBMODULE_DEPS: process.env.SKIP_SUBMODULE_DEPS || '1' };
  if (command === 'corepack') run(command, [...prefix, 'approve-builds', '--all'], { env: buildEnv });
  run(command, [...prefix, ...args], { env: buildEnv });
  // code-server's release build creates a self-contained `out` tree and the
  // executable launcher under `bin/`; invoke the upstream documented build.
  run(command, [...prefix, ...(command === 'npm' ? ['run'] : []), 'build'], { env: buildEnv });
  const sourceServer = path.join(sourceDir, 'lib', 'vscode', 'out', 'server-main.js');
  if (fs.existsSync(sourceServer)) return sourceDir;

  // `npm run build` compiles code-server itself. The browser also needs the
  // VS Code Remote Web bundle, which is produced by the upstream release
  // steps below.
  run('npm', ['run', 'build:vscode'], { env: { ...buildEnv, VERSION: config.version, MINIFY: '' } });
  run('npm', ['run', 'release'], { env: { ...buildEnv, RELEASE_PATH: 'release' } });
  if (!fs.existsSync(path.join(sourceDir, 'release', 'lib', 'vscode', 'out', 'server-main.js'))) {
    throw new Error('Code-Server release 未生成 lib/vscode/out/server-main.js');
  }
  return path.join(sourceDir, 'release');
}

function locateExecutable(root = sourceDir) {
  const candidates = [
    path.join(root, 'bin', 'code-server'),
    path.join(root, 'lib', 'vscode', 'bin', 'code-server'),
    path.join(root, 'out', 'node', 'entry.js'),
  ];
  return candidates.find((file) => fs.existsSync(file)) || null;
}

function install(sourceRoot = sourceDir) {
  const sourceExecutable = locateExecutable(sourceRoot);
  const executable = sourceExecutable && sourceExecutable.replace(sourceRoot, sourceRoot);
  if (!executable) throw new Error('源码构建完成但未找到 Code-Server 启动文件（bin/code-server 或 out/node/entry.js）');
  fs.rmSync(installDir, { recursive: true, force: true });
  fs.mkdirSync(installDir, { recursive: true });
  // Preserve the upstream tree because the launcher resolves adjacent assets.
  fs.cpSync(sourceRoot, installDir, { recursive: true, filter: (source) => !source.includes(`${path.sep}.git${path.sep}`) && !source.endsWith(`${path.sep}.git`) });
  const installed = executable.replace(sourceRoot, installDir);
  if (process.platform !== 'win32' && fs.existsSync(installed)) fs.chmodSync(installed, 0o755);
  console.log(`Code-Server 源码构建产物：${installed}`);
  console.log(`启动：QLOSU_CODE_SERVER_BIN=${installed} npm run code-server`);
}

function rebuildNativeModules(sourceRoot = sourceDir) {
  const vscodeRoot = path.join(sourceRoot, 'lib', 'vscode');
  const spdlogPackage = path.join(vscodeRoot, 'node_modules', '@vscode', 'spdlog', 'package.json');
  if (!fs.existsSync(spdlogPackage)) return;
  // The source tree may be built with a different Node ABI than the host that
  // launches QLosu. Rebuild this required VS Code server dependency so a
  // missing binding cannot turn the workbench into a misleading HTTP 404.
  require('./repair-code-server').rebuild(vscodeRoot);
}

try {
  if (process.platform !== 'linux' || process.arch !== 'x64') {
    console.warn(`当前平台为 ${process.platform}/${process.arch}；脚本仍尝试构建，但主要验证目标是 Fedora x86_64。`);
  }
  ensureSource();
  const buildRoot = build();
  rebuildNativeModules(buildRoot);
  install(buildRoot);
} catch (error) {
  console.error(`Code-Server 源码构建失败：${error.message}`);
  process.exitCode = 1;
}
