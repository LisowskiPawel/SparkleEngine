# CLI11 integration

[Migration strategy](../ThirdPartyMigration.md) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary implementation plan. Delete this file when the feature closes.

**Status:** stage 1 captured on 2026-11-08 in `DevelopmentEditor`, with production source unchanged at `10e171cd`. Stage 4 AssetCooker implementation and bounded request, cook and failure checks pass; records are in ignored `build/validation/cli11/assetcooker-20261108/report.md`. Temporary validation cleanup is blocked by automatic approval review. Delivery closure still depends on the recorded stage 2–3 cost/shared-host gaps and the remaining stages' acceptance. Stage 4 does not close those gates.

Stage 5's ShaderCompiler conversion was implemented independently at the user's request on 2026-11-08. Its command/request comparisons, failure checks, debug/optimization export comparisons and existing `ShaderCompilerCliValidation` checks pass. Initial evidence and measurements are in ignored `build/validation/cli11/shader-20261108/report.md`; the current file ownership, source/executable identities and validation are in ignored `build/validation/cli11/shader-structure-20261108/report.md`. The cumulative three-tool parser rebuild exceeds the frozen build-time budget and remains an integration closure blocker. Stage 5 does not close the preceding stages or stage 6.

The baseline report, cases, raw output, hashes and comparison budgets are in ignored `build/validation/cli11/baseline-20261108/report.md`. Use these records for the conversion. They cover command parsing, Launcher routing functions and bounded shell/shader execution; native GUI, successful asset cooking, game and package workflows still need delivery-stage checks.

The result is three tools using CLI11 directly for argument syntax, option relationships, common validation and generated help. Sparkle keeps typed requests, domain validation and command execution; Windows argument quoting stays with process-launch code. Each tool has one option definition used for parsing and help. Remove the handwritten parsing machinery without introducing a shared parser framework or changing the command interface.

