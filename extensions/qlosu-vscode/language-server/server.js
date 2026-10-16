#!/usr/bin/env node
/* Dependency-free LSP for Losu. It provides editor semantics while the native
 * parser is unavailable and keeps all offsets UTF-16 compatible with VS Code. */
const fs = require('fs');
const path = require('path');
let buffer = Buffer.alloc(0); let root = null;
const docs = new Map();
// Keep stdio server alive when launched by vscode-languageclient.
process.stdin.resume();
const languageSymbols = {
  control: {
    if: ['条件分支', 'if condition:\n  ...'],
    else: ['默认分支', 'else:\n  ...'],
    elif: ['多条件分支', 'elif condition:\n  ...'],
    pass: ['空操作', 'pass'],
    while: ['条件循环', 'while condition:\n  ...'],
    until: ['直到条件成立时停止的循环', 'until condition:\n  ...'],
    loop: ['重复循环，可与 to 配合', 'loop\n  ...\nto condition'],
    to: ['loop 循环的结束条件', 'to condition'],
    for: ['计次或列举循环', 'for item in values:\n  ...'],
    in: ['从容器中逐项取值', 'for item in values'],
    若: ['if 的中文别名，条件分支', '若 条件：\n  ...'],
    否则: ['else 的中文别名，默认分支', '否则：\n  ...'],
    当: ['while 的中文别名，条件循环', '当 条件：\n  ...'],
    直到: ['until 的中文别名', '直到 条件：\n  ...'],
    对于: ['for 的中文别名', '对于 项目 在 集合：\n  ...'],
  },
  function: {
    def: ['定义函数', 'def name(arg1, arg2):\n  return value'],
    函数: ['def 的中文别名，定义函数', '函数 名称(参数)：\n  返回 值'],
    func: ['定义函数', 'func name(args) { ... }'],
    function: ['定义函数', 'function name(args) { ... }'],
    return: ['从函数返回值', 'return value'],
    返回: ['return 的中文别名', '返回 值'],
    break: ['提前结束当前循环', 'break'],
    continue: ['跳过当前迭代', 'continue'],
    import: ['导入模块或标准库', 'import module'],
    导入: ['import 的中文别名', '导入 模块'],
    yield: ['从生成器产出一个值', 'yield value'],
  },
  declaration: {
    var: ['声明全局或局部变量', 'var name = value'],
    let: ['声明局部变量', 'let name = value'],
    const: ['声明不可重新绑定的变量', 'const name = value'],
    local: ['声明局部变量', 'local name = value'],
    block: ['创建块级作用域', 'block\n  ...\n;'],
    async: ['创建异步任务或线程', 'async function(args) { ... }'],
    await: ['等待异步任务完成', 'await task'],
    '$': ['变量声明前缀（旧版语法）', '$name = value'],
  },
  logical: {
    and: ['逻辑与', 'left and right'], or: ['逻辑或', 'left or right'], not: ['逻辑非', 'not value'], xor: ['逻辑异或', 'left xor right'],
    与: ['and 的中文别名', '左值 与 右值'], 或: ['or 的中文别名', '左值 或 右值'], 非: ['not 的中文别名', '非 值'],
  },
  constants: {
    True: ['布尔真值', 'True'], False: ['布尔假值', 'False'], true: ['布尔真值', 'true'], false: ['布尔假值', 'false'],
    null: ['空值', 'null'], nil: ['洛书空值（旧版别名）', 'nil'], this: ['当前对象引用', 'this'], self: ['当前对象引用（旧版别名）', 'self'], arg: ['可变参数集合', 'arg'], main: ['程序入口函数', 'def main()'],
    PI: ['圆周率常量', 'PI'], E: ['自然对数底数常量', 'E'],
  },
  types: {
    Float: ['统一数值类型，底层使用双精度浮点数', 'Float'], String: ['UTF-8 字符串类型', 'String'], Int: ['整数类型（兼容运行时）', 'Int'], Number: ['数值类型别名', 'Number'], Bool: ['布尔类型', 'Bool'], Boolean: ['布尔类型别名', 'Boolean'], Func: ['函数类型', 'Func'], Unit: ['洛书容器类型，可表示数组或键值表', 'Unit'], Resource: ['受运行时管理的资源类型', 'Resource'], Meta: ['描述类型的元类型', 'Meta'], Thread: ['异步线程类型', 'Thread'], QuReg: ['量子寄存器类型', 'QuReg'], Qubit: ['量子比特类型', 'Qubit'], QReg: ['量子寄存器类型别名', 'QReg'],
  },
  quantum: {
    H: ['Hadamard 门，将 |0> 与 |1> 置于叠加态', 'H(q)'], X: ['Pauli-X 门（比特翻转）', 'X(q)'], Y: ['Pauli-Y 门', 'Y(q)'], Z: ['Pauli-Z 门（相位翻转）', 'Z(q)'], S: ['S 相位门', 'S(q)'], T: ['T 相位门', 'T(q)'], I: ['恒等门', 'I(q)'], M: ['测量门', 'M(q)'], U: ['通用单量子比特门', 'U(theta, phi, lambda, q)'], Rx: ['绕 X 轴旋转', 'Rx(theta, q)'], Ry: ['绕 Y 轴旋转', 'Ry(theta, q)'], Rz: ['绕 Z 轴旋转', 'Rz(theta, q)'], CNOT: ['受控非门，参数为控制位和目标位', 'CNOT(control, target)'], CX: ['CNOT 的别名', 'CX(control, target)'], CZ: ['受控 Z 门', 'CZ(control, target)'], SWAP: ['交换两个量子比特', 'SWAP(a, b)'], Toffoli: ['双控制受控非门', 'Toffoli(a, b, target)'], measure: ['测量量子比特并写入经典寄存器', 'measure q[0] -> c[0]'], 测量: ['measure 的中文别名', '测量 q[0]'],
  },
  stdlib: {
    print: ['输出一个或多个值', 'print(value)'], println: ['输出并换行', 'println(value)'], input: ['读取用户输入', 'input(prompt)'], int: ['将值转换为整数', 'int(value)'], float: ['将值转换为浮点数', 'float(value)'], str: ['将值转换为字符串', 'str(value)'], string: ['str 的别名', 'string(value)'], len: ['返回容器或字符串长度', 'len(value)'], type: ['返回值的运行时类型', 'type(value)'], asc: ['返回字符的 ASCII/Unicode 数值', 'asc(character)'], chr: ['根据数值创建字符', 'chr(number)'], eval: ['求值表达式字符串', 'eval(expression)'], error: ['抛出运行时错误', 'error(message)'], assert: ['断言条件成立', 'assert(condition, message)'], range: ['创建数字范围', 'range(start, stop, step)'], map: ['映射函数到容器', 'map(function, values)'], filter: ['按条件筛选容器', 'filter(function, values)'], reduce: ['归约容器中的值', 'reduce(function, values)'],
  },
  math: {
    abs: ['绝对值', 'abs(value)'], sqrt: ['平方根', 'sqrt(value)'], cbrt: ['立方根', 'cbrt(value)'], pow: ['幂运算', 'pow(base, exponent)'], exp: ['自然指数', 'exp(value)'], log: ['自然对数', 'log(value)'], log2: ['以 2 为底的对数', 'log2(value)'], log10: ['以 10 为底的对数', 'log10(value)'], sin: ['正弦函数', 'sin(angle)'], cos: ['余弦函数', 'cos(angle)'], tan: ['正切函数', 'tan(angle)'], asin: ['反正弦函数', 'asin(value)'], acos: ['反余弦函数', 'acos(value)'], atan: ['反正切函数', 'atan(value)'], min: ['返回最小值', 'min(a, b)'], max: ['返回最大值', 'max(a, b)'], floor: ['向下取整', 'floor(value)'], ceil: ['向上取整', 'ceil(value)'], round: ['四舍五入', 'round(value)'],
  },
  wasm: {
    line: ['WASM 绘制直线', 'line(x1, y1, x2, y2)'], rect: ['WASM 绘制矩形', 'rect(x, y, width, height)'], circle: ['WASM 绘制圆', 'circle(x, y, radius)'], text: ['WASM 绘制文本', 'text(x, y, value)'], triangle: ['WASM 绘制三角形', 'triangle(...)'], speek: ['WASM 语音输出', 'speek(value)'], image: ['WASM 加载图像', 'image(path, x, y)'], sound: ['WASM 播放音频', 'sound(path)'], time: ['读取当前时间', 'time()'], canvas: ['获取绘图画布', 'canvas()'], color: ['创建颜色值', 'color(r, g, b, a)'],
  },
  ui: {
    init: ['初始化 UI', 'init(callback)'], draw: ['UI 绘制回调', 'draw(callback)'], click: ['点击事件回调', 'click(callback)'], mousedown: ['鼠标按下事件', 'mousedown(callback)'], mouseup: ['鼠标释放事件', 'mouseup(callback)'], mousemove: ['鼠标移动事件', 'mousemove(callback)'], keypress: ['键盘按键事件', 'keypress(callback)'], keydown: ['键盘按下事件', 'keydown(callback)'], keyup: ['键盘释放事件', 'keyup(callback)'], resize: ['窗口尺寸变化事件', 'resize(callback)'],
  },
};
const categoryOrder = ['control', 'function', 'declaration', 'logical', 'constants', 'types', 'quantum', 'stdlib', 'math', 'wasm', 'ui'];
const categoryLabels = { control: '控制流', function: '函数与模块', declaration: '声明与并发', logical: '逻辑运算', constants: '常量', types: '类型', quantum: '量子 API', stdlib: '标准库', math: '数学库', wasm: 'WASM API', ui: 'UI API' };
const symbolCatalog = new Map(categoryOrder.flatMap((category) => Object.entries(languageSymbols[category]).map(([name, [description, signature]]) => [name, { category, description, signature }])));
const keywords = categoryOrder.slice(0, 4).flatMap((category) => Object.keys(languageSymbols[category]));
const quantum = Object.keys(languageSymbols.quantum);
const reserved = new Set([...symbolCatalog.keys()]);
const completionSnippets = { def: ' ${1:name}(${2:args}):\n  ${0:pass}', 函数: ' ${1:名称}(${2:参数})：\n  ${0:pass}', if: ' ${1:condition}:\n  ${0:pass}', 若: ' ${1:条件}：\n  ${0:pass}', for: ' ${1:item} in ${2:values}:\n  ${0:pass}', 对于: ' ${1:项目} 在 ${2:集合}：\n  ${0:pass}', print: '(${1:value})', CNOT: '(${1:control}, ${2:target})', measure: ' ${1:q[0]} -> ${2:c[0]}' };
const send = (id, result, error) => { const msg = { jsonrpc: '2.0', id }; if (error) msg.error = error; else msg.result = result === undefined ? null : result; const body = Buffer.from(JSON.stringify(msg)); process.stdout.write(`Content-Length: ${body.length}\r\n\r\n`); process.stdout.write(body); };
const notify = (method, params) => { const body = Buffer.from(JSON.stringify({ jsonrpc: '2.0', method, params })); process.stdout.write(`Content-Length: ${body.length}\r\n\r\n`); process.stdout.write(body); };
function source(uri) { return docs.get(uri)?.text || ''; }
function offsets(text) { const result = [0]; for (let i = 0; i < text.length; i += 1) if (text[i] === '\n') result.push(i + 1); return result; }
function position(text, offset) { const lines = offsets(text); let lo = 0; let hi = lines.length; while (lo + 1 < hi) { const mid = (lo + hi) >> 1; if (lines[mid] <= offset) lo = mid; else hi = mid; } return { line: lo, character: offset - lines[lo] }; }
function offset(text, pos) { return (offsets(text)[pos.line] || text.length) + pos.character; }
function span(text, start, end) { return { start: position(text, start), end: position(text, end) }; }
function word(text, at) { const left = text.slice(0, at).match(/[\p{L}\p{N}_$]*$/u)?.[0] || ''; const right = text.slice(at).match(/^[\p{L}\p{N}_$]*/u)?.[0] || ''; return { name: left + right, start: at - left.length, end: at + right.length }; }
function maskNonCode(text) {
  const chars = [...text]; let state = 'code'; let quote = '';
  const mask = (index) => { if (chars[index] !== '\n' && chars[index] !== '\r') chars[index] = ' '; };
  for (let i = 0; i < chars.length; i += 1) {
    if (state === 'line') { if (chars[i] === '\n' || chars[i] === '\r') state = 'code'; else mask(i); continue; }
    if (state === 'block') { if (chars[i] === '*' && chars[i + 1] === '/') { mask(i); mask(i + 1); i += 1; state = 'code'; } else mask(i); continue; }
    if (state === 'triple') { if (chars[i] === quote && chars[i + 1] === quote && chars[i + 2] === quote) { mask(i); mask(i + 1); mask(i + 2); i += 2; state = 'code'; } else mask(i); continue; }
    if (state === 'string') { if (chars[i] === '\\') { mask(i); if (i + 1 < chars.length) mask(++i); } else if (chars[i] === quote) { mask(i); state = 'code'; } else mask(i); continue; }
    if ((chars[i] === '/' && chars[i + 1] === '/') || chars[i] === '#') { mask(i); if (chars[i] === '/') mask(++i); state = 'line'; continue; }
    if (chars[i] === '/' && chars[i + 1] === '*') { mask(i); mask(++i); state = 'block'; continue; }
    if ((chars[i] === "'" || chars[i] === '"') && chars[i + 1] === chars[i] && chars[i + 2] === chars[i]) { quote = chars[i]; mask(i); mask(i + 1); mask(i + 2); i += 2; state = 'triple'; continue; }
    if (chars[i] === "'" || chars[i] === '"' || chars[i] === '`') { quote = chars[i]; mask(i); state = 'string'; }
  }
  return chars.join('');
}
function parse(text) {
  const code = maskNonCode(text); const declarations = []; const declarationPattern = /\b(?:var|let|const|local|def|函数|func|function)\s+([\p{L}_$][\p{L}\p{N}_$]*)/gu; let match;
  while ((match = declarationPattern.exec(code))) { const start = match.index + match[0].length - match[1].length; declarations.push({ name: match[1], start, end: start + match[1].length, kind: /(?:def|函数|func|function)/u.test(match[0]) ? 12 : 13 }); }
  const declarationRanges = new Set(declarations.map((item) => `${item.start}:${item.end}`)); const references = []; const pattern = /[\p{L}_$][\p{L}\p{N}_$]*/gu;
  while ((match = pattern.exec(code))) if (!declarationRanges.has(`${match.index}:${match.index + match[0].length}`) && !reserved.has(match[0])) references.push({ name: match[0], start: match.index, end: match.index + match[0].length });
  return { declarations, references };
}
function diagnostics(uri) {
  const text = source(uri); const code = maskNonCode(text); const result = []; let cursor = 0; const stack = [];
  for (const line of code.split(/\r?\n/)) { for (let i = 0; i < line.length; i += 1) { const ch = line[i]; if ('([{'.includes(ch)) stack.push({ ch, at: cursor + i }); else if (')]}'.includes(ch)) { const expected = { ')': '(', ']': '[', '}': '{' }[ch]; const top = stack.pop(); if (!top || top.ch !== expected) result.push({ range: span(text, cursor + i, cursor + i + 1), severity: 1, code: 'LOSUBRACE', source: 'losu', message: `未匹配的符号 ${ch}` }); } } const trimmed = line.trim(); if (/^(?:若|if|elif|else|否则|对于|for|当|while|until|直到)\b/u.test(trimmed) && !/[{:}]$/.test(trimmed) && !/\b(?:in|do|to)\b/u.test(trimmed)) result.push({ range: span(text, cursor, cursor + line.length), severity: 2, code: 'LOSUCOLON', source: 'losu', message: '控制流语句需要以 :、{ 或 ： 结束' }); cursor += line.length + 1; }
  for (const item of stack) result.push({ range: span(text, item.at, item.at + 1), severity: 1, code: 'LOSUBRACE', source: 'losu', message: `未匹配的符号 ${item.ch}` });
  const version = text.match(/^\s*(?:\/\/|#)\s*losu[-_ ]version\s*[:=]\s*([\w.-]+)/im)?.[1] || docs.get(uri)?.config?.losuVersion || '2.0.0'; if (!/^\d+\.\d+(?:\.\d+)?$/.test(version) && !['1.x', 'legacy'].includes(version)) result.push({ range: span(text, 0, Math.min(1, text.length)), severity: 2, code: 'LOSUVERSION', source: 'losu', message: `未识别的洛书版本 ${version}` });
  return result;
}
function config() { if (!root) return {}; try { return JSON.parse(fs.readFileSync(path.join(root, 'qlosu.json'), 'utf8')); } catch { return {}; } }
function completion(uri, pos) {
  const text = source(uri); const cursor = offset(text, pos); const item = word(text, cursor); const prefix = item.name;
  const names = [...new Set([...parse(text).declarations.map((x) => x.name), ...symbolCatalog.keys()])];
  return {
    isIncomplete: false,
    items: names.filter((x) => !prefix || x.startsWith(prefix)).map((label) => {
      const info = symbolCatalog.get(label); const isFunction = info && ['function', 'stdlib', 'math', 'wasm', 'ui', 'quantum'].includes(info.category);
      const insertText = completionSnippets[label];
      return {
        label,
        kind: isFunction ? 3 : info?.category === 'types' ? 22 : info?.category === 'constants' ? 21 : 14,
        detail: info ? `${categoryLabels[info.category]} · ${info.signature}` : '当前文件声明',
        documentation: info ? { kind: 'markdown', value: info.description } : undefined,
        ...(insertText ? { insertText, insertTextFormat: 2 } : {}),
      };
    }),
  };
}
function symbols(uri) { const text = source(uri); return parse(text).declarations.map((x) => ({ name: x.name, kind: x.kind, range: span(text, x.start, x.end), selectionRange: span(text, x.start, x.end) })); }
function locations(uri, name) { const text = source(uri); const parsed = parse(text); return [...parsed.declarations, ...parsed.references.filter((x) => x.name === name)].filter((x) => x.name === name).map((x) => ({ uri, range: span(text, x.start, x.end) })); }
function format(uri) { const text = source(uri); let indent = 0; const result = text.split(/\r?\n/).map((line) => { const trimmed = line.trim(); if (/^[}\])]/.test(trimmed)) indent = Math.max(0, indent - 1); const out = trimmed ? `${'  '.repeat(indent)}${trimmed}` : ''; if (/[{[(]$/.test(trimmed) || /^(?:若|if|for|对于|当|while|else|否则)\b.*[{:]$/u.test(trimmed)) indent += 1; return out; }).join('\n'); return [{ range: span(text, 0, text.length), newText: result }]; }
function hover(uri, pos) {
  const text = source(uri); const item = word(text, offset(text, pos)); if (!item.name) return null;
  const info = symbolCatalog.get(item.name);
  if (!info) {
    const declaration = parse(text).declarations.find((entry) => entry.name === item.name);
    if (!declaration) return null;
    const kind = declaration.kind === 12 ? '函数' : '变量';
    return { contents: { kind: 'markdown', value: `### ${item.name}\n\n**${kind}** · 当前文件声明\n\n标识符：${item.name}` }, range: span(text, item.start, item.end) };
  }
  const version = docs.get(uri)?.config?.losuVersion || '2.0.0';
  const value = `### ${item.name}\n\n\`\`\`losu\n${info.signature}\n\`\`\`\n\n${info.description}\n\n**分类**：${categoryLabels[info.category]}  ·  **版本**：${version}`;
  return { contents: { kind: 'markdown', value }, range: span(text, item.start, item.end) };
}
const semanticTokenTypes = ['function', 'variable', 'keyword', 'type', 'number', 'string', 'comment', 'operator', 'constant', 'quantum', 'parameter'];
function semanticTokens(uri) {
  const text = source(uri); const code = maskNonCode(text); const parsed = parse(text); const declarations = new Map(parsed.declarations.map((entry) => [`${entry.start}:${entry.end}`, entry])); const tokens = [];
  const add = (start, end, tokenType) => { if (end > start) tokens.push({ start, end, tokenType }); };
  const wordPattern = /[\p{L}_$][\p{L}\p{N}_$]*/gu; let match;
  while ((match = wordPattern.exec(code))) {
    const name = match[0]; const start = match.index; const end = start + name.length; const declaration = declarations.get(`${start}:${end}`); const info = symbolCatalog.get(name);
    let tokenType = declaration ? (declaration.kind === 12 ? 0 : 1) : 1;
    if (info) {
      if (info.category === 'function' || info.category === 'stdlib' || info.category === 'math' || info.category === 'wasm' || info.category === 'ui') tokenType = 0;
      else if (info.category === 'types') tokenType = 3;
      else if (info.category === 'quantum') tokenType = 9;
      else if (info.category === 'constants') tokenType = 8;
      else if (info.category === 'control' || info.category === 'declaration' || info.category === 'logical') tokenType = 2;
    }
    add(start, end, tokenType);
  }
  const numberPattern = /\b(?:0[xX][0-9A-Fa-f]+|0[bB][01]+|(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?)\b/g;
  while ((match = numberPattern.exec(code))) add(match.index, match.index + match[0].length, 4);
  const operatorPattern = /\.\.|->\*?|=>|::|:>|~>|==|!=|<=|>=|&&|\|\||[+\-*/%=&|<>!?:~]/g;
  while ((match = operatorPattern.exec(code))) add(match.index, match.index + match[0].length, 7);
  tokens.sort((a, b) => a.start - b.start || a.end - b.end); const data = []; let lastLine = 0; let lastStart = 0;
  for (const token of tokens) { const point = position(text, token.start); data.push(point.line - lastLine, point.line === lastLine ? point.character - lastStart : point.character, token.end - token.start, token.tokenType, 0); lastLine = point.line; lastStart = point.character; }
  return { data };
}
function handle(message) {
  const { id, method, params = {} } = message;
  try {
    if (method === 'initialize') { root = params.rootUri ? decodeURIComponent(params.rootUri.replace(/^file:\/\//, '')) : params.rootPath || process.cwd(); config(); return send(id, { serverInfo: { name: 'QLosu Losu Language Server', version: '0.2.0' }, capabilities: { textDocumentSync: { openClose: true, change: 1, save: { includeText: true } }, completionProvider: { triggerCharacters: ['.', ':', '>', '('] }, hoverProvider: true, definitionProvider: true, referencesProvider: true, documentSymbolProvider: true, documentFormattingProvider: true, documentRangeFormattingProvider: true, renameProvider: { prepareProvider: true }, codeActionProvider: { codeActionKinds: ['quickfix'] }, foldingRangeProvider: true, semanticTokensProvider: { legend: { tokenTypes: semanticTokenTypes, tokenModifiers: [] }, full: true } } }); }
    if (method === 'initialized' || method === 'exit') return; if (method === 'shutdown') return send(id, null);
    if (method === 'textDocument/didOpen') { const d = params.textDocument; docs.set(d.uri, { text: d.text, version: d.version, config: config() }); notify('textDocument/publishDiagnostics', { uri: d.uri, diagnostics: diagnostics(d.uri) }); return; }
    if (method === 'textDocument/didChange') { const d = params.textDocument; const old = docs.get(d.uri) || {}; docs.set(d.uri, { ...old, text: params.contentChanges.at(-1)?.text || '', version: d.version }); notify('textDocument/publishDiagnostics', { uri: d.uri, diagnostics: diagnostics(d.uri) }); return; }
    if (method === 'textDocument/didSave') { notify('textDocument/publishDiagnostics', { uri: params.textDocument.uri, diagnostics: diagnostics(params.textDocument.uri) }); return; }
    if (method === 'textDocument/didClose') { docs.delete(params.textDocument.uri); return; }
    const uri = params.textDocument?.uri;
    if (method === 'textDocument/completion') return send(id, completion(uri, params.position));
    if (method === 'textDocument/hover') return send(id, hover(uri, params.position));
    if (method === 'textDocument/documentSymbol') return send(id, symbols(uri));
    if (method === 'textDocument/definition') { const text = source(uri); const item = word(text, offset(text, params.position)); const target = parse(text).declarations.find((x) => x.name === item.name); return send(id, target ? [{ uri, range: span(text, target.start, target.end) }] : []); }
    if (method === 'textDocument/references') { const text = source(uri); return send(id, locations(uri, word(text, offset(text, params.position)).name)); }
    if (method === 'textDocument/prepareRename') { const text = source(uri); const item = word(text, offset(text, params.position)); return send(id, item.name ? span(text, item.start, item.end) : null); }
    if (method === 'textDocument/rename') { const text = source(uri); const item = word(text, offset(text, params.position)); return send(id, { changes: { [uri]: locations(uri, item.name).map((x) => ({ range: x.range, newText: params.newName })) } }); }
    if (method === 'textDocument/formatting' || method === 'textDocument/rangeFormatting') return send(id, format(uri));
    if (method === 'textDocument/codeAction') { const fixes = (params.context?.diagnostics || []).filter((x) => x.code === 'LOSUCOLON').map((x) => ({ title: '添加 :', kind: 'quickfix', diagnostics: [x], edit: { changes: { [uri]: [{ range: { start: x.range.end, end: x.range.end }, newText: ':' }] } } })); return send(id, fixes); }
    if (method === 'textDocument/foldingRange') { const ranges = []; let start = null; source(uri).split(/\r?\n/).forEach((line, lineNo) => { if (/[{[]\s*$/.test(line) && start == null) start = lineNo; if (start != null && /[}\]]\s*$/.test(line) && lineNo > start) { ranges.push({ startLine: start, endLine: lineNo }); start = null; } }); return send(id, ranges); }
    if (method === 'textDocument/semanticTokens/full') return send(id, semanticTokens(uri));
    if (id !== undefined) send(id, null);
  } catch (error) { if (id !== undefined) send(id, null, { code: -32603, message: error.message }); else fs.writeSync(2, `${error.stack}\n`); }
}
process.stdin.on('data', (chunk) => { buffer = Buffer.concat([buffer, chunk]); while (true) { const split = buffer.indexOf('\r\n\r\n'); if (split < 0) break; const match = buffer.subarray(0, split).toString().match(/Content-Length:\s*(\d+)/i); if (!match) { buffer = buffer.subarray(split + 4); continue; } const length = Number(match[1]); const start = split + 4; if (buffer.length < start + length) break; try { handle(JSON.parse(buffer.subarray(start, start + length).toString())); } catch (error) { fs.writeSync(2, `${error.stack}\n`); } buffer = buffer.subarray(start + length); } });
