# 0012 内置洛书 AI Agent

- **优先级：** P1
- **状态：** 流式代理与工具/差异确认、Markdown 已验收；真实模型仍待配置
- **范围：** `extensions/qlosu-vscode/agent.js`、`server/llm.js`、`extensions/qlosu-vscode/package.json`
- **依赖：** 0002、0003、0004

## 本次完成

- 在洛书活动栏增加独立的 `AI Agent` 标签页和 `QLosu: Open AI Agent` 命令。
- 默认连接本机 Ollama 的 OpenAI 兼容接口，也支持 OpenAI/DeepSeek/vLLM 等兼容服务和 Anthropic Messages API。
- Agent 可读取工作区文件、文件列表、当前诊断，运行当前 `.losu` 文件，打开量子线路或复杂程序流程图。
- `write_file` 和 `run_terminal` 工具始终要求 VS Code 模态确认；模型不能直接访问扩展进程或绕过工作区路径检查。
- 文件、选区、诊断和文件列表作为可选上下文；工具结果通过统一流式 NDJSON 协议回传，最多处理 8 轮工具调用。
- API key 使用 VS Code SecretStorage 保存，模型地址、协议和模型名保存在用户状态中，不写入仓库。

## 验证与边界

`extensions/qlosu-vscode/test/agent.test.js` 覆盖工具清单、默认配置、分片 NDJSON、CSP 和 Agent webview；模型联网聊天、模型质量和工具确认交互仍需要在目标机器配置实际 Ollama/API 后验收。

## 0014 后续进展

补充：0014 实测模拟服务经 HTTP 代理流式返回工具调用、扩展执行工具并回传结果的闭环；文件写入先打开真实差异预览，取消不会写入，拒绝符号链接逃逸。回复使用富 Markdown。当前本机 Ollama 端口未启动，真实模型与质量未标记完成；可用 agent:check 显式发送合成探针。
