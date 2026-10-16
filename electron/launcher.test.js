const assert = require('node:assert/strict');
const path = require('node:path');
const {
  electronExecutablePath,
  findElectronBinary,
  requestedWorkspace,
} = require('./launcher');

const root = path.resolve(__dirname, '..');
assert.match(electronExecutablePath({ root }), /node_modules[\\/]electron[\\/]dist/);
assert.equal(requestedWorkspace(['node', 'desktop', '--workspace=/tmp'], {}), '/tmp');
assert.equal(requestedWorkspace(['node', 'desktop', '--workspace', '/tmp'], {}), '/tmp');
assert.equal(requestedWorkspace(['node', 'desktop', '--', '/tmp'], {}), '/tmp');
assert.equal(requestedWorkspace(['node', 'desktop'], { QLOSU_WORKSPACE: '/var/tmp' }), '/var/tmp');
assert.equal(findElectronBinary({ root, env: { QLOSU_ELECTRON_BIN: '/does/not/exist' } }), null);
console.log('PASS Electron launcher path, workspace, and missing-binary checks');
