# QuEST 项目中文编译说明

## 项目简介

本项目是 **QuEST**，全称 **Quantum Exact Simulation Toolkit**，是一个高性能量子态矢量和密度矩阵模拟库。

它支持：

- 普通 CPU 单机运行
- OpenMP 多线程
- MPI 分布式运行
- NVIDIA CUDA GPU 加速
- AMD HIP GPU 加速
- NVIDIA cuQuantum/cuStateVec 后端
- C 和 C++ 用户接口

项目主要用于量子计算模拟、量子线路演化、量子测量、噪声模型、Pauli 运算等。

## 使用语言

本项目主要语言如下：

- **库源码**：C++
- **QuEST 核心源码标准**：C++17
- **C 用户接口**：C11
- **C++ 用户接口**：C++14 起可用，但本项目内部源码按 C++17 编译
- **测试代码**：C++20
- **构建系统**：CMake
- **可选脚本**：Bash

主要目录：

```text
.
├── CMakeLists.txt              # 顶层 CMake 构建入口
├── quest/
│   ├── include/                # 公共头文件
│   └── src/                    # QuEST 核心源码
├── examples/                   # 示例程序
├── tests/                      # 单元测试和集成测试
├── docs/                       # 官方文档
├── cmake/                      # CMake 包配置模板
└── utils/                      # 工具脚本和文档资源
```

## 编译依赖

基础编译需要：

- CMake
- C 编译器，例如 `gcc` 或 `clang`
- C++ 编译器，例如 `g++` 或 `clang++`
- Make 或 Ninja

推荐版本：

- CMake 3.x
- GCC 10+ 或 Clang 10+
- 支持 C++17 的编译器

可选依赖：

- OpenMP：用于多线程，默认开启
- MPI：用于分布式运行
- CUDA：用于 NVIDIA GPU
- HIP/ROCm：用于 AMD GPU
- cuQuantum：用于 NVIDIA cuStateVec 加速
- Catch2：用于测试，测试开启时需要；默认可由 CMake 自动下载

## 最简单编译方式

在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build --parallel
```

编译完成后会生成：

```text
build/libQuEST.so
build/min_example
```

其中：

- `libQuEST.so` 是 QuEST 动态库
- `min_example` 是默认总会编译的最小示例程序

运行最小示例：

```bash
./build/min_example
```

如果你想避免 OpenMP 依赖，可以关闭多线程：

```bash
cmake -S . -B build \
  -D ENABLE_MULTITHREADING=OFF \
  -D ENABLE_TESTING=OFF \
  -D BUILD_EXAMPLES=OFF

cmake --build build --parallel
./build/min_example
```

## 推荐 Release 编译

正式使用建议开启 Release：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release

cmake --build build --parallel
```

如果希望重新配置，建议先删除旧构建目录：

```bash
rm -rf build
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## 编译示例程序

默认只编译 `min_example`。

如需编译 `examples/` 下更多示例：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D BUILD_EXAMPLES=ON

cmake --build build --parallel
```

示例程序会生成在类似目录中：

```text
build/examples/isolated/
build/examples/extended/
build/examples/automated/
```

运行示例：

```bash
./build/examples/extended/dynamics_c
./build/examples/extended/dynamics_cpp
```

文件后缀含义：

- `_c`：由 `.c` 文件编译
- `_cpp`：由 `.cpp` 文件编译

## 编译测试

测试默认关闭。开启测试：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_TESTING=ON

