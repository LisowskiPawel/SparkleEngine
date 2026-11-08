# Platform Capability Inventory

[Docs](../../../../README.md) / [Up](../README.md)

**Status:** capability snapshot; scoped source/build/native controls, not product or release qualification

**Snapshot:** module ownership and input contracts reconciled on 2026-11-08 at `0758e939292f` plus the scoped working tree. The local repair record below distinguishes source/build/native controls from capability-table evidence `S` and release acceptance.

**Scope:** native window ownership, DPI, message pumping, Win32 input acquisition, layered routing, capture, and cursor control

**Owner:** `Engine/Platform` / `SparklePlatform`

**Input cleanup, 2026-11-01:** the deleted Game UI left no producer for the capture-query callback. That API, storage and deferred-dispatch hook are removed; Editor continues to supply per-frame routing intent. [Submission checks](../../../../../artifacts/validation/changelist-submit-20261101/completion.json) retain source/build limits and do not establish new interactive input acceptance.

**Evidence and disposition:** [Capability Evidence Plan](../../CapabilityEvidencePlan.md) and [First Release Acceptance Contract](../../../../Acceptance/FirstRelease.md)

**Supported host:** Windows only. Linux support is excluded from the selected roadmap and has no delivery plan.

**Current readiness:** **50/100** for the tracked Windows platform scope — Win32 window/input paths exist; lifecycle/runtime/package proof remains open and other host platforms are excluded. See [Current Feature Readiness](../../../../Acceptance/CurrentReadiness.md#foundation-world-content-shaders-and-tools).

## At A Glance

| Surface | Current implementation | Explicit gap |
| --- | --- | --- |
| host platform | one Win32 top-level window, message pump, size/state, borderless fullscreen, DPI handling | no Linux/macOS/headless or multi-window platform route |
| input acquisition | Win32 keyboard/mouse/wheel translation | no gamepad, touch, pen, IME, raw-device catalog, or hotplug |
| routing | immediate/deferred typed events across Gameplay/UI/System layers with capture and viewport bounds | physical alt-tab, mixed-monitor DPI and the complete interaction matrix remain unqualified |
| cursor/window control | capture, visibility, centering, native drag/minimize/maximize/restore | public contracts expose Win32 types and are not yet portable |

The module separates native event acquisition from Core input vocabulary and Application/editor consumers. That keeps routing consistent, but the present public window seam still makes Windows the effective product platform.

## Platform Boundary

The present implementation is Windows-only: public `Window` exposes `HWND` and Win32 message types, the input backend is `Win32InputBackend`, the application manifest is Windows-specific, and `dwmapi` is linked privately. There is no Linux/macOS backend in this snapshot.

## Capability Surface

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `PLAT-001` | Native Win32 window | Implemented path | Registers one Sparkle window class, creates one top-level popup/resizable application window, extends DWM frame into client area, exposes its `HWND`, and fatals on registration/creation failure. | `S` |
| `PLAT-002` | Message pump | Implemented path | Non-blocking `PollEvents` drains the thread queue, recognizes `WM_QUIT`, and synchronously broadcasts raw window messages; `WaitForEvent` blocks while minimized/idle until a message arrives. | `S` |
| `PLAT-003` | Size/state | Implemented path | Atomically published client width/height, valid-size check, and Normal/Minimized/Maximized state plus independently retained fullscreen mode. Minimum interactive size is 320x240. | `S` |
| `PLAT-004` | Window controls | Implemented path | Request close, minimize, maximize, restore, maximize/restore toggle, borderless monitor fullscreen toggle, and native drag move. Startup is maximized; fullscreen remains an explicit runtime action. | `S` |
| `PLAT-005` | DPI awareness/events | Implemented path | Manifest plus per-window DPI query, scale relative to 96 DPI, DPI-change event, suggested-rect handling, and DPI-aware resize-border metrics. Multi-monitor behavior remains unrun. | `S` |
| `PLAT-006` | Window event contracts | Implemented path | Synchronous resized, DPI-scale-changed, and capacity-16 raw message events. Subscribers run on the polling thread and can mark raw messages handled. | `S` |
| `PLAT-007` | Win32 keyboard/mouse backend | Implemented path | Translates Windows messages into typed keyboard, mouse-button, move, vertical/horizontal wheel events and modifier state. No gamepad, touch, pen, IME composition, or raw-input device catalog was found. | `S` |
| `PLAT-008` | Per-frame input state | Implemented path | Global and per-layer key/button transitions advance at `BeginFrame`, which also clears accumulated pointer and vertical/horizontal wheel deltas. Pixel position, modifiers and capture/cursor flags persist until changed. Both typed event delivery and the full polling/convenience API remain available. | `S` |
| `PLAT-009` | Immediate/deferred dispatch | Implemented path | Subscribers choose typed event, Gameplay/UI/System layer, and immediate/deferred delivery. Deferred events preserve cross-type arrival order and are processed explicitly after the native message pump. Cancellation releases reach both modes synchronously. | `S` |
| `PLAT-010` | Layer routing | Implemented path | Gameplay/UI enablement, unconditional System observation, one active target, per-frame UI target rectangles, text-input/interactions-disabled policy, point/mouse-move targeting, and layer cancellation. | `S` |
| `PLAT-011` | UI capture integration | Implemented path | Editor supplies per-frame UI capture and viewport routing intent through `BeginInputRoutingFrame` and viewport registration. Game uses the gameplay layer directly. The unused Game-UI capture callback channel is removed. | `S` |
| `PLAT-012` | Mouse/cursor control | Implemented path | Capture/release, hide/show/visibility, center cursor, and capture-layer tracking. Cancellation has focused controls; foreground capture and physical alt-tab product coverage remain open. | `S` |
| `PLAT-013` | Owner-thread enforcement | Implemented path | Input registration, mutation, state access and dispatch are guarded by Core `OwnerThread`; the module is not generally thread-safe. | `S` |

## Vertical Input Trace

### Current Ownership And Message Flow

Source reconciled on 2026-11-08 against `0758e939292f` plus this working tree.
Native controls run on the window's creating thread. Input registration,
mutation, state access and callbacks run on InputSystem's creating thread;
atomic window dimensions retain their existing cross-thread read contract.

| Owner | Responsibility |
| --- | --- |
| `Window.cpp` | window lifetime, message admission and capability selection; no native creation/display/pump algorithms |
| `WindowCreation.cpp` | class registration, UTF-8 title conversion, native creation and destruction |
| `WindowMessages.cpp` | native message pumping and per-window pointer binding through `WM_NCDESTROY` |
| `WindowDisplay.cpp` | show commands, fullscreen placement/restore, DPI and size publication, native window shortcuts |
| `WindowNonClient.cpp` | client-area adjustment, resize hit testing and tracking constraints |
| `InputSystem.cpp` | subscription lifetime and the decode, cancellation, pointer-baseline, focus and dispatch sequence |
| `InputStateTransitions.cpp` | typed event projection, frame transitions and relative-pointer baseline |
| `InputCancellation.cpp` | focus/capture/layer reset, stale queue removal and release notification |
| `InputPointerControl.cpp` | logical capture/cursor publication; private Win32 helpers own native operations |
| `Input/Routing` | one active-target authority, viewport-region targeting, text/interaction admission and enabled layers |
| `Input/Dispatch` | typed callback registration, callable-snapshot lifetime and one FIFO deferred queue and cancellation delivery epochs |
| `Input/Backends` | native message selection plus focused keyboard/mouse decoders and pointer operations |

Win32 message -> Window raw event -> stateless native decoder -> InputSystem
selects cancellation or normal admission -> global/routed state publication ->
deferred enqueue -> immediate callbacks -> explicit deferred processing.
Acquisition observes the raw message without marking it consumed, so Window's
F11/Alt+Enter handling and native Alt+F4 processing remain reachable. Shortcut
repeats do not repeatedly toggle fullscreen. Window retains a valid native
handle through final non-client destruction and recognizes thread `WM_QUIT`. Invalid UTF-8 titles fail before native class acquisition.
Fullscreen stores native window placement and restores the prior normal or
maximized state; display mode remains observable while minimized.

Keyboard identity comes from scan code and extended-key metadata, independently
of sampled modifier state. Acquisition covers all eleven declared modifier bits, including both Super keys and the Caps/Num/Scroll toggle bits; Core polling uses the same full-width flag predicate as typed events. Mouse motion
carries modifiers; vertical/horizontal wheel positions are converted from
screen to client coordinates exactly once. Missing window/conversion results
reject the wheel event before state or routing publication. Unknown keys are
not published as physical `Key::Unknown` presses.

Result packets directly initialize their selected union payload. Event records have nontrivial default constructors, so assignment into an inactive payload is not used to establish its lifetime. [C++ union lifetime rules](https://eel.is/c++draft/class.union) support this source-level lifetime check; compiled native cases separately verify the published event values.

InputSystem retains one global state and two routed states, for UI and Gameplay.
System subscribers observe global input; there is no unread System-layer copy.
The focus router owns the active target. Native acquisition, dispatch and state
remain private and have no Application/Editor implementation dependency.

Focus loss/deactivation clears all accepted state, queued events and pointer
baseline before notifying consumers. Capture loss clears pointer state and
queued pointer events while preserving keys and their deferred delivery. Layer
cancellation clears only that layer and does not fabricate a global release.
Both immediate and deferred subscribers receive cancellation releases
synchronously, including when their layer has just been disabled. Cancellation
snapshots exist only until callbacks settle; the published states are already
reset. Core's public `InputState::ResetPointerState` owns pointer-field reset,
without a Platform friendship or private-storage dependency. Layer cancellation downgrades queued events to System-only delivery so global observers retain accepted input.

Deferred events retain arrival order across keyboard/button/motion/wheel kinds.
Each event leaves the queue before invoking callbacks, so recursive processing,
queue clearing and callback failure do not replay a consumed event. Registry
mutations apply to the next invocation snapshot. Each registration retains one callable; invocation snapshots hold lifetime handles, preserving mutable target state even during nested calls or unsubscribe. Layer and pointer delivery epochs stop stale normal callbacks after cancellation, while cleanup notifications settle independently of repeated empty resets. `BeginFrame` retains the existing
policy of dropping unprocessed events from the previous frame.

Capture targets the subscribed window and publishes success only after native
capture ownership is observed. Release publishes logical state before the
native call, so synchronous capture-loss reentry sees the accepted state.
Destruction removes the Window subscription before releasing pointer control.
Application's camera collector consumes logical state and capture results; it
contains no foreground-window polling. Its remaining Window reference supplies
window dimensions and the existing cursor-centering target.

### Cursor Recentering And Failure Publication

Source rechecked on 2026-10-31 at `655f5fe8…` plus the FWC-1 patch: `CenterWin32Cursor` rejects a missing/invalid window, empty client area, or failed client/screen conversion, cursor move or position readback through its existing `std::optional<MousePosition>` result. After a successful move it reads the actual screen position and converts that observation to client coordinates. [Windows may clip a requested cursor position](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setcursorpos); returning the requested center would give the next mouse event a false baseline.

`InputSystem::CenterCursor` remains the sole last-pointer-cache publisher and updates it only when that optional result exists. The helper owns native mechanics and no retained state; Application's camera collector retains its existing mouse-look policy. No public API, input layer, capture state, event lifetime or package path changes. Null results preserve the previous accepted cache instead of fabricating a new position.

[FCR-PLAT-01's FWC-1 records](../../../../Acceptance/FeatureCompletionReports.md#foundation-contract-progress) own the passed native-call failure controls, compiled InputSystem consumer checks, actual Windows client/screen/clipping observations and helper zero-size/restore evidence. Current dependency-built Game/Editor candidates contain the repair; the record also retains native Game D3D12 Sponza capture/recenter/release and focus-loss observations at 144 DPI. Selected-stage closure includes an explicit desktop-proof exclusion. It grants no complete product interaction matrix, mixed-monitor DPI, rendering-quality or installed acceptance.

<details>
<summary>Scoped ownership repair and checks</summary>

### Module Ownership Repair

`ITER-PLAT-02` starts at `0758e939292f` with concurrent changes outside Platform.
The owner is `Engine/Platform`; the scoped repair is complete. Scope is the complete Windows window/input module and its camera-input
consumer. `NS-OWNERSHIP`, `NS-SIMPLIFY`, `NS-REAL` and `NS-EVIDENCE` apply to the repaired production path. It preserves `PLAT-001` through `PLAT-013`; graphics requirements and
release qualification are not advanced by this maintenance slice.

| Criteria and failure controls | Smallest check |
| --- | --- |
| `AC-PLAT-02-01`: orchestration selects and sequences focused capabilities; one input/window lifetime owns state. `FM-PLAT-02-01`: stale sources, duplicate state or outward client dependency. | `CHK-PLAT-02-01`: complete module/consumer review, generated membership, scoped source-style and documentation checks. |
| `AC-PLAT-02-02`: decoded input preserves native message processing, client-space coordinates and sided key identity. `FM-PLAT-02-02`: swallowed shortcuts, offset wheel routing or stuck modifiers. | `CHK-PLAT-02-02`: compiled message/decoder probes with negative controls. |
| `AC-PLAT-02-03`: focus/capture/layer cancellation resets accepted state and notifies affected consumers; deferred delivery preserves arrival order. `FM-PLAT-02-03`: stale held keys, capture, queued presses or suppressed releases. | `CHK-PLAT-02-03`: compiled cancellation, callback mutation, deferred-order and camera-consumer probes. |
| `AC-PLAT-02-04`: native lifetime/display work retains valid message handles and supported window controls. `FM-PLAT-02-04`: destruction or display-transition regression. | `CHK-PLAT-02-04`: focused Platform build and bounded native window checks. |

`RISK-PLAT-02-01` is callback reentry during cancellation: state and queue
publication must precede callbacks; the probes detect stale publication and the
Platform owner repairs the same path before completion. `RISK-PLAT-02-02` is
native window/input regression: source-preserving extraction plus focused native
checks retire the exercised cells, while interactive multi-monitor acceptance
remains with its existing report. Performance is classified as preserves:
ordinary decoding remains bounded, cancellation scans only fixed key/button
sets, and the unused System-layer state is removed. No new subsystem, registry,
backend abstraction or public configuration is admitted.

**Decision:** `PASS` for `AC-PLAT-02-01` through `AC-PLAT-02-04`. [Local check report and source identity](../../../../../build/validation/platform-ownership-20261108/result.json) bind the scoped working tree, commands, logs and limits. Core and Platform `DevelopmentEditor` builds passed; 46 compiled public/native checks passed; all six controlled fault variants were detected. Mechanical source-style checks cover all 28 Platform source/header files and the three Core/Application integration files. The complete module review found no remaining orchestration, file/folder cohesion or duplicate-authority defect in this slice.

**Evidence boundary:** native controls ran on a separate Win32 desktop without switching the user's input desktop. Foreground capture success, physical alt-tab, mixed-monitor DPI, GPU behavior and product/release qualification remain outside these checks. The module's readiness projection is unchanged. New probes and fault-injection sources/binaries are local-only and removed before handoff; the check logs and source hashes remain.

The only production integration hooks outside Platform are Core input-state reset and the Application camera consumer. Core owns pointer-field reset; Application removes native focus inference and respects capture admission. Their defect-detecting checks are `CHK-PLAT-02-03`; the Core/Application dossiers link the same ownership boundary.

</details>

<details>
<summary>Next module iteration: callback and modifier hardening</summary>

### Callback And Modifier Hardening

`ITER-PLAT-03` continues the complete module review at `0758e939292f` plus the
working tree, preserving concurrent changes. The earlier candidate's recorded
checks remain historical evidence; they did not cover the callback-state,
in-flight cancellation, deferred System-observation and modifier-polling defects
reproduced in this iteration. Platform owns the repair; Core's existing flag
predicate owns full-width polling. No graphics/workload or release gate advances.
`NS-OWNERSHIP`, `NS-SIMPLIFY`, `NS-REAL` and `NS-EVIDENCE` apply.

| Criteria and failure control | Smallest check |
| --- | --- |
| `AC-PLAT-03-01`: one registered callable retains its mutable state while invocation lifetime survives registry mutation. `FM-PLAT-03-01`: callback cloning resets captured state or unsubscribe destroys a running callable. | `CHK-PLAT-03-01`: compiled mutable/self-unsubscribe/nested-dispatch probes and baseline comparison. |
| `AC-PLAT-03-02`: cancellation invalidates affected in-flight input, delivers cleanup releases, and preserves accepted System observation when only a routed layer is cancelled. `FM-PLAT-03-02`: release-then-press inversion or dropped global events. | `CHK-PLAT-03-02`: public InputSystem probes across immediate/deferred delivery and layer/focus/capture cancellation, with negative controls. |
| `AC-PLAT-03-03`: native acquisition and polling preserve the complete modifier mask, and event packets initialize their selected payload lifetime. `FM-PLAT-03-03`: missing/truncated flags or inactive-union assignment. | `CHK-PLAT-03-03`: all declared flag bits, native keyboard-state controls, decoder cases and a source lifetime check. |
| `AC-PLAT-03-04`: the complete folder retains focused responsibilities, inward dependencies and current generated membership. | `CHK-PLAT-03-04`: full module review, focused owner builds, scoped source-style/docs checks and source hashes. |

**Decision:** `PASS` for `AC-PLAT-03-01` through `AC-PLAT-03-04`. The [iteration report and source hashes](../../../../../build/validation/platform-iteration3-20261108/result.json) bind the complete folder review, six reproduced baseline failures, 63 passing checks, five detected regressions, owner builds and verification limits. `RISK-PLAT-03-01` is stale delivery
after callback reentry, already reproduced and capable of leaving consumers with
held input after cancellation; per-layer and pointer-lifetime invalidation must
stop it before handoff. `RISK-PLAT-03-02` is callback lifetime/cost: registration
retains one callable and active invocations borrow ownership instead of cloning
mutable targets. A bounded dispatch/decoder probe compares the cost with the
frozen baseline. Copies are restricted to invocation handles, delivery epochs,
native acquisition results and existing cancellation/queue boundaries. The
Platform owner repairs a failed oracle in place; no compatibility path or new
subsystem is admitted.

The frozen standalone cost check measured small callbacks at 0.0306 -> 0.0340 microseconds, large callbacks at 0.0715 -> 0.0330 microseconds, and modifier sampling at 2.58 -> 3.70 microseconds. Large-target copies fell from 100,000 to zero across 100,000 dispatches. The complete modifier mask requires eleven native key-state queries rather than eight. These are local CPU microchecks within their declared bounds, not frame-time or GPU performance evidence. No new production file, subsystem, public operation or dependency is added; the callback lifetime box is per registration and delivery epochs occupy fixed owner state.

</details>

## Explicit Non-Capabilities And Risks

- No non-Windows window/input backend, multiple-window manager, headless platform host, gamepad, controller hotplug, clipboard, file dialog, drag-and-drop, accessibility API, touch, pen, or IME composition capability was found in `Engine/Platform`.
- Fullscreen is borderless-monitor fullscreen, not an exclusive display-mode API.
- `Window` directly exposes Win32 types; this is not an opaque cross-platform contract.
- Resize/DPI/focus/capture behavior needs paired normal, minimized, fullscreen, multi-monitor, alt-tab, and high-DPI execution evidence.
