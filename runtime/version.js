const fs = require('node:fs');
const path = require('node:path');

const HEADER = /^\s*(?:\/\/|#)\s*losu[-_ ]version\s*[:=]\s*([\w.-]+)/im;

function readWorkspaceConfig(workspace) {
  try {
    const file = path.join(workspace, 'qlosu.json');
    return JSON.parse(fs.readFileSync(file, 'utf8')) || {};
  } catch {
    return {};
  }
}

function readSource(file, code) {
  if (typeof code === 'string') return code;
  if (!file) return '';
  try { return fs.readFileSync(file, 'utf8'); } catch { return ''; }
}

/** Resolve one runtime version for run, diagnose and trace requests. */
function resolveRuntimeVersion({ workspace = process.cwd(), version, file, code } = {}) {
  if (typeof version === 'string' && version.trim()) return version.trim();
  const source = readSource(file, code);
  const declared = source.match(HEADER)?.[1];
  if (declared) return declared;
  const config = readWorkspaceConfig(workspace);
  return config.losuVersion || config.runtimeVersion || '2.0.0';
}

module.exports = { resolveRuntimeVersion, readWorkspaceConfig };
