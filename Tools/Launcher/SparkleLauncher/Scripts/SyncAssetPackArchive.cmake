function(sparkle_inspect_cached_archive archive_path archive_bytes expected_sha256 pack_id out_valid)
    set(_archive_is_valid FALSE)
    if(EXISTS "${archive_path}")
        file(SIZE "${archive_path}" _cached_archive_bytes)
        if(_cached_archive_bytes EQUAL archive_bytes)
            file(SHA256 "${archive_path}" _cached_archive_sha256)
            string(TOLOWER "${_cached_archive_sha256}" _cached_archive_sha256)
            if(_cached_archive_sha256 STREQUAL expected_sha256)
                set(_archive_is_valid TRUE)
            else()
                message(STATUS "Discarding cached archive with the wrong SHA-256 for ${pack_id}.")
                file(REMOVE "${archive_path}")
            endif()
        else()
            message(STATUS "Discarding incomplete cached archive for ${pack_id}.")
            file(REMOVE "${archive_path}")
        endif()
    endif()

    set(${out_valid} ${_archive_is_valid} PARENT_SCOPE)
endfunction()

function(sparkle_download_archive url archive_path archive_bytes expected_sha256 pack_id)
    set(partial_archive_path "${archive_path}.partial")
    file(REMOVE "${partial_archive_path}")
    message(STATUS "Downloading asset pack ${pack_id} (${archive_bytes} bytes).")
    file(DOWNLOAD
        "${url}"
        "${partial_archive_path}"
        SHOW_PROGRESS
        TLS_VERIFY ON
        INACTIVITY_TIMEOUT 180
        TIMEOUT 21600
        STATUS _download_status)
    list(GET _download_status 0 _download_code)
    list(GET _download_status 1 _download_message)
    if(NOT _download_code EQUAL 0)
        file(REMOVE "${partial_archive_path}")
        message(FATAL_ERROR "Failed to download ${pack_id}: ${_download_message}")
    endif()

    file(SIZE "${partial_archive_path}" _downloaded_archive_bytes)
    if(NOT _downloaded_archive_bytes EQUAL archive_bytes)
        file(REMOVE "${partial_archive_path}")
        message(FATAL_ERROR
            "Downloaded ${pack_id} archive has ${_downloaded_archive_bytes} bytes; expected ${archive_bytes}.")
    endif()

    file(SHA256 "${partial_archive_path}" _downloaded_archive_sha256)
    string(TOLOWER "${_downloaded_archive_sha256}" _downloaded_archive_sha256)
    if(NOT _downloaded_archive_sha256 STREQUAL expected_sha256)
        file(REMOVE "${partial_archive_path}")
        message(FATAL_ERROR
            "Downloaded ${pack_id} archive SHA-256 is ${_downloaded_archive_sha256}; expected ${expected_sha256}.")
    endif()
    file(RENAME "${partial_archive_path}" "${archive_path}")
endfunction()

function(sparkle_stage_asset_pack_archive pack_id url archive_name archive_bytes archive_sha256 cache_root staging_root)
    string(TOLOWER "${archive_sha256}" expected_sha256)
    set(_archive_path "${cache_root}/${archive_name}")
    cmake_path(NORMAL_PATH _archive_path OUTPUT_VARIABLE _archive_path)
    cmake_path(IS_PREFIX cache_root "${_archive_path}" NORMALIZE _archive_is_cache_owned)
    if(NOT _archive_is_cache_owned OR _archive_path STREQUAL cache_root)
        message(FATAL_ERROR "Asset pack archive must remain below the launcher cache: ${_archive_path}")
    endif()

    sparkle_inspect_cached_archive("${_archive_path}" "${archive_bytes}" "${expected_sha256}" "${pack_id}" _archive_is_valid)
    if(NOT _archive_is_valid)
        sparkle_download_archive("${url}" "${_archive_path}" "${archive_bytes}" "${expected_sha256}" "${pack_id}")
    endif()
    file(REMOVE_RECURSE "${staging_root}")
    file(MAKE_DIRECTORY "${staging_root}")
    message(STATUS "Extracting ${pack_id} into a transactional staging directory.")
    file(ARCHIVE_EXTRACT INPUT "${_archive_path}" DESTINATION "${staging_root}")
endfunction()
