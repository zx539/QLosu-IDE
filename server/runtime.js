/**
 * QLosu 运行时管理：worker 生命周期、串行队列与执行超时
 */
const { Worker } = require('worker_threads');
const path = require('path');

const WORKER_FILE = path.join(__dirname, 'runtime-worker.js');
const DEFAULT_WASM_DIR = path.join(__dirname, '..', 'wasm');

class Runtime {
  constructor({ wasmDir = DEFAULT_WASM_DIR, timeoutMs = 15000, maxOutput } = {}) {
    this.wasmDir = wasmDir;
    this.timeoutMs = timeoutMs;
    this.maxOutput = maxOutput;
    this.worker = null;
    this.pending = new Map();
    this.nextId = 1;
    // QuEST 环境与脚本输出缓冲区均为模块级单例，因此所有请求串行执行
    this.queue = Promise.resolve();
  }

  _ensureWorker() {
    if (this.worker) return this.worker;
    const worker = new Worker(WORKER_FILE, {
      workerData: { wasmDir: this.wasmDir, maxOutput: this.maxOutput },
    });
    worker.on('message', (msg) => {
      const job = this.pending.get(msg.id);
      if (!job) return;
      this.pending.delete(msg.id);
      clearTimeout(job.timer);
      if (msg.crashed) this._reset();
      if (msg.error) job.reject(new Error(msg.error));
      else job.resolve(msg);
    });
    // 仅处理当前 worker 的意外退出；被 _reset() 主动终止的旧 worker 不影响新任务
    worker.on('error', (err) => {
      if (this.worker === worker) this._failAll(err);
    });
    worker.on('exit', () => {
      if (this.worker !== worker) return;
      this.worker = null;
      this._failAll(new Error('运行时线程已退出'));
    });
    this.worker = worker;
    return worker;
  }

  _reset() {
    const worker = this.worker;
    this.worker = null;
    if (worker) worker.terminate().catch(() => {});
  }

  _failAll(err) {
    for (const [id, job] of this.pending) {
      clearTimeout(job.timer);
      job.reject(err);
      this.pending.delete(id);
    }
  }

  _request(payload, timeoutMs) {
    const job = this.queue.then(() => new Promise((resolve, reject) => {
      const id = this.nextId++;
      const worker = this._ensureWorker();
      const timer = setTimeout(() => {
        this.pending.delete(id);
        this._reset();
        resolve({ id, ret: -1, stdout: '', timedOut: true });
      }, timeoutMs);
      this.pending.set(id, { resolve, reject, timer });
      worker.postMessage({ ...payload, id });
    }));
    this.queue = job.catch(() => {});
    return job;
  }

  /** 执行脚本，返回 { ret, stdout, truncated, timedOut } */
  run(code, { timeoutMs = this.timeoutMs } = {}) {
    return this._request({ type: 'run', code }, timeoutMs);
  }

  async version() {
    const r = await this._request({ type: 'version' }, 30000);
    return r.version;
  }

  close() {
    this._reset();
  }
}

module.exports = { Runtime };
