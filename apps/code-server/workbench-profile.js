const fs = require('fs');
const path = require('path');

const managedDefaults = Object.freeze({
  'window.title': '洛书 IDE — ${activeEditorShort}',
  'workbench.startupEditor': 'none',
  'window.commandCenter': false,
  'workbench.layoutControl.enabled': false,
  'workbench.tips.enabled': false,
  'extensions.ignoreRecommendations': true,
  'telemetry.telemetryLevel': 'off',
  'qlosu.workbenchMode': true,
});

function settingsPath(userDataRoot) {
  return path.join(userDataRoot, 'User', 'settings.json');
}

function readSettings(file) {
  try {
    const value = JSON.parse(fs.readFileSync(file, 'utf8'));
    return value && typeof value === 'object' && !Array.isArray(value) ? value : {};
  } catch (error) {
    if (error.code === 'ENOENT') return {};
    throw new Error(`无法读取 Code-Server 设置：${file}（${error.message}）`);
  }
}

function ensureLosuWorkbench(userDataRoot) {
  const file = settingsPath(userDataRoot);
  const settings = readSettings(file);
  let changed = false;
  for (const [key, value] of Object.entries(managedDefaults)) {
    if (!(key in settings)) {
      settings[key] = value;
      changed = true;
    }
  }
  if (changed) {
    fs.mkdirSync(path.dirname(file), { recursive: true });
    fs.writeFileSync(file, `${JSON.stringify(settings, null, 2)}\n`);
  }
  return { file, settings, changed };
}

module.exports = { managedDefaults, settingsPath, readSettings, ensureLosuWorkbench };
