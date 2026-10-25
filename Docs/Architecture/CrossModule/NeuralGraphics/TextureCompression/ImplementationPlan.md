# NTC Staged Delivery

[Docs](../../../../README.md) / [Up](NtcDesign.md)

**Status:** selected target sequence; discovery and prerequisite gates remain open. No stage is executed by this plan.
**Contract:** [design, semantics, measurement and local acceptance](NtcDesign.md); [Roadmap](../../../../Strategy/Roadmap.md) owns global order.

## Work This Plan

After REL-11, qualified material/reference inputs and M3's classical measurement handoff. Deliver one immutable on-sample material profile; keep the owned neural model work and NRC evidence distinct. Execute the first unfinished stage only. The discovery card freezes numerical tolerances, practical-effect bands, exact eligibility, rights, hook/copy/resource limits and failure checks before production; a source-present mechanism is not a satisfied gate.

## Stage Contract

Every stage inspects HEAD/status, applicable AGENTS.md, live producers/consumers/lifetime/build membership and concurrent edits. Apply [Change Integration](../../../../Engineering/Workflow/ChangeIntegration.md), [Change Lifecycle](../../../../Engineering/Workflow/ChangeLifecycle.md), relevant Renderer/RHI/Tools standards and the [Stage Source-Style Gate](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), including authored readability/spacing review after formatting. Discovery/no-source work records justified source-style N/A.

Ledger every external feature hook and material copy; default to existing semantic inputs, one feature-local owner, zero new public types and no framework. Each stage removes replaced state/paths/selectors/dependencies in the same slice and checks retained consumers. A zero-deletion stage names why no replaced path exists. Temporary probes stay local and are removed before handoff; keep valuable source/commands/results as ignored evidence.

Each exit records exact candidate/dirty state, API/device/tool/content/shader/SDK/artifact identities, command, oracle, observation and remaining cells in `neural-textures/ntc/` under the existing workload artifact root. Use the cheapest defect-detecting check; production/native/quality/cost/package/adoption verdicts remain distinct. Stop for missing semantics, required unavailable proof, unsafe lifetime, quality change, silent fallback or a hook outside the frozen budget.

## NTC-S0 Freeze NTC-D0

**Start:** Planning and installed-environment inspection only.

**Work:** Pin source/rights/compressor/decoder toolchain and actual API/device support. Freeze channel/color/normal/mip/filtering semantics, BCn baseline, held-out material split and numerical/value/peak-memory gates. Select a non-preview production route.

**Cleanup:** Reject sparse-feedback/streaming, unused texture classes and generic codec/plugin APIs; no sample explorer import.

**Exit:** NTC-D0 artifact/capability/filtering decisions and failure checks are implementable; unsupported required tuples remain blocked.

```text
Execute only NTC-S0 from Docs/Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## NTC-S1 Cook one real latent material set

**Start:** NTC-D0; original material ownership and rights qualified.

**Work:** Invoke the pinned offline compressor from the existing cooker and publish immutable channel/mip/profile/latent/weight metadata via Assets. Verify payload sizes/hashes and repeat policy; reject malformed/partial output transactionally.

**Cleanup:** Delete one-off compressor launch wrappers, duplicated manifests and decode-on-load detours that are not selected; keep BCn source baseline.

**Exit:** AC-NTC-01 passes for a real artifact without adding runtime training or internal format migration.

```text
Execute only NTC-S1 from Docs/Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## NTC-S2 Implement shared on-sample decode

**Start:** NTC-S1 and installed shader/native eligibility.

**Work:** Upload through existing residency/MaterialCache; decode real channels in the common raster/ray material helper. Qualify texel values, linear/sRGB, normal reconstruction and scalar ranges against independent decompression fixtures.

**Cleanup:** Replace duplicate decoder bindings/texture-set records; no separate NTC material cache or RHI facade.

**Exit:** AC-NTC-02 passes on D3D12/Vulkan for the selected supported decoder; actual shader sampling is traceable to artifact bytes.

```text
Execute only NTC-S2 from Docs/Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## NTC-S3 Qualify filtering and temporal behavior

**Start:** Decoder conformance; frozen LOD/filtering contract.

**Work:** Implement the selected mip/derivative or ray-cone and filtering policy. Run grazing/minification/magnification, moving camera, normal/highlight and mip-transition cases. Verify retained BCn alpha/emission behavior and history interaction.

**Cleanup:** Remove experimental sampling variants and unsupported knobs; share only actual common sampling semantics.

**Exit:** AC-NTC-03 and semantic failure controls pass; no unfiltered texel comparison is presented as texture-sampling parity.

```text
Execute only NTC-S3 from Docs/Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## NTC-S4 Measure the memory compute frontier

**Start:** Sampling/material quality qualified and held-out rules frozen.

**Work:** Measure held-out shaded/raw/temporal error, compression time, package/resident/upload/workspace bytes and full-frame costs in memory-pressure and shader-bound cases. Add one supported accelerated decode profile only if conformance and whole-product value justify it.

**Cleanup:** Keep the justified profile(s) with explicit capability needs; delete losing production variants and preview-only dependencies.

**Exit:** AC-NTC-04 meets frozen quality/value gates and discloses decode/filtering cost and complete memory.

```text
Execute only NTC-S4 from Docs/Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## NTC-S5 Package replace and reproduce

**Start:** Technical cells pass; exact distributable bytes available.

**Work:** Exercise bad payload/profile/hash, unsupported device, upload/allocation/compile failures and in-flight replacement. Package the actual runtime/cooked/shader/license tuple; the author or agent reproduces material and value results.

**Cleanup:** Remove source training sets, sample/test executables and codec build cache from package/source membership; retire replaced paths without deleting BCn baseline.

**Exit:** AC-NTC-05 and all previous local cells pass with truthful ownership and supported subset.

```text
Execute only NTC-S5 from Docs/Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```
