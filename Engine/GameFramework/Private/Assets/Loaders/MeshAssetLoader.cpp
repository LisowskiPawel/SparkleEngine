#include "PCH.h"
#include "Assets/Loaders/MeshAssetLoader.h"
#include "Assets/Cooked/CookedMeshAsset.h"
#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"

#include <span>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iterator>
#include <utility>

namespace Assets
{
	template <typename T> static T ReadMeshRecord(std::span<const std::uint8_t> bytes, std::size_t index) noexcept
	{
		T value;
		std::memcpy(&value, bytes.data() + index * sizeof(T), sizeof(T));
		return value;
	}

	static void ValidateMeshHeader(const CookedMeshAssetHeader& header)
	{
		const bool bCurrentVertexLayout = header.fileHeader.HasMagic(kCookedMeshAssetMagic) && header.vertexStride == sizeof(CookedMeshVertex);
		if (!bCurrentVertexLayout || header.indexStride != sizeof(std::uint32_t))
		{
			throw Diagnostics::Error("Invalid cooked mesh asset header; recook the asset");
		}
		if (header.assetKind != CookedMeshAssetKind::Static && header.assetKind != CookedMeshAssetKind::Skeletal)
		{
			throw Diagnostics::Error("Invalid cooked mesh asset kind");
		}
	}

	static void ValidateSkinStream(const CookedMeshAssetHeader& header)
	{
		const bool bHasSkin = (header.flags & static_cast<std::uint32_t>(CookedMeshAssetFlag::HasSkinInfluences)) != 0u;
		const bool bSkeletal = header.assetKind == CookedMeshAssetKind::Skeletal;
		const auto expectedCount = bSkeletal ? header.vertexCount : 0u;
		const bool bValidCount = bHasSkin == bSkeletal && header.skinInfluenceCount == expectedCount;
		if (!bValidCount || header.skinInfluenceStride != sizeof(CookedMeshSkinInfluence))
		{
			throw Diagnostics::Error("Invalid cooked mesh skin influence stream");
		}
	}

	static void ValidateMorphStream(const CookedMeshAssetHeader& header)
	{
		const bool bHasMorph = (header.flags & static_cast<std::uint32_t>(CookedMeshAssetFlag::HasMorphTargets)) != 0u;
		const bool bNonempty = header.morphTargetCount != 0u && header.morphTargetDeltaCount != 0u;
		const bool bEmpty = header.morphTargetCount == 0u && header.morphTargetDeltaCount == 0u;
		const bool bValidNonempty = bHasMorph && bNonempty;
		const bool bValidEmpty = !bHasMorph && bEmpty;
		const bool bValidCounts = bValidNonempty || bValidEmpty;
		const bool bValidKind = !bHasMorph || header.assetKind == CookedMeshAssetKind::Skeletal;
		const bool bValidLayout = header.morphTargetRecordStride == sizeof(CookedMeshMorphTargetRecord) && header.morphTargetDeltaStride == sizeof(CookedMeshMorphTargetDelta);
		if (!bValidCounts || !bValidKind)
		{
			throw Diagnostics::Error("Invalid cooked mesh morph target stream");
		}
		if (!bValidLayout)
		{
			throw Diagnostics::Error("Invalid cooked mesh morph target layout");
		}
	}

	static MeshData DecodeGeometry(CookedAssetByteReader& reader, const CookedMeshAssetHeader& header, CookedAssetAllocationBudget& budget)
	{
		const auto vertices = reader.ReadArrayBytes<CookedMeshVertex>(header.vertexCount);
		MeshData geometry;
		budget.ReserveArray<VertexData>(header.vertexCount);
		geometry.vertices.resize(header.vertexCount);
		for (std::size_t index = 0; index < geometry.vertices.size(); ++index)
		{
			const auto vertex = ReadMeshRecord<CookedMeshVertex>(vertices, index);
			geometry.vertices[index] = VertexData(vertex.position, vertex.uv, vertex.color, vertex.normal, vertex.tangent);
		}
		geometry.indices = reader.ReadArray<std::uint32_t>(header.indexCount);
		return geometry;
	}

