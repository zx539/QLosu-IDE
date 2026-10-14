#!/usr/bin/env bash
# QLosu WASM 构建脚本
# 将 Losu v2 内核 + Qufu 声明式框架 + QuEST 4.2.0 编译为 WebAssembly（qlosu.m.js / qlosu.m.wasm）
#
# 前置条件：
#   1. 安装 emscripten（emcc/em++/emcmake 在 PATH 中），版本 ≥ 3.1.x
#   2. QuEST 需先完成 WASM 适配（memory.cpp 已含 __EMSCRIPTEN__ 分支，见
#      ../losu-quest/QuEST/quest/src/core/memory.cpp 顶部）
#
# 用法：
#   ./build.sh <目标输出目录>   # 默认输出到 ../wasm-build-out
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"          # src/ 的上一级，即包根目录
OUT_DIR="${1:-$ROOT/wasm}"

LOSU_SRC="$ROOT/src/losu-quest/lpk-sdk/core"
APP_SRC="$ROOT/src/QLosu/Qapp/src/script/src"   # 含 losu_config.h 与 qufu.losu
QUEST_SRC="$ROOT/src/losu-quest/QuEST"

BUILD_TMP="$(mktemp -d /tmp/qlosu-wasm.XXXXXX)"
trap 'rm -rf "$BUILD_TMP"' EXIT

echo "==> 编译 Losu v2 内核 (emcc)"
mkdir -p "$BUILD_TMP/losu-obj"
for c in $(find "$LOSU_SRC" -name '*.c'); do
  emcc -O2 -I "$LOSU_SRC/include" -I "$APP_SRC" -c "$c" -o "$BUILD_TMP/losu-obj/$(basename "$c" .c).o"
done

echo "==> 编译 QuEST（WASM 适配版）"
mkdir -p "$BUILD_TMP/quest-build"
# 预置空 MATH_LIBRARY，跳过 find_library(m REQUIRED)（emscripten 下无原生 m 库）
emcmake cmake -S "$QUEST_SRC" -B "$BUILD_TMP/quest-build" \
  -D ENABLE_MULTITHREADING=OFF -D MATH_LIBRARY="" -D CMAKE_BUILD_TYPE=Release >/dev/null
# emcc 不支持 -Ofast，QuEST Release 构建默认使用 -Ofast（见其 CMakeLists.txt 的 patch_flags）
# 此处统一替换为 -O3 后重新构建
grep -rl '"\-Ofast"' "$BUILD_TMP/quest-build" 2>/dev/null | xargs -r sed -i 's/"-Ofast"/"-O3"/g' || true
emmake make -C "$BUILD_TMP/quest-build" -j"$(nproc)" >/dev/null

echo "==> 生成 qufu.losu 内嵌模块"
python3 "$SCRIPT_DIR/gen_mod.py" "$APP_SRC/qufu.losu" "$BUILD_TMP/qlosu_mod.c"

echo "==> 编译适配层"
emcc -O2 -I "$LOSU_SRC/include" -I "$APP_SRC" -I "$QUEST_SRC" -I "$BUILD_TMP/quest-build" \
  -c "$SCRIPT_DIR/ffi_wasm.c" -o "$BUILD_TMP/ffi_wasm.o"
emcc -O2 -I "$LOSU_SRC/include" -I "$APP_SRC" -I "$QUEST_SRC" -I "$BUILD_TMP/quest-build" \
  -c "$SCRIPT_DIR/qlosu_web.c" -o "$BUILD_TMP/qlosu_web.o"
emcc -O2 -I "$LOSU_SRC/include" -I "$APP_SRC" -I "$QUEST_SRC" -I "$BUILD_TMP/quest-build" \
  -c "$SCRIPT_DIR/qlosu_ffi_reg.c" -o "$BUILD_TMP/qlosu_ffi_reg.o"

echo "==> 链接 WASM"
em++ -O2 "$BUILD_TMP/losu-obj/"*.o "$BUILD_TMP/ffi_wasm.o" "$BUILD_TMP/qlosu_web.o" \
  "$BUILD_TMP/qlosu_ffi_reg.o" "$BUILD_TMP/qlosu_mod.o" \
  "$BUILD_TMP/quest-build/libQuEST.a" -o "$OUT_DIR/qlosu.m.js" \
  -s MODULARIZE=1 -s EXPORT_NAME=createQLosu \
  -s EXPORTED_FUNCTIONS=_qlosu_version,_qlosu_run,_malloc,_free \
  -s EXPORTED_RUNTIME_METHODS=stringToUTF8,lengthBytesUTF8,UTF8ToString \
  -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web,node --no-entry

echo "==> 完成：$OUT_DIR/qlosu.m.js / qlosu.m.wasm"
