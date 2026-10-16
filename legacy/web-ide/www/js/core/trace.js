/**
 * 运行输出解析：把 WASM 运行时的标准输出拆分为
 *   - 用户输出（println 等）
 *   - 量子执行轨迹（寄存器、门、测量及每一步之后的量子态）
 *   - 错误信息（含源码行号）
 *
 * 输出格式说明：
 *   运行时在每个原始门调用前后打印 `<Gate>` + 两次 reportQureg + `</Gate>`；
 *   插桩前导代码（server/instrument.js）在门调用完成后打印 `@@QV G ...`，
 *   因此 `@@QV` 行总是紧跟在对应的状态块之后。
 */
(function (root, factory) {
  if (typeof module === 'object' && module.exports) module.exports = factory();
  else root.QTrace = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  'use strict';

  const HANDLE_RE = /\(Resource<[^>]*>\s+(0x[0-9a-fA-F]+)\)/;
  const OPEN_RE = /^<(Qureg|Hadamard|apply\w+|Measure)(?: (\d+))?>$/;
  const CLOSE_RE = /^<\/(Qureg|Hadamard|apply\w+|Measure)(?: (\d+) = (\d+))?>$/;
  const REPORT_RE = /^Qureg \((\d+) qubit (statevector|density matrix)/;
  const AMP_RE = /^\s+(\S+)\s+\|(\d+)⟩\s*$/;
  const ELLIPSIS_RE = /^\s+⋮\s*$/;

  /** 解析 QuEST 打印的复数，如 0.5、-0.5i、0.60283-0.3696i、1e-05+2e-05i */
  function parseComplex(s) {
    s = s.trim();
    if (!s.endsWith('i')) return { re: Number(s) || 0, im: 0 };
    const body = s.slice(0, -1);
    let pos = -1;
    for (let i = body.length - 1; i > 0; i--) {
      const ch = body[i];
      if ((ch === '+' || ch === '-') && body[i - 1] !== 'e' && body[i - 1] !== 'E') {
        pos = i;
        break;
      }
    }
    const num = (t) => (t === '' || t === '+' ? 1 : t === '-' ? -1 : Number(t) || 0);
    if (pos < 0) return { re: 0, im: num(body) };
    return { re: Number(body.slice(0, pos)) || 0, im: num(body.slice(pos)) };
  }

  /** 从 lines[i] 开始解析一个 reportQureg 块，返回 { state, next } */
  function readReport(lines, i) {
    const m = REPORT_RE.exec(lines[i]);
    const state = { nQubits: Number(m[1]), kind: m[2], amps: [], partial: false };
    i++;
    for (; i < lines.length; i++) {
      const line = lines[i];
      const a = AMP_RE.exec(line);
      if (a) {
        const c = parseComplex(a[1]);
        state.amps.push({ index: Number(a[2]), re: c.re, im: c.im });
      } else if (ELLIPSIS_RE.test(line)) {
        state.partial = true;
      } else {
        break;
      }
    }
    // reportQureg 以一个空行结尾
    if (i < lines.length && lines[i] === '') i++;
    return { state, next: i };
  }

  function parseMarker(line) {
    // @@QV G CX (Resource<...> 0x1) 0 1  |  @@QV R (Resource<...> 0x1) 3  |  @@QV M (Resource<...> 0x1) 0 1
    const hm = HANDLE_RE.exec(line);
    if (!hm) return null;
    const head = line.slice(5, hm.index).trim().split(/\s+/);
    const tail = line.slice(hm.index + hm[0].length).trim();
    const nums = tail ? tail.split(/\s+/).map(Number) : [];
    return { kind: head[0], gate: head[1] || '', handle: hm[1], nums };
  }

  function errorInfo(lines) {
    const errors = [];
    for (let i = 0; i < lines.length; i++) {
      const m = /^losu (syntax|vm|runtime) error: (.*)$/.exec(lines[i]);
      if (!m) continue;
      const err = { type: m[1], message: m[2], line: null, frames: [] };
      for (let j = i + 1; j < lines.length && /^\t|^stack trace:/.test(lines[j]); j++) {
        const sm = /line: (\d+), file: <qlosu-web>/.exec(lines[j]);
        if (sm && err.line == null) err.line = Number(sm[1]);
        const fm = /file: "([^"]+)", line: (\d+)/.exec(lines[j]);
        if (fm) err.frames.push({ file: fm[1], line: Number(fm[2]) });
      }
      if (err.line == null) {
        // 栈帧由内向外；插桩前导代码位于第 1 行，优先取第一个非第 1 行的用户帧
        const user = err.frames.filter((f) => f.file === '<qlosu-web>');
        const pick = user.find((f) => f.line !== 1) || user[0];
        if (pick) err.line = pick.line;
      }
      errors.push(err);
    }
    return errors;
  }

  /**
   * 解析一次运行的完整输出
   * @returns {{ stdout: string, log: Array<{kind,text}>, registers, ops, errors }}
   */
  function parse(raw) {
    const lines = String(raw || '').replace(/\r/g, '').split('\n');
    if (lines.length && lines[lines.length - 1] === '') lines.pop();

    const registers = [];
    const byHandle = new Map();
    const ops = [];
    const log = [];
    const user = [];
    let block = null;
    let last = null;
    let lastReport = null;

    const regOf = (handle, bits) => {
      let reg = byHandle.get(handle);
      if (!reg) {
        reg = { id: registers.length, handle, nQubits: bits || 0, opCount: 0, initial: null };
        registers.push(reg);
        byHandle.set(handle, reg);
      }
      return reg;
    };

    for (let i = 0; i < lines.length;) {
      const line = lines[i];
      const open = OPEN_RE.exec(line);
      if (open) {
        block = { tag: open[1], states: [] };
        log.push({ kind: 'tag', text: line });
        i++;
        continue;
      }
      const close = CLOSE_RE.exec(line);
      if (close && block) {
        last = { tag: block.tag, before: block.states[0], after: block.states[block.states.length - 1] };
        if (close[3] != null) last.result = Number(close[3]);
        block = null;
        log.push({ kind: 'tag', text: line });
        i++;
        continue;
      }
      if (REPORT_RE.test(line)) {
        const start = i;
        const { state, next } = readReport(lines, i);
        if (block) block.states.push(state);
        else lastReport = state;
        for (let k = start; k < next; k++) log.push({ kind: 'state', text: lines[k] });
        i = next;
        continue;
      }
      if (line.startsWith('@@QV ')) {
        const mk = parseMarker(line);
        if (mk) {
          if (mk.kind === 'R') {
            // 同一地址可能在前一个寄存器释放后被复用，因此每次创建都登记为新寄存器
            byHandle.delete(mk.handle);
            const reg = regOf(mk.handle, mk.nums[0]);
            reg.initial = (last && last.after) || lastReport || null;
          } else if (mk.kind === 'G' || mk.kind === 'M') {
            const reg = regOf(mk.handle, last && last.after ? last.after.nQubits : 0);
            if (!reg.nQubits && last && last.after) reg.nQubits = last.after.nQubits;
            const op = { step: ops.length, reg: reg.id, state: last ? last.after : null, before: last ? last.before : null };
            if (mk.kind === 'M') {
              op.gate = 'M';
              op.targets = [mk.nums[0]];
              op.controls = [];
              op.result = mk.nums[1];
            } else {
              op.gate = mk.gate;
              const n = mk.nums;
              if (mk.gate === 'CX') {
                op.controls = [n[0]];
                op.targets = [n[1]];
              } else if (mk.gate === 'CCX') {
                op.controls = [n[0], n[1]];
                op.targets = [n[2]];
              } else if (mk.gate === 'RX') {
                op.param = n[0];
                op.targets = [n[1]];
                op.controls = [];
              } else {
                op.targets = [n[0]];
                op.controls = [];
              }
            }
            reg.opCount++;
            ops.push(op);
          }
        }
        last = null;
        i++;
        continue;
      }
      user.push(line);
      log.push({ kind: /^losu \w+ error:|^stack trace:|^\t/.test(line) ? 'error' : 'out', text: line });
      i++;
    }

    return {
      stdout: user.join('\n'),
      log,
      registers,
      ops,
      errors: errorInfo(lines),
    };
  }

  // ---------- 量子态工具 ----------

  function prob(a) {
    return a.re * a.re + a.im * a.im;
  }

  function ket(index, n) {
    return '|' + index.toString(2).padStart(Math.max(n, 1), '0') + '⟩';
  }

  function fmtNum(x) {
    if (Math.abs(x) < 5e-6) return '0';
    return String(Number(x.toFixed(5)));
  }

  function fmtComplex(a) {
    const re = fmtNum(a.re);
    const im = fmtNum(a.im);
    if (im === '0') return re;
    const imPart = (im === '1' ? '' : im === '-1' ? '-' : im) + 'i';
    if (re === '0') return imPart;
    return re + (a.im >= 0 ? '+' : '') + imPart;
  }

  /** 每个量子比特测得 1 的边缘概率（状态被截断时为近似值） */
  function marginals(state) {
    const p1 = new Array(state.nQubits).fill(0);
    for (const a of state.amps) {
      const p = prob(a);
      for (let q = 0; q < state.nQubits; q++) if ((a.index >> q) & 1) p1[q] += p;
    }
    return p1;
  }

  /** 概率不为 0 的基态，按下标排序 */
  function nonZero(state, eps = 1e-9) {
    return state.amps.filter((a) => prob(a) > eps).map((a) => ({ ...a, p: prob(a) }));
  }

  /** 生成适合给大模型阅读的执行摘要 */
  function summarize(trace, { maxOps = 80 } = {}) {
    const lines = [];
    for (const reg of trace.registers) {
      lines.push(`寄存器 #${reg.id}: ${reg.nQubits} qubit，${reg.opCount} 个操作`);
    }
    const shown = trace.ops.slice(0, maxOps);
    for (const op of shown) {
      lines.push('  ' + describeOp(op));
    }
    if (trace.ops.length > maxOps) lines.push(`  ……共 ${trace.ops.length} 个操作，已省略其余部分`);
    const finals = new Map();
    for (const op of trace.ops) if (op.state) finals.set(op.reg, op.state);
    for (const [reg, st] of finals) {
      const nz = nonZero(st).slice(0, 16)
        .map((a) => `${ket(a.index, st.nQubits)}:${fmtComplex(a)} (p=${fmtNum(a.p)})`);
      lines.push(`寄存器 #${reg} 最终态${st.partial ? '（部分）' : ''}: ${nz.join(', ')}`);
    }
    return lines.join('\n');
  }

  function describeOp(op) {
    const q = (x) => 'q' + x;
    let s = `#${op.step} [reg${op.reg}] `;
    if (op.gate === 'M') s += `测量 ${q(op.targets[0])} = ${op.result}`;
    else if (op.gate === 'RX') s += `RX(${fmtNum(op.param)}) ${q(op.targets[0])}`;
    else if (op.controls.length) s += `${op.gate} 控制 ${op.controls.map(q).join(',')} → 目标 ${q(op.targets[0])}`;
    else s += `${op.gate} ${q(op.targets[0])}`;
    return s;
  }

  return { parse, parseComplex, prob, ket, fmtNum, fmtComplex, marginals, nonZero, summarize, describeOp };
});
