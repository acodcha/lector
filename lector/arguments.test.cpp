// Copyright © 2026, Alexandre Coderre-Chabot.

// This file is part of Lector (https://github.com/acodcha/lector), a C++ library for parsing
// command line arguments. Lector is licensed under the MIT License (https://mit-license.org).

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// - The above copyright notice and this permission notice shall be included in all copies or
//   substantial portions of the Software.
// - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
//   BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "lector/arguments.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "lector/parse.hpp"
#include "lector/print.hpp"

#ifdef _MSC_VER
  #include <string.h>
#endif

namespace test {

namespace {

/// @brief Shape. Enumeration type used for testing the parsing of enumeration command line
/// arguments.
enum class Shape : std::int8_t {
  /// @brief Circle shape.
  Circle,

  /// @brief Triangle shape.
  Triangle,

  /// @brief Square shape.
  Square,
};

/// @brief Point in three-dimensional space. Data structure type used for testing the parsing of
/// data structure command line arguments.
struct Point final {
public:
  /// @brief Cartesian x-coordinate of this point.
  float x{0.0F};

  /// @brief Cartesian y-coordinate of this point.
  float y{0.0F};

  /// @brief Cartesian z-coordinate of this point.
  float z{0.0F};
};

/// @brief Equality operator for the test::Point data structure.
/// @param[in] first The first point to compare.
/// @param[in] second The second point to compare.
/// @return Returns true if both points are equal, and false otherwise.
constexpr bool operator==(const test::Point& first, const test::Point& second) {
  return first.x == second.x && first.y == second.y && first.z == second.z;
}

/// @brief Input stream operator for the test::Point data structure. Populates a test::Point data
/// structure from an input stream.
inline std::istream& operator>>(std::istream& input_stream, test::Point& point) {
  input_stream >> point.x >> point.y >> point.z;
  return input_stream;
}

/// @brief Output stream operator for the test::Point data structure. Prints a test::Point data
/// structure to an output stream.
inline std::ostream& operator<<(std::ostream& output_stream, const test::Point& point) {
  output_stream
      << lector::print(point.x) << " " << lector::print(point.y) << " " << lector::print(point.z);
  return output_stream;
}

/// @brief Default point in three-dimensional space.
inline constexpr test::Point FirstPoint{1.0F, 2.0F, 3.0F};

/// @brief Another point in three-dimensional space.
inline constexpr test::Point SecondPoint{4.0F, 5.0F, 6.0F};

/// @brief A third point in three-dimensional space.
inline constexpr test::Point ThirdPoint{7.0F, 8.0F, 9.0F};

/// @brief A fourth point in three-dimensional space.
inline constexpr test::Point FourthPoint{10.0F, 11.0F, 12.0F};

}  // namespace

}  // namespace test

namespace lector {

/// @brief Specialization of the lector::Names constant for the test::Shape enumeration.
template <>
inline constexpr std::array<lector::Name<test::Shape>, 3> Names<test::Shape>{
  {
   {test::Shape::Circle, "Circle"},
   {test::Shape::Triangle, "Triangle"},
   {test::Shape::Square, "Square"},
   }
};

/// @brief Specialization of the lector::Spellings constant for the test::Shape enumeration.
template <>
inline constexpr std::array<lector::Spelling<test::Shape>, 9> Spellings<test::Shape>{
  {
   {"Circle", test::Shape::Circle},
   {"Triangle", test::Shape::Triangle},
   {"Square", test::Shape::Square},
   {"circle", test::Shape::Circle},
   {"triangle", test::Shape::Triangle},
   {"square", test::Shape::Square},
   {"CIRCLE", test::Shape::Circle},
   {"TRIANGLE", test::Shape::Triangle},
   {"SQUARE", test::Shape::Square},
   }
};

}  // namespace lector

