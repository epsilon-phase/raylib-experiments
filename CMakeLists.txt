cmake_minimum_required(VERSION 3.12..3.15)
project(rayfuck VERSION 0.0.0
  DESCRIPTION "Some fucking around"
  LANGUAGES C)
add_subdirectory(third-party/raylib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third-party/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)
#No need for examples here since it's embedded
SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW) #enable interprocedural optimization for all projects
cmake_policy(SET CMP0076 NEW)
add_executable(particle-dabbling)
add_executable(circular_buffer_test)
target_link_libraries(raylib-experiment-1 raylib)
add_subdirectory(src)
add_sanitizers(raylib-experiment-1)