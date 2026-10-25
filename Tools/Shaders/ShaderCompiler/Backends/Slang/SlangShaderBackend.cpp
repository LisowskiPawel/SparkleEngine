#include "PCH.h"

#include "Slang/SlangShaderBackend.h"

#include "Compiler/ShaderCompileProfile.h"
#include "Compiler/SpirV/SpirVBindingNormalizer.h"
#include "Compiler/SpirV/SpirVDisassembler.h"
#include "Compiler/SpirV/SpirVReflectionExtractor.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Slang/SlangReflectionExtractor.h"
#include "Slang/SlangSourceFiles.h"

#include <array>
#include <filesystem>
#include <algorithm>
#include <utility>

struct SlangShaderBackend::SymbolSelection final
{
	std::string Name;
	bool Failed = false;
};

SlangShaderBackend::SlangShaderBackend()
{
	if (SLANG_FAILED(slang::createGlobalSession(m_globalSession.writeRef())) || !m_globalSession)
	{
		m_backendVersion = Hash::Fnv1a64("slang-unavailable");
		return;
	}

	m_backendVersion = QueryBackendVersion(*m_globalSession.get());
}

ShaderBackendCapabilities SlangShaderBackend::GetStaticCapabilities() noexcept
{
	return ShaderBackendCapabilities{.SupportsDxil = true, .SupportsSpirV = true, .SupportsSpirVRayTracingLibrary = true, .SupportsSpirVInlineRayQuery = true};
}

std::uint64_t SlangShaderBackend::QueryBackendVersion()
{
	Slang::ComPtr<slang::IGlobalSession> globalSession;
	if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef())) || !globalSession)
	{
		return Hash::Fnv1a64("slang-unavailable");
	}
	return QueryBackendVersion(*globalSession.get());
}

std::uint64_t SlangShaderBackend::QueryBackendVersion(slang::IGlobalSession& globalSession)
{
	const char* buildTag = globalSession.getBuildTagString();
	return Hash::Fnv1a64(std::string_view(buildTag != nullptr ? buildTag : "slang-unknown"));
}

ShaderBackendCapabilities SlangShaderBackend::GetCapabilities() const
{
	ShaderBackendCapabilities capabilities = GetStaticCapabilities();
	capabilities.SupportsDxil = capabilities.SupportsDxil && IsValid();
	capabilities.SupportsSpirV = capabilities.SupportsSpirV && IsValid();
	capabilities.SupportsSpirVRayTracingLibrary = capabilities.SupportsSpirVRayTracingLibrary && IsValid();
	capabilities.SupportsSpirVInlineRayQuery = capabilities.SupportsSpirVInlineRayQuery && IsValid();
	return capabilities;
}

std::string_view SlangShaderBackend::GetBackendName() const
{
	return "slang";
}

std::uint64_t SlangShaderBackend::GetBackendVersion() const
{
	return m_backendVersion;
}

CompiledShader SlangShaderBackend::Compile(const ShaderCompileRequest& request)
{
	if (!IsValid())
	{
		throw Diagnostics::Error("Slang global session is unavailable");
	}

	const Slang::ComPtr<slang::ISession> session = CreateSession(request);
	std::string diagnostics;
	const Slang::ComPtr<slang::IComponentType> program = LinkProgram(*session.get(), request, diagnostics);
	std::vector<std::uint8_t> bytecode = ExtractBytecode(*program.get(), diagnostics);
	if (IsSpirVTarget(request.Target))
	{
		SpirVBindingNormalizer::Normalize(bytecode, request.DescriptorBindingRemaps);
	}

	ShaderReflection reflection = ExtractReflection(request, *program.get(), bytecode, diagnostics);
	ShaderDebugArtifactSet debugArtifacts;
	if (request.CaptureDebugArtifacts)
	{
		debugArtifacts = CaptureDebugArtifacts(request, *program.get(), bytecode, diagnostics);
	}

	CompiledShader compiledShader(std::move(bytecode));
	compiledShader.SetReflection(std::move(reflection));
	compiledShader.SetDebugArtifacts(std::move(debugArtifacts));
	return compiledShader;
}

