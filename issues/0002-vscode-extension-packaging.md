# 0002 QLosu VS Code 扩展打包与安装

- **优先级：** P0
- **状态：** VSIX 与真实工作台交互验收已完成
- **范围：** `extensions/qlosu-vscode/`

## 背景

扩展源码和 `package.json` 已建立，但尚未安装 `vscode-languageclient`、生成锁文件、构建 VSIX 或让 Code-Server 自动加载扩展。

## 待完成

- 在实际 Code-Server 工作台验证扩展激活、活动栏、命令和 `.losu` 自动激活。
- 实测语法手册与图表 Webview 的显示效果，以及 LSP 客户端与 Code-Server 的兼容性。

## 验收标准

- 全新 Code-Server 工作区可以安装扩展并看到 QLosu 命令。
- 扩展激活失败时，Output 面板显示原因和修复建议。
- 不需要修改 Code-Server 核心源码。

## 本次完成

- 扩展使用 `img/LOGO.png` 作为市场/扩展标识和语法手册、工作台欢迎视图中的品牌图；活动栏使用配套单色 SVG 图标。
- 新增“洛书”活动栏工作台视图，提供运行、量子线路、程序流程图、语法手册和运行时版本命令入口。
- 修正 VSIX 收集生产依赖的排除规则，修正根目录扩展打包脚本的工作目录；VSIX 验证包含 Logo、活动栏资源及 `vscode-languageclient` 生产依赖。
- Code-Server 子进程携带项目根目录环境变量，使扩展安装在项目数据目录时仍能定位仓库内的 LSP。
- 为 `.losu` 编辑器增加标题栏和编辑器上下文菜单入口，运行、量子线路和程序流程图命令按语言 ID 显示；命令面板继续保留完整入口。
- `vscode:prepublish` 会把 `language-server/losu-lsp/server.js` 复制到 VSIX，独立安装时优先使用包内 LSP；开发时缺少包内文件则通过 `QLOSU_ROOT` 定位仓库版本。

- 增加 `package-lock.json`、`.vscodeignore`、VSIX 打包脚本和扩展元数据。
- 将 draw.io viewer 纳入 VSIX，并保留本地语言服务器启动方式。
- 增加语言服务器启动成功/失败日志，错误写入 QLosu Output 面板。
- 扩展命令和 `.losu` 语言激活配置保持 Code-Server/VSCodium 兼容。

验证：`npm run extension:package` 成功生成包含品牌图片、活动栏 SVG、draw.io 和语言客户端依赖的 VSIX。
`node extensions/qlosu-vscode/test/package.test.js` 锁定语言声明、包内 LSP 和 `.losu` 菜单贡献。

## 0014 后续进展

补充：0013/0014 已通过实际浏览器工作台的手册、图表、终端键盘交互、版本选择与文件焦点验收。VSIX 包含富 Markdown 生产依赖、Mermaid、KaTeX 资源和使用说明书。
