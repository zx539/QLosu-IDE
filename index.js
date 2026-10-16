/**
 * QLosu 量子编程平台 —— 独立本地运行版 服务端入口
 *
 * QLosu 将量子编程语言 Losu v2（LGPL 兼容双许可）与声明式量子框架 Qufu、
 * 量子模拟器 QuEST 4.2.0 通过 emcc 编译为 WebAssembly（WASM），
 * 无需任何原生依赖即可在本机离线运行。
 *
 * 运行方式：
 *   node index.js                    # 默认 http://127.0.0.1:8080/
 *   PORT=9000 node index.js          # 指定端口
 *   QLOSU_WORKSPACE=~/code node index.js   # 指定工作区目录
 *
 * 桌面版见 electron/main.js（npm run desktop）。
 *
 * API：
 *   GET  /api/version        # 运行时版本信息
 *   GET  /api/examples       # 示例脚本列表
 *   GET  /api/examples/:name # 示例脚本内容
 *   POST /api/run            # 运行 .losu 代码（body: { code, trace }），返回输出与门调用记录
 *   GET  /api/fs/*           # 工作区文件读写
 *   POST /api/llm/chat       # 大模型代理（Agent 使用，流式 NDJSON）
 */
const { startServer } = require('./apps/server/app');

const PORT = Number(process.env.PORT) || 8080;
const HOST = process.env.HOST || '127.0.0.1';

startServer({ port: PORT, host: HOST })
  .then(({ url }) => {
    console.log(`QLosu 量子编程平台（本地运行版）已启动：${url}`);
    if (HOST !== '127.0.0.1' && HOST !== 'localhost') {
      console.warn('警告：服务监听在非本机地址，工作区与大模型接口没有身份验证，请勿暴露到不可信网络。');
    }
    console.log('按 Ctrl+C 退出');
  })
  .catch((e) => {
    console.error('启动失败：', e.message);
    process.exit(1);
  });
