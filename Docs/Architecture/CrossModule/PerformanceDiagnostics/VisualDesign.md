# Performance Diagnostics Visual Research

[Docs](../../../README.md) / [Up](PerformanceDiagnosticsDesign.md) / [Delivery](ImplementationPlan.md)

**Status:** research alternatives; the depicted internal workspace, stat groups and GPU visualizer are unselected.

These four retained mockups illustrate ways a future measurement consumer might present information. Their numbers, capacities, tabs, controls and sample frames are illustrative. They prove no implementation or bottleneck and prescribe no UI delivery sequence.

The former widget-by-widget stat/workspace/evidence-screen specification is retired. It duplicated the architecture and turned an optional internal tool into a large implementation checklist. Current release/workload evidence uses existing native tools and original measurements without recreating these screens.

## Current Capture Experience

The implemented capture interface and remaining UX checks belong to [External Capture](ExternalCapture/UserExperience.md). Use that route for startup selection and the viewport action. This study predates delivered capture UI and cannot authorize rebuilding it or inserting another controller.

## Design Alternatives

### Overview

![Performance overview research mockup](Images/sparkle-performance-overview-mockup-v2.png)

This alternative makes frame distribution, configuration and missing data visible together. If a future live fact is admitted, first test whether an existing panel or the offline result table answers the question; an Overview workspace is not the default implementation.

### GPU Captured Frame

![GPU captured-frame research mockup](Images/sparkle-performance-gpu-capture-mockup.png)

The diagram illustrates marker hierarchy and interval attribution, not hardware cause. Native tools already own API/resource/shader inspection. Internal capture needs separate admission and the architecture's parallel-recording and inclusive/exclusive correctness contract.

### Memory

![Memory diagnostics research mockup](Images/sparkle-performance-memory-mockup.png)

The useful principle is distinguishing working/private memory, tracked allocations/API budgets, sampling age and high-water identity. That does not require a graph, allocation explorer or history service; use the smallest current observation/export that proves the selected claim.

### System Scope

![Diagnostics system-scope research visualization](Images/sparkle-performance-system-scope.png)

Physical CPU threads, logical phases, GPU queues and native artifacts answer different questions. Shared identity and explicit validity connect them; a universal collector, merged timeline or workspace is not implied.

## Rules For A Selected Presenter

Name the actual consumer and observation before choosing a screen. Use milliseconds/counts/bytes with definitions, frame/range and configuration; display age/loss/invalidity rather than plausible zeros. Keep labels/errors understandable without color and supported keyboard/DPI behavior observable. UI reads immutable results, never polls private live state or creates another metric series.

Prototype only the selected question when existing tools prove insufficient. Freeze its producer/lifetime, bounded rows/storage, observer-cost and failure checks, then deliver one real producer-to-consumer slice and delete the replaced presentation. No generic view catalog, dashboard, hitch browser or standard twelve-group layout is selected by these images.

Metric semantics and future capture math remain in [Architecture](PerformanceDiagnosticsDesign.md); delivery admission is [Roadmap](../../../Strategy/Roadmap.md#scope-protection); original comparisons/results stay with their workload/report owner.
