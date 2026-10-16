# Continue AI in QLosu

QLosu uses the official `Continue.continue` extension as an optional Code-Server
companion. The launcher does not enable network installation unless explicitly
requested:

```bash
QLOSU_INSTALL_CONTINUE=1 npm run code-server
```

To make an installation reproducible, pin the extension version after checking
that it works with the bundled Code-Server/VS Code version (`4.95.3`):

```bash
QLOSU_INSTALL_CONTINUE=1 QLOSU_CONTINUE_VERSION=VERSION npm run code-server
```

`VERSION` is intentionally supplied by the operator. Marketplace availability
and Continue compatibility change independently of QLosu, so the repository does
not claim an online version that has not been tested on the target machine.

## Configure a model

The repository includes templates for the supported backends:

- `config/continue/ollama.yaml` for a local Ollama model.
- `config/continue/openai-compatible.yaml` for OpenAI, DeepSeek, vLLM, or another compatible endpoint.
- `config/continue/anthropic.yaml` for Anthropic Messages API.

Create a project-local config and QLosu rules with:

```bash
npm run continue:setup -- --backend ollama --workspace "$PWD"
```

Use `openai-compatible` or `anthropic` as the backend when needed. The command
refuses to overwrite `.continue/config.yaml`; this keeps an existing setup and
its review history intact. Edit the generated `apiBase` and model name for the
provider in use.

API keys must be entered in Continue's secret store (or supplied through its
documented secret environment mechanism). They must never be committed to
`.continue/config.yaml`, `qlosu.json`, or a source file. Add `.continue` to a
private/local ignore file if the project is shared and the config contains local
provider details.

## QLosu context and safety

The generated `.continue/rules/qlosu.md` tells Continue how to use the following
project context:

- `.losu` source and `qlosu.json` runtime selection.
- VS Code problems and language-server diagnostics.
- Terminal output and runtime error/trace results.
- Quantum circuit and program-flow diagrams as derived views.
- Current file, workspace code, and pending diff.

Continue's edit/apply actions remain subject to the editor's normal review flow.
Review a diff before applying it. Terminal commands require explicit confirmation
in the Code-Server terminal; the QLosu rules forbid the assistant from treating a
command as approved merely because it was suggested. Logs and chat transcripts
must not contain API keys or token values.

The templates are configuration inputs, not a bundled model service. End-to-end
chat, context retrieval, and patch application still need verification after the
chosen provider is available on the local machine.

Before starting Code-Server, validate the offline templates and installation
arguments with:

```bash
npm run continue:check
```

This confirms the repository templates contain no committed secret and reports
the exact `--install-extension` argument when `QLOSU_INSTALL_CONTINUE=1` is set.
