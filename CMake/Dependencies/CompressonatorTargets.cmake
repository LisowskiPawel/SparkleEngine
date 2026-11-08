# Compose the main block encoder and its separately compiled instruction-set variants.
add_library(CMP_Core STATIC
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc1_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc2_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc3_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc4_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc5_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc6_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/shaders/bc7_encode_kernel.cpp
    ${compressonator_SOURCE_DIR}/cmp_core/source/cmp_core.cpp
    ${compressonator_SOURCE_DIR}/applications/_libs/cmp_math/cpu_extensions.cpp
    ${compressonator_SOURCE_DIR}/applications/_libs/cmp_math/cmp_math_common.cpp
)

target_include_directories(CMP_Core PUBLIC
    ${compressonator_SOURCE_DIR}/cmp_core/source
    ${compressonator_SOURCE_DIR}/cmp_core/shaders
    ${compressonator_SOURCE_DIR}/applications/_libs/cmp_math
)

# Each variant is compiled with its own arch flags, then linked into CMP_Core.

add_library(CMP_Core_SSE STATIC
    ${compressonator_SOURCE_DIR}/cmp_core/source/core_simd_sse.cpp
)
target_include_directories(CMP_Core_SSE PRIVATE
    ${compressonator_SOURCE_DIR}/cmp_core/source
    ${compressonator_SOURCE_DIR}/cmp_core/shaders
    ${compressonator_SOURCE_DIR}/applications/_libs/cmp_math
)

add_library(CMP_Core_AVX STATIC
    ${compressonator_SOURCE_DIR}/cmp_core/source/core_simd_avx.cpp
)
target_include_directories(CMP_Core_AVX PRIVATE
    ${compressonator_SOURCE_DIR}/cmp_core/source
    ${compressonator_SOURCE_DIR}/cmp_core/shaders
    ${compressonator_SOURCE_DIR}/applications/_libs/cmp_math
)

add_library(CMP_Core_AVX512 STATIC
    ${compressonator_SOURCE_DIR}/cmp_core/source/core_simd_avx512.cpp
)
target_include_directories(CMP_Core_AVX512 PRIVATE
    ${compressonator_SOURCE_DIR}/cmp_core/source
    ${compressonator_SOURCE_DIR}/cmp_core/shaders
    ${compressonator_SOURCE_DIR}/applications/_libs/cmp_math
)

# Architecture-specific compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(CMP_Core        PRIVATE -msse4.1)
    target_compile_options(CMP_Core_SSE    PRIVATE -msse4.1)
    target_compile_options(CMP_Core_AVX    PRIVATE -mavx2)
    target_compile_options(CMP_Core_AVX512 PRIVATE -mavx512f)
elseif(MSVC)
    # SSE2 is default on x64 MSVC - no flag needed
    target_compile_options(CMP_Core_AVX    PRIVATE /arch:AVX2)
    target_compile_options(CMP_Core_AVX512 PRIVATE /arch:AVX512)
endif()

target_link_libraries(CMP_Core PRIVATE CMP_Core_SSE CMP_Core_AVX CMP_Core_AVX512)

# Silence warnings in third-party code
if(MSVC)
    target_compile_options(CMP_Core        PRIVATE /W0)
    target_compile_options(CMP_Core_SSE    PRIVATE /W0)
    target_compile_options(CMP_Core_AVX    PRIVATE /W0)
    target_compile_options(CMP_Core_AVX512 PRIVATE /W0)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(CMP_Core        PRIVATE -w)
    target_compile_options(CMP_Core_SSE    PRIVATE -w)
    target_compile_options(CMP_Core_AVX    PRIVATE -w)
    target_compile_options(CMP_Core_AVX512 PRIVATE -w)
endif()

set_target_properties(CMP_Core CMP_Core_SSE CMP_Core_AVX CMP_Core_AVX512
    PROPERTIES FOLDER "ThirdParty/Compressonator"
)
