#!/usr/bin/env node
const fs = require('node:fs');
const path = require('node:path');

const root = path.resolve(__dirname, '..');
const configRoot = path.join(root, 'config', 'continue');
const required = ['ollama.yaml', 'openai-compatible.yaml', 'anthropic.yaml', 'qlosu-rules.md'];
for (const name of required) {
  const file = path.join(configRoot, name);
  if (!fs.existsSync(file) || !fs.statSync(file).isFile()) throw new Error(`缺少 Continue 模板：${name}`);
  const text = fs.readFileSync(file, 'utf8');
  if (!text.trim()) throw new Error(`Continue 模板为空：${name}`);
  if (/sk-[A-Za-z0-9]{16,}|api[_-]?key:\s*['"]?[^$\s]/i.test(text)) throw new Error(`Continue 模板疑似包含密钥：${name}`);
}
const rules = fs.readFileSync(path.join(configRoot, 'qlosu-rules.md'), 'utf8');
for (const phrase of ['qlosu.json', 'diagnostics', 'confirmation']) if (!rules.toLowerCase().includes(phrase.toLowerCase())) throw new Error(`Continue 规则缺少关键约束：${phrase}`);
const manager = require('../apps/code-server/manager');
const args = manager.continueInstallArgs();
if (process.env.QLOSU_INSTALL_CONTINUE === '1' && !args.includes('--install-extension')) throw new Error('Continue 安装参数生成失败');
console.log(JSON.stringify({ ok: true, templates: required, installRequested: process.env.QLOSU_INSTALL_CONTINUE === '1', installArgs: args }, null, 2));
