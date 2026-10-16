const fs = require('fs');
const os = require('os');
const path = require('path');

function electronExecutablePath({ root = path.resolve(__dirname, '..', '..', '..'), platform = process.platform, env = process.env } = {}) {
  const packageRoot = path.join(root, 'node_modules', 'electron');
  const overrideRoot = env.ELECTRON_OVERRIDE_DIST_PATH;
  if (overrideRoot) {
    return path.join(overrideRoot, platform === 'darwin' ? 'Electron.app/Contents/MacOS/Electron' : platform === 'win32' ? 'electron.exe' : 'electron');
  }

  let relativePath;
  try {
    relativePath = fs.readFileSync(path.join(packageRoot, 'path.txt'), 'utf8').trim();
  } catch {
    relativePath = platform === 'darwin'
      ? 'Electron.app/Contents/MacOS/Electron'
      : platform === 'win32' ? 'electron.exe' : 'electron';
  }
  return path.join(packageRoot, 'dist', relativePath);
}

function isExecutable(file) {
  try {
    fs.accessSync(file, fs.constants.X_OK);
    return fs.statSync(file).isFile();
  } catch {
    return false;
  }
}

function findElectronBinary({ root, env = process.env } = {}) {
  const explicit = env.QLOSU_ELECTRON_BIN;
  if (explicit) return isExecutable(explicit) ? path.resolve(explicit) : null;
  const bundled = electronExecutablePath({ root, env });
  if (isExecutable(bundled)) return bundled;
  return null;
}

function installHint({ root = path.resolve(__dirname, '..', '..', '..') } = {}) {
  const packageRoot = path.join(root, 'node_modules', 'electron');
  return [
    `Electron 二进制不存在：${electronExecutablePath({ root })}`,
    `请在项目根目录运行 npm install，或设置 QLOSU_ELECTRON_BIN 指向 Electron ${os.platform()} 可执行文件。`,
    `如果 npm 安装缓存损坏，可删除 ${packageRoot} 后重新安装。`,
  ].join('\n');
}

function requestedWorkspace(argv = process.argv, env = process.env) {
  if (env.QLOSU_WORKSPACE) return env.QLOSU_WORKSPACE;
  for (let index = 0; index < argv.length; index += 1) {
    const value = argv[index];
    if (value.startsWith('--workspace=')) return value.slice('--workspace='.length);
    if (value === '--workspace' && argv[index + 1]) return argv[index + 1];
  }
  const separator = argv.indexOf('--');
  return separator >= 0 ? argv[separator + 1] : undefined;
}

module.exports = {
  electronExecutablePath,
  findElectronBinary,
  installHint,
  isExecutable,
  requestedWorkspace,
};
