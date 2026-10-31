# First Release RHI And GPU Execution Plan

[Docs](../../../../README.md) / [Up](RHIDesign.md)

Prove that the selected frame and product transitions lower correctly to D3D12/Vulkan, with bounded native lifetime and truthful failure.

**Status:** remaining release integration and proof; stage names do not imply missing implementation or passed evidence.

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

Assign the demonstrated failure or open native claim to its service. RHI-0 closes only missing consumer/capability decisions; existing services and qualified GCD/native evidence are reused. Inventory coverage does not require every extension or service to be reimplemented before repairing a frame.

Run the stage that owns the demonstrated missing contract or failing check. Stage numbers identify responsibilities; they do not require rebuilding existing layers or completing unrelated stages first. Every stage and copied prompt follows the [Stage Source-Style Gate](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the [shared delivery contract](../../../CrossModule/FirstRelease/ImplementationPlan.md#phase-card-contract).

## Dependencies

Resource ownership precedes use; completion precedes reuse or retirement. Ray/HDR work uses current native owners through its feature discovery. Paired checks follow each affected shared contract, rather than a separate late port. RHI-6 joins final candidate results.

## `RHI-0` — Freeze The Service And Backend Matrix

**Start:** A selected Renderer request lacks a current native tuple, consumer or decidable support/failure contract.

**Work:** Trace that request through its service/native owner, build/SDK/device prerequisites and lifetime. Reconcile affected D3D12/Vulkan capability/failure cells in existing dossiers; consider narrowing unused public surface rather than implementing it.

**Cleanup:** Remove stale capability claims and duplicate service definitions; no native Renderer policy or convenience RHI.

**Exit:** Each affected admitted cell has a real lowering or named blocker, consumer and AC/FM/check; unselected extensions stay outside this stage.

```text
Execute only RHI-0 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-0--freeze-the-service-and-backend-matrix. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Trace that request through its service/native owner, build/SDK/device prerequisites and lifetime.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to the stage's existing report owner; no neighboring stage or new mechanism merely to fill a report.
```

## `RHI-1` — Device, Resources, Descriptors, And Lifetime

**Start:** A selected device/resource/descriptor creation, partial failure or retirement edge needs closure.

**Work:** Preserve owner-thread aggregate publication and reverse teardown. Check affected buffer/texture/view/sampler/descriptor/upload/readback/pipeline inputs, generation/bounds/mapping/state and allocation failures; prove all-queue completion before recycling. Record changed descriptor/memory high-water.

**Cleanup:** Delete duplicate wrappers, copied ownership/state and replaced descriptor/resource-lifetime machinery; retain one published device aggregate.

**Exit:** FCR-RHI-01 and applicable FCR-RHI-06 lifecycle cells pass both-API valid/invalid/partial-create/allocation and native live-object checks.

```text
Execute only RHI-1 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-1--device-resources-descriptors-and-lifetime. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Preserve owner-thread aggregate publication and reverse teardown.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-RHI-01, FCR-RHI-06; no neighboring stage or new mechanism merely to fill a report.
```

## `RHI-2` — Command Recording, Synchronization, Resize, And Present

**Start:** Valid resources exist; recording/submission/completion or product presentation is the affected edge.

**Work:** Check lease/frame/token identity, batches/order/waits/barriers, recycling and frames-in-flight; exercise affected minimize/zero-size/resize/out-of-date/acquire/present/cancel/shutdown. HDR native work runs through its admitted leaf stage, not a duplicate sequence here. Measure changed submit/present/wait/pacing cost.

**Cleanup:** Remove redundant token/state/wait/swapchain paths; lower graph intent without pass policy or unconditional waits.

**Exit:** FCR-RHI-02 affected normal/serial, transition/failure and lifetime cells pass both APIs with clean or properly dispositioned native diagnostics.

```text
Execute only RHI-2 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-2--command-recording-synchronization-resize-and-present. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Check lease/frame/token identity, batches/order/waits/barriers, recycling and frames-in-flight; exercise affected minimize/zero-size/resize/out-of-date/acquire/present/cancel/shutdown.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-RHI-02; no neighboring stage or new mechanism merely to fill a report.
```

## `RHI-3` — Paired Backend Lowering

**Start:** A retained common contract changed or lacks required paired-native proof.

**Work:** Compare affected descriptor/format/pipeline/command/state/queue/interop/present/error cells with identical logical inputs. Repair missing lowering at its backend owner; retain intentional native differences behind the shared invariant. Reuse representative frame and focused invalid-input controls.

**Cleanup:** Delete duplicate mapping/capability truth and native bypass replaced by the fix. Do not demand identical command streams or implement unselected extensions.

**Exit:** FCR-RHI-03 admitted cells meet owned result/failure tolerances on both APIs. Missing Included support stays blocked; scope changes go through the release owner.

```text
Execute only RHI-3 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-3--paired-backend-lowering. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Compare affected descriptor/format/pipeline/command/state/queue/interop/present/error cells with identical logical inputs.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-RHI-03; no neighboring stage or new mechanism merely to fill a report.
```

## `RHI-4` — Ray-Tracing And Acceleration-Structure Execution

**Start:** Selected scene/TLAS policy and required shader/RT composition are valid.

**Work:** Check admitted BLAS/TLAS build/update/refit, scratch/result/operation bounds, contribution/SBT/hit identity, queues/state/retirement and allocation failures. Use current inline/native controls and shader composition/record/dispatch checks. PTLAS retains its experimental disposition; scene policy remains Renderer-owned.

**Cleanup:** Remove duplicate native tables/mappings/handle state and unsupported fallback producers replaced by the repair.

**Exit:** FCR-RHI-04 required native RT/AS cells pass both APIs with identity, bounds, failure and cost evidence. No new extension, compaction tier or traversal architecture is implied.

```text
Execute only RHI-4 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-4--ray-tracing-and-acceleration-structure-execution. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Check admitted BLAS/TLAS build/update/refit, scratch/result/operation bounds, contribution/SBT/hit identity, queues/state/retirement and allocation failures.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-RHI-04; no neighboring stage or new mechanism merely to fill a report.
```

## `RHI-5` — Diagnostics, Capture, Device Loss, And Recovery

**Start:** An admitted readback/native diagnostic/failure cell is missing or fails; qualified crash tooling is reused where applicable.

**Work:** Exercise affected timestamp validity, names/messages/loss, memory/live objects, destination-free readback and external correlation. Distinguish recoverable swapchain events from terminal device loss; prove settlement/no post-loss use, candidate/frame/object provenance and local privacy. Missing automatic CPU crash/hang collection stays unavailable.

**Cleanup:** Remove duplicate capture/diagnostic state, stale callbacks/provider leases and temporary fault probes; no internal profiler, device resurrection or support service.

**Exit:** FCR-RHI-05/06 affected capture, loss/failure/shutdown and observer/privacy cells pass. Artifacts identify the actual candidate; terminal loss is never called recovery.

```text
Execute only RHI-5 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-5--diagnostics-capture-device-loss-and-recovery. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Exercise affected timestamp validity, names/messages/loss, memory/live objects, destination-free readback and external correlation.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-RHI-05; no neighboring stage or new mechanism merely to fill a report.
```

## `RHI-6` — Candidate Closure

**Start:** Final package/shader/provider identities and admitted native results exist.

**Work:** Join six reports and the paired backend/driver/configuration matrix. Run only missing package-relative or cross-boundary checks; reuse unchanged GCD/native artifacts. Invalidate claims affected by device/provider/driver/code changes.

**Cleanup:** Verify prior wrapper/state/registration/dependency deletions and remove stale current instructions; unique artifacts remain linked.

**Exit:** Each admitted FCR-RHI-01 through 06 claim has an exact decision and required negatives. Unrun hardware cells cannot pass; unselected services create no implementation task.

```text
Execute only RHI-6 from Docs/Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-6--candidate-closure. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Join six reports and the paired backend/driver/configuration matrix.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-RHI-01; no neighboring stage or new mechanism merely to fill a report.
```

## Reference And Ownership

[RHI design](RHIDesign.md) and [feature dossiers](Features/RHIFeatureGuide.md) own semantics and AC/FM/checks. [Device/resources](Features/DeviceAndResources/DeviceAndResourcesDesign.md), [execution](Features/PipelineAndExecution/PipelineAndExecutionDesign.md), [presentation](Features/PresentationAndInterop/PresentationAndInteropDesign.md) and [capture](Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md) remain owners; this plan assigns release work.
