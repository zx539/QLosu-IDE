/**
 * QLosu-standalone 自检脚本：验证 WASM 运行时与内置示例
 * 运行：npm test  或  node test.js
 */
const fs = require('fs');
const path = require('path');

const createQLosu = require('./wasm/qlosu.m.js');
const EXAMPLES_DIR = path.join(__dirname, 'examples');

let capture = '';
let failures = 0;

function check(name, cond, detail) {
  console.log(`${cond ? '✓' : '✗'} ${name}${cond ? '' : '  ← ' + detail}`);
  if (!cond) failures++;
}

async function main() {
  const Module = await createQLosu({
    wasmBinary: fs.readFileSync(path.join(__dirname, 'wasm', 'qlosu.m.wasm')),
    print: (t) => { capture += t + '\n'; },
    printErr: (t) => { capture += t + '\n'; },
  });

  // 1. 版本
  capture = '';
  const ver = Module.UTF8ToString(Module._qlosu_version());
  check('版本信息', ver.includes('QLosu 1.0.0'), ver);

  // 2. 基础语法
  capture = '';
  run(Module, 'var { println } import qufu.IO;\nprintln("hello", 1 + 1);');
  check('println 输出', capture.includes('hello 2'), capture.trim());

  // 3. 量子电路：H 门概率
  capture = '';
  run(Module, [
    'var { println } import qufu.IO;',
    'var QuReg import qufu.QuReg;',
    'var { Hadamard: H } import qufu.Gates;',
    'var reg = QuReg(1);',
    'var w = reg[[0]];',
    'w :> H();',
    'println("P0", w(0));',
    'println("P1", w(1));',
  ].join('\n'));
  check('H 门概率 0.5/0.5', capture.includes('P0 0.5') && capture.includes('P1 0.5'), capture.trim());

  // 4. Bell 纠缠态
  capture = '';
  run(Module, [
    'var { println } import qufu.IO;',
    'var QuReg import qufu.QuReg;',
    'var { Hadamard: H, ControlledPauliX: CNOT } import qufu.Gates;',
    'var { Measure: M } import qufu;',
    'var reg = QuReg(2);',
    'reg[[0]] :> H();',
    'reg[[0]] :> CNOT(reg[[1]]);',
    'println("P00", reg[[0,1]](0));',
    'println("M", M(reg[[0]]), M(reg[[1]]));',
  ].join('\n'));
  check('Bell 态 P(00)=0.5', capture.includes('P00 0.5'), capture.trim());

  // 5. 全部内置示例
  const files = fs.readdirSync(EXAMPLES_DIR).filter((f) => f.endsWith('.losu')).sort();
  console.log(`\n内置示例（${files.length} 个）：`);
  for (const f of files) {
    capture = '';
    const code = fs.readFileSync(path.join(EXAMPLES_DIR, f), 'utf8');
    const ret = run(Module, code);
    check(`  ${f}（ret=${ret}）`, ret === 0 && capture.length > 0, `ret=${ret} out=${capture.length}B`);
  }

  console.log(`\n${failures === 0 ? '全部通过 ✓' : failures + ' 项失败 ✗'}`);
  process.exit(failures === 0 ? 0 : 1);
}

function run(Module, code) {
  const len = Module.lengthBytesUTF8(code) + 1;
  const ptr = Module._malloc(len);
  Module.stringToUTF8(code, ptr, len);
  let ret;
  try {
    ret = Module._qlosu_run(ptr, len - 1);
  } catch (e) {
    ret = -1;
  }
  Module._free(ptr);
  return ret;
}

main().catch((e) => {
  console.error('FATAL:', e && e.message);
  process.exit(1);
});
