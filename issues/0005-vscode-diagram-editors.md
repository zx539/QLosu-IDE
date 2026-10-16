# 0005 量子线路与程序流程图自定义编辑器

- **优先级：** P1
- **状态：** Webview 渲染和标签页生命周期已完成；原生自定义编辑器注册仍待实现
- **范围：** `extensions/qlosu-vscode/`、`server/app.js`、`www/js/core/drawio.js`

## 背景

扩展命令已经调用 `/api/trace` 和 `/api/flowchart`，但 Webview 的 draw.io 渲染仍需要完整的资源打包、CSP、生命周期和错误处理验证。

## 待完成

- 将线路图和流程图注册为真正的自定义编辑器/可复用 Webview 文档。
- 支持标签页重开、刷新、拆分编辑器和当前文件同步。
- 线路图使用 trace 数据，流程图使用源码和版本解析器。
- 支持缩放、导航、导出和源码行跳转。
- 处理无量子操作、解析失败、超时和超大图。

## 验收标准

- 命令面板可以从当前 `.losu` 文件打开两类图表标签页。
- 图表渲染失败时显示具体错误，不出现空白 Webview。
- 图表标签页可关闭、重开并保持与源码一致。

## 本次完成

- 将图表 Webview 模板独立到 `extensions/qlosu-vscode/diagram.js`，使用 CSP nonce 和扩展资源 URI 加载内置 draw.io viewer。
- 通过 draw.io 的 `onDrawioViewerLoad` 钩子执行一次初始化，避免 viewer 自动初始化后再次调用造成空白或重复图表。
- 线路没有量子操作、流程图数据为空、XML 无效、viewer 加载失败和超时均显示可读错误状态。
- 文件名和完整 `data-mxgraph` JSON 均经过 HTML 转义，避免引号、实体或文件名破坏 Webview DOM。
- 增加 `extensions/qlosu-vscode/test/diagrams.test.js`，锁定空线路、HTML 注入、CSP、viewer 错误和 XML 实体回归。
- 按图表类型和源文件 URI 复用打开的标签页；关闭后从注册表清理，重新打开会创建新面板。
- 监听 `.losu` 文档变更并以 350ms 防抖重新请求 `/api/trace` 或 `/api/flowchart`；面板提供刷新按钮和刷新失败状态。
- 增强轻量 Losu 结构解析器：兼容缩进与大括号风格，保留函数、递归调用、嵌套 `if/elif/else`、`for`/`while`、`break`/`continue` 及源码行号；比较表达式不再误识别为赋值。
- 流程图节点按赋值、调用、量子操作、输入输出和控制流使用不同视觉样式，复杂控制流保留循环回边和多级判断。
- 增加 `extensions/qlosu-vscode/test/flowchart-complex.test.js`，以快速排序和嵌套循环算法回归图结构、源码映射和节点数量。

验证：`node extensions/qlosu-vscode/test/diagrams.test.js`（6 项通过）、`node extensions/qlosu-vscode/test/flowchart-complex.test.js`（3 项通过）、`node --check extensions/qlosu-vscode/diagram.js`、`node --check extensions/qlosu-vscode/extension.js`。使用本地 Chrome 无头加载带 XML 实体的图表页面确认生成 SVG；Chrome 在离线加载 draw.io 附属资源时会记录资源错误，但页面保持已渲染 SVG，不再出现空白 Webview。
