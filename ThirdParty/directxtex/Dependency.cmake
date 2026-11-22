set(SPARKLE_DIRECTXTEX_REVISION 4feb3e11a020f35b796fc769a74216a555d4f5ef)

sparkle_declare_git_dependency(directxtex
    https://github.com/microsoft/DirectXTex.git
    ${SPARKLE_DIRECTXTEX_REVISION}
    ID directxtex VERSION "may2026" PACKAGE_NAME "DirectXTex" LICENSE_ID "MIT"
    REQUIRED_PATHS CMakeLists.txt DirectXTex/DirectXTex.h DirectXTex/DirectXTexDDS.cpp DirectXTex/DirectXTexMipmaps.cpp DirectXTex/DirectXTexCompress.cpp LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()
# Keep upstream's generic options local to this dependency composition.
function(sparkle_add_directxtex)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_TOOLS OFF)
    set(BUILD_SAMPLE OFF)
    set(BUILD_DX11 OFF)
    set(BUILD_DX12 OFF)
    set(BC_USE_OPENMP OFF)
    set(ENABLE_OPENEXR_SUPPORT OFF)
    set(ENABLE_LIBJPEG_SUPPORT OFF)
    set(ENABLE_LIBPNG_SUPPORT OFF)
    sparkle_add_dependency_subdirectory(${directxtex_SOURCE_DIR} ${directxtex_BINARY_DIR} EXCLUDE_FROM_ALL)
    set_target_properties(DirectXTex PROPERTIES FOLDER "ThirdParty/directxtex")

    sparkle_add_dependency_source_package(DirectXTex directxtex)
endfunction()
sparkle_add_directxtex()
