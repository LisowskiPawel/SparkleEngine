#include "PCH.h"

#include "Compiler/Slang/SlangReflection.h"
#include "Compiler/ShaderCompileProfile.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Json/JsonInputAdmission.h"

#include <nlohmann/json.hpp>
#include <algorithm>
#include <array>
#include <format>
#include <limits>
#include <string>
#include <utility>
#include <tuple>

static std::uint32_t ReadLayoutNumber(const nlohmann::json& object, std::string_view name, std::uint32_t absent = 0)
{
	const auto value = object.find(name);
	if (value == object.end())
	{
		return absent;
	}
	if (!value->is_number_unsigned())
	{
		throw Diagnostics::Error(std::format("Slang reflection '{}' requires an unsigned integer.", name));
	}
	const std::uint64_t number = value->get<std::uint64_t>();
	if (number > std::numeric_limits<std::uint32_t>::max())
	{
		throw Diagnostics::Error("Slang reflection integer exceeds the engine representation.");
	}
	return static_cast<std::uint32_t>(number);
}

static std::uint32_t ReadElementCount(const nlohmann::json& type)
{
	const auto value = type.find("elementCount");
	if (value == type.end() || *value == "unknown")
	{
		return 1;
	}
	if (*value == "unbounded")
	{
		return 0;
	}
	return std::max(1u, ReadLayoutNumber(type, "elementCount"));
}

static const nlohmann::json& GetElementType(const nlohmann::json& type)
{
	const std::string kind = type.at("kind").get<std::string>();
	const bool bParameterGroup = kind == "constantBuffer" || kind == "parameterBlock";
	const bool bBufferGroup = kind == "textureBuffer" || kind == "shaderStorageBuffer";
	if (bParameterGroup || bBufferGroup)
	{
		const auto layout = type.find("elementVarLayout");
		return layout == type.end() ? type.at("elementType") : layout->at("type");
	}
	return type;
}

static CookedShaderScalarType ReadScalarType(const nlohmann::json& type)
{
	const std::string kind = type.at("kind").get<std::string>();
	const nlohmann::json& scalar = kind == "vector" || kind == "matrix" ? type.at("elementType") : type;
	const std::string name = scalar.value("scalarType", "");
	static constexpr std::array names{"", "bool", "int32", "uint32", "float16", "float32", "int16", "uint16", "int64", "uint64", "float64"};
	const auto found = std::ranges::find(names, name);
	return found == names.end() ? CookedShaderScalarType::Unknown : static_cast<CookedShaderScalarType>(found - names.begin());
}

static const nlohmann::json* FindLayoutBinding(const nlohmann::json& variable, std::string_view category)
{
	if (const auto binding = variable.find("binding"); binding != variable.end())
	{
		return binding->at("kind") == category ? &*binding : nullptr;
	}
	if (const auto bindings = variable.find("bindings"); bindings != variable.end())
	{
		const auto binding = std::ranges::find_if(*bindings, [category](const nlohmann::json& value) { return value.at("kind") == category; });
		if (binding != bindings->end())
		{
			return &*binding;
		}
	}
	return nullptr;
}

static void ReadMembers(const nlohmann::json& type, std::uint32_t parentOffset, std::vector<ShaderReflectionConstantBufferMember>& members)
{
	if (type.at("kind") != "struct")
	{
		return;
	}
	for (const nlohmann::json& field : type.at("fields"))
	{
		const nlohmann::json& fieldType = field.at("type");
		const nlohmann::json* binding = FindLayoutBinding(field, "uniform");
		const std::uint32_t fieldOffset = binding ? ReadLayoutNumber(*binding, "offset") : 0u;
		if (fieldOffset > std::numeric_limits<std::uint32_t>::max() - parentOffset)
		{
			throw Diagnostics::Error("Slang reflection member offset overflows.");
		}
		const std::string kind = fieldType.at("kind").get<std::string>();
		const bool bScalarOrVector = kind == "scalar" || kind == "vector";
		const std::uint32_t rows = ReadLayoutNumber(fieldType, "rowCount", bScalarOrVector ? 1u : 0u);
		const std::uint32_t columns = ReadLayoutNumber(fieldType, "columnCount", bScalarOrVector ? ReadElementCount(fieldType) : 0u);
		if (rows > 255u || columns > 255u)
		{
			throw Diagnostics::Error("Slang reflection matrix dimensions exceed the engine representation.");
		}
		ShaderReflectionConstantBufferMember member{
		    .Name = field.at("name").get<std::string>(),
		    .OffsetInBytes = parentOffset + fieldOffset,
		    .SizeInBytes = binding ? ReadLayoutNumber(*binding, "size") : 0u,
		    .ArrayCount = ReadElementCount(fieldType),
		    .ArrayStrideInBytes = binding ? ReadLayoutNumber(*binding, "elementStride") : 0u,
		    .ScalarType = ReadScalarType(fieldType),
		    .RowCount = static_cast<std::uint8_t>(rows),
		    .ColumnCount = static_cast<std::uint8_t>(columns)};
		members.push_back(std::move(member));
		ReadMembers(fieldType, parentOffset + fieldOffset, members);
	}
}

