#include "PCH.h"

#include "Cooking/ShaderArtifactWriter.h"

#include "Cooking/ReflectionSerializer.h"
#include "Cooking/ShaderCookContext.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryStreamWriter.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Strings/StringTableBuilder.h"
#include "RHI/Public/Shaders/GlobalShaderMap.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <format>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace ShaderArtifactWriter
{
	struct ArtifactBinding final
	{
		std::string Name;
		ShaderParameterSemanticKind SemanticKind = ShaderParameterSemanticKind::ReadTexture;
		ShaderParameterResourceDomain ResourceDomain = ShaderParameterResourceDomain::None;
		ShaderParameterAccess Access = ShaderParameterAccess::None;
		ShaderStageMask VisibilityMask = ShaderStageMask::None;
		std::uint32_t LogicalBindingIndex = 0;
		std::uint32_t ArrayCount = 1;
		std::uint32_t ValueSizeInBytes = 0;
	};

	struct ArtifactEntry final
	{
		ShaderTypeId ShaderType = 0;
		ShaderTarget Target = kDefaultShaderTarget;
		ShaderStage Stage = ShaderStage::Count;
		ShaderFeatureFlags Features = ShaderFeatureFlags::None;
		RayTracingShaderMetadata RayTracing;
		ShaderCodeHash CodeHash = 0;
		ShaderParameterSignature ParameterSignature = 0;
		std::uint64_t CompileInputHash = 0;
		std::uint64_t BackendVersion = 0;
		std::string ShaderName;
		std::string EntryPoint;
		std::string BackendName;
		std::string CodegenTarget;
		std::vector<ArtifactBinding> Bindings;
		ShaderReflection Reflection;
		std::vector<std::uint8_t> Code;
	};

	static ShaderStageMask ToStageMask(ShaderStageVisibility visibility) noexcept
	{
		switch (visibility)
		{
			case ShaderStageVisibility::Vertex:
				return ShaderStageMask::Vertex;
			case ShaderStageVisibility::Pixel:
				return ShaderStageMask::Pixel;
			case ShaderStageVisibility::Compute:
				return ShaderStageMask::Compute;
			case ShaderStageVisibility::RayTracing:
				return ShaderStageMask::AllRayTracing;
			case ShaderStageVisibility::AllGraphics:
				return ShaderStageMask::Vertex | ShaderStageMask::Pixel;
			case ShaderStageVisibility::All:
				return ShaderStageMask::Vertex | ShaderStageMask::Pixel | ShaderStageMask::Compute | ShaderStageMask::AllRayTracing;
			case ShaderStageVisibility::None:
			default:
				return ShaderStageMask::None;
		}
	}

	static std::uint32_t FindBindingIndex(const ShaderCookProduct& product, std::string_view name)
	{
		const auto found = std::ranges::find_if(product.bindingRemaps, [name](const ShaderDescriptorBindingRemap& remap) { return remap.Name == name; });
		if (found == product.bindingRemaps.end())
		{
			throw Diagnostics::Error(std::format("Compiled shader output is missing descriptor binding '{}'.", name));
		}
		return found->Binding;
	}

	static ArtifactEntry BuildEntryFromProduct(const ShaderCookDesc& shader, const ShaderCookProduct& product)
	{
		if (product.compiled.bytecode.empty())
		{
			throw Diagnostics::Error("A compiled shader product cannot publish empty bytecode.");
		}
		const ShaderCodeHash codeHash = Hash::Fnv1a64(product.compiled.bytecode.data(), product.compiled.bytecode.size());
		if (product.shaderTypeId != shader.shaderTypeId || !IsShaderTarget(product.target) || product.features != shader.features || product.rayTracing != shader.rayTracing
		    || product.compiled.stage != shader.stage || product.compiled.format != GetShaderBinaryFormat(product.target) || product.compiled.sourcePath != shader.sourcePath
		    || product.compiled.entryPoint != shader.entryPoint || product.compiled.backendName.empty() || product.compiled.codegenTarget != GetShaderTargetName(product.target)
		    || product.compiled.backendVersion == 0 || product.compiled.compileInputHash == 0 || product.compiled.bytecodeHash != codeHash
		    || BuildShaderParameterSignature(product.parameterLayout) != BuildShaderParameterSignature(shader.parameterLayout))
		{
			throw Diagnostics::Error(std::format("Compiled shader product for '{}' does not match its catalog contract.", shader.shaderTypeName));
		}

		ArtifactEntry entry;
		entry.ShaderType = product.shaderTypeId;
		entry.Target = product.target;
		entry.Stage = product.compiled.stage;
		entry.Features = product.features;
		entry.RayTracing = product.rayTracing;
		entry.CodeHash = codeHash;
		entry.ParameterSignature = BuildShaderParameterSignature(product.parameterLayout);
		entry.CompileInputHash = product.compiled.compileInputHash;
		entry.BackendVersion = product.compiled.backendVersion;
		entry.ShaderName = shader.shaderTypeName;
		entry.EntryPoint = product.compiled.entryPoint;
		entry.BackendName = product.compiled.backendName;
		entry.CodegenTarget = product.compiled.codegenTarget;
		entry.Reflection = product.compiled.reflection;
		entry.Code = product.compiled.bytecode;
		for (const PassParameterDesc& parameter : product.parameterLayout.GetParameters())
		{
			entry.Bindings.push_back(
			    ArtifactBinding{
			        .Name = parameter.Name,
			        .SemanticKind = parameter.Kind,
			        .ResourceDomain = parameter.ResourceDomain,
			        .Access = parameter.Access,
			        .VisibilityMask = ToStageMask(parameter.Visibility),
			        .LogicalBindingIndex = FindBindingIndex(product, parameter.Name),
			        .ArrayCount = parameter.ArrayCount,
			        .ValueSizeInBytes = parameter.ValueSizeInBytes});
		}
		return entry;
	}

	static ArtifactEntry ReadExistingEntry(const GlobalShaderMap& map, const CookedShaderLibrary& library, const GlobalShaderMapEntry& source)
	{
		ArtifactEntry entry;
		entry.ShaderType = source.ShaderType;
		entry.Target = source.Target;
		entry.Stage = source.Stage;
		entry.Features = source.Features;

		entry.RayTracing = RayTracingShaderMetadata{
		    .PayloadSizeInBytes = source.RayPayloadSizeInBytes,
		    .AttributeSizeInBytes = source.RayAttributeSizeInBytes,
		    .MinimumRecursionDepth = source.MinimumRayRecursionDepth,
		    .LocalRecordSizeInBytes = source.LocalRecordSizeInBytes,
		    .LocalRecordSignature = source.LocalRecordSignature};

		entry.CodeHash = source.CodeHash;
		entry.ParameterSignature = source.ParameterSignature;
		entry.CompileInputHash = source.CompileInputHash;
		entry.BackendVersion = source.BackendVersion;
		entry.ShaderName = map.ResolveString(source.ShaderName);
		entry.EntryPoint = map.ResolveString(source.EntryPoint);
		entry.BackendName = map.ResolveString(source.BackendName);
		entry.CodegenTarget = map.ResolveString(source.CodegenTarget);
		for (const ShaderMapBindingRecord& binding : map.GetBindings(source))
		{
			entry.Bindings.push_back(
			    ArtifactBinding{
			        .Name = std::string(map.ResolveString(binding.Name)),
			        .SemanticKind = binding.SemanticKind,
			        .ResourceDomain = binding.ResourceDomain,
			        .Access = binding.Access,
			        .VisibilityMask = binding.VisibilityMask,
			        .LogicalBindingIndex = binding.LogicalBindingIndex,
			        .ArrayCount = binding.ArrayCount,
			        .ValueSizeInBytes = binding.ValueSizeInBytes});
		}
		entry.Reflection = DecodeShaderReflection(map, source);
		const CookedShaderCodeRecord* const code = library.Find(source.CodeHash);
		if (code == nullptr)
		{
			throw Diagnostics::Error("Existing global shader map references missing library code.");
		}
		const ShaderBytecode bytecode = library.GetBytecode(*code);
		entry.Code.assign(static_cast<const std::uint8_t*>(bytecode.Data), static_cast<const std::uint8_t*>(bytecode.Data) + bytecode.Size);
		return entry;
	}

	struct ArtifactTables final
	{
		Strings::StringTableBuilder Strings;
		std::vector<GlobalShaderMapEntry> Entries;
		std::vector<ShaderMapBindingRecord> Bindings;
		std::vector<ShaderReflection> Reflections;
		// The sorted entries own these bytes until map/library encoding finishes.
		std::map<ShaderCodeHash, std::span<const std::uint8_t>> UniqueCode;
	};

	static ShaderMapStringRef ToStringRef(const Strings::StringTableEntry& entry) noexcept
	{
		return ShaderMapStringRef{entry.OffsetInBytes, entry.SizeInBytes};
	}

	static void AppendEntry(const ArtifactEntry& source, ArtifactTables& tables, ShaderCookOutput& result)
	{
		const std::span<const std::uint8_t> bytecode(source.Code);
		const auto [code, inserted] = tables.UniqueCode.try_emplace(source.CodeHash, bytecode);
		if (!inserted && !std::ranges::equal(code->second, bytecode))
		{
			throw Diagnostics::Error("Two shader code records collided on one code hash.");
		}
		GlobalShaderMapEntry entry{};
		entry.ShaderType = source.ShaderType;
		entry.Target = source.Target;
		entry.Stage = source.Stage;
		entry.BinaryFormat = GetShaderBinaryFormat(source.Target);
		entry.Features = source.Features;
		entry.RayPayloadSizeInBytes = source.RayTracing.PayloadSizeInBytes;
		entry.RayAttributeSizeInBytes = source.RayTracing.AttributeSizeInBytes;
		entry.MinimumRayRecursionDepth = source.RayTracing.MinimumRecursionDepth;
		entry.LocalRecordSizeInBytes = source.RayTracing.LocalRecordSizeInBytes;
		entry.LocalRecordSignature = source.RayTracing.LocalRecordSignature;
		entry.CodeHash = source.CodeHash;
		entry.ParameterSignature = source.ParameterSignature;
		entry.CompileInputHash = source.CompileInputHash;
		entry.BackendVersion = source.BackendVersion;
		entry.ShaderName = ToStringRef(tables.Strings.Add(source.ShaderName));
		entry.EntryPoint = ToStringRef(tables.Strings.Add(source.EntryPoint));
		entry.BackendName = ToStringRef(tables.Strings.Add(source.BackendName));
		entry.CodegenTarget = ToStringRef(tables.Strings.Add(source.CodegenTarget));
		entry.BindingRecordOffset = static_cast<std::uint32_t>(tables.Bindings.size());
		entry.BindingRecordCount = static_cast<std::uint32_t>(source.Bindings.size());
		entry.ReflectionRecordIndex = static_cast<std::uint32_t>(tables.Reflections.size());
		for (const ArtifactBinding& sourceBinding : source.Bindings)
		{
			tables.Bindings.push_back(
			    ShaderMapBindingRecord{
			        .Name = ToStringRef(tables.Strings.Add(sourceBinding.Name)),
			        .SemanticKind = sourceBinding.SemanticKind,
			        .ResourceDomain = sourceBinding.ResourceDomain,
			        .Access = sourceBinding.Access,
			        .VisibilityMask = sourceBinding.VisibilityMask,
			        .LogicalBindingIndex = sourceBinding.LogicalBindingIndex,
			        .ArrayCount = sourceBinding.ArrayCount,
			        .ValueSizeInBytes = sourceBinding.ValueSizeInBytes});
		}
		tables.Entries.push_back(entry);
		tables.Reflections.push_back(source.Reflection);

		result.entries.push_back(
		    ShaderCookedEntry{.shaderType = source.ShaderType, .shaderName = source.ShaderName, .target = source.Target, .codeHash = source.CodeHash, .codeSizeInBytes = source.Code.size()});
	}

	static ArtifactTables BuildTables(std::vector<ArtifactEntry>& entries, ShaderCookOutput& result)
	{
		std::ranges::sort(
		    entries,
		    [](const ArtifactEntry& left, const ArtifactEntry& right) { return left.ShaderType < right.ShaderType || (left.ShaderType == right.ShaderType && left.Target < right.Target); });

		const auto duplicate = std::adjacent_find(
		    entries.begin(),
		    entries.end(),
		    [](const ArtifactEntry& left, const ArtifactEntry& right) { return left.ShaderType == right.ShaderType && left.Target == right.Target; });

		if (duplicate != entries.end())
		{
			throw Diagnostics::Error("Global shader map production contains a duplicate logical key.");
		}

		ArtifactTables tables;
		for (const ArtifactEntry& source : entries)
		{
			AppendEntry(source, tables, result);
		}

		return tables;
	}

	static std::vector<CookedShaderCodeRecord> BuildCodeRecords(const std::map<ShaderCodeHash, std::span<const std::uint8_t>>& uniqueCode, std::vector<std::uint8_t>& codeBlob)
	{
		std::vector<CookedShaderCodeRecord> codeRecords;
		for (const auto& [hash, bytes] : uniqueCode)
		{
			const std::uint32_t offset = static_cast<std::uint32_t>(codeBlob.size());
			codeBlob.insert(codeBlob.end(), bytes.begin(), bytes.end());
			codeRecords.push_back(CookedShaderCodeRecord{.CodeHash = hash, .Code = ShaderCodeBlobRef{offset, static_cast<std::uint32_t>(bytes.size())}});
		}
		return codeRecords;
	}

	static std::uint64_t ComputePublicationHash(
	    const ArtifactTables& tables,
	    const ShaderReflectionTables& reflectionOutput,
	    const std::vector<CookedShaderCodeRecord>& codeRecords,
	    const std::vector<std::uint8_t>& codeBlob)
	{
		std::uint64_t publicationHash = Hash::kFnv64OffsetBasis;
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, tables.Entries);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, tables.Bindings);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.reflectionRecords);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.resourceBindings);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.constantBuffers);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.constantBufferMembers);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.inputElements);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.pushConstantRanges);
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, reflectionOutput.specializationConstants);
		publicationHash = Hash::ContinueFnv1a64(publicationHash, tables.Strings.GetBytes().data(), tables.Strings.GetBytes().size());
		publicationHash = Hash::ContinueFnv1a64Vector(publicationHash, codeRecords);
		publicationHash = Hash::ContinueFnv1a64(publicationHash, codeBlob.data(), codeBlob.size());
		return Hash::FinalizeFnv1a64(publicationHash);
	}

	static void WriteMap(const std::filesystem::path& path, std::uint64_t publicationHash, const ArtifactTables& tables, const ShaderReflectionTables& reflection)
	{
		GlobalShaderMapHeader mapHeader{};
		mapHeader.EntryCount = static_cast<std::uint32_t>(tables.Entries.size());
		mapHeader.PublicationHash = publicationHash;
		mapHeader.BindingRecordCount = static_cast<std::uint32_t>(tables.Bindings.size());
		mapHeader.ReflectionRecordCount = static_cast<std::uint32_t>(reflection.reflectionRecords.size());
		mapHeader.ResourceBindingRecordCount = static_cast<std::uint32_t>(reflection.resourceBindings.size());
		mapHeader.ConstantBufferRecordCount = static_cast<std::uint32_t>(reflection.constantBuffers.size());
		mapHeader.ConstantBufferMemberRecordCount = static_cast<std::uint32_t>(reflection.constantBufferMembers.size());
		mapHeader.InputElementRecordCount = static_cast<std::uint32_t>(reflection.inputElements.size());
		mapHeader.PushConstantRangeRecordCount = static_cast<std::uint32_t>(reflection.pushConstantRanges.size());
		mapHeader.SpecializationConstantRecordCount = static_cast<std::uint32_t>(reflection.specializationConstants.size());
		mapHeader.StringTableSizeInBytes = tables.Strings.SizeInBytes();
		std::ofstream output;
		std::string error;
		if (!Files::TryOpenBinaryOutput(path, output, error) || !Files::BinaryStreamWriter::WriteValue(output, mapHeader, error)
		    || !Files::BinaryStreamWriter::WriteArray(output, tables.Entries, error) || !Files::BinaryStreamWriter::WriteArray(output, tables.Bindings, error)
		    || !Files::BinaryStreamWriter::WriteArray(output, reflection.reflectionRecords, error) || !Files::BinaryStreamWriter::WriteArray(output, reflection.resourceBindings, error)
		    || !Files::BinaryStreamWriter::WriteArray(output, reflection.constantBuffers, error) || !Files::BinaryStreamWriter::WriteArray(output, reflection.constantBufferMembers, error)
		    || !Files::BinaryStreamWriter::WriteArray(output, reflection.inputElements, error) || !Files::BinaryStreamWriter::WriteArray(output, reflection.pushConstantRanges, error)
		    || !Files::BinaryStreamWriter::WriteArray(output, reflection.specializationConstants, error) || !Files::BinaryStreamWriter::WriteArray(output, tables.Strings.GetBytes(), error)
		    || !Files::TryCloseOutput(output, path, error))
		{
			throw Diagnostics::Error(error);
		}
	}

	static void WriteLibrary(const std::filesystem::path& path, std::uint64_t publicationHash, const std::vector<CookedShaderCodeRecord>& records, const std::vector<std::uint8_t>& code)
	{
		CookedShaderLibraryHeader libraryHeader{};
		libraryHeader.RecordCount = static_cast<std::uint32_t>(records.size());
		libraryHeader.PublicationHash = publicationHash;
		libraryHeader.CodeBlobSizeInBytes = static_cast<std::uint32_t>(code.size());

		std::ofstream output;
		std::string error;
		if (!Files::TryOpenBinaryOutput(path, output, error) || !Files::BinaryStreamWriter::WriteValue(output, libraryHeader, error) || !Files::BinaryStreamWriter::WriteArray(output, records, error)
		    || !Files::BinaryStreamWriter::WriteArray(output, code, error) || !Files::TryCloseOutput(output, path, error))
		{
			throw Diagnostics::Error(error);
		}
	}

	static ShaderCookOutput WriteArtifacts(std::vector<ArtifactEntry> entries, const std::filesystem::path& mapPath, const std::filesystem::path& libraryPath)
	{
		ShaderCookOutput result{.mapPath = mapPath, .libraryPath = libraryPath};
		ArtifactTables tables = BuildTables(entries, result);
		ShaderReflectionTables reflectionOutput;
		BuildShaderReflectionTables(tables.Reflections, tables.Strings, reflectionOutput);

		std::vector<std::uint8_t> codeBlob;
		const std::vector<CookedShaderCodeRecord> codeRecords = BuildCodeRecords(tables.UniqueCode, codeBlob);
		result.uniqueCodeCount = codeRecords.size();
		const std::uint64_t publicationHash = ComputePublicationHash(tables, reflectionOutput, codeRecords, codeBlob);
		WriteMap(mapPath, publicationHash, tables, reflectionOutput);
		WriteLibrary(libraryPath, publicationHash, codeRecords, codeBlob);
		return result;
	}

	static std::vector<ArtifactEntry> BuildEntries(const ShaderCookPipelinePlan& plan, bool replaceCompleteCatalog, const std::filesystem::path& mapPath, const std::filesystem::path& libraryPath)
	{
		std::map<std::pair<ShaderTypeId, ShaderTarget>, ArtifactEntry> entries;
		if (!replaceCompleteCatalog)
		{
			const CookedShaderLibrary existingLibrary = CookedShaderLibrary::Open(libraryPath);
			const GlobalShaderMap existingMap = GlobalShaderMap::Open(mapPath, existingLibrary);
			for (const GlobalShaderMapEntry& entry : existingMap.GetEntries())
			{
				if (!std::ranges::binary_search(plan.registeredShaderTypes, entry.ShaderType))
				{
					continue;
				}
				entries.emplace(std::pair{entry.ShaderType, entry.Target}, ReadExistingEntry(existingMap, existingLibrary, entry));
			}
		}
		for (std::size_t shaderIndex = 0; shaderIndex < plan.shaders.size(); ++shaderIndex)
		{
			for (const ShaderCookProduct& product : plan.shaderOutputs[shaderIndex])
			{
				ArtifactEntry entry = BuildEntryFromProduct(plan.shaders[shaderIndex], product);
				entries.insert_or_assign(std::pair{entry.ShaderType, entry.Target}, std::move(entry));
			}
		}
		std::vector<ArtifactEntry> orderedEntries;
		for (auto& [key, entry] : entries)
		{
			(void) key;
			orderedEntries.push_back(std::move(entry));
		}
		return orderedEntries;
	}

	ShaderCookOutput Write(
	    const ShaderCookPipelinePlan& plan,
	    bool replaceCompleteCatalog,
	    const std::filesystem::path& mapPath,
	    const std::filesystem::path& libraryPath,
	    const std::filesystem::path& stagedMapPath,
	    const std::filesystem::path& stagedLibraryPath)
	{
		std::vector<ArtifactEntry> entries = BuildEntries(plan, replaceCompleteCatalog, mapPath, libraryPath);
		return WriteArtifacts(std::move(entries), stagedMapPath, stagedLibraryPath);
	}

}
