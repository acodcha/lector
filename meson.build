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

project(
  'lector',
  'cpp',
  version : '1.0.0',
  default_options : [
    'cpp_std=c++17',
    'warning_level=3',  # Sets -Wall -Wextra on GCC and Clang, and /W4 on MSVC.
    'optimization=3'  # Sets -O3 on GCC and Clang, and /O2 on MSVC.
  ]
)

# Handle compiler-specific flags cleanly without breaking other platforms.
cpp_compiler = meson.get_compiler('cpp')
if cpp_compiler.get_id() in ['gcc', 'clang']
  add_project_arguments(
    '-Wpedantic',
    '-Wno-return-type',
    language : 'cpp'
  )
elif cpp_compiler.get_id() == 'msvc'
  add_project_arguments(
    '/permissive-',  # Enforces strict C++ standards compliance in MSVC.
    language : 'cpp'
  )
endif

# Define the GoogleTest dependency.
gtest = dependency(
  'gtest',
  main : true,
  fallback : ['gtest', 'gtest_main_dep'],
  required : true
)

# Header-only library dependency configuration.
# Leaving '.' allows users to do #include <lector/print.hpp>.
lector = declare_dependency(
  include_directories : include_directories('.')
)

# Expose the dependency externally.
meson.override_dependency('lector', lector)

# Dynamically generate targets for the test files.
test_modules = ['arguments', 'parse', 'print', 'text']
foreach module : test_modules
  test_exe = executable(
    module + '_test',
    'lector/' + module + '.test.cpp',
    dependencies : [gtest, lector]
  )
  test('lector_' + module + '_test', test_exe)
endforeach

# Windows MSVC coverage configuration with OpenCppCoverage.
if cpp_compiler.get_id() == 'msvc'
  opencppcoverage = find_program('OpenCppCoverage', required: false)
  if opencppcoverage.found()
    # Create a custom 'coverage' target that outputs directly to the project root.
    run_target('coverage',
      command: [
        opencppcoverage,
        '--sources', meson.project_source_root() + '\\lector',
        # Change output from meson.project_build_root() to meson.project_source_root().
        '--export_type', 'html:' + meson.project_source_root() + '\\coverage',
        '--', 'meson', 'test', '-C', meson.project_build_root()
      ]
    )
    message('The OpenCppCoverage utility was found at: ' + opencppcoverage.full_path())
    message('Run "meson compile -C build coverage" to compute code coverage with the OpenCppCoverage utility.')
  else
    warning('The OpenCppCoverage utility was not found. Download it from: https://github.com/OpenCppCoverage/OpenCppCoverage')
  endif
endif
