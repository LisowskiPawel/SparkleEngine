# SparkleEngine Documentation

**Status:** documentation entry point and authority map

Use these pages to understand the engine, choose your next task and find the design or proof behind a feature.

## Start Here

| I want to... | Open |
| --- | --- |
| Continue the first release | [Next task](Strategy/Roadmap.md#where-we-are-and-what-is-next) -> [PD-3: installed failures and removal](Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#pd-3--package-and-installation) -> [shipping checks](Acceptance/FirstRelease.md) |
| Decide what to deepen, simplify or retire | [Product direction](Strategy/ExecutiveSummary.md#product-identity) -> [portfolio focus and existing-code cuts](Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification) -> [scope protection](Strategy/Roadmap.md#scope-protection) |
| Understand the engine | [Engine at a glance](Architecture/EngineAtAGlance.md) -> [repository map](Architecture/WholeRepositoryMap.md) |
| Find a module or feature | [Architecture](Architecture/README.md) -> [modules](Architecture/Modules/ModuleInventory.md) |
| Work on rendering | [Renderer](Architecture/Modules/Engine/Renderer/RendererDesign.md) or [RHI](Architecture/Modules/Engine/RHI/RHIDesign.md) |
| Deliver the persona's evidence | [All fifteen targets to exact plans](Architecture/CrossModule/FeatureDocumentation/StableIdentifierCoverage.md#principal-graphics-engineer-requirements) -> [subject artifact homes](Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) |
| Build, cook or package | [Launcher](Architecture/Modules/Tools/Launcher/LauncherCapabilities.md) -> [build](Architecture/Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md) -> [package](Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md) |
| Contribute or review | [Contributing](../CONTRIBUTING.md) -> [engineering rules](Engineering/README.md#choose-by-task) |
| Find results or workload checks | [Acceptance](Acceptance/README.md) |

For renderer specialization, open the [depth-to-evidence map](Strategy/FeatureDeliveryCatalog.md#selected-rendering-depth) and [M5R order](Strategy/Roadmap.md#required-renderer-depth-wave): reference/direct/indirect, shading, denoising/upscaling, bounded volumes, SHaRC before NRC, NTC and PTLAS. This changes no current first-release next task.

## Design Or Implementation?

| File | Use it for |
| --- | --- |
| `README.md` | Navigation: choose the document to open. |
| `*Design.md`, `*Capabilities.md` or a named contract | What it does, why it exists, how it works, limits and checks. |
| `*ImplementationPlan.md` | Ordered stages, prerequisites, work, exit criteria and copyable prompts. |
| [Roadmap](Strategy/Roadmap.md) | Current task and global order; direct links to implementation stages. |
| Acceptance reports | What the exact candidate proved or still lacks. |

Each page has a **Docs / Up** route. Keep design open as reference while executing one linked implementation stage.

## How The Documentation Fits Together

The repository is organized by owning subject first. Strategy, Engineering, and release Acceptance remain distinct top-level authorities; architecture, plans, research, capability snapshots, and feature-local acceptance are colocated under the module or cross-module subject they describe.

```mermaid
flowchart LR
    Strategy[Strategy<br/>What should we achieve?] --> Subject[Architecture subject folder<br/>Who owns this knowledge?]
    Research[Colocated research<br/>What can we learn?] --> Subject
    Subject --> Plan[Colocated plan<br/>In what order will it change?]
    Plan --> Work[Code and build configuration<br/>What is implemented?]
    Engineering[Engineering<br/>How must work be done?] -. governs .-> Work
    Subject -. defines local proof .-> Acceptance[Acceptance<br/>What did this candidate prove?]
    Work --> Acceptance
```

Code and executable build configuration are the authority for implemented behavior. Documentation explains intent, design, limits, procedures, and required evidence; it does not turn source presence into a successful build or runtime result.

## Authority Boundaries

<details>
<summary>Compare documentation responsibilities</summary>

| Authority | Owns | Does not own |
| --- | --- | --- |
| [Strategy](Strategy/README.md) | desired capabilities, priorities, roadmap, operating model, dated assessments | implementation rules or system internals |
| [Architecture subject tree](Architecture/README.md) | module/feature ownership and navigation; current maps, design, decisions, capability snapshots, and feature-local proof contracts | release results or proof that source-present behavior ran |
| Colocated `*ImplementationPlan.md` | ordered delivery, dependencies, stop conditions, migration and validation sequence for its owning subject | enduring design decisions or completion claims |
| Colocated research document | external precedent, option studies, visual exploration, and dated baselines for its owning subject | local decisions, implementation state, or evidence grades |
| [Engineering](Engineering/README.md) | workflow, implementation standards, module rules, verification, engineering decisions | release scope, system design, or product research |
| [Acceptance](Acceptance/README.md) | shared completion language, candidate reports, workload/release gates, high-level progress | duplicate feature architecture or local feature criteria |

</details>

Colocation changes how readers find related knowledge; it does not merge authority. Each document's filename, status header, and responsibility still distinguish current state, target architecture, plan, research, and acceptance.

## Status Language

| Label | Meaning |
| --- | --- |
| `Implemented path` | Source and build membership contain the named route. It may still be unbuilt and untested. |
| `Partial` | A useful route exists, but important cases or product boundaries are missing. |
| `Capability-gated` | SDK, build option, API, device, vendor, content, or configuration controls availability. |
| `Not found` | The targeted source/build audit found no current implementation owner. |
| `Unproved` | The architecture is documented, but its required executable evidence has not been accepted. |
| `Blocked` | A named prerequisite prevents the next acceptance or delivery decision. |

The numeric readiness score is separate from these state labels. It adds implementation, integration, verification, and delivery components for orientation; it does not turn acceptance into an average. See the [scoring model](Acceptance/CurrentReadiness.md#scoring-model).
