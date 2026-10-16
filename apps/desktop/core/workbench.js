const path = require('node:path');
const { Workspace } = require('../../server/core/workspace');

async function startWorkbench({ workspace, startServer = require('../../server/core/app').startServer, startCodeServer = require('../../code-server/manager').start, extraArgs = [] }) {
  const selected = path.resolve(workspace);
  const api = await startServer({ port: 0, host: '127.0.0.1', workspace: new Workspace(selected) });
  let codeServer;
  let startupError;
  try {
    process.env.QLOSU_SERVER_URL = api.url;
    codeServer = await startCodeServer({ workspace: selected, extraArgs });
  } catch (error) { startupError = error; }
  let closing;
  return {
    api, codeServer, workspace: selected, startupError,
    url: codeServer ? `${codeServer.url}?folder=${encodeURIComponent(selected)}` : api.url,
    close() {
      return closing ||= (async () => { try { await codeServer?.close(); } finally { await api.close(); } })();
    },
  };
}
module.exports = { startWorkbench };
