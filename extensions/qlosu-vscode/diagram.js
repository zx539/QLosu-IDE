const crypto = require('node:crypto');
const { circuitContent, circuitStyles, circuitScript } = require('./circuit');

function escapeHtml(value) {
  return String(value).replace(/[&<>"']/g, (char) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[char]));
}

function diagramHtml(kind, fileName, result, { viewer, cspSource = 'vscode-webview:' }) {
  const simpleCircuit = kind === 'circuit' && result?.trace?.registers?.length > 0;
  const xml = kind === 'circuit' ? result?.circuitXml : result?.xml;
  const nonce = crypto.randomBytes(16).toString('base64');
  const csp = `default-src 'none'; img-src ${cspSource} data: blob:; style-src ${cspSource} 'unsafe-inline'; script-src 'nonce-${nonce}'; font-src ${cspSource} data:;`;
  // Escape the complete JSON attribute. Otherwise the HTML parser decodes XML
  // entities before JSON.parse, corrupting labels containing quotes or ampersands.
  const config = escapeHtml(JSON.stringify({ xml, nav: true, toolbar: 'pages zoom layers', resize: true }));
  const state = xml ? '正在加载图表…' : kind === 'circuit'
    ? '当前程序没有量子操作，无法生成量子线路。' : '服务未返回流程图数据。';
  const graph = simpleCircuit ? circuitContent(result.trace) : xml ? `<div class="mxgraph" id="diagram" data-mxgraph='${config}'></div>` : '';
  const scripts = simpleCircuit ? `<script nonce="${nonce}">${circuitScript}</script>` : xml ? `<script nonce="${nonce}">
let rendered=false;
function showDiagramError(error){
  const state=document.getElementById('diagram-state');
  state.hidden=false;state.className='state error';state.setAttribute('role','alert');
  state.textContent='图表渲染失败：'+(error&&error.message||String(error));
}
// The bundled viewer calls this hook once after loading; avoid its automatic
// processElements plus a second invocation, which recreates the same graph.
window.onDrawioViewerLoad=function(){
  try{
    if(!window.GraphViewer)throw new Error('draw.io viewer 未加载');
    const container=document.getElementById('diagram');
    const parsed=new DOMParser().parseFromString(${JSON.stringify(xml).replace(/</g, '\\u003c')},'text/xml');
    if(parsed.querySelector('parsererror'))throw new Error('服务返回的图表 XML 无效');
    window.GraphViewer.createViewerForElement(container,function(viewer){
      if(!viewer.graph||!container.querySelector('svg'))throw new Error('draw.io 未生成图表');
      rendered=true;document.getElementById('diagram-state').hidden=true;
    });
  }catch(error){showDiagramError(error);}
};
window.addEventListener('error',function(event){
  if(rendered)return;
  if(event.target&&event.target.tagName==='SCRIPT')showDiagramError(new Error('draw.io viewer 资源加载失败'));
  else if(event.message)showDiagramError(new Error(event.message));
},true);
setTimeout(function(){if(!rendered)showDiagramError(new Error('draw.io viewer 加载超时，请重新打开图表'));},8000);
</script><script nonce="${nonce}" src="${escapeHtml(viewer.toString())}"></script>` : '';
  const controls = `<script nonce="${nonce}">const qlosuWebview=acquireVsCodeApi();const refreshButton=document.getElementById('refresh-diagram');if(refreshButton)refreshButton.addEventListener('click',()=>qlosuWebview.postMessage({type:'refresh'}));window.addEventListener('message',function(event){const state=document.getElementById('diagram-state');if(!state)return;if(event.data&&event.data.type==='refreshing'){state.hidden=false;state.className='state';state.textContent='正在更新图表…';}if(event.data&&event.data.type==='refreshError'){state.hidden=false;state.className='state error';state.setAttribute('role','alert');state.textContent='图表刷新失败：'+event.data.message;}});</script>`;
  return `<!doctype html><html><head><meta charset="utf-8"><meta http-equiv="Content-Security-Policy" content="${csp}"><meta name="viewport" content="width=device-width,initial-scale=1"><style>
html,body{min-height:100%;margin:0}body{box-sizing:border-box;padding:16px;font:14px system-ui;color:var(--vscode-foreground);background:var(--vscode-editor-background)}.toolbar{display:flex;align-items:center;gap:10px}.toolbar h2{flex:1;margin:0 0 6px;font-size:18px}.toolbar button{color:inherit;background:var(--vscode-button-secondaryBackground);border:1px solid var(--vscode-panel-border);border-radius:3px;padding:4px 10px;cursor:pointer}.toolbar button:hover{background:var(--vscode-button-secondaryHoverBackground)}.filename{margin:0 0 12px;color:var(--vscode-descriptionForeground);overflow-wrap:anywhere}.mxgraph{width:100%;height:calc(100vh - 100px);min-height:420px}.state{padding:12px;border:1px solid var(--vscode-panel-border);border-radius:4px}.state.error{color:var(--vscode-errorForeground)}[hidden]{display:none!important}${simpleCircuit ? circuitStyles : ''}
</style></head><body><div class="toolbar"><h2>${kind === 'circuit' ? '量子线路' : '程序流程图'}</h2><button id="refresh-diagram" type="button" title="重新生成图表">刷新</button></div><p class="filename">${escapeHtml(fileName)}</p><p class="state" id="diagram-state" role="status" ${simpleCircuit ? 'hidden' : ''}>${state}</p>${graph}${scripts}${controls}</body></html>`;
}

module.exports = { diagramHtml };
