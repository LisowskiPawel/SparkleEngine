# RHI selects graphics backends before composing these upstream implementations.
if(SPARKLE_RHI_WITH_D3D12)
    add_library(imgui_d3d12 STATIC ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp)
    target_link_libraries(imgui_d3d12 PUBLIC imgui d3d12 dxgi d3dcompiler)
    set_target_properties(imgui_d3d12 PROPERTIES FOLDER "ThirdParty/imgui")
endif()

if(SPARKLE_RHI_WITH_VULKAN)
    add_library(imgui_vulkan STATIC ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
    target_link_libraries(imgui_vulkan PUBLIC imgui Vulkan::Vulkan)
    set_target_properties(imgui_vulkan PROPERTIES FOLDER "ThirdParty/imgui")
endif()
