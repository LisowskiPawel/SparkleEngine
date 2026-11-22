# BuildAndPackaging owns release composition. Target owners still own their files.
set(_sparkle_release_profiles ShippingEditor ShippingGame)
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE IN_LIST _sparkle_release_profiles)
        return()
    endif()
    set(_sparkle_release_profiles "${CMAKE_BUILD_TYPE}")
endif()
foreach(_profile IN LISTS _sparkle_release_profiles)
    set(_sparkle_release_products_${_profile})
endforeach()
foreach(_product IN ITEMS
    "Launcher|SparkleLauncher|ShippingEditor"
    "Editor|ShowcaseEditor|ShippingEditor"
    "Game|ShowcaseGame|ShippingGame"
    "AssetCooker|AssetCooker|ShippingEditor"
    "TextureCooker|TextureCooker|ShippingEditor"
    "ShaderCompiler|ShaderCompiler|ShippingEditor")
    string(REPLACE "|" ";" _fields "${_product}")
    list(GET _fields 0 _component)
    list(GET _fields 1 _target)
    list(GET _fields 2 _profile)
    if(NOT _profile IN_LIST _sparkle_release_profiles)
        continue()
    endif()
    set(_directory "")
    set(_source "")
    set(_symbols "")
    set(_runtime_json "")
    set(_support_json "")
    set(_source_packages "")
    if(TARGET ${_target})

        # Let CMake select conditional, private/static and LINK_ONLY contributors.
        set_property(TARGET ${_target} APPEND PROPERTY TRANSITIVE_LINK_PROPERTIES SPARKLE_SOURCE_PACKAGES)
        set(_source_packages "$<REMOVE_DUPLICATES:$<TARGET_GENEX_EVAL:${_target},$<TARGET_PROPERTY:${_target},SPARKLE_SOURCE_PACKAGES>>>")
        set(_source_packages "$<JOIN:${_source_packages},,>")

        string(TOUPPER "${_profile}" _profile_upper)
        get_target_property(_runtime_directory ${_target} RUNTIME_OUTPUT_DIRECTORY_${_profile_upper})
        file(RELATIVE_PATH _directory "${SPARKLE_REPOSITORY_ROOT}" "${_runtime_directory}")
        if(_directory MATCHES "^\\.\\." OR IS_ABSOLUTE "${_directory}")
            message(FATAL_ERROR "Release product lies outside the canonical product layout: ${_target}")
        endif()
        set(_source "$<TARGET_FILE:${_target}>")
        if(MSVC)
            set(_symbols "$<TARGET_PDB_FILE:${_target}>")
        endif()
        get_target_property(_runtime_files ${_target} SPARKLE_MSVC_RUNTIME_FILES)
        if(_runtime_files)
            foreach(_runtime IN LISTS _runtime_files)
                file(TO_CMAKE_PATH "${_runtime}" _runtime)
                list(APPEND _runtime_json "\"${_runtime}\"")
            endforeach()
        endif()
        set(_support_files "$<REMOVE_DUPLICATES:$<TARGET_GENEX_EVAL:${_target},$<TARGET_PROPERTY:${_target},SPARKLE_RUNTIME_SUPPORT_FILES>>>")
        set(_support_json "$<$<BOOL:${_support_files}>:\"$<JOIN:${_support_files},\",\">\">")

    endif()
    list(JOIN _runtime_json "," _runtime_json)
    list(APPEND _sparkle_release_products_${_profile}
        "{\"Component\":\"${_component}\",\"Target\":\"${_target}\",\"Configuration\":\"${_profile}\",\"Source\":\"${_source}\",\"Symbols\":\"${_symbols}\",\"MSVCRuntime\":[${_runtime_json}],\"RuntimeSupport\":[${_support_json}],\"SourcePackages\":[${_source_packages}],\"Path\":\"${_directory}/${_target}.exe\"}")
endforeach()
foreach(_profile IN LISTS _sparkle_release_profiles)
    list(JOIN _sparkle_release_products_${_profile} ",\n" _sparkle_release_product_json)
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/private/release-products/${_profile}.json"
        CONTENT "{\"Version\":\"${SPARKLE_VERSION}\",\"Products\":[${_sparkle_release_product_json}]}\n"
        CONDITION "$<CONFIG:${_profile}>")
endforeach()

set(SPARKLE_RELEASE_INPUTS "" CACHE FILEPATH "Reviewed prepared payload inventory and final shader-export path")
add_custom_target(release_package
    COMMAND ${SPARKLE_POWERSHELL_EXECUTABLE} -NoProfile -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_LIST_DIR}/Package.ps1"
        -BuildDirectory "${CMAKE_BINARY_DIR}"
        -InputsPath "${SPARKLE_RELEASE_INPUTS}"
    WORKING_DIRECTORY "${SPARKLE_REPOSITORY_ROOT}"
    USES_TERMINAL
    VERBATIM)