Slang::ComPtr<slang::ISession> SlangShaderBackend::CreateSession(const ShaderCompileRequest& request) const
{
	std::vector<slang::PreprocessorMacroDesc> macroDescs;
	macroDescs.reserve(request.Defines.size());
	std::vector<std::string> defineNames;
	std::vector<std::string> defineValues;
	defineNames.reserve(request.Defines.size());
	defineValues.reserve(request.Defines.size());
	for (const std::string& define : request.Defines)
	{
		const std::size_t equals = define.find('=');
		defineNames.push_back(define.substr(0, equals));
		defineValues.push_back(equals != std::string::npos ? define.substr(equals + 1) : "1");
		macroDescs.push_back(slang::PreprocessorMacroDesc{defineNames.back().c_str(), defineValues.back().c_str()});
	}

	slang::TargetDesc targetDesc{};
	targetDesc.format = MapTarget(request.Target);
	targetDesc.profile = m_globalSession->findProfile(ShaderCompileProfile::GetSlangTargetProfileName(request.Target));
	if (targetDesc.format == SLANG_TARGET_UNKNOWN || targetDesc.profile == SLANG_PROFILE_UNKNOWN)
	{
		throw Diagnostics::Error("Slang backend does not support requested shader target");
	}

	std::array<slang::CompilerOptionEntry, 6> targetOptions{};
	std::uint32_t targetOptionCount = 0;
	const auto optimization = request.EnableOptimizations ? SLANG_OPTIMIZATION_LEVEL_MAXIMAL : SLANG_OPTIMIZATION_LEVEL_NONE;
	const auto debugInformation = request.EnableDebugInfo ? SLANG_DEBUG_INFO_LEVEL_STANDARD : SLANG_DEBUG_INFO_LEVEL_NONE;
	targetOptions[targetOptionCount++] = {slang::CompilerOptionName::Optimization, {slang::CompilerOptionValueKind::Int, static_cast<std::int32_t>(optimization), 0, nullptr, nullptr}};
	targetOptions[targetOptionCount++] = {slang::CompilerOptionName::DebugInformation, {slang::CompilerOptionValueKind::Int, static_cast<std::int32_t>(debugInformation), 0, nullptr, nullptr}};
	if (IsSpirVTarget(request.Target))
	{
		targetOptions[targetOptionCount++] = {slang::CompilerOptionName::EmitSpirvDirectly, {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}};
		targetOptions[targetOptionCount++] = {slang::CompilerOptionName::VulkanUseEntryPointName, {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}};
		targetOptions[targetOptionCount++] = {slang::CompilerOptionName::ForceDXLayout, {slang::CompilerOptionValueKind::Int, ShaderCompileProfile::SpirVUseDirectXBufferLayout, 0, nullptr, nullptr}};

		targetOptions[targetOptionCount++] = {
		    slang::CompilerOptionName::DefaultImageFormatUnknown,
		    {slang::CompilerOptionValueKind::Int, ShaderCompileProfile::SpirVUseUnknownStorageImageFormat, 0, nullptr, nullptr}};
	}
	else if (request.EnableDebugInfo)
	{
		targetOptions[targetOptionCount++] = {slang::CompilerOptionName::DebugInformationFormat, {slang::CompilerOptionValueKind::Int, SLANG_DEBUG_INFO_FORMAT_PDB, 0, nullptr, nullptr}};
	}
	targetDesc.compilerOptionEntries = targetOptions.data();
	targetDesc.compilerOptionEntryCount = targetOptionCount;

	slang::SessionDesc sessionDesc{};
	sessionDesc.targets = &targetDesc;
	sessionDesc.targetCount = 1;
	sessionDesc.searchPaths = nullptr;
	sessionDesc.searchPathCount = 0;
	sessionDesc.preprocessorMacros = macroDescs.data();
	sessionDesc.preprocessorMacroCount = static_cast<SlangInt>(macroDescs.size());
	Slang::ComPtr<ISlangFileSystem> sourceFiles;
	if (request.CaptureDebugArtifacts)
	{
		sourceFiles.attach(new SlangSourceFiles(request.SourceFiles));
		sessionDesc.fileSystem = sourceFiles.get();
	}

	Slang::ComPtr<slang::ISession> session;
	if (SLANG_FAILED(m_globalSession->createSession(sessionDesc, session.writeRef())) || !session)
	{
		throw Diagnostics::Error("Slang failed to create compile session");
	}

	return session;
}