namespace test {

namespace {

/// @brief Labels of the command line arguments used for testing.
enum class Label : std::int8_t {
  Tokens,
  Title,
  OutputDirectory,
  Shape,
  Point,
  Iterations,
  IterationsDuplicateKeys,
  Tolerance,
  ConfusingLong,
  ConfusingShort,
  Weird,
  Help,
};

/// @brief The number 100.
constexpr std::int32_t OneHundred{100};

/// @brief The number 200.
constexpr std::int32_t TwoHundred{200};

/// @brief The number 300.
constexpr std::int32_t ThreeHundred{300};

/// @brief The number 400.
constexpr std::int32_t FourHundred{400};

/// @brief The fraction 1/8.
constexpr float OneOverEight{0.125F};

/// @brief The fraction 1/16.
constexpr float OneOverSixteen{0.0625F};

/// @brief The fraction 1/32.
constexpr float OneOverThirtyTwo{0.03125F};

/// @brief The fraction 1/64.
constexpr float OneOverSixtyFour{0.015625F};

/// @brief Helper function that creates a simple lector::Configuration data structure for testing
/// the lector::Arguments class.
/// @return A simple lector::Configuration data structure for testing the lector::Arguments class.
lector::Configuration configuration() {
  lector::Configuration configuration{
    "My Application", "An application for testing the Lector library.",
    "Additional notes for the application for testing the lector library."};
  return configuration;
};

/// @brief Helper function that creates simple keys for a boolean command line argument.
/// @return Simple keys for a boolean command line argument.
std::vector<std::string> keys_boolean() {
  return std::vector<std::string>{"-h", "--help"};
}

/// @brief Helper function that creates simple keys for a command line argument with long confusing
/// keys.
/// @return Simple keys for a command line argument with long confusing keys.
std::vector<std::string> keys_confusing_long() {
  return std::vector<std::string>{"--key=200"};
}

/// @brief Helper function that creates simple keys for a command line argument with short confusing
/// keys.
/// @return Simple keys for a command line argument with short confusing keys.
std::vector<std::string> keys_confusing_short() {
  return std::vector<std::string>{"--key"};
}

/// @brief Helper function that creates simple keys for a data structure command line argument.
/// @return Simple keys for a data structure command line argument.
std::vector<std::string> keys_data_structure() {
  return std::vector<std::string>{"-p", "--point"};
}

/// @brief Helper function that creates simple keys for an enumeration command line argument.
/// @return Simple keys for an enumeration command line argument.
std::vector<std::string> keys_enumeration() {
  return std::vector<std::string>{"-s", "--shape"};
}

/// @brief Helper function that creates simple keys for a filesystem path command line argument.
/// @return Simple keys for a filesystem path command line argument.
std::vector<std::string> keys_filesystem_path() {
  return std::vector<std::string>{"-o", "--output_directory"};
}

/// @brief Helper function that creates simple keys for a floating-point number command line
/// argument.
/// @return Simple keys for a floating-point number command line argument.
std::vector<std::string> keys_floating_point_number() {
  return std::vector<std::string>{"-t", "--tolerance"};
}

/// @brief Helper function that creates simple keys for an integer command line argument.
/// @return Simple keys for an integer command line argument.
std::vector<std::string> keys_integer() {
  return std::vector<std::string>{"-i", "--iterations"};
}

/// @brief Helper function that creates simple keys for an integer command line argument. One of the
/// keys intentionally duplicates a key from test::keys_integer().
/// @return Simple keys for an integer command line argument.
std::vector<std::string> keys_integer_duplicate_keys() {
  return std::vector<std::string>{"-it", "--iterations"};
}

/// @brief Helper function that creates simple keys for a string command line argument.
/// @return Simple keys for a string command line argument.
std::vector<std::string> keys_string() {
  return std::vector<std::string>{"-t", "--title"};
}

/// @brief Helper function that creates weird keys for a command line argument. The keys
/// intentionally contain equal sign (=) characters to test the handling of such cases.
/// @return Weird keys for a command line argument.
std::vector<std::string> keys_weird() {
  return std::vector<std::string>{"=w=k", "==weird=key"};
}

/// @brief Helper function that creates a repeatable named optional boolean command line argument.
/// @return The repeatable named optional boolean command line argument.
lector::RepeatableArgument<test::Label::Help, bool> repeatable_argument_boolean() {
  return lector::RepeatableArgument<test::Label::Help, bool>{
    test::keys_boolean(), "Display this help information and exit. Optional."};
}

/// @brief Helper function that creates a repeatable named optional command line argument with a
/// long confusing key.
/// @return The repeatable named optional command line argument with a long confusing key.
lector::RepeatableArgument<test::Label::ConfusingLong, std::int32_t>
repeatable_argument_confusing_long() {
  return lector::RepeatableArgument<test::Label::ConfusingLong, std::int32_t>{
    test::keys_confusing_long(), "Long confusing argument.",
    std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

/// @brief Helper function that creates a repeatable named optional command line argument with a
/// short confusing key.
/// @return The repeatable named optional command line argument with a short confusing key.
lector::RepeatableArgument<test::Label::ConfusingShort, std::int32_t>
repeatable_argument_confusing_short() {
  return lector::RepeatableArgument<test::Label::ConfusingShort, std::int32_t>{
    test::keys_confusing_short(), "Short confusing argument.",
    std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

/// @brief Helper function that creates a repeatable named optional data structure command line
/// argument.
/// @return The repeatable named optional data structure command line argument.
lector::RepeatableArgument<test::Label::Point, test::Point>
repeatable_argument_data_structure_named_optional() {
  return lector::RepeatableArgument<test::Label::Point, test::Point>{
    test::keys_data_structure(), "Starting point.",
    std::vector<test::Point>{test::FirstPoint, test::SecondPoint}
  };
}

/// @brief Helper function that creates a repeatable named required data structure command line
/// argument.
/// @return The repeatable named required data structure command line argument.
lector::RepeatableArgument<test::Label::Point, test::Point>
repeatable_argument_data_structure_named_required() {
  return lector::RepeatableArgument<test::Label::Point, test::Point>{
    test::keys_data_structure(), "Starting point."};
}

/// @brief Helper function that creates a repeatable positional optional data structure command line
/// argument.
/// @return The repeatable positional optional data structure command line argument.
lector::RepeatableArgument<test::Label::Point, test::Point>
repeatable_argument_data_structure_positional_optional() {
  return lector::RepeatableArgument<test::Label::Point, test::Point>{
    "Starting point.", std::vector<test::Point>{test::FirstPoint, test::SecondPoint}
  };
}

/// @brief Helper function that creates a repeatable positional required data structure command line
/// argument.
/// @return The repeatable positional required data structure command line argument.
lector::RepeatableArgument<test::Label::Point, test::Point>
repeatable_argument_data_structure_positional_required() {
  return lector::RepeatableArgument<test::Label::Point, test::Point>{"Starting point."};
}

/// @brief Helper function that creates a repeatable named optional enumeration command line
/// argument.
/// @return The repeatable named optional enumeration command line argument.
lector::RepeatableArgument<test::Label::Shape, test::Shape>
repeatable_argument_enumeration_named_optional() {
  return lector::RepeatableArgument<test::Label::Shape, test::Shape>{
    test::keys_enumeration(), "Favorite shape.",
    std::vector<test::Shape>{test::Shape::Circle, test::Shape::Triangle}
  };
}

/// @brief Helper function that creates a repeatable named required enumeration command line
/// argument.
/// @return The repeatable named required enumeration command line argument.
lector::RepeatableArgument<test::Label::Shape, test::Shape>
repeatable_argument_enumeration_named_required() {
  return lector::RepeatableArgument<test::Label::Shape, test::Shape>{
    test::keys_enumeration(), "Favorite shape."};
}

/// @brief Helper function that creates a repeatable positional optional enumeration command line
/// argument.
/// @return The repeatable positional optional enumeration command line argument.
lector::RepeatableArgument<test::Label::Shape, test::Shape>
repeatable_argument_enumeration_positional_optional() {
  return lector::RepeatableArgument<test::Label::Shape, test::Shape>{
    "Favorite shape.", std::vector<test::Shape>{test::Shape::Circle, test::Shape::Triangle}
  };
}

/// @brief Helper function that creates a repeatable positional required enumeration command line
/// argument.
/// @return The repeatable positional required enumeration command line argument.
lector::RepeatableArgument<test::Label::Shape, test::Shape>
repeatable_argument_enumeration_positional_required() {
  return lector::RepeatableArgument<test::Label::Shape, test::Shape>{"Favorite shape."};
}

/// @brief Helper function that creates a repeatable named optional filesystem path command line
/// argument.
/// @return The repeatable named optional filesystem path command line argument.
lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>
repeatable_argument_filesystem_path_named_optional() {
  return lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>{
    test::keys_filesystem_path(), "Output directory.",
    std::vector<std::filesystem::path>{
                                       std::filesystem::path{"/first/path"},
                                       std::filesystem::path{"/second/path"},
                                       }
  };
}

/// @brief Helper function that creates a repeatable named required filesystem path command line
/// argument.
/// @return The repeatable named required filesystem path command line argument.
lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>
repeatable_argument_filesystem_path_named_required() {
  return lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>{
    test::keys_filesystem_path(), "Output directory."};
}

/// @brief Helper function that creates a repeatable positional optional filesystem path command
/// line argument.
/// @return The repeatable positional optional filesystem path command line argument.
lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>
repeatable_argument_filesystem_path_positional_optional() {
  return lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>{
    "Output directory.",
    std::vector<std::filesystem::path>{
                                       std::filesystem::path{"/first/path"},
                                       std::filesystem::path{"/second/path"},
                                       }
  };
}

/// @brief Helper function that creates a repeatable positional required filesystem path command
/// line argument.
/// @return The repeatable positional required filesystem path command line argument.
lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>
repeatable_argument_filesystem_path_positional_required() {
  return lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path>{
    "Output directory."};
}

/// @brief Helper function that creates a repeatable named optional floating-point number command
/// line argument.
/// @return The repeatable named optional floating-point number command line argument.
lector::RepeatableArgument<test::Label::Tolerance, float>
repeatable_argument_floating_point_number_named_optional() {
  return lector::RepeatableArgument<test::Label::Tolerance, float>{
    test::keys_floating_point_number(), "Tolerance value.",
    std::vector<float>{test::OneOverThirtyTwo, test::OneOverSixtyFour}
  };
}

/// @brief Helper function that creates a repeatable named required floating-point number command
/// line argument.
/// @return The repeatable named required floating-point number command line argument.
lector::RepeatableArgument<test::Label::Tolerance, float>
repeatable_argument_floating_point_number_named_required() {
  return lector::RepeatableArgument<test::Label::Tolerance, float>{
    test::keys_floating_point_number(), "Tolerance value."};
}

/// @brief Helper function that creates a repeatable positional optional floating-point number
/// command line argument.
/// @return The repeatable positional optional floating-point number command line argument.
lector::RepeatableArgument<test::Label::Tolerance, float>
repeatable_argument_floating_point_number_positional_optional() {
  return lector::RepeatableArgument<test::Label::Tolerance, float>{
    "Tolerance value.", std::vector<float>{test::OneOverThirtyTwo, test::OneOverSixtyFour}
  };
}

/// @brief Helper function that creates a repeatable positional required floating-point number
/// command line argument.
/// @return The repeatable positional required floating-point number command line argument.
lector::RepeatableArgument<test::Label::Tolerance, float>
repeatable_argument_floating_point_number_positional_required() {
  return lector::RepeatableArgument<test::Label::Tolerance, float>{"Tolerance value."};
}

/// @brief Helper function that creates a repeatable named optional integer command line argument.
/// @return The repeatable named optional integer command line argument.
lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_named_optional() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations.",
    std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

/// @brief Helper function that creates a repeatable named required integer command line argument.
/// @return The repeatable named required integer command line argument.
lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_named_required() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations."};
}

/// @brief Helper function that creates a repeatable positional optional integer command line
/// argument.
/// @return The repeatable positional optional integer command line argument.
lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_positional_optional() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{
    "Number of iterations.", std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

/// @brief Helper function that creates a repeatable positional required integer command line
/// argument.
/// @return The repeatable positional required integer command line argument.
lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_positional_required() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{"Number of iterations."};
}

/// @brief Helper function that creates an invalid repeatable named required argument with all empty
/// keys.
void repeatable_argument_invalid_all_empty_keys() {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid repeatable named required argument with an empty
/// key.
void repeatable_argument_invalid_an_empty_key() {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"-i", "--iterations", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid repeatable positional required boolean argument.
/// Boolean command line must always specify one or more keys.
void repeatable_argument_invalid_boolean_positional() {
  const lector::RepeatableArgument<test::Label::Help, bool> argument{
    "Display this help information and exit. Optional."};
}

/// @brief Helper function that creates an invalid repeatable named optional boolean argument.
/// Boolean command line arguments are always optional and always default to false, so they cannot
/// specify default values.
void repeatable_argument_invalid_boolean_with_default_values() {
  const lector::RepeatableArgument<test::Label::Help, bool> argument{
    test::keys_boolean(), "Display this help information and exit. Optional.",
    std::vector<bool>{true, true}
  };
}

/// @brief Helper function that creates an invalid repeatable named required argument with duplicate
/// keys.
void repeatable_argument_invalid_duplicate_keys() {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"-i", "--iterations", "-i"},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid repeatable named required argument with an empty
/// description.
void repeatable_argument_invalid_empty_description() {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::keys_integer(), ""};
}

/// @brief Helper function that creates an invalid repeatable named required argument with no keys.
void repeatable_argument_invalid_no_keys() {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{}, "Number of iterations."};
}

/// @brief Helper function that creates a repeatable named optional string command line argument.
/// @return The repeatable named optional string command line argument.
lector::RepeatableArgument<test::Label::Title, std::string>
repeatable_argument_string_named_optional() {
  return lector::RepeatableArgument<test::Label::Title, std::string>{
    test::keys_string(), "Report title.",
    std::vector<std::string>{"My First Report", "My Second Report"}
  };
}

/// @brief Helper function that creates a repeatable named required string command line argument.
/// @return The repeatable named required string command line argument.
lector::RepeatableArgument<test::Label::Title, std::string>
repeatable_argument_string_named_required() {
  return lector::RepeatableArgument<test::Label::Title, std::string>{
    test::keys_string(), "Report title."};
}

/// @brief Helper function that creates a repeatable positional optional string command line
/// argument.
/// @return The repeatable positional optional string command line argument.
lector::RepeatableArgument<test::Label::Title, std::string>
repeatable_argument_string_positional_optional() {
  return lector::RepeatableArgument<test::Label::Title, std::string>{
    "Report title.", std::vector<std::string>{"My First Report", "My Second Report"}
  };
}

/// @brief Helper function that creates a repeatable positional required string command line
/// argument.
/// @return The repeatable positional required string command line argument.
lector::RepeatableArgument<test::Label::Title, std::string>
repeatable_argument_string_positional_required() {
  return lector::RepeatableArgument<test::Label::Title, std::string>{"Report title."};
}

/// @brief Helper function that creates a repeatable named optional integer command line argument
/// with weird keys.
/// @return The repeatable named optional integer command line argument with weird keys.
lector::RepeatableArgument<test::Label::Weird, std::int32_t>
repeatable_argument_weird_keys_optional() {
  return lector::RepeatableArgument<test::Label::Weird, std::int32_t>{
    test::keys_weird(), "Weird argument.",
    std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

/// @brief Helper function that creates a repeatable named required integer command line argument
/// with weird keys.
/// @return The repeatable named required integer command line argument with weird keys.
lector::RepeatableArgument<test::Label::Weird, std::int32_t>
repeatable_argument_weird_keys_required() {
  return lector::RepeatableArgument<test::Label::Weird, std::int32_t>{
    test::keys_weird(), "Weird argument."};
}

/// @brief Helper function that creates a singular named optional boolean command line argument.
/// @return The singular named optional boolean command line argument.
lector::SingularArgument<test::Label::Help, bool> singular_argument_boolean() {
  return lector::SingularArgument<test::Label::Help, bool>{
    test::keys_boolean(), "Display this help information and exit. Optional."};
}

/// @brief Helper function that creates a singular named optional command line argument with a long
/// confusing key.
/// @return The singular named optional command line argument with a long confusing key.
lector::SingularArgument<test::Label::ConfusingLong, std::int32_t>
singular_argument_confusing_long() {
  return lector::SingularArgument<test::Label::ConfusingLong, std::int32_t>{
    test::keys_confusing_long(), "Long confusing argument.", test::OneHundred};
}

/// @brief Helper function that creates a singular named optional command line argument with a short
/// confusing key.
/// @return The singular named optional command line argument with a short confusing key.
lector::SingularArgument<test::Label::ConfusingShort, std::int32_t>
singular_argument_confusing_short() {
  return lector::SingularArgument<test::Label::ConfusingShort, std::int32_t>{
    test::keys_confusing_short(), "Short confusing argument.", test::OneHundred};
}

/// @brief Helper function that creates a singular named optional data structure command line
/// argument.
/// @return The singular named optional data structure command line argument.
lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_named_optional() {
  return lector::SingularArgument<test::Label::Point, test::Point>{
    test::keys_data_structure(), "Starting point.", test::FirstPoint};
}

/// @brief Helper function that creates a singular named required data structure command line
/// argument.
/// @return The singular named required data structure command line argument.
lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_named_required() {
  return lector::SingularArgument<test::Label::Point, test::Point>{
    test::keys_data_structure(), "Starting point."};
}

/// @brief Helper function that creates a singular positional optional data structure command line
/// argument.
/// @return The singular positional optional data structure command line argument.
lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_positional_optional() {
  return lector::SingularArgument<test::Label::Point, test::Point>{
    "Starting point.", test::FirstPoint};
}

/// @brief Helper function that creates a singular positional required data structure command line
/// argument.
/// @return The singular positional required data structure command line argument.
lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_positional_required() {
  return lector::SingularArgument<test::Label::Point, test::Point>{"Starting point."};
}

/// @brief Helper function that creates a singular named optional enumeration command line argument.
/// @return The singular named optional enumeration command line argument.
lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_named_optional() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{
    test::keys_enumeration(), "Favorite shape.", test::Shape::Circle};
}

/// @brief Helper function that creates a singular named required enumeration command line argument.
/// @return The singular named required enumeration command line argument.
lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_named_required() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{
    test::keys_enumeration(), "Favorite shape."};
}

/// @brief Helper function that creates a singular positional optional enumeration command line
/// argument.
/// @return The singular positional optional enumeration command line argument.
lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_positional_optional() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{
    "Favorite shape.", test::Shape::Circle};
}

/// @brief Helper function that creates a singular positional required enumeration command line
/// argument.
/// @return The singular positional required enumeration command line argument.
lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_positional_required() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{"Favorite shape."};
}

/// @brief Helper function that creates a singular named optional filesystem path command line
/// argument.
/// @return The singular named optional filesystem path command line argument.
lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_named_optional() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    test::keys_filesystem_path(), "Output directory.", std::filesystem::path{"/some/path"}};
}

/// @brief Helper function that creates a singular named required filesystem path command line
/// argument.
/// @return The singular named required filesystem path command line argument.
lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_named_required() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    test::keys_filesystem_path(), "Output directory."};
}

/// @brief Helper function that creates a singular positional optional filesystem path command line
/// argument.
/// @return The singular positional optional filesystem path command line argument.
lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_positional_optional() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    "Output directory.", std::filesystem::path{"/some/path"}};
}

/// @brief Helper function that creates a singular positional required filesystem path command line
/// argument.
/// @return The singular positional required filesystem path command line argument.
lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_positional_required() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    "Output directory."};
}

/// @brief Helper function that creates a singular named optional floating-point number command line
/// argument.
/// @return The singular named optional floating-point number command line argument.
lector::SingularArgument<test::Label::Tolerance, float>
singular_argument_floating_point_number_named_optional() {
  return lector::SingularArgument<test::Label::Tolerance, float>{
    test::keys_floating_point_number(), "Tolerance value.", test::OneOverThirtyTwo};
}

