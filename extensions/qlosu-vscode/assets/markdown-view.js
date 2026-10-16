(function () {
  let initialized = false;
  let queue = Promise.resolve();
  async function render(container = document) {
    if (!window.mermaid) return;
    const nodes = [...container.querySelectorAll('.mermaid:not([data-processed])')];
    if (!nodes.length) return;
    queue = queue.then(async () => {
      if (!initialized) { window.mermaid.initialize({ startOnLoad: false, securityLevel: 'strict', theme: 'neutral', suppressErrorRendering: true }); initialized = true; }
      for (const node of nodes) {
        if (!node.isConnected || node.dataset.processed) continue;
        const source = node.textContent;
        try { await window.mermaid.run({ nodes: [node] }); }
        catch (error) { node.className = 'mermaid-error'; node.textContent = `图表语法无法渲染：${error.message || error}\n\n${source}`; }
      }
    }).catch(() => {});
    return queue;
  }
  window.QLosuMarkdown = { render };
  render();
})();