Slang::ComPtr<slang::IComponentType> SlangShaderBackend::LinkProgram(slang::ISession& session, const ShaderCompileRequest& request, std::string& diagnostics)
{
	Slang::ComPtr<slang::IBlob> diagnosticBlob;
	constexpr std::string_view moduleName = "SparkleShader";
	// The expanded input is not the original file named by its #line directives.
	const std::string modulePath = request.CaptureDebugArtifacts ? "/SparkleCompilerInput.hlsl" : request.VirtualSourcePath;
	slang::IModule* module = session.loadModuleFromSourceString(moduleName.data(), modulePath.c_str(), request.SourceCode.c_str(), diagnosticBlob.writeRef());
	diagnostics += BlobToString(diagnosticBlob);
	if (module == nullptr)
	{
		throw Diagnostics::Error("Slang failed to load module - " + diagnostics);
	}

	Slang::ComPtr<slang::IEntryPoint> entryPoint;
	diagnosticBlob.setNull();
	const SlangStage stage = MapStage(request.Stage);
	SlangResult entryResult = module->findAndCheckEntryPoint(request.EntryPoint.c_str(), stage, entryPoint.writeRef(), diagnosticBlob.writeRef());
	diagnostics += BlobToString(diagnosticBlob);
	if (SLANG_FAILED(entryResult) || !entryPoint)
	{
		throw Diagnostics::Error("Slang failed to find entry point - " + diagnostics);
	}

	std::array<slang::IComponentType*, 2> components = {module, entryPoint.get()};
	Slang::ComPtr<slang::IComponentType> composedProgram;
	diagnosticBlob.setNull();
	SlangResult composeResult = session.createCompositeComponentType(components.data(), components.size(), composedProgram.writeRef(), diagnosticBlob.writeRef());
	diagnostics += BlobToString(diagnosticBlob);
	if (SLANG_FAILED(composeResult) || !composedProgram)
	{
		throw Diagnostics::Error("Slang failed to compose shader program - " + diagnostics);
	}

	Slang::ComPtr<slang::IComponentType> linkedProgram;
	diagnosticBlob.setNull();
	SlangResult linkResult = composedProgram->link(linkedProgram.writeRef(), diagnosticBlob.writeRef());
	diagnostics += BlobToString(diagnosticBlob);
	if (SLANG_FAILED(linkResult) || !linkedProgram)
	{
		throw Diagnostics::Error("Slang failed to link shader program - " + diagnostics);
	}
	if (request.CaptureDebugArtifacts)
	{
		for (SlangInt32 index = 0; index < module->getDependencyFileCount(); ++index)
		{
			const char* path = module->getDependencyFilePath(index);
			if (path == nullptr || (path != modulePath && std::ranges::find(request.SourceFiles, std::string_view(path), &ShaderSourceFile::VirtualPath) == request.SourceFiles.end()))
			{
				throw Diagnostics::Error("Slang read a source dependency outside the captured compile inputs.");
			}
		}
	}

	return linkedProgram;
}

