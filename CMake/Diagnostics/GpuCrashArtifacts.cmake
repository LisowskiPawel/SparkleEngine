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
    # Several products share this admitted variant payload. Give it one writer.
    if(NOT TARGET SparkleAftermathRuntime)
        add_custom_target(SparkleAftermathRuntime)
        set_target_properties(SparkleAftermathRuntime PROPERTIES FOLDER "Runtime")
        sparkle_stage_runtime_files(SparkleAftermathRuntime Vendor
            DESTINATION_DIRECTORY "${_sparkle_product_root}/Engine/Binaries/Aftermath/$<CONFIG>"
            FILES "${SPARKLE_AFTERMATH_SDK_ROOT}/lib/x64/GFSDK_Aftermath_Lib.x64.dll"
                "${SPARKLE_AFTERMATH_SDK_ROOT}/LICENSE"
                "${SPARKLE_AFTERMATH_SDK_ROOT}/nsight-aftermath-usage-guidelines.txt")
    endif()
    add_dependencies(${target_name} SparkleAftermathRuntime)
    sparkle_add_source_package(${target_name} "NVIDIA Aftermath" "${SPARKLE_AFTERMATH_VERSION}"
        LicenseRef-NVIDIA-Aftermath-SDK "https://developer.nvidia.com/nsight-aftermath")
endfunction()
