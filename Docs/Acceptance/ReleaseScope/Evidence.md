# Release Scope Evidence

**Status:** dated source audit and iteration records; release acceptance remains blocked

**Responsibility:** retain source identities, rights observations and historical checks behind the [scope decisions](README.md).

**Snapshot:** reconciled 2026-10-10 at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`. Earlier iterations retain their own revisions and dirty-state descriptions.

These records prove only the inspection or document checks they name. Current product choices and next actions belong in [Release Scope](README.md#approval-and-next-work); this page does not grant redistribution permission or build/runtime acceptance.

## Selected Content Receipts

This is the seven-map source/rights snapshot from the reconciliation. [ReleaseMapSet](README.md#releasemapset) owns intended membership. Cameras and input identities below are retained for the owning workload and content checks.

| Level | Product classification | Fixed view/route | Input and rights decision |
| --- | --- | --- | --- |
| `CornellBox` | Included; reference/lighting example | Authored camera `(0, 1, 3.8)`, yaw pi, pitch 0, FOV 45; fixed camera, 300 warmup + 300 sampled frames, three repeats | Existing publisher archive revision 2011-07-27; catalog SHA-256 `f27eaf47afec74b5236ce3095ac02d4256e8ac8610bfdb5a53560c67c2e0d8f1`. Inspected local publisher notice states CC BY 3.0; original OBJ additionally states public domain. [Owned conversion/attribution record](../../../Projects/Showcase/Assets/Meshes/CornellBox/Conversion/README.md) names Guedis Cardenas/Morgan McGuire. Current GLB hash matches its documented `a2af112abeaa1b08087009afa3efef2c6b665c59bd7a6efe70d703e8f2349d91`. Retain complete redistribution/cook receipts in `REL-01`; rect-light behavior still needs its real feature proof. |
| `CesiumMan` | Included; animation example | Authored camera `(0, 1.5, -3)`, yaw/pitch 0, FOV 60; stationary camera, animation running; same sampling protocol | [Pinned upstream metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/CesiumMan/README.md) identifies CC BY 4.0 and separate logo/trademark notice. Local model JSON matches this revision after external URI relocation; both referenced buffer/texture bytes match exactly. Retain attribution and separate logo notice; no endorsement claim. Cooked animation/material fidelity and final notices remain packaging proof. |
| `Sponza` | Included; classic lighting example | Existing authored camera; freeze exact static and motion/disocclusion observations in MAP-00 | [Pinned primary metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/Sponza/README.md) names a CRYENGINE-specific agreement. Selection is not redistribution permission; REL-01 must establish exact usable terms and local dependency identity. |
| `ModernSponza` | Included; first-run/default | Authored camera `(5.39, 6.53, 0.26)`, yaw 4.77, pitch -0.58, FOV 66.9; `REL-NATIVE-1080P` | Current level uses base plus colorful curtains, not candle/knight variants. Catalog pins both archives; [publisher samples](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html) describe attribution licensing. Both installed embedded notices contain CC BY 4.0 text alongside narrower personal/educational/limited-commercial wording. REL-01 must resolve that inconsistency with the rights owner and retain exact dependency/notices identity; publisher-page wording alone cannot clear distribution. Combined source archives are 4,774,507,032 bytes; source size is not cooked package size or GPU complexity proof. |
| `LPSHead` | Included; head/material example | Existing authored camera, fixed pose | [Owned conversion record](../../../Projects/Showcase/Assets/Meshes/LPSHead/Conversion/README.md) names source attribution, geometry conversion and omitted bump-map interpretation. Retain pinned archive/texture/GLB identity and prove actual material fidelity; selection does not certify subsurface transport. |
| `DiffuseTransmissionPlant` | Included; plant example, material qualification open | Existing authored camera; freeze animation pose for any admitted reference comparison | [Pinned primary metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/DiffuseTransmissionPlant/README.md) describes diffuse transmission and animated punctual lights. Qualify the exact local importer/rendered material contract before package admission; unsupported effects cannot be silently dropped or treated as reference truth. |
| `ABeautifulGame` | Included; chess/material example, material qualification open | Existing authored camera, static view | [Pinned primary metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/ABeautifulGame/README.md) names transmission/volume and attribution terms. Close exact local source/dependency identity and material support; an explicitly authored reduced-domain variant requires a separate owner decision. |

## FR-00 Review Package

**Reconciled:** 2026-10-10 at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`, initially clean tree. This is source, document and receipt inspection only.

