// QLosu IDE - Losu 结构解析器
//
// 这是流程图和编辑器预览使用的轻量解析器，不承担运行时语义检查。
// 它同时接受洛书文档中的缩进风格和 C/Python 混合的大括号风格，
// 保留源码行号，让图中的节点可以回到编辑器对应位置。
(function () {
  'use strict';

  function indentOf(value) {
    const match = String(value).match(/^[ \t]*/);
    return (match ? match[0] : '').replace(/\t/g, '    ').length;
  }

  function stripComment(value) {
    let quote = '';
    let escaped = false;
    for (let i = 0; i < value.length; i += 1) {
      const ch = value[i];
      if (escaped) {
        escaped = false;
        continue;
      }
      if (ch === '\\' && quote) {
        escaped = true;
        continue;
      }
      if ((ch === '"' || ch === "'") && (!quote || quote === ch)) {
        quote = quote ? '' : ch;
        continue;
      }
      if (!quote && ch === '#') return value.slice(0, i);
      if (!quote && ch === '/' && value[i + 1] === '/') return value.slice(0, i);
    }
    return value;
  }

  function hasTrailingBrace(value) {
    return /\{\s*;?$/.test(value);
  }

  function tokenize(code) {
    return String(code || '').split(/\r?\n/).map((raw, index) => {
      const clean = stripComment(raw);
      const indent = indentOf(clean);
      let text = clean.trim().replace(/;\s*$/, '').trim();
      let closeCount = 0;

      // `} else {` and `}} else {` are common in the reference Losu examples.
      // Keep the close marker on the same token so the parent block can consume
      // it before handling the following else/elif branch.
      while (text.startsWith('}')) {
        closeCount += 1;
        text = text.slice(1).trim();
      }

      let openBrace = false;
      if (hasTrailingBrace(text)) {
        openBrace = true;
        text = text.replace(/\{\s*;?$/, '').trim();
      }

      const endKeyword = /^(?:结束|end|endif|endfor|endwhile|endfunction)(?:$|\s)/i.test(text);
      return { line: index + 1, indent, text, closeCount, openBrace, endKeyword };
    }).filter((item) => item.text || item.closeCount || item.endKeyword);
  }

  function functionHeader(text) {
    const match = String(text).match(/^(?:函数|def|func|function)\s+([\w\u3400-\u9fff]+)\s*(?:\(([^)]*)\))?\s*$/i);
    if (!match) return null;
    return {
      name: match[1],
      params: (match[2] || '').split(',').map((part) => part.trim()).filter(Boolean),
    };
  }

  function blockHeader(text) {
    const source = String(text).trim();
    let match = source.match(/^(?:如果|若|if)\s*(?:\((.*)\)|(.*))$/i);
    if (match) return { type: 'if', condition: (match[1] || match[2] || '').trim() };
    match = source.match(/^(?:(?:否则|else)\s+(?:如果|若|if)|否则如果|否则若|elif|elseif)\s*(?:\((.*)\)|(.*))$/i);
    if (match) return { type: 'elif', condition: (match[1] || match[2] || '').trim() };
    if (/^(?:否则|else)\s*$/i.test(source)) return { type: 'else' };

    match = source.match(/^(?:对于|foreach|for)\s+(?!\()(.+?)\s+(?:在|in)\s+(.+)$/i);
    if (match) return { type: 'for', variable: match[1].trim(), range: match[2].trim() };
    match = source.match(/^for\s*\((.*)\)$/i);
    if (match) {
      const inner = match[1].trim();
      const inMatch = inner.match(/^(.+?)\s+(?:在|in)\s+(.+)$/i);
      return inMatch
        ? { type: 'for', variable: inMatch[1].trim(), range: inMatch[2].trim() }
        : { type: 'for', range: inner };
    }
    match = source.match(/^(?:当|while)\s*(?:\((.*)\)|(.*))$/i);
    if (match) return { type: 'while', condition: (match[1] || match[2] || '').trim() };
    if (/^(?:main|主程序)\s*$/i.test(source)) return { type: 'main' };
    return null;
  }

  function isQuantumOp(line) {
    const quantumKeywords = [
      'H(', 'X(', 'Y(', 'Z(', 'S(', 'T(', 'Rx(', 'Ry(', 'Rz(',
      'CNOT(', 'CX(', 'CZ(', 'SWAP(', 'M(', 'measure(',
      '量子寄存器', '量子比特', 'qreg', 'qubit', 'qalloc',
    ];
    return quantumKeywords.some((keyword) => line.includes(keyword));
  }

  function simpleStatement(item) {
    const line = item.text;
    const returnMatch = line.match(/^(?:返回|return)(?:\s+|$)(.*)$/i);
    if (returnMatch) return { type: 'return', value: returnMatch[1].trim(), line: item.line };
    if (/^(?:跳出|break)(?:\s|$)/i.test(line)) return { type: 'break', line: item.line };
    if (/^(?:继续|continue)(?:\s|$)/i.test(line)) return { type: 'continue', line: item.line };
    if (/^(?:导入|import)(?:\s|$)/i.test(line)) return { type: 'import', text: line, line: item.line };
    if (/^(?:打印|输出|print|println|input|输入)(?:\s|\(|$)/i.test(line)) return { type: 'io', operation: line, line: item.line };
    if (isQuantumOp(line)) return { type: 'quantum', operation: line, line: item.line };

    // Do not classify comparisons (`==`, `<=`, `>=`, `!=`, `=>`) as assignments.
    const assignment = line.match(/^(?:(?:let|var|const|定义)\s+)?([A-Za-z_\u3400-\u9fff][\w\u3400-\u9fff.\[\]]*)\s*(?<![=!<>])=(?!=)\s*(.*)$/);
    if (assignment) return { type: 'assign', variable: assignment[1], value: assignment[2].trim(), line: item.line };
    if (/\([^)]*\)/.test(line)) return { type: 'call', expression: line, line: item.line };
    return { type: 'stmt', text: line, line: item.line };
  }

  function parseProgram(code) {
    const lines = tokenize(code);
    const state = { lines, index: 0 };
    const ast = { type: 'program', functions: [], main: [], diagnostics: [] };

    function nextMeaningful(start) {
      let i = start;
      while (i < lines.length && !lines[i].text && !lines[i].closeCount && !lines[i].endKeyword) i += 1;
      return lines[i];
    }

    function consumeClose() {
      const item = lines[state.index];
      if (!item || !item.closeCount) return false;
      item.closeCount -= 1;
      if (!item.closeCount && !item.text) state.index += 1;
      return true;
    }

    function consumeEnd() {
      if (lines[state.index]?.endKeyword) {
        state.index += 1;
        return true;
      }
      return false;
    }

    function parseBlock(baseIndent, braceBlock) {
      const body = [];
      while (state.index < lines.length) {
        const item = lines[state.index];
        if (item.closeCount || item.endKeyword) break;
        if (body.length && !braceBlock && item.indent < baseIndent) break;
        if (!body.length && item.indent > baseIndent) baseIndent = item.indent;

        const header = blockHeader(item.text);
        if (header?.type === 'else' || header?.type === 'elif') break;
        state.index += 1;

        if (header?.type === 'main') {
          const child = parseChild(item);
          body.push(...child.body);
          continue;
        }
        if (header?.type === 'if') {
          const node = { type: 'if', condition: header.condition, then: [], else: [], branches: [], line: item.line };
          const branch = parseChild(item, { consumeEndMarker: false });
          node.then = branch.body;
          node.branches.push({ condition: header.condition, body: branch.body, line: item.line });
          parseBranches(node, item);
          body.push(node);
          continue;
        }
        if (header?.type === 'for' || header?.type === 'while') {
          const node = { type: header.type, line: item.line, body: [] };
          if (header.type === 'for') {
            node.variable = header.variable || '';
            node.range = header.range || '';
          } else node.condition = header.condition || '';
          node.body = parseChild(item).body;
          body.push(node);
          continue;
        }
        if (header?.type === 'else' || header?.type === 'elif') continue;
        body.push(simpleStatement(item));
      }
      return { body, index: state.index };
    }

    function parseChild(headerItem, { consumeEndMarker = true } = {}) {
      const item = nextMeaningful(state.index);
      if (!item) return { body: [] };
      const braceBlock = headerItem.openBrace;
      if (!braceBlock && item.indent <= headerItem.indent && !item.endKeyword) return { body: [] };
      const childIndent = braceBlock ? item.indent : Math.max(item.indent, headerItem.indent + 1);
      const result = parseBlock(childIndent, braceBlock);
      if (braceBlock) consumeClose();
      else if (consumeEndMarker) consumeEnd();
      return result;
    }

    function parseBranches(node, parentItem) {
      while (state.index < lines.length) {
        const item = lines[state.index];
        if (item.closeCount) {
          // A close marker may be followed by `else` on the same physical line.
          if (!item.text) return;
          consumeClose();
          if (lines[state.index] && lines[state.index].text) continue;
          return;
        }
        if (item.endKeyword) {
          consumeEnd();
          return;
        }
        const header = blockHeader(item.text);
        if (!header || (header.type !== 'else' && header.type !== 'elif') || item.indent < parentItem.indent) return;
        state.index += 1;
        const branch = parseChild(item);
        if (header.type === 'elif') node.branches.push({ condition: header.condition, body: branch.body, line: item.line });
        else node.else = branch.body;
      }
    }

    while (state.index < lines.length) {
      const item = lines[state.index];
      if (item.closeCount) { consumeClose(); continue; }
      if (item.endKeyword) { consumeEnd(); continue; }
      const fn = functionHeader(item.text);
      if (fn) {
        state.index += 1;
        const child = parseChild(item);
        ast.functions.push({ type: 'function', name: fn.name, params: fn.params, body: child.body, line: item.line });
        continue;
      }
      const main = blockHeader(item.text);
      if (main?.type === 'main') {
        state.index += 1;
        ast.main.push(...parseChild(item).body);
        continue;
      }
      // A top-level control statement is still useful in a script without main {}.
      ast.main.push(...parseBlock(item.indent, false).body);
    }
    return ast;
  }

  if (typeof module !== 'undefined' && module.exports) module.exports = { parseProgram };
  else window.LosuParse = { parseProgram };
})();
