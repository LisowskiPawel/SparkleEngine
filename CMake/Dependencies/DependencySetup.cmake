include(FetchContent)

# Suppress the CMP0169 warning in CMake 4.x - we need FetchContent_Populate()
# because several deps have no usable top-level CMakeLists.txt.
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

option(SPARKLE_VERBOSE_DEPENDENCIES "Show detailed source dependency configure output" OFF)
if(SPARKLE_VERBOSE_DEPENDENCIES)
    set(FETCHCONTENT_QUIET OFF)
else()
    set(FETCHCONTENT_QUIET ON)
endif()
option(SPARKLE_GIT_PROGRESS "Show raw Git object transfer progress while fetching third-party dependencies" OFF)
if(SPARKLE_GIT_PROGRESS)
    set(_sparkle_git_progress TRUE)
    set(_sparkle_git_progress_args --progress)
else()
    set(_sparkle_git_progress FALSE)
    set(_sparkle_git_progress_args)
endif()

function(sparkle_log_dependency_step name ref size purpose repository)
    if(SPARKLE_VERBOSE_DEPENDENCIES)
        message(STATUS "")
        message(STATUS "  ${name} ${ref}")
        message(STATUS "        Purpose: ${purpose}")
        message(STATUS "        Source:  ${repository}")
        message(STATUS "        Expected download: ${size}")
    endif()
endfunction()

function(sparkle_log_dependency_ready name source size)
    if(SPARKLE_VERBOSE_DEPENDENCIES)
        message(STATUS "  ${name}: ${source} (${size})")
    endif()
endfunction()

function(sparkle_add_dependency_subdirectory source_dir binary_dir)
    if(NOT SPARKLE_VERBOSE_DEPENDENCIES)
        set(CMAKE_MESSAGE_LOG_LEVEL WARNING)
    endif()

    if(ARGC GREATER 2 AND "${ARGV2}" STREQUAL "EXCLUDE_FROM_ALL")
        add_subdirectory(${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)
    else()
        add_subdirectory(${source_dir} ${binary_dir})
    endif()

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
