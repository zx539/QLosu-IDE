/**
 * 量子门调用插桩
 *
 * WASM 运行时内置的 qufu 框架无法在不重新编译的情况下修改，因此这里在用户脚本
 * 第 1 行前拼接一段（不含换行的）前导代码，包装 qufu 的 QuReg / Gates / Measure，
 * 每次调用后输出一行 `@@QV ...` 标记，再把脚本中的 `import qufu` 替换为对包装
 * 对象的引用。
 *
 * - 前导代码不含换行，因此报错行号与用户源码保持一致；
 * - 包装函数先调用原始门、再打印标记，原始门的类型断言先于包装逻辑执行，
 *   用户看到的错误信息与未插桩时一致；
 * - 运行时在每个原始门调用前后打印 <Gate>…</Gate> 状态块，标记行紧随其后，
 *   解析器据此把门参数与门执行后的量子态关联起来（见 www/js/trace.js）。
 */

const MARK = '@@QV';

// 单个门：field 为 qufu.Gates 中的字段名，name 为 trace 中的门名，
// outer 为外层（控制端）参数名，extra 为额外打印的参数表达式
function gate(field, name, { outer = '', extra = [] } = {}) {
  const head = outer ? `{ ${outer} => { input =>` : '{ => { input =>';
  const call = `__qv_g.${field}(${outer})`;
  const printed = ["input::['handle']", ...extra, "input::['io']::[0]"].join(', ');
  return `${field}: ${head} var __qv_o = input :> ${call}; __qv_p("${MARK} G ${name}", ${printed}); __qv_o }}`;
}

const GATES = [
  gate('Hadamard', 'H'),
  gate('PauliX', 'X'),
  gate('PauliZ', 'Z'),
  // CNOT：ctrl 为控制比特，input（管道左侧）为目标比特
  gate('ControlledPauliX', 'CX', { outer: 'ctrl', extra: ["ctrl::['io']::[0]"] }),
  gate('RotateX', 'RX', { outer: 'theta', extra: ['theta'] }),
  gate('Toffoli', 'CCX', { outer: 'ctrl', extra: ["ctrl::['io']::[0]", "ctrl::['io']::[1]"] }),
];

const PRELUDE = [
  'var __qv_q import qufu;',
  'var __qv_p = __qv_q.IO.println;',
  'var __qv_g = __qv_q.Gates;',
  'var __qv_qufu = {',
  'PI: __qv_q.PI, QuEnv: __qv_q.QuEnv, IO: __qv_q.IO,',
  `QuReg: { bits => var __qv_r = __qv_q.QuReg(bits); __qv_p("${MARK} R", __qv_r::['handle'], bits); __qv_r },`,
  `Measure: { wire => var __qv_m = __qv_q.Measure(wire); __qv_p("${MARK} M", wire::['handle'], wire::['io']::[0], __qv_m); __qv_m },`,
  `Gates: { ${GATES.join(', ')} }`,
  '};',
].join(' ') + ' ';

const IMPORT_RE = /\bimport(\s+)qufu\b/g;

/** 返回插桩后的脚本；脚本未引用 qufu 时原样返回 */
function instrument(code) {
  if (!/\bimport\s+qufu\b/.test(code)) return { code, instrumented: false };
  return { code: PRELUDE + code.replace(IMPORT_RE, '=$1__qv_qufu'), instrumented: true };
}

module.exports = { instrument, MARK, PRELUDE };
