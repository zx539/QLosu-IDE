# QLosu IDE 改进说明

## 📅 更新日期：2026-10-14

---

## ✅ 已完成的改进

### 1. 本地文件支持 🗂️

#### Electron 桌面版
- **文件对话框**：通过 Electron IPC 实现原生文件选择
- **拖拽打开**：支持将 `.losu` 文件拖入编辑器
- **快捷键**：`Ctrl+O` 快速打开文件

#### 实现细节
- `electron/preload.js`：安全的上下文隔离 API
- `electron/main.js`：`dialog:openFile` 和 `dialog:openFolder` 处理器
- `www/js/app.js`：`openLocalFile()` 兼容 Web 和 Electron 模式

### 2. 增强的编辑器体验 ✨

#### Monaco Editor 配置优化
```javascript
{
  minimap: { enabled: true },          // 显示小地图
  automaticLayout: true,               // 自动布局
  scrollBeyondLastLine: false,         // 不滚动到最后一行之后
  renderWhitespace: 'selection',       // 显示选中的空白字符
  cursorBlinking: 'smooth',            // 平滑光标闪烁
  cursorSmoothCaretAnimation: 'on',    // 平滑光标移动
  smoothScrolling: true,               // 平滑滚动
  tabSize: 2,                          // 2空格缩进
}
```

#### 键盘快捷键
- `Ctrl+Enter`：运行代码
- `Ctrl+S`：保存文件
- `Ctrl+O`：打开文件
- `Ctrl+Enter`（在 AI 输入框）：发送消息

### 3. AI 助手集成 🤖

#### 支持的 AI 提供商
1. **Ollama（推荐）**
   - 完全本地运行
   - 无需 API Key
   - 推荐模型：`qwen2.5-coder:7b`、`deepseek-coder-v2`
   - 默认地址：`http://localhost:11434`

2. **OpenAI**
   - 支持 GPT-4、GPT-3.5
   - 自定义 API 端点（兼容 OpenAI 格式的其他服务）

3. **Anthropic Claude**
   - Claude 3.5 Sonnet
   - 官方 API

#### 功能特性
- **流式响应**：实时显示 AI 回复
- **上下文感知**：可附带当前代码和错误信息
- **Markdown 渲染**：代码高亮和格式化
- **对话历史**：自动保存最近 20 轮对话
- **快速提示**：预设常见问题（创建贝尔态、Grover 算法等）

#### 使用方法
1. 点击右侧 AI 助手标签
2. 点击设置图标配置 AI 提供商
3. 输入问题，点击发送或按 `Ctrl+Enter`

### 4. 可视化改进 📊

#### 量子线路图
- **自动布局**：门操作按时间步排列，避免控制线穿越
- **彩色门符号**：H（蓝）、X（紫）、Z（绿）、Rx（橙）、M（红）
- **步进调试**：滑动条控制时间步，实时显示量子态

#### 程序流程图
- **泳道布局**：主流程 + 每个函数独立泳道
- **语法解析**：识别分支、循环、量子操作、IO 操作
- **自动生成**：运行后自动生成，无需手动绘制

#### 容器样式优化
```css
.diagram-host {
  display: flex;
  align-items: center;
  justify-content: center;
  min-height: 400px;
}
```

### 5. 打包配置 📦

#### electron-builder 完整配置
- **Linux**: AppImage、deb、rpm
- **Windows**: NSIS 安装器、portable exe
- **macOS**: DMG、zip

#### 构建命令
```bash
npm run dist          # 构建所有平台
npm run dist:linux    # 仅 Linux
npm run dist:win      # 仅 Windows
npm run dist:mac      # 仅 macOS
```

---

## 🔧 技术架构

