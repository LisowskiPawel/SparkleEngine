#include "PCH.h"

#include "Scene/Preparation/Geometry/RenderPrimitivePreparation.h"

#include <algorithm>

void PrepareRenderPrimitives(std::span<const ResolvedRenderPrimitive> inputs, std::span<PreparedRenderPrimitive> outputs) noexcept
{
	const std::size_t count = (std::min) (inputs.size(), outputs.size());
	for (std::size_t index = 0u; index < count; ++index)
	{
		const ResolvedRenderPrimitive& input = inputs[index];
		PreparedRenderPrimitive& output = outputs[index];

		output = PreparedRenderPrimitive{
		    .Object = input.Object,
		    .Draw = input.Draw,
		    .WorldBounds = RenderMeshWorldBounds::Transform(input.Draw.Geometry, input.WorldMatrix),
		    .Material = input.Material,
		    .InstanceGroupIndex = input.InstanceGroupIndex,
		    .MaterialAlphaMode = input.MaterialAlphaMode};

		output.Draw.Transform = MeshDrawTransform{.WorldMatrix = input.WorldMatrix, .PreviousWorldMatrix = input.PreviousWorldMatrix, .WorldInvTranspose = input.WorldInverseTranspose};
	}
}
