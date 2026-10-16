# QLosu 本地 Issue 清单

这些 Issue 记录 Code-Server 迁移和洛书多版本支持中尚未完成的工作。它们是本地开发清单，不会自动同步到 GitHub。

## 优先级与依赖

| 编号 | 主题 | 优先级 | 状态 | 依赖 |
| --- | --- | --- | --- | --- |
| [0001](0001-code-server-distribution.md) | Code-Server 固定版本安装与发布 | P0 | Linux tar.gz 与离线发布包验收已完成；其他平台待验收 | — |
| [0002](0002-vscode-extension-packaging.md) | QLosu VS Code 扩展打包与安装 | P0 | VSIX、手册/图表/终端及运行时真实工作台验收已完成 | 0001 |
| [0003](0003-losu-lsp-complete-language-experience.md) | 完整 Losu LSP 语言体验 | P0 | 已完成（丰富词典、悬浮文档、补全片段和语义高亮；正式 AST 可替换） | 0002、0004 |
| [0004](0004-losu-runtime-v2-and-multi-version.md) | 洛书 2.0 与多版本运行时 | P0 | 原生 lpk 1.0 与 2.0 WASM 列表选择已验证；2.0 C VM 待上游 | — |
| [0005](0005-vscode-diagram-editors.md) | 量子线路/流程图自定义编辑器 | P1 | 复杂控制流解析与 Webview 生命周期、源码同步已完成；原生 CustomEditorProvider 待增强 | 0002、0004 |
| [0006](0006-syntax-manual-webview.md) | 语法手册 Docsify 风格弹窗 | P1 | 已完成（离线 Webview） | 0002 |
| [0007](0007-continue-ai-integration.md) | Continue 完整 AI 体验接入 | P1 | 配置模板与安装入口已完成；联网端到端聊天待验收 | 0001、0002 |
| [0008](0008-directory-migration.md) | 目录迁移与旧 GUI 归档 | P1 | 服务与桌面入口已迁移；旧 GUI 归档进行中 | 0002、0005、0006 |
| [0009](0009-electron-browser-startup.md) | Electron/浏览器启动闭环 | P0 | 浏览器与 Electron 启动、终端、两版运行时及退出清理已验收 | 0001、0002 |
| [0010](0010-release-validation-security.md) | 测试、安全和发布验收 | P0 | Linux 发布与本地回归已完成；CI 配置已建立、远程执行待验证 | 0001–0009 |
| [0011](0011-codeserver-losu-gui-binding.md) | Code-Server GUI 与洛书工作台绑定 | P1 | 浏览器与 Electron 工作台加载、运行及终端已验收 | 0002、0008、0009 |
| [0012](0012-losu-agent.md) | 内置洛书 AI Agent | P1 | 流式代理、工具/差异确认与 Markdown 已验收；真实模型待配置 | 0002、0003、0004 |
| [0013](0013-workbench-usability.md) | 手册路径、原生 1.0、线路简化、终端与上次文件 | P0 | 五项修复及本机浏览器交互验收已完成 | 0002、0004、0005、0009、0012 |

推荐顺序：0001 → 0002 → 0004 → 0003 → 0009 → 0005/0006/0007 → 0008 → 0011 → 0010。

## 当前推荐顺序（0014 之后）

1. 在目标 Linux 基线系统重新构建与复验，执行新建 CI（本地通过不等于远程 CI 已运行）。
2. 配置真实 Ollama 或兼容服务，执行 Agent 合成探针和工作台对话验收。
3. 固定并验证 Continue 兼容版本；推进 Windows/macOS 发布、目录归档和图表编辑增强。

[0014：桌面发布、CI、说明书与 Markdown](0014-desktop-release-markdown.md) 汇总本轮实现和验证边界。
