# Sparkle build profiles
#
# This is the canonical, inspectable list of supported build configurations.
# Each profile combines a build state with a target shape:
#
#   Debug      - no optimization, full debug information, highest diagnostics.
#   Development - optimized build with debug information and developer diagnostics.
#   Shipping   - optimized for runtime performance with shipping diagnostics policy.
#
#   Editor     - builds editor-capable launch targets.
#   Game       - builds editorless game launch targets.

set(SPARKLE_BUILD_CONFIGURATIONS
    DevelopmentEditor
    DevelopmentGame
    DebugEditor
    DebugGame
    ShippingEditor
    ShippingGame
)

set(SPARKLE_DEBUG_CONFIGURATIONS DebugEditor DebugGame)
set(SPARKLE_DEVELOPMENT_CONFIGURATIONS DevelopmentEditor DevelopmentGame)
set(SPARKLE_SHIPPING_CONFIGURATIONS ShippingEditor ShippingGame)
set(SPARKLE_GAME_CONFIGURATIONS DevelopmentGame DebugGame ShippingGame)

set(SPARKLE_DEFAULT_CONFIGURATION DevelopmentEditor)

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "${SPARKLE_BUILD_CONFIGURATIONS}" CACHE STRING "Sparkle build profiles" FORCE)
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE ${SPARKLE_DEFAULT_CONFIGURATION} CACHE STRING "Sparkle build profile" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${SPARKLE_BUILD_CONFIGURATIONS})
    if(NOT CMAKE_BUILD_TYPE IN_LIST SPARKLE_BUILD_CONFIGURATIONS)
        message(FATAL_ERROR "Unsupported CMAKE_BUILD_TYPE '${CMAKE_BUILD_TYPE}'. Use one of: ${SPARKLE_BUILD_CONFIGURATIONS}")
    endif()
endif()

if(MSVC)
    # Keep PDBs in every state; /FS supports concurrent compiler writes.
    set(_sparkle_DEBUG_compile_flags "/Od /Ob0 /Zi /FS /RTC1")
    set(_sparkle_DEVELOPMENT_compile_flags "/O2 /Ob2 /Zi /FS /DNDEBUG")
    set(_sparkle_SHIPPING_compile_flags "${_sparkle_DEVELOPMENT_compile_flags}")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Remove the workspace prefix from shipping source paths in native symbols.
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}" _sparkle_source_prefix)
        string(APPEND _sparkle_SHIPPING_compile_flags " /d1trimfile:\"${_sparkle_source_prefix}\"")
    endif()
    set(_sparkle_DEBUG_link_flags "/DEBUG:FULL")
    set(_sparkle_DEVELOPMENT_link_flags "/DEBUG:FULL /INCREMENTAL:NO /OPT:REF /OPT:ICF")
    set(_sparkle_SHIPPING_link_flags "${_sparkle_DEVELOPMENT_link_flags}")
else()
    set(_sparkle_DEBUG_compile_flags "-O0 -g")
    set(_sparkle_DEVELOPMENT_compile_flags "-O2 -g -DNDEBUG")
    set(_sparkle_SHIPPING_compile_flags "-O3 -DNDEBUG")
    set(_sparkle_DEBUG_link_flags "")
    set(_sparkle_DEVELOPMENT_link_flags "")
    set(_sparkle_SHIPPING_link_flags "")
endif()

# SDKs use standard configuration names; the empty entry also admits a generic import.
set(_sparkle_DEBUG_imported_configs "Debug;Release;")
set(_sparkle_DEVELOPMENT_imported_configs "RelWithDebInfo;Release;")
set(_sparkle_SHIPPING_imported_configs "Release;")

# Editor and Game share compiler, linker and SDK policy for their build state.
foreach(_sparkle_state IN ITEMS DEBUG DEVELOPMENT SHIPPING)
    foreach(_sparkle_profile IN LISTS SPARKLE_${_sparkle_state}_CONFIGURATIONS)
        string(TOUPPER "${_sparkle_profile}" _sparkle_profile_upper)
        set(CMAKE_MAP_IMPORTED_CONFIG_${_sparkle_profile_upper} "${_sparkle_${_sparkle_state}_imported_configs}")
        foreach(_sparkle_language IN ITEMS C CXX)
            set(CMAKE_${_sparkle_language}_FLAGS_${_sparkle_profile_upper}
                "${_sparkle_${_sparkle_state}_compile_flags}" CACHE STRING "${_sparkle_profile} ${_sparkle_language} flags" FORCE)
        endforeach()
        foreach(_sparkle_link_kind IN ITEMS EXE SHARED MODULE)
            set(CMAKE_${_sparkle_link_kind}_LINKER_FLAGS_${_sparkle_profile_upper}
                "${_sparkle_${_sparkle_state}_link_flags}" CACHE STRING "${_sparkle_profile} ${_sparkle_link_kind} linker flags" FORCE)
        endforeach()
        set(CMAKE_STATIC_LINKER_FLAGS_${_sparkle_profile_upper} "" CACHE STRING "${_sparkle_profile} static linker flags" FORCE)
    endforeach()
endforeach()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:DebugEditor,DebugGame>:Debug>DLL")

add_compile_definitions(
    "SPARKLE_BUILD_DEBUG=$<CONFIG:DebugEditor,DebugGame>"
    "SPARKLE_BUILD_DEVELOPMENT=$<CONFIG:DevelopmentEditor,DevelopmentGame>"
    "SPARKLE_BUILD_SHIPPING=$<CONFIG:ShippingEditor,ShippingGame>"
    "SPARKLE_TARGET_EDITOR=$<CONFIG:DebugEditor,DevelopmentEditor,ShippingEditor>"
    "SPARKLE_TARGET_GAME=$<CONFIG:DebugGame,DevelopmentGame,ShippingGame>"
)
