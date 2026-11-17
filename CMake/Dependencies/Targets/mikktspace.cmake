# MikkTSpace - canonical tangent-space generation used by glTF authoring tools
# https://github.com/mmikk/MikkTSpace
#
# Target:  mikktspace (STATIC)
# Usage:   target_link_libraries(YourTarget PRIVATE mikktspace)
sparkle_declare_git_dependency(mikktspace
    https://github.com/mmikk/MikkTSpace.git
    3e895b49d05ea07e4c2133156cfa94369e19e409)
sparkle_log_dependency_step("MikkTSpace" "3e895b49" "<1 MB" "Canonical tangent generation for normal-mapped glTF imports" "https://github.com/mmikk/MikkTSpace.git")
FetchContent_Populate(mikktspace)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    sparkle_log_dependency_ready("MikkTSpace" "${mikktspace_SOURCE_DIR}" "<1 MB")
    return()
endif()

add_library(mikktspace STATIC
    ${mikktspace_SOURCE_DIR}/mikktspace.c
    ${mikktspace_SOURCE_DIR}/mikktspace.h
)
target_include_directories(mikktspace PUBLIC ${mikktspace_SOURCE_DIR})
if(MSVC)
    target_compile_options(mikktspace PRIVATE /W0)
endif()
set_target_properties(mikktspace PROPERTIES FOLDER "ThirdParty")
sparkle_log_dependency_ready("MikkTSpace" "${mikktspace_SOURCE_DIR}" "<1 MB")
