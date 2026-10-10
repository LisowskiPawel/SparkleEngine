# First Release Capability Dispositions

**Status:** scope ledger under the approved product choices; technical closure/native evidence remains required; no capability receives acceptance evidence here

**Responsibility:** give every existing module inventory row one release classification and an owning module, then split compound user surfaces explicitly

**Parent decisions:** [Release Scope Record](README.md). **Requirement authority:** [First Release](../FirstRelease.md). **Source meaning:** [Module inventory](../../Architecture/Modules/README.md).

## Interpretation

Included is an obligation to deliver and accept the exact bounded path, not a statement that it works. Experimental still needs correctness within its declared limits and safe failure. Excluded has no first-release promise. No row is Removed. The Exposure column restricts distribution: Source only does not enter a shipped component; Game/source covers Game and shared product mechanics; Editor/Launcher/source covers admitted shipped-tool workflows. The release-owner amendment includes Launcher and Editor, seven selectable examples plus built-in Empty, and only the features required by those release paths. Other choices are development or unavailable. Internal mechanisms do not become standalone public features or a stable SDK.

The capability name and source link retain the existing inventory owner instead of copying its implementation narrative. A section names exactly one module owner; cross-module feature/report ownership remains in the existing FCR registry. Compound rows are split in the public-surface tables below. Old inventory evidence remains dated; a classification does not refresh all of its source assertions.

No fixed function, data format or hidden low-level flag is a new public selector. Generic development CVar/CLI access is absent from Game; the Game settings allowlist below is exhaustive. Editor/Launcher use their classified typed workflows; this does not admit every development flag or debug-build feature. A new advertised/reachable consumer choice requires a row before scope approval.

## Module Coverage

| Module / disposition owner | Rows | Inventory |
| --- | ---: | --- |
| Application / Application | 16 | [Inventory](../../Architecture/Modules/Engine/Application/README.md) |
| Build and packaging / Build/package | 21 | [Inventory](../../Architecture/Modules/BuildAndPackaging/README.md) |
| Core / Core | 18 | [Inventory](../../Architecture/Modules/Engine/Core/README.md) |
| Editor / Editor | 24 | [Inventory](../../Architecture/Modules/Engine/Editor/README.md) |
| Engine assets / Assets/content | 5 | [Inventory](../../Architecture/Modules/Engine/Assets/README.md) |
| GameFramework / GameFramework | 26 | [Inventory](../../Architecture/Modules/Engine/GameFramework/README.md) |
| Launcher / Launcher | 22 | [Inventory](../../Architecture/Modules/Tools/Launcher/README.md) |
| Cooking / Cooking | 22 | [Inventory](../../Architecture/Modules/Tools/Cooking/README.md) |
| Platform / Platform | 13 | [Inventory](../../Architecture/Modules/Engine/Platform/README.md) |
| RHI / RHI | 85 | [Inventory](../../Architecture/Modules/Engine/RHI/CapabilityInventory.md) |
| Renderer / Renderer | 144 | [Inventory](../../Architecture/Modules/Engine/Renderer/CapabilityInventory.md) |
| ShaderCompiler / ShaderCompiler | 69 | [Inventory](../../Architecture/Modules/Tools/ShaderCompiler/README.md) |
| Showcase / Showcase | 7 | [Inventory](../../Architecture/Modules/Projects/Showcase/README.md) |
| SourceImporters / SourceImporters | 19 | [Inventory](../../Architecture/Modules/Tools/SourceImporters/README.md) |
| Tasks / Tasks | 14 | [Inventory](../../Architecture/Modules/Engine/Tasks/README.md) |
| ToolSupport / ToolSupport | 7 | [Inventory](../../Architecture/Modules/Tools/ToolSupport/README.md) |

## Application

