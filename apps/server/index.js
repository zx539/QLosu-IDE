#!/usr/bin/env node
const { startServer } = require('./app');

const port = Number(process.env.PORT) || 8080;
const host = process.env.HOST || '127.0.0.1';

startServer({ port, host })
  .then(({ url }) => {
    console.log(`QLosu 量子编程平台（本地运行版）已启动：${url}`);
    if (host !== '127.0.0.1' && host !== 'localhost') {
      console.warn('警告：服务监听在非本机地址，工作区与大模型接口没有身份验证，请勿暴露到不可信网络。');
    }
    console.log('按 Ctrl+C 退出');
  })
  .catch((error) => {
    console.error('启动失败：', error.message);
    process.exitCode = 1;
  });
