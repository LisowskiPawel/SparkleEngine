#include "PCH.h"

#include "CookedSceneManifestWriter.h"

#include "CookedSceneBuild.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryFileWriter.h"

#include <limits>

namespace CookedSceneManifestWriter
{
	static std::uint32_t GetRecordCount(std::size_t count)
	{
		if (count > (std::numeric_limits<std::uint32_t>::max)())
		{
			throw Diagnostics::Error("Cooked scene manifest record count exceeds the cooked format.");
		}

		return static_cast<std::uint32_t>(count);
	}

	void Write(const CookedSceneManifestBuildData& manifest, const std::filesystem::path& outputPath)
	{
		const Assets::CookedSceneManifestHeader header{
		    .meshAssetReferenceCount = GetRecordCount(manifest.meshAssetReferences.size()),
		    .materialAssetReferenceCount = GetRecordCount(manifest.materialAssetReferences.size()),
		    .instanceCount = GetRecordCount(manifest.instances.size()),
		    .instanceGroupCount = GetRecordCount(manifest.instanceGroups.size()),
		    .cameraCount = GetRecordCount(manifest.cameras.size()),
		    .lightCount = GetRecordCount(manifest.lights.size()),
		    .skeletonRefCount = GetRecordCount(manifest.skeletonRefs.size()),
		    .animationRefCount = GetRecordCount(manifest.animationReferences.size()),
		    .morphWeightCount = GetRecordCount(manifest.morphWeights.size()),
		    .materialVariantCount = GetRecordCount(manifest.materialVariants.size()),
		    .materialVariantMappingCount = GetRecordCount(manifest.materialVariantMappings.size()),
		    .featureFlags = manifest.featureFlags};

		Files::WriteBinaryFile(
		    outputPath,
		    header,
		    manifest.meshAssetReferences,
		    manifest.materialAssetReferences,
		    manifest.instances,
		    manifest.instanceGroups,
		    manifest.cameras,
		    manifest.lights,
		    manifest.skeletonRefs,
		    manifest.animationReferences,
		    manifest.morphWeights,
		    manifest.materialVariants,
		    manifest.materialVariantMappings);
	}
}
