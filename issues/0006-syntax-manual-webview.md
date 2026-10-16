# 0006 洛书语法手册 Docsify 风格弹窗

- **优先级：** P1
- **状态：** 已完成（离线 Webview）
- **范围：** `extensions/qlosu-vscode/`、`src/QLosu/洛书语法/`

## 背景

用户确认语法手册应单独弹窗显示，并可使用 Docsify 风格。当前 Webview 有目录和切换逻辑，但 Markdown 渲染仍是简化替换，不支持完整 GFM、代码块、目录锚点、搜索和版本切换。

## 待完成

- 使用本地 Markdown 渲染器实现 GFM、代码块、表格和公式展示。
- 根据目录生成可折叠侧边导航和当前文档锚点。
- 支持搜索、上一篇/下一篇和文档目录刷新。
- 按 `qlosu.json` 版本选择对应语法目录。
- 严格限制 Webview 资源访问，避免 Markdown 注入脚本。

## 验收标准

- 文档来自 `src/QLosu/洛书语法`，离线可打开。
- Markdown 结构和代码块正确渲染。
- 语法手册以独立 VS Code 标签页/Webview 打开，不占用常驻右侧栏。

## 本次完成

- 语法手册从 `src/QLosu/洛书语法` 递归读取 Markdown，支持 `qlosu.json` 的版本目录映射。
- Webview 加入离线 Markdown 渲染：标题锚点、GFM 风格链接、粗体/斜体、代码块、行内代码、列表、表格和引用。
- 增加文档搜索、上一篇/下一篇、刷新和目录高亮；严格使用转义后的文本，避免 Markdown 注入脚本。
- 手册保持独立标签页，并允许多个 Code-Server 工作区分别打开。

验证：`node --check extension.js` 和 `git diff --check` 通过。