static CookedShaderResourceDimension ReadDimension(const nlohmann::json& type)
{
	const std::string shape = type.value("baseShape", "");
	const bool bArray = type.value("array", false);
	if (shape == "texture1D")
	{
		return bArray ? CookedShaderResourceDimension::Texture1DArray : CookedShaderResourceDimension::Texture1D;
	}
	if (shape == "texture2D")
	{
		if (type.value("multisample", false))
		{
			return bArray ? CookedShaderResourceDimension::Texture2DMSArray : CookedShaderResourceDimension::Texture2DMS;
		}
		return bArray ? CookedShaderResourceDimension::Texture2DArray : CookedShaderResourceDimension::Texture2D;
	}
	if (shape == "texture3D")
	{
		return CookedShaderResourceDimension::Texture3D;
	}
	if (shape == "textureCube")
	{
		return bArray ? CookedShaderResourceDimension::TextureCubeArray : CookedShaderResourceDimension::TextureCube;
	}
	const bool bStructuredOrByte = shape == "structuredBuffer" || shape == "byteAddressBuffer";
	return bStructuredOrByte || shape == "textureBuffer" ? CookedShaderResourceDimension::Buffer : CookedShaderResourceDimension::Unknown;
}

static CookedShaderResourceKind ReadResourceKind(const nlohmann::json& type, std::string_view category, bool bWritable)
{
	if (category == "constantBuffer")
	{
		return CookedShaderResourceKind::ConstantBuffer;
	}
	if (category == "samplerState")
	{
		return CookedShaderResourceKind::Sampler;
	}
	const std::string shape = type.value("baseShape", "");
	if (shape == "structuredBuffer")
	{
		return bWritable ? CookedShaderResourceKind::RWStructuredBuffer : CookedShaderResourceKind::StructuredBuffer;
	}
	if (shape == "byteAddressBuffer")
	{
		return bWritable ? CookedShaderResourceKind::RWByteAddressBuffer : CookedShaderResourceKind::ByteAddressBuffer;
	}
	if (shape == "textureBuffer")
	{
		return bWritable ? CookedShaderResourceKind::RWTypedBuffer : CookedShaderResourceKind::TypedBuffer;
	}
	if (shape == "accelerationStructure")
	{
		return CookedShaderResourceKind::AccelerationStructure;
	}
	return bWritable ? CookedShaderResourceKind::RWTexture : CookedShaderResourceKind::Texture;
}

static void ReadVariables(const nlohmann::json& variables, ShaderStage stage, ShaderReflection& reflection);

static void ReadVariableBinding(const nlohmann::json& variable, const nlohmann::json& binding, ShaderStage stage, ShaderReflection& reflection)
{
	const nlohmann::json& type = variable.at("type");
	const std::string category = binding.at("kind").get<std::string>();
	const bool bResource = category == "shaderResource" || category == "unorderedAccess";
	const bool bBufferOrSampler = category == "constantBuffer" || category == "samplerState";
	const bool bDescriptor = bResource || bBufferOrSampler;
	if (bDescriptor || category == "descriptorTableSlot")
	{
		const nlohmann::json* resourceType = &type;
		while (resourceType->at("kind") == "array")
		{
			resourceType = &resourceType->at("elementType");
		}
		const bool bWritable = category == "unorderedAccess" || resourceType->value("access", "read") != "read";
		ShaderReflectionResourceBinding resource{
		    .Name = variable.at("name").get<std::string>(),
		    .Kind = ReadResourceKind(*resourceType, category, bWritable),
		    .Dimension = ReadDimension(*resourceType),
		    .bIsReadOnly = !bWritable,
		    .Set = ReadLayoutNumber(binding, "space"),
		    .Slot = ReadLayoutNumber(binding, "index"),
		    .ArrayCount = ReadElementCount(type)};
		if (category == "constantBuffer")
		{
			const nlohmann::json& element = GetElementType(type);
			const std::string typeName = element.value("name", "");
			if (!typeName.empty())
			{
				resource.Name = typeName;
			}
			const nlohmann::json* uniform = FindLayoutBinding(type.at("elementVarLayout"), "uniform");
			resource.SizeInBytes = uniform ? ReadLayoutNumber(*uniform, "size") : 0u;
			resource.ConstantBufferIndex = static_cast<std::uint32_t>(reflection.ConstantBuffers.size());
			ShaderReflectionConstantBuffer buffer{.Name = resource.Name, .SizeInBytes = resource.SizeInBytes};
			ReadMembers(element, 0, buffer.Members);
			reflection.ConstantBuffers.push_back(std::move(buffer));
		}
		const bool bDuplicate = std::ranges::any_of(
		    reflection.Bindings,
		    [&resource](const ShaderReflectionResourceBinding& existing)
		    { return std::tie(existing.Name, existing.Kind, existing.Set, existing.Slot) == std::tie(resource.Name, resource.Kind, resource.Set, resource.Slot); });
		if (!bDuplicate)
		{
			reflection.Bindings.push_back(std::move(resource));
		}
	}
	else if (category == "varyingInput" && stage == ShaderStage::Vertex)
	{
		const std::string kind = type.at("kind").get<std::string>();
		const std::uint32_t components = kind == "vector" ? ReadElementCount(type) : 1u;
		if (components > 255u)
		{
			throw Diagnostics::Error("Slang reflection input component count exceeds the engine representation.");
		}
		reflection.InputElements.push_back(
		    {.Semantic = variable.value("semanticName", variable.at("name").get<std::string>()),
		        .SemanticIndex = ReadLayoutNumber(variable, "semanticIndex"),
		        .Location = ReadLayoutNumber(binding, "index"),
		        .ScalarType = ReadScalarType(type),
		        .ComponentCount = static_cast<std::uint8_t>(components)});
	}
	else if (category == "pushConstantBuffer")
	{
		const nlohmann::json* uniform = FindLayoutBinding(variable, "uniform");
		reflection.PushConstants.push_back(
		    {.OffsetInBytes = ReadLayoutNumber(binding, "index"), .SizeInBytes = uniform ? ReadLayoutNumber(*uniform, "size") : 0u, .VisibilityMask = ToShaderStageMask(stage)});
	}
}