/// @brief Helper function that creates a singular named required floating-point number command line
/// argument.
/// @return The singular named required floating-point number command line argument.
lector::SingularArgument<test::Label::Tolerance, float>
singular_argument_floating_point_number_named_required() {
  return lector::SingularArgument<test::Label::Tolerance, float>{
    test::keys_floating_point_number(), "Tolerance value."};
}

/// @brief Helper function that creates a singular positional optional floating-point number command
/// line argument.
/// @return The singular positional optional floating-point number command line argument.
lector::SingularArgument<test::Label::Tolerance, float>
singular_argument_floating_point_number_positional_optional() {
  return lector::SingularArgument<test::Label::Tolerance, float>{
    "Tolerance value.", test::OneOverThirtyTwo};
}

/// @brief Helper function that creates a singular positional required floating-point number command
/// line argument.
/// @return The singular positional required floating-point number command line argument.
lector::SingularArgument<test::Label::Tolerance, float>
singular_argument_floating_point_number_positional_required() {
  return lector::SingularArgument<test::Label::Tolerance, float>{"Tolerance value."};
}

/// @brief Helper function that creates a singular named optional integer command line argument with
/// duplicate keys.
/// @return The singular named optional integer command line argument with duplicate keys.
lector::SingularArgument<test::Label::IterationsDuplicateKeys, std::int32_t>
singular_argument_integer_duplicate_keys() {
  return lector::SingularArgument<test::Label::IterationsDuplicateKeys, std::int32_t>{
    test::keys_integer_duplicate_keys(), "Number of iterations, again.", test::OneHundred};
}

/// @brief Helper function that creates a singular named optional integer command line argument.
/// @return The singular named optional integer command line argument.
lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_named_optional() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations.", test::OneHundred};
}

/// @brief Helper function that creates a singular named required integer command line argument.
/// @return The singular named required integer command line argument.
lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_named_required() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations."};
}

/// @brief Helper function that creates a singular positional optional integer command line
/// argument.
/// @return The singular positional optional integer command line argument.
lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_positional_optional() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{
    "Number of iterations.", test::OneHundred};
}

/// @brief Helper function that creates a singular positional required integer command line
/// argument.
/// @return The singular positional required integer command line argument.
lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_positional_required() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{"Number of iterations."};
}

/// @brief Helper function that creates an invalid singular named required argument with all empty
/// keys.
void singular_argument_invalid_all_empty_keys() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid singular named required argument with an empty
/// key.
void singular_argument_invalid_an_empty_key() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"-i", "--iterations", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid singular positional required boolean argument.
/// Boolean command line must always specify one or more keys.
void singular_argument_invalid_boolean_positional() {
  const lector::SingularArgument<test::Label::Help, bool> argument{
    "Display this help information and exit. Optional."};
}

/// @brief Helper function that creates an invalid singular named optional boolean argument. Boolean
/// command line arguments are always optional and always default to false, so they cannot specify
/// default values.
void singular_argument_invalid_boolean_with_default_value() {
  const lector::SingularArgument<test::Label::Help, bool> argument{
    test::keys_boolean(), "Display this help information and exit. Optional.", true};
}

/// @brief Helper function that creates an invalid singular named required argument with duplicate
/// keys.
void singular_argument_invalid_duplicate_keys() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"-i", "--iterations", "-i"},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid singular named required argument with an empty
/// description.
void singular_argument_invalid_empty_description() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::keys_integer(), ""};
}

/// @brief Helper function that creates an invalid singular named required argument with no keys.
void singular_argument_invalid_no_keys() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{}, "Number of iterations."};
}

/// @brief Helper function that creates a singular named optional string command line argument.
/// @return The singular named optional string command line argument.
lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_named_optional() {
  return lector::SingularArgument<test::Label::Title, std::string>{
    test::keys_string(), "Report title.", "My Report"};
}

/// @brief Helper function that creates a singular named required string command line argument.
/// @return The singular named required string command line argument.
lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_named_required() {
  return lector::SingularArgument<test::Label::Title, std::string>{
    test::keys_string(), "Report title."};
}

/// @brief Helper function that creates a singular positional optional string command line argument.
/// @return The singular positional optional string command line argument.
lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_positional_optional() {
  return lector::SingularArgument<test::Label::Title, std::string>{"Report title.", "My Report"};
}

/// @brief Helper function that creates a singular positional required string command line argument.
/// @return The singular positional required string command line argument.
lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_positional_required() {
  return lector::SingularArgument<test::Label::Title, std::string>{"Report title."};
}

/// @brief Helper function that creates a singular named optional integer command line argument with
/// weird keys.
/// @return The singular named optional integer command line argument with weird keys.
lector::SingularArgument<test::Label::Weird, std::int32_t> singular_argument_weird_keys_optional() {
  return lector::SingularArgument<test::Label::Weird, std::int32_t>{
    test::keys_weird(), "Weird argument.", test::OneHundred};
}

/// @brief Helper function that creates a singular named required integer command line argument with
/// weird keys.
/// @return The singular named required integer command line argument with weird keys.
lector::SingularArgument<test::Label::Weird, std::int32_t> singular_argument_weird_keys_required() {
  return lector::SingularArgument<test::Label::Weird, std::int32_t>{
    test::keys_weird(), "Weird argument."};
}

/// @brief Helper class to construct argc and argv for testing the parsing of command line
/// arguments.
class Command final {
public:
  /// @brief Default constructor. Initializes argc to 0 and argv to nullptr. Represents a completely
  /// empty command line.
  constexpr Command() noexcept = default;

  /// @brief Constructor. Builds argc and argv from the initializer list.
  /// @param[in] arguments The list of command line arguments, starting with the executable path.
  explicit Command(const std::vector<std::string>& arguments)
    : argc_{static_cast<int>(arguments.size())} {
    // Allocate the argv array. Note that the argv array must be null-terminated by the C standard;
    // the +1 is for the null terminator at the end of the argv array.
    argv_ = new char*[argc_ + 1];
    argv_[argc_] = nullptr;
    // Populate the argv array with C-strings from the specified arguments.
    std::size_t index{0};
    for (const std::string& argument : arguments) {
      const std::size_t length{argument.length() + 1};
      argv_[index] = new char[length];
#ifdef _MSC_VER
      ::strncpy_s(argv_[index], length, argument.c_str(), length);
#else
      std::strncpy(argv_[index], argument.c_str(), length);
#endif
      ++index;
    }
  }

  /// @brief Destructor. Deletes the dynamically allocated C-strings in argv and argv itself.
  ~Command() {
    delete_argv();
  }

  /// @brief Copy constructor. Deleted to prevent multiple-free errors when a Command object is
  /// copied, since it manages a dynamically allocated array of C-strings.
  Command(const test::Command&) = delete;

  /// @brief Copy assignment operator. Deleted to prevent multiple-free errors when a Command object
  /// is copied, since it manages a dynamically allocated array of C-strings.
  test::Command& operator=(const test::Command&) = delete;

  /// @brief Move constructor. Moves the resources of another Command object into this one, leaving
  /// the other object in a valid but unspecified state, with argc set to 0 and argv set to nullptr.
  /// @param[in, out] other The Command object to move from.
  Command(test::Command&& other) noexcept : argc_(other.argc_), argv_(other.argv_) {
    other.argc_ = 0;
    other.argv_ = nullptr;
  }

  /// @brief Move assignment operator. Moves the resources of another Command object into this one,
  /// leaving the other object in a valid but unspecified state, with argc set to 0 and argv set to
  /// nullptr. Frees the current resources of this object before taking ownership of the new
  /// resources.
  /// @param[in, out] other The Command object to move from.
  /// @return This Command object after the move assignment.
  test::Command& operator=(test::Command&& other) noexcept {
    if (this != &other) {
      delete_argv();
      argc_ = other.argc_;
      argv_ = other.argv_;
      other.argc_ = 0;
      other.argv_ = nullptr;
    }
    return *this;
  }

  /// @brief Number of command line arguments, including the executable path.
  [[nodiscard]] int argc() const {
    return argc_;
  }

  /// @brief Array of C-strings that represents the command line arguments, starting with the
  /// executable path.
  [[nodiscard]] char** argv() const {
    return argv_;
  }

private:
  /// @brief Deletes the dynamically allocated C-strings in argv and argv itself. Called by the
  /// destructor and the move assignment operator.
  void delete_argv() {
    if (argv_ != nullptr) {
      const std::size_t count{static_cast<std::size_t>(argc_)};
      for (std::size_t index{0}; index < count; ++index) {
        delete[] argv_[index];
      }
      delete[] argv_;
    }
  }

  /// @brief Number of command line arguments, including the executable path. Set at construction.
  int argc_{0};

  /// @brief Array of C-strings that represents the command line arguments, starting with the
  /// executable path. Set at construction.
  char** argv_{nullptr};
};

}  // namespace

}  // namespace test

namespace {

/// @brief Validate that an empty list of types is unique.
static_assert(lector::AreUnique<>::value);

/// @brief Validate that a list of only one type is unique.
static_assert(lector::AreUnique<test::Label::Shape>::value);

/// @brief Validate that a list of unique types are unique.
static_assert(
    lector::AreUnique<test::Label::Shape, test::Label::Iterations, test::Label::Help>::value);

/// @brief Validate that a list of duplicated types is not unique.
static_assert(!lector::AreUnique<test::Label::Shape, test::Label::Shape>::value);

/// @brief Validate that a list of duplicated and unique types is not unique.
static_assert(
    !lector::AreUnique<test::Label::Shape, test::Label::Iterations, test::Label::Shape>::value);

TEST(Lector, ArgumentsEmptyNoConfigurationNoCommand) {
  lector::Arguments arguments;
  const test::Command command;
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.usage().empty());
  EXPECT_TRUE(arguments.options().empty());
  EXPECT_TRUE(arguments.help().empty());
  EXPECT_TRUE(arguments.execution().empty());
}

TEST(Lector, ArgumentsEmptyNoConfigurationExecutableOnly) {
  lector::Arguments arguments;
  const test::Command command{{"/path/to/executable"}};
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::string expected_usage{"executable"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  EXPECT_TRUE(arguments.options().empty());
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage);
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsEmptyWithConfigurationNoCommand) {
  lector::Arguments arguments{test::configuration()};
  const test::Command command;
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.usage().empty());
  EXPECT_TRUE(arguments.options().empty());
  EXPECT_EQ(arguments.help(),
            "My Application\n\n"
            "An application for testing the Lector library.\n\n"
            "Additional notes for the application for testing the lector library.");
  EXPECT_TRUE(arguments.execution().empty());
}

