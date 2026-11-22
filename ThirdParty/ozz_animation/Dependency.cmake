set(SPARKLE_OZZ_ANIMATION_REVISION 744eb9d99f606eda849acb0b1204f7a3dc20bca1)

sparkle_declare_git_dependency(ozz_animation
    https://github.com/guillaumeblanc/ozz-animation.git
    ${SPARKLE_OZZ_ANIMATION_REVISION}
    ID ozz_animation VERSION "0.17.0" PACKAGE_NAME "ozz-animation" LICENSE_ID "MIT"
    REQUIRED_PATHS CMakeLists.txt LICENSE.md include/ozz/animation/runtime/skeleton.h include/ozz/animation/runtime/animation.h include/ozz/animation/runtime/sampling_job.h include/ozz/animation/offline/skeleton_builder.h include/ozz/animation/offline/animation_builder.h include/ozz/base/memory/allocator.h)

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

function(sparkle_add_ozz_animation)
    set(BUILD_SHARED_LIBS OFF)
    set(ozz_build_tools OFF)
    set(ozz_build_fbx OFF)
    set(ozz_build_gltf OFF)
    set(ozz_build_data OFF)
    set(ozz_build_samples OFF)
    set(ozz_build_howtos OFF)
    set(ozz_build_tests OFF)
    set(ozz_build_msvc_rt_dll ON)
    sparkle_add_dependency_subdirectory(${ozz_animation_SOURCE_DIR} ${ozz_animation_BINARY_DIR} EXCLUDE_FROM_ALL)
    foreach(target IN ITEMS ozz_base ozz_animation ozz_animation_offline)
        set_target_properties(${target} PROPERTIES FOLDER "ThirdParty/ozz_animation")
        if(MSVC)
            # Upstream recognizes Debug; Sparkle has DebugEditor/DebugGame profiles.
            set_target_properties(${target} PROPERTIES MSVC_RUNTIME_LIBRARY "${CMAKE_MSVC_RUNTIME_LIBRARY}")
        endif()

        sparkle_add_dependency_source_package(${target} ozz_animation)
    endforeach()
endfunction()
sparkle_add_ozz_animation()
