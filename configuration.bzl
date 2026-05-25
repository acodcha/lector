# Copyright © 2026, Alexandre Coderre-Chabot.

# This file is part of Args (https://github.com/acodcha/args), a C++ library for parsing command
# line arguments. Args is licensed under the MIT License (https://mit-license.org).

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#   - The above copyright notice and this permission notice shall be included in all copies or
#     substantial portions of the Software.
#   - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
#     BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#     NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
#     DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
#     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

"""
Defines the Bazel macros used throughout the Args library.
"""

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

# Standard set of C++ compiler options used in the Args library's C++ targets.
ARGS_CPP_COMPILER_OPTIONS = [
    "-O3",
    "-Wall",
    "-Wextra",
    "-Wno-return-type",
    "-Wpedantic",
    "-std=c++17",
]

def args_cpp_binary(name, srcs, deps = [], **kwargs):
    """
    C++ binary in the Args library.

    Args:
      name: Required. Name of the binary.
      srcs: Required. List of source files.
      deps: Optional. List of dependencies.
      **kwargs: Additional arguments passed to the native cc_binary rule.
    """
    cc_binary(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = ["//visibility:public"],
        copts = ARGS_CPP_COMPILER_OPTIONS,
        **kwargs
    )

def args_cpp_library(name, hdrs, srcs, visibility, deps = [], **kwargs):
    """
    C++ library in the Args library.

    Args:
      name: Required. Name of the library.
      hdrs: Required. List of header files.
      srcs: Required. List of source files.
      visibility: Required. List of visibility labels.
      deps: Optional. List of dependencies.
      **kwargs: Additional arguments passed to the native cc_library rule.
    """
    cc_library(
        name = name,
        hdrs = hdrs,
        srcs = srcs,
        deps = deps,
        visibility = visibility,
        copts = ARGS_CPP_COMPILER_OPTIONS,
        **kwargs
    )

def args_cpp_test(name, size, srcs, data = [], deps = [], **kwargs):
    """
    C++ test in the Args library.

    Args:
      name: Required. Name of the test.
      size: Required. Size of the test: "small", "medium", "large", or "enormous".
      srcs: Required. List of source files.
      data: Optional. List of data files.
      deps: Optional. List of dependencies.
      **kwargs: Additional arguments passed to the native cc_test rule.
    """
    cc_test(
        name = name,
        data = data,
        size = size,
        srcs = srcs,
        visibility = ["//visibility:public"],
        deps = deps + ["@googletest//:gtest_main"],
        copts = ARGS_CPP_COMPILER_OPTIONS,
        **kwargs
    )