TEST(Lector, ArgumentsEmptyWithConfigurationExecutableOnly) {
  lector::Arguments arguments{test::configuration()};
  const test::Command command{{"/path/to/executable"}};
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::string expected_usage{"executable"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  EXPECT_TRUE(arguments.options().empty());
  EXPECT_EQ(arguments.help(),
            "My Application\n\n"
            "Usage:\n" +
            expected_usage + "\n\n"
            "An application for testing the Lector library.\n\n"
            "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsInvalidDuplicateArgumentNoConfigurationInline) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations=200", "--iterations=300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicateArgumentNoConfigurationMixed) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200", "--iterations=300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicateArgumentNoConfigurationSeparated) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200", "--iterations", "300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicateArgumentWithConfigurationInline) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations=200", "--iterations=300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicateArgumentWithConfigurationMixed) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200", "--iterations=300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicateArgumentWithConfigurationSeparated) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200", "--iterations", "300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicateKeysNoConfiguration) {
  EXPECT_ANY_THROW(lector::Arguments(test::singular_argument_integer_named_optional(),
                                     test::singular_argument_integer_duplicate_keys()));
}

TEST(Lector, ArgumentsInvalidDuplicateKeysWithConfiguration) {
  EXPECT_ANY_THROW(
      lector::Arguments(test::configuration(), test::singular_argument_integer_named_optional(),
                        test::singular_argument_integer_duplicate_keys()));
}

TEST(Lector, ArgumentsInvalidDuplicatedTokenSingularPositionalNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_positional_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "200", "300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidDuplicatedTokenSingularPositionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_positional_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "200", "300", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidLineLengthZero) {
  lector::Arguments arguments{test::singular_argument_integer_named_optional()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "--iterations", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_ANY_THROW((void)arguments.usage(static_cast<std::size_t>(0UL)));
  EXPECT_ANY_THROW((void)arguments.options(static_cast<std::size_t>(0UL)));
  EXPECT_ANY_THROW((void)arguments.help(static_cast<std::size_t>(0UL)));
  EXPECT_ANY_THROW((void)arguments.execution(static_cast<std::size_t>(0UL)));
}

TEST(Lector, ArgumentsInvalidMissingKeyRequiredNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_required(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  EXPECT_ANY_THROW(arguments.validate());
}

TEST(Lector, ArgumentsInvalidMissingKeyRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_required(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  EXPECT_ANY_THROW(arguments.validate());
}

TEST(Lector, ArgumentsInvalidMissingValueNoConfigurationFirst) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidMissingValueNoConfigurationLast) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_named_required(),
                              test::singular_argument_integer_named_optional()};
  const test::Command command{
    {"/path/to/executable", "--output_directory", "/path/to/output", "--iterations"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidMissingValueNoConfigurationMiddle) {
  lector::Arguments arguments{
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--output_directory", "/path/to/output", "--iterations", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidMissingValueWithConfigurationFirst) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidMissingValueWithConfigurationLast) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional()};
  const test::Command command{
    {"/path/to/executable", "--output_directory", "/path/to/output", "--iterations"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidMissingValueWithConfigurationMiddle) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--output_directory", "/path/to/output", "--iterations", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidMixedRepeatablePositionalWithOtherPositional) {
  EXPECT_ANY_THROW((void)lector::Arguments(
      test::repeatable_argument_floating_point_number_positional_optional(),
      test::singular_argument_integer_positional_optional()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokenNoConfigurationRepeatablePositional) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "1.0", "Hello"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokenNoConfigurationSingularPositional) {
  lector::Arguments arguments{test::singular_argument_integer_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "200", "Hello"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokenWithConfigurationRepeatablePositional) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "1.0", "Hello"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokenWithConfigurationSingularPositional) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "200", "Hello"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokensNoConfigurationRepeatablePositional) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "1.0", "Hello", "World"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokensNoConfigurationSingularPositional) {
  lector::Arguments arguments{test::singular_argument_integer_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "200", "Hello", "World"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokensWithConfigurationRepeatablePositional) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "1.0", "Hello", "World"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnmatchedTokensWithConfigurationSingularPositional) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_positional_required()};
  const test::Command command{
    std::vector<std::string>{"/path/to/executable", "200", "Hello", "World"}
  };
  EXPECT_ANY_THROW((void)arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnknownKeyNoConfigurationInline) {
  lector::Arguments arguments{test::singular_argument_integer_named_optional()};
  const test::Command command{
    {"/path/to/executable", "--iterations=200", "--unknown"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnknownKeyNoConfigurationSeparated) {
  lector::Arguments arguments{test::singular_argument_integer_named_optional()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200", "--unknown"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnknownKeyWithConfigurationInline) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional()};
  const test::Command command{
    {"/path/to/executable", "--iterations=200", "--unknown"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidUnknownKeyWithConfigurationSeparated) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200", "--unknown"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueNoConfigurationNamedInline) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations=Hello", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueNoConfigurationNamedSeparated) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "Hello", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueNoConfigurationPositional) {
  lector::Arguments arguments{
    test::singular_argument_integer_positional_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "Hello", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueWithConfigurationNamedInline) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations=Hello", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueWithConfigurationNamedSeparated) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "Hello", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueWithConfigurationPositional) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_positional_optional(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "Hello", "--help"}
  };
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsValidMissingOptionalNoConfigurationInline) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_required(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Iterations>().parsed_value().has_value()
              && arguments.get<test::Label::Iterations>().parsed_value().value()
                     == static_cast<std::int32_t>(200));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsValidMissingOptionalNoConfigurationSeparated) {
  lector::Arguments arguments{
    test::singular_argument_integer_named_required(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Iterations>().parsed_value().has_value()
              && arguments.get<test::Label::Iterations>().parsed_value().value()
                     == static_cast<std::int32_t>(200));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsValidMissingOptionalWithConfigurationInline) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_required(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Iterations>().parsed_value().has_value()
              && arguments.get<test::Label::Iterations>().parsed_value().value()
                     == static_cast<std::int32_t>(200));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsValidMissingOptionalWithConfigurationSeparated) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_named_required(),
    test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "--iterations", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Iterations>().parsed_value().has_value()
              && arguments.get<test::Label::Iterations>().parsed_value().value()
                     == static_cast<std::int32_t>(200));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsValidConfusingInlineShortLongLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_short(), test::singular_argument_confusing_long()};
  const test::Command command{
    {"/path/to/executable", "--key=200=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key <number>      Short confusing argument.\n"
    "--key=200 <number>  Long confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingInlineShortLongLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_short(),
                              test::singular_argument_confusing_long()};
  const test::Command command{
    {"/path/to/executable", "--key=200=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key <number>      Short confusing argument.\n"
    "--key=200 <number>  Long confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingInlineLongShortLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_long(), test::singular_argument_confusing_short()};
  const test::Command command{
    {"/path/to/executable", "--key=200=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key=200 <number>  Long confusing argument.\n"
    "--key <number>      Short confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingInlineLongShortLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_long(),
                              test::singular_argument_confusing_short()};
  const test::Command command{
    {"/path/to/executable", "--key=200=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key=200 <number>  Long confusing argument.\n"
    "--key <number>      Short confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_short(), test::singular_argument_confusing_long()};
  const test::Command command{
    {"/path/to/executable", "--key=200", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key <number>      Short confusing argument.\n"
    "--key=200 <number>  Long confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_short(),
                              test::singular_argument_confusing_long()};
  const test::Command command{
    {"/path/to/executable", "--key=200", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key <number>      Short confusing argument.\n"
    "--key=200 <number>  Long confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongShortNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_short(), test::singular_argument_confusing_long()};
  const test::Command command{
    {"/path/to/executable", "--key", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingShort>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingShort>().parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key <number>      Short confusing argument.\n"
    "--key=200 <number>  Long confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongShortWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_short(),
                              test::singular_argument_confusing_long()};
  const test::Command command{
    {"/path/to/executable", "--key", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingShort>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingShort>().parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key <number>      Short confusing argument.\n"
    "--key=200 <number>  Long confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_long(), test::singular_argument_confusing_short()};
  const test::Command command{
    {"/path/to/executable", "--key=200", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key=200 <number>  Long confusing argument.\n"
    "--key <number>      Short confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_long(),
                              test::singular_argument_confusing_short()};
  const test::Command command{
    {"/path/to/executable", "--key=200", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingLong>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingLong>().parsed_value().value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key=200 <number>  Long confusing argument.\n"
    "--key <number>      Short confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortShortNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_long(), test::singular_argument_confusing_short()};
  const test::Command command{
    {"/path/to/executable", "--key", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingShort>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingShort>().parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key=200 <number>  Long confusing argument.\n"
    "--key <number>      Short confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortShortWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_long(),
                              test::singular_argument_confusing_short()};
  const test::Command command{
    {"/path/to/executable", "--key", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(
      arguments.get<test::Label::ConfusingShort>().parsed_value().has_value()
      && arguments.get<test::Label::ConfusingShort>().parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "--key=200 <number>  Long confusing argument.\n"
    "--key <number>      Short confusing argument."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidManyInlineLongKeysNoConfiguration) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "--output_directory=/path/to/output",
     "--iterations=200", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyInlineLongKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "--output_directory=/path/to/output",
     "--iterations=200", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyInlineShortKeysNoConfiguration) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "-o=/path/to/output", "-i=200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyInlineShortKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "-o=/path/to/output", "-i=200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedLongKeysNoConfiguration) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "--output_directory=/path/to/output",
     "--iterations=200", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedLongKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "--output_directory=/path/to/output",
     "--iterations=200", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedShortKeysNoConfiguration) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "-o", "/path/to/output", "-i=200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedShortKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "-o", "/path/to/output", "-i=200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyPositionalNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_filesystem_path_positional_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "/path/to/output", "-i", "200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{"executable <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<path>                              Output directory.\n"
    "-i <number>, --iterations <number>  Number of iterations.\n"
    "-h, --help                          Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(), "/path/to/executable /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyPositionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_positional_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "/path/to/output", "-i", "200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{"executable <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<path>                              Output directory.\n"
    "-i <number>, --iterations <number>  Number of iterations.\n"
    "-h, --help                          Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(), "/path/to/executable /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceLongKeysNoConfiguration) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "--output_directory", "/path/to/output",
     "--iterations", "200", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceLongKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "--output_directory", "/path/to/output",
     "--iterations", "200", "--help"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceShortKeysNoConfiguration) {
  lector::Arguments arguments{
    test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "-o", "/path/to/output", "-i", "200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(arguments.help(), "Usage:\n" + expected_usage + "\n\nOptions:\n" + expected_options);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceShortKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::repeatable_argument_floating_point_number_positional_optional(),
    test::singular_argument_filesystem_path_named_required(),
    test::singular_argument_integer_named_optional(), test::singular_argument_boolean()};
  const test::Command command{
    {"/path/to/executable", "0.125", "0.0625", "-o", "/path/to/output", "-i", "200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  ASSERT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().size(),
            static_cast<std::size_t>(2UL));
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(0), 0.125F);
  EXPECT_EQ(arguments.get<test::Label::Tolerance>().parsed_values().at(1), 0.0625F);
  EXPECT_TRUE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value()
              && arguments.get<test::Label::OutputDirectory>().parsed_value().value()
                     == std::filesystem::path("/path/to/output"));
  EXPECT_TRUE(
      arguments.get<test::Label::Iterations>().parsed_value().has_value()
      && arguments.get<test::Label::Iterations>().parsed_value().value() == test::TwoHundred);
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_value().has_value()
              && arguments.get<test::Label::Help>().parsed_value().value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               Tolerance value.\n"
    "-o <path>, --output_directory <path>  Output directory.\n"
    "-i <number>, --iterations <number>    Number of iterations.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "An application for testing the Lector library.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes for the application for testing the lector library.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.1250000000 0.06250000000 "
            "--output_directory /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsWeirdLongInlineNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_optional()};
  const test::Command command{
    {"/path/to/executable", "==weird=key=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongInlineWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_weird_keys_optional()};
  const test::Command command{
    {"/path/to/executable", "==weird=key=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongWhitespaceNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_optional()};
  const test::Command command{
    {"/path/to/executable", "==weird=key", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongWhitespaceWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_weird_keys_optional()};
  const test::Command command{
    {"/path/to/executable", "==weird=key", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortInlineNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_required()};
  const test::Command command{
    {"/path/to/executable", "=w=k=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortInlineWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_weird_keys_required()};
  const test::Command command{
    {"/path/to/executable", "=w=k=200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortWhitespaceNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_required()};
  const test::Command command{
    {"/path/to/executable", "=w=k", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortWhitespaceWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_weird_keys_required()};
  const test::Command command{
    {"/path/to/executable", "=w=k", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_TRUE(arguments.get<test::Label::Weird>().parsed_value().has_value()
              && arguments.get<test::Label::Weird>().parsed_value().value() == test::TwoHundred);
}

TEST(Lector, ArityParseEnumeration) {
  static_assert(lector::parse_enumeration<lector::Arity>("") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("Hello, world!") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("UnKnOwN") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("SiNgLe") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("RePeAtAbLe") == std::nullopt);
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("UNKNOWN")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Unknown);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("Unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Unknown);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Unknown);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("SINGULAR")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Singular);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("Singular")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Singular);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("singular")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Singular);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("REPEATABLE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Repeatable);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("Repeatable")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Repeatable);
  }
  {
    constexpr std::optional<lector::Arity> parsed{
      lector::parse_enumeration<lector::Arity>("repeatable")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Repeatable);
  }
}

TEST(Lector, ArityParseGeneral) {
  EXPECT_EQ(lector::parse<lector::Arity>(""), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Arity>("Hello, world!"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Arity>("UnKnOwN"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Arity>("SiNgLe"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Arity>("RePeAtAbLe"), std::nullopt);
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("UNKNOWN")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Unknown);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("Unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Unknown);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Unknown);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("SINGULAR")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Singular);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("Singular")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Singular);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("singular")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Singular);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("REPEATABLE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Repeatable);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("Repeatable")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Repeatable);
  }
  {
    const std::optional<lector::Arity> parsed{lector::parse<lector::Arity>("repeatable")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Arity::Repeatable);
  }
}

TEST(Lector, ArityPrintEnumeration) {
  EXPECT_EQ(lector::print_enumeration<lector::Arity>(lector::Arity::Unknown), "Unknown");
  EXPECT_EQ(lector::print_enumeration<lector::Arity>(lector::Arity::Singular), "Singular");
  EXPECT_EQ(lector::print_enumeration<lector::Arity>(lector::Arity::Repeatable), "Repeatable");
}

TEST(Lector, ArityPrintGeneral) {
  EXPECT_EQ(lector::print<lector::Arity>(lector::Arity::Unknown), "Unknown");
  EXPECT_EQ(lector::print<lector::Arity>(lector::Arity::Singular), "Singular");
  EXPECT_EQ(lector::print<lector::Arity>(lector::Arity::Repeatable), "Repeatable");
}

TEST(Lector, FormParseEnumeration) {
  static_assert(lector::parse_enumeration<lector::Form>("") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Form>("Hello, world!") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Form>("UnKnOwN") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Form>("PoSiTiOnAl") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Form>("NaMeD") == std::nullopt);
  {
    constexpr std::optional<lector::Form> parsed{
      lector::parse_enumeration<lector::Form>("UNKNOWN")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Unknown);
  }
  {
    constexpr std::optional<lector::Form> parsed{
      lector::parse_enumeration<lector::Form>("Unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Unknown);
  }
  {
    constexpr std::optional<lector::Form> parsed{
      lector::parse_enumeration<lector::Form>("unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Unknown);
  }
  {
    constexpr std::optional<lector::Form> parsed{
      lector::parse_enumeration<lector::Form>("POSITIONAL")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Positional);
  }
  {
    constexpr std::optional<lector::Form> parsed{
      lector::parse_enumeration<lector::Form>("Positional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Positional);
  }
  {
    constexpr std::optional<lector::Form> parsed{
      lector::parse_enumeration<lector::Form>("positional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Positional);
  }
  {
    constexpr std::optional<lector::Form> parsed{lector::parse_enumeration<lector::Form>("NAMED")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Named);
  }
  {
    constexpr std::optional<lector::Form> parsed{lector::parse_enumeration<lector::Form>("Named")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Named);
  }
  {
    constexpr std::optional<lector::Form> parsed{lector::parse_enumeration<lector::Form>("named")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Named);
  }
}

TEST(Lector, FormParseGeneral) {
  EXPECT_EQ(lector::parse<lector::Form>(""), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Form>("Hello, world!"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Form>("UnKnOwN"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Form>("PoSiTiOnAl"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Form>("NaMeD"), std::nullopt);
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("UNKNOWN")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Unknown);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("Unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Unknown);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Unknown);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("POSITIONAL")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Positional);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("Positional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Positional);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("positional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Positional);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("NAMED")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Named);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("Named")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Named);
  }
  {
    const std::optional<lector::Form> parsed{lector::parse<lector::Form>("named")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Form::Named);
  }
}

TEST(Lector, FormPrintEnumeration) {
  EXPECT_EQ(lector::print_enumeration<lector::Form>(lector::Form::Unknown), "Unknown");
  EXPECT_EQ(lector::print_enumeration<lector::Form>(lector::Form::Positional), "Positional");
  EXPECT_EQ(lector::print_enumeration<lector::Form>(lector::Form::Named), "Named");
}

TEST(Lector, FormPrintGeneral) {
  EXPECT_EQ(lector::print<lector::Form>(lector::Form::Unknown), "Unknown");
  EXPECT_EQ(lector::print<lector::Form>(lector::Form::Positional), "Positional");
  EXPECT_EQ(lector::print<lector::Form>(lector::Form::Named), "Named");
}

TEST(Lector, ImportanceParseEnumeration) {
  static_assert(lector::parse_enumeration<lector::Importance>("") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Importance>("Hello, world!") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Importance>("UnKnOwN") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Importance>("OpTiOnAl") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Importance>("ReQuIrEd") == std::nullopt);
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("UNKNOWN")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Unknown);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("Unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Unknown);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Unknown);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("OPTIONAL")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Optional);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("Optional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Optional);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("optional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Optional);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("REQUIRED")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Required);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("Required")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Required);
  }
  {
    constexpr std::optional<lector::Importance> parsed{
      lector::parse_enumeration<lector::Importance>("required")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Required);
  }
}

TEST(Lector, ImportanceParseGeneral) {
  EXPECT_EQ(lector::parse<lector::Importance>(""), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Importance>("Hello, world!"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Importance>("UnKnOwN"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Importance>("OpTiOnAl"), std::nullopt);
  EXPECT_EQ(lector::parse<lector::Importance>("ReQuIrEd"), std::nullopt);
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("UNKNOWN")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Unknown);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("Unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Unknown);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("unknown")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Unknown);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("OPTIONAL")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Optional);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("Optional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Optional);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("optional")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Optional);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("REQUIRED")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Required);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("Required")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Required);
  }
  {
    const std::optional<lector::Importance> parsed{lector::parse<lector::Importance>("required")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == lector::Importance::Required);
  }
}

TEST(Lector, ImportancePrintEnumeration) {
  EXPECT_EQ(lector::print_enumeration<lector::Importance>(lector::Importance::Unknown), "Unknown");
  EXPECT_EQ(
      lector::print_enumeration<lector::Importance>(lector::Importance::Optional), "Optional");
  EXPECT_EQ(
      lector::print_enumeration<lector::Importance>(lector::Importance::Required), "Required");
}

TEST(Lector, ImportancePrintGeneral) {
  EXPECT_EQ(lector::print<lector::Importance>(lector::Importance::Unknown), "Unknown");
  EXPECT_EQ(lector::print<lector::Importance>(lector::Importance::Optional), "Optional");
  EXPECT_EQ(lector::print<lector::Importance>(lector::Importance::Required), "Required");
}

TEST(Lector, RepeatableArgumentBooleanDefault) {
  lector::RepeatableArgument<test::Label::Help, bool> argument;
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_TRUE(argument.keys_with_value_type().empty());
  EXPECT_EQ(argument.usage(), "...");
  EXPECT_TRUE(argument.options().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(true));
}

TEST(Lector, RepeatableArgumentBooleanNamed) {
  lector::RepeatableArgument<test::Label::Help, bool> argument{test::repeatable_argument_boolean()};
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), test::keys_boolean());
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help] ...");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(true);
  argument.set_parsed_value(true);
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), test::keys_boolean());
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), true);
  EXPECT_EQ(argument.parsed_values().at(1), true);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), true);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), true);
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help] ...");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_EQ(argument.execution(), "--help --help");
}

TEST(Lector, RepeatableArgumentCopyAssignmentOperator) {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_FALSE(second.has_default());
  EXPECT_TRUE(second.default_values().empty());
  EXPECT_FALSE(second.has_parsed());
  EXPECT_TRUE(second.parsed_values().empty());
  EXPECT_TRUE(second.parsed_or_default_values().empty());
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number> ...");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = first;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentCopyConstructor) {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second{first};
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentConfusingLongDefault) {
  lector::RepeatableArgument<test::Label::ConfusingLong, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::ConfusingLong);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number> ...");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::OneHundred));
}

TEST(Lector, RepeatableArgumentConfusingLongMain) {
  lector::RepeatableArgument<test::Label::ConfusingLong, std::int32_t> argument{
    test::repeatable_argument_confusing_long()};
  EXPECT_EQ(argument.label(), test::Label::ConfusingLong);
  EXPECT_EQ(argument.keys(), test::keys_confusing_long());
  EXPECT_EQ(argument.description(), "Long confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key=200 <number>");
  EXPECT_EQ(argument.usage(), "[--key=200 <number>] ...");
  EXPECT_EQ(argument.options(), "--key=200 <number>  Long confusing argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::ConfusingLong);
  EXPECT_EQ(argument.keys(), test::keys_confusing_long());
  EXPECT_EQ(argument.description(), "Long confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key=200 <number>");
  EXPECT_EQ(argument.usage(), "[--key=200 <number>] ...");
  EXPECT_EQ(argument.options(), "--key=200 <number>  Long confusing argument.");
  EXPECT_EQ(argument.execution(), "--key=200 300 --key=200 400");
}

TEST(Lector, RepeatableArgumentConfusingShortDefault) {
  lector::RepeatableArgument<test::Label::ConfusingShort, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::ConfusingShort);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number> ...");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::OneHundred));
}

TEST(Lector, RepeatableArgumentConfusingShortMain) {
  lector::RepeatableArgument<test::Label::ConfusingShort, std::int32_t> argument{
    test::repeatable_argument_confusing_short()};
  EXPECT_EQ(argument.label(), test::Label::ConfusingShort);
  EXPECT_EQ(argument.keys(), test::keys_confusing_short());
  EXPECT_EQ(argument.description(), "Short confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key <number>");
  EXPECT_EQ(argument.usage(), "[--key <number>] ...");
  EXPECT_EQ(argument.options(), "--key <number>  Short confusing argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::ConfusingShort);
  EXPECT_EQ(argument.keys(), test::keys_confusing_short());
  EXPECT_EQ(argument.description(), "Short confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key <number>");
  EXPECT_EQ(argument.usage(), "[--key <number>] ...");
  EXPECT_EQ(argument.options(), "--key <number>  Short confusing argument.");
  EXPECT_EQ(argument.execution(), "--key 300 --key 400");
}

TEST(Lector, RepeatableArgumentDataStructureDefault) {
  lector::RepeatableArgument<test::Label::Point, test::Point> argument;
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::FirstPoint));
}

TEST(Lector, RepeatableArgumentDataStructureNamedOptional) {
  lector::RepeatableArgument<test::Label::Point, test::Point> argument{
    test::repeatable_argument_data_structure_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::FirstPoint);
  EXPECT_EQ(argument.default_values().at(1), test::SecondPoint);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::FirstPoint);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::SecondPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>] ...");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThirdPoint);
  argument.set_parsed_value(test::FourthPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::FirstPoint);
  EXPECT_EQ(argument.default_values().at(1), test::SecondPoint);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourthPoint);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourthPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>] ...");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_EQ(argument.execution(),
            "--point \"7.000000000 8.000000000 9.000000000\" "
            "--point \"10.00000000 11.00000000 12.00000000\"");
}

TEST(Lector, RepeatableArgumentDataStructureNamedRequired) {
  lector::RepeatableArgument<test::Label::Point, test::Point> argument{
    test::repeatable_argument_data_structure_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "--point <value> ...");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThirdPoint);
  argument.set_parsed_value(test::FourthPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourthPoint);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourthPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "--point <value> ...");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_EQ(argument.execution(),
            "--point \"7.000000000 8.000000000 9.000000000\" "
            "--point \"10.00000000 11.00000000 12.00000000\"");
}

TEST(Lector, RepeatableArgumentDataStructurePositionalOptional) {
  lector::RepeatableArgument<test::Label::Point, test::Point> argument{
    test::repeatable_argument_data_structure_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::FirstPoint);
  EXPECT_EQ(argument.default_values().at(1), test::SecondPoint);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::FirstPoint);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::SecondPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>] ...");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThirdPoint);
  argument.set_parsed_value(test::FourthPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::FirstPoint);
  EXPECT_EQ(argument.default_values().at(1), test::SecondPoint);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourthPoint);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourthPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>] ...");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_EQ(argument.execution(),
            "\"7.000000000 8.000000000 9.000000000\" \"10.00000000 11.00000000 12.00000000\"");
}

TEST(Lector, RepeatableArgumentDataStructurePositionalRequired) {
  lector::RepeatableArgument<test::Label::Point, test::Point> argument{
    test::repeatable_argument_data_structure_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThirdPoint);
  argument.set_parsed_value(test::FourthPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourthPoint);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThirdPoint);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourthPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_EQ(argument.execution(),
            "\"7.000000000 8.000000000 9.000000000\" \"10.00000000 11.00000000 12.00000000\"");
}

TEST(Lector, RepeatableArgumentEnumerationDefault) {
  lector::RepeatableArgument<test::Label::Shape, test::Shape> argument;
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::Shape::Square));
}

TEST(Lector, RepeatableArgumentEnumerationNamedOptional) {
  lector::RepeatableArgument<test::Label::Shape, test::Shape> argument{
    test::repeatable_argument_enumeration_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::Shape::Circle);
  EXPECT_EQ(argument.default_values().at(1), test::Shape::Triangle);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::Shape::Circle);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::Shape::Triangle);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>] ...");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  argument.set_parsed_value(test::Shape::Circle);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::Shape::Circle);
  EXPECT_EQ(argument.default_values().at(1), test::Shape::Triangle);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_values().at(1), test::Shape::Circle);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>] ...");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "--shape Square --shape Circle");
}

