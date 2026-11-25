# SparkleEngine portfolio ownership plan

Revised 25 November 2026 for Pawel Lisowski. Available time: **10–15 hours a week**. Career target: **NVIDIA Developer Technology, integrating rendering technology and contributing to its development**.

## The direction you have chosen

**SparkleEngine will be the main portfolio piece. Reduce its scope until you can personally design, understand and stand behind everything that remains.** Small GitHub experiment repositories come afterward, once this cleanup is complete.

This changes the earlier recommendation to make a separate application the primary project. Sparkle itself is now the deliverable. The open questions are how much engine it should contain and how to rebuild your ownership of it.

Your quality requirement applies to all retained first-party work: rendering, Launcher, scene loading, cooking, scripts, CMake, integration bindings and delivery. A support subsystem earns its place by helping the finished portfolio engine and being something you want to own.

## Recommendation

Choose **Option B, a compact rendering workbench**, and **rework it one responsibility at a time after cutting scope**. Keep Sparkle's identity, selected rendering capabilities and useful experience. Personally decide the surviving architecture, retain existing implementations only after understanding and accepting them, and rewrite the parts whose design you cannot defend.

A small Launcher can belong in that result. Give it a narrow purpose and the same engineering standard as the renderer. Remove broader product responsibilities when their maintenance and learning cost exceed their contribution to your selected engine.

For this plan, "handcrafted" means that the Sparkle-owned design and code are deliberately yours: you can explain them, justify the choices, modify them independently and demonstrate their behavior. Existing generated code receives no exemption. Third-party libraries and vendor algorithms remain explicitly credited dependencies; you must understand their contracts and the behavior relevant to Sparkle, while distinguishing their implementation from yours.

Translate "engineering masterpiece" into a finishable standard: clear responsibilities, direct data flow, checked mathematics, explicit lifetime, useful diagnostics, measured behavior and a reproducible product. Additional abstraction or polish needs an actual purpose. Every retained subsystem should meet that standard within its stated scope.

## Options for the finished Sparkle

All three options keep Sparkle as an engine portfolio piece. They differ in how much product surrounds the rendering engine. These are proposed scopes; no feature removal or backend change has been selected or implemented.

| Option | Finished experience | Main scope reduction | Ownership burden | Assessment |
| --- | --- | --- | --- | --- |
| **A Lean rendering engine** | A renderer library and one Showcase application with camera, a few rendering controls and reproducible captures | Separate Launcher, broad Editor, general gameplay/world machinery and broad content workflows | Smallest of these options; native graphics and shaders still require substantial work | Strong if finishing with deep rendering ownership is the priority |
| **B Compact rendering workbench** | Sparkle renderer, one Showcase application with inspection controls, and a small Launcher for the selected demonstration workflow | General authoring, separate broad Editor/Game feature sets, workspace management UI and unused content paths | Adds a bounded, visible example of product engineering to the renderer | **Recommended balance for your stated goal** |
| **C Reduced engine suite** | Distinct Launcher, Editor and Game, with a selected set of scene edits and content operations | Unused features within each product, while preserving three application lifecycles | Largest: editing, transactions, persistence, asynchronous operations and delivery all need ownership | Choose if these workflows are personally important enough to justify the extra months |

A rendering engine does not need general gameplay features to be a coherent engine. What makes the result convincing is how well the retained parts work together and how completely you understand them.

Option A removes the Launcher because of the chosen product boundary. Option B deliberately keeps one because a reliable, understandable entry point can strengthen the whole presentation. Option C is viable, but should be a conscious investment in tools and product engineering alongside graphics.

### Backend scope is a separate decision

