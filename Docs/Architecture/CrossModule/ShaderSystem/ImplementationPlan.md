# Shader System Delivery Plan

[Docs](../../../README.md) / [Up](ShaderSystemDesign.md)

Close the current shader, pipeline and ray-execution contract with focused compiler, ABI, native, lifetime and adoption evidence. Repair an observed discrepancy in its current owner; do not replay the source migration.

**Status:** remaining source-contract reconciliation and executable proof; no candidate verdict is granted here.

## Work This Plan

Start with the smallest unresolved [CHK-SHS-*](Acceptance.md#controlled-failures-and-key-checks) for the selected release frame. Existing source checkpoints live in [Delivery Evidence](DeliveryEvidence.md); architecture and vocabulary live in [Shader System Design](ShaderSystemDesign.md). Source delivery does not pass a check, but an old migration prompt does not make delivered code unfinished again.

## Remaining Stages

| Task | Necessary input | Output consumed by the release |
| --- | --- | --- |
| [Current contract/deletion floor](#current-contract-and-deletion-floor) | Actual program/binding/runtime failure or a stale source assumption. | One current authority per source/catalog/job/map/library/pipeline/table/effect responsibility. |
| [Compiler and publication proof](#compiler-and-publication-proof) | Exact selected registrations, targets and current toolchain. | Valid current cooked code/ABI with transactional failure and dependency identity. |
| [Runtime and tooling proof](#runtime-and-tooling-proof) | Valid cooked generation and the existing product workflow. | Correct typed execution, reload/retirement and a source-to-native provenance trace. |
| [Phase 12 candidate closure](#phase-12---regenerate-validate-and-hand-off-the-complete-shader-graphics-pipeline-and-ray-tracing-candidate) | Compatible focused results and final delivery inputs. | FCR-SHDR-01 and its Renderer/RHI evidence consumers, with exact limitations. |

These are proof dependencies, not four framework implementations. A valid preceding result is reused. A failed check returns only to its demonstrated owner. First-release proof uses admitted release content and focused conformance controls; later Bistro/San Miguel evidence stays in its own workload program.

## Execution Contract

Use [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md), its [source-style gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), [scope/retirement](../../../Engineering/Workflow/ChangeLifecycle.md#scope-and-retirement-decision) and [claim-driven validation](../../../Engineering/Verification/ValidationAndEvidence.md#claim-driven-validation-selection). Each repair names the selected consumer, failed invariant, smallest change and same-stage deletion. Keep mechanism in existing tool/Renderer/RHI owners, one typed contract and all-queue retirement. Preserve unrelated changes and leave source-control actions to the user.

No permutations, precache/preload, driver cache, compiler service, cache browser, duplicate schema/generation, compatibility reader, diagnostic product or permanent test-only machinery is selected. Check IDs authorize evidence, not a runner or production fault API. A missing mandatory render product fails before scheduling.

## Current Contract And Deletion Floor

**Start:** a source-contract failure, failed acceptance check or stale declaration must be reconciled.

Trace definitions to their actual producers/consumers/build membership. Use the [canonical identity model](ShaderSystemDesign.md#required-identity-model), [ownership](ShaderSystemDesign.md#ownership-and-dependency-boundaries), [preserve/delete/defer decisions](ShaderSystemDesign.md#preserve-improve-delete-and-defer) and [deletion floor](Acceptance.md#current-contract-and-deletion-floor). Correct only the demonstrated current delta. Phases 0-11 are historical migration identities, not instructions to rebuild sources, registration, maps, RT execution or Apply Changed.

**Cleanup/exit:** the repair deletes replaced fields, wrappers, aliases, registrations, CMake/include references, dual paths and disposable output in the same slice. A clean final search is verification of prior deletions, not permission to postpone them. Preserve unique historical source/measurement records. If no production delta is needed, proceed directly to its missing proof.

```text
Reconcile only the selected current-contract discrepancy from Docs/Architecture/CrossModule/ShaderSystem/ImplementationPlan.md. Use its actual failed AC/CHK and current owner. Preserve delivered code, delete only the replaced responsibility and run its smallest falsifying check. Do not replay historical phases 0-11. Apply the owning execution contract and its Stage Source-Style Gate.
```

## Compiler And Publication Proof

**Start:** current source/catalog declarations and exact compiler/target policy are known.

Run CHK-SHS-02/03 and applicable CHK-SHS-04 on the selected contract: virtual source/dependency/input hashes, DXIL/SPIR-V reflection/layout/stage/export identity, job fan-out, repeated cook, serial/1/2/N, cancellation, portable replay, corruption/stale replacement and transactional publication. Compile/cook breadth follows the claimed program/target matrix; a focused ABI failure stops unrelated runtime work. Regenerate changed products through the existing current representation, without old-format readers or schema versions.

**Cleanup/exit:** remove temporary invalid inputs and probes; retain exact raw failures and hashes. Valid code/map/library products reach the real runtime consumer, or the corresponding claim stays blocked. No persistent results cache or generated support-report subsystem is needed.

```text
Execute only the selected compiler/publication checks from Docs/Architecture/CrossModule/ShaderSystem/ImplementationPlan.md. Freeze actual inputs, targets and oracle before results; reuse current tooling and repair the first failing owner. Retain current products and raw evidence, remove temporary additions and state which runtime claim remains unproved. Apply the owning execution contract and its Stage Source-Style Gate.
```

## Runtime And Tooling Proof

**Start:** required cooked code/ABI is valid and the selected native/product path is available.

Use CHK-SHS-05/06 and applicable CHK-SHS-04 for typed graphics/compute/inline/RT binding, exact pre-record pipeline identity, independent capabilities, composition/SBT arithmetic, selected-effect resolution and generation retirement. The enduring [native conformance, graphics-key and same-frame GBuffer oracles](Acceptance.md#native-conformance-and-comparison-oracles) own exact sentinels, tolerances and negative cases. Reuse one immutable input for Inline/Pipeline comparison; production still schedules one selected frontend.

Exercise current Apply Changed, changed-include closure, failed replacement, direct source navigation/replay and delayed all-queue completion. Record any missing frontend behavior as a bounded current repair, not a repeat of Phase 10. Use existing native capture/source/symbol joins and separate optimized timing from instrumented runs; collect cold/warm, creation/table/update, frame and memory costs only where the affected acceptance claim requires them. Device recreation may be a focused lifetime control where supported; this plan does not add public automatic recovery.

**Cleanup/exit:** delete duplicate effect/lookup/generation/status paths in their repair; remove comparison resources, fault injection and local fixtures before handoff. Native output, failures and provenance satisfy the selected checks on both required APIs or remain explicitly blocked.

```text
Execute only the selected runtime/tooling proof from Docs/Architecture/CrossModule/ShaderSystem/ImplementationPlan.md. Use current code and Acceptance's frozen oracle, exact candidate inputs and real native tools. Fix only demonstrated discrepancies, delete their replaced paths now and remove all temporary proof additions. Add no dual-output production mode or diagnostics platform. Apply the owning execution contract and its Stage Source-Style Gate.
```

## Phase 12 - Regenerate, Validate, And Hand Off The Complete Shader, Graphics-Pipeline, And Ray-Tracing Candidate

**Start:** current contract and required focused proof are valid; exact delivery inputs and candidate are available.

Join [AC-SHS-01 through 31](Acceptance.md#completion-criteria) and FM/CHK results, current generated hashes, program/target/API/capability matrix, native artifacts, cost observations and release/source-adopter proof. Fill only missing integration or package cells. A changed artifact invalidates its dependent evidence, rather than forcing every implementation and check to restart. Required unavailable tools/hardware remain blocked; unselected capabilities are not added to make the matrix rectangular.

**Exit:** the owning report records every required decision, original evidence and limitation; current source/build/generated/UI paths have one authority and no replacement residue. Completed instructions retire after their design/proof owners retain valuable outputs. This plan does not itself pass FCR-SHDR-01, Renderer/RHI reports or release gates.

```text
Execute only Phase 12 from Docs/Architecture/CrossModule/ShaderSystem/ImplementationPlan.md. Reuse compatible focused proof, regenerate only invalidated current products and close missing integration/package/adoption checks for the exact candidate. Verify stage-local deletions and Acceptance coverage; report required unavailable evidence without manufacturing a pass or starting a new framework. Apply the owning execution contract and its Stage Source-Style Gate.
```

## Reference And Ownership

Design owns semantics; Acceptance owns stable AC/FM/checks and numeric oracles; Delivery Evidence and Migration Baseline retain historical source provenance. The release mother plan owns admission/order and FCR assignment. This file contains remaining delivery/proof work only.