	static MeshMorphTarget DecodeMorphTarget(const CookedMeshMorphTargetRecord& record, std::span<const std::uint8_t> deltas, const CookedMeshAssetHeader& header, CookedAssetAllocationBudget& budget)
	{
		const bool bOffsetValid = record.firstDelta <= header.morphTargetDeltaCount;
		const bool bRangeValid = bOffsetValid && record.deltaCount <= header.morphTargetDeltaCount - record.firstDelta;
		if (!bRangeValid || record.deltaCount != header.vertexCount)
		{
			throw Diagnostics::Error("Cooked mesh morph target references an invalid delta range");
		}
		if (!Strings::IsNullTerminated(std::span(record.name)) || !std::isfinite(record.defaultWeight))
		{
			throw Diagnostics::Error("Cooked mesh morph target metadata is invalid");
		}

		MeshMorphTarget target;
		budget.Reserve(sizeof(record.name));
		target.name = record.name;
		target.defaultWeight = record.defaultWeight;
		budget.ReserveArray<MeshMorphTargetDelta>(record.deltaCount);
		target.deltas.resize(record.deltaCount);
		for (std::uint32_t index = 0; index < record.deltaCount; ++index)
		{
			const auto delta = ReadMeshRecord<CookedMeshMorphTargetDelta>(deltas, record.firstDelta + index);
			target.deltas[index] = MeshMorphTargetDelta{.position = delta.position, .normal = delta.normal, .tangent = delta.tangent};
		}
		return target;
	}

	static MeshMorphData DecodeMorphTargets(
	    std::span<const CookedMeshMorphTargetRecord> records,
	    std::span<const std::uint8_t> deltas,
	    const CookedMeshAssetHeader& header,
	    CookedAssetAllocationBudget& budget)
	{
		MeshMorphData targets;
		budget.ReserveArray<MeshMorphTarget>(records.size());
		targets.reserve(records.size());
		for (const auto& record : records)
		{
			MeshMorphTarget target = DecodeMorphTarget(record, deltas, header, budget);
			targets.push_back(std::move(target));
		}
		return targets;
	}

	static std::vector<VertexSkinInfluence> DecodeSkinInfluences(std::span<const std::uint8_t> records, std::uint32_t count, CookedAssetAllocationBudget& budget)
	{
		budget.ReserveArray<VertexSkinInfluence>(count);
		std::vector<VertexSkinInfluence> influences(count);
		for (std::size_t index = 0; index < influences.size(); ++index)
		{
			const auto record = ReadMeshRecord<CookedMeshSkinInfluence>(records, index);
			std::copy(std::begin(record.jointIndices), std::end(record.jointIndices), std::begin(influences[index].jointIndices));
			std::copy(std::begin(record.jointWeights), std::end(record.jointWeights), std::begin(influences[index].jointWeights));
		}
		return influences;
	}

	LoadedMeshAsset MeshAssetLoader::Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget)
	{
		CookedAssetByteReader reader(bytes, budget);
		const auto header = reader.Read<CookedMeshAssetHeader>();
		ValidateMeshHeader(header);
		ValidateSkinStream(header);
		ValidateMorphStream(header);

		MeshData geometry = DecodeGeometry(reader, header, budget);
		const auto skin = reader.ReadArrayBytes<CookedMeshSkinInfluence>(header.skinInfluenceCount);
		const auto morphRecords = reader.ReadArray<CookedMeshMorphTargetRecord>(header.morphTargetCount);
		const auto morphDeltas = reader.ReadArrayBytes<CookedMeshMorphTargetDelta>(header.morphTargetDeltaCount);
		if (reader.GetRemainingByteCount() != 0)
		{
			throw Diagnostics::Error("Cooked mesh asset contains unexpected trailing bytes");
		}

		MeshMorphData morphs = DecodeMorphTargets(morphRecords, morphDeltas, header, budget);
		if (header.assetKind == CookedMeshAssetKind::Skeletal)
		{
			std::vector<VertexSkinInfluence> skinInfluences = DecodeSkinInfluences(skin, header.skinInfluenceCount, budget);
			SkeletalMeshData mesh{std::move(geometry), std::move(morphs), std::move(skinInfluences)};
			return LoadedMeshAsset{.payload = std::move(mesh)};
		}
		return LoadedMeshAsset{.payload = std::move(geometry)};
	}
}