**Single row owner:** Application. [Existing source scope and limits](../../Architecture/Modules/Engine/Application/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `APP-001` | Shared application lifecycle | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-002` | Cooked game host | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-003` | Editor host | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `APP-004` | Build-time editor erasure | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-005` | Frame pump | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-006` | Task ownership | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-007` | Renderer execution selection | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-008` | Camera input bridge | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-009` | Viewport request/products | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-010` | Ordered shutdown | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `APP-011` | Command-line CVar assignment | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `APP-012` | Game console | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `APP-013` | Editor shader recook | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `APP-014` | Editor operation service | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `APP-015` | Viewport capture | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `APP-016` | Rendering-settings persistence | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |

## Build and packaging

**Single row owner:** Build/package. [Existing source scope and limits](../../Architecture/Modules/BuildAndPackaging/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `BUILD-001` | CMake baseline | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-002` | Six profiles | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-003` | Profile optimization/debug policy | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-004` | Compiler routes | Included | Game/source | Only the pinned MSVC toolchain is promised; alternate compiler routes remain unqualified. |
| `BUILD-005` | Static/shared engine modules | Included | Game/source | Use the selected product linkage; no binary SDK or stable exported ABI. |
| `BUILD-006` | Optional tool/features | Included | Game/source | Only classified options and exact eligible profiles; new options reopen scope. |
| `BUILD-007` | Project discovery | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-008` | Target layering | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-009` | Host-tool exclusion | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-010` | Dependency acquisition | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-011` | Selective dependency sync | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-012` | Development artifact contract | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-013` | Product layout | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-014` | Runtime support staging | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-015` | Tool bundles | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-016` | Architecture check | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-017` | Code-style targets | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `BUILD-018` | Install/stage/package | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `BUILD-019` | Automated tests | Excluded | No release promise | No new permanent test framework in this scope; existing/future claim-specific checks do not inherit this exclusion. |
| `BUILD-020` | CI | Excluded | No release promise | No hosted CI service promise; reproducible build/verification remains required. |
| `BUILD-021` | Root onboarding | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |

## Core

**Single row owner:** Core. [Existing source scope and limits](../../Architecture/Modules/Engine/Core/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `CORE-001` | Named diagnostics | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-002` | Console-variable registry | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-003` | Console command/session | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `CORE-004` | Event dispatch | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-005` | Whole-file I/O | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-006` | Bounded binary decode/encode | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-007` | Lightweight JSON helpers | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-008` | Workspace/project path model | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-009` | Typed asset path resolution | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-010` | Child-process execution | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `CORE-011` | Environment and command-line utilities | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-012` | Project level catalog | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-013` | Input vocabulary/state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-014` | Math and coordinate helpers | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-015` | Pixel conversions | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-016` | Timer | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-017` | Thread ownership assertions | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `CORE-018` | Stable hashing/string tables | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |

## Editor

**Single row owner:** Editor. [Existing source scope and limits](../../Architecture/Modules/Engine/Editor/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `ED-001` | Fixed workspace shell | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-002` | Native window controls | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-003` | Level open/save | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-004` | Level progress/failure gating | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-005` | Scene outliner | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-006` | Camera inspector | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-007` | Light inspector | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-008` | Mesh inspector | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-009` | Sky inspector | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-010` | Material variant selector | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-011` | Undo/redo | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-012` | Stale edit protection | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-013` | Editor viewport | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-014` | View modes | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-015` | Exposure overrides | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-016` | Rendering settings | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-017` | Restart service | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-018` | Viewport capture | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-019` | Editor console | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-020` | Shader tools | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-021` | Mesh diagnostics | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-022` | Texture diagnostics | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-023` | Memory/renderer diagnostics | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `ED-024` | Icon/theme assets | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |

## Engine assets

**Single row owner:** Assets/content. [Existing source scope and limits](../../Architecture/Modules/Engine/Assets/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `EASSET-001` | Shader entry sources | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `EASSET-002` | Shader includes/contracts | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `EASSET-003` | Default textures | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `EASSET-004` | Sky environments | Included | Game/source | Only a rights-cleared dependency of ReleaseMapSet may enter the runtime package; do not ship the entire sky corpus. |
| `EASSET-005` | Cube fixtures | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |

## GameFramework

**Single row owner:** GameFramework. [Existing source scope and limits](../../Architecture/Modules/Engine/GameFramework/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `GF-001` | Level documents | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-002` | Level registry/session | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-003` | Asynchronous scene load | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-004` | Cooked-only runtime | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-005` | Cooked format identification | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-006` | Scene registry | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-007` | File lifetime during load | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-008` | Manifest validation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-009` | Entity identity/lifetime | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-010` | Fixed component schema | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-011` | Typed queries and frozen structure | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-012` | Compiled system graph | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-013` | Parallel world evaluation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-014` | Transform hierarchy evaluation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-015` | Camera simulation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-016` | Lighting model | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-017` | Material model and variants | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-018` | Static/instanced meshes | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-019` | Skeletal animation | Included | Game/source | Cesium Man skeletal animation is an Included example; exact clip and pose require cook/runtime proof. |
| `GF-020` | Morph animation | Included | Game/source | Existing source morph capability; no consumer morph demonstration or arbitrary animation-format promise. |
| `GF-021` | World edits | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `GF-022` | Read snapshots/change journal | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-023` | Structural publication | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-024` | Dynamic publication | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-025` | View publication | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `GF-026` | Stable render identity | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |

## Launcher

**Single row owner:** Launcher. [Existing source scope and limits](../../Architecture/Modules/Tools/Launcher/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `LAUNCH-001` | Qt GUI | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-002` | Shell route | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-003` | Immutable operation plan | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-004` | Quick Start dependency graph | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-005` | Background operation service | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-006` | Repository/content discovery | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-007` | Toolchain detection | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-008` | Source dependency sync | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-009` | Build-file generation | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-010` | Freshness diagnosis | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-011` | Workspace build | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-012` | Launcher self-build | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-013` | Host-tool install | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-014` | Level sync | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-015` | Cook workspace | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-016` | Focused/full cooks | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-017` | Force recook safety | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-018` | Run level | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-019` | Build profiles | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-020` | Graphics API choice | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-021` | Clean workspace | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `LAUNCH-022` | Logs/recovery | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |

## Cooking

**Single row owner:** Cooking. [Existing source scope and limits](../../Architecture/Modules/Tools/Cooking/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `COOK-001` | Project cook CLI | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-002` | Project discovery | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-003` | Capability preflight | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-004` | Shader stage delegation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-005` | Texture request planning | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-006` | Scene generation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-007` | Tool/output reporting | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-008` | Source texture formats | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-009` | Texture dimensions | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-010` | Mip policy | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-011` | Channel extraction | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-012` | Semantic format policy | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-013` | Parallel batch | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-014` | Memory bound | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-015` | Generation publication | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-016` | Request inspection | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-017` | Mesh | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-018` | Material | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-019` | Scene manifest | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-020` | Skeleton | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-021` | Animation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `COOK-022` | Registry | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |

## Platform

**Single row owner:** Platform. [Existing source scope and limits](../../Architecture/Modules/Engine/Platform/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `PLAT-001` | Native Win32 window | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-002` | Message pump | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-003` | Size/state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-004` | Window controls | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-005` | DPI awareness/events | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-006` | Window event contracts | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-007` | Win32 keyboard/mouse backend | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-008` | Per-frame input state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-009` | Immediate/deferred dispatch | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-010` | Layer routing | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-011` | UI capture integration | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-012` | Mouse/cursor control | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `PLAT-013` | Owner-thread enforcement | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |

## RHI

**Single row owner:** RHI. [Existing source scope and limits](../../Architecture/Modules/Engine/RHI/CapabilityInventory.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `RHI-BACK-01` | Common public RHI | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BACK-02` | Diagnostics implementation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BACK-03` | D3D12 backend | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BACK-04` | Vulkan backend | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BACK-05` | Backend selection | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BACK-06` | Backend parity | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-01` | API/version identity | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-02` | Shader binary | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-03` | Queue kinds | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-04` | Synchronization | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-05` | Dynamic rendering | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-06` | Descriptor indexing | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-DEV-07` | Mesh shaders | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `RHI-DEV-08` | Task shaders | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `RHI-DEV-09` | Adapter preference | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-LIFE-01` | Aggregate service ownership | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-LIFE-02` | Atomic creation/publication | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-LIFE-03` | Ordered settlement and destruction | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-LIFE-04` | Swapchain recovery | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-LIFE-05` | Device-loss diagnostics | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-LIFE-06` | In-process device recreation | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `RHI-RES-01` | 2D textures | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-02` | Buffers | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-03` | Resource views | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-04` | Upload | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-05` | Readback | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-06` | Persistent allocation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-07` | Transient allocation and aliasing | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-08` | Residency pressure | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RES-09` | Samplers | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-FMT-01` | Float/color | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-FMT-02` | 8-bit color | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-FMT-03` | Depth/stencil | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-FMT-04` | BC color | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-FMT-05` | BC scalar/vector | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-FMT-06` | BC HDR | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-01` | Bindful descriptor sets/tables | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-02` | Fixed descriptor arrays | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-03` | Non-uniform indexing | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-04` | Partially-bound arrays | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-05` | Renderer material texture table | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-06` | Raster-material coverage | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-BIND-07` | Runtime-sized bindless | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `RHI-PIPE-01` | Graphics pipeline | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-02` | Primitive topology | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-03` | Vertex inputs | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-04` | Index formats | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-05` | Raster state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-06` | Depth/stencil | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-07` | Blending | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-08` | Compute pipeline | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PIPE-09` | Geometry/hull/domain stages | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `RHI-PIPE-10` | Mesh/task shaders | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `RHI-CMD-01` | Recording leases | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-CMD-02` | Raster commands | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-CMD-03` | Compute commands | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-CMD-04` | Ray commands | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-CMD-05` | Copy commands | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-CMD-06` | Explicit state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-CMD-07` | Diagnostic scopes | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-01` | Bottom-level AS | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-02` | BLAS update/refit | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-03` | Classic TLAS | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-04` | Inline ray query | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-05` | Native RT pipeline | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-06` | Shader table | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RT-07` | Triangle hit groups | Included | Game/source | Triangle geometry only; procedural/intersection/callable generality is excluded. |
| `RHI-RT-08` | Partitioned TLAS | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `RHI-RTC-01` | Ray-traced GBuffer | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RTC-02` | Direct-shadow visibility | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-RTC-03` | Reference path-traced direct/indirect | Included | Game/source | Required native lowering for full admitted reference transport; no finite-depth diagnostic substituted as oracle. |
| `RHI-RTC-04` | ReSTIR indirect temporal/spatial/resolve | Included | Game/source | Required native execution of accepted realtime indirect paths/reuse; paired backend correctness and bounded costs. |
| `RHI-PRES-01` | Swapchain | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PRES-02` | VSync | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PRES-03` | Frame pacing | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PRES-04` | Back-buffer commands | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `RHI-PRES-05` | HDR presentation | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `RHI-DIAG-01` | Object naming and GPU events | Included | Game/source | Native names/markers retained as mechanics; capture SDK/event-runtime package inclusion is separately allowlisted. |
| `RHI-DIAG-02` | Timestamp queries | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `RHI-DIAG-03` | Validation messages | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `RHI-DIAG-04` | D3D12 crash diagnostics | Included | Game/source | Local actionable device-loss diagnostics; no automatic upload or crash SDK. |
| `RHI-DIAG-05` | Live-object reporting | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `RHI-DIAG-06` | Texture capture | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `RHI-DIAG-07` | External provider handles | Experimental | Game/source | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `RHI-DIAG-08` | ImGui backend | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |

## Renderer

**Single row owner:** Renderer. [Existing source scope and limits](../../Architecture/Modules/Engine/Renderer/CapabilityInventory.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `REN-OWN-01` | Public renderer facade | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-OWN-02` | Scene ownership | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-OWN-03` | View ownership | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-OWN-04` | Frame ownership | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-OWN-05` | Threaded or serial execution | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-OWN-06` | Renderer/RHI boundary | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-01` | Typed graph authoring | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-02` | Compiled pass kinds | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-03` | Resource model | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-04` | Dependency compilation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-05` | Transient aliasing | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-06` | Multi-queue submission | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-07` | Parallel command recording | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FG-08` | Graph rebuild and retirement | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PIPE-01` | Typed pass parameter contract | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PIPE-02` | Runtime shader and binding-layout validation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PIPE-03` | Graphics and compute materialization | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PIPE-04` | Native ray-pipeline and table materialization | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PIPE-05` | Atomic shader-generation replacement | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DIAG-08` | Shader hot reload | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-SCENE-01` | Persistent GPU scene | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-02` | Parallel scene preparation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-03` | Static meshes | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-04` | Skeletal meshes | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-05` | Morph targets | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-06` | Ray-traced deforming geometry | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-07` | Static BLAS reuse | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-08` | Mesh residency | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-09` | Texture residency | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SCENE-10` | Automatic mesh batching | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-01` | Per-view frustum visibility | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-02` | Material visibility classification | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-03` | Raster candidate validation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-04` | Authored/preserved groups | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-05` | Opaque sorting and auto batching | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-06` | Transparent draw preparation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-07` | Visibility task/failure bounds | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VIS-08` | Visibility/batch workload facts | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-VIS-09` | Advanced visibility/draw generation | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-TEMP-01` | Per-view previous-camera state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-TEMP-02` | Active temporal jitter | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-TEMP-03` | Common history invalidation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-TEMP-04` | Motion and reprojection convention | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-TEMP-05` | Provider temporal constants | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RESO-01` | Output extent resolution | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RESO-02` | Render extent resolution | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RESO-03` | Extent-driven topology and history | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RESO-04` | Active raster sample count | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RESO-05` | Renderer MSAA | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-RESO-06` | Standalone post-process AA | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-RESO-07` | Dynamic resolution | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-MAT-01` | Base color | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-02` | Tangent-space normal | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-03` | Roughness | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-04` | Metallic | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-05` | Ambient occlusion | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-06` | Dielectric F0 | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-07` | Emissive | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-08` | Subsurface | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-09` | Alpha mask | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-MAT-10` | Double-sided | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-01` | Base color | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-02` | Normal | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-03` | Material | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-04` | Emissive | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-05` | Subsurface | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-06` | Motion vector | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-07` | Device depth | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-GBUF-08` | Scene depth | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FRONT-01` | Raster GBuffer | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FRONT-02` | Ray GBuffer: inline | Included | Game/source | Required traced surface frontend for realtime/native parity; complete owned hit/material/traversal contract. |
| `REN-FRONT-03` | Ray GBuffer: native pipeline | Included | Game/source | Required traced surface frontend for realtime/native parity; complete owned hit/material/traversal contract. |
| `REN-FRONT-04` | Automatic ray execution | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-FRONT-05` | Transparent blending | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-FRONT-06` | Wireframe | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-FRONT-07` | Material binding mode selector | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-PBR-01` | Active direct specular BRDF | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-02` | Active direct diffuse BRDF | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-03` | Active subsurface approximation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-04` | Alternate BRDF implementations | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-PBR-05` | Indirect approximation helpers | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-06` | Directional lights | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-07` | Point lights | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-08` | Spot lights | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-09` | Rect lights | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-PBR-10` | Non-ray lighting fallback | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-LGT-01` | Source-named ReSTIR lighting route | Included | Game/source | Required direct/composite and realtime transport; actual estimator correctness must pass its owner contract, not ReSTIR naming. |
| `REN-LGT-04` | Reference Path Tracer | Included | Game/source | Required consumer progressive SurfaceTransportReference and raw export; accepted domain, independent oracle, uncertainty and invalid-sample proof. |
| `REN-LGT-05` | Accumulation invalidation | Included | Game/source | Required progressive/realtime reset and accumulation identity; no stale generations or invalid-sample advancement. |
| `REN-LGT-06` | Lighting composite | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-LGT-02` | Direct shadow visibility | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-LGT-03` | Source-named ReSTIR indirect prototype | Included | Game/source | Required realtime transport/reuse closure under the 30 FPS preset; current seed-replay prototype is not accepted path tracing. |
| `REN-LGT-07` | Sky | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-VOL-01` | Participating media and fog volumes | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-VOL-02` | Volumetric light transport | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-VOL-03` | Atmosphere and aerial perspective | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-DECAL-01` | Authored and scene decal data | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `REN-DECAL-02` | Primary deferred GBuffer composition | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `REN-DECAL-03` | Secondary-ray decal evaluation | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `REN-RT-01` | Classic TLAS | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RT-02` | Partitioned TLAS | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `REN-RT-03` | Shared scene identity | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RT-04` | Shader-table plan | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RT-05` | Hit-group coverage | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-RT-06` | Plan invalidation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-01` | Manual exposure | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-02` | Automatic exposure | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-03` | Async exposure | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-04` | Linear upscaler | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-05` | NVIDIA DLSS Super Resolution | Included | Game/source | Required complete DLSS SR quality/lifecycle/package/input contract on both D3D12 and Vulkan; fallback cannot pass the provider cell. |
| `REN-POST-06` | NVIDIA DLSS Ray Reconstruction | Included | Game/source | Required complete RR guide/extent/frame/reset/quality contract on both APIs; SDK-supported composition must be frozen before performance runs. |
| `REN-POST-07` | Tone mapping | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-11` | Color grading | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `REN-POST-12` | Chromatic aberration | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `REN-POST-14` | Basic vignette | Included | Game/source | Required basic lens example; independent AC-VIG contract under FCR-REN-25, neutral-work erasure, both-backend formula/order/package proof. |
| `REN-POST-13` | Frame generation | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `REN-POST-08` | Output encoding | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-POST-09` | HDR display output | Included | Game/source | Required delivery obligation; source absence/partial path cannot accept this row. |
| `REN-POST-10` | Exact debug presentation | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DBG-01` | Final/material | Included | Source only | Lit is consumer Included; Wireframe source-only. This compound row is split in the view-mode table. |
| `REN-DBG-02` | GBuffer | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DBG-03` | Lighting | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DBG-04` | Scene diagnostics | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-UI-01` | UI render packets | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-UI-02` | Host overlay | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-UI-03` | Editor viewport presentation | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-UI-04` | Editor texture handles | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-LAT-01` | Logical frame latency markers | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-LAT-02` | Streamline PCL marker route | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `REN-LAT-03` | Reflex simulation sleep | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `REN-LAT-04` | Provider call/shutdown lifetime | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `REN-LAT-05` | Frame-token identity | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `REN-DIAG-01` | Frame/pass diagnostics | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DIAG-02` | Mesh diagnostics | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DIAG-03` | Texture diagnostics | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DIAG-04` | Memory diagnostics | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DIAG-05` | Viewport products | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-DIAG-06` | Async captures | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-DIAG-07` | Mesh preview | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `REN-SET-01` | Aggregate public settings state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SET-02` | Application-owned rendering-settings persistence | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `REN-SET-03` | Startup and editor commit | Included | Game/source | Game startup and Editor settings commit Included; keep Editor-only dependency isolation in the Game component. |
| `REN-SET-04` | Serial/threaded settings handoff | Included | Game/source | Existing serial/threaded handoff retained; no public topology or concurrency tuning promise. |
| `REN-SET-05` | Live versus restart-active state | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |

## ShaderCompiler

**Single row owner:** ShaderCompiler. [Existing source scope and limits](../../Architecture/Modules/Tools/ShaderCompiler/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `SHD-BUILD-01` | Offline compiler executable | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-BUILD-02` | Shared shader contracts | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-BUILD-03` | Contract-only registrations | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-BUILD-04` | Runtime shader source dependency | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-BUILD-05` | Tool dependencies | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CLI-01` | `cook` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CLI-02` | `list-backends` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CLI-03` | `list-targets` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CLI-04` | `list-shaders` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CLI-05` | `inspect-shader` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-01` | Selection | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-02` | Targets | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-03` | Backend | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-04` | Parallelism | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-05` | Diagnostics | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-06` | Compiler policy | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-OPT-07` | Cancellation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-TGT-01` | DXC | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-TGT-02` | Slang | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-01` | Compute | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-02` | Vertex | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-03` | Pixel | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-04` | Ray generation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-05` | Miss | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-06` | Closest hit | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-07` | Any hit | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-CAT-08` | Geometry | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-CAT-09` | Hull | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-CAT-10` | Domain | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-CAT-11` | Intersection | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-CAT-12` | Callable | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-REG-01` | Typed global registration | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-REG-02` | Registry validation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-REG-03` | Parameter-structure contract | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-REG-04` | Ray contract | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-REG-05` | Runtime registration closure | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PLAN-01` | Virtual shader roots | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PLAN-02` | Include preprocessing | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PLAN-03` | Immutable jobs | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PLAN-04` | Changed-source planning | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PLAN-05` | In-operation deduplication | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PLAN-06` | Persistent result cache | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `SHD-PLAN-07` | Parallel compilation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-01` | DXIL generation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-02` | SPIR-V generation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-03` | DXIL reflection | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-04` | SPIR-V reflection | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-05` | Resource reflection | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-06` | Constant data | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-07` | Stage inputs | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-08` | Specialization constants | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-09` | C++ parameter ABI validation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-ABI-10` | Vulkan descriptor normalization | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-01` | `GlobalShaderMap.smap` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-02` | `CookedShaderLibrary.slib` | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-03` | Shared publication hash | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-04` | Staged validation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-05` | Atomic file-set publish | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-06` | Deterministic identity | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-PUB-07` | Incremental preservation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-01` | DXC debug artifacts | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-02` | Slang debug artifacts | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-03` | Cook analysis | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-04` | Cooked inspection | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-05` | Out-of-process editor recook | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-06` | Used Shaders UI | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-07` | Runtime reload | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `SHD-DIAG-08` | Shipping tool erasure | Included | Source/helper pending | Included source workflow obligation: prove all compiler/editor payload absent from Shipping. |
| `SHD-DIAG-09` | Cook progress | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |

## Showcase

**Single row owner:** Showcase. [Existing source scope and limits](../../Architecture/Modules/Projects/Showcase/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `SHOW-001` | Project discovery | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `SHOW-002` | Editor product | Included | Editor/Launcher/source | Required shipped Editor/Launcher workflow within the release-map scope; qualify exact helper/dependency membership and clean-machine behavior. |
| `SHOW-003` | Game product | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `SHOW-004` | Six-profile build naming | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |
| `SHOW-005` | Startup level selection | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `SHOW-006` | Game level switching | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `SHOW-007` | Authored level save | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |

## SourceImporters

**Single row owner:** SourceImporters. [Existing source scope and limits](../../Architecture/Modules/Tools/SourceImporters/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `IMP-001` | glTF 2.0 JSON | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-002` | GLB | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-003` | FBX | Experimental | Source only | Capability/limitation-gated; explicit requested/active truth and safe rejection; no unproved vendor/backend/estimator claim. |
| `IMP-004` | OBJ/USD/Alembic/OpenVDB | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |
| `IMP-005` | Triangle geometry | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-006` | Tangent generation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-007` | Coordinate normalization | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-008` | Mesh instances | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-009` | GPU-authored instancing | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-010` | Metallic-roughness material | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-011` | Material textures | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-012` | Alpha | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-013` | Material variants | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-014` | Cameras | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-015` | Lights | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-016` | Skeleton/skin | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-017` | Morph targets | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-018` | Animation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `IMP-019` | Embedded FBX textures | Excluded | No release promise | Outside first-release scope; do not expose or advertise this capability in the consumer product. |

## Tasks

**Single row owner:** Tasks. [Existing source scope and limits](../../Architecture/Modules/Engine/Tasks/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `TASK-001` | Three-lane execution | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-002` | Deterministic serial reference path | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-003` | Compiled DAG | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-004` | Lane dependency safety | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-005` | Parallel ranges | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-006` | Logical nested completion | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-007` | Failure propagation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-008` | Cooperative cancellation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-009` | Structured scopes | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-010` | Execution observation | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-011` | Blocking event | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-012` | Drain/cancel shutdown | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-013` | Capacity policy | Included | Game/source | Required bounded product mechanic; accept through existing owner/FCR and exact candidate, not source presence. |
| `TASK-014` | Windows task tracing | Included | Source only | Retain existing developer/source route; no packaged developer archive or consumer control. |

## ToolSupport

**Single row owner:** ToolSupport. [Existing source scope and limits](../../Architecture/Modules/Tools/ToolSupport/README.md); the approved scope bounds these rows; technical owners retain their proof obligations.

| ID | Capability | Classification | Exposure | Decision / required closure |
| --- | --- | --- | --- | --- |
| `TOOL-001` | Severity-prefixed messages | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `TOOL-002` | Named fields | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `TOOL-003` | Progress records | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `TOOL-004` | Summaries and lists | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `TOOL-005` | Path display helpers | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `TOOL-006` | Current consumers | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |
| `TOOL-007` | Game-profile isolation | Included | Source/helper pending | Source path retained; only helpers required by admitted Launcher/Editor workflows may ship after exact payload/toolchain closure is frozen. No standalone tool or SDK promise. |

## Consumer And Development Surface Splits

This table takes precedence over a compound inventory title. It does not replace feature semantics or create a second setting/state authority.

| Surface | Classification / exposure | Owner and bounds |
| --- | --- | --- |
| D3D12 / Vulkan | Included / runtime | RHI native mechanics; Application launch precedence; separate matrix evidence on MIN-01 |
| Raster GBuffer | Included / runtime, default | Renderer surface-production owner; opaque/alpha-mask, single-sample |
| Ray GBuffer, inline/native-pipeline/automatic traversal | Included / runtime | Shared Renderer/RHI ray owner; exact supported lowering/target; do not advertise procedural geometry |
| Direct diffuse/specular/subsurface and direct shadows | Included / runtime | Direct-lighting owner; local physical/approximation boundaries, no unproved ReSTIR DI conformance |
| Indirect diffuse/specular, indirect shadows, temporal/spatial reservoir reuse | Included / runtime | Indirect-lighting owner; exact estimator limits; no ReSTIR GI/GRIS equivalence or unbiasedness claim |
| Directional, point, spot, rect lights; sky | Included / runtime | Light/scene owners; only supported units/geometry and rights-cleared scene inputs |
| Volumetric Lighting / Frame Generation | Excluded | Existing release contract; no setting, dependency or advertised output |
| Deferred decals including secondary-ray semantics | Included / game target | Existing mandatory FCR-REN-23 obligation; initial maps have no decal actors; no silent omission of required ray semantics |
| Progressive Reference Path Tracer | Included / runtime | Reliable full admitted SurfaceTransportReference, consumer mode/raw export, independent convergence/oracle/lifetime proof. Preserve PTD-00 and finish recovery/Stage 10; no finite diagnostic or denoised image substituted. |
| Manual/Automatic exposure; Histogram/DownsamplePyramid | Included / runtime | Exposure owner; all current enumerators retained with validated values/history behavior |
| Reinhard / AcesApprox / AcesFilmic | Included / runtime | Tone-mapping owner; AcesApprox release default, exactly one transform |
| Automatic/Linear/Srgb encoding | Included / runtime | Presentation owner; validated surface combination, no implication that Linear means HDR |
| Linear provider / native extent | Included / runtime, default | Reconstruction owner; this is not TAA or a new native AA implementation |
| DLSS / NativeAA, Quality, Balanced, Performance, UltraPerformance | Included / runtime | Vendor-compatible tuples only; all enum quality choices require supported-provider/extent proof. Missing provider is visible unavailable, not a hidden baseline switch. |
| DLSS Ray Reconstruction | Included / runtime | Required complete compatible lighting/guide/provider cells on D3D12 and Vulkan; Off native control, On realtime preset; current Vulkan initialization gap cannot be marked accepted |
| Classic TLAS / refit | Included / runtime | Shared AS identity/lifetime; no new consumer acceleration-structure editor |
| PTLAS and partition controls | Experimental / source only | Narrow installed capability and build/update semantics; Off consumer baseline; no performance savings claim |
| Mesh batching / single-sample raster / fixed Halton jitter | Included / runtime | Existing mechanics; advanced occlusion/mesh shaders, MSAA, standalone TAA/FXAA and dynamic resolution excluded |
| Color Grading / Chromatic Aberration / Vignette | Included / game targets | FCR-REN-24/25; neutral grading, chromatic aberration On and vignette On defaults; each feature needs independent bounded controls, formula/order/failure and both-backend proof |
| SDR / HDR10 | Included / runtime | SDR default; HDR10 only with qualified display/OS/swapchain tuple and mandatory SDR fallback; FCR-REN-26 remains open |
| PIX / Nsight Graphics / RenderDoc | Experimental / Debug and Development source products only | Existing external-capture authority, one selected provider None -> Nsight -> PIX -> RenderDoc; exact supported native cells only. No optional Shipping capture payload. |
| PIX Timing / Nsight GPU Trace and Systems / WPR-WPA / PresentMon | Experimental / source runbooks only | Distinct tool-managed activities/artifacts; not universal frame-provider APIs; native evidence remains in owning external-capture report |
| AMD specialist profilers, embedded crash SDK/handler, internal profiler expansion | Excluded | User exclusion and separate discovery/privacy owners; no dormant adapter or new SDK in this scope |
| Reflex/PCL and developer concurrency/diagnostic tuning | Experimental / source only | Existing latency/startup owners; no consumer latency savings or exposed topology control promise |

The [owner-directed rendering closure](../FirstRelease.md#required-rendering-closure) additionally requires the realtime >=30 FPS preset, complete frame graph, full Included parity and independent principal-level frame review. These are release obligations, not source acceptance. See the scope record for native/realtime/reference presets. Vignette adds one target inventory row; no new enum or CVar was implemented.

## Every View Mode

**Owner:** Renderer semantic, Application Game composition, Editor presentation. [Actual enum](../../../Engine/Renderer/Public/Viewport/RenderViewMode.h) has 18 values; Count is not a mode.

| Mode | Classification | Exposure |
| --- | --- | --- |
| `Lit = 0` | Included | Game/source |
| `ReferencePathTracer = 1` | Included | Game/source |
| `Wireframe = 2` | Included | Source only |
| `GBufferDiffuse = 3` | Included | Source only |
| `GBufferWorldNormal = 4` | Included | Source only |
| `GBufferWorldTangent = 5` | Included | Source only |
| `GBufferRoughness = 6` | Included | Source only |
| `GBufferMetallic = 7` | Included | Source only |
| `GBufferEmissive = 8` | Included | Source only |
| `GBufferAmbientOcclusion = 9` | Included | Source only |
| `GBufferSubsurfaceColor = 10` | Included | Source only |
| `GBufferSubsurfaceStrength = 11` | Included | Source only |
| `DirectDiffuse = 12` | Included | Source only |
| `DirectSpecular = 13` | Included | Source only |
| `DirectSubsurface = 14` | Included | Source only |
| `IndirectDiffuse = 15` | Included | Source only |
| `IndirectSpecular = 16` | Included | Source only |
| `GpuSceneInstances = 17` | Included | Source only |

## Every Persisted Setting

**Single persistence owner:** Application. [Actual allowlist/writer](../../../Engine/Application/Private/RenderingSettings/EngineRenderingSettingsPersistence.cpp) contains 25 common names plus the conditional Editor startup-provider name. Renderer owns values/effects, not persistence or consumer UI. Current INI values below are observations, not the proposed release baseline. Public controls must use validated owning domains; no unrestricted numeric/CVar console in Shipping.

| Setting | Current immutable INI | Classification / exposure | Release decision |
| --- | --- | --- | --- |
| `r.ExternalCapture.StartupProvider` | `absent; typed default None` | Experimental / source only | Editor Debug/Development only; None/Nsight/PIX/RenderDoc; one provider; no Shipping option. |
| `r.VSync` | `0` | Included / runtime | Consumer default On; benchmark explicitly Off; current repository INI Off remains source observation. |
| `r.BackBufferFormat` | `28` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.PreferHighPerformanceAdapter` | `1` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.ToneMapper` | `2` | Included / runtime | Change consumer baseline from AcesFilmic (2) to AcesApprox (1) in PD-1; all three modes Included. |
| `r.Exposure.Mode` | `1` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.MeteringMethod` | `0` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.OutputColorEncoding` | `0` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.Manual` | `0.800000` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.Compensation` | `0.000000` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.TargetLuminance` | `0.180000` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.Min` | `0.000001` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.Max` | `65536.000000` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.AdaptationSpeedUp` | `2.000000` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Exposure.AdaptationSpeedDown` | `1.000000` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.MeshAutoBatching` | `1` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.Upscaler.Provider` | `1` | Included / runtime | Linear/native and RR-Off are native controls; Included DLSS quality/RR cells must pass on both APIs. Unsupported requests are visible failures, not accepted substitutes. |
| `r.Upscaler.QualityMode` | `0` | Included / runtime | Linear/native and RR-Off are native controls; Included DLSS quality/RR cells must pass on both APIs. Unsupported requests are visible failures, not accepted substitutes. |
| `r.RayReconstruction.Mode` | `1` | Included / runtime | Linear/native and RR-Off are native controls; Included DLSS quality/RR cells must pass on both APIs. Unsupported requests are visible failures, not accepted substitutes. |
| `r.GBuffer.Algorithm` | `0` | Included / runtime | Included Rasterized default; RayTracing Included; realtime preset requires complete traced primary/secondary transport; shared automatic ray lowering. |
| `r.RayTracing.Tlas.Refit` | `1` | Included / runtime | Retain typed validated control; value/range/domain from owning settings contract. |
| `r.RayTracing.PreferPartitionedTlas` | `1` | Experimental / source only | Off consumer baseline; no exposed consumer partition planner controls. |
| `r.RayTracing.Ptlas.PartitionsPerAxis` | `8` | Experimental / source only | Off consumer baseline; no exposed consumer partition planner controls. |
| `r.RayTracing.Ptlas.PartitionUpdateMode` | `0` | Experimental / source only | Off consumer baseline; no exposed consumer partition planner controls. |
| `r.RayTracing.Ptlas.MarkAllDynamicInPartition` | `0` | Experimental / source only | Off consumer baseline; no exposed consumer partition planner controls. |
| `r.RayTracing.Ptlas.ModeChangeDistance` | `100.000000` | Experimental / source only | Off consumer baseline; no exposed consumer partition planner controls. |

## Coverage And Remaining Evidence

This revised proposal assigns **512 capability IDs** across **16 owners**: 475 Included, 8 Experimental, 29 Excluded, 0 Removed. The original 511 IDs are retained; REN-POST-14 is the newly admitted vignette target. Every capability ID is present once. This is coverage of the existing detailed inventory, not a claim that every implementation fact in its historical snapshot was freshly audited.

Live supplemental closure covers the product CMake targets, graphics-launch parser, package/path contracts, 20 actual catalog levels, all 18 view modes, every persisted setting and the current external-capture path. All other registered developer CVar/CLI settings are development-only, unstable, and absent from the Game control promise. Shipping Editor/Launcher controls are bounded by their owning contracts; helper membership must be explicitly frozen, not inferred from this source inventory. Future source changes or an unmatched consumer option invalidate the scope check.

Primary-source rights metadata does not prove the identity of locally acquired/cooked bytes. REL-01 must close that identity, credits, environment/texture/dependency closure, root license, publisher and redistribution receipts. Feature-local native proof, observer comparisons, failures and Shipping erasure remain in their existing acceptance owners. Approval adds A only for the disposition; never B/R/N/P.

