# Linux 发布与回退检查

## 构建前

- 确认仓库修改与许可证来源完整；不将 `.qlosu` 用户数据、模型密钥或下载缓存放入包中。
- `npm ci`、扩展生产依赖安装完成，运行 `npm run losu1:build`。
- Electron 二进制通过包内官方 SHA-256 校验，Code-Server 固定为 4.95.3。
- `npm run code-server:check` 验证原生 PTY / 文件监听与本机 Node ABI 匹配。
- `npm run test:all` 与 `npm run extension:package` 通过。
- `npm run continue:check` 通过；真实模型聊天仍需本机 Ollama/API 配置后单独验收。

## 生成和验收

```bash
npm run smoke:local
npm run smoke:workbench
npm run smoke:markdown
npm run smoke:desktop
npm run dist:linux
npm run dist:linux:packages
QLOSU_DESKTOP_APP="$PWD/dist/linux-unpacked/qlosu-standalone" npm run smoke:desktop
```

无显示会话使用 `xvfb-run -a` 或仓库的 `scripts/with-display.js`，虚拟显示只用于测试。

- 检查 `dist/SHA256SUMS` 和 `release-manifest.json`。
- 在临时目录运行解压包，确认从自身 `resources/app` 定位资源，而非依赖源码工作目录。
- 使用随包 Node，验证原生 1.0、WASM 2.0、终端、手册以及关闭窗口后的服务清理。
- 删除或移动源码的能力无需通过破坏开发目录验证；从非源码 cwd 启动并检查 `QLOSU_NODE_BIN` 和进程资源路径即可。
- Fedora 生成的包不代表兼容更旧的 glibc。正式分发应采用目标基线系统构建，并在目标系统复验。
- `npm run dist:linux:packages` 使用本机 `dpkg-deb` 和 `rpmbuild` 生成 `.deb` / `.rpm`；Electron Builder 的 fpm 路径依赖旧版 `libcrypt.so.1`，不作为 Fedora 的必要前置条件。
- 本次 CI 文件为待远程执行的配置；没有远程运行记录时不得标记为“CI 已通过”。

## 升级与回退

先关闭应用并保存文件。保留工作区和用户数据，完整解压新版至新目录；不要混合不同版本的 Electron、Node 和原生模块。需要回退时关闭新版并重新使用旧版完整目录。工作区源码与用户数据不会因更换应用目录自动删除。

## 当前边界

Windows / macOS 构建、不同发行版兼容性、真实模型服务和 Continue 兼容版本须分别验收。核心 CI、真实浏览器测试、桌面与发布包验收记录写入本地 Issue，避免把配置或单元测试视为平台发布结论。
Windows `exe` 使用 `npm run dist:windows`，需要 Windows 环境或 Linux 上可用的 Wine/NSIS，并能下载 Electron Windows 二进制；本机 Fedora 未生成 exe，不能将 Linux 结果当作 Windows 验收。
