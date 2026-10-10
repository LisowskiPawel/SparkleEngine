# Release identity shared by the workspace and standalone Launcher entry points.
include_guard(GLOBAL)
set(SPARKLE_VERSION "0.1.0")
set(SPARKLE_PUBLISHER "Pawel Lisowski")
set(SPARKLE_COPYRIGHT "Copyright (c) 2026 Pawel Lisowski")

function(sparkle_configure_product_identity target_name product_name)
    set_target_properties(${target_name} PROPERTIES VERSION "${SPARKLE_VERSION}")
    if(WIN32)
        set(SPARKLE_PRODUCT_NAME "${product_name}")
        set(SPARKLE_PRODUCT_EXECUTABLE "${target_name}.exe")
        string(REPLACE "." "," SPARKLE_VERSION_COMPONENTS "${SPARKLE_VERSION}")
        set(identity_resource "${CMAKE_CURRENT_BINARY_DIR}/${target_name}Version.rc")
        configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/SparkleProductVersion.rc.in"
            "${identity_resource}" @ONLY)
        target_sources(${target_name} PRIVATE "${identity_resource}")
    endif()
endfunction()
