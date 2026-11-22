include_guard(GLOBAL)
find_program(SPARKLE_POWERSHELL_EXECUTABLE NAMES powershell pwsh REQUIRED)

# This file outlives build/cache cleans, so all processes coordinate on one object.
set(SPARKLE_SOURCE_CACHE_LOCK_NAME ".sparkle-source-cache.lock")
set(SPARKLE_SOURCE_CACHE_LOCK_FILE "${SPARKLE_REPOSITORY_ROOT}/${SPARKLE_SOURCE_CACHE_LOCK_NAME}")
file(LOCK "${SPARKLE_SOURCE_CACHE_LOCK_FILE}" GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE source_cache_lock_result)
if(NOT source_cache_lock_result STREQUAL "0")
    message(FATAL_ERROR "Source dependency cache is in use by a build, configure or Clean. Retry after it settles; no dependency inputs were changed. (${source_cache_lock_result})")
endif()

function(sparkle_protect_source_cache_readers directory)
    if(directory STREQUAL CMAKE_SOURCE_DIR AND NOT TARGET SparkleSourceDependenciesReady)
        sparkle_configure_source_dependency_admission()
    endif()
    get_property(targets DIRECTORY "${directory}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target IN LISTS targets)
        get_target_property(type ${target} TYPE)
        get_target_property(reads_cache ${target} SPARKLE_READS_SOURCE_CACHE)
        if(type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|EXECUTABLE)$" OR reads_cache)
            add_dependencies(${target} SparkleSourceDependenciesReady)
        endif()
        if(type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|EXECUTABLE)$")
            if(CMAKE_GENERATOR MATCHES "^Visual Studio ")
                set_target_properties(${target} PROPERTIES
                    VS_GLOBAL_ForceImportAfterCppTargets "${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/SourceCacheRead.targets"
                    VS_GLOBAL_SparkleSourceCacheLock "${SPARKLE_SOURCE_CACHE_LOCK_FILE}")
            elseif(WIN32)
                # Compiler launchers omit Ninja's module/resource dependency scans.
                # The documented rule launch properties cover the complete rule,
                # including MASM, and retain any compiler cache inside our reader.
                # Keep this target-local so configure's try_compile uses its writer.
                foreach(action IN ITEMS COMPILE LINK)
                    get_target_property(launcher ${target} RULE_LAUNCH_${action})
                    set(reader_launcher "\"${SPARKLE_POWERSHELL_EXECUTABLE}\" -NoProfile -ExecutionPolicy Bypass -File \"${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/SourceCacheRead.ps1\" \"${SPARKLE_SOURCE_CACHE_LOCK_FILE}\"")
                    if(launcher)
                        string(APPEND reader_launcher " ${launcher}")
                    endif()
                    set_property(TARGET ${target} PROPERTY RULE_LAUNCH_${action} "${reader_launcher}")
                endforeach()
            endif()
        endif()
    endforeach()
    get_property(subdirectories DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
    foreach(subdirectory IN LISTS subdirectories)
        sparkle_protect_source_cache_readers("${subdirectory}")
    endforeach()
endfunction()
cmake_language(DEFER CALL sparkle_protect_source_cache_readers "${CMAKE_SOURCE_DIR}")
# Regeneration can run inside cmake --build; release before that process starts native readers.
cmake_language(DEFER CALL file LOCK "${SPARKLE_SOURCE_CACHE_LOCK_FILE}" RELEASE)
