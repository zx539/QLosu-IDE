# 洛书（Losu）

洛书（Losu）是一款国产开源编程语言及其运行时基础设施，项目整体规划包含语言设计、编译器、虚拟机和标准库；当前仓库主要包含以 C 语言实现的词法分析、语法分析、抽象语法树、字节码生成、虚拟机、对象系统、内存管理、垃圾回收、线程/纤程和 C API 等模块。

项目面向需要可嵌入、可扩展和可控执行能力的软件，重点探索物联网业务规则、工业软件中的规则执行、边缘数据转换、设备逻辑编排、领域特定语言和 C/C++ 宿主扩展场景。申报材料中提到的 W806、LoongArch、QuEST、国产操作系统/RTOS 等应用方向，应以对应示例、版本和测试记录为准。

> 当前状态：开发中。工业适配、性能基准、完整构建链路、自动化测试和生产案例仍在建设中。

## 主要特性

- C99 风格的语言运行时实现；
- Lexer、Parser、AST、Backend 和 VM 的完整模块划分；
- 固定格式字节码指令和操作数编码；
- 栈、寄存器上下文、函数、字符串池和对象管理；
- Nil、Float、String、Unit、Func、Resource、Thread、Meta、Context 等对象类型；
- 垃圾回收的标记和清扫接口；
- 用户级线程、纤程、yield/await 相关接口；
- 一元/二元操作符、调用、获取和设置操作符扩展入口；
- 通过统一头文件 `src/losu/include/losu.h` 访问公共 API；
- 可配置的平台支持、I/O 缓冲区、GC 更新规则、日志级别和后端优化开关。

申报材料提出多范式、轻量、高性能、WebAssembly、元编程和 Agent 编码等方向；其中具体指标和完整能力范围需要以相应版本的测试报告、语言规范或可运行示例为准。

## 快速开始

### 获取源码

```bash
git clone https://gitee.com/chen-chaochen/lpk.git
cd lpk
```

### 当前源码验证

当前仓库提供 `src/test/makefile`，使用 C99 编译选项构建源码模块并生成静态归档。上游 C 树目前缺少 VM 初始化和脚本加载入口，因此该归档是可复用的编译产物，不是完整解释器。

```bash
cd src/test
make                 # 生成 liblosu-v2.a
make objects         # 只编译目标文件
make clean
```

仓库根目录的 `scripts/losu2-cli.js` 是洛书 2.0 的统一命令入口：

```bash
npm run losu -- --version
npm run losu -- check examples/hello.losu
npm run losu:build
npm run losu -- run examples/hello.losu
```

`run` 和 `trace` 当前使用 QLosu WASM 后端；待上游补齐 `losu_vm_init`、IO、脚本加载和执行核心后，再切换到原生 C VM。

## 安装方式

当前尚未提供稳定的系统级安装包、动态库或包管理器发布方式；`liblosu-v2.a` 可作为 C 宿主的实验性静态归档。

计划支持：

- CMake 构建和安装；
- 动态库和 C 宿主 SDK；
- 原生 CLI 编译/运行工具（当前仓库 CLI 使用 WASM 运行后端）；
- Linux x86_64、ARM64 及国产操作系统/CPU 环境；
- C/C++ 宿主 SDK 和示例工程。

申报材料称项目已在 W806 物联网芯片、LoongArch 相关平台及其他国产软硬件环境开展应用探索。欢迎在补充硬件型号、系统版本、构建过程和测试结果后，将这些内容升级为正式兼容性说明。

## 使用示例

当前 C 仓库没有稳定的脚本执行入口；可使用根目录 CLI 验证 `.losu` 文件，C API 宿主示例仍需等待 VM 初始化流程稳定：

```c
/* 伪代码：待 API 初始化流程稳定后更新为可编译示例 */
#include "losu/include/losu.h"

int main(void) {
    /* 1. 创建 VM */
    /* 2. 注册宿主函数或 Resource */
    /* 3. 载入并解析 Losu 源码 */
    /* 4. 生成字节码并执行 */
    /* 5. 处理错误并释放 VM */
    return 0;
}
```

工业场景示例规划：

1. 告警条件表达式；
2. 边缘数据格式转换；
3. 设备模拟器规则；
4. 非安全关键任务编排；
5. C/C++ 宿主注入资源和回调。

