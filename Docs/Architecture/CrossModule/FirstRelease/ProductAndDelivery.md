# First Release Product And Delivery Plan

Deliver Sparkle Launcher, Sparkle Editor and Sparkle Game with user-selected release content, a reproducible source build and self-service recovery.

**Status:** implementation plan; phase work and exit criteria, not feature or release evidence

## Work This Plan

[Current progress](../../../Strategy/Roadmap.md#release-progress) separates implemented work from missing acceptance checks. Do not use the phase order below as a claim that earlier source work is absent.

**Next:** qualify [reviewed package inputs](../../Modules/BuildAndPackaging/ReleaseInputs.md#reviewed-package-inputs), then continue [PD-3](#pd-3--package-and-installation) to produce the exact archive. Complete missing PD-1/PD-2 checks in their existing owners; PD-4's native package checks wait for that archive. Admission blockers remain stops, not waivers.

## `PD-1` — Game And Editor Product Shells

**Goal:** close `FCR-PROD-01` and the required shipped Editor family `FCR-PROD-04` so the first run, controls, settings, level selection, failure, reset, and exit are intentional.

**Non-goals:** turning Showcase into a general sample browser framework, adding a general Editor framework, or hiding engine failures behind generic dialogs.

**Required work:**

- preserve `GameApplication` as game host and extend the current Showcase entry/product path rather than adding a second launcher;
- make the expected Launcher -> Editor/Game entry, selected-map activation, loading state, controls/help, settings/reset/quit and unsupported-state messaging discoverable; use user/display-selected resolution and approved chromatic aberration/vignette On defaults; keep benchmark resolution separate;
- ensure level failure cannot silently become successful `Empty` content and settings resolve requested versus active state truthfully;
- keep Editor layering explicit; prove its required shipped journey and keep Editor dependencies isolated from the Game component;
- use representative fresh-user, standard-user, resize/DPI, missing/corrupt content, unsupported-provider, and shutdown routes from the owned contracts.

**Failure modes to control:** blank first frame; trapped mouse/focus; stale settings; load failure shown as success; editor-only service required by Game; quit leaves work or files in flight; fallback is active but hidden.

**Phase exit criteria:** the feature-local checks for `FCR-PROD-01` and the selected `FCR-PROD-04` scope pass on the exact configuration; all three products are represented truthfully and Game imports no Editor/authoring dependency; results are filed in candidate FCR reports.

## `PD-2` — Source Build, Cook, And Launcher

**Goal:** close `FCR-PROD-02` and `FCR-PROD-03` with one documented underlying operation model usable directly and through the required shipped Launcher for the seven release maps.

**Non-goals:** making launcher UI the only automation API, duplicating build/cook logic in GUI code, or accepting warmed author-machine state.

**Required work:**

- pin public prerequisites and dependency identities; define supported profiles, outputs, cleanup, and source compatibility statement;
- add a fresh-checkout automation entry for the approved deterministic build/static/package gates; it invokes owner checks and reports failed, timed-out, cancelled, skipped, unavailable, and missing-artifact states without reinterpretation;
- keep sync/configure/build/cook/run/clean as typed operations with one execution authority and truthful process result;
- make Launcher discovery, readiness, progress, cancellation, error detail, retry/restart and concurrent-action exclusion explicit; qualify Empty, each of the seven maps and a selected subset with exact sync/helper closure, marking other choices development or unavailable;
- prove cold and warm build/cook paths plus missing tool, bad path, canceled operation, failed child, locked output, stale cook, and Unicode/path quoting behavior;
- ensure direct command and launcher invoke equivalent underlying semantics and artifacts.

**Failure modes to control:** false-success summary; launcher PID mistaken for launched runtime; partial cook published; cancel leaves child processes; clean deletes unowned files; prerequisite is fetched mutably; GUI command differs from documented CLI.

**Phase exit criteria:** independent source-adopter rehearsal produces the named runtime through documented prerequisites; launcher classification is enforced; direct and GUI routes agree; exact logs/artifacts and negative results are in the FCR reports.

## `PD-3` — Package And Installation

**Goal:** implement the missing `FCR-PROD-05` owner and pass `REL-03` with a manifest-owned three-product portable Windows x64 bundle and verified user-selected content store.

**Non-goals:** an installer, registry integration, a binary SDK, package self-modification, or bundling developer tools/source/debug files without explicit admission.

**Required work:**

- select one existing build/delivery owner for build → cook → stage → verify → package; do not put packaging policy in renderer or application code;
- add explicit install/stage membership, component/allowlist policy, package-relative discovery, per-user writable state, manifests, notices/SBOM, provenance, hashes, and symbols handling;
- make staging fresh and transactional; reject stale, missing, extra, absolute, unlicensed, or incompatible products;
- create the archive from an immutable verified stage, then verify archive contents against `manifests/sparkle-package-manifest.json`;
- freeze exact Launcher/Editor/Game release configurations, executable names, helper closure and content-store/workspace paths; prove extract/start/selective-sync/offline-run/remove with no source checkout, undeclared toolchain, admin rights, environment override or author cache. CMake's [install rules](https://cmake.org/cmake/help/latest/command/install.html) are a mechanics reference, not an instruction to expose internal targets.

**Failure modes to control:** stage accumulates leftovers; runtime writes beside executable; package relies on current directory; missing redistributable/provider DLL; manifest mismatch; source/private path leak; archive emitted after verification failure.

**Phase exit criteria:** the Architecture-owned `FCR-PROD-05` criteria and `REL-03` pass for the exact archive; a clean standard-user machine route is retained; stage/package failure leaves no publishable partial artifact.

## `PD-4` — Support, Incident Response, And Product Closure

**Goal:** close the amended `FCR-PROD-06`: self-service onboarding, local diagnostics, known-issue review and immutable replacement, with no user support service.

**Non-goals:** silent telemetry, collecting private state by default, promising perpetual compatibility, or calling documentation an operated process.

**Required work:**

- publish truthful no-support and known-issue/self-service instructions; define local diagnostic privacy, retention, symbols and finite release-review ownership;
- connect process crash/hang and GPU/device diagnostics to a bounded user-visible collection/export route without leaking paths or content;
- define immutable patch identity, applicability, rollback/withdrawal/advisory, and evidence invalidation;
- rehearse one crash, one hang/timeout if supported, one GPU/device diagnostic route, one critical-defect review, and one replacement/withdraw decision;
- reconcile all six product FCR reports against the same candidate and package identity. Unreal's [crash-reporting architecture](https://dev.epicgames.com/documentation/en-us/unreal-engine/crash-reporting-in-unreal-engine) is a responsibility reference only; no Epic service is assumed.

**Failure modes to control:** report loses candidate identity; consent bypass; private paths or content escape; symbols cannot match build; unsupported recovery promised; patch replaces public bytes under same version; public policy invents an intake or response promise.

**Phase exit criteria:** all three self-service journeys and local-diagnostic routes were exercised; no service deadline is advertised; failures preserve consent and identity; patch policy is reproducible; all six FCR reports have explicit candidate-bound verdicts and residual limitations.

## Plan Completion

This plan is complete only when all six product FCRs have Architecture-owned contracts, exact candidate reports, and no unresolved blocker for their admitted scope. The release is not complete until the mother plan's remaining map, native, adoption, publication, and stabilization gates pass.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Responsibility:** sequence the first-release product journeys, source/Launcher workflows, three-product archive and self-service diagnostics

**Orchestrator:** [First Release Implementation Plan](README.md)

**Architecture owners:** [Projects/Showcase](../../Modules/Projects/Showcase/README.md), [Application](../../Modules/Engine/Application/README.md), [Editor](../../Modules/Engine/Editor/README.md), [Launcher](../../Modules/Tools/Launcher/README.md), and [Build And Packaging](../../Modules/BuildAndPackaging/README.md)

</details>
