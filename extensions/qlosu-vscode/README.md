# QLosu VS Code 扩展

此扩展提供 Losu 语言服务、运行命令、量子线路/程序流程图 Webview、洛书语法手册和内置洛书 AI Agent。Agent 可以读取当前工作区和诊断，生成图表，并在确认后修改文件或执行终端命令。Continue 仍作为可选的官方 AI 扩展使用；在 Code-Server 中安装官方 Continue 扩展，再按仓库的 Continue 配置模板启用 QLosu 上下文即可获得另一套 AI 工作流。

语法高亮和 LSP 符号词典按 `https://gitee.com/zx2198762717/lpk/tree/vscode/` 的控制流、标准库、量子 API 和 UI/WASM API 组织，并保留洛书中文关键字；本地服务在上游 AST 不稳定时使用恢复式解析器。

Continue 的模型配置和 QLosu 规则不打包进扩展，也不保存 API key。请参考仓库根目录的
[`docs/continue-ai.md`](../../docs/continue-ai.md)。

开发安装依赖：

```bash
cd extensions/qlosu-vscode
npm install
```

扩展依赖 `vscode-languageclient`。打包前自动复制仓库的 Losu LSP，安装后通过 stdio 启动包内语言服务器；可通过 `qlosu.languageServer` 指定其他实现。

点击活动栏的“洛书”图标可打开工作台，查看洛书标识以及 Agent、运行、量子线路、流程图、语法手册和版本选择入口。
