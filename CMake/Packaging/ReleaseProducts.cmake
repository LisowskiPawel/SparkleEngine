# BuildAndPackaging owns release composition. Target owners still own their files.
set(_sparkle_release_products)
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
    if(TARGET ${_target})
        set(_source "$<TARGET_FILE:${_target}>")
    endif()
    list(APPEND _sparkle_release_products
        "{\"Component\":\"${_component}\",\"Target\":\"${_target}\",\"Configuration\":\"${_profile}\",\"Source\":\"${_source}\",\"Path\":\"${_directory}/${_target}.exe\"}")
endforeach()
list(JOIN _sparkle_release_products ",\n" _sparkle_release_product_json)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/private/release-products/$<CONFIG>.json"
    CONTENT "{\"Version\":\"${SPARKLE_VERSION}\",\"Products\":[${_sparkle_release_product_json}]}\n")

set(SPARKLE_RELEASE_INPUTS "" CACHE FILEPATH "Reviewed candidate-bound release payload admission JSON")
add_custom_target(release_package
    COMMAND ${SPARKLE_POWERSHELL_EXECUTABLE} -NoProfile -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_LIST_DIR}/Package.ps1"
        -BuildDirectory "${CMAKE_BINARY_DIR}"
        -InputsPath "${SPARKLE_RELEASE_INPUTS}"
    WORKING_DIRECTORY "${SPARKLE_REPOSITORY_ROOT}"
    USES_TERMINAL
    VERBATIM)
