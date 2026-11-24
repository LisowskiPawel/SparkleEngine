#include "PCH.h"

#include "DxilReflectionExtractor.h"

#include "Core/Public/Diagnostics/Error.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <utility>
#include <wrl/client.h>

static CookedShaderResourceKind MapDxilResourceKind(D3D_SHADER_INPUT_TYPE type, D3D_SRV_DIMENSION dim)
{
	switch (type)
	{
		case D3D_SIT_CBUFFER:
			return CookedShaderResourceKind::ConstantBuffer;
		case D3D_SIT_TBUFFER:
			return CookedShaderResourceKind::TypedBuffer;
		case D3D_SIT_TEXTURE:
			return CookedShaderResourceKind::Texture;
		case D3D_SIT_SAMPLER:
			return CookedShaderResourceKind::Sampler;
		case D3D_SIT_UAV_RWTYPED:
			return (dim == D3D_SRV_DIMENSION_BUFFER) ? CookedShaderResourceKind::RWTypedBuffer : CookedShaderResourceKind::RWTexture;
		case D3D_SIT_STRUCTURED:
			return CookedShaderResourceKind::StructuredBuffer;
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			return CookedShaderResourceKind::RWStructuredBuffer;
		case D3D_SIT_BYTEADDRESS:
			return CookedShaderResourceKind::ByteAddressBuffer;
		case D3D_SIT_UAV_RWBYTEADDRESS:
			return CookedShaderResourceKind::RWByteAddressBuffer;
		case D3D_SIT_RTACCELERATIONSTRUCTURE:
			return CookedShaderResourceKind::AccelerationStructure;
		default:
			return CookedShaderResourceKind::Unknown;
	}
}

static CookedShaderResourceDimension MapDxilDimension(D3D_SRV_DIMENSION dim)
{
	switch (dim)
	{
		case D3D_SRV_DIMENSION_BUFFER:
			return CookedShaderResourceDimension::Buffer;
		case D3D_SRV_DIMENSION_TEXTURE1D:
			return CookedShaderResourceDimension::Texture1D;
		case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
			return CookedShaderResourceDimension::Texture1DArray;
		case D3D_SRV_DIMENSION_TEXTURE2D:
			return CookedShaderResourceDimension::Texture2D;
		case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
			return CookedShaderResourceDimension::Texture2DArray;
		case D3D_SRV_DIMENSION_TEXTURE2DMS:
			return CookedShaderResourceDimension::Texture2DMS;
		case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
			return CookedShaderResourceDimension::Texture2DMSArray;
		case D3D_SRV_DIMENSION_TEXTURE3D:
			return CookedShaderResourceDimension::Texture3D;
		case D3D_SRV_DIMENSION_TEXTURECUBE:
			return CookedShaderResourceDimension::TextureCube;
		case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
			return CookedShaderResourceDimension::TextureCubeArray;
		default:
			return CookedShaderResourceDimension::Unknown;
	}
}

static bool IsDxilReadOnlyKind(CookedShaderResourceKind kind)
{
	switch (kind)
	{
		case CookedShaderResourceKind::RWTexture:
		case CookedShaderResourceKind::RWStructuredBuffer:
		case CookedShaderResourceKind::RWByteAddressBuffer:
		case CookedShaderResourceKind::RWTypedBuffer:
			return false;
		default:
			return true;
	}
}

static CookedShaderScalarType MapDxilScalarType(D3D_SHADER_VARIABLE_TYPE type)
{
	switch (type)
	{
		case D3D_SVT_BOOL:
			return CookedShaderScalarType::Bool;
		case D3D_SVT_INT:
			return CookedShaderScalarType::Int32;
		case D3D_SVT_UINT:
			return CookedShaderScalarType::UInt32;
		case D3D_SVT_FLOAT:
			return CookedShaderScalarType::Float32;
		case D3D_SVT_INT16:
			return CookedShaderScalarType::Int16;
		case D3D_SVT_UINT16:
			return CookedShaderScalarType::UInt16;
		case D3D_SVT_FLOAT16:
			return CookedShaderScalarType::Float16;
		case D3D_SVT_DOUBLE:
			return CookedShaderScalarType::Float64;
		case D3D_SVT_INT64:
			return CookedShaderScalarType::Int64;
		case D3D_SVT_UINT64:
			return CookedShaderScalarType::UInt64;
		default:
			return CookedShaderScalarType::Unknown;
	}
}

static CookedShaderScalarType MapInputComponent(D3D_REGISTER_COMPONENT_TYPE type)
{
	switch (type)
	{
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return CookedShaderScalarType::Float32;
		case D3D_REGISTER_COMPONENT_SINT32:
			return CookedShaderScalarType::Int32;
		case D3D_REGISTER_COMPONENT_UINT32:
			return CookedShaderScalarType::UInt32;
		default:
			return CookedShaderScalarType::Unknown;
	}
}

