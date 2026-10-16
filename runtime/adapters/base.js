class RuntimeAdapter {
  constructor({ id, label, root }) {
    this.id = id;
    this.label = label;
    this.root = root;
  }

  info() { return { id: this.id, label: this.label }; }
  async check() { return { ok: false, message: '未实现运行时检查' }; }
  async build() { throw new Error(`${this.id} 不支持构建`); }
  async run() { throw new Error(`${this.id} 不支持运行`); }
  async diagnose() { return { diagnostics: [] }; }
  async trace() { return this.run(...arguments); }

  /** Convert a process result into LSP/Problems-friendly diagnostics. */
  diagnosticsFromResult(result) {
    const text = `${result.stderr || ''}\n${result.stdout || ''}`;
    return text.split(/\r?\n/).filter(Boolean).map((message) => ({
      severity: 'error', message, source: this.id,
    }));
  }
}

module.exports = { RuntimeAdapter };
