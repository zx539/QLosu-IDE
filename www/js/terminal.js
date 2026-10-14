// QLosu IDE - 终端集成
(function() {
  'use strict';

  let term = null;
  let fitAddon = null;

  function init(containerElement) {
    if (typeof Terminal === 'undefined') {
      console.warn('xterm.js not loaded');
      return;
    }

    const container = containerElement || document.getElementById('terminal-container');
    if (!container) return;

    // 创建终端实例
    term = new Terminal({
      cursorBlink: true,
      fontSize: 13,
      fontFamily: 'Menlo, Monaco, "Courier New", monospace',
      theme: {
        background: '#0b0f15',
        foreground: '#d7deea',
        cursor: '#6aa7ff',
        black: '#0d1117',
        red: '#f85149',
        green: '#3fb950',
        yellow: '#d29922',
        blue: '#6aa7ff',
        magenta: '#a78bfa',
        cyan: '#56d4dd',
        white: '#d7deea',
        brightBlack: '#6b778c',
        brightRed: '#ff6b6b',
        brightGreen: '#51cf66',
        brightYellow: '#ffd43b',
        brightBlue: '#8bb9fe',
        brightMagenta: '#b794f6',
        brightCyan: '#74e1e8',
        brightWhite: '#f0f6fc',
      },
      scrollback: 1000,
      convertEol: true,
    });

    // FitAddon
    if (typeof FitAddon !== 'undefined') {
      fitAddon = new FitAddon.FitAddon();
      term.loadAddon(fitAddon);
    }

    term.open(container);
    if (fitAddon) fitAddon.fit();

    // 欢迎信息
    term.writeln('\x1b[1;36m欢迎使用 QLosu IDE 终端\x1b[0m');
    term.writeln('\x1b[90m提示: 此终端用于显示程序输出和调试信息\x1b[0m');
    term.writeln('');
    term.write('$ ');

    // 监听窗口大小变化
    window.addEventListener('resize', () => {
      if (fitAddon) fitAddon.fit();
    });

    // 监听面板切换，自动调整大小
    const observer = new MutationObserver(() => {
      const isVisible = container.offsetParent !== null;
      if (isVisible && fitAddon) {
        setTimeout(() => fitAddon.fit(), 100);
      }
    });
    observer.observe(container, { attributes: true, attributeFilter: ['style', 'class'] });
    if (container.parentElement) {
      observer.observe(container.parentElement, { attributes: true, attributeFilter: ['style', 'class'] });
    }
  }

  function write(text) {
    if (term) {
      term.write(text);
    }
  }

  function writeln(text) {
    if (term) {
      term.writeln(text);
    }
  }

  function clear() {
    if (term) {
      term.clear();
    }
  }

  function updateTheme(themeName) {
    if (!term) return;

    const themes = {
      dark: {
        background: '#0b0f15',
        foreground: '#d7deea',
        cursor: '#6aa7ff',
      },
      light: {
        background: '#ffffff',
        foreground: '#24292f',
        cursor: '#0969da',
      },
      aurora: {
        background: '#1a1b26',
        foreground: '#c0caf5',
        cursor: '#bb9af7',
      }
    };

    const theme = themes[themeName] || themes.dark;
    term.options.theme = { ...term.options.theme, ...theme };
  }

  window.QTerminal = {
    init,
    write,
    writeln,
    clear,
    updateTheme,
    get instance() { return term; }
  };
})();
