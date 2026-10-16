# 洛书编译与运行

QLosu-IDE 同时包含洛书 2.0 的 C 模块源码和一个可直接运行的 WASM 后端。两者的职责不同：C 源码可以编译成静态归档，WASM 后端负责当前 IDE 的脚本执行、量子轨迹和流程图数据。

## 洛书 1.0（Gitee lpk）

源码目录为 `src/losu-1.0`，来源为用户指定的 <https://gitee.com/chen-chaochen/lpk>，当前提交 `d92bc57bb3a4c668821dbc0252b4ea5ef4f7f32c`。保留原始源码，构建结果放到 `runtime/losu1/linux-x64`（其他架构使用相应目录）。

```bash
# 首次获取源码；仓库已有源码时无需重复克隆
# git clone https://gitee.com/chen-chaochen/lpk src/losu-1.0
npm run losu1:build
runtime/losu1/linux-x64/losu -v
runtime/losu1/linux-x64/losu examples/losu1/hello.els
```

构建包括内核动态库、解释器和十个标准库模块，动态库通过 `$ORIGIN` 定位，可移动整个产物目录。无需安装到 `/els` 或修改系统动态库配置。当前构建脚本支持 Linux C99 工具链。

在洛书工作台点击“选择运行时版本”，从列表选择“洛书 1.0 · lpk 原生解释器”，再打开 `examples/losu1/hello.els` 运行；不用手输版本。列表会显示已安装的内置和外部运行时及可用状态。工作台选择优先于项目默认 `qlosu.json`，Agent 运行也使用所选版本。

“1.0”是 IDE 对此 lpk 代码线的分类名；上游当前的 `-v` 自报 `core_version 2.0.0`，列表与 `build.json` 保留这个原始版本信息。此解释器不包含 QLosu 量子库；量子示例和线路追踪请选择“洛书 2.0.0”。

## 手册、终端与文件焦点

语法手册随扩展离线打包，按所选运行时显示 1.0 或 2.0 文档；不要求工作区就是 IDE 源码目录。`qlosu.syntaxRoot` 可指定工作区相对路径或绝对路径，也支持 `qlosu.json.syntaxRoots` 按版本配置。内置手册的相对文档链接和图片均做存在性检查。

运行、量子线路与程序流程图会记住上次打开的洛书文件，切换到手册、终端或 Agent 后可直接点击。工作区状态保存上次文件 URI，窗口重新加载后仍可恢复；执行时读取当前编辑内容，包括未保存的修改。

Code-Server 的原生终端依赖本机 Node ABI。升级 Node 或从源码重新构建后执行：

```bash
npm run code-server:repair
npm run code-server:check
```

修复会重建 PTY、watchdog、文件监听和日志原生模块；检查使用真实 shell 输入输出和文件监听事件。原生模块重建后请重启旧的 Code-Server 进程。

## 洛书 2.0

在仓库根目录执行：

```bash
npm run losu -- --version
npm run losu -- check examples/hello.losu
npm run losu:build
npm run losu -- run examples/hello.losu
npm run losu -- trace examples/hello.losu
```

构建命令调用 `src/losu v2.0.0/src/test/makefile`，使用 `clang`（也可以通过 `cc=` 指定编译器）将 C 模块编译为 `liblosu-v2.a`：

```bash
cd 'src/losu v2.0.0/src/test'
make
make objects
make clean
```

当前上游 C 树还缺少稳定的 `losu_vm_init`、脚本加载、IO 注册和执行入口，所以归档不是完整的原生解释器。CLI 的 `run` 和 `trace` 使用仓库内 QLosu WASM 后端，并在 trace 的诊断输出中标明后端。补齐上游 VM 入口后，可以在 `runtime/adapters/makefile.js` 替换为原生执行器。

## 普通洛书源码

如果要直接编译上游洛书（而不是通过本 IDE 的 WASM 后端），可以使用已有的 VS Code 分支源码：

```bash
git clone --branch vscode https://gitee.com/zx2198762717/lpk.git
cd lpk/src/test
make                 # 编译源码模块并生成 liblosu-v2.a
make objects         # 只生成 .o 文件
make clean
```

这条链路依赖本机 C99 工具链。当前上游仓库的 `src/test/makefile` 主要产出静态归档，是否能链接成可执行解释器取决于该分支是否补齐 VM 初始化、脚本加载和宿主 IO；若没有这些入口，使用 QLosu-IDE 的 `npm run losu -- run file.losu`。

## 其他洛书版本

在项目根目录的 `qlosu.json` 中注册外部运行时：

```json
{
  "losuVersion": "losu-system",
  "runtimes": [
    {
      "id": "losu-system",
      "label": "系统洛书",
      "command": "losu",
      "versionArgs": ["--version"],
      "runArgs": ["run"],
      "traceArgs": ["trace"],
      "timeoutMs": 30000
    }
  ]
}
```

运行时服务会通过 `/api/runtime/versions` 检测版本，通过 `/api/runtime/run`、`diagnose` 和 `trace` 统一调用，并把退出码、标准输出、错误和超时映射回 IDE。未配置的版本会直接报错，不会自动使用另一版本。

## 编译环境

Fedora x86_64 至少需要 Node.js 20、npm、Git、`clang` 或 `gcc`、`make` 和 Python。Code-Server 的源码构建还需要其文档列出的 Fedora 开发包；洛书静态归档本身只需要 C99 编译工具链。
