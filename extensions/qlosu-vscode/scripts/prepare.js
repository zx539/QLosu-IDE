const fs = require('fs');
const path = require('path');

const extensionRoot = path.resolve(__dirname, '..');
const source = path.resolve(extensionRoot, '..', '..', 'language-server', 'losu-lsp', 'server.js');
const destination = path.join(extensionRoot, 'language-server', 'server.js');
fs.mkdirSync(path.dirname(destination), { recursive: true });
fs.copyFileSync(source, destination);
fs.copyFileSync(path.resolve(extensionRoot, '../../apps/server/core/markdown.js'), path.join(extensionRoot, 'markdown.js'));
fs.mkdirSync(path.join(extensionRoot, 'manual', 'guide'), { recursive: true });
fs.copyFileSync(path.resolve(extensionRoot, '../../使用说明书.md'), path.join(extensionRoot, 'manual', 'guide', '使用说明书.md'));
console.log(`Copied Losu language server to ${path.relative(extensionRoot, destination)}`);

const projectRoot = path.resolve(extensionRoot, '..', '..');
for (const [from, to] of [
  ['src/QLosu/洛书语法', 'manual/2.0.0'],
  ['src/losu-1.0/document', 'manual/1.0'],
]) {
  const target = path.join(extensionRoot, to);
  fs.rmSync(target, { recursive: true, force: true });
  fs.cpSync(path.join(projectRoot, from), target, { recursive: true, filter: (file) => !['.git', 'LosuBook-syntaxs'].includes(path.basename(file)) });
}
const marked = path.join(projectRoot, 'node_modules', 'marked', 'marked.min.js');
fs.mkdirSync(path.join(extensionRoot, 'vendor', 'marked'), { recursive: true });
fs.copyFileSync(marked, path.join(extensionRoot, 'vendor', 'marked', 'marked.min.js'));
fs.copyFileSync(path.join(projectRoot, 'node_modules', 'marked', 'LICENSE.md'), path.join(extensionRoot, 'vendor', 'marked', 'LICENSE.md'));
fs.mkdirSync(path.join(extensionRoot, 'vendor', 'mermaid'), { recursive: true });
fs.copyFileSync(path.join(projectRoot, 'node_modules', 'mermaid', 'dist', 'mermaid.min.js'), path.join(extensionRoot, 'vendor', 'mermaid', 'mermaid.min.js'));
fs.copyFileSync(path.join(projectRoot, 'node_modules', 'mermaid', 'LICENSE'), path.join(extensionRoot, 'vendor', 'mermaid', 'LICENSE'));
fs.cpSync(path.join(projectRoot, 'node_modules', 'katex', 'dist'), path.join(extensionRoot, 'vendor', 'katex'), { recursive: true });
fs.copyFileSync(path.join(projectRoot, 'node_modules', 'katex', 'LICENSE'), path.join(extensionRoot, 'vendor', 'katex', 'LICENSE'));
fs.copyFileSync(path.join(projectRoot, 'legacy', 'web-ide', 'www', 'css', 'markdown.css'), path.join(extensionRoot, 'assets', 'markdown.css'));
fs.copyFileSync(path.join(projectRoot, 'legacy', 'web-ide', 'www', 'js', 'core', 'markdown-view.js'), path.join(extensionRoot, 'assets', 'markdown-view.js'));
console.log('Copied offline syntax manuals (1.0 / 2.0) and Markdown renderer');
