// Copyright © 2026, Alexandre Coderre-Chabot.

// This file is part of Lector (https://github.com/acodcha/::lector), a C++ library for parsing
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
enum class Shape : ::std::int8_t {
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
constexpr bool operator==(const ::test::Point& first, const ::test::Point& second) {
  return first.x == second.x && first.y == second.y && first.z == second.z;
}

/// @brief Input stream operator for the test::Point data structure. Populates a test::Point data
/// structure from an input stream.
inline ::std::istream& operator>>(::std::istream& input_stream, ::test::Point& point) {
  input_stream >> point.x >> point.y >> point.z;
  return input_stream;
}

/// @brief Output stream operator for the test::Point data structure. Prints a test::Point data
/// structure to an output stream.
inline ::std::ostream& operator<<(::std::ostream& output_stream, const ::test::Point& point) {
  output_stream << point.x << " " << point.y << " " << point.z;
  return output_stream;
}

/// @brief Default point in three-dimensional space.
inline constexpr ::test::Point FirstPoint{1.0F, 2.0F, 3.0F};

/// @brief Another point in three-dimensional space. Different from the default point.
inline constexpr ::test::Point SecondPoint{4.0F, 5.0F, 6.0F};

}  // namespace

}  // namespace test

namespace lector {

/// @brief Specialization of the lector::Names constant for the test::Shape enumeration.
template <>
inline constexpr ::std::array<::lector::Name<::test::Shape>, 3> Names<::test::Shape>{
  {
   {::test::Shape::Circle, "Circle"},
   {::test::Shape::Triangle, "Triangle"},
   {::test::Shape::Square, "Square"},
   }
};

/// @brief Specialization of the lector::Spellings constant for the test::Shape enumeration.
template <>
inline constexpr ::std::array<::lector::Spelling<::test::Shape>, 9> Spellings<::test::Shape>{
  {
   {"Circle", ::test::Shape::Circle},
   {"Triangle", ::test::Shape::Triangle},
   {"Square", ::test::Shape::Square},
   {"circle", ::test::Shape::Circle},
   {"triangle", ::test::Shape::Triangle},
   {"square", ::test::Shape::Square},
   {"CIRCLE", ::test::Shape::Circle},
   {"TRIANGLE", ::test::Shape::Triangle},
   {"SQUARE", ::test::Shape::Square},
   }
};

}  // namespace lector

namespace test {

namespace {

/// @brief Labels of the command line arguments used for testing.
enum class Label : ::std::int8_t {
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

/// @brief The number 100. Used with Label::Iterations.
constexpr ::std::int32_t OneHundred{100};

/// @brief The number 200. Used with Label::Iterations.
constexpr ::std::int32_t TwoHundred{200};

/// @brief The fraction 1/32. Used with Label::Tolerance.
constexpr double OneOverThirtyTwo{0.03125};

/// @brief The fraction 1/64. Used with Label::Tolerance.
constexpr double OneOverSixtyFour{0.015625};

::lector::Argument<::test::Label::ConfusingLong, ::std::int32_t> create_argument_confusing_long() {
  return ::lector::Argument<::test::Label::ConfusingLong, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"--key=200"}, "Long confusing argument.",
    ::test::OneHundred};
}

::lector::Argument<::test::Label::ConfusingShort, ::std::int32_t>
create_argument_confusing_short() {
  return ::lector::Argument<::test::Label::ConfusingShort, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"--key"}, "Short confusing argument.",
    ::test::OneHundred};
}

::lector::Argument<::test::Label::Help, bool> create_argument_help() {
  return ::lector::Argument<::test::Label::Help, bool>{
    ::std::initializer_list<::std::string>{"-h", "--help"},
    "Display usage information and exit. Optional."
  };
}

::lector::Argument<::test::Label::IterationsAgain, ::std::int32_t>
create_argument_iterations_again_invalid() {
  return ::lector::Argument<::test::Label::IterationsAgain, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"-it", "--iterations"},
    "Number of iterations, again.",
    ::test::OneHundred
  };
}

::lector::Argument<::test::Label::IterationsAgain, ::std::int32_t>
create_argument_iterations_again_valid() {
  return ::lector::Argument<::test::Label::IterationsAgain, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"-it", "--iter"},
    "Number of iterations, again.",
    ::test::OneHundred
  };
}

::lector::Argument<::test::Label::Iterations, ::std::int32_t>
create_argument_iterations_optional() {
  return ::lector::Argument<::test::Label::Iterations, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"-i", "--iterations"},
    "Number of iterations.",
    ::test::OneHundred
  };
}

::lector::Argument<::test::Label::Iterations, ::std::int32_t>
create_argument_iterations_required() {
  return ::lector::Argument<::test::Label::Iterations, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"-i", "--iterations"},
    "Number of iterations."
  };
}

::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>
create_argument_output_directory_optional() {
  return ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>{
    ::std::initializer_list<::std::string>{"-o", "--output"},
    "Output directory.",
    ::std::filesystem::path("/some/path")
  };
}

::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>
create_argument_output_directory_required() {
  return ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>{
    ::std::initializer_list<::std::string>{"-o", "--output"},
    "Output directory."
  };
}

::lector::Argument<::test::Label::Point, ::test::Point> create_argument_point_optional() {
  return ::lector::Argument<::test::Label::Point, ::test::Point>{
    ::std::initializer_list<::std::string>{"-p", "--point"},
    "Starting point.", ::test::FirstPoint
  };
}

::lector::Argument<::test::Label::Point, ::test::Point> create_argument_point_required() {
  return ::lector::Argument<::test::Label::Point, ::test::Point>{
    ::std::initializer_list<::std::string>{"-p", "--point"},
    "Starting point."
  };
}

::lector::Argument<::test::Label::Shape, ::test::Shape> create_argument_shape_optional() {
  return ::lector::Argument<::test::Label::Shape, ::test::Shape>{
    ::std::initializer_list<::std::string>{"-s", "--shape"},
    "Favorite shape.",
    ::test::Shape::Circle
  };
}

::lector::Argument<::test::Label::Shape, ::test::Shape> create_argument_shape_required() {
  return ::lector::Argument<::test::Label::Shape, ::test::Shape>{
    ::std::initializer_list<::std::string>{"-s", "--shape"},
    "Favorite shape."
  };
}

