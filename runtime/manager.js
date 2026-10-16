const path = require('path');
const { MakefileRuntimeAdapter } = require('./adapters/makefile');
const { ExternalRuntimeAdapter } = require('./adapters/external');
const { Losu1RuntimeAdapter } = require('./adapters/losu1');

function readConfig(workspace) {
  try {
    const file = path.join(workspace, 'qlosu.json');
    return require('fs').existsSync(file) ? JSON.parse(require('fs').readFileSync(file, 'utf8')) : {};
  } catch { return {}; }
}

function createAdapters({ workspace, config = readConfig(workspace) }) {
  const adapters = new Map();
  adapters.set('losu-1.0', new Losu1RuntimeAdapter());
  const bundled = path.join(__dirname, '..', 'src', 'losu v2.0.0');
  adapters.set('losu-2.0.0', new MakefileRuntimeAdapter({ root: bundled }));
  for (const runtime of config.runtimes || []) {
    if (runtime.id && runtime.command) adapters.set(runtime.id, new ExternalRuntimeAdapter({ ...runtime, cwd: runtime.cwd ? path.resolve(workspace, runtime.cwd) : undefined }));
  }
  return adapters;
}

function runtimeId(version, config = {}) {
  const requested = version || config.losuVersion || config.runtimeVersion || '2.0.0';
  if (['1.0', '1.0.0', 'losu-1.0', 'losu-1.0.0'].includes(requested)) return 'losu-1.0';
  if (requested === '2.0.0' || requested === 'losu-2.0.0') return 'losu-2.0.0';
  return requested;
}

function selectAdapter(options = {}) {
  const config = options.config || readConfig(options.workspace || process.cwd());
  const adapters = createAdapters({ ...options, config });
  const id = runtimeId(options.version, config);
  const adapter = adapters.get(id);
  if (!adapter) {
    const error = new Error(`未找到洛书运行时 ${id}，请在 qlosu.json 的 runtimes 中配置该版本。`);
    error.code = 'RUNTIME_NOT_FOUND';
    throw error;
  }
  return adapter;
}

async function listVersions(options) {
  const adapters = createAdapters(options);
  return Promise.all([...adapters.values()].map(async (adapter) => ({ ...adapter.info(), ...(await adapter.check()) })));
}

module.exports = { createAdapters, listVersions, readConfig, runtimeId, selectAdapter };
