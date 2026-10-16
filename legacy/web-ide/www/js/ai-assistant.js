/**
 * AI 助手模块
 * 支持多种后端：OpenAI API、Anthropic API、本地 Ollama
 */
(function (root, factory) {
  if (typeof module === 'object' && module.exports) module.exports = factory();
  else root.AIAssistant = factory();
})(typeof self !== 'undefined' ? self : this, function () {
  'use strict';

  class AIAssistant {
    constructor(config) {
      this.config = config || this.loadConfig();
      this.history = [];
    }

    loadConfig() {
      const stored = localStorage.getItem('qlosu_ai_config');
      if (stored) return JSON.parse(stored);
      return {
        provider: 'ollama', // ollama | openai | anthropic
        ollama: { baseURL: 'http://localhost:11434', model: 'qwen2.5-coder:7b' },
        openai: { apiKey: '', baseURL: 'https://api.openai.com/v1', model: 'gpt-4' },
        anthropic: { apiKey: '', model: 'claude-3-5-sonnet-20241022' },
      };
    }

    saveConfig(config) {
      this.config = config;
      localStorage.setItem('qlosu_ai_config', JSON.stringify(config));
    }

    async chat(message, context = {}) {
      const { provider } = this.config;
      if (provider === 'ollama') return this.chatOllama(message, context);
      if (provider === 'openai') return this.chatOpenAI(message, context);
      if (provider === 'anthropic') return this.chatAnthropic(message, context);
      throw new Error('未配置 AI 提供商');
    }

    async *streamChat(message, context = {}) {
      const { provider } = this.config;
      if (provider === 'ollama') yield* this.streamOllama(message, context);
      else if (provider === 'openai') yield* this.streamOpenAI(message, context);
      else if (provider === 'anthropic') yield* this.streamAnthropic(message, context);
      else throw new Error('未配置 AI 提供商');
    }

    async *streamOllama(message, context) {
      const { baseURL, model } = this.config.ollama;
      const systemPrompt = `你是 QLosu 量子编程 IDE 的 AI 助手。
用户正在使用 Losu 量子编程语言开发量子算法。
Losu 语法：
- 导入：var { println } import qufu.IO;
- 量子寄存器：var reg = QuReg(2);
- 量子门：reg[[0]] :> Hadamard();
- 控制门：reg[[1]] :> ControlledPauliX(reg[[0]]);
- 测量：Measure(reg[[0]])
请用简洁的中文回答，优先给出代码示例。`;

      const messages = [
        { role: 'system', content: systemPrompt },
        ...this.history,
        { role: 'user', content: this.buildPrompt(message, context) },
      ];

      const res = await fetch(`${baseURL}/api/chat`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ model, messages, stream: true }),
      });

      if (!res.ok) throw new Error(`Ollama 错误: ${res.status}`);

      const reader = res.body.getReader();
      const decoder = new TextDecoder();
      let buffer = '';

      while (true) {
        const { done, value } = await reader.read();
        if (done) break;
        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split('\n');
        buffer = lines.pop();

        for (const line of lines) {
          if (!line.trim()) continue;
          try {
            const data = JSON.parse(line);
            if (data.message?.content) {
              yield { type: 'text', content: data.message.content };
            }
            if (data.done) return;
          } catch (e) {
            console.warn('解析 Ollama 响应失败:', line, e);
          }
        }
      }
    }

    async *streamOpenAI(message, context) {
      const { apiKey, baseURL, model } = this.config.openai;
      if (!apiKey) throw new Error('未配置 OpenAI API Key');

      const messages = [
        { role: 'system', content: 'You are a quantum programming assistant for QLosu IDE.' },
        ...this.history,
        { role: 'user', content: this.buildPrompt(message, context) },
      ];

      const res = await fetch(`${baseURL}/chat/completions`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          Authorization: `Bearer ${apiKey}`,
        },
        body: JSON.stringify({ model, messages, stream: true }),
      });

      if (!res.ok) throw new Error(`OpenAI 错误: ${res.status}`);

      const reader = res.body.getReader();
      const decoder = new TextDecoder();
      let buffer = '';

      while (true) {
        const { done, value } = await reader.read();
        if (done) break;
        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split('\n');
        buffer = lines.pop();

        for (const line of lines) {
          if (!line.startsWith('data: ')) continue;
          const data = line.slice(6);
          if (data === '[DONE]') return;
          try {
            const parsed = JSON.parse(data);
            const delta = parsed.choices?.[0]?.delta?.content;
            if (delta) yield { type: 'text', content: delta };
          } catch (e) {}
        }
      }
    }

    async *streamAnthropic(message, context) {
      const { apiKey, model } = this.config.anthropic;
      if (!apiKey) throw new Error('未配置 Anthropic API Key');

      const messages = [
        ...this.history,
        { role: 'user', content: this.buildPrompt(message, context) },
      ];

      const res = await fetch('https://api.anthropic.com/v1/messages', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'x-api-key': apiKey,
          'anthropic-version': '2023-06-01',
        },
        body: JSON.stringify({
          model,
          max_tokens: 4096,
          messages,
          stream: true,
          system: 'You are a quantum programming assistant for QLosu IDE.',
        }),
      });

      if (!res.ok) throw new Error(`Anthropic 错误: ${res.status}`);

      const reader = res.body.getReader();
      const decoder = new TextDecoder();
      let buffer = '';

      while (true) {
        const { done, value } = await reader.read();
        if (done) break;
        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split('\n');
        buffer = lines.pop();

        for (const line of lines) {
          if (!line.startsWith('data: ')) continue;
          const data = line.slice(6);
          try {
            const parsed = JSON.parse(data);
            if (parsed.type === 'content_block_delta' && parsed.delta?.text) {
              yield { type: 'text', content: parsed.delta.text };
            }
          } catch (e) {}
        }
      }
    }

    buildPrompt(message, context) {
      let prompt = message;
      if (context.code) {
        prompt += `\n\n当前代码：\n\`\`\`losu\n${context.code}\n\`\`\``;
      }
      if (context.selection) {
        prompt += `\n\n选中代码：\n\`\`\`losu\n${context.selection}\n\`\`\``;
      }
      if (context.error) {
        prompt += `\n\n错误信息：\n${context.error}`;
      }
      return prompt;
    }

    async chatOllama(message, context) {
      let response = '';
      for await (const chunk of this.streamOllama(message, context)) {
        if (chunk.type === 'text') response += chunk.content;
      }
      return response;
    }

    async chatOpenAI(message, context) {
      let response = '';
      for await (const chunk of this.streamOpenAI(message, context)) {
        if (chunk.type === 'text') response += chunk.content;
      }
      return response;
    }

    async chatAnthropic(message, context) {
      let response = '';
      for await (const chunk of this.streamAnthropic(message, context)) {
        if (chunk.type === 'text') response += chunk.content;
      }
      return response;
    }

    addToHistory(role, content) {
      this.history.push({ role, content });
      if (this.history.length > 20) this.history = this.history.slice(-20);
    }

    clearHistory() {
      this.history = [];
    }
  }

  return AIAssistant;
});
