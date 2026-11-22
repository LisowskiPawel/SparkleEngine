#include "PCH.h"

#include "Backend/Slang/SlangShaderBackend.h"

#include "Compiler/ShaderCompileProfile.h"
#include "Compiler/SpirV/SpirVBindingNormalizer.h"
#include "Compiler/SpirV/SpirVDisassembler.h"
#include "Compiler/SpirV/SpirVReflectionExtractor.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Backend/Slang/SlangReflectionExtractor.h"
#include "Backend/Slang/SlangSourceFiles.h"

#include <array>
#include <filesystem>
#include <algorithm>
#include <utility>

static constexpr std::string_view kSlangModuleName = "SparkleShader";

struct SlangShaderBackend::SymbolSelection final
{
	std::string Name;
	bool bFailed = false;
};

SlangShaderBackend::SlangShaderBackend()
{
	const SlangResult createStatus = slang::createGlobalSession(m_globalSession.writeRef());
	if (SLANG_FAILED(createStatus) || !m_globalSession)
	{
		throw Diagnostics::Error("Slang global session is unavailable.");
	}

	m_backendVersion = QueryBackendVersion(*m_globalSession.get());
}

SlangShaderBackend::~SlangShaderBackend() = default;

ShaderBackendCapabilities SlangShaderBackend::GetStaticCapabilities() noexcept
{
	return ShaderBackendCapabilities{.bSupportsDxil = true, .bSupportsSpirV = true, .bSupportsSpirVRayTracingLibrary = true, .bSupportsSpirVInlineRayQuery = true};
}

std::uint64_t SlangShaderBackend::QueryBackendVersion()
{
	Slang::ComPtr<slang::IGlobalSession> globalSession;
	const SlangResult createStatus = slang::createGlobalSession(globalSession.writeRef());
	if (SLANG_FAILED(createStatus) || !globalSession)
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
	std::vector<std::string> arguments = BuildCompileArguments(request);
	const Slang::ComPtr<slang::ISession> session = CreateSession(request, arguments);
	std::string diagnostics;
	const Slang::ComPtr<slang::IComponentType> program = LinkProgram(*session.get(), request, diagnostics);
	std::vector<std::uint8_t> bytecode = ExtractBytecode(*program.get(), diagnostics);
	if (IsSpirVTarget(request.Target))
	{
		NormalizeSpirVBindings(bytecode, request.DescriptorBindingRemaps);
	}

	ShaderReflection reflection = ExtractReflection(request, *program.get(), bytecode, diagnostics);
	ShaderDebugArtifactSet debugArtifacts;
	if (request.bCaptureDebugArtifacts)
	{
		debugArtifacts = CaptureDebugArtifacts(request, *program.get(), bytecode, diagnostics, std::move(arguments));
	}

	CompiledShader compiledShader(std::move(bytecode));
	compiledShader.SetReflection(std::move(reflection));
	compiledShader.SetDebugArtifacts(std::move(debugArtifacts));
	return compiledShader;
}

Slang::ComPtr<slang::ISession> SlangShaderBackend::CreateSession(const ShaderCompileRequest& request, std::span<const std::string> arguments) const
{
	std::vector<const char*> argumentPointers;
	argumentPointers.reserve(arguments.size());
	for (const std::string& argument : arguments)
	{
		argumentPointers.push_back(argument.c_str());
	}

	slang::SessionDesc sessionDesc{};
	Slang::ComPtr<ISlangUnknown> optionStorage;
	const SlangResult parseStatus = m_globalSession->parseCommandLineArguments(static_cast<int>(argumentPointers.size()), argumentPointers.data(), &sessionDesc, optionStorage.writeRef());
	const bool bTargetMissing = sessionDesc.targetCount != 1 || sessionDesc.targets == nullptr;
	if (SLANG_FAILED(parseStatus) || bTargetMissing)
	{
		throw Diagnostics::Error("Slang failed to parse the requested compiler options.");
	}

	const slang::TargetDesc& target = sessionDesc.targets[0];
	const bool bTargetMatches = target.format == MapTarget(request.Target);
	const bool bProfileMatches = target.profile == m_globalSession->findProfile(GetSlangTargetProfileName(request.Target));
	if (!bTargetMatches || !bProfileMatches)
	{
		throw Diagnostics::Error("Slang compiler options do not match the requested shader target.");
	}

	Slang::ComPtr<ISlangFileSystem> sourceFiles;
	if (request.bCaptureDebugArtifacts)
	{
		sourceFiles.attach(new SlangSourceFiles(request.SourceFiles));
		sessionDesc.fileSystem = sourceFiles.get();
	}

	Slang::ComPtr<slang::ISession> session;
	const SlangResult createStatus = m_globalSession->createSession(sessionDesc, session.writeRef());
	if (SLANG_FAILED(createStatus) || !session)
	{
		throw Diagnostics::Error("Slang failed to create compile session");
	}

	return session;
}

