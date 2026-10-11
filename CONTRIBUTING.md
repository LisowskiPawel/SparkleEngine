# Contributing to SparkleEngine

Submit bugs, documentation fixes and code changes to [SparkleEngine](https://github.com/stoleckipawel/SparkleEngine). Pawel Lisowski reviews contributions when available. There is no support service or response deadline.

## Bugs and proposals

Search [existing issues](https://github.com/stoleckipawel/SparkleEngine/issues) first.

- [Bug report](https://github.com/stoleckipawel/SparkleEngine/issues/new?template=bug_report.md): include reproduction steps, expected and actual results, commit/version, build configuration, backend, map and GPU/driver. Attach relevant logs with private data removed. For regressions, name the last working version if known.
- [Feature or refactor proposal](https://github.com/stoleckipawel/SparkleEngine/issues/new?template=feature_proposal.md): describe the problem, proposed behavior and affected modules before starting a large change.
- Small fixes and typo corrections can go directly to a pull request.

## Pull requests

1. Fork the repository and create a branch from its default branch.
2. Follow [Build and launch](README.md#build-and-launch). Find the affected module in [Docs](Docs/README.md), then follow [Change Integration](Docs/Engineering/Workflow/ChangeIntegration.md) and the relevant [engineering rules](Docs/Engineering/README.md#choose-by-task).
3. Fix one problem per PR. Update affected build files and documentation. Follow the repository's formatting configuration.
4. Run checks for the changed behavior. Record commands, configuration and results; list checks you could not run. Include screenshots for visual changes and measurements for performance claims.
5. Fill in the [PR template](.github/PULL_REQUEST_TEMPLATE.md). Link the issue and explain the change. Use a descriptive title, such as `Fix Launcher map selection after sync`. Open a draft for unfinished work.

Respond to review in the same PR. Keep discussion respectful. The maintainer decides whether to merge.

## Checks

Run `git diff --check` before submitting. Choose other checks using [Validation and Evidence](Docs/Engineering/Verification/ValidationAndEvidence.md).

Architecture check, without a build:

```powershell
cmake -P CMake/ArchitectureBoundaryCheck.cmake
```

Architecture, style and shader CLI checks, after generating the root workspace:

```powershell
powershell -NoProfile -File CMake/RunBaselineChecks.ps1 -Configuration ShippingEditor
```

The [CI workflow](.github/workflows/repository-checks.yml) checks whitespace and architecture. It has not run on GitHub yet; it does not build or run the products.

## Licenses and submitted files

Contributions to Sparkle-owned code use [MIT](LICENSE.txt). You retain your copyright; no separate contributor agreement is required.

For third-party code or assets, include the source URL, revision/hash and license notices. Follow [Release Inputs](Docs/Architecture/Modules/BuildAndPackaging/ReleaseInputs.md). Do not commit publisher downloads, generated builds, PDBs, credentials or personal diagnostics.

Review and test all submitted code, including generated code. Identify copied sources and generated portions that need explanation during review.
