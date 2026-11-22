set(DXC_SDK_ROOT "$ENV{VULKAN_SDK}" CACHE PATH "DXC SDK root")
set(SLANG_SDK_ROOT "$ENV{VULKAN_SDK}" CACHE PATH "Slang SDK root")
set(SPIRV_TOOLS_SDK_ROOT "$ENV{VULKAN_SDK}" CACHE PATH "SPIRV-Tools SDK root")
if(SPARKLE_WITH_GPU_CRASH_DEBUGGING)
    sparkle_verify_gpu_crash_inputs("${DXC_SDK_ROOT}" Dxc)
    sparkle_verify_gpu_crash_inputs("${SLANG_SDK_ROOT}" Slang)
    sparkle_verify_gpu_crash_inputs("${SPIRV_TOOLS_SDK_ROOT}" SpirvTools)
endif()
# SDK selection is root-bound, including after a prior configure with another SDK.
foreach(_input IN ITEMS SPIRV_TOOLS_INCLUDE_DIR SPIRV_TOOLS_IMPORT_LIBRARY SPIRV_TOOLS_RUNTIME_LIBRARY
        DXC_INCLUDE_DIR DXC_IMPORT_LIBRARY DXC_RUNTIME_LIBRARY DXC_TOOL_EXECUTABLE SLANG_TOOL_EXECUTABLE
        SLANG_RUNTIME_LIBRARY SLANG_COMPILER_RUNTIME_LIBRARY
        SLANG_GLSL_MODULE_RUNTIME_LIBRARY SLANG_GLSLANG_RUNTIME_LIBRARY SLANG_RT_RUNTIME_LIBRARY SLANG_STANDARD_LIBRARY_FILE)
    unset(${_input})
    unset(${_input} CACHE)
