

# QLosu · 量子编程平台

**一个基于声明式范式的量子程序设计环境**

[![License: MIT](https://img.shields.io/badge/license-MIT-lightgrey.svg)](LICENCE.txt)
[![QuEST](https://img.shields.io/badge/simulator-QuEST-6699ff.svg)](https://github.com/QuEST-Kit/QuEST)
[![Online](https://img.shields.io/badge/在线平台-qlosu.online-orange.svg)](http://www.qlosu.online)

</div>

---

## 项目简介

QLosu 包含三个核心组成部分：

- **Losu（洛书）** — 专为量子计算扩展的动态强类型脚本语言，支持 AOT 编译与 WebAssembly
- **QLvm** — 高性能基于栈的轻量级虚拟机，集成垃圾回收与量子资源管理
- **Qufu** — 声明式量子计算框架，封装底层 [QuEST](https://github.com/QuEST-Kit/QuEST) 模拟器

底层量子模拟由 **QuEST**（Quantum Exact Simulation Toolkit）提供，支持 CPU 多线程、MPI 分布式、NVIDIA/AMD GPU 加速。

---

## 目录

| 文档                                           | 说明               |
| -------------------------------------------- | ---------------- |
| [使用教程/使用教程.md](使用教程/使用教程.md)                 | 在线平台登录与使用指南      |
| [使用教程/洛书语法/README.md](使用教程/洛书语法/README.md)   | Losu 语言语法手册      |
| [使用教程/部署文档.md](使用教程/部署文档.md)                 | 服务器部署与运维文档       |
| [About_Cmake.md](About_Cmake.md)             | QuEST 编译详细说明（中文） |
| [docs/README.md](docs/README.md)             | QuEST 文档索引       |
| [docs/compile.md](docs/compile.md)           | 编译指南             |
| [docs/cmake.md](docs/cmake.md)               | CMake 参数说明       |
| [docs/compilers.md](docs/compilers.md)       | 支持的编译器列表         |
| [docs/launch.md](docs/launch.md)             | 运行方式（笔记本到超算）     |
| [docs/tutorial.md](docs/tutorial.md)         | QuEST 入门教程       |
| [docs/v4.md](docs/v4.md)                     | QuEST v4 新特性     |
| [docs/architecture.md](docs/architecture.md) | 代码架构说明           |
| [docs/contributing.md](docs/contributing.md) | 贡献指南             |
| [examples/README.md](examples/README.md)     | 示例程序说明           |
| [tests/README.md](tests/README.md)           | 测试说明             |

---

## 项目结构

```
.
├── quest/                  # QuEST 核心库（C++17）
│   ├── include/            # 公共头文件
│   └── src/                # 核心源码（cpu / gpu / comm / api）
├── src/
│   ├── code/               # 量子算法 C 实现（bell, grover, deutsch…）
│   ├── script/             # Losu 脚本示例与解释器配置
│   └── doc/md/             # 论文章节文档
├── examples/               # QuEST 示例程序
├── tests/                  # 单元测试与集成测试
├── docs/                   # QuEST 官方文档
├── 使用教程/               # 在线平台使用与部署文档
├── About_Cmake.md          # 中文编译说明
└── CMakeLists.txt          # 顶层构建入口
```

---

## 快速开始

**最小编译（需要 CMake + C++17 编译器）：**

```bash
git clone https://github.com/quest-kit/QuEST.git
cmake -S . -B build
cmake --build build --parallel
./build/min_example
```

无 OpenMP 环境：

```bash
cmake -S . -B build \
  -D ENABLE_MULTITHREADING=OFF \
  -D ENABLE_TESTING=OFF \
  -D BUILD_EXAMPLES=OFF
cmake --build build --parallel
```

更多编译选项（CUDA、MPI、HIP、cuQuantum）详见 [About_Cmake.md](About_Cmake.md)。

---

## 在线平台

访问 [http://www.qlosu.online](http://www.qlosu.online)，无需安装，浏览器即可使用。

内置 Losu 解释器，运行示例：

```bash
QLosu $LOSU_PATH/grover.losu
```

使用说明见 [使用教程/使用教程.md](使用教程/使用教程.md)。



<div align="center">
<sub>QuEST simulator by <a href="https://www.epcc.ed.ac.uk/">EPCC</a> / University of Edinburgh & Oxford · MIT License</sub>
</div>
