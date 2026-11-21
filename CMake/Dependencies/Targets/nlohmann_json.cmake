set(SPARKLE_NLOHMANN_JSON_REVISION "d732d616badc771ab1e85876b3a9eb8191fdf4c0")
set(SPARKLE_NLOHMANN_JSON_VERSION "3.13.0-dev.d732d616 (unreleased)")
sparkle_declare_git_dependency(nlohmann_json
    https://github.com/nlohmann/json.git
    ${SPARKLE_NLOHMANN_JSON_REVISION})
FetchContent_Populate(nlohmann_json)
sparkle_prepare_git_fetchcontent_source(nlohmann_json
    EXPECTED_REVISION "${SPARKLE_NLOHMANN_JSON_REVISION}"
    REQUIRED_PATHS CMakeLists.txt include/nlohmann/json.hpp include/nlohmann/json_fwd.hpp LICENSE.MIT)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
set(JSON_CI OFF CACHE BOOL "" FORCE)
set(JSON_ImplicitConversions OFF CACHE BOOL "" FORCE)
set(JSON_Diagnostics OFF CACHE BOOL "" FORCE)
set(JSON_Diagnostic_Positions OFF CACHE BOOL "" FORCE)
set(JSON_LegacyDiscardedValueComparison OFF CACHE BOOL "" FORCE)
sparkle_add_dependency_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
sparkle_set_source_package(nlohmann_json "nlohmann/json" "${SPARKLE_NLOHMANN_JSON_VERSION}"
    "MIT" "https://github.com/nlohmann/json/archive/${SPARKLE_NLOHMANN_JSON_REVISION}.tar.gz")
