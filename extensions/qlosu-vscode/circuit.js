const esc = (s) => String(s).replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
const gateNames = { H: '叠加（Hadamard）', X: '翻转（X）', Z: '相位翻转（Z）', RX: '绕 X 轴旋转', CX: '受控翻转（CNOT）', CCX: '双控制翻转（Toffoli）', M: '测量' };

function describe(op, index) {
  const targets = (op.targets || []).map((q) => `q${q}`).join('、');
  const controls = (op.controls || []).map((q) => `q${q}`).join('、');
  return `第 ${index + 1} 步：${gateNames[op.gate] || op.gate} · ${controls ? `控制 ${controls} → ` : ''}${targets}${op.param != null ? `，角度 ${Number(op.param).toFixed(3)} 弧度` : ''}${op.gate === 'M' ? `，本次结果 ${op.result}` : ''}`;
}

function circuitContent(trace) {
  const registers = trace.registers || [];
  const options = registers.map((reg) => `<option value="${esc(reg.id)}">寄存器 ${esc(reg.id)} · ${esc(reg.nQubits)} 个量子比特</option>`).join('');
  const views = registers.map((reg, regIndex) => {
    const allOps = trace.ops.filter((op) => op.reg === reg.id), ops = allOps.slice(0, 400);
    const rows = Math.max(reg.nQubits || 0, 1, ...ops.flatMap((op) => [...op.controls, ...op.targets]).map((q) => q + 1));
    const width = Math.max(360, 110 + ops.length * 68), height = rows * 64 + 54;
    let svg = `<svg role="img" aria-label="寄存器 ${esc(reg.id)} 的量子线路，从左向右阅读" viewBox="0 0 ${width} ${height}" style="width:${width}px;height:${height}px" xmlns="http://www.w3.org/2000/svg">`;
    for (let q = 0; q < rows; q++) {
      const y = 52 + q * 64;
      svg += `<text class="qubit" x="12" y="${y + 5}">q${q}</text><line class="wire" x1="68" y1="${y}" x2="${width - 16}" y2="${y}"/>`;
    }
    ops.forEach((op, index) => {
      const x = 102 + index * 68, target = op.targets[0], y = 52 + target * 64;
      svg += `<text class="step-number" x="${x}" y="16" text-anchor="middle">${index + 1}</text><g class="gate gate-${esc(op.gate)}" role="button" tabindex="0" data-step="${index}" aria-label="${esc(describe(op, index))}"><title>${esc(describe(op, index))}</title>`;
      if (op.controls.length) {
        const ys = [...op.controls, target].map((q) => 52 + q * 64);
        svg += `<line class="control-line" x1="${x}" y1="${Math.min(...ys)}" x2="${x}" y2="${Math.max(...ys)}"/>`;
        for (const control of op.controls) svg += `<circle class="control-dot" cx="${x}" cy="${52 + control * 64}" r="5"/>`;
        svg += `<circle class="target" cx="${x}" cy="${y}" r="16"/><path class="control-line" d="M${x - 10},${y}h20 M${x},${y - 10}v20"/>`;
      } else {
        svg += `<rect x="${x - 23}" y="${y - 21}" width="46" height="42" rx="7"/><text class="gate-label" x="${x}" y="${y + 5}" text-anchor="middle">${esc(op.gate === 'M' ? `M=${op.result}` : op.gate)}</text>`;
      }
      svg += '</g>';
    });
    svg += '</svg>';
    return `<section class="register-view" data-register="${esc(reg.id)}" ${regIndex ? 'hidden' : ''}><p class="circuit-summary">${rows} 个量子比特 · ${allOps.length} 步操作${allOps.length > ops.length ? '（显示前 400 步）' : ''}。${ops.length ? '点击量子门查看说明。' : '尚未执行量子门。'}</p><div class="circuit-scroll">${svg}</div><p class="gate-detail" role="status">${ops.length ? esc(describe(ops[0], 0)) : '每一条横线代表一个量子比特。'}</p><details><summary>逐步说明</summary><ol>${ops.map((op, i) => `<li><button class="step-button" data-step="${i}">${esc(describe(op, i))}</button></li>`).join('')}</ol></details></section>`;
  }).join('');
  return `<div class="circuit-guide"><strong>从左向右看：</strong>每条横线是一个量子比特，方块是操作；● 是控制位，⊕ 是被翻转的目标位。</div><label>查看寄存器 <select id="circuit-register">${options}</select></label><div class="circuit-legend"><span>H · 叠加</span><span>X · 翻转</span><span>Z · 相位</span><span>RX · 旋转</span><span>M · 测量结果</span></div>${views}<p class="circuit-note">M=0 / M=1 表示本次执行的测量结果；再次运行可能得到不同结果。步骤编号表示实际执行顺序。</p>`;
}