TEST(Lector, RepeatableArgumentEnumerationNamedRequired) {
  lector::RepeatableArgument<test::Label::Shape, test::Shape> argument{
    test::repeatable_argument_enumeration_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "--shape <value> ...");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  argument.set_parsed_value(test::Shape::Circle);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_values().at(1), test::Shape::Circle);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "--shape <value> ...");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "--shape Square --shape Circle");
}

TEST(Lector, RepeatableArgumentEnumerationPositionalOptional) {
  lector::RepeatableArgument<test::Label::Shape, test::Shape> argument{
    test::repeatable_argument_enumeration_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::Shape::Circle);
  EXPECT_EQ(argument.default_values().at(1), test::Shape::Triangle);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::Shape::Circle);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::Shape::Triangle);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>] ...");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  argument.set_parsed_value(test::Shape::Circle);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::Shape::Circle);
  EXPECT_EQ(argument.default_values().at(1), test::Shape::Triangle);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_values().at(1), test::Shape::Circle);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>] ...");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "Square Circle");
}

TEST(Lector, RepeatableArgumentEnumerationPositionalRequired) {
  lector::RepeatableArgument<test::Label::Shape, test::Shape> argument{
    test::repeatable_argument_enumeration_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  argument.set_parsed_value(test::Shape::Circle);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_values().at(1), test::Shape::Circle);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "Square Circle");
}

