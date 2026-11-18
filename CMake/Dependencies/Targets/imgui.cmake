sparkle_declare_git_dependency(imgui
    https://github.com/ocornut/imgui.git
    6d910d5487d11ca567b61c7824b0c78c569d62f0)
FetchContent_Populate(imgui)

if(NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

add_library(imgui STATIC
    ${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/ImGui/SparkleImGuiConfig.h
    ${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/ImGui/SparkleImGuiContext.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    # Platform backend shared by editor/game UI. Renderer backends are owned by RHI implementations.
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)

target_include_directories(imgui PUBLIC
    ${SPARKLE_REPOSITORY_ROOT}/CMake/Dependencies/ImGui
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    user32
    imm32
    winmm
)

target_compile_features(imgui PUBLIC cxx_std_20)

target_compile_definitions(imgui PUBLIC
    IMGUI_USER_CONFIG="SparkleImGuiConfig.h"
)

if(MSVC)
    target_compile_options(imgui PRIVATE /W0)
endif()

set_target_properties(imgui PROPERTIES FOLDER "ThirdParty")
