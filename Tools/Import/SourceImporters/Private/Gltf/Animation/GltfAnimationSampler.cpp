#include "PCH.h"

#include "Gltf/Animation/GltfAnimationSampler.h"
#include "Gltf/GltfCoordinateConverter.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <algorithm>
#include <cmath>
#include <format>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace GltfAnimationSampler
{
	static ImportedAnimationInterpolation ToImportedInterpolation(cgltf_interpolation_type interpolation)
	{
		switch (interpolation)
		{
			case cgltf_interpolation_type_step:
				return ImportedAnimationInterpolation::Step;
			case cgltf_interpolation_type_cubic_spline:
				return ImportedAnimationInterpolation::CubicSpline;
			case cgltf_interpolation_type_linear:
				return ImportedAnimationInterpolation::Linear;
			default:
				throw Diagnostics::Error("glTF animation sampler uses an unsupported interpolation mode.");
		}
	}

	static std::uint32_t GetValueComponentCount(ImportedAnimationTargetPath targetPath) noexcept
	{
		switch (targetPath)
		{
			case ImportedAnimationTargetPath::Translation:
			case ImportedAnimationTargetPath::Scale:
				return 3;
			case ImportedAnimationTargetPath::Rotation:
				return 4;
			case ImportedAnimationTargetPath::Weights:
			case ImportedAnimationTargetPath::Unknown:
			default:
				return 0;
		}
	}

	static DirectX::XMFLOAT4 ReadOutputValue(const cgltf_accessor* accessor, std::size_t index, std::uint32_t componentCount)
	{
		if (accessor == nullptr || index >= accessor->count || componentCount == 0 || componentCount > 4)
		{
			throw Diagnostics::Error(std::format("glTF animation output element {} is outside its accessor.", index));
		}

		cgltf_float values[4] = {};
		if (!cgltf_accessor_read_float(accessor, index, values, componentCount))
		{
			throw Diagnostics::Error(std::format("Cannot decode glTF animation output element {}.", index));
		}
		return {values[0], values[1], values[2], componentCount > 3 ? values[3] : 0.0f};
	}

	static DirectX::XMFLOAT4 ReadWeightOutputValue(const cgltf_accessor* accessor, std::size_t firstScalarIndex)
	{
		if (accessor == nullptr || firstScalarIndex > accessor->count || accessor->count - firstScalarIndex < 4u)
		{
			throw Diagnostics::Error(std::format("glTF animation weight element {} is outside its accessor.", firstScalarIndex));
		}

		DirectX::XMFLOAT4 value;
		float* outputComponents[4] = {&value.x, &value.y, &value.z, &value.w};
		for (std::size_t componentIndex = 0; componentIndex < 4u; ++componentIndex)
		{
			if (!cgltf_accessor_read_float(accessor, firstScalarIndex + componentIndex, outputComponents[componentIndex], 1))
			{
				throw Diagnostics::Error(std::format("Cannot decode glTF animation weight element {}.", firstScalarIndex + componentIndex));
			}
		}
		return value;
	}

	static float ReadInputTime(const cgltf_accessor* accessor, std::size_t index)
	{
		float time = 0.0f;
		if (accessor == nullptr || index >= accessor->count || !cgltf_accessor_read_float(accessor, index, &time, 1) || !std::isfinite(time))
		{
			throw Diagnostics::Error(std::format("Cannot decode glTF animation input time {}.", index));
		}
		return time;
	}

	static bool IsFinite(const DirectX::XMFLOAT4& value) noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
	}

	static DirectX::XMFLOAT4 ConvertTranslationValue(const DirectX::XMFLOAT4& source, bool tangent) noexcept
	{
		const DirectX::XMFLOAT3 converted = tangent ? GltfCoordinateConverter::ConvertTranslationTangent({source.x, source.y, source.z})
		                                            : GltfCoordinateConverter::ConvertTranslation({source.x, source.y, source.z});

		return {converted.x, converted.y, converted.z, 0.0f};
	}

	static DirectX::XMFLOAT4 ConvertAnimationValue(ImportedAnimationTargetPath targetPath, const DirectX::XMFLOAT4& source, bool tangent) noexcept
	{
		switch (targetPath)
		{
			case ImportedAnimationTargetPath::Translation:
				return ConvertTranslationValue(source, tangent);
			case ImportedAnimationTargetPath::Rotation:
				return tangent ? GltfCoordinateConverter::ConvertRotationTangent(source) : GltfCoordinateConverter::ConvertRotation(source);
			case ImportedAnimationTargetPath::Scale:
			case ImportedAnimationTargetPath::Weights:
			case ImportedAnimationTargetPath::Unknown:
			default:
				return source;
		}
	}

	static void NormalizeKeyframe(ImportedAnimationTargetPath targetPath, bool cubicSpline, const DirectX::XMFLOAT4* previousRotation, ImportedAnimationKeyframe& keyframe)
	{
		keyframe.value = ConvertAnimationValue(targetPath, keyframe.value, false);
		if (cubicSpline)
		{
			keyframe.inTangent = ConvertAnimationValue(targetPath, keyframe.inTangent, true);
			keyframe.outTangent = ConvertAnimationValue(targetPath, keyframe.outTangent, true);
		}
		if (!IsFinite(keyframe.value) || (cubicSpline && (!IsFinite(keyframe.inTangent) || !IsFinite(keyframe.outTangent))))
		{
			throw Diagnostics::Error("glTF animation contains a non-finite keyframe value or tangent.");
		}

		if (targetPath != ImportedAnimationTargetPath::Rotation)
		{
			return;
		}

		DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&keyframe.value);
		if (DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation)) <= 1.0e-8f)
		{
			throw Diagnostics::Error("glTF animation contains a zero-length rotation keyframe.");
		}
		rotation = DirectX::XMQuaternionNormalize(rotation);
		if (previousRotation != nullptr && DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(previousRotation), rotation)) < 0.0f)
		{
			rotation = DirectX::XMVectorNegate(rotation);
			if (cubicSpline)
			{
				DirectX::XMStoreFloat4(&keyframe.inTangent, DirectX::XMVectorNegate(DirectX::XMLoadFloat4(&keyframe.inTangent)));
				DirectX::XMStoreFloat4(&keyframe.outTangent, DirectX::XMVectorNegate(DirectX::XMLoadFloat4(&keyframe.outTangent)));
			}
		}
		DirectX::XMStoreFloat4(&keyframe.value, rotation);
	}

	static std::size_t ValidateSamplerAccessors(
	    const cgltf_animation_sampler& sampler,
	    ImportedAnimationTargetPath targetPath,
	    std::uint32_t morphWeightCount,
	    ImportedAnimationInterpolation interpolation,
	    std::uint32_t componentCount)
	{
		const cgltf_accessor* input = sampler.input;
		const cgltf_accessor* output = sampler.output;
		if (input == nullptr || output == nullptr || input->count == 0 || input->type != cgltf_type_scalar || input->component_type != cgltf_component_type_r_32f
		    || output->component_type != cgltf_component_type_r_32f)
		{
			throw Diagnostics::Error("glTF animation sampler has incompatible input or output accessors.");
		}

		const bool morphWeights = targetPath == ImportedAnimationTargetPath::Weights;
		if ((morphWeights && morphWeightCount != 4u) || (!morphWeights && componentCount == 0u))
		{
			throw Diagnostics::Error("glTF animation sampler has an incompatible target value shape.");
		}
		const cgltf_type expectedOutputType = morphWeights ? cgltf_type_scalar : componentCount == 3u ? cgltf_type_vec3 : cgltf_type_vec4;
		if (output->type != expectedOutputType)
		{
			throw Diagnostics::Error("glTF animation sampler output type differs from its target path.");
		}
		const bool cubicSpline = interpolation == ImportedAnimationInterpolation::CubicSpline;
		const std::size_t outputElementsPerKeyframe = (cubicSpline ? 3u : 1u) * (morphWeights ? morphWeightCount : 1u);
		if (input->count > (std::numeric_limits<std::size_t>::max)() / outputElementsPerKeyframe)
		{
			throw Diagnostics::Error("glTF animation sampler keyframe count exceeds the engine range.");
		}
		const std::size_t expectedOutputCount = input->count * outputElementsPerKeyframe;
		if (output->count != expectedOutputCount)
		{
			throw Diagnostics::Error("glTF animation sampler input and output counts do not agree.");
		}
		return outputElementsPerKeyframe;
	}

	static void ReadKeyframeValues(const cgltf_accessor* output, std::size_t baseIndex, std::uint32_t componentCount, bool morphWeights, bool cubicSpline, ImportedAnimationKeyframe& keyframe)
	{
		const auto readValue = [output, componentCount, morphWeights](std::size_t index)
		{
			return morphWeights ? ReadWeightOutputValue(output, index) : ReadOutputValue(output, index, componentCount);
		};

		if (cubicSpline)
		{
			const std::size_t stride = morphWeights ? 4u : 1u;
			keyframe.inTangent = readValue(baseIndex);
			keyframe.value = readValue(baseIndex + stride);
			keyframe.outTangent = readValue(baseIndex + 2u * stride);
		}
		else
		{
			keyframe.value = readValue(baseIndex);
		}
	}

	ImportedAnimationSampler Import(const cgltf_animation_sampler& sampler, ImportedAnimationTargetPath targetPath, std::uint32_t morphWeightCount, float& inOutClipDurationSeconds)
	{
		ImportedAnimationSampler importedSampler;
		importedSampler.interpolation = ToImportedInterpolation(sampler.interpolation);
		const std::uint32_t componentCount = GetValueComponentCount(targetPath);
		const bool morphWeights = targetPath == ImportedAnimationTargetPath::Weights;
		const bool cubicSpline = importedSampler.interpolation == ImportedAnimationInterpolation::CubicSpline;
		const std::size_t outputElementsPerKeyframe = ValidateSamplerAccessors(sampler, targetPath, morphWeightCount, importedSampler.interpolation, componentCount);
		const cgltf_accessor* input = sampler.input;
		const cgltf_accessor* output = sampler.output;
		importedSampler.keyframes.reserve(input->count);
		float previousTime = -1.0f;
		DirectX::XMFLOAT4 previousRotation{};
		bool hasPreviousRotation = false;
		for (cgltf_size keyframeIndex = 0; keyframeIndex < input->count; ++keyframeIndex)
		{
			ImportedAnimationKeyframe keyframe;
			keyframe.timeSeconds = ReadInputTime(input, keyframeIndex);
			if (keyframe.timeSeconds < 0.0f || keyframe.timeSeconds <= previousTime)
			{
				throw Diagnostics::Error(std::format("glTF animation keyframe {} is not strictly time ordered.", keyframeIndex));
			}

			const std::size_t outputBaseIndex = static_cast<std::size_t>(keyframeIndex) * outputElementsPerKeyframe;
			ReadKeyframeValues(output, outputBaseIndex, componentCount, morphWeights, cubicSpline, keyframe);

			NormalizeKeyframe(targetPath, cubicSpline, hasPreviousRotation ? &previousRotation : nullptr, keyframe);
			if (targetPath == ImportedAnimationTargetPath::Rotation)
			{
				previousRotation = keyframe.value;
				hasPreviousRotation = true;
			}
			inOutClipDurationSeconds = (std::max) (inOutClipDurationSeconds, keyframe.timeSeconds);
			importedSampler.keyframes.push_back(keyframe);
			previousTime = keyframe.timeSeconds;
		}

		return importedSampler;
	}
}
