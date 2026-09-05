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
#include <initializer_list>
#include <iostream>
#include <optional>
#include <sstream>
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
  output_stream << point.x << " " << point.y << " " << point.z;
  return output_stream;
}

/// @brief Default point in three-dimensional space.
inline constexpr test::Point FirstPoint{1.0F, 2.0F, 3.0F};

/// @brief Another point in three-dimensional space. Different from the default point.
inline constexpr test::Point SecondPoint{4.0F, 5.0F, 6.0F};

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
  Title,
  OutputDirectory,
  Shape,
  Point,
  Iterations,
  Tolerance,
  ConfusingShort,
  ConfusingLong,
  IterationsAgain,
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

/// @brief The fraction 1/32.
constexpr double OneOverThirtyTwo{0.03125};

/// @brief The fraction 1/64.
constexpr double OneOverSixtyFour{0.015625};

lector::Configuration configuration() {
  lector::Configuration configuration{
    "My Application", "An application for testing the Lector library.",
    "Additional notes for the application for testing the lector library."};
  return configuration;
};

std::vector<std::string> keys_integer() {
  return std::vector<std::string>{"-i", "--iterations"};
}

lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_optional_named() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations.",
    std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_optional_positional() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{
    "Number of iterations.", std::vector<std::int32_t>{test::OneHundred, test::TwoHundred}
  };
}

lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_required_named() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations."};
}

lector::RepeatableArgument<test::Label::Iterations, std::int32_t>
repeatable_argument_integer_required_positional() {
  return lector::RepeatableArgument<test::Label::Iterations, std::int32_t>{"Number of iterations."};
}

lector::SingularArgument<test::Label::Help, bool> singular_argument_boolean_named() {
  return lector::SingularArgument<test::Label::Help, bool>{
    std::vector<std::string>{"-h", "--help"},
    "Display this help information and exit. Optional."
  };
}

lector::SingularArgument<test::Label::Help, bool> singular_argument_boolean_positional() {
  return lector::SingularArgument<test::Label::Help, bool>{
    "Display this help information and exit. Optional."};
}

lector::SingularArgument<test::Label::ConfusingLong, std::int32_t>
singular_argument_confusing_long() {
  return lector::SingularArgument<test::Label::ConfusingLong, std::int32_t>{
    std::vector<std::string>{"--key=200"}, "Long confusing argument.", test::OneHundred};
}

lector::SingularArgument<test::Label::ConfusingShort, std::int32_t>
singular_argument_confusing_short() {
  return lector::SingularArgument<test::Label::ConfusingShort, std::int32_t>{
    std::vector<std::string>{"--key"}, "Short confusing argument.", test::OneHundred};
}

lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_optional_named() {
  return lector::SingularArgument<test::Label::Point, test::Point>{
    std::vector<std::string>{"-p", "--point"},
    "Starting point.", test::FirstPoint
  };
}

lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_optional_positional() {
  return lector::SingularArgument<test::Label::Point, test::Point>{
    "Starting point.", test::FirstPoint};
}

lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_required_named() {
  return lector::SingularArgument<test::Label::Point, test::Point>{
    std::vector<std::string>{"-p", "--point"},
    "Starting point."
  };
}

lector::SingularArgument<test::Label::Point, test::Point>
singular_argument_data_structure_required_positional() {
  return lector::SingularArgument<test::Label::Point, test::Point>{"Starting point."};
}

lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_optional_named() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{
    std::vector<std::string>{"-s", "--shape"},
    "Favorite shape.", test::Shape::Circle
  };
}

lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_optional_positional() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{
    "Favorite shape.", test::Shape::Circle};
}

lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_required_named() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{
    std::vector<std::string>{"-s", "--shape"},
    "Favorite shape."
  };
}

lector::SingularArgument<test::Label::Shape, test::Shape>
singular_argument_enumeration_required_positional() {
  return lector::SingularArgument<test::Label::Shape, test::Shape>{"Favorite shape."};
}

lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_optional_named() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    std::vector<std::string>{"-o", "--output"},
    "Output directory.",
    std::filesystem::path("/some/path")
  };
}

lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_optional_positional() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    "Output directory.", std::filesystem::path("/some/path")};
}

lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_required_named() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    std::vector<std::string>{"-o", "--output"},
    "Output directory."
  };
}

lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>
singular_argument_filesystem_path_required_positional() {
  return lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
    "Output directory."};
}

lector::SingularArgument<test::Label::Tolerance, double>
singular_argument_floating_point_number_optional_named() {
  return lector::SingularArgument<test::Label::Tolerance, double>{
    std::vector<std::string>{"-t", "--tolerance"},
    "Tolerance value.", test::OneOverThirtyTwo
  };
}

lector::SingularArgument<test::Label::Tolerance, double>
singular_argument_floating_point_number_optional_positional() {
  return lector::SingularArgument<test::Label::Tolerance, double>{
    "Tolerance value.", test::OneOverThirtyTwo};
}

lector::SingularArgument<test::Label::Tolerance, double>
singular_argument_floating_point_number_required_named() {
  return lector::SingularArgument<test::Label::Tolerance, double>{
    std::vector<std::string>{"-t", "--tolerance"},
    "Tolerance value."
  };
}

lector::SingularArgument<test::Label::Tolerance, double>
singular_argument_floating_point_number_required_positional() {
  return lector::SingularArgument<test::Label::Tolerance, double>{"Tolerance value."};
}

lector::SingularArgument<test::Label::IterationsAgain, std::int32_t>
singular_argument_integer_again_invalid() {
  return lector::SingularArgument<test::Label::IterationsAgain, std::int32_t>{
    std::vector<std::string>{"-it", "--iterations"},
    "Number of iterations, again.",
    test::OneHundred
  };
}

lector::SingularArgument<test::Label::IterationsAgain, std::int32_t>
singular_argument_integer_again_valid() {
  return lector::SingularArgument<test::Label::IterationsAgain, std::int32_t>{
    std::vector<std::string>{"-it", "--iter"},
    "Number of iterations, again.", test::OneHundred
  };
}

lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_optional_named() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations.", test::OneHundred};
}

lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_optional_positional() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{
    "Number of iterations.", test::OneHundred};
}

lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_required_named() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{
    test::keys_integer(), "Number of iterations."};
}

lector::SingularArgument<test::Label::Iterations, std::int32_t>
singular_argument_integer_required_positional() {
  return lector::SingularArgument<test::Label::Iterations, std::int32_t>{"Number of iterations."};
}

/// @brief Helper function that creates an invalid lector::SingularArgument with all empty keys.
void singular_argument_invalid_all_empty_keys() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid lector::SingularArgument with an empty key.
void singular_argument_invalid_an_empty_key() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"-i", "--iterations", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid lector::SingularArgument with a boolean type and
/// a default value.
void singular_argument_invalid_boolean_with_default_value() {
  const lector::SingularArgument<test::Label::Help, bool> argument{
    std::vector<std::string>{"-h", "--help"},
    "Display this help information and exit. Optional.",
    true,
  };
}

