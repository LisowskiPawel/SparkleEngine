# Microsoft D3DX12 helpers

[`Include/d3dx12.h`](Include/d3dx12.h) is Microsoft upstream code under the accompanying MIT license. Sparkle's native warning/include wrapper lives in [RHI](../../Engine/RHI/Private/D3D12/D3DX12Includes.h); [Dependency.cmake](Dependency.cmake) owns target composition.

This historical import did not record its upstream commit. Its preserved SHA-256 is `bc48e96fe3995e8181a5ecd1cab5840b095dba58044523abacbacb52fd7ead2b`; the separation change does not modify the header. Do not invent an upstream version for these bytes. Review replacement bytes, provenance, license and callers together when upgrading from [Microsoft DirectX-Headers](https://github.com/microsoft/DirectX-Headers).
