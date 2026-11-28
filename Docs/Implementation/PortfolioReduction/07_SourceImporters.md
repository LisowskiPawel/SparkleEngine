# SourceImporters: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 07 of 21 (first focused visit).** Start: Stages 0-1 in Wave 2. Resume / dependency: Stage 2 before SceneCooker stage 1; stage 3 after SceneCooker stage 2, in Wave 5. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Admit only the retained scene inputs and animation subset, preserving exact geometry/material/camera/light semantics needed by the portfolio.

**Retain:** cgltf glTF/GLB, FBX/Assimp for Bistro/Knight, source-authored cameras/lights, tangent generation/remapping and basic skeletal input. Shared geometry/material/bone conversion remains.

**Target flow:** Explicit source path -> format reader -> one checked imported scene representation -> cookers. Imported arrays/indices and owned bytes remain valid through parallel consumers.

Dominant execution owner for: IM-01, IM-02, IM-03, IM-04, IM-05, IM-06, IM-07, IM-08, IM-09, RV-04, RV-05. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Explicit admitted inputs:** after PLY and embedded FBX extraction retirement, narrow source admission, imported material/texture references and error paths to the retained glTF/GLB and external-texture FBX routes. Remove extraction/cache options and intermediates through cooker callers; keep the Assimp FBX recipe and shared geometry conversion.
- **Animation by actual need:** conditionally narrow curve/clip conversion only after Knight/Cesium equivalence. Update imported records and cook consumers to the single qualified selection path; remove unneeded resampling/selection plumbing rather than leaving a disabled general importer. Preserve source cameras/lights, units, skeleton identity, tangents and owned bytes for parallel consumers.

**Structural acceptance:** native-import retained Bistro/Knight/Cesium sources, audit external texture identities and verify cooked moving output. Show removal of obsolete imported fields and callbacks through consumers. Silent curve approximation, static-pose substitution, dangling source spans or loss of FBX support fails; a failed conditional cut stays open.

