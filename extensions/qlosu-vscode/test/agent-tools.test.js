const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const test = require('node:test');
const { createAgentTools } = require('../agent');

function harness(root, answer = '取消') {
  const calls = [], writes = [];
  const uri = (fsPath) => ({ fsPath, scheme: 'file' });
  const vscode = {
    Uri: { joinPath: (base, ...parts) => uri(path.join(base.fsPath, ...parts)) },
    window: { showWarningMessage: async (...args) => { calls.push(['confirm', ...args]); return answer; } },
    commands: { executeCommand: async (...args) => { calls.push(args); } },
    workspace: {
      workspaceFolders: [{ uri: uri(root) }],
      openTextDocument: async ({ content }) => { calls.push(['preview', content]); return { uri: { scheme: 'untitled', fsPath: '' } }; },
      fs: { readFile: (u) => fs.promises.readFile(u.fsPath), writeFile: async (u, content) => { writes.push(u.fsPath); await fs.promises.writeFile(u.fsPath, content); } },
    },
  };
  const execute = createAgentTools({ vscode, callServer: async (...args) => { calls.push(args); return { code: 0, stdout: 'ok', exitCode: 0 }; }, getDocument: async () => ({ languageId: 'losu', fileName: 'main.els', getText: () => 'print(42)' }), getVersion: async () => 'losu-1.0', openDiagram: async () => {} });
  return { execute, calls, writes };
}
test('cancelled file writes and terminal execution have no side effects', async () => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-agent-tools-'));
  try {
    fs.writeFileSync(path.join(root, 'a.txt'), 'old');
    const h = harness(root);
    assert.equal((await h.execute('write_file', { path: 'a.txt', content: 'new' })).cancelled, true);
    assert.equal((await h.execute('run_terminal', { command: 'echo no' })).cancelled, true);
    assert.equal(fs.readFileSync(path.join(root, 'a.txt'), 'utf8'), 'old');
    assert.equal(h.writes.length, 0);
    assert.equal(h.calls.filter((c) => c[0] === '/api/terminal/exec').length, 0);
  } finally { fs.rmSync(root, { recursive: true, force: true }); }
});
test('write preview opens an actual diff before confirmation and writes after approval', async () => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-agent-tools-'));
  try {
    fs.writeFileSync(path.join(root, 'a.txt'), 'old');
    const h = harness(root, '确认');
    await h.execute('write_file', { path: 'a.txt', content: '**new**' });
    const diffIndex = h.calls.findIndex((c) => c[0] === 'vscode.diff');
    assert.ok(diffIndex >= 0);
    assert.ok(diffIndex < h.calls.findIndex((c) => c[0] === 'confirm'));
    assert.equal(fs.readFileSync(path.join(root, 'a.txt'), 'utf8'), '**new**');
    await h.execute('run_file', {});
    assert.equal(h.calls.find((c) => c[0] === '/api/runtime/run')[1].version, 'losu-1.0');
  } finally { fs.rmSync(root, { recursive: true, force: true }); }
});
test('Agent read/write refuse traversal and symlink escapes', async () => {
  const temp = fs.mkdtempSync(path.join(os.tmpdir(), 'qlosu-agent-path-'));
  const root = path.join(temp, 'workspace'); fs.mkdirSync(root);
  fs.writeFileSync(path.join(temp, 'outside'), 'private');
  fs.symlinkSync(path.join(temp, 'outside'), path.join(root, 'link'));
  fs.symlinkSync(path.join(temp, 'missing'), path.join(root, 'dangling'));
  try {
    const h = harness(root, '确认');
    await assert.rejects(h.execute('read_file', { path: '../outside' }));
    await assert.rejects(h.execute('read_file', { path: 'link' }));
    await assert.rejects(h.execute('write_file', { path: 'link', content: 'changed' }));
    await assert.rejects(h.execute('write_file', { path: 'dangling', content: 'changed' }));
    assert.equal(fs.existsSync(path.join(temp, 'missing')), false);
    assert.equal(fs.readFileSync(path.join(temp, 'outside'), 'utf8'), 'private');
  } finally { fs.rmSync(temp, { recursive: true, force: true }); }
});
