# QLosu 量子编程 IDE

完整安装、工作台操作、两版运行时、图表、终端和 AI 配置见 [使用说明书](使用说明书.md)。工作台也提供离线“使用说明书”按钮。

QLosu IDE 是一款集成量子编程语言、可视化量子线路、程序流程图与 AI 助手的现代化量子开发环境。项目使用 `img/LOGO.png` 作为洛书品牌标识，显示在旧版标题栏、Electron 窗口和 Code-Server 扩展工作台中。基于 Losu v2 语言、Qufu 声明式量子框架和 QuEST 4.2.0 模拟器，通过 WebAssembly 编译，无需任何原生依赖即可在本机离线运行。

## 特性

### 核心功能
- **量子编程语言 Losu v2**：简洁的函数式语法，原生支持量子门操作与测量
- **声明式量子框架 Qufu**：管道式量子线路构建，支持柯里化门操作
- **Monaco Editor**：与 VS Code 同款编辑器，语法高亮、自动缩进、多文件标签页
- **工作区管理**：本地文件浏览、保存、打开，支持多文件项目
- **示例库**：内置 Bell 态、量子隐形传态、Deutsch-Jozsa、Grover 搜索等经典量子算法

### 可视化与调试
- **实时量子线路图**：每次运行后自动生成量子线路图，彩色门符号（H/X/Z/Rx/M）、控制比特、CNOT 操作
- **程序流程图**：解析源码结构，生成包含分支、循环、函数调用的泳道式流程图
  - `docs/flowchart-complex.losu` 提供快速排序、嵌套判断、循环回边和递归调用的检测样例
- **量子态步进调试器**：
  - 逐步查看每个门操作后的量子态演化
  - 实时显示概率分布与复振幅
  - 支持手动步进（◀ ▶）和自动播放（▶ 播放 / ⏸ 暂停）
  - 滑动条快速跳转到任意步骤
- **可调整面板**：所有面板支持拖拽调整大小，自适应布局

### 开发工具
- **集成终端**：基于 xterm.js 的全功能终端模拟器，支持 ANSI 颜色和中文显示
- **Markdown 文档查看器**：
  - 内置洛书语法文档，支持 GitHub 风格 Markdown (GFM)
  - LaTeX 数学公式渲染（行内 `$...$` 和块级 `$$...$$`）
  - 一键打开语法参考模态框
- **洛书 AI Agent**：Code-Server 活动栏提供独立 Agent 标签页，可读取当前文件、文件列表和诊断，生成量子线路/程序流程图，并在用户确认后修改文件或执行终端命令

### 主题与体验
- **三套精美主题**：
  - 深色主题（默认）：五级表面色阶，量子紫渐变强调
  - 浅色主题：高对比度，适合明亮环境
  - 极光主题：赛博朋克风格，霓虹渐变边框
- **跨平台**：基于 Electron 打包为独立桌面应用（Windows、macOS、Linux），也可作为 Web 服务运行

## 安装与运行

### Code-Server 版（新主界面）

QLosu 的新架构以 Code-Server 作为默认工作台，QLosu 能力通过 `extensions/qlosu-vscode` 接入。开发机可以安装固定版本 Code-Server 4.95.3，或通过 `QLOSU_CODE_SERVER_BIN` 指定现有可执行文件：

```bash
QLOSU_CODE_SERVER_BIN=/opt/code-server/bin/code-server npm run code-server
```

启动器会打印两个地址：`QLosu API` 用于扩展调用，`Code-Server` 是浏览器工作台。需要直接打开指定目录时，把目录作为参数传给启动器：

```bash
QLOSU_WORKSPACE="$PWD/examples" npm run code-server
# 或
node apps/code-server/launch.js /绝对路径/项目目录
```

浏览器地址支持 VS Code 的工作区参数，例如 `http://127.0.0.1:端口/?folder=/绝对路径/项目目录`。首次启动会自动写入 `.qlosu/user-data/User/settings.json`，启用洛书工作台模式并关闭遥测；已有用户设置不会被覆盖。

首次准备 Code-Server 和扩展（需要联网）：

```bash
npm run code-server:download
npm run extension:install
npm run extension:package
```

下载器默认保留已有归档；设置 `QLOSU_CODE_SERVER_VERIFY_RELEASE=1` 会同时读取发布版
`checksums.txt` 并校验 SHA-256，也可以用 `QLOSU_CODE_SERVER_SHA256` 或
`QLOSU_CODE_SERVER_CHECKSUM_URL` 指定校验值/清单。`npm run test:distribution` 可在离线环境回归校验解析和归档路径安全性。

Fedora x86_64 可直接从 Code-Server 源码构建（构建脚本固定使用 v4.95.3，源码和依赖缓存于 `.cache/`，产物安装到 `vendor/`）：

```bash
npm run code-server:build
npm run code-server
```

