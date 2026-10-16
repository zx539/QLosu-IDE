# 0014 桌面发布、CI、使用说明书与 Markdown

- **状态：** 本机 Linux 桌面与发布包、Markdown 与 Agent 离线集成验收已完成；真实模型和远程 CI 运行待配置/执行
- **关联：** 0001、0002、0004、0009、0010、0011、0012、0013

## 完成内容

- Electron 44.7.0 分段下载、缓存及官方 SHA-256 校验安装，保持固定版本。
- Electron 的 API、Code-Server 使用同一工作区；增加菜单目录选择、上次工作区记录、可写数据目录、导航边界和退出清理。
- 发布包携带匹配原生模块 ABI 的独立 Node；版本化 CLI 与 Code-Server 通过该 Node 启动。
- 修正 electron-builder 默认排除嵌套 node_modules 导致的发布资源缺失，打包后强制检查 PTY 和扩展依赖。
- 生成 Linux tar.gz、SHA256SUMS 和记录平台/glibc/Node ABI/关键文件哈希的 release-manifest；补充发布与回退检查文档。
- 建立 GitHub Actions 核心回归与 Linux 浏览器/桌面发布任务，尚未触发远程 CI。
- 编写根目录 `使用说明书.md`，覆盖安装、文件、两版运行时、图表、终端、Agent、配置、构建、数据位置与故障排查；工作台提供离线说明书入口。
- 手册、Agent 回复和兼容 Web IDE 共用 Markdown 渲染器：安全 HTML/折叠、表格、任务列表、提示框、代码高亮、KaTeX 公式和离线 Mermaid；代码块内不执行数学或 HTML。
- Express 4 系列与 sanitize-html 更新至修复版本，生产依赖审计无已知漏洞。
- Agent 修改工具先展示真实 diff，再确认保存；取消保持无副作用，预览后检查文件并发变动，拒绝符号链接越界。增加 HTTP 流式代理与工具往返集成测试。

## 验证结果

- `npm run test:all`：核心及新增 Markdown、桌面会话、Agent 工具和代理集成测试通过。
- `npm run smoke:local`、`npm run smoke:workbench`：浏览器工作台运行、版本选择、终端及手册路径通过；真实说明书 Webview 中的 Mermaid SVG、KaTeX 和折叠确认可见。
- `npm run smoke:markdown`：说明书在真实浏览器中渲染表格、折叠、公式、高亮、提示框和 Mermaid。
- `node scripts/with-display.js scripts/smoke-desktop.js`：源码 Electron 工作台、终端键盘执行、两版运行时、关闭窗口后 API 与 Code-Server 清理通过。
- `QLOSU_DESKTOP_APP=... node scripts/with-display.js scripts/smoke-desktop.js`：从临时 cwd 启动 Linux 发布目录并执行同样的验收通过。
- `npm run extension:package`、`npm run dist:linux`：本地 VSIX 与 Linux tar.gz 构建完成。
- 最终 tar.gz 的 SHA256SUMS 校验、解压后的资源清单逐项哈希检查及从解压目录运行桌面 smoke 均通过；发布包未包含 `.qlosu` 用户数据、`.git`、`.aws`、`.ssh` 或本地 `.env`。

## 环境与剩余项

本机 Fedora/glibc 2.43、Node 24.21.0、Electron 44.7.0；此包不可据此声称兼容旧 glibc。跨发行版基线和 Windows/macOS 仍须各自构建测试。系统原生目录选择对话框尚需目标桌面人工确认。

本机 `127.0.0.1:11434` 无 Ollama 服务，未调用未配置的外部模型。用户明确选择先完成离线工具验收、真实模型验收保留待配置。提供 `agent:check` 合成探针；完整真实模型工具调用及 Continue 的兼容版本仍需目标服务验收。
