# Owned Diffuse-Indirect Denoising Semantics

[Docs](../../../README.md) / [Up](NeuralGraphicsDesign.md) / [Delivery](ImplementationPlan.md)

**Status:** concrete target proposal for NG-D0 ratification; no model, dataset, numerical result or runtime implementation is claimed

**Responsibility:** define the first owned signal, fixed network, tensor math, training experiment and numerical/value checks. [Discovery](Discovery.md) records accepted revisions and unresolved qualifications; [Execution Architecture](ExecutionArchitecture.md) owns production boundaries. These proposed choices replace an unspecified topology, not the decision gate.

## Selected Product And Reference Dependency

One spatial network reconstructs the existing scene-linear, albedo-modulated `IndirectDiffuse` RGB product before lighting composition. Compare it to the bounded classical filter below; a separate spatial diffuse denoiser is not established by the current source scan. Direct light, specular, exposure, grading and vendor whole-image reconstruction remain separate. There is no recurrent state, motion input, super resolution, generic model graph or reference-authority claim for the network.

The current `RestirIndirectResolve` source writes weighted diffuse/specular contributions; it is an input seam, not proof of correctness. The current Reference product publishes beauty mean/M2. **Beauty is not a diffuse-indirect training label.** NG-S1 must obtain the following qualified component before dataset publication:

| Required measurement | Meaning and falsifier |
| --- | --- |
| Diffuse-indirect reference component | Apply the accepted Indirect transport contract's path-event partition and the primary surface's diffuse BSDF contribution to the accepted Reference estimator. Preserve the actual joint sampling density, MIS and roulette compensation. The direct/environment boundary must match the frozen Indirect contract; an isolated environment/direct/emissive fixture decides each event class before generation. |
| Same measurement population | Scene/camera/pose, pixel footprint, subpixel sampling, extent, units, light/material domain and contributing generations agree. Publish component mean, uncertainty and committed prefix with original identity. Do not derive a label by subtracting unrelated noisy/direct/specular images or relabel beauty. |
| Independent component proof | Black/diffuse/metallic, isolated direct source, environment and one secondary diffuse surface isolate the component. Detect direct/specular leakage, wrong PDF/normalization and stale/mixed prefixes. Check component conservation where a complete partition is available. Selecting a sampled lobe alone is insufficient unless its estimator derivation proves the intended component. |

This is an after-release NG-S1 prerequisite, not a new first-release blocker or a second path tracer. Its producer belongs to the existing Reference owner, with generic typed raw publication/readback and one explicit consumer. Requalify affected estimator/output evidence after that change. Training stays blocked until component validity and reference uncertainty are accepted.

## Input And Output Contract

For pixel `p`, use eleven channels in this order: transformed noisy diffuse-indirect RGB, world shading-normal XYZ, linear diffuse-reflectance RGB, transformed positive view depth, surface-valid mask. Reuse accepted GBuffer/guide definitions; `BaseColor` is not automatically diffuse reflectance. For a no-surface pixel, set all eleven network inputs to zero, still evaluate hidden layers consistently, and bypass the reconstructed output in favor of the producer's defined finite value. This makes convolution neighborhoods at silhouettes deterministic. A malformed surface input is a rejected input, not a no-surface sample.

Use `T(L) = log(1 + L / s)` with `s = 1` in the declared scene-linear radiance unit. Negative/non-finite radiance is invalid input, not silently clamped training data. Depth in metres maps to `d / (d + 1 m)`. Valid-surface normals are finite unit world vectors, reflectance is in its accepted linear range and validity is exactly zero or one. Exposure and display transforms never enter these tensors.

The proposed transformed-radiance input/target qualification range is `[0,16]`; prediction outside that finite range is also an invalid model result, not a clipping operation. Out-of-domain/non-finite data rejects the sample/request. A model-result fault may use the qualified classical filter on valid inputs, with the failure count and mixed/fallback identity visible; malformed raw inputs remain unavailable. Report the population; rejection cannot hide a failing required workload. NG-D02/07 must qualify this domain before freezing it.

