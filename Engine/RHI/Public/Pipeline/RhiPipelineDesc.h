#pragma once

#include "../Formats/CompareOp.h"
#include "../Formats/PixelFormat.h"
#include "../RHIAPI.h"
#include "../Resources/RhiResourceDesc.h"

#include <array>
#include <cstddef>
#include <cstdint>

class RenderBindingLayout;
struct ResolvedShader;

enum class ERhiCullMode : std::uint8_t
{
	None = 0,
	Front = 1,
	Back = 2,
};

enum class ERhiFrontFaceWinding : std::uint8_t
{
	Clockwise = 0,
	CounterClockwise = 1,
};

enum class RhiStencilOp : std::uint8_t
{
	Keep = 0,
	Zero = 1,
	Replace = 2,
	IncrementClamp = 3,
	DecrementClamp = 4,
	Invert = 5,
	IncrementWrap = 6,
	DecrementWrap = 7,
};

enum class RhiFillMode : std::uint8_t
{
	Solid = 0,
	Wireframe = 1,
};

enum class RhiBlendFactor : std::uint8_t
{
	Zero = 0,
	One = 1,
	SourceAlpha = 2,
	InverseSourceAlpha = 3,
};

enum class RhiBlendOperation : std::uint8_t
{
	Add = 0,
};

enum class RhiVertexElementFormat : std::uint8_t
{
	Float2 = 0,
	Float3 = 1,
	Float4 = 2,
};

enum class RhiVertexSemantic : std::uint8_t
{
	Position = 0,
	TexCoord = 1,
	Normal = 2,
	Tangent = 3,
};

struct RhiBlendTargetState
{
	bool bBlendEnable = false;
	RhiBlendFactor SourceColor = RhiBlendFactor::One;
	RhiBlendFactor DestinationColor = RhiBlendFactor::Zero;
	RhiBlendOperation ColorOperation = RhiBlendOperation::Add;
	RhiBlendFactor SourceAlpha = RhiBlendFactor::One;
	RhiBlendFactor DestinationAlpha = RhiBlendFactor::Zero;
	RhiBlendOperation AlphaOperation = RhiBlendOperation::Add;
	std::uint8_t ColorWriteMask = 0x0F;

	bool operator==(const RhiBlendTargetState&) const noexcept = default;
};

struct RhiBlendState
{
	bool bAlphaToCoverageEnable = false;
	bool bIndependentBlendEnable = false;
	std::array<RhiBlendTargetState, 8> Targets = {};

	SPARKLE_RHI_API bool operator==(const RhiBlendState& other) const noexcept;
};

struct RhiRasterizerState
{
	RhiFillMode FillMode = RhiFillMode::Solid;
	ERhiCullMode CullMode = ERhiCullMode::Back;
	ERhiFrontFaceWinding FrontFaceWinding = ERhiFrontFaceWinding::Clockwise;
	bool bDepthClipEnable = true;

	bool operator==(const RhiRasterizerState&) const noexcept = default;
};

struct RhiDepthState
{
	bool bDepthEnable = true;
	bool bDepthWriteEnable = true;
	CompareOp DepthFunc = CompareOp::Less;

	SPARKLE_RHI_API bool IsValid() const noexcept;

	bool operator==(const RhiDepthState&) const noexcept = default;
};

struct RhiStencilState
{
	bool bStencilEnable = false;
	std::uint8_t StencilReadMask = 0xFF;
	std::uint8_t StencilWriteMask = 0xFF;
	CompareOp FrontFaceStencilFunc = CompareOp::Always;
	RhiStencilOp FrontFaceStencilFailOp = RhiStencilOp::Keep;
	RhiStencilOp FrontFaceStencilDepthFailOp = RhiStencilOp::Keep;
	RhiStencilOp FrontFaceStencilPassOp = RhiStencilOp::Keep;
	CompareOp BackFaceStencilFunc = CompareOp::Always;
	RhiStencilOp BackFaceStencilFailOp = RhiStencilOp::Keep;
	RhiStencilOp BackFaceStencilDepthFailOp = RhiStencilOp::Keep;
	RhiStencilOp BackFaceStencilPassOp = RhiStencilOp::Keep;

	SPARKLE_RHI_API bool HasWrites() const noexcept;

	bool operator==(const RhiStencilState&) const noexcept = default;
};

struct RhiVertexInputBinding
{
	std::uint32_t Binding = 0;
	std::uint32_t StrideInBytes = 0;
	bool bPerInstance = false;

	bool operator==(const RhiVertexInputBinding&) const noexcept = default;
};

struct RhiVertexInputElement
{
	RhiVertexSemantic Semantic = RhiVertexSemantic::Position;
	std::uint8_t SemanticIndex = 0;
	std::uint8_t Location = 0;
	std::uint8_t Binding = 0;
	RhiVertexElementFormat Format = RhiVertexElementFormat::Float3;
	std::uint32_t OffsetInBytes = 0;

	bool operator==(const RhiVertexInputElement&) const noexcept = default;
};

struct RhiVertexInputDeclaration
{
	std::array<RhiVertexInputBinding, 4> Bindings = {};
	std::array<RhiVertexInputElement, 16> Elements = {};
	std::uint8_t BindingCount = 0;
	std::uint8_t ElementCount = 0;

	SPARKLE_RHI_API bool operator==(const RhiVertexInputDeclaration& other) const noexcept;
};

struct GraphicsPipelineDesc
{
	const RenderBindingLayout* BindingLayout = nullptr;
	const ResolvedShader* VertexShader = nullptr;
	const ResolvedShader* PixelShader = nullptr;
	RhiBlendState Blend = {};
	RhiRasterizerState Rasterizer = {};
	RhiDepthState Depth = {};
	RhiStencilState Stencil = {};
	RhiPrimitiveTopology PrimitiveTopology = RhiPrimitiveTopology::TriangleList;
	RhiVertexInputDeclaration VertexInput = {};
	std::array<PixelFormat, 8> ColorAttachmentFormats = {};
	std::uint32_t ColorAttachmentCount = 0;
	PixelFormat DepthStencilAttachmentFormat = PixelFormat::Unknown;
	std::uint8_t SampleCount = 1;
	const wchar_t* DebugName = L"RHI_GraphicsPipeline";

	static SPARKLE_RHI_API bool IsValidSampleCount(std::uint8_t sampleCount) noexcept;
};

struct ComputePipelineDesc
{
	const RenderBindingLayout* BindingLayout = nullptr;
	const ResolvedShader* ComputeShader = nullptr;
	const wchar_t* DebugName = L"RHI_ComputePipeline";
};

class SPARKLE_RHI_API RenderPipeline
{
public:
	virtual ~RenderPipeline() noexcept;

	RenderPipeline(const RenderPipeline&) = delete;
	RenderPipeline& operator=(const RenderPipeline&) = delete;
	RenderPipeline(RenderPipeline&&) = delete;
	RenderPipeline& operator=(RenderPipeline&&) = delete;

protected:
	RenderPipeline() noexcept;
};
