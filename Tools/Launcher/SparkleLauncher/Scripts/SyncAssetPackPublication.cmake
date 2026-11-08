function(sparkle_lock_asset_pack cache_root pack_id)
    file(MAKE_DIRECTORY "${cache_root}/Locks")
    set(_lock_path "${cache_root}/Locks/${pack_id}.lock")
    file(LOCK "${_lock_path}" GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE _lock_result)
    if(NOT _lock_result STREQUAL "0")
        message(FATAL_ERROR "Asset pack ${pack_id} is already being synchronized by another launcher operation.")
    endif()
endfunction()

function(sparkle_restore_asset_pack extract_root backup_root)
    if(EXISTS "${backup_root}")
        if(EXISTS "${extract_root}")
            file(REMOVE_RECURSE "${backup_root}")
        else()
            file(RENAME "${backup_root}" "${extract_root}")
        endif()
    endif()
endfunction()

function(sparkle_write_asset_pack_receipt staging_root root_relative pack_id url source_page version license archive_name archive_bytes hash_label payload_hash)
    set(_staged_manifest_path "${staging_root}/${root_relative}/.sparkle-acquisition.txt")
    string(TIMESTAMP _acquired_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
    file(WRITE "${_staged_manifest_path}"
        "PackId=${pack_id}\n"
        "SourceUrl=${url}\n"
        "SourcePage=${source_page}\n"
        "Version=${version}\n"
        "License=${license}\n"
        "ArchiveName=${archive_name}\n"
        "ArchiveBytes=${archive_bytes}\n"
        "${hash_label}=${payload_hash}\n"
        "AcquiredUtc=${_acquired_utc}\n")
endfunction()

function(sparkle_validate_staged_asset_pack staging_root root_relative required_relative pack_id)
    set(_staged_required_path "${staging_root}/${root_relative}/${required_relative}")
    cmake_path(NORMAL_PATH _staged_required_path OUTPUT_VARIABLE _staged_required_path)
    if(NOT EXISTS "${_staged_required_path}")
        file(REMOVE_RECURSE "${staging_root}")
        message(FATAL_ERROR "${pack_id} payload does not contain the required path: ${_staged_required_path}")
    endif()
endfunction()

function(sparkle_publish_asset_pack staging_root backup_root extract_root pack_id)
    file(REMOVE_RECURSE "${backup_root}")
    if(EXISTS "${extract_root}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E rename "${extract_root}" "${backup_root}"
            RESULT_VARIABLE _backup_result
            ERROR_VARIABLE _backup_error)
        if(NOT _backup_result EQUAL 0)
            file(REMOVE_RECURSE "${staging_root}")
            message(FATAL_ERROR "Could not preserve the existing ${pack_id} extraction: ${_backup_error}")
        endif()
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E rename "${staging_root}" "${extract_root}"
        RESULT_VARIABLE _publish_result
        ERROR_VARIABLE _publish_error)
    if(NOT _publish_result EQUAL 0)
        if(EXISTS "${backup_root}" AND NOT EXISTS "${extract_root}")
            execute_process(
                COMMAND "${CMAKE_COMMAND}" -E rename "${backup_root}" "${extract_root}"
                RESULT_VARIABLE _restore_result
                ERROR_VARIABLE _restore_error)
            if(NOT _restore_result EQUAL 0)
                message(FATAL_ERROR
                    "Could not publish or restore ${pack_id}. Backup remains at ${backup_root}: ${_restore_error}")
            endif()
        endif()
        file(REMOVE_RECURSE "${staging_root}")
        message(FATAL_ERROR "Could not publish ${pack_id}: ${_publish_error}")
    endif()
    file(REMOVE_RECURSE "${backup_root}")
endfunction()
