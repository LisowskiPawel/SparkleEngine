# Editor: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep a polished scene-inspection/editing frontend while deleting optional asset/shader tools and making the remaining state flow readable.

**Retain:** Outliner, live transforms/visibility/material variants/cameras/lights, render/effect controls, undo/redo and drag coalescing, console execution/history, compile/reload, screenshot/capture launch, preferences, custom title bar and existing generated icons. Maintainer saves persist approved fields; reviewer changes remain in-session.

**Target flow:** UI reads immutable world views -> typed edits reach the game-thread owner -> accepted state publishes to Renderer; explicit maintainer save writes the narrow level schema.

Dominant execution owner for: ED-01, ED-02, ED-03, ED-04, ED-05, ED-06, ED-07, ED-08, ED-09, ED-10, ED-11, ED-12, ED-13, ED-14, ED-15, ED-16, ED-17, ED-18, ED-19, ED-20, ED-21, ED-22, ED-23, ED-24. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **152 files / 12,005 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **24 files / 2,394 lines**; assigned shared spans: **35 lines in 2 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| ED-12 | 2 files / 49 lines |
| ED-13 | 2 files / 49 lines |
| ED-19 | 14 files / 1,293 lines |
| ED-24 | 2 files / 497 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace AssetInspection and ShaderTools panels to their Application/Renderer producers before cutting. Separate console completion from console execution/history. For ED-23, inspect existing source/reimport identity and LevelWorldProjection: Save All currently saves lighting/sky/active-camera description, not every object edit. Freeze screenshots and a transform/visibility/material-variant edit/reload example.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Editor/Private/AssetInspection](../../../Engine/Editor/Private/AssetInspection)
- [Engine/Editor/Private/Viewport](../../../Engine/Editor/Private/Viewport)
- [Engine/GameFramework/Private/Level/LevelWorldProjection.cpp](../../../Engine/GameFramework/Private/Level/LevelWorldProjection.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Delete mesh, texture and shader inspection subsystems

Decisions: ED-12, ED-13, ED-14.

Remove panels, preview rendering/tool actions, selection models and their exclusive diagnostic collectors. Follow dependencies into Application/Renderer/Core and remove now-unconsumed producers, registrations and includes in the same stage. Renderer-owned Reference overlay removal belongs to Renderer stage 1. Keep ordinary runtime mesh/texture/shader data, residency, useful errors, signal views and external native captures.

**Exit evidence:** Editor starts and renders retained maps without retired panels/preview paths. Compile/reload and native capture still work. Search confirms old panel producers have no retained consumer; removal does not serialize asset preparation or damage GPU resource lifetime. Build after regenerating source membership.

## Stage 2: Remove property search and console completion

Decisions: ED-19, ED-20.

Delete per-property query plumbing and CompleteConsoleLine/CompleteConsoleCVarName plus completion-only UI/state. Retain fixed property sections, outliner selection, input parsing, command/CVar execution, console history, help and useful failures. Remove redundant view/filter copies rather than replacing search with another generic query layer.

**Exit evidence:** Fixed inspectors still expose every kept control. Commands and CVars execute, retain history and report errors; completion/search UI and exclusive producers are gone. Retained immutable view and game-thread mutation boundaries pass focused edit checks.

## Stage 3: Bound maintainer persistence and forbid package authoring

Decisions: ED-22, ED-23, ED-24.

With GameFramework stage 3, retain one explicit save projection and small writer for camera/light/sky plus approved per-object transform/visibility/material overrides. Reuse source-qualified cooked node/object identity; runtime EntityId or pointer values are not persistent keys. Reject/reset overrides when source identity no longer matches. Keep supplied reviewer data read-only through product capability/writability; reviewer edits stay in-session. No sidecar authoring database, compatibility migration or new user-role framework.

**Exit evidence:** An explicit maintainer save/reload restores the selected fields and undo semantics remain correct. Reordered/replaced source identities cannot silently edit another object. Failed save preserves accepted document bytes. A relocated reviewer bundle supports all live edits but writes no scene data; reload restores defaults. Record ED-23 added lines separately: this approved new behavior is not deletion savings.

## Stage 4: Consolidate retained frontend ownership

Decisions: ED-01, ED-08, ED-10, ED-17, ED-18.

Collapse forwarding-only controllers and duplicate derived view state exposed by the cuts, using existing Editor composition owners. Keep custom title/hit testing, icon catalog/lifetime and the working console, preferences and undo command set. Coordinate GF-09 loading/empty failure UI without adding a second world-lifecycle owner.

**Exit evidence:** Native window interaction, DPI, icons, selection/undo and in-process map switching work on both APIs. UI observes snapshots and edits execute on the world owner; no render/global-world lock is introduced. Specialist can trace one edit and one saved property without walking obsolete diagnostic layers.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
