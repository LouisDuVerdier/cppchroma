cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as default")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

project(cppchroma VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(cmake/re2.cmake)
include(cmake/yamlcpp.cmake)

include_directories(include)
add_executable(cppchroma
  src/main.cpp
  src/config.cpp
  src/cppchroma.cpp
)
target_compile_options(cppchroma PRIVATE -W -Wall -Wextra -g)
target_link_libraries(cppchroma PRIVATE re2::re2 yaml-cpp::yaml-cpp absl::log absl::log_initialize)

# Add tests
enable_testing()
add_subdirectory(tests)
