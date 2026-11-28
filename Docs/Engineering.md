# Developer guide

[Goals](Goals.md) | [Code style](CodeStyle.md) | [Roadmap](Roadmap.md) | [Implementation plans](Implementation/PortfolioReduction/README.md)

Durable engineering principles for a small, understandable graphics portfolio. Module plans own current commands, environment requirements, implementation choices and qualification records.

## Build and run

Keep one documented maintainer workflow with reproducible dependencies, efficient incremental builds and useful native debugging. Build, cook and source acquisition serve the maintainer; reviewers run prepared products without development tools.

Keep workspace, generated products and authored inputs distinct. Packaging and runtime discovery follow one explicit layout and dependency authority. Exact setup commands and supported tool versions belong with the workflow that uses them.

## Making a change

Understand the affected owner, callers, data flow and existing equivalents before editing. Preserve unrelated work. State the intended result, constraints and useful validation before implementation; work in reviewable batches.

Read complete changed functions and their neighbors, repair callers and build membership, and remove replaced code as the change lands. Record what was checked, the observed result and remaining limits. Reuse earlier evidence only while its relevant inputs still match.

## Module boundaries

A system owns its algorithms, mutable state, validation, cancellation and lifetime. Callers use narrow contracts; composition connects concrete owners. Orchestration expresses intent at one useful abstraction level, while cohesive operations own implementation mechanics.

Put cross-module contracts in Public and implementation in Private. New public APIs need two real consumers or one necessary host/module boundary; existing foundation contracts are judged by purpose and semantics. Dependencies follow actual use, not convenient transitive includes.

Core is product-independent. GameFramework does not depend on Renderer or RHI; RHI does not depend on Renderer. Native types remain within a backend or an explicit interop boundary. Keep platform, world, frontend, rendering and graphics-API responsibilities distinct.

Prefer direct code over speculative extensibility. New caches, queues, layers, modes and APIs need a current consumer and a real invariant or lifetime. Avoid god classes, forwarding-only collaborators, generic state bags and duplicate authorities.

After a scope cut, reshape surviving contracts and control flow around the remaining requirements. Retire unused callers, state, dependencies and products together. Measure the complete replacement, including moved code and adapters; relocation alone is not simplification.

Reuse supported library mechanisms where appropriate. Own integration, source identity, failure handling and resource lifetime; external examples do not override local contracts or demonstrate a local performance improvement.

### Execution ownership

Preserve distinct game and render threads, explicit immutable handoff and bounded frame lead. Keep dependency-correct parallel world, loading, cooking, compilation and recording work. Removing multithreading or serializing independent work is prohibited as a simplification strategy.

Respect UI/native thread affinity, game-owned mutation, per-worker recording ownership and queue-specific submission. Use explicit dependencies and fence-governed retirement across GPU queues. Composition supplies execution resources; domain owners control admission, cancellation and settlement.

Published results and cancellation are not physical completion. Keep inputs, callbacks and reservations alive until all readers settle; do not invoke reentrant callbacks under locks they can reacquire. Shutdown must settle owned work and required native diagnostics safely within declared bounds.

### File placement

Group implementation by responsibility, shared invariant, lifetime or backend. Public exposes contracts; Private contains mechanisms. Keep dependency recipes and required patches with their library, and native integration with its consumer.

Create files and folders for independent responsibilities, not file counts. Small cohesive surfaces may stay together. Avoid generic helper directories, forwarding namespaces and fragmentation that hides a shared invariant. Names should let a reader find the actual owner.

## Data and lifetime

Borrow or move before copying. Extra storage needs an authority, lifetime and invalidation rule. Choose data-oriented layout from access patterns and size; define CPU/GPU representations, bounds and arithmetic explicitly.

Prefer values and sole ownership; use shared ownership only for a real shared lifetime. Views and callback captures do not extend their inputs' lifetime. CPU settlement and GPU completion are separate obligations.

Validate immutable input at admission and express guaranteed presence through construction or references. Check changing identity, capacity, thread ownership and retirement at consumption. Report useful errors at their owner instead of duplicating checks and logs.

