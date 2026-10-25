# Difficult Graphics Incident Evidence

[Docs](../../../../../README.md) / [Up](../WorkloadStudiesDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected incident workflow; no new incident/root cause is diagnosed here

**Outcome:** a specialist can reproduce one difficult graphics failure, distinguish its cause from alternatives and verify the minimal owning repair. Targets PGE-01/05/06/07/09/10/13, PGD-04, CASE-02, AC-WS-03/CHK-WS-03. Capture/crash tools retain their existing feature acceptance; a qualified tool is not an incident diagnosis.

## Current Inputs And Ownership

The current Reference recovery plan retains dated Lit/Reference failures; external capture discovery and qualified GPU-crash tooling retain other original candidates. These are possible inputs, not proven current causes. Choose one actual sufficiently evidenced incident, including an already repaired release failure when the original bad/good artifacts and counterfactual remain available. Never reintroduce a production defect to create portfolio evidence.

The failing producer owns the repair. Application, Renderer, ShaderSystem, FrameGraph or RHI may own different hypotheses; native tool policy remains with the existing provider/runbook. No incident coordinator, public fault API, new collector or support service is selected.

## Incident Record And Semantics

| Record | Required content |
| --- | --- |
| Symptom and population | Exact bad candidate/API/profile/scene/input, observed expected-versus-actual product, reproducibility rate and first known good boundary. A responsive process or plausible screenshot is insufficient. |
| Identity chain | Requested/active output -> source/options/cooked inputs -> executed pass/native shader/resource and symbols; preserve capture and frame/request identity. |
| Competing hypotheses | At least two plausible mechanisms with discriminating observations, ordered experiments, rejected explanations and remaining uncertainty. Application/API/compiler/driver/hardware claims need separating evidence. |
| Root cause and bounded repair | Earliest wrong product, causal mechanism, smallest owning delta and deleted state/path. A workaround or correlation is labeled accordingly. |
| Counterfactual and result | The original bad candidate or safe disposable negative reproduces the wrong result; the fixed candidate passes the same oracle plus preservation/failure checks on required APIs. No permanent fault injection. |

Use the [workload-owned artifact home](../../../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) with package ID `WS-INCIDENT` for the hypothesis log, reduced input/commands, original bad/good hashes, captures and conclusion. Keep sensitive dumps/symbols private and link rather than copy originals. Existing CASE/WL/FCR reports own verdicts.

## Checks, Risks And Stop Conditions

`CHK-WS-03` rejects an unproved root cause, mixed symbols/candidate, missing bad sample, inability to distinguish two explanations or a fix that cannot be falsified. `CHK-WS-04` obtains independent replay/review at M6 / parent WS-S4 through the portfolio exercise; one participant can cover the case without an adopter per study. The local causal/replay packet hands off at M3 without pretending that later adoption already passed. Keep assistance and unavailable cells explicit.

Main risk (incident/evidence owners): an easy-looking symptom becomes a speculative driver diagnosis or broad rewrite. Freeze the discriminating check first; preserve the original failure, shrink to the first wrong boundary, and return an unproved claim to investigation. If rights/tool access prevent sharing, prepare a cleared reduced reproduction and mark the stronger claim blocked. Existing release repairs may proceed under their own gate; new production tooling still waits for REL-11.