/// @brief Helper function that creates an invalid lector::SingularArgument with duplicate keys.
void singular_argument_invalid_duplicate_keys() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{"-i", "--iterations", "-i"},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid lector::SingularArgument with an empty
/// description.
void singular_argument_invalid_empty_description() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::keys_integer(), ""};
}

/// @brief Helper function that creates an invalid lector::SingularArgument with no keys.
void singular_argument_invalid_no_keys() {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    std::vector<std::string>{}, "Number of iterations."};
}

lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_optional_named() {
  return lector::SingularArgument<test::Label::Title, std::string>{
    std::vector<std::string>{"-t", "--title"},
    "Report title.", "My Report"
  };
}

lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_optional_positional() {
  return lector::SingularArgument<test::Label::Title, std::string>{"Report title.", "My Report"};
}

lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_required_named() {
  return lector::SingularArgument<test::Label::Title, std::string>{
    std::vector<std::string>{"-t", "--title"},
    "Report title."
  };
}

lector::SingularArgument<test::Label::Title, std::string>
singular_argument_string_required_positional() {
  return lector::SingularArgument<test::Label::Title, std::string>{"Report title."};
}

lector::SingularArgument<test::Label::Weird, std::int32_t>
singular_argument_weird_keys_optional_named() {
  return lector::SingularArgument<test::Label::Weird, std::int32_t>{
    std::vector<std::string>{"=w=k", "==weird=key"},
    "Weird argument.", test::OneHundred
  };
}

