const fs = require('node:fs');
const path = require('node:path');
const { ExternalRuntimeAdapter } = require('./external');

class Losu1RuntimeAdapter extends ExternalRuntimeAdapter {
  constructor() {
    const root = path.join(__dirname, '..', 'losu1', `${process.platform}-${process.arch}`);
    super({ id: 'losu-1.0', label: '洛书 1.0 · lpk 原生解释器', command: path.join(root, process.platform === 'win32' ? 'losu.exe' : 'losu'), versionArgs: ['-v'] });
    this.backend = 'native';
  }
  async check() {
    if (!fs.existsSync(this.command)) return { ok: false, runnable: false, backend: this.backend, message: '尚未编译，请运行 npm run losu1:build' };
    const result = await this._exec(this.versionArgs, { timeoutMs: 5000 });
    return { ok: result.code === 0, runnable: result.code === 0, backend: this.backend, version: result.stdout.trim(), message: result.code === 0 ? 'Gitee lpk 原生运行时已就绪' : result.stderr || result.stdout };
  }
  async run(options) { return { ...await super.run(options), backend: this.backend }; }
  async diagnose() {
    return {
      code: 0,
      diagnostics: [],
      message: 'lpk 原生解释器没有独立的静态诊断入口；请查看语言服务诊断。',
      backend: this.backend,
    };
  }
  async trace() {
    const message = '洛书 1.0 lpk 未集成量子门追踪；量子示例请选择洛书 2.0（QLosu WASM）。';
    return { code: 1, stdout: '', stderr: message, diagnostics: [{ severity: 'error', message, source: this.id }], backend: this.backend };
  }
}
module.exports = { Losu1RuntimeAdapter };
