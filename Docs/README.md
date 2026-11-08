# SparkleEngine

SparkleEngine is a Windows rendering project with D3D12 and Vulkan backends. It combines a scene launcher, an inspection editor and a standalone rendering application. The first public release is still being validated.

## Getting started

1. [Build and run](Engineering.md#build-and-run) Launcher, then open an example scene.
2. Read the [rendering overview](Rendering.md#overview) and [source map](Rendering.md#source-map).
3. Follow the [code style](CodeStyle.md) and [development workflow](Engineering.md#making-a-change) when contributing.

## Guides

| Guide | Contents |
| --- | --- |
| [Goals](Goals.md) | Target engineer, detailed capabilities, progress review, product scope and portfolio |
| [Roadmap](Roadmap.md) | Current problem, delivery order and release requirements |
| [Third-party migration](ThirdPartyMigration.md) | Dependency choices, expected benefits, ownership and adoption requirements |
| [Rendering](Rendering.md) | Features, current limits, test scenes and planned research |
| [Engineering](Engineering.md) | Setup, module boundaries, development, validation and packaging |
| [Code style](CodeStyle.md) | Formatting, naming, C++, shaders and coordinate conventions |
| [References](References.md) | Role requirements and sources, professional baseline, papers, SDK guides and NVIDIA examples |

The guides distinguish existing code from planned work. Use the recorded build, input and test results when deciding whether a feature is ready.

Active developer plans live temporarily in `Docs/Implementation/`; follow the [documentation rules](Engineering.md#writing-documentation).
