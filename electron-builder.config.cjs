const fs = require('node:fs');
const path = require('node:path');
const manifest = require('./package.json');
const productionList = path.join(__dirname, '.cache', 'extension-production-files.json');
if (!fs.existsSync(productionList)) throw new Error('请先运行 npm run release:prepare。');
const crossWindows = process.argv.includes('--win') || process.argv.includes('win32');
module.exports = {
  ...manifest.build,
  asar: false,
  npmRebuild: false,
  electronVersion: manifest.devDependencies.electron,
  electronDist: crossWindows ? undefined : path.join(__dirname, 'node_modules', 'electron', 'dist'),
  artifactName: 'QLosu-IDE-${version}-${os}-${arch}.${ext}',
  files: [
    ...manifest.build.files.filter((file) => !file.startsWith('extensions/qlosu-vscode/node_modules/')),
    ...JSON.parse(fs.readFileSync(productionList, 'utf8')),
    'extensions/qlosu-vscode/markdown.js', '使用说明书.md', 'release-manifest.json', 'vendor/node/**/*',
    'scripts/repair-code-server.js', 'scripts/check-code-server-native.js',
    '!**/*.map', '!**/*.vsix', '!**/.git/**', '!**/node_modules/**/{test,tests,__tests__}/**',
    '!**/.env', '!**/.env.local', '!**/.aws/**', '!**/.ssh/**', '!**/.qlosu/**',
  ],
  linux: { ...manifest.build.linux, target: process.env.QLOSU_LINUX_TARGET ? process.env.QLOSU_LINUX_TARGET.split(',') : ['tar.gz'] },
  afterPack: async (context) => {
    // electron-builder excludes nested node_modules from normal files globs.
    // These are independent applications whose native bindings use bundled Node.
    const destination = path.join(context.appOutDir, 'resources', 'app');
    const copy = (relative) => {
      const source = path.join(__dirname, relative), target = path.join(destination, relative);
      fs.rmSync(target, { recursive: true, force: true });
      fs.cpSync(source, target, { recursive: true, dereference: false, filter: (file) => {
        const parts = path.relative(source, file).split(path.sep);
        return !parts.some((part) => ['.git', '.github', '.aws', '.ssh', '.qlosu', '.env', '.env.local', 'test', 'tests', '__tests__'].includes(part)) && !file.endsWith('.vsix');
      } });
    };
    const codeServer = `vendor/code-server/${require('./apps/code-server/config').version}/${require('./apps/code-server/config').artifactPlatform()}-${require('./apps/code-server/config').artifactArch()}`;
    copy(codeServer);
    const dependencies = JSON.parse(fs.readFileSync(productionList, 'utf8'));
    for (const glob of dependencies) copy(glob.replace(/\/\*\*\/\*$/, ''));
    for (const required of [
      `${codeServer}/node_modules/@coder/logger/package.json`,
      `${codeServer}/lib/vscode/node_modules/node-pty/build/Release/pty.node`,
      'extensions/qlosu-vscode/node_modules/vscode-languageclient/package.json',
      'extensions/qlosu-vscode/node_modules/sanitize-html/package.json',
    ]) if (!fs.existsSync(path.join(destination, required))) throw new Error(`发布包缺少必需资源：${required}`);
  },
};