static void ReadVariables(const nlohmann::json& variables, ShaderStage stage, ShaderReflection& reflection)
{
	for (const nlohmann::json& variable : variables)
	{
		if (const auto binding = variable.find("binding"); binding != variable.end())
		{
			ReadVariableBinding(variable, *binding, stage, reflection);
		}
		else if (const auto bindings = variable.find("bindings"); bindings != variable.end())
		{
			for (const nlohmann::json& item : *bindings)
			{
				ReadVariableBinding(variable, item, stage, reflection);
			}
		}
		const nlohmann::json& element = GetElementType(variable.at("type"));
		if (element.at("kind") == "struct")
		{
			ReadVariables(element.at("fields"), stage, reflection);
		}
	}
}

ShaderReflection ReadSlangReflection(std::string_view input, ShaderStage stage, std::string_view entryPoint)
{
	Json::InputAdmission admission(input, 64u * 1024u * 1024u);
	static constexpr std::array consumed{
	    "parameters",
	    "entryPoints",
	    "binding",
	    "bindings",
	    "type",
	    "kind",
	    "name",
	    "fields",
	    "elementType",
	    "elementVarLayout",
	    "size",
	    "offset",
	    "elementCount",
	    "elementStride",
	    "scalarType",
	    "rowCount",
	    "columnCount",
	    "baseShape",
	    "access",
	    "array",
	    "multisample",
	    "index",
	    "space",
	    "semanticName",
	    "semanticIndex",
	    "threadGroupSize",
	    "stage"};
	const auto admit = [&admission](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
	{
		admission.ReadEvent(depth, event);
		if (event == nlohmann::json::parse_event_t::key)
		{
			const std::string& key = value.get_ref<const std::string&>();
			const auto field = std::ranges::find(consumed, key);
			if (field != consumed.end())
			{
				admission.ReadKey(depth, key);
			}
		}
		return true;
	};
	try
	{
		const nlohmann::json document = nlohmann::json::parse(input, admit);
		ShaderReflection reflection;
		if (const auto globals = document.find("parameters"); globals != document.end())
		{
			ReadVariables(*globals, stage, reflection);
		}
		const auto& entries = document.at("entryPoints");
		if (!entries.is_array() || entries.size() != 1u)
		{
			throw Diagnostics::Error("Slang reflection requires exactly one selected entry point.");
		}
		const auto entry = std::ranges::find_if(entries, [entryPoint](const nlohmann::json& value) { return value.at("name") == entryPoint; });
		if (entry == entries.end())
		{
			throw Diagnostics::Error("Slang reflection does not contain the requested entry point.");
		}
		if (entry->at("stage") != GetSlangShaderStageName(stage))
		{
			throw Diagnostics::Error("Slang reflection stage does not match the requested shader.");
		}
		if (stage == ShaderStage::Compute)
		{
			const auto& group = entry->at("threadGroupSize");
			if (!group.is_array() || group.size() != 3u)
			{
				throw Diagnostics::Error("Slang reflection requires three compute group dimensions.");
			}
			for (std::size_t index = 0; index < 3; ++index)
			{
				const auto& dimension = group.at(index);
				if (!dimension.is_number_unsigned())
				{
					throw Diagnostics::Error("Slang compute group dimensions require unsigned integers.");
				}
				const std::uint64_t size = dimension.get<std::uint64_t>();
				if (size == 0 || size > std::numeric_limits<std::uint32_t>::max())
				{
					throw Diagnostics::Error("Slang compute group dimension is outside the engine representation.");
				}
				reflection.ThreadGroupSize[index] = static_cast<std::uint32_t>(size);
			}
		}
		if (const auto parameters = entry->find("parameters"); parameters != entry->end())
		{
			ReadVariables(*parameters, stage, reflection);
		}
		return reflection;
	}
	catch (const nlohmann::json::exception& error)
	{
		throw Diagnostics::Error(std::format("Invalid Slang reflection: {}", error.what()));
	}
}
