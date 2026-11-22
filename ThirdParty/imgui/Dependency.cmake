set(SPARKLE_IMGUI_VERSION 1.92.5)
set(SPARKLE_IMGUI_REVISION 6d910d5487d11ca567b61c7824b0c78c569d62f0)
set(SPARKLE_IMGUI_REQUIRED_FILES imgui.h imgui.cpp imgui_draw.cpp imgui_tables.cpp
    imgui_widgets.cpp imgui_demo.cpp imgui_internal.h imstb_rectpack.h imstb_textedit.h
    imstb_truetype.h LICENSE.txt backends/imgui_impl_sdl3.cpp backends/imgui_impl_sdl3.h
    backends/imgui_impl_dx12.cpp backends/imgui_impl_dx12.h
    backends/imgui_impl_vulkan.cpp backends/imgui_impl_vulkan.h)

sparkle_declare_git_dependency(imgui
    https://github.com/ocornut/imgui.git
    ${SPARKLE_IMGUI_REVISION}
    ID imgui VERSION "${SPARKLE_IMGUI_VERSION}" PACKAGE_NAME "Dear ImGui" LICENSE_ID "MIT"
    REQUIRED_PATHS ${SPARKLE_IMGUI_REQUIRED_FILES})

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(imgui STATIC
    ${CMAKE_CURRENT_LIST_DIR}/Bindings/SparkleImGuiConfig.h
    ${CMAKE_CURRENT_LIST_DIR}/Bindings/SparkleImGuiContext.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Bindings
)
target_include_directories(imgui SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_compile_features(imgui PUBLIC cxx_std_20)

target_compile_definitions(imgui PUBLIC
    IMGUI_USER_CONFIG="SparkleImGuiConfig.h"
)

if(MSVC)
    target_compile_options(imgui PRIVATE /W0)
endif()

set_target_properties(imgui PROPERTIES FOLDER "ThirdParty/imgui")

sparkle_add_dependency_source_package(imgui imgui)
sparkle_add_bundled_source_package(imgui imgui "Dear ImGui bundled stb" MIT
    imstb_rectpack.h imstb_textedit.h imstb_truetype.h LICENSE.txt)

if(TARGET SDL3::SDL3-shared)
    add_library(imgui_sdl3 STATIC ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp)
    target_link_libraries(imgui_sdl3 PUBLIC imgui SDL3::SDL3-shared)
    set_target_properties(imgui_sdl3 PROPERTIES FOLDER "ThirdParty/imgui")
endif()
