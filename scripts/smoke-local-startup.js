#!/usr/bin/env node
const assert = require('node:assert/strict');
const path = require('node:path');
const { startServer } = require('../server/app');
const { Workspace } = require('../server/workspace');
const codeServerManager = require('../apps/code-server/manager');
const { apiEndpoint } = require('../extensions/qlosu-vscode/api');
const { settingsPath, readSettings } = require('../apps/code-server/workbench-profile');

const root = path.resolve(__dirname, '..');
const workspace = new Workspace(root);
const baseHeaders = { 'content-type': 'application/json' };
const REQUEST_TIMEOUT_MS = 10_000;
const CLOSE_TIMEOUT_MS = 10_000;

function timeoutSignal() {
  return AbortSignal.timeout(REQUEST_TIMEOUT_MS);
}

function withTimeout(operation, label, timeoutMs = CLOSE_TIMEOUT_MS, onTimeout) {
  let timer;
  const timeout = new Promise((_, reject) => {
    timer = setTimeout(() => {
      try { onTimeout?.(); } finally {
        reject(new Error(`${label} timed out after ${timeoutMs}ms`));
      }
    }, timeoutMs);
    timer.unref?.();
  });
  return Promise.race([Promise.resolve().then(operation), timeout]).finally(() => clearTimeout(timer));
}

async function expectResource(base, route, type) {
  const response = await fetch(new URL(route, base), { signal: timeoutSignal() });
  assert.equal(response.status, 200, `${route} should return HTTP 200`);
  assert.match(response.headers.get('content-type') || '', new RegExp(`^${type}`), `${route} should have ${type} content type`);
  await response.arrayBuffer();
}

async function main() {
  const api = await startServer({ port: 0, host: '127.0.0.1', workspace });
  let codeServer;
  try {
    await expectResource(api.url, '/', 'text/html');
    await expectResource(api.url, '/img/LOGO.png', 'image/png');
    await expectResource(api.url, '/vendor/xterm/css/xterm.css', 'text/css');
    await expectResource(api.url, '/vendor/xterm/lib/xterm.js', 'application/javascript');
    await expectResource(api.url, '/vendor/drawio/viewer-static.min.js', 'application/javascript');

    const flowResponse = await fetch(apiEndpoint(api.url, '/api/flowchart'), {
      method: 'POST', headers: baseHeaders, signal: timeoutSignal(),
      body: JSON.stringify({ code: 'main {\n  let answer = 42;\n  println(answer);\n}', title: 'smoke.losu' }),
    });
    assert.equal(flowResponse.status, 200, 'flowchart endpoint should return HTTP 200');
    const flow = await flowResponse.json();
    assert.match(flow.xml, /<mxfile[\s>]/, 'flowchart endpoint should return draw.io XML');

    const runtimeResponse = await fetch(new URL('/api/runtime/versions', api.url), { signal: timeoutSignal() });
    assert.equal(runtimeResponse.status, 200, 'runtime versions endpoint should return HTTP 200');
    const runtimeVersions = await runtimeResponse.json();
    assert.ok(runtimeVersions.versions.some((version) => version.id === 'losu-2.0.0'), 'Losu 2.0 runtime should be listed');

    const terminalResponse = await fetch(apiEndpoint(api.url, '/api/terminal/exec'), {
      method: 'POST', headers: baseHeaders, signal: timeoutSignal(),
      body: JSON.stringify({ command: "node -e \"process.stdout.write('terminal-ok')\"" }),
    });
    assert.equal(terminalResponse.status, 200, 'terminal endpoint should return HTTP 200');
    const terminal = await terminalResponse.json();
    assert.equal(terminal.exitCode, 0, 'terminal command should succeed');
    assert.match(terminal.stdout, /terminal-ok/, 'terminal should return command output');

    process.env.QLOSU_SERVER_URL = api.url;
    codeServer = await codeServerManager.start({ workspace: root });
    const workbench = await fetch(codeServer.url, { signal: timeoutSignal() });
    assert.equal(workbench.status, 200, 'Code-Server workbench should return HTTP 200');
    await workbench.arrayBuffer();
    assert.ok(codeServer.child.spawnargs.includes(root), 'Code-Server should receive the selected workspace');
    const workspaceWorkbench = await fetch(`${codeServer.url}?folder=${encodeURIComponent(root)}`, { signal: timeoutSignal() });
    assert.equal(workspaceWorkbench.status, 200, 'Code-Server workspace URL should return HTTP 200');
    await workspaceWorkbench.arrayBuffer();
    const settings = readSettings(settingsPath(codeServerManager.config.userDataRoot));
    assert.equal(settings['window.title'], '洛书 IDE — ${activeEditorShort}', '洛书 workbench title should be configured');
    assert.equal(settings['qlosu.workbenchMode'], true, '洛书 workbench mode should be enabled by default');
    assert.equal(settings['telemetry.telemetryLevel'], 'off', '洛书 workbench should disable telemetry');
  } finally {
    let closeError;
    if (codeServer) {
      try {
        await withTimeout(() => codeServer.close(), 'Code-Server close');
      } catch (error) {
        closeError = error;
      }
    }
    try {
      await withTimeout(() => api.close(), 'API close', CLOSE_TIMEOUT_MS, () => {
        api.server.closeAllConnections?.();
      });
    } catch (error) {
      closeError ||= error;
    }
    if (closeError) throw closeError;
  }
  assert.ok(!codeServer || codeServer.child.exitCode !== null, 'Code-Server child should stop on close');
  console.log('PASS local workbench assets, logo, terminal, flowchart API, and Code-Server workspace');
}

main().catch((error) => {
  console.error(error.stack || error.message || error);
  process.exitCode = 1;
});