::lector::Argument<::test::Label::Title, ::std::string> create_argument_title_optional() {
  return ::lector::Argument<::test::Label::Title, ::std::string>{
    ::std::initializer_list<::std::string>{"-t", "--title"},
    "Report title.", "My Report"
  };
}

::lector::Argument<::test::Label::Title, ::std::string> create_argument_title_required() {
  return ::lector::Argument<::test::Label::Title, ::std::string>{
    ::std::initializer_list<::std::string>{"-t", "--title"},
    "Report title."
  };
}

::lector::Argument<::test::Label::Tolerance, double> create_argument_tolerance_optional() {
  return ::lector::Argument<::test::Label::Tolerance, double>{
    ::std::initializer_list<::std::string>{"-t", "--tolerance"},
    "Tolerance value.",
    ::test::OneOverThirtyTwo
  };
}

::lector::Argument<::test::Label::Tolerance, double> create_argument_tolerance_required() {
  return ::lector::Argument<::test::Label::Tolerance, double>{
    ::std::initializer_list<::std::string>{"-t", "--tolerance"},
    "Tolerance value."
  };
}

::lector::Argument<::test::Label::Weird, ::std::int32_t> create_argument_weird_optional() {
  return ::lector::Argument<::test::Label::Weird, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"=w=k", "==weird=key"},
    "Weird argument.",
    ::test::OneHundred
  };
}

::lector::Argument<::test::Label::Weird, ::std::int32_t> create_argument_weird_required() {
  return ::lector::Argument<::test::Label::Weird, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"=w=k", "==weird=key"},
    "Weird argument."
  };
}

/// @brief Helper function that creates an invalid lector::Argument with all empty keys.
void create_invalid_argument_with_all_empty_keys() {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    {"", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid lector::Argument with an empty key.
void create_invalid_argument_with_an_empty_key() {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    {"-i", "--iterations", ""},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid lector::Argument with duplicate keys.
void create_invalid_argument_with_duplicate_keys() {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    {"-i", "--iterations", "-i"},
    "Number of iterations."
  };
}

/// @brief Helper function that creates an invalid lector::Argument with an empty description.
void create_invalid_argument_with_empty_description() {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    {"-i", "--iterations"},
    ""
  };
}

/// @brief Helper function that creates an invalid lector::Argument with no keys.
void create_invalid_argument_with_no_keys() {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    {}, "Number of iterations."};
}

/// @brief Helper function that creates an invalid lector::Argument with a boolean type and a
/// default value.
void create_invalid_boolean_argument_with_a_default_value() {
  const ::lector::Argument<::test::Label::Help, bool> argument{
    {"-h", "--help"},
    "Display usage information and exit. Optional.",
    true,
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
  explicit Command(::std::initializer_list<::std::string> arguments)
    : argc_{static_cast<int>(arguments.size())} {
    // Allocate the argv array. Note that the argv array must be null-terminated by the C standard;
    // the +1 is for the null terminator at the end of the argv array.
    argv_ = new char*[argc_ + 1];
    argv_[argc_] = nullptr;
    // Populate the argv array with C-strings from the specified arguments.
    ::std::size_t index{0};
    for (const ::std::string& argument : arguments) {
      const ::std::size_t length{argument.length() + 1};
      argv_[index] = new char[length];
#ifdef _MSC_VER
      ::strncpy_s(argv_[index], length, argument.c_str(), length);
#else
      ::std::strncpy(argv_[index], argument.c_str(), length);
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
  Command(const ::test::Command&) = delete;

  /// @brief Copy assignment operator. Deleted to prevent double-free errors when a Command object
  /// is copied, since it manages a dynamically allocated array of C-strings.
  ::test::Command& operator=(const ::test::Command&) = delete;

  /// @brief Move constructor. Moves the resources of another Command object into this one, leaving
  /// the other object in a valid but unspecified state, with argc set to 0 and argv set to nullptr.
  /// @param[in, out] other The Command object to move from.
  Command(::test::Command&& other) noexcept : argc_(other.argc_), argv_(other.argv_) {
    other.argc_ = 0;
    other.argv_ = nullptr;
  }

  /// @brief Move assignment operator. Moves the resources of another Command object into this one,
  /// leaving the other object in a valid but unspecified state, with argc set to 0 and argv set to
  /// nullptr. Frees the current resources of this object before taking ownership of the new
  /// resources.
  /// @param[in, out] other The Command object to move from.
  /// @return This Command object after the move assignment.
  ::test::Command& operator=(::test::Command&& other) noexcept {
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
      const ::std::size_t count{static_cast<::std::size_t>(argc_)};
      for (::std::size_t index{0}; index < count; ++index) {
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
static_assert(::lector::AreUnique<>::value);

/// @brief Validate that a list of only one type is unique.
static_assert(::lector::AreUnique<::test::Label::Shape>::value);

/// @brief Validate that a list of unique types are unique.
static_assert(::lector::AreUnique<::test::Label::Shape, ::test::Label::Iterations,
                                  ::test::Label::Help>::value);

/// @brief Validate that a list of duplicated types is not unique.
static_assert(!::lector::AreUnique<::test::Label::Shape, ::test::Label::Shape>::value);

/// @brief Validate that a list of duplicated and unique types is not unique.
static_assert(!::lector::AreUnique<::test::Label::Shape, ::test::Label::Iterations,
                                   ::test::Label::Shape>::value);

TEST(Lector, ArgumentsDuplicatedArgumentInline) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--iterations=200", "--iterations=300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentMixed) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--iterations", "200", "--iterations=300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicatedArgumentWhitespace) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--iterations", "200", "--iterations", "300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsDuplicateKeys) {
  EXPECT_ANY_THROW(::lector::Arguments(::test::create_argument_iterations_optional(),
                                       ::test::create_argument_iterations_again_invalid()));
}

TEST(Lector, ArgumentsEmptyExecutableOnly) {
  ::lector::Arguments arguments;
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable");
  EXPECT_TRUE(arguments.usage_options().empty());
}

TEST(Lector, ArgumentsEmptyNoExecutable) {
  ::lector::Arguments arguments;
  const ::test::Command command;
  arguments.parse(command.argc(), command.argv());
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.execution().empty());
  EXPECT_TRUE(arguments.usage_command().empty());
  EXPECT_TRUE(arguments.usage_options().empty());
}

TEST(Lector, ArgumentsInvalidValueForArgumentInline) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--iterations=Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsInvalidValueForArgumentWhitespace) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--iterations", "Hello", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentRequired) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentOptionalInline) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--output=/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::filesystem::path>& parsed_ouput_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_ouput_directory.has_value()
              && parsed_ouput_directory.value() == ::std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<::test::Label::Help>().parsed_value(), ::std::nullopt);
}

