# Showcase content and conversion scripts: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep ten deliberate portfolio scenes and reproducible preparation, with only the Knight Modern Sponza variant and no general runtime content composition.

**Retain:** Empty, CornellBox, classic Sponza, BistroExterior, BistroInteriorWine, ModernSponzaKnight, CesiumMan, LPSHead, DamagedHelmet and ABeautifulGame; Cornell/LPS conversions and required attribution. Retain base/Curtains/Knight source dependencies.

**Target flow:** Pinned source/rights and fixed conversion/preparation -> explicit level scene reference -> cooked bundle -> Launcher scene selection -> Editor/Game and bounded regression examples.

Dominant execution owner for: PR-01, PR-02, PR-03, PR-04, PR-05, PR-07, PR-08, PR-11, PR-12, PR-15, PR-16. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **15 files / 762 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **1 files / 21 lines**; assigned shared spans: **30 lines in 1 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

## Stage 0: freeze the actual responsibility and smaller target

Resolve each selected .level to its actual source and recipe; snapshot the retained source/texture/reference graph and separately inventory binary bytes. Current Knight lists three extensionless assets; it is not yet a single prepared scene. KTX and earlier retired scenes are already gone and must not be recounted.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Projects/Showcase/Levels.catalog](../../../Projects/Showcase/Levels.catalog)
- [Projects/Showcase/Levels/ModernSponzaKnight.level](../../../Projects/Showcase/Levels/ModernSponzaKnight.level)
- [Projects/Showcase/Levels/ModernSponzaCandles.level](../../../Projects/Showcase/Levels/ModernSponzaCandles.level)
- [Projects/Showcase/Assets/Meshes/ModernSponza](../../../Projects/Showcase/Assets/Meshes/ModernSponza)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove Candles and its otherwise unused content route

Decisions: PR-07.

Delete Candles level/catalog/pack entry, preview and conversion/acquisition/build/cook/package references. Retire only payloads whose retained reference set is empty; do not delete Modern Sponza base or Curtains. Prune current generated outputs through the owning controlled clean/recook workflow, not arbitrary user download/capture folders.

**Exit evidence:** Catalog contains exactly the ten selected scene entries. No retained recipe/level/package depends on Candles; Knight still resolves all required sources. Report binary byte retirement separately from text/code. Update applicable notices/source references without erasing history or claiming redistribution qualification.

## Stage 2: Reproduce one prepared Knight product and explicit scene paths

Participation boundary: execute the shared one-product migration inside **SceneCooker stage 2**, which owns the complete vertical cut. This stage owns its explicit resolver/project-recipe details and verifies its consumers; it does not start a second composition migration. Close both affected stages against the same qualified source identity when their distinct exits pass.

Decisions: PR-08.

Implement the small project-owned fixed preparation entry consumed by SceneCooker stage 2. Inputs are Base/main_sponza/NewSponza_Main_glTF_003.gltf, AddOns/Curtains/pkg_a_curtains/NewSponza_Curtains_glTF.gltf and AddOns/Knight/pkg_e_knight_anim/Exports/FBX/Knight_USD_002.fbx. Keep the routine readable and specific; no generic recipe parser. Update all retained level inputs to the explicit source/prepared-scene contracts selected by AssetCooker.

**Exit evidence:** Knight reproduces complete composition/animation after clean preparation. A changed input is recooked and a missing one fails usefully. Import/cook/resource identity tests cover source-unit transforms and stable saved object references. The runtime contains one scene product per level.

## Stage 3: Close selected-scene walkthrough and acquisition boundaries

Decisions: PR-01, PR-02, PR-03, PR-04, PR-05, PR-11, PR-12, PR-15, PR-16.

Remove unused script/catalog helpers exposed by the smaller set; retain the existing CornellBox/LPSHead reproducibility routes and source-authored camera/light/tangent semantics. Set intentional defaults and explain each selected scene purpose, controls and limits without adding new demos or features.

**Exit evidence:** All promised map/product/API combinations load from prepared output and source preparation remains reproducible for the maintainer. Rights/notice review is explicit and public export restrictions remain until resolved. Existing runtime/image defects are tracked separately from source-route success.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
