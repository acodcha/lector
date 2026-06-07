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

#include "lector.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <gtest/gtest.h>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace test {

namespace {

/// @brief Enumeration type used for testing the parsing of enumeration command line arguments.
enum class Color : ::std::int8_t {
  Red,
  Green,
  Blue,
};

}  // namespace

}  // namespace test

namespace lector {

/// @brief Specialization of the lector::Printings constant for the test::Color enumeration.
template <>
inline constexpr ::std::array<::lector::Printing<::test::Color>, 3> Printings<::test::Color>{
  {
   {::test::Color::Red, "Red"},
   {::test::Color::Green, "Green"},
   {::test::Color::Blue, "Blue"},
   }
};

/// @brief Specialization of the lector::Spellings constant for the test::Color enumeration.
template <>
inline constexpr ::std::array<::lector::Spelling<::test::Color>, 9> Spellings<::test::Color>{
  {
   {"Red", ::test::Color::Red},
   {"Green", ::test::Color::Green},
   {"Blue", ::test::Color::Blue},
   {"red", ::test::Color::Red},
   {"green", ::test::Color::Green},
   {"blue", ::test::Color::Blue},
   {"RED", ::test::Color::Red},
   {"GREEN", ::test::Color::Green},
   {"BLUE", ::test::Color::Blue},
   }
};

}  // namespace lector

