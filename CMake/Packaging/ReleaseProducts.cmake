# BuildAndPackaging owns release composition. Target owners still own their files.
set(_sparkle_release_profiles ShippingEditor ShippingGame)
foreach(_profile IN LISTS _sparkle_release_profiles)
    set(_sparkle_release_products_${_profile})
endforeach()
foreach(_product IN ITEMS
    "Launcher|SparkleLauncher|ShippingEditor|launcher"
    "Editor|ShowcaseEditor|ShippingEditor|editor"
    "Game|ShowcaseGame|ShippingGame|game"
    "AssetCooker|AssetCooker|ShippingEditor|tools/AssetCooker"
    "TextureCooker|TextureCooker|ShippingEditor|tools/TextureCooker"
    "ShaderCompiler|ShaderCompiler|ShippingEditor|tools/ShaderCompiler")
    string(REPLACE "|" ";" _fields "${_product}")
    list(GET _fields 0 _component)
    list(GET _fields 1 _target)
    list(GET _fields 2 _profile)
    list(GET _fields 3 _directory)
    set(_source "")
    set(_symbols "")
    set(_runtime_json "")
    if(TARGET ${_target})
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
    endif()
    list(JOIN _runtime_json "," _runtime_json)
    list(APPEND _sparkle_release_products_${_profile}
        "{\"Component\":\"${_component}\",\"Target\":\"${_target}\",\"Configuration\":\"${_profile}\",\"Source\":\"${_source}\",\"Symbols\":\"${_symbols}\",\"MSVCRuntime\":[${_runtime_json}],\"Path\":\"${_directory}/${_target}.exe\"}")
endforeach()
foreach(_profile IN LISTS _sparkle_release_profiles)
    list(JOIN _sparkle_release_products_${_profile} ",\n" _sparkle_release_product_json)
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/private/release-products/${_profile}.json"
        CONTENT "{\"Version\":\"${SPARKLE_VERSION}\",\"Products\":[${_sparkle_release_product_json}]}\n"
        CONDITION "$<CONFIG:${_profile}>")
endforeach()

set(SPARKLE_RELEASE_INPUTS "" CACHE FILEPATH "Reviewed candidate-bound release payload admission JSON")
add_custom_target(release_package
    COMMAND ${SPARKLE_POWERSHELL_EXECUTABLE} -NoProfile -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_LIST_DIR}/Package.ps1"
        -BuildDirectory "${CMAKE_BINARY_DIR}"
        -InputsPath "${SPARKLE_RELEASE_INPUTS}"
    WORKING_DIRECTORY "${SPARKLE_REPOSITORY_ROOT}"
    USES_TERMINAL
    VERBATIM)
