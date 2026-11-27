# SourceImporters: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Admit only the retained scene inputs and animation subset, preserving exact geometry/material/camera/light semantics needed by the portfolio.

**Retain:** cgltf glTF/GLB, FBX/Assimp for Bistro/Knight, source-authored cameras/lights, tangent generation/remapping and basic skeletal input. Shared geometry/material/bone conversion remains.

**Target flow:** Explicit source path -> format reader -> one checked imported scene representation -> cookers. Imported arrays/indices and owned bytes remain valid through parallel consumers.

Dominant execution owner for: IM-01, IM-02, IM-03, IM-04, IM-05, IM-06, IM-07, IM-08, IM-09, RV-04, RV-05. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **89 files / 5,900 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **4 files / 269 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| IM-08 | 4 files / 439 lines |
| IM-09 | 6 files / 547 lines |

## Stage 0: freeze the actual responsibility and smaller target

Read the retained nine-at-audit glTF/GLB and three FBX metadata, then actually import native Knight/Bistro before extraction deletion. Refresh the current ten-map target after Candles removal. CesiumMan declares one clip/57 LINEAR samplers; FBX samplers default linear. This is evidence for a candidate subset, not runtime animation qualification.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Import/SourceImporters/Private/Fbx/FbxImporter.cpp](../../../Tools/Import/SourceImporters/Private/Fbx/FbxImporter.cpp)
- [Tools/Import/SourceImporters/Private/Fbx/FbxEmbeddedTextureImporter.cpp](../../../Tools/Import/SourceImporters/Private/Fbx/FbxEmbeddedTextureImporter.cpp)
- [Tools/Import/SourceImporters/Private/Gltf/Geometry/GltfMeshGeometryExtractor.cpp](../../../Tools/Import/SourceImporters/Private/Gltf/Geometry/GltfMeshGeometryExtractor.cpp)
- [Tools/Import/SourceImporters/Private/Fbx/FbxAnimationImporter.cpp](../../../Tools/Import/SourceImporters/Private/Fbx/FbxAnimationImporter.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove PLY and embedded FBX texture extraction

Decisions: IM-03, IM-06, RV-05.

Delete PLY-only reader/dispatch/build options and FBX embedded-image extraction/cache/TGA encoding plus exclusive callers and cache writes. Retain Assimp FBX and externally resolved Bistro DDS, PNG/JPEG/default EXR routes. Remove unsupported admission explicitly; do not delete shared Assimp or TGA decoding used elsewhere. GameFramework owns morph removal through this importer.

**Exit evidence:** Native retained FBX imports confirm no embedded payload is required; missing external texture errors stay actionable. PLY/embedded unsupported inputs fail cleanly and cannot partially replace accepted outputs. Knight skeleton/geometry and Bistro material/camera/light conversion remain intact.

## Stage 2: Narrow animation curves and clip selection conditionally

Decisions: IM-08, IM-09.

Select required Knight/Cesium clips explicitly; drop Step/Cubic/all-source-clips obligations only after prepared equivalent data passes. Preserve linear TRS/quaternion interpolation, timing, bind transforms, joint association and all required channels. Integrate with SceneCooker Ozz preparation; do not add a second runtime conversion path or force one worker.

**Exit evidence:** Import/cook and threaded runtime playback of Knight and CesiumMan on both APIs preserve motion, skinning, previous-frame and RT geometry behavior. If either needs a supposedly removed curve/clip route, retain that minimum support and leave the cut conditional. Source declarations or a static pose do not pass.

## Stage 3: Narrow retained conversion flow and external references

Decisions: IM-01, IM-02, IM-04, IM-05, IM-07, RV-04.

After fixed Knight preparation, remove unused importer-to-cooker adapters and duplicated coordinate/unit/material conversion. Keep source cameras/lights and MikkTSpace-backed missing tangents: DamagedHelmet plus ABeautifulGame need them. Retain current FBX source pipeline, not an external-only conversion replacement.

**Exit evidence:** Check geometry/index bounds, normals/tangents/handedness, units, camera/light transforms and texture paths. Compare imported/cooked outputs and bounded rendered examples; retained inputs render correctly, and narrowed admissions have no permissive fallback.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
