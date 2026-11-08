function(sparkle_validate_asset_pack_request )
    set(_required_variables
        SPARKLE_PACK_ID
        SPARKLE_PACK_URL
        SPARKLE_PACK_SOURCE_PAGE
        SPARKLE_PACK_VERSION
        SPARKLE_PACK_LICENSE
        SPARKLE_PACK_PROJECT_ROOT
        SPARKLE_PACK_CACHE_ROOT
        SPARKLE_PACK_EXTRACT_ROOT
        SPARKLE_PACK_ROOT_RELATIVE
        SPARKLE_PACK_REQUIRED_RELATIVE)

    if(NOT DEFINED SPARKLE_PACK_FILES_MANIFEST OR SPARKLE_PACK_FILES_MANIFEST STREQUAL "")
        list(APPEND _required_variables
            SPARKLE_PACK_ARCHIVE_NAME
            SPARKLE_PACK_ARCHIVE_BYTES
            SPARKLE_PACK_ARCHIVE_SHA256)
    endif()

    foreach(_variable IN LISTS _required_variables)
        if(NOT DEFINED ${_variable} OR "${${_variable}}" STREQUAL "")
            message(FATAL_ERROR "Asset pack sync is missing ${_variable}.")
        endif()
    endforeach()

    if(NOT SPARKLE_PACK_ID MATCHES "^[A-Za-z0-9_-]+$")
        message(FATAL_ERROR "Asset pack id is not safe for staging paths: ${SPARKLE_PACK_ID}")
    endif()
    if(NOT SPARKLE_PACK_URL MATCHES "^https://")
        message(FATAL_ERROR "Asset pack download URL must use HTTPS: ${SPARKLE_PACK_URL}")
    endif()
    if(NOT DEFINED SPARKLE_PACK_FILES_MANIFEST OR SPARKLE_PACK_FILES_MANIFEST STREQUAL "")
        string(LENGTH "${SPARKLE_PACK_ARCHIVE_SHA256}" _expected_sha256_length)
        if(NOT _expected_sha256_length EQUAL 64 OR NOT SPARKLE_PACK_ARCHIVE_SHA256 MATCHES "^[0-9A-Fa-f]+$")
            message(FATAL_ERROR "Asset pack SHA-256 is invalid: ${SPARKLE_PACK_ARCHIVE_SHA256}")
        endif()
        string(TOLOWER "${SPARKLE_PACK_ARCHIVE_SHA256}" _expected_sha256)
    elseif(NOT SPARKLE_PACK_URL MATCHES "/$")
        message(FATAL_ERROR "Loose-file asset pack source URL must end in '/': ${SPARKLE_PACK_URL}")
    endif()
endfunction()