TEST(Lector, RepeatableArgumentFilesystemPathDefault) {
  lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path> argument;
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path> ...");
  EXPECT_EQ(argument.options(), "<path>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(std::filesystem::path{"/some/path"}));
}

TEST(Lector, RepeatableArgumentFilesystemPathNamedOptional) {
  lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::repeatable_argument_filesystem_path_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), std::filesystem::path{"/first/path"});
  EXPECT_EQ(argument.default_values().at(1), std::filesystem::path{"/second/path"});
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), std::filesystem::path{"/first/path"});
  EXPECT_EQ(argument.parsed_or_default_values().at(1), std::filesystem::path{"/second/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "[--output_directory <path>] ...");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(std::filesystem::path{"/third/path"});
  argument.set_parsed_value(std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), std::filesystem::path{"/first/path"});
  EXPECT_EQ(argument.default_values().at(1), std::filesystem::path{"/second/path"});
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_values().at(1), std::filesystem::path{"/fourth/path"});
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_or_default_values().at(1), std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "[--output_directory <path>] ...");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_EQ(argument.execution(), "--output_directory /third/path --output_directory /fourth/path");
}

TEST(Lector, RepeatableArgumentFilesystemPathNamedRequired) {
  lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::repeatable_argument_filesystem_path_named_required()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "--output_directory <path> ...");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(std::filesystem::path{"/third/path"});
  argument.set_parsed_value(std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_values().at(1), std::filesystem::path{"/fourth/path"});
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_or_default_values().at(1), std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "--output_directory <path> ...");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_EQ(argument.execution(), "--output_directory /third/path --output_directory /fourth/path");
}

TEST(Lector, RepeatableArgumentFilesystemPathPositionalOptional) {
  lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::repeatable_argument_filesystem_path_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), std::filesystem::path{"/first/path"});
  EXPECT_EQ(argument.default_values().at(1), std::filesystem::path{"/second/path"});
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), std::filesystem::path{"/first/path"});
  EXPECT_EQ(argument.parsed_or_default_values().at(1), std::filesystem::path{"/second/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "[<path>] ...");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(std::filesystem::path{"/third/path"});
  argument.set_parsed_value(std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), std::filesystem::path{"/first/path"});
  EXPECT_EQ(argument.default_values().at(1), std::filesystem::path{"/second/path"});
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_values().at(1), std::filesystem::path{"/fourth/path"});
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_or_default_values().at(1), std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "[<path>] ...");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_EQ(argument.execution(), "/third/path /fourth/path");
}

TEST(Lector, RepeatableArgumentFilesystemPathPositionalRequired) {
  lector::RepeatableArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::repeatable_argument_filesystem_path_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path> ...");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(std::filesystem::path{"/third/path"});
  argument.set_parsed_value(std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_values().at(1), std::filesystem::path{"/fourth/path"});
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), std::filesystem::path{"/third/path"});
  EXPECT_EQ(argument.parsed_or_default_values().at(1), std::filesystem::path{"/fourth/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path> ...");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_EQ(argument.execution(), "/third/path /fourth/path");
}

TEST(Lector, RepeatableArgumentFloatingPointNumberDefault) {
  lector::RepeatableArgument<test::Label::Tolerance, float> argument;
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::OneOverSixteen));
}

TEST(Lector, RepeatableArgumentFloatingPointNumberNamedOptional) {
  lector::RepeatableArgument<test::Label::Tolerance, float> argument{
    test::repeatable_argument_floating_point_number_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.default_values().at(1), test::OneOverSixtyFour);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::OneOverSixtyFour);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>] ...");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverEight);
  argument.set_parsed_value(test::OneOverSixteen);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.default_values().at(1), test::OneOverSixtyFour);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_values().at(1), test::OneOverSixteen);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::OneOverSixteen);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>] ...");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "--tolerance 0.1250000000 --tolerance 0.06250000000");
}

TEST(Lector, RepeatableArgumentFloatingPointNumberNamedRequired) {
  lector::RepeatableArgument<test::Label::Tolerance, float> argument{
    test::repeatable_argument_floating_point_number_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "--tolerance <value> ...");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverEight);
  argument.set_parsed_value(test::OneOverSixteen);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_values().at(1), test::OneOverSixteen);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::OneOverSixteen);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "--tolerance <value> ...");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "--tolerance 0.1250000000 --tolerance 0.06250000000");
}

TEST(Lector, RepeatableArgumentFloatingPointNumberPositionalOptional) {
  lector::RepeatableArgument<test::Label::Tolerance, float> argument{
    test::repeatable_argument_floating_point_number_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.default_values().at(1), test::OneOverSixtyFour);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::OneOverSixtyFour);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>] ...");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverEight);
  argument.set_parsed_value(test::OneOverSixteen);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.default_values().at(1), test::OneOverSixtyFour);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_values().at(1), test::OneOverSixteen);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::OneOverSixteen);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>] ...");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "0.1250000000 0.06250000000");
}

TEST(Lector, RepeatableArgumentFloatingPointNumberPositionalRequired) {
  lector::RepeatableArgument<test::Label::Tolerance, float> argument{
    test::repeatable_argument_floating_point_number_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverEight);
  argument.set_parsed_value(test::OneOverSixteen);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_values().at(1), test::OneOverSixteen);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneOverEight);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::OneOverSixteen);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value> ...");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "0.1250000000 0.06250000000");
}

TEST(Lector, RepeatableArgumentIntegerDefault) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number> ...");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::OneHundred));
}

TEST(Lector, RepeatableArgumentIntegerNamedOptional) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentIntegerNamedRequired) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "--iterations <number> ...");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "--iterations <number> ...");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentIntegerPositionalOptional) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>] ...");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>] ...");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "300 400");
}

TEST(Lector, RepeatableArgumentIntegerPositionalRequired) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number> ...");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number> ...");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "300 400");
}

TEST(Lector, RepeatableArgumentInvalidAllEmptyKeys) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_all_empty_keys());
}

TEST(Lector, RepeatableArgumentInvalidAnEmptyKey) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_an_empty_key());
}

TEST(Lector, RepeatableArgumentInvalidBooleanParsedFalse) {
  lector::RepeatableArgument<test::Label::Help, bool> argument{test::repeatable_argument_boolean()};
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), test::keys_boolean());
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help] ...");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(false));
}

TEST(Lector, RepeatableArgumentInvalidBooleanPositional) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_boolean_positional());
}

TEST(Lector, RepeatableArgumentInvalidBooleanWithDefaultValues) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_boolean_with_default_values());
}

TEST(Lector, RepeatableArgumentInvalidDuplicateKeys) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_duplicate_keys());
}

TEST(Lector, RepeatableArgumentInvalidEmptyDescription) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_empty_description());
}

TEST(Lector, RepeatableArgumentInvalidNoKeys) {
  EXPECT_ANY_THROW(test::repeatable_argument_invalid_no_keys());
}

TEST(Lector, RepeatableArgumentMoveAssignmentOperator) {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_FALSE(second.has_default());
  EXPECT_TRUE(second.default_values().empty());
  EXPECT_FALSE(second.has_parsed());
  EXPECT_TRUE(second.parsed_values().empty());
  EXPECT_TRUE(second.parsed_or_default_values().empty());
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number> ...");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = std::move(first);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentMoveConstructor) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second{std::move(first)};
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.has_parsed());
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>] ...");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentStringDefault) {
  lector::RepeatableArgument<test::Label::Title, std::string> argument;
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text> ...");
  EXPECT_EQ(argument.options(), "<text>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value("My Report"));
}

TEST(Lector, RepeatableArgumentStringNamedOptional) {
  lector::RepeatableArgument<test::Label::Title, std::string> argument{
    test::repeatable_argument_string_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), "My First Report");
  EXPECT_EQ(argument.default_values().at(1), "My Second Report");
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), "My First Report");
  EXPECT_EQ(argument.parsed_or_default_values().at(1), "My Second Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>] ...");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("My Third Report");
  argument.set_parsed_value("My Fourth Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), "My First Report");
  EXPECT_EQ(argument.default_values().at(1), "My Second Report");
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_values().at(1), "My Fourth Report");
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_or_default_values().at(1), "My Fourth Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>] ...");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_EQ(argument.execution(), "--title \"My Third Report\" --title \"My Fourth Report\"");
}

TEST(Lector, RepeatableArgumentStringNamedRequired) {
  lector::RepeatableArgument<test::Label::Title, std::string> argument{
    test::repeatable_argument_string_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "--title <text> ...");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("My Third Report");
  argument.set_parsed_value("My Fourth Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_values().at(1), "My Fourth Report");
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_or_default_values().at(1), "My Fourth Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "--title <text> ...");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_EQ(argument.execution(), "--title \"My Third Report\" --title \"My Fourth Report\"");
}

TEST(Lector, RepeatableArgumentStringPositionalOptional) {
  lector::RepeatableArgument<test::Label::Title, std::string> argument{
    test::repeatable_argument_string_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), "My First Report");
  EXPECT_EQ(argument.default_values().at(1), "My Second Report");
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), "My First Report");
  EXPECT_EQ(argument.parsed_or_default_values().at(1), "My Second Report");
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "[<text>] ...");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("My Third Report");
  argument.set_parsed_value("My Fourth Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), "My First Report");
  EXPECT_EQ(argument.default_values().at(1), "My Second Report");
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_values().at(1), "My Fourth Report");
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_or_default_values().at(1), "My Fourth Report");
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "[<text>] ...");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_EQ(argument.execution(), "\"My Third Report\" \"My Fourth Report\"");
}