lector::SingularArgument<test::Label::Weird, std::int32_t>
singular_argument_weird_keys_required_named() {
  return lector::SingularArgument<test::Label::Weird, std::int32_t>{
    std::vector<std::string>{"=w=k", "==weird=key"},
    "Weird argument."
  };
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
  explicit Command(std::initializer_list<std::string> arguments)
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

  /// @brief Copy constructor. Deleted to prevent double-free errors when a Command object is
  /// copied, since it manages a dynamically allocated array of C-strings.
  Command(const test::Command&) = delete;

  /// @brief Copy assignment operator. Deleted to prevent double-free errors when a Command object
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

TEST(Lector, ArgumentsDuplicatedArgumentInlineNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--iterations=200", "--iterations=300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--iterations=200", "--iterations=300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentMixedNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--iterations", "200", "--iterations=300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentMixedWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--iterations", "200", "--iterations=300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentWhitespaceNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--iterations", "200", "--iterations", "300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--iterations", "200", "--iterations", "300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicateKeysNoConfiguration) {
  EXPECT_ANY_THROW(lector::Arguments(test::singular_argument_integer_optional_named(),
                                     test::singular_argument_integer_again_invalid()));
}

TEST(Lector, ArgumentsDuplicateKeysWithConfiguration) {
  EXPECT_ANY_THROW(
      lector::Arguments(test::configuration(), test::singular_argument_integer_optional_named(),
                        test::singular_argument_integer_again_invalid()));
}

TEST(Lector, ArgumentsEmptyExecutableOnlyNoConfiguration) {
  lector::Arguments arguments;
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::string expected_usage{"executable"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  EXPECT_TRUE(arguments.options().empty());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsEmptyExecutableOnlyWithConfiguration) {
  lector::Arguments arguments{test::configuration()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::string expected_usage{"executable"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  EXPECT_TRUE(arguments.options().empty());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsEmptyNoExecutableNoConfiguration) {
  lector::Arguments arguments;
  const test::Command command;
  arguments.parse(command.argc(), command.argv());
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.usage().empty());
  EXPECT_TRUE(arguments.options().empty());
  EXPECT_TRUE(arguments.help().empty());
  EXPECT_TRUE(arguments.execution().empty());
}

TEST(Lector, ArgumentsEmptyNoExecutableWithConfiguration) {
  lector::Arguments arguments{test::configuration()};
  const test::Command command;
  arguments.parse(command.argc(), command.argv());
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.usage().empty());
  EXPECT_TRUE(arguments.options().empty());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_TRUE(arguments.execution().empty());
}

TEST(Lector, ArgumentsExtraTokenNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_positional(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "200", "300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsExtraTokenWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_positional(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "200", "300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentNamedInlineNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations=Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentNamedInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations=Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentNamedWhitespaceNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations", "Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentNamedWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations", "Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentPositionalNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_positional(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentPositionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_positional(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_required_named(),
                              test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentOptionalInlineNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_required_named(),
                              test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--output=/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_ouput_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_ouput_directory.has_value()
              && parsed_ouput_directory.value() == std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsMissingArgumentOptionalInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--output=/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_ouput_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_ouput_directory.has_value()
              && parsed_ouput_directory.value() == std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsMissingArgumentOptionalWhitespaceNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_required_named(),
                              test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_ouput_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_ouput_directory.has_value()
              && parsed_ouput_directory.value() == std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsMissingArgumentOptionalWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_ouput_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_ouput_directory.has_value()
              && parsed_ouput_directory.value() == std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<test::Label::Help>().parsed_value(), std::nullopt);
}

TEST(Lector, ArgumentsMissingValueArgumentFirstNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentFirstWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentLastNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable", "--iterations"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentLastWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable", "--iterations"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentMiddleInlineNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--output=/path/to/output", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentMiddleInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--output=/path/to/output", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentMiddleWhitespaceNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--output", "/path/to/output", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentMiddleWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--output", "/path/to/output", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsNoExecutableNoArgumentsNoConfiguration) {
  lector::Arguments arguments;
  const test::Command command;
  arguments.parse(command.argc(), command.argv());
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.usage().empty());
  EXPECT_TRUE(arguments.options().empty());
  EXPECT_TRUE(arguments.help().empty());
  EXPECT_TRUE(arguments.execution().empty());
}

TEST(Lector, ArgumentsNoExecutableNoArgumentsWithConfiguration) {
  lector::Arguments arguments{test::configuration()};
  const test::Command command;
  arguments.parse(command.argc(), command.argv());
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.usage().empty());
  EXPECT_TRUE(arguments.options().empty());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_TRUE(arguments.execution().empty());
}

TEST(Lector, ArgumentsUnknownArgumentInlineNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable", "--iterations=200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsUnknownArgumentInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable", "--iterations=200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsUnknownArgumentWhitespaceNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable", "--iterations", "200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsUnknownArgumentWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable", "--iterations", "200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsValidConfusingInlineShortLongLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_short(), test::singular_argument_confusing_long()};
  const test::Command command{"/path/to/executable", "--key=200=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key <number>      Short confusing argument." << std::endl
                   << "--key=200 <number>  Long confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingInlineShortLongLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_short(),
                              test::singular_argument_confusing_long()};
  const test::Command command{"/path/to/executable", "--key=200=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key <number>      Short confusing argument." << std::endl
                   << "--key=200 <number>  Long confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingInlineLongShortLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_long(), test::singular_argument_confusing_short()};
  const test::Command command{"/path/to/executable", "--key=200=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key=200 <number>  Long confusing argument." << std::endl
                   << "--key <number>      Short confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingInlineLongShortLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_long(),
                              test::singular_argument_confusing_short()};
  const test::Command command{"/path/to/executable", "--key=200=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key=200 <number>  Long confusing argument." << std::endl
                   << "--key <number>      Short confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_short(), test::singular_argument_confusing_long()};
  const test::Command command{"/path/to/executable", "--key=200", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key <number>      Short confusing argument." << std::endl
                   << "--key=200 <number>  Long confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_short(),
                              test::singular_argument_confusing_long()};
  const test::Command command{"/path/to/executable", "--key=200", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key <number>      Short confusing argument." << std::endl
                   << "--key=200 <number>  Long confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongShortNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_short(), test::singular_argument_confusing_long()};
  const test::Command command{"/path/to/executable", "--key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_confusing_short{
    arguments.get<test::Label::ConfusingShort>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_short.has_value() && parsed_confusing_short.value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key <number>      Short confusing argument." << std::endl
                   << "--key=200 <number>  Long confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongShortWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_short(),
                              test::singular_argument_confusing_long()};
  const test::Command command{"/path/to/executable", "--key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_confusing_short{
    arguments.get<test::Label::ConfusingShort>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_short.has_value() && parsed_confusing_short.value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key <number>] [--key=200 <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key <number>      Short confusing argument." << std::endl
                   << "--key=200 <number>  Long confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortLongNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_long(), test::singular_argument_confusing_short()};
  const test::Command command{"/path/to/executable", "--key=200", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key=200 <number>  Long confusing argument." << std::endl
                   << "--key <number>      Short confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortLongWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_long(),
                              test::singular_argument_confusing_short()};
  const test::Command command{"/path/to/executable", "--key=200", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<test::Label::ConfusingShort>().parsed_value(), std::nullopt);
  const std::optional<std::int32_t>& parsed_confusing_long{
    arguments.get<test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key=200 <number>  Long confusing argument." << std::endl
                   << "--key <number>      Short confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortShortNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_confusing_long(), test::singular_argument_confusing_short()};
  const test::Command command{"/path/to/executable", "--key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_confusing_short{
    arguments.get<test::Label::ConfusingShort>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_short.has_value() && parsed_confusing_short.value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key=200 <number>  Long confusing argument." << std::endl
                   << "--key <number>      Short confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortShortWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_confusing_long(),
                              test::singular_argument_confusing_short()};
  const test::Command command{"/path/to/executable", "--key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_confusing_short{
    arguments.get<test::Label::ConfusingShort>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_short.has_value() && parsed_confusing_short.value() == test::TwoHundred);
  EXPECT_EQ(arguments.get<test::Label::ConfusingLong>().parsed_value(), std::nullopt);
  const std::string expected_usage{"executable [--key=200 <number>] [--key <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "--key=200 <number>  Long confusing argument." << std::endl
                   << "--key <number>      Short confusing argument.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
}

TEST(Lector, ArgumentsValidIndividualHelpNotSpecifiedNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Help>().parsed_value().has_value());
  const std::string expected_usage{"executable [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "-h, --help  Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualHelpNotSpecifiedWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Help>().parsed_value().has_value());
  const std::string expected_usage{"executable [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "-h, --help  Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualHelpSpecifiedNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<bool>& parsed_argument{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value());
  const std::string expected_usage{"executable [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "-h, --help  Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --help");
}

TEST(Lector, ArgumentsValidIndividualHelpSpecifiedWithConfiguration) {
  lector::Arguments arguments{test::configuration(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<bool>& parsed_argument{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value());
  const std::string expected_usage{"executable [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{
    "-h, --help  Display this help information and exit. Optional."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --help");
}

TEST(Lector, ArgumentsValidIndividualIterationsOptionalNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Iterations>().parsed_value().has_value());
  const std::string expected_usage{"executable [--iterations <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-i <number>, --iterations <number>  Number of iterations."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualIterationsOptionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Iterations>().parsed_value().has_value());
  const std::string expected_usage{"executable [--iterations <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-i <number>, --iterations <number>  Number of iterations."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualIterationsRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_integer_required_named()};
  const test::Command command{"/path/to/executable", "--iterations", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_argument{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == 200);
  const std::string expected_usage{"executable --iterations <number>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-i <number>, --iterations <number>  Number of iterations."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200");
}

TEST(Lector, ArgumentsValidIndividualIterationsRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_required_named()};
  const test::Command command{"/path/to/executable", "--iterations", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_argument{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == 200);
  const std::string expected_usage{"executable --iterations <number>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-i <number>, --iterations <number>  Number of iterations."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200");
}

TEST(Lector, ArgumentsValidIndividualOutputDirectoryOptionalNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value());
  const std::string expected_usage{"executable [--output <path>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-o <path>, --output <path>  Output directory."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualOutputDirectoryOptionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::OutputDirectory>().parsed_value().has_value());
  const std::string expected_usage{"executable [--output <path>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-o <path>, --output <path>  Output directory."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualOutputDirectoryRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_required_named()};
  const test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_argument{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == "/path/to/output");
  const std::string expected_usage{"executable --output <path>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-o <path>, --output <path>  Output directory."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --output /path/to/output");
}

TEST(Lector, ArgumentsValidIndividualOutputDirectoryRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named()};
  const test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_argument{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == "/path/to/output");
  const std::string expected_usage{"executable --output <path>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-o <path>, --output <path>  Output directory."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --output /path/to/output");
}

TEST(Lector, ArgumentsValidIndividualPointOptionalNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_data_structure_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Point>().parsed_value().has_value());
  const std::string expected_usage{"executable [--point <value>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-p <value>, --point <value>  Starting point."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualPointOptionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_data_structure_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Point>().parsed_value().has_value());
  const std::string expected_usage{"executable [--point <value>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-p <value>, --point <value>  Starting point."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualPointRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_data_structure_required_named()};
  const test::Command command{"/path/to/executable", "--point", "4.0 5.0 6.0"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Point>& parsed_argument{
    arguments.get<test::Label::Point>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == test::SecondPoint);
  const std::string expected_usage{"executable --point <value>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-p <value>, --point <value>  Starting point."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --point 4 5 6");
}

TEST(Lector, ArgumentsValidIndividualPointRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_data_structure_required_named()};
  const test::Command command{"/path/to/executable", "--point", "4.0 5.0 6.0"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Point>& parsed_argument{
    arguments.get<test::Label::Point>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == test::SecondPoint);
  const std::string expected_usage{"executable --point <value>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-p <value>, --point <value>  Starting point."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --point 4 5 6");
}

TEST(Lector, ArgumentsValidIndividualShapeOptionalNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_enumeration_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Shape>().parsed_value().has_value());
  const std::string expected_usage{"executable [--shape <value>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-s <value>, --shape <value>  Favorite shape."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualShapeOptionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Shape>().parsed_value().has_value());
  const std::string expected_usage{"executable [--shape <value>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-s <value>, --shape <value>  Favorite shape."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualShapeRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_enumeration_required_named()};
  const test::Command command{"/path/to/executable", "--shape", "triangle"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_argument{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == test::Shape::Triangle);
  const std::string expected_usage{"executable --shape <value>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-s <value>, --shape <value>  Favorite shape."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --shape Triangle");
}

TEST(Lector, ArgumentsValidIndividualShapeRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named()};
  const test::Command command{"/path/to/executable", "--shape", "triangle"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_argument{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == test::Shape::Triangle);
  const std::string expected_usage{"executable --shape <value>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-s <value>, --shape <value>  Favorite shape."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --shape Triangle");
}

TEST(Lector, ArgumentsValidIndividualTitleOptionalNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_string_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Title>().parsed_value().has_value());
  const std::string expected_usage{"executable [--title <text>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <text>, --title <text>  Report title."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualTitleOptionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_string_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Title>().parsed_value().has_value());
  const std::string expected_usage{"executable [--title <text>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <text>, --title <text>  Report title."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualTitleRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_string_required_named()};
  const test::Command command{"/path/to/executable", "--title", "Some Other Report"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::string>& parsed_argument{
    arguments.get<test::Label::Title>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == "Some Other Report");
  const std::string expected_usage{"executable --title <text>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <text>, --title <text>  Report title."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --title Some Other Report");
}

TEST(Lector, ArgumentsValidIndividualTitleRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_string_required_named()};
  const test::Command command{"/path/to/executable", "--title", "Some Other Report"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::string>& parsed_argument{
    arguments.get<test::Label::Title>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == "Some Other Report");
  const std::string expected_usage{"executable --title <text>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <text>, --title <text>  Report title."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --title Some Other Report");
}

TEST(Lector, ArgumentsValidIndividualToleranceOptionalNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_floating_point_number_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Tolerance>().parsed_value().has_value());
  const std::string expected_usage{"executable [--tolerance <value>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <value>, --tolerance <value>  Tolerance value."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualToleranceOptionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_floating_point_number_optional_named()};
  const test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<test::Label::Tolerance>().parsed_value().has_value());
  const std::string expected_usage{"executable [--tolerance <value>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <value>, --tolerance <value>  Tolerance value."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
}

TEST(Lector, ArgumentsValidIndividualToleranceRequiredNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_floating_point_number_required_named()};
  const test::Command command{"/path/to/executable", "--tolerance", "0.015625"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<double>& parsed_argument{
    arguments.get<test::Label::Tolerance>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == test::OneOverSixtyFour);
  const std::string expected_usage{"executable --tolerance <value>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <value>, --tolerance <value>  Tolerance value."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options;
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --tolerance " + lector::print<double>(test::OneOverSixtyFour));
}

TEST(Lector, ArgumentsValidIndividualToleranceRequiredWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_floating_point_number_required_named()};
  const test::Command command{"/path/to/executable", "--tolerance", "0.015625"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<double>& parsed_argument{
    arguments.get<test::Label::Tolerance>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == test::OneOverSixtyFour);
  const std::string expected_usage{"executable --tolerance <value>"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  const std::string expected_options{"-t <value>, --tolerance <value>  Tolerance value."};
  EXPECT_EQ(arguments.options(), expected_options);
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --tolerance " + lector::print<double>(test::OneOverSixtyFour));
}

TEST(Lector, ArgumentsValidManyInlineLongKeysNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--shape=Circle", "--output=/path/to/output",
                              "--iterations=200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyInlineLongKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--shape=Circle", "--output=/path/to/output",
                              "--iterations=200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyInlineShortKeysNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "-s=Circle", "-o=/path/to/output", "-i=200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyInlineShortKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "-s=Circle", "-o=/path/to/output", "-i=200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedLongKeysNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--shape=Circle", "--output=/path/to/output",
                              "--iterations=200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedLongKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--shape=Circle", "--output=/path/to/output",
                              "--iterations=200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedShortKeysNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "-s=Circle", "-o", "/path/to/output", "-i=200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyMixedShortKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "-s=Circle", "-o", "/path/to/output", "-i=200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyPositionalNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_positional(),
    test::singular_argument_filesystem_path_required_positional(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "Circle", "/path/to/output", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable <value> <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "<value>                             Favorite shape." << std::endl;
  expected_options << "<path>                              Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(
      arguments.execution(), "/path/to/executable Circle /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyPositionalWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_positional(),
    test::singular_argument_filesystem_path_required_positional(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "Circle", "/path/to/output", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable <value> <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "<value>                             Favorite shape." << std::endl;
  expected_options << "<path>                              Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(
      arguments.execution(), "/path/to/executable Circle /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceLongKeysNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--shape",      "Circle", "--output",
                              "/path/to/output",     "--iterations", "200",    "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceLongKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--shape",      "Circle", "--output",
                              "/path/to/output",     "--iterations", "200",    "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceShortKeysNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "-s", "Circle", "-o", "/path/to/output", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidManyWhitespaceShortKeysWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_enumeration_required_named(),
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "-s", "Circle", "-o", "/path/to/output", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<test::Shape>& parsed_shape{
    arguments.get<test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == test::Shape::Circle);
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{
    "executable --shape <value> --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-s <value>, --shape <value>         Favorite shape." << std::endl;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidSeveralIterationsIterationsAgainNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_integer_optional_named(),
                              test::singular_argument_integer_again_valid()};
  const test::Command command{"/path/to/executable", "--iterations", "200", "--iter", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<std::int32_t>& parsed_iterations_again{
    arguments.get<test::Label::IterationsAgain>().parsed_value()};
  EXPECT_TRUE(
      parsed_iterations_again.has_value() && parsed_iterations_again.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--iterations <number>] [--iter <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options << "-it <number>, --iter <number>       Number of iterations, again.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200 --iter 200");
}

TEST(Lector, ArgumentsValidSeveralIterationsIterationsAgainWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_integer_again_valid()};
  const test::Command command{"/path/to/executable", "--iterations", "200", "--iter", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<std::int32_t>& parsed_iterations_again{
    arguments.get<test::Label::IterationsAgain>().parsed_value()};
  EXPECT_TRUE(
      parsed_iterations_again.has_value() && parsed_iterations_again.value() == test::TwoHundred);
  const std::string expected_usage{"executable [--iterations <number>] [--iter <number>]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options << "-it <number>, --iter <number>       Number of iterations, again.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200 --iter 200");
}

TEST(Lector, ArgumentsValidSeveralIterationsHelpNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations", "200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200 --help");
}

TEST(Lector, ArgumentsValidSeveralIterationsHelpWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_integer_optional_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--iterations", "200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200 --help");
}

TEST(Lector, ArgumentsValidSeveralOutputDirectoryHelpNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_filesystem_path_required_named(),
                              test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--output", "/path/to/output", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable --output <path> [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-o <path>, --output <path>  Output directory." << std::endl;
  expected_options
      << "-h, --help                  Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --output /path/to/output --help");
}

TEST(Lector, ArgumentsValidSeveralOutputDirectoryHelpWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_boolean_named()};
  const test::Command command{"/path/to/executable", "--output", "/path/to/output", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable --output <path> [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-o <path>, --output <path>  Output directory." << std::endl;
  expected_options
      << "-h, --help                  Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --output /path/to/output --help");
}

TEST(Lector, ArgumentsValidSeveralOutputDirectoryIterationsHelpNoConfiguration) {
  lector::Arguments arguments{
    test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--output", "/path/to/output", "--iterations", "200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str();
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsValidSeveralOutputDirectoryIterationsHelpWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_filesystem_path_required_named(),
    test::singular_argument_integer_optional_named(), test::singular_argument_boolean_named()};
  const test::Command command{
    "/path/to/executable", "--output", "/path/to/output", "--iterations", "200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::filesystem::path>& parsed_output_directory{
    arguments.get<test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == std::filesystem::path("/path/to/output"));
  const std::optional<std::int32_t>& parsed_iterations{
    arguments.get<test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == test::TwoHundred);
  const std::optional<bool>& parsed_help{arguments.get<test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  const std::string expected_usage{"executable --output <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options << "-o <path>, --output <path>          Output directory." << std::endl;
  expected_options << "-i <number>, --iterations <number>  Number of iterations." << std::endl;
  expected_options
      << "-h, --help                          Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "An application for testing the Lector library." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes for the application for testing the lector library.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output /path/to/output --iterations 200 --help");
}

TEST(Lector, ArgumentsWeirdLongInlineNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_optional_named()};
  const test::Command command{"/path/to/executable", "==weird=key=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_weird_keys_optional_named()};
  const test::Command command{"/path/to/executable", "==weird=key=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongWhitespaceNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_optional_named()};
  const test::Command command{"/path/to/executable", "==weird=key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_weird_keys_optional_named()};
  const test::Command command{"/path/to/executable", "==weird=key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortInlineNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_required_named()};
  const test::Command command{"/path/to/executable", "=w=k=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortInlineWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_weird_keys_required_named()};
  const test::Command command{"/path/to/executable", "=w=k=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortWhitespaceNoConfiguration) {
  lector::Arguments arguments{test::singular_argument_weird_keys_required_named()};
  const test::Command command{"/path/to/executable", "=w=k", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortWhitespaceWithConfiguration) {
  lector::Arguments arguments{
    test::configuration(), test::singular_argument_weird_keys_required_named()};
  const test::Command command{"/path/to/executable", "=w=k", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), std::filesystem::path("/path/to/executable"));
  const std::optional<std::int32_t>& parsed_weird{
    arguments.get<test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == test::TwoHundred);
}

TEST(Lector, ArityParseEnumeration) {
  static_assert(lector::parse_enumeration<lector::Arity>("") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("Hello, world!") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("UnKnOwN") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("SiNgLe") == std::nullopt);
  static_assert(lector::parse_enumeration<lector::Arity>("RePeAtEd") == std::nullopt);
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
  EXPECT_EQ(lector::parse<lector::Arity>("RePeAtEd"), std::nullopt);
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

TEST(Lector, RepeatableArgumentCopyAssignmentOperator) {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(second.default_values().empty());
  EXPECT_TRUE(second.parsed_values().empty());
  EXPECT_TRUE(second.parsed_or_default_values().empty());
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number>");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = first;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentCopyConstructor) {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second{first};
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentIntegerDefault) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(argument.default_values().empty());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_EQ(argument.execution(), "300 400");
}

TEST(Lector, RepeatableArgumentIntegerOptionalNamed) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>]");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>]");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentIntegerOptionalPositional) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(argument.parsed_values().empty());
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>]");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(argument.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.default_values().at(0), test::OneHundred);
  EXPECT_EQ(argument.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>]");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "300 400");
}

TEST(Lector, RepeatableArgumentIntegerRequiredNamed) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "--iterations <number>");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(argument.default_values().empty());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "--iterations <number>");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentIntegerRequiredPositional) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> argument{
    test::repeatable_argument_integer_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(argument.default_values().empty());
  EXPECT_TRUE(argument.parsed_values().empty());
  EXPECT_TRUE(argument.parsed_or_default_values().empty());
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::ThreeHundred);
  argument.set_parsed_value(test::FourHundred);
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(argument.default_values().empty());
  ASSERT_EQ(argument.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(argument.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(argument.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(argument.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "300 400");
}

TEST(Lector, RepeatableArgumentMoveAssignmentOperator) {
  const lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  EXPECT_TRUE(second.default_values().empty());
  EXPECT_TRUE(second.parsed_values().empty());
  EXPECT_TRUE(second.parsed_or_default_values().empty());
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number>");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = std::move(first);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, RepeatableArgumentMoveConstructor) {
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> first{
    test::repeatable_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(first.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(first.parsed_values().empty());
  ASSERT_EQ(first.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(first.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(first.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(first.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(first.usage(), "[--iterations <number>]");
  EXPECT_EQ(first.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(first.execution().empty());
  lector::RepeatableArgument<test::Label::Iterations, std::int32_t> second{std::move(first)};
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  EXPECT_TRUE(second.parsed_values().empty());
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(test::ThreeHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(1UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300");
  second.set_parsed_value(test::FourHundred);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Repeatable);
  ASSERT_EQ(second.default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.default_values().at(0), test::OneHundred);
  EXPECT_EQ(second.default_values().at(1), test::TwoHundred);
  ASSERT_EQ(second.parsed_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_values().at(1), test::FourHundred);
  ASSERT_EQ(second.parsed_or_default_values().size(), static_cast<std::size_t>(2UL));
  EXPECT_EQ(second.parsed_or_default_values().at(0), test::ThreeHundred);
  EXPECT_EQ(second.parsed_or_default_values().at(1), test::FourHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 300 --iterations 400");
}

TEST(Lector, SingularArgumentBooleanDefault) {
  const lector::SingularArgument<test::Label::Help, bool> argument;
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_type().empty());
  EXPECT_TRUE(argument.usage().empty());
  EXPECT_TRUE(argument.options().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentBooleanOptionalNamed) {
  const std::vector<std::string> expected_keys{"-h", "--help"};
  lector::SingularArgument<test::Label::Help, bool> argument{
    test::singular_argument_boolean_named()};
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_TRUE(argument.default_value().has_value() && !argument.default_value().value());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_FALSE(argument.parsed_or_default_value());
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help]");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(true);
  // TODO.
}

TEST(Lector, SingularArgumentBooleanOptionalPositional) {
  EXPECT_ANY_THROW((void)test::singular_argument_boolean_positional());
}

TEST(Lector, SingularArgumentCopyAssignmentOperator) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.default_value(), std::nullopt);
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number>");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = first;
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(second_default_value.has_value() && second_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == test::OneHundred);
  const std::optional<std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentCopyConstructor) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(second_default_value.has_value() && second_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == test::OneHundred);
  const std::optional<std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentDataStructureDefault) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument;
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentDataStructureOptionalNamed) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  const std::vector<std::string> expected_keys{"-p", "--point"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Point>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::FirstPoint);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::FirstPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>]");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentDataStructureOptionalPositional) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Point>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::FirstPoint);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::FirstPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentDataStructureRequiredNamed) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  const std::vector<std::string> expected_keys{"-p", "--point"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "--point <value>");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentDataStructureRequiredPositional) {
  const lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentEnumerationDefault) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument;
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentEnumerationOptionalNamed) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  const std::vector<std::string> expected_keys{"-s", "--shape"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Shape>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::Shape::Circle);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>]");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentEnumerationOptionalPositional) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Shape>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::Shape::Circle);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentEnumerationRequiredNamed) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  const std::vector<std::string> expected_keys{"-s", "--shape"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "--shape <value>");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentEnumerationRequiredPositional) {
  const lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFilesystemPathDefault) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument;
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path>");
  EXPECT_EQ(argument.options(), "<path>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFilesystemPathOptionalNamed) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  const std::vector<std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::filesystem::path>& default_value{argument.default_value()};
  EXPECT_TRUE(
      default_value.has_value() && default_value.value() == std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output <path>");
  EXPECT_EQ(argument.usage(), "[--output <path>]");
  EXPECT_EQ(argument.options(), "-o <path>, --output <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentFilesystemPathOptionalPositional) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::filesystem::path>& default_value{argument.default_value()};
  EXPECT_TRUE(
      default_value.has_value() && default_value.value() == std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "[<path>]");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentFilesystemPathRequiredNamed) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_required_named()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  const std::vector<std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output <path>");
  EXPECT_EQ(argument.usage(), "--output <path>");
  EXPECT_EQ(argument.options(), "-o <path>, --output <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFilesystemPathRequiredPositional) {
  const lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<path>");
  EXPECT_EQ(argument.usage(), "<path>");
  EXPECT_EQ(argument.options(), "<path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFloatingPointNumberDefault) {
  const lector::SingularArgument<test::Label::Tolerance, double> argument;
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFloatingPointNumberOptionalNamed) {
  const lector::SingularArgument<test::Label::Tolerance, double> argument{
    test::singular_argument_floating_point_number_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  const std::vector<std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<double>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>]");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentFloatingPointNumberOptionalPositional) {
  const lector::SingularArgument<test::Label::Tolerance, double> argument{
    test::singular_argument_floating_point_number_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<double>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "[<value>]");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentFloatingPointNumberRequiredNamed) {
  const lector::SingularArgument<test::Label::Tolerance, double> argument{
    test::singular_argument_floating_point_number_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  const std::vector<std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "--tolerance <value>");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentFloatingPointNumberRequiredPositional) {
  const lector::SingularArgument<test::Label::Tolerance, double> argument{
    test::singular_argument_floating_point_number_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<value>");
  EXPECT_EQ(argument.usage(), "<value>");
  EXPECT_EQ(argument.options(), "<value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentIntegerDefault) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentIntegerOptionalNamed) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneHundred);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>]");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentIntegerOptionalPositional) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneHundred);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "[<number>]");
  EXPECT_EQ(argument.options(), "<number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentIntegerRequiredNamed) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "--iterations <number>");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentIntegerRequiredPositional) {
  const lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
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

TEST(Lector, SingularArgumentInvalidDuplicateKeys) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_duplicate_keys());
}

TEST(Lector, SingularArgumentInvalidEmptyDescription) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_empty_description());
}

TEST(Lector, SingularArgumentInvalidNoKeys) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_no_keys());
}

TEST(Lector, SingularArgumentInvalidBooleanWithDefaultValue) {
  EXPECT_ANY_THROW(test::singular_argument_invalid_boolean_with_default_value());
}

TEST(Lector, SingularArgumentMoveAssignmentOperator) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Required);
  EXPECT_EQ(second.form(), lector::Form::Positional);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  EXPECT_EQ(second.default_value(), std::nullopt);
  EXPECT_EQ(second.parsed_value(), std::nullopt);
  EXPECT_EQ(second.keys_with_value_type(), "<number>");
  EXPECT_EQ(second.usage(), "<number>");
  EXPECT_EQ(second.options(), "<number>");
  EXPECT_TRUE(second.execution().empty());
  second = std::move(first);
  EXPECT_EQ(second.label(), test::Label::Iterations);
  EXPECT_EQ(second.keys(), test::keys_integer());
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(second_default_value.has_value() && second_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == test::OneHundred);
  const std::optional<std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentMoveConstructor) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> first{
    test::singular_argument_integer_optional_named()};
  EXPECT_EQ(first.label(), test::Label::Iterations);
  EXPECT_EQ(first.keys(), test::keys_integer());
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), lector::Importance::Optional);
  EXPECT_EQ(first.form(), lector::Form::Named);
  EXPECT_EQ(first.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(second_default_value.has_value() && second_default_value.value() == test::OneHundred);
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
  EXPECT_EQ(second.importance(), lector::Importance::Optional);
  EXPECT_EQ(second.form(), lector::Form::Named);
  EXPECT_EQ(second.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == test::OneHundred);
  const std::optional<std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.usage(), "[--iterations <number>]");
  EXPECT_EQ(second.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentStringDefault) {
  const lector::SingularArgument<test::Label::Title, std::string> argument;
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text>");
  EXPECT_EQ(argument.options(), "<text>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentStringOptionalNamed) {
  const lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  const std::vector<std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::string>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == "My Report");
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>]");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentStringOptionalPositional) {
  const lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_optional_positional()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::string>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == "My Report");
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "[<text>]");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentStringRequiredNamed) {
  const lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  const std::vector<std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "--title <text>");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentStringRequiredPositional) {
  const lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_required_positional()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<text>");
  EXPECT_EQ(argument.usage(), "<text>");
  EXPECT_EQ(argument.options(), "<text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentWeirdKeysDefault) {
  const lector::SingularArgument<test::Label::Weird, std::int32_t> argument;
  EXPECT_EQ(argument.label(), test::Label::Weird);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Positional);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "<number>");
  EXPECT_EQ(argument.usage(), "<number>");
  EXPECT_EQ(argument.options(), "<number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentWeirdKeysOptionalNamed) {
  const lector::SingularArgument<test::Label::Weird, std::int32_t> argument{
    test::singular_argument_weird_keys_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Weird);
  const std::vector<std::string> expected_keys{"=w=k", "==weird=key"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneHundred);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "[==weird=key <number>]");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, SingularArgumentWeirdKeysRequiredNamed) {
  const lector::SingularArgument<test::Label::Weird, std::int32_t> argument{
    test::singular_argument_weird_keys_required_named()};
  EXPECT_EQ(argument.label(), test::Label::Weird);
  const std::vector<std::string> expected_keys{"=w=k", "==weird=key"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.importance(), lector::Importance::Required);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  EXPECT_EQ(argument.default_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.keys_with_value_type(), "=w=k <number>, ==weird=key <number>");
  EXPECT_EQ(argument.usage(), "==weird=key <number>");
  EXPECT_EQ(argument.options(), "=w=k <number>, ==weird=key <number>  Weird argument.");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, SingularArgumentSetParsedValueHelp) {
  lector::SingularArgument<test::Label::Help, bool> argument{
    test::singular_argument_boolean_named()};
  EXPECT_EQ(argument.label(), test::Label::Help);
  const std::vector<std::string> expected_keys{"-h", "--help"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<bool>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && !default_value.value());
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), false);
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help]");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(true);
  EXPECT_EQ(argument.label(), test::Label::Help);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Display this help information and exit. Optional.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<bool>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && !default_value_again.value());
  const std::optional<bool>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value());
  EXPECT_EQ(argument.parsed_or_default_value(), true);
  EXPECT_EQ(argument.keys_with_value_type(), "-h, --help");
  EXPECT_EQ(argument.usage(), "[--help]");
  EXPECT_EQ(argument.options(), "-h, --help  Display this help information and exit. Optional.");
  EXPECT_EQ(argument.execution(), "--help");
}

TEST(Lector, SingularArgumentSetParsedValuePoint) {
  lector::SingularArgument<test::Label::Point, test::Point> argument{
    test::singular_argument_data_structure_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Point);
  const std::vector<std::string> expected_keys{"-p", "--point"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Point>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::FirstPoint);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::FirstPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>]");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::SecondPoint);
  EXPECT_EQ(argument.label(), test::Label::Point);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Point>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && default_value_again.value() == test::FirstPoint);
  const std::optional<test::Point>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == test::SecondPoint);
  EXPECT_EQ(argument.parsed_or_default_value(), test::SecondPoint);
  EXPECT_EQ(argument.keys_with_value_type(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.usage(), "[--point <value>]");
  EXPECT_EQ(argument.options(), "-p <value>, --point <value>  Starting point.");
  EXPECT_EQ(argument.execution(), "--point 4 5 6");
}

TEST(Lector, SingularArgumentSetParsedValueShape) {
  lector::SingularArgument<test::Label::Shape, test::Shape> argument{
    test::singular_argument_enumeration_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Shape);
  const std::vector<std::string> expected_keys{"-s", "--shape"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Shape>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::Shape::Circle);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>]");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::Shape::Square);
  EXPECT_EQ(argument.label(), test::Label::Shape);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<test::Shape>& default_value_again{argument.default_value()};
  EXPECT_TRUE(
      default_value_again.has_value() && default_value_again.value() == test::Shape::Circle);
  const std::optional<test::Shape>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_value(), test::Shape::Square);
  EXPECT_EQ(argument.keys_with_value_type(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.usage(), "[--shape <value>]");
  EXPECT_EQ(argument.options(), "-s <value>, --shape <value>  Favorite shape.");
  EXPECT_EQ(argument.execution(), "--shape Square");
}

TEST(Lector, SingularArgumentSetParsedValueOutputDirectory) {
  lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path> argument{
    test::singular_argument_filesystem_path_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  const std::vector<std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::filesystem::path>& default_value{argument.default_value()};
  EXPECT_TRUE(
      default_value.has_value() && default_value.value() == std::filesystem::path{"/some/path"});
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path{"/some/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output <path>");
  EXPECT_EQ(argument.usage(), "[--output <path>]");
  EXPECT_EQ(argument.options(), "-o <path>, --output <path>  Output directory.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("/another/path");
  EXPECT_EQ(argument.label(), test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::filesystem::path>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value()
              && default_value_again.value() == std::filesystem::path{"/some/path"});
  const std::optional<std::filesystem::path>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(
      parsed_value.has_value() && parsed_value.value() == std::filesystem::path{"/another/path"});
  EXPECT_EQ(argument.parsed_or_default_value(), std::filesystem::path{"/another/path"});
  EXPECT_EQ(argument.keys_with_value_type(), "-o <path>, --output <path>");
  EXPECT_EQ(argument.usage(), "[--output <path>]");
  EXPECT_EQ(argument.options(), "-o <path>, --output <path>  Output directory.");
  EXPECT_EQ(argument.execution(), "--output /another/path");
}

TEST(Lector, SingularArgumentSetParsedValueTolerance) {
  lector::SingularArgument<test::Label::Tolerance, double> argument{
    test::singular_argument_floating_point_number_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  const std::vector<std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<double>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverThirtyTwo);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>]");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(test::OneOverSixtyFour);
  EXPECT_EQ(argument.label(), test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<double>& default_value_again{argument.default_value()};
  EXPECT_TRUE(
      default_value_again.has_value() && default_value_again.value() == test::OneOverThirtyTwo);
  const std::optional<double>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == test::OneOverSixtyFour);
  EXPECT_EQ(argument.parsed_or_default_value(), test::OneOverSixtyFour);
  EXPECT_EQ(argument.keys_with_value_type(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.usage(), "[--tolerance <value>]");
  EXPECT_EQ(argument.options(), "-t <value>, --tolerance <value>  Tolerance value.");
  EXPECT_EQ(argument.execution(), "--tolerance 0.0156250000000000000");
}

TEST(Lector, SingularArgumentSetParsedValueIterations) {
  lector::SingularArgument<test::Label::Iterations, std::int32_t> argument{
    test::singular_argument_integer_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Iterations);
  EXPECT_EQ(argument.keys(), test::keys_integer());
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == test::OneHundred);
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
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::int32_t>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && default_value_again.value() == test::OneHundred);
  const std::optional<std::int32_t>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_type(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.usage(), "[--iterations <number>]");
  EXPECT_EQ(argument.options(), "-i <number>, --iterations <number>  Number of iterations.");
  EXPECT_EQ(argument.execution(), "--iterations 200");
}

TEST(Lector, SingularArgumentSetParsedValueTitle) {
  lector::SingularArgument<test::Label::Title, std::string> argument{
    test::singular_argument_string_optional_named()};
  EXPECT_EQ(argument.label(), test::Label::Title);
  const std::vector<std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::string>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == "My Report");
  EXPECT_EQ(argument.parsed_value(), std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>]");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("Some Other Report");
  EXPECT_EQ(argument.label(), test::Label::Title);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), lector::Importance::Optional);
  EXPECT_EQ(argument.form(), lector::Form::Named);
  EXPECT_EQ(argument.arity(), lector::Arity::Singular);
  const std::optional<std::string>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && default_value_again.value() == "My Report");
  const std::optional<std::string>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == "Some Other Report");
  EXPECT_EQ(argument.parsed_or_default_value(), "Some Other Report");
  EXPECT_EQ(argument.keys_with_value_type(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.usage(), "[--title <text>]");
  EXPECT_EQ(argument.options(), "-t <text>, --title <text>  Report title.");
  EXPECT_EQ(argument.execution(), "--title Some Other Report");
}

