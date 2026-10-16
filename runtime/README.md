# Losu 运行时

版本运行时通过适配器统一管理。工作台通过列表选择版本，普通运行和 Agent 使用相同选择。

`losu-1.0` 使用 `src/losu-1.0` 中的 Gitee lpk 源码。运行 `npm run losu1:build` 构建原生解释器和标准库，产物位于 `runtime/losu1/<platform>-<arch>/`；示例为 `examples/losu1/hello.els`。此代码线未包含量子库，量子示例使用 2.0 WASM。

`losu-2.0.0` 使用仓库内
`src/losu v2.0.0/src/test/makefile` 构建 C 源码静态归档，并通过仓库提供的
`scripts/losu2-cli.js` 暴露 `check/build/run/trace` 入口。当前上游 C 树仍缺少
VM 初始化和脚本加载实现，因此 `run/trace` 使用已验证的 QLosu WASM 后端；CLI
输出会明确标明后端，不会把 C 静态归档冒充原生解释器。不会自动降级到其他版本。
外部版本在 `qlosu.json` 的 `runtimes` 中配置，支持
`versionArgs`、`runArgs`、`traceArgs`、`env`、`cwd` 与 `timeoutMs`。

```bash
# 查看命令
npm run losu -- --help
# 只做源文件结构诊断
npm run losu -- check examples/hello.losu
# 编译洛书 2.0 C 源码模块（输出 src/test/liblosu-v2.a）
npm run losu:build
# 使用当前 QLosu WASM 后端运行示例
npm run losu -- run examples/hello.losu
```

```json
{"losuVersion":"losu-system","runtimes":[{"id":"losu-system","command":"losu","runArgs":["run"],"traceArgs":["trace"],"timeoutMs":30000}]}
```