TEST(Lector, RepeatableArgumentStringPositionalRequired) {
  lector::RepeatableArgument<test::Label::Title, std::string> argument{
    test::repeatable_argument_string_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text> ...");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("My Third Report");
  argument.set_parsed_value("My Fourth Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_values().at(1), "My Fourth Report");
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), "My Third Report");
  EXPECT_EQ(argument.parsed_or_default_values().at(1), "My Fourth Report");
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text> ...");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_EQ(argument.execution(), "\"My Third Report\" \"My Fourth Report\"");
}

TEST(Lector, RepeatableArgumentWeirdKeysDefault) {
  lector::RepeatableArgument<test::Label::Weird, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number> ...");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(test::OneHundred));
}

TEST(Lector, RepeatableArgumentWeirdKeysOptional) {
  lector::RepeatableArgument<test::Label::Weird, std::int32_t> argument{
    test::repeatable_argument_weird_keys_optional()};
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "[==weird=key <number>] ...");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "[==weird=key <number>] ...");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_EQ(argument.execution(), "==weird=key 300 ==weird=key 400");
}

TEST(Lector, RepeatableArgumentWeirdKeysRequired) {
  lector::RepeatableArgument<test::Label::Weird, std::int32_t> argument{
    test::repeatable_argument_weird_keys_required()};
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "==weird=key <number> ...");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.has_parsed());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "==weird=key <number> ...");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_EQ(argument.execution(), "==weird=key 300 ==weird=key 400");
}

TEST(Lector, SingularArgumentBooleanDefault) {
  lector::SingularArgument<test::Label::Help, bool> argument;
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_type().empty());
  EXPECT_TRUE(argument.usage().empty());
  EXPECT_TRUE(argument.options().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(true));
}

TEST(Lector, SingularArgumentBooleanNamedOptional) {
  lector::SingularArgument<test::Label::Help, bool> argument{test::singular_argument_boolean()};
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), test::keys_boolean());
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value() && !argument.default_value().value());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_FALSE(argument.parsed_or_default_value());
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help]");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(true);
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), test::keys_boolean());
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value() && !argument.default_value().value());
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value() && argument.parsed_value().value());
  EXPECT_TRUE(argument.parsed_or_default_value());
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help]");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_EQ(argument.execution(), "--help");
}

TEST(Lector, SingularArgumentConfusingLongDefault) {
  lector::SingularArgument<test::Label::ConfusingLong, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::ConfusingLong);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::TwoHundred));
}

TEST(Lector, SingularArgumentConfusingLongMain) {
  lector::SingularArgument<test::Label::ConfusingLong, std::int32_t> argument{
    test::singular_argument_confusing_long()};
  EXPECT_EQ(argument.label(), test::Label::ConfusingLong);
  EXPECT_EQ(argument.keys(), test::keys_confusing_long());
  EXPECT_EQ(argument.description(), "Long confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key=200 <number>");
  EXPECT_EQ(argument.usage(), "[--key=200 <number>]");
  EXPECT_EQ(argument.options(), "--key=200 <number>  Long confusing argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(argument.label(), test::Label::ConfusingLong);
  EXPECT_EQ(argument.keys(), test::keys_confusing_long());
  EXPECT_EQ(argument.description(), "Long confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key=200 <number>");
  EXPECT_EQ(argument.usage(), "[--key=200 <number>]");
  EXPECT_EQ(argument.options(), "--key=200 <number>  Long confusing argument.");
  EXPECT_EQ(argument.execution(), "--key=200 200");
}

TEST(Lector, SingularArgumentConfusingShortDefault) {
  lector::SingularArgument<test::Label::ConfusingShort, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::ConfusingShort);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::TwoHundred));
}

TEST(Lector, SingularArgumentConfusingShortMain) {
  lector::SingularArgument<test::Label::ConfusingShort, std::int32_t> argument{
    test::singular_argument_confusing_short()};
  EXPECT_EQ(argument.label(), test::Label::ConfusingShort);
  EXPECT_EQ(argument.keys(), test::keys_confusing_short());
  EXPECT_EQ(argument.description(), "Short confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key <number>");
  EXPECT_EQ(argument.usage(), "[--key <number>]");
  EXPECT_EQ(argument.options(), "--key <number>  Short confusing argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(argument.label(), test::Label::ConfusingShort);
  EXPECT_EQ(argument.keys(), test::keys_confusing_short());
  EXPECT_EQ(argument.description(), "Short confusing argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "--key <number>");
  EXPECT_EQ(argument.usage(), "[--key <number>]");
  EXPECT_EQ(argument.options(), "--key <number>  Short confusing argument.");
  EXPECT_EQ(argument.execution(), "--key 200");
}

TEST(Lector, SingularArgumentCopyAssignmentOperator) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  EXPECT_TRUE(
      first.default_value().has_value() && first.default_value().value() == test::OneHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_EQ(first.parsed_value(), std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::SingularArgument<test::Label::Iterations, std::int32_t> second;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_FALSE(second.has_default());
  EXPECT_EQ(second.default_value(), std::nullopt);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number>");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = first;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_TRUE(second.has_parsed());
  EXPECT_TRUE(
      second.parsed_value().has_value() && second.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentCopyConstructor) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  EXPECT_TRUE(
      first.default_value().has_value() && first.default_value().value() == test::OneHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_EQ(first.parsed_value(), std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::SingularArgument<test::Label::Iterations, std::int32_t> second{first};
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_TRUE(second.has_parsed());
  EXPECT_TRUE(
      second.parsed_value().has_value() && second.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentDataStructureDefault) {
  lector::SingularArgument<test::Label::Point, test::Point> argument;
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::SecondPoint));
}

TEST(Lector, SingularArgumentDataStructureNamedOptional) {
  lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::FirstPoint);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::FirstPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>]");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::SecondPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::FirstPoint);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::SecondPoint);
  EXPECT_EQ(argument.parsed_or_default_value(), test::SecondPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>]");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_EQ(argument.execution(), "--point \"4.000000000 5.000000000 6.000000000\"");
}

TEST(Lector, SingularArgumentDataStructureNamedRequired) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), test::keys_data_structure());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "--point <value>");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentDataStructurePositionalOptional) {
  lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::FirstPoint);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::FirstPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::SecondPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::FirstPoint);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::SecondPoint);
  EXPECT_EQ(argument.parsed_or_default_value(), test::SecondPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_EQ(argument.execution(), "\"4.000000000 5.000000000 6.000000000\"");
}

TEST(Lector, SingularArgumentDataStructurePositionalRequired) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentEnumerationDefault) {
  lector::SingularArgument<test::Label::Shape, test::Shape> argument;
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::Shape::Square));
}

TEST(Lector, SingularArgumentEnumerationNamedOptional) {
  lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::Shape::Circle);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>]");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::Shape::Circle);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value()
              && argument.parsed_value().value() == test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Square);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>]");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "--shape Square");
}

TEST(Lector, SingularArgumentEnumerationNamedRequired) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), test::keys_enumeration());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "--shape <value>");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentEnumerationPositionalOptional) {
  lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::Shape::Circle);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::Shape::Circle);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value()
              && argument.parsed_value().value() == test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Square);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "Square");
}

TEST(Lector, SingularArgumentEnumerationPositionalRequired) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFilesystemPathDefault) {
  lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument;
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path>");
  EXPECT_EQ(argument.options(), "<path>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(std::filesystem::path("/some/other/path")));
}

TEST(Lector, SingularArgumentFilesystemPathNamedOptional) {
  lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == std::filesystem::path("/some/path"));
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "[--output_directory <path>]");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(std::filesystem::path{"/some/other/path"});
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == std::filesystem::path("/some/path"));
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value()
              && argument.parsed_value().value() == std::filesystem::path("/some/other/path"));
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path("/some/other/path"));
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "[--output_directory <path>]");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_EQ(argument.execution(), "--output_directory /some/other/path");
}

TEST(Lector, SingularArgumentFilesystemPathNamedRequired) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_named_required()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), test::keys_filesystem_path());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output_directory <path>");
  EXPECT_EQ(argument.usage(), "--output_directory <path>");
  EXPECT_EQ(argument.options(), "-o <path>, --output_directory <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFilesystemPathPositionalOptional) {
  lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == std::filesystem::path("/some/path"));
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "[<path>]");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(std::filesystem::path{"/some/other/path"});
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == std::filesystem::path("/some/path"));
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value()
              && argument.parsed_value().value() == std::filesystem::path("/some/other/path"));
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path("/some/other/path"));
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "[<path>]");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_EQ(argument.execution(), "/some/other/path");
}

TEST(Lector, SingularArgumentFilesystemPathPositionalRequired) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path>");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFloatingPointNumberDefault) {
  lector::SingularArgument<test::Label::Tolerance, float> argument;
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::OneOverSixtyFour));
}

TEST(Lector, SingularArgumentFloatingPointNumberNamedOptional) {
  lector::SingularArgument<test::Label::Tolerance, float> argument{
    test::singular_argument_floating_point_number_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::OneOverThirtyTwo);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>]");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverSixtyFour);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::OneOverThirtyTwo);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value()
              && argument.parsed_value().value() == test::OneOverSixtyFour);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverSixtyFour);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>]");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "--tolerance 0.01562500000");
}

TEST(Lector, SingularArgumentFloatingPointNumberNamedRequired) {
  const lector::SingularArgument<test::Label::Tolerance, float> argument{
    test::singular_argument_floating_point_number_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), test::keys_floating_point_number());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "--tolerance <value>");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFloatingPointNumberPositionalOptional) {
  lector::SingularArgument<test::Label::Tolerance, float> argument{
    test::singular_argument_floating_point_number_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::OneOverThirtyTwo);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverSixtyFour);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value()
              && argument.default_value().value() == test::OneOverThirtyTwo);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(argument.parsed_value().has_value()
              && argument.parsed_value().value() == test::OneOverSixtyFour);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverSixtyFour);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "0.01562500000");
}

TEST(Lector, SingularArgumentFloatingPointNumberPositionalRequired) {
  const lector::SingularArgument<test::Label::Tolerance, float> argument{
    test::singular_argument_floating_point_number_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentIntegerDefault) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::TwoHundred));
}

TEST(Lector, SingularArgumentIntegerNamedOptional) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>]");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>]");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentIntegerNamedRequired) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "--iterations <number>");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentIntegerPositionalOptional) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>]");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>]");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "200");
}

TEST(Lector, SingularArgumentIntegerPositionalRequired) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentInvalidAllEmptyKeys) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_all_empty_keys());
}

TEST(Lector, SingularArgumentInvalidAnEmptyKey) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_an_empty_key());
}

TEST(Lector, SingularArgumentInvalidBooleanParsedFalse) {
  lector::SingularArgument<test::Label::Help, bool> argument{test::singular_argument_boolean()};
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), test::keys_boolean());
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(argument.default_value().has_value() && !argument.default_value().value());
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_FALSE(argument.parsed_or_default_value());
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help]");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(argument.set_parsed_value(false));
}

TEST(Lector, SingularArgumentInvalidBooleanPositional) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_boolean_positional());
}

TEST(Lector, SingularArgumentInvalidBooleanWithDefaultValue) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_boolean_with_default_value());
}

TEST(Lector, SingularArgumentInvalidDuplicateKeys) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_duplicate_keys());
}

TEST(Lector, SingularArgumentInvalidEmptyDescription) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_empty_description());
}

TEST(Lector, SingularArgumentInvalidNoKeys) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_no_keys());
}