namespace test {

namespace {

/// @brief Labels of the command line arguments used for testing.
enum class Label : ::std::int8_t {
  Title,
  OutputDirectory,
  Color,
  Iterations,
  Tolerance,
  Help,
};

::lector::Argument<::test::Label::Title, ::std::string> create_argument_title_optional() {
  return ::lector::Argument<::test::Label::Title, ::std::string>{
    ::std::initializer_list<::std::string>{"-t", "--title"},
    "Title of the report.", "My Report"
  };
}

::lector::Argument<::test::Label::Title, ::std::string> create_argument_title_required() {
  return ::lector::Argument<::test::Label::Title, ::std::string>{
    ::std::initializer_list<::std::string>{"-t", "--title"},
    "Title of the report."
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

::lector::Argument<::test::Label::Color, ::test::Color> create_argument_color_optional() {
  return ::lector::Argument<::test::Label::Color, ::test::Color>{
    ::std::initializer_list<::std::string>{"-c", "--color"},
    "Main output color.",
    ::test::Color::Red
  };
}

::lector::Argument<::test::Label::Color, ::test::Color> create_argument_color_required() {
  return ::lector::Argument<::test::Label::Color, ::test::Color>{
    ::std::initializer_list<::std::string>{"-c", "--color"},
    "Main output color."
  };
}

::lector::Argument<::test::Label::Iterations, ::std::int32_t>
create_argument_iterations_optional() {
  return ::lector::Argument<::test::Label::Iterations, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"-i", "--iterations"},
    "Number of iterations.", 100
  };
}

::lector::Argument<::test::Label::Iterations, ::std::int32_t>
create_argument_iterations_required() {
  return ::lector::Argument<::test::Label::Iterations, ::std::int32_t>{
    ::std::initializer_list<::std::string>{"-i", "--iterations"},
    "Number of iterations."
  };
}

::lector::Argument<::test::Label::Tolerance, double> create_argument_tolerance_optional() {
  return ::lector::Argument<::test::Label::Tolerance, double>{
    ::std::initializer_list<::std::string>{"-t", "--tolerance"},
    "Tolerance value.", 0.03125
  };
}

::lector::Argument<::test::Label::Tolerance, double> create_argument_tolerance_required() {
  return ::lector::Argument<::test::Label::Tolerance, double>{
    ::std::initializer_list<::std::string>{"-t", "--tolerance"},
    "Tolerance value."
  };
}

::lector::Argument<::test::Label::Help, bool> create_argument_help() {
  return ::lector::Argument<::test::Label::Help, bool>{
    ::std::initializer_list<::std::string>{"-h", "--help"},
    "Print usage information."
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
    "Print usage information.",
    true,
  };
}

/// @brief Helper class to construct argc and argv for testing the parsing of command line
/// arguments.
class Command {
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
      argv_[index] = new char[argument.length() + 1];
      ::std::strcpy(argv_[index], argument.c_str());
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
    if (argv_) {
      for (::std::size_t index{0}; index < static_cast<::std::size_t>(argc_); ++index) {
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

TEST(Lector, ArgumentsDuplicatedArgument) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--output", "--iterations", "200", "--iterations", "300", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsExecutableOnlyNoArguments) {
  ::lector::Arguments arguments;
  const ::test::Command command{"/path/to/executable"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.print_execution(), "/path/to/executable");
  EXPECT_EQ(arguments.print_usage_command(), "executable");
  EXPECT_TRUE(arguments.print_usage_options().empty());
}

TEST(Lector, ArgumentsInvalidValueForArgument) {
  ::lector::Arguments arguments{
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "--output", "--iterations", "Hello, world!", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentRequired) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--help"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsMissingArgumentOptional) {
  ::lector::Arguments arguments{
    ::test::create_argument_output_directory_required(), ::test::create_argument_help()};
  const ::test::Command command{"/path/to/executable", "--output", "/path/to/output"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  ASSERT_TRUE(arguments.get<::test::Label::OutputDirectory>().parsed_value().has_value());
  EXPECT_EQ(arguments.get<::test::Label::OutputDirectory>().parsed_value(),
            ::std::filesystem::path("/path/to/output"));
  ASSERT_FALSE(arguments.get<::test::Label::Help>().parsed_value().has_value());
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

TEST(Lector, ArgumentsMissingValueArgumentMiddle) {
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
  EXPECT_TRUE(arguments.print_execution().empty());
  EXPECT_TRUE(arguments.print_usage_command().empty());
  EXPECT_TRUE(arguments.print_usage_options().empty());
}

TEST(Lector, ArgumentsUnknownArgument) {
  ::lector::Arguments arguments{::test::create_argument_iterations_optional()};
  const ::test::Command command{"/path/to/executable", "--iterations", "200", "--unknown"};
  EXPECT_ANY_THROW(arguments.parse(command.argc(), command.argv()));
}

TEST(Lector, ArgumentsValid) {
  ::lector::Arguments arguments{
    ::test::create_argument_color_required(), ::test::create_argument_output_directory_required(),
    ::test::create_argument_iterations_optional(), ::test::create_argument_help()};
  const ::test::Command command{
    "/path/to/executable", "-c", "Red", "-o", "/path/to/output", "-i", "200", "-h"};
  arguments.parse(command.argc(), command.argv());
  EXPECT_EQ(arguments.executable_path(), ::std::filesystem::path("/path/to/executable"));
  EXPECT_EQ(arguments.get<::test::Label::Color>().parsed_value(), ::test::Color::Red);
  EXPECT_EQ(arguments.get<::test::Label::OutputDirectory>().parsed_value(),
            ::std::filesystem::path("/path/to/output"));
  EXPECT_EQ(arguments.get<::test::Label::Iterations>().parsed_value(), 200);
  EXPECT_TRUE(arguments.get<::test::Label::Help>().parsed_value());
  EXPECT_EQ(arguments.print_execution(),
            "/path/to/executable --color Red --output /path/to/output --iterations 200 --help");
  EXPECT_EQ(arguments.print_usage_command(),
            "executable --color <value> --output <path> [--iterations <number>] [--help]");
  std::ostringstream expected_usage_details;
  expected_usage_details << "-c <value>, --color <value>  Main output color." << std::endl;
  expected_usage_details << "-o <path>, --output <path>  Output directory." << std::endl;
  expected_usage_details
      << "[-i <number>, --iterations <number>]  Number of iterations." << std::endl;
  expected_usage_details << "[-h, --help]  Print usage information." << std::endl;
  EXPECT_EQ(arguments.print_usage_options(), expected_usage_details.str());
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
}

TEST(Lector, ArgumentConstructorBooleanOptional) {
  const ::lector::Argument<::test::Label::Help, bool> argument{::test::create_argument_help()};
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  const ::std::vector<::std::string> expected_keys{"-h", "--help"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Print usage information.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), false);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_FALSE(argument.parsed_or_default_value());
  EXPECT_EQ(argument.keys_with_value_types(), "-h, --help");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorEnumerationDefault) {
  const ::lector::Argument<::test::Label::Color, ::test::Color> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_TRUE(argument.keys_with_value_types().empty());
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorEnumerationOptional) {
  const ::lector::Argument<::test::Label::Color, ::test::Color> argument{
    ::test::create_argument_color_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  const ::std::vector<::std::string> expected_keys{"-c", "--color"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Main output color.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::test::Color::Red);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Color::Red);
  EXPECT_EQ(argument.keys_with_value_types(), "-c <value>, --color <value>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentConstructorEnumerationRequired) {
  const ::lector::Argument<::test::Label::Color, ::test::Color> argument{
    ::test::create_argument_color_required()};
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  const ::std::vector<::std::string> expected_keys{"-c", "--color"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Main output color.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-c <value>, --color <value>");
  EXPECT_TRUE(argument.execution().empty());
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
}

TEST(Lector, ArgumentConstructorFilesystemPathOptional) {
  const ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    ::test::create_argument_output_directory_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const ::std::vector<::std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::std::filesystem::path("/some/path"));
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
}

TEST(Lector, ArgumentConstructorNumberFloatingPointOptional) {
  const ::lector::Argument<::test::Label::Tolerance, double> argument{
    ::test::create_argument_tolerance_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  const ::std::vector<::std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 0.03125);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 0.03125);
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
}

TEST(Lector, ArgumentConstructorNumberIntegerOptional) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> argument{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 100);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 100);
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
}

TEST(Lector, ArgumentConstructorStringOptional) {
  const ::lector::Argument<::test::Label::Title, ::std::string> argument{
    ::test::create_argument_title_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const ::std::vector<::std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Title of the report.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), "My Report");
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
  EXPECT_EQ(argument.description(), "Title of the report.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.keys_with_value_types(), "-t <text>, --title <text>");
  EXPECT_TRUE(argument.execution().empty());
}

TEST(Lector, ArgumentCopyConstructor) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
  EXPECT_TRUE(first.execution().empty());
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> second{first};
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
}

TEST(Lector, ArgumentCopyAssignmentOperator) {
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
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
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
}

TEST(Lector, ArgumentMoveConstructor) {
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
  EXPECT_TRUE(first.execution().empty());
  const ::lector::Argument<::test::Label::Iterations, ::std::int32_t> second{std::move(first)};
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
}

TEST(Lector, ArgumentMoveAssignmentOperator) {
  ::lector::Argument<::test::Label::Iterations, ::std::int32_t> first{
    ::test::create_argument_iterations_optional()};
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const ::std::vector<::std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
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
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
  EXPECT_EQ(second.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_TRUE(second.execution().empty());
}

TEST(Lector, ArgumentSetParsedValueBoolean) {
  ::lector::Argument<::test::Label::Help, bool> argument{::test::create_argument_help()};
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  const ::std::vector<::std::string> expected_keys{"-h", "--help"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Print usage information.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), false);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), false);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(true);
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Print usage information.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), false);
  EXPECT_EQ(argument.parsed_value(), true);
  EXPECT_EQ(argument.parsed_or_default_value(), true);
  EXPECT_EQ(argument.keys_with_value_types(), "-h, --help");
  EXPECT_EQ(argument.execution(), "--help");
}