cmake --build build --parallel
```

运行测试：

```bash
./build/tests/tests
```

或使用 CTest：

```bash
cd build
ctest
```

测试代码需要 C++20。
如果系统没有 Catch2，项目默认会尝试下载 Catch2：

```bash
-D DOWNLOAD_CATCH2=ON
```

如果你已经安装了 Catch2，可以关闭自动下载：

```bash
-D DOWNLOAD_CATCH2=OFF
```

## 常用 CMake 参数

| 参数 | 默认值 | 说明 |
|---|---:|---|
| `CMAKE_BUILD_TYPE` | `Release` 或 CMake 默认 | 构建类型，常用 `Release` / `Debug` |
| `BUILD_SHARED_LIBS` | `ON` | 构建动态库；设为 `OFF` 可构建静态库 |
| `LIB_NAME` | `QuEST` | 输出库名 |
| `VERBOSE_LIB_NAME` | `OFF` | 是否在库名中加入精度、MPI、GPU 等信息 |
| `FLOAT_PRECISION` | `2` | 浮点精度：`1` 单精度，`2` 双精度，`4` 四精度 |
| `BUILD_EXAMPLES` | `OFF` | 是否编译全部示例 |
| `ENABLE_TESTING` | `OFF` | 是否编译测试 |
| `ENABLE_MULTITHREADING` | `ON` | 是否启用 OpenMP |
| `ENABLE_DISTRIBUTION` | `OFF` | 是否启用 MPI |
| `ENABLE_CUDA` | `OFF` | 是否启用 NVIDIA CUDA |
| `ENABLE_HIP` | `OFF` | 是否启用 AMD HIP |
| `ENABLE_CUQUANTUM` | `OFF` | 是否启用 NVIDIA cuQuantum |
| `ENABLE_DEPRECATED_API` | `OFF` | 是否启用旧 API |
| `DISABLE_DEPRECATION_WARNINGS` | `OFF` | 是否关闭旧 API 警告 |

## 浮点精度选择

默认是双精度：

```bash
-D FLOAT_PRECISION=2
```

可选值：

```bash
-D FLOAT_PRECISION=1   # single / float
-D FLOAT_PRECISION=2   # double，默认
-D FLOAT_PRECISION=4   # quad / long double
```

注意：

- GPU 编译不支持 `FLOAT_PRECISION=4`
- 如果开启 CUDA 或 HIP，请使用 `1` 或 `2`

## OpenMP 多线程编译

OpenMP 默认开启：

```bash
-D ENABLE_MULTITHREADING=ON
```

显式开启：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_MULTITHREADING=ON

cmake --build build --parallel
```

运行时可以设置线程数：

```bash
OMP_NUM_THREADS=8 ./build/min_example
```

如果你的系统没有 OpenMP 或配置失败，可以关闭：

```bash
cmake -S . -B build \
  -D ENABLE_MULTITHREADING=OFF
```

macOS 上如果找不到 OpenMP，通常需要：

```bash
brew install libomp
```

## MPI 分布式编译

开启 MPI：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_DISTRIBUTION=ON

cmake --build build --parallel
```

确认 MPI 编译器可用：

```bash
mpicxx --version
```

运行分布式程序：

```bash
mpirun -np 4 ./build/min_example
```

如果运行测试：

```bash
mpirun -np 8 ./build/tests/tests
```

## NVIDIA CUDA 编译

需要安装 CUDA Toolkit，并确认：

```bash
nvcc --version
```

开启 CUDA：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_CUDA=ON \
  -D CMAKE_CUDA_ARCHITECTURES=80

cmake --build build --parallel
```

`CMAKE_CUDA_ARCHITECTURES` 需要根据 GPU 计算能力设置，例如：

- A100：`80`
- V100：`70`
- RTX 30 系列：通常 `86`
- RTX 40 系列：通常 `89`

注意：GPU 构建不支持四精度：

```bash
-D FLOAT_PRECISION=4
```

不能与 CUDA/HIP 一起使用。

## AMD HIP 编译

需要安装 ROCm/HIP。

查看 AMD GPU 架构名：

```bash
rocm_agent_enumerator -name
```

示例：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_HIP=ON \
  -D CMAKE_HIP_ARCHITECTURES=gfx90a

cmake --build build --parallel
```

注意：

- `ENABLE_CUDA` 和 `ENABLE_HIP` 不能同时开启
- HIP 同样不支持 `FLOAT_PRECISION=4`

## cuQuantum 编译

cuQuantum 依赖 CUDA，因此必须同时开启 CUDA：

```bash
export CUQUANTUM_ROOT=/path/to/cuquantum

cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_CUDA=ON \
  -D CMAKE_CUDA_ARCHITECTURES=80 \
  -D ENABLE_CUQUANTUM=ON

cmake --build build --parallel
```

限制：

- 需要 NVIDIA GPU
- 需要 CUDA
- `ENABLE_CUQUANTUM=ON` 时必须 `ENABLE_CUDA=ON`
- cuStateVec 通常要求较新的 NVIDIA GPU 架构

## 编译自己的程序

项目支持通过 `USER_SOURCE` 和 `OUTPUT_EXE` 直接把用户源码和 QuEST 一起编译。

假设你有：

```text
my_program.c
```

可以执行：

```bash
cmake -S . -B build \
  -D USER_SOURCE=my_program.c \
  -D OUTPUT_EXE=my_program

