/**
 * draw.io（mxGraph）图生成
 *
 *   circuitXml(trace, regId)  执行轨迹 → 量子线路图
 *   flowchartXml(program)     源码结构 → 程序流程图（主流程 + 每个函数一个泳道）
 *
 * 生成的 XML 是未压缩的 mxfile，可直接在 draw.io / diagrams.net 中打开编辑。
 * 每个节点带有 `qv_line`（源码行号）或 `qv_step`（执行步骤）自定义属性，
 * IDE 用它们实现图与代码之间的联动跳转。
 */
(function (root, factory) {
  if (typeof module === 'object' && module.exports) module.exports = factory();
  else root.QDrawio = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  'use strict';

  function esc(s) {
    return String(s)
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;')
      .replace(/\n/g, '&#xa;');
  }

  class Builder {
    constructor() {
      this.cells = [];
      this.nextId = 2;
    }

    id() {
      return 'c' + this.nextId++;
    }

    vertex({ value = '', style, x, y, w, h, parent = '1', attrs = null, id }) {
      const cid = id || this.id();
      const geo = `<mxGeometry x="${round(x)}" y="${round(y)}" width="${round(w)}" height="${round(h)}" as="geometry"/>`;
      if (attrs) {
        const extra = Object.entries(attrs).map(([k, v]) => ` ${k}="${esc(v)}"`).join('');
        this.cells.push(`<object id="${cid}" label="${esc(value)}"${extra}><mxCell style="${style}" vertex="1" parent="${parent}">${geo}</mxCell></object>`);
      } else {
        this.cells.push(`<mxCell id="${cid}" value="${esc(value)}" style="${style}" vertex="1" parent="${parent}">${geo}</mxCell>`);
      }
      return cid;
    }

    edge({ source, target, value = '', style = '', parent = '1', points = null, from = null, to = null }) {
      const cid = this.id();
      let geo = '<mxGeometry relative="1" as="geometry">';
      if (from) geo += `<mxPoint x="${round(from[0])}" y="${round(from[1])}" as="sourcePoint"/>`;
      if (to) geo += `<mxPoint x="${round(to[0])}" y="${round(to[1])}" as="targetPoint"/>`;
      if (points && points.length) {
        geo += '<Array as="points">' + points.map((p) => `<mxPoint x="${round(p[0])}" y="${round(p[1])}"/>`).join('') + '</Array>';
      }
      geo += '</mxGeometry>';
      const s = source ? ` source="${source}"` : '';
      const t = target ? ` target="${target}"` : '';
      this.cells.push(`<mxCell id="${cid}" value="${esc(value)}" style="${style}" edge="1" parent="${parent}"${s}${t}>${geo}</mxCell>`);
      return cid;
    }

    xml(name) {
      const body = this.cells.join('');
      return '<mxfile host="QLosu-IDE" type="device">' +
        `<diagram id="qlosu" name="${esc(name)}">` +
        '<mxGraphModel dx="1200" dy="800" grid="0" gridSize="10" guides="1" tooltips="1" connect="1" arrows="1" fold="1" page="0" pageScale="1" math="0" shadow="0">' +
        `<root><mxCell id="0"/><mxCell id="1" parent="0"/>${body}</root>` +
        '</mxGraphModel></diagram></mxfile>';
    }
  }

  function round(v) {
    return Math.round(v * 100) / 100;
  }

  // ---------- 配色 ----------

  const PALETTES = {
    dark: {
      text: '#d7deea', dim: '#8a95a8', wire: '#5b6680', bg: '#161b24',
      H: ['#1e3a5f', '#6aa7ff'], X: ['#3d2a52', '#b38bff'], Z: ['#1f3d33', '#3fb950'], RX: ['#4a3518', '#e3a74b'],
      M: ['#3a2a2a', '#f08080'], ctrl: '#d7deea', lane: '#1c2230', laneStroke: '#2f3a4f',
      start: ['#1f3d33', '#3fb950'], end: ['#3a2a2a', '#f08080'], stmt: ['#1c2230', '#4a5878'],
      quantum: ['#2a2350', '#9a83f5'], io: ['#1e3a5f', '#6aa7ff'], decision: ['#3d3318', '#d2a03c'],
      loop: ['#1f3542', '#4fb7c9'], ret: ['#3a2a2a', '#f08080'],
    },
    light: {
      text: '#1f2633', dim: '#5f6b7f', wire: '#8a96aa', bg: '#ffffff',
      H: ['#dbe9ff', '#2f6fe4'], X: ['#efe4ff', '#7c3aed'], Z: ['#dcf5e4', '#1a7f37'], RX: ['#fff1d6', '#b7791f'],
      M: ['#ffe2e2', '#cf222e'], ctrl: '#1f2633', lane: '#f5f7fb', laneStroke: '#c9d2df',
      start: ['#dcf5e4', '#1a7f37'], end: ['#ffe2e2', '#cf222e'], stmt: ['#ffffff', '#8a96aa'],
      quantum: ['#efe4ff', '#7c3aed'], io: ['#dbe9ff', '#2f6fe4'], decision: ['#fff1d6', '#b7791f'],
      loop: ['#dff5f8', '#1b8ea3'], ret: ['#ffe2e2', '#cf222e'],
    },
  };

  function pal(theme) {
    return PALETTES[theme === 'light' ? 'light' : 'dark'];
  }

  function boxStyle([fill, stroke], P, extra = '') {
    return `rounded=1;whiteSpace=wrap;html=1;arcSize=18;fillColor=${fill};strokeColor=${stroke};fontColor=${P.text};fontSize=12;${extra}`;
  }

  // ---------- 量子线路图 ----------

  const GATE_LABEL = { H: 'H', X: 'X', Z: 'Z', RX: 'Rx', M: 'M', CX: 'X', CCX: 'X' };

  /**
   * 将门序列排列到时间片（列）上：每个门放在其涉及的所有比特都空闲的最早一列，
   * 跨越的比特范围（含中间比特）都视为被占用，避免控制线穿过其他门。
   */
  function layoutColumns(ops) {
    const free = [];
    const cols = [];
    for (const op of ops) {
      const qs = op.controls.concat(op.targets);
      const lo = Math.min(...qs);
      const hi = Math.max(...qs);
      let col = 0;
      for (let q = lo; q <= hi; q++) col = Math.max(col, free[q] || 0);
      for (let q = lo; q <= hi; q++) free[q] = col + 1;
      cols.push(col);
    }
    return cols;
  }

  function circuitXml(trace, regId, { theme = 'dark', maxOps = 400 } = {}) {
    const P = pal(theme);
    const reg = trace.registers.find((r) => r.id === regId);
    const b = new Builder();
    if (!reg) return b.xml('量子线路');
    const allOps = trace.ops.filter((o) => o.reg === regId);
    const ops = allOps.slice(0, maxOps);
    const n = reg.nQubits || Math.max(1, ...ops.flatMap((o) => o.controls.concat(o.targets)).map((q) => q + 1));
    const cols = layoutColumns(ops);
    const nCols = cols.length ? Math.max(...cols) + 1 : 0;

    const ROW = 56;
    const COL = 58;
    const X0 = 70;
    const Y0 = 30;
    const G = 36;
    const width = X0 + nCols * COL + 40;
    const yOf = (q) => Y0 + q * ROW;
    const xOf = (c) => X0 + c * COL + COL / 2;

    // 比特标签与导线
    for (let q = 0; q < n; q++) {
      b.vertex({
        value: `q${q}  |0⟩`,
        style: `text;html=1;align=right;verticalAlign=middle;fontColor=${P.dim};fontSize=12;fontFamily=monospace;`,
        x: 0, y: yOf(q) - 12, w: X0 - 10, h: 24,
      });
      b.edge({
        style: `endArrow=none;html=1;strokeColor=${P.wire};strokeWidth=1.5;`,
        from: [X0 - 4, yOf(q)], to: [width - 20, yOf(q)],
      });
    }

    // 每列的测量结果用于经典线标注
    ops.forEach((op, k) => {
      const cx = xOf(cols[k]);
      const attrs = { qv_step: op.step, tooltip: describe(op) };
      if (op.controls.length) {
        const qs = op.controls.concat(op.targets);
        const lo = Math.min(...qs);
        const hi = Math.max(...qs);
        b.edge({
          style: `endArrow=none;html=1;strokeColor=${P.ctrl};strokeWidth=1.5;`,
          from: [cx, yOf(lo)], to: [cx, yOf(hi)],
        });
        for (const c of op.controls) {
          b.vertex({
            style: `ellipse;html=1;fillColor=${P.ctrl};strokeColor=${P.ctrl};`,
            x: cx - 6, y: yOf(c) - 6, w: 12, h: 12, attrs,
          });
        }
        // 目标比特：⊕
        const t = op.targets[0];
        b.vertex({
          style: `shape=orEllipse;perimeter=ellipsePerimeter;html=1;fillColor=${P.bg};strokeColor=${P.ctrl};strokeWidth=1.5;`,
          x: cx - 13, y: yOf(t) - 13, w: 26, h: 26, attrs,
        });
        return;
      }
      const t = op.targets[0];
      const kind = op.gate === 'M' ? 'M' : op.gate;
      const colors = P[kind] || P.H;
      let label = GATE_LABEL[op.gate] || op.gate;
      if (op.gate === 'RX') label = `Rx<br><font style="font-size:9px">${trimNum(op.param)}</font>`;
      if (op.gate === 'M') label = `M<br><font style="font-size:9px">=${op.result}</font>`;
      b.vertex({
        value: label,
        style: boxStyle(colors, P, `fontStyle=1;strokeWidth=1.5;${op.gate === 'M' ? 'shape=mxgraph.basic.rect;' : ''}`),
        x: cx - G / 2, y: yOf(t) - G / 2, w: G, h: G, attrs,
      });
    });

    if (allOps.length > ops.length) {
      b.vertex({
        value: `…其余 ${allOps.length - ops.length} 个操作未绘制`,
        style: `text;html=1;fontColor=${P.dim};fontSize=12;`,
        x: X0, y: yOf(n) - 10, w: 300, h: 24,
      });
    }
    return b.xml(`量子线路 · 寄存器 #${regId}`);
  }

  function trimNum(x) {
    return String(Number(Number(x).toFixed(3)));
  }

  function describe(op) {
    const q = (x) => 'q' + x;
    if (op.gate === 'M') return `第 ${op.step} 步：测量 ${q(op.targets[0])} → ${op.result}`;
    if (op.gate === 'RX') return `第 ${op.step} 步：Rx(${trimNum(op.param)}) 作用于 ${q(op.targets[0])}`;
    if (op.controls.length) return `第 ${op.step} 步：${op.gate}，控制 ${op.controls.map(q).join(', ')}，目标 ${q(op.targets[0])}`;
    return `第 ${op.step} 步：${op.gate} 作用于 ${q(op.targets[0])}`;
  }

  // ---------- 程序流程图 ----------

  const NODE_W = 220;
  const MAX_LABEL = 46;

  function shortLabel(text) {
    const s = String(text).replace(/\s+/g, ' ').trim();
    return s.length > MAX_LABEL ? s.slice(0, MAX_LABEL - 1) + '…' : s;
  }

  /**
   * 递归布局语句序列。返回 { w, h, entry, exits }：
   *   entry  入口节点 id
   *   exits  [{ id, label }]，需要连接到下一个节点的出口
   * 坐标采用相对 (x, y) 的绝对定位，x 为本段中心线。
   */
  function layoutSeq(b, P, stmts, cx, y, parent, ctx) {
    let cur = null;
    let top = y;
    let maxHalf = NODE_W / 2;
    let first = null;
    const link = (exits, target) => {
      for (const e of exits) {
        b.edge({ source: e.id, target, value: e.label || '', parent, style: edgeStyle(P, e.style) });
      }
    };
    for (const s of stmts) {
      if (s.type === 'assert') continue;
      const r = layoutStmt(b, P, s, cx, top, parent, ctx);
      if (!r) continue;
      if (cur) link(cur, r.entry);
      if (!first) first = r.entry;
      cur = r.exits;
      top += r.h + 26;
      maxHalf = Math.max(maxHalf, r.half);
    }
    return { entry: first, exits: cur || [], h: Math.max(0, top - y - 26), half: maxHalf };
  }

  function edgeStyle(P, extra = '') {
    return `edgeStyle=orthogonalEdgeStyle;rounded=1;html=1;strokeColor=${P.wire};fontColor=${P.dim};fontSize=11;endArrow=block;endFill=1;endSize=5;${extra}`;
  }

  function layoutStmt(b, P, s, cx, y, parent, ctx) {
    const attrs = { qv_line: s.line, tooltip: `第 ${s.line} 行` };
    if (s.type === 'stmt' || s.type === 'return' || s.type === 'break' || s.type === 'continue') {
      let colors = P.stmt;
      let label = s.text;
      let extra = '';
      if (s.type === 'stmt' && s.kind === 'quantum') colors = P.quantum;
      if (s.type === 'stmt' && s.kind === 'io') {
        colors = P.io;
        extra = 'shape=parallelogram;perimeter=parallelogramPerimeter;fixedSize=1;size=12;';
      }
      if (s.type === 'stmt' && s.kind === 'import') colors = P.stmt;
      if (s.type === 'return') { colors = P.ret; label = 'return ' + s.text; }
      if (s.type === 'break' || s.type === 'continue') { colors = P.ret; label = s.type; }
      const h = 36;
      const id = b.vertex({
        value: shortLabel(label), style: boxStyle(colors, P, extra + 'fontFamily=monospace;fontSize=11;'),
        x: cx - NODE_W / 2, y, w: NODE_W, h, parent, attrs: { ...attrs, tooltip: `第 ${s.line} 行：${label}` },
      });
      // return / break / continue 不再连接后续语句
      if (s.type === 'return') {
        ctx.returns.push(id);
        return { entry: id, exits: [], h, half: NODE_W / 2 };
      }
      if (s.type === 'break' || s.type === 'continue') {
        (s.type === 'break' ? ctx.breaks : ctx.continues).push(id);
        return { entry: id, exits: [], h, half: NODE_W / 2 };
      }
      return { entry: id, exits: [{ id }], h, half: NODE_W / 2 };
    }

    if (s.type === 'if') return layoutIf(b, P, s, cx, y, parent, ctx);
    if (s.type === 'loop') return layoutLoop(b, P, s, cx, y, parent, ctx);
    return null;
  }

  function layoutIf(b, P, s, cx, y, parent, ctx) {
    // 将 if / else if / else 展开为嵌套的二分支判断
    const [head, ...rest] = s.branches;
    const DH = 54;
    const DW = 200;
    const dec = b.vertex({
      value: shortLabel(head.cond), style: boxStyle(P.decision, P, 'rhombus;fontFamily=monospace;fontSize=11;'),
      x: cx - DW / 2, y, w: DW, h: DH, parent, attrs: { qv_line: head.line, tooltip: `第 ${head.line} 行：if (${head.cond})` },
    });
    const elseStmts = rest.length
      ? [{ type: 'if', branches: rest, elseBody: s.elseBody, line: rest[0].line }]
      : s.elseBody || [];

    // 先测量两侧宽度
    const probe = (stmts) => measure(stmts);
    const lw = Math.max(NODE_W / 2, probe(head.body).half);
    const rw = Math.max(NODE_W / 2, probe(elseStmts).half);
    const gap = 30;
    const yBody = y + DH + 30;

    const exits = [];
    let h = DH;
    let leftHalf = DW / 2;
    let rightHalf = DW / 2;

    if (elseStmts.length) {
      const lcx = cx - gap / 2 - lw;
      const rcx = cx + gap / 2 + rw;
      const L = layoutSeq(b, P, head.body, lcx, yBody, parent, ctx);
      const R = layoutSeq(b, P, elseStmts, rcx, yBody, parent, ctx);
      if (L.entry) b.edge({ source: dec, target: L.entry, value: '是', parent, style: edgeStyle(P, 'exitX=0;exitY=0.5;entryX=0.5;entryY=0;') });
      else exits.push({ id: dec, label: '是' });
      if (R.entry) b.edge({ source: dec, target: R.entry, value: '否', parent, style: edgeStyle(P, 'exitX=1;exitY=0.5;entryX=0.5;entryY=0;') });
      else exits.push({ id: dec, label: '否' });
      exits.push(...L.exits, ...R.exits);
      h = DH + 30 + Math.max(L.h, R.h);
      leftHalf = gap / 2 + 2 * lw;
      rightHalf = gap / 2 + 2 * rw;
    } else {
      const L = layoutSeq(b, P, head.body, cx, yBody, parent, ctx);
      if (L.entry) {
        b.edge({ source: dec, target: L.entry, value: '是', parent, style: edgeStyle(P) });
        exits.push(...L.exits);
        h = DH + 30 + L.h;
      } else {
        exits.push({ id: dec, label: '是' });
      }
      // “否”分支从菱形右侧绕过
      exits.push({ id: dec, label: '否', style: 'exitX=1;exitY=0.5;' });
      leftHalf = Math.max(DW / 2, L.half);
      rightHalf = Math.max(DW / 2, L.half) + 30;
    }
    return { entry: dec, exits, h, half: Math.max(leftHalf, rightHalf) };
  }

  function layoutLoop(b, P, s, cx, y, parent, ctx) {
    const DH = 54;
    const DW = 220;
    const label = (s.kind === 'for' ? 'for ' : 'while ') + s.cond;
    const dec = b.vertex({
      value: shortLabel(label), style: boxStyle(P.loop, P, 'shape=hexagon;perimeter=hexagonPerimeter2;fixedSize=1;size=14;fontFamily=monospace;fontSize=11;'),
      x: cx - DW / 2, y, w: DW, h: DH, parent, attrs: { qv_line: s.line, tooltip: `第 ${s.line} 行：${label}` },
    });
    const inner = { returns: ctx.returns, breaks: [], continues: [] };
    const B = layoutSeq(b, P, s.body, cx, y + DH + 30, parent, inner);
    const half = Math.max(DW / 2, B.half);
    const backX = cx - half - 24;
    if (B.entry) {
      b.edge({ source: dec, target: B.entry, value: s.kind === 'for' ? '下一项' : '是', parent, style: edgeStyle(P) });
      for (const e of B.exits.concat(inner.continues.map((id) => ({ id })))) {
        // 回边绕到左侧回到循环头
        b.edge({ source: e.id, target: dec, value: e.label || '', parent, style: edgeStyle(P, `exitX=0;exitY=0.5;entryX=0;entryY=0.5;dashed=1;`), points: [[backX, y + DH / 2]] });
      }
    }
    const exits = [{ id: dec, label: s.kind === 'for' ? '结束' : '否', style: 'exitX=1;exitY=0.5;' }];
    for (const id of inner.breaks) exits.push({ id, label: 'break' });
    return { entry: dec, exits, h: DH + 30 + B.h, half: half + 30 };
  }

  /** 只计算尺寸不输出节点（用于 if 两侧布局） */
  function measure(stmts) {
    const fake = new Builder();
    const P = PALETTES.dark;
    return layoutSeq(fake, P, stmts, 0, 0, '1', { returns: [], breaks: [], continues: [] });
  }

  function flowchartXml(program, { theme = 'dark', title = '程序流程图' } = {}) {
    const P = pal(theme);
    const b = new Builder();
    const sections = [{ name: '主流程', params: [], body: program.main, line: 1, main: true }].concat(
      program.funcs.map((f) => ({ name: f.name, params: f.params, body: f.body, line: f.line })),
    );

    let x = 0;
    for (const sec of sections) {
      const m = measure(sec.body);
      const half = Math.max(NODE_W / 2 + 20, m.half + 30);
      const laneW = half * 2 + 20;
      const laneH = 44 + 34 + 26 + (m.entry ? m.h + 26 : 0) + 34 + 20;
      const header = sec.main ? sec.name : `函数 ${sec.name}(${sec.params.filter(Boolean).join(') (')})`;
      const lane = b.vertex({
        value: header,
        style: `swimlane;html=1;startSize=30;rounded=1;arcSize=6;fillColor=${P.lane};strokeColor=${P.laneStroke};fontColor=${P.text};fontStyle=1;fontSize=12;swimlaneFillColor=${P.lane};`,
        x, y: 0, w: laneW, h: laneH, attrs: { qv_line: sec.line, tooltip: `第 ${sec.line} 行` },
      });
      const cx = laneW / 2;
      const ctx = { returns: [], breaks: [], continues: [] };
      const start = b.vertex({
        value: sec.main ? '开始' : '进入',
        style: boxStyle(P.start, P, 'ellipse;fontStyle=1;'),
        x: cx - 50, y: 44, w: 100, h: 34, parent: lane,
      });
      const body = layoutSeq(b, P, sec.body, cx, 44 + 34 + 26, lane, ctx);
      const endY = 44 + 34 + 26 + (body.entry ? body.h + 26 : 0);
      const end = b.vertex({
        value: sec.main ? '结束' : '返回',
        style: boxStyle(P.end, P, 'ellipse;fontStyle=1;'),
        x: cx - 50, y: endY, w: 100, h: 34, parent: lane,
      });
      if (body.entry) {
        b.edge({ source: start, target: body.entry, parent: lane, style: edgeStyle(P) });
        for (const e of body.exits) b.edge({ source: e.id, target: end, value: e.label || '', parent: lane, style: edgeStyle(P, e.style) });
      } else {
        b.edge({ source: start, target: end, parent: lane, style: edgeStyle(P) });
      }
      for (const r of ctx.returns) b.edge({ source: r, target: end, parent: lane, style: edgeStyle(P, 'dashed=1;') });
      x += laneW + 40;
    }
    return b.xml(title);
  }

  return { circuitXml, flowchartXml, layoutColumns, esc };
});
