const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');

const manifest = JSON.parse(fs.readFileSync(path.join(__dirname, '..', 'package.json'), 'utf8'));

test('Losu commands are available in editor title and context menus', () => {
  const title = manifest.contributes.menus['editor/title'];
  const context = manifest.contributes.menus['editor/context'];
  for (const command of ['qlosu.runFile', 'qlosu.generateCircuit', 'qlosu.generateFlowchart']) {
    assert.ok(title.some((item) => item.command === command && item.when === 'editorLangId == losu'));
    assert.ok(context.some((item) => item.command === command && item.when === 'editorLangId == losu'));
  }
});

test('Losu language and bundled language server remain declared', () => {
  assert.equal(manifest.contributes.languages[0].id, 'losu');
  assert.equal(manifest.main, 'extension.js');
  assert.ok(fs.existsSync(path.join(__dirname, '..', '..', '..', 'language-server', 'losu-lsp', 'server.js')));
});

test('QLosu Agent is available as a first-class workbench view', () => {
  const views = manifest.contributes.views.qlosu;
  assert.ok(views.some((view) => view.id === 'qlosu.agent' && view.type === 'webview'));
  assert.ok(manifest.contributes.commands.some((command) => command.command === 'qlosu.openAgent'));
  assert.ok(manifest.activationEvents.includes('onView:qlosu.agent'));
});

test('bundled Losu language experience contains rich completion and hover metadata', () => {
  const source = fs.readFileSync(path.join(__dirname, '..', '..', '..', 'language-server', 'losu-lsp', 'server.js'), 'utf8');
  assert.match(source, /completionSnippets/);
  assert.match(source, /semanticTokenTypes/);
  assert.match(source, /symbolCatalog/);
  assert.match(source, /contents: \{ kind: 'markdown'/);
});
