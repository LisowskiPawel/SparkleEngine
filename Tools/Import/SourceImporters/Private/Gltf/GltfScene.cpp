#include "PCH.h"

#include "Gltf/GltfScene.h"

#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <format>
#include <memory>
#include <string>

GltfScene::GltfScene(const std::filesystem::path& filePath)
{
	if (!std::filesystem::is_regular_file(filePath))
	{
		throw Diagnostics::Error(std::format("glTF source file does not exist: '{}'.", filePath.string()));
	}

	const std::string path = filePath.string();
	cgltf_options options{};
	cgltf_data* parsedData = nullptr;
	const cgltf_result parseResult = cgltf_parse_file(&options, path.c_str(), &parsedData);
	std::unique_ptr<cgltf_data, decltype(&cgltf_free)> data(parsedData, &cgltf_free);
	if (parseResult != cgltf_result_success)
	{
		throw Diagnostics::Error(std::format("Cannot parse glTF source '{}' (cgltf error {}).", path, static_cast<int>(parseResult)));
	}

	const cgltf_result bufferResult = cgltf_load_buffers(&options, data.get(), path.c_str());
	if (bufferResult != cgltf_result_success)
	{
		throw Diagnostics::Error(std::format("Cannot load glTF buffers for '{}' (cgltf error {}).", path, static_cast<int>(bufferResult)));
	}

	const cgltf_result validateResult = cgltf_validate(data.get());
	if (validateResult != cgltf_result_success)
	{
		throw Diagnostics::Error(std::format("glTF source '{}' fails cgltf validation (error {}).", path, static_cast<int>(validateResult)));
	}

	m_data = data.release();
}

GltfScene::~GltfScene()
{
	cgltf_free(m_data);
}
