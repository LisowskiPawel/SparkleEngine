# Launcher scene cards through Qt item views

[Migration decision](../ThirdPartyMigration.md#native-removal-estimates) | [Cut candidate](../RepositoryFootprint.md#launcher-presentation-machinery) | [Engineering](../Engineering.md) | [Code style](../CodeStyle.md)

**Next: Stage 2.** The [source/contract baseline](../../build/validation/launcher-item-views/candidate-20261122/STAGE-0.md) and [isolated Qt fit receipt](../../build/validation/launcher-item-views/candidate-20261122/STAGE-1.md) bind the selected design and passing trial controls. Production integration, complete net removal and final qualification remain open.

The deliverable is a simpler Launcher scene catalogue: Qt owns item placement, scrolling and index navigation; a small scene model and delegate replace the custom grid and per-card widget assembly. Preserve scene information, responsive sizing, DPI, keyboard/accessibility, Source and primary actions, readiness and operation feedback. The complete connected UI must be smaller and easier to reason about. The historical **Q ceiling is 160 physical / 140 code lines**, not a promised saving or the whole implementation budget.

Reuse the existing **Qt 6.11.1 Widgets** kit selected in [Launcher CMake](../../Tools/Launcher/SparkleLauncher/CMakeLists.txt). No Qt upgrade, new dependency, QML conversion, Editor migration or build/cook/run redesign is required. D3D12/Vulkan, operation execution, capture controls and deployment remain with their existing owners; charge any necessary integration repairs to this change.

## Ownership and removal boundary

Paths below are relative to `Tools/Launcher/SparkleLauncher/Private/`.

| Current owner | Intended treatment |
| --- | --- |
| [`Gui/Widgets/LauncherLayoutWidgets.*`](../../Tools/Launcher/SparkleLauncher/Private/Gui/Widgets/LauncherLayoutWidgets.cpp): `ResponsiveCardGridWidget` | Retire the class, card list, ancestor-width search, column/size caches and layout teardown/reinsertion. Qt must actually own placement. |
| `ProportionalCardFrame`, in the same pair | Its only current construction is in scene cards. Retire it if the delegate absorbs card geometry and activation. Count this separately from Q. |
| `ElidedLabel`, `SceneHeroCardWidget`, in the same pair | Preserve live consumers: Clean options also use `ElidedLabel`; the hero remains a separate responsibility. Remove only genuinely unused portions. |
| [`Gui/Pages/Scenes/LauncherSceneCards.cpp`](../../Tools/Launcher/SparkleLauncher/Private/Gui/Pages/Scenes/LauncherSceneCards.cpp) | Replace per-level frames, labels, layouts, buttons and captured entry copies with delegate painting and explicit action dispatch. Reuse artwork painting/placeholder policy where useful. |
| [`Gui/Pages/Scenes/LauncherSceneCatalog.*`](../../Tools/Launcher/SparkleLauncher/Private/Gui/Pages/Scenes/LauncherSceneCatalog.h), `LauncherSceneLaunch.cpp` | Keep scene-page composition and operations. Replace widget-pointer status/button maps and `ActionIntent` widget properties with model changes and typed action decisions. Keep one opening run/level identity. |
| [`Gui/Models/LauncherLevelUiModel.*`](../../Tools/Launcher/SparkleLauncher/Private/Gui/Models/LauncherLevelUiModel.cpp) | Evolve this presentation boundary or replace its redundant parts when adding a Qt list model. Do not retain two writable row stores or rebuild readiness inside `data()`/painting. |
| [`Gui/Shell/LauncherMainWindow*.cpp`](../../Tools/Launcher/SparkleLauncher/Private/Gui/Shell/LauncherMainWindow.cpp) | Integrate focus registration, scene-page scrolling, settings/content refresh and operation completion. Avoid rebuilding the scene page for ordinary row-state changes. Other workflow pages keep their existing ownership. |
| [`Gui/Style/LauncherUiDesign.h`](../../Tools/Launcher/SparkleLauncher/Private/Gui/Style/LauncherUiDesign.h), `LauncherVisualStyle.cpp`, artwork widgets | Retain one source of sizing and visual values; remove obsolete card-only stylesheet rules after cutover. No parallel palette/geometry table. |
| [`Levels/LevelPreparation.*`](../../Tools/Launcher/SparkleLauncher/Private/Levels/LevelPreparation.cpp), [`Levels/LevelRunOperations.cpp`](../../Tools/Launcher/SparkleLauncher/Private/Levels/LevelRunOperations.cpp), operation request/service/executor | Retain source/cooked/product readiness, selection writing, request construction, admission, progress, cancellation and child readiness. The view dispatches intent; it does not execute operations. |

Put new scene-only view/delegate code beside the scene page, and model code with the existing GUI models. These are proposed responsibilities, not a quota of new classes/files. Prefer the smallest cohesive implementation; no public Qt API, generic card framework, forwarding controller or replacement layout engine.

Qt's [model/view architecture](https://doc.qt.io/qt-6.11/model-view-programming.html) supplies the division between data, views and delegates. A `QAbstractListModel` over one owned row snapshot is the starting candidate; adopt another existing model only if it removes work without mirroring the snapshot. `LauncherContentModel` still owns discovered content identity, and the existing readiness owners produce the values displayed by rows.

## Acceptance and cost rules

Bind evidence to HEAD **and changed-file hashes**, Qt kit/toolchain/configuration, executable identity, content/catalogue inputs, settings and display environment. Keep receipts and temporary probes under ignored `build/validation/launcher-item-views/<candidate>/`. Stage 0 establishes the baseline; do not require a whole-engine benchmark campaign before the fit trial.

Use three independent verdicts: **technical delivery**, **simplification**, and **product qualification**. Full delivery of this promise requires the first two plus the focused UI controls below. A technically working migration that grows the complete UI is not a delivered simplification. Missing actual interaction/display observations stay UNVERIFIED; screenshots, source inspection and synthetic events have narrower scope.

- Preserve the original Q fragment identities and ceiling. Report Q retirement as gross credit only. Separately identify any retired card assembly, proportional-frame code, state mirrors or stylesheet rules, with no overlapping credit.
- Freeze a complete connected-owner baseline covering every file above that will change, plus all new files and any additional callers/build inputs. Report complete before/after physical and code totals, and separately retired versus new/rewritten/relocated lines. A move is not retirement; unrelated cleanup earns no Qt credit.
- **Required simplification exit:** positive net removal in both first-party native physical and code lines across that complete boundary; no second row-state authority; no comparable reflow controller; no persistent per-item widget tree. Explain the resulting ownership in one short request-to-presentation trace. Set any stronger reduction target before the candidate, never after measuring it.
- Charge model roles, painting, hit testing, keyboard/accessibility, caches, focus/scroll restoration and shell changes. Report CMake/scripts, upstream patches, build/runtime/package costs separately. Existing Qt does not imply zero integration cost; do not credit Qt's upstream implementation as deleted Sparkle code.
- Fix defects and unnecessary machinery, then review a numerical miss once under the [delivery policy](../ThirdPartyMigration.md#code-delivery-and-qualification). If the complete candidate still fails the simplification exit, record NO-GO for this design. Do not quietly waive the promise, reduce supported behavior or repeat arbitrary tuning to hit a line count.

At every stage exit, record the exact candidate, passed/missing controls, commands/artifacts, limitations, invalidation triggers and next runnable action. Remove completed stages, prompts and individual completed actions in the same run under the [documentation rules](../Engineering.md#writing-documentation); preserve stage numbers and keep receipts outside the active plan. Reuse matching evidence. A named-stage request includes its necessary prerequisites and ends at its verified exit; a full-delivery request continues through closure. A concrete unavailable input needs its owner and resume action, not another identical trial.

## Stage 2: consolidate row state and action dispatch

**Purpose:** replace widget-owned state with one live presentation model and preserve the operation boundary.

- [ ] Evolve/replace `LauncherLevelUiModel` so one GUI-thread-owned row snapshot feeds the view, accessible text and action presentation. Keep content/catalogue loading and filesystem readiness outside `data()`, `paint()` and `sizeHint()`. Avoid a second DTO store, generic role registry or per-row copies of content/settings. Use only roles needed by actual consumers, including stable identity and standard accessible text/description.
- [ ] Replace string-valued widget `ActionIntent` and pointer-map updates with one action decision used by display, enabled state and dispatch. Preserve current priority: opening suppresses primary actions; a supported source-missing level that can sync offers Sync; unsupported/unselectable levels offer Details; otherwise the label is Open or Run from current settings. The operation planner still diagnoses missing executable/cooked outputs; `bReady` alone must not silently change the existing Open path into a new disabled state.
- [ ] Dispatch by current content/level identity and freshly resolved row/settings state. Revalidate before starting an operation, including after refresh/reorder/removal and queued input. Keep UI current index independent of catalogue `bSelected`; navigation must not write `Levels.catalog`. Preserve targeted sync, Source URL, blocker Details, confirmation for clean, bulk sync/clean and request profile/API/capture options.
- [ ] Keep one opening run/level pair in its existing lifetime owner. Preserve duplicate-start suppression, active-output focus, visible Opening state and all-primary-action disabling during opening. Completion must match run identity before clearing it, including success, failure and cancellation; unrelated completion must not unlock it. Keep operation workers/services alive independently of scene-page widgets.
- [ ] Use narrow `dataChanged` notifications for row-state changes and correct insert/remove/reset notifications for structural changes. Restore current/focused level and scroll position by stable identity after actual reset; if removed, choose a deterministic adjacent item without activating it. Content replacement cannot reuse stale identity. Apply model APIs on the GUI thread, parent the model/delegate deliberately, and disconnect/destroy safely when pages are replaced. Qt documents [thread affinity and reset invalidation](https://doc.qt.io/qt-6.11/qabstractitemmodel.html).
- [ ] Exercise the refresh sources: SettingsChanged, content discovery, sync, clean, build/cook and launch settlement, plus catalogue changes with unchanged `LauncherContentModel` identity. Route them through the existing refresh owner. Do not depend only on `ContentChanged`, and do not add a polling framework or reread the filesystem per displayed role.

**Do:** make action presentation read like the supported behavior, with meaningful locals and narrow types. **Avoid:** readiness logic in a delegate, copied busy flags per widget, dangling QModelIndex/row references, dispatch from `data()`, or a second execution service.

**Failures to expose:** launching the old row after reorder, showing stale Run/Open labels, repaint-triggered I/O, status never refreshed after failed sync, completion clearing another run, or callbacks into a deleted page.

**Exit:** actual catalogue updates and actions use the single model/decision path with focused success/failure/reset controls. The selected view is ready for production composition; no test-only abstraction enters the submitted code.

Ready-to-use prompt:

```text
Complete Stage 2 of Docs/Implementation/LauncherItemViews.md and its necessary technical prerequisites. Consolidate existing scene presentation into one Qt model and one action decision; dispatch current stable identities through the existing operation owners. Preserve readiness semantics, settings/options, opening run lifetime, refresh and cancellation. Exercise stale-row/reset/completion/destruction failures and GUI-thread ownership. Count all code and callers, remove local probes and delivered plan work, and record the next unfinished item. Do not expand into operation or rendering redesign.
```

## Stage 3: cut over and remove the displaced mechanisms

**Purpose:** ship one production scene UI and realize the ownership transfer.

- [ ] Compose the selected model/view/delegate into the actual scene page and register its focus/action surfaces with the shell. Preserve the hero, catalogue errors/empty state, footer selectors and bulk actions. Update the scene refresh path so ordinary state changes do not recreate cards or the full scene page. Keep necessary structural reset/destruction handling explicit.
- [ ] Delete `ResponsiveCardGridWidget` and every construction/reference. Delete displaced card widget assembly, pointer maps, `ActionIntent` properties, activation forwarding and unused proportional-frame behavior. Remove `ProportionalCardFrame` if its last consumer is gone. Retain hero and Clean-page elision consumers and any still-used artwork helpers.
- [ ] Remove obsolete scene-card-only stylesheet rules, includes, signals and cache fields. Do not retire a shared helper or Qt focus registration used by other workflows. Replace the old path directly: no production switch, retained fallback, abandoned adapter or parallel card builder.
- [ ] Update explicit GUI source/header lists and AUTOMOC membership in Launcher CMake, then regenerate. Build the standalone Launcher and any affected root composition with the existing kit/profile; use the documented commands below. Preserve CLI/core separation, Qt deployment/plugins/notices and source/prebuilt workspace behavior. Wait for builds already using a shared output directory.
- [ ] Review the complete neighbourhood and full connected-owner ledger. Trace current row -> decision -> request -> completion -> model update. Resolve accidental wrappers/state mirrors and run `git diff --check` plus applicable code-style checks. Report the full positive or negative net result; retained helpers and new adapters remain charged.

**Do:** remove the old mechanism with its callers and build membership. **Avoid:** declaring victory because the old class name vanished or moving its calculations into several small helpers.

**Exit:** one built production path remains, the defined mechanisms are retired, and the simplification exit passes. A cost miss receives one supported review/NO-GO disposition before claiming this promise delivered.

Ready-to-use prompt:

```text
Complete Stage 3 of Docs/Implementation/LauncherItemViews.md, including missing relevant prerequisites. Cut the actual scene page over to the accepted Qt design, remove the superseded grid/card/state mechanisms and all dead callers/style/build entries, and preserve shared hero/Clean helpers. Regenerate and build the affected Launcher compositions without competing with shared-output builds. Review the complete UI ledger and ownership trace, fix defects and unnecessary glue, and report numerical misses honestly. Remove local probes and completed plan work before handoff. A class deletion alone is not a simplification result.
```

## Stage 4: verify the delivered UI and close

**Purpose:** demonstrate the promise on the production executable and retain honest qualification limits.

Run the affected controls below on the final candidate. Reuse earlier results only when their relevant source/product/input/display identities still match. Stage 1 observations do not automatically qualify later production integration.

| Control | Required positive observation | Required failure/edge observation |
| --- | --- | --- |
| Catalogue/presentation | Catalogue order, Empty, descriptions/status/details, artwork and Source are preserved; useful full text remains available. | Missing workspace, unreadable/empty catalogue, bad artwork and long text show useful diagnostics/fallbacks without stale rows. Preserve a way to copy previously selectable metadata. |
| Geometry/scrolling | 1–4 columns, 400–620 sizing where space permits, 2.42 aspect, gaps/margins, partial rows and reachable last item match the contract. | Narrow/very wide windows, threshold widths ±1 logical pixel, scrollbar appearance, resize storms and page switches cause no overlap, clipping, horizontal trap or resize oscillation. |
| DPI/style | Available 100/125/150/200% observations keep text, artwork, focus and hit regions aligned, including a real mixed-display move when available. | Font/style/palette/DPR changes invalidate needed geometry/artwork without stale hit boxes or double scaling. Unavailable display cells remain UNVERIFIED. |
| Keyboard/accessibility | Tab/shift-tab enter/leave predictably; arrows navigate; Return/Enter/Space activate once; Source and primary have discoverable names, states and independent actions. Focus scrolls into view. | Source never launches; blank/disabled/removed rows never dispatch; double click and auto-repeat do not duplicate starts; reset/focus loss leave no stuck press or hidden focus. Inspect the actual Windows accessibility tree/actions. |
| Readiness/request meaning | Ready, source-ready, syncable, unavailable and unsupported rows preserve their correct actions and reasons; mode/profile/API/capture selections reach existing requests. | Missing products/cooked output still produce the planner's truthful failure, with no implicit build/cook. Empty avoids external scene acquisition but keeps its real executable/texture/shader prerequisites. View selection never changes persisted scene selection. |
| Operation lifecycle | Actual targeted sync, confirmed clean, bulk actions and a scene launch update Activity/status and opening state; successful launch reaches a responsive child through the existing readiness path. | Failed/cancelled sync or launch, rejected start, rapid activation and unrelated completion preserve output/error identity and correct gating. Page rebuild/workflow switch/Launcher close during work causes no dangling UI callback or orphaned borrowed storage. |
| Model/refresh | Settings, catalogue/content changes and operation completion update the current UI; same-identity status changes work without full page reconstruction. | Reorder/removal/reset during queued input cannot launch the wrong scene; model notifications remain valid; current identity/scroll restore predictably. Use a local model checker only if it tests a concrete risk. |
| Other workflows/build delivery | Build/Cook/Clean/Workspace pages, footer and Activity still work; affected standalone/root products build and launch with the existing Qt payload. | Shared `ElidedLabel`/hero consumers survive; no QtTest/prototype dependency, missing AUTOMOC entry, extra runtime plugin or leftover legacy path enters delivery. |
| Maintenance/responsiveness | Complete native physical/code net removal is positive; one row/action authority remains; measured UI samples stay within Stage 0 tolerances. | Count accessibility, artwork caching, scroll restoration, caller/build changes and every added file. Repeated resize/refresh does not allocate a new widget tree per row or reread readiness during painting. |

Follow a successful GUI launch through the affected Editor/Game handoff; check both run modes and D3D12/Vulkan request selection. Actual GPU/image/capture/profile and relocated-package matrices retain their existing release owners. A captured request proves request selection, not execution on that backend. Do not expand a presentation refactor into new release prerequisites or label missing GUI/DPI/accessibility cells as PASS.

Use the existing [Launcher build workflow](../Engineering.md#build-launcher); these are execution-time commands, not checks run when this plan was authored:

```powershell
cmake -S Tools/Launcher -B build/private/tools/SparkleLauncher -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4
& ./artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe
git diff --check
```

- [ ] Fix observed regressions through their existing owners and repeat only invalidated controls. Keep exact missing capabilities/inputs and resume actions if a required observation cannot be obtained. Separate technical completion, simplification and the focused qualification verdict; do not close the full promise while its required controls remain unobserved.
- [ ] Publish the final source/product-bound receipt and complete cost ledger in ignored output. Record gross Q retirement, other non-overlapping retirements, complete native net and separate infrastructure/runtime costs. Remove local test/probe classes, fixtures, executables and build hooks; preserve existing tests. Do not mark closure while required cleanup is unresolved.
- [ ] On supported delivery, update the existing Qt decision and [footprint cut candidate](../RepositoryFootprint.md#launcher-presentation-machinery) with the actual result/limits, remove delivered work from open scope and reconcile any affected Roadmap pointer. Preserve the frozen historical Q ceiling and pending-union accounting rather than rewriting the baseline. Keep broader product limits with their existing owner. Delete this plan and repair incoming links when its final required item closes. A supported NO-GO removes the rejected candidate/plan and records the failed design without claiming integration or savings.

**Exit:** the production UI passes the required focused controls and complete-cost simplification gate, the old mechanisms and local probes are gone, and the owning docs state the result truthfully. Broader product qualification remains explicit and separate.

Ready-to-use prompt:

```text
Complete Stage 4 and close Docs/Implementation/LauncherItemViews.md, preparing and fixing relevant prerequisites through their existing owners. Validate the final production Launcher against the focused positive/failure matrix, actual keyboard/accessibility and available physical DPI environments; follow real operations through feedback and responsive child readiness. Bind every claim to matching identities and retain unobserved cells. Verify the full UI reduction ledger and remove local probes. Update existing decision/footprint/roadmap owners, delete delivered stages and finally the plan and incoming links only when its required exit passes. Keep release/GPU/package qualification separate. Report a supported NO-GO honestly if the design cannot meet the simplification contract.
```
