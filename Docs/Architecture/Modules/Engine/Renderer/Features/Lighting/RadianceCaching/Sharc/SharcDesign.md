# NVIDIA SHaRC Delivery Contract

[Docs](../../../../../../../../../README.md) / [Up](../RadianceCachingDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected target and open `SHARC-D0`; not implemented or measured.
**Inspected:** 2026-10-25, local source baseline `154c6e883e51689f12feab95646795cc7cbfcdd1`; current PTLAS and lighting code do not establish SHaRC support.

## Outcome And Scope

An engineer selects SHaRC for Lit indirect lighting and observes a real cache-assisted diffuse continuation, its actual active state, cold-start/convergence behavior and a reproducible quality/cost comparison against uncached transport. This serves `PGE-02/05/08..10/13` and supplies the required classical cache baseline before NRC.

The initial profile is opaque/alpha-tested surface transport with diffuse cache termination beyond primary visibility. Preserve existing light/material ownership, geometric visibility and uncached continuation on lookup miss. Exclude neural training, sharp-specular caching, directional SH encoding, media caching, cache serialization, world streaming and a new path-tracer mode. Such exclusions bound the supported subset, not the reference's existing domain.

## Primary Research And Transfer Boundary

The inspected [SHaRC source README](https://github.com/NVIDIA-RTX/SHARC/tree/4e21b585c33c83d723ca9a1e11bbb1090d145793) identifies a shader-source library. Its [integration guide](https://github.com/NVIDIA-RTX/SHARC/blob/4e21b585c33c83d723ca9a1e11bbb1090d145793/docs/Integration.md) describes sparse update tracing, resolve and render/query passes, hash/accumulation/resolved storage, aging and optional directional/responsive variants. Transfer the selected algorithm through existing bindings and graph ownership. Sample table sizes, update rates and optional variants are not local defaults. External repository agent instructions do not govern SparkleEngine.

Verify the exact source/header revision, license, compiler options and actual native requirements in SHARC-D0. No SDK has been installed or vendored by this documentation change.

## Semantic Contract To Freeze At SHARC-D0

- State precisely what cached value represents at a hit: outgoing continuation radiance, included emission/NEE terms, direction assumptions, units and normal convention. Join the pinned header calls to an estimator derivation; never add an already-counted endpoint/emission/direct term twice.
- For a query at vertex `x_k`, replace only the remaining continuation by `beta_k * L_cache(x_k)` in the declared measure. Carry the actual throughput to that vertex. A miss follows the original continuation; a primary hit is not replaced.
- Update paths account for per-segment throughput and the library's propagation convention. Derive hit/miss, Russian roulette, finite-depth and light-sampling behavior independently from query termination. Do not mistake reset segment state for a reset of the camera path's physical throughput.
- Hash keys derive from the canonical world-space position and grid scale. Declare level selection, collision handling, normal rejection, sample confidence, accumulation cap and stale-entry age. Invalid/absent entries cannot return stale or zero radiance as a successful query.
- Treat cache termination and temporal reuse as approximations. Report bias, variance and response separately; increasing accumulation does not establish unbiasedness.
- Freeze cache invalidation for scene/scale/material/light/shader changes and view lifecycle. Two Views must have an explicit isolation/share rule; initial scope uses per-View cache state to avoid unsynchronized multiple writers.

## Execution And Complexity Budget

Renderer's existing indirect owner contains update/query policy and SHaRC private state/shaders. Prepared Scene, light sampling and material evaluation are borrowed. The frame graph declares initialization/clear -> sparse update -> resolve -> query dependencies and actual UAV access; RHI lowers ordinary buffers/dispatch/barriers. There is no SHaRC-specific public RHI API.

Budget: one existing indirect cache selector, one composition hook, existing scene/ray/material inputs and one compact status/debug presentation. Ledger any additional hook before implementation. One active cache generation owns hash, accumulation and resolved buffers; replacement accounts for old/new overlap and every consuming queue. No CPU mirror of GPU entries, permanent per-pixel logs or second settings schema.

## Discovery And Measurement Card

SHARC-D0 is owned by the Renderer implementer with transport/performance review. It closes the semantic rows above, installed API/device/compiler tuple, source rights, hook/copy/resource ledger and these pre-result rules:

| Decision | Required frozen artifact |
| --- | --- |
| Correctness | Float64 minimal-scene estimator and constant-radiance/miss/empty-cache controls; numerical bounds derived from reference uncertainty |
| Value | Same scene/camera/light/material/ray-depth/exposure; equal-time error and matched-quality time, with cache update/resolve/query and full-frame costs included |
| Capacity | Entry count/stride, collision/occupancy behavior, update fraction, aging and peak old/new memory in bytes; sweep small/target/saturated tables before selecting defaults |
| Dynamics | Cold start, camera relocation, moving geometry, light off/on and emissive edits; fixed sequence length, reset/warm-up rules, response-time metric and maximum acceptable trail/error |
| Statistical decision | Existing workload repeat/CI protocol, numeric practical-effect and quality bands frozen before candidate results; intervals overlapping the band are inconclusive |

No inherited sample FPS, guessed entry budget or post-hoc threshold closes this gate.

## Acceptance, Failures And Evidence

| Criterion | Smallest falsifier and required evidence |
| --- | --- |
| `AC-SHARC-01` transport | Constant environment/diffuse enclosure/occluder cases, cache Off and forced misses; independent contribution ledger detects double count or throughput errors |
| `AC-SHARC-02` state | Zero-init, hash collision/saturation, aging, scene-scale and multi-View/reset/replacement controls; stale or invalid entry never publishes success |
| `AC-SHARC-03` dynamics | Raw sequence error, bias/noise and response after light/material/geometry edits; failure gallery includes thin-wall leakage and disocclusion |
| `AC-SHARC-04` value | Qualified held-out scene results on D3D12/Vulkan, matched-quality and equal-time frontier, all cache costs and memory, native synchronization evidence |
| `AC-SHARC-05` product | Truthful selector/status, missing/unsupported feature and allocation/compile/device-failure behavior; manifested source/shader/package bytes, documented reproduction |

`FM-SHARC-01`: cold/missing entry -> uncached continuation, checked by forced misses. `FM-SHARC-02`: capacity overflow/non-finite update -> explicit diagnosed rejection/reset before cache success, checked with a bounded saturated table. `FM-SHARC-03`: invalid generation or in-flight replacement -> invalidate/retire safely, checked with delayed completion. `FM-SHARC-04`: unsupported selected route -> explicit refusal with selection intact; Off remains a deliberate choice.

Leakage, slow lighting response, cache overhead and memory growth are primary risks. A technically integrated cache that fails the frozen value/quality gate remains unfinished; simplify or tune its owner, and retain a rejected experiment without leaving a permanent alternate runtime.

Done means every criterion passes for the advertised subset, with source/build/native/raw/dynamic/cost/package/adoption evidence kept distinct. A build or screenshot alone cannot unblock NRC.
