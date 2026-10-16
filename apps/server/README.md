# QLosu API 服务

`apps/server/index.js` 是本地服务入口，`apps/server/app.js` 提供稳定的兼容导出。底层实现位于 `apps/server/core/`，根目录 `server/` 仅保留兼容转发入口。Code-Server 和 VS Code 扩展通过同一套 API 访问运行时、轨迹、流程图和版本信息。

旧的 `index.js` 仍保留为兼容入口；两者都使用 `PORT`、`HOST` 和 `QLOSU_WORKSPACE` 环境变量。
