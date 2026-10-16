const { startServer } = require('../../server/app');
const { start, continueInstallArgs } = require('./manager');
const { Workspace } = require('../../server/workspace');

async function main() {
  const workspace = process.env.QLOSU_WORKSPACE || process.argv[2];
  const api = await startServer({ port: 0, host: '127.0.0.1', workspace: new Workspace(workspace) });
  try {
    process.env.QLOSU_SERVER_URL = api.url;
    const codeServer = await start({ workspace, extraArgs: continueInstallArgs() });
    console.log(`QLosu API: ${api.url}`);
    console.log(`Code-Server: ${codeServer.url}`);
    let shuttingDown = false;
    const shutdown = async (code = 0) => {
      if (shuttingDown) return;
      shuttingDown = true;
      await codeServer.close();
      await api.close();
      process.exit(code);
    };
    process.once('SIGINT', () => shutdown(0));
    process.once('SIGTERM', () => shutdown(0));
    codeServer.child.once('exit', (code) => { if (!shuttingDown) shutdown(Number.isInteger(code) ? code : 1); });
    return codeServer;
  } catch (error) {
    await api.close();
    console.error(error.message);
    if (error.stderr) console.error(error.stderr.trim());
    if (error.installHint) console.error(`安装地址：${error.installHint}`);
    process.exitCode = 1;
  }
}

if (require.main === module) main();
module.exports = { main };
