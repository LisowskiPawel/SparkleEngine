set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
    add_link_options("$<$<CONFIG:ShippingEditor,ShippingGame>:/PDBALTPATH:$<TARGET_PDB_FILE_NAME:$<TARGET_PROPERTY:NAME>>>")
endif()

function(sparkle_enable_parallel_compilation target_name)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR NOT CMAKE_GENERATOR MATCHES "^Visual Studio ")
        return()
    endif()

    # Keep the generated /Yc source serial; /MP is incompatible with PCH creation.
    # Two compiler workers per project stay bounded with Launcher's four build workers.
    get_target_property(_sources ${target_name} SOURCES)
    foreach(_source IN LISTS _sources)
        if(_source MATCHES "[.](cpp|cxx|cc)$")
            get_source_file_property(_options "${_source}" TARGET_DIRECTORY ${target_name} COMPILE_OPTIONS)
            if(NOT "/MP2" IN_LIST _options)
                set_property(SOURCE "${_source}" TARGET_DIRECTORY ${target_name} APPEND PROPERTY COMPILE_OPTIONS /MP2)
            endif()
        endif()
    endforeach()
endfunction()

function(sparkle_target_precompile_headers target_name)
    target_precompile_headers(${target_name} PRIVATE ${ARGN})

    if(MSVC AND CMAKE_GENERATOR MATCHES "^Visual Studio ")
        target_compile_options(${target_name}
            PRIVATE
                $<$<COMPILE_LANGUAGE:CXX>:/Fp${CMAKE_CURRENT_BINARY_DIR}/${target_name}.dir/$<CONFIG>/cmake_pch.pch>
        )
    endif()
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/Build/Sanitizers.cmake")

option(ENABLE_STRICT_WARNINGS "Enable strict compiler warnings and treat warnings as errors" OFF)
if(ENABLE_STRICT_WARNINGS)
    message(STATUS "Strict warnings enabled: adding compiler warning flags and Werror")
    if(MSVC)
        add_compile_options(/W4 /WX)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion -Wshadow -Wformat-security)
    else()
        message(WARNING "ENABLE_STRICT_WARNINGS requested but compiler is unsupported: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()
