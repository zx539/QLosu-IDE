const fs = require('node:fs');
const path = require('node:path');

function nodeExecutable({ root = path.resolve(__dirname, '..'), env = process.env } = {}) {
  if (env.QLOSU_NODE_BIN) return env.QLOSU_NODE_BIN;
  const bundled = path.join(root, 'vendor', 'node', `${process.platform}-${process.arch}`, process.platform === 'win32' ? 'node.exe' : 'node');
  if (fs.existsSync(bundled)) return bundled;
  return process.versions.electron ? 'node' : process.execPath;
}
module.exports = { nodeExecutable };
