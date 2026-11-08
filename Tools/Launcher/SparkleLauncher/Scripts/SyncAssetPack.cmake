cmake_minimum_required(VERSION 3.20)

include("${CMAKE_CURRENT_LIST_DIR}/SyncAssetPackRequest.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SyncAssetPackPaths.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SyncAssetPackPublication.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SyncAssetPackArchive.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SyncAssetPackFiles.cmake")

sparkle_validate_asset_pack_request()
sparkle_resolve_asset_pack_paths(
    "${SPARKLE_PACK_PROJECT_ROOT}" "${SPARKLE_PACK_EXTRACT_ROOT}" "${SPARKLE_PACK_CACHE_ROOT}"
    "${SPARKLE_PACK_ROOT_RELATIVE}" "${SPARKLE_PACK_REQUIRED_RELATIVE}" "${SPARKLE_PACK_ID}" _paths)
sparkle_lock_asset_pack("${_paths_cache_root}" "${SPARKLE_PACK_ID}")
sparkle_restore_asset_pack("${_paths_extract_root}" "${_paths_backup_root}")
if(EXISTS "${_paths_required_path}")
    message(STATUS "${SPARKLE_PACK_ID} is already acquired: ${_paths_required_path}")
    return()
endif()

if(DEFINED SPARKLE_PACK_FILES_MANIFEST AND NOT SPARKLE_PACK_FILES_MANIFEST STREQUAL "")
    sparkle_stage_asset_pack_files(
        "${SPARKLE_PACK_ID}" "${SPARKLE_PACK_URL}" "${SPARKLE_PACK_FILES_MANIFEST}"
        "${_paths_project_root}" "${_paths_staging_root}" "${SPARKLE_PACK_ROOT_RELATIVE}" _payload_hash)
    set(_payload_hash_label "SourceFilesSha256")
else()
    sparkle_stage_asset_pack_archive(
        "${SPARKLE_PACK_ID}" "${SPARKLE_PACK_URL}" "${SPARKLE_PACK_ARCHIVE_NAME}"
        "${SPARKLE_PACK_ARCHIVE_BYTES}" "${SPARKLE_PACK_ARCHIVE_SHA256}" "${_paths_cache_root}" "${_paths_staging_root}")
    string(TOLOWER "${SPARKLE_PACK_ARCHIVE_SHA256}" _payload_hash)
    set(_payload_hash_label "ArchiveSha256")
endif()
sparkle_write_asset_pack_receipt(
    "${_paths_staging_root}" "${SPARKLE_PACK_ROOT_RELATIVE}" "${SPARKLE_PACK_ID}" "${SPARKLE_PACK_URL}"
    "${SPARKLE_PACK_SOURCE_PAGE}" "${SPARKLE_PACK_VERSION}" "${SPARKLE_PACK_LICENSE}"
    "${SPARKLE_PACK_ARCHIVE_NAME}" "${SPARKLE_PACK_ARCHIVE_BYTES}" "${_payload_hash_label}" "${_payload_hash}")
sparkle_validate_staged_asset_pack("${_paths_staging_root}" "${SPARKLE_PACK_ROOT_RELATIVE}" "${SPARKLE_PACK_REQUIRED_RELATIVE}" "${SPARKLE_PACK_ID}")
sparkle_publish_asset_pack("${_paths_staging_root}" "${_paths_backup_root}" "${_paths_extract_root}" "${SPARKLE_PACK_ID}")
message(STATUS "Asset pack ${SPARKLE_PACK_ID} is ready. SHA-256: ${_payload_hash}")