TEST(Lector, TutorialSection1Basic) {
  lector::Arguments arguments{
    lector::Configuration{{"My Application"},
                          {"Description of my application."},
                          {"Additional notes about my application."}},
    lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
                          {"-o", "--output_directory"}, "Output directory. Required."},
    lector::SingularArgument<test::Label::Iterations, std::int32_t>(
        {"-i", "--iterations"},
    "Number of iterations. Optional. Default 100.", 100),
    lector::SingularArgument<test::Label::Help, bool>(
        {"-h", "--help"},
    "Display this help information and exit. Optional.")
  };
  const test::Command command{"/path/to/executable", "-o", "/path/to/directory", "-i", "200"};
  arguments.parse(command.argc(), command.argv());
  if (arguments.get<test::Label::Help>().parsed_or_default_value()) {
    std::cout << arguments.help() << std::endl;
  } else {
    std::cout << "Execution:" << std::endl << arguments.execution() << std::endl;
    const std::filesystem::path& output_directory_path{
      arguments.get<test::Label::OutputDirectory>().parsed_or_default_value()};
    std::cout << "The output directory is: " << output_directory_path << std::endl;
    const std::int32_t iterations_count{
      arguments.get<test::Label::Iterations>().parsed_or_default_value()};
    std::cout << "The number of iterations is: " << iterations_count << std::endl;
  }
  EXPECT_FALSE(arguments.get<test::Label::Help>().parsed_or_default_value());
  const std::string expected_usage{
    "executable --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.configuration().title, "My Application");
  EXPECT_EQ(arguments.configuration().description, "Description of my application.");
  EXPECT_EQ(arguments.configuration().notes, "Additional notes about my application.");
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options
      << "-o <path>, --output_directory <path>  Output directory. Required." << std::endl;
  expected_options
      << "-i <number>, --iterations <number>    Number of iterations. Optional. Default 100."
      << std::endl;
  expected_options
      << "-h, --help                            Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Description of my application." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes about my application.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output_directory /path/to/directory --iterations 200");
}

