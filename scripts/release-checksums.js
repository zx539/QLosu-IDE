#!/usr/bin/env node
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const dist = path.resolve(__dirname, '../dist');
const artifacts = fs.readdirSync(dist).filter((name) => /^QLosu-IDE-.*\.(tar\.gz|AppImage|deb|rpm)$/.test(name));
if (!artifacts.length) throw new Error('没有可生成校验和的 Linux 发布产物');
fs.writeFileSync(path.join(dist, 'SHA256SUMS'), artifacts.sort().map((name) => `${crypto.createHash('sha256').update(fs.readFileSync(path.join(dist, name))).digest('hex')}  ${name}`).join('\n') + '\n');
console.log(`PASS ${artifacts.length} 个发布产物的 SHA-256 清单`);
