# 0004 洛书 2.0 与多版本运行时

- **优先级：** P0
- **状态：** 原生 lpk 1.0 与 2.0 WASM 已接入并验收；2.0 原生 VM 待上游
- **范围：** `runtime/`、`src/losu v2.0.0/`

## 背景

洛书 2.0 上游 C 树可以编译模块，但缺少 VM 初始化、IO 和脚本加载入口。仓库现在提供明确的 CLI/构建包装：C 模块生成静态归档，`.losu` 的 `run/trace` 走已验证的 QLosu WASM 后端，并在状态中标明后端。

## 待完成

- ~~修复并记录洛书 2.0 Makefile 的完整构建链路。~~
- ~~明确 CLI、VM 初始化、脚本加载和退出码接口。~~（CLI 已定义；原生 VM 接口仍待上游）
- ~~为 2.0 增加 `build/run/diagnose/trace` 适配器实现。~~（运行后端为 WASM）
- 支持 `qlosu.json` 的 `losuVersion`、文件级声明和外部运行时配置。
- 外部运行时增加版本探测、超时、环境变量和工作目录隔离。
- 缺少指定版本时提供构建、配置和文档入口，不自动降级。

## 验收标准

- `qlosu.json` 指定 2.0 时，运行、诊断和追踪使用同一版本。
- 指定外部版本时能显示版本号、运行日志和退出码。
- 运行时失败、超时和版本缺失都能映射到 Problems/Terminal。
- 至少有一个可重复的 Losu 2.0 示例通过 CI。

## 本次完成

- `runtime/manager.js` 已将 `src/losu v2.0.0/src/test/makefile` 注册为 `losu-2.0.0` 适配器，并支持 `qlosu.json` 中的外部版本配置。
- `/api/runtime/versions`、`/api/runtime/run`、`/api/runtime/diagnose` 和 `/api/runtime/trace` 已接入统一适配器；冒烟测试会确认 2.0 运行时可被发现。
- `scripts/losu2-cli.js` 提供 `check/build/run/trace`，`npm run losu:build` 会生成 `src/test/liblosu-v2.a`。
- `MakefileRuntimeAdapter` 已接入 CLI；`/api/runtime/run`、`diagnose` 和 `trace` 均返回统一退出码、诊断和 `backend: wasm`。
- 原生 C VM 尚未完成，CLI 不会伪称已经链接了上游 VM；补齐 `losu_vm_init` 等入口后可替换后端。

## 0014 后续进展

补充：0013 接入原生 lpk 及列表选择；0014 的 Electron 和发布包均实测运行 1.0 native 与 2.0 WASM。运行时进程使用独立随包 Node，避免 Electron ABI 不匹配。
