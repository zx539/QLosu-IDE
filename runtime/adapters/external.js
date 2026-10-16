const { spawn } = require('child_process');
const { RuntimeAdapter } = require('./base');

class ExternalRuntimeAdapter extends RuntimeAdapter {
  constructor({ id, label, command, args = [], versionArgs = ['--version'], runArgs, traceArgs, diagnoseArgs, env = {}, timeoutMs = 30000, cwd }) {
    super({ id, label: label || id, root: cwd });
    this.command = command;
    this.args = args;
    this.versionArgs = versionArgs;
    this.runArgs = runArgs || args;
    this.traceArgs = traceArgs || this.runArgs;
    this.diagnoseArgs = diagnoseArgs || this.runArgs;
    this.env = env;
    this.timeoutMs = timeoutMs;
    this.cwd = cwd;
  }

  async check() {
    try {
      const result = await this._exec(this.versionArgs, { timeoutMs: Math.min(this.timeoutMs, 5000) });
      const output = (result.stdout + result.stderr).trim();
      return { ok: result.code === 0, runnable: result.code === 0, version: output, message: result.timedOut ? '运行时版本检测超时' : output || `退出码 ${result.code}` };
    } catch { return { ok: false, runnable: false, message: `找不到运行时：${this.command}` }; }
  }

  _exec(args, { file, cwd = this.cwd, timeoutMs = this.timeoutMs } = {}) {
    return new Promise((resolve, reject) => {
      const workdir = cwd || process.cwd();
      const child = spawn(this.command, [...args, ...(file ? [file] : [])], { cwd: workdir, env: { ...process.env, ...this.env }, stdio: ['ignore', 'pipe', 'pipe'] });
      let stdout = ''; let stderr = '';
      let timedOut = false;
      let killTimer;
      const timer = setTimeout(() => { timedOut = true; child.kill('SIGTERM'); killTimer = setTimeout(() => child.kill('SIGKILL'), 1000); killTimer.unref(); }, timeoutMs);
      child.stdout.on('data', (b) => { stdout += b; }); child.stderr.on('data', (b) => { stderr += b; });
      child.on('error', (error) => { clearTimeout(timer); clearTimeout(killTimer); reject(error); });
      child.on('close', (code, signal) => { clearTimeout(timer); clearTimeout(killTimer); resolve({ code: code == null ? 1 : code, signal, stdout, stderr, timedOut }); });
    });
  }

  run(options = {}) { return this._exec(this.runArgs, options); }
  trace(options = {}) { return this._exec(this.traceArgs, options); }
  async diagnose(options = {}) {
    const result = await this._exec(this.diagnoseArgs, options);
    return { ...result, diagnostics: this.diagnosticsFromResult(result) };
  }
}

module.exports = { ExternalRuntimeAdapter };
