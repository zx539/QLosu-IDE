.PHONY : all help run test

NODE := node

all: help

# 启动本地独立运行版服务器（依赖已随包提供，无需 npm install）
run:
	@echo "正在启动 QLosu 本地独立运行版服务器..."
	@echo "访问地址: http://localhost:8080"
	@$(NODE) index.js

# 运行自检（验证 WASM 运行时与内置示例）
test:
	@$(NODE) test.js

# 重新构建 WASM（需要 emscripten 环境）
rebuild:
	@src/wasm-build/build.sh

help:
	@echo "QLosu 量子编程平台 · 本地独立运行版"
	@echo ""
	@echo "可用命令:"
	@echo "  make run     - 启动本地服务器 (node index.js)"
	@echo "  make test    - 运行自检 (node test.js)"
	@echo "  make rebuild - 从源码重建 WASM（需 emscripten）"
	@echo ""
	@echo "访问地址: http://localhost:8080"
