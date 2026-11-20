# tinyexr - Header-only OpenEXR image loader/writer
# https://github.com/syoyo/tinyexr
#
# Target:  tinyexr (INTERFACE)
# Usage:   target_link_libraries(YourTarget PRIVATE tinyexr)
#          #define TINYEXR_IMPLEMENTATION (in exactly one .cpp)
set(_sparkle_tinyexr_revision 6e8cac308cdf4d717078f3f37c4aa39bf3b356b4)
sparkle_declare_git_dependency(tinyexr
    https://github.com/syoyo/tinyexr.git
    ${_sparkle_tinyexr_revision})
FetchContent_Populate(tinyexr)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(tinyexr INTERFACE)
sparkle_set_source_package(tinyexr "tinyexr" "1.0.7"
    "BSD-3-Clause" "https://github.com/syoyo/tinyexr/tree/${_sparkle_tinyexr_revision}")
target_include_directories(tinyexr INTERFACE ${tinyexr_SOURCE_DIR})
