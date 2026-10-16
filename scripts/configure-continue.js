#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

const root = path.resolve(__dirname, '..');
const backends = new Set(['ollama', 'openai-compatible', 'anthropic']);
const args = process.argv.slice(2);
const value = (name) => {
  const index = args.indexOf(name);
  return index >= 0 ? args[index + 1] : undefined;
};
const workspace = path.resolve(value('--workspace') || process.cwd());
const backend = value('--backend') || 'ollama';
if (!backends.has(backend)) {
  console.error(`Unsupported backend: ${backend}. Choose ${[...backends].join(', ')}.`);
  process.exit(2);
}

const destination = path.join(workspace, '.continue');
const configPath = path.join(destination, 'config.yaml');
const rulesPath = path.join(destination, 'rules', 'qlosu.md');
if (fs.existsSync(configPath)) {
  console.error(`Refusing to overwrite existing ${configPath}`);
  process.exit(1);
}
fs.mkdirSync(path.dirname(rulesPath), { recursive: true });
fs.copyFileSync(path.join(root, 'config', 'continue', `${backend}.yaml`), configPath);
fs.copyFileSync(path.join(root, 'config', 'continue', 'qlosu-rules.md'), rulesPath);
console.log(`Created ${configPath}`);
console.log(`Created ${rulesPath}`);
console.log('Configure Continue secrets in its secret store before starting a remote model.');
