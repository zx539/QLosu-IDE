const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const test = require('node:test');
const { ensureLosuWorkbench } = require('./workbench-profile');

test('workbench profile preserves user settings and adds missing Losu defaults', () => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-workbench-'));
  const file = path.join(root, 'User', 'settings.json');
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, JSON.stringify({ 'window.title': 'My ${activeEditorShort}', 'editor.fontSize': 15 }));

  const first = ensureLosuWorkbench(root);
  assert.equal(first.changed, true);
  assert.equal(first.settings['window.title'], 'My ${activeEditorShort}');
  assert.equal(first.settings['editor.fontSize'], 15);
  assert.equal(first.settings['workbench.startupEditor'], 'none');
  assert.equal(first.settings['window.commandCenter'], false);
  assert.equal(first.settings['workbench.layoutControl.enabled'], false);
  assert.equal(first.settings['qlosu.workbenchMode'], true);

  const second = ensureLosuWorkbench(root);
  assert.equal(second.changed, false);
  assert.deepEqual(second.settings, first.settings);
  fs.rmSync(root, { recursive: true, force: true });
});
