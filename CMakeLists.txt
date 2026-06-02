# Copyright © 2026, Alexandre Coderre-Chabot.

# This file is part of Lector (https://github.com/acodcha/lector), a C++ library for parsing command
# line arguments. Lector is licensed under the MIT License (https://mit-license.org).

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# - The above copyright notice and this permission notice shall be included in all copies or
#   substantial portions of the Software.
# - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
#   BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
#   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
  lector
  VERSION 1.0.0
  LANGUAGES CXX
  DESCRIPTION "C++ library for parsing command line arguments."
  HOMEPAGE_URL "https://github.com/acodcha/lector"
)

# Set the default build type to Release without overriding user choices.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(
    CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE
  )
  set_property(
    CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
  )
endif()

option(LECTOR_TEST "Configure the Lector library tests." OFF)
option(LECTOR_COVERAGE "Configure the Lector library code coverage." OFF)

add_library(lector INTERFACE)

# Required for standard installation paths (like CMAKE_INSTALL_INCLUDEDIR)
include(GNUInstallDirs)

target_include_directories(
  lector
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Force consumers to use at least C++17, replacing global variables.
target_compile_features(lector INTERFACE cxx_std_17)

# Install the target and link it to an export set.
install(
  TARGETS lector
  EXPORT lectorTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the actual header files.
install(
  FILES ${PROJECT_SOURCE_DIR}/lector.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export the targets to a CMake script so find_package() works.
install(
  EXPORT lectorTargets
  FILE lectorTargets.cmake
  NAMESPACE lector::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lector
)

# Generate and install package configuration and version files.
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  lectorConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Create a basic configuration file that points to the exported targets.
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lectorConfig.cmake
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/lectorTargets.cmake\")\n"
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/lectorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/lectorConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lector
)

# Configure tests.
if(LECTOR_TEST)
  enable_testing()

  # Find or fetch GoogleTest.
  find_package(GTest QUIET)
  if(GTest_FOUND)
    message(STATUS "The GoogleTest library was found at: ${GTest_CONFIG}")
  else()
    include(FetchContent)
    FetchContent_Declare(
      GoogleTest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG main
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(GoogleTest)
    message(STATUS "The GoogleTest library was fetched from: https://github.com/google/googletest")
  endif()

  add_executable(lector_test ${PROJECT_SOURCE_DIR}/lector.test.cpp)

  # Apply strict warnings ONLY to the tests, checking the compiler type first.
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lector_test PRIVATE -Wall -Wextra -Wno-return-type -Wpedantic)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(lector_test PRIVATE /W4)
  endif()

  # Basic compiler hook-up for the code coverage option.
  if(LECTOR_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lector_test PRIVATE --coverage -O0)
    target_link_options(lector_test PRIVATE --coverage)
  endif()

  target_link_libraries(lector_test PRIVATE lector GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(lector_test)

  message(STATUS "The Lector library tests were configured. Build and run with: \"cmake --build . && ctest\"")
else()
  message(STATUS "The Lector library tests were not configured. Run \"cmake -B build -S . -D LECTOR_TEST=ON\" to configure.")
endif()
