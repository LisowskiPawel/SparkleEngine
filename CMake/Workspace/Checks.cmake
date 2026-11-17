find_program(SPARKLE_POWERSHELL_EXECUTABLE NAMES pwsh powershell REQUIRED)
set(SPARKLE_CLANG_FORMAT_EXECUTABLE "" CACHE FILEPATH "Optional path to the required clang-format 22.1.3 executable")
set(SPARKLE_CLANG_TIDY_EXECUTABLE "" CACHE FILEPATH "Optional path to the required clang-tidy 22.1.3 executable")

add_custom_target(
    code_style_check
    COMMAND ${SPARKLE_POWERSHELL_EXECUTABLE}
        -NoProfile
        -ExecutionPolicy Bypass
        -File ${CMAKE_SOURCE_DIR}/CMake/CodeStyle.ps1
        -Mode Check
        -ClangFormatPath "${SPARKLE_CLANG_FORMAT_EXECUTABLE}"
        -ClangTidyPath "${SPARKLE_CLANG_TIDY_EXECUTABLE}"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking tracked owned source against the Sparkle code-style policy..."
    USES_TERMINAL
    VERBATIM
)

add_custom_target(
    code_style_format
    COMMAND ${SPARKLE_POWERSHELL_EXECUTABLE}
        -NoProfile
        -ExecutionPolicy Bypass
        -File ${CMAKE_SOURCE_DIR}/CMake/CodeStyle.ps1
        -Mode Format
        -ClangFormatPath "${SPARKLE_CLANG_FORMAT_EXECUTABLE}"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting tracked owned source with the Sparkle code-style policy..."
    USES_TERMINAL
    VERBATIM
)
