const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');
const { RuntimeAdapter } = require('./base');
const { nodeExecutable } = require('../node');

class MakefileRuntimeAdapter extends RuntimeAdapter {
  constructor({ id = 'losu-2.0.0', label = '洛书 2.0.0 · QLosu WASM（量子）', root, cliPath }) {
    super({ id, label, root });
    this.makefile = path.join(root, 'src', 'test', 'makefile');
    this.cliPath = cliPath || path.join(root, '..', '..', 'scripts', 'losu2-cli.js');
    this.archive = path.join(root, 'src', 'test', 'liblosu-v2.a');
  }

  async check() {
    const exists = fs.existsSync(this.makefile);
    let dryRun = null;
    if (exists) {
      try { dryRun = await this._make(['-n', 'liblosu-v2.a']); } catch (error) { dryRun = { error: error.message }; }
    }
    const cli = fs.existsSync(this.cliPath);
    return {
      ok: exists,
      buildable: exists && !dryRun?.error,
      runnable: cli,
      backend: cli ? 'wasm' : null,
      message: exists
        ? (dryRun?.error ? `Makefile 存在但无法执行：${dryRun.error}` : (cli ? '洛书 2.0.0 C 源码可构建；运行入口使用 QLosu WASM 后端。' : '已找到洛书 2.0.0 Makefile，但未找到 CLI 入口。'))
        : '未找到洛书 2.0.0 Makefile',
    };
  }

  _make(extra = [], { cwd = path.dirname(this.makefile), timeoutMs = 120000 } = {}) {
    return new Promise((resolve, reject) => {
      const child = spawn('make', ['-f', path.basename(this.makefile), ...extra], { cwd, stdio: ['ignore', 'pipe', 'pipe'] });
      let stdout = ''; let stderr = '';
      const timer = setTimeout(() => child.kill('SIGTERM'), timeoutMs);
      child.stdout.on('data', (b) => { stdout += b; });
      child.stderr.on('data', (b) => { stderr += b; });
      child.on('error', (error) => { clearTimeout(timer); reject(error); });
      child.on('exit', (code, signal) => { clearTimeout(timer); resolve({ code: code == null ? 1 : code, signal, stdout, stderr, timedOut: signal === 'SIGTERM' }); });
    });
  }

  build(options = {}) { return this._make(['liblosu-v2.a'], options); }

  _cli(command, { file, cwd, timeoutMs = 120000 } = {}) {
    return new Promise((resolve, reject) => {
      const args = [this.cliPath, command];
      if (file) args.push(file);
      const child = spawn(nodeExecutable(), args, {
        cwd: cwd || process.cwd(),
        env: { ...process.env, QLOSU_RUNTIME_BACKEND: 'wasm' },
        stdio: ['ignore', 'pipe', 'pipe'],
      });
      let stdout = '';
      let stderr = '';
      let timedOut = false;
      const timer = setTimeout(() => { timedOut = true; child.kill('SIGTERM'); }, timeoutMs);
      child.stdout.on('data', (b) => { stdout += b; });
      child.stderr.on('data', (b) => { stderr += b; });
      child.on('error', (error) => { clearTimeout(timer); reject(error); });
      child.on('exit', (code, signal) => {
        clearTimeout(timer);
        resolve({ code: code == null ? 1 : code, signal, stdout, stderr, timedOut, backend: 'wasm' });
      });
    });
  }

  async run(options = {}) {
    return this._cli('run', options);
  }

  async trace(options = {}) {
    return this._cli('trace', options);
  }

  async diagnose(options = {}) {
    const result = await this._cli('check', options);
    return { ...result, diagnostics: result.code === 0 ? [] : this.diagnosticsFromResult(result) };
  }
}

module.exports = { MakefileRuntimeAdapter };
