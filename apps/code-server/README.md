# Code-Server 运行器

QLosu 默认使用固定版本的 Code-Server（当前为 `4.95.3`）。开发机可以通过
`QLOSU_CODE_SERVER_BIN` 指定可执行文件；发布包将把相同版本放到
`vendor/code-server/`。

```bash
QLOSU_CODE_SERVER_BIN=/opt/code-server/bin/code-server npm run code-server
```

Fedora x86_64 从源码构建：

```bash
npm run code-server:build
npm run code-server
```

默认源码版本为 `v4.95.3`，可通过 `QLOSU_CODE_SERVER_REF` 覆盖。构建缓存位于 `.cache/code-server-source`，不会修改上游源码；生成的运行树位于 `vendor/code-server/4.95.3/linux-amd64`。

若希望首次启动时让 Code-Server 安装官方 Continue 扩展，可以设置
`QLOSU_INSTALL_CONTINUE=1`。默认不自动联网安装扩展。需要复现固定版本时，
再设置 `QLOSU_CONTINUE_VERSION`，启动器会传入
`Continue.continue@版本号`；不设置时使用 Open VSX/Marketplace 当前可用版本。

仓库提供不含密钥的配置模板，详见 [`docs/continue-ai.md`](../../docs/continue-ai.md)
和 [`config/continue/`](../../config/continue/)。Continue 的配置应放在项目的
`.continue/config.yaml` 或用户配置目录，API key 通过 Continue secrets/环境变量提供。

运行器只监听 `127.0.0.1`，使用无密码本地模式；不要把监听地址改成公网地址，除非已经在外层配置认证和 TLS。