### 前端模块
```
www/
├── js/
│   ├── app.js              # 主应用逻辑
│   ├── ai-assistant.js     # AI 助手（新增）
│   └── core/
│       ├── trace.js        # 轨迹解析
│       ├── losu-parse.js   # 语法解析
│       └── drawio.js       # 图表生成
├── css/
│   └── app.css             # 三套主题 + 设计系统
└── index.html              # 主界面
```

### Electron 桌面
```
electron/
├── main.js                 # 主进程（IPC + 服务器）
└── preload.js              # 预加载脚本（新增）
```

### 服务器
```
server/
├── app.js                  # Express 服务器
├── instrument.js           # 代码插桩
└── llm.js                  # LLM 代理接口
```

---

## 🚀 使用指南

### 开发模式
```bash
# 1. 安装依赖
npm install

# 2. 启动 Web 服务
npm start
# 访问 http://127.0.0.1:8080

# 3. 启动 Electron 桌面版
npm run desktop
```

### 使用 AI 助手（推荐 Ollama）

#### 安装 Ollama
```bash
# Linux
curl -fsSL https://ollama.com/install.sh | sh

# 下载模型
ollama pull qwen2.5-coder:7b
```

#### 配置 IDE
1. 打开 QLosu IDE
2. 切换到"AI 助手"标签
3. 点击右上角设置图标
4. 选择"Ollama (本地)"
5. 确认地址为 `http://localhost:11434`
6. 模型填写 `qwen2.5-coder:7b`
7. 保存配置

#### 示例对话
```
用户: 如何创建一个 2 比特的贝尔态？

AI: 创建贝尔态需要以下步骤：

1. 初始化 2 比特寄存器
2. 对第 0 个比特应用 Hadamard 门
3. 对第 1 个比特应用控制 NOT 门，控制比特为第 0 个

代码示例：
\`\`\`losu
var { println } import qufu.IO;

var reg = QuReg(2);
reg[[0]] :> Hadamard();
reg[[1]] :> ControlledPauliX(reg[[0]]);

var m0 = Measure(reg[[0]]);
var m1 = Measure(reg[[1]]);
println("测量结果: " + m0 + " " + m1);
\`\`\`

这将创建纠缠态 |00⟩ + |11⟩，测量时会得到 00 或 11。
```

---

## 📝 常见问题

### Q: 量子线路图显示空白？
A: 确保已运行代码，并且代码中使用了量子操作。检查浏览器控制台是否有错误。

### Q: AI 助手无法连接？
A: 
- Ollama：确保 Ollama 服务正在运行（`ollama serve`）
- OpenAI/Anthropic：检查 API Key 是否正确，网络是否可访问

### Q: 如何打包成独立应用？
A: 运行 `npm run dist`，产物在 `dist/` 目录。首次打包会下载 Electron 二进制文件，需要较长时间。

### Q: Web 模式和桌面版有什么区别？
A:
- **Web 模式**：浏览器访问，文件选择通过 `<input type="file">`
- **桌面版**：原生文件对话框、更好的性能、离线使用

---

## 🎯 下一步计划

### 短期
- [ ] 文件夹浏览树（多文件项目）
- [ ] 多寄存器可视化切换
- [ ] AI 代码补全（类似 Copilot）
- [ ] 暗色/亮色主题自动切换

### 中期
- [ ] Git 集成（提交、推送、分支）
- [ ] 调试器（断点、单步执行）
- [ ] 量子态 Bloch 球可视化
- [ ] 插件系统

### 长期
- [ ] 云端协作编辑
- [ ] 量子算法库市场
- [ ] 性能分析工具
- [ ] WebGPU 加速模拟

---

## 📄 开源组件

- **Monaco Editor**: Microsoft 的代码编辑器（VS Code 核心）
- **draw.io**: 图表渲染引擎
- **QuEST**: 量子仿真器（C 库，编译为 WASM）
- **marked**: Markdown 解析器
- **Electron**: 跨平台桌面框架

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

仓库地址：https://github.com/zx539/QLosu-IDE

---

**版权所有 © 2026 QLosu 项目**
