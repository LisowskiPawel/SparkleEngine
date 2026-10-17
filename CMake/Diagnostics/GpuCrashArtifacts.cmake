include_guard(GLOBAL)

function(sparkle_configure_gpu_crash_artifacts target_name)
    if(NOT SPARKLE_WITH_GPU_CRASH_DEBUGGING)
        return()
    endif()
    set(_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/GpuCrashArtifacts.ps1")
    set(_snapshot "${CMAKE_BINARY_DIR}/private/gpu-crash/${target_name}/$<CONFIG>/source.json")
    add_custom_target(${target_name}GpuCrashSource
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File "${_script}"
            -Action Prepare -RepositoryRoot "${SPARKLE_REPOSITORY_ROOT}"
            -SnapshotPath "${_snapshot}" -Profile "$<CONFIG>" -ReceiptPath "$<TARGET_FILE_DIR:${target_name}>/gpu-crash-product.json"
        VERBATIM)
    add_dependencies(${target_name} ${target_name}GpuCrashSource)
    add_custom_target(${target_name}GpuCrashReceipt
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File "${_script}"
            -Action Publish -RepositoryRoot "${SPARKLE_REPOSITORY_ROOT}"
            -SnapshotPath "${_snapshot}" -Profile "$<CONFIG>"
            -ExecutablePath "$<TARGET_FILE:${target_name}>" -PdbPath "$<TARGET_PDB_FILE:${target_name}>"
            -SymbolRoot "$<TARGET_PDB_FILE_DIR:${target_name}>/retained"
            -RuntimeFiles "$<JOIN:$<TARGET_RUNTIME_DLLS:${target_name}>,|>"
        VERBATIM)
    add_dependencies(${target_name}GpuCrashReceipt ${target_name})
endfunction()

function(sparkle_stage_aftermath_runtime target_name)
    if(NOT SPARKLE_WITH_GPU_CRASH_DEBUGGING)
        return()
    endif()
    set(_destination "${_sparkle_development_artifact_root}/Engine/Binaries/Aftermath/$<CONFIG>")
    add_custom_target(${target_name}AftermathRuntime
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_destination}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SPARKLE_AFTERMATH_SDK_ROOT}/lib/x64/GFSDK_Aftermath_Lib.x64.dll" "${_destination}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SPARKLE_AFTERMATH_SDK_ROOT}/LICENSE" "${_destination}/LICENSE"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SPARKLE_AFTERMATH_SDK_ROOT}/nsight-aftermath-usage-guidelines.txt" "${_destination}/nsight-aftermath-usage-guidelines.txt"
        VERBATIM)
    add_dependencies(${target_name} ${target_name}AftermathRuntime)
endfunction()
