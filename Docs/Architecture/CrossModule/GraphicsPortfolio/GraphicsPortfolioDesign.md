# Graphics Portfolio Evidence

[Docs](../../../README.md) / [Up](../README.md) / [Implementation](ImplementationPlan.md)

Implementation stages: [ImplementationPlan](ImplementationPlan.md).

Sections: [Graphics Portfolio Delivery Plan](ImplementationPlan.md#graphics-portfolio-delivery-plan) / [Graphics Knowledge Transfer](#graphics-knowledge-transfer) / [Upstream Graphics Contribution](#upstream-graphics-contribution).

**Status:** target dossier; case delivery and independent adoption remain unproved

**Scope:** three reproducible technical cases at M6, then observed direction, second-context transfer, teaching and feedback revision at M7; reuse the same rendering work

A reviewer should be able to see what you built, why it works, where it fails and how to reproduce it. This folder joins evidence already owned by the engine's feature and workload reports.

## The Three Cases

1. **Content to correct pixel:** follow an asset/material through cooking, shading and raw reference output.
2. **One measured frame, two APIs:** join complementary CPU preparation, GPU shader and resource-lifetime studies, the difficult incident and whole-frame results, including a rejected or inconclusive alternative.
3. **Owned model to shader:** connect dataset/training choices to correct GPU operators, runtime behavior and held-out evaluation.

The [review contract](ReviewAndTransfer.md) selects exact CASE joins, minimal package, mathematical/native proof and AC-PORT/CHK-PORT criteria. The [Roadmap](../../../Strategy/Roadmap.md#retained-advanced-graphics-roadmap) decides when to deliver them. Cases are selected; actual candidate/rights/participants and results remain execution inputs.

## Where To Start

| Your task | Open |
| --- | --- |
| Choose artifacts and assemble the cases | [Delivery plan](ImplementationPlan.md#graphics-portfolio-delivery-plan) |
| Prepare M6 reproduction and M7 direction/teaching/transfer | [Review criteria](ReviewAndTransfer.md#binary-review-criteria), [actual participant card](#graphics-knowledge-transfer) |
| Plan a justified upstream patch or technical submission after M6 | [Upstream contribution](#upstream-graphics-contribution) |

## Evidence To Reuse

| Source | What it contributes |
| --- | --- |
| [First Release](../../../Acceptance/FirstRelease.md) | Exact product/source, rights, package and adoption records |
| [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md) | Existing case identities and evaluation/adoption criteria |
| [Workload Studies](../PerformanceDiagnostics/WorkloadStudies/WorkloadStudiesDesign.md) | Original runs/captures, causal comparisons and incident evidence |
| [Neural Graphics](../NeuralGraphics/NeuralGraphicsDesign.md) | Dataset/model/operator/runtime joins and held-out results |
| [Persona](../../../Strategy/EngineerPersona.md), [Requirements](../../../Strategy/Requirements.md) | Personal ownership, explanation and principal judgment targets |

Link original immutable manifests and artifacts. Keep measurements and conclusions in their existing reports; this folder owns their reader-facing assembly and exercise. It adds no runtime feature, collector, viewer, report ID or data schema.

Before sharing, verify candidate identity, artifact validity, rights and privacy for the selected subset. Missing or mismatched evidence leaves that case unproved. Negative results remain useful when their question and limitations are clear.

## Graphics Knowledge Transfer

**Status:** selected M6/M7 evidence contract; actual participant and outcome fields remain unproved

**Scope:** help another person understand and reproduce one evidence-backed graphics case, then use their feedback

M6 requires first independent reproduction/review. M7 requires a meaningful expert challenge/direction record, second actual consumer context, observed before/after teaching task and one feedback-driven post-adoption revision/retest. [Review And Transfer](ReviewAndTransfer.md) defines the binary checks; [portfolio stage 4](ImplementationPlan.md#4-demonstrate-direction-and-transfer) owns execution. Continuing maintenance or additional adopters beyond that finite exercise remains conditional; no support service is selected.

The [Technology Transfer subject folder](TechnologyTransfer/TransferDesign.md) owns the actual participant/context, technical question, before/after task/rubric and revision/retest contract. Its [KT-0..3 implementation plan](TechnologyTransfer/ImplementationPlan.md) supplies prerequisites, bounded work, falsifiers, cleanup and copied prompts. Prepare the concrete exercise in KT-0; missing people, permission and outcomes remain actual evidence dependencies.

M6 reproduction remains in [portfolio stage 3](ImplementationPlan.md#3-reproduce-review-and-revise). Reuse valid specialist review and original CASE/WL evidence; no extra adopter per study or ongoing support service is selected. [Evidence folders](../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) separate original proof and the minimal reproduction/transfer package. Writing these plans sends no message or publication.

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
