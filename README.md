# Lector

[![tests](https://github.com/acodcha/lector/actions/workflows/tests.yaml/badge.svg?branch=main)](https://github.com/acodcha/lector/actions/workflows/tests.yaml)

Lector is a C++ library for parsing command line arguments. Lector is hosted at <https://github.com/acodcha/lector>.

- [**Requirements**](#requirements)
- [**Configuration**](#configuration): [Bazel](#configuration-bazel), [CMake](#configuration-cmake)
- [**Usage**](#usage): [Bazel](#usage-bazel), [CMake](#usage-cmake)
- [**License**](#license)

## Requirements

Lector requires the following software to be installed on your system:

- **Git:** The Git source control system must be installed on your system. On Ubuntu, install Git with `sudo apt install git` or visit <https://git-scm.com> for alternate means of installation.
- **C++:** A C++ compiler with support for the C++17 standard or any more recent standard must be installed on your system. Any reasonably recent compiler for the C++ programming language should do, such as GCC. On Ubuntu, install GCC with `sudo apt install g++` or visit <https://gcc.gnu.org> for alternate means of installation.
- **Bazel** or **CMake:** Either the Bazel build system or the CMake build system must be installed on your system.
  - **Bazel:** Follow the instructions at <https://bazel.build/install> to install Bazel on your system.
  - **CMake:** On Ubuntu, install the CMake build system with `sudo apt install cmake` or visit <https://cmake.org> for alternate means of installation.

[(Back to Top)](#lector)

## Configuration

- [Bazel](#configuration-bazel)
- [CMake](#configuration-cmake)

[(Back to Top)](#lector)

### Configuration: Bazel

Clone the Lector library's repository with:

```bash
git clone git@github.com:acodcha/lector.git lector
cd lector
```

Build the Lector library from the base directory of the Lector repository with:

```bash
bazel build //...
```

Optionally run all Lector library tests from the base directory of the Lector repository with:

```bash
bazel test //...
```

[(Back to Configuration)](#configuration)

### Configuration: CMake

Clone the Lector library's repository with:

```bash
git clone git@github.com:acodcha/lector.git lector
cd lector
```

Build the Lector library from the base directory of the Lector repository with:

```bash
cmake -S . -B build
cmake --build build --parallel 4
```

Optionally run all Lector library tests from the base directory of the Lector repository with:

```bash
cmake -S . -B build -D LECTOR_TEST=ON
cmake --build build --parallel 4
ctest --test-dir build
```

[(Back to Configuration)](#configuration)

## Usage

- [Bazel](#usage-bazel)
- [CMake](#usage-cmake)

[(Back to Top)](#lector)

### Usage: Bazel

To use the Lector library in one of your Bazel C++ projects, first add the following code to your project's `MODULE.bazel` file:

```starlark
bazel_dep(name = "lector", version = "1.0.0")

git_override(
    module_name = "lector",
    remote = "[https://github.com/acodcha/lector.git](https://github.com/acodcha/lector.git)",
    branch = "main",  # Alternatively, you can use `tag = "v1.0.0"` or `commit = "HEAD"`.
)
```

Alternatively, if your project uses the legacy `WORKSPACE.bazel` system instead of the newer `MODULE.bazel` system, add the following to your project's `WORKSPACE.bazel` file:

```starlark
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "lector",
    remote = "[https://github.com/acodcha/lector.git](https://github.com/acodcha/lector.git)",
    branch = "main",  # Alternatively, you can use `tag = "v1.0.0"` or `commit = "HEAD"`.
)
```

The above code automatically downloads the Lector library from GitHub and makes it available to your Bazel project.

Next, link the Lector library to your C++ targets in your project's `BUILD.bazel` files:

```starlark
cc_library(
    name = "my_library_name",
    hdrs = ["my_file.hpp"],
    srcs = ["my_file.cpp"],
    deps = [
        "@lector//:lector",
    ],
)
```

Once configured, simply include the Lector library's header in your C++ source files with `#include <lector/lector.hpp>`. All of the library's contents are cleanly encapsulated within the `lector::` namespace.

[(Back to Usage)](#usage)

### Usage: CMake

To use the Lector library in one of your CMake C++ projects, add the following code to your project's `CMakeLists.txt` file:

```cmake
find_package(lector CONFIG QUIET)

if(lector_FOUND)
  message(STATUS "The Lector library was found at: ${lector_CONFIG}")
else()
  include(FetchContent)
  FetchContent_Declare(
    lector
    GIT_REPOSITORY [https://github.com/acodcha/lector.git](https://github.com/acodcha/lector.git)
    GIT_TAG main  # You can also specify a release tag such as: v1.0.0
  )
  FetchContent_MakeAvailable(lector)

  # Create an alias so the target name is consistent with find_package.
  add_library(lector::lector ALIAS lector)
  message(STATUS "The Lector library was fetched from: https://github.com/acodcha/lector")
endif()

# [...]

# Link the Lector library to your executable or library.
target_link_libraries(your_target_name PRIVATE lector::lector)
```

The above CMake code checks whether the Lector library is already installed on your system; if not, it automatically downloads it from its GitHub repository and makes it available to your CMake project.

Once configured, simply include the Lector library's header in your C++ source files with `#include <lector/lector.hpp>`. All of the library's contents are cleanly encapsulated within the `lector::` namespace.

[(Back to Usage)](#usage)

## License

Copyright © 2026, Alexandre Coderre-Chabot.

Lector is licensed under the MIT License (<https://mit-license.org>).

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[(Back to Top)](#lector)