TEST(Lector, SingularArgumentMoveAssignmentOperator) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  EXPECT_TRUE(
      first.default_value().has_value() && first.default_value().value() == test::OneHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_EQ(first.parsed_value(), std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::SingularArgument<test::Label::Iterations, std::int32_t> second;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_FALSE(second.has_default());
  EXPECT_EQ(second.default_value(), std::nullopt);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number>");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = std::move(first);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_TRUE(second.has_parsed());
  EXPECT_TRUE(
      second.parsed_value().has_value() && second.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentMoveConstructor) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_named_optional()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_TRUE(first.has_default());
  EXPECT_TRUE(
      first.default_value().has_value() && first.default_value().value() == test::OneHundred);
  EXPECT_FALSE(first.has_parsed());
  EXPECT_EQ(first.parsed_value(), std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::SingularArgument<test::Label::Iterations, std::int32_t> second{std::move(first)};
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_FALSE(second.has_parsed());
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_TRUE(second.has_default());
  EXPECT_TRUE(
      second.default_value().has_value() && second.default_value().value() == test::OneHundred);
  EXPECT_TRUE(second.has_parsed());
  EXPECT_TRUE(
      second.parsed_value().has_value() && second.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentStringDefault) {
  lector::SingularArgument<test::Label::Title, std::string> argument;
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text>");
  EXPECT_EQ(argument.options(), "<text>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value("My Other Title"));
}

TEST(Lector, SingularArgumentStringNamedOptional) {
  lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_named_optional()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == "My Report");
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>]");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("My Other Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == "My Report");
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == "My Other Report");
  EXPECT_EQ(argument.parsed_or_default_value(), "My Other Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>]");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_EQ(argument.execution(), "--title \"My Other Report\"");
}

TEST(Lector, SingularArgumentStringNamedRequired) {
  const lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_named_required()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), test::keys_string());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "--title <text>");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentStringPositionalOptional) {
  lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_positional_optional()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == "My Report");
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "[<text>]");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("My Other Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == "My Report");
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == "My Other Report");
  EXPECT_EQ(argument.parsed_or_default_value(), "My Other Report");
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "[<text>]");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_EQ(argument.execution(), "\"My Other Report\"");
}

TEST(Lector, SingularArgumentStringPositionalRequired) {
  const lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_positional_required()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text>");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentWeirdKeysDefault) {
  lector::SingularArgument<test::Label::Weird, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
  EXPECT_ANY_THROW(argument.set_parsed_value(test::TwoHundred));
}

TEST(Lector, SingularArgumentWeirdKeysOptional) {
  lector::SingularArgument<test::Label::Weird, std::int32_t> argument{
    test::singular_argument_weird_keys_optional()};
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "[==weird=key <number>]");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::TwoHundred);
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_TRUE(argument.has_default());
  EXPECT_TRUE(
      argument.default_value().has_value() && argument.default_value().value() == test::OneHundred);
  EXPECT_TRUE(argument.has_parsed());
  EXPECT_TRUE(
      argument.parsed_value().has_value() && argument.parsed_value().value() == test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "[==weird=key <number>]");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_EQ(argument.execution(), "==weird=key 200");
}

TEST(Lector, SingularArgumentWeirdKeysRequired) {
  const lector::SingularArgument<test::Label::Weird, std::int32_t> argument{
    test::singular_argument_weird_keys_required()};
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_EQ(argument.keys(), test::keys_weird());
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_FALSE(argument.has_default());
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_FALSE(argument.has_parsed());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "==weird=key <number>");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, TutorialSection1Basic) {
  lector::Arguments arguments{
    lector::Configuration{{"My Application"},
                          {"Description of my application."},
                          {"Additional notes about my application."}},
    lector::RepeatableArgument<test::Label::Tolerance, float>(
        "List of tolerances. Optional. Default 0.125.", std::vector<float>{0.125F}
    ),
    lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
                          {"-o", "--output_directory"}, "Output directory. Required."},
    lector::SingularArgument<test::Label::Iterations, std::int32_t>(
        {"-i", "--iterations"},
    "Number of iterations. Optional. Default 100.", 100),
    lector::SingularArgument<test::Label::Help, bool>(
        {"-h", "--help"},
    "Display this help information and exit. Optional.")
  };
  const test::Command command{
    {"/path/to/executable", "0.25", "0.5", "-o", "/path/to/directory", "-i", "200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  if (arguments.get<test::Label::Help>().parsed_or_default_value()) {
    std::cout << arguments.help() << std::endl;
  } else {
    std::cout << "Execution:" << std::endl << arguments.execution() << std::endl;
    const std::vector<float>& tolerances{
      arguments.get<test::Label::Tolerance>().parsed_or_default_values()};
    std::cout << "The tolerances are:";
    for (const float tolerance : tolerances) {
      std::cout << " " << tolerance;
    }
    std::cout << std::endl;
    const std::filesystem::path& output_directory_path{
      arguments.get<test::Label::OutputDirectory>().parsed_or_default_value()};
    std::cout << "The output directory is: " << output_directory_path << std::endl;
    const std::int32_t iterations_count{
      arguments.get<test::Label::Iterations>().parsed_or_default_value()};
    std::cout << "The number of iterations is: " << iterations_count << std::endl;
  }
  EXPECT_FALSE(arguments.get<test::Label::Help>().parsed_or_default_value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.configuration().title, "My Application");
  EXPECT_EQ(arguments.configuration().description, "Description of my application.");
  EXPECT_EQ(arguments.configuration().notes, "Additional notes about my application.");
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               List of tolerances. Optional. Default 0.125.\n"
    "-o <path>, --output_directory <path>  Output directory. Required.\n"
    "-i <number>, --iterations <number>    Number of iterations. Optional. Default 100.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "Description of my application.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes about my application.");
  EXPECT_EQ(
      arguments.execution(),
      "/path/to/executable 0.2500000000 0.5000000000 --output_directory /path/to/directory "
      "--iterations 200");
}

TEST(Lector, TutorialSection1Help) {
  lector::Arguments arguments{
    lector::Configuration{{"My Application"},
                          {"Description of my application."},
                          {"Additional notes about my application."}},
    lector::RepeatableArgument<test::Label::Tolerance, float>(
        "List of tolerances. Optional. Default 0.125.", std::vector<float>{0.125F}
    ),
    lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
                          {"-o", "--output_directory"}, "Output directory. Required."},
    lector::SingularArgument<test::Label::Iterations, std::int32_t>(
        {"-i", "--iterations"},
    "Number of iterations. Optional. Default 100.", 100),
    lector::SingularArgument<test::Label::Help, bool>(
        {"-h", "--help"},
    "Display this help information and exit. Optional.")
  };
  const test::Command command{
    {"/path/to/executable", "0.25", "0.5", "-o", "/path/to/directory", "-i", "200", "-h"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  if (arguments.get<test::Label::Help>().parsed_or_default_value()) {
    std::cout << arguments.help() << std::endl;
  } else {
    std::cout << "Execution:" << std::endl << arguments.execution() << std::endl;
    const std::vector<float>& tolerances{
      arguments.get<test::Label::Tolerance>().parsed_or_default_values()};
    std::cout << "The tolerances are:";
    for (const float tolerance : tolerances) {
      std::cout << " " << tolerance;
    }
    std::cout << std::endl;
    const std::filesystem::path& output_directory_path{
      arguments.get<test::Label::OutputDirectory>().parsed_or_default_value()};
    std::cout << "The output directory is: " << output_directory_path << std::endl;
    const std::int32_t iterations_count{
      arguments.get<test::Label::Iterations>().parsed_or_default_value()};
    std::cout << "The number of iterations is: " << iterations_count << std::endl;
  }
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_or_default_value());
  const std::string expected_usage{
    "executable [<value>] ... --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.configuration().title, "My Application");
  EXPECT_EQ(arguments.configuration().description, "Description of my application.");
  EXPECT_EQ(arguments.configuration().notes, "Additional notes about my application.");
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                               List of tolerances. Optional. Default 0.125.\n"
    "-o <path>, --output_directory <path>  Output directory. Required.\n"
    "-i <number>, --iterations <number>    Number of iterations. Optional. Default 100.\n"
    "-h, --help                            Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "Description of my application.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes about my application.");
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable 0.2500000000 0.5000000000 --output_directory /path/to/directory "
            "--iterations 200 --help");
}

TEST(Lector, TutorialSection3Subsection2) {
  lector::Arguments arguments{
    lector::Configuration{{"My Application"},
                          {"Description of my application."},
                          {"Additional notes about my application."}},
    lector::RepeatableArgument<test::Label::Tolerance, float>(
        "List of tolerances. Optional. Default 0.125.", std::vector<float>{0.125F}
    ),
    lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
                          {"o", "=o", "__out_dir__"}, "Output directory. Required."},
    lector::SingularArgument<test::Label::Iterations, std::int32_t>(
        {"=i=", "_it_", "==iter=="},
    "Number of iterations. Optional. Default 100.", 100)
  };
  const test::Command command{
    {"/path/to/executable", "0.25", "0.5", "__out_dir__", "/path/to/directory", "=i==200"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  const std::string expected_usage{
    "executable [<value>] ... __out_dir__ <path> [==iter== <number>]"};
  EXPECT_EQ(arguments.configuration().title, "My Application");
  EXPECT_EQ(arguments.configuration().description, "Description of my application.");
  EXPECT_EQ(arguments.configuration().notes, "Additional notes about my application.");
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "<value>                                         List of tolerances. Optional. Default 0.125.\n"
    "o <path>, =o <path>, __out_dir__ <path>         Output directory. Required.\n"
    "=i= <number>, _it_ <number>, ==iter== <number>  Number of iterations. Optional. Default 100."};
  EXPECT_EQ(arguments.options(), expected_options);
  EXPECT_EQ(
    arguments.help(),
    "My Application\n\n"
    "Usage:\n" +
    expected_usage + "\n\n"
    "Description of my application.\n\n"
    "Options:\n" +
    expected_options + "\n\n"
    "Additional notes about my application.");
  EXPECT_EQ(
      arguments.execution(),
      "/path/to/executable 0.2500000000 0.5000000000 __out_dir__ /path/to/directory "
      "==iter== 200");
}

TEST(Lector, TutorialSection3Subsection3) {
  const std::string printed_triangle{lector::print(test::Shape::Triangle)};
  EXPECT_EQ(printed_triangle, "Triangle");
  const std::optional<test::Shape> parsed_triangle{lector::parse<test::Shape>("TRIANGLE")};
  EXPECT_TRUE(parsed_triangle.has_value() && parsed_triangle.value() == test::Shape::Triangle);
  const std::optional<test::Shape> invalid_shape{lector::parse<test::Shape>("Invalid Shape")};
  EXPECT_TRUE(!invalid_shape.has_value());
  lector::Arguments arguments{lector::SingularArgument<test::Label::Shape, test::Shape>(
      {"-s", "--shape"}, "Your favorite shape. Optional.", test::Shape::Circle)};
  const test::Command command{
    {"/path/to/executable", "--shape", "square"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  const test::Shape shape{arguments.get<test::Label::Shape>().parsed_or_default_value()};
  std::cout << "Your favorite shape is: " << lector::print(shape) << std::endl;
  EXPECT_EQ(shape, test::Shape::Square);
}

TEST(Lector, TutorialSection3Subsection4) {
  const std::string printed_point{lector::print(test::Point{1.0, 2.0, 3.0})};
  EXPECT_EQ(printed_point, "1.000000000 2.000000000 3.000000000");
  const std::optional<test::Point> parsed_point{lector::parse<test::Point>("4.0 5.0 6.0")};
  const test::Point expected_point{4.0, 5.0, 6.0};
  EXPECT_TRUE(
      parsed_point.has_value() && parsed_point.value().x == expected_point.x
      && parsed_point.value().y == expected_point.y && parsed_point.value().z == expected_point.z);
  lector::Arguments arguments{lector::SingularArgument<test::Label::Point, test::Point>(
      {"-p", "--point"}, "Your favorite point. Optional.", test::Point{})};
  const test::Command command{
    {"/path/to/executable", "--point", "4.0 5.0 6.0"}
  };
  arguments.parse(command.argc(), command.argv());
  arguments.validate();
  const test::Point point{arguments.get<test::Label::Point>().parsed_or_default_value()};
  std::cout << "Your favorite point is: " << point << std::endl;
  EXPECT_TRUE(
      point.x == expected_point.x && point.y == expected_point.y && point.z == expected_point.z);
}

}  // namespace
