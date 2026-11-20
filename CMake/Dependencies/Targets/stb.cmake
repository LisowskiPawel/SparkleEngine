# stb - Header-only raster source decoding
# https://github.com/nothings/stb
#
# Provides: stb_image.h for source formats outside native DirectXTex loaders.
#
# Target:  stb (INTERFACE)
# Usage:   target_link_libraries(YourTarget PRIVATE stb)
#          #define STB_IMAGE_IMPLEMENTATION  (in exactly one .cpp)
set(_sparkle_stb_revision 2c980bb59875b0d32144a71867fbdebb2f77cd20)
sparkle_declare_git_dependency(stb
    https://github.com/nothings/stb.git
    ${_sparkle_stb_revision})
FetchContent_Populate(stb)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(stb INTERFACE)
sparkle_set_source_package(stb "stb" "${_sparkle_stb_revision}"
    "MIT" "https://github.com/nothings/stb/tree/${_sparkle_stb_revision}")
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
