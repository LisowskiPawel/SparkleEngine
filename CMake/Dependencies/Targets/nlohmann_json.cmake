set(SPARKLE_NLOHMANN_JSON_REVISION "d732d616badc771ab1e85876b3a9eb8191fdf4c0")
set(SPARKLE_NLOHMANN_JSON_VERSION "3.12.0+git.d732d616 (unreleased)")
sparkle_declare_git_dependency(nlohmann_json
    https://github.com/nlohmann/json.git
    ${SPARKLE_NLOHMANN_JSON_REVISION}
    ID nlohmann_json VERSION "${SPARKLE_NLOHMANN_JSON_VERSION}" PACKAGE_NAME "nlohmann/json" LICENSE_ID "MIT"
    REQUIRED_PATHS CMakeLists.txt include/nlohmann/json.hpp include/nlohmann/json_fwd.hpp LICENSE.MIT
        include/nlohmann/thirdparty/hedley/hedley.hpp LICENSES/CC0-1.0.txt
        include/nlohmann/detail/conversions/to_chars.hpp include/nlohmann/detail/conversions/zmij.hpp)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

block(SCOPE_FOR VARIABLES)
    set(JSON_BuildTests OFF)
    set(JSON_Install OFF)
    set(JSON_CI OFF)
    set(JSON_ImplicitConversions OFF)
    set(JSON_Diagnostics OFF)
    set(JSON_Diagnostic_Positions OFF)
    set(JSON_LegacyDiscardedValueComparison OFF)
    sparkle_add_dependency_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
endblock()

sparkle_add_dependency_source_package(nlohmann_json nlohmann_json)
sparkle_add_bundled_source_package(nlohmann_json nlohmann_json "nlohmann/json bundled Hedley" "MIT AND CC0-1.0"
    include/nlohmann/thirdparty/hedley/hedley.hpp LICENSES/CC0-1.0.txt)
sparkle_add_bundled_source_package(nlohmann_json nlohmann_json "nlohmann/json bundled Grisu2" MIT
    include/nlohmann/detail/conversions/to_chars.hpp LICENSE.MIT)
sparkle_add_bundled_source_package(nlohmann_json nlohmann_json "nlohmann/json bundled Zmij" MIT
    include/nlohmann/detail/conversions/zmij.hpp LICENSE.MIT)
