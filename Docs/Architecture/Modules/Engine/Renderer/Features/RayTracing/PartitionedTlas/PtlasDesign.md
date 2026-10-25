# Partitioned TLAS Supported Feature Target

[Docs](../../../../../../../README.md) / [Up](../RayTracingDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected post-release support target; current implementation remains experimental and unqualified.
**Inspected:** 2026-10-25, baseline `154c6e883e51689f12feab95646795cc7cbfcdd1`. Existing Renderer partition planner/strategy/build and RHI services are the starting production path.

## Supported Result To Deliver

An engineer selects PTLAS on an eligible machine, renders the same prepared scene/hit/material identity as classic TLAS, and observes actual incremental partition work with a measured useful case. `PGE-02/05/08..10/13` is served by build/traversal tradeoff, native API ownership and trustworthy product support, not a feature toggle.

The current strategy disables instance updates and partition translation and emits at most one operation. [Ray Tracing](../RayTracingDesign.md) and [execution architecture](../ExecutionArchitecture.md) retain that current-state limit. It must not be mistaken for the target below or silently relabeled supported.

| Required target | Bounded promise |
| --- | --- |
| Incremental instance writes | Stable instance identity, transform/mask/material changes, creation/removal and partition reassignment; sparse changed-instance records rather than unconditional full-scene rewrite |
| Partition behavior | Fixed spatial partitions plus bounded global partition policy; migration in/out and capacity/overflow are explicit |
| BLAS replacement | Correct address and SBT contribution updates, with explicit-bounds/update legality checked against the actual provider |
| Partition translation | One qualified whole-partition translation use case, including reset and rebase; no world-streaming framework |
| Shared traversal | Same scene/hit/material/SBT products for classic and PTLAS; no PTLAS shader variants or new effect-specific scene |

Existing source declarations for these operations do not establish that they execute. D3D12 and Vulkan support must be proved separately on actual eligible tuples. Unsupported devices keep classic TLAS available as an explicit choice; a selected PTLAS refusal reports its reason without masquerading as PTLAS success.

## Primary Research And Provider Matrix

The [Vulkan extension proposal](https://docs.vulkan.org/features/latest/features/proposals/VK_NV_partitioned_acceleration_structure.html) and [native acceleration-structure specification](https://docs.vulkan.org/spec/latest/chapters/accelstructures.html) define partition operations and limits. The [NVIDIA sample](https://github.com/nvpro-samples/vk_partitioned_tlas) is precedent for partial-update workloads, not evidence of Sparkle performance or a reason to add physics.

WRITE_INSTANCE carries transforms and partition assignment; UPDATE_INSTANCE changes the permitted BLAS/SBT fields and is not an arbitrary transform update. Partition translation is distinct. Freeze provider-specific legal-use rules, bounds, alignment, uniqueness, capacities and build-size flags in PTLAS-D0, including disabled/reactivated instance behavior. Native documentation and headers outrank sample assumptions.

The current public RHI exposes Vulkan NV, D3D12 NVAPI and D3D12 public RTAS-operation provider identities. PTLAS-D0 verifies which are actually built, available, licensed and executable; it does not require maintaining multiple D3D12 providers for appearance. Select one working D3D12 provider and Vulkan NV for the advertised Windows target. If a required tuple is unavailable, record a blocker rather than calling the target complete. Do not infer eligibility from vendor name or ray-tracing support alone.

## Ownership, Invariants And Budget

Extend `RayTracingPtlasPartitionPlanner`, `RayTracingPartitionedTlasStrategy` and the existing RHI PTLAS service. Prepared scene/GPU-scene identity and `RayTracingShaderTablePlan` remain authoritative. Native packing/sizes/build commands and descriptors stay backend-private; Renderer uses neutral existing RHI descriptions.

Instance index is stable across partial updates and differs from current packed-list position. A generation ledger must reconcile add/remove/reuse/migrate, material alpha/double-sided flags, BLAS generation and SBT contribution. Duplicate writes to an instance in one operation batch are rejected/coalesced before submission. Source/destination/scratch/operation buffers and descriptors use one exact capacity/layout contract in size query and recording.

Budget: reuse the existing PTLAS selector and scene-build hook; narrow existing planner settings to qualified policies. Keep one feature-local compact dirty-update record only where its lifetime is necessary, no second prepared-scene/material copy. GPU-generated operation streams, clustering/CLAS, streaming worlds, simulation and a new AS framework are excluded. Use CPU-authored sparse updates initially; GPU generation needs a later measured consumer.

## PTLAS-D0 And Measurement Card

The Renderer/RHI implementer with native review freezes actual API/provider/GPU/driver/SDK matrix, operation legality, capacity policy, exact identity/update state machine and byte/lifetime budget before implementation. Native feature probes and source inventories may run before release; production follows release and correct scene/ray ownership.

Use a deterministic existing instanced-geometry workload with fixed transforms, not a physics subsystem. Freeze instance counts, partition occupancy, changed fractions, local/global migration and translation scripts. Compare classic rebuild, legal classic refit and PTLAS at identical content/visibility/shader settings. Include CPU planning/packing/upload, AS build, barriers, trace time and total frame cost; report AS/scratch/op/history and replacement peak memory. Freeze quality/parity tolerances, repeat/CI protocol and minimum practical benefit before timing; include a near-all-dynamic losing case.

## Acceptance And Failure Coverage

| Criterion | Smallest required check |
| --- | --- |
| `AC-PTLAS-01` identity | Classic/PTLAS hit-ID/depth/material/alpha/SBT parity with add/remove/reorder/slot reuse and sparse motion |
| `AC-PTLAS-02` operations | Initial/empty/zero-change and changed-instance builds; migration/global capacity, BLAS replacement and whole-partition translation; native legal-use evidence |
| `AC-PTLAS-03` safety | Max/overflow/duplicate operation controls, out-of-bounds/alignment rejection, two Views, resize/scene/device/shutdown and delayed-completion replacement |
| `AC-PTLAS-04` benefit | Paired native runs on required tuples, build-plus-trace/full-frame distributions and complete memory; useful sparse case plus documented losing regime |
| `AC-PTLAS-05` support | Actual requested/active/provider truth, unsupported refusal, packaged shader/native dependency bytes, documented reproduction and honest supported-subset documentation |

`FM-PTLAS-01`: unsupported operation/provider -> reject before recording, preserve selected intent. `FM-PTLAS-02`: capacity/invalid duplicate or identity -> reject batch before partial publication; no silent drop. `FM-PTLAS-03`: stale BLAS/SBT/descriptor -> invalidate/rebuild through existing owner before traversal. `FM-PTLAS-04`: allocation/build/device failure -> diagnosed operation failure with completion-safe cleanup.

Evidence belongs in `ray-tracing/ptlas/` beneath the existing workload artifact root. Existing `RHI-E11`/`REN-E12` and FCR ray families remain first-release owners; these local criteria extend later support proof without retroactively passing those reports. Experimental status changes only after all required cells and product controls pass.
