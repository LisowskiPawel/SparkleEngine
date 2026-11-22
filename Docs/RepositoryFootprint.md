# Repository footprint and code movement

[Goals](Goals.md#the-three-cases) | [Integration actions](ThirdPartyMigration.md#next-actions) | [Roadmap](Roadmap.md)

Make the renderer easier to understand and maintain by removing duplicate policy, custom support mechanisms and state without a consumer. Keep work that delivers correct pixels, measured frames or the owned model case. A file move counts as an improvement only when ownership or dependency direction becomes clearer.

## Footprint baseline

The frozen **22 November 2026, 19:05 Warsaw** inventory contains **150,211 owned code lines in 2,429 files**, including **136,684 native code lines**. It predates task-runtime publication and later cuts; it is not a live count.

| Responsibility | Owned code lines | Share |
| --- | ---: | ---: |
| RHI, Renderer and shaders | 70,276 | 46.8% |
| Launcher, Editor, Application and entry points | 33,084 | 22.0% |
| GameFramework, importers and cookers | 20,952 | 13.9% |
| Core, former Tasks, Platform, ToolSupport and native build glue | 11,928 | 7.9% |
| ShaderCompiler and automation | 13,971 | 9.3% |

Against the 21 November baseline, native code fell by 967 lines while automation grew by 1,061 and shaders by five: **total owned code grew by 99 lines**. Count the complete change before claiming simplification. Do not add later overlapping integration ledgers to this total.

The [snapshot and metrics](../artifacts/analysis/footprint-refresh-20261122/metrics.json) bind revision `d5bf01d53791407de8bf691a8bf54cb404c69226` plus recorded local changes. cloc 2.06 excludes blanks/comment-only lines; first-party bindings count wherever placed, while upstream/generated code and assets are separate. [Peer proportions and pinned data](Images/RepositoryFootprint/peer-proportions.csv) provide context, with different product scopes and no ideal LOC target. Source volume, disk storage and build/runtime cost are separate measures.

## Cut candidates

Continue [current rendering repairs](Roadmap.md#current-priority). Use the active Qt integration or independent preprocessing trial for the next bounded support cut. The remaining rows are investigations, with no promised deletion count or new release prerequisite.

| Responsibility | Move authority to | Retire when the replacement is proved | Case supported |
| --- | --- | --- | --- |
| Launcher scene layout | Existing Qt item view; scene view/delegate beside `Gui/Pages/Scenes`, one presentation model in `Gui/Models`. | Custom card reflow, per-card widget trees and redundant row state. | Reproduction of all three cases. |
| Shader preprocessing | DXC/Slang; keep only required source mapping/generated declarations in ShaderCompiler. | Recursive include expansion and duplicate language semantics. | Source-to-native debugging and measured frames. |
| Dependency facts | Existing `ThirdParty/<name>/Dependency.cmake` declarations, consumed by Launcher/deployment/packaging. | Any remaining handwritten copies of pins, required files or payload identity. | Repeatable builds and delivery. |
| Integration glue | Upstream mechanism plus the consuming module's private domain/lifetime owner. | Duplicate completion/stop state, result translations and wrappers without independent policy. | Smaller support cost around the cases. |
| Material interpretation | One admitted scene/material meaning carried through import, cook, runtime and sampling. | Divergent UV/address/occlusion interpretation and superseded conversions. | Content to correct pixel. |
| Production sampling | One selected production estimator after the RTXDI trial qualifies. | Replaced production sampling code; preserve independent Reference and bounded research controls. | Measured lighting quality/cost. |

### Launcher presentation machinery

Start with [LauncherLayoutWidgets](../Tools/Launcher/SparkleLauncher/Private/Gui/Widgets/LauncherLayoutWidgets.cpp) and the [active item-view plan](Implementation/LauncherItemViews.md). Qt must actually own layout/navigation. Preserve selection, readiness, Source/primary actions, progress/errors, cancellation and DPI. Charge model/delegate/shell changes to the complete result. Shared operation dispatch and the Activity panel were already simplified; they are not future savings.

### Dependency and build policy

Source/target separation and generated Launcher facts are already implemented. Trace remaining repeated facts from [recipes](../ThirdParty/README.md) through readiness, runtime deployment and packaging; remove a copy only after its consumers use the authoritative declaration. Finish the [remaining reuse/cleanup checks](Implementation/ThirdPartyIntegration.md). Keep source admission, offline use, configuration differences and runtime repair.

### Correctness before structural cuts

The material review identified different raster/ray handling of UV mappings, address modes and occlusion strength. Finish the checked [material contract](Rendering.md#frame-and-materials), retaining path-specific derivatives/filtering and independent Reference transport. This may require more shader code; no net saving or cause of the current Sponza failure is established.

Frame-plan reuse remains conditional on profiling. Prove repeated stable planning cost and correct invalidation for imports, histories, providers, resize and reload before adding a cache. Existing allocation/vector cleanup is delivered.

## Ownership to retain

Keep RHI/frame graph, resource identity/synchronization/retirement, scene/material semantics, Reference correctness controls, domain admission/cancellation/publication/recovery and useful diagnostics. Defaults, persisted intent, device selection and UI presentation can have different lifetimes; merge only actual duplicate authority. [Goals](Goals.md#product-scope) defines the product boundary.

Keep code together when it shares an invariant or lifetime. Place domain adapters with their consumers and dependency bindings/patches with their recipes. File count and module size alone do not justify a move, split or replacement framework.

## Choosing the next cut

1. **Name what disappears.** Identify the current owner, producer, consumer and lifetime, including failure/destruction. Trace a real request through the whole workflow.
2. **Prove the complete change.** Freeze its boundary; charge callers, bindings, relocated code, shaders, scripts and maintained patches. Use [migration cost results](ThirdPartyMigration.md#delivered-integrations-versus-savings) to distinguish upstream delegation from actual reduction.
3. **Keep one authority.** Convert consumers and remove the replaced code/state/targets. Validate the affected behavior through its existing owner; distinguish source/build checks from interactive, GPU and package qualification.

Keep only remaining actions in active plans and raw receipts in ignored evidence. Remove duplicate documentation, completed diaries and obsolete wrappers in the same change; retain useful style rules, checks and reproduction commands. Do not create another report or catalogue to hold removed clutter.