TEST(Lector, ArgumentSetParsedValueEnumeration) {
  ::lector::Argument<::test::Label::Color, ::test::Color> argument{
    ::test::create_argument_color_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  const ::std::vector<::std::string> expected_keys{"-c", "--color"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Main output color.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::test::Color::Red);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Color::Red);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(::test::Color::Blue);
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Main output color.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::test::Color::Red);
  EXPECT_EQ(argument.parsed_value(), ::test::Color::Blue);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Color::Blue);
  EXPECT_EQ(argument.keys_with_value_types(), "-c <value>, --color <value>");
  EXPECT_EQ(argument.execution(), "--color Blue");
}

TEST(Lector, ArgumentSetParsedValueFilesystemPath) {
  ::lector::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    ::test::create_argument_output_directory_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const ::std::vector<::std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), "/some/path");
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "/some/path");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("/another/path");
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), "/some/path");
  EXPECT_EQ(argument.parsed_value(), "/another/path");
  EXPECT_EQ(argument.parsed_or_default_value(), "/another/path");
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
  EXPECT_EQ(argument.default_value(), 0.03125);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 0.03125);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(0.015625);
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 0.03125);
  EXPECT_EQ(argument.parsed_value(), 0.015625);
  EXPECT_EQ(argument.parsed_or_default_value(), 0.015625);
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
  EXPECT_EQ(argument.default_value(), 100);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 100);
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value(200);
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 100);
  EXPECT_EQ(argument.parsed_value(), 200);
  EXPECT_EQ(argument.parsed_or_default_value(), 200);
  EXPECT_EQ(argument.keys_with_value_types(), "-i <number>, --iterations <number>");
  EXPECT_EQ(argument.execution(), "--iterations 200");
}

