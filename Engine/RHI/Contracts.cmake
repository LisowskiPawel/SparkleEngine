include_guard(GLOBAL)

# Public vocabulary can be consumed without linking a device or native backend.
add_library(SparkleRHIContracts INTERFACE)
target_include_directories(SparkleRHIContracts INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_LIST_DIR}/Public)
target_link_libraries(SparkleRHIContracts INTERFACE SparkleCore)
target_compile_features(SparkleRHIContracts INTERFACE cxx_std_20)
