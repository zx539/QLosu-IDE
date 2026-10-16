const assert = require('node:assert/strict');
const test = require('node:test');
const { diagramHtml } = require('../diagram');

test('simple circuit explains Bell preparation, measurements and all registers', () => {
  const html = diagramHtml('circuit', 'bell.losu', { trace: {
    registers: [{ id: 0, nQubits: 2 }, { id: 1, nQubits: 1 }],
    ops: [
      { reg: 0, gate: 'H', controls: [], targets: [0] },
      { reg: 0, gate: 'CX', controls: [0], targets: [1] },
      { reg: 0, gate: 'M', controls: [], targets: [1], result: 1 },
    ],
  } }, { viewer: '' });
  assert.match(html, /从左向右看/);
  assert.match(html, /第 2 步：受控翻转（CNOT） · 控制 q0 → q1/);
  assert.match(html, /M=1/);
  assert.match(html, /data-register="1" hidden/);
  assert.match(html, /<svg/);
  assert.doesNotMatch(html, /viewer-static|正在加载图表/);
});