TEST(Lector, TutorialSection1Help) {
  lector::Arguments arguments{
    lector::Configuration{{"My Application"},
                          {"Description of my application."},
                          {"Additional notes about my application."}},
    lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
                          {"-o", "--output_directory"}, "Output directory. Required."},
    lector::SingularArgument<test::Label::Iterations, std::int32_t>(
        {"-i", "--iterations"},
    "Number of iterations. Optional. Default 100.", 100),
    lector::SingularArgument<test::Label::Help, bool>(
        {"-h", "--help"},
    "Display this help information and exit. Optional.")
  };
  const test::Command command{"/path/to/executable", "-o", "/path/to/directory", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  if (arguments.get<test::Label::Help>().parsed_or_default_value()) {
    std::cout << arguments.help() << std::endl;
  } else {
    std::cout << "Execution:" << std::endl << arguments.execution() << std::endl;
    const std::filesystem::path& output_directory_path{
      arguments.get<test::Label::OutputDirectory>().parsed_or_default_value()};
    std::cout << "The output directory is: " << output_directory_path << std::endl;
    const std::int32_t iterations_count{
      arguments.get<test::Label::Iterations>().parsed_or_default_value()};
    std::cout << "The number of iterations is: " << iterations_count << std::endl;
  }
  EXPECT_TRUE(arguments.get<test::Label::Help>().parsed_or_default_value());
  const std::string expected_usage{
    "executable --output_directory <path> [--iterations <number>] [--help]"};
  EXPECT_EQ(arguments.configuration().title, "My Application");
  EXPECT_EQ(arguments.configuration().description, "Description of my application.");
  EXPECT_EQ(arguments.configuration().notes, "Additional notes about my application.");
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options
      << "-o <path>, --output_directory <path>  Output directory. Required." << std::endl;
  expected_options
      << "-i <number>, --iterations <number>    Number of iterations. Optional. Default 100."
      << std::endl;
  expected_options
      << "-h, --help                            Display this help information and exit. Optional.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Description of my application." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes about my application.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output_directory /path/to/directory --iterations 200 --help");
}

