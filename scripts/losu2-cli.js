#!/usr/bin/env node
/*
 * Losu v2 command line entry point.
 *
 * The C tree under src/losu v2.0.0 is still missing the VM bootstrap and
 * executable loader. This CLI keeps that fact visible: `build` compiles the
 * C sources into a static archive, `check` performs source diagnostics, and
 * `run`/`trace` use the bundled QLosu WASM runtime until the native VM is
 * complete.
 */
'use strict';

const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const ROOT = path.resolve(__dirname, '..');
const TEST_DIR = path.join(ROOT, 'src', 'losu v2.0.0', 'src', 'test');
const MAKEFILE = path.join(TEST_DIR, 'makefile');

function usage() {
  process.stdout.write([
    '洛书 2.0 CLI',
    '',
    '用法:',
    '  losu --version',
    '  losu check <file.losu> [file.losu ...]',
    '  losu build',
    '  losu run <file.losu>',
    '  losu trace <file.losu>',
    '',
    'build 编译 src/losu v2.0.0 的 C 源码并生成 src/test/liblosu-v2.a。',
    'run/trace 当前使用仓库内的 QLosu WASM 后端；原生 C VM 完成后再切换。',
    '',
  ].join('\n'));
}

function fail(message, code = 2) {
  process.stderr.write(`losu: ${message}\n`);
  process.exitCode = code;
}

function readSource(file) {
  const absolute = path.resolve(process.cwd(), file);
  if (!fs.existsSync(absolute) || !fs.statSync(absolute).isFile()) {
    throw new Error(`找不到源文件：${file}`);
  }
  if (path.extname(absolute).toLowerCase() !== '.losu') {
    throw new Error(`只支持 .losu 源文件：${file}`);
  }
  return { absolute, source: fs.readFileSync(absolute, 'utf8') };
}

function diagnostics(source) {
  const result = [];
  const stack = [];
  let line = 1;
  let column = 0;
  let quote = null;
  let escaped = false;
  let lineComment = false;
  let blockComment = false;
  const pairs = { ')': '(', ']': '[', '}': '{' };
  const opens = new Set(['(', '[', '{']);

  for (let i = 0; i < source.length; i++) {
    const ch = source[i];
    const next = source[i + 1];
    column++;
    if (ch === '\n') {
      line++;
      column = 0;
      lineComment = false;
      continue;
    }
    if (lineComment) continue;
    if (blockComment) {
      if (ch === '*' && next === '/') {
        blockComment = false;
        i++;
        column++;
      }
      continue;
    }
    if (quote) {
      if (escaped) escaped = false;
      else if (ch === '\\') escaped = true;
      else if (ch === quote) quote = null;
      continue;
    }
    if (ch === '/' && next === '/') {
      lineComment = true;
      i++;
      column++;
      continue;
    }
    if (ch === '/' && next === '*') {
      blockComment = true;
      i++;
      column++;
      continue;
    }
    if (ch === '"' || ch === "'" || ch === '`') {
      quote = ch;
      continue;
    }
    if (opens.has(ch)) {
      stack.push({ ch, line, column });
    } else if (Object.prototype.hasOwnProperty.call(pairs, ch)) {
      const previous = stack.pop();
      if (!previous) {
        result.push({ severity: 'error', line, column, message: `多余的闭合符号 ${ch}` });
      } else if (previous.ch !== pairs[ch]) {
        result.push({ severity: 'error', line, column, message: `闭合符号 ${ch} 与 ${previous.ch} 不匹配` });
      }
    }
  }

  if (quote) result.push({ severity: 'error', line, column, message: `字符串没有闭合（${quote}）` });
  if (blockComment) result.push({ severity: 'error', line, column, message: '块注释没有闭合（/* ... */）' });
  for (let i = stack.length - 1; i >= 0; i--) {
    const item = stack[i];
    const closing = { '(': ')', '[': ']', '{': '}' }[item.ch];
    result.push({ severity: 'error', line: item.line, column: item.column, message: `缺少闭合符号 ${closing}` });
  }
  return result.sort((a, b) => a.line - b.line || a.column - b.column);
}

