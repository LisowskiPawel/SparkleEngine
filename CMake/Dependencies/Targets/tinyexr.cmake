# tinyexr - Header-only OpenEXR image loader/writer
# https://github.com/syoyo/tinyexr
#
# Target:  tinyexr (STATIC)
# Usage:   target_link_libraries(YourTarget PRIVATE tinyexr)
set(_sparkle_tinyexr_revision 6e8cac308cdf4d717078f3f37c4aa39bf3b356b4)
sparkle_declare_git_dependency(tinyexr
    https://github.com/syoyo/tinyexr.git
    ${_sparkle_tinyexr_revision}
    ID tinyexr VERSION "1.0.7" PACKAGE_NAME "tinyexr" LICENSE_ID "BSD-3-Clause"
    REQUIRED_PATHS tinyexr.h)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(tinyexr STATIC "${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/Bindings/tinyexr/TinyExrImplementation.cpp")
target_compile_definitions(tinyexr PUBLIC TINYEXR_USE_MINIZ=0 TINYEXR_USE_ZFP=0)
target_link_libraries(tinyexr PRIVATE ZLIB::ZLIB)
set_target_properties(tinyexr PROPERTIES FOLDER "ThirdParty/TinyEXR")
if(MSVC)
    target_compile_options(tinyexr PRIVATE /W0)
endif()

sparkle_add_dependency_source_package(tinyexr tinyexr)
sparkle_add_bundled_source_package(tinyexr tinyexr "TinyEXR bundled OpenEXR code" BSD-3-Clause tinyexr.h)
target_include_directories(tinyexr SYSTEM PUBLIC ${tinyexr_SOURCE_DIR})
