# Adoption, Support, And Incident Response

**Status:** target product capability dossier; self-service adoption/local diagnostics remain unproved; ongoing support excluded

**Scope:** define public onboarding, failure recovery, local diagnostics, self-service recovery, release review, immutable replacement and independent reproduction

**Owner:** release/product delivery; Launcher, Application, Core diagnostics, Docs, and project products provide owned parts of the route

**Snapshot:** 2026-09-08 at committed `master` revision `ffe60e3a`; current product/workflow inventories, repository/public entry points, Core/RHI diagnostics, symbols, and First Release failure/support surfaces reconciled; source evidence `S` only

**Strategy and acceptance sources:** `NS-ADOPTION`, `NS-EVIDENCE`, and `NS-OWNERSHIP` in the [Engineer Persona](../../../Strategy/EngineerPersona.md); `FCR-PROD-01`, `FCR-PROD-02`, `FCR-PROD-06`; [First Release](../../../Acceptance/FirstRelease.md)

**Current readiness:** **15/100** — developer diagnostics and partial onboarding exist; the dated public adoption/diagnostic/replacement snapshot is incomplete; the current scope excludes ongoing support. See [Current Feature Readiness](../../../Acceptance/CurrentReadiness.md#product-build-and-delivery).

## At A Glance

The release owner offers **no user support service**. Deliver self-service onboarding, known issues, local diagnostics and reproducible recovery. Do not require a staffed bug/security inbox, maintained-version window, acknowledgement deadline or guaranteed fix. [Release Scope](../../../Acceptance/ReleaseScope/README.md#decision-summary) owns that decision.

The source snapshot above remains historical; changing this contract does not improve readiness or prove implementation.

## Capability Identity

| ID | First-release boundary |
| --- | --- |
| `ADOPT-01` | Public identity, requirements, controls, first use, reset and exit for Launcher, Editor, Game and source adoption. |
| `ADOPT-02` | Actionable local startup/content/backend failure and self-service recovery. |
| `ADOPT-03` | Bounded, inspectable local diagnostics and candidate/symbol correlation; intake/upload service excluded. |
| `ADOPT-04` | Finite maintainer release review and immutable replacement/withdrawal; ongoing response/maintenance service excluded. |

## Capability Boundary

A non-author can reach the promised product result or understand its failure without private guidance. Core/RHI/Application own diagnostic production; release delivery owns public instructions, local-data privacy, candidate-to-symbol identity and release review. No reporting channel or automatic upload is implied.

## Contract

- Inputs: verified product/source identity, selected content, public prerequisites, exact configuration and local diagnostic records.
- Outputs: successful first use or actionable failure, inspectable diagnostics, documented reset/removal and truthful known issues/no-support policy.
- Local logs/captures/crash data are bounded and associated with the exact candidate. Explain sensitive paths/content before any voluntary sharing; retain originals locally.
- Any voluntarily published correction has a new immutable identity, affected-gate revalidation and a documented user-state policy.

## `FCR-PROD-06` Support And Incident Contract

This stable family ID now covers adoption, local diagnostics and release review. Its former operating-service clocks and mandatory intake criteria are superseded by the release-owner amendment. [First Release](../../../Acceptance/FirstRelease.md#frozen-paths-mutation-boundary-and-budgets) owns paths/budgets and the no-support boundary.

| ID | Binary acceptance criterion |
| --- | --- |
| `AC-ADOPT-01` | A non-author reaches onboarding for all three products and source adoption; identifies exact version, requirements, controls, reset/exit, known issues and the absence of a user support service without private guidance. |
| `AC-ADOPT-02` | Startup, crash, hang/timeout and GPU/device failure produce bounded inspectable local diagnostics where supported. No automatic upload occurs; unavailable diagnostics are reported explicitly. |
| `AC-ADOPT-03` | Native/shader symbols, local logs, consented dumps/captures and manifest/provenance correlate to the exact immutable candidate; mismatched/insufficient data is rejected with recovery. |
| `AC-ADOPT-04` | Maintainer release review records known defect severity, affected candidate/version and disposition. Public documentation promises no intake service, response deadline, supported-version period or fix. Known `S0`/`S1` blocks approval/closeout. |
| `AC-ADOPT-05` | Any replacement uses a new version; hash applicability, user-state preservation/reset, rollback/side-by-side and withdrawal are explicit and exercised on an isolated candidate copy. Published bytes never change under an existing identity. |

| ID | Cause/injection and safe result |
| --- | --- |
| `FM-ADOPT-01` | Omit a prerequisite, break onboarding or insert an unsupported service promise. The journey blocks until accurate instructions/no-support boundaries are restored (`AC-ADOPT-01`, `AC-ADOPT-04`). |
| `FM-ADOPT-02` | Add private paths/tokens, unrelated content, oversized logs or wrong candidate identity to a proposed diagnostic export. Preview/redaction/validation rejects it and leaves originals local (`AC-ADOPT-02`, `AC-ADOPT-03`). |
| `FM-ADOPT-03` | Disable diagnostic collection or omit symbols. Report the unavailable item and manual recovery; empty output is never success (`AC-ADOPT-02`, `AC-ADOPT-03`). |
| `FM-ADOPT-04` | Add a known `S0`/`S1` to a closeout fixture or an invented response deadline to release docs. Review blocks; preserve the record and correct/revalidate or withdraw affected bytes (`AC-ADOPT-04`). |
| `FM-ADOPT-05` | Present a replacement for the wrong hash/version or interrupt it. Reject before mutation; preserve original bytes/user state and existing published versions (`AC-ADOPT-05`). |

| Check | Coverage | Smallest route and oracle |
| --- | --- | --- |
| `CHK-ADOPT-01` | `AC-ADOPT-01`; `FM-ADOPT-01` | Non-author packaged-product and source journeys through success and a missing prerequisite. Private repair, dead onboarding or misleading support claims fail. |
| `CHK-ADOPT-02` | `AC-ADOPT-02`, `AC-ADOPT-03`; `FM-ADOPT-02`, `FM-ADOPT-03` | Isolated local startup/crash/hang/GPU diagnostics, with private/mismatched/oversized/unavailable fixtures. Inspect identity, symbols, redaction, bounds and absence of network uploads. |
| `CHK-ADOPT-03` | `AC-ADOPT-04`; `FM-ADOPT-04` | Review exact candidate known-defect ledger and public policy, then inject an unresolved critical defect and a false support promise. Both must block closeout; no live inbox submission is required. |
| `CHK-ADOPT-04` | `AC-ADOPT-05`; `FM-ADOPT-05` | Apply/interrupt a versioned replacement on copies with representative user state; verify applicability, atomicity, state policy, rollback/withdrawal and unchanged public bytes. |

Candidate results remain in `FCR-PROD-06`. No onboarding, local diagnostic, replacement or closeout check ran in this documentation amendment. Missing intake channels alone are no longer a blocker.
