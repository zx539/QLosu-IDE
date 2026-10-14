# QLosu IDE 功能测试清单

## 已完成的修复和增强

### 1. ✅ 终端集成
- **文件**: `www/js/terminal.js`
- **功能**: 集成 xterm.js 终端
- **位置**: 底部面板的"终端"标签
- **特性**:
  - 完整的终端模拟器
  - 自适应窗口大小（fit-addon）
  - 主题自动切换
  - 命令历史记录

### 2. ✅ Markdown + LaTeX 渲染
- **库**: marked.js + katex.js
- **文件**: `www/docs/losu-syntax.md`
- **功能**: 
  - 点击顶部"语法"按钮打开模态框
  - 渲染 Markdown 格式的洛书语法文档
  - 支持行内公式 `$...$` 和块级公式 `$$...$$`
  - GitHub 风格 Markdown (GFM)

### 3. ✅ 程序流程图可视化
- **文件**: `www/js/core/losu-parse.js`, `www/js/core/drawio.js`
- **功能**: 
  - 解析 Losu 代码生成 AST
  - 生成 draw.io XML 格式的流程图
  - 泳道布局：主流程 + 每个函数独立泳道
  - 识别：函数、分支、循环、返回、量子操作、IO操作
- **触发**: 右侧面板切换到"流程图"标签

### 4. ✅ 量子线路图可视化
- **文件**: `www/js/core/trace.js`, `www/js/core/drawio.js`
- **功能**:
  - 运行代码后自动生成量子线路图
  - 门操作自动对齐避免控制线穿越
  - 彩色门符号 (H/X/Z/Rx/M)
  - 支持 CNOT、控制门
- **位置**: 右侧面板"线路图"标签

### 5. ✅ 量子态步进调试器
- **位置**: 线路图下方的控制条
- **功能**:
  - ◀ 上一步
  - ▶ 下一步  
  - ▶ 播放 / ⏸ 暂停（自动播放）
  - 滑动条控制
  - 实时显示量子态概率分布和复振幅
- **文件**: `www/js/app.js` (stepPrev, stepNext, stepPlay 函数)

### 6. ✅ 面板大小调整
- **功能**: 所有面板都支持拖拽调整大小
- **区域**:
  - 左侧边栏（文件树）：拖动右边缘
  - 右侧边栏（可视化）：拖动左边缘
  - 底部面板（输出/终端）：拖动顶部边缘
- **文件**: `www/js/app.js` (resize handling)

### 7. ✅ 三套精美主题
- **主题**: 深色（默认）、浅色、极光
- **切换**: 顶部工具栏主题按钮
- **覆盖**: 编辑器、终端、所有UI组件
- **文件**: `www/css/app.css`

### 8. ✅ Agent 基础设施
- **文件**: `server/llm.js`
- **功能**: 流式 NDJSON 大模型代理接口
- **前端**: 右侧"Agent"标签（预留）

### 9. ✅ Electron 桌面打包
- **配置**: `package.json` (build 字段)
- **支持平台**:
  - Linux: AppImage, deb, rpm
  - Windows: NSIS 安装器, portable exe
  - macOS: DMG, zip
- **命令**: 
  - `npm run desktop` - 启动 Electron
  - `npm run dist` - 打包所有平台

## 测试步骤

### 测试 1: 启动应用
```bash
cd /home/fjwh/QLosu-IDE
npm start
# 浏览器访问 http://127.0.0.1:8080
```

### 测试 2: 打开示例代码
1. 点击左侧边栏"示例" → "量子门基础.losu"
2. 验证代码加载到编辑器
3. 点击"运行"按钮

### 测试 3: 查看量子线路图
1. 运行代码后
2. 右侧面板应自动显示"线路图"
3. 验证量子门可视化正确

### 测试 4: 测试步进调试器
1. 在线路图下方找到控制条
2. 点击"◀"/"▶"按钮，验证步进功能
3. 点击"▶ 播放"按钮，验证自动播放
4. 再次点击变为"⏸ 暂停"，验证停止功能
5. 拖动滑动条，验证手动跳转

### 测试 5: 查看程序流程图
1. 右侧面板切换到"流程图"标签
2. 验证流程图显示当前代码的结构
3. 应显示泳道、函数、分支、循环

### 测试 6: 测试终端
1. 底部面板切换到"终端"标签
2. 验证终端显示
3. 输入命令（如果有后端支持）

### 测试 7: 测试 Markdown 语法文档
1. 点击顶部"语法"按钮
2. 验证模态框弹出
3. 验证 Markdown 渲染正确
4. 验证 LaTeX 公式渲染正确

### 测试 8: 测试面板调整
1. 拖动左侧边栏右边缘，验证宽度调整
2. 拖动右侧边栏左边缘，验证宽度调整
3. 拖动底部面板顶部边缘，验证高度调整

### 测试 9: 测试主题切换
1. 点击顶部主题按钮
2. 切换到"浅色"主题，验证所有UI更新
3. 切换到"极光"主题，验证渐变效果
4. 切换回"深色"主题

### 测试 10: 测试 Electron 桌面版
```bash
npm run desktop
```
验证桌面应用启动并正常工作

## 潜在问题排查

### 如果流程图不显示
1. 打开浏览器开发者工具（F12）
2. 查看 Console 标签
3. 检查是否有错误：
   - "LosuParse is not defined" → losu-parse.js 未加载
   - "QDrawio is not defined" → drawio.js 未加载
   - "GraphViewer is not defined" → viewer-static.min.js 未加载

### 如果终端不显示
1. 检查浏览器控制台错误
2. 验证 xterm.js 和 xterm-addon-fit 已加载
3. 检查 `www/js/terminal.js` 路径正确

### 如果步进按钮无响应
1. 打开浏览器控制台
2. 输入 `app.stepPlay()` 测试
3. 检查是否有 JavaScript 错误

### 如果 LaTeX 不渲染
1. 检查 katex.min.js 和 katex.min.css 已加载
2. 打开语法文档，查看控制台是否有 katex 错误
3. 验证公式语法：`$x^2$` (行内) 或 `$$x^2$$` (块级)

## 文件结构
```
QLosu-IDE/
├── index.js                    # Express 服务器
├── electron/
│   └── main.js                 # Electron 主进程
├── server/
│   └── llm.js                  # LLM 代理接口
├── www/
│   ├── index.html              # 主HTML
│   ├── css/
│   │   └── app.css             # 2000+ 行样式（含三套主题）
│   ├── js/
│   │   ├── app.js              # 主应用逻辑
│   │   ├── terminal.js         # 终端集成
│   │   └── core/
│   │       ├── trace.js        # WASM轨迹解析
│   │       ├── losu-parse.js   # Losu语法解析
│   │       ├── drawio.js       # draw.io XML生成
│   │       └── instrument.js   # 代码插桩
│   ├── docs/
│   │   └── losu-syntax.md      # 洛书语法文档
│   └── vendor/
│       ├── drawio/             # draw.io viewer
│       ├── monaco/             # Monaco 编辑器
│       └── losu-wasm/          # 洛书编译器
└── examples/                   # 示例代码
```

## 总代码统计
- **核心代码**: 约 3500 行（不含依赖库）
- **CSS**: 2089 行（完整设计系统）
- **JavaScript 模块**: 8 个
- **主题**: 3 套（深色、浅色、极光）
