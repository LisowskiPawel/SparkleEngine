# Editor

[Docs](../README.md) ? [Subjects](README.md)

Editor inspects the world and rendered result, submits supported typed edits, and hosts developer controls. Game is a separate rendering product; it does not acquire Editor UI or tools through shared hosting.

## Read and edit

GameFramework publishes an immutable read view and sequence. Editor pins that view to build scene/inspector rows; selection and presentation are local UI state. A forward/inverse typed command carries the expected world generation. GameWorld validates and commits it, then publishes a new view and render submission. Undo/redo uses the same route. A held read view remains valid across newer publications and world retirement.

Only explicit Save All writes the selected authored `.level` source. Failed saves preserve accepted source bytes; rejected or stale edits cannot become optimistic source truth. Unsupported edits remain visibly unsupported.

## Rendering and developer tools

Application composes viewport, settings and read-only mesh/texture/shader/memory products. Renderer owns settings semantics and active results; Application owns persistence; Editor owns controls and presentation. Failed persistence retains accepted state and reports the error. Restart-required intent remains visible.

**Window > Shaders > Recompile Shaders** and `CompileShaders [symbols]` invoke the existing Launcher shader-cook path. `ReloadShaders` reloads existing outputs. [Shaders](Shaders.md) distinguishes full recompile from undelivered dependency-directed Apply Changed.

Captures, GPU-crash information and optional shader artifacts must distinguish unavailable data from operation failure. An inspection panel owns no compiler, GPU resource or profiling authority. [Diagnostics](Diagnostics.md) covers the native tool route.

Icons/artwork use existing catalogs and required font/assets; retain their own notices and provenance. Do not introduce a parallel UI state/operation framework for a feature.

## Work and checks

Preserve the approved inspect -> typed edit -> undo -> redo -> Save All -> reload path on a disposable authored level, with expected world/source hashes. Exercise stale generation, rejected edit, denied save, cancelled background operation, absent optional inspection data and restart-required settings. Settle work before panel/host shutdown.

For actual package proof, launch Empty and selected maps as a standard user without undeclared source/SDK dependencies, verify expected scene/camera/material/presented output, and inspect Game imports for zero Editor linkage. Build/source evidence does not pass those product cells.

[Editor source](../../Engine/Editor) owns UI mechanisms; [Application](../../Engine/Application) owns host composition; [GameFramework](../../Engine/GameFramework) owns authoritative mutation. Broader authoring tools and public lens-effect controls are not selected merely because a settings seam exists.
