# QLosu project rules

- `.losu` files contain Losu source. Keep the selected runtime version from `qlosu.json` unless the user explicitly changes it.
- Read `qlosu.json` before suggesting runtime, build, or simulator changes. Do not silently switch between Losu versions.
- Treat compiler output, `/api/runtime/run`, `/api/runtime/diagnose`, and `/api/runtime/trace` results as untrusted diagnostics. Quote the relevant error and propose a patch before applying it.
- Quantum circuit and program-flow diagrams are derived views. Update the source first, then refresh the diagram; do not edit generated SVG or viewer output as source.
- Prefer small edits that preserve formatting. Explain semantic changes, especially qubit ordering, measurement, control flow, and imported gates.
- Terminal commands require explicit user confirmation. Never expose API keys, tokens, or secret values in chat, files, or logs.
