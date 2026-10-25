# Python Automation And Analysis

[Docs](../../../README.md) / [Up](README.md)

**Status:** negative/target capability dossier; narrow project conversion scripts exist, but no repository-wide Python engineering layer was found

**Scope:** distinguish the two project-local conversion scripts from absent reusable automation, analysis, bindings, editor scripting, and runtime Python capabilities

**Owner:** Tools for reusable automation; the producing module owns domain semantics and schemas

**Snapshot:** 2026-09-07; repository file inventory found exactly `ConvertLPSHead.py` and `ConvertCornellBox.py`, both under Showcase conversion assets; source evidence `S` only

**Strategy sources:** [`PGE-07`](../../../Strategy/Requirements.md), [Gap Assessment](../../../Strategy/Assessments/GapAssessment.md), and `NS-EVIDENCE`/`NS-SIMPLIFY` in the [Engineer Persona](../../../Strategy/EngineerPersona.md)

**Current readiness:** **10/100** for reusable Python automation — two project conversion scripts exist, but no repository automation, analysis product, binding, or editor/game Python layer exists. See [Current Feature Readiness](../../../Acceptance/CurrentReadiness.md#explicit-missing-or-not-yet-admitted-capabilities).

## At A Glance

| Capability | Current state | Correct interpretation |
| --- | --- | --- |
| LPS Head conversion | one Showcase-owned script exists | a project content conversion, not reusable engine automation |
| Cornell Box conversion | one Showcase-owned script exists | a project content conversion, not an analysis framework |
| repository automation/result analysis | Not found | future tools need one named workflow, schema, owner, and consumer |
| editor/game Python or public bindings | Not found and not promised | separate product capabilities, not implied by offline scripts |

Choose Python for one actual offline workflow. Runtime-asset preparation must satisfy its existing native ingestion contract; a human-facing study analyzer instead needs correct original-input joins, independent recomputation and bounded publication. Neither requires a general Python layer or an otherwise unused C++ report reader.

## Capability Identity

| ID | Capability | Current state |
| --- | --- | --- |
| `PY-01` | Showcase-owned LPS Head source conversion | Implemented script path; unproved |
| `PY-02` | Showcase-owned Cornell Box source conversion | Implemented script path; unproved |
| `PY-03` | Reusable repository automation or result-analysis tool | Not found |
| `PY-04` | Public Python bindings, editor scripting, or runtime embedding | Not found and not currently promised |

## Current Boundary

The current tree contains two Showcase-oriented source-conversion scripts. They demonstrate bounded offline scripting, not a supported Python API, test harness, workload-analysis package, build orchestrator, bindings layer, or reusable asset/graphics automation product. No such broader capability should be inferred.

## Target Contract

The selected `PY-03` consumer is the [Graphics Workload Studies plan](../../CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md): one offline analysis CLI over the existing workload schema, with per-run comparisons/plots and identity/partial-input negatives. This is target work after its discovery/release admission, not an implemented Python layer or runtime binding promise.

A future tool names its actual consumer: an existing native asset/tool boundary or the engineer using the study output. Freeze interpreter/dependencies, current input/output contract and hashes, path policy, resource bounds, exits and cancellation/publication. Native ingestion validates bytes when it exists. For standalone analysis, frozen-input repeat, independent reanalysis and explicit schema/identity negatives validate the derivative output; do not add a native consumer solely to satisfy a template. Domain meaning remains with the workload/asset owner, with one current representation and no internal version dispatch.

The selected delivery is the workload analyzer. Conversion/training preparation stays inside its existing feature owner and does not select a reusable build orchestrator, automation framework or additional Python product. Runtime embedding, editor scripting and public bindings remain unselected; documenting their absence does not schedule them.

## Acceptance Handoff

- `AC-PY-01`: one selected workflow runs from clean documented prerequisites with pinned external dependencies and deterministic current-contract/source/input identity; no internal schema-version compatibility path.
- `AC-PY-02`: valid, boundary, malformed, missing, oversized, interrupted, and repeated inputs produce defined outputs, exit status, and cleanup.
- `AC-PY-03`: the actual consumer validates the output: existing native ingestion for runtime assets, or independently recomputable original-input joins and schema/identity controls for human-facing analysis. No duplicate authority or artificial C++ reader is introduced.
- `AC-PY-04`: runtime, memory, input/output volume, and concurrency are bounded and attributable.
- `FM-PY-01`: interpreter/dependency/schema mismatch -> fail before output publication.
- `FM-PY-02`: cancellation or exception -> remove or quarantine partial output and return nonzero.
- `CHK-PY-01`: run twice from frozen inputs and compare manifests/hashes/results; inject malformed, missing, incompatible, and interrupted cases.

These criteria have no candidate results in this documentation pass. [`PY-E01`](../CapabilityEvidencePlan.md#world-content-and-tool-evidence) owns the smallest current-scope proof and negative-claim audit.
