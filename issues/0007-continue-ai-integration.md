# 0007 Continue 完整 AI 体验接入

- **优先级：** P1
- **状态：** 配置模板与安装入口已完成；联网端到端聊天待验收
- **范围：** Code-Server 启动器、扩展配置、项目文档

## 背景

设计已确定使用官方 Continue 扩展。启动器支持可选安装和显式版本锁定；仓库提供 Ollama、OpenAI 兼容接口和 Anthropic 配置模板，以及 QLosu 项目规则。实际模型服务和 Code-Server 在线聊天仍需在目标机器联网配置后验收。

## 待完成

- 在目标环境固定并验证 Continue 的兼容版本（启动器支持 `QLOSU_CONTINUE_VERSION`）。
- 提供本地 Ollama、OpenAI 兼容 API 和 Anthropic 配置模板。
- 提供 Losu 文件、运行输出、错误和图表数据的上下文入口（通过 QLosu 规则和 Continue 文件上下文）。
- 让 AI 修改文件使用 VS Code 编辑预览/确认流程。
- 所有终端命令默认弹出 VS Code 确认。
- 明确 API Key 存储位置和日志脱敏规则。

## 验收标准

- Continue 可以在 Code-Server 中正常聊天、读取项目上下文、生成补丁。
- AI 能理解 `.losu`、`qlosu.json`、运行错误和量子轨迹。
- 未确认的命令不会执行；密钥不会写入仓库或普通日志。
