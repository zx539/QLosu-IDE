/**
 * Electron preload script
 * 暴露安全的 API 给渲染进程：文件对话框、拖拽打开
 */
const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  openFile: () => ipcRenderer.invoke('dialog:openFile'),
  openFolder: () => ipcRenderer.invoke('dialog:openFolder'),
  platform: process.platform,
});