const circuitStyles = `.circuit-guide{padding:12px;background:var(--vscode-textBlockQuote-background);border-left:3px solid var(--vscode-focusBorder);line-height:1.7;margin-bottom:16px}.circuit-legend{display:flex;flex-wrap:wrap;gap:8px;margin:14px 0}.circuit-legend span{padding:4px 9px;border:1px solid var(--vscode-panel-border);border-radius:5px}.circuit-scroll{overflow:auto;border:1px solid var(--vscode-panel-border);border-radius:8px;padding:12px}.circuit-scroll svg{display:block}.wire{stroke:var(--vscode-descriptionForeground);stroke-width:1.5}.qubit{fill:var(--vscode-foreground);font:14px monospace}.step-number{fill:var(--vscode-descriptionForeground);font:12px system-ui}.gate{cursor:pointer;outline:none}.gate rect{fill:#dbeafe;stroke:#2563eb;stroke-width:2}.gate-X rect{fill:#ede9fe;stroke:#7c3aed}.gate-Z rect{fill:#dcfce7;stroke:#16803c}.gate-M rect{fill:#fef3c7;stroke:#b45309}.gate-label{fill:#172033;font:bold 12px system-ui;pointer-events:none}.control-line{stroke:var(--vscode-foreground);stroke-width:2}.control-dot{fill:var(--vscode-foreground)}.target{fill:var(--vscode-editor-background);stroke:var(--vscode-foreground);stroke-width:2}.gate.selected rect,.gate:focus rect,.gate.selected .target,.gate:focus .target{stroke:#f97316;stroke-width:4}.gate-detail{min-height:24px;padding:12px;border-radius:6px;background:var(--vscode-textCodeBlock-background)}.step-button{background:transparent;color:var(--vscode-textLink-foreground);border:0;text-align:left;padding:5px;cursor:pointer}.circuit-summary,.circuit-note{color:var(--vscode-descriptionForeground);line-height:1.6}select{color:var(--vscode-dropdown-foreground);background:var(--vscode-dropdown-background);padding:6px;border:1px solid var(--vscode-dropdown-border)}details{margin:12px 0}`;

const circuitScript = `document.getElementById('circuit-register').addEventListener('change',event=>{document.querySelectorAll('[data-register]').forEach(section=>section.hidden=section.dataset.register!==event.target.value);});function selectGate(target){const section=target.closest('[data-register]');if(!section)return;const step=target.dataset.step;section.querySelectorAll('.gate').forEach(gate=>gate.classList.toggle('selected',gate.dataset.step===step));const gate=section.querySelector('.gate[data-step="'+step+'"]');if(gate){section.querySelector('.gate-detail').textContent=gate.getAttribute('aria-label');gate.scrollIntoView({block:'nearest',inline:'nearest'});}}document.addEventListener('click',event=>{const target=event.target.closest('[data-step]');if(target)selectGate(target);});document.addEventListener('keydown',event=>{if((event.key==='Enter'||event.key===' ')&&event.target.matches('.gate')){event.preventDefault();selectGate(event.target);}});`;

module.exports = { circuitContent, circuitStyles, circuitScript };
