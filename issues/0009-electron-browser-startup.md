# 0009 Electron/浏览器启动闭环

- **优先级：** P0
- **状态：** 浏览器与 Linux Electron 工作台启动、运行、终端和退出清理已完成验收
- **范围：** `electron/`、`apps/code-server/`、启动脚本

## 背景

Electron 已尝试启动 Code-Server，缺少 Code-Server 时回退旧页面；当前环境调查显示 Electron 44.7.0 二进制未安装，浏览器模式也需要明确区分 API 服务和 Code-Server 地址。

## 待完成

- 修复 Electron 依赖下载/缓存和打包验证。
- Electron 启动后自动加载 Code-Server，而不是依赖用户复制地址。
- 浏览器模式输出 API 地址和 Code-Server 地址，并正确注入扩展环境变量。
- 支持打开指定工作区、关闭子进程和异常退出清理。
- 对端口冲突、Code-Server 崩溃和用户关闭窗口增加提示。

## 验收标准

- `npm run code-server` 和 `npm run desktop` 都能启动主界面。
- 关闭窗口后 Code-Server/API 子进程不会残留。
- 选择工作区后 Code-Server 资源管理器显示正确目录。

## 本次完成

- 新增 `npm run smoke:local`：实测旧版回退页的根资源、洛书 Logo、xterm CSS/JS、draw.io viewer、流程图 API、终端命令和 Code-Server 页面。
- 冒烟测试以当前项目目录启动 Code-Server，断言工作区目录参数正确，并在结束时验证子进程已退出。
- 增加旧 Web IDE 的 Logo 静态资源映射，确认现有 xterm 映射返回正确 MIME；旧流程图继续在独立 iframe 渲染。
- 修复扩展拼接带尾斜杠的 API 地址时产生 `//api/...` 的问题；冒烟测试使用同一地址生成函数。
- SIGINT/SIGTERM 通过回调传入数值退出码，避免把信号名称传给 `process.exit()`。
- 修复冒烟测试打印 PASS 后仍不退出的问题：读取完 HTTP 响应体，避免未消费的连接阻塞 API 关闭；请求和关闭过程增加超时，Code-Server 关闭失败时仍清理 API。PASS 只在清理与子进程退出断言完成后输出。
- 回归验证：`npm run smoke:local` 成功并自然退出，覆盖终端命令输出、流程图 XML、Logo 和静态资源、Code-Server 工作区页面及子进程退出。该检查不代替工作台中的终端键盘交互和文件夹选择器验收。
- 浏览器工作台实测根地址和带 `?folder=` 的工作区地址均返回 HTTP 200；`npm run smoke:local` 固化了该检查及洛书工作台配置检查。
- 新增 `electron/launcher.js`、`scripts/launch-desktop.js` 和 `npm run desktop:check`：启动前检查 Electron 本机二进制，支持 `QLOSU_ELECTRON_BIN`、`QLOSU_WORKSPACE`、`--workspace`，缺失时输出可执行修复提示，不再显示 Electron 包装器的模糊错误。
- Electron 主进程监听 Code-Server 异常退出并回退旧版兼容页面；窗口关闭和应用退出继续幂等清理 API 与 Code-Server 子进程。

尚待目标桌面人工验收：系统文件夹选择对话框及不同平台窗口行为。此前的 Electron 二进制缺失问题已在 0014 解决。

## 0014 后续进展

补充：Electron 44.7.0 已通过官方 checksum 校验安装。新增一致的 API/Code-Server 工作区、选择目录菜单、上次工作区记录、可写用户数据目录及退出信号清理。源码与打包应用都通过真实窗口加载、终端键盘命令和关闭窗口后的 API/Code-Server 退出测试。系统原生目录对话框实现仍需目标桌面人工验收；其他平台未验收。
