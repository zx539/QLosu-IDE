/**
 * QLosu IDE 前端主逻辑
 *
 * 三栏布局：左侧边栏（工作区文件 / 示例），中间编辑器（Monaco Editor），右侧（量子线路 / 流程图 / Agent）
 * Monaco 定义 Losu 语言的语法高亮（keywords / operators / comments）
 * 运行时 /api/run 返回插桩后的输出，trace.js 解析为结构化轨迹，drawio.js 生成图 XML，
 * 然后通过 draw.io viewer 渲染（www/vendor/drawio/viewer-static.min.js）。
 */
(function () {
  'use strict';
  const API = location.origin;

  // ---------- 状态 ----------
  const state = {
    ws: { root: '', entries: [] },
    examples: [],
    tabs: [],
    activeTab: null,
    activeView: 'workspace',
    theme: localStorage.getItem('qlosu_theme') || 'dark',
    panelTab: 'output',
    rightTab: 'circuit',
    lastTrace: null,
    chatHistory: [],
    aiAssistant: null,
  };

  let editor;
  let diagram;
  let runAbort;

  // ---------- DOM ----------
  const $ = (sel, ctx = document) => ctx.querySelector(sel);
  const $$ = (sel, ctx = document) => Array.from(ctx.querySelectorAll(sel));

  const els = {
    activeView: { workspace: $('.ab-btn[data-view="workspace"]'), examples: $('.ab-btn[data-view="examples"]') },
    sideView: { workspace: $('.side-view[data-view="workspace"]'), examples: $('.side-view[data-view="examples"]') },
    wsRoot: $('#ws-root-text'),
    wsTree: $('#ws-tree'),
    exList: $('#ex-list'),
    tabs: $('#tabs'),
    editorHost: $('#editor'),
    welcome: $('#welcome'),
    output: $('#output'),
    problems: $('#problems'),
    rightTabs: $$('.rtab'),
    rviews: $$('.rview'),
    diagramHost: $('.diagram-host'),
    diagramEmpty: $('.diagram-empty'),
    stepper: $('#stepper'),
    stepRange: $('#step-range'),
    stepLabel: $('#step-label'),
    stateView: $('#state-view'),
    stateTitle: $('#state-title'),
    stateBody: $('#state-body'),
    chat: $('#chat'),
    composer: $('#composer-input'),
    agentContext: $('#agent-context'),
    statusbar: $('#statusbar'),
    sbVersion: $('#sb-version'),
    sbPos: $('#sb-pos'),
  };

  // ---------- 主题 ----------
  function applyTheme(t) {
    state.theme = t;
    document.documentElement.setAttribute('data-theme', t);
    localStorage.setItem('qlosu_theme', t);
    if (editor) monaco.editor.setTheme(t === 'light' ? 'vs' : 'vs-dark');
  }

  // ---------- 视图切换 ----------
  function switchView(name) {
    state.activeView = name;
    for (const [k, v] of Object.entries(els.activeView)) {
      v.classList.toggle('active', k === name);
      els.sideView[k].classList.toggle('active', k === name);
    }
  }

  function switchPanelTab(name) {
    state.panelTab = name;
    $$('.ptab').forEach((t) => t.classList.toggle('active', t.dataset.tab === name));
    $$('.pview').forEach((v) => v.classList.toggle('active', v.id === name));
  }

  function switchRightTab(name) {
    state.rightTab = name;
    els.rightTabs.forEach((t) => t.classList.toggle('active', t.dataset.tab === name));
    els.rviews.forEach((v) => v.classList.toggle('active', v.dataset.tab === name));
    if (name === 'circuit' || name === 'flowchart') refreshDiagram();
  }

  // ---------- 编辑器标签页 ----------
  function openTab({ type, path, name, code }) {
    let tab = state.tabs.find((t) => t.type === type && t.path === path);
    if (!tab) {
      tab = { type, path, name, code, dirty: false, model: null };
      state.tabs.push(tab);
      renderTabs();
    }
    activateTab(tab);
  }

  function activateTab(tab) {
    state.activeTab = tab;
    if (!tab.model) {
      tab.model = monaco.editor.createModel(tab.code, tab.type === 'example' || tab.type === 'file' ? 'losu' : 'plaintext');
      tab.model.onDidChangeContent(() => {
        tab.code = tab.model.getValue();
        tab.dirty = true;
        renderTabs();
      });
    }
    editor.setModel(tab.model);
    renderTabs();
    els.welcome.classList.add('hidden');
    editor.focus();
  }

  function closeTab(tab) {
    const idx = state.tabs.indexOf(tab);
    if (idx < 0) return;
    if (tab.dirty && !confirm(`${tab.name} 未保存，确定关闭？`)) return;
    if (tab.model) tab.model.dispose();
    state.tabs.splice(idx, 1);
    if (state.activeTab === tab) {
      state.activeTab = state.tabs[Math.min(idx, state.tabs.length - 1)] || null;
      if (state.activeTab) activateTab(state.activeTab);
      else { editor.setModel(null); els.welcome.classList.remove('hidden'); }
    }
    renderTabs();
  }

  function renderTabs() {
    els.tabs.innerHTML = state.tabs.map((t) => {
      const active = t === state.activeTab ? ' active' : '';
      const mark = t.dirty ? '<span class="dirty"></span>' : '';
      return `<div class="tab${active}" data-path="${esc(t.path)}">${esc(t.name)} ${mark}<button class="close" onclick="event.stopPropagation();app.closeTabByPath('${esc(t.path)}')">×</button></div>`;
    }).join('');
    $$('.tab').forEach((el) => el.onclick = () => {
      const tab = state.tabs.find((t) => t.path === el.dataset.path);
      if (tab) activateTab(tab);
    });
  }

  // ---------- 工作区 ----------
  async function loadWorkspace() {
    const r = await fetch(`${API}/api/fs/list`).then((x) => x.json());
    state.ws = r;
    els.wsRoot.textContent = r.root;
    els.wsTree.innerHTML = r.entries.length
      ? r.entries.map((e) => `<div class="tree-item" data-path="${esc(e.path)}"><svg fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="${e.type === 'dir' ? 'M3 7v10a2 2 0 002 2h14a2 2 0 002-2V9a2 2 0 00-2-2h-6l-2-2H5a2 2 0 00-2 2z' : 'M7 21h10a2 2 0 002-2V9.414a1 1 0 00-.293-.707l-5.414-5.414A1 1 0 0012.586 3H7a2 2 0 00-2 2v14a2 2 0 002 2z'}"/></svg><span class="name">${esc(e.path)}</span></div>`).join('')
      : '<div class="empty-hint">工作区为空<br>可在编辑器中编写代码后保存</div>';
    $$('.tree-item').forEach((el) => el.onclick = () => openFile(el.dataset.path));
  }

  async function openFile(path) {
    const r = await fetch(`${API}/api/fs/read?path=${encodeURIComponent(path)}`).then((x) => x.json()).catch(() => null);
    if (r) openTab({ type: 'file', path, name: path.split('/').pop(), code: r.content });
  }

  async function saveFile() {
    const tab = state.activeTab;
    if (!tab || tab.type !== 'file') return toast('当前标签页不可保存', 'err');
    await fetch(`${API}/api/fs/write`, { method: 'POST', headers: { 'content-type': 'application/json' }, body: JSON.stringify({ path: tab.path, content: tab.code }) });
    tab.dirty = false;
    renderTabs();
    toast('已保存');
  }

  // ---------- 示例 ----------
  async function loadExamples() {
    const r = await fetch(`${API}/api/examples`).then((x) => x.json());
    state.examples = r.examples;
    els.exList.innerHTML = r.examples.map((e) => `<div class="ex-item" data-name="${esc(e.name)}"><div class="t">${esc(e.name)}</div><div class="d">${esc(e.description)}</div></div>`).join('');
    $$('.ex-item').forEach((el) => el.onclick = () => openExample(el.dataset.name));
  }

  async function openExample(name) {
    const r = await fetch(`${API}/api/examples/${encodeURIComponent(name)}`).then((x) => x.json());
    openTab({ type: 'example', path: 'example:' + name, name, code: r.code });
  }

  // ---------- 运行 ----------
  async function run() {
    const tab = state.activeTab;
    if (!tab) return;
    if (runAbort) runAbort.abort();
    runAbort = new AbortController();
    switchPanelTab('output');
    els.output.innerHTML = '<span class="o-dim">运行中...</span>';
    const t0 = Date.now();
    try {
      const r = await fetch(`${API}/api/run`, { method: 'POST', headers: { 'content-type': 'application/json' }, body: JSON.stringify({ code: tab.code, trace: true }), signal: runAbort.signal }).then((x) => x.json());
      const dt = Date.now() - t0;
      runAbort = null;
      if (r.timedOut) return showOutput(['<span class="o-err">运行超时（15 秒）</span>'], []);
      if (r.ret !== 0) {
        const trace = QTrace.parse(r.stdout);
        state.lastTrace = trace;
        const lines = trace.log.map((l) => {
          const cls = l.kind === 'error' ? 'o-err' : l.kind === 'state' ? 'o-state' : l.kind === 'tag' ? 'o-tag' : '';
          return cls ? `<span class="${cls}">${esc(l.text)}</span>` : esc(l.text);
        });
        showOutput(lines, trace.errors);
        if (trace.errors.length && trace.errors[0].line && tab.model) highlightError(tab.model, trace.errors[0].line);
        return;
      }
      const trace = QTrace.parse(r.stdout);
      state.lastTrace = trace;
      const out = trace.stdout ? trace.stdout.split('\n').map(esc) : [];
      out.push(`<span class="o-ok">✓ 完成（${dt} ms）</span>`);
      showOutput(out, []);
      if (state.rightTab === 'circuit' || state.rightTab === 'flowchart') refreshDiagram();
    } catch (e) {
      if (e.name === 'AbortError') return;
      showOutput([`<span class="o-err">运行失败：${esc(e.message)}</span>`], []);
    }
  }

  function showOutput(lines, errors) {
    els.output.innerHTML = lines.join('\n');
    els.problems.innerHTML = errors.length
      ? errors.map((e) => `<div class="problem" data-line="${e.line}"><span class="loc">第 ${e.line} 行</span><span class="msg">${esc(e.message)}</span></div>`).join('')
      : '<div class="empty-hint">无问题</div>';
    $$('.problem').forEach((el) => {
      el.onclick = () => {
        const line = Number(el.dataset.line);
        if (line && editor.getModel()) editor.revealLineInCenter(line);
      };
    });
    $$('.ptab').forEach((t) => {
      const badge = t.querySelector('.badge');
      if (badge) badge.textContent = errors.length || '';
      if (t.dataset.tab === 'problems') badge.style.display = errors.length ? '' : 'none';
    });
  }

  function highlightError(model, line) {
    const deco = model.deltaDecorations([], [{
      range: new monaco.Range(line, 1, line, 1),
      options: { isWholeLine: true, className: 'err-line', glyphMarginClassName: 'err-glyph' },
    }]);
    setTimeout(() => model.deltaDecorations(deco, []), 4000);
  }

  // ---------- 图表 ----------
  function refreshDiagram() {
    if (!state.lastTrace) return showDiagramEmpty('请先运行代码');
    if (state.rightTab === 'circuit') {
      if (!state.lastTrace.registers.length) return showDiagramEmpty('本次运行未使用量子寄存器');
      const regId = 0;
      const xml = QDrawio.circuitXml(state.lastTrace, regId, { theme: state.theme });
      renderDiagram(xml);
      refreshStepper();
    } else if (state.rightTab === 'flowchart') {
      const tab = state.activeTab;
      if (!tab) return showDiagramEmpty('未打开文件');
      const prog = LosuParse.parseProgram(tab.code);
      const xml = QDrawio.flowchartXml(prog, { theme: state.theme, title: tab.name });
      renderDiagram(xml);
    }
  }

  function showDiagramEmpty(msg) {
    if (diagram) { diagram.destroy(); diagram = null; }
    els.diagramHost.innerHTML = '';
    els.diagramEmpty.textContent = msg;
    els.diagramEmpty.style.display = 'block';
    els.stepper.style.display = 'none';
  }

  function renderDiagram(xml) {
    els.diagramEmpty.style.display = 'none';
    els.diagramHost.innerHTML = `<div class="mxgraph" style="max-width:100%"></div>`;
    const cfg = { xml, highlight: '#6aa7ff', nav: false, toolbar: '', lightbox: false, resize: true };
    const cont = els.diagramHost.querySelector('.mxgraph');
    cont.setAttribute('data-mxgraph', JSON.stringify(cfg));
    if (typeof GraphViewer !== 'undefined') GraphViewer.processElements();
  }

  function refreshStepper() {
    if (state.rightTab !== 'circuit' || !state.lastTrace) return els.stepper.style.display = 'none';
    const ops = state.lastTrace.ops.filter((o) => o.reg === 0);
    if (!ops.length) return els.stepper.style.display = 'none';
    els.stepper.style.display = 'flex';
    els.stepRange.max = ops.length - 1;
    els.stepRange.value = ops.length - 1;
    updateStep(ops.length - 1);
  }

  function updateStep(idx) {
    const ops = state.lastTrace.ops.filter((o) => o.reg === 0);
    const op = ops[idx];
    els.stepLabel.textContent = `步骤 ${idx + 1} / ${ops.length}`;
    if (!op || !op.state) return els.stateView.style.display = 'none';
    els.stateView.style.display = 'block';
    const st = op.state;
    const nz = QTrace.nonZero(st).slice(0, 32);
    els.stateTitle.textContent = `寄存器 #${op.reg} 状态（${st.nQubits} qubit${st.partial ? '，部分显示' : ''}）`;
    els.stateBody.innerHTML = nz.map((a) => {
      const ket = QTrace.ket(a.index, st.nQubits);
      const amp = QTrace.fmtComplex(a);
      const pct = (a.p * 100).toFixed(1);
      return `<div class="amp-row"><span class="ket">${esc(ket)}</span><div class="amp-bar"><div style="width:${pct}%"></div></div><span class="p">${pct}%</span><span class="a mono">${esc(amp)}</span></div>`;
    }).join('');
  }

  // ---------- 工具 ----------
  function toast(msg, type = 'ok') {
    const cls = type === 'err' ? 'toast err' : type === 'ok' ? 'toast ok' : 'toast';
    const el = document.createElement('div');
    el.className = cls;
    el.textContent = msg;
    const host = $('.toast-host') || (() => {
      const h = document.createElement('div');
      h.className = 'toast-host';
      document.body.appendChild(h);
      return h;
    })();
    host.appendChild(el);
    setTimeout(() => el.remove(), 3000);
  }

  function esc(s) {
    return String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
  }

  // ---------- AI 助手 ----------
  async function sendAIMessage() {
    const input = els.composer.value.trim();
    if (!input) return;

    if (!state.aiAssistant) {
      state.aiAssistant = new AIAssistant();
    }

    els.composer.value = '';
    els.composer.disabled = true;

    // 添加用户消息
    addChatMessage('user', input);

    // 构建上下文
    const context = {};
    if (els.agentContext.checked && state.activeTab) {
      context.code = state.activeTab.code;
      const selection = editor.getModel()?.getValueInRange(editor.getSelection());
      if (selection) context.selection = selection;
    }
    if (state.lastTrace?.errors?.length) {
      context.error = state.lastTrace.errors.map(e => `第 ${e.line} 行: ${e.message}`).join('\n');
    }

    // 显示思考中
    const thinkingId = addChatMessage('assistant', '思考中...');
    const statusEl = document.getElementById('ai-status');
    statusEl.textContent = '正在生成回复...';

    try {
      let fullResponse = '';
      const msgEl = document.querySelector(`[data-msg-id="${thinkingId}"]`);

      for await (const chunk of state.aiAssistant.streamChat(input, context)) {
        if (chunk.type === 'text') {
          fullResponse += chunk.content;
          if (msgEl) msgEl.innerHTML = marked.parse(fullResponse);
        }
      }

      state.aiAssistant.addToHistory('user', input);
      state.aiAssistant.addToHistory('assistant', fullResponse);
      statusEl.textContent = '';
    } catch (error) {
      const msgEl = document.querySelector(`[data-msg-id="${thinkingId}"]`);
      if (msgEl) {
        msgEl.innerHTML = `<div class="chat-error">❌ 错误: ${esc(error.message)}\n\n请检查 AI 配置（点击右上角设置图标）</div>`;
      }
      statusEl.textContent = '';
    } finally {
      els.composer.disabled = false;
      els.composer.focus();
    }
  }

  function sendAIPrompt(prompt) {
    els.composer.value = prompt;
    sendAIMessage();
  }

  function clearAIChat() {
    if (!confirm('确定清空对话历史？')) return;
    els.chat.innerHTML = '<div class="chat-welcome"><p>对话已清空</p></div>';
    state.chatHistory = [];
    if (state.aiAssistant) state.aiAssistant.clearHistory();
  }

  function addChatMessage(role, content) {
    const id = `msg-${Date.now()}-${Math.random()}`;
    const msgDiv = document.createElement('div');
    msgDiv.className = `msg ${role}`;
    msgDiv.setAttribute('data-msg-id', id);

    if (role === 'user') {
      msgDiv.textContent = content;
    } else {
      msgDiv.innerHTML = typeof marked !== 'undefined' ? marked.parse(content) : esc(content);
    }

    // 移除欢迎消息
    const welcome = els.chat.querySelector('.chat-welcome');
    if (welcome) welcome.remove();

    els.chat.appendChild(msgDiv);
    els.chat.scrollTop = els.chat.scrollHeight;

    return id;
  }

  function openAISettings() {
    const config = state.aiAssistant?.config || new AIAssistant().loadConfig();

    const modal = document.createElement('div');
    modal.className = 'modal-mask';
    modal.innerHTML = `
      <div class="modal">
        <div class="modal-head">
          AI 助手配置
          <div class="spacer"></div>
          <button class="icon-btn" onclick="this.closest('.modal-mask').remove()">×</button>
        </div>
        <div class="modal-body">
          <div class="form-row">
            <label>AI 提供商</label>
            <select class="input" id="ai-provider">
              <option value="ollama" ${config.provider === 'ollama' ? 'selected' : ''}>Ollama (本地)</option>
              <option value="openai" ${config.provider === 'openai' ? 'selected' : ''}>OpenAI</option>
              <option value="anthropic" ${config.provider === 'anthropic' ? 'selected' : ''}>Anthropic Claude</option>
            </select>
          </div>
          <div class="form-hint">推荐使用 Ollama 本地运行，无需 API Key</div>

          <div id="ollama-config" style="${config.provider === 'ollama' ? '' : 'display:none'}">
            <div class="form-row">
              <label>Ollama 地址</label>
              <input class="input" id="ollama-url" value="${esc(config.ollama.baseURL)}">
            </div>
            <div class="form-row">
              <label>模型</label>
              <input class="input" id="ollama-model" value="${esc(config.ollama.model)}">
            </div>
            <div class="form-hint">推荐模型：qwen2.5-coder:7b, deepseek-coder-v2</div>
          </div>

          <div id="openai-config" style="${config.provider === 'openai' ? '' : 'display:none'}">
            <div class="form-row">
              <label>API Key</label>
              <input class="input" type="password" id="openai-key" value="${esc(config.openai.apiKey)}">
            </div>
            <div class="form-row">
              <label>API 地址</label>
              <input class="input" id="openai-url" value="${esc(config.openai.baseURL)}">
            </div>
            <div class="form-row">
              <label>模型</label>
              <input class="input" id="openai-model" value="${esc(config.openai.model)}">
            </div>
          </div>

          <div id="anthropic-config" style="${config.provider === 'anthropic' ? '' : 'display:none'}">
            <div class="form-row">
              <label>API Key</label>
              <input class="input" type="password" id="anthropic-key" value="${esc(config.anthropic.apiKey)}">
            </div>
            <div class="form-row">
              <label>模型</label>
              <input class="input" id="anthropic-model" value="${esc(config.anthropic.model)}">
            </div>
          </div>
        </div>
        <div class="modal-foot">
          <button class="btn" onclick="this.closest('.modal-mask').remove()">取消</button>
          <button class="btn primary" onclick="app.saveAIConfig()">保存</button>
        </div>
      </div>
    `;

    document.body.appendChild(modal);

    // 切换提供商时显示对应配置
    modal.querySelector('#ai-provider').onchange = (e) => {
      modal.querySelector('#ollama-config').style.display = e.target.value === 'ollama' ? '' : 'none';
      modal.querySelector('#openai-config').style.display = e.target.value === 'openai' ? '' : 'none';
      modal.querySelector('#anthropic-config').style.display = e.target.value === 'anthropic' ? '' : 'none';
    };
  }

  function saveAIConfig() {
    const modal = document.querySelector('.modal-mask');
    const provider = modal.querySelector('#ai-provider').value;

    const config = {
      provider,
      ollama: {
        baseURL: modal.querySelector('#ollama-url').value,
        model: modal.querySelector('#ollama-model').value,
      },
      openai: {
        apiKey: modal.querySelector('#openai-key').value,
        baseURL: modal.querySelector('#openai-url').value,
        model: modal.querySelector('#openai-model').value,
      },
      anthropic: {
        apiKey: modal.querySelector('#anthropic-key').value,
        model: modal.querySelector('#anthropic-model').value,
      },
    };

    if (!state.aiAssistant) state.aiAssistant = new AIAssistant();
    state.aiAssistant.saveConfig(config);

    toast('AI 配置已保存');
    modal.remove();
  }

  // ---------- 本地文件打开 ----------
  async function openLocalFile() {
    if (typeof window.electronAPI !== 'undefined') {
      const file = await window.electronAPI.openFile();
      if (file) {
        openTab({ type: 'local', path: file.path, name: file.name, code: file.content });
      }
    } else {
      // Web 模式：使用 <input type="file">
      const input = document.createElement('input');
      input.type = 'file';
      input.accept = '.losu,.txt';
      input.onchange = async (e) => {
        const file = e.target.files[0];
        if (file) {
          const content = await file.text();
          openTab({ type: 'local', path: file.name, name: file.name, code: content });
        }
      };
      input.click();
    }
  }

  async function openLocalFolder() {
    if (typeof window.electronAPI !== 'undefined') {
      const folder = await window.electronAPI.openFolder();
      if (folder) {
        toast('文件夹打开功能开发中', 'warn');
      }
    } else {
      toast('Web 模式不支持打开文件夹，请使用桌面版', 'warn');
    }
  }

  // ---------- 初始化 ----------
  async function init() {
    applyTheme(state.theme);
    require.config({ paths: { vs: `${API}/vendor/monaco/vs` } });
    require(['vs/editor/editor.main'], async () => {
      monaco.languages.register({ id: 'losu' });
      monaco.languages.setMonarchTokensProvider('losu', {
        keywords: ['var', 'return', 'if', 'else', 'for', 'while', 'break', 'continue', 'import', 'in', 'and', 'or', 'not', 'true', 'false', 'nil', 'yield'],
        operators: ['=>', ':>', '~>', '->', '::'],
        tokenizer: {
          root: [
            [/\/\/.*$/, 'comment'],
            [/\/\*/, 'comment', '@comment'],
            [/"/, 'string', '@string'],
            [/'/, 'string', '@string_single'],
            [/\b(var|return|if|else|for|while|break|continue|import|in|and|or|not|yield)\b/, 'keyword'],
            [/\b(true|false|nil)\b/, 'keyword'],
            [/[A-Za-z_\u0080-￿][A-Za-z0-9_\u0080-￿]*/, 'identifier'],
            [/[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?/, 'number'],
          ],
          comment: [[/\*\//, 'comment', '@pop'], [/./, 'comment']],
          string: [[/[^\\"]+/, 'string'], [/\\./, 'string.escape'], [/"/, 'string', '@pop']],
          string_single: [[/[^\\']+/, 'string'], [/\\./, 'string.escape'], [/'/, 'string', '@pop']],
        },
      });
      editor = monaco.editor.create(els.editorHost, {
        theme: state.theme === 'light' ? 'vs' : 'vs-dark',
        fontSize: 13,
        minimap: { enabled: true },
        lineNumbers: 'on',
        glyphMargin: true,
        automaticLayout: true,
        scrollBeyondLastLine: false,
        renderWhitespace: 'selection',
        cursorBlinking: 'smooth',
        cursorSmoothCaretAnimation: 'on',
        smoothScrolling: true,
        tabSize: 2,
        quickSuggestions: false,
      });
      editor.onDidChangeCursorPosition((e) => {
        els.sbPos.textContent = `第 ${e.position.lineNumber} 行 ${e.position.column} 列`;
      });
      window.addEventListener('resize', () => editor.layout());
      const ver = await fetch(`${API}/api/version`).then((x) => x.json());
      els.sbVersion.textContent = ver.version;
      await Promise.all([loadWorkspace(), loadExamples()]);
      if (state.examples.length) openExample(state.examples[0].name);
    });
  }

  // ---------- 对外接口 ----------
  window.app = {
    init,
    run,
    saveFile,
    openExample,
    openFile,
    openLocalFile,
    openLocalFolder,
    openTab,
    closeTabByPath: (p) => closeTab(state.tabs.find((t) => t.path === p)),
    switchView,
    switchPanelTab,
    switchRightTab,
    applyTheme,
    refreshDiagram,
    sendAIMessage,
    sendAIPrompt,
    clearAIChat,
    openAISettings,
    saveAIConfig,
  };
  window.addEventListener('DOMContentLoaded', init);

  els.stepRange.oninput = () => updateStep(Number(els.stepRange.value));
})();
