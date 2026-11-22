set(SPARKLE_BOOST_REVISION "afdfa32505af73e3d208144b3f623f0096cb62b6")
set(SPARKLE_BOOST_VERSION "1.92.0")

# The supported Process target's source closure; the root pin owns all gitlinks.
set(_sparkle_boost_modules
    libs/algorithm libs/align libs/array libs/asio libs/assert libs/atomic libs/bind
    libs/compat libs/concept_check libs/config libs/container libs/container_hash
    libs/context libs/conversion libs/core libs/date_time libs/describe libs/detail
    libs/exception libs/filesystem libs/function libs/function_types libs/fusion
    libs/integer libs/intrusive libs/io libs/iterator libs/lexical_cast libs/move libs/mp11 libs/mpl
    libs/numeric/conversion libs/optional libs/pool libs/predef libs/preprocessor
    libs/process libs/range libs/regex libs/scope libs/smart_ptr libs/system
    libs/throw_exception libs/tokenizer libs/tuple libs/type_index libs/type_traits
    libs/typeof libs/unordered libs/utility libs/variant2 libs/winapi)

set(SPARKLE_BOOST_REQUIRED_PATHS
    CMakeLists.txt .gitmodules LICENSE_1_0.txt tools/cmake/include/BoostRoot.cmake
    libs/process/include/boost/process/v2/process.hpp
    libs/process/src/windows/default_launcher.cpp
    libs/asio/include/boost/asio/awaitable.hpp
    libs/asio/include/boost/asio/readable_pipe.hpp)
foreach(_module IN LISTS _sparkle_boost_modules)
    list(APPEND SPARKLE_BOOST_REQUIRED_PATHS "${_module}/CMakeLists.txt")
endforeach()

sparkle_declare_git_dependency(boost-process
    https://github.com/boostorg/boost.git ${SPARKLE_BOOST_REVISION}
    GIT_SUBMODULES ${_sparkle_boost_modules} tools/cmake
    GIT_SUBMODULES_RECURSE FALSE GIT_CONFIG submodule.fetchJobs=4
    ID boost-process VERSION "${SPARKLE_BOOST_VERSION}" PACKAGE_NAME "Boost" LICENSE_ID "BSL-1.0"
    REQUIRED_PATHS ${SPARKLE_BOOST_REQUIRED_PATHS})

if(SPARKLE_SOURCE_DEPENDENCY_METADATA_ONLY OR NOT SPARKLE_SYNC_SOURCE_DEPENDENCY STREQUAL "")
    return()
endif()

function(sparkle_add_boost_process)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_TESTING OFF)
    set(BOOST_INCLUDE_LIBRARIES process)
    set(BOOST_PROCESS_USE_STD_FS ON)
    set(BOOST_SKIP_INSTALL_RULES ON)
    sparkle_add_dependency_subdirectory(${boost-process_SOURCE_DIR} ${boost-process_BINARY_DIR} EXCLUDE_FROM_ALL)
    get_directory_property(_version DIRECTORY "${boost-process_SOURCE_DIR}" DEFINITION PROJECT_VERSION)
    if(NOT _version STREQUAL SPARKLE_BOOST_VERSION)
        message(FATAL_ERROR "Boost source version '${_version}' does not match '${SPARKLE_BOOST_VERSION}'.")
    endif()
    target_compile_definitions(boost_process PUBLIC BOOST_ALL_NO_LIB)
    if(WIN32)
        target_compile_definitions(boost_process PUBLIC _WIN32_WINNT=0x0A00)
    endif()
    foreach(_target IN ITEMS boost_process boost_container boost_context boost_date_time)
        set_target_properties(${_target} PROPERTIES FOLDER "ThirdParty/boost-process")

        sparkle_add_dependency_source_package(${_target} boost-process)
    endforeach()
endfunction()
sparkle_add_boost_process()
