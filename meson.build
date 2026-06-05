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
  default_options : ['cpp_std=c++17']
)

# Find Google Test, or download it via wrap if not available locally.
gtest_dep = dependency(
  'gtest',
  main : true,
  fallback : ['gtest', 'gtest_main_dep'],
  required : true
)

# Compiler options.
add_global_arguments(
  '-O3',
  '-Wall',
  '-Wextra',
  '-Wno-return-type',
  '-Wpedantic',
  language : 'cpp'
)

# Define the header-only library.
lector = declare_dependency(
  include_directories : include_directories('.')
)

# Define the test.
test('lector_test',
  executable('lector_test',
    'lector.test.cpp',
    dependencies : [gtest_dep, lector]
  )
)
