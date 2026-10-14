#!/usr/bin/env python3
# 生成 qufu.losu 的 C 内嵌模块文件（构建产物）
# 用法: gen_mod.py <qufu.losu 路径> <输出 .c 路径>
import sys

src_path = sys.argv[1] if len(sys.argv) > 1 else 'qufu.losu'
out_path = sys.argv[2] if len(sys.argv) > 2 else 'qlosu_mod.c'

with open(src_path, encoding='utf-8') as f:
    src = f.read()
escaped = src.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')

def chunk(s, n=60):
    """按 n 字符切分，断点避开转义序列（如 \\"、\\\\）"""
    chunks = []
    i = 0
    while i < len(s):
        j = min(i + n, len(s))
        # 若断点前是反斜杠且它确实是转义符（前面反斜杠数为偶数），则后退一位
        if j < len(s) and s[j - 1] == '\\':
            k = j - 1
            while k > i and s[k - 1] == '\\':
                k -= 1
            if (j - 1 - k) % 2 == 0:
                j -= 1
        chunks.append(s[i:j])
        i = j
    return chunks


lines = []
lines.append('/* 由 qufu.losu 自动生成的模块内嵌文件（构建产物） */')
lines.append('#include <stddef.h>')
lines.append('#include "losu.h"')
lines.append('')
lines.append('const char qufu_losu[] = \\')
for piece in chunk(escaped):
    lines.append('  "%s"' % piece)
lines.append('  "";')
lines.append('const size_t qufu_losu_len = sizeof qufu_losu;')
lines.append('')
lines.append('struct losu_module_t losu_module_index[] = {')
lines.append('  { "qufu", qufu_losu, qufu_losu_len },')
lines.append('};')
lines.append('size_t losu_module_nindex')
lines.append('    = sizeof losu_module_index / sizeof losu_module_index[0];')

with open(out_path, 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines) + '\n')
print('generated:', out_path)
