# 0010 测试、安全和发布验收

- **优先级：** P0
- **状态：** Linux 发布包与本地回归已完成；CI 配置已建立，远程运行及其他平台待验收
- **范围：** 全项目

## 背景

目前只有旧 WASM 运行时测试和静态语法检查。Code-Server、扩展、LSP、多版本运行时和 Electron 尚未形成 CI 验收矩阵。

## 待完成

- 添加 API、运行时、LSP 协议和扩展命令测试。
- 添加 Code-Server 启动/停止、端口和工作区测试。
- 测试路径穿越、命令注入、Webview CSP、Markdown 注入和 API Key 泄露。
- 测试 Windows/macOS/Linux 构建和 Electron 二进制可用性。
- 检查第三方许可证：Code-Server、Continue、draw.io、Monaco、Losu 许可证。
- 生成发布检查清单、故障排查文档和版本升级策略。

## 本次完成

- 增加 `npm run smoke:local`，覆盖本地静态资源 MIME、终端命令、流程图 API、Code-Server 工作区启动及子进程清理。
- VSIX 打包、WASM 内置示例、关键 JS 静态检查和 `git diff --check` 已验证通过。
- 修复冒烟测试未消费 HTTP 响应导致清理挂起的问题，验证 `npm run smoke:local` 输出 PASS 后自然退出。
- 图表渲染回归：`node extensions/qlosu-vscode/test/diagrams.test.js` 的 5 项测试通过，覆盖空线路、文件名转义、CSP、viewer 失败提示和 XML 实体序列化。
- Chrome 无头浏览器加载实际内置 draw.io viewer，断言生成 SVG、加载提示隐藏且中文与 XML 实体标签保留。此项是独立渲染页面验证；真实 VS Code Webview、标签页生命周期与源码同步仍待验收。
- 重新执行 `npm run extension:package` 成功；检查 VSIX 包含 `diagram.js`、原始洛书 Logo、draw.io viewer、LSP 及语言客户端依赖，排除测试目录。Logo 与 `img/LOGO.png` 按字节一致。桌面打包清单也补入 `diagram.js`，Electron 发布包仍待构建验证。
- 工作区路径检查增加真实路径边界校验，拒绝通过符号链接读写工作区外文件；`npm run test:security` 覆盖链接逃逸和正常嵌套文件写入。
- 增加 `npm run test:all`，统一执行 Code-Server 分发校验、工作区安全、图表、工作台配置和 Electron 启动器测试。

## 验收标准

- CI 在无图形环境中通过核心测试。
- 发布包能离线启动固定 Code-Server、扩展和洛书运行时。
- 默认只监听本机，不允许未确认的 AI 命令执行。
- 所有已知阻塞项都有明确错误信息和恢复路径。

## 0014 后续进展

补充：新增 Linux tar.gz/SHA256SUMS、发布资源清单、CI 两个任务、发布检查清单、桌面与 Markdown 浏览器验收。Agent 确认取消不执行、差异预览、符号链接边界以及流式工具往返已回归。CI 尚无远程执行记录。
