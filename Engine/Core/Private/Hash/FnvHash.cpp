#include "PCH.h"

#include "Core/Public/Hash/HashUtils.h"

#include <array>
#include <fstream>

namespace Hash
{
	static constexpr std::size_t kHashBufferSize = 64 * 1024;

	uint64_t FinalizeFnv1a64(uint64_t hash) noexcept
	{
		return hash != 0 ? hash : kFnv64OffsetBasis;
	}

	uint64_t ContinueFnv1a64(uint64_t hash, const void* data, size_t size) noexcept
	{
		const auto* bytes = static_cast<const unsigned char*>(data);
		for (size_t index = 0; index < size; ++index)
		{
			hash ^= static_cast<uint64_t>(bytes[index]);
			hash *= kFnv64Prime;
		}
		return hash;
	}

	uint64_t Fnv1a64(std::string_view str) noexcept
	{
		return ContinueFnv1a64(kFnv64OffsetBasis, str.data(), str.size());
	}

	uint64_t Fnv1a64(const void* data, size_t size) noexcept
	{
		return ContinueFnv1a64(kFnv64OffsetBasis, data, size);
	}

	bool TryFnv1a64File(const std::filesystem::path& path, uint64_t& outHash, std::string& outErrorMessage)
	{
		std::ifstream input(path, std::ios::binary);
		if (!input.is_open())
		{
			outHash = 0;
			outErrorMessage = "Failed to open file for hashing: '" + path.string() + "'";
			return false;
		}

		uint64_t hash = kFnv64OffsetBasis;
		std::array<char, kHashBufferSize> buffer{};
		while (input.good())
		{
			input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
			const std::streamsize bytesRead = input.gcount();
			hash = ContinueFnv1a64(hash, buffer.data(), static_cast<std::size_t>(bytesRead));
		}

		if (!input.eof())
		{
			outHash = 0;
			outErrorMessage = "Failed to read file for hashing: '" + path.string() + "'";
			return false;
		}

		outHash = FinalizeFnv1a64(hash);
		outErrorMessage.clear();
		return true;
	}

	uint32_t Fnv1a32(std::string_view str) noexcept
	{
		uint32_t hash = kFnv32OffsetBasis;
		for (const char character : str)
		{
			hash ^= static_cast<uint32_t>(static_cast<unsigned char>(character));
			hash *= kFnv32Prime;
		}
		return hash;
	}
}
