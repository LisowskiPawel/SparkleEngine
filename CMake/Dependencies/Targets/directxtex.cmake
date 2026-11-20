sparkle_declare_git_dependency(directxtex
    https://github.com/microsoft/DirectXTex.git
    ${SPARKLE_DIRECTXTEX_REVISION})
FetchContent_Populate(directxtex)
sparkle_prepare_git_fetchcontent_source(directxtex
    EXPECTED_REVISION "${SPARKLE_DIRECTXTEX_REVISION}"
    REQUIRED_PATHS CMakeLists.txt DirectXTex/DirectXTex.h DirectXTex/DirectXTexDDS.cpp
    DirectXTex/DirectXTexMipmaps.cpp DirectXTex/DirectXTexCompress.cpp LICENSE)
if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
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
    set_target_properties(DirectXTex PROPERTIES FOLDER "ThirdParty/DirectXTex")
    sparkle_set_source_package(DirectXTex "DirectXTex" "may2026"
        "MIT" "https://github.com/microsoft/DirectXTex/tree/${SPARKLE_DIRECTXTEX_REVISION}")
endfunction()
sparkle_add_directxtex()
