# Change Lifecycle

[Docs](../../README.md) / [Up](README.md)

**Status:** binding workflow standard

**Applies to:** every material owned repository change; scale the record to risk and scope

**This document owns the lifecycle of a change:** preparation, implementation discipline, reconciliation, acceptance, and completion reporting. Architecture, coding, domain, and validation rules remain in their subject standards.

## Read This Page

- **Prepare the change:** [Before editing](#before-editing): create the control record and inspect owners.
- **Implement one slice:** [During implementation](#during-implementation), including the [source-style gate](#stage-source-style-gate).
- **Close or resume a stage:** [Stage completion and resume](#stage-completion-and-resume); use the existing result rather than restarting its instructions.
- **Review and hand off:** [Review and acceptance](#review-and-acceptance), [completion report](#completion-report) and [stop conditions](#stop-conditions).

## Before Editing

### Create The Iteration Control Record

Every material implementation, documentation, release, or evidence iteration MUST have one small control record. Keep it in the existing issue, change description, applicable `FCR-*` report, or owning plan; do not create a permanent document merely to satisfy the format. Use `N/A` only with a reason.

| Field | Required content |
| --- | --- |
| Iteration identity | Stable `ITER-<scope>-NN` identifier, owner, status, exact scope, start revision/dirty state, and intended decision. |
| North Star | Applicable [`NS-*` outcomes](../../Strategy/EngineerPersona.md#north-star) and one sentence describing the user/engine improvement. |
| Persona targets | Applicable `PGE-*` IDs with `advance`, `preserve`, `not applicable`, or `blocked`; never claim an evidence-level increase without the Requirements proof. |
| Delivery target | Applicable roadmap `REL-*`, plan phase/item, capability row/evidence item, workload `MAP-*`/`CASE-*`, and `FCR-*` report. |
| Risks | `RISK-<scope>-NN` entries with cause, event, consequence, likelihood rationale, impact, prevention, detection trigger, contingency/fallback, owner, and retirement evidence. |
| Acceptance criteria | Binary `AC-<scope>-NN` statements naming actor/input, precondition, production route, observable result, matrix, threshold/tolerance, and evidence required. |
| Failure modes | `FM-<scope>-NN` entries naming cause or injected fault, detection boundary, safe state, user-visible behavior, recovery/cleanup, severity, and affected criteria/risks. |
| Checks | `CHK-<scope>-NN` mappings from each applicable `AC-*` and `FM-*` to the cheapest claim-falsifying build, static check, runtime route, capture, benchmark, fault exercise, or temporary harness. |
| Decision | `PASS`, `BLOCKED`, `EXCLUDED`, or `SUPERSEDED`, with exact evidence links, open defects/waivers, invalidation triggers, and next permitted step. |

The identifier scopes are local and descriptive, for example `AC-REN07-01` or `FM-REL-03-02`; they do not create a second global registry. Existing `REL-*`, `MAP-*`, `CASE-*`, `FCR-*`, and capability-evidence IDs retain their owning definitions and are linked rather than renumbered.

An acceptance criterion is valid only when a reviewer can decide pass or fail without interpreting intent after seeing the result. A risk is valid only when its trigger and retirement evidence are observable. A check is valid only when it names the claim, initial state, action or injection, oracle, matrix, artifacts, cleanup, and escalation condition. Every acceptance criterion needs at least one mapped check; every applicable failure mode needs a controlled negative check; every material risk needs prevention, detection, contingency, an owner, and retirement evidence. A broad test name, "works," "looks correct," average FPS, source presence, or a risk score without rationale is not a control record.

The [validation standard](../Verification/ValidationAndEvidence.md#check-and-test-design-contract) owns check design and test-code authorization. The [first-release acceptance contract](../../Acceptance/FirstRelease.md#traceability-and-acceptance-identity) and [feature completion reports](../../Acceptance/FeatureCompletionReports.md#iteration-traceability-and-coverage) specialize this record for release work.

### Select Applicable Authority

Use the [Engineering task map](../README.md#choose-by-task) to record:

- the integration contract and each touched subject standard;
- applicable [`PGE-01` through `PGE-15` requirements](../../Strategy/Requirements.md) only when the change affects or claims them;
- applicable [acceptance-workload gates](../../Acceptance/GraphicsWorkloads.md) only when the change affects their proof surface;
- expected evidence and unavailable checks.

Classify each conditional gate as **advance**, **preserve**, **not applicable**, or **blocked**. “Not applicable” needs a short reason.

### Define the Outcome and Complexity Budget

At the start of each implementation stage, choose evidence-only work, a repair in the existing owner, a bounded simplification/retirement, or an admitted missing capability. A simplification names the selected path it preserves and the unused concepts, misleading surface or ownership hops it removes; it need not invent a functional defect or a replacement feature. Reuse working code and valid proof before choosing a production addition. Apply the [complexity budget](../Foundations/ModuleOwnership.md#complexity-budget), [integration-hook budget](../Foundations/ModuleOwnership.md#feature-enclosure-and-integration-hook-budget) and [copy budget](../Foundations/DataAndMemory.md#single-truth-and-copy-budget) to the actual stage, including changes to existing code. In its existing control record, state:

- the user or engine outcome and authoritative production path;
- the current owner that will absorb the change;
- the essential state, branch, type, dependency, configuration, or public API being added, its current consumer and why the existing representation or direct code is insufficient;
- each material copy or new value holder and the boundary reason that prevents using a reference, view, handle, or move;
- the disposition of the touched existing path: retain for a named selected result, narrow to that result, replace and delete, or retire through the scope owner; include repeated policy, unused selectors, dead surface and unnecessary work;
- the owned artifacts that will be regenerated and the internal versions, migrations, adapters, aliases, fallbacks, or dual paths that will be deleted rather than preserved;
- the performance classification and budgets that may move;
- explicit non-goals that bound cleanup and prevent a second architecture.

This is a reasoning ledger, not a target for negative line count. A change may add substantial implementation while remaining structurally reductive. If permanent complexity has no current consumer, owner, invariant, or evidence-backed need, do not add it.

The stage exit compares that budget with the actual diff: every persistent addition has its accepted consumer and owner, every replaced path and its reachable residue is gone, and retained selected behavior has a positive check. Unjustified state, hooks, variants or dependencies and deferred same-slice cleanup keep the stage open even if the output works. Record only material deltas; a short reason suffices when no addition or deletion is justified. A later hardening/closure stage verifies earlier cleanup rather than collecting it as unfinished work. Documentation-only stages freeze decisions without landing dormant production vocabulary; evidence-only stages reuse current tooling and remove disposable validation code before handoff.

### Scope And Retirement Decision

Before spending implementation effort, apply [Roadmap scope protection](../../Strategy/Roadmap.md#scope-protection). Record the admitted output, current consumer, smallest necessary delta, explicit non-delivery, and cleanup in the existing iteration record. If no selected promise depends on a capability, evaluate **reuse**, **narrow**, **defer**, or **retire** before choosing to complete it. This decision applies to existing engine code as well as proposed additions.

For retirement, trace the current producers, consumers, serialized/cooked contracts, public selectors, build/package membership, external dependencies and evidence users. Name what remains and prove that it still produces the selected result. An unneeded private helper or replaced path is removed within the authorized slice. Removing an advertised or required capability first changes its owning scope/classification and dependent acceptance claims; a `Removed` result requires the implementation and reachable residue to be gone. A documentation decision alone is not removal evidence.

Land the corresponding source, shader, settings/UI, registration, build/dependency, generated-output and documentation cleanup together. Reuse the smallest positive preservation check and a negative reachability/stale-reference check. Preserve unique notices, provenance and valid evidence. Do not retain a disabled framework, compatibility path, placeholder menu or unused API for possible future work. If no deletion is justified, say why; do not invent churn to satisfy a subtraction quota.

### Reconcile Current State

Before relying on an earlier prompt, completion record, test, or criterion:

1. inspect the current owner, data path, consumers, lifetime, and replacement history;
2. classify the criterion as an enduring invariant, a still-relevant transition, or a superseded intermediate requirement;
3. prove enduring and still-relevant requirements against current code and evidence;
4. retain historical wording only as source trace, rationale, or regression context;
5. preserve the underlying correctness, ownership, lifetime, determinism, parity, and evidence invariant without restoring obsolete architecture.

### Find the Existing Responsibility

Search the complete owned repository with `rg` and `rg --files` before adding or renaming a concept. Search exact and rejected names, semantic counterparts, producers/consumers, services, queues, caches, packets, handles, build membership, tests, profiler labels, and thread labels.

For each counterpart decide **use**, **extend**, **refactor**, **replace and delete**, or **add because no owner exists**. Do not escape integration with `New*`, `*2`, vague concurrency suffixes, compatibility namespaces, or parallel directory trees.

### Map the Ownership Path

For each touched or proposed file record:

- target, module, subsystem, and `Public`/`Private` placement;
- primary type/operation and reason to change;
- each changed capability/invariant's authoritative definition and every material use classified as authority, composition, producer, consumer, or duplicate;
- mutable-state and lifetime owners;
- direct producers and consumers;
- final location and nearby instances of the same ownership defect.

Trace definitions to uses and changed uses back to their proper definition using the [definition and usage placement audit](../Foundations/ModuleOwnership.md#definition-and-usage-placement). Inspect the complete direct ownership path, not only prompt-named files. This is bounded reconciliation, not permission for unrelated cleanup.

### Reconcile the Touched Neighborhood

Review the changed unit, its direct owner and consumers, its folder boundary, and the build/test/documentation surfaces that express the same responsibility. The change MUST:

- remove superseded code, names, includes, flags, configuration, tests, comments, diagnostics, and documentation;
- correct the same local ownership or vocabulary defect where leaving it would create two conventions or preserve duplicate authority in the touched path;
- keep adjacent code consistent with the new invariant when that repair is small, low-risk, and validated with the change;
- stop or split an explicit prerequisite when the required reconciliation is too large to review safely.

Do not turn neighborhood cleanup into repository-wide reformatting, speculative decomposition, or unrelated debt collection. The boundary is coherence of the delivered ownership path.

### Keep Small Change Ledgers

Record preservation and deletion for every material change. Add the [data/access inventory](../Foundations/DataAndMemory.md#dataaccess-inventory) or a concurrency/hazard ledger only when those subjects are touched. Keep these in the change description or an existing implementation record, not in a permanent runtime reporting system.

Before parallelism or pipelining, define ownership, input/output identity, deterministic ordering, serial reference behavior, exclusive writable ranges, serial crossover, and failure/publication policy. Partial publication and ambiguous ownership are not policies.

### Plan the Fast Feedback Loop

Implement or generate one coherent owner/invariant-sized batch at a time. Before each batch, identify the files and production edge expected to change plus the cheapest check that could expose a mistake. Then:

1. locate the current owner and nearby accepted precedent with exact searches;
2. make the bounded implementation or generation pass;
3. inspect the scoped diff immediately for unintended files, duplicated policy or data, unjustified holders/copies, stale names, generated boilerplate, internal versioning/compatibility machinery, and ownership drift;
4. run cheap applicable formatting, static, schema, documentation, or architecture checks;
5. compile the smallest owning target only when the batch needs compilation evidence;
6. escalate runtime or broad validation only under the [claim-driven validation rules](../Verification/ValidationAndEvidence.md#claim-driven-validation-selection).

Do not accumulate a large AI-generated diff and use a full engine, game, editor, or workspace build as the first feedback signal. Broad builds, whole validation sets, all-content cooks, clean rebuilds, multi-backend runs, and acceptance workloads are final evidence only when the affected contract makes that breadth applicable. Uncertainty is resolved by inspecting the owner and choosing a more discriminating check, not by launching unrelated work.

## Stage Completion And Resume

This contract applies to every implementation plan and copied stage prompt, including older prompts. Keep the decision in the existing iteration record or owning candidate report; a separate tracker is unnecessary.

Before work, freeze one finite stage exit: selected output/consumer, exact prerequisites and input identity, applicable AC/FM/CHK cells, observable oracles and thresholds, required cleanup/style checks, result owner and next permitted step. Link existing definitions instead of copying their matrices. An unresolved value is a named discovery decision, not an invitation to implement until the result seems sufficient. A discovery stage closes when its required decisions and check cards are accepted; downstream production or release proof belongs to its consuming stage.

On every resume:

1. Read the last result and inspect current owners/inputs. Classify each required cell as already passed with applicable evidence, invalidated by a named change, or still missing/failed/unavailable. Reuse applicable passes. A new date, session or unrelated revision is not evidence of invalidation; changed binaries are not qualified by relabelling older results.
2. Execute the next unmet check or its bounded owning repair. If a stage exceeds a reviewable slice, name its finite child items and exits before further implementation; close each satisfied item once. The parent waits for the union of its required results. A completed delegated stage does not reopen merely because its parent, another provider or final release acceptance is still blocked.
3. After the final change, evaluate the frozen exit and applicable source-style/documentation checks. Record `PASS` immediately when every required cell passes or has an explicit scope-owner exclusion. Record `BLOCKED` when required proof failed, was not run or is unavailable; state the exact cell, observation, dependency/owner and next discriminating action. A partial pass is not a stage pass. `EXCLUDED` and `SUPERSEDED` retain their scope and reason in the owning report.
4. Reconcile the plan in the same handoff: remove delivered instructions, retain only unfinished repairs/proof, and point to the exact next open item and its prerequisite. Report which item closed and where the evidence lives. Do not end with only "continue this stage." A blocked handoff identifies the input, repair or external change needed before retry; repeating the same broad run without such a change is not progress.

Keep local stage completion distinct from feature/product/release acceptance. A source or provider slice can close its own gate while the aggregate remains open; an aggregate closes only after every applicable required result is present for its declared candidate and scope. Never weaken a required matrix, hide an unresolved defect in prose, or accept source presence in place of runtime proof to obtain closure.

Once the frozen exit passes, stop that stage. Additional optimization, architecture cleanup or adjacent features need their existing later owner or a separately admitted slice; they do not extend the completed stage. Necessary same-slice cleanup and defect repairs remain part of the exit. Reopen only the affected cells when concrete new evidence contradicts their accepted claim.

## During Implementation

- Keep high-level workflow visible as orchestration and cohesive mechanism in capability implementations.
- Keep one mutable authority and one production path.
- Apply the [single-truth and copy budget](../Foundations/DataAndMemory.md#single-truth-and-copy-budget): prefer references, views, handles, and moves; copy only at a named lifetime or publication boundary and resolve that snapshot once per epoch.
- Spend the declared complexity budget only on current behavior; remove scaffolding that becomes unnecessary as the vertical slice closes.
- Apply the [current clean-break policy](ChangeIntegration.md#current-clean-break-policy): update all producers and consumers, delete replaced paths and internal compatibility/versioning machinery in the same coherent change, and regenerate disposable local artifacts. Do not leave a later compatibility or deletion gate.
- Update implementation, headers, build membership, exports, removal of obsolete test registrations, and documentation atomically when ownership moves. Temporary validation tests are not submitted; follow [Validation, Performance, and Evidence](../Verification/ValidationAndEvidence.md#submitted-test-code).
- Re-check preservation and deletion when a new consumer appears.
- Keep generated and AI-assisted batches bounded, inspect their scoped diff immediately, and treat every changed line as untrusted until independently reviewed and proportionally validated.
- Stop when a product or architecture decision exceeds the task's authority.

### Finish With Responsibility Refinement

Every implementation stage MUST end with a deliberate source-shape pass before validation and handoff. Re-read the complete changed production path using the [responsibility audit](../Foundations/ModuleOwnership.md#responsibility-audit), not only the newest lines, and record one responsibility sentence for each substantive changed file, class, and function. Split a unit when that sentence joins independently changing policy, state, lifetime, orchestration, allocation, encoding, backend, or presentation responsibilities. Keep a cohesive state machine or algorithm together when a split would only create forwarding wrappers, one-method collaborators, numbered fragments, or navigation without knowledge removal.

The refinement pass MUST also remove dead scaffolding, duplicate state or policy, ceremonial helpers/namespaces, speculative abstractions, needless validation or diagnostics, and implementation details that leaked into an orchestrator or public contract. Reinspect includes, names, build membership, direct callers, dependency direction, and the feature integration-hook ledger after the final shape is known. A stage is not ready for handoff while a changed god function, god class, god file, diffuse feature mechanism, or unexplained abstraction remains, even when formatting, static checks, or a build succeed.

### Stage Source-Style Gate

Every implementation stage MUST pass this gate after responsibility refinement and before submission, handoff, or recording its exit as complete. This applies to future delivery plans and to stages run from older prompts. [Code Style](../Foundations/CodeStyle.md) and executable formatter configuration own the rules; this gate owns their required place in delivery.

1. Identify every added or modified owned source file in the stage, including headers and shaders. Reconcile the selected formatter paths with this list; an empty or partial selection cannot prove stage coverage. Deleted files need no formatting check. Record other exclusions with their owning policy and reason.
2. Apply the repository formatter only where needed in the owned scope, then run `CMake\CodeStyle.bat -Mode Check -Path "<repo-relative-file>"` for each selected file, or `CMake\CodeStyle.bat -Mode Check -Staged` when its selection covers the complete stage. Follow the [formatter procedure](../Foundations/CodeStyle.md#repository-commands). The full manifest and `-Path` cover new untracked owned source files; `-Staged` covers only staged paths. All selectors check working-tree contents. Use `-ManifestPath "<evidence-path>.json"` when retaining file-by-file selection evidence. Do not change the Git index merely to manufacture coverage or include unrelated work. For partial staging, also inspect the exact submitted diff; a working-tree check does not certify different index contents.
3. Review the complete changed functions, header groups, and shader blocks against the [Visual Spacing Contract](../Foundations/CodeStyle.md#visual-spacing-contract) and [Expressions And Wrapping](../Foundations/CodeStyle.md#expressions-and-wrapping). Verify purposeful paragraph boundaries, cohesive declarations/operations, attached comments, explicit control-flow braces, readable inputs, and assignment/call wrapping. A formatter pass cannot establish logical grouping or responsibility separation.
4. Run `git diff --check` and inspect the final scoped diff. Repeat affected style checks and review after any subsequent source or formatter-policy edit; retain evidence for the final submitted contents.
5. Record exact commands, tool version, checked file coverage, exclusions, mechanical check result, and authored layout review result in the existing stage control record or handoff. An applicable failure, unavailable required tool, or uncovered source file keeps this gate open. For a stage with no source changes, record source-style `N/A` with that reason and run the applicable documentation/build-configuration checks.

Keep the check proportional to the stage. Repository-wide formatting and broad builds are not prerequisites for a scoped style result. Future plan templates and ready-to-use implementation prompts MUST reference this gate; they must not defer it to final aggregate acceptance.

## Review and Acceptance

Review the final owned changelist at the end of **every implementation stage**, before handoff or advancement. Formatting alone is insufficient: resolve actionable review findings, repeat affected checks after repairs, and state any build/runtime checks deferred by the stage contract. Do not describe a source-only review as executable validation.

Use the [SparkleEngine Code Review](CodeReview.md) procedure and run the review section or acceptance rules in every selected subject standard; do not recreate those checklists in the change description. Then verify the integration itself:

- the requested outcome works through the authoritative production path;
- ownership, lifetime, publication, and failure behavior are explicit;
- moved, split, or deleted paths are reconciled with build membership, includes, exports, tests, and documentation;
- preservation and deletion records match the final implementation;
- old names and production references are gone where intended;
- no internal version, migration reader/writer, compatibility adapter, alias, fallback, old/new dispatch, or dual representation preserves the replaced Sparkle-owned contract;
- the final structure spends no unexplained complexity and leaves the touched ownership path easier to navigate;
- the [Stage Source-Style Gate](#stage-source-style-gate) covers the final changed source, with both mechanical and authored layout results;
- performance impact is classified and any material hot-path, memory, latency, loading, cooking, or build-time risk has proportional evidence;
- exact claim-to-check mappings, validation commands, configurations, backends, results, escalation, and applicable unavailable checks are recorded; unrelated broad checks are not reported as missing evidence;
- performance and AI-assisted claims meet [Validation, Performance, and Evidence](../Verification/ValidationAndEvidence.md);
- applicable `PGE-*` and workload gates are linked and classified without copying their contracts.

## Completion Report

Report only applicable sections:

1. iteration ID, applicable `NS-*`, `PGE-*`, roadmap/plan/workload, capability, and `FCR-*` links;
2. outcome and user-visible behavior;
3. current-state reconciliation;
   reconcile the owning plan under [Documentation Organization](DocumentationOrganization.md#lifecycle): remove completed delivery instructions, retain unfinished evidence and repairs, and route enduring decisions and necessary results to their owners;
4. repository search and use/extend/refactor/replace/add decisions;
5. ownership, lifetime, publication, and failure contract;
6. files grouped by responsibility;
7. complexity budget, preservation, deletion, and touched-neighborhood cleanup;
8. structure/data/concurrency reconciliation;
9. risk disposition and remaining exposure;
10. `AC-*`/`FM-*` to `CHK-*` coverage with exact validation and performance evidence;
11. performance classification and budgets;
12. naming and stale-reference audit;
13. limitations and unavailable evidence;
14. evidence-level movement or preservation rationale;
15. `PASS`, `BLOCKED`, `EXCLUDED`, or `SUPERSEDED` decision.

## Stop Conditions

Stop for a product or architecture decision when a prerequisite has not passed; required behavior cannot survive the owner/lifetime model; the repository contradicts an assumption; a public abstraction has no real consumer; permanent machinery exceeds the stated complexity budget without an essential current invariant; exclusive outputs or deterministic order cannot be proven; serial parity differs without explanation; material performance exposure has no proportional evidence; native validation reports ownership/state errors; scope materially exceeds authorization; or the only justification is future use, familiarity, framework elegance, or unmeasured performance.

Do not hide a stop condition behind compatibility code, a broad mutex, retries, logging, or a temporary second architecture. Rejected technical patterns remain in the subject standard that owns their reason.
