const { app, BrowserWindow, ipcMain, dialog, Menu, shell } = require('electron');
const path = require('node:path');
const fs = require('node:fs');
const { requestedWorkspace } = require('./launcher');
const { nodeExecutable } = require('../../../runtime/node');
const root = path.resolve(__dirname, '..', '..', '..');
const dataRoot = process.env.QLOSU_DATA_DIR || (app.isPackaged ? path.join(app.getPath('userData'), 'qlosu') : path.join(root, '.qlosu'));
if (process.env.QLOSU_DATA_DIR) app.setPath('userData', path.join(dataRoot, 'electron-profile'));
process.env.QLOSU_DATA_DIR = dataRoot;
process.env.QLOSU_NODE_BIN = nodeExecutable({ root });
const { startWorkbench } = require('./workbench');
const { continueInstallArgs } = require('../../code-server/manager');
const stateFile = path.join(dataRoot, 'desktop.json');
let mainWindow, session, closing, switching = false;
function previousWorkspace() {
  try { const value = JSON.parse(fs.readFileSync(stateFile, 'utf8')); if (fs.statSync(value.workspace).isDirectory()) return value.workspace; } catch {}
}
function saveWorkspace(workspace) {
  fs.mkdirSync(dataRoot, { recursive: true });
  fs.writeFileSync(stateFile, JSON.stringify({ workspace }, null, 2) + '\n');
}
async function shutdown() {
  return closing ||= session?.close().catch((error) => console.error(`工作台清理失败：${error.message}`));
}
async function loadWorkspace(workspace) {
  if (!fs.statSync(workspace).isDirectory()) throw new Error(`工作区目录不存在：${workspace}`);
  switching = true;
  try {
    await session?.close(); closing = undefined;
    session = await startWorkbench({ workspace, extraArgs: continueInstallArgs() });
    if (session.startupError) console.error(`Code-Server 启动失败，使用兼容页面：${session.startupError.message}`);
    const current = session;
    current.codeServer?.child.once('exit', (code, signal) => {
      if (closing || switching || session !== current || mainWindow?.isDestroyed()) return;
      console.error(`Code-Server 意外退出：${signal || code}`);
      mainWindow?.loadURL(current.api.url).catch(console.error);
      dialog.showErrorBox('洛书工作台已停止', '工作台进程意外退出，已打开兼容页面。请保存文件后重启应用。');
    });
    await mainWindow.loadURL(session.url); saveWorkspace(workspace);
    console.log(`QLosu API: ${session.api.url}`);
    console.log(`QLosu workspace: ${workspace}`);
    if (process.argv.includes('--qlosu-smoke')) console.log(`QLOSU_DESKTOP_READY ${JSON.stringify({ url: session.url, apiUrl: session.api.url, workspace, codeServerPid: session.codeServer?.child.pid, fallback: !!session.startupError })}`);
  } finally { switching = false; }
}
async function chooseWorkspace() {
  if (switching) return;
  const result = await dialog.showOpenDialog(mainWindow, { title: '打开洛书工作区', defaultPath: session?.workspace, properties: ['openDirectory', 'createDirectory'] });
  if (result.canceled || !result.filePaths[0] || result.filePaths[0] === session?.workspace) return;
  const confirmation = await dialog.showMessageBox(mainWindow, { type: 'question', message: '切换工作区会重新加载工作台，请先保存当前文件。', buttons: ['取消', '已保存并切换'], defaultId: 0, cancelId: 0 });
  if (confirmation.response !== 1) return;
  try { await loadWorkspace(result.filePaths[0]); } catch (error) { dialog.showErrorBox('无法打开工作区', error.message); }
}
function installMenu() {
  Menu.setApplicationMenu(Menu.buildFromTemplate([
    ...(process.platform === 'darwin' ? [{ role: 'appMenu' }] : []),
    { label: '文件', submenu: [{ label: '打开工作区…', accelerator: 'CmdOrCtrl+Shift+O', click: chooseWorkspace }, { type: 'separator' }, { role: 'close' }, { role: 'quit' }] },
    { label: '编辑', submenu: [{ role: 'undo' }, { role: 'redo' }, { type: 'separator' }, { role: 'cut' }, { role: 'copy' }, { role: 'paste' }, { role: 'selectAll' }] },
    { label: '视图', submenu: [{ role: 'reload' }, { role: 'toggleDevTools' }, { role: 'resetZoom' }, { role: 'zoomIn' }, { role: 'zoomOut' }, { role: 'togglefullscreen' }] },
    { label: '帮助', submenu: [{ label: '使用说明书', click: () => shell.openPath(path.join(root, '使用说明书.md')) }] },
  ]));
}
async function createWindow() {
  mainWindow = new BrowserWindow({ width: 1440, height: 960, minWidth: 1000, minHeight: 600, show: false, title: '洛书 IDE', icon: path.join(root, 'img/LOGO-app.png'), webPreferences: { nodeIntegration: false, contextIsolation: true, sandbox: true, webSecurity: true, preload: path.join(__dirname, 'preload.js') } });
  mainWindow.webContents.setWindowOpenHandler(({ url }) => { if (/^https?:\/\//i.test(url)) shell.openExternal(url); return { action: 'deny' }; });
  mainWindow.webContents.on('will-navigate', (event, url) => {
    const origins = [session?.api.url, session?.codeServer?.url].filter(Boolean).map((base) => new URL(base).origin);
    try { if (!origins.includes(new URL(url).origin)) event.preventDefault(); } catch { event.preventDefault(); }
  });
  mainWindow.webContents.on('will-prevent-unload', (event) => {
    const choice = dialog.showMessageBoxSync(mainWindow, { type: 'question', message: '工作台有尚未保存的内容，确认离开？', buttons: ['继续编辑', '离开'], defaultId: 0, cancelId: 0 });
    if (choice === 1) event.preventDefault();
  });
  mainWindow.on('closed', () => { mainWindow = null; });
  const requested = requestedWorkspace() || previousWorkspace();
  const workspace = path.resolve(requested || path.join(app.getPath('documents'), 'QLosu-Workspace'));
  if (!requested) fs.mkdirSync(workspace, { recursive: true });
  await loadWorkspace(workspace); mainWindow.show();
}
ipcMain.handle('dialog:openFile', async () => {
  const result = await dialog.showOpenDialog(mainWindow, { properties: ['openFile'], filters: [{ name: '洛书文件', extensions: ['losu', 'els'] }, { name: '所有文件', extensions: ['*'] }] });
  if (result.canceled || !result.filePaths[0]) return null;
  const file = result.filePaths[0]; return { path: file, name: path.basename(file), content: fs.readFileSync(file, 'utf8') };
});
ipcMain.handle('dialog:openFolder', async () => {
  const result = await dialog.showOpenDialog(mainWindow, { properties: ['openDirectory'] });
  return result.canceled ? null : result.filePaths[0];
});
app.whenReady().then(async () => { installMenu(); await createWindow(); }).catch(async (error) => { console.error(error.stack || error); await shutdown(); app.exit(1); });
app.on('window-all-closed', () => { shutdown().finally(() => app.quit()); });
app.on('before-quit', (event) => { if (!closing && session) { event.preventDefault(); shutdown().finally(() => app.quit()); } });
for (const signal of ['SIGINT', 'SIGTERM']) process.once(signal, () => shutdown().finally(() => app.quit()));
app.on('activate', () => { if (!mainWindow && !closing) createWindow().catch(console.error); });