std::vector<std::uint8_t> SlangShaderBackend::ExtractBytecode(slang::IComponentType& program, std::string& diagnostics)
{
	Slang::ComPtr<slang::IBlob> codeBlob;
	Slang::ComPtr<slang::IBlob> diagnosticBlob;
	SlangResult codeResult = program.getEntryPointCode(0, 0, codeBlob.writeRef(), diagnosticBlob.writeRef());
	diagnostics += BlobToString(diagnosticBlob);
	if (SLANG_FAILED(codeResult) || !codeBlob || codeBlob->getBufferSize() == 0)
	{
		throw Diagnostics::Error("Slang failed to emit target bytecode - " + diagnostics);
	}

	const auto* bytes = static_cast<const std::uint8_t*>(codeBlob->getBufferPointer());
	return {bytes, bytes + codeBlob->getBufferSize()};
}

ShaderReflection SlangShaderBackend::ExtractReflection(const ShaderCompileRequest& request, slang::IComponentType& program, std::span<const std::uint8_t> bytecode, std::string& diagnostics)
{
	ShaderReflection reflection;
	if (IsSpirVTarget(request.Target))
	{
		reflection = ExtractSpirVReflection(bytecode, request.Stage);
	}
	else
	{
		Slang::ComPtr<slang::IBlob> diagnosticBlob;
		slang::ProgramLayout* layout = program.getLayout(0, diagnosticBlob.writeRef());
		diagnostics += BlobToString(diagnosticBlob);
		if (layout == nullptr)
		{
			throw Diagnostics::Error(
			    "Slang failed to produce reflection layout for target '" + std::string{GetShaderTargetName(request.Target)} + "' source '" + request.VirtualSourcePath + "' entry '"
			    + request.EntryPoint + "' - " + diagnostics);
		}
		reflection = SlangReflectionExtractor::Extract(*layout, request.Stage);
	}

	return reflection;
}

SlangStage SlangShaderBackend::MapStage(ShaderStage stage)
{
	switch (stage)
	{
		case ShaderStage::Vertex:
			return SLANG_STAGE_VERTEX;
		case ShaderStage::Pixel:
			return SLANG_STAGE_FRAGMENT;
		case ShaderStage::Compute:
			return SLANG_STAGE_COMPUTE;
		case ShaderStage::Geometry:
			return SLANG_STAGE_GEOMETRY;
		case ShaderStage::Hull:
			return SLANG_STAGE_HULL;
		case ShaderStage::Domain:
			return SLANG_STAGE_DOMAIN;
		case ShaderStage::RayGeneration:
			return SLANG_STAGE_RAY_GENERATION;
		case ShaderStage::Miss:
			return SLANG_STAGE_MISS;
		case ShaderStage::ClosestHit:
			return SLANG_STAGE_CLOSEST_HIT;
		case ShaderStage::AnyHit:
			return SLANG_STAGE_ANY_HIT;
		case ShaderStage::Intersection:
			return SLANG_STAGE_INTERSECTION;
		case ShaderStage::Callable:
			return SLANG_STAGE_CALLABLE;
		default:
			return SLANG_STAGE_NONE;
	}
}

SlangCompileTarget SlangShaderBackend::MapTarget(ShaderTarget target)
{
	if (IsDxilTarget(target))
	{
		return SLANG_DXIL;
	}
	if (IsSpirVTarget(target))
	{
		return SLANG_SPIRV;
	}
	return SLANG_TARGET_UNKNOWN;
}

std::string SlangShaderBackend::BlobToString(slang::IBlob* blob)
{
	if (blob == nullptr || blob->getBufferPointer() == nullptr || blob->getBufferSize() == 0)
	{
		return {};
	}

	return std::string(static_cast<const char*>(blob->getBufferPointer()), blob->getBufferSize());
}

