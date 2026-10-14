/**
 * 工作区文件管理：所有路径限制在工作区根目录内，防止路径穿越
 */
const fs = require('fs');
const path = require('path');
const os = require('os');

const MAX_FILE = 2 * 1024 * 1024;
const IGNORED = new Set(['.git', 'node_modules', '.DS_Store']);

class Workspace {
  constructor(root) {
    this.setRoot(root || process.env.QLOSU_WORKSPACE || path.join(os.homedir(), 'QLosu-Workspace'));
  }

  setRoot(root) {
    this.root = path.resolve(root);
  }

  _ensure() {
    fs.mkdirSync(this.root, { recursive: true });
  }

  /** 将相对路径解析为工作区内的绝对路径，越界时抛错 */
  resolve(rel) {
    const clean = String(rel || '').replace(/\\/g, '/').replace(/^\/+/, '');
    if (clean.split('/').some((seg) => seg === '..')) throw httpError(400, '非法路径');
    const abs = path.resolve(this.root, clean);
    const relCheck = path.relative(this.root, abs);
    if (relCheck.startsWith('..') || path.isAbsolute(relCheck)) throw httpError(400, '非法路径');
    return abs;
  }

  list() {
    this._ensure();
    const out = [];
    const walk = (dir, depth) => {
      if (depth > 6 || out.length > 2000) return;
      const entries = fs.readdirSync(dir, { withFileTypes: true })
        .filter((e) => !IGNORED.has(e.name))
        .sort((a, b) => (b.isDirectory() - a.isDirectory()) || a.name.localeCompare(b.name));
      for (const e of entries) {
        const abs = path.join(dir, e.name);
        const rel = path.relative(this.root, abs).split(path.sep).join('/');
        if (e.isDirectory()) {
          out.push({ path: rel, type: 'dir' });
          walk(abs, depth + 1);
        } else if (e.isFile()) {
          out.push({ path: rel, type: 'file', size: fs.statSync(abs).size });
        }
      }
    };
    walk(this.root, 0);
    return out;
  }

  read(rel) {
    const abs = this.resolve(rel);
    if (!fs.existsSync(abs) || !fs.statSync(abs).isFile()) throw httpError(404, '文件不存在');
    if (fs.statSync(abs).size > MAX_FILE) throw httpError(413, '文件过大');
    return fs.readFileSync(abs, 'utf8');
  }

  write(rel, content) {
    if (typeof content !== 'string') throw httpError(400, '缺少文件内容');
    if (Buffer.byteLength(content) > MAX_FILE) throw httpError(413, '文件过大');
    const abs = this.resolve(rel);
    if (abs === this.root) throw httpError(400, '非法路径');
    fs.mkdirSync(path.dirname(abs), { recursive: true });
    fs.writeFileSync(abs, content, 'utf8');
  }

  remove(rel) {
    const abs = this.resolve(rel);
    if (abs === this.root) throw httpError(400, '不能删除工作区根目录');
    if (!fs.existsSync(abs)) throw httpError(404, '文件不存在');
    fs.rmSync(abs, { recursive: true });
  }

  rename(from, to) {
    const a = this.resolve(from);
    const b = this.resolve(to);
    if (a === this.root || b === this.root) throw httpError(400, '非法路径');
    if (!fs.existsSync(a)) throw httpError(404, '文件不存在');
    if (fs.existsSync(b)) throw httpError(409, '目标已存在');
    fs.mkdirSync(path.dirname(b), { recursive: true });
    fs.renameSync(a, b);
  }
}

function httpError(status, message) {
  const e = new Error(message);
  e.status = status;
  return e;
}

module.exports = { Workspace, httpError };