template <typename TReflection> static Microsoft::WRL::ComPtr<TReflection> OpenReflection(IDxcUtils& utils, std::span<const std::uint8_t> reflectionBytes, std::string_view context)
{
	if (reflectionBytes.empty())
	{
		throw Diagnostics::Error(std::format("{} payload is empty.", context));
	}

	const DxcBuffer reflectionBuffer{.Ptr = reflectionBytes.data(), .Size = reflectionBytes.size(), .Encoding = 0};
	Microsoft::WRL::ComPtr<TReflection> reflection;
	const HRESULT hr = utils.CreateReflection(&reflectionBuffer, IID_PPV_ARGS(reflection.ReleaseAndGetAddressOf()));
	if (FAILED(hr) || !reflection)
	{
		throw Diagnostics::Error(std::format("{}: CreateReflection failed.", context));
	}
	return reflection;
}

template <typename TReflection> static void ReflectConstantBuffers(TReflection& shader, UINT count, ShaderReflection& reflection, std::string_view context)
{
	// Emit one record per reflected constant buffer.
	// Bindings resolve CB indices by name in the next loop.
	reflection.ConstantBuffers.reserve(count);
	for (UINT i = 0; i < count; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* cb = shader.GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC cbDesc{};
		if (cb == nullptr || FAILED(cb->GetDesc(&cbDesc)))
		{
			throw Diagnostics::Error(std::format("{} could not read a constant-buffer declaration.", context));
		}

		ShaderReflectionConstantBuffer outCb;
		outCb.Name = cbDesc.Name ? cbDesc.Name : "";
		outCb.SizeInBytes = cbDesc.Size;
		outCb.Members.reserve(cbDesc.Variables);

		for (UINT v = 0; v < cbDesc.Variables; ++v)
		{
			ID3D12ShaderReflectionVariable* var = cb->GetVariableByIndex(v);
			D3D12_SHADER_VARIABLE_DESC vDesc{};
			if (var == nullptr || FAILED(var->GetDesc(&vDesc)))
			{
				throw Diagnostics::Error(std::format("{} could not read a constant-buffer member.", context));
			}

			ID3D12ShaderReflectionType* type = var->GetType();
			D3D12_SHADER_TYPE_DESC tDesc{};
			if (type == nullptr || FAILED(type->GetDesc(&tDesc)))
			{
				throw Diagnostics::Error(std::format("{} could not read a constant-buffer member type.", context));
			}

			ShaderReflectionConstantBufferMember member;
			member.Name = vDesc.Name ? vDesc.Name : "";
			member.OffsetInBytes = vDesc.StartOffset;
			member.SizeInBytes = vDesc.Size;
			member.ArrayCount = (tDesc.Elements > 0) ? tDesc.Elements : 1u;
			// D3D reflection does not expose array stride.
			// Approximate it as SizeInBytes / ArrayCount.
			member.ArrayStrideInBytes = member.SizeInBytes / member.ArrayCount;
			member.ScalarType = MapDxilScalarType(tDesc.Type);
			member.RowCount = static_cast<std::uint8_t>(tDesc.Rows);
			member.ColumnCount = static_cast<std::uint8_t>(tDesc.Columns);
			outCb.Members.push_back(std::move(member));
		}

		reflection.ConstantBuffers.push_back(std::move(outCb));
	}
}

template <typename TReflection> static void ReflectResourceBindings(TReflection& shader, UINT count, ShaderReflection& reflection, std::string_view context)
{
	// Resource bindings.
	reflection.Bindings.reserve(count);
	for (UINT i = 0; i < count; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
		if (FAILED(shader.GetResourceBindingDesc(i, &bindDesc)))
		{
			throw Diagnostics::Error(std::format("{} could not read a resource binding.", context));
		}

		ShaderReflectionResourceBinding binding;
		binding.Name = bindDesc.Name ? bindDesc.Name : "";
		binding.Kind = MapDxilResourceKind(bindDesc.Type, bindDesc.Dimension);
		binding.Dimension = MapDxilDimension(bindDesc.Dimension);
		binding.bIsReadOnly = IsDxilReadOnlyKind(binding.Kind);
		binding.Set = bindDesc.Space;
		binding.Slot = bindDesc.BindPoint;
		binding.ArrayCount = bindDesc.BindCount;

		if (binding.Kind == CookedShaderResourceKind::ConstantBuffer)
		{
			for (std::size_t cbIndex = 0; cbIndex < reflection.ConstantBuffers.size(); ++cbIndex)
			{
				if (reflection.ConstantBuffers[cbIndex].Name == binding.Name)
				{
					binding.ConstantBufferIndex = static_cast<std::uint32_t>(cbIndex);
					binding.SizeInBytes = reflection.ConstantBuffers[cbIndex].SizeInBytes;
					break;
				}
			}
		}
		else if (binding.Kind == CookedShaderResourceKind::StructuredBuffer || binding.Kind == CookedShaderResourceKind::RWStructuredBuffer)
		{
			binding.SizeInBytes = bindDesc.NumSamples; // element stride
		}

		reflection.Bindings.push_back(std::move(binding));
	}
}