Slang::ComPtr<slang::IComponentType> SlangShaderBackend::LinkProgram(slang::ISession& session, const ShaderCompileRequest& request, std::string& diagnostics)
{
	Slang::ComPtr<slang::IBlob> diagnosticBlob;
	// The expanded input is not the original file named by its #line directives.
	const std::string modulePath = request.bCaptureDebugArtifacts ? "/SparkleCompilerInput.hlsl" : request.VirtualSourcePath;
	slang::IModule* module = session.loadModuleFromSourceString(kSlangModuleName.data(), modulePath.c_str(), request.SourceCode.c_str(), diagnosticBlob.writeRef());
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
	if (request.bCaptureDebugArtifacts)
	{
		for (SlangInt32 index = 0; index < module->getDependencyFileCount(); ++index)
		{
			const char* path = module->getDependencyFilePath(index);
			const bool bSourceFileUnregisteredPath = path == nullptr;
			const bool bDifferentModulePath = !bSourceFileUnregisteredPath && (path != modulePath);

			const bool bSourceFileUnregisteredSourceFiles = bDifferentModulePath
			    && (std::ranges::find(request.SourceFiles, std::string_view(path), &ShaderSourceFile::VirtualPath) == request.SourceFiles.end());

			const bool bSourceFileUnregistered = bSourceFileUnregisteredPath || bSourceFileUnregisteredSourceFiles;

			if (bSourceFileUnregistered)
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
		reflection = ExtractSlangReflection(*layout, request.Stage);
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

std::vector<std::string> SlangShaderBackend::BuildCompileArguments(const ShaderCompileRequest& request)
{
	const bool bSpirVTarget = IsSpirVTarget(request.Target);
	std::vector<std::string> args;
	args.push_back("-entry");
	args.push_back(request.EntryPoint);
	args.push_back("-target");
	args.push_back(bSpirVTarget ? "spirv" : "dxil");
	args.push_back("-profile");
	args.push_back(GetSlangTargetProfileName(request.Target));
	args.push_back("-module-name");
	args.emplace_back(kSlangModuleName);
	args.push_back("-matrix-layout-row-major");
	args.push_back(request.bEnableOptimizations ? "-O3" : "-O0");
	args.push_back(request.bEnableDebugInfo ? "-g2" : "-g0");
	if (request.bTreatWarningsAsErrors)
	{
		args.push_back("-warnings-as-errors");
		args.push_back("all");
	}
	for (const std::string& define : request.Defines)
	{
		args.push_back("-D");
		args.push_back(define.find('=') == std::string::npos ? define + "=1" : define);
	}
	if (bSpirVTarget)
	{
		args.push_back("-emit-spirv-directly");
		args.push_back("-fvk-use-entrypoint-name");
		if constexpr (bSpirVUseDirectXBufferLayout)
		{
			args.push_back("-fvk-use-dx-layout");
		}
		if constexpr (bSpirVUseUnknownStorageImageFormat)
		{
			args.push_back("-default-image-format-unknown");
		}
	}
	else
	{
		if (request.bEnableDebugInfo)
		{
			args.push_back("-gpdb");
		}
		if (request.bStripDebugInfo)
		{
			args.push_back("-Xdxc");
			args.push_back("-Qstrip_debug");
		}
	}
	return args;
}

ShaderDebugArtifactSet SlangShaderBackend::CaptureDebugArtifacts(
    const ShaderCompileRequest& request,
    slang::IComponentType& program,
    std::span<const std::uint8_t> bytecode,
    std::string_view diagnostics,
    std::vector<std::string> arguments)
{
	ShaderDebugArtifactSet artifacts;
	artifacts.CompileArguments = std::move(arguments);
	artifacts.CompileArguments.insert(artifacts.CompileArguments.begin(), "slang-api");
	artifacts.CompilerOutput.assign(diagnostics);
	artifacts.PreprocessedSource = request.SourceCode;
	if (IsSpirVTarget(request.Target))
	{
		artifacts.Disassembly = DisassembleSpirV(bytecode);
	}
	else
	{
		ExtractSymbols(program, artifacts);
	}
	Slang::ComPtr<slang::IBlob> compilerHash;
	program.getEntryPointHash(0, 0, compilerHash.writeRef());
	if (compilerHash && compilerHash->getBufferPointer() != nullptr && compilerHash->getBufferSize() != 0)
	{
		const auto* hashBytes = static_cast<const std::uint8_t*>(compilerHash->getBufferPointer());
		artifacts.CompilerLookupKey.assign(hashBytes, hashBytes + compilerHash->getBufferSize());
	}
	return artifacts;
}

void SlangShaderBackend::ExtractSymbols(slang::IComponentType& program, ShaderDebugArtifactSet& artifacts)
{
	Slang::ComPtr<ISlangMutableFileSystem> outputs;
	const SlangResult outputStatus = program.getResultAsFileSystem(0, 0, outputs.writeRef());
	if (SLANG_FAILED(outputStatus) || !outputs)
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
					output.bFailed = true;
				}
				output.Name = name;
			}
		}
		catch (...)
		{
			output.bFailed = true;
		}
	};

	const SlangResult enumerateStatus = outputs->enumeratePathContents(".", select, &selection);
	if (SLANG_FAILED(enumerateStatus) || selection.bFailed || selection.Name.empty())
	{
		throw Diagnostics::Error("Slang did not return one unambiguous full shader PDB.");
	}
	Slang::ComPtr<ISlangBlob> pdb;
	const SlangResult loadStatus = outputs->loadFile(selection.Name.c_str(), pdb.writeRef());
	if (SLANG_FAILED(loadStatus) || !pdb || pdb->getBufferSize() == 0)
	{
		throw Diagnostics::Error("Slang returned an unreadable shader PDB.");
	}
	const auto* bytes = static_cast<const std::uint8_t*>(pdb->getBufferPointer());
	artifacts.Symbols.assign(bytes, bytes + pdb->getBufferSize());
	artifacts.SymbolName = std::move(selection.Name);
}
