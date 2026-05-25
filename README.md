# Args

Args is a C++ library for parsing command line arguments. Args is hosted at <https://github.com/acodcha/args>.

- [**Requirements**](#requirements)
- [**Configuration**](#configuration)
- [**License**](#license)

## Requirements

Args requires the following software to be installed on your system:

- **Git:** The Git source control system must be installed on your system. On Ubuntu, install Git with `sudo apt install git` or visit <https://git-scm.com> for alternate means of installation.
- **C++:** A C++ compiler with support for the C++17 standard or any more recent standard must be installed on your system. Any reasonably recent compiler for the C++ programming language should do, such as GCC. On Ubuntu, install GCC with `sudo apt install g++` or visit <https://gcc.gnu.org> for alternate means of installation.
- **Bazel:** The Bazel build system must be installed on your system. Follow the instructions at <https://bazel.build/install> to install Bazel on your system.

[(Back to Top)](#args)

## Configuration

Clone the Args library's repository with:

```bash
git clone git@github.com:acodcha/args.git args
cd args
```

Build the Args library from the base directory of the Args repository with:

```bash
bazel build //...
```

Optionally run all Args library tests from the base directory of the Args repository with:

```bash
bazel test //...
```

[(Back to Top)](#args)

## License

Copyright © 2026, Alexandre Coderre-Chabot.

Args is licensed under the MIT License (<https://mit-license.org>).

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[(Back to Top)](#args)