运行 `npm run smoke:local` 可回归本地 Logo、终端资源、终端命令、流程图接口、Code-Server 工作区打开和退出清理。

### Continue AI 助手

Code-Server 可选安装官方 Continue 扩展，并支持通过 `QLOSU_CONTINUE_VERSION` 锁定已验收版本。Ollama、OpenAI 兼容接口和 Anthropic 的配置模板，以及 QLosu 专用上下文规则，见 [`docs/continue-ai.md`](docs/continue-ai.md)：

```bash
npm run continue:setup -- --backend ollama --workspace "$PWD"
QLOSU_INSTALL_CONTINUE=1 npm run code-server
```

模板不包含 API key；密钥放在 Continue secrets 中。AI 修改仍需在编辑器中审核，终端命令需要明确确认。

### 内置洛书 Agent

扩展自带 Agent，不依赖 Continue。打开 Code-Server 后，在洛书活动栏选择“AI Agent”，默认连接本机 Ollama 的 OpenAI 兼容接口：

```bash
ollama serve
ollama pull qwen2.5-coder:7b
npm run code-server
```

也可以在 Agent 面板中填写 OpenAI、DeepSeek、vLLM 等兼容地址，或选择 Anthropic
Messages 接口。模型密钥使用 VS Code SecretStorage 保存，不写入工作区。Agent 的
`write_file` 和 `run_terminal` 工具始终弹出确认框；读取、诊断和图表工具会把结果回传给
模型，最多连续处理 8 轮工具调用。

### 洛书 CLI 与编译

仓库 CLI 为洛书 2.0 提供可重复的检查、构建和运行入口：

```bash
npm run losu -- --version
npm run losu -- check examples/hello.losu
npm run losu:build
npm run losu -- run examples/hello.losu
npm run losu -- trace examples/hello.losu
```

`losu:build` 在 `src/losu v2.0.0/src/test` 使用 C99 编译器生成
`liblosu-v2.a`。上游 C 树当前没有完整的 `losu_vm_init`、脚本加载和 IO 执行入口，
因此 CLI 的 `run/trace` 明确使用仓库内已验证的 QLosu WASM 后端，不把静态归档冒充成
完整原生解释器。需要使用其他洛书版本时，在 `qlosu.json` 的 `runtimes` 中配置外部
命令和 `runArgs`/`traceArgs`，版本不会静默降级。

普通洛书源码的上游编译命令、C99 环境要求和原生 VM 限制见 [`docs/losu-build.md`](docs/losu-build.md)。

源码构建需要 Node.js 20、npm、Git、C/C++ 编译器和 Python；如果系统依赖安装失败，请先安装 `gcc-c++ make python3`。完整 VS Code Remote/Kerberos 依赖还需要 Fedora 的 `krb5-devel`，没有 sudo 权限时脚本会跳过该可选子模块依赖并完成 Code-Server 主体构建。

Code-Server 版使用真实 VS Code 文件树、标签页、命令面板和终端；洛书语言服务位于 `language-server/losu-lsp`，版本运行时位于 `runtime/`。当前旧版 `www/` 页面仍作为回退入口保留。

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

# 检查 Electron 二进制（无图形环境也可执行）
npm run desktop:check

# 指定工作区
npm run desktop -- --workspace=/绝对路径/项目目录

# 打包为可分发文件
npm run dist

# Linux tar.gz 发布包：
npm run dist:linux
# Fedora/Debian 安装包（使用 dpkg-deb 和 rpmbuild）：
npm run dist:linux:packages
# Windows nsis/portable（需要 Windows 或配置完整的 Wine/NSIS 环境）：
npm run dist:windows
```

`npm run desktop` 会先检查 `node_modules/electron` 中的本机二进制，再启动桌面窗口。
如果依赖下载受限，可将 `QLOSU_ELECTRON_BIN` 指向已安装的 Electron 可执行文件；
`npm run desktop:check` 会输出缺失路径和修复提示。桌面启动器会把 `QLOSU_WORKSPACE`
或 `--workspace` 传给 Code-Server，窗口关闭时同时清理 Code-Server 与洛书 API 子进程。

## 项目结构

```
QLosu-IDE/
├── apps/               # Code-Server 启动管理与应用入口
│   ├── server/         # 本地服务入口和兼容导出
│   └── desktop/        # Electron 稳定入口
├── extensions/         # qlosu-vscode 扩展
├── language-server/    # Losu LSP
├── runtime/            # 洛书版本适配器
├── legacy/             # 迁移期旧版入口说明
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
│   ├── css/app.css    # 主题与布局（2000+ 行，完整设计系统）
│   ├── js/
│   │   ├── app.js     # 主逻辑（编辑器、标签页、运行、图表、步进调试）
│   │   ├── terminal.js # 终端集成（xterm.js + fit-addon）
│   │   └── core/
│   │       ├── trace.js      # 运行输出解析（门序列、量子态）
│   │       ├── losu-parse.js # Losu 源码结构解析（AST lite）
│   │       ├── drawio.js     # draw.io XML 生成（线路图 + 流程图）
│   │       └── instrument.js # 代码插桩（轨迹标记注入）
│   ├── docs/
│   │   └── losu-syntax.md # 洛书语法文档（Markdown + LaTeX）
│   ├── vendor/
│   │   ├── monaco/    # Monaco Editor（从 npm 链接）
│   │   └── drawio/    # draw.io viewer（MIT 许可）
│   └── icon.png       # 应用图标（256x256）
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