**Owner map revision incorporated:** the two-map proposal is superseded by seven selected maps and Modern Sponza/native 1080p Lit first run. Bistro and Moana are postponed. The named laptop and mandatory rendering requirements remain approved. The finite Windows ShippingGame/source product and compatibility/support proposals still need approval; selected maps need rights, material and package-budget qualification. No runtime acceptance is inferred.

### Facts Closed By Inspection

- All 13 retained production/configuration inputs are byte-identical to the preceding scope audit. The ledger remains 512 IDs, 16 inventory owners, 20 catalog levels, 18 view modes and 26 persisted names. No new classification is needed for these unchanged surfaces.
- Cornell Box has eight source materials, no skin or animation, and only `KHR_materials_emissive_strength`. Cesium Man has ordinary metallic/roughness materials, no declared glTF extensions, one skin and one animation. Neither level declares a Sky section. This closes the Cornell/Cesium source-domain inspection only, not importer fidelity, a cooked dependency manifest or radiance correctness. Material values and exact input hashes are retained in the receipt artifact.
- All eight scene-source files used by the seven selected levels are available locally and hashed, including Modern Sponza base and curtains. Plant declares diffuse transmission and an animation; A Beautiful Game declares transmission/volume. These are source facts, not proof the importer or renderer implements them. Modern Sponza and LPS Head installed notices are retained by exact hash; Modern Sponza usage wording needs rights-owner resolution.
- The accepted reflective reference domain is preserved. It does not expand to transmission, volume, spectral or physical subsurface transport. Cesium reference comparisons require a frozen pose; Cornell's emissive panel and authored rect light require a single physical emitter interpretation in the existing reference/light owners. These are executable qualification checks, not a restart of `PTD-00`.
- Streamline is pinned to 2.11.1 by the owning dependency CMake. Current initialization explicitly rejects Vulkan. Current RR evaluation uses `sl::kFeatureDLSS_RR` after setting SR/RR options; this does not prove native support or a second final-image composition. Both-API SR/RR remains mandatory work in `DSP-3`; Linear fallback cannot satisfy the realtime preset.
- `REL-NATIVE-1080P`, `REL-RT-1080P` and `REL-REFERENCE` keep separate purposes and the existing budgets. SDK-derived render extent, realtime depth/reuse controls and exact source-to-output composition must be frozen in the owning feature discoveries before measurement. No arbitrary numeric setting or performance result is invented here.

### REL-00 Clause Coverage

