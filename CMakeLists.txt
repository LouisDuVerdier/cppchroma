cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as default")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set the project name
project(cppchroma VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(cmake/re2.cmake)

add_compile_options(-Wall -Wextra)

# Include directories
include_directories(include)

# Add the executable
add_executable(cppchroma
  src/main.cpp
  src/cppchroma.cpp
)

# Add tests
enable_testing()
add_subdirectory(tests)
