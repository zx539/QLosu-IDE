# 0001 固定版本 Code-Server 安装与发布

- **优先级：** P0
- **状态：** Linux 固定版本工作台、随包 Node 与 tar.gz 发布验收完成；其他平台待验收
- **范围：** `apps/code-server/`、`scripts/fetch-code-server.js`

## 背景

Code-Server 启动管理器已经建立，但当前开发环境没有 Code-Server 可执行文件。下载脚本只完成了入口，尚未完成跨平台解压、校验、目录布局和发布包携带。

## 待完成

- 固定并验证 Code-Server 4.95.3 的 Linux、Windows、macOS 构建来源。
- 下载后校验 SHA-256，并拒绝不匹配的文件。
- 自动解压到 `vendor/code-server/<version>/<platform>-<arch>/`。
- 让 `findBinary()` 识别上述目录。
- 在 `electron-builder` 中携带对应平台资源。
- 没有网络或资源缺失时给出可操作错误提示。

## 本次完成

- 新增 `npm run code-server:build`，从 Code-Server `v4.95.3` 源码初始化 VS Code 子模块、安装依赖并执行官方 TypeScript 构建。
- Fedora x86_64 已生成 `vendor/code-server/4.95.3/linux-amd64/out/node/entry.js`，启动管理器可自动发现该源码产物。
- 启动器会校验 `lib/vscode/out/server-main.js`；不完整的 TypeScript-only 构建不会被启动，避免浏览器收到 HTTP 500。
- 构建脚本支持 Corepack pnpm 和 npm；默认使用 Code-Server 官方兼容的 npm 流程。
- 构建完成后按当前主机 Node ABI 重建 `@vscode/spdlog`；缺少该原生绑定会使 Code-Server 进程启动后返回 404，而不是正常提供工作台。
- 下载器使用 `.part` 临时文件并在完整下载后原子替换，失败时清理残留文件；支持 `QLOSU_CODE_SERVER_VERIFY_RELEASE=1` 和 `QLOSU_CODE_SERVER_CHECKSUM_URL` 校验发布 SHA-256 清单。
- tar.gz 解压前检查归档条目，拒绝绝对路径和 `..` 路径；`npm run test:distribution` 覆盖 checksum 解析和归档路径防护。

验证：`npm run test:distribution`、`npm run smoke:local` 通过；后者继续覆盖 Code-Server 工作区启动、静态资源、终端、流程图 API 和子进程清理。

## 验收标准

- 全新环境执行启动命令可以发现固定版本 Code-Server，或明确提示安装路径。
- Code-Server 只监听 `127.0.0.1`，端口随机分配。
- 发布包不依赖用户全局安装 `code-server`。
- Linux、Windows、macOS 的资源路径和进程关闭行为有测试记录。

## 0014 后续进展

补充：Linux 发布包携带独立 Node（匹配 Code-Server 原生模块 ABI）、扩展完整生产依赖、两版运行时和离线文档；从临时 cwd 运行发布包通过桌面 smoke。详见 0014。