| Contract area | Reviewable record | Remaining decision or proof |
| --- | --- | --- |
| Product, executable, version, audiences and developer contents | [Product/selection sections](README.md#decision-summary) and [capability ledger](CapabilityDispositions.md) | Owner approves finite runtime/source promise |
| Windows, GPU/API, machines, storage and source toolchain | [Product matrix](README.md#product-and-supported-matrix); named laptop approved | Owner approves proposed broader OS boundary; later qualification is unrun |
| Rendering surface and default configuration | [Baseline/selection](README.md#baseline-and-selection); mandatory amendment; every view/setting disposition | Owner approves consumer defaults; numerical feature controls close before their runs |
| Maps, first run, routes and redistribution | [ReleaseMapSet](README.md#releasemapset), cameras and selected input receipts | Membership/default selected; exact rights, material/domain and size receipts remain |
| Compatibility, reset, update and side-by-side | [Mutable-data/compatibility section](README.md#mutable-data-and-compatibility) | Owner approves publication promises; no compatibility implementation added |
| Budgets, observation, reference and >=30 FPS | Budgets/observation and acceptance owner | Existing thresholds preserved; executable results unrun |
| Display/input/language/accessibility and non-goals | Baseline/selection and product limitations | Proposed finite consumer surface awaits approval; HDR needs display qualification |
| Network, privacy, supply chain, signing and distribution | [Network/security/support section](README.md#network-security-publication-and-support) | Policy approval; publisher/contact/credentials and complete receipts in REL-01 |
| Support, security response, patch and withdrawal | Same finite support policy | Owner approves service promises; working channels remain unprovisioned |
| Accepted tracer discovery versus release oracle | Accepted PTD-00-R1 plus reference feature plan | Development discovery PASS preserved; PTD-02/03 and Stage 10 remain unaccepted |

Current decisions and next actions are maintained once in [Release Scope](README.md#approval-and-next-work). Cornell's required archive is `224880` bytes with SHA-256 `f27eaf47afec74b5236ce3095ac02d4256e8ac8610bfdb5a53560c67c2e0d8f1`; both reconciliation publisher requests returned HTTP 406 and inspected Launcher caches had no matching archive. Cesium buffer/texture bytes matched retained upstream pins; [refreshed pinned metadata](https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Assets/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/CesiumMan/README.md) retained distinct attribution/logo terms. Neither observation grants redistribution permission.

Artifacts: `build/validation/fr00-reconciliation-20261010/` retains baseline source identities, selected material/asset receipts, exact commands and refreshed coverage results. Earlier receipts remain useful under their recorded identities.

## ITER-REL-00-03 Current-Source Reconciliation

| Field | Record |
| --- | --- |
| Scope / start | FR-00 only; release/document owner, `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`, initially clean. No production/CMake/configuration edits. |
| North Star / persona | Preserve NS-EVIDENCE, NS-OWNERSHIP and NS-MATH-DATA; improve the review route toward NS-ADOPTION. PGE-01/13/15 scope clarity only; no evidence-level or readiness increase. |
| Decision and risks | Missing owner decision or unavailable archive may be mistaken for approval. Detect through explicit proposal labels, primary/pinned identities and the unchanged acceptance gate. Retain the exact missing owner/input; do not substitute assets or weaken Included obligations. |
| AC / FM / checks | Existing AC-SCOPE-01..04, FM-SCOPE-01..04 and CHK-SCOPE-01..03; exact inventory/classification and input hashes, three in-memory omission/duplicate/weakened-class negatives, local links/anchors, UTF-8/whitespace and git diff --check. |
| Rights/domain inspection | Selected level/source identities plus available glTF/GLB material, extension and animation inventory; publisher archive requests and current workspace cache inspection; current Cesium hashes and primary metadata. No cook, legal clearance, oracle or native qualification inferred. |
| Static result | **PASS:** 118 checks across three changed documents, 147 local links, 104 anchors, 13 unchanged production inputs and three rejected in-memory negative controls. Seven exact selected map IDs and all eight available scene-source identities checked. This is document/source consistency only. |
| Disposition | Static coverage and source-domain inspection complete; final scope decision and exact rights remain BLOCKED. Original development PTD-00 PASS preserved; no source/native/package/release claim promoted. |

## ITER-REL-00-01 Control Record

| Field | Record |
| --- | --- |
| Scope / owner | Stage 0 only; repository maintainer/release owner. Start revision `93e86b8113c3fcd0c4921729c3513538b95bc794`, clean tree. Documentation/source/host inspection, no production or build configuration changes. |
| North Star / persona | `NS-REAL`, `NS-EVIDENCE`, `NS-OWNERSHIP`, `NS-ADOPTION`, `NS-SIMPLIFY`; `PGE-01`, `PGE-05`–`10`, `PGE-13`, `PGE-15` advance scope clarity only; mathematical/graphics proof preserved, not increased. |
| Delivery mapping | `REL-00`, `FR-00`, `RISK-REL-01/05/09/11/12/13`; all inventory owners and `FCR-*` routes retained; `PTD-00` accepted development discovery preserved, not rerun or promoted to oracle proof. |
| Complexity / preservation | Two subject-local scope/ledger documents; navigation edits only. No new production API, data copy, dependency, implementation plan, test harness, legacy alias or runtime mechanism. Preserve PD-0 budgets and mandatory feature obligations. |
| Risk `RISK-SCOPE-01` | Historical inventory/count/default mistaken for live support -> overbroad product claim. High likelihood from dated snapshot; high impact. Detect via live catalog/settings/enums/target membership and exhaustive ID reconciliation; reduce/fix proposal and hold approval. Release owner retires with approved ledger and retained source inputs. |
| Risk `RISK-SCOPE-02` | Third-party sample availability mistaken for redistribution/material fidelity -> invalid public package. High likelihood from mixed license and extension metadata; critical impact. Detect through primary metadata, asset hashes/notices and material-consumer trace; exclude ambiguous content pending receipts. Content owner retires through `REL-01`, not this source audit. |
| `AC-SCOPE-01` | Every existing module inventory capability ID has exactly one disposition and module owner; all 20 level entries, 18 view enum values and 26 persisted selector names (25 common plus one conditional Editor setting) have an explicit distribution/classification decision. |
| `AC-SCOPE-02` | Product/audiences, named machines, baseline, maps, budgets, mutable roots, compatibility/security/support policies are concrete and trace to the acceptance contract. Mandatory Included targets remain Included; no executable acceptance is inferred. |
| `FM-SCOPE-01` | Missing/duplicate/unmatched row, new catalog entry or selector -> coverage check fails, retain source input, revise ledger before approval; an unexplained omission is a scope blocker. |
| `FM-SCOPE-02` | Unknown asset right or unqualified feature -> explicitly unresolved/excluded, never approved by source presence; retain primary metadata and request owner disposition before package work. |
| Checks | `CHK-SCOPE-01`: exact ID/set coverage, source-reference existence, catalog/view/settings reconciliation and mandatory-feature assertion. `CHK-SCOPE-02`: local Markdown links/anchors, UTF-8, stale names and whitespace. `CHK-SCOPE-03`: review against each `REL-00` clause, public-versus-source boundary and existing PD-0 constants. Negative controls reject missing/duplicate IDs and an invalid disposition using in-memory inputs; no permanent probes. |
| Result / invalidation | **SUPERSEDED for amended documents:** the original 511-ID static slice remains retained in `build/validation/release-scope-20261010/`; amended requirements/classifications are rechecked in ITER-REL-00-02 below. Rights receipts remain useful within their stated identities. No build/cook/runtime/native/performance/package/adopter verdict is added. |

## ITER-REL-00-02 Rendering Amendment

| Field | Record |
| --- | --- |
| Scope / start | Owner-requested revision of Stage 0 only, at `93e86b8113c3fcd0c4921729c3513538b95bc794`; the preceding eight-document scope draft is pre-existing owned work and preserved. No production/CMake/configuration changes. |
| Outcome / mappings | `NS-REAL`, `NS-MATH-DATA`, `NS-EVIDENCE`, `NS-OWNERSHIP`, `NS-ADOPTION`; `PGE-02/05/06/08/09/13/15` advance requirement precision only. `REL-00/04/06/07`, FCR-REN-02/06/07/08/09/10/14/15/25 and existing domain/feature checks. No score or native acceptance increase. |
| Structural budget | Amend existing scope/ledger/feature/plan/report owners; one small Vignette dossier and target inventory ID REN-POST-14. FCR-REN-25 owns two independently proved lens results; retain 49 families, remove weaker Experimental/source-only classifications for mandatory rendering, preserve PTD-00 and separate graph/feature/native authority. |
| `RISK-SCOPE-03` | Required reference/realtime/provider/parity work remains labelled experimental or source-only -> release escapes the requested quality target. High likelihood from previous proposal; critical impact. Detect via disposition/surface/mode assertions, numeric preset review and owner/phase crosswalk; fix docs and hold acceptance. Release owner retires through approved scope plus real feature/performance/review reports. |
| `AC-SCOPE-03` | Required tracer/reconstruction rows and Reference mode are Included/runtime; vignette has one owner/ID/dossier; two independent lens proofs preserve the 49-family registry. Existing exclusions remain explicit. |
| `AC-SCOPE-04` | Native/realtime/reference presets, >=30 FPS and reference truth have distinct proof contracts; parity requires both APIs; principal review requires retained independent identity/findings and full frame evidence. No fallback, finite diagnostic, generated frame, unrun reviewer or metadata can pass those gates. |
| `FM-SCOPE-03` | Missing vignette/duplicate ID or a mandatory row reverted to Experimental -> static coverage/required-row checks reject; revise before freeze, no production mutation. |
| `FM-SCOPE-04` | Source/SDK/example output treated as feature completeness -> retain the current D3D12-only initialization gap, PTD domain/uncertainty limits and unrun review/performance status; acceptance stays blocked. |
| Checks / artifacts | Exact inventory/ledger/class/mode/settings coverage, negative in-memory omission/duplicate/weakened-class controls, local links/anchors, UTF-8, whitespace, production-input hash preservation and `git diff --check`; retained commands/results in `build/validation/release-scope-amendment-20261010/`. Native builds/runs/review are not appropriate proof for this documentation-only amendment and remain unrun. |
| Static result | **PASS:** 138 checks, 22 changed/new documents, 622 local links, 152 anchors, 13 unchanged production inputs and three rejected in-memory negative controls at the revision above. Coverage is 512 capability IDs across 16 inventory owners; the existing 49 report families remain unchanged. This accepts documentation consistency only. |
| Disposition / next work | Rendering requirements incorporated; REL-00 remains BLOCKED for remaining scope/rights/domain/preset freeze. Execute dependency-ready source/SDK/domain discovery through existing owners; implementation requires its own authorized phase and checks. Preserve numerical budgets and do not retune them to candidate results. |

## ITER-REL-00-04 Readability

| Field | Record |
| --- | --- |
| Scope / start | User-requested scope-document cleanup at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`; pre-existing changes in ReleaseScope/README.md, FirstRelease.md and the first-release plan preserved. Owner: documentation/release maintainer. |
| Outcome / targets | Advance NS-SIMPLIFY and NS-ADOPTION through concrete decisions; preserve NS-OWNERSHIP/NS-EVIDENCE and PGE-13/15 contracts. `REL-00` readability only; no feature or release evidence promotion. |
| Change boundary | Scope README owns decisions; this evidence page owns receipts/history. Acceptance and stage-plan entry links point to the decisions. No runtime exposure, code, build configuration, threshold or approval changes. |
| RISK-SCOPE-04 | Dense audit text hides real commitments or cleanup erases a requirement. High likelihood given reader feedback; high impact if publication promises change. Prevent with explicit unanswered proposals and retained exact contracts/receipts; detect through contract, link and negative-control checks. Documentation owner restores missing requirements before handoff; retire when checks pass. |
| AC-SCOPE-05 | Reader can identify settled choices, seven unanswered proposals with values/consequences, and agent investigations without reading audit history. Scope/rights remain BLOCKED. |
| AC-SCOPE-06 | Seven map IDs, exclusions, operation/performance limits, precise contracts, source identities and previous iteration results remain available through one owning route. Incoming document links resolve. |
| FM-SCOPE-05 | Cleanup omits a selected map, labels an unanswered row approved, changes a budget or breaks a route. Static check rejects; restore the requirement/link. Documentation regression; no source/candidate mutation. |
| CHK-SCOPE-04 | In-memory contract assertions, UTF-8/whitespace, scoped Markdown links/anchors plus incoming links, rejected missing-map/false-approval/changed-budget/broken-link controls and git diff --check. AC-SCOPE-05/06, FM-SCOPE-05 and RISK-SCOPE-04. |
| Decision / evidence | **PASS for document readability/consistency:** scoped local and incoming links, UTF-8/whitespace, contract assertions, four rejected in-memory negative controls and git diff --check. Exact command and hashed results: `build/validation/release-scope-readability-20261010/commands.md` and `verification.json`. Existing REL-00 remains BLOCKED. No build/cook/runtime/GPU/package acceptance is claimed. Recheck on relevant document changes. |