其他探索方向包括与 QuEST 量子模拟器结合的声明式量子程序设计语言、科学计算、工业 GUI、工业仿真和机器人运动学等领域特定语言。上述内容属于扩展方向或申报材料中的应用案例，应通过独立示例仓库和复现实验说明与核心运行时边界。

## 项目架构

```text
src/losu/
├── include/
│   ├── base/       # 内存、字符串、I/O、CPU 基础 API
│   ├── syntax/     # Lexer、Parser、AST、Backend API
│   ├── object/     # 对象和对象类型 API
│   └── vm/         # VM、GC、栈、线程、纤程和信号 API
└── src/
    ├── base/
    ├── object/
    ├── syntax/
    └── vm/
```

执行链路：

```text
源码 → Lexer → Token → Parser/AST → Backend/字节码 → VM → 对象/GC/线程
```

## 贡献方式

欢迎围绕以下方向贡献：

- 构建系统和跨平台移植；
- Lexer、Parser、AST 和字节码测试；
- VM、GC、栈和对象系统测试；
- C/C++ 宿主 API 设计；
- 文档、示例和语言规范；
- 工业场景 PoC 与适配器；
- 性能基准、安全边界和模糊测试。

在正式贡献前，请先阅读或协助完善：

- `CONTRIBUTING.md`（待补充）；
- `CODE_OF_CONDUCT.md`（待补充）；
- `SECURITY.md`（待补充）；
- RFC/设计讨论流程（待补充）。

推荐流程：

1. 提交 Issue 描述问题或设计目标；
2. 对涉及语言、VM、ABI 或许可证的变化先进行设计讨论；
3. 提交小而独立的 Pull Request；
4. 添加测试和文档；
5. 通过代码审查和 CI 后合并。

## 项目成果与社区经历

根据申报材料，项目/团队曾获得或参与：GVP—Gitee 最有价值开源项目（2024）、G-Star 产品毕业认证（Gitcode）、OpenKylin 年度技术新秀奖（2024）、中科院开源之夏（OSPP）合作社区，以及《国产编程语言蓝皮书》相关工作。申报材料还列出新华网报道、2025 年国家关键领域语言科技赋能创新项目案例、中国国际大学生创新大赛金奖（2025）和中国机器人及人工智能大赛一等奖（2026）等信息。

相关荣誉、报道、合作及应用案例的正式引用，应在发布前补充可访问链接、证书或授权说明。

## 开源许可证

Losu 源码文件声明：

```text
AGPL-3.0-or-later OR MulanPSL-2.0
```

详细条款请参阅：

- [`LICENSE`](./LICENSE)
- [`LICENSE-MulanPSLv2`](./LICENSE-MulanPSLv2)

商业使用或许可证适用问题请联系：`losulang@foxmail.com`。最终许可证政策以项目维护者发布的正式说明和许可证文本为准。

## 联系方式

- 官网：[https://losu.tech](https://losu.tech)
- 代码仓库：[https://gitee.com/chen-chaochen/lpk](https://gitee.com/chen-chaochen/lpk)
- 申报联系人：方淼，`fangmiao@neuq.edu.cn`
- 技术/商务联系：`losulang@foxmail.com`

## 路线图

### v2.x 工程化

- [ ] 修复并稳定基础构建链路；
- [ ] 增加最小可运行 CLI；
- [ ] 增加单元测试、集成测试和 CI；
- [ ] 完善 VM 初始化和宿主 API；
- [ ] 发布语言与字节码说明；
- [ ] 增加错误诊断、栈追踪和日志文档。

### 工业场景验证

- [ ] 规则表达式示例；
- [ ] 边缘数据转换示例；
- [ ] Resource/Context 宿主绑定示例；
- [ ] 资源配额、执行超时和能力白名单；
- [ ] Linux/ARM64/国产环境验证；
- [ ] 性能与可靠性报告。

### 生态建设

- [ ] CMake、安装包和版本化制品；
- [ ] API 文档和语言规范；
- [ ] 格式化工具、语法高亮和 LSP；
- [ ] RFC、贡献指南和安全响应流程；
- [ ] 工业适配器生态和联合案例。
