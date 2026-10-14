/**
 * Losu 源码结构解析（用于生成程序流程图）
 *
 * 不是完整的编译器前端，只识别流程图需要的结构：
 *   顺序语句、if / else if / else、for、while、return、break / continue、
 *   以及顶层的函数（lambda）定义。表达式内部不做解析，原文作为节点标签。
 */
(function (root, factory) {
  if (typeof module === 'object' && module.exports) module.exports = factory();
  else root.LosuParse = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  'use strict';

  // ---------- 词法 ----------

  const PUNCT = ['...', '->?*', '->&', '->?', '->*', '=>', '->', ':>', '~>', '::', '..', '==', '!=', '<=', '>=', '**', '/%',
    '<<', '>>>', '>>', '&&', '||', '{', '}', '(', ')', '[', ']', ';', ',', '.', '=', '+', '-', '*', '/', '%', '<', '>',
    '!', '&', '|', '^', '~', ':', '?', '$', '@', '#'];

  function tokenize(src) {
    const toks = [];
    let i = 0;
    let line = 1;
    const n = src.length;
    while (i < n) {
      const c = src[i];
      if (c === '\n') {
        toks.push({ t: 'nl', v: '\n', line, start: i, end: i + 1 });
        line++;
        i++;
        continue;
      }
      if (c === ' ' || c === '\t' || c === '\r') { i++; continue; }
      if (c === '/' && src[i + 1] === '/') {
        while (i < n && src[i] !== '\n') i++;
        continue;
      }
      if (c === '/' && src[i + 1] === '*') {
        const end = src.indexOf('*/', i + 2);
        const stop = end < 0 ? n : end + 2;
        for (let k = i; k < stop; k++) if (src[k] === '\n') line++;
        i = stop;
        continue;
      }
      if (c === '"' || c === "'" || c === '`') {
        const start = i;
        const startLine = line;
        i++;
        while (i < n && src[i] !== c) {
          if (src[i] === '\\') i++;
          else if (src[i] === '\n') line++;
          i++;
        }
        i++;
        toks.push({ t: 'str', v: src.slice(start, i), line: startLine, start, end: i });
        continue;
      }
      if (/[0-9]/.test(c)) {
        const m = /^(0x[0-9a-fA-F]+|[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?)/.exec(src.slice(i, i + 64));
        toks.push({ t: 'num', v: m[0], line, start: i, end: i + m[0].length });
        i += m[0].length;
        continue;
      }
      if (/[A-Za-z_\u0080-￿]/.test(c)) {
        const start = i;
        while (i < n && /[A-Za-z0-9_\u0080-￿]/.test(src[i])) i++;
        toks.push({ t: 'id', v: src.slice(start, i), line, start, end: i });
        continue;
      }
      const p = PUNCT.find((s) => src.startsWith(s, i));
      const v = p || c;
      toks.push({ t: 'p', v, line, start: i, end: i + v.length });
      i += v.length;
    }
    return toks;
  }

  // ---------- 语句切分 ----------

  const OPEN = { '{': '}', '(': ')', '[': ']' };
  // 以这些记号结尾 / 开头时，换行不结束语句
  const CONT_END = new Set(['=', '=>', '->', ':>', '~>', '..', '+', '-', '*', '/', '%', ',', '(', '[', '{', '.', '::', '==', '!=',
    '<', '>', '<=', '>=', 'and', 'or', 'not', '&', '|', '^', 'import', 'var', 'in', '**']);
  const CONT_START = new Set([':>', '~>', '..', '.', '::', '->', ')', ']', '}', 'else', 'and', 'or', '=', '=>', ',', '*', '/',
    '+', '==', '!=', '->?', '->*', '->&', '->?*', '%', '**']);

  /** 匹配括号，返回与 toks[i] 对应的闭合括号下标 */
  function matchClose(toks, i) {
    const stack = [];
    for (let k = i; k < toks.length; k++) {
      const v = toks[k].t === 'p' ? toks[k].v : '';
      if (OPEN[v]) stack.push(OPEN[v]);
      else if (v === '}' || v === ')' || v === ']') {
        stack.pop();
        if (!stack.length) return k;
      }
    }
    return toks.length - 1;
  }

  /** 把记号序列切分为语句（每条语句为记号数组，不含分隔符和换行） */
  function splitStatements(toks) {
    const stmts = [];
    let cur = [];
    let depth = 0;
    const nextReal = (k) => {
      while (k < toks.length && toks[k].t === 'nl') k++;
      return toks[k];
    };
    for (let k = 0; k < toks.length; k++) {
      const tk = toks[k];
      if (tk.t === 'p' && OPEN[tk.v]) depth++;
      else if (tk.t === 'p' && (tk.v === '}' || tk.v === ')' || tk.v === ']')) depth = Math.max(0, depth - 1);
      if (depth === 0 && tk.t === 'p' && tk.v === ';') {
        if (cur.length) stmts.push(cur);
        cur = [];
        continue;
      }
      if (tk.t === 'nl') {
        if (depth === 0 && cur.length) {
          const last = cur[cur.length - 1];
          const nx = nextReal(k + 1);
          if (!CONT_END.has(last.v) && !(nx && CONT_START.has(nx.v))) {
            stmts.push(cur);
            cur = [];
          }
        }
        continue;
      }
      // `}` 闭合后紧跟新语句（同一行无分号），如 `if(a){...} println(...)`
      if (depth === 0 && cur.length && tk.t !== 'p' && cur[cur.length - 1].v === '}' && isBlockStmt(cur) && tk.v !== 'else') {
        stmts.push(cur);
        cur = [];
      }
      cur.push(tk);
    }
    if (cur.length) stmts.push(cur);
    return stmts;
  }

  function isBlockStmt(toks) {
    return toks.length && (toks[0].v === 'if' || toks[0].v === 'for' || toks[0].v === 'while');
  }

  // ---------- 语法结构 ----------

  function textOf(src, toks) {
    if (!toks.length) return '';
    return src.slice(toks[0].start, toks[toks.length - 1].end).replace(/\s+/g, ' ').trim();
  }

  /** 解析 `{ ... }` 块内部（toks 不含外层括号） */
  function parseBlock(src, toks) {
    return splitStatements(toks).map((s) => parseStatement(src, s)).filter(Boolean);
  }

  function inner(toks, open) {
    const close = matchClose(toks, open);
    return { body: toks.slice(open + 1, close), close };
  }

  function parseStatement(src, toks) {
    if (!toks.length) return null;
    const first = toks[0];
    const line = first.line;
    const kw = first.t === 'id' ? first.v : '';

    if (kw === 'if') return parseIf(src, toks);

    if ((kw === 'for' || kw === 'while') && toks[1] && toks[1].v === '(') {
      const head = inner(toks, 1);
      const bOpen = toks.findIndex((t, k) => k > head.close && t.v === '{');
      const body = bOpen >= 0 ? inner(toks, bOpen).body : [];
      return {
        type: 'loop',
        kind: kw,
        cond: textOf(src, head.body),
        body: parseBlock(src, body),
        line,
      };
    }

    if (kw === 'return') return { type: 'return', text: textOf(src, toks.slice(1)), line };
    if (kw === 'break' || kw === 'continue') return { type: kw, line };

    // var NAME = { params => ... }
    if (kw === 'var' && toks[1] && toks[1].t === 'id' && toks[2] && toks[2].v === '=' && toks[3] && toks[3].v === '{') {
      const close = matchClose(toks, 3);
      if (close === toks.length - 1) {
        const fn = parseLambda(src, toks.slice(4, close));
        if (fn) return { type: 'func', name: toks[1].v, params: fn.params, body: fn.body, line, endLine: toks[close].line };
      }
    }

    const text = textOf(src, toks);
    // 类型断言（如 `input -> \`Unit<...>\``）在流程图中不单独成框
    if (toks.length >= 3 && toks[0].t === 'id' && toks[1].v === '->' && !toks.some((t) => t.v === '=' || t.v === '(')) {
      return { type: 'assert', text, line };
    }
    return { type: 'stmt', text, kind: classify(toks, text), line };
  }

  function parseIf(src, toks) {
    const branches = [];
    let k = 0;
    let elseBody = null;
    while (k < toks.length) {
      if (toks[k].v === 'if' && toks[k + 1] && toks[k + 1].v === '(') {
        const head = inner(toks, k + 1);
        const bOpen = toks.findIndex((t, j) => j > head.close && t.v === '{');
        if (bOpen < 0) break;
        const blk = inner(toks, bOpen);
        branches.push({ cond: textOf(src, head.body), body: parseBlock(src, blk.body), line: toks[k].line });
        k = blk.close + 1;
        if (toks[k] && toks[k].v === 'else') {
          if (toks[k + 1] && toks[k + 1].v === 'if') { k++; continue; }
          if (toks[k + 1] && toks[k + 1].v === '{') {
            elseBody = parseBlock(src, inner(toks, k + 1).body);
          }
        }
        break;
      }
      break;
    }
    if (!branches.length) return { type: 'stmt', text: textOf(src, toks), kind: 'plain', line: toks[0].line };
    return { type: 'if', branches, elseBody, line: toks[0].line };
  }

  /** 解析 lambda 体：`params => body`，支持柯里化 `ctrl => { input => ... }` */
  function parseLambda(src, toks) {
    const arrow = topLevelIndex(toks, '=>');
    if (arrow < 0) return null;
    const params = [toks.slice(0, arrow).map((t) => t.v).join('').replace(/,/g, ', ')];
    let rest = toks.slice(arrow + 1);
    // 去掉 rest 两端的换行
    while (rest.length && rest[0].t === 'nl') rest = rest.slice(1);
    // 柯里化：rest 是单个 `{ x => ... }`
    if (rest.length && rest[0].v === '{' && matchClose(rest, 0) === lastReal(rest)) {
      const sub = parseLambda(src, rest.slice(1, matchClose(rest, 0)));
      if (sub) return { params: params.concat(sub.params), body: sub.body };
    }
    return { params, body: parseBlock(src, rest) };
  }

  function lastReal(toks) {
    let k = toks.length - 1;
    while (k >= 0 && (toks[k].t === 'nl' || toks[k].v === ';')) k--;
    return k;
  }

  function topLevelIndex(toks, v) {
    let depth = 0;
    for (let k = 0; k < toks.length; k++) {
      const tv = toks[k].t === 'p' ? toks[k].v : '';
      if (OPEN[tv]) depth++;
      else if (tv === '}' || tv === ')' || tv === ']') depth--;
      else if (depth === 0 && tv === v) return k;
    }
    return -1;
  }

  function classify(toks, text) {
    if (toks.some((t) => t.v === 'import')) return 'import';
    if (/\bprintln\s*\(|\berrprintln\s*\(|\bwarnprintln\s*\(|\bprint\s*\(/.test(text)) return 'io';
    if (toks.some((t) => t.v === ':>') || /\bQuReg\s*\(|\bM\s*\(|\bMeasure\s*\(/.test(text)) return 'quantum';
    return 'plain';
  }

  /** 解析整个程序：返回 { main: Stmt[], funcs: Func[] } */
  function parseProgram(src) {
    const toks = tokenize(String(src || ''));
    const all = parseBlock(src, toks);
    const funcs = all.filter((s) => s.type === 'func');
    const main = all.filter((s) => s.type !== 'func');
    return { main, funcs };
  }

  return { tokenize, splitStatements, parseProgram };
});
