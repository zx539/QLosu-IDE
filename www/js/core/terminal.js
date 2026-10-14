// QLosu IDE - 终端模块
// 基于 xterm.js 的集成终端
(function() {
  'use strict';

  let term = null;
  let fitAddon = null;
  let websocket = null;

  const QTerminal = {
    /**
     * 初始化终端
     * @param {HTMLElement} container - 终端容器
     */
    init(container) {
      if (!container) {
        console.error('Terminal container not found');
        return;
      }

      // 创建 xterm 实例
      if (typeof Terminal === 'undefined') {
        console.error('xterm.js not loaded');
        return;
      }

      term = new Terminal({
        cursorBlink: true,
        cursorStyle: 'block',
        fontSize: 14,
        fontFamily: '"Cascadia Code", "Fira Code", "Consolas", monospace',
        theme: {
          background: '#0d1117',
          foreground: '#c9d1d9',
          cursor: '#58a6ff',
          cursorAccent: '#0d1117',
          selection: 'rgba(88, 166, 255, 0.3)',
          black: '#484f58',
          red: '#ff7b72',
          green: '#3fb950',
          yellow: '#d29922',
          blue: '#58a6ff',
          magenta: '#bc8cff',
          cyan: '#39c5cf',
          white: '#b1bac4',
          brightBlack: '#6e7681',
          brightRed: '#ffa198',
          brightGreen: '#56d364',
          brightYellow: '#e3b341',
          brightBlue: '#79c0ff',
          brightMagenta: '#d2a8ff',
          brightCyan: '#56d4dd',
          brightWhite: '#f0f6fc'
        },
        allowTransparency: false,
        convertEol: true,
        scrollback: 1000,
        tabStopWidth: 4
      });

      // 加载 fit addon
      if (typeof FitAddon !== 'undefined') {
        fitAddon = new FitAddon.FitAddon();
        term.loadAddon(fitAddon);
      }

      // 打开终端
      term.open(container);

      // 自适应大小
      if (fitAddon) {
        setTimeout(() => {
          try {
            fitAddon.fit();
          } catch (e) {
            console.warn('Terminal fit failed:', e);
          }
        }, 100);

        // 监听窗口大小变化
        window.addEventListener('resize', () => {
          if (fitAddon && term) {
            try {
              fitAddon.fit();
            } catch (e) {
              // 忽略错误
            }
          }
        });
      }

      // 欢迎信息
      term.writeln('\x1b[1;36m╔════════════════════════════════════════╗\x1b[0m');
      term.writeln('\x1b[1;36m║\x1b[0m  \x1b[1;35m洛书 IDE 集成终端\x1b[0m                 \x1b[1;36m║\x1b[0m');
      term.writeln('\x1b[1;36m╚════════════════════════════════════════╝\x1b[0m');
      term.writeln('');
      term.writeln('\x1b[33m提示：此终端当前仅支持本地命令模拟。\x1b[0m');
      term.writeln('\x1b[33m完整的 shell 功能需要后端支持。\x1b[0m');
      term.writeln('');
      term.write('\x1b[32m$\x1b[0m ');

      // 简单的命令处理（模拟）
      let currentLine = '';
      term.onData(data => {
        if (data === '\r') { // Enter
          term.write('\r\n');
          if (currentLine.trim()) {
            handleCommand(currentLine.trim());
          }
          currentLine = '';
          term.write('\x1b[32m$\x1b[0m ');
        } else if (data === '\x7f' || data === '\b') { // Backspace
          if (currentLine.length > 0) {
            currentLine = currentLine.slice(0, -1);
            term.write('\b \b');
          }
        } else if (data === '\x03') { // Ctrl+C
          term.write('^C\r\n');
          currentLine = '';
          term.write('\x1b[32m$\x1b[0m ');
        } else if (data === '\x04') { // Ctrl+D
          term.write('^D\r\n');
          term.writeln('exit');
        } else {
          currentLine += data;
          term.write(data);
        }
      });
    },

    /**
     * 更新主题
     * @param {string} themeName - 主题名称
     */
    updateTheme(themeName) {
      if (!term) return;

      const themes = {
        dark: {
          background: '#0d1117',
          foreground: '#c9d1d9',
          cursor: '#58a6ff',
          cursorAccent: '#0d1117',
          selection: 'rgba(88, 166, 255, 0.3)'
        },
        light: {
          background: '#ffffff',
          foreground: '#24292f',
          cursor: '#0969da',
          cursorAccent: '#ffffff',
          selection: 'rgba(9, 105, 218, 0.3)'
        },
        aurora: {
          background: '#1a1f2e',
          foreground: '#e0e7ff',
          cursor: '#a78bfa',
          cursorAccent: '#1a1f2e',
          selection: 'rgba(167, 139, 250, 0.3)'
        }
      };

      const theme = themes[themeName] || themes.dark;
      term.options.theme = { ...term.options.theme, ...theme };
    },

    /**
     * 写入内容到终端
     * @param {string} text - 要写入的文本
     */
    write(text) {
      if (term) {
        term.write(text);
      }
    },

    /**
     * 写入一行到终端
     * @param {string} text - 要写入的文本
     */
    writeln(text) {
      if (term) {
        term.writeln(text);
      }
    },

    /**
     * 清空终端
     */
    clear() {
      if (term) {
        term.clear();
      }
    },

    /**
     * 获取终端实例
     */
    getTerm() {
      return term;
    }
  };

  /**
   * 处理命令（简单模拟）
   * @param {string} cmd - 命令
   */
  function handleCommand(cmd) {
    const parts = cmd.trim().split(/\s+/);
    const command = parts[0];
    const args = parts.slice(1);

    switch (command) {
      case 'help':
        term.writeln('可用命令：');
        term.writeln('  \x1b[36mhelp\x1b[0m     - 显示帮助信息');
        term.writeln('  \x1b[36mclear\x1b[0m    - 清空终端');
        term.writeln('  \x1b[36mecho\x1b[0m     - 输出文本');
        term.writeln('  \x1b[36mdate\x1b[0m     - 显示当前时间');
        term.writeln('  \x1b[36mpwd\x1b[0m      - 显示当前目录');
        term.writeln('  \x1b[36mls\x1b[0m       - 列出文件（模拟）');
        break;

      case 'clear':
        term.clear();
        break;

      case 'echo':
        term.writeln(args.join(' '));
        break;

      case 'date':
        term.writeln(new Date().toString());
        break;

      case 'pwd':
        term.writeln('/home/qlosu/workspace');
        break;

      case 'ls':
        term.writeln('\x1b[34mexamples\x1b[0m    \x1b[34msrc\x1b[0m    \x1b[32mREADME.md\x1b[0m');
        break;

      case '':
        break;

      default:
        term.writeln(`\x1b[31m${command}: command not found\x1b[0m`);
        term.writeln('\x1b[33m提示：输入 "help" 查看可用命令\x1b[0m');
        break;
    }
  }

  // 导出到全局
  window.QTerminal = QTerminal;
})();
