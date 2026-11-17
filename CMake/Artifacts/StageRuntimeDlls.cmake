# An empty runtime graph is valid for a fully static product.
foreach(_runtime_dll IN LISTS SPARKLE_RUNTIME_DLLS)
    cmake_path(GET _runtime_dll FILENAME _runtime_name)
    file(COPY_FILE "${_runtime_dll}" "${SPARKLE_RUNTIME_DIRECTORY}/${_runtime_name}" ONLY_IF_DIFFERENT)
endforeach()
