set(TEXTURE_COOK_SHARED_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Public/TextureCookRequestList.h
)

set(TEXTURE_COOK_SHARED_PRIVATE_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Private/Requests/TextureCookRequestCodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Requests/TextureCookRequestList.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Private/Requests/TextureCookRequestSet.cpp
)

set(TEXTURE_COOK_SHARED_PRIVATE_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/Private/Requests/TextureCookRequestCodec.h
)

set(TEXTURE_COOKER_PRIVATE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Processing/TextureProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/SourceLoading/TextureSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/SourceLoading/KtxTextureSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/App/TextureCookerApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/CookTextureCookRequestFileCommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/InspectTextureCookRequestFileCommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureCookRequestBatchProcessor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureCookBatchExecutor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureCookMemoryLimiter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureAssetCooker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookedTextureAssetWriter.cpp
)

set(TEXTURE_COOKER_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Processing/TextureProcessor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/SourceLoading/TextureSource.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/App/TextureCookerApplication.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/CookTextureCookRequestFileCommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cli/InspectTextureCookRequestFileCommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Constants/TextureCookerConstants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureCookRequestBatchProcessor.h
	${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureCookBatchExecutor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureCookMemoryLimiter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/TextureAssetCooker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/Cooking/CookedTextureAssetWriter.h
)

add_library(TextureCookShared STATIC
    ${TEXTURE_COOK_SHARED_PUBLIC_HEADERS}
    ${TEXTURE_COOK_SHARED_PRIVATE_SOURCES}
    ${TEXTURE_COOK_SHARED_PRIVATE_HEADERS}
)
sparkle_configure_host_tool_target(TextureCookShared)

target_include_directories(TextureCookShared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Engine
)

target_compile_features(TextureCookShared PUBLIC cxx_std_20)

target_link_libraries(TextureCookShared
    PUBLIC
        SparkleCore
)

add_executable(TextureCooker
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp
    ${TEXTURE_COOKER_PRIVATE_SOURCES}
    ${TEXTURE_COOKER_PRIVATE_HEADERS}
)
sparkle_configure_host_tool_target(TextureCooker)
sparkle_configure_development_tool_artifacts(TextureCooker)

target_include_directories(TextureCooker
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${CMAKE_SOURCE_DIR}/Engine
        ${CMAKE_SOURCE_DIR}/Engine/Core/Public/Diagnostics
        ${stb_SOURCE_DIR}
        ${tinyexr_SOURCE_DIR}
)

target_compile_features(TextureCooker PRIVATE cxx_std_20)

sparkle_target_precompile_headers(TextureCooker
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/PCH.h
)

target_link_libraries(TextureCooker
    PRIVATE
        DirectXTex
        ToolSupport
        SparkleCore
        SparkleTasks
        TextureCookShared
        zlibstatic
        SparkleRHIContracts
        stb
        tinyexr
        ktx_read
)

sparkle_declare_runtime_dll_owner(TextureCooker SparkleCore)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Public PREFIX "Public" FILES ${TEXTURE_COOK_SHARED_PUBLIC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Private PREFIX "Private" FILES ${TEXTURE_COOK_SHARED_PRIVATE_SOURCES} ${TEXTURE_COOK_SHARED_PRIVATE_HEADERS} ${TEXTURE_COOKER_PRIVATE_SOURCES} ${TEXTURE_COOKER_PRIVATE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "Source" FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp)
