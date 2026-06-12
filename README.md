# Lector

[![tests](https://github.com/acodcha/lector/actions/workflows/tests.yaml/badge.svg?branch=main)](https://github.com/acodcha/lector/actions/workflows/tests.yaml)

Lector is a C++ library for parsing command line arguments. Lector is hosted at <https://github.com/acodcha/lector>.

Contents:

- [**Introduction**](#1-introduction)
- [**Configuration**](#2-configuration): [Bazel](#21-configuration-bazel), [CMake](#22-configuration-cmake), [Meson](#23-configuration-meson)
- [**User Guide**](#3-user-guide): [Arguments](#31-user-guide-arguments), [Command Line](#32-user-guide-command-line), [Enumerations](#33-user-guide-enumerations), [Classes and Structures](#34-user-guide-classes-and-structures), [Error Checking](#35-user-guide-error-checking)
- [**Developer Guide**](#4-developer-guide): [Bazel](#41-developer-guide-bazel), [CMake](#42-developer-guide-cmake), [Meson](#43-developer-guide-meson)
- [**License**](#5-license)

## 1. Introduction

The following example illustrates the use of the Lector library:

```cpp
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <lector/arguments.hpp>
#include <my_project/my_main_function.hpp>

enum class Label : std::int8_t {OutputDirectory, Iterations, Help};

int main(int argc, char* argv[]) {
  lector::Arguments arguments{
    lector::Argument<Label::OutputDirectory, std::filesystem::path>{
      {"-o", "--output_directory"}, "Output directory. Required."
    },
    lector::Argument<Label::Iterations, std::int32_t>{
      {"-i", "--iterations"}, "Number of iterations. Optional. Default: 100.", 100
    },
    lector::Argument<Label::Help, bool>{
      {"-h", "--help"}, "Display usage information and exit. Optional."
    }
  };

  arguments.parse(argc, argv);

  if (arguments.get<Label::Help>().parsed_or_default_value()) {
    std::cout << "Usage: " << arguments.print_usage_command() << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << arguments.print_usage_options() << std::endl;
    return EXIT_SUCCESS;
  }

  std::cout << "Start." << std::endl;
  std::cout << "Execution: " << arguments.print_execution() << std::endl;

  const std::filesystem::path& output_directory{
    arguments.get<Label::OutputDirectory>().parsed_value().value()};

  const std::int32_t iterations{
    arguments.get<Label::Iterations>().parsed_or_default_value()};

  std::cout << "Running my project's main function..." << std::endl;
  my_project::my_main_function(output_directory, iterations);

  std::cout << "End." << std::endl;
  return EXIT_SUCCESS;
}
```

The above example imports the Lector library, defines an enumeration of argument labels, creates a collection of arguments, parses the arguments from the command line `argc` and `argv` variables, checks whether usage information should be printed, prints the execution information, and passes the parsed arguments to `my_project::my_main_function()`.

In the Lector library, command line arguments are strongly-typed, support arbitrary types, can be declared as either required or optional, and feature strict error checking.

The Lector library can parse command line arguments as whitespace-separated key-value pairs of the form `key value` and as inline key-value pairs of the form `key=value`; it can even handle keys that contain arbitrary characters, including equal signs (`=`)!

[(Back to Top)](#lector)

## 2. Configuration

This section describes how to use the Lector library in one of your C++ projects.

First, make sure that the Git source control system is installed on your system. For example, on Ubuntu, install Git with `sudo apt install git`, or visit <https://git-scm.com> for alternate means of installation.

Second, make sure that a C++ compiler with support for the C++17 standard or any more recent standard is installed on your system. For example, on Ubuntu, install the GCC C++ compiler with `sudo apt install g++`, or visit <https://gcc.gnu.org> for alternate means of installation.

Third, make sure that your C++ project uses a supported build system; the Lector library currently supports the Bazel, CMake, and Meson build systems. Refer to the section for your preferred build system:

- [Bazel](#21-configuration-bazel)
- [CMake](#22-configuration-cmake)
- [Meson](#23-configuration-meson)

[(Back to Top)](#lector)

### 2.1. Configuration: Bazel

To use the Lector library in one of your Bazel C++ projects, first make sure that the Bazel build system is installed on your system. Follow the instructions at <https://bazel.build/install> to install Bazel on your system.

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

Finally, simply include the Lector library's headers in your C++ source files with:

```cpp
#include <lector/arguments.hpp>
#include <lector/parse.hpp>
#include <lector/print.hpp>
```

All of the Lector library's contents are cleanly encapsulated within the `lector::` namespace.

[(Back to Configuration)](#2-configuration)

### 2.2. Configuration: CMake

To use the Lector library in one of your CMake C++ projects, first make sure that the CMake build system is installed on your system. For example, on Ubuntu, install CMake with `sudo apt install cmake`, or visit <https://cmake.org> for alternate means of installation.

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

Finally, simply include the Lector library's headers in your C++ source files with:

```cpp
#include <lector/arguments.hpp>
#include <lector/parse.hpp>
#include <lector/print.hpp>
```

All of the Lector library's contents are cleanly encapsulated within the `lector::` namespace.

[(Back to Configuration)](#2-configuration)

### 2.3. Configuration: Meson

To use the Lector library in one of your Meson C++ projects, first make sure that the Meson build system is installed on your system. For example, on Ubuntu, install Meson with `sudo apt install meson ninja-build`, or visit <https://mesonbuild.com> for alternate means of installation.

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

Finally, simply include the Lector library's headers in your C++ source files with:

```cpp
#include <lector/arguments.hpp>
#include <lector/parse.hpp>
#include <lector/print.hpp>
```

All of the Lector library's contents are cleanly encapsulated within the `lector::` namespace.

[(Back to Configuration)](#2-configuration)

## 3. User Guide

This section contains a comprehensive guide for using the Lector library in your C++ projects.

- [Arguments](#31-user-guide-arguments)
- [Command Line](#32-user-guide-command-line)
- [Enumerations](#33-user-guide-enumerations)
- [Classes and Structures](#34-user-guide-classes-and-structures)
- [Error Checking](#35-user-guide-error-checking)

[(Back to Top)](#lector)

### 3.1. User Guide: Arguments

The Lector library uses enumeration values to label arguments. For example, the code from the [**Introduction**](#1-introduction) section defines the following enumeration:

```cpp
enum class Label : std::int8_t {OutputDirectory, Iterations, Help};
```

Defining an argument requires an enumeration value used as a label, a type, one or more keys, a description, and an optional default value. For example, the code from the [**Introduction**](#1-introduction) section defines the following arguments:

```cpp
lector::Arguments arguments{
  lector::Argument<Label::OutputDirectory, std::filesystem::path>{
    {"-o", "--output_directory"}, "Output directory. Required."
  },
  lector::Argument<Label::Iterations, std::int32_t>{
    {"-i", "--iterations"}, "Number of iterations. Optional. Default: 100.", 100
  },
  lector::Argument<Label::Help, bool>{
    {"-h", "--help"}, "Display usage information and exit. Optional."
  }
};
```

Supported argument types include:

- Boolean flags: `bool`.
- Natural numbers: `std::uint8_t`, `std::uint16_t`, `std::uint32_t`, `std::uint64_t`.
- Integer numbers: `std::int8_t`, `std::int16_t`, `std::int32_t`, `std::int64_t`.
- Floating-point numbers: `float`, `double`, `long double`.
- Strings: `std::string`. If the string contains whitespace, make sure to enclose it in double quotes (`"`).
- Paths: `std::filesystem::path`.
- Enumerations: see the [Enumerations](#33-user-guide-enumerations) section.
- Classes and Structures: see the [Classes and Structures](#34-user-guide-classes-and-structures) section.

Once all arguments have been defined, the `lector::Arguments::parse()` method can be used to parse `argc` and `argv`. For example:

```cpp
arguments.parse(argc, argv);
```

This populates all arguments with their parsed values and performs strict error checking. See the [Error Checking](#35-user-guide-error-checking) section for details.

Usage information can be obtained via the `lector::Arguments::print_usage_command()` and `lector::Arguments::print_usage_options()` methods. For example:

```cpp
std::cout << "Usage: " << arguments.print_usage_command() << std::endl;
std::cout << "Options: " << std::endl;
std::cout << arguments.print_usage_options() << std::endl;
```

Execution information can be obtained via the  `lector::Arguments::print_execution()` method. For example:

```cpp
std::cout << "Execution: " << arguments.print_execution() << std::endl;
```

See the [Command Line](#32-user-guide-command-line) section for details.

Individual arguments can be fetched via the `lector::Arguments::get()` method using the argument's enumeration value as a template. For example:

```cpp
const std::filesystem::path& output_directory{
  arguments.get<Label::OutputDirectory>().parsed_value().value()};

const std::int32_t iterations{
  arguments.get<Label::Iterations>().parsed_or_default_value()};
```

[(Back to User Guide)](#3-user-guide)

### 3.2. User Guide: Command Line

The Lector library allows you to flexibly run your program from the command line and to conveniently display the usage and execution information of your program. The following examples use the code from the [**Introduction**](#1-introduction) section.

Display usage information via the `lector::Arguments::print_usage_command()` and `lector::Arguments::print_usage_options()` methods:

```text
path/to/my_project_main --help

Usage: my_project_main --output_directory <path> [--iterations <number>] [--help]
Options:
-o <path>, --output_directory <path>  Output directory. Required.
[-i <number>, --iterations <number>]  Number of iterations. Optional. Default: 100.
[-h, --help]  Display usage information and exit. Optional.
```

Display execution information via the  `lector::Arguments::print_execution()` method:

```text
path/to/my_project_main --output_directory /some/path --iterations 200

Start.
Execution: path/to/my_project_main --output_directory /some/path --iterations 200
Running my project's main function...
End.
```

Inline key-value pairs of the form `key=value` are also supported:

```text
path/to/my_project_main --output_directory=/some/path --iterations=200

Start.
Execution: path/to/my_project_main --output_directory /some/path --iterations 200
Running my project's main function...
End.
```

Arguments can be defined with multiple keys. For example:

```text
path/to/my_project_main -o /some/path -i=200

Start.
Execution: path/to/my_project_main --output_directory /some/path --iterations 200
Running my project's main function...
End.
```

Keys do not need to start with a hyphen (`-`); keys can be composed of any characters, including equal signs (`=`). For example, the following definition is unusual but perfectly valid:

```cpp
lector::Argument<Label::Iterations, std::int32_t>{
  {"=i=", "=_==it==_=", "==iterations=="}, "Number of iterations. Optional. Default: 100.", 100
},
```

```text
path/to/my_project_main =i= 200

Start.
Execution: path/to/my_project_main ==iterations== 200
Running my project's main function...
End.
```

[(Back to User Guide)](#3-user-guide)

### 3.3. User Guide: Enumerations

Enumerations can be used as argument types, but require specializing the `lector::Printings` and `lector::Parsings` compile-time constant expression fixed-size arrays for their values. For example:

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
inline constexpr std::array<Printing<my_project::Shape>, 3> Printings<my_project::Shape>{
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

With the above definitions, the `lector::print()` and `lector::parse()` methods can now be used with this enumeration. For example:

```cpp
const std::string printed_triangle{lector::print(my_project::Shape::Triangle)};
assert(printed_triangle == "Triangle");

const std::optional<my_project::Shape> parsed_triangle{lector::parse("TRIANGLE")};
assert(parsed_triangle.has_value());
assert(parsed_triangle.value() == my_project::Shape::Triangle);

const std::optional<my_project::Shape> invalid_shape{lector::parse("Invalid Shape")};
assert(!invalid_shape.has_value());
```

The enumeration can also be used as a command line argument. For example:

```cpp
#include <cstdint>
#include <iostream>
#include <lector/arguments.hpp>
#include <lector/print.hpp>
#include <my_project/shape.hpp>

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

### 3.4. User Guide: Classes and Structures

Classes and structures can be used as argument types, but require specializing the input and output stream operators (`<<` and `>>`). For example:

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

With the above definitions, the `lector::print()` and `lector::parse()` methods from `#include <lector/print.hpp>` and `#include <lector/parse.hpp>` can now be used with this data structure. For example:

```cpp
const std::string printed_point{lector::print(my_project::Point{.x = 1.0, .y = 2.0, .z = 3.0})};
assert(printed_point == "1 2 3");

const std::optional<my_project::Point> parsed_point{lector::parse("4.0 5.0 6.0")};
assert(parsed_point.has_value());
assert(parsed_point.value() == my_project::Point{.x = 4.0, .y = 5.0, .z = 6.0});
```

The data structure can also be used as a command line argument. For example:

```cpp
#include <cstdint>
#include <iostream>
#include <lector/arguments.hpp>
#include <lector/print.hpp>
#include <my_project/point.hpp>

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

  std::cout << "Your favorite point is: " << lector::print(point) << std::endl;

  return EXIT_SUCCESS;
}
```

[(Back to User Guide)](#3-user-guide)

### 3.5. User Guide: Error Checking

The Lector library performs strict error checking when defining command line arguments and again when parsing these arguments from the command line. The following examples use the code from the [**Introduction**](#1-introduction) section.

When defining arguments, all arguments must each have at least one key:

```cpp
lector::Argument<Label::OutputDirectory, std::filesystem::path>{
  {}, "Output directory. Required."
};
```

```text
terminate called after throwing an instance of 'std::logic_error'
  what(): All arguments must each have at least one key.
```

When defining arguments, arguments cannot have empty keys:

```cpp
lector::Argument<Label::OutputDirectory, std::filesystem::path>{
  {"-o", ""}, "Output directory. Required."
};
```

```text
terminate called after throwing an instance of 'std::logic_error'
  what(): Empty key in argument '-o <path>'. Arguments cannot have empty keys.
```

When defining arguments, an argument cannot have duplicated keys:

```cpp
lector::Argument<Label::OutputDirectory, std::filesystem::path>{
  {"-o", "-o"}, "Output directory. Required."
};
```

```text
terminate called after throwing an instance of 'std::logic_error'
  what(): Duplicated key '-o' in argument '-o <path>'. Arguments cannot have duplicate keys.
```

When defining arguments, all arguments must have descriptions:

```cpp
lector::Argument<Label::OutputDirectory, std::filesystem::path>{
  {"-o", "--output_directory"}, ""
};
```

```text
terminate called after throwing an instance of 'std::logic_error'
  what(): Empty description in argument '--output_directory <path>'. All arguments must have
          descriptions.
```

When defining arguments, boolean arguments must be optional:

```cpp
lector::Argument<Label::OutputDirectory, bool>{
  {"-v", "--verbose"}, "Enable verbose mode.", true
};
```

```text
terminate called after throwing an instance of 'std::logic_error'
  what(): Specified default value for boolean argument '--verbose'. Boolean arguments are always
          false by default and cannot specify default values.
```

When parsing arguments, the Lector library checks for missing required arguments:

```text
path/to/my_project_main --iterations 200

terminate called after throwing an instance of 'std::invalid_argument'
  what(): Missing required argument '--output_directory <path>'.
```

When parsing arguments, the Lector library checks for duplicated arguments:

```text
path/to/my_project_main --output_directory /some/path --output_directory /some/other/path

terminate called after throwing an instance of 'std::invalid_argument'
  what(): Duplicated argument '--output_directory <path>'.
```

When parsing arguments, the Lector library checks for invalid argument values:

```text
path/to/my_project_main --output_directory /some/path --iterations invalid_value

terminate called after throwing an instance of 'std::invalid_argument'
  what(): Invalid value 'invalid_value' for argument '--iterations <number>'.
```

When parsing arguments, the Lector library checks for arguments missing values:

```text
path/to/my_project_main --output_directory /some/path --iterations

terminate called after throwing an instance of 'std::invalid_argument'
  what(): Missing value for argument '--iterations <number>'.
```

When parsing arguments, the Lector library checks for unknown arguments:

```text
path/to/my_project_main --output_directory /some/path --unknown_argument

terminate called after throwing an instance of 'std::invalid_argument'
  what(): Unknown argument '--unknown_argument'.
```

[(Back to User Guide)](#3-user-guide)

## 4. Developer Guide

To check out, build, and test the Lector library for yourself, first clone the Lector repository with:

```bash
git clone git@github.com:acodcha/lector.git lector
cd lector
```

Next, refer to the section for your preferred build system:

- [Bazel](#41-developer-guide-bazel)
- [CMake](#42-developer-guide-cmake)
- [Meson](#43-developer-guide-meson)

[(Back to Top)](#lector)

### 4.1. Developer Guide: Bazel

If using the Bazel build system, build the Lector library from the base directory of the Lector repository with:

```bash
bazel build //...
```

Optionally build and run all Lector library tests from the base directory of the Lector repository with:

```bash
bazel test //...
```

[(Back to Developer Guide)](#4-developer-guide)

### 4.2. Developer Guide: CMake

If using the CMake build system, build the Lector library from the base directory of the Lector repository with:

```bash
cmake -S . -B build
cmake --build build --parallel
```

Optionally build and run all Lector library tests from the base directory of the Lector repository with:

```bash
cmake -S . -B build -D LECTOR_TEST=ON
cmake --build build --parallel
ctest --test-dir build
```

[(Back to Developer Guide)](#4-developer-guide)

### 4.3. Developer Guide: Meson

If using the Meson build system, build the Lector library from the base directory of the Lector repository with:

```bash
mkdir --parents subprojects
meson wrap install gtest
meson setup build
meson compile -C build
```

Optionally run all Lector library tests from the base directory of the Lector repository with:

```bash
meson test -C build
```

[(Back to Developer Guide)](#4-developer-guide)

## 5. License

Copyright © 2026, Alexandre Coderre-Chabot.

Lector is licensed under the MIT License. See the [LICENSE](LICENSE) file or <https://mit-license.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[(Back to Top)](#lector)
