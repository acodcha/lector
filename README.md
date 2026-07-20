# Lector

[![Linux](https://github.com/acodcha/lector/actions/workflows/Linux.yaml/badge.svg?branch=main)](https://github.com/acodcha/lector/actions/workflows/Linux.yaml) [![macOS](https://github.com/acodcha/lector/actions/workflows/macOS.yaml/badge.svg?branch=main)](https://github.com/acodcha/lector/actions/workflows/macOS.yaml) [![Windows](https://github.com/acodcha/lector/actions/workflows/Windows.yaml/badge.svg?branch=main)](https://github.com/acodcha/lector/actions/workflows/Windows.yaml)

Lector is a C++ library for parsing command line arguments. Lector is supported on Linux, macOS, and Windows, and can be built with Bazel, CMake, or Meson. The Lector library's repository is hosted at <https://github.com/acodcha/lector> and its documentation is hosted at <https://acodcha.github.io/lector>.

Contents:

- [**§1. Introduction**](#1-introduction)
- [**§2. Configuration**](#2-configuration): [Bazel](#21-configuration-bazel), [CMake](#22-configuration-cmake), [Meson](#23-configuration-meson)
- [**§3. User Guide**](#3-user-guide): [Arguments](#31-user-guide-arguments), [Command Line](#32-user-guide-command-line), [Enumerations](#33-user-guide-enumerations), [Data Structures](#34-user-guide-data-structures), [Error Checking](#35-user-guide-error-checking)
- [**§4. Developer Guide**](#4-developer-guide): [Formatting](#41-developer-guide-formatting), [Building](#42-developer-guide-building), [Testing](#43-developer-guide-testing), [Linting](#44-developer-guide-linting), [Coverage](#45-developer-guide-coverage), [Documentation](#46-developer-guide-documentation)
- [**§5. License**](#5-license)

## §1. Introduction

The following example illustrates the use of the Lector library:

```cpp
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <lector/arguments.hpp>

enum class Label : std::int8_t {OutputDirectory, Iterations, Help};

int main(int argc, char* argv[]) {
  lector::Arguments arguments{
    lector::Configuration{
      "My Application",
      "Description of my application.",
      "Additional notes about my application."},
    lector::Argument<Label::OutputDirectory, std::filesystem::path>{
      {"-o", "--output_directory"}, "Output directory. Required."
    },
    lector::Argument<Label::Iterations, std::int32_t>{
      {"-i", "--iterations"}, "Number of iterations. Optional. Default 100.", 100
    },
    lector::Argument<Label::Help, bool>{
      {"-h", "--help"}, "Display this help information and exit. Optional."
    }
  };

  arguments.parse(argc, argv);

  if (arguments.get<Label::Help>().parsed_or_default_value()) {
    std::cout << arguments.help() << std::endl;
    return EXIT_SUCCESS;
  }

  std::cout << "Execution:" << std::endl << arguments.execution() << std::endl;

  const std::filesystem::path& output_directory{
    arguments.get<Label::OutputDirectory>().parsed_or_default_value()};

  std::cout << "The output directory is: " << output_directory << std::endl;

  const std::int32_t iterations{
    arguments.get<Label::Iterations>().parsed_or_default_value()};

  std::cout << "The number of iterations is: " << iterations << std::endl;

  return EXIT_SUCCESS;
}
```

The above example imports the Lector library, defines an enumeration of argument labels, creates a collection of arguments, parses the arguments from the command line `argc` and `argv` variables, checks whether usage information should be printed, prints the execution information, and obtains and prints the parsed arguments.

In the Lector library, command line arguments are strongly-typed, support arbitrary types, can be declared as required or optional, and feature strict error checking.

The Lector library can parse command line arguments as whitespace-separated key-value pairs of the form `key value` or as inline key-value pairs of the form `key=value`. It can also handle keys that contain arbitrary characters.

[(Back to Top)](#lector)

## §2. Configuration

This section describes how to use the Lector library in one of your C++ projects.

First, ensure that the [Git](<https://git-scm.com>) source control system is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install Git with `sudo apt install git`.
- On macOS, install Git using the [Homebrew](<https://brew.sh>) package manager with `brew install git`.
- On Windows, install Git using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install git`.
- On other systems, visit <https://git-scm.com> for alternate means of installation.

Second, ensure that a C++ compiler with support for the C++17 standard or any more recent standard is installed on your system:

- On Ubuntu or other Debian-based Linux systems, the [GNU GCC](<https://gcc.gnu.org>) and [LLVM Clang](<https://llvm.org>) C++ compilers are most commonly used. Install the GNU GCC C++ compiler with `sudo apt install g++` or the LLVM library that contains the Clang C++ compiler with `sudo apt install llvm`.
- On macOS, the [LLVM Clang](<https://llvm.org>) C++ compiler is most commonly used. Install the LLVM library that contains the Clang C++ compiler using the [Homebrew](<https://brew.sh>) package manager with `brew install llvm`.
- On Windows, the [MSVC](<https://learn.microsoft.com/cpp/windows>) C++ compiler is most commonly used. Visit <https://learn.microsoft.com/cpp/windows> to install the MSVC C++ compiler.
- On other systems, visit <https://gcc.gnu.org> or <https://llvm.org> for alternate means of installation for the GNU GCC and LLVM Clang C++ compilers, respectively.

Third, ensure that your C++ project uses a supported build system; the Lector library currently supports the Bazel, CMake, and Meson build systems. Refer to the section for your preferred build system:

- [§2.1. Bazel](#21-configuration-bazel)
- [§2.2. CMake](#22-configuration-cmake)
- [§2.3. Meson](#23-configuration-meson)

Finally, once your project's build system has been installed and configured, simply include the Lector library's headers in your project's C++ source files with:

```cpp
#include <lector/arguments.hpp>
#include <lector/parse.hpp>
#include <lector/print.hpp>
#include <lector/text.hpp>
```

The Lector library is modular:

- The file `<lector/arguments.hpp>` defines the `lector::Argument` and `lector::Arguments` classes and the `lector::Configuration` data structure, as demonstrated in the [§1. Introduction](#1-introduction) section. See the [§3.1. User Guide: Arguments](#31-user-guide-arguments) section for usage.
- The file `<lector/parse.hpp>` defines the `lector::parse()` utility function. See the [§3.3 User Guide: Enumerations](#33-user-guide-enumerations) section for usage.
- The file `<lector/print.hpp>` defines the `lector::print()` utility function. See the [§3.3 User Guide: Enumerations](#33-user-guide-enumerations) section for usage.
- The file `<lector/text.hpp>` defines general-purpose utilities for manipulating strings of text; these utilities are used by the `lector::Arguments` class.

All of the Lector library's contents are cleanly encapsulated within the `lector::` namespace.

[(Back to Top)](#lector)

### §2.1. Configuration: Bazel

To use the Lector library in one of your [Bazel](<https://bazel.build/install>) C++ projects, first ensure that the Bazel build system is installed on your system. Follow the instructions at <https://bazel.build/install> to install Bazel on your system.

Then, add the following code to your project's `MODULE.bazel` file:

```starlark
bazel_dep(name = "lector", version = "1.0.0")

git_override(
    module_name = "lector",
    remote = "https://github.com/acodcha/lector.git",
    branch = "main",
)
```

Note: You can specify a release tag such as `tag = "v1.0.0"` instead of `branch = "main"`.

Alternatively, if your project uses the legacy `WORKSPACE.bazel` system instead of the newer `MODULE.bazel` system, add the following code to your project's `WORKSPACE.bazel` file:

```starlark
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "lector",
    remote = "https://github.com/acodcha/lector.git",
    branch = "main",
)
```

Note: You can specify a release tag such as `tag = "v1.0.0"` instead of `branch = "main"`.

The above code automatically downloads the Lector library from its GitHub repository and makes it available to your Bazel project.

Next, link the Lector library to the C++ targets in your project's `BUILD.bazel` files. Because the Lector library is modular, you only need to depend on the specific parts of the library you actually use:

```starlark
cc_library(
    name = "my_library_name",
    hdrs = ["my_library_file.hpp"],
    srcs = ["my_library_file.cpp"],
    deps = [
        "@lector//lector:arguments",
        "@lector//lector:parse",
        "@lector//lector:print",
        ":my_other_dependency",
    ],
)
```

The above Bazel code defines your C++ Bazel library to depend on the Lector library modules.

[(Back to Configuration)](#2-configuration)

### §2.2. Configuration: CMake

To use the Lector library in one of your [CMake](<https://cmake.org>) C++ projects, first ensure that the CMake build system is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install CMake with `sudo apt install cmake`.
- On macOS, install CMake using the [Homebrew](<https://brew.sh>) package manager with `brew install cmake`.
- On Windows, install CMake using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install cmake`.
- On other systems, visit <https://cmake.org> for alternate means of installation.

Then, add the following code to your project's `CMakeLists.txt` file:

```cmake
find_package(lector CONFIG QUIET)

if(lector_FOUND)
    message(STATUS "The Lector library was found at: ${lector_CONFIG}")
else()
    include(FetchContent)
    FetchContent_Declare(
        lector
        GIT_REPOSITORY https://github.com/acodcha/lector.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(lector)
    add_library(lector::lector ALIAS lector)
    message(STATUS "The Lector library was fetched from: https://github.com/acodcha/lector")
endif()

# [...]

target_link_libraries(my_target_name PRIVATE lector::lector my_other_dependency)
```

Note: You can specify a release tag such as `GIT_TAG v1.0.0` instead of `GIT_TAG main`.

The above CMake code checks whether the Lector library is already installed on your system; if not, it automatically downloads it from its GitHub repository and makes it available to your CMake project.

[(Back to Configuration)](#2-configuration)

### §2.3. Configuration: Meson

To use the Lector library in one of your [Meson](<https://mesonbuild.com>) C++ projects, first ensure that the Meson build system is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install Meson with `sudo apt install meson ninja-build`.
- On macOS, install Meson using the [Homebrew](<https://brew.sh>) package manager with `brew install meson ninja`.
- On Windows, install Meson using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install meson ninja`.
- On other systems, visit <https://mesonbuild.com> for alternate means of installation.

Then, create a `subprojects/lector.wrap` file in your project and give it the following contents:

```text
[wrap-git]
url = https://github.com/acodcha/lector.git
revision = main
```

Note: You can specify a release tag such as `revision = v1.0.0` instead of `revision = main`.

The above code automatically downloads the Lector library from its GitHub repository and makes it available to your Meson project.

Next, add the following code to your project's `meson.build` files:

```text
lector = dependency(
    'lector',
    fallback : ['lector', 'lector'],
    required : true
)

my_library_name = library(
    'my_library_name',
    'src/my_library_file.cpp',
    include_directories : include_directories('src'),
    dependencies : [lector, my_other_dependency],
    install : true
)
```

The above Meson code imports the Lector library into your project, defines your library, and adds the Lector library as a dependency to your library.

[(Back to Configuration)](#2-configuration)

## §3. User Guide

This section presents a comprehensive guide for using the Lector library in your C++ projects. Refer to the following sections:

- [§3.1. Arguments](#31-user-guide-arguments)
- [§3.2. Command Line](#32-user-guide-command-line)
- [§3.3. Enumerations](#33-user-guide-enumerations)
- [§3.4. Data Structures](#34-user-guide-data-structures)
- [§3.5. Error Checking](#35-user-guide-error-checking)

[(Back to Top)](#lector)

### §3.1. User Guide: Arguments

The Lector library uses enumeration values to label arguments. For example, the code from the [§1. Introduction](#1-introduction) section defines the following enumeration:

```cpp
enum class Label : std::int8_t {OutputDirectory, Iterations, Help};
```

Defining an argument requires an enumeration value used as a label, a type, one or more keys, a description, and an optional default value. For example, the code from the [§1. Introduction](#1-introduction) section defines the following arguments:

```cpp
lector::Arguments arguments{
  lector::Configuration{
    "My Application",
    "Description of my application.",
    "Additional notes about my application."},
  lector::Argument<Label::OutputDirectory, std::filesystem::path>{
    {"-o", "--output_directory"}, "Output directory. Required."
  },
  lector::Argument<Label::Iterations, std::int32_t>{
    {"-i", "--iterations"}, "Number of iterations. Optional. Default 100.", 100
  },
  lector::Argument<Label::Help, bool>{
    {"-h", "--help"}, "Display this help information and exit. Optional."
  }
};
```

Note: The `lector::Configuration` data structure is not a command line argument; it is an optional data structure that lists an optional title, description, and miscellaneous notes to be included when printing the help information of collection of command line arguments.

Supported argument types include:

- Boolean flags: `bool`.
- Natural numbers: `std::uint8_t`, `std::uint16_t`, `std::uint32_t`, `std::uint64_t`.
- Integer numbers: `std::int8_t`, `std::int16_t`, `std::int32_t`, `std::int64_t`.
- Floating-point numbers: `float`, `double`, `long double`.
- Strings: `std::string`. If the string contains whitespace, enclose it in double quotes (`""`).
- Paths: `std::filesystem::path`.
- Enumerations: See the [§3.3. User Guide: Enumerations](#33-user-guide-enumerations) section.
- Data Structures: See the [§3.4. User Guide: Data Structures](#34-user-guide-data-structures) section.

Once all arguments have been defined, the `lector::Arguments::parse()` method can be used to parse `argc` and `argv`. For example:

```cpp
arguments.parse(argc, argv);
```

This populates all arguments with their parsed values and performs strict error checking. See the [§3.5. Error Checking](#35-user-guide-error-checking) section for details.

Help information can be obtained via the `lector::Arguments::help()` method. For example:

```cpp
std::cout << arguments.help() << std::endl;
```

Execution information can be obtained via the  `lector::Arguments::execution()` method. For example:

```cpp
std::cout << "Execution:" << std::endl << arguments.execution() << std::endl;
```

See the [§3.2. Command Line](#32-user-guide-command-line) section for details.

Individual arguments can be fetched via the `lector::Arguments::get()` method using the argument's enumeration value as a template. For example:

```cpp
const std::filesystem::path& output_directory{
  arguments.get<Label::OutputDirectory>().parsed_or_default_value()};

const std::int32_t iterations{
  arguments.get<Label::Iterations>().parsed_or_default_value()};
```

[(Back to User Guide)](#3-user-guide)

### §3.2. User Guide: Command Line

The Lector library allows you to flexibly run your program from the command line and to conveniently display the usage, options, and execution information of your program. The following examples use the code from the [§1. Introduction](#1-introduction) section.

Display help information via the `lector::Arguments::help()` method:

```text
path/to/my_project_main --help

My Application

Usage:
my_project_main --output_directory <path> [--iterations <number>] [--help]

Description of my application.

Options:
-o <path>, --output_directory <path>  Output directory. Required.
-i <number>, --iterations <number>    Number of iterations. Optional. Default
                                      100.
-h, --help                            Display this help information and exit.
                                      Optional.

Additional notes about my application.
```

Display execution information via the  `lector::Arguments::execution()` method:

```text
path/to/my_project_main --output_directory /some/path --iterations 200

Execution:
path/to/my_project_main --output_directory /some/path --iterations 200
The output directory is: /some/path
The number of iterations is: 200
```

Inline key-value pairs of the form `key=value` are also supported:

```text
path/to/my_project_main --output_directory=/some/path --iterations=200

Execution:
path/to/my_project_main --output_directory /some/path --iterations 200
The output directory is: /some/path
The number of iterations is: 200
```

Arguments can be defined with multiple keys. For example:

```text
path/to/my_project_main -o /some/path -i=200

Execution:
path/to/my_project_main --output_directory /some/path --iterations 200
The output directory is: /some/path
The number of iterations is: 200
```

Keys do not need to start with a hyphen (`-`); keys can be composed of any characters, including equal signs (`=`). For example, the following definitions are unusual but perfectly valid:

```cpp
lector::Arguments arguments{
  lector::Argument<Label::OutputDirectory, std::filesystem::path>{
    {"o", "=o", "__out_dir__"}, "Output directory. Required."
  },
  lector::Argument<Label::Iterations, std::int32_t>{
    {"=i=", "_it_", "==iterations=="}, "Number of iterations. Optional. Default 100.", 100
  }
};
```

```text
path/to/my_project_main __out_dir__ /some/path =i= 200

Execution:
path/to/my_project_main __out_dir__ /some/path ==iterations== 200
The output directory is: /some/path
The number of iterations is: 200
```

[(Back to User Guide)](#3-user-guide)

### §3.3. User Guide: Enumerations

Enumerations can be used as argument types, but require specializing the `lector::Names` and `lector::Spellings` constants for their values. For example:

```cpp
#ifndef MY_PROJECT_SHAPE_HPP
#define MY_PROJECT_SHAPE_HPP

#include <array>
#include <cstdint>
#include <lector/parse.hpp>
#include <lector/print.hpp>

namespace my_project {

enum class Shape : std::int8_t {Circle, Triangle, Square};

}  // namespace my_project

namespace lector {

template <>
inline constexpr std::array<Name<my_project::Shape>, 3> Names<my_project::Shape>{
  {
    {my_project::Shape::Circle,   "Circle"},
    {my_project::Shape::Triangle, "Triangle"},
    {my_project::Shape::Square,   "Square"},
  }
};

template <>
inline constexpr std::array<Spelling<my_project::Shape>, 9> Spellings<my_project::Shape>{
  {
    {"Circle",   my_project::Shape::Circle},
    {"Triangle", my_project::Shape::Triangle},
    {"Square",   my_project::Shape::Square},
    {"circle",   my_project::Shape::Circle},
    {"triangle", my_project::Shape::Triangle},
    {"square",   my_project::Shape::Square},
    {"CIRCLE",   my_project::Shape::Circle},
    {"TRIANGLE", my_project::Shape::Triangle},
    {"SQUARE",   my_project::Shape::Square},
  }
};

}  // namespace lector

#endif  // MY_PROJECT_SHAPE_HPP
```

With the above definitions, the `lector::print()` and `lector::parse()` methods can now be used with the `my_project::Shape` enumeration. For example:

```cpp
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <lector/parse.hpp>
#include <lector/print.hpp>
#include "my_project/shape.hpp"

int main() {
  const std::string printed_triangle{lector::print(my_project::Shape::Triangle)};
  assert(printed_triangle == "Triangle");

  const std::optional<my_project::Shape> parsed_triangle{
    lector::parse<my_project::Shape>("TRIANGLE")};
  assert(parsed_triangle.has_value() &&
         parsed_triangle.value() == my_project::Shape::Triangle);

  const std::optional<my_project::Shape> invalid_shape{
    lector::parse<my_project::Shape>("Invalid Shape")};
  assert(!invalid_shape.has_value());

  return EXIT_SUCCESS;
}
```

The `my_project::Shape` enumeration can also be used as a command line argument. For example:

```cpp
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <lector/arguments.hpp>
#include <lector/print.hpp>
#include "my_project/shape.hpp"

enum class Label : std::int8_t {FavoriteShape};

int main(int argc, char* argv[]) {
  lector::Arguments arguments{
    lector::Argument<Label::FavoriteShape, my_project::Shape>{
      {"-s", "--shape"}, "Your favorite shape. Optional.", my_project::Shape::Circle
    }
  };

  arguments.parse(argc, argv);

  const my_project::Shape shape{
    arguments.get<Label::FavoriteShape>().parsed_or_default_value()};

  std::cout << "Your favorite shape is: " << lector::print(shape) << std::endl;

  return EXIT_SUCCESS;
}
```

[(Back to User Guide)](#3-user-guide)

### §3.4. User Guide: Data Structures

Data structures can be used as argument types, but require specializing the input and output stream operators (`<<` and `>>`). For example:

```cpp
#ifndef MY_PROJECT_POINT_HPP
#define MY_PROJECT_POINT_HPP

#include <iostream>

namespace my_project {

struct Point {
  double x{0.0};
  double y{0.0};
  double z{0.0};
};

inline std::istream& operator>>(std::istream& input_stream, Point& point) {
  input_stream >> point.x >> point.y >> point.z;
  return input_stream;
}

inline std::ostream& operator<<(std::ostream& output_stream, const Point& point) {
  output_stream << point.x << " " << point.y << " " << point.z;
  return output_stream;
}

}  // namespace my_project

#endif  // MY_PROJECT_POINT_HPP
```

With the above definitions, the `lector::print()` and `lector::parse()` methods can now be used with the `my_project::Point` data structure. For example:

```cpp
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <lector/parse.hpp>
#include <lector/print.hpp>
#include "my_project/point.hpp"

int main() {
  const std::string printed_point{lector::print(my_project::Point{1.0, 2.0, 3.0})};
  assert(printed_point == "1 2 3");

  const std::optional<my_project::Point> parsed_point{
    lector::parse<my_project::Point>("4.0 5.0 6.0")};
  const my_project::Point expected_point{4.0, 5.0, 6.0};
  assert(parsed_point.has_value() &&
         parsed_point.value().x == expected_point.x &&
         parsed_point.value().y == expected_point.y &&
         parsed_point.value().z == expected_point.z);

  return EXIT_SUCCESS;
}
```

The `my_project::Point` data structure can also be used as a command line argument. For example:

```cpp
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <lector/arguments.hpp>
#include "my_project/point.hpp"

enum class Label : std::int8_t {FavoritePoint};

int main(int argc, char* argv[]) {
  lector::Arguments arguments{
    lector::Argument<Label::FavoritePoint, my_project::Point>{
      {"-p", "--point"}, "Your favorite point. Optional.", my_project::Point{}
    }
  };

  arguments.parse(argc, argv);

  const my_project::Point point{
    arguments.get<Label::FavoritePoint>().parsed_or_default_value()};

  std::cout << "Your favorite point is: " << point << std::endl;

  return EXIT_SUCCESS;
}
```

[(Back to User Guide)](#3-user-guide)

### §3.5. User Guide: Error Checking

The Lector library performs strict error checking when defining command line arguments and again when parsing these arguments from the command line.

The following checks are performed when defining command line arguments:

- All arguments must have unique labels. For example, a `lector::Arguments` constructed from `lector::Argument<Label::Help, bool>{ {"-h"}, "Help." }` and `lector::Argument<Label::Help, bool>{ {"--help"}, "Help." }` throws an exception because both arguments use the same label `Label::Help`.
- All arguments must each have at least one key. For example, `lector::Argument<Label::Iterations, std::int32_t>{ {}, "Iterations." }` throws an exception because no keys are defined.
- Arguments cannot have empty keys. For example, `lector::Argument<Label::Iterations, std::int32_t>{ {"", "-i"}, "Iterations." }` throws an exception because the first key is empty.
- Arguments cannot have duplicate keys. For example, `lector::Argument<Label::Iterations, std::int32_t>{ {"-i", "-i"}, "Iterations." }` throws an exception because the key `-i` is duplicated.
- Keys cannot be duplicated across arguments. For example, a `lector::Arguments` constructed from `lector::Argument<Label::Iterations, std::int32_t>{ {"-i"}, "Iterations." }` and `lector::Argument<Label::Help, bool>{ {"-i"}, "Help." }` throws an exception because the two arguments use the same key `-i`.
- All arguments must have descriptions. For example, `lector::Argument<Label::Iterations, std::int32_t>{ {"-i"}, "" }` throws an exception because the description is empty.
- Boolean arguments are always false by default and cannot specify default values. For example, `lector::Argument<Label::Help, bool>{ {"-h"}, "Help.", true }` throws an exception because `true` was specified as a default value.

The following checks are performed when parsing command line arguments. These examples use the code from the [§1. Introduction](#1-introduction) section:

- Missing required arguments. For example, `path/to/my_project_main --iterations 200` throws an exception because the required argument `--output_directory <path>` is missing.
- Duplicated arguments. For example, `path/to/my_project_main --output_directory /tmp --output_directory /home` throws an exception because the argument `--output_directory <path>` is duplicated.
- Invalid argument values. For example, `path/to/my_project_main --output_directory /tmp --iterations hello` throws an exception because `hello` is not a valid value for the argument `--iterations <number>`.
- Arguments missing values. For example, `path/to/my_project_main --output_directory /tmp --iterations` throws an exception because the argument `--iterations <number>` is missing its value.
- Unknown arguments. For example, `path/to/my_project_main --output_directory /tmp --unknown` throws an exception because the argument `--unknown` is unknown.

[(Back to User Guide)](#3-user-guide)

## §4. Developer Guide

To experiment with the Lector library for yourself, first clone the Lector library's repository:

```bash
git clone git@github.com:acodcha/lector.git lector
cd lector
```

Then, refer to the following sections:

- [§4.1. Formatting](#41-developer-guide-formatting)
- [§4.2. Building](#42-developer-guide-building)
- [§4.3. Testing](#43-developer-guide-testing)
- [§4.4. Linting](#44-developer-guide-linting)
- [§4.5. Coverage](#45-developer-guide-coverage)
- [§4.6. Documentation](#46-developer-guide-documentation)

[(Back to Top)](#lector)

### §4.1. Developer Guide: Formatting

The Lector library uses the [ClangFormat](<https://clang.llvm.org/docs/ClangFormat.html>) automatic source code formatter by LLVM to standardize the formatting of its source code files. See the [clang-format](<https://github.com/acodcha/lector/blob/main/.clang-format>) file for the formatting options that are used.

To format the source code files of the Lector library, ensure that ClangFormat is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install ClangFormat with `sudo apt install clang-format`.
- On macOS, install ClangFormat using the [Homebrew](<https://brew.sh>) package manager with `brew install clang-format`.
- On Windows, install the LLVM library that contains ClangFormat using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install llvm`.
- On other systems, visit <https://clang.llvm.org/docs/ClangFormat.html> for alternate means of installation.

Format the Lector library's source code files with:

```bash
find . -iname '*.hpp' -or -iname '*.cpp' | xargs clang-format --style=file -i
```

All source code files must be correctly formatted before being submitted to the Lector library's repository.

[(Back to Developer Guide)](#4-developer-guide)

### §4.2. Developer Guide: Building

If using the Bazel build system, build the Lector library with:

```bash
bazel build //...
```

If using the CMake build system, build the Lector library with:

```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

If using the Meson build system, build the Lector library with:

```bash
meson wrap install gtest
meson setup build
meson compile -C build
```

[(Back to Developer Guide)](#4-developer-guide)

### §4.3. Developer Guide: Testing

If using the Bazel build system, run all of the Lector library's tests with:

```bash
bazel test //... --test_output=all
```

If using the CMake build system, run all of the Lector library's tests with:

```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release -D LECTOR_TEST=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

If using the Meson build system, run all of the Lector library's tests with:

```bash
meson wrap install gtest
meson setup build
meson compile -C build
meson test -C build
```

[(Back to Developer Guide)](#4-developer-guide)

### §4.4. Developer Guide: Linting

The Lector library uses the [ClangTidy](<https://clang.llvm.org/extra/clang-tidy>) automatic source code linter by LLVM to lint its source code files to identify and diagnose common programming issues. See the [clang-tidy](<https://github.com/acodcha/lector/blob/main/.clang-tidy>) file for the linting options that are used.

To lint the source code files of the Lector library, ensure that ClangTidy is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install ClangTidy with `sudo apt install clang-tidy`.
- On macOS, install the LLVM library that contains ClangTidy using the [Homebrew](<https://brew.sh>) package manager with `brew install llvm`.
- On Windows, install the LLVM library that contains ClangTidy using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install llvm`.
- On other systems, visit <https://clang.llvm.org/extra/clang-tidy> for alternate means of installation.

Lint the Lector library's source code files with:

```bash
find . -iname '*.cpp' -print0 | xargs -0 -I {} clang-tidy --extra-arg=-std=c++17 {} -- -I.
```

[(Back to Developer Guide)](#4-developer-guide)

### §4.5. Developer Guide: Coverage

The Lector library currently has 100% code coverage. To compute the Lector library's code coverage yourself, ensure that the [LCOV](<https://github.com/linux-test-project/lcov>) utility is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install LCOV with `sudo apt install lcov`.
- On macOS, install LCOV using the [Homebrew](<https://brew.sh>) package manager with `brew install lcov`.
- On Windows, install LCOV using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install lcov`.
- On other systems, visit <https://github.com/linux-test-project/lcov> for alternate means of installation.

If using the Bazel build system on Linux or macOS, compute the Lector library's code coverage with:

```bash
bazel coverage //...
genhtml bazel-out/_coverage/_coverage_report.dat --output-directory coverage
```

If using the Bazel build system on Windows, to compute the Lector library's code coverage, the [MSVC](<https://learn.microsoft.com/cpp/windows>) C++ compiler will not work, and the [LLVM Clang](<https://llvm.org>) C++ compiler must be used instead. Ensure that the LLVM library that contains the Clang C++ compiler is installed on your system; you can install it using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install llvm`. Then, compute the Lector library's code coverage using the Bazel build system with:

```bash
bazel coverage --compiler=clang-cl //...
genhtml bazel-out/_coverage/_coverage_report.dat --output-directory coverage
```

If using the CMake build system on Linux or macOS, compute the Lector library's code coverage with:

```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug -D LECTOR_TEST=ON -D LECTOR_COVERAGE=ON
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
cmake --build build --target coverage
```

If using the CMake build system on Windows, to compute the Lector library's code coverage, ensure that the [OpenCppCoverage](<https://github.com/OpenCppCoverage/OpenCppCoverage>) utility is installed on your system. Download it from <https://github.com/OpenCppCoverage/OpenCppCoverage>. Then, compute the Lector library's code coverage with:

```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug -D LECTOR_TEST=ON -D LECTOR_COVERAGE=ON
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
cmake --build build --target coverage
```

If using the Meson build system on Linux or macOS, compute the Lector library's code coverage with:

```bash
meson wrap install gtest
meson setup build -Db_coverage=true -Db_coverage_dir=../coverage
meson compile -C build
meson test -C build
meson compile -C build coverage-html
```

If using the Meson build system on Windows, to compute the Lector library's code coverage, ensure that the [OpenCppCoverage](<https://github.com/OpenCppCoverage/OpenCppCoverage>) utility is installed on your system. Download it from <https://github.com/OpenCppCoverage/OpenCppCoverage>. Then, compute the Lector library's code coverage with:

```bash
meson wrap install gtest
meson setup build
meson compile -C build
meson compile -C build coverage
```

Regardless of the build system or operating system, in all cases, a code coverage report is generated at `coverage/index.html`. Open this file in any web browser to view the report.

[(Back to Developer Guide)](#4-developer-guide)

### §4.6. Developer Guide: Documentation

The Lector library's documentation is hosted at <https://acodcha.github.io/lector>. The Lector library uses the [Doxygen](<https://www.doxygen.org>) automatic source code documentation generator to generate the documentation of its source code files. See the [Doxyfile.txt](<https://github.com/acodcha/lector/blob/main/Doxyfile.txt>) file for the documentation options that are used. To generate the Lector library's documentation yourself, ensure that Doxygen is installed on your system:

- On Ubuntu or other Debian-based Linux systems, install Doxygen with `sudo apt install doxygen`.
- On macOS, install Doxygen using the [Homebrew](<https://brew.sh>) package manager with `brew install doxygen`.
- On Windows, install Doxygen using the [Chocolatey](<https://chocolatey.org>) package manager with `choco install doxygen.install`.
- On other systems, visit <https://www.doxygen.org> for alternate means of installation.

Generate the Lector library's documentation with:

```bash
doxygen Doxyfile.txt
```

This builds HTML documentation pages in the `docs/` directory. Browse the documentation by opening the `docs/index.html` file in any web browser.

[(Back to Developer Guide)](#4-developer-guide)

## §5. License

Copyright © 2026, Alexandre Coderre-Chabot.

Lector is licensed under the MIT License. See the [LICENSE.md](<https://github.com/acodcha/lector/blob/main/LICENSE.md>) file or <https://mit-license.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[(Back to Top)](#lector)