- **前端**：原生 JavaScript（无框架）、Monaco Editor、draw.io viewer、xterm.js
- **后端**：Node.js + Express
- **量子运行时**：Losu v2（C++）+ Qufu（声明式 DSL）+ QuEST 4.2.0（C），通过 Emscripten 编译为 WASM
- **桌面打包**：Electron 44.7.0 + electron-builder 26.15.3
- **图表渲染**：draw.io（mxGraph）静态 viewer
- **文档渲染**：marked.js (Markdown) + KaTeX (LaTeX)
- **终端模拟器**：xterm.js + xterm-addon-fit

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
- **量子线路图**显示 H 门、CNOT 门及两次测量，彩色门符号自动对齐
- **程序流程图**解析代码结构（import、变量声明、量子操作、打印）
- **步进调试器**可逐步查看 H 门和 CNOT 门操作后的量子态演化

### 2. 量子隐形传态

打开 `examples/teleportation.losu`，点击"运行"：
- 量子态从 Alice 传送到 Bob，保真度 100%
- 线路图显示纠缠对制备、贝尔测量、条件门操作
- 步进器滑动条可逐步查看每个门操作后的量子态演化
- 点击"▶ 播放"按钮自动演示整个传态过程

### 3. 查看语法文档

点击顶部工具栏的"语法"按钮，打开洛书语法文档：
- 支持 Markdown 格式化文本
- LaTeX 数学公式自动渲染（如 $|\psi\rangle = \alpha|0\rangle + \beta|1\rangle$）
- 包含量子门操作、控制流、函数定义等完整语法说明

### 4. 使用终端

切换到底部面板的"终端"标签：
- 完整的终端模拟器，支持 ANSI 转义序列
- 自动适配主题颜色
- 支持中文显示和复制粘贴

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

### 编译与运行洛书 2.0

仓库内的 `src/losu v2.0.0` 当前可以通过 C99 编译生成静态归档；根目录 CLI 统一提供源码检查、构建和运行入口：

```bash
npm run losu -- --help
npm run losu -- check examples/hello.losu
npm run losu:build
npm run losu -- run examples/hello.losu
```

`losu:build` 会在 `src/losu v2.0.0/src/test/liblosu-v2.a` 生成 C 模块归档。由于上游 C 树还缺少 VM 初始化、IO 和脚本加载入口，`run`/`trace` 当前使用仓库内已经验证的 QLosu WASM 后端，输出会标明该后端。

### 洛书 1.0 与工作台操作

```bash
npm run losu1:build            # 从 src/losu-1.0 编译 Gitee lpk 原生解释器及标准库
npm run code-server:check      # 检查原生终端、命令输出和文件监听
npm run code-server:repair     # Node 升级或原生模块缺失时重建依赖，然后重启工作台
npm run smoke:workbench        # 无头 Chrome 验证实际工作台交互（需要 Chrome）
```

在“选择运行时版本”列表中选择洛书 1.0 或 2.0，也可选择项目已配置的外部运行时。1.0 示例为 `examples/losu1/hello.els`；量子示例使用 2.0 WASM。语法手册随扩展离线打包并按版本切换，无需把 IDE 源码目录作为工作区。运行、量子线路和流程图默认使用上次打开的洛书文件；切换到手册或终端后也能直接执行。量子线路提供中文操作说明、测量结果和寄存器切换。

详见 [编译与运行说明](docs/losu-build.md)。

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
- xterm.js：MIT
- marked.js：MIT
- KaTeX：MIT

## 致谢

- **Losu 语言与洛书语法**由 QLosu 项目开发
- **QuEST**（Quantum Exact Simulation Toolkit）by Quest Team
- **Monaco Editor** by Microsoft
- **draw.io / diagrams.net** by jgraph
- **xterm.js** by xtermjs
- **marked.js** by Christopher Jeffrey
- **KaTeX** by Khan Academy

---

**问题反馈与贡献**：https://github.com/zx539/QLosu-IDE/issues

**在线体验**：访问测试页面 http://127.0.0.1:8080/test.html（启动服务后）

**版本**：1.0.0 (2026-10-14)