## Bounded Classical Baseline

NG-S1 qualifies one fixed 7x7 spatial cross-bilateral filter over the same raw RGB, surface mask and guides, with no image history. It is a necessary baseline/fallback inside this denoising owner, not a new reconstruction system. The normal temporal/vendor product remains a separate contextual comparison.

For valid center `p`, accumulate only valid in-bounds neighbors `q` in radius three. Use linear radiance `C(p)=sum(w(p,q)*L(q))/sum(w(p,q))`, where

`w = exp(-||q-p||^2/(2*1.5^2)) * max(dot(n_p,n_q),0)^32 * exp(-(d_p-d_q)^2/(2*(0.01*max(d_p,1 m))^2)) * exp(-||rho_p-rho_q||^2/(2*0.1^2))`.

Here `d` is positive view depth in metres and `rho` is accepted linear diffuse reflectance. The center has weight one, so a valid population has a positive denominator. No-surface pixels retain their defined source value. Reject non-finite/malformed inputs; do not turn them into accepted zero radiance. Use the same equation in independent CPU fixtures, the baseline dispatch and the final-pass fault fallback; one semantic implementation on GPU, not a second filter copy. Freeze all constants through NG-D01/07 before candidate results, with flat-field, impulse, silhouette/depth, black/metallic and non-finite controls. A baseline revision invalidates its comparisons.

The normal neural path need not execute a full classical dispatch as well: its final pass can evaluate this bounded filter only for an invalid model pixel. Mark/count those pixels and retain exact input/model/frame identity. A supported quality/value run requires zero in-domain model-fault fallbacks; an attractive mixed result cannot pass the model. Include actual guard/fallback work in timings and workspace. Whole-feature absence/capability failure selects the ordinary classical dispatch; neither a clear nor a copy pass can impersonate it.

This fixed filter is a compact same-signal control, not an industry-best claim. Qualify it before model selection and retain raw/noisy and normal shipped reconstruction results as declared context; do not choose deliberately weak filter constants to manufacture a neural win. The final case must explain when the owned path is useful and when the normal classical/vendor product is preferable. A failed value/deployment premise returns to the bounded research decision instead of adding a permanent product mode merely to display the model.

## Fixed First Network

Use three stride-one 3x3 cross-correlation layers, with replicated image-edge padding, dilation/groups one and biases. Width/height do not change. Channels are `11 -> 16 -> 16 -> 3`; ReLU follows the first two layers. The final prediction is `Y = max(0, X_rgb + C3)`, where `X_rgb` is the transformed noisy RGB. Decode with `L_hat = s * (exp(Y) - 1)` and reject overflow/non-finite results. This is a biased reconstruction, not an unbiased estimator or converged reference.

For each layer, `C[p,o] = b[o] + sum(W[o,i,dy,dx] * A[clamp(p+(dx,dy)),i])`, summed over input channels and offsets `[-1,1]^2`. Replicate padding occurs at every layer. Training tiles include a three-pixel input halo and loss is evaluated on the interior; only a true image boundary clamps. Receptive field is 7x7.

Weights use logical OIHW order; runtime activations use NHWC order, with sixteen-channel intermediates represented by four `float4` values per pixel. Export does the explicit layout conversion; no implicit transpose or spatial kernel flip is allowed. Biases are applied exactly once. There is no batch normalization, attention, pooling, learned upsampling or runtime operator interpreter.

