#include "PCH.h"
#include "D3D12/Pipeline/D3D12RootSignature.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "RHI/Private/D3D12/ThirdParty/D3DX12Includes.h"

D3D12RootSignature::D3D12RootSignature(D3D12Rhi& rhi, const RootSignatureDesc& desc) :
    m_rhi(rhi)
{
	Create(desc);
}

void D3D12RootSignature::Create(const RootSignatureDesc& desc)
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(desc.ParameterCount, desc.Parameters, desc.StaticSamplerCount, desc.StaticSamplers, desc.Flags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	const HRESULT serializeResult = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
	CHECK(serializeResult);

	const HRESULT createResult = m_rhi.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));
	CHECK(createResult);
	m_rootSignature->SetName(desc.DebugName);
}

D3D12RootSignature::~D3D12RootSignature() noexcept
{
	m_rootSignature.Reset();
}