TEST(Lector, ArgumentsMissingArgumentOptionalWhitespace) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::filesystem::path>& parsed_ouput_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_ouput_directory.has_value()
              && parsed_ouput_directory.value() == ::std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<::test::Label::Help>().parsed_value(), ::std::nullopt);
}

TEST(Lector, ArgumentsMissingValueArgumentFirst) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentLast) {
  ::lector::Arguments arguments{::test::create_argument_iterations_optional()};
  const ::test::Command command{"/path/to/executable", "--iterations"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentMiddleInline) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--output=/path/to/output", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingValueArgumentMiddleWhitespace) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--output", "/path/to/output", "--iterations", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsNoExecutableNoArguments) {
  ::lector::Arguments arguments;
  const ::test::Command command;
  arguments.parse(command.argc(), command.argv());
  EXPECT_TRUE(arguments.executable_path().empty());
  EXPECT_TRUE(arguments.execution().empty());
  EXPECT_TRUE(arguments.usage_command().empty());
  EXPECT_TRUE(arguments.usage_options().empty());
}

TEST(Lector, ArgumentsUnknownArgumentInline) {
  ::lector::Arguments arguments{::test::create_argument_iterations_optional()};
  const ::test::Command command{"/path/to/executable", "--iterations=200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsUnknownArgumentWhitespace) {
  ::lector::Arguments arguments{::test::create_argument_iterations_optional()};
  const ::test::Command command{"/path/to/executable", "--iterations", "200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsValidConfusingInlineShortLongLong) {
  ::lector::Arguments arguments{
    ::test::create_argument_confusing_short(), ::test::create_argument_confusing_long()};
  const ::test::Command command{"/path/to/executable", "--key=200=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<::test::Label::ConfusingShort>().parsed_value(), ::std::nullopt);
  const ::std::optional<::std::int32_t>& parsed_confusing_long{
    arguments.get<::test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--key <number>] [--key=200 <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "[--key <number>]  Short confusing argument." << ::std::endl
                         << "[--key=200 <number>]  Long confusing argument.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidConfusingInlineLongShortLong) {
  ::lector::Arguments arguments{
    ::test::create_argument_confusing_long(), ::test::create_argument_confusing_short()};
  const ::test::Command command{"/path/to/executable", "--key=200=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<::test::Label::ConfusingShort>().parsed_value(), ::std::nullopt);
  const ::std::optional<::std::int32_t>& parsed_confusing_long{
    arguments.get<::test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--key=200 <number>] [--key <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "[--key=200 <number>]  Long confusing argument." << ::std::endl
                         << "[--key <number>]  Short confusing argument.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongLong) {
  ::lector::Arguments arguments{
    ::test::create_argument_confusing_short(), ::test::create_argument_confusing_long()};
  const ::test::Command command{"/path/to/executable", "--key=200", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<::test::Label::ConfusingShort>().parsed_value(), ::std::nullopt);
  const ::std::optional<::std::int32_t>& parsed_confusing_long{
    arguments.get<::test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--key <number>] [--key=200 <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "[--key <number>]  Short confusing argument." << ::std::endl
                         << "[--key=200 <number>]  Long confusing argument.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidConfusingWhitespaceShortLongShort) {
  ::lector::Arguments arguments{
    ::test::create_argument_confusing_short(), ::test::create_argument_confusing_long()};
  const ::test::Command command{"/path/to/executable", "--key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_confusing_short{
    arguments.get<::test::Label::ConfusingShort>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_short.has_value() && parsed_confusing_short.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.get<::test::Label::ConfusingLong>().parsed_value(), ::std::nullopt);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--key <number>] [--key=200 <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "[--key <number>]  Short confusing argument." << ::std::endl
                         << "[--key=200 <number>]  Long confusing argument.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortLong) {
  ::lector::Arguments arguments{
    ::test::create_argument_confusing_long(), ::test::create_argument_confusing_short()};
  const ::test::Command command{"/path/to/executable", "--key=200", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<::test::Label::ConfusingShort>().parsed_value(), ::std::nullopt);
  const ::std::optional<::std::int32_t>& parsed_confusing_long{
    arguments.get<::test::Label::ConfusingLong>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_long.has_value() && parsed_confusing_long.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key=200 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--key=200 <number>] [--key <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "[--key=200 <number>]  Long confusing argument." << ::std::endl
                         << "[--key <number>]  Short confusing argument.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidConfusingWhitespaceLongShortShort) {
  ::lector::Arguments arguments{
    ::test::create_argument_confusing_long(), ::test::create_argument_confusing_short()};
  const ::test::Command command{"/path/to/executable", "--key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_confusing_short{
    arguments.get<::test::Label::ConfusingShort>().parsed_value()};
  EXPECT_TRUE(
      parsed_confusing_short.has_value() && parsed_confusing_short.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.get<::test::Label::ConfusingLong>().parsed_value(), ::std::nullopt);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --key 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--key=200 <number>] [--key <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "[--key=200 <number>]  Long confusing argument." << ::std::endl
                         << "[--key <number>]  Short confusing argument.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidIndividualHelpNotSpecified) {
  ::lector::Arguments arguments{::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::Help>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--help]");
  EXPECT_EQ(
      arguments.usage_options(), "[-h, --help]  Display usage information and exit. Optional.");
}

TEST(Lector, ArgumentsValidIndividualHelpSpecified) {
  ::lector::Arguments arguments{::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<bool>& parsed_argument{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --help");
  EXPECT_EQ(arguments.usage_command(), "executable [--help]");
  EXPECT_EQ(
      arguments.usage_options(), "[-h, --help]  Display usage information and exit. Optional.");
}

TEST(Lector, ArgumentsValidIndividualIterationsOptional) {
  ::lector::Arguments arguments{::test::create_argument_iterations_optional()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::Iterations>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--iterations <number>]");
  EXPECT_EQ(
      arguments.usage_options(), "[-i <number>, --iterations <number>]  Number of iterations.");
}

TEST(Lector, ArgumentsValidIndividualIterationsRequired) {
  ::lector::Arguments arguments{::test::create_argument_iterations_required()};
  const ::test::Command command{"/path/to/executable", "--iterations", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_argument{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == 200);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200");
  EXPECT_EQ(arguments.usage_command(), "executable --iterations <number>");
  EXPECT_EQ(arguments.usage_options(), "-i <number>, --iterations <number>  Number of iterations.");
}

TEST(Lector, ArgumentsValidIndividualOutputDirectoryOptional) {
  ::lector::Arguments arguments{::test::create_argument_output_directory_optional()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::OutputDirectory>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--output <path>]");
  EXPECT_EQ(arguments.usage_options(), "[-o <path>, --output <path>]  Output directory.");
}

TEST(Lector, ArgumentsValidIndividualOutputDirectoryRequired) {
  ::lector::Arguments arguments{::test::create_argument_output_directory_required()};
  const ::test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::filesystem::path>& parsed_argument{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == "/path/to/output");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --output /path/to/output");
  EXPECT_EQ(arguments.usage_command(), "executable --output <path>");
  EXPECT_EQ(arguments.usage_options(), "-o <path>, --output <path>  Output directory.");
}

TEST(Lector, ArgumentsValidIndividualPointOptional) {
  ::lector::Arguments arguments{::test::create_argument_point_optional()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::Point>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--point <value>]");
  EXPECT_EQ(arguments.usage_options(), "[-p <value>, --point <value>]  Starting point.");
}

TEST(Lector, ArgumentsValidIndividualPointRequired) {
  ::lector::Arguments arguments{::test::create_argument_point_required()};
  const ::test::Command command{"/path/to/executable", "--point", "4.0 5.0 6.0"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Point>& parsed_argument{
    arguments.get<::test::Label::Point>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == ::test::SecondPoint);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --point 4 5 6");
  EXPECT_EQ(arguments.usage_command(), "executable --point <value>");
  EXPECT_EQ(arguments.usage_options(), "-p <value>, --point <value>  Starting point.");
}

TEST(Lector, ArgumentsValidIndividualShapeOptional) {
  ::lector::Arguments arguments{::test::create_argument_shape_optional()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::Shape>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--shape <value>]");
  EXPECT_EQ(arguments.usage_options(), "[-s <value>, --shape <value>]  Favorite shape.");
}

TEST(Lector, ArgumentsValidIndividualShapeRequired) {
  ::lector::Arguments arguments{::test::create_argument_shape_required()};
  const ::test::Command command{"/path/to/executable", "--shape", "triangle"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_argument{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == ::test::Shape::Triangle);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --shape Triangle");
  EXPECT_EQ(arguments.usage_command(), "executable --shape <value>");
  EXPECT_EQ(arguments.usage_options(), "-s <value>, --shape <value>  Favorite shape.");
}

TEST(Lector, ArgumentsValidIndividualTitleOptional) {
  ::lector::Arguments arguments{::test::create_argument_title_optional()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::Title>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--title <text>]");
  EXPECT_EQ(arguments.usage_options(), "[-t <text>, --title <text>]  Report title.");
}

TEST(Lector, ArgumentsValidIndividualTitleRequired) {
  ::lector::Arguments arguments{::test::create_argument_title_required()};
  const ::test::Command command{"/path/to/executable", "--title", "Some Other Report"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::string>& parsed_argument{
    arguments.get<::test::Label::Title>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == "Some Other Report");
  EXPECT_EQ(arguments.execution(), "/path/to/executable --title Some Other Report");
  EXPECT_EQ(arguments.usage_command(), "executable --title <text>");
  EXPECT_EQ(arguments.usage_options(), "-t <text>, --title <text>  Report title.");
}

TEST(Lector, ArgumentsValidIndividualToleranceOptional) {
  ::lector::Arguments arguments{::test::create_argument_tolerance_optional()};
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_FALSE(arguments.get<::test::Label::Tolerance>().parsed_value().has_value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable");
  EXPECT_EQ(arguments.usage_command(), "executable [--tolerance <value>]");
  EXPECT_EQ(arguments.usage_options(), "[-t <value>, --tolerance <value>]  Tolerance value.");
}

TEST(Lector, ArgumentsValidIndividualToleranceRequired) {
  ::lector::Arguments arguments{::test::create_argument_tolerance_required()};
  const ::test::Command command{"/path/to/executable", "--tolerance", "0.015625"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<double>& parsed_argument{
    arguments.get<::test::Label::Tolerance>().parsed_value()};
  EXPECT_TRUE(parsed_argument.has_value() && parsed_argument.value() == ::test::OneOverSixtyFour);
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --tolerance " + ::lector::print<double>(::test::OneOverSixtyFour));
  EXPECT_EQ(arguments.usage_command(), "executable --tolerance <value>");
  EXPECT_EQ(arguments.usage_options(), "-t <value>, --tolerance <value>  Tolerance value.");
}

TEST(Lector, ArgumentsValidManyInlineLongKeys) {
  ::lector::Arguments arguments{
    ::test::create_argument_shape_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--shape=Circle", "--output=/path/to/output",
                                "--iterations=200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_shape{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == ::test::Shape::Circle);
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(),
            "executable --shape <value> --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-s <value>, --shape <value>  Favorite shape." << ::std::endl;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidManyInlineShortKeys) {
  ::lector::Arguments arguments{
    ::test::create_argument_shape_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "-s=Circle", "-o=/path/to/output", "-i=200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_shape{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == ::test::Shape::Circle);
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(),
            "executable --shape <value> --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-s <value>, --shape <value>  Favorite shape." << ::std::endl;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidManyMixedLongKeys) {
  ::lector::Arguments arguments{
    ::test::create_argument_shape_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--shape=Circle", "--output=/path/to/output",
                                "--iterations=200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_shape{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == ::test::Shape::Circle);
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(),
            "executable --shape <value> --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-s <value>, --shape <value>  Favorite shape." << ::std::endl;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidManyMixedShortKeys) {
  ::lector::Arguments arguments{
    ::test::create_argument_shape_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "-s=Circle", "-o", "/path/to/output", "-i=200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_shape{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == ::test::Shape::Circle);
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(),
            "executable --shape <value> --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-s <value>, --shape <value>  Favorite shape." << ::std::endl;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidManyWhitespaceLongKeys) {
  ::lector::Arguments arguments{
    ::test::create_argument_shape_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--shape",      "Circle", "--output",
                                "/path/to/output",     "--iterations", "200",    "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_shape{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == ::test::Shape::Circle);
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(),
            "executable --shape <value> --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-s <value>, --shape <value>  Favorite shape." << ::std::endl;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidManyWhitespaceShortKeys) {
  ::lector::Arguments arguments{
    ::test::create_argument_shape_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "-s", "Circle", "-o", "/path/to/output", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::test::Shape>& parsed_shape{
    arguments.get<::test::Label::Shape>().parsed_value()};
  EXPECT_TRUE(parsed_shape.has_value() && parsed_shape.value() == ::test::Shape::Circle);
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --shape Circle --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(),
            "executable --shape <value> --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-s <value>, --shape <value>  Favorite shape." << ::std::endl;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidSeveralIterationsIterationsAgain) {
  ::lector::Arguments arguments{::test::create_argument_iterations_optional(),
                                ::test::create_argument_iterations_again_valid()};
  const ::test::Command command{"/path/to/executable", "--iterations", "200", "--iter", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<::std::int32_t>& parsed_iterations_again{
    arguments.get<::test::Label::IterationsAgain>().parsed_value()};
  EXPECT_TRUE(
      parsed_iterations_again.has_value() && parsed_iterations_again.value() == ::test::TwoHundred);
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200 --iter 200");
  EXPECT_EQ(arguments.usage_command(), "executable [--iterations <number>] [--iter <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-it <number>, --iter <number>]  Number of iterations, again.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidSeveralIterationsHelp) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--iterations", "200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --iterations 200 --help");
  EXPECT_EQ(arguments.usage_command(), "executable [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidSeveralOutputDirectoryHelp) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--output", "/path/to/output", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(), "/path/to/executable --output /path/to/output --help");
  EXPECT_EQ(arguments.usage_command(), "executable --output <path> [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsValidSeveralOutputDirectoryIterationsHelp) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--output", "/path/to/output", "--iterations", "200", "--help"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::filesystem::path>& parsed_output_directory{
    arguments.get<::test::Label::OutputDirectory>().parsed_value()};
  EXPECT_TRUE(parsed_output_directory.has_value()
              && parsed_output_directory.value() == ::std::filesystem::path("/path/to/output"));
  const ::std::optional<::std::int32_t>& parsed_iterations{
    arguments.get<::test::Label::Iterations>().parsed_value()};
  EXPECT_TRUE(parsed_iterations.has_value() && parsed_iterations.value() == ::test::TwoHundred);
  const ::std::optional<bool>& parsed_help{arguments.get<::test::Label::Help>().parsed_value()};
  EXPECT_TRUE(parsed_help.has_value() && parsed_help.value());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(
      arguments.usage_command(), "executable --output <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options << "-o <path>, --output <path>  Output directory." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations." << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
}

TEST(Lector, ArgumentsWeirdLongInline) {
  ::lector::Arguments arguments{::test::create_argument_weird_optional()};
  const ::test::Command command{"/path/to/executable", "==weird=key=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_weird{
    arguments.get<::test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == ::test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdLongWhitespace) {
  ::lector::Arguments arguments{::test::create_argument_weird_optional()};
  const ::test::Command command{"/path/to/executable", "==weird=key", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_weird{
    arguments.get<::test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == ::test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortInline) {
  ::lector::Arguments arguments{::test::create_argument_weird_required()};
  const ::test::Command command{"/path/to/executable", "=w=k=200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_weird{
    arguments.get<::test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == ::test::TwoHundred);
}

TEST(Lector, ArgumentsWeirdShortWhitespace) {
  ::lector::Arguments arguments{::test::create_argument_weird_required()};
  const ::test::Command command{"/path/to/executable", "=w=k", "200"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  const ::std::optional<::std::int32_t>& parsed_weird{
    arguments.get<::test::Label::Weird>().parsed_value()};
  EXPECT_TRUE(parsed_weird.has_value() && parsed_weird.value() == ::test::TwoHundred);
}

TEST(Lector, ArgumentConstructorBooleanDefault) {
  const ::lector::Argument<::test::Label::Help, bool> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorBooleanOptional) {
  const ::lector::Argument<::test::Label::Help, bool> argument{::test::create_argument_help()};
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  const ::std::vector<::std::string> expected_keys{"-h", "--help"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Display usage information and exit. Optional.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<bool>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && !default_value.value());
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_FALSE(argument.parsed_or_default_value());
  EXPECT_EQ(argument.keys_with_value_types(), "-h, --help");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorDataStructureDefault) {
  const ::lector::Argument<::test::Label::Point, ::test::Point> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Point);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorDataStructureOptional) {
  const ::lector::Argument<::test::Label::Point, ::test::Point> argument{
    ::test::create_argument_point_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Point);
  const ::std::vector<::std::string> expected_keys{"-p", "--point"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::test::Point>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::FirstPoint);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::FirstPoint);
  EXPECT_EQ(argument.keys_with_value_types(), "-p <value>, --point <value>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorDataStructureRequired) {
  const ::lector::Argument<::test::Label::Point, ::test::Point> argument{
    ::test::create_argument_point_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Point);
  const ::std::vector<::std::string> expected_keys{"-p", "--point"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-p <value>, --point <value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorEnumerationDefault) {
  const ::lector::Argument<::test::Label::Shape, ::test::Shape> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Shape);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorEnumerationOptional) {
  const ::lector::Argument<::test::Label::Shape, ::test::Shape> argument{
    ::test::create_argument_shape_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Shape);
  const ::std::vector<::std::string> expected_keys{"-s", "--shape"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::test::Shape>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::Shape::Circle);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Shape::Circle);
  EXPECT_EQ(argument.keys_with_value_types(), "-s <value>, --shape <value>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorEnumerationRequired) {
  const ::lector::Argument<::test::Label::Shape, ::test::Shape> argument{
    ::test::create_argument_shape_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Shape);
  const ::std::vector<::std::string> expected_keys{"-s", "--shape"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-s <value>, --shape <value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorFilesystemPathDefault) {
  const ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument;
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorFilesystemPathOptional) {
  const ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    ::test::create_argument_output_directory_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const ::std::vector<::std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::filesystem::path>& default_value{argument.default_value()};
  EXPECT_TRUE(
      default_value.has_value() && default_value.value() == ::std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.keys_with_value_types(), "-o <path>, --output <path>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorFilesystemPathRequired) {
  const ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    ::test::create_argument_output_directory_required()};
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const ::std::vector<::std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-o <path>, --output <path>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorNumberFloatingPointDefault) {
  const ::lector::Argument<::test::Label::Tolerance, double> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorNumberFloatingPointOptional) {
  const ::lector::Argument<::test::Label::Tolerance, double> argument{
    ::test::create_argument_tolerance_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  const ::std::vector<::std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<double>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::OneOverThirtyTwo);
  EXPECT_EQ(argument.keys_with_value_types(), "-t <value>, --tolerance <value>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorNumberFloatingPointRequired) {
  const ::lector::Argument<::test::Label::Tolerance, double> argument{
    ::test::create_argument_tolerance_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  const ::std::vector<::std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-t <value>, --tolerance <value>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorNumberIntegerDefault) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorNumberIntegerOptional) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::OneHundred);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorNumberIntegerRequired) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    ::test::create_argument_iterations_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorStringDefault) {
  const ::lector::Argument<::test::Label::Title, ::std::string> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorStringOptional) {
  const ::lector::Argument<::test::Label::Title, ::std::string> argument{
    ::test::create_argument_title_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const ::std::vector<::std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::string>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == "My Report");
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_EQ(argument.keys_with_value_types(), "-t <text>, --title <text>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorStringRequired) {
  const ::lector::Argument<::test::Label::Title, ::std::string> argument{
    ::test::create_argument_title_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const ::std::vector<::std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-t <text>, --title <text>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorWeirdDefault) {
  const ::lector::Argument<::test::Label::Weird, ::std::int32_t> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Weird);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentConstructorWeirdOptional) {
  const ::lector::Argument<::test::Label::Weird, ::std::int32_t> argument{
    ::test::create_argument_weird_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Weird);
  const ::std::vector<::std::string> expected_keys{"=w=k", "==weird=key"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::OneHundred);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_EQ(argument.keys_with_value_types(), "=w=k <number>, ==weird=key <number>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorWeirdRequired) {
  const ::lector::Argument<::test::Label::Weird, ::std::int32_t> argument{
    ::test::create_argument_weird_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Weird);
  const ::std::vector<::std::string> expected_keys{"=w=k", "==weird=key"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Weird argument.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "=w=k <number>, ==weird=key <number>");
  EXPECT_TRUE(argument.execution().empty());
  EXPECT_ANY_THROW(static_cast<void>(argument.parsed_or_default_value()));
}

TEST(Lector, ArgumentCopyConstructor) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_TRUE(first.execution().empty());
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> second{first};
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(
      second_default_value.has_value() && second_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(::test::TwoHundred);
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == ::test::OneHundred);
  const ::std::optional<::std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == ::test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, ArgumentCopyAssignmentOperator) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_TRUE(first.execution().empty());
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> second;
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.importance(), ::lector::Importance::Required);
  EXPECT_EQ(second.default_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(second.execution().empty());
  second = first;
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(
      second_default_value.has_value() && second_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(::test::TwoHundred);
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == ::test::OneHundred);
  const ::std::optional<::std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == ::test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, ArgumentMoveConstructor) {
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_TRUE(first.execution().empty());
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> second{::std::move(first)};
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(
      second_default_value.has_value() && second_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(::test::TwoHundred);
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == ::test::OneHundred);
  const ::std::optional<::std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == ::test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, ArgumentMoveAssignmentOperator) {
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& first_default_value{first.default_value()};
  EXPECT_TRUE(first_default_value.has_value() && first_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_TRUE(first.execution().empty());
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> second;
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.importance(), ::lector::Importance::Required);
  EXPECT_EQ(second.default_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(second.execution().empty());
  second = ::std::move(first);
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& second_default_value{second.default_value()};
  EXPECT_TRUE(
      second_default_value.has_value() && second_default_value.value() == ::test::OneHundred);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
  second.set_parsed_value(::test::TwoHundred);
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t> second_default_value_again{second.default_value()};
  EXPECT_TRUE(second_default_value_again.has_value()
              && second_default_value_again.value() == ::test::OneHundred);
  const ::std::optional<::std::int32_t> second_parsed_value{second.parsed_value()};
  EXPECT_TRUE(second_parsed_value.has_value() && second_parsed_value.value() == ::test::TwoHundred);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_EQ(second.execution(), "--iterations 200");
}

TEST(Lector, ArgumentSetParsedValueBoolean) {
  ::lector::Argument<::test::Label::Help, bool> argument{::test::create_argument_help()};
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  const ::std::vector<::std::string> expected_keys{"-h", "--help"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Display usage information and exit. Optional.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<bool>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && !default_value.value());
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), false);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(true);
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Display usage information and exit. Optional.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<bool>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && !default_value_again.value());
  const ::std::optional<bool>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value());
  EXPECT_EQ(argument.parsed_or_default_value(), true);
  EXPECT_EQ(argument.keys_with_value_types(), "-h, --help");
  EXPECT_EQ(argument.execution(), "--help");
}

TEST(Lector, ArgumentSetParsedValueDataStructure) {
  ::lector::Argument<::test::Label::Point, ::test::Point> argument{
    ::test::create_argument_point_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Point);
  const ::std::vector<::std::string> expected_keys{"-p", "--point"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::test::Point>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::FirstPoint);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::FirstPoint);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(::test::SecondPoint);
  EXPECT_EQ(argument.label(), ::test::Label::Point);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Starting point.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::test::Point>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && default_value_again.value() == ::test::FirstPoint);
  const ::std::optional<::test::Point>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == ::test::SecondPoint);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::SecondPoint);
  EXPECT_EQ(argument.keys_with_value_types(), "-p <value>, --point <value>");
  EXPECT_EQ(argument.execution(), "--point 4 5 6");
}

TEST(Lector, ArgumentSetParsedValueEnumeration) {
  ::lector::Argument<::test::Label::Shape, ::test::Shape> argument{
    ::test::create_argument_shape_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Shape);
  const ::std::vector<::std::string> expected_keys{"-s", "--shape"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::test::Shape>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::Shape::Circle);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Shape::Circle);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(::test::Shape::Square);
  EXPECT_EQ(argument.label(), ::test::Label::Shape);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Favorite shape.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::test::Shape>& default_value_again{argument.default_value()};
  EXPECT_TRUE(
      default_value_again.has_value() && default_value_again.value() == ::test::Shape::Circle);
  const ::std::optional<::test::Shape>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == ::test::Shape::Square);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Shape::Square);
  EXPECT_EQ(argument.keys_with_value_types(), "-s <value>, --shape <value>");
  EXPECT_EQ(argument.execution(), "--shape Square");
}

TEST(Lector, ArgumentSetParsedValueFilesystemPath) {
  ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    ::test::create_argument_output_directory_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const ::std::vector<::std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::filesystem::path>& default_value{argument.default_value()};
  EXPECT_TRUE(
      default_value.has_value() && default_value.value() == ::std::filesystem::path{"/some/path"});
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::filesystem::path{"/some/path"});
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("/another/path");
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::filesystem::path>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value()
              && default_value_again.value() == ::std::filesystem::path{"/some/path"});
  const ::std::optional<::std::filesystem::path>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(
      parsed_value.has_value() && parsed_value.value() == ::std::filesystem::path{"/another/path"});
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::filesystem::path{"/another/path"});
  EXPECT_EQ(argument.keys_with_value_types(), "-o <path>, --output <path>");
  EXPECT_EQ(argument.execution(), "--output /another/path");
}

TEST(Lector, ArgumentSetParsedValueNumberFloatingPoint) {
  ::lector::Argument<::test::Label::Tolerance, double> argument{
    ::test::create_argument_tolerance_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  const ::std::vector<::std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<double>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::OneOverThirtyTwo);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::OneOverThirtyTwo);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(::test::OneOverSixtyFour);
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<double>& default_value_again{argument.default_value()};
  EXPECT_TRUE(
      default_value_again.has_value() && default_value_again.value() == ::test::OneOverThirtyTwo);
  const ::std::optional<double>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == ::test::OneOverSixtyFour);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::OneOverSixtyFour);
  EXPECT_EQ(argument.keys_with_value_types(), "-t <value>, --tolerance <value>");
  EXPECT_EQ(argument.execution(), "--tolerance 0.0156250000000000000");
}

TEST(Lector, ArgumentSetParsedValueNumberInteger) {
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == ::test::OneHundred);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::OneHundred);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(::test::TwoHundred);
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::int32_t>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && default_value_again.value() == ::test::OneHundred);
  const ::std::optional<::std::int32_t>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == ::test::TwoHundred);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::TwoHundred);
  EXPECT_EQ(argument.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.execution(), "--iterations 200");
}

TEST(Lector, ArgumentSetParsedValueString) {
  ::lector::Argument<::test::Label::Title, ::std::string> argument{
    ::test::create_argument_title_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const ::std::vector<::std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::string>& default_value{argument.default_value()};
  EXPECT_TRUE(default_value.has_value() && default_value.value() == "My Report");
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("Some Other Report");
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Report title.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  const ::std::optional<::std::string>& default_value_again{argument.default_value()};
  EXPECT_TRUE(default_value_again.has_value() && default_value_again.value() == "My Report");
  const ::std::optional<::std::string>& parsed_value{argument.parsed_value()};
  EXPECT_TRUE(parsed_value.has_value() && parsed_value.value() == "Some Other Report");
  EXPECT_EQ(argument.parsed_or_default_value(), "Some Other Report");
  EXPECT_EQ(argument.keys_with_value_types(), "-t <text>, --title <text>");
  EXPECT_EQ(argument.execution(), "--title Some Other Report");
}

TEST(Lector, ArgumentValidate) {
  EXPECT_ANY_THROW(::test::create_invalid_argument_with_all_empty_keys());
  EXPECT_ANY_THROW(::test::create_invalid_argument_with_an_empty_key());
  EXPECT_ANY_THROW(::test::create_invalid_argument_with_duplicate_keys());
  EXPECT_ANY_THROW(::test::create_invalid_argument_with_empty_description());
  EXPECT_ANY_THROW(::test::create_invalid_argument_with_no_keys());
  EXPECT_ANY_THROW(::test::create_invalid_boolean_argument_with_a_default_value());
}

TEST(Lector, TutorialSection1Basic) {
  ::lector::Arguments arguments{
    ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>(
        {"-o", "--output_directory"}, "Output directory. Required."),
    ::lector::Argument<::test::Label::Iterations, ::std::int32_t>(
        {"-i", "--iterations"}, "Number of iterations. Optional. Default: 100.", 100),
    ::lector::Argument<::test::Label::Help, bool>(
        {"-h", "--help"}, "Display usage information and exit. Optional.")};

  const ::test::Command command{"/path/to/executable", "-o", "/path/to/directory", "-i", "200"};

  arguments.parse(command.argc(), command.argv());

  if (arguments.get<::test::Label::Help>().parsed_or_default_value()) {
    ::std::cout << "Usage: " << arguments.usage_command() << ::std::endl;
    ::std::cout << "Options: " << ::std::endl << arguments.usage_options() << ::std::endl;
  } else {
    ::std::cout << "Execution: " << arguments.execution() << ::std::endl;

    const ::std::filesystem::path& output_directory{
      arguments.get<::test::Label::OutputDirectory>().parsed_or_default_value()};

    ::std::cout << "The output directory is: " << output_directory << ::std::endl;

    const ::std::int32_t iterations{
      arguments.get<::test::Label::Iterations>().parsed_or_default_value()};

    ::std::cout << "The number of iterations is: " << iterations << ::std::endl;
  }

  EXPECT_FALSE(arguments.get<::test::Label::Help>().parsed_or_default_value());
  EXPECT_EQ(arguments.usage_command(),
            "executable --output_directory <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options
      << "-o <path>, --output_directory <path>  Output directory. Required." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations. Optional. Default: 100."
      << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output_directory /path/to/directory --iterations 200");
}

TEST(Lector, TutorialSection1Help) {
  ::lector::Arguments arguments{
    ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>(
        {"-o", "--output_directory"}, "Output directory. Required."),
    ::lector::Argument<::test::Label::Iterations, ::std::int32_t>(
        {"-i", "--iterations"}, "Number of iterations. Optional. Default: 100.", 100),
    ::lector::Argument<::test::Label::Help, bool>(
        {"-h", "--help"}, "Display usage information and exit. Optional.")};

  const ::test::Command command{
    "/path/to/executable", "-o", "/path/to/directory", "-i", "200", "-h"};

  arguments.parse(command.argc(), command.argv());

  if (arguments.get<::test::Label::Help>().parsed_or_default_value()) {
    ::std::cout << "Usage: " << arguments.usage_command() << ::std::endl;
    ::std::cout << "Options: " << ::std::endl << arguments.usage_options() << ::std::endl;
  } else {
    ::std::cout << "Execution: " << arguments.execution() << ::std::endl;

    const ::std::filesystem::path& output_directory{
      arguments.get<::test::Label::OutputDirectory>().parsed_or_default_value()};

    ::std::cout << "The output directory is: " << output_directory << ::std::endl;

    const ::std::int32_t iterations{
      arguments.get<::test::Label::Iterations>().parsed_or_default_value()};

    ::std::cout << "The number of iterations is: " << iterations << ::std::endl;
  }

  EXPECT_TRUE(arguments.get<::test::Label::Help>().parsed_or_default_value());
  EXPECT_EQ(arguments.usage_command(),
            "executable --output_directory <path> [--iterations <number>] [--help]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options
      << "-o <path>, --output_directory <path>  Output directory. Required." << ::std::endl;
  expected_usage_options
      << "[-i <number>, --iterations <number>]  Number of iterations. Optional. Default: 100."
      << ::std::endl;
  expected_usage_options << "[-h, --help]  Display usage information and exit. Optional.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable --output_directory /path/to/directory --iterations 200 --help");
}

TEST(Lector, TutorialSection32) {
  ::lector::Arguments arguments{
    ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path>(
        {"o", "=o", "__out_dir__"}, "Output directory. Required."),
    ::lector::Argument<::test::Label::Iterations, ::std::int32_t>(
        {"=i=", "_it_", "==iterations=="}, "Number of iterations. Optional. Default: 100.", 100)};

  const ::test::Command command{"/path/to/executable", "=o", "/path/to/directory", "_it_", "200"};

  arguments.parse(command.argc(), command.argv());

  EXPECT_EQ(arguments.usage_command(), "executable __out_dir__ <path> [==iterations== <number>]");
  ::std::ostringstream expected_usage_options;
  expected_usage_options
      << "o <path>, =o <path>, __out_dir__ <path>  Output directory. Required." << ::std::endl;
  expected_usage_options << "[=i= <number>, _it_ <number>, ==iterations== <number>]  "
                            "Number of iterations. Optional. Default: 100.";
  EXPECT_EQ(arguments.usage_options(), expected_usage_options.str());
  EXPECT_EQ(arguments.execution(),
            "/path/to/executable __out_dir__ /path/to/directory ==iterations== 200");
}

TEST(Lector, TutorialSection33) {
  const ::std::string printed_triangle{lector::print(::test::Shape::Triangle)};
  EXPECT_EQ(printed_triangle, "Triangle");

  const ::std::optional<::test::Shape> parsed_triangle{lector::parse<::test::Shape>("TRIANGLE")};
  EXPECT_TRUE(parsed_triangle.has_value() && parsed_triangle.value() == ::test::Shape::Triangle);

  const ::std::optional<::test::Shape> invalid_shape{lector::parse<::test::Shape>("Invalid Shape")};
  EXPECT_TRUE(!invalid_shape.has_value());

  ::lector::Arguments arguments{::lector::Argument<::test::Label::Shape, ::test::Shape>(
      {"-s", "--shape"}, "Your favorite shape. Optional.", ::test::Shape::Circle)};

  const ::test::Command command{"/path/to/executable", "--shape", "square"};

  arguments.parse(command.argc(), command.argv());

  const ::test::Shape shape{arguments.get<::test::Label::Shape>().parsed_or_default_value()};

  ::std::cout << "Your favorite shape is: " << ::lector::print(shape) << ::std::endl;

  EXPECT_EQ(shape, ::test::Shape::Square);
}

TEST(Lector, TutorialSection34) {
  const ::std::string printed_point{::lector::print(::test::Point{1.0, 2.0, 3.0})};
  EXPECT_EQ(printed_point, "1 2 3");

  const ::std::optional<::test::Point> parsed_point{::lector::parse<::test::Point>("4.0 5.0 6.0")};
  const ::test::Point expected_point{4.0, 5.0, 6.0};
  EXPECT_TRUE(
      parsed_point.has_value() && parsed_point.value().x == expected_point.x
      && parsed_point.value().y == expected_point.y && parsed_point.value().z == expected_point.z);

  ::lector::Arguments arguments{::lector::Argument<::test::Label::Point, ::test::Point>(
      {"-p", "--point"}, "Your favorite point. Optional.", ::test::Point{})};

  const ::test::Command command{"/path/to/executable", "--point", "4.0 5.0 6.0"};

  arguments.parse(command.argc(), command.argv());

  const ::test::Point point{arguments.get<::test::Label::Point>().parsed_or_default_value()};

  ::std::cout << "Your favorite point is: " << point << ::std::endl;

  EXPECT_TRUE(
      point.x == expected_point.x && point.y == expected_point.y && point.z == expected_point.z);
}

}  // namespace
