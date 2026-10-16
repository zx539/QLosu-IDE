# 0008 目录迁移与旧 GUI 归档

- **优先级：** P1
- **状态：** 服务与桌面入口已迁移；旧 GUI 归档仍进行中
- **范围：** 根目录、`apps/`、`legacy/`

## 背景

新目录边界已经建立，但真正的服务、Electron 和旧 Web IDE 仍在根目录 `server/`、`electron/`、`www/`。当前 `apps/server` 和 `apps/desktop` 只有迁移说明。

## 待完成

- 将服务入口迁移到 `apps/server`，保留兼容导出。
- 将 Electron 启动器迁移到 `apps/desktop`。
- 将旧 `www/` 页面归档到 `legacy/web-ide`。
- 更新 electron-builder 文件清单、README、测试和脚本路径。
- 清理重复的终端、语法文档和旧资源实现。
- 记录每次迁移的回退入口。

## 验收标准

- 新架构不依赖根目录旧 GUI 才能运行。
- 旧 GUI 在迁移验收前仍能通过明确命令启动。
- 打包、开发、测试和 Code-Server 扩展路径全部一致。

## 品牌资源归属

- `img/LOGO.png` 为用户提供的原始洛书标识，保留原图；`img/LOGO-app.png` 为带白色补边的 512×512 Linux/Electron 图标。
- 扩展使用 `assets/losu-logo.png` 的副本以支持独立 VSIX 安装，活动栏使用单色 `assets/losu-icon.svg`；更新原图时应同步扩展副本和应用图标。
- Electron 文件清单携带品牌资源及扩展生产依赖，排除扩展打包工具和 VSIX 副本。

## 本次完成

- 新增 `apps/server/index.js` 和 `apps/server/app.js`，将本地服务启动入口与 API 兼容导出固定在 `apps/server`。
- 新增 `apps/desktop/main.js`，package 主入口和 Electron 启动器统一经过 `apps/desktop`；根目录 `server/`、`electron/` 仍作为兼容实现保留。
- `npm start` 已切换到 `apps/server/index.js`，旧 `index.js` 继续可直接运行，便于迁移期间回退。

验证：`timeout 4s npm start` 已在本机真实监听 `127.0.0.1:8080` 并正常输出启动地址；旧版 `www/` 仍由根服务明确作为回退入口，尚未复制迁移到 `legacy/web-ide`。
