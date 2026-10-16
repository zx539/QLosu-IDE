#!/usr/bin/env node
// Explicit opt-in live probe; sends only fixed synthetic context and no files.
const { chat } = require('../server/llm');
async function main() {
  const baseUrl = process.env.QLOSU_AGENT_BASE_URL;
  const model = process.env.QLOSU_AGENT_MODEL;
  if (!baseUrl || !model) throw new Error('请设置 QLOSU_AGENT_BASE_URL 和 QLOSU_AGENT_MODEL；不会自动调用外部服务或读取工程文件。');
  const provider = { type: process.env.QLOSU_AGENT_PROTOCOL === 'anthropic' ? 'anthropic' : 'openai', baseUrl, model, apiKey: process.env.QLOSU_AGENT_API_KEY || '' };
  const request = { provider, messages: [{ role: 'user', content: 'This is a synthetic QLosu connectivity test. Reply with QLOSU_AGENT_OK and no other text.' }], maxTokens: 64 };
  let answer = '';
  for await (const event of chat(request, { signal: AbortSignal.timeout(60000) })) {
    if (event.type === 'text') answer += event.delta || '';
    if (event.type === 'error') throw new Error('模型服务返回错误，请检查本机配置。');
  }
  if (!answer.includes('QLOSU_AGENT_OK')) throw new Error('服务已响应，但未返回预期探针标记。');
  console.log('PASS 真实模型连接与流式回复（仅发送固定测试文本）');
}
main().catch((error) => { console.error(error.message.replace(/(?:sk-|key-)[\w-]+/g, '<REDACTED>')); process.exitCode = 1; });
