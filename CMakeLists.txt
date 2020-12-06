cmake_minimum_required(VERSION 3.12..3.15)
project(rayfuck VERSION 0.0.0
  DESCRIPTION "Some fucking around"
  LANGUAGES C)
find_package(OpenMP)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_subdirectory(third-party/raylib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third-party/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)
#No need for examples here since it's embedded
SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) #enable interprocedural optimization for all projects
cmake_policy(SET CMP0076 NEW)
add_library(utility STATIC)
add_executable(particle-dabbling)
add_executable(nbody)
add_executable(circular_buffer_test)
add_executable(timing_test)
add_executable(grid_partition_test)
target_link_libraries(particle-dabbling raylib utility)
target_link_libraries(nbody raylib OpenMP::OpenMP_C utility Threads::Threads)
target_link_libraries(utility raylib)
target_link_libraries(timing_test utility)
target_link_libraries(grid_partition_test utility)
#add_subdirectory(src/particle)


add_sanitizers(particle-dabbling)
add_subdirectory(src)