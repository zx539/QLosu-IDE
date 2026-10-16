#!/usr/bin/env node
// Build the unmodified Gitee lpk sources into a relocatable native runtime.
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

const root = path.resolve(__dirname, '..');
const source = path.join(root, 'src', 'losu-1.0');
const output = path.join(root, 'runtime', 'losu1', `${process.platform}-${process.arch}`);
const core = path.join(source, 'losu_core');
function run(args) {
  const result = spawnSync(process.env.CC || 'cc', args, { cwd: root, encoding: 'utf8' });
  if (result.status !== 0 || result.error) throw new Error(result.error?.message || result.stderr || `Compiler exited ${result.status}`);
}

function build() {
  if (process.platform !== 'linux') throw new Error('此构建入口目前支持 Linux；其他平台请在运行时列表配置原生 Losu。');
  if (!fs.existsSync(path.join(core, 'els.c'))) throw new Error('缺少源码：请将 https://gitee.com/chen-chaochen/lpk 克隆到 src/losu-1.0。');
  fs.mkdirSync(output, { recursive: true });
  fs.copyFileSync(path.join(source, 'LICENSE'), path.join(output, 'LICENSE'));
  // lpk predates GCC 14's conversion of these C99 warnings to errors.
  const flags = ['-O2', '-std=gnu99', '-DELS_CONF_OS_LINUX', '-fPIC', '-Wno-incompatible-pointer-types', '-Wno-int-conversion', '-Wno-implicit-function-declaration', '-I', core];
  const sources = fs.readdirSync(core).filter((n) => n.endsWith('.c')).map((n) => path.join(core, n));
  run([...flags, '-shared', ...sources, '-lm', '-ldl', '-o', path.join(output, 'libeasylosu.so')]);
  const modules = ['std', 'stdbyte', 'stdfile', 'stdmath', 'stdstring', 'stdsys', 'stdthread', 'stdtime', 'stdvm', 'stdlib'];
  for (const name of modules) {
    const platformSource = path.join(source, name, 'Ubuntu', `${name}.c`);
    const file = fs.existsSync(platformSource) ? platformSource : path.join(source, name, `${name}.c`);
    run([...flags, '-I', path.join(source, name), '-shared', file, '-L', output, '-leasylosu', '-Wl,-rpath,$ORIGIN', '-lm', '-ldl', '-pthread', '-o', path.join(output, `ElsLib_${name}.lsd`)]);
  }
  run([...flags, path.join(source, 'losu', 'losu.c'), '-L', output, '-leasylosu', '-Wl,-rpath,$ORIGIN', '-lm', '-ldl', '-rdynamic', '-o', path.join(output, 'losu')]);
  const version = spawnSync(path.join(output, 'losu'), ['-v'], { encoding: 'utf8', timeout: 5000 });
  if (version.status !== 0 || version.error) throw new Error(version.error?.message || version.stderr || version.stdout);
  const commit = spawnSync('git', ['-C', source, 'rev-parse', 'HEAD'], { encoding: 'utf8' });
  fs.writeFileSync(path.join(output, 'build.json'), JSON.stringify({ source: 'https://gitee.com/chen-chaochen/lpk', commit: commit.stdout?.trim(), version: version.stdout.trim(), platform: process.platform, arch: process.arch }, null, 2) + '\n');
  console.log(version.stdout.trim());
  console.log(`PASS 洛书 1.0 原生运行时：${output}`);
}
if (require.main === module) { try { build(); } catch (error) { console.error(error.message); process.exitCode = 1; } }
module.exports = { build, output };