TEST(Lector, TutorialSection3Subsection2) {
  lector::Arguments arguments{
    lector::Configuration{{"My Application"},
                          {"Description of my application."},
                          {"Additional notes about my application."}},
    lector::SingularArgument<test::Label::OutputDirectory, std::filesystem::path>{
                          {"o", "=o", "__out_dir__"}, "Output directory. Required."},
    lector::SingularArgument<test::Label::Iterations, std::int32_t>(
        {"=i=", "_it_", "==iterations=="},
    "Number of iterations. Optional. Default 100.", 100)
  };
  const test::Command command{
    "/path/to/executable", "__out_dir__", "/path/to/directory", "=i=", "200"};
  arguments.parse(command.argc(), command.argv());
  const std::string expected_usage{"executable __out_dir__ <path> [==iterations== <number>]"};
  EXPECT_EQ(arguments.configuration().title, "My Application");
  EXPECT_EQ(arguments.configuration().description, "Description of my application.");
  EXPECT_EQ(arguments.configuration().notes, "Additional notes about my application.");
  EXPECT_EQ(arguments.usage(), expected_usage);
  std::ostringstream expected_options;
  expected_options
      << "o <path>, =o <path>, __out_dir__ <path>               Output directory. Required."
      << std::endl;
  expected_options << "=i= <number>, _it_ <number>, ==iterations== <number>  "
                   << "Number of iterations. Optional. Default 100.";
  EXPECT_EQ(arguments.options(), expected_options.str());
  std::ostringstream expected_help;
  expected_help << "My Application" << std::endl << std::endl;
  expected_help << "Usage:" << std::endl;
  expected_help << expected_usage << std::endl << std::endl;
  expected_help << "Description of my application." << std::endl << std::endl;
  expected_help << "Options:" << std::endl;
  expected_help << expected_options.str() << std::endl << std::endl;
  expected_help << "Additional notes about my application.";
  EXPECT_EQ(arguments.help(), expected_help.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable __out_dir__ /path/to/directory ==iterations== 200");
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
  const test::Command command{"/path/to/executable", "--shape", "square"};
  arguments.parse(command.argc(), command.argv());
  const test::Shape shape{arguments.get<test::Label::Shape>().parsed_or_default_value()};
  std::cout << "Your favorite shape is: " << lector::print(shape) << std::endl;
  EXPECT_EQ(shape, test::Shape::Square);
}

TEST(Lector, TutorialSection3Subsection4) {
  const std::string printed_point{lector::print(test::Point{1.0, 2.0, 3.0})};
  EXPECT_EQ(printed_point, "1 2 3");
  const std::optional<test::Point> parsed_point{lector::parse<test::Point>("4.0 5.0 6.0")};
  const test::Point expected_point{4.0, 5.0, 6.0};
  EXPECT_TRUE(
      parsed_point.has_value() && parsed_point.value().x == expected_point.x
      && parsed_point.value().y == expected_point.y && parsed_point.value().z == expected_point.z);
  lector::Arguments arguments{lector::SingularArgument<test::Label::Point, test::Point>(
      {"-p", "--point"}, "Your favorite point. Optional.", test::Point{})};
  const test::Command command{"/path/to/executable", "--point", "4.0 5.0 6.0"};
  arguments.parse(command.argc(), command.argv());
  const test::Point point{arguments.get<test::Label::Point>().parsed_or_default_value()};
  std::cout << "Your favorite point is: " << point << std::endl;
  EXPECT_TRUE(
      point.x == expected_point.x && point.y == expected_point.y && point.z == expected_point.z);
}

}  // namespace
