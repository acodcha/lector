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
# This automatically handles -O3 on GCC and Clang and /O2 on MSVC.
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

# Required for standard installation paths like CMAKE_INSTALL_INCLUDEDIR.
include(GNUInstallDirs)

target_include_directories(
  lector
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Force consumers to use at least C++17, replacing global variables.
target_compile_features(lector INTERFACE cxx_std_17)

# Instrument the library headers for code coverage.
if(LECTOR_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(lector INTERFACE --coverage -O0)
  target_link_options(lector INTERFACE --coverage)
endif()

# Install the target and link it to an export set.
install(
  TARGETS lector
  EXPORT lectorTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the actual header files.
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/lector
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

  include(GoogleTest)

  set(LECTOR_TEST_MODULES arguments parse print)

  foreach(module ${LECTOR_TEST_MODULES})
    set(LECTOR_TEST_TARGET lector_${module}_test)

    add_executable(${LECTOR_TEST_TARGET} ${PROJECT_SOURCE_DIR}/lector/${module}.test.cpp)

    # Apply strict warnings only to the tests, checking the compiler type first.
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(${LECTOR_TEST_TARGET} PRIVATE -Wall -Wextra -Wno-return-type -Wpedantic)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      target_compile_options(${LECTOR_TEST_TARGET} PRIVATE /W4 /permissive-)
    endif()

    # Basic compiler hook-up for the code coverage option.
    if(LECTOR_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(${LECTOR_TEST_TARGET} PRIVATE --coverage -O0)
      target_link_options(${LECTOR_TEST_TARGET} PRIVATE --coverage)
    endif()

    target_link_libraries(${LECTOR_TEST_TARGET} PRIVATE lector GTest::gtest_main)

    # Automatically register tests with ctest.
    gtest_discover_tests(${LECTOR_TEST_TARGET})
  endforeach()

  message(STATUS "The Lector library tests were configured. Build and run with: \"cmake --build build --config Release --parallel . && ctest --test-dir build -C Release\"")
else()
  message(STATUS "The Lector library tests were not configured. Configure with: \"cmake -S . -B build -D CMAKE_BUILD_TYPE=Release -D LECTOR_TEST=ON\"")
endif()

# Configure code coverage.
if(LECTOR_COVERAGE)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "Code coverage requires either the GCC or Clang C++ compilers.")
  endif()

  # Find required tools for code coverage.
  find_program(LCOV_EXECUTABLE lcov)
  find_program(GENHTML_EXECUTABLE genhtml)

  if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
    message(STATUS "The LCOV package was found at: ${LCOV_EXECUTABLE}")
    message(STATUS "The GENHTML package was found at: ${GENHTML_EXECUTABLE}")

    # Create a custom target for generating code coverage reports.
    add_custom_target(
      coverage
      COMMAND ${LCOV_EXECUTABLE} --directory . --base-directory ${CMAKE_CURRENT_SOURCE_DIR} --capture --output-file coverage.info --ignore-errors empty,mismatch
      COMMAND ${LCOV_EXECUTABLE} --extract coverage.info ${CMAKE_CURRENT_SOURCE_DIR}/lector/* --output-file coverage.info.cleaned --ignore-errors empty,unused
      COMMAND ${GENHTML_EXECUTABLE} coverage.info.cleaned --output-directory ${CMAKE_CURRENT_SOURCE_DIR}/coverage --ignore-errors empty
      COMMAND ${CMAKE_COMMAND} -E echo "Generated code coverage report in: ${CMAKE_CURRENT_SOURCE_DIR}/coverage/index.html"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating code coverage report with lcov."
      VERBATIM
    )
  else()
    message(WARNING "The LCOV package was not found. Install LCOV with: \"sudo apt install lcov\"")
  endif()

  message(STATUS "The Lector library code coverage was configured. Compute code coverage with: \"cmake --build build --config Debug --parallel && ctest --test-dir build -C Debug && cmake --build build --target coverage\"")
else()
  message(STATUS "The Lector library code coverage was not configured. Configure with: \"cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug -D LECTOR_TEST=ON -D LECTOR_COVERAGE=ON\"")
endif()
