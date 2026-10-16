const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');

const { parseProgram } = require('../../../legacy/web-ide/www/js/core/losu-parse');
const { flowchartXml } = require('../../../legacy/web-ide/www/js/core/drawio');

const quickSort = `
函数 快速排序(arr, low, high)
  如果 low < high
    pivot = 分区(arr, low, high)
    对于 i 在 low, high - 1
      如果 arr[i] <= arr[high]
        如果 i == low
          输出 "pivot 起点"
        否则如果 arr[i] == pivot
          继续
        否则
          交换(arr, i, pivot)
        结束
      否则
        跳出
      结束
    结束
    快速排序(arr, low, pivot - 1)
    快速排序(arr, pivot + 1, high)
  否则如果 low == high
    返回 arr[low]
  否则
    返回 0
  结束
结束

函数 分区(arr, low, high)
  while low < high
    low = low + 1
  结束
  返回 low
结束

数据 = [9, 4, 7, 1, 3]
快速排序(数据, 0, 4)
输出 数据
`;

function count(source, pattern) {
  return (source.match(pattern) || []).length;
}

test('复杂排序算法保留多层控制流和源码行号', () => {
  const program = parseProgram(quickSort);
  assert.equal(program.functions.length, 2);
  assert.equal(program.functions[0].name, '快速排序');
  assert.equal(program.functions[0].body[0].type, 'if');
  assert.equal(program.functions[0].body[0].branches.length, 2, 'if + elif should remain separate branches');
  assert.equal(program.functions[0].body[0].else[0].type, 'return');

  const outer = program.functions[0].body[0];
  const loop = outer.then.find((node) => node.type === 'for');
  assert.ok(loop, 'nested for loop should be detected');
  const innerIf = loop.body.find((node) => node.type === 'if');
  assert.ok(innerIf, 'nested if should be detected');
  assert.equal(innerIf.branches.length, 1, 'outer nested condition should be detected');
  assert.equal(innerIf.then[0].type, 'if', 'third nesting level should be retained');
  assert.equal(innerIf.then[0].branches.length, 2, 'nested elif should be detected');
  assert.equal(innerIf.then[0].line, 7);
  assert.equal(loop.body[0].line, 6);
  assert.equal(program.main[0].line, 34);
});

test('复杂算法生成含泳道、判断、循环回边和调用节点的 draw.io XML', () => {
  const xml = flowchartXml(parseProgram(quickSort), { theme: 'dark', title: '快速排序控制流' });
  assert.match(xml, /快速排序/);
  assert.match(xml, /函数 分区/);
  assert.match(xml, /qv_line=/);
  assert.ok(count(xml, /rhombus/g) >= 4, 'nested conditions should render as diamond nodes');
  assert.ok(count(xml, /hexagon/g) >= 2, 'for/while and call nodes should render distinct shapes');
  assert.ok(count(xml, /dashed=1/g) >= 1, 'loop back edges should be dashed');
  assert.ok(count(xml, /label="continue"/g) >= 1, 'continue should be visible');
  assert.ok(count(xml, /label="break"/g) >= 1, 'break should be visible');
  assert.ok(xml.length > 10000, 'complex source should not collapse into a tiny graph');
});

test('比较表达式不会被误识别为赋值节点', () => {
  const program = parseProgram('如果 left == right\n  输出 left\n结束');
  assert.equal(program.main[0].type, 'if');
  assert.equal(program.main[0].condition, 'left == right');
  assert.equal(program.main[0].then[0].type, 'io');
});

test('文档中的复杂流程图样例可以被解析并绘制', () => {
  const source = fs.readFileSync(path.join(__dirname, '..', '..', '..', 'docs', 'flowchart-complex.losu'), 'utf8');
  const program = parseProgram(source);
  assert.equal(program.functions.length, 2);
  assert.match(flowchartXml(program, { title: '文档复杂流程图' }), /函数 快速排序/);
});