Publish complete immutable results. Failed edits and saves preserve accepted authored output; incomplete generated output is rejected and regenerated. Scene replacement has its declared failure policy and safe partial-construction cleanup, without implying a rollback guarantee. Navigation and explicit authoring edits remain distinct.

### Compatibility

There is no supported internal legacy format or public plugin ABI. Change owned representations in place, update all producers and consumers, remove replaced paths and regenerate disposable output. Do not add internal schema/ABI versions, migrations, aliases, disabled legacy modes or parallel implementations.

Required external versions, provenance and stale-handle/lifetime counters are different contracts. Persisted user data or installed external consumers require an explicit product decision.

## Verification

Review behavior, ownership and function responsibility as well as style. Start with the smallest check that can falsify the claim; broaden to native execution, image, performance and package checks when the changed contract requires them. Compilation is not runtime or release proof.

Exercise relevant failure, cancellation, bounds, concurrency, publication and delayed-retirement paths through actual consumers. Shared-library changes need a real consumer; source moves need correct build membership. Keep executable checks with their invariant, not in a separate source-text policy framework.

Bind observations to relevant source, inputs, configuration and environment. Record the method, expected result or tolerance, observation and exclusions. An unobserved gate stays open; later changed inputs require affected checks again.

Permanent test-only additions require explicit project approval. Remove temporary probes before submission and preserve existing tests unless their retirement is agreed. Keep raw evidence outside active guides.

### Script ownership

Every script needs an actual workflow, caller and useful result. Prefer supported build/compiler/formatter tools and retain project-specific rules where needed. Entry points validate inputs and compose subject-owned operations; supervision, rollback and cleanup stay with the owner.

Update callers when moving a script and remove unused workflows with their exclusive helpers. Check the affected success and failure paths.

### Performance and images

Fix workload, settings, reference domain, uncertainty and acceptance bounds before measuring. Separate correctness, image quality and cost; use analytic or independent comparisons with stated limits. A second internal renderer is not a validation requirement.

Report stable per-run distributions, memory and overhead. Preserve useful parallelism and compare equivalent quality and work. Account for correlated frames and equal sampling; inconclusive results stay inconclusive.

Separate instrumented captures from optimized timing. Distinguish CPU time, GPU intervals and presentation latency, and distinguish logical bytes, allocator storage, device budgets and memory high-water.

## Build and delivery

Deliver a complete offline inspection bundle with matching source and prepared content. Reviewers can launch the selected products, explore supplied scenes and controls, and return to defaults without acquiring source, compiling or cooking.

Validate the actual final package: dependency closure, relocation, standard-user/offline use, writable paths, failures, supported products and promised scenes. Verify maintainer preparation separately. Changed source, shaders, SDKs or packaged bytes invalidate affected qualification.

Acquisition is not redistribution permission. Review source/content provenance, licenses, notices, required source companions, signing and downloaded identity for the actual shipped files. Review diagnostic data and symbols before sharing. Do not claim public-release readiness from an internal archive.

## Writing documentation

Keep the five guides durable and high-level: Goals owns purpose and product boundaries; Engineering owns engineering principles; CodeStyle owns coding conventions; References owns source material and lessons; Roadmap owns outcome priorities and product limits.

Current commands, installed versions, source/API inventories, module-specific designs, stage prompts, checklists, bugs and qualification receipts belong with the relevant implementation or study. Link to the owner rather than copying the detail into a guide. A completed stage should not add a progress paragraph to these five documents.

Add a rule only for a recurring need that survives the current implementation. Search the existing owner, remove repetition and update the guide only when a lasting principle or requirement changes. Preserve useful numerical requirements and dated source evidence without embedding their execution history in the guides.

Prune verified completed work from active plans, keep only unmet actions for partial work and preserve evidence separately. Keep stable stage/decision numbers and repair links; remove an exhausted plan while retaining durable requirements with their owner. Check documentation links, text integrity and whitespace after edits.