std::vector<std::string> SlangShaderBackend::BuildDebugArgumentStrings(const ShaderCompileRequest& request)
{
	std::vector<std::string> args;
	args.push_back("slang-api");
	args.push_back("-entry");
	args.push_back(request.EntryPoint);
	args.push_back("-target");
	args.push_back(IsSpirVTarget(request.Target) ? "spirv" : "dxil");
	args.push_back("-profile");
	args.push_back(ShaderCompileProfile::GetSlangTargetProfileName(request.Target));
	args.push_back(request.EnableOptimizations ? "-O3" : "-O0");
	args.push_back(request.EnableDebugInfo ? "-g2" : "-g0");
	if (!IsSpirVTarget(request.Target) && request.EnableDebugInfo)
	{
		args.push_back("-gpdb");
	}
	if (IsSpirVTarget(request.Target))
	{
		args.push_back("-emit-spirv-directly");
		args.push_back("-fvk-use-entrypoint-name");
		if constexpr (ShaderCompileProfile::SpirVUseDirectXBufferLayout)
		{
			args.push_back("-fvk-use-dx-layout");
		}
		if constexpr (ShaderCompileProfile::SpirVUseUnknownStorageImageFormat)
		{
			args.push_back("-default-image-format-unknown");
		}
	}
	return args;
}

ShaderDebugArtifactSet SlangShaderBackend::CaptureDebugArtifacts(
    const ShaderCompileRequest& request,
    slang::IComponentType& program,
    std::span<const std::uint8_t> bytecode,
    std::string_view diagnostics)
{
	ShaderDebugArtifactSet artifacts;
	artifacts.CompileArguments = BuildDebugArgumentStrings(request);
	artifacts.CompilerOutput.assign(diagnostics);
	artifacts.PreprocessedSource = request.SourceCode;
	if (IsSpirVTarget(request.Target))
	{
		artifacts.Disassembly = SpirVDisassembler::Disassemble(bytecode);
	}
	else
	{
		ExtractSymbols(program, artifacts);
	}
	Slang::ComPtr<slang::IBlob> compilerHash;
	program.getEntryPointHash(0, 0, compilerHash.writeRef());
	if (!compilerHash || compilerHash->getBufferSize() == 0)
	{
		throw Diagnostics::Error("Slang did not return the requested compiler entry-point hash.");
	}
	const auto* hashBytes = static_cast<const std::uint8_t*>(compilerHash->getBufferPointer());
	artifacts.CompilerLookupKey.assign(hashBytes, hashBytes + compilerHash->getBufferSize());
	return artifacts;
}

void SlangShaderBackend::ExtractSymbols(slang::IComponentType& program, ShaderDebugArtifactSet& artifacts)
{
	Slang::ComPtr<ISlangMutableFileSystem> outputs;
	if (SLANG_FAILED(program.getResultAsFileSystem(0, 0, outputs.writeRef())) || !outputs)
	{
		throw Diagnostics::Error("Slang did not return the requested compiler debug outputs.");
	}

	SymbolSelection selection;

	const auto select = [](SlangPathType type, const char* name, void* userData) noexcept
	{
		auto& output = *static_cast<SymbolSelection*>(userData);
		try
		{
			if (type == SLANG_PATH_TYPE_FILE && std::filesystem::path(name).extension() == ".pdb")
			{
				if (!output.Name.empty())
				{
					output.Failed = true;
				}
				output.Name = name;
			}
		}
		catch (...)
		{
			output.Failed = true;
		}
	};

	if (SLANG_FAILED(outputs->enumeratePathContents(".", select, &selection)) || selection.Failed || selection.Name.empty())
	{
		throw Diagnostics::Error("Slang did not return one unambiguous full shader PDB.");
	}
	Slang::ComPtr<ISlangBlob> pdb;
	if (SLANG_FAILED(outputs->loadFile(selection.Name.c_str(), pdb.writeRef())) || !pdb || pdb->getBufferSize() == 0)
	{
		throw Diagnostics::Error("Slang returned an unreadable shader PDB.");
	}
	const auto* bytes = static_cast<const std::uint8_t*>(pdb->getBufferPointer());
	artifacts.Symbols.assign(bytes, bytes + pdb->getBufferSize());
	artifacts.SymbolName = std::move(selection.Name);
}