Copy a stage prompt below to start the work. It refers back to this plan so acceptance rules stay in one place. Run stage 1 first; prepare stage 2 and deliver it with stage 3, then continue in order. The baseline, [adoption requirements](../ThirdPartyMigration.md#requirements-for-adoption), [dependency workflow](../ThirdPartyMigration.md#ownership-and-dependency-workflow) and repository coding rules apply to every stage.

For each stage, report changed/deleted code, exact commands and results, comparison artifacts and unresolved failures. Investigate failures and rerun affected checks. Missing prerequisites or unrun checks remain open with their reason; they cannot satisfy acceptance. Keep comparison records in ignored output and bind them to the actual source, executables and inputs.

Before closing each stage, format and check every changed owned source file, then review paragraph spacing, names, evaluation order and ownership by hand. Prefer direct bindings and named functions over lambdas where they read better. Remove unnecessary helpers, state and indirection; passing the formatter alone does not complete this review.

## 1. Capture the current behavior

```text
Capture stage 1 of the CLI11 plan in Docs/Implementation/CLI11.md.
Follow Docs/Engineering.md and Docs/CodeStyle.md. Run the unchanged tools and record the behavior and measurements required by this stage.
Keep production code unchanged. Check every acceptance condition and report the evidence and gaps.
```

Record the revision, executable hashes, build configuration and representative inputs before changing code. Run the existing tools and save arguments, parsed requests or selected operations, stdout/stderr and exit codes. Count first-party parser/help source and headers separately from dependency code. Set build/startup/size budgets before comparing results.

| Tool | Behavior to preserve |
| --- | --- |
| [Launcher](../../Tools/Launcher/SparkleLauncher/Private/Shell/LauncherShellArguments.cpp) | GUI/shell selection; profile defaults and catalog validation; product/API selection; optional `--dry-run` value defaulting to `workspace.sync-code`; exclusion with `--run`; level/shader-symbol operation restrictions; clean/force confirmations; rejection of duplicate GPU-crash/export options. Help aliases are `--help`, `-h` and `/?`, returning 0 on stdout. |
| [AssetCooker](../../Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp) | All four cook commands, optional project/ALL for `cook-project`, required project for category commands, positional configuration recognition and default `DevelopmentGame`, `--tool-profile` and `--root`. Top-level help also accepts `/h`; it currently returns 1 on stderr. |
| [ShaderCompiler](../../Tools/Shaders/ShaderCompiler/Private/Cli) | `cook`, `list-backends`, `list-targets`, `list-shaders [--validate]` and `inspect-shader`. Cook defaults to DxilSm66/SpirV16 and four parallel compiles; explicit targets replace defaults and are deduplicated. Preserve repeated changed paths, comma-separated analysis passes, selection exclusion, on/true/off/false values and the 1–8 compile limit. Exit codes are success 0, usage 1, no work 2 and cook failure 6. |

Cover option order, repeated values, empty/Unicode/option-like values, missing values, unknown arguments and help both alone and mixed with other arguments. Capture help positions and error precedence rather than assuming CLI11 defaults match. Generated help may change layout; supported syntax, useful explanations, output streams and exit behavior must remain correct.

**Complete when:** the comparison set covers every option, command, default and invalid combination used by callers. Inventory scripts and child-process argument producers as well as interactive examples.

**Failure checks:** unavailable tools, unrecorded exit/output behavior, missing caller invocations or irreproducible inputs leave the baseline incomplete. Source inspection alone cannot establish a runtime comparison.

## 2. Prepare the dependency and build integration

```text
Implement stages 2 and 3 of the CLI11 plan in Docs/Implementation/CLI11.md as one delivery.
Use the existing dependency workflow and the recorded baseline. Follow Docs/Engineering.md and Docs/CodeStyle.md.
Deliver CLI11 registration, Launcher sync/readiness, private target integration and the Launcher parser together. Run both stages' acceptance and failure checks and report evidence and code removed.
```

Start with the official [CLI11 2.7.2 release](https://github.com/CLIUtils/CLI11/releases/tag/v2.7.2); verify and pin its full commit and license through the existing dependency cache. Use header-only integration initially, with private target usage. Disable upstream tests/examples in production builds.

Register `cli11` through the existing CMake selection, standalone Launcher allowlist and `SourceDependencyState.cpp`. Agree on the display version, `cli11-src` cache, required headers/license and enabled/required state. CLI11 is required by Launcher independently of optional content, shader and graphics SDK features. Keep its types out of Core, public request headers and unrelated runtime targets. Add its identity/license to existing notices and reviewed package metadata. Land dependency wiring with the first converted tool.

**Complete when:** independent Launcher and tool configurations resolve one pin; supported host-library configurations build; Launcher shows CLI11 and its individual Sync/Clean actions work; full Sync Code populates it; and warm/offline configure reuses the same cache. Verify single-dependency sync returns before unrelated SDK/target setup. Include required notices/SBOM identity without a CLI11 runtime DLL, vendored copy or extra dependency manager.

**Failure checks:** missing required headers/license must not appear ready; dirty or wrong-revision caches must fail CMake verification and remain preserved. Test interrupted/failed sync, metadata/pin drift, unexpected downloads and cache cleanup in a disposable workspace. A dependency row alone is insufficient.

## 3. Convert Launcher

```text
Complete stage 3 and the prepared stage 2 dependency work in Docs/Implementation/CLI11.md.
Follow Docs/Engineering.md and Docs/CodeStyle.md. Replace Launcher parsing/help with CLI11 while preserving the baseline and existing domain catalogs.
Run the Launcher acceptance and failure checks. Report code removed, measurements and unresolved differences before closing either stage.
```

Replace the private shell parser and manual usage text with CLI11 declarations. Reuse profile, IDE, compiler and clean-scope catalogs for domain values. Keep operation-specific restrictions with Launcher. Preserve GUI startup, shell routing, confirmation behavior and shadow-process handoff.

**Complete when:** the Launcher comparison set passes, generated help describes every supported option, and the old parser class, value-reading loop and duplicate help definitions are removed. No new option-name table should duplicate the declarations.

**Failure checks:** exercise missing `--run` values, conflicting run/dry-run requests, duplicate GPU options, invalid profiles and mixed help/errors. Catch changed GUI/shell routing, lost confirmations or shadow handoff, execution after a parse error and renamed parsing machinery that still duplicates CLI11.

## 4. Convert AssetCooker

The follow-up review keeps orchestration and implementation details with their CLI owner, exposes only `RunAssetCooker` to the entry point and aligns CMake membership/source groups. Current request, executable, cook, failure and structure checks are recorded in ignored `build/validation/cli11/assetcooker-structure-20261108/report.md`; preceding readability results remain in `build/validation/cli11/assetcooker-readability-20261108/report.md`.

```text
Implement stage 4 of the CLI11 plan in Docs/Implementation/CLI11.md after the Launcher/dependency delivery passes.
Follow Docs/Engineering.md and Docs/CodeStyle.md. Convert all four AssetCooker commands directly to CLI11 without changing service behavior.
Run this stage's acceptance and failure checks and report request comparisons, real cook results and removed parsing/help code.
```

Declare the four cook commands and their existing positional/option grammar. Map directly to the current project, configuration, tool profile, root and category used by `AssetCookerService`. Keep CLI11 private to the executable; the service and cooking libraries remain independent of it.

**Complete when:** all four commands preserve request selection, diagnostics and exit behavior; category commands still require a project; repeat/order cases pass; and the old argument loop, syntax helpers and manual usage table are removed.

**Failure checks:** missing category projects, missing option values, extra positionals and reordered/repeated configurations must match the baseline. Detect wrong category/profile/root selection, changed help exit/stream, accidental cooking on invalid input and replacement parsing loops hidden in helpers.

## 5. Convert ShaderCompiler

```text
Implement stage 5 of the CLI11 plan in Docs/Implementation/CLI11.md after the preceding conversions pass.
Follow Docs/Engineering.md and Docs/CodeStyle.md. Convert all five commands, cook options and help using the existing shader/domain owners.
Run parser comparisons, this stage's failure checks and ShaderCompilerCliValidation. Report artifacts, exit behavior and deleted parsing/dispatch duplication.
```

Cover all five commands, including top-level dispatch and help. Declare cook options once and map them to `ShaderCookSettings`. Keep shader registration, target naming, backend selection and cooking behavior with their existing owners. Preserve debug-export and optimization settings, target replacement/deduplication and selection rules.

**Complete when:** parser comparisons pass for every command, help uses the same declarations, and the handwritten cook parser state/methods and duplicate command usage strings are removed. Run the existing `ShaderCompilerCliValidation` target and retain its artifact-preservation checks.

**Failure checks:** test conflicting selections, invalid changed paths/targets, parallel counts 0 and 9, nonnumeric counts and missing inspect IDs. Detect lost target defaults/deduplication, changed debug/optimization behavior, wrong usage/no-work/failure exits or replacement of accepted shader output after a failed request.

## 6. Verify the complete workflow and finish

```text
Complete stage 6 of the CLI11 plan in Docs/Implementation/CLI11.md on the final files from all three conversions.
Run every workflow, acceptance condition and failure check in that stage, including Launcher dependency sync and relocated/offline delivery.
Report reproducible results, first-party code reduction and cost comparisons. Close the integration only when required checks pass; identify every remaining blocker precisely.
```

Use the same small representative content and shader inputs as the baseline. Exercise real process boundaries and inspect their outputs:

| Workflow | Required result |
| --- | --- |
| Launcher shell and GUI cook actions | Exercise both direct ShaderCompiler and AssetCooker paths. Matching project/profile/root/category/selection reaches each tool; progress, cancellation, failure reporting and completion still settle correctly. |
| Editor Recompile Shaders / `CompileShaders` → Launcher → ShaderCompiler | Preserve the existing request and completion path, with shader symbols both enabled and disabled. |
| AssetCooker project and category cooks | Each command produces expected output, including the AssetCooker → ShaderCompiler path. A failed request preserves accepted output. Use bounded fixtures for textures and scene assets. |
| Shader cook → list/inspect | Compile a representative shader to both targets, then inspect its map/library records. Exercise changed-source selection, serial/parallel settings and debug export. Preserve no-work and failure exits. |
| Launcher scene run | Dry-run selection and a real bounded run preserve product/API choice and process handoff on the supported D3D12/Vulkan routes. |
| Prebuilt and matching-source delivery | Run the prepared tools offline from a relocated workspace, then reproduce source configuration/build and the representative cook. Verify notices and runtime dependencies. |
| Dependency lifecycle | From the final Launcher, verify full Sync Code, individual CLI11 Sync/Clean, missing-cache readiness, failed sync and warm/offline reuse against the same CMake pin. |

Compare behavior with the baseline, then report removed/retained parsing mechanisms, net first-party source size, build time, CLI startup, executable/dependency size and any unresolved differences. Review every changed file under the existing coding rules. Reuse existing checks; keep temporary comparison probes local and remove them before submission.

**Complete when:** all three conversions and the workflow checks pass on the final files, superseded parsing/help code is gone, and the integration reduces first-party maintenance. Record commands, configuration, observations and missing evidence with the result. Keep the final comparison in ignored output while reviewing closure. Carry only lasting guidance into the existing main guides, then delete this file; do not archive it or copy its progress history into those guides. CLI/workflow evidence does not establish rendering or release acceptance.

**Failure checks:** inject a missing/nonzero child tool, cancellation and paths containing spaces/Unicode. Reject false success, lost error/progress reporting, damaged accepted output, leftover child work, wrong target/selection, offline network access, missing notices or exceeded budgets. Builds and isolated parser tests cannot replace these workflow results.
