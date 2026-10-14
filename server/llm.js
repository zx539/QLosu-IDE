/**
 * 大模型接口代理（供 IDE 内置 Agent 使用）
 *
 * 浏览器端维护与提供商无关的统一消息格式：
 *   { role: 'user', content }
 *   { role: 'assistant', content, tool_calls: [{ id, name, arguments }] }
 *   { role: 'tool', tool_call_id, name, content }
 * 本模块将其转换为 OpenAI 兼容（OpenAI / DeepSeek / 通义千问 / Kimi / 智谱 /
 * Ollama / vLLM 等）或 Anthropic Messages 格式，并把流式响应统一为 NDJSON 事件：
 *   { type: 'text', delta }
 *   { type: 'tool_call', id, name, arguments }
 *   { type: 'done', stop_reason, usage }
 *   { type: 'error', message }
 */

const DEFAULT_BASE = {
  openai: 'https://api.openai.com/v1',
  anthropic: 'https://api.anthropic.com/v1',
};

function trimSlash(s) {
  return String(s || '').replace(/\/+$/, '');
}

// ---------- 请求转换 ----------

function toOpenAI({ model, system, messages, tools, temperature, maxTokens }) {
  const out = [];
  if (system) out.push({ role: 'system', content: system });
  for (const m of messages) {
    if (m.role === 'tool') {
      out.push({ role: 'tool', tool_call_id: m.tool_call_id, content: String(m.content ?? '') });
    } else if (m.role === 'assistant') {
      const msg = { role: 'assistant', content: m.content || '' };
      if (m.tool_calls && m.tool_calls.length) {
        msg.tool_calls = m.tool_calls.map((c) => ({
          id: c.id,
          type: 'function',
          function: { name: c.name, arguments: JSON.stringify(c.arguments || {}) },
        }));
      }
      out.push(msg);
    } else {
      out.push({ role: 'user', content: String(m.content ?? '') });
    }
  }
  const body = { model, messages: out, stream: true };
  if (tools && tools.length) {
    body.tools = tools.map((t) => ({
      type: 'function',
      function: { name: t.name, description: t.description, parameters: t.parameters },
    }));
  }
  if (temperature != null) body.temperature = temperature;
  if (maxTokens) body.max_tokens = maxTokens;
  return body;
}

function toAnthropic({ model, system, messages, tools, temperature, maxTokens }) {
  const out = [];
  const push = (role, block) => {
    const last = out[out.length - 1];
    if (last && last.role === role) last.content.push(block);
    else out.push({ role, content: [block] });
  };
  for (const m of messages) {
    if (m.role === 'tool') {
      push('user', { type: 'tool_result', tool_use_id: m.tool_call_id, content: String(m.content ?? '') });
    } else if (m.role === 'assistant') {
      if (m.content) push('assistant', { type: 'text', text: m.content });
      for (const c of m.tool_calls || []) {
        push('assistant', { type: 'tool_use', id: c.id, name: c.name, input: c.arguments || {} });
      }
    } else {
      push('user', { type: 'text', text: String(m.content ?? '') });
    }
  }
  const body = { model, max_tokens: maxTokens || 8192, messages: out, stream: true };
  if (system) body.system = system;
  if (tools && tools.length) {
    body.tools = tools.map((t) => ({ name: t.name, description: t.description, input_schema: t.parameters }));
  }
  if (temperature != null) body.temperature = temperature;
  return body;
}

// ---------- SSE 解析 ----------

async function* sseEvents(stream) {
  const decoder = new TextDecoder();
  let buf = '';
  for await (const chunk of stream) {
    buf += decoder.decode(chunk, { stream: true });
    let idx;
    while ((idx = buf.search(/\r?\n\r?\n/)) >= 0) {
      const raw = buf.slice(0, idx);
      buf = buf.slice(idx).replace(/^\r?\n\r?\n/, '');
      let event = 'message';
      const data = [];
      for (const line of raw.split(/\r?\n/)) {
        if (line.startsWith('event:')) event = line.slice(6).trim();
        else if (line.startsWith('data:')) data.push(line.slice(5).replace(/^ /, ''));
      }
      if (data.length) yield { event, data: data.join('\n') };
    }
  }
}

function parseArgs(text) {
  if (!text) return {};
  try {
    return JSON.parse(text);
  } catch {
    return { __raw: text };
  }
}

