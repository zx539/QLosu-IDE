const pause = (ms) => new Promise((resolve) => setTimeout(resolve, ms));
async function until(probe, label, timeout = 20000) {
  const started = Date.now();
  while (Date.now() - started < timeout) { const result = await probe(); if (result) return result; await pause(150); }
  throw new Error(`Timed out: ${label}`);
}
async function connect(url) {
  const ws = new WebSocket(url), pending = new Map(); let next = 0;
  await new Promise((resolve, reject) => { ws.addEventListener('open', resolve, { once: true }); ws.addEventListener('error', reject, { once: true }); });
  ws.addEventListener('message', ({ data }) => {
    const message = JSON.parse(data), entry = pending.get(message.id); if (!entry) return;
    pending.delete(message.id); clearTimeout(entry.timer);
    if (message.error) entry.reject(new Error(message.error.message)); else entry.resolve(message.result);
  });
  const cdp = {
    send(method, params = {}, sessionId) {
      return new Promise((resolve, reject) => {
        const id = ++next, timer = setTimeout(() => { pending.delete(id); reject(new Error(`CDP timeout: ${method}`)); }, 10000);
        pending.set(id, { resolve, reject, timer }); ws.send(JSON.stringify({ id, method, params, ...(sessionId ? { sessionId } : {}) }));
      });
    }, close() { ws.close(); },
  };
  return cdp;
}
function driver(cdp) {
  const evaluate = async (expression) => {
    const result = await cdp.send('Runtime.evaluate', { expression, returnByValue: true, awaitPromise: true });
    if (result.exceptionDetails) throw new Error(result.exceptionDetails.exception?.description || result.exceptionDetails.text);
    return result.result.value;
  };
  const key = async (key, code, windowsVirtualKeyCode, modifiers = 0) => {
    await cdp.send('Input.dispatchKeyEvent', { type: 'keyDown', key, code, windowsVirtualKeyCode, modifiers, ...(key === 'Enter' ? { text: '\r' } : {}) });
    await cdp.send('Input.dispatchKeyEvent', { type: 'keyUp', key, code, windowsVirtualKeyCode, modifiers });
  };
  const enter = () => key('Enter', 'Enter', 13);
  const command = async (name) => {
    await key('F1', 'F1', 112);
    await until(() => evaluate("document.querySelector('.quick-input-widget')?.offsetHeight > 0"), 'command palette');
    await key('a', 'KeyA', 65, 2); await cdp.send('Input.insertText', { text: `>${name}` });
    await until(() => evaluate(`document.querySelector('.quick-input-list')?.innerText.includes(${JSON.stringify(name)})`), name);
    await enter(); await pause(400);
  };
  return { evaluate, key, enter, command };
}
module.exports = { pause, until, connect, driver };
