/**
 * QLosu WASM 运行时工作线程
 *
 * 在独立 worker 中加载 WASM 模块并串行执行脚本，主线程可在超时后
 * terminate() 本线程，避免用户代码死循环阻塞整个服务。
 */
const { parentPort, workerData } = require('worker_threads');
const fs = require('fs');
const path = require('path');

const createQLosu = require(path.join(workerData.wasmDir, 'qlosu.m.js'));
const MAX_OUTPUT = workerData.maxOutput || 8 * 1024 * 1024;

let buffer = '';
let truncated = false;
function capture(text) {
  if (truncated) return;
  if (buffer.length + text.length + 1 > MAX_OUTPUT) {
    truncated = true;
    return;
  }
  buffer += text + '\n';
}

const modulePromise = createQLosu({
  wasmBinary: fs.readFileSync(path.join(workerData.wasmDir, 'qlosu.m.wasm')),
  print: capture,
  printErr: capture,
});

parentPort.on('message', async (msg) => {
  try {
    const Module = await modulePromise;
    if (msg.type === 'version') {
      parentPort.postMessage({ id: msg.id, version: Module.UTF8ToString(Module._qlosu_version()) });
      return;
    }
    buffer = '';
    truncated = false;
    const len = Module.lengthBytesUTF8(msg.code) + 1;
    const ptr = Module._malloc(len);
    Module.stringToUTF8(msg.code, ptr, len);
    let ret;
    let crashed = false;
    try {
      ret = Module._qlosu_run(ptr, len - 1);
    } catch (e) {
      // WASM abort 后模块状态不可信，由主线程重建 worker
      ret = -1;
      crashed = true;
      capture('losu vm error: ' + String((e && e.message) || e));
    }
    if (!crashed) Module._free(ptr);
    parentPort.postMessage({ id: msg.id, ret, stdout: buffer, truncated, crashed });
  } catch (e) {
    parentPort.postMessage({ id: msg.id, error: String((e && e.message) || e) });
  }
});