async function* readOpenAI(stream) {
  const calls = new Map();
  let stop = null;
  let usage = null;
  for await (const { data } of sseEvents(stream)) {
    if (data === '[DONE]') break;
    let json;
    try {
      json = JSON.parse(data);
    } catch {
      continue;
    }
    if (json.error) throw new Error(json.error.message || JSON.stringify(json.error));
    if (json.usage) usage = json.usage;
    const choice = json.choices && json.choices[0];
    if (!choice) continue;
    const d = choice.delta || {};
    if (d.content) yield { type: 'text', delta: d.content };
    // 部分推理模型（DeepSeek-R1 等）通过 reasoning_content 返回思考过程
    if (d.reasoning_content) yield { type: 'reasoning', delta: d.reasoning_content };
    for (const tc of d.tool_calls || []) {
      const key = tc.index ?? 0;
      const cur = calls.get(key) || { id: '', name: '', args: '' };
      if (tc.id) cur.id = tc.id;
      if (tc.function && tc.function.name) cur.name += tc.function.name;
      if (tc.function && tc.function.arguments) cur.args += tc.function.arguments;
      calls.set(key, cur);
    }
    if (choice.finish_reason) stop = choice.finish_reason;
  }
  let n = 0;
  for (const c of calls.values()) {
    yield { type: 'tool_call', id: c.id || `call_${Date.now()}_${n++}`, name: c.name, arguments: parseArgs(c.args) };
  }
  yield { type: 'done', stop_reason: calls.size ? 'tool_calls' : stop || 'stop', usage };
}

async function* readAnthropic(stream) {
  const blocks = new Map();
  let stop = null;
  let usage = null;
  for await (const { event, data } of sseEvents(stream)) {
    let json;
    try {
      json = JSON.parse(data);
    } catch {
      continue;
    }
    if (event === 'error' || json.type === 'error') {
      throw new Error((json.error && json.error.message) || 'Anthropic API error');
    }
    if (json.type === 'content_block_start') {
      const b = json.content_block;
      blocks.set(json.index, { type: b.type, id: b.id, name: b.name, args: '' });
    } else if (json.type === 'content_block_delta') {
      const d = json.delta;
      if (d.type === 'text_delta') yield { type: 'text', delta: d.text };
      else if (d.type === 'thinking_delta') yield { type: 'reasoning', delta: d.thinking };
      else if (d.type === 'input_json_delta') {
        const b = blocks.get(json.index);
        if (b) b.args += d.partial_json;
      }
    } else if (json.type === 'content_block_stop') {
      const b = blocks.get(json.index);
      if (b && b.type === 'tool_use') {
        yield { type: 'tool_call', id: b.id, name: b.name, arguments: parseArgs(b.args) };
      }
    } else if (json.type === 'message_delta') {
      if (json.delta && json.delta.stop_reason) stop = json.delta.stop_reason;
      if (json.usage) usage = json.usage;
    }
  }
  yield { type: 'done', stop_reason: stop === 'tool_use' ? 'tool_calls' : stop || 'stop', usage };
}

// ---------- 对外接口 ----------

/**
 * 以统一事件流的形式调用大模型
 * @param {object} req  { provider: { type, baseUrl, apiKey, model }, system, messages, tools, temperature, maxTokens }
 * @param {object} opts { fetchImpl, signal }
 */
async function* chat(req, { fetchImpl = fetch, signal } = {}) {
  const provider = req.provider || {};
  const type = provider.type === 'anthropic' ? 'anthropic' : 'openai';
  if (!provider.model) throw new Error('未配置模型名称');
  const base = trimSlash(provider.baseUrl) || DEFAULT_BASE[type];
  if (!/^https?:\/\//i.test(base)) throw new Error('接口地址必须以 http:// 或 https:// 开头');

  const params = { ...req, model: provider.model };
  let url;
  let headers;
  let body;
  if (type === 'anthropic') {
    url = base + '/messages';
    headers = { 'content-type': 'application/json', 'anthropic-version': '2023-06-01' };
    if (provider.apiKey) headers['x-api-key'] = provider.apiKey;
    body = toAnthropic(params);
  } else {
    url = base + '/chat/completions';
    headers = { 'content-type': 'application/json' };
    if (provider.apiKey) headers.authorization = 'Bearer ' + provider.apiKey;
    body = toOpenAI(params);
  }

  const res = await fetchImpl(url, { method: 'POST', headers, body: JSON.stringify(body), signal });
  if (!res.ok) {
    let detail = '';
    try {
      detail = await res.text();
    } catch {
      /* 忽略 */
    }
    throw new Error(`HTTP ${res.status} ${res.statusText || ''} ${detail.slice(0, 600)}`.trim());
  }
  if (!res.body) throw new Error('接口未返回数据流');
  yield* type === 'anthropic' ? readAnthropic(res.body) : readOpenAI(res.body);
}

module.exports = { chat, toOpenAI, toAnthropic };
