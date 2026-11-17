#pragma once

#include <filesystem>

struct cgltf_data;

class GltfScene final
{
public:
	explicit GltfScene(const std::filesystem::path& filePath);
	~GltfScene();
	GltfScene(const GltfScene&) = delete;
	GltfScene& operator=(const GltfScene&) = delete;

	const cgltf_data& GetData() const noexcept { return *m_data; }

private:
	cgltf_data* m_data = nullptr;
};
