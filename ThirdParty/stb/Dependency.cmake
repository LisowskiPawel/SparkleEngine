# stb - Header-only raster source decoding
# https://github.com/nothings/stb
#
# Provides: stb_image.h for source formats outside native DirectXTex loaders.
#
# Target:  stb (STATIC)
# Usage:   target_link_libraries(YourTarget PRIVATE stb)
set(_sparkle_stb_revision 2c980bb59875b0d32144a71867fbdebb2f77cd20)
sparkle_declare_git_dependency(stb
    https://github.com/nothings/stb.git
    ${_sparkle_stb_revision}
    ID stb VERSION "${_sparkle_stb_revision}" PACKAGE_NAME "stb" LICENSE_ID "MIT"
    REQUIRED_PATHS stb_image.h LICENSE)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(stb STATIC "${CMAKE_CURRENT_LIST_DIR}/Bindings/StbImageImplementation.cpp")
set_target_properties(stb PROPERTIES FOLDER "ThirdParty/stb")
if(MSVC)
    target_compile_options(stb PRIVATE /W0)
endif()

sparkle_add_dependency_source_package(stb stb)
target_include_directories(stb SYSTEM PUBLIC ${stb_SOURCE_DIR})
