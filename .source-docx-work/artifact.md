# Source-code template execution contract

## Reference

- Source: `/mnt/d/软件杯-基于大模型的个性化资源生成与学习多智能体系统/QLosu-standalone/源代码模板.docx`
- SHA-256: `af76359c12c8c4ba6fa8fe4ad09032e4e87dabb20b34ab524c5a578b6dcdc030`
- Baseline: 61 Word-rendered pages, 1 section, 2931 body paragraphs, no tables or images.
- Evidence: `.source-docx-work/template.pdf` and `.source-docx-work/template-render/`.

## Page system

- A4 portrait: 8.2681 x 11.6931 inches.
- Margins: top 0.1965 inch, bottom 0.2125 inch, left 0.9368 inch, right 0.9764 inch.
- Header distance 0.2757 inch; footer distance 0.2361 inch.
- One section. Header contains the software name; footer contains an automatic page number.

## Typography and line model

- Body style: `HTML Preformatted`.
- Typeface: SimSun / 宋体, 12 pt (小四), black, left aligned.
- One source-code line per Word paragraph.
- No paragraph spacing, indentation, blank paragraphs, tables, figures or prose blocks.
- User override: exactly 50 nonblank, comment-free source lines per page.

## Content flow and provenance

- Page 1: JavaScript server and WASM invocation code from `index.js`.
- Page 2: WebAssembly build/export configuration from `src/wasm-build/build.sh`.
- Page 3: C bridge code from `src/wasm-build/ffi_wasm.c`.
- Page 4: HTML interface structure from `www/index.html`.
- Page 5: CSS interface styling from the `<style>` block in `www/index.html`.
- Excluded: every path under `src/**/QuEST/**`, `src/**/quest/**`, generated QuEST implementation, and binary disassembly from `wasm/qlosu.m.wasm`.

## Slot map

- All 2931 template body paragraphs are replaceable code-line slots.
- Section geometry, the header component, footer page-number field, styles, numbering and theme are preserve-only.
- Header text may be updated from the old Losu product name to the requested QLosu product name while retaining its paragraph/run formatting.

## Fidelity gates

- Reference file remains byte-for-byte unchanged.
- Final document remains A4 portrait with the recorded margins, header distance and footer distance.
- Final has exactly 5 pages, 250 body paragraphs and 50 code lines per page.
- Every body paragraph is nonblank, uses 宋体 12 pt, and contains no source comments.
- No code is sourced from a QuEST directory or from `.wasm` disassembly.
- Word-rendered pages must have no wrapped code lines, clipping, overlap or extra blank page.