static void ReflectVertexInputs(ID3D12ShaderReflection& shader, UINT count, ShaderReflection& reflection)
{
	reflection.InputElements.reserve(count);
	for (UINT i = 0; i < count; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
		if (FAILED(shader.GetInputParameterDesc(i, &paramDesc)))
		{
			throw Diagnostics::Error("DXIL reflection could not read a shader input parameter.");
		}

		// Skip system-value semantics (SV_*) that are not consumed from
		// the input layout.
		if (paramDesc.SystemValueType != D3D_NAME_UNDEFINED)
		{
			continue;
		}

		ShaderReflectionInputElement element;
		element.Semantic = paramDesc.SemanticName ? paramDesc.SemanticName : "";
		element.SemanticIndex = paramDesc.SemanticIndex;
		element.Location = paramDesc.Register;
		element.ScalarType = MapInputComponent(paramDesc.ComponentType);
		element.ComponentCount = static_cast<std::uint8_t>(std::popcount(paramDesc.Mask));
		reflection.InputElements.push_back(std::move(element));
	}
}

ShaderReflection ExtractDxilReflection(IDxcUtils& utils, std::span<const std::uint8_t> reflectionBytes, ShaderStage stage)
{
	ShaderReflection reflection;

	auto shaderReflection = OpenReflection<ID3D12ShaderReflection>(utils, reflectionBytes, "DXIL reflection");

	D3D12_SHADER_DESC desc{};
	if (FAILED(shaderReflection->GetDesc(&desc)))
	{
		throw Diagnostics::Error("DXIL reflection: GetDesc failed");
	}

	ReflectConstantBuffers(*shaderReflection.Get(), desc.ConstantBuffers, reflection, "DXIL reflection");

	ReflectResourceBindings(*shaderReflection.Get(), desc.BoundResources, reflection, "DXIL reflection");

	if (stage == ShaderStage::Vertex)
	{
		ReflectVertexInputs(*shaderReflection.Get(), desc.InputParameters, reflection);
	}

	// Compute thread-group size.
	if (stage == ShaderStage::Compute)
	{
		UINT x = 0, y = 0, z = 0;
		shaderReflection->GetThreadGroupSize(&x, &y, &z);
		reflection.ThreadGroupSize = {x, y, z};
	}

	// DXIL does not expose push constants or specialization constants here.
	// Both arrays stay empty.

	return reflection;
}

static bool IsRequestedLibraryExport(std::string_view reflectedName, std::string_view entryPoint)
{
	if (reflectedName == entryPoint)
	{
		return true;
	}

	constexpr std::string_view mangledNamePrefix{"\x01?"};
	if (!reflectedName.starts_with(mangledNamePrefix))
	{
		return false;
	}
	reflectedName.remove_prefix(mangledNamePrefix.size());
	return reflectedName.starts_with(entryPoint) && reflectedName.substr(entryPoint.size()).starts_with("@@");
}

static ID3D12FunctionReflection* FindLibraryExport(ID3D12LibraryReflection& library, std::string_view entryPoint, D3D12_FUNCTION_DESC& functionDesc)
{
	D3D12_LIBRARY_DESC libraryDesc{};
	if (FAILED(library.GetDesc(&libraryDesc)))
	{
		throw Diagnostics::Error("DXIL library reflection: GetDesc failed.");
	}

	ID3D12FunctionReflection* function = nullptr;
	std::string reflectedFunctionNames;
	for (UINT functionIndex = 0; functionIndex < libraryDesc.FunctionCount; ++functionIndex)
	{
		ID3D12FunctionReflection* candidate = library.GetFunctionByIndex(functionIndex);
		D3D12_FUNCTION_DESC candidateDesc{};
		if (candidate == nullptr || FAILED(candidate->GetDesc(&candidateDesc)) || candidateDesc.Name == nullptr)
		{
			continue;
		}
		if (!reflectedFunctionNames.empty())
		{
			reflectedFunctionNames += ", ";
		}
		reflectedFunctionNames += candidateDesc.Name;
		if (IsRequestedLibraryExport(candidateDesc.Name, entryPoint))
		{
			function = candidate;
			functionDesc = candidateDesc;
			break;
		}
	}
	if (function == nullptr)
	{
		throw Diagnostics::Error(
		    std::format("DXIL library reflection did not contain requested export '{}'; reflected functions: {}.", entryPoint, reflectedFunctionNames.empty() ? "<none>" : reflectedFunctionNames));
	}
	return function;
}

ShaderReflection ExtractDxilLibraryReflection(IDxcUtils& utils, std::span<const std::uint8_t> reflectionBytes, std::string_view entryPoint)
{
	if (entryPoint.empty())
	{
		throw Diagnostics::Error("DXIL library reflection requires an export name.");
	}

	auto library = OpenReflection<ID3D12LibraryReflection>(utils, reflectionBytes, "DXIL library reflection");
	D3D12_FUNCTION_DESC functionDesc{};
	ID3D12FunctionReflection* function = FindLibraryExport(*library.Get(), entryPoint, functionDesc);

	ShaderReflection reflection;
	ReflectConstantBuffers(*function, functionDesc.ConstantBuffers, reflection, "DXIL library reflection");
	ReflectResourceBindings(*function, functionDesc.BoundResources, reflection, "DXIL library reflection");

	return reflection;
}
