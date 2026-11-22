include(FetchContent)

function(sparkle_add_dependency_subdirectory source_dir binary_dir)
    # Keep upstream status chatter out of normal workspace configuration.
    set(CMAKE_MESSAGE_LOG_LEVEL WARNING)
    add_subdirectory("${source_dir}" "${binary_dir}" SYSTEM ${ARGN})
endfunction()

# Locate Git once - used by FetchContent, cache verification, and sparse checkout.
# Fresh Windows machines often have Git installed in the standard Program Files
# location without the current shell inheriting an updated PATH, so honor both
# CMake/launcher-provided overrides and common install roots before failing.
set(SPARKLE_GIT_EXE "" CACHE FILEPATH "Path to Git executable used by Sparkle dependency fetches.")
if(DEFINED GIT_EXECUTABLE AND NOT "${GIT_EXECUTABLE}" STREQUAL "" AND EXISTS "${GIT_EXECUTABLE}")
    set(_git_exe "${GIT_EXECUTABLE}")
elseif(NOT "${SPARKLE_GIT_EXE}" STREQUAL "" AND EXISTS "${SPARKLE_GIT_EXE}")
    set(_git_exe "${SPARKLE_GIT_EXE}")
elseif(DEFINED ENV{SPARKLE_GIT_EXE} AND NOT "$ENV{SPARKLE_GIT_EXE}" STREQUAL "" AND EXISTS "$ENV{SPARKLE_GIT_EXE}")
    set(_git_exe "$ENV{SPARKLE_GIT_EXE}")
else()
    find_program(_git_exe
        NAMES git git.exe
        PATHS
        "C:/Program Files/Git/cmd"
        "C:/Program Files/Git/bin"
        "C:/Program Files (x86)/Git/cmd"
        "C:/Program Files (x86)/Git/bin"
    )
endif()

if(NOT _git_exe OR NOT EXISTS "${_git_exe}")
    message(FATAL_ERROR
        "Git executable was not found. Install Git for Windows, add it to PATH, "
    "or configure with -DGIT_EXECUTABLE=<path-to-git.exe> / -DSPARKLE_GIT_EXE=<path-to-git.exe>.")
endif()
set(GIT_EXECUTABLE "${_git_exe}" CACHE FILEPATH "Git executable used by CMake/FetchContent." FORCE)
set(SPARKLE_GIT_EXE "${_git_exe}" CACHE FILEPATH "Path to Git executable used by Sparkle dependency fetches." FORCE)
