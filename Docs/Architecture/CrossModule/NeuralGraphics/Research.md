# Owned Neural Denoising Research

[Docs](../../../README.md) / [Up](NeuralGraphicsDesign.md) / [Proposed semantics](Semantics.md)

**Status:** primary-source precedent and local design reasoning, inspected 2026-10-25; not a trained-model, tool-compatibility or hiring result

## What The Sources Support

| Primary source | Supported observation | Transfer to this proposal and limit |
| --- | --- | --- |
| [NVIDIA neural-shading course, 2025](https://research.nvidia.com/labs/rtr/publication/duca2025neural/) | The course connects compact trainable functions, deployment in shaders and optional matrix hardware, with texture/appearance examples. | Require owned training-to-shader evidence. The selected diffuse denoiser and CNN are local choices; the course does not demonstrate their quality or require texture compression, a new shading language or cooperative vectors. |
| [NVIDIA recurrent Monte Carlo reconstruction paper, 2017](https://research.nvidia.com/publication/2017-07_interactive-reconstruction-monte-carlo-image-sequences-using-recurrent) | The paper studies learned reconstruction of noisy rendered sequences. | Use noisy/clean signal, auxiliary-input and sequence-quality questions. A spatial three-layer proposal does not inherit the paper's architecture, temporal behavior or results; test motion even without history. |
| [PyTorch Conv2d](https://docs.pytorch.org/docs/main/generated/torch.nn.Conv2d.html) | The documented operation is cross-correlation; channel, stride, padding, groups and precision choices affect meaning. | Write the equation/layout/edge contract explicitly and detect flips/transposes with asymmetric fixtures. Pin the actual installed release at NG-D0; documentation at `main` is not a dependency pin. |
| [PyTorch reproducibility](https://docs.pytorch.org/docs/2.14/notes/randomness.html), [numerical accuracy](https://docs.pytorch.org/docs/2.14/notes/numerical_accuracy.html) | Reproducibility depends on environment/algorithms, and floating-point evaluation order/precision changes results. | Separate model-byte identity, repeat-training tolerance and CPU/export/GPU equivalence. Seeds or one successful comparison are insufficient. These are library semantics, not proof the local environment is configured. |
| [Nsight shader profiler](https://docs.nvidia.com/nsight-graphics/UserGuide/shader-profiler.html) | Source correlation and sampled shader-limit analysis can inform register, memory and latency hypotheses; full disassembly access has edition/support limits. | Bind the executed shader to its compiler/artifact and measured counters. Distinguish DXIL/SPIR-V, instruction-range correlation and full native ISA. Missing access is explicit; do not build a profiler or purchase a tool to fill a claim. |

## Why This First Model

The proposal makes the complete math, tensor layout, work and storage inspectable: eleven real rendering channels, three small convolution layers, two transient hidden buffers and one output lobe. An owned residual model is a tractable test of data generation, autodiff, training reproducibility, export, GPU numerics, kernel tradeoffs and product value. It is deliberately distinct from calling an opaque vendor denoiser.

Alternatives considered: a pointwise MLP has little spatial evidence for denoising; a recurrent/U-Net route adds history or resolution operators before the producer/numerical chain is proved; copying an external trained network weakens ownership/provenance. These are local scope/cost judgments, not claims those alternatives perform worse. If the proposed model fails the predeclared value gate, preserve the negative study and ratify one bounded revision; do not call failure a shipped neural feature.

The local recipe, topology, 2 ms/workspace envelope and quality deltas in [Semantics](Semantics.md#numerical-and-product-gates) are proposed program targets. They are neither numbers from these papers nor employer hiring criteria. NG-D0 must qualify and ratify them before candidate training. No paper/code/checkpoint/dependency is imported by this research; reuse needs an exact source/license record under the existing discovery gate.
