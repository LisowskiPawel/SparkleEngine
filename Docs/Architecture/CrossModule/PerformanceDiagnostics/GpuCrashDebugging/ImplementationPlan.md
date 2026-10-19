# GPU Crash Debugging Implementation Plan

[Docs](../../../../README.md) / [Design](GpuCrashDebuggingDesign.md) / [Roadmap](../../../../Strategy/Roadmap.md#where-we-are-and-what-is-next)

**Status:** GCD-4 final qualification. GCD-0 through GCD-3 are implemented. The four-profile/two-API matrix passed 32 normal runs and 16 native fault cells. Serial D3D12 passed; serial Vulkan collected and recovered, but its source-line oracle failed. A subsequent normal-quit cleanup repair preserves ownership when a manifest is locked; all four products build and its locked/unlocked controls pass. That repair and subsequent responsibility/spacing refactors invalidate affected candidate results, so GCD-4 remains open.

## GCD-4 — Prove Native Crash Debugging

Remaining:

- Resolve the retained serial Vulkan dump's source mapping through a verified compiler/driver/decoder repair. The 2026-10-19 offline check confirms that the pinned SDK receives the exact shader hash and `.nvdbg` identifier, yet PC `0x2f0` still maps to `comp.10000.spv:0`; the same shader maps the earlier dump's PC `0x2e0` to line 23. Missing-input controls return no mapping. Retain the unchanged lines 20?29 oracle. No further faults under the exhausted attempt record, nearest-line substitution or product-stage advance. See [the retained diagnosis](GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures).
- Revalidate the repaired candidate through the affected native and normal checks after source mapping is resolved. Keep earlier dumps bound to their original PE/PDB/shader identities. Disposable fault shaders are restored; private recipes and candidate reports are retained. No completed-plan deletion or product-stage advance before this gate passes.

Use the [design criteria](GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures), existing owners and the Stage Review Gate in [Change Lifecycle](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate). Do not repeat the completed matrix unless a repair invalidates it. Keep the independent 30-second fault supervisor, finite attempt limits, no TDR/registry changes and no permanent injector.

```text
Finish only the remaining GCD-4 checks above. Preserve the existing sixteen native cell results and their exact identities. Qualify serial-host finalization on both APIs, restore fixture material, then finalize candidate reports and cleanup. Do not advance on an unavailable required check.
```

## After This Plan

After all criteria pass, retain the design and useful Acceptance evidence, mark delivery briefly in the roadmap and remove this completed plan. Resume [PD-1](../../FirstRelease/ProductImplementationPlan.md#pd-1--game-and-editor-product-shells): collect Modern Sponza with matched native evidence, fix its owner and rerun affected journeys. Regenerate the archive and continue PD-2/3/4. Keep the unfinished product plan and final FR-07 signing obligations.
