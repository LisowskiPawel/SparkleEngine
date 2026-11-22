# Read every recipe in declaration-only mode, including dependencies not built by Launcher.
block(SCOPE_FOR VARIABLES)
    set(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY TRUE)
    include("${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/FetchDependencies.cmake")
endblock()

function(sparkle_source_fact_literal value output)
    string(REPLACE "\\" "\\\\" literal "${value}")
    string(REPLACE "\"" "\\\"" literal "${literal}")
    string(REPLACE "\n" "\\n" literal "${literal}")
    set(${output} "\"${literal}\"" PARENT_SCOPE)
endfunction()

get_property(dependencies GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCIES)
set(SPARKLE_SOURCE_DEPENDENCY_ROWS)
foreach(dependency IN LISTS dependencies)
    sparkle_source_fact_literal("${dependency}" id)
    foreach(field IN ITEMS PACKAGE_NAME VERSION CACHE_DIRECTORY REVISION)
        get_property(value GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency}_${field})
        sparkle_source_fact_literal("${value}" ${field})
    endforeach()
    foreach(field IN ITEMS REQUIRED_PATHS CACHE_FILES)
        get_property(paths GLOBAL PROPERTY SPARKLE_SOURCE_DEPENDENCY_${dependency}_${field})
        set(${field})
        foreach(path IN LISTS paths)
            sparkle_source_fact_literal("${path}" literal)
            list(APPEND ${field} "${literal}")
        endforeach()
        list(JOIN ${field} ", " ${field})
    endforeach()
    string(APPEND SPARKLE_SOURCE_DEPENDENCY_ROWS
        "        {.Id = ${id}, .Label = ${PACKAGE_NAME}, .Version = ${VERSION}, .CacheDirectoryName = ${CACHE_DIRECTORY}, .RequiredRelativePaths = {${REQUIRED_PATHS}}, .ExpectedGitRevision = ${REVISION}, .AdditionalCacheFiles = {${CACHE_FILES}}},\n")
endforeach()
configure_file("${CMAKE_CURRENT_LIST_DIR}/SourceDependencyFacts.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/SourceDependencyFacts.cpp" @ONLY)
target_sources(SparkleLauncherCore PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/SourceDependencyFacts.cpp")

set(_nvapi_source_enabled TRUE)
set(_d3d12_source_enabled TRUE)
foreach(option IN ITEMS SPARKLE_RHI_WITH_D3D12 SPARKLE_RHI_WITH_D3D12_NVAPI SPARKLE_RHI_D3D12_NVAPI_FETCH_FROM_GITHUB)
    if(DEFINED ${option} AND NOT ${option})
        set(_nvapi_source_enabled FALSE)
    endif()
endforeach()
if(DEFINED SPARKLE_RHI_WITH_D3D12 AND NOT SPARKLE_RHI_WITH_D3D12)
    set(_d3d12_source_enabled FALSE)
endif()
if(DEFINED SPARKLE_RHI_WITH_VULKAN)
    set(_vulkan_source_enabled "${SPARKLE_RHI_WITH_VULKAN}")
else()
    find_package(Vulkan QUIET)
    set(_vulkan_source_enabled "${Vulkan_FOUND}")
endif()
target_compile_definitions(SparkleLauncherCore PRIVATE
    SPARKLE_SOURCE_CACHE_LOCK_NAME="${SPARKLE_SOURCE_CACHE_LOCK_NAME}"
    SPARKLE_SOURCE_NVAPI_ENABLED=$<BOOL:${_nvapi_source_enabled}>
    SPARKLE_SOURCE_D3D12_ENABLED=$<BOOL:${_d3d12_source_enabled}>
    SPARKLE_SOURCE_VULKAN_ENABLED=$<BOOL:${_vulkan_source_enabled}>
    SPARKLE_SOURCE_PIX_ENABLED=$<OR:$<BOOL:${SPARKLE_RHI_WITH_D3D12_PIX_EVENTS}>,$<BOOL:${SPARKLE_RHI_WITH_EXTERNAL_CAPTURE}>>)
