// QLosu IDE - Losu 语法解析器（简化版）
// 用于生成程序流程图
(function() {
  'use strict';

  /**
   * 解析 Losu 代码生成 AST（简化版）
   * @param {string} code - Losu 源代码
   * @returns {Object} AST 结构
   */
  function parseProgram(code) {
    const lines = code.split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#'));
    const ast = {
      type: 'program',
      functions: [],
      main: []
    };

    let currentFunc = null;
    let currentBlock = ast.main;
    let blockStack = [];

    for (let i = 0; i < lines.length; i++) {
      const line = lines[i];

      // 函数定义
      if (line.startsWith('函数 ') || line.startsWith('def ')) {
        const match = line.match(/(?:函数|def)\s+(\w+)\s*\((.*?)\)/);
        if (match) {
          currentFunc = {
            type: 'function',
            name: match[1],
            params: match[2] ? match[2].split(',').map(p => p.trim()) : [],
            body: []
          };
          ast.functions.push(currentFunc);
          currentBlock = currentFunc.body;
          blockStack = [currentBlock];
        }
      }
      // 返回语句
      else if (line.startsWith('返回 ') || line.startsWith('return ')) {
        const value = line.replace(/^(?:返回|return)\s+/, '');
        currentBlock.push({ type: 'return', value });
      }
      // if 语句
      else if (line.startsWith('若 ') || line.startsWith('if ')) {
        const condition = line.replace(/^(?:若|if)\s+/, '').replace(/[：:]\s*$/, '');
        const node = {
          type: 'if',
          condition,
          then: [],
          else: []
        };
        currentBlock.push(node);
        blockStack.push(currentBlock);
        currentBlock = node.then;
      }
      // else 语句
      else if (line === '否则:' || line === 'else:' || line === '否则：' || line === 'else：') {
        if (blockStack.length > 0) {
          const parentBlock = blockStack[blockStack.length - 1];
          const lastNode = parentBlock[parentBlock.length - 1];
          if (lastNode && lastNode.type === 'if') {
            currentBlock = lastNode.else;
          }
        }
      }
      // for 循环
      else if (line.startsWith('对于 ') || line.startsWith('for ')) {
        const match = line.match(/(?:对于|for)\s+(\w+)\s+(?:在|in)\s+(.+?)[：:]\s*$/);
        if (match) {
          const node = {
            type: 'for',
            variable: match[1],
            range: match[2],
            body: []
          };
          currentBlock.push(node);
          blockStack.push(currentBlock);
          currentBlock = node.body;
        }
      }
      // while 循环
      else if (line.startsWith('当 ') || line.startsWith('while ')) {
        const condition = line.replace(/^(?:当|while)\s+/, '').replace(/[：:]\s*$/, '');
        const node = {
          type: 'while',
          condition,
          body: []
        };
        currentBlock.push(node);
        blockStack.push(currentBlock);
        currentBlock = node.body;
      }
      // 量子操作
      else if (isQuantumOp(line)) {
        currentBlock.push({
          type: 'quantum',
          operation: line
        });
      }
      // IO 操作
      else if (line.startsWith('打印 ') || line.startsWith('print ') ||
               line.startsWith('输入 ') || line.startsWith('input ')) {
        currentBlock.push({
          type: 'io',
          operation: line
        });
      }
      // 变量赋值
      else if (line.includes('=') && !line.includes('==')) {
        const parts = line.split('=');
        currentBlock.push({
          type: 'assign',
          variable: parts[0].trim(),
          value: parts.slice(1).join('=').trim()
        });
      }
      // 函数调用
      else if (line.includes('(') && line.includes(')')) {
        currentBlock.push({
          type: 'call',
          expression: line
        });
      }
      // 块结束（通过缩进判断 - 简化版本直接检测新块开始）
    }

    return ast;
  }

  /**
   * 判断是否为量子操作
   */
  function isQuantumOp(line) {
    const quantumKeywords = [
      'H(', 'X(', 'Y(', 'Z(', 'S(', 'T(',
      'Rx(', 'Ry(', 'Rz(',
      'CNOT(', 'CZ(', 'SWAP(',
      'M(', 'measure(',
      '量子寄存器', 'qreg', 'qubit'
    ];
    return quantumKeywords.some(kw => line.includes(kw));
  }

  // 导出
  if (typeof module !== 'undefined' && module.exports) {
    module.exports = { parseProgram };
  } else {
    window.LosuParse = { parseProgram };
  }
})();
