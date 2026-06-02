# Lector

[![tests](https://github.com/acodcha/lector/actions/workflows/tests.yaml/badge.svg?branch=main)](https://github.com/acodcha/lector/actions/workflows/tests.yaml)

Lector is a C++ library for parsing command line arguments. Lector is hosted at <https://github.com/acodcha/lector>.

- [**Requirements**](#requirements)
- [**Configuration**](#configuration): [Bazel](#configuration-bazel), [CMake](#configuration-cmake)
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
cmake -B build -S .
cmake --build
```

Optionally run all Lector library tests from the base directory of the Lector repository with:

```bash
cmake -B build -S . -D LECTOR_TEST=ON
cmake --build .
ctest
```

[(Back to Configuration)](#configuration)

## License

Copyright © 2026, Alexandre Coderre-Chabot.

Lector is licensed under the MIT License (<https://mit-license.org>).

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[(Back to Top)](#lector)
