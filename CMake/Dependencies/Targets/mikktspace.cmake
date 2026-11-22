# MikkTSpace - canonical tangent-space generation used by glTF authoring tools
# https://github.com/mmikk/MikkTSpace
#
# Target:  mikktspace (STATIC)
# Usage:   target_link_libraries(YourTarget PRIVATE mikktspace)
set(_sparkle_mikktspace_revision 3e895b49d05ea07e4c2133156cfa94369e19e409)
sparkle_declare_git_dependency(mikktspace
    https://github.com/mmikk/MikkTSpace.git
    ${_sparkle_mikktspace_revision}
    ID mikktspace VERSION "${_sparkle_mikktspace_revision}" PACKAGE_NAME "MikkTSpace" LICENSE_ID "Zlib"
    REQUIRED_PATHS mikktspace.c mikktspace.h)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(mikktspace STATIC
    ${mikktspace_SOURCE_DIR}/mikktspace.c
    ${mikktspace_SOURCE_DIR}/mikktspace.h
)
target_include_directories(mikktspace SYSTEM PUBLIC ${mikktspace_SOURCE_DIR})
if(MSVC)
    target_compile_options(mikktspace PRIVATE /W0)
endif()
set_target_properties(mikktspace PROPERTIES FOLDER "ThirdParty")

sparkle_add_dependency_source_package(mikktspace mikktspace)
