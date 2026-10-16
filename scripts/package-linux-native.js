#!/usr/bin/env node
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

const root = path.resolve(__dirname, '..');
const version = require('../package.json').version;
const source = path.join(root, 'dist', 'linux-unpacked');
const output = path.join(root, 'dist');
const arch = process.arch === 'x64' ? 'amd64' : process.arch;
const rpmArch = process.arch === 'x64' ? 'x86_64' : process.arch;
if (!fs.existsSync(path.join(source, 'resources', 'app'))) throw new Error('请先运行 npm run pack 生成 dist/linux-unpacked');

function run(command, args) {
  const result = spawnSync(command, args, { stdio: 'inherit' });
  if (result.error) throw result.error;
  if (result.status !== 0) throw new Error(`${command} 退出码 ${result.status}`);
}
function copyTree(from, to) {
  fs.rmSync(to, { recursive: true, force: true });
  fs.mkdirSync(path.dirname(to), { recursive: true });
  fs.cpSync(from, to, { recursive: true, dereference: false });
}
function write(file, text, mode) {
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, text);
  if (mode) fs.chmodSync(file, mode);
}
function desktopFile() {
  return `[Desktop Entry]\nName=QLosu IDE\nComment=洛书量子编程平台\nExec=/opt/QLosu-IDE/qlosu-standalone %U\nIcon=qlosu-standalone\nType=Application\nCategories=Development;IDE;\nTerminal=false\n`;
}
function repairAbsoluteSymlinks(tree) {
  const appRoot = path.join(tree, 'resources', 'app');
  const codeServerRoot = path.join(appRoot, 'vendor', 'code-server', '4.95.3', 'linux-amd64');
  const walk = (dir) => {
    for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
      const file = path.join(dir, entry.name);
      if (entry.isSymbolicLink()) {
        const target = fs.readlinkSync(file);
        if (!path.isAbsolute(target)) continue;
        let candidate;
        const cacheMarker = `${path.sep}.cache${path.sep}code-server-source${path.sep}node_modules${path.sep}`;
        const cacheIndex = target.indexOf(cacheMarker);
        if (cacheIndex >= 0) candidate = path.join(codeServerRoot, 'node_modules', target.slice(cacheIndex + cacheMarker.length));
        const sourceMarker = `${path.sep}vendor${path.sep}code-server${path.sep}4.95.3${path.sep}linux-amd64${path.sep}`;
        const sourceIndex = target.indexOf(sourceMarker);
        if (!candidate && sourceIndex >= 0) candidate = path.join(codeServerRoot, target.slice(sourceIndex + sourceMarker.length));
        if (candidate && fs.existsSync(candidate)) {
          fs.unlinkSync(file);
          fs.symlinkSync(path.relative(path.dirname(file), candidate), file);
        }
      } else if (entry.isDirectory()) walk(file);
    }
  };
  walk(appRoot);
}

repairAbsoluteSymlinks(source);

const debRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-deb-'));
copyTree(source, path.join(debRoot, 'opt', 'QLosu-IDE'));
copyTree(path.join(root, 'img', 'LOGO-app.png'), path.join(debRoot, 'usr', 'share', 'icons', 'hicolor', '512x512', 'apps', 'qlosu-standalone.png'));
write(path.join(debRoot, 'usr', 'share', 'applications', 'qlosu-standalone.desktop'), desktopFile());
write(path.join(debRoot, 'DEBIAN', 'control'), `Package: qlosu-standalone\nVersion: ${version}\nSection: devel\nPriority: optional\nArchitecture: ${arch}\nMaintainer: QLosu Contributors <zx539@users.noreply.github.com>\nDepends: libgtk-3-0, libnotify4, libnss3, libxss1, libxtst6, xdg-utils, libatspi2.0-0, libuuid1, libsecret-1-0\nDescription: QLosu 量子编程平台独立本地运行版\n 洛书 IDE、运行时、终端和离线文档。\n`);
const deb = path.join(output, `QLosu-IDE-${version}-linux-${arch}.deb`);
fs.rmSync(deb, { force: true });
run('dpkg-deb', ['--build', '--root-owner-group', '-Z', 'gzip', '-z', '1', debRoot, deb]);

const rpmTop = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-rpm-'));
const spec = path.join(rpmTop, 'SPECS', 'qlosu-standalone.spec');
write(spec, `Name: qlosu-standalone\nVersion: ${version}\nRelease: 1\nSummary: QLosu 量子编程平台\nLicense: MIT\nURL: https://github.com/zx539/QLosu-IDE\nBuildArch: ${rpmArch}\n%description\n洛书 IDE、运行时、终端和离线文档。\n%install\nmkdir -p %{buildroot}/opt/QLosu-IDE %{buildroot}/usr/share/applications %{buildroot}/usr/share/icons/hicolor/512x512/apps\ncp -a ${source}/. %{buildroot}/opt/QLosu-IDE/\ncp -a ${path.join(root, 'img', 'LOGO-app.png')} %{buildroot}/usr/share/icons/hicolor/512x512/apps/qlosu-standalone.png\ncat > %{buildroot}/usr/share/applications/qlosu-standalone.desktop <<'EOF'\n${desktopFile()}EOF\n%files\n/opt/QLosu-IDE\n/usr/share/applications/qlosu-standalone.desktop\n/usr/share/icons/hicolor/512x512/apps/qlosu-standalone.png\n`);
const rpm = path.join(output, `QLosu-IDE-${version}-linux-${arch}.rpm`);
fs.rmSync(rpm, { force: true });
run('rpmbuild', ['--define', `_topdir ${rpmTop}`, '--define', '_binary_payload w9.gzdio', '-bb', spec]);
const rpmDir = path.join(rpmTop, 'RPMS', rpmArch);
const builtRpm = fs.existsSync(rpmDir) ? fs.readdirSync(rpmDir).filter((name) => name.endsWith('.rpm')).map((name) => path.join(rpmDir, name))[0] : null;
if (!builtRpm) throw new Error(`RPM 未生成：${rpmDir}`);
fs.copyFileSync(builtRpm, rpm);
console.log(`PASS Linux deb: ${deb}`);
console.log(`PASS Linux rpm: ${rpm}`);
