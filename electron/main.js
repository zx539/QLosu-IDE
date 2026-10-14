/**
 * Electron 主进程
 *
 * 启动内嵌 HTTP 服务（server/app.js），监听 127.0.0.1 随机端口，
 * 创建 BrowserWindow 加载 http://127.0.0.1:PORT/。
 * 窗口关闭时同步关闭服务。
 */
const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const { startServer } = require('../server/app');

let mainWindow;
let server;

async function createWindow() {
  const { url, close } = await startServer({ port: 0, host: '127.0.0.1' });
  server = { close };
  console.log('QLosu 服务已启动：', url);

  mainWindow = new BrowserWindow({
    width: 1400,
    height: 900,
    minWidth: 1000,
    minHeight: 600,
    title: 'QLosu 量子编程 IDE',
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      webSecurity: true,
      preload: path.join(__dirname, 'preload.js'),
    },
    icon: path.join(__dirname, '../www/icon.png'),
  });

  mainWindow.loadURL(url);
  mainWindow.on('closed', () => { mainWindow = null; });

  // 文件对话框
  ipcMain.handle('dialog:openFile', async () => {
    const result = await dialog.showOpenDialog(mainWindow, {
      properties: ['openFile'],
      filters: [
        { name: 'Losu 文件', extensions: ['losu'] },
        { name: '所有文件', extensions: ['*'] },
      ],
    });
    if (result.canceled || !result.filePaths.length) return null;
    const filePath = result.filePaths[0];
    const content = fs.readFileSync(filePath, 'utf8');
    return { path: filePath, name: path.basename(filePath), content };
  });

  ipcMain.handle('dialog:openFolder', async () => {
    const result = await dialog.showOpenDialog(mainWindow, {
      properties: ['openDirectory'],
    });
    if (result.canceled || !result.filePaths.length) return null;
    return result.filePaths[0];
  });
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  if (server) server.close();
  app.quit();
});

app.on('activate', () => {
  if (mainWindow === null) createWindow();
});
