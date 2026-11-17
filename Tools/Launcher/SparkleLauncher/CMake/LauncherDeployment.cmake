add_custom_command(TARGET SparkleLauncher POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Visuals"
        "$<TARGET_FILE_DIR:SparkleLauncher>/Visuals"
    COMMENT "Deploying SparkleLauncher visual artwork"
)

if(WIN32)
    get_target_property(SPARKLE_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(SPARKLE_QT_BIN_DIR "${SPARKLE_QMAKE_EXECUTABLE}" DIRECTORY)

    find_program(SPARKLE_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${SPARKLE_QT_BIN_DIR}"
        REQUIRED
    )

    set(_sparkle_qt_deploy_command "${SPARKLE_WINDEPLOYQT_EXECUTABLE}")
    if(CMAKE_GENERATOR MATCHES "^Visual Studio" AND CMAKE_GENERATOR_INSTANCE)
        string(REGEX REPLACE ",.*$" "" _sparkle_visual_studio_root "${CMAKE_GENERATOR_INSTANCE}")
        set(_sparkle_qt_deploy_command
            "${CMAKE_COMMAND}" -E env "VCINSTALLDIR=${_sparkle_visual_studio_root}/VC/"
            "${SPARKLE_WINDEPLOYQT_EXECUTABLE}")
    endif()

    set(_sparkle_qt_notice_files
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/README.md"
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/ThirdPartyNotices.txt"
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/LGPL-3.0-only.txt"
        "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt/GPL-3.0-only.txt")
    foreach(_notice IN LISTS _sparkle_qt_notice_files)
        if(NOT EXISTS "${_notice}")
            message(FATAL_ERROR "Qt deployment notice is missing: '${_notice}'")
        endif()
    endforeach()
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/QtRuntimeVersion.txt"
        CONTENT "${Qt6_VERSION}\n")

    add_custom_target(SparkleLauncherQtNotices
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${SPARKLE_REPOSITORY_ROOT}/CMake/Notices/Qt"
            "$<TARGET_FILE_DIR:SparkleLauncher>/Notices/Qt"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/QtRuntimeVersion.txt"
            "$<TARGET_FILE_DIR:SparkleLauncher>/Notices/Qt/QtRuntimeVersion.txt"
        DEPENDS ${_sparkle_qt_notice_files} "${CMAKE_CURRENT_BINARY_DIR}/QtRuntimeVersion.txt"
        COMMENT "Deploying Qt notices"
        VERBATIM
    )
    add_dependencies(SparkleLauncher SparkleLauncherQtNotices)

    add_custom_command(TARGET SparkleLauncher POST_BUILD
        COMMAND ${_sparkle_qt_deploy_command}
            --verbose 0
            --no-compiler-runtime
            --dir "$<TARGET_FILE_DIR:SparkleLauncher>"
            "$<TARGET_FILE:SparkleLauncher>"
        COMMENT "Deploying Qt runtime for SparkleLauncher artifact"
    )
endif()