cmake --build build --parallel
```

运行：

```bash
./build/my_program
```

C++ 文件也可以：

```bash
cmake -S . -B build \
  -D USER_SOURCE=my_program.cpp \
  -D OUTPUT_EXE=my_program

cmake --build build --parallel
```

## 安装 QuEST

安装到默认位置：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release

cmake --build build --parallel
cmake --install build
```

指定安装目录：

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_INSTALL_PREFIX=/opt/QuEST

cmake --build build --parallel
cmake --install build
```

安装后会包含：

```text
include/
lib/
bin/
lib/cmake/QuEST/
```

## 外部 CMake 项目中使用 QuEST

如果 QuEST 已安装，可在外部项目中使用：

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_quest_app C CXX)

find_package(QuEST REQUIRED)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE QuEST::QuEST)
```

然后：

```bash
cmake -S . -B build
cmake --build build --parallel
```

如果 QuEST 安装在非系统路径，配置时指定：

```bash
cmake -S . -B build \
  -D CMAKE_PREFIX_PATH=/opt/QuEST
```

## Windows 编译说明

Windows 推荐使用 CMake + Visual Studio 或 Ninja。

示例：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Windows 注意事项：

- 测试在 Windows 上可能要求 `BUILD_SHARED_LIBS=OFF`
- MSVC 不兼容 deprecated API，因此不要开启 `ENABLE_DEPRECATED_API`
- 可执行文件通常位于 `Release` 子目录中

## 清理构建目录

CMake 构建文件建议放在 `build/` 中。需要重新配置时：

```bash
rm -rf build
```

然后重新执行：

```bash
cmake -S . -B build
cmake --build build --parallel
```

## 常见问题

### 1. 找不到 OpenMP

关闭 OpenMP：

```bash
-D ENABLE_MULTITHREADING=OFF
```

或安装 OpenMP 运行库。

macOS：

```bash
brew install libomp
```

### 2. 找不到 MPI

确认 MPI 编译器存在：

```bash
mpicxx --version
```

如没有安装 OpenMPI 或 MPICH，需要先安装。

### 3. CUDA 架构设置错误

如果 `CMAKE_CUDA_ARCHITECTURES` 设置错误，可能编译失败或运行结果异常。需要根据你的 GPU compute capability 设置。

示例：

```bash
-D CMAKE_CUDA_ARCHITECTURES=80
```

### 4. 测试下载 Catch2 失败

如果网络不可用，关闭测试：

```bash
-D ENABLE_TESTING=OFF
```

或提前安装 Catch2 并使用：

```bash
-D DOWNLOAD_CATCH2=OFF
```

### 5. 四精度和 GPU 冲突

下面组合不允许：

```bash
-D FLOAT_PRECISION=4 -D ENABLE_CUDA=ON
```

或：

```bash
-D FLOAT_PRECISION=4 -D ENABLE_HIP=ON
```

请改用：

```bash
-D FLOAT_PRECISION=2
```

## 推荐快速命令

### CPU 基础版

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_TESTING=OFF \
  -D BUILD_EXAMPLES=OFF

cmake --build build --parallel
./build/min_example
```

### 无 OpenMP 兼容版

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_MULTITHREADING=OFF \
  -D ENABLE_TESTING=OFF \
  -D BUILD_EXAMPLES=OFF

cmake --build build --parallel
./build/min_example
```

### 编译全部示例

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D BUILD_EXAMPLES=ON

cmake --build build --parallel
```

### 编译并运行测试

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_TESTING=ON

cmake --build build --parallel
cd build
ctest
```

### MPI 版

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_DISTRIBUTION=ON

cmake --build build --parallel
mpirun -np 4 ./build/min_example
```

### CUDA 版

```bash
cmake -S . -B build \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_CUDA=ON \
  -D CMAKE_CUDA_ARCHITECTURES=80

cmake --build build --parallel
./build/min_example
```

## 结论

这个项目是一个 **C++ 实现、C/C++ 可调用、CMake 构建的量子模拟库**。

最小编译命令是：

```bash
cmake -S . -B build
cmake --build build --parallel
```

最小运行命令是：

```bash
./build/min_example
```

如果只想确认本机可以编译，建议先用：

```bash
cmake -S . -B build \
  -D ENABLE_MULTITHREADING=OFF \
  -D ENABLE_TESTING=OFF \
  -D BUILD_EXAMPLES=OFF

cmake --build build --parallel
./build/min_example
```