**Reference application:** use [Assimp build admission and explicit dependency precedents](../../References.md#integration-precedents), plus [P1](README.md#reference-choices-and-limits) for the importer/cooker boundary. No new universal scene-import framework.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **89 files / 5,900 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **4 files / 269 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| IM-08 | 4 files / 439 lines |
| IM-09 | 6 files / 547 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| IM-03, RV-05; stage 1 | PLY-only importer/dispatch. Remove SourceSceneImporter format admission, PLY adapter/options/build selection and ASSIMP_BUILD_PLY_IMPORTER ON in ThirdParty/assimp/Dependency.cmake. Recompute upstream compiled objects/deployment/notices actually affected; no whole Assimp deletion. | FBX importer/material bounds patch and shared Assimp conversion/zlib/runtime; cached upstream source and historical evidence are not authored source savings. |
| IM-06; stage 1 | FBX embedded texture extraction/cache/TGA encoder. Retire FbxEmbeddedTextureImporter, extraction vector/material handoff, imported-cache root helper and cache-path/encoding APIs only if their last users vanish. Check Core/ToolSupport and texture request source-root logic. | External Bistro DDS and Knight FBX; shared PNG/TGA/stb, JPEG/WIC, zlib/TinyEXR and any non-embedded input users. |
| IM-08; stage 2 | Unused Step/Cubic curve semantics. Retire only unused sampler enum/validation/tangent coefficients/interpolation branches and cook resampling paths after all retained animation producers use the proven subset. Regenerate cooked data; no reader compatibility version. | Required linear TRS/quaternion sampling, timing/rotations and any Step/Cubic semantics actually needed by Knight/Cesium until equivalence passes. |
| IM-09; stage 2 | Unbounded every-source-animation-clip obligation. Close generic clip selectors/catalog/state/metadata/load paths superseded by explicit required clips, while checking multiple skeleton associations and downstream Ozz/cooker/runtime consumers. | Every channel/clip necessary for the selected Knight/Cesium motion; no static-pose fallback or forced serial prep. |

## Stage 0: freeze the actual responsibility and smaller target

Read the retained nine-at-audit glTF/GLB and three FBX metadata, then actually import native Knight/Bistro before extraction deletion. Refresh the current ten-map target after Candles removal. CesiumMan declares one clip/57 LINEAR samplers; FBX samplers default linear. This is evidence for a candidate subset, not runtime animation qualification.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Import/SourceImporters/Private/Fbx/FbxImporter.cpp](../../../Tools/Import/SourceImporters/Private/Fbx/FbxImporter.cpp)
- [Tools/Import/SourceImporters/Private/Fbx/FbxEmbeddedTextureImporter.cpp](../../../Tools/Import/SourceImporters/Private/Fbx/FbxEmbeddedTextureImporter.cpp)
- [Tools/Import/SourceImporters/Private/Gltf/Geometry/GltfMeshGeometryExtractor.cpp](../../../Tools/Import/SourceImporters/Private/Gltf/Geometry/GltfMeshGeometryExtractor.cpp)
- [Tools/Import/SourceImporters/Private/Fbx/FbxAnimationImporter.cpp](../../../Tools/Import/SourceImporters/Private/Fbx/FbxAnimationImporter.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/07_SourceImporters.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical SourceImporters paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Remove PLY and embedded FBX texture extraction

Decisions: IM-03, IM-06, RV-05.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/07_SourceImporters.md: Remove PLY and embedded FBX texture extraction.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Delete PLY-only reader/dispatch/build options and stop compiling Assimp's PLY importer (ASSIMP_BUILD_PLY_IMPORTER) and FBX embedded-image extraction/cache/TGA encoding plus exclusive callers and cache writes. Retain Assimp FBX and externally resolved Bistro DDS, PNG/JPEG/default EXR routes. Remove unsupported admission explicitly; do not delete shared Assimp or TGA decoding used elsewhere. GameFramework owns morph removal through this importer.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Native retained FBX imports confirm no embedded payload is required; missing external texture errors stay actionable. PLY/embedded unsupported inputs fail cleanly and cannot partially replace accepted outputs. Knight skeleton/geometry and Bistro material/camera/light conversion remain intact.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- PLY/embedded extraction survives through another dispatch/cache route or a permissive fallback.
- Shared FBX/Assimp/external DDS conversion is deleted, or native import reveals a retained embedded-texture need.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Narrow animation curves and clip selection conditionally

Decisions: IM-08, IM-09.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/07_SourceImporters.md: Narrow animation curves and clip selection conditionally.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Select required Knight/Cesium clips explicitly; drop Step/Cubic/all-source-clips obligations only after prepared equivalent data passes. Preserve linear TRS/quaternion interpolation, timing, bind transforms, joint association and all required channels. Integrate with SceneCooker Ozz preparation; do not add a second runtime conversion path or force one worker.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Import/cook and threaded runtime playback of Knight and CesiumMan on both APIs preserve motion, skinning, previous-frame and RT geometry behavior. If either needs a supposedly removed curve/clip route, retain that minimum support and leave the cut conditional. Source declarations or a static pose do not pass.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Required Knight/Cesium clips/curves/channels are lost or alter bind pose, timing, skinning or RT motion.
- A forced linear/static fallback hides missing semantics, or both old/new animation admission routes are kept.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Narrow retained conversion flow and external references

Decisions: IM-01, IM-02, IM-04, IM-05, IM-07, RV-04.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/07_SourceImporters.md: Narrow retained conversion flow and external references.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
After fixed Knight preparation, remove unused importer-to-cooker adapters and duplicated coordinate/unit/material conversion. Keep source cameras/lights and MikkTSpace-backed missing tangents: DamagedHelmet plus ABeautifulGame need them. Retain current FBX source pipeline, not an external-only conversion replacement.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Check geometry/index bounds, normals/tangents/handedness, units, camera/light transforms and texture paths. Compare imported/cooked outputs and bounded rendered examples; retained inputs render correctly, and narrowed admissions have no permissive fallback.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Unit/coordinate/tangent/camera/light/material conversion is duplicated or applied twice.
- DamagedHelmet/ABeautifulGame normal mapping or retained FBX behavior regresses while a metadata-only check passes.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