TEST(Lector, ArgumentSetParsedValueString) {
  ::lector::Argument<::test::Label::Title, ::std::string> argument{
    ::test::create_argument_title_optional()};
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const ::std::vector<::std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Title of the report.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), "My Report");
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
  EXPECT_TRUE(argument.execution().empty());
  argument.set_parsed_value("Some Other Report");
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Title of the report.");
  EXPECT_EQ(argument.importance(), ::lector::Importance::Optional);
  EXPECT_EQ(argument.default_value(), "My Report");
  EXPECT_EQ(argument.parsed_value(), "Some Other Report");
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

TEST(Lector, ParseBoolean) {
  EXPECT_EQ(::lector::parse<bool>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("TRUE"), true);
  EXPECT_EQ(::lector::parse<bool>("True"), true);
  EXPECT_EQ(::lector::parse<bool>("true"), true);
  EXPECT_EQ(::lector::parse<bool>("FALSE"), false);
  EXPECT_EQ(::lector::parse<bool>("False"), false);
  EXPECT_EQ(::lector::parse<bool>("false"), false);
}

TEST(Lector, ParseEnumeration) {
  static_assert(::lector::parse_enumeration<::lector::Importance>("Optional").value()
                == ::lector::Importance::Optional);
  static_assert(!::lector::parse_enumeration<::lector::Importance>("Hello, world!").has_value());

  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>(" Optional"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("Optional "), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>(" Required"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("Required "), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("OPTIONAL"),
            ::lector::Importance::Optional);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("Optional"),
            ::lector::Importance::Optional);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("optional"),
            ::lector::Importance::Optional);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("REQUIRED"),
            ::lector::Importance::Required);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("Required"),
            ::lector::Importance::Required);
  EXPECT_EQ(::lector::parse_enumeration<::lector::Importance>("required"),
            ::lector::Importance::Required);

  EXPECT_EQ(::lector::parse<::lector::Importance>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::lector::Importance>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::lector::Importance>(" Optional"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::lector::Importance>("Optional "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::lector::Importance>(" Required"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::lector::Importance>("Required "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::lector::Importance>("OPTIONAL"), ::lector::Importance::Optional);
  EXPECT_EQ(::lector::parse<::lector::Importance>("Optional"), ::lector::Importance::Optional);
  EXPECT_EQ(::lector::parse<::lector::Importance>("optional"), ::lector::Importance::Optional);
  EXPECT_EQ(::lector::parse<::lector::Importance>("REQUIRED"), ::lector::Importance::Required);
  EXPECT_EQ(::lector::parse<::lector::Importance>("Required"), ::lector::Importance::Required);
  EXPECT_EQ(::lector::parse<::lector::Importance>("required"), ::lector::Importance::Required);
}

TEST(Lector, ParseFilesystemPath) {
  EXPECT_EQ(::lector::parse<::std::filesystem::path>(""), ::std::filesystem::path{});
  EXPECT_EQ(
      ::lector::parse<::std::filesystem::path>("some_path"), ::std::filesystem::path{"some_path"});
  EXPECT_EQ(::lector::parse<::std::filesystem::path>("some_file.txt"),
            ::std::filesystem::path{"some_file.txt"});
  EXPECT_EQ(::lector::parse<::std::filesystem::path>("/some/path"),
            ::std::filesystem::path{"/some/path"});
  EXPECT_EQ(::lector::parse<::std::filesystem::path>("/some/file.txt"),
            ::std::filesystem::path{"/some/file.txt"});
}

TEST(Lector, ParseNumberFloatingPointPrecisionDouble) {
  // Invalid.
  EXPECT_EQ(::lector::parse<double>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(::lector::parse<double>("NAN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<double>("NAN").value()));
  ASSERT_TRUE(::lector::parse<double>("NaN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<double>("NaN").value()));
  ASSERT_TRUE(::lector::parse<double>("nan").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<double>("nan").value()));
  ASSERT_TRUE(::lector::parse<double>("-NAN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<double>("-NAN").value()));
  ASSERT_TRUE(::lector::parse<double>("-NaN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<double>("-NaN").value()));
  ASSERT_TRUE(::lector::parse<double>("-nan").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<double>("-nan").value()));

  // Infinity.
  EXPECT_EQ(::lector::parse<double>("INF"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("Inf"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("inf"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("-INF"), -::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("-Inf"), -::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("-inf"), -::std::numeric_limits<double>::infinity());

  // Zero.
  EXPECT_EQ(::lector::parse<double>("0F"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0f"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0L"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0l"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0.0F"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0.0f"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0.0"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0.0L"), 0.0);
  EXPECT_EQ(::lector::parse<double>("0.0l"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0F"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0f"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0L"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0l"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0.0F"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0.0f"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0.0"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0.0L"), 0.0);
  EXPECT_EQ(::lector::parse<double>("-0.0l"), 0.0);

  // Integer.
  EXPECT_EQ(::lector::parse<double>("-10F"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10f"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10L"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10l"), -10.0);

  // Decimal notation.
  EXPECT_EQ(::lector::parse<double>("-3.14F"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14f"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14L"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14l"), -3.14);

  // Scientific notation, small number.
  EXPECT_EQ(::lector::parse<double>("-3.14E-12F"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14E-12f"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14E-12"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14E-12L"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14E-12l"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14e-12F"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14e-12f"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14e-12"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14e-12L"), -3.14E-12);
  EXPECT_EQ(::lector::parse<double>("-3.14e-12l"), -3.14E-12);

  // Scientific notation, large number.
  EXPECT_EQ(::lector::parse<double>("-3.14E12F"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E12f"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E12"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E12L"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E12l"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e12F"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e12f"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e12"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e12L"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e12l"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E+12F"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E+12f"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E+12"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E+12L"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14E+12l"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e+12F"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e+12f"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e+12"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e+12L"), -3.14E12);
  EXPECT_EQ(::lector::parse<double>("-3.14e+12l"), -3.14E12);
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtended) {
  // Invalid.
  EXPECT_EQ(::lector::parse<long double>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(::lector::parse<long double>("NAN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<long double>("NAN").value()));
  ASSERT_TRUE(::lector::parse<long double>("NaN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<long double>("NaN").value()));
  ASSERT_TRUE(::lector::parse<long double>("nan").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<long double>("nan").value()));
  ASSERT_TRUE(::lector::parse<long double>("-NAN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<long double>("-NAN").value()));
  ASSERT_TRUE(::lector::parse<long double>("-NaN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<long double>("-NaN").value()));
  ASSERT_TRUE(::lector::parse<long double>("-nan").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<long double>("-nan").value()));

  // Infinity.
  EXPECT_EQ(::lector::parse<long double>("INF"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("Inf"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("inf"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("-INF"), -::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("-Inf"), -::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("-inf"), -::std::numeric_limits<long double>::infinity());

  // Zero.
  EXPECT_EQ(::lector::parse<long double>("0F"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0f"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0L"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0l"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0.0F"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0.0f"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0.0"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0.0L"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("0.0l"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0F"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0f"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0L"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0l"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0.0F"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0.0f"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0.0"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0.0L"), 0.0L);
  EXPECT_EQ(::lector::parse<long double>("-0.0l"), 0.0L);

  // Integer.
  EXPECT_EQ(::lector::parse<long double>("-10F"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10f"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10L"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10l"), -10.0L);

  // Decimal notation.
  EXPECT_EQ(::lector::parse<long double>("-3.14F"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14f"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14L"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14l"), -3.14L);

  // Scientific notation, small number.
  EXPECT_EQ(::lector::parse<long double>("-3.14E-12F"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E-12f"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E-12"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E-12L"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E-12l"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e-12F"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e-12f"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e-12"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e-12L"), -3.14E-12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e-12l"), -3.14E-12L);

  // Scientific notation, large number.
  EXPECT_EQ(::lector::parse<long double>("-3.14E12F"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E12f"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E12"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E12L"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E12l"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e12F"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e12f"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e12"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e12L"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e12l"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E+12F"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E+12f"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E+12"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E+12L"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14E+12l"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e+12F"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e+12f"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e+12"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e+12L"), -3.14E12L);
  EXPECT_EQ(::lector::parse<long double>("-3.14e+12l"), -3.14E12L);
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingle) {
  // Invalid.
  EXPECT_EQ(::lector::parse<float>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("-1.0E1000000000000000000000000000000"), ::std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(::lector::parse<float>("NAN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<float>("NAN").value()));
  ASSERT_TRUE(::lector::parse<float>("NaN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<float>("NaN").value()));
  ASSERT_TRUE(::lector::parse<float>("nan").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<float>("nan").value()));
  ASSERT_TRUE(::lector::parse<float>("-NAN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<float>("-NAN").value()));
  ASSERT_TRUE(::lector::parse<float>("-NaN").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<float>("-NaN").value()));
  ASSERT_TRUE(::lector::parse<float>("-nan").has_value());
  EXPECT_TRUE(::std::isnan(::lector::parse<float>("-nan").value()));

  // Infinity.
  EXPECT_EQ(::lector::parse<float>("INF"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("Inf"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("inf"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("-INF"), -::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("-Inf"), -::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("-inf"), -::std::numeric_limits<float>::infinity());

  // Zero.
  EXPECT_EQ(::lector::parse<float>("0F"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0f"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0L"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0l"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0.0F"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0.0f"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0.0"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0.0L"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("0.0l"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0F"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0f"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0L"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0l"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0.0F"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0.0f"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0.0"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0.0L"), 0.0F);
  EXPECT_EQ(::lector::parse<float>("-0.0l"), 0.0F);

  // Integer.
  EXPECT_EQ(::lector::parse<float>("-10F"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10f"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10L"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10l"), -10.0F);

  // Decimal notation.
  EXPECT_EQ(::lector::parse<float>("-3.14F"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14f"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14L"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14l"), -3.14F);

  // Scientific notation, small number.
  EXPECT_EQ(::lector::parse<float>("-3.14E-12F"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E-12f"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E-12"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E-12L"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E-12l"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e-12F"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e-12f"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e-12"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e-12L"), -3.14E-12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e-12l"), -3.14E-12F);

  // Scientific notation, large number.
  EXPECT_EQ(::lector::parse<float>("-3.14E12F"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E12f"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E12"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E12L"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E12l"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e12F"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e12f"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e12"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e12L"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e12l"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E+12F"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E+12f"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E+12"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E+12L"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14E+12l"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e+12F"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e+12f"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e+12"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e+12L"), -3.14E12F);
  EXPECT_EQ(::lector::parse<float>("-3.14e+12l"), -3.14E12F);
}

TEST(Lector, ParseNumberIntegerBits08) {
  EXPECT_EQ(::lector::parse<::std::int8_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("1000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("-1000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::int8_t>("-0"), 0);
  EXPECT_EQ(::lector::parse<::std::int8_t>("123"), 123);
  EXPECT_EQ(::lector::parse<::std::int8_t>("-123"), -123);
}

TEST(Lector, ParseNumberIntegerBits16) {
  EXPECT_EQ(::lector::parse<::std::int16_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("1000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("-1000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::int16_t>("-0"), 0);
  EXPECT_EQ(::lector::parse<::std::int16_t>("123"), 123);
  EXPECT_EQ(::lector::parse<::std::int16_t>("-123"), -123);
}

TEST(Lector, ParseNumberIntegerBits32) {
  EXPECT_EQ(::lector::parse<::std::int32_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("10000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("-10000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::int32_t>("-0"), 0);
  EXPECT_EQ(::lector::parse<::std::int32_t>("123"), 123);
  EXPECT_EQ(::lector::parse<::std::int32_t>("-123"), -123);
}

TEST(Lector, ParseNumberIntegerBits64) {
  EXPECT_EQ(::lector::parse<::std::int64_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("-1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::int64_t>("-0"), 0);
  EXPECT_EQ(::lector::parse<::std::int64_t>("123"), 123);
  EXPECT_EQ(::lector::parse<::std::int64_t>("-123"), -123);
}

TEST(Lector, ParseNumberNaturalBits08) {
  EXPECT_EQ(::lector::parse<::std::uint8_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("1000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("123"), 123);
}

TEST(Lector, ParseNumberNaturalBits16) {
  EXPECT_EQ(::lector::parse<::std::uint16_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("1000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("123"), 123);
}

TEST(Lector, ParseNumberNaturalBits32) {
  EXPECT_EQ(::lector::parse<::std::uint32_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("10000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("123"), 123);
}

TEST(Lector, ParseNumberNaturalBits64) {
  EXPECT_EQ(::lector::parse<::std::uint64_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("0"), 0);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("123"), 123);
}

TEST(Lector, ParseString) {
  EXPECT_EQ(::lector::parse<::std::string>(""), "");
  EXPECT_EQ(::lector::parse<::std::string>("Hello, world!"), "Hello, world!");
}

TEST(Lector, ParseStringView) {
  EXPECT_EQ(::lector::parse<::std::string_view>(""), "");
  EXPECT_EQ(::lector::parse<::std::string_view>("Hello, world!"), "Hello, world!");
}

TEST(Lector, PrintBoolean) {
  EXPECT_EQ(::lector::print(true), "true");
  EXPECT_EQ(::lector::print(false), "false");
}

TEST(Lector, PrintEnumeration) {
  EXPECT_EQ(::lector::print_enumeration<::lector::Importance>(::lector::Importance::Optional),
            "Optional");
  EXPECT_EQ(::lector::print_enumeration<::lector::Importance>(::lector::Importance::Required),
            "Required");
  EXPECT_EQ(
      ::lector::print_enumeration<::lector::Importance>(static_cast<::lector::Importance>(123)),
      "");

  EXPECT_EQ(::lector::print<::lector::Importance>(::lector::Importance::Optional), "Optional");
  EXPECT_EQ(::lector::print<::lector::Importance>(::lector::Importance::Required), "Required");
  EXPECT_EQ(::lector::print<::lector::Importance>(static_cast<::lector::Importance>(123)), "");
}

TEST(Lector, PrintFilesystemPath) {
  EXPECT_EQ(::lector::print<::std::filesystem::path>(""), "");
  EXPECT_EQ(::lector::print<::std::filesystem::path>("/some/path"), "/some/path");
  EXPECT_EQ(::lector::print<::std::filesystem::path>("file.txt"), "file.txt");
  EXPECT_EQ(::lector::print<::std::filesystem::path>("/some/file.txt"), "/some/file.txt");
}

TEST(Lector, PrintNumberFloatingPointPrecisionDouble) {
  EXPECT_EQ(::lector::print<double>(-std::numeric_limits<double>::quiet_NaN()), "-nan");
  EXPECT_EQ(::lector::print<double>(-std::numeric_limits<double>::infinity()), "-inf");
  EXPECT_EQ(::lector::print<double>(-4096.0), "-4096.00000000000000");
  EXPECT_EQ(::lector::print<double>(-512.0), "-512.000000000000000");
  EXPECT_EQ(::lector::print<double>(-64.0), "-64.0000000000000000");
  EXPECT_EQ(::lector::print<double>(-8.0), "-8.00000000000000000");
  EXPECT_EQ(::lector::print<double>(-1.0), "-1.00000000000000000");
  EXPECT_EQ(::lector::print<double>(-0.125), "-0.125000000000000000");
  EXPECT_EQ(::lector::print<double>(-0.015625), "-0.0156250000000000000");
  EXPECT_EQ(::lector::print<double>(-0.0), "0");
  EXPECT_EQ(::lector::print<double>(0.0), "0");
  EXPECT_EQ(::lector::print<double>(0.015625), "0.0156250000000000000");
  EXPECT_EQ(::lector::print<double>(0.125), "0.125000000000000000");
  EXPECT_EQ(::lector::print<double>(1.0), "1.00000000000000000");
  EXPECT_EQ(::lector::print<double>(8.0), "8.00000000000000000");
  EXPECT_EQ(::lector::print<double>(64.0), "64.0000000000000000");
  EXPECT_EQ(::lector::print<double>(512.0), "512.000000000000000");
  EXPECT_EQ(::lector::print<double>(4096.0), "4096.00000000000000");
  EXPECT_EQ(::lector::print<double>(std::numeric_limits<double>::infinity()), "inf");
  EXPECT_EQ(::lector::print<double>(std::numeric_limits<double>::quiet_NaN()), "nan");
}

TEST(Lector, PrintNumberFloatingPointPrecisionExtended) {
  EXPECT_EQ(::lector::print<long double>(-std::numeric_limits<long double>::quiet_NaN()), "-nan");
  EXPECT_EQ(::lector::print<long double>(-std::numeric_limits<long double>::infinity()), "-inf");
  EXPECT_EQ(::lector::print<long double>(-0.0L), "0");
  EXPECT_EQ(::lector::print<long double>(0.0L), "0");
  EXPECT_EQ(::lector::print<long double>(std::numeric_limits<long double>::infinity()), "inf");
  EXPECT_EQ(::lector::print<long double>(std::numeric_limits<long double>::quiet_NaN()), "nan");
}

TEST(Lector, PrintNumberFloatingPointPrecisionSingle) {
  EXPECT_EQ(::lector::print<float>(-std::numeric_limits<float>::quiet_NaN()), "-nan");
  EXPECT_EQ(::lector::print<float>(-std::numeric_limits<float>::infinity()), "-inf");
  EXPECT_EQ(::lector::print<float>(-4096.0F), "-4096.000000");
  EXPECT_EQ(::lector::print<float>(-512.0F), "-512.0000000");
  EXPECT_EQ(::lector::print<float>(-64.0F), "-64.00000000");
  EXPECT_EQ(::lector::print<float>(-8.0F), "-8.000000000");
  EXPECT_EQ(::lector::print<float>(-1.0F), "-1.000000000");
  EXPECT_EQ(::lector::print<float>(-0.125F), "-0.1250000000");
  EXPECT_EQ(::lector::print<float>(-0.015625F), "-0.01562500000");
  EXPECT_EQ(::lector::print<float>(-0.0F), "0");
  EXPECT_EQ(::lector::print<float>(0.0F), "0");
  EXPECT_EQ(::lector::print<float>(0.015625F), "0.01562500000");
  EXPECT_EQ(::lector::print<float>(0.125F), "0.1250000000");
  EXPECT_EQ(::lector::print<float>(1.0F), "1.000000000");
  EXPECT_EQ(::lector::print<float>(8.0F), "8.000000000");
  EXPECT_EQ(::lector::print<float>(64.0F), "64.00000000");
  EXPECT_EQ(::lector::print<float>(512.0F), "512.0000000");
  EXPECT_EQ(::lector::print<float>(4096.0F), "4096.000000");
  EXPECT_EQ(::lector::print<float>(std::numeric_limits<float>::infinity()), "inf");
  EXPECT_EQ(::lector::print<float>(std::numeric_limits<float>::quiet_NaN()), "nan");
}

TEST(Lector, PrintNumberIntegerBits08) {
  EXPECT_EQ(::lector::print<::std::int8_t>(-123), "-123");
  EXPECT_EQ(::lector::print<::std::int8_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::int8_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::int8_t>(123), "123");
}

TEST(Lector, PrintNumberIntegerBits16) {
  EXPECT_EQ(::lector::print<::std::int16_t>(-123), "-123");
  EXPECT_EQ(::lector::print<::std::int16_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::int16_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::int16_t>(123), "123");
}

TEST(Lector, PrintNumberIntegerBits32) {
  EXPECT_EQ(::lector::print<::std::int32_t>(-123), "-123");
  EXPECT_EQ(::lector::print<::std::int32_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::int32_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::int32_t>(123), "123");
}

TEST(Lector, PrintNumberIntegerBits64) {
  EXPECT_EQ(::lector::print<::std::int64_t>(-123), "-123");
  EXPECT_EQ(::lector::print<::std::int64_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::int64_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::int64_t>(123), "123");
}

TEST(Lector, PrintNumberNaturalBits08) {
  EXPECT_EQ(::lector::print<::std::uint8_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::uint8_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::uint8_t>(123), "123");
}

TEST(Lector, PrintNumberNaturalBits16) {
  EXPECT_EQ(::lector::print<::std::uint16_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::uint16_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::uint16_t>(123), "123");
}

TEST(Lector, PrintNumberNaturalBits32) {
  EXPECT_EQ(::lector::print<::std::uint32_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::uint32_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::uint32_t>(123), "123");
}

TEST(Lector, PrintNumberNaturalBits64) {
  EXPECT_EQ(::lector::print<::std::uint64_t>(-0), "0");
  EXPECT_EQ(::lector::print<::std::uint64_t>(0), "0");
  EXPECT_EQ(::lector::print<::std::uint64_t>(123), "123");
}

TEST(Lector, PrintString) {
  EXPECT_EQ(::lector::print<::std::string>(""), "");
  EXPECT_EQ(::lector::print<::std::string>("Hello, world!"), "Hello, world!");
}

TEST(Lector, PrintStringView) {
  EXPECT_EQ(::lector::print<::std::string_view>(""), "");
  EXPECT_EQ(::lector::print<::std::string_view>("Hello, world!"), "Hello, world!");
}

}  // namespace
