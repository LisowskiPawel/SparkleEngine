set(SPARKLE_SDL_VERSION 3.4.20)
set(SPARKLE_SDL_REVISION f00dc919ca35b9756316e86c4777392de442f9b2)
set(SPARKLE_SDL_REQUIRED_FILES CMakeLists.txt LICENSE.txt
    include/SDL3/SDL.h include/SDL3/SDL_video.h include/SDL3/SDL_events.h
    include/SDL3/SDL_mouse.h include/SDL3/SDL_system.h
    src/video/windows/SDL_windowsevents.c src/video/windows/SDL_windowswindow.c
    src/libm/e_pow.c src/libm/math_private.h src/libm/s_isinf.c src/libm/s_isinff.c)

sparkle_declare_git_dependency(sdl3
    https://github.com/libsdl-org/SDL.git
    ${SPARKLE_SDL_REVISION}
    ID sdl3 VERSION "${SPARKLE_SDL_VERSION}" PACKAGE_NAME "SDL3" LICENSE_ID "Zlib"
    REQUIRED_PATHS ${SPARKLE_SDL_REQUIRED_FILES})

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

function(sparkle_add_sdl3)
    set(SDL_SHARED ON)
    set(SDL_STATIC OFF)
    set(SDL_VIDEO ON)
    set(SDL_VENDOR_INFO "${SPARKLE_SDL_REVISION}")
    foreach(subsystem IN ITEMS AUDIO GPU RENDER CAMERA JOYSTICK HAPTIC HIDAPI POWER
            SENSOR DIALOG TRAY OPENGL OPENGLES VULKAN DUMMYVIDEO OFFSCREEN
            TEST_LIBRARY TESTS EXAMPLES INSTALL INSTALL_DOCS UNINSTALL)
        set(SDL_${subsystem} OFF)
    endforeach()

    # Upstream owns its compilation policy; Sparkle's CRT/profile flags remain compatible.
    sparkle_add_dependency_subdirectory(${sdl3_SOURCE_DIR} ${sdl3_BINARY_DIR} EXCLUDE_FROM_ALL)
    set_target_properties(SDL3-shared PROPERTIES FOLDER "ThirdParty/SDL3")

    sparkle_add_dependency_source_package(SDL3-shared sdl3)
    sparkle_add_source_package(SDL_uclibc "SDL3 bundled fdlibm" "${SPARKLE_SDL_REVISION}" "SunPro AND LicenseRef-Public-Domain"
        "https://github.com/libsdl-org/SDL/tree/${SPARKLE_SDL_REVISION}/src/libm")
endfunction()
sparkle_add_sdl3()
