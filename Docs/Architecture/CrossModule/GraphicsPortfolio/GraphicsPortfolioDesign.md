# Graphics Portfolio Evidence

[Docs](../../../README.md) / [Up](../README.md) / [Implementation](ImplementationPlan.md)

Implementation stages: [ImplementationPlan](ImplementationPlan.md).

Sections: [Graphics Portfolio Delivery Plan](ImplementationPlan.md#graphics-portfolio-delivery-plan) / [Graphics Knowledge Transfer](#graphics-knowledge-transfer) / [Upstream Graphics Contribution](#upstream-graphics-contribution).

**Status:** target dossier; case delivery and independent adoption remain unproved

**Scope:** turn existing product, rendering and neural evidence into three understandable, reproducible case studies

A reviewer should be able to see what you built, why it works, where it fails and how to reproduce it. This folder joins evidence already owned by the engine's feature and workload reports.

## The Three Cases

1. **Content to correct pixel:** follow an asset/material through cooking, shading and raw reference output.
2. **A measured rendering improvement:** explain the actual limiter, the experiment and its whole-frame result, including a rejected or inconclusive alternative.
3. **Owned model to shader:** connect dataset/training choices to correct GPU operators, runtime behavior and held-out evaluation.

The [delivery catalog](../../../Strategy/FeatureDeliveryCatalog.md#three-headline-cases) gives output examples. The [Roadmap](../../../Strategy/Roadmap.md#retained-advanced-graphics-roadmap) decides when to prepare and deliver them.

## Where To Start

| Your task | Open |
| --- | --- |
| Choose artifacts and assemble the cases | [Delivery plan](ImplementationPlan.md#graphics-portfolio-delivery-plan) |
| Prepare an exercise, first-use review or later teaching activity | [Knowledge transfer](#graphics-knowledge-transfer) |
| Plan a justified upstream patch or technical submission after M6 | [Upstream contribution](#upstream-graphics-contribution) |

## Evidence To Reuse

| Source | What it contributes |
| --- | --- |
| [First Release](../../../Acceptance/FirstRelease.md) | Exact product/source, rights, package and adoption records |
| [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md) | Existing case identities and evaluation/adoption criteria |
| [Workload Studies](../PerformanceDiagnostics/WorkloadStudies/README.md) | Original runs/captures, causal comparisons and incident evidence |
| [Neural Graphics](../NeuralGraphics/NeuralGraphicsDesign.md) | Dataset/model/operator/runtime joins and held-out results |
| [Persona](../../../Strategy/EngineerPersona.md), [Requirements](../../../Strategy/Requirements.md) | Personal ownership, explanation and principal judgment targets |

Link original immutable manifests and artifacts. Keep measurements and conclusions in their existing reports; this folder owns their reader-facing assembly and exercise. It adds no runtime feature, collector, viewer, report ID or data schema.

Before sharing, verify candidate identity, artifact validity, rights and privacy for the selected subset. Missing or mismatched evidence leaves that case unproved. Negative results remain useful when their question and limitations are clear.

## Graphics Knowledge Transfer

**Status:** fillable planning card; teaching, mentoring and adopter evidence remain unproved

**Scope:** help another person understand and reproduce one evidence-backed graphics case, then use their feedback

At M6, the first independent reproduction/review is required. Deeper teaching, a second adopter and sustained maintenance are later conditional work. The [Roadmap](../../../Strategy/Roadmap.md#conditional-and-excluded-work) owns that priority; the [portfolio plan](ImplementationPlan.md#graphics-portfolio-delivery-plan) owns assembly.

### Decisions To Fill

| Choose… | Record… |
| --- | --- |
| **Audience and question** | Author: actual consenting participant/audience and one selected case question. |
| **Evidence** | Evidence owner: original case/workload report, exact candidate/artifact and shareable subset. |
| **Exercise** | Author: entry point, commands/input, expected inspectable result and a bounded completion check. |
| **Learning and feedback** | Reviewer: an observable task/decision before and after the explanation, actual feedback and the resulting revision. |
| **Independent adoption** | Adopter: prior involvement, environment and every intervention. Distinguish an assisted exercise from an unaided acceptance run. |
| **Maintenance** | Support owner: one feedback-driven follow-up, supported scope, contact and retirement route. |

These decisions are still open. Fill them from one accepted case before arranging the exercise.

### Remaining Delivery

1. **Prepare:** choose the audience, question and cleared evidence; build only the minimal exercise.
2. **Deliver:** after consent/authorization, retain questions, interventions, failures and feedback.
3. **Improve:** record the resulting changes and reproduction/maintenance outcomes in the existing case/workload/support report, with clear attribution and limitations.

Planning does not authorize contacting people or publishing material. Actual participant feedback establishes transfer evidence.

## Upstream Graphics Contribution

**Status:** conditional planning card; no external issue, contribution or publication is selected

**Scope:** one upstream fix or technical submission based on an accepted graphics finding

Start only when a real finding has an affected external user and a useful recipient. This is conditional work after M6 under the [Roadmap](../../../Strategy/Roadmap.md#conditional-and-excluded-work); [Requirements](../../../Strategy/Requirements.md#non-repository-requirements) owns the influence target.

### Upstream Graphics Contribution: Decisions To Fill

| Choose… | Record… |
| --- | --- |
| **Problem and consumer** | Author: demonstrated local finding, original case evidence and the affected upstream user. |
| **Recipient** | Author: exact project/revision or publication venue and its current contribution policy. |
| **Bounded output** | Author: minimal reproducer/patch or one technical argument, exclusions and personal attribution. |
| **Rights and disclosure** | Provenance owner: license, CLA/DCO requirements, permitted source/capture sharing and responsible security handling. |
| **Validation** | Reviewer: reproduction, upstream validation/formatting rules, negative control and supported tuple. |
| **External outcome** | Author: actual review, merge, rejection or feedback, and the resulting revision. Submission alone does not establish adoption. |

All selections remain open. Refresh the recipient's primary policy/source when this work is admitted.

### Upstream Graphics Contribution: Remaining Delivery

1. **Select and freeze:** choose one evidenced issue, the bounded patch/article and its check card.
2. **Prepare:** produce the concrete reviewable result using existing production owners and upstream conventions.
3. **Submit and learn:** obtain explicit authorization before submission, then retain feedback and disposition in the existing case report. Remove disposable repro files from Sparkle's submitted tree.

Filling this card does not authorize an issue, PR, message or publication. An unavailable recipient or inconclusive issue keeps this activity deferred while valid local work remains useful.
