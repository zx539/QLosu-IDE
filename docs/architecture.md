# QLosu 新架构

```text
apps/
├── code-server/       Code-Server 版本管理、启动和本地端口
├── desktop/           Electron 迁移说明
└── server/            API 迁移说明
extensions/
└── qlosu-vscode/      Losu 语言、运行命令、图表和语法手册
language-server/
└── losu-lsp/          LSP stdio 服务
runtime/
├── adapters/          Makefile 和外部 CLI 适配器
└── manager.js         qlosu.json 版本选择
legacy/
└── web-ide/           旧 www/server/electron 入口说明
```

Code-Server 是默认工作台。旧 Web IDE 在迁移验收前继续保留。扩展通过本地 API 调用 WASM 运行时；LSP 通过 stdio 启动，不需要开放额外端口。

## 当前边界

- `losu-1.0` 是从 Gitee lpk 编译的原生解释器，包含内核和标准库。
- `losu-2.0.0` 的 C 模块可构建为静态归档；脚本执行明确使用 QLosu WASM 后端。
- 运行时通过工作台列表选择，API 统一返回后端、输出和退出码。
- 手册随扩展按版本打包，支持工作区覆盖路径；图表和运行共享上次洛书文档。
- 其他版本通过 `qlosu.json` 的 `runtimes` 配置外部命令。
- Continue 使用官方扩展；设置 `QLOSU_INSTALL_CONTINUE=1` 可让 Code-Server 启动时尝试安装。
- Linux 发布包使用 `scripts/package-linux-native.js` 调用 `dpkg-deb` 和 `rpmbuild`，避免 Fedora 上 electron-builder 内置 fpm 的旧版 Ruby/libcrypt 依赖；Windows 包仍由 electron-builder 负责。
