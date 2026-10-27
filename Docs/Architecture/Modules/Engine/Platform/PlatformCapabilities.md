# Platform Capability Inventory

[Docs](../../../../README.md) / [Up](../README.md)

**Status:** capability snapshot; current, but not portability or runtime evidence

**Snapshot:** 2026-09-06 at committed `master` revision `8414b5dc`; `Engine/Platform` public/private source and CMake membership inspected; evidence `S` only

**Scope:** native window ownership, DPI, message pumping, Win32 input acquisition, layered routing, capture, and cursor control

**Owner:** `Engine/Platform` / `SparklePlatform`

**Evidence and disposition:** [Capability Evidence Plan](../../CapabilityEvidencePlan.md) and [First Release Acceptance Contract](../../../../Acceptance/FirstRelease.md)

**Supported host:** Windows only. Linux support is excluded from the selected roadmap and has no delivery plan.

**Current readiness:** **50/100** for the tracked Windows platform scope — Win32 window/input paths exist; lifecycle/runtime/package proof remains open and other host platforms are excluded. See [Current Feature Readiness](../../../../Acceptance/CurrentReadiness.md#foundation-world-content-shaders-and-tools).

## At A Glance

| Surface | Current implementation | Explicit gap |
| --- | --- | --- |
| host platform | one Win32 top-level window, message pump, size/state, borderless fullscreen, DPI handling | no Linux/macOS/headless or multi-window platform route |
| input acquisition | Win32 keyboard/mouse/wheel translation | no gamepad, touch, pen, IME, raw-device catalog, or hotplug |
| routing | immediate/deferred typed events across Gameplay/UI/System layers with capture and viewport bounds | focus loss, alt-tab, DPI/multi-monitor, and cancellation behavior unproved |
| cursor/window control | capture, visibility, centering, native drag/minimize/maximize/restore | public contracts expose Win32 types and are not yet portable |

The module separates native event acquisition from Core input vocabulary and Application/editor consumers. That keeps routing consistent, but the present public window seam still makes Windows the effective product platform.

## Platform Boundary

The present implementation is Windows-only: public `Window` exposes `HWND` and Win32 message types, the input backend is `Win32InputBackend`, the application manifest is Windows-specific, and `dwmapi` is linked privately. There is no Linux/macOS backend in this snapshot.

## Capability Surface

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `PLAT-001` | Native Win32 window | Implemented path | Registers one Sparkle window class, creates one top-level popup/resizable application window, extends DWM frame into client area, exposes its `HWND`, and fatals on registration/creation failure. | `S` |
| `PLAT-002` | Message pump | Implemented path | Non-blocking `PollEvents` drains the thread queue and synchronously broadcasts raw window messages; `WaitForEvent` blocks while minimized/idle until a message arrives. | `S` |
| `PLAT-003` | Size/state | Implemented path | Atomically published client width/height, valid-size check, and Normal/Minimized/Maximized/FullScreen state. Minimum interactive size is 320x240. | `S` |
| `PLAT-004` | Window controls | Implemented path | Request close, minimize, maximize, restore, maximize/restore toggle, borderless monitor fullscreen toggle, and native drag move. Startup is maximized; fullscreen remains an explicit runtime action. | `S` |
| `PLAT-005` | DPI awareness/events | Implemented path | Manifest plus per-window DPI query, scale relative to 96 DPI, DPI-change event, suggested-rect handling, and DPI-aware resize-border metrics. Multi-monitor behavior remains unrun. | `S` |
| `PLAT-006` | Window event contracts | Implemented path | Synchronous resized, DPI-scale-changed, and capacity-16 raw message events. Subscribers run on the polling thread and can mark raw messages handled. | `S` |
| `PLAT-007` | Win32 keyboard/mouse backend | Implemented path | Translates Windows messages into typed keyboard, mouse-button, move, vertical/horizontal wheel events and modifier state. No gamepad, touch, pen, IME composition, or raw-input device catalog was found. | `S` |
| `PLAT-008` | Per-frame input state | Implemented path | Global and per-layer pressed/released/held state, pointer position/delta, wheel accumulation, modifiers, capture, and cursor-hidden state reset at `BeginFrame`. | `S` |
| `PLAT-009` | Immediate/deferred dispatch | Implemented path | Subscribers choose typed event, Gameplay/UI/System layer, and immediate/deferred delivery. Deferred events are processed explicitly after the native message pump. | `S` |
| `PLAT-010` | Layer routing | Implemented path | Gameplay/UI/System enablement, active fallback layer, per-frame UI target rectangles, text-input/interactions-disabled policy, point/mouse-move targeting, and layer cancellation. | `S` |
| `PLAT-011` | UI capture integration | Implemented path | Host supplies a capture query (currently ImGui keyboard/mouse capture); routing uses it with registered viewport bounds to prevent UI/gameplay double-consumption. | `S` |
| `PLAT-012` | Mouse/cursor control | Implemented path | Capture/release, hide/show/visibility, center cursor, and capture-layer tracking. Focus-loss and alt-tab recovery require runtime evidence. | `S` |
| `PLAT-013` | Owner-thread enforcement | Implemented path | Registration and dispatch are guarded by Core `OwnerThread`; the module is not generally thread-safe. | `S` |

## Vertical Input Trace

Orchestration ownership reconciled on 2026-10-27: `Window::HandleMessage` retains
raw-event admission and native message dispatch. Private `WindowNonClient.cpp`
owns client-area adjustment, resize-border classification and tracking-size
constraints, including the existing default-window-procedure fallthrough.
`ProcessWin32InputMessage` selects focused keyboard, button, motion and wheel
decoders over the same result object. `InputSystem` sequences admission, pointer
delta, target selection and typed dispatch. `InputEventDispatcher.cpp` owns
non-template subscription removal, queue clearing and layer eligibility;
necessary typed dispatch templates remain in its private header. Window/input
state, callback and deferred-queue lifetime, public operations and event values
remain unchanged. This is source/build preservation evidence, not native window,
input, focus, capture, DPI or multi-monitor acceptance.

The same date's state-transition pass keeps fullscreen admission/selection in
`SetFullScreen` and native style/placement work in private enter/restore functions.
Startup directly retains the existing maximized show path; the constant-false
startup predicate and unreachable fullscreen branch are removed. Layer
cancellation sequences keyboard releases, mouse-button releases and final reset;
private release functions retain state updates before immediate callbacks and
the original pointer/modifier sampling order. Native calls, state publication,
callback lifetime and public operations are source-preserved. This adds no
interactive fullscreen, focus-loss or input-cancellation acceptance evidence.

Pointer-control ownership reconciled on 2026-10-27: private
`Input/Backends/Win32PointerControl` owns foreground mouse capture/release, cursor
display-counter loops and the cursor-centering coordinate round trip. It retains
no input state or native owner. `InputSystem` publishes logical capture/layer and
hidden state after the same native operations, and consumes the returned client
position for its existing last-pointer cache. Capture admission means a foreground
target existed and the request was issued; native success/error handling remains
unchanged. Public APIs, routing, first-move baseline policy and callback lifetime
are preserved. This boundary has source/build evidence only; desktop pointer,
focus-loss and capture recovery remain runtime evidence items.

Win32 sends a window message -> `Window::WindowProc` exposes it through `OnWindowMessage` -> `InputSystem` calls the private `ProcessWin32InputMessage` decoder -> global state is updated -> focus router selects System/UI/Gameplay using capture state and registered rectangles -> immediate subscribers run or an event is queued -> Application calls `ProcessDeferredEvents` -> camera/editor/console consumers receive only their enabled layer.

Input ownership rechecked on 2026-10-25: hosts construct `InputSystem` directly and consume its state, registration and routing operations. Dispatcher and focus-router lifetime share one private state allocation; native decoding is stateless. Backend results and the router remain Platform-private. No injected backend interface or forwarding factory remains. The window subscription is released before input state is destroyed. This source boundary does not qualify focus-loss, DPI or interactive behavior.

## Explicit Non-Capabilities And Risks

- No non-Windows window/input backend, multiple-window manager, headless platform host, gamepad, controller hotplug, clipboard, file dialog, drag-and-drop, accessibility API, touch, pen, or IME composition capability was found in `Engine/Platform`.
- Fullscreen is borderless-monitor fullscreen, not an exclusive display-mode API.
- `Window` directly exposes Win32 types; this is not an opaque cross-platform contract.
- Resize/DPI/focus/capture behavior needs paired normal, minimized, fullscreen, multi-monitor, alt-tab, and high-DPI execution evidence.
