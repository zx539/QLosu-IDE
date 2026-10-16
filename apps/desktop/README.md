# QLosu Desktop

`apps/desktop/main.js` 是 Electron 的稳定入口，底层实现位于 `apps/desktop/core/`；根目录 `electron/` 仅保留兼容测试和转发入口。这样 `npm run desktop`、electron-builder 和后续迁移使用同一个入口，同时保留找不到 Code-Server 时回退到旧版 Web IDE 的行为。