endforeach()
find_path(SPIRV_TOOLS_INCLUDE_DIR NAMES spirv-tools/libspirv.hpp PATHS "${SPIRV_TOOLS_SDK_ROOT}/Include" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_library(SPIRV_TOOLS_IMPORT_LIBRARY NAMES SPIRV-Tools-shared PATHS "${SPIRV_TOOLS_SDK_ROOT}/Lib" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SPIRV_TOOLS_RUNTIME_LIBRARY NAMES SPIRV-Tools-shared.dll PATHS "${SPIRV_TOOLS_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_path(DXC_INCLUDE_DIR NAMES dxcapi.h PATHS "${DXC_SDK_ROOT}/Include/dxc" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_library(DXC_IMPORT_LIBRARY NAMES dxcompiler PATHS "${DXC_SDK_ROOT}/Lib" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(DXC_RUNTIME_LIBRARY NAMES dxcompiler.dll PATHS "${DXC_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(DXC_TOOL_EXECUTABLE NAMES dxc.exe PATHS "${DXC_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_RUNTIME_LIBRARY NAMES slang.dll PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_TOOL_EXECUTABLE NAMES slangc.exe PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_COMPILER_RUNTIME_LIBRARY NAMES slang-compiler.dll PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_GLSL_MODULE_RUNTIME_LIBRARY NAMES slang-glsl-module.dll PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_GLSLANG_RUNTIME_LIBRARY NAMES slang-glslang.dll PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_RT_RUNTIME_LIBRARY NAMES slang-rt.dll PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
find_file(SLANG_STANDARD_LIBRARY_FILE NAMES slang.slang PATHS "${SLANG_SDK_ROOT}/Bin" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
file(GLOB SLANG_STANDARD_MODULE_DIRECTORIES LIST_DIRECTORIES true "${SLANG_SDK_ROOT}/Bin/slang-standard-module-*")
list(SORT SLANG_STANDARD_MODULE_DIRECTORIES)
list(LENGTH SLANG_STANDARD_MODULE_DIRECTORIES _slang_standard_module_count)
if(SPARKLE_WITH_GPU_CRASH_DEBUGGING AND NOT _slang_standard_module_count EQUAL 1)
    message(FATAL_ERROR "GPU crash debugging requires exactly the verified Slang standard module directory.")
endif()
if(NOT SLANG_STANDARD_MODULE_DIRECTORIES)
    message(FATAL_ERROR "Slang standard modules were not found under ${SLANG_SDK_ROOT}/Bin.")
endif()

set(SLANG_RUNTIME_SUPPORT_FILES
    "${SLANG_COMPILER_RUNTIME_LIBRARY}"
    "${SLANG_GLSL_MODULE_RUNTIME_LIBRARY}"
    "${SLANG_GLSLANG_RUNTIME_LIBRARY}"
    "${SLANG_RT_RUNTIME_LIBRARY}"
    "${SLANG_STANDARD_LIBRARY_FILE}")

add_library(Dxc::Dxc SHARED IMPORTED GLOBAL)
set_target_properties(Dxc::Dxc PROPERTIES
    IMPORTED_IMPLIB "${DXC_IMPORT_LIBRARY}"
    IMPORTED_LOCATION "${DXC_RUNTIME_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${DXC_INCLUDE_DIR}"
)
add_library(SPIRVTools::SPIRVTools SHARED IMPORTED GLOBAL)
set_target_properties(SPIRVTools::SPIRVTools PROPERTIES
    IMPORTED_IMPLIB "${SPIRV_TOOLS_IMPORT_LIBRARY}"
    IMPORTED_LOCATION "${SPIRV_TOOLS_RUNTIME_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SPIRV_TOOLS_INCLUDE_DIR}"
    INTERFACE_COMPILE_DEFINITIONS SPIRV_TOOLS_SHAREDLIB
)

# Bind consumed headers/import libraries and shipped tools/runtime, without
# inventing an upstream revision for a locally installed SDK.
file(GLOB_RECURSE SPIRV_TOOLS_IDENTITY_HEADERS CONFIGURE_DEPENDS "${SPIRV_TOOLS_INCLUDE_DIR}/spirv-tools/*.h" "${SPIRV_TOOLS_INCLUDE_DIR}/spirv-tools/*.hpp")
set(DXC_IDENTITY_INPUTS "${DXC_RUNTIME_LIBRARY}" "${DXC_IMPORT_LIBRARY}" "${DXC_TOOL_EXECUTABLE}" "${DXC_INCLUDE_DIR}/dxcapi.h")
set(SPIRV_TOOLS_IDENTITY_INPUTS "${SPIRV_TOOLS_RUNTIME_LIBRARY}" "${SPIRV_TOOLS_IMPORT_LIBRARY}" ${SPIRV_TOOLS_IDENTITY_HEADERS})
foreach(component IN ITEMS DXC SPIRV_TOOLS)
    set(identity)
    foreach(input IN LISTS ${component}_IDENTITY_INPUTS)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${input}")
        file(SHA256 "${input}" hash)
        string(APPEND identity "${hash}\n")
    endforeach()
    string(SHA256 identity "${identity}")
    if(component STREQUAL DXC)
        sparkle_add_source_package(Dxc::Dxc "Microsoft DXC runtime" "sha256:${identity}"
            LicenseRef-DXC-ThirdPartyNotices "https://github.com/microsoft/DirectXShaderCompiler")
    else()
        sparkle_add_source_package(SPIRVTools::SPIRVTools SPIRV-Tools "sha256:${identity}"
            Apache-2.0 "https://github.com/KhronosGroup/SPIRV-Tools")
    endif()
endforeach()
set(_slang_identity)
set(_slang_module_inputs)
foreach(directory IN LISTS SLANG_STANDARD_MODULE_DIRECTORIES)
    file(GLOB_RECURSE files CONFIGURE_DEPENDS LIST_DIRECTORIES false "${directory}/*")
    list(APPEND _slang_module_inputs ${files})
endforeach()
foreach(input IN ITEMS "${SLANG_TOOL_EXECUTABLE}" "${SLANG_RUNTIME_LIBRARY}" ${SLANG_RUNTIME_SUPPORT_FILES} ${_slang_module_inputs})
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${input}")
    file(SHA256 "${input}" hash)
    string(APPEND _slang_identity "${hash}\n")
endforeach()
string(SHA256 _slang_identity "${_slang_identity}")
sparkle_add_source_package(ShaderCompiler "Slang compiler and runtime" "sha256:${_slang_identity}"
    "Apache-2.0 WITH LLVM-exception AND LicenseRef-Slang-ThirdPartyNotices"
    "https://github.com/shader-slang/slang")
