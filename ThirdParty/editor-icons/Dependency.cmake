
set(_sparkle_editor_icons_dir "${FETCHCONTENT_BASE_DIR}/editor-icons/fontawesome-6.7.1")
set(SPARKLE_FONT_AWESOME_SOLID_TTF "${_sparkle_editor_icons_dir}/fa-solid-900.ttf" CACHE FILEPATH "Font Awesome Free Solid TTF path" FORCE)
set(SPARKLE_FONT_AWESOME_LICENSE "${_sparkle_editor_icons_dir}/LICENSE.txt" CACHE FILEPATH "Font Awesome Free license path" FORCE)

sparkle_record_source_dependency(editor-icons VERSION 6.7.1
    PACKAGE_NAME "Font Awesome Free Solid" LICENSE_ID OFL-1.1
    LOCATION "https://github.com/FortAwesome/Font-Awesome/tree/3447c58c6b3ff327b35171d866ec70c25c72f4ef"
    CACHE_DIRECTORY editor-icons
    REQUIRED_PATHS fontawesome-6.7.1/fa-solid-900.ttf fontawesome-6.7.1/LICENSE.txt)
if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY)
    return()
endif()

download_sparkle_editor_asset(
    "https://raw.githubusercontent.com/FortAwesome/Font-Awesome/3447c58c6b3ff327b35171d866ec70c25c72f4ef/webfonts/fa-solid-900.ttf"
    "${SPARKLE_FONT_AWESOME_SOLID_TTF}"
    "Font Awesome Free Solid TTF"
    "b111a8fdb5f70aefb2b1922a390176d1bca439338d485386a7674583ef9acb7c"
)
download_sparkle_editor_asset(
    "https://raw.githubusercontent.com/FortAwesome/Font-Awesome/3447c58c6b3ff327b35171d866ec70c25c72f4ef/LICENSE.txt"
    "${SPARKLE_FONT_AWESOME_LICENSE}"
    "Font Awesome Free license"
    "9b914eae88817d63b576eab5aafde7068c7a1abae125d7cdfb034f1da43a9afc"
)
if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(SparkleEditorIcons INTERFACE)
sparkle_add_dependency_source_package(SparkleEditorIcons editor-icons)
