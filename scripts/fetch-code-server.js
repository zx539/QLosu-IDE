#!/usr/bin/env node
/**
 * Fetch and unpack the pinned Code-Server release for the current platform.
 * Set QLOSU_CODE_SERVER_SHA256 to enforce a checksum (recommended for CI).
 */
const https = require('https');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');
const { spawnSync } = require('child_process');
const { config } = require('../apps/code-server/manager');

const platform = config.artifactPlatform();
const arch = config.artifactArch();
// Linux/macOS releases are tarballs; Windows releases are zip archives.
const ext = process.platform === 'win32' ? '.zip' : '.tar.gz';
const artifact = `code-server-${config.version}-${platform}-${arch}${ext}`;
const url = process.env.QLOSU_CODE_SERVER_URL ||
  `https://github.com/coder/code-server/releases/download/v${config.version}/${artifact}`;
const downloadDir = path.join(config.root, 'vendor', 'code-server', 'downloads');
const output = path.join(downloadDir, artifact);
const partialOutput = `${output}.part`;
const installDir = config.installRoot();
const expectedSha = (process.env.QLOSU_CODE_SERVER_SHA256 || '').trim().toLowerCase();
const verifyRelease = process.env.QLOSU_CODE_SERVER_VERIFY_RELEASE === '1';
const checksumUrl = process.env.QLOSU_CODE_SERVER_CHECKSUM_URL ||
  `https://github.com/coder/code-server/releases/download/v${config.version}/checksums.txt`;

function fail(message) { console.error(`Code-Server 下载失败：${message}`); process.exitCode = 1; }
function sha256(file) {
  const hash = crypto.createHash('sha256');
  hash.update(fs.readFileSync(file));
  return hash.digest('hex');
}
function download(target, destination, redirectCount = 0) {
  if (redirectCount > 5) return Promise.reject(new Error('重定向次数过多'));
  return new Promise((resolve, reject) => {
    https.get(target, (response) => {
      if (response.statusCode >= 300 && response.statusCode < 400 && response.headers.location) {
        response.resume();
        return download(new URL(response.headers.location, target).toString(), destination, redirectCount + 1).then(resolve, reject);
      }
      if (response.statusCode !== 200) {
        response.resume();
        return reject(new Error(`HTTP ${response.statusCode}`));
      }
      const file = fs.createWriteStream(destination, { flags: 'wx' });
      response.pipe(file);
      const fail = (error) => {
        file.destroy();
        try { fs.unlinkSync(destination); } catch {}
        reject(error);
      };
      file.once('finish', () => file.close(() => resolve(destination)));
      file.once('error', fail);
      response.once('error', fail);
    }).on('error', reject);
  });
}
function parseChecksumManifest(text, name) {
  const target = path.basename(name);
  for (const line of String(text || '').split(/\r?\n/)) {
    const match = line.trim().match(/^([a-f0-9]{64})\s+[* ]?(.+)$/i);
    if (match && path.basename(match[2].trim()) === target) return match[1].toLowerCase();
  }
  return null;
}
async function expectedChecksum() {
  if (expectedSha) {
    if (!/^[a-f0-9]{64}$/.test(expectedSha)) throw new Error('QLOSU_CODE_SERVER_SHA256 必须是 64 位十六进制 SHA-256');
    return expectedSha;
  }
  if (!verifyRelease) return null;
  const manifestPath = `${partialOutput}.checksums`;
  try {
    await download(checksumUrl, manifestPath);
    const checksum = parseChecksumManifest(fs.readFileSync(manifestPath, 'utf8'), artifact);
    if (!checksum) throw new Error(`checksum 清单中未找到 ${artifact}`);
    return checksum;
  } finally {
    try { fs.unlinkSync(manifestPath); } catch {}
  }
}
function assertSafeTarEntries(file) {
  const result = spawnSync('tar', ['-tzf', file], { encoding: 'utf8' });
  if (result.status !== 0) throw new Error(`无法读取归档目录：${result.stderr || 'tar 退出失败'}`);
  validateArchiveEntries(result.stdout.split(/\r?\n/).filter(Boolean));
}
function validateArchiveEntries(entries) {
  for (const entry of entries) {
    const normalized = path.posix.normalize(entry.replace(/\\/g, '/').replace(/^\.\//, ''));
    if (normalized === '..' || normalized.startsWith('../') || normalized.startsWith('/')) {
      throw new Error(`归档包含非法路径：${entry}`);
    }
  }
}
function replaceDownload(source, destination) {
  if (process.platform === 'win32') {
    fs.rmSync(destination, { force: true });
  }
  fs.renameSync(source, destination);
}
function extract() {
  fs.rmSync(installDir, { recursive: true, force: true });
  fs.mkdirSync(installDir, { recursive: true });
  if (process.platform === 'win32') {
    const result = spawnSync('powershell.exe', ['-NoProfile', '-Command',
      `Expand-Archive -LiteralPath '${output.replace(/'/g, "''")}' -DestinationPath '${installDir.replace(/'/g, "''")}' -Force`],
    { stdio: 'inherit' });
    if (result.status !== 0) throw new Error('Expand-Archive 执行失败');
  } else {
    assertSafeTarEntries(output);
    const result = spawnSync('tar', ['-xzf', output, '-C', installDir, '--strip-components=1'], { stdio: 'inherit' });
    if (result.status !== 0) throw new Error('tar 解压失败');
  }
  const expected = process.platform === 'win32' ? 'code-server.exe' : 'code-server';
  const found = [];
  const walk = (dir) => {
    for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
      const current = path.join(dir, entry.name);
      if (entry.isDirectory()) walk(current);
      else if (entry.name === expected) found.push(current);
    }
  };
  walk(installDir);
  if (!found.length) throw new Error(`解压后未找到可执行文件：${expected}`);
  const executable = found.find((file) => path.basename(path.dirname(file)) === 'bin') || found[0];
  if (process.platform !== 'win32') fs.chmodSync(executable, 0o755);
}
async function main() {
  try {
    fs.mkdirSync(downloadDir, { recursive: true });
    if (!fs.existsSync(output) || process.env.QLOSU_CODE_SERVER_FORCE === '1') {
      console.log(`下载 Code-Server ${config.version}：${url}`);
      try { fs.unlinkSync(partialOutput); } catch {}
      await download(url, partialOutput);
      replaceDownload(partialOutput, output);
    } else console.log(`使用已有下载文件：${output}`);
    const digest = sha256(output);
    const checksum = await expectedChecksum();
    if (checksum && digest !== checksum) throw new Error(`SHA-256 校验失败：期望 ${checksum}，实际 ${digest}`);
    if (!checksum) console.warn(`警告：未设置 Code-Server SHA-256，实际 SHA-256：${digest}`);
    extract();
    console.log(`已安装 Code-Server ${config.version}：${installDir}`);
  } catch (error) {
    try { fs.unlinkSync(partialOutput); } catch {}
    fail(error.message);
  }
}

if (require.main === module) main();

module.exports = { parseChecksumManifest, assertSafeTarEntries, validateArchiveEntries, replaceDownload, sha256 };