function printDiagnostics(file, items, json) {
  if (json) {
    process.stdout.write(`${JSON.stringify({ file, diagnostics: items })}\n`);
    return;
  }
  for (const item of items) process.stdout.write(`${file}:${item.line}:${item.column}: ${item.severity}: ${item.message}\n`);
}

function runProcess(command, args, options = {}) {
  return new Promise((resolve, reject) => {
    const child = spawn(command, args, { ...options, stdio: ['ignore', 'pipe', 'pipe'] });
    let stdout = '';
    let stderr = '';
    let timedOut = false;
    const timeoutMs = Number(options.timeoutMs) || 120000;
    const timer = setTimeout(() => {
      timedOut = true;
      child.kill('SIGTERM');
    }, timeoutMs);
    child.stdout.on('data', (chunk) => { stdout += chunk; });
    child.stderr.on('data', (chunk) => { stderr += chunk; });
    child.on('error', (error) => { clearTimeout(timer); reject(error); });
    child.on('exit', (code, signal) => {
      clearTimeout(timer);
      resolve({ code: code == null ? 1 : code, signal, stdout, stderr, timedOut });
    });
  });
}

async function build() {
  if (!fs.existsSync(MAKEFILE)) throw new Error(`未找到构建文件：${MAKEFILE}`);
  const result = await runProcess('make', ['-f', path.basename(MAKEFILE), 'liblosu-v2.a'], { cwd: TEST_DIR, timeoutMs: 120000 });
  process.stdout.write(result.stdout);
  process.stderr.write(result.stderr);
  if (result.code !== 0) process.exitCode = result.code;
  else process.stdout.write(`洛书 2.0 C 源码构建完成：${path.join(TEST_DIR, 'liblosu-v2.a')}\n`);
}

async function execute(file, command) {
  const { absolute, source } = readSource(file);
  const errors = diagnostics(source);
  if (errors.some((item) => item.severity === 'error')) {
    printDiagnostics(file, errors, false);
    process.exitCode = 1;
    return;
  }
  // Runtime is loaded lazily so --version and check work without WASM files.
  const { Runtime } = require('../server/runtime');
  const { instrument } = require('../server/instrument');
  const runtime = new Runtime();
  try {
    const prepared = command === 'trace' ? instrument(source) : { code: source, instrumented: false };
    const result = await runtime.run(prepared.code, { timeoutMs: 120000 });
    if (result.stdout) process.stdout.write(result.stdout);
    if (result.ret !== 0 || result.timedOut) {
      if (result.timedOut) process.stderr.write('losu: 执行超时\n');
      process.exitCode = result.timedOut ? 124 : 1;
    }
    if (command === 'trace' && prepared.instrumented) {
      process.stderr.write(`losu: trace 使用 QLosu WASM 后端（${absolute}）\n`);
    }
  } finally {
    runtime.close();
  }
}

async function main(argv) {
  const args = argv.slice(2);
  const json = args.includes('--json');
  const filtered = args.filter((arg) => arg !== '--json');
  const command = filtered[0];
  if (!command || command === '--help' || command === '-h') return usage();
  if (command === '--version' || command === '-v' || command === 'version') {
    process.stdout.write('Losu v2.0.0 CLI (QLosu WASM backend)\n');
    return;
  }
  if (command === 'build') return build();
  if (command === 'check') {
    const files = filtered.slice(1);
    if (!files.length) return fail('check 需要至少一个 .losu 文件');
    let failed = false;
    for (const file of files) {
      try {
        const { source } = readSource(file);
        const items = diagnostics(source);
        printDiagnostics(file, items, json);
        if (items.some((item) => item.severity === 'error')) failed = true;
        else if (!json) process.stdout.write(`${file}: 通过\n`);
      } catch (error) {
        fail(error.message, 1);
        failed = true;
      }
    }
    if (failed) process.exitCode = 1;
    return;
  }
  if (command === 'run' || command === 'trace') {
    const file = filtered[1];
    if (!file) return fail(`${command} 需要一个 .losu 文件`);
    try { return await execute(file, command); } catch (error) { return fail(error.message, 1); }
  }
  return fail(`未知命令：${command}`);
}

if (require.main === module) main(process.argv).catch((error) => fail(error.message, 1));

module.exports = { diagnostics, main };

