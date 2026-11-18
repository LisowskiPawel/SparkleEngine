# Texture cooking acceptance

[Migration strategy](../ThirdPartyMigration.md#texture-cooking) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Keep this temporary plan until the remaining acceptance gates close. Lasting ownership, supported inputs, HDR selection, request format, resource admission and publication rules belong to the migration guide. Completed implementation stages and historical recipes have been removed from this plan; their original records remain in ignored evidence archives.

## HDR decision

The user requested both BC6H and RGBA32F, with uncompressed storage used by default. `TextureHdrEncoding::Rgba32Float` is the default; `Bc6h` is explicit. Requests have ten fields, producers serialize the choice, material identities include it, and old requests/content must be regenerated. The processor remains DirectXTex; no second encoder, filter selector or decoder fallback was added.

RGBA32F preserves finite signed HDR values and meaningful alpha. Explicit BC6H keeps its unsigned range and negative-clamping behavior; alpha that BC6H cannot preserve stays uncompressed. Default HDR source-quality checks pass. The original varying-HDR BC6H failure remains recorded separately: selecting BC6H is not a numerical quality waiver.

## Remaining gates

| Gate | Remaining work |
| --- | --- |
| Explicit BC6H quality | The varying-HDR fixture still exceeds the original limits at mips 0-2. Top-level normalized RMS is approximately 0.1605, against 0.04; the old encoder also fails at approximately 0.1486. Keep these failures and the 0.005 regression allowance. Qualify any further encoder change against the unchanged references, or record an explicit acceptance decision for this opt-in limitation. |
| Complete visuals | Focused HDR, alpha over black/white/checker, normal RGB/angular maps, cube faces and array layers have been reviewed. Finish representative material shading, remaining representative corpus review and interactive rendering on both APIs. Diagnostic texture sampling does not close these product-image checks. |
| Controlled costs | Finish the original warm/cold and clean/incremental build comparisons. Use the final identified binaries and preserve all earlier failed or inconclusive runs. Five controlled reboot/cache pairs remain required for cold cooking; do not purge the shared workspace or relabel a first run as cold. Fresh source builds establish reconstruction, not a baseline-relative build-cost pass. |
| Final delivery | Freeze and test the complete product package, its notices/SBOM/provenance and imported/runtime files. Run the admitted source set on a prepared machine without optional codecs or processing SDK installation. Relocated cooker workflows and tool import closure do not establish complete package or physical clean-machine acceptance. |

Retain the original allowances: warm max(10%, 2 ms), cold max(15%, 10 ms), startup max(20%, 10 ms), clean build max(25%, 30 s), incremental build max(10%, 2 s), added fetch 200 MiB, upstream build 512 MiB and tool package 20 MiB. Warm/startup use two discarded warmups, ten alternating pairs and a paired bootstrap 95% interval; overlap with the allowance is inconclusive. Keep the original failed 256 MiB per-request and cooperative-cancellation trials separate from the standalone-tool delivery policy.

## Evidence

All roots below are under ignored `build/validation/texture-cooking/`. Results bind their source, executable, input and toolchain bytes; unrelated moving workspace edits are excluded from the claim.

- `20261112-stage1`: original baseline, corpus and unchanged budgets. `budgets.json` SHA-256 is `c1b56b60c13d735029470a18d6c3b8506be6f8d428ac0a98f045f130aa33609b`.
- `20261117-directxtex-migration`, `20261117-stage5`, `20261117-prerequisite-closure`: dependency/workflow, fault, memory, cancellation, publication and real-reader qualification with original failures retained.
- `20261118-stage6`: original native sampling, source-quality failure, startup, tool size and focused figures. `20261118-cleanup` retains the subsequent structure/footprint and replay receipts.
- `20261118-final-acceptance`: user HDR decision, frozen source reconstruction, four cooker configurations, producer requests, both encoding paths, policy failures, corpus replay, native sampling, focused visual review and Shipping tool delivery inventory. Use its final receipts for exact counts and scope. The complete pre-consolidation plan is in `before.zip`.

Temporary probes, targets, fixtures and binaries must be archived and removed before handoff, with a cleanup receipt. Preserve active dependency caches, product variants and unrelated work. Remove this plan and update roadmap references only after every remaining gate closes.
