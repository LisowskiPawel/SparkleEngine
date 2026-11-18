# Per-configuration defaults preserve private dependency outputs when upstream
# CMake overrides generic directory settings.
if(CMAKE_CONFIGURATION_TYPES)
    foreach(CONFIG_TYPE IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER "${CONFIG_TYPE}" CONFIG_UPPER)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/private/runtime/${CONFIG_TYPE}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/private/lib/${CONFIG_TYPE}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/private/runtime/${CONFIG_TYPE}")
    endforeach()
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/private/runtime/${CMAKE_BUILD_TYPE}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/private/lib/${CMAKE_BUILD_TYPE}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/private/runtime/${CMAKE_BUILD_TYPE}")
endif()
