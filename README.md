# QLosu 量子编程 IDE

QLosu IDE 是一款集成量子编程语言、可视化量子线路、程序流程图与 AI 助手的现代化量子开发环境。基于 Losu v2 语言、Qufu 声明式量子框架和 QuEST 4.2.0 模拟器，通过 WebAssembly 编译，无需任何原生依赖即可在本机离线运行。

## 特性

- **量子编程语言 Losu v2**：简洁的函数式语法，原生支持量子门操作与测量
- **声明式量子框架 Qufu**：管道式量子线路构建，支持柯里化门操作
- **实时量子线路可视化**：每次运行后自动生成量子线路图，显示门操作、控制比特、测量结果
- **程序流程图生成**：解析源码结构，生成包含分支、循环、函数调用的流程图
- **量子态演化追踪**：步进调试每个门操作后的量子态，实时显示概率分布与复振幅
- **Monaco Editor**：与 VS Code 同款编辑器，语法高亮、自动缩进、多文件标签页
- **示例库**：内置 Bell 态、量子隐形传态、Deutsch-Jozsa、Grover 搜索等经典量子算法
- **工作区管理**：本地文件浏览、保存、打开，支持多文件项目
- **三种主题**：深色、浅色、极光主题，匹配不同使用场景
- **跨平台**：基于 Electron 打包为独立桌面应用（Windows、macOS、Linux），也可作为 Web 服务运行

## 安装与运行

### 作为 Web 服务运行（推荐开发模式）

```bash
# 克隆仓库
git clone https://github.com/zx539/QLosu-IDE.git
cd QLosu-IDE

# 安装依赖
npm install

# 启动服务（默认 http://127.0.0.1:8080）
npm start

# 自定义端口
PORT=9000 npm start
```

### 作为 Electron 桌面应用运行

```bash
# 开发模式运行
npm run desktop

# 打包为可分发文件
npm run dist

# 打包产物位于 dist/ 目录：
#   Linux: QLosu-IDE-1.0.0.AppImage, qlosu-ide_1.0.0_amd64.deb, qlosu-ide-1.0.0.x86_64.rpm
#   Windows: QLosu-IDE Setup 1.0.0.exe, QLosu-IDE 1.0.0.exe (portable)
#   macOS: QLosu-IDE-1.0.0.dmg, QLosu-IDE-1.0.0-mac.zip
```

## 项目结构

```
QLosu-IDE/
├── electron/          # Electron 主进程
│   └── main.js
├── server/            # 后端服务（Express）
│   ├── app.js         # HTTP 路由
│   ├── runtime.js     # WASM 运行时管理（worker 线程池）
│   ├── workspace.js   # 工作区文件管理
│   ├── instrument.js  # 源码插桩（注入轨迹记录代码）
│   └── llm.js         # 大模型代理（Agent 后端，流式 NDJSON）
├── www/               # 前端
│   ├── index.html     # 入口页面
│   ├── css/app.css    # 主题与布局
│   ├── js/
│   │   ├── app.js     # 主逻辑（编辑器、标签页、运行、图表）
│   │   └── core/
│   │       ├── trace.js      # 运行输出解析（门序列、量子态）
│   │       ├── losu-parse.js # Losu 源码结构解析（AST lite）
│   │       └── drawio.js     # draw.io XML 生成（线路图 + 流程图）
│   ├── vendor/
│   │   ├── monaco/    # Monaco Editor（从 npm 链接）
│   │   └── drawio/    # draw.io viewer（MIT 许可）
│   └── icon.png       # 应用图标
├── wasm/              # WebAssembly 模块
│   ├── qlosu.js       # Emscripten 生成的胶水代码
│   └── qlosu.wasm     # 编译后的 Losu + Qufu + QuEST
├── examples/          # 示例脚本
│   ├── bell.losu
│   ├── teleportation.losu
│   ├── grover.losu
│   └── ...
├── src/               # 源码（上游子模块，仅供参考）
│   ├── QLosu/         # Losu 语言实现（C++ + 洛书语法）
│   ├── losu-quest/    # Qufu + QuEST 集成
│   └── wasm-build/    # Emscripten 构建脚本
├── index.js           # CLI 入口（npm start）
├── package.json
└── README.md
```

## 技术栈

- **前端**：原生 JavaScript（无框架）、Monaco Editor、draw.io viewer
- **后端**：Node.js + Express
- **量子运行时**：Losu v2（C++）+ Qufu（声明式 DSL）+ QuEST 4.2.0（C），通过 Emscripten 编译为 WASM
- **桌面打包**：Electron + electron-builder
- **图表渲染**：draw.io（mxGraph）静态 viewer

## 使用示例

### 1. Bell 纠缠态

```losu
var { println } import qufu.IO;
var QuReg import qufu.QuReg;
var { Hadamard: H, ControlledPauliX: CNOT } import qufu.Gates;
var { Measure: M } import qufu;

var reg = QuReg(2);
var w0 = reg[[0]];
var w1 = reg[[1]];

w0 :> H();
w1 :> CNOT(w0);

println("P(w0=1) =", w0(1), " P(w1=1) =", w1(1));
println("Bell 态测量: ", M(w0), " ", M(w1));
```

运行后：
- **输出面板**显示测量结果（00 或 11，各 50% 概率）
- **量子线路图**显示 H 门、CNOT 门及两次测量
- **程序流程图**解析代码结构（import、变量声明、量子操作、打印）

### 2. 量子隐形传态

打开 `examples/teleportation.losu`，点击"运行"：
- 量子态从 Alice 传送到 Bob，保真度 100%
- 线路图显示纠缠对制备、贝尔测量、条件门操作
- 步进器滑动条可逐步查看每个门操作后的量子态演化

## 开发指南

### 修改前端

编辑 `www/` 下的文件后，刷新浏览器或重启 Electron 即可看到更改。主题配色在 `www/css/app.css` 的 `:root` 中定义。

### 修改后端逻辑

编辑 `server/` 下的文件后，重启服务（`npm start`）。运行时线程池会在首次 `/api/run` 时懒加载。

### 重新编译 WASM

WASM 模块已预编译（`wasm/qlosu.wasm`）。如需修改上游（`src/QLosu`、`src/losu-quest`），在 `src/wasm-build/` 中运行：

```bash
cd src/wasm-build
./build.sh  # 需要 emcc 3.1.73+
```

生成的 `qlosu.js` 和 `qlosu.wasm` 复制到项目根目录的 `wasm/`。

### 添加新示例

在 `examples/` 中创建 `.losu` 文件，首行以 `//` 开头的注释将作为示例描述显示在侧边栏。

## 许可证

本项目采用 MIT 许可证（见 `LICENSE`）。

**内置组件声明**（见 `开源组件声明.md`）：
- Losu v2 语言：双许可（MIT / LGPL 3.0，与 GPL 兼容）
- Qufu 量子框架：MIT
- QuEST 4.2.0 模拟器：MIT
- Monaco Editor：MIT
- draw.io viewer：Apache 2.0
- Express：MIT

## 致谢

- **Losu 语言与洛书语法**由 QLosu 项目开发
- **QuEST**（Quantum Exact Simulation Toolkit）by Quest Team
- **Monaco Editor** by Microsoft
- **draw.io / diagrams.net** by jgraph

---

**问题反馈与贡献**：https://github.com/zx539/QLosu-IDE/issues

**文档与教程**：见 `docs/`（待补充）

**版本**：1.0.0 (2026-10-14)
