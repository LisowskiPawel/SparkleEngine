# Packaging And Installation

**Status:** package contract and current implementation route; consumer package acceptance remains blocked

**Scope:** define deterministic staging, manifests, integrity/signing, packaging, installation, writable state, relocation, and clean-machine operation

**Owner:** repository build and release-delivery configuration; product contributors are Launcher, Showcase, Application, Tools, and the engine modules whose runtime bytes enter the package

**Snapshot:** 2026-09-08 at committed `master` revision `ffe60e3a`; root/module `CMakeLists.txt`, artifact/path discovery, product membership, and `CMake/*.cmake` were inspected; no `install(...)`, CPack, stage, manifest generation, or package owner was found; source evidence `S` only

**Strategy and acceptance sources:** [`PGE-01`, `PGE-13`, `PGE-14`, `PGE-15`](../../../Strategy/Requirements.md), [First Release](../../../Acceptance/FirstRelease.md), and `FCR-PROD-02`/`FCR-PROD-05` in [Feature Completion Reports](../../../Acceptance/FeatureCompletionReports.md)

**Acceptance:** blocked. The implementation below has local structural checks; no three-product archive or clean-machine package result exists. See [Release progress](../../../Strategy/Roadmap.md#release-progress).


## Current Package Entry

`CMake/Packaging` is the Build And Packaging owner. Root CMake generates product inputs from actual targets and registers `release_package`. It invokes existing CMake builds and AssetCooker commands, then copies only reviewed file entries into a new stage. No install rule exposes internal targets, and Launcher/Renderer/Application do not own packaging policy.

```powershell
cmake -S . -B build -DSPARKLE_RELEASE_INPUTS=<reviewed-inputs.json>
cmake --build build --config ShippingEditor --target release_package
```

The input contract is in [Release Inputs](ReleaseInputs.md#reviewed-package-inputs). Missing rights, dependency or symbol admission stops before build/cook/stage. Each retained owner receipt must pass and identify the exact source candidate. Discovery/content-store acceptance follows the produced archive; it is not a prerequisite that requires an archive before one can be made.

The stage verifier checks paths, hashes, notices, components, read-only attributes, x64 PE headers and vendor signatures. It reads ordinary/delay imports and requires app-local closure or explicit Windows/driver prerequisites; Game cannot import Editor/tool dependencies. SPDX file hashes/licenses and provenance product/source identities must match the manifest. These checks do not replace legal review or complete SPDX schema validation.

ZIP creation uses sorted membership and fixed timestamps. Independent ZIP enumeration checks membership, size, hashes and attributes before publication. The manifest excludes its own digest; ZIP verification checks its exact staged bytes, and the external report binds the archive hash.

Outputs stay under `artifacts/releases/<version>/`. Failed transactions remain under `.partial-<id>/` with reports/logs and no published ZIP. Earlier output is not overwritten. Successful assembly produces a hash-named **unsigned internal** archive; it does not pass REL-03 automatically. Trusted signing remains a pre-release step.

Core reads `ProjectPath`, `EnginePath` and `ContentStore` from the manifest. Project/engine paths are extraction-relative; cooked content is under the per-user store. Malformed/missing paths cannot fall back to a source checkout. Launcher still uses repository-only discovery and sync/cook paths: consumer entry, store initialization, selective/offline sync and removal remain unfinished. Exact payload rights, app-local CRT/helper closure, symbol correlation, private-path inspection and LGPL replacement remain unqualified. The maintainer will run CHK-PKG-02 after an archive is ready.

## At A Glance

| Product boundary | Current state | Required result |
| --- | --- | --- |
| development artifacts | implemented workspace-oriented output/copy routes | remain inputs only; never advertised as a release package |
| immutable staging | source route and structural rejection checks exist | qualify exact three-product bytes through the complete route |
| trust/publication | integrity, import and metadata verification exist | complete notices/SBOM/source obligations, symbols and pre-release signing |
| installation and writable state | Core manifest paths and per-user state exist | Launcher store initialization/sync/cook/run/remove |
| relocation and clean-machine use | unqualified | retained standard-user rehearsal of the exact archive |

The target route is one directional transaction: Build -> Cook -> Stage -> Verify/Sign -> Package -> Install/Run. Each step consumes a declared predecessor product and must never scan unrelated development directories to infer missing membership.

## Capability Identity

| ID | Capability | Current state |
| --- | --- | --- |
| `PKG-01` | Deterministic immutable staging tree and manifest | implemented source route; product archive unqualified |
| `PKG-02` | License/SBOM, signature, integrity, and publication verification | partial; legal/symbol/signing closure unqualified |
| `PKG-03` | Standard-user extract/run/remove with separate writable state | partial Core paths; Launcher journey unfinished |
| `PKG-04` | Relocated, read-only, offline, clean-machine product operation | blocked; native rehearsal unavailable until archive creation |

## Capability Boundary

The CMake package owner consumes reviewed development outputs and creates a separate immutable stage/archive. An installer, registry integration and binary SDK are outside scope. Source discovery remains a separate development route; package consumers must not inherit its fallback or cleanup behavior.

The target capability is one Build -> Cook -> Stage -> Sign -> Verify -> Package route. It must classify the runtime, editor, Launcher, host tools, symbols, source/debug material, licenses, vendor binaries, cooked content, configuration, and writable per-user state independently.

## Inputs, Outputs, Ownership, And Lifetime

- Inputs: frozen source revision, profile/toolchain/options, dependency identities, selected product, cooked-content manifest, redistribution policy, signing identity, and package version.
- Outputs: immutable staging tree, byte/permission/hash manifest, license/SBOM inventory, signatures, archive or installer, verification result, and clean-machine evidence reference.
- Build/release configuration owns membership and publication. Individual modules own declarations of their required game products; no consumer may scan development directories to infer membership.
- Package bytes are immutable. Logs, settings, caches, captures, saves, and crash data live in an explicit per-user writable root and survive or reset only by declared policy.

## Requested And Active State

The source packaging route is implemented but unqualified. A development artifact directory is not a package. The manifest and completion report must name the exact products/profiles, backend/provider support, content set, candidate and signature status before acceptance.

## Failure, Capacity, And Observability

Missing, extra, unsigned, hash-mismatched, license-incomplete, path-escaping, source-tree-dependent, or package-mutating bytes fail verification. Unsupported standard-user paths and unavailable optional providers must produce bounded actionable failures. The report records archive size, installed size, file count, peak stage/package memory, cold/warm duration, and deletion/update behavior.

## Design Decisions And Tradeoffs

| Decision | Benefit | Cost or constraint |
| --- | --- | --- |
| manifest-driven membership | reproducibility and review do not depend on directory scans | every contributing target must declare its game products |
| immutable installed bytes plus separate user state | verification, update, and uninstall behavior stay deterministic | paths/configuration must be designed for relocation and standard users |
| verify before candidate publication | corrupt, unsigned, or license-incomplete output cannot escape as a release | staging adds time, storage, signing, and provenance infrastructure |
| classify editor/game/tools/symbols independently | product contents match the intended audience and redistribution policy | multiple product profiles need explicit package matrices |

## External Precedent And Transfer Boundary

The external reference is **Unreal Engine 5.8 documentation**, retrieved 2026-09-08: [Packaging Unreal Engine Projects](https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project?application_version=5.8) and [BuildGraph](https://dev.epicgames.com/documentation/unreal-engine/buildgraph-for-unreal-engine?lang=en-US&application_version=5.8), including its [script-elements reference](https://dev.epicgames.com/documentation/unreal-engine/buildgraph-script-elements-reference-for-unreal-engine?lang=en-US&application_version=5.8). No Unreal source checkout, code commit, AutomationTool implementation, or Epic service was inspected or adopted.

Only these invariants transfer into Sparkle's local contract:

- Build, Cook, Stage, Verify/Sign, and Package are distinct owned products; Stage is a fresh declared tree outside development outputs, not a rename of `artifacts/dev`.
- A requested terminal product executes only the dependency nodes it requires; every node has explicit inputs, required predecessors, produced outputs, result, and retained artifact identity.
- Shipping membership deliberately differs from development membership, including removal of developer console/debug/authoring surfaces.
- cancellation/failure remains visible at the owning operation, and a failed prerequisite or node prevents dependent publication.

Sparkle does **not** adopt UAT, BuildGraph XML, nodes/agents/triggers as public vocabulary, Unreal's directory/Pak layout, Editor-driven packaging, or Launcher-owned package policy. The amended three-product bundle includes Launcher, with package orchestration owned by Build And Packaging. Local CMake/tool owners and the frozen Sparkle manifest remain authoritative.

## `FCR-PROD-05` Package Contract

| ID | Binary acceptance criterion |
| --- | --- |
| `AC-PKG-01` | One command consumes a frozen revision/configuration/content set and produces Build -> Cook -> fresh Stage -> Sign -> Verify -> Package results with explicit predecessor/output identities; failure at any node prevents archive publication. |
| `AC-PKG-02` | `manifests/sparkle-package-manifest.json` accounts for every staged path, byte size, hash, permission/role, owner, origin, license, signature expectation, and component; independent verification rejects missing, extra, changed, absolute, escaping, or forbidden entries. |
| `AC-PKG-03` | The bundle contains all three manifest-owned Launcher, Editor and Game components with exact release configurations and declared helper closure. Game imports no Editor/authoring dependency. Selected content uses a verified writable store separate from immutable product bytes. Symbols are separate; installer/SDK/source/debug products and undeclared helpers are absent. |
| `AC-PKG-04` | All three products start as a standard user with immutable extraction bytes read-only; Editor/Game run offline after selected content sync, from Unicode/spaced/relocated paths with no repository/build/SDK/environment/admin dependency; writes occur only in the frozen per-user root. |
| `AC-PKG-05` | Normal extract/first/repeat run, side-by-side replacement policy, archive removal, retained-user-data policy, and missing/corrupt dependency/content paths are documented and stay within frozen size/time/memory budgets. |
| `AC-PKG-06` | SBOM/provenance, notices/redistribution decisions, trusted PE signatures, checksums, archive/source/candidate identity, and native/shader symbol correlation all refer to the same immutable candidate. |

| ID | Cause/injection, safe result, and affected criterion |
| --- | --- |
| `FM-PKG-01` | Seed stale/extra/source/debug or undeclared helper files, or omit any declared Launcher/Editor/Game component or required input. Fresh staging plus manifest verification rejects the tree before signing/publication (`AC-PKG-01`–`03`). |
| `FM-PKG-02` | Alter a copied PE/archive/manifest/license/signature after staging. Verification quarantines the output and no candidate/archive success is emitted (`AC-PKG-02`, `AC-PKG-06`). |
| `FM-PKG-03` | Make package bytes read-only, remove overrides, relocate, and block network. Any package/repository write or external-path/SDK dependency fails `AC-PKG-04`. |
| `FM-PKG-04` | Deny/fill/corrupt the per-user root. The runtime names the path/space, preserves immutable bytes and last accepted state, and exits or disables only the optional operation (`AC-PKG-04`, `AC-PKG-05`). |
| `FM-PKG-05` | Remove each allowlisted DLL/content file from isolated archive copies. Startup identifies the missing component and exits without partial success or fallback content (`AC-PKG-03`–`05`). |
| `FM-PKG-06` | Search output names/docs/files for installer or SDK semantics. Any bootstrapper/registry/install API/binary ABI implication without an admitted owner fails `AC-PKG-03` and blocks publication. |

| Check | Claims falsified | Smallest route and fixed oracle |
| --- | --- | --- |
| `CHK-PKG-01` | `AC-PKG-01`–`03`, `AC-PKG-06`; `FM-PKG-01`, `FM-PKG-02`, `FM-PKG-06` | Generate from a clean destination, retain per-node results, independently enumerate/verify allowlist, denylist, hashes, signatures, SBOM/notices/symbol identity, then inject stale/extra/missing/tampered inputs. No archive may exist after a failed node. |
| `CHK-PKG-02` | `AC-PKG-04`, `AC-PKG-05`; `FM-PKG-03`–`05` | On a clean standard-user minimum machine, extract to Unicode/spaced and read-only locations, test user-selected sync, then block network/remove overrides, inventory all file/network/process activity, run consumer first/repeat/fault cases, remove archive, and compare budgets/state hashes. |

No acceptance row is satisfied by this documentation pass. [`BUILD-E03`](../CapabilityEvidencePlan.md#product-workflow-and-delivery-evidence) is the smallest central proof destination; candidate-bound results belong in the release artifact path required by [First Release](../../../Acceptance/FirstRelease.md).