The model has `1584+16 + 2304+16 + 432+3 = 4355` scalar parameters: 17,420 FP32 bytes, or 17,424 with final 16-byte padding. It performs 4,320 MACs per output pixel before activations/transforms. Two full FP32 intermediate buffers require `128*W*H` bytes: 253.125 MiB at 1920x1080. These are arithmetic storage/work counts, not measured GPU cost. [Execution Architecture](ExecutionArchitecture.md#kernel-and-memory-baseline) owns the implementation boundary.

## Data And Training Experiment

Train on qualified Sponza/Cornell data; use frozen Bistro validation sequences for model/recipe selection and keep all San Miguel test sequences sealed. Rights and exact source/cook/camera/pose/seed manifests remain NG-D03 prerequisites. Adjacent patches/frames from one sequence cannot cross splits. Any extra training scene needs the same source/split decision; no scene depot is selected.

Capture noisy inputs from the accepted real-time indirect policy, including its frozen sample/reuse identity, and pair them with the qualified component target. A clean Reference 2-SPP image is not assumed to reproduce ReSTIR's noise/bias distribution. Record the guide generation and uncertainty mask. No learned correction is used to excuse a wrong raw estimator.

Initial recipe proposal: PyTorch FP32, Adam (`lr=1e-3`, betas `0.9/0.999`, epsilon `1e-8`), batch 16 of 64x64 loss interiors with the required halo, maximum 100,000 updates, fixed checkpoint-selection rule on validation log-space MSE. Loss is masked transformed-RGB MSE; normalize by valid scalar count and reject an empty mask. Seeds `0,1,2` supply repeat runs; record optimizer state, exact environment, data order and deterministic-algorithm settings. Disable TF32/mixed precision for conformance. These are starting experiments to ratify, not a training-success claim or a universal deterministic recipe.

Proposed repeat rule: rerun one seed on the same frozen environment/data order and require validation log-RMSE within 1% relative difference, with all product gates still passing. Report all three seeds and their spread separately; different seeds need not produce identical weights. Re-exporting the same accepted checkpoint must produce identical artifact bytes. A different software/hardware environment is a new qualification cell, not an implied reproducibility pass. Ratify this policy in NG-D04 before training; never raise the tolerance to excuse a measured failure.

Before full training: overfit one tiny batch, check loss reduction against a zero-residual network, inspect input/target alignment, and verify a finite-difference gradient on a small operator away from activation discontinuities. Ablate guide channels and the residual connection using validation only. Run the bounded profile controls below; preserve failed runs and justify a recipe/topology revision before new training. Do not search against held-out San Miguel.

## Required Bounded Profile Controls

PGE-12 requires actual batch/precision/layout evidence. These are small predeclared diagnostic comparisons, not three optimizations that must ship or a combinatorial tuning program. NG-D04/05/07 freeze inputs, repetitions, numerical/cost rules and the exact probe before execution.

| Axis and stage | Minimal comparison and required observation |
| --- | --- |
| Training batch, NG-S2 | Profile batches 8 and 16 on the same bounded training/validation population and loss normalization. Record examples/s, ms/update and actual peak memory, separate from inference. This diagnoses throughput/memory; it does not require two full training runs or claim equal convergence. The accepted recipe remains explicit. |
| Precision, NG-S3/5 | Compare FP32 weights to one FP16 storage round trip through the same fixed operator/FP32 evaluator; retain bytes, operator/full-model drift and image-error impact. This is quantization/storage evidence, not native FP16 arithmetic or Tensor Core speed proof. A native precision speed claim additionally needs actual kernel and whole-frame measurements. |
| Runtime layout, NG-S5 | Measure the accepted NHWC16 baseline against one predeclared alternate layout/packing prototype on a representative fixed operator. Record indexing/traffic prediction, actual GPU time/peak workspace and numerical parity. State operator versus full-model/API scope; an adopted production change still needs the whole-product/both-API checks. |

Use existing Tools/ShaderSystem/Renderer owners and disposable local probes. Keep only the accepted production representation; remove rejected prototype code, formats, switches and registrations at that stage's exit while retaining original measurements. No autotuner, benchmark platform, additional model topology or mandatory vendor SDK is selected. A negative result is useful; an unrun axis cannot be reported as measured.

Retain the exact probe source/patch, frozen inputs, compiler/build/run recipe, source/cooked/native hashes and raw observations in the existing ignored evidence home. Removing a probe from submitted production must not erase the information needed to reproduce its measured claim. Share only the cleared subset; this is an artifact, not a second supported implementation.

## Numerical And Product Gates

NG-D05/07 must accept or explicitly revise these **local proposed targets before candidate results**. They are not NVIDIA hiring thresholds or published performance claims.

Evaluate valid RGB scalars over each declared scene/sequence/mask. `log-RMSE = sqrt(mean((T(pred)-T(target))^2))`; `linear NRMSE = sqrt(sum((pred-target)^2) / max(sum(target^2), 1e-12*N*s^2))`, with `N` the valid scalar count. Empty populations reject. Report both errors and their populations separately; reference uncertainty must decide the difference. No pooled average may hide a failing required scene.

The [workload contract](../../../Acceptance/GraphicsWorkloads.md#neural-rendering-contract-across-bistro-and-san-miguel) also requires FLIP and temporal assessment. NG-D07 must freeze the actual FLIP implementation/variant, parameters, image domain/range and comparison transform/exposure, reference population and numeric decision rule before training. Compare the controlled scene result with only the reconstructed diffuse product changed; direct/specular and presentation settings stay matched. Raw/log metrics remain separate. Neither log-RMSE nor a favorable display comparison waives another required metric; a missing qualified FLIP rule keeps the gate open.

| Claim | Proposed falsifying rule |
| --- | --- |
| FP32 operator/model conformance | On frozen finite-domain fixtures, every intermediate and transformed output meets `abs(error) <= 1e-4 + 1e-4*abs(reference)` against a separately implemented float64 evaluator; also compare the exported model with PyTorch FP32. Include near-zero, impulses, asymmetric weights, bias-only, edge/halo, non-square/partial extents and seeded transpose/padding/channel faults. Failure returns to the contract; no post-result tolerance relaxation. |
| Actual value | Against the semantically equivalent, same-input/history-domain fixed classical baseline: either at least 5% lower masked log-RMSE with linear NRMSE and whole-frame p95 each no worse by more than 1%, or at least 10% lower denoising GPU p95 with both error measures and whole-frame p95 no worse by more than 1%. Also report raw/noisy and the normal temporal/vendor product as separate context. Never conceal different inputs/history. |
| Cost envelope | Initial target: denoiser GPU p95 <=2 ms at 1920x1080 on MIN-01, additional GPU workspace <=320 MiB, and no persistent image history. Measure actual peak overlap and whole-frame cost; parameter bytes alone are not the memory result. |
| Temporal/edge safety | Proposed motion residual-change RMS and spatial gradient-error RMS each <=1.02 times the matched classical baseline on every required sequence/mask; absolute normalized signed bias <=0.01. Report disocclusion and bright/dark populations separately. Ratify metric validity and limits on the accepted baseline before training; missing motion/reference correspondence keeps NG-D07 open. Lower mean error cannot waive flicker or lost detail. |

For the temporal metric, form transformed residual `e_t=T(pred_t)-T(target_t)` and measure RMS of `e_t-warp(e_(t-1))` on independently qualified motion/visibility correspondences; never evaluate across disocclusion or invent a previous surface. Separately inspect the newly visible population against its same-frame target. Spatial gradient error is RMS of horizontal/vertical finite differences of `e_t` on valid neighboring surfaces, with the edge/discontinuity mask frozen before selection. Normalized signed bias is `mean(pred-target)/max(mean(abs(target)),1e-6*s)` in linear radiance, evaluated per RGB channel and required population so cancellation cannot hide a color error. Retain error maps and worst-tail sequences. The evaluator may use motion; the network has no recurrent state or motion input.

Use the workload owner's repeat, observer, practical-effect and correlation-aware uncertainty rules. Reference uncertainty must be small enough to decide the proposed quality difference; otherwise the comparison is inconclusive. Show per-scene/sequence/mask populations and failure cases, not just a favorable pooled metric. A failed value gate may retire the model and preserve the study, but cannot pass the owned-neural product requirement.

## Evidence And Change Control

`AC-NG-01..04/06` and `CHK-NG-01..03` consume these semantics; `AC-NG-09` adds the qualified component producer. Each accepted model/semantic/export change invalidates affected data, numerical, quality and runtime evidence. Keep one current model format; no internal schema migration, compatibility reader or generic tensor API. [Research](Research.md) supplies precedent and its limits, not ratification or results.
