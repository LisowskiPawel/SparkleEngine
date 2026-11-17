include(${CMAKE_SOURCE_DIR}/CMake/SparkleHostTool.cmake)
add_subdirectory(Support/ToolSupport)
add_subdirectory(Launcher/SparkleLauncher)

if(SPARKLE_ENABLE_CONTENT_PIPELINE)
	add_subdirectory(Cooking/TextureCooker)
	add_subdirectory(Import/SourceImporters)
	add_subdirectory(Cooking/MeshCooker)
	add_subdirectory(Cooking/MaterialCooker)
	add_subdirectory(Cooking/SceneCooker)
	add_subdirectory(Cooking/AssetCooker)
endif()

if(SPARKLE_ENABLE_SHADER_COMPILER)
	add_subdirectory(Shaders/ShaderCompiler)
endif()

set(SPARKLE_COOK_TOOL_TARGETS "")
if(SPARKLE_ENABLE_CONTENT_PIPELINE)
	list(APPEND SPARKLE_COOK_TOOL_TARGETS AssetCooker TextureCooker)
endif()
if(SPARKLE_ENABLE_SHADER_COMPILER)
	list(APPEND SPARKLE_COOK_TOOL_TARGETS ShaderCompiler)
endif()

add_custom_target(SparkleCookTools DEPENDS ${SPARKLE_COOK_TOOL_TARGETS})

sparkle_configure_host_tool_target(SparkleCookTools)
