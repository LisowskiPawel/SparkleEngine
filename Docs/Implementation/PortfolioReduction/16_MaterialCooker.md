# MaterialCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 16 of 21 (first focused visit).** Start: Stages 0-1 in Wave 5 after MeshCooker. Resume / dependency: No independent return visit; participate earlier in complete-system cuts when needed. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Keep a small material/texture-request boundary that carries the selected rendering semantics correctly.

**Retain:** Distinct library, source material semantics and texture references, correct color/channel/mip policies, camera/light-independent identity and required subsurface/alpha/two-sided behavior.

**Target flow:** Imported material/texture references -> one cook material representation and texture request set -> TextureCooker -> runtime material/shader parameters.

Dominant execution owner for: MA-01. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **One semantic material/request path:** after embedded-cache and composition retirement, simplify material source references, texture keys and writer/request-builder inputs around the retained external sources and one prepared scene. Remove obsolete aliases/intermediates through TextureCooker and runtime consumers; keep one source-qualified texture identity and stable material associations.
- **Preserve rendering meaning:** maintain channel/color-space/mip policies, UV/address behavior, alpha, two-sided and subsurface semantics. Deduplicate shared material/texture requests without copying per mesh instance or introducing another material model. Keep parallel request admission and publication ownership explicit.

**Structural acceptance:** follow Bistro DDS, glTF textures and retained HDR/EXR requests to runtime/shader parameters; inspect deduplication under parallel callers. Show removal of obsolete cache/composition fields, not a new request schema. A second semantic authority, broken source identity or discarded retained material channel fails.

**Reference application:** [P1](README.md#reference-choices-and-limits) guides the narrow library contract; [integration precedents](../../References.md#integration-precedents) support reusing existing mechanisms. No material framework or independent capability purge is authorized.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **8 files / 434 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

## Retirement participation

No independent system retirement is approved in this owner. Follow the shared last-consumer procedure when a delegated system removes local fields/adapters: delete newly orphaned helpers, exports, source/deployment membership and derived state only after proving there is no retained consumer. Credit the system owner once; retain this module's selected functionality.

## Stage 0: freeze the actual responsibility and smaller target

Trace TextureCookRequestBuilder, source roots/cache resolution and material writing into runtime/shader layout. Separate embedded FBX extraction retirement from external DDS/material handling. Keep HDR/EXR input unrelated to display HDR removal.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/MaterialCooker/Private/TextureCookRequestBuilder.cpp](../../../Tools/Cooking/MaterialCooker/Private/TextureCookRequestBuilder.cpp)
- [Tools/Cooking/MaterialCooker](../../../Tools/Cooking/MaterialCooker)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/16_MaterialCooker.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical MaterialCooker paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Narrow retained material/request flow after input cuts

Decisions: MA-01.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/16_MaterialCooker.md: Narrow retained material/request flow after input cuts.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
With SourceImporters and SceneCooker, remove unconsumed embedded-cache/morph/composition adapters and duplicate material/request keys. Keep one source-qualified texture identity and every retained semantic channel; do not duplicate a material/texture per mesh instance or add another request schema. Retain the existing library boundary.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Bistro DDS, glTF PNG/JPEG, default EXR, normal maps and material variants retain valid mappings. Color space, alpha, UV transform/address/occlusion behavior are checked against the retained renderer; existing unresolved parity remains open. Parallel callers do not race request deduplication. No whole-library deletion saving is booked.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Removing embedded-source plumbing breaks external Bistro DDS or glTF texture/channel references.
- A replacement duplicates material/texture data per instance, races deduplication or loses color/UV/alpha/subsurface semantics.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
