# Window/input: remaining SDL3 delivery

[Delivery rules](../ThirdPartyMigration.md#code-delivery-and-qualification) | [Ownership](../Engineering.md#module-boundaries) | [Dependencies](../ThirdPartyMigration.md#ownership-and-dependency-workflow)

**Next: stage 0; all stages are unstarted.** Trial SDL3 first, GLFW as the alternative. Recorded research inputs are SDL 3.4.20 at `f00dc919ca35b9756316e86c4777392de442f9b2` and GLFW 3.5.1 at `d9d6f0f1f967807ffade6598ea9a631ebaf37a56`; select/admit the actual pin through the existing dependency owner. Runtime fit and net savings remain unverified.

## Stage 0: integration design deep dive

- [ ] Read the nine Docs guides and complete host/window/input/ImGui/RHI producer, consumer, state, thread and teardown paths. Choose the smallest supported boundary, compare alternatives and name actual retirements, ABI/caller/target changes and decisive fit experiments.
- [ ] Decide one runtime, ImGui backend placement, HWND/native interop, capture/cursor authority, relative-motion precision/units, chrome/hit regions, DPI/pixels and owned deferred events. Explain rejected alternatives/unsupported requirements; this research stage adds no prototype/production code.

## Stage 1: bind the baseline and checks

- [ ] Freeze source/local hashes, upstream/options/tools/profiles and scene/camera/settings; define runnable A1-A9 controls and expected outcomes. Set coordinate/camera tolerances and planned startup/build/payload/memory/frame comparison limits before measuring.
- [ ] Capture the focused Win32 baseline needed for the decisive trial; preserve pre-existing defects/unavailable cells separately. No production edits, moving limits or full benchmark prerequisite.

## Stage 2: prove the real integration fit

- [ ] In one isolated candidate, exercise actual engine D3D12/Vulkan presentation, custom chrome, stock ImGui, routed relative camera motion, capture interruption, initialization failure and teardown. A standalone SDL window is insufficient.
- [ ] Pass A1-A4 and decisive A5-A8 without duplicate runtime/state or unsafe cancellation/lifetime. Revisit a narrow supported composition once; if comparable native machinery/fork is needed, record no-fit and evaluate GLFW under the same contract before conversion.

## Stage 3: complete window and presentation integration

- [ ] Convert creation/state, startup maximize, borderless/fullscreen placement, hit testing, minimum/work area, DPI/pixel extent, minimized waiting, native-handle/capture consumers and teardown in final owners.
- [ ] Remove replaced native window/message machinery, update exports/callers/CMake and verify A1/A2/A4/A6 plus window A7 through both existing presentation backends. Preserve atomic dimensions and RHI GPU retirement; actual shared consumers must build/link.

## Stage 4: complete input, camera and Editor integration

- [ ] Finish precise Core value mapping, camera pointer modes, InputSystem subscriptions and Editor/ImGui platform input/shortcuts. Preserve routing/state/dispatch/cancellation ordering, modifiers, text, both wheel axes and safe pointer recovery.
- [ ] Remove superseded Win32 decoders, pointer/message APIs and recurring camera warps; rebuild actual ABI consumers and prove A3-A5 plus input A6-A7, including negative/reentrant cases. Core stays SDL-free.

## Stage 5: retire the backend and close code delivery

- [ ] Audit connected callers/public headers/targets, remove superseded mechanisms/selectors/dependencies and reconcile pin/required files, Launcher Sync/Clean, notices, runtime staging and package inputs. Regenerate fresh membership/builds; preserve existing tests and legitimate native graphics interop.
- [ ] Complete scoped style/manual ownership review and focused A1-A9 with actual shared/runtime/offline/feature-disabled consumers. Remove local probes, report full gross/net and native/non-native/build/runtime/package costs; review misses once without manufacturing savings.
- [ ] Close when focused code/failure/lifetime and retirement exits pass. Transfer broader profile/hardware/image/cost/clean-machine qualification with honest results, repair incoming links and delete this plan.

## Constraints for the remaining work

Keep D3D12/Vulkan, Renderer UI and RHI swapchains/surfaces; no OpenGL, SDL GPU/2D renderer or copied sample framework. Prefer narrow borrowed-HWND Platform interop; keep Vulkan loader/instance policy in its graphics owner. Settle GPU use and destroy surfaces/swapchains before the window; main-thread SDL queries do not replace atomic dimension publication.

One main-thread pump owns native acquisition; Core keeps values/frame state and existing routed policy. Required state updates precede callbacks even when UI consumes an event. Borrow events synchronously, own deferred text/values and preserve subscriber and queued-before-immediate order. Cancellation clears state/queues first, sends releases once under reentrancy and distinguishes pointer-only from full keyboard cancellation. Keep camera intent in its existing collector; retain fractional/continuous edge-crossing motion without warps or mode-entry jumps. Stock ImGui/camera must share coherent capture/cursor authority; command keys stay distinct from text.

Decide SDL shared/static ownership and stock `imgui_impl_sdl3.cpp` placement from the DLL graph; avoid duplicate SDL globals or platform dependencies spreading through ImGui into RHI. Use pinned cache/recipe/target/staging/notices/source-package owners and the current ImGui revision. Disable unused SDL services, `SDL_OPENGL` and `SDL_OPENGLES`; verify ImGui gamepad/backend calls still link. Native HWND Vulkan need not enable SDL helpers; Sparkle D3D12/Vulkan remain independent. Keep one runtime/private dependencies; no upstream fork, compatibility backend, mirrored native state or service facade.

Fixed retirement scope: **752 code / at most 868 physical lines in 13 files**. Net removal is `868 - retained old scoped lines - all new/relocated integration and caller lines`; name additional caller scopes separately. Report CMake/scripts/upstream/build/runtime/memory/package costs separately. Preserve supported behavior and existing product blockers; do not manipulate Core enums or safeguards to manufacture savings.

## Focused acceptance checks still to run

| Check | Positive path | Negative/failure path and required outcome |
| --- | --- | --- |
| A1 — native presentation | Existing scene presents on D3D12 and Vulkan separately; normal resize/fullscreen and affected external capture work. | Zero extent, resize with delayed GPU completion and close during presentation: skip invalid work, settle GPU references and destroy surface/swapchain before the native window. No new native validation errors. |
| A2 — window/chrome | Correct initial maximize, work area, minimum size, drag, edge/corner resize, double-click and windowed/fullscreen restoration. | Click menus/buttons instead of the drag region; drag from maximized; interrupt native drag: controls remain usable and no false camera capture or stuck mouse state survives. |
| A3 — focus/cancellation | UI/Gameplay/System routing and capture ownership remain correct for immediate/deferred subscribers. | Hold movement/look then Alt+Tab, interrupt same-focus capture, open a modal, change target, minimize or close; reenter cancellation callbacks. Clear state first, cancel stale queued input, deliver each logical release once; pointer-only cancellation preserves keyboard state. |
| A4 — event lifetime/wait | One pump, deterministic subscriber order, mandatory native state updates and owned deferred values. | A consumed UI event still updates required state; a single close/release/restore event wakes a minimized app without loss. No queued reference to borrowed SDL text/event storage survives delivery. |
| A5 — input values | Agreed camera response, fractional motion, full exposed key/modifier/repeat behavior, both wheel axes and existing text/clipboard workflows. | Slow/fast/edge-crossing motion, mode-entry failure, focus recovery, AltGr/layout cases where applicable: no jump/truncation, wrong key/wheel direction or hidden cursor. Unestablished IME/layout capabilities remain labelled rather than silently added as release requirements. |
| A6 — DPI/coordinates | 100/150/200% UI scale with exact queried drawable extent and aligned viewport input. | Move across mixed-DPI monitors, resize/minimize/maximize and restore: no double scaling, wrong minimum/work area or mismatch between pointer and rendered viewport. |
| A7 — failure/teardown/ABI | Actual Game/Editor/shared-module consumers use coherent values and the same runtime. | Fail SDL/window/backend/mode initialization, repeat startup/teardown, close while active: truthful state, useful owning error and safe unwinding; no dangling subscriber, stale handle, wrong-thread SDL call or callback into retired state. |
| A8 — dependency/delivery | Correct pin/required files/options, one loaded runtime, target-driven staging, notices and supported offline consumption. | Missing/wrong runtime or source revision: fail clearly and preserve inspection state; no silent provider fallback or unattended download. Feature-disabled/standalone-Launcher configurations gain no unnecessary SDL consumer. |
| A9 — structural retirement | Existing useful contracts remain, code reads in domain steps and complete caller/adapter costs are accounted. | Fresh regenerated outputs expose stale references: none may be satisfied by old objects/backends. No duplicate owner, compatibility facade, copied upstream implementation, permanent probe or deleted existing test masks the change. |

Replacement cells remain UNVERIFIED until observed on bound inputs. Keep PASS/FAIL/INCONCLUSIVE/UNVERIFIED distinct; only an explicit waiver is WAIVED / UNVERIFIED. Required fit/correctness/unsafe lifetime failures block code delivery; pending broader campaigns do not extend a completed code initiative.

```text
Read Docs/Implementation/WindowInput.md and apply Docs/ThirdPartyMigration.md#code-delivery-and-qualification.
Complete the requested remaining stage and prerequisites. Keep design/baseline research code-free until their exits; for full integration continue the proven candidate through conversion, A1-A9 and retirement. Preserve concurrent work and native graphics/input contracts. Remove each completed stage/action immediately and delete the plan when required delivery closes.
```