Your current [goals](Goals.md#product-scope) retain both D3D12 and Vulkan. You can choose one final backend for greater concentration, or keep both to demonstrate API differences and resource/synchronization design. Two supported APIs increase the amount you must understand and qualify.

My recommendation is to establish ownership and correctness on D3D12 first, then assess the Vulkan cost before freezing the final scope. If Vulkan remains in the finished source, it must meet the same ownership standard. Calling an unfamiliar backend "deferred" would leave the central problem unresolved. A decision to ship only one API would require an explicit update to the goals and removal of the retired path.

## What the recommended scope would contain

The proposed workbench has a simple purpose: let another engineer run and inspect a small number of well-understood rendering configurations, while giving you a system you can explain from launch through the final pixel and shutdown.

| Area | Proposed treatment | Reason to retain it |
| --- | --- | --- |
| RHI and frame graph | Keep and narrow to actual retained requirements; personally review resource identity, states, submission, synchronization and retirement | Central evidence of graphics systems design |
| Rendering and shaders | Keep one coherent production frame, a bounded independent Reference path and selected existing integration work | Correct pixels, algorithm reasoning and realistic integration |
| Content | Keep one selected scene/material path, a small controlled scene and one representative scene | Understandable asset-to-pixel flow with useful regression workloads |
| Scene/world code | Keep the representation and motion the chosen demos need; retire general gameplay scheduling or animation features without a retained demonstration | Avoid maintaining a general game framework by default |
| Showcase controls | Keep camera, relevant settings, signal inspection and capture/profiling access | Makes the rendering understandable and debuggable |
| Launcher | Keep a small scene/configuration/run workflow with readiness, progress and failure reporting where needed | A polished entry point with an explainable execution lifecycle |
| Importing and cooking | Keep the required source-to-runtime path and reproducible commands; narrow supported formats and operations | A portfolio engine still needs a reliable content workflow |
| Core, build and delivery | Keep what the retained applications use, including necessary validation, diagnostics and dependency handling | The result must build, run and fail understandably |

This is a starting proposal, rather than a module-by-module deletion authorization. For example, controlled object motion can justify retained transform updates without justifying an entire skeletal-animation pipeline. A needed diagnostic can survive even if its broad panel framework disappears.

Do not add the whole future rendering roadmap during cleanup. Choose among existing capabilities first. An owned neural training pipeline, more SDK providers and broad rendering experiments can wait until the retained engine is understood and complete.

### How to treat the Launcher

For Option B, the initial proposed Launcher responsibility is: select an approved scene/configuration, report the prerequisites for running it, start the Showcase application, and report its outcome. Keep build and cook available through documented commands. Add Launcher buttons for those operations only if you explicitly choose to own their additional workflow.

The current Launcher covers considerably more. Its [build operations](../Tools/Launcher/SparkleLauncher/Public/SparkleLauncher/BuildWorkspaceOperations.h), [cook operations](../Tools/Launcher/SparkleLauncher/Public/SparkleLauncher/CookOperations.h) and [operation dispatch](../Tools/Launcher/SparkleLauncher/Private/Operations/LauncherOperationExecution.cpp) include workspace and tool preparation. Removing GUI actions alone would leave their underlying code and dependency obligations in place; audit remaining CLI and Editor callers before retiring anything.

For every retained action, you should be able to trace selection into a request, prerequisite evaluation, process launch, output/progress, completion and cleanup. Explain thread boundaries, cancellation and close behavior wherever they exist. A stopped child process and a finished UI state must agree. Paths, settings and operation outcomes need clear owners.

Use Qt for Qt's responsibilities. Personally design the Sparkle workflow around it. Reimplementing widget layout or copying state between several models would need a specific justification. This is how a small non-rendering component can become an example you are proud to explain.

## Options for changing the code

The final product scope and the migration method are separate choices.

| Method | How it works | Main risk | When it fits |
| --- | --- | --- | --- |
| **Review and refine in place** | Trace existing code, accept designs you understand, then simplify or repair them | Better-looking code can still reflect decisions you never made | A subsystem whose model becomes clear and defensible during study |
| **Gradually reauthor retained responsibilities** | Write your own requirements and design, implement a bounded replacement, prove the selected behavior, then remove the replaced mechanism | Temporary interaction with old boundaries and integration work | **Recommended default for the ownership gap you described** |
| **Reconstruct a reduced Sparkle on an isolated branch** | Preserve the old working state as reference and build only the selected engine, bringing over code after review | Rebuilding infrastructure can delay a working result and repeat scope growth | Existing coupling prevents a coherent small boundary without constant unrelated changes |

A new branch or directory does not establish authorship. The gain comes from your design decisions and implementation work. Conversely, useful existing code can become part of your design after you understand it, challenge its assumptions and consciously retain it. Rewriting every line regardless of its merit would spend time without necessarily adding understanding.

For a gradual replacement, keep the old implementation available in history or a preserved reference. Remove superseded production code, callers, state and build membership once the replacement is accepted. Avoid accumulating a permanent old/new compatibility layer.

## The standard for everything that stays

Apply the following to every retained first-party responsibility, including scripts and the Launcher. Understanding one rendering feature will not close ownership of the rest of Sparkle.

| Requirement | What you must be able to show |
| --- | --- |
| Purpose and design | Explain why it exists, its smallest useful scope and why you chose its design over a simpler alternative |
| Complete data flow | Trace real inputs through transformations to every relevant consumer; identify units, formats, identities and mutable-state owners |
| Lifetime and failure | Explain creation, use, synchronization, cancellation where relevant, recovery and destruction; exercise meaningful failure paths |
| Independent understanding | Make a meaningful change and predict its effects without asking an AI to design or explain the solution for you |
| Implementation quality | Readable cohesive code, narrow justified interfaces, one authority for important facts, and removal of replaced machinery |
| Correctness and cost | Appropriate numerical/image/API checks and measurements for the claims you make; performance-sensitive behavior explained by evidence |
| Usability | A reproducible build/run path, useful diagnostics, required sharing rights and instructions that match actual behavior |

This standard covers normal and exceptional behavior within the selected scope. It does not require speculative infrastructure for every possible future use. Keep the architecture explanation short enough that you can teach it; create detailed notes only where they preserve important reasoning.

For the parts that most directly demonstrate authorship, start from your own sketch and write the central implementation yourself. Use AI to question assumptions, review code and help investigate bounded issues. Accept generated assistance only after checking and understanding it. A document written by AI saying that you own the design is not evidence that you do.

## A staged cleanup and ownership plan

### Stage zero Choose the engine you will finish

Select A, B or C; decide the actual workflows, scenes, rendering configurations and intended backend scope. Preserve the current committed and uncommitted state and any content needed to reproduce the reference. Establish one fixed-camera baseline where possible, while retaining known defects as defects.

Map the whole retained footprint, beyond the files you already know. Every responsibility needs one outcome: remove it, retain it after you understand and accept it, or redesign and reimplement it. Include callers, build rules, scripts and integration bindings. Keep one concise scope record in this document rather than creating another overlapping inventory.

**Exit:** a concrete product boundary and a reason for every retained responsibility. Then reconcile [Goals](Goals.md) and [Roadmap](Roadmap.md) with that choice before implementation. The direction is agreed; the precise feature cuts remain open.

### Stage one Remove responsibilities outside that boundary

Retire unused applications, features, formats and workflows through their full dependency paths. Remove the associated targets, settings, UI, scripts, payloads and documentation when they have no remaining consumer. Retain checks and failure handling that protect the smaller product.

**Exit:** the selected product builds and reaches its selected baseline without depending on retired systems. Count the complete maintained change; moving C++ into scripts or bindings is not a saving by itself.

### Stage two Own the application and support workflows

Make the selected launch-to-shutdown path yours. If Launcher stays, use one small real action as the first complete reauthoring exercise, including its failure and close paths. Continue through application startup, settings, content preparation and scene loading. Keep source conversion and runtime loading distinct where their lifetimes differ.

**Exit:** you can trace and modify all retained support workflows, and explain how they connect to rendering. A clean UI alone does not complete this stage.

### Stage three Own the graphics system

Work through one complete frame: scene data, GPU resources, descriptors, shaders, passes, graph decisions, command recording, submission and presentation. Address native API state, CPU/GPU concurrency, resource retirement, resize and shutdown. For retained lighting and reconstruction, derive the relevant math, signal meanings and temporal assumptions.

Rework a bounded responsibility when its current architecture prevents a clear explanation. Use existing code as a reference to challenge, rather than a template to regenerate unchanged. Extend the review to every retained path, including Reference and any second backend.

**Exit:** the entire retained graphics design is explainable and independently changeable by you, with appropriate checks of its important behavior.

### Stage four Qualify the finished engine

Exercise the retained workflows together. Check asset/material interpretation, images, motion/history reset, API validation, actual frame cost and memory behavior. Verify the supported build/run and package paths. Include missing inputs, operation failures and interruption cases appropriate to what remains.

**Exit:** the agreed product works within stated limits, measurements are tied to actual source/content/settings, and ownership reviews have no unexplained retained areas. Finish remaining review findings before adding features.

### Stage five Present Sparkle and close cleanup

Prepare a short demonstration, a concise system overview, selected code/capture links and accounts of significant decisions and failures. Rehearse explaining the whole architecture and changing a meaningful part under questions. Distinguish your implementation, AI assistance and upstream contributions accurately.

**Exit:** Sparkle is a coherent portfolio piece, you can defend all retained first-party systems, and its instructions reproduce the promised result. Only then start the separate small experiment repositories.

## Time and finish conditions

The earlier 12-week schedule was for one bounded case. It is not a credible promise for owning every retained subsystem in an engine of the observed size.

Use the first **20–30 hours** to settle scope and inspect representative support and graphics responsibilities. Time the first completed ownership/reimplementation exercises, then forecast the remainder from comparable work. Treat the first 12 weeks as a checkpoint: review how much scope has actually disappeared, what you personally understand, and whether the target still fits your available time.

For scale only, these are hypothetical remaining budgets, not estimates for Sparkle:

| Remaining effort | Calendar time at 10–15 hours a week |
| --- | --- |
| 300 hours | 20–30 weeks |
| 600 hours | 40–60 weeks |
| 900 hours | 60–90 weeks |

If the forecast is too long, remove more responsibilities or choose Option A. Preserve the ownership standard. A larger engine with undocumented or unexplained corners would leave your original concern unresolved.

Cleanup is complete when the chosen product is working and reproducible, every retained first-party responsibility meets the ownership standard, replaced machinery is removed, and you can explain the whole design. Remaining ambitions belong to later work. Known limitations can be explicit, but an unverified core behavior promised by the chosen product remains unfinished.

## The decision in front of you

My recommendation is **Option B with gradual reauthoring**: Sparkle as a compact rendering workbench, your own graphics architecture, a small deliberately designed Launcher, limited content and controls, and full understanding of everything retained. Choose A if that support scope is not worth its cost to you. Choose C if the distinct Editor/Game workflows are important enough to own in comparable depth.

The immediate decision is which Sparkle you want to finish. The architecture and implementation should then be shaped through your own design work, with the present repository serving as experience and evidence rather than an obligation to preserve every feature.

## Evidence behind the plan

The following professional and repository context was reviewed for the original memo. Its dated measurements and role sources support the scope discussion; they do not establish that the present implementation is correct or interview-ready.

### Your starting position

Your supplied LinkedIn screenshot lists Senior Engineer — Rendering from August 2026, an earlier rendering-engineer role, technical-art engineering roles, and four years at Techland. It names UE5, path/ray tracing, denoising, upscaling, Lumen, Nanite and performance, with a Witcher 4 demonstration link. Earlier roles include shaders, optimization, pipelines and teaching. This is a substantial professional foundation for a graphics application and integration role.

The important unresolved issue is your own assessment that you cannot confidently explain much of this repository or defend its design. That matters directly to an interview. Repository breadth cannot substitute for being able to diagnose an unfamiliar failure, change code under constraints, or explain why a design is correct.

The screenshot supports the career history and listed topics. It does not establish your exact personal contribution to a team demonstration or your present C++/API proficiency. Prepare those accounts from work you actually did. The [July CV baseline](References.md#professional-baseline) contains useful older claims, but needs updating, particularly for your August promotion. Direct access to the [LinkedIn profile](https://www.linkedin.com/in/lisowskipawel/) was blocked, so this assessment uses your supplied screenshot and the repository's dated baseline.

Your professional background gives Sparkle a useful context. The next step is to make its selected architecture and implementation something you can defend throughout, and to describe your own contribution accurately.

| Capability | Your current position supports | What would strengthen the account |
| --- | --- | --- |
| Rendering and content integration | Several years of listed rendering, shader, technical-art and optimization work | One precise production example with your decisions, constraints and personal contribution |
| Independent C++ and API ownership | Relevant source exists; your reported understanding is the present concern | Every retained Sparkle subsystem explained, debugged and changed by you |
| Mathematics and algorithm design | Relevant graphics topics appear in your experience | A derivation, a checked implementation and an explanation of assumptions or bias |
| GPU performance engineering | Performance is explicitly part of your listed work | A controlled capture-based investigation connecting a hardware mechanism to measured results |
| Neural technology development | Integration is a plausible starting point; model/kernel ownership is unestablished here | A later bounded model/operator study if the chosen role needs it |

These are evidence priorities, rather than a finding that unshown skills are absent. Your rendering and content background can help you identify consequential failures and realistic workloads; retain that advantage as you deepen the systems work.

### What the target work asks you to demonstrate

There are two related directions within your preference. **DevTech integration work** connects technology to an application's real constraints: inputs, correctness, quality, performance, debugging and communication with developers. **Core SDK and algorithm development** adds deeper responsibility for the implementation being integrated. An integration case can establish the first; a substantive owned implementation or improvement strengthens the second.

These official role descriptions illustrate the distinction:

| Role example | What it emphasizes | Implication for your portfolio |
| --- | --- | --- |
| [NVIDIA Principal Developer Technology Engineer, JR2013547](https://nvidia.wd5.myworkdayjobs.com/nvidiaexternalcareersite/job/Principal-Developer-Technology-Engineer_JR2013547) | Advanced rendering, AAA developer collaboration, GPU/system optimization, neural algorithms, architecture/driver collaboration, demos and technical communication. Requires 15+ relevant years and a relevant master's degree or equivalent experience. The listed locations include Poland. | Strong directional match. Treat its principal scope as a long-term reference; choose near-term level from your actual experience and the specific vacancy. |
| [NVIDIA Neural Graphics Developer Tools, JR2015681](https://nvidia.wd5.myworkdayjobs.com/nvidiaexternalcareersite/job/US-CA-Santa-Clara/Software-Engineer--Neural-Graphics-Developer-Tools_JR2015681) | Maintainable C/C++, graphics APIs, GPU/computer architecture, collaboration and delivery. Also asks for demonstrated AI/deep-learning experience and 5+ relevant years. Listed locations are in the US. | A renderer alone would leave the AI requirement unresolved. Tools development is an adjacent route, with its own requirements and location constraints. |
| [AMD Senior Rendering Development Engineer, 80456](https://careers.amd.com/careers-home/jobs/80456) | Turns research prototypes into mature technology, integrates with game partners, and optimizes, extends, packages and documents shader/C++ code. Lists modern shaders/C++, mathematics and GPU knowledge under preferred experience; ML frameworks, PBR and explicit APIs appear under desirable experience. Located in Gdansk. | A particularly relevant comparison for integration plus technology development. A bounded feature delivered to another developer fits this work well. |
| [AMD Senior Graphics Machine Learning Engineer, 93698](https://careers.amd.com/careers-home/jobs/93698) | Graphics fundamentals, Python/C++, ML applied to visual computing, GPU optimization and collaboration. | Choose this direction if model development interests you enough to become a major study commitment. A DLSS integration demonstrates integration work, without establishing ownership of its trained model. |

These are dated role examples retrieved from official employer listings on 25 November 2026. Search-accessible descriptions do not guarantee that a vacancy is still accepting applications. The current AMD 80456 wording also differs from the older role archive: use the current listing's distinction between preferred and desirable experience.

**The portfolio implication is depth plus transfer.** Show that you can understand a rendering problem, make a consequential implementation decision, measure the result, investigate a failure and make the work usable by another engineer. This is my synthesis of the roles, rather than an employer promise about hiring.

### What the repository footprint tells us

The deleted `Docs/RepositoryFootprint.md` can be recovered with:

```powershell
git show 'e83458721^:Docs/RepositoryFootprint.md'
```

Its final baseline recorded **150,211 owned code lines in 2,429 files**, using cloc on a frozen 22 November snapshot. RHI, Renderer and shaders represented **46.8%**. It also recorded a period where native code fell by 967 lines but automation grew by 1,061, leaving total owned code slightly larger. That supports a practical caution: delegation and cleanup can move maintenance rather than remove it.

The inventory recorded earlier on 25 November gives this scale for the reviewed working tree:

| Area | Files | Physical lines |
| --- | ---: | ---: |
| RHI, Renderer and shader sources under Engine/Assets | 1,136 | 82,164 |
| Launcher, Editor, Application and project code | 433 | 37,881 |
| GameFramework, importers and cookers | 413 | 24,090 |
| ShaderCompiler and its build files | 104 | 7,938 |
| Core, Platform, ToolSupport and remaining measured build glue | 226 | 13,778 |
| **Total within this inventory** | **2,312** | **165,851** |

These are **physical lines including comments and blanks**, measured from existing tracked and nonignored untracked files in `Engine`, `Tools`, `Projects`, `CMake` and the root CMake file, at HEAD `3262d5b9f` with local edits. The included extensions are C/C++ headers/sources, HLSL/Slang/GLSL, CMake, Python, PowerShell and batch scripts. ThirdParty, generated outputs, content binaries, documentation and other roots are excluded. This is a scale estimate with a different method and scope from the deleted cloc baseline; the two totals do not establish growth or savings.

Three findings support reducing scope before reworking the implementation:

1. **There is a substantial graphics project worth preserving.** [Frame construction](../Engine/Renderer/Private/Frame/Graph/BuildRenderFrameGraph.cpp), [frame lifetime](../Engine/Renderer/Private/Frame/FramePipeline.cpp), native RHI backends and shader sources give you real systems to investigate.
2. **Much of the maintenance serves an engine product.** Launcher, editing, content conversion, cooking and deployment support broader workflows than one graphics experiment. Removing those capabilities could reduce your responsibility substantially, but preserving all current functionality limits those cuts.
3. **Graphics infrastructure itself remains large.** The RHI alone accounts for 37,814 physical lines. Removing UI and tooling would still leave a considerable systems project. Large reductions require choosing which responsibilities you will own and which you will borrow or retire.

The existing Streamline path is especially relevant to your target. [View constants](../Engine/Renderer/Private/Streamline/StreamlineViewConstants.cpp) translate matrices, jitter, motion-vector scale, depth convention and history reset. [DLSS resource tagging](../Engine/Renderer/Private/Upscaling/NvidiaDlss/StreamlineDlssResourceTags.cpp) and [Ray Reconstruction tagging](../Engine/Renderer/Private/RayReconstruction/NvidiaDlssRayReconstruction/StreamlineRayReconstructionResourceTags.cpp) connect application signals and native resources to the SDK. These are examples of the integration details you would need to understand if this path remains in Sparkle.

Source presence establishes implementation, with further checks needed for correctness. Your roadmap still lists Sponza image recovery, reference accuracy and separate D3D12/Vulkan qualification as open work. This review does not establish a working current build or a correct frame. It also cannot determine an AI-generated percentage or classify every module's quality from its size.
