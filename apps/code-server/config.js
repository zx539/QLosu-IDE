const path = require('path');

const CODE_SERVER_VERSION = process.env.QLOSU_CODE_SERVER_VERSION || '4.95.3';
const ROOT = path.join(__dirname, '..', '..');
const DATA_ROOT = process.env.QLOSU_DATA_DIR || path.join(ROOT, '.qlosu');

function artifactPlatform(platform = process.platform) {
  return platform === 'darwin' ? 'macos' : platform === 'win32' ? 'win32' : platform;
}

function artifactArch(arch = process.arch) {
  return ({ x64: 'amd64', arm64: 'arm64', ia32: '386' })[arch] || arch;
}

function installRoot(platform = process.platform, arch = process.arch) {
  return path.join(ROOT, 'vendor', 'code-server', CODE_SERVER_VERSION,
    `${artifactPlatform(platform)}-${artifactArch(arch)}`);
}

module.exports = {
  root: ROOT,
  version: CODE_SERVER_VERSION,
  bindHost: process.env.QLOSU_CODE_SERVER_HOST || '127.0.0.1',
  extensionRoot: path.join(DATA_ROOT, 'extensions'),
  qlosuExtension: path.join(ROOT, 'extensions', 'qlosu-vscode'),
  dataRoot: path.join(DATA_ROOT, 'code-server'),
  userDataRoot: path.join(DATA_ROOT, 'user-data'),
  vendorRoot: installRoot(),
  artifactPlatform,
  artifactArch,
  installRoot,
};
