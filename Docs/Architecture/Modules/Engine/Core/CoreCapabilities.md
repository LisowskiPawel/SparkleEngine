# Core Capability Inventory

[Docs](../../../../README.md) / [Up](../README.md)

**Status:** capability snapshot; current, but not release approval or runtime evidence

**Snapshot:** 2026-09-06 at committed `master` revision `8414b5dc`; `Engine/Core` public/private source and CMake membership inspected in the live working tree; evidence `S` only

**Scope:** dependency-free engine foundations: diagnostics, console/configuration, events, files, paths, process execution, serialization helpers, input vocabulary, math, pixels, time, and thread ownership

**Owner:** `Engine/Core` / `SparkleCore`

**Evidence and disposition:** [Capability Evidence Plan](../../CapabilityEvidencePlan.md) and [First Release Acceptance Contract](../../../../Acceptance/FirstRelease.md)

**Current readiness:** **50/100** — broad integrated source foundations exist; verification and delivery/adoption credit remain zero. See [Current Feature Readiness](../../../../Acceptance/CurrentReadiness.md#foundation-world-content-shaders-and-tools).

Source ownership reconciled on 2026-10-06: `CommandLine::ReadToken` owns the borrowed wide-token view and advancing offset used by Application CVar parsing and RHI backend selection. Core also owns the immutable default texture product catalog shared by Renderer and AssetCooker; cooking and runtime policy remain with those consumers.

Path ownership amendment (2026-11-01): package/installed discovery, manifest parsing, source-vs-bundle product paths and the release-version state branch are deleted from Core. Source and extracted products use the same markers and `artifacts/dev` layout. Core resolves ordinary inputs and workspace-keyed user state; `ToolOutputPaths` is tool-only. Publication guards are generic local-cook/read safety, unchanged by delivery. [Current source checks](../../../../../artifacts/validation/launcher-owned-delivery-20261101/completion.json) do not qualify a new archive.

**Preparation metadata boundary, 2026-11-01:** the remaining project/acquisition catalog model and its reader/validator/selection writer have moved to existing ToolSupport. Core contains no release/download/game-support or cook-selection policy. The existing architecture checker rejects reintroducing `ProjectLevelCatalog`, `ToolOutputPaths` or `LauncherProductPaths` into Engine. [Source result](../../../../../artifacts/validation/runtime-inputs-slim-20261101/completion.json) is separate from package/runtime-frame acceptance.

## At A Glance

| Foundation | Current role | Deliberate boundary |
| --- | --- | --- |
| diagnostics and console | logging/fatal/verify, typed CVars, commands, bounded interactive session | product hosts decide reachability; Core does not persist settings or define feature policy |
| files, paths, processes, serialization | validated paths, transactional publication helpers, bounded byte decoding, lightweight JSON, child execution | no general asset database, schema migration system, or package manager |
| events, input vocabulary, time, ownership | synchronous typed events, platform-neutral input state, timer, thread-role assertions | not an async bus, fixed-step simulation scheduler, or automatic thread safety |
| math, pixels, hashing/string tables | shared transforms, sampling helpers, linear/sRGB conversion, deterministic identifiers | consuming schemas own units, collision policy, formats, and numerical oracles |

Core keeps dependencies pointed downward by owning only reusable mechanisms. That reduces duplication, but it also means a higher-level feature must own every semantic policy, lifetime, capacity, and user-visible failure built from those mechanisms.

**Publication ownership amendment, 2026-11-01:** `Files/FileUtils.h` and `FilePublication.cpp` provide single-file finalization; `Files/FileSetPublication.h` and `FileSetPublication.cpp` own the publication record and coordinated reader/writer transaction. The file-set owner sequences validation, incomplete markers, replacement, backup retirement and rollback. Cookers supply staged files; runtime consumers hold read scopes. Splitting these responsibilities preserves the existing locking, commit and failure order and adds no artifact format or recovery path. This is an ownership description, not new runtime acceptance.

## Module Boundary

`SparkleCore` is the base engine library. It has no other engine dependency; it publicly carries the header-only `spdlog` contract and links Windows `bcrypt` privately. It can be static or shared. Logging compile level is Trace in Debug, Info in Development, and Warning in Shipping profiles.

## Capability Surface

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `CORE-001` | Named diagnostics | Implemented path | Process-wide `spdlog` bootstrap, lightweight declared `LogCategory` frontend, Core-private named logger creation/lifetime, active log-file discovery, game level control, fatal/verify helpers, and debugger break-if-attached. Fatal paths are process-terminating policy, not recoverable errors. | `S` |
| `CORE-002` | Console-variable registry | Implemented path | Statically registered typed CVars support bool, integral, floating, enum-as-number, and string values. Lookup and string mutation report parse errors. Enum owners can define a pure `IsValidConsoleVariableValue` overload, found by argument-dependent lookup, to reject unsupported numeric values before mutation; Core retains no feature-specific rules or validator registry. There is no persistence layer in Core. | `S` |
| `CORE-003` | Console command/session | Moved to Editor | Interactive registration, parsing, completion, history and session storage are private to Editor. Core retains the shared output/result values and CVar control consumed by the Editor host and shader actions. | `S` |
| `CORE-004` | Event dispatch | Implemented path | Fixed-capacity typed synchronous events return handles; `ScopedEventHandle` removes a subscription on destruction. This is same-thread callback dispatch, not an asynchronous message bus. | `S` |
| `CORE-005` | Whole-file I/O | Implemented path | Binary/text reads and writes, UTF-8 read-error paths, temporary paths, close/finalize helpers, atomic text write, and multi-file publication with cleanup. Atomicity/durability across crash or hostile filesystems is not runtime-proven. | `S` |
| `CORE-006` | Bounded binary decode/encode | Implemented path | Span reader checks remaining bytes and multiplication overflow; buffer/stream writers cover trivially-copyable values/arrays, raw bytes, and UInt32-length strings. These helpers do not provide schema evolution by themselves. | `S` |
| `CORE-007` | Lightweight JSON helpers | Partial | Escaped JSON writing and targeted property/string/unsigned/hex reads exist. This is not a general DOM, schema validator, or complete JSON parser. | `S` |
| `CORE-008` | Workspace/project path model | Implemented path | Executable/workspace/project/engine markers, cooked product paths and workspace-keyed user state. Core never reads a release manifest, switches installed mode, discovers a compiler or creates cooked directories during path discovery. Build/library/tool output grammar lives in tool support. | `S` |
| `CORE-009` | Cooked product path resolution | Implemented path | Runtime texture lookup accepts existing `.stex` files at absolute paths or under the cooked texture/asset roots; it never searches raw source assets. Scene/material/mesh/skeleton/animation product paths remain shared with their cookers. Source-family lookup, import-cache paths and shader-dependency-manifest paths belong to ToolSupport. | `S` |
| `CORE-010` | Child-process execution | Implemented path | Synchronous Win32 child launch with argv, working directory, environment overrides, output capture/callback, log path, cancellation token, exit code, and explicit launch/failure state. Native start failures classify missing executable/access denial/application-policy rejection without localized-text matching and expose Windows messages as UTF-8. Process-tree termination and package execution still need evidence. | `S` |
| `CORE-011` | Environment and command-line utilities | Implemented path | String/bool/UInt environment access and UTF-8/wide conversion support tool and host boundaries. One Windows argument encoder serves child execution, Launcher previews and shader-command text, including embedded quotes and trailing backslashes. | `S` |
| `CORE-012` | Project level/acquisition catalog | Moved to ToolSupport | Preparation-only metadata/parser/editor are outside Core. Launcher and Cooker consume the same model; runtime discovers ordinary level documents and consumes cooked data. This retained ID describes the former ownership, not an Engine dependency. | `S` |
| `CORE-013` | Input vocabulary/state | Implemented path | Key, mouse button, modifier, device, dispatch-layer/mode and typed events; pressed/released/held polling, per-frame pointer/wheel accumulation, modifiers, capture and cursor state. Polling helpers and pixel/normalized/NDC conversion helpers remain available. OS acquisition and layered routing belong to Platform. | `S` |
| `CORE-014` | Math and coordinate helpers | Implemented path | DirectXMath-based transforms/normalization/frustum utilities, declared world-coordinate convention, and Bessel/sinc/Kaiser functions used by filtering. This is not a general numerics library. | `S` |
| `CORE-015` | Pixel conversions | Implemented path | Float conversion plus linear/sRGB byte encode/decode helpers. RHI format support and texture cooking own storage-format decisions. | `S` |
| `CORE-016` | Timer | Implemented path | Per-frame raw/scaled delta and total time, frame count, time scale, pause/resume, and seconds/milliseconds conversion. No fixed-step simulation scheduler exists here. | `S` |
| `CORE-017` | Thread ownership assertions | Implemented path | Threads can receive diagnostic roles; `OwnerThread` captures an owner and asserts access. This detects contract violations in instrumented paths; it does not make objects thread-safe. | `S` |
| `CORE-018` | Stable hashing/string tables | Implemented path | Hash helpers, string escaping/parsing, and deterministic string-table construction support cooked identifiers and manifests. Collision policy remains the consuming schema's responsibility. | `S` |

## Vertical Traces

### Transactional file publication

Cooking declares the output directories and complete staged file set. Core provides fail-fast read/write scopes, containment checks and checked in-process rollback. Writers mark output incomplete before replacement and clear the marker only after successful publication or ordinary rollback. A killed publisher or failed cleanup leaves generated output unusable: clean it and regenerate it from source. There is no persisted transaction log, checksum parser, file-identity recovery or cross-directory recovery protocol.

GameFramework holds one read scope from fresh catalog lookup through mapping/decoding and worker settlement. A reader refuses an incomplete marker; cooperative writers cannot change that domain during the load. OS handle lifetime releases the lock. Source assets and user settings are outside this disposable-output boundary. [Cooking owns the minimal recovery contract](../../Tools/Cooking/CookingCapabilities.md#interrupted-scene-publication); [FWC-4 results](../../../../Acceptance/FeatureCompletionReports.md#foundation-contract-progress) distinguish the current marker controls from superseded journal-replay evidence. This is Windows process-interruption handling, not power-loss recovery or archive acceptance.

### Configuration entry

Static `ConsoleVariable<T>` construction registers with the singleton registry -> command-line or console host resolves the name -> string parsing mutates the typed value -> Application/Renderer/RHI reads it at its chosen boundary. Core provides no persistence, range schema, restart policy, or UI ownership; those remain with the consumer.

Console ownership reconciled on 2026-10-26: `CVarControl.cpp` sequences batch
resolution/validation, assignment and result collection; private
`CVarControlOperations.cpp` owns those mechanisms and preserves validation of
the whole batch before mutation. Editor-private `Console/ConsoleBuiltinCommands.cpp` declares the four built-ins;
its execution and completion use the supplied `CVarControlExecutor`. Shared
`ConsoleOutput` records/results stay in Core; command/session policy is described
in the [Editor boundary](../Editor/EditorCapabilities.md#ui-implementation-boundary).

### Child-process execution

Process relaunch ownership reconciled on 2026-11-01: `Process::RelaunchCurrentProcess` starts the current executable with the unchanged native command line and current working directory, then releases its process/thread handles. It does not wait for readiness or close the caller. Application owns that close decision after successful launch. Reusing the original command line removes Editor's duplicate argument parser/encoder; this is a launch contract, not session restoration or startup-health evidence.

`Process::RunChildProcess` is the direct process contract used by tool and host
clients. `ChildProcess.cpp` selects the platform route and
`ChildProcessWindows.cpp` selects captured completion or readiness. Private
`ChildProcessWindowsLaunch.cpp` owns Windows handle lifetime, command/environment
lowering, suspended launch and job ownership; `ChildProcessWindowsOutput.cpp`
owns pipe setup and output draining; `ChildProcessWindowsReadiness.cpp` owns the
durable readiness channel, validation and ownership transfer. The wait loops
remain cohesive state machines, with cancellation callbacks destroyed before
their event handles. This 2026-10-26 source decomposition preserves the result
contract and does not qualify package launch or clean-machine behavior.

### Logging category ownership

A receiver declares a stable category with `SPARKLE_DEFINE_LOG_CATEGORY_STATIC`
in its implementation file, or pairs `SPARKLE_DECLARE_LOG_CATEGORY` in a
shared header with one `SPARKLE_DEFINE_LOG_CATEGORY` definition. Receivers log
through that category and do not cache `shared_ptr<spdlog::logger>` instances or
create registry entries. `SparkleCore` alone owns lazy category resolution,
registry synchronization, sinks, levels, and logger lifetime. This keeps the
frontend declarative while preserving one backend policy and one creation path.
Logger initialization sequences private `LoggerBootstrap` operations for
configured severity and sink construction before registering and publishing the
core logger. Formatting, rotating-file creation and Windows debugger output
remain with that bootstrap implementation, which writes the registry's active
file path through a reference and retains no registry state.
Bootstrap follows `Logger -> LoggerBootstrap -> LogPathPolicy -> stateless
user-state and filesystem discovery`. The bootstrap and path policy must not
call the logger registry or aggregate path
getters; the public `Paths::LogFile` function delegates to the same policy. This
allows aggregate path initialization to report diagnostics without re-entering
itself. These private boundaries were reconciled on 2026-10-26;
`architecture_boundary_check` rejects dependency reversals.

### Input frame state

Platform's `InputSystem` advances its system and layer states through
`InputState::BeginFrame`. Core sequences keyboard advancement, mouse-button
advancement and transient-delta reset; both device families use the same private
button-transition operation over borrowed spans. Position, modifiers, capture
and cursor state persist across this boundary. This 2026-10-26 decomposition
adds no input policy or state ownership to Core.

### Workspace-derived paths

Executable/current directory and markers -> workspace/project discovery -> `ConfigureProjectRoot` publishes the active project -> typed getters derive Engine/Project/cooked paths -> runtime/tools open those products. This is a vital package risk because a successful workspace path does not prove operation outside the repository.

Path ownership reconciled on 2026-11-01: public `Paths/FileSystemPaths.h` declares process/workspace/cooked path operations; `Paths/ProductFilePaths.h` builds product/log file paths. `FileSystemPathState.cpp` owns cached root/layout initialization. `FileSystemPathRoots.cpp` uses ordinary marker discovery, `CookedAssetPathLayout.cpp` composes the same cooked suffixes for every root, and loaders validate the products they consume. Discovery never materializes cooked output. `FileSystemDiscovery.cpp` and `UserStatePaths.cpp` remain stateless bootstrap owners. The package discovery/manifest/product-path files and compiler-specific symbol/signal getters are deleted. Host-tool `ToolOutputPaths` owns build/library/tool layout; Launcher owns source-action admission and executable selection. No Engine component depends on Launcher.

## Explicit Non-Capabilities And Risks

- No networking, sockets, plugin loader, reflection/type system, general archive/container format, database, allocator framework, or cross-platform process implementation was found in this module.
- `Windows.h`, DirectXMath, Win32 process code, and private `bcrypt` make the current Core surface Windows-oriented despite otherwise portable helpers.
- CVar assignment is mutable global state and not synchronized or persisted by Core.
- `Files::BinarySpanReader` owns cursor advancement, bounds, multiplication overflow, and borrowed raw array bytes. GameFramework's cooked reader translates failures into domain exceptions around that same cursor; it does not implement another byte decoder. The owning cooked schema still validates magic, counts, cross-references, and semantic invariants. This source boundary was rechecked on 2026-10-06.
- Reader surface rechecked on 2026-10-25: non-template decoding lives in Core's matching source file. The unused typed-array reinterpretation and skip APIs are removed; consumers borrow raw bytes or copy decoded values without requiring aligned file storage. Cooked representations are unchanged.
- Source presence does not establish crash-safe publication, Unicode/path edge behavior, privilege behavior, or packaged path correctness.
