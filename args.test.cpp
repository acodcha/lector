// Copyright © 2026, Alexandre Coderre-Chabot.

// This file is part of Args (https://github.com/acodcha/args), a C++ library for parsing command
// line arguments. Args is licensed under the MIT License (https://mit-license.org).

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//   - The above copyright notice and this permission notice shall be included in all copies or
//     substantial portions of the Software.
//   - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
//     BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//     NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//     DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "args.hpp"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>

namespace {

namespace test {

/// @brief Enumeration type used for testing the parsing of enumeration command line arguments.
enum class Color : int8_t {
  Red,
  Green,
  Blue,
};

}  // namespace test

}  // namespace

namespace args {

/// @brief Specialization of the args::Printings constant for the test::Color enumeration.
template <>
inline constexpr ::std::array<::args::Printing<::test::Color>, 3> Printings<::test::Color>{
  {
   {::test::Color::Red, "Red"},
   {::test::Color::Green, "Green"},
   {::test::Color::Blue, "Blue"},
   }
};

/// @brief Specialization of the args::Spellings constant for the test::Color enumeration.
template <>
inline constexpr ::std::array<::args::Spelling<::test::Color>, 9> Spellings<::test::Color>{
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

}  // namespace args

namespace {

namespace test {

/// @brief Labels of the command line arguments used for testing.
enum class Label : int8_t {
  Title,
  OutputDirectory,
  Color,
  Iterations,
  Tolerance,
  Help,
};

}  // namespace test

TEST(Args, ArgumentConstructorBooleanBasic) {
  const ::args::Argument<::test::Label::Help, bool> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorBooleanNoDefaultValue) {
  const ::args::Argument<::test::Label::Help, bool> argument{
    {"-h", "--help"},
    "Print usage information.",
    ::args::Importance::Optional,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Help);
  const std::vector<std::string> expected_keys{"-h", "--help"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Print usage information.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorEnumerationBasic) {
  const ::args::Argument<::test::Label::Color, ::test::Color> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorEnumerationNoDefaultValue) {
  const ::args::Argument<::test::Label::Color, ::test::Color> argument{
    {"-c", "--color"},
    "Main output color.",
    ::args::Importance::Required,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  const std::vector<std::string> expected_keys{"-c", "--color"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Main output color.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorEnumerationWithDefaultValue) {
  const ::args::Argument<::test::Label::Color, ::test::Color> argument{
    {"-c", "--color"},
    "Main output color.",
    ::args::Importance::Optional,
    ::test::Color::Red,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Color);
  const std::vector<std::string> expected_keys{"-c", "--color"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Main output color.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::test::Color::Red);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::test::Color::Red);
}

TEST(Args, ArgumentConstructorFilesystemPathBasic) {
  const ::args::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument;
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorFilesystemPathNoDefaultValue) {
  const ::args::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    {"-o", "--output"},
    "Output directory.",
    ::args::Importance::Required,
  };
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const std::vector<std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorFilesystemPathWithDefaultValue) {
  const ::args::Argument<::test::Label::OutputDirectory, ::std::filesystem::path> argument{
    {"-o", "--output"},
    "Output directory.",
    ::args::Importance::Optional,
    ::std::filesystem::path("/some/path"),
  };
  EXPECT_EQ(argument.label(), ::test::Label::OutputDirectory);
  const std::vector<std::string> expected_keys{"-o", "--output"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Output directory.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), ::std::filesystem::path("/some/path"));
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::filesystem::path("/some/path"));
}

TEST(Args, ArgumentConstructorNumberDoubleBasic) {
  const ::args::Argument<::test::Label::Tolerance, double> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorNumberDoubleNoDefaultValue) {
  const ::args::Argument<::test::Label::Tolerance, double> argument{
    {"-t", "--tolerance"},
    "Tolerance value.",
    ::args::Importance::Required,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  const std::vector<std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorNumberDoubleWithDefaultValue) {
  const ::args::Argument<::test::Label::Tolerance, double> argument{
    {"-t", "--tolerance"},
    "Tolerance value.",
    ::args::Importance::Optional,
    1.0E-3,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Tolerance);
  const std::vector<std::string> expected_keys{"-t", "--tolerance"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Tolerance value.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 1.0E-3);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 1.0E-3);
}

TEST(Args, ArgumentConstructorNumberIntegerBasic) {
  const ::args::Argument<::test::Label::Iterations, int32_t> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorNumberIntegerNoDefaultValue) {
  const ::args::Argument<::test::Label::Iterations, int32_t> argument{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Required,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorNumberIntegerWithDefaultValue) {
  const ::args::Argument<::test::Label::Iterations, int32_t> argument{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
    100,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 100);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 100);
}

TEST(Args, ArgumentConstructorStringBasic) {
  const ::args::Argument<::test::Label::Title, ::std::string> argument;
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  EXPECT_TRUE(argument.keys().empty());
  EXPECT_TRUE(argument.description().empty());
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorStringNoDefaultValue) {
  const ::args::Argument<::test::Label::Title, ::std::string> argument{
    {"-t", "--title"},
    "Title of the application.",
    ::args::Importance::Required,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const std::vector<std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Title of the application.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Required);
  EXPECT_EQ(argument.default_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), ::std::nullopt);
}

TEST(Args, ArgumentConstructorStringWithDefaultValue) {
  const ::args::Argument<::test::Label::Title, ::std::string> argument{
    {"-t", "--title"},
    "Title of the report.",
    ::args::Importance::Optional,
    "My Report",
  };
  EXPECT_EQ(argument.label(), ::test::Label::Title);
  const std::vector<std::string> expected_keys{"-t", "--title"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Title of the report.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), "My Report");
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), "My Report");
}

TEST(Args, ArgumentCopyConstructor) {
  const ::args::Argument<::test::Label::Iterations, int32_t> first{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
    100,
  };
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::args::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
  const ::args::Argument<::test::Label::Iterations, int32_t> second{first};
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::args::Importance::Optional);
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
}

TEST(Args, ArgumentCopyAssignmentOperator) {
  const ::args::Argument<::test::Label::Iterations, int32_t> first{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
    100,
  };
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::args::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
  ::args::Argument<::test::Label::Iterations, int32_t> second;
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.importance(), ::args::Importance::Required);
  EXPECT_EQ(second.default_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), ::std::nullopt);
  second = first;
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::args::Importance::Optional);
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
}

TEST(Args, ArgumentMoveConstructor) {
  ::args::Argument<::test::Label::Iterations, int32_t> first{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
    100,
  };
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::args::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
  const ::args::Argument<::test::Label::Iterations, int32_t> second{std::move(first)};
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::args::Importance::Optional);
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
}

TEST(Args, ArgumentMoveAssignmentOperator) {
  ::args::Argument<::test::Label::Iterations, int32_t> first{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
    100,
  };
  EXPECT_EQ(first.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(first.keys(), expected_keys);
  EXPECT_EQ(first.description(), "Number of iterations.");
  EXPECT_EQ(first.importance(), ::args::Importance::Optional);
  EXPECT_EQ(first.default_value(), 100);
  EXPECT_EQ(first.parsed_value(), ::std::nullopt);
  EXPECT_EQ(first.parsed_or_default_value(), 100);
  ::args::Argument<::test::Label::Iterations, int32_t> second;
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_TRUE(second.keys().empty());
  EXPECT_TRUE(second.description().empty());
  EXPECT_EQ(second.importance(), ::args::Importance::Required);
  EXPECT_EQ(second.default_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), ::std::nullopt);
  second = std::move(first);
  EXPECT_EQ(second.label(), ::test::Label::Iterations);
  EXPECT_EQ(second.keys(), expected_keys);
  EXPECT_EQ(second.description(), "Number of iterations.");
  EXPECT_EQ(second.importance(), ::args::Importance::Optional);
  EXPECT_EQ(second.default_value(), 100);
  EXPECT_EQ(second.parsed_value(), ::std::nullopt);
  EXPECT_EQ(second.parsed_or_default_value(), 100);
}

TEST(Args, ArgumentSetParsedValue) {
  ::args::Argument<::test::Label::Iterations, int32_t> argument{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
    100,
  };
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  const std::vector<std::string> expected_keys{"-i", "--iterations"};
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 100);
  EXPECT_EQ(argument.parsed_value(), ::std::nullopt);
  EXPECT_EQ(argument.parsed_or_default_value(), 100);
  argument.set(200);
  EXPECT_EQ(argument.label(), ::test::Label::Iterations);
  EXPECT_EQ(argument.keys(), expected_keys);
  EXPECT_EQ(argument.description(), "Number of iterations.");
  EXPECT_EQ(argument.importance(), ::args::Importance::Optional);
  EXPECT_EQ(argument.default_value(), 100);
  EXPECT_EQ(argument.parsed_value(), 200);
  EXPECT_EQ(argument.parsed_or_default_value(), 200);
}

/// @brief Helper function that creates an invalid args::Argument with an optional non-boolean type
/// and no default value.
void create_invalid_argument_with_missing_default_value() {
  const ::args::Argument<::test::Label::Iterations, int32_t> argument{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Optional,
  };
}

/// @brief Helper function that creates an invalid args::Argument with a required non-boolean type
/// and a default value.
void create_invalid_argument_with_superfluous_default_value() {
  const ::args::Argument<::test::Label::Iterations, int32_t> argument{
    {"-i", "--iterations"},
    "Number of iterations.",
    ::args::Importance::Required,
    100,
  };
}

/// @brief Helper function that creates an invalid args::Argument with a boolean type and a default
/// value.
void create_invalid_boolean_argument_with_default_value() {
  const ::args::Argument<::test::Label::Help, bool> argument{
    {"-h", "--help"},
    "Print usage information.",
    ::args::Importance::Required,
    true,
  };
}

TEST(Args, ArgumentConstructorInvalid) {
  EXPECT_ANY_THROW(create_invalid_argument_with_missing_default_value());
  EXPECT_ANY_THROW(create_invalid_argument_with_superfluous_default_value());
  EXPECT_ANY_THROW(create_invalid_boolean_argument_with_default_value());
}

TEST(Args, ParseBoolean) {
  EXPECT_EQ(::args::parse<bool>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<bool>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<bool>("TRUE"), true);
  EXPECT_EQ(::args::parse<bool>("True"), true);
  EXPECT_EQ(::args::parse<bool>("true"), true);
  EXPECT_EQ(::args::parse<bool>("1"), true);
  EXPECT_EQ(::args::parse<bool>("FALSE"), false);
  EXPECT_EQ(::args::parse<bool>("False"), false);
  EXPECT_EQ(::args::parse<bool>("false"), false);
  EXPECT_EQ(::args::parse<bool>("0"), false);
}

TEST(Args, ParseEnumeration) {
  static_assert(::args::parse_enumeration<::args::Importance>("Optional").value()
                == ::args::Importance::Optional);
  static_assert(!::args::parse_enumeration<::args::Importance>("Hello, world!").has_value());

  EXPECT_EQ(::args::parse_enumeration<::args::Importance>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse_enumeration<::args::Importance>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse_enumeration<::args::Importance>(" Optional"), ::std::nullopt);
  EXPECT_EQ(::args::parse_enumeration<::args::Importance>("Optional "), ::std::nullopt);
  EXPECT_EQ(::args::parse_enumeration<::args::Importance>(" Required"), ::std::nullopt);
  EXPECT_EQ(::args::parse_enumeration<::args::Importance>("Required "), ::std::nullopt);
  EXPECT_EQ(
      ::args::parse_enumeration<::args::Importance>("OPTIONAL"), ::args::Importance::Optional);
  EXPECT_EQ(
      ::args::parse_enumeration<::args::Importance>("Optional"), ::args::Importance::Optional);
  EXPECT_EQ(
      ::args::parse_enumeration<::args::Importance>("optional"), ::args::Importance::Optional);
  EXPECT_EQ(
      ::args::parse_enumeration<::args::Importance>("REQUIRED"), ::args::Importance::Required);
  EXPECT_EQ(
      ::args::parse_enumeration<::args::Importance>("Required"), ::args::Importance::Required);
  EXPECT_EQ(
      ::args::parse_enumeration<::args::Importance>("required"), ::args::Importance::Required);

  EXPECT_EQ(::args::parse<::args::Importance>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<::args::Importance>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<::args::Importance>(" Optional"), ::std::nullopt);
  EXPECT_EQ(::args::parse<::args::Importance>("Optional "), ::std::nullopt);
  EXPECT_EQ(::args::parse<::args::Importance>(" Required"), ::std::nullopt);
  EXPECT_EQ(::args::parse<::args::Importance>("Required "), ::std::nullopt);
  EXPECT_EQ(::args::parse<::args::Importance>("OPTIONAL"), ::args::Importance::Optional);
  EXPECT_EQ(::args::parse<::args::Importance>("Optional"), ::args::Importance::Optional);
  EXPECT_EQ(::args::parse<::args::Importance>("optional"), ::args::Importance::Optional);
  EXPECT_EQ(::args::parse<::args::Importance>("REQUIRED"), ::args::Importance::Required);
  EXPECT_EQ(::args::parse<::args::Importance>("Required"), ::args::Importance::Required);
  EXPECT_EQ(::args::parse<::args::Importance>("required"), ::args::Importance::Required);
}

TEST(Args, ParseFilesystemPath) {
  EXPECT_EQ(::args::parse<::std::filesystem::path>(""), ::std::filesystem::path{});
  EXPECT_EQ(
      ::args::parse<::std::filesystem::path>("some_path"), ::std::filesystem::path{"some_path"});
  EXPECT_EQ(::args::parse<::std::filesystem::path>("some_file.txt"),
            ::std::filesystem::path{"some_file.txt"});
  EXPECT_EQ(
      ::args::parse<::std::filesystem::path>("/some/path"), ::std::filesystem::path{"/some/path"});
  EXPECT_EQ(::args::parse<::std::filesystem::path>("/some/file.txt"),
            ::std::filesystem::path{"/some/file.txt"});
}

TEST(Args, ParseNumberDouble) {
  // Invalid.
  EXPECT_EQ(::args::parse<double>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(::args::parse<double>("NAN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<double>("NAN").value()));
  ASSERT_TRUE(::args::parse<double>("NaN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<double>("NaN").value()));
  ASSERT_TRUE(::args::parse<double>("nan").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<double>("nan").value()));
  ASSERT_TRUE(::args::parse<double>("-NAN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<double>("-NAN").value()));
  ASSERT_TRUE(::args::parse<double>("-NaN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<double>("-NaN").value()));
  ASSERT_TRUE(::args::parse<double>("-nan").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<double>("-nan").value()));

  // Infinity.
  EXPECT_EQ(::args::parse<double>("INF"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::args::parse<double>("Inf"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::args::parse<double>("inf"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::args::parse<double>("-INF"), -::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::args::parse<double>("-Inf"), -::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::args::parse<double>("-inf"), -::std::numeric_limits<double>::infinity());

  // Zero.
  EXPECT_EQ(::args::parse<double>("0F"), 0.0);
  EXPECT_EQ(::args::parse<double>("0f"), 0.0);
  EXPECT_EQ(::args::parse<double>("0"), 0.0);
  EXPECT_EQ(::args::parse<double>("0L"), 0.0);
  EXPECT_EQ(::args::parse<double>("0l"), 0.0);
  EXPECT_EQ(::args::parse<double>("0.0F"), 0.0);
  EXPECT_EQ(::args::parse<double>("0.0f"), 0.0);
  EXPECT_EQ(::args::parse<double>("0.0"), 0.0);
  EXPECT_EQ(::args::parse<double>("0.0L"), 0.0);
  EXPECT_EQ(::args::parse<double>("0.0l"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0F"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0f"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0L"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0l"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0.0F"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0.0f"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0.0"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0.0L"), 0.0);
  EXPECT_EQ(::args::parse<double>("-0.0l"), 0.0);

  // Integer.
  EXPECT_EQ(::args::parse<double>("-10F"), -10.0);
  EXPECT_EQ(::args::parse<double>("-10f"), -10.0);
  EXPECT_EQ(::args::parse<double>("-10"), -10.0);
  EXPECT_EQ(::args::parse<double>("-10L"), -10.0);
  EXPECT_EQ(::args::parse<double>("-10l"), -10.0);

  // Decimal notation.
  EXPECT_EQ(::args::parse<double>("-3.14F"), -3.14);
  EXPECT_EQ(::args::parse<double>("-3.14f"), -3.14);
  EXPECT_EQ(::args::parse<double>("-3.14"), -3.14);
  EXPECT_EQ(::args::parse<double>("-3.14L"), -3.14);
  EXPECT_EQ(::args::parse<double>("-3.14l"), -3.14);

  // Scientific notation, small number.
  EXPECT_EQ(::args::parse<double>("-3.14E-12F"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14E-12f"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14E-12"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14E-12L"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14E-12l"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14e-12F"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14e-12f"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14e-12"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14e-12L"), -3.14E-12);
  EXPECT_EQ(::args::parse<double>("-3.14e-12l"), -3.14E-12);

  // Scientific notation, large number.
  EXPECT_EQ(::args::parse<double>("-3.14E12F"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E12f"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E12"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E12L"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E12l"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e12F"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e12f"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e12"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e12L"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e12l"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E+12F"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E+12f"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E+12"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E+12L"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14E+12l"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e+12F"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e+12f"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e+12"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e+12L"), -3.14E12);
  EXPECT_EQ(::args::parse<double>("-3.14e+12l"), -3.14E12);
}

TEST(Args, ParseNumberFloat) {
  // Invalid.
  EXPECT_EQ(::args::parse<float>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<float>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<float>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<float>("-1.0E1000000000000000000000000000000"), ::std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(::args::parse<float>("NAN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<float>("NAN").value()));
  ASSERT_TRUE(::args::parse<float>("NaN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<float>("NaN").value()));
  ASSERT_TRUE(::args::parse<float>("nan").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<float>("nan").value()));
  ASSERT_TRUE(::args::parse<float>("-NAN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<float>("-NAN").value()));
  ASSERT_TRUE(::args::parse<float>("-NaN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<float>("-NaN").value()));
  ASSERT_TRUE(::args::parse<float>("-nan").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<float>("-nan").value()));

  // Infinity.
  EXPECT_EQ(::args::parse<float>("INF"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::args::parse<float>("Inf"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::args::parse<float>("inf"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::args::parse<float>("-INF"), -::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::args::parse<float>("-Inf"), -::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::args::parse<float>("-inf"), -::std::numeric_limits<float>::infinity());

  // Zero.
  EXPECT_EQ(::args::parse<float>("0F"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0f"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0L"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0l"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0.0F"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0.0f"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0.0"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0.0L"), 0.0F);
  EXPECT_EQ(::args::parse<float>("0.0l"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0F"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0f"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0L"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0l"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0.0F"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0.0f"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0.0"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0.0L"), 0.0F);
  EXPECT_EQ(::args::parse<float>("-0.0l"), 0.0F);

  // Integer.
  EXPECT_EQ(::args::parse<float>("-10F"), -10.0F);
  EXPECT_EQ(::args::parse<float>("-10f"), -10.0F);
  EXPECT_EQ(::args::parse<float>("-10"), -10.0F);
  EXPECT_EQ(::args::parse<float>("-10L"), -10.0F);
  EXPECT_EQ(::args::parse<float>("-10l"), -10.0F);

  // Decimal notation.
  EXPECT_EQ(::args::parse<float>("-3.14F"), -3.14F);
  EXPECT_EQ(::args::parse<float>("-3.14f"), -3.14F);
  EXPECT_EQ(::args::parse<float>("-3.14"), -3.14F);
  EXPECT_EQ(::args::parse<float>("-3.14L"), -3.14F);
  EXPECT_EQ(::args::parse<float>("-3.14l"), -3.14F);

  // Scientific notation, small number.
  EXPECT_EQ(::args::parse<float>("-3.14E-12F"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14E-12f"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14E-12"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14E-12L"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14E-12l"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14e-12F"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14e-12f"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14e-12"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14e-12L"), -3.14E-12F);
  EXPECT_EQ(::args::parse<float>("-3.14e-12l"), -3.14E-12F);

  // Scientific notation, large number.
  EXPECT_EQ(::args::parse<float>("-3.14E12F"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E12f"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E12"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E12L"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E12l"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e12F"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e12f"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e12"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e12L"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e12l"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E+12F"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E+12f"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E+12"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E+12L"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14E+12l"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e+12F"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e+12f"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e+12"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e+12L"), -3.14E12F);
  EXPECT_EQ(::args::parse<float>("-3.14e+12l"), -3.14E12F);
}

TEST(Args, ParseNumberInteger8) {
  EXPECT_EQ(::args::parse<int8_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<int8_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int8_t>("1000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int8_t>("-1000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int8_t>("0"), 0);
  EXPECT_EQ(::args::parse<int8_t>("-0"), 0);
  EXPECT_EQ(::args::parse<int8_t>("123"), 123);
  EXPECT_EQ(::args::parse<int8_t>("-123"), -123);
}

TEST(Args, ParseNumberInteger16) {
  EXPECT_EQ(::args::parse<int16_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<int16_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int16_t>("1000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int16_t>("-1000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int16_t>("0"), 0);
  EXPECT_EQ(::args::parse<int16_t>("-0"), 0);
  EXPECT_EQ(::args::parse<int16_t>("123"), 123);
  EXPECT_EQ(::args::parse<int16_t>("-123"), -123);
}

TEST(Args, ParseNumberInteger32) {
  EXPECT_EQ(::args::parse<int32_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<int32_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int32_t>("10000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int32_t>("-10000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int32_t>("0"), 0);
  EXPECT_EQ(::args::parse<int32_t>("-0"), 0);
  EXPECT_EQ(::args::parse<int32_t>("123"), 123);
  EXPECT_EQ(::args::parse<int32_t>("-123"), -123);
}

TEST(Args, ParseNumberInteger64) {
  EXPECT_EQ(::args::parse<int64_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<int64_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int64_t>("1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int64_t>("-1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<int64_t>("0"), 0);
  EXPECT_EQ(::args::parse<int64_t>("-0"), 0);
  EXPECT_EQ(::args::parse<int64_t>("123"), 123);
  EXPECT_EQ(::args::parse<int64_t>("-123"), -123);
}

TEST(Args, ParseNumberLongDouble) {
  // Invalid.
  EXPECT_EQ(::args::parse<long double>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<long double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<long double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<long double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(::args::parse<long double>("NAN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<long double>("NAN").value()));
  ASSERT_TRUE(::args::parse<long double>("NaN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<long double>("NaN").value()));
  ASSERT_TRUE(::args::parse<long double>("nan").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<long double>("nan").value()));
  ASSERT_TRUE(::args::parse<long double>("-NAN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<long double>("-NAN").value()));
  ASSERT_TRUE(::args::parse<long double>("-NaN").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<long double>("-NaN").value()));
  ASSERT_TRUE(::args::parse<long double>("-nan").has_value());
  EXPECT_TRUE(::std::isnan(::args::parse<long double>("-nan").value()));

  // Infinity.
  EXPECT_EQ(::args::parse<long double>("INF"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::args::parse<long double>("Inf"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::args::parse<long double>("inf"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::args::parse<long double>("-INF"), -::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::args::parse<long double>("-Inf"), -::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::args::parse<long double>("-inf"), -::std::numeric_limits<long double>::infinity());

  // Zero.
  EXPECT_EQ(::args::parse<long double>("0F"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0f"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0L"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0l"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0.0F"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0.0f"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0.0"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0.0L"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("0.0l"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0F"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0f"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0L"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0l"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0.0F"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0.0f"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0.0"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0.0L"), 0.0L);
  EXPECT_EQ(::args::parse<long double>("-0.0l"), 0.0L);

  // Integer.
  EXPECT_EQ(::args::parse<long double>("-10F"), -10.0L);
  EXPECT_EQ(::args::parse<long double>("-10f"), -10.0L);
  EXPECT_EQ(::args::parse<long double>("-10"), -10.0L);
  EXPECT_EQ(::args::parse<long double>("-10L"), -10.0L);
  EXPECT_EQ(::args::parse<long double>("-10l"), -10.0L);

  // Decimal notation.
  EXPECT_EQ(::args::parse<long double>("-3.14F"), -3.14L);
  EXPECT_EQ(::args::parse<long double>("-3.14f"), -3.14L);
  EXPECT_EQ(::args::parse<long double>("-3.14"), -3.14L);
  EXPECT_EQ(::args::parse<long double>("-3.14L"), -3.14L);
  EXPECT_EQ(::args::parse<long double>("-3.14l"), -3.14L);

  // Scientific notation, small number.
  EXPECT_EQ(::args::parse<long double>("-3.14E-12F"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E-12f"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E-12"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E-12L"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E-12l"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e-12F"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e-12f"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e-12"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e-12L"), -3.14E-12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e-12l"), -3.14E-12L);

  // Scientific notation, large number.
  EXPECT_EQ(::args::parse<long double>("-3.14E12F"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E12f"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E12"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E12L"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E12l"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e12F"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e12f"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e12"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e12L"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e12l"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E+12F"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E+12f"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E+12"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E+12L"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14E+12l"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e+12F"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e+12f"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e+12"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e+12L"), -3.14E12L);
  EXPECT_EQ(::args::parse<long double>("-3.14e+12l"), -3.14E12L);
}

TEST(Args, ParseNumberNatural8) {
  EXPECT_EQ(::args::parse<uint8_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint8_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint8_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint8_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint8_t>("1000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint8_t>("0"), 0);
  EXPECT_EQ(::args::parse<uint8_t>("123"), 123);
}

TEST(Args, ParseNumberNatural16) {
  EXPECT_EQ(::args::parse<uint16_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint16_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint16_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint16_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint16_t>("1000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint16_t>("0"), 0);
  EXPECT_EQ(::args::parse<uint16_t>("123"), 123);
}

TEST(Args, ParseNumberNatural32) {
  EXPECT_EQ(::args::parse<uint32_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint32_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint32_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint32_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint32_t>("10000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint32_t>("0"), 0);
  EXPECT_EQ(::args::parse<uint32_t>("123"), 123);
}

TEST(Args, ParseNumberNatural64) {
  EXPECT_EQ(::args::parse<uint64_t>(""), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint64_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint64_t>("-0"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint64_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint64_t>("1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::args::parse<uint64_t>("0"), 0);
  EXPECT_EQ(::args::parse<uint64_t>("123"), 123);
}

TEST(Args, ParseString) {
  EXPECT_EQ(::args::parse<::std::string>(""), ::std::string{""});
  EXPECT_EQ(::args::parse<::std::string>("Hello, world!"), ::std::string{"Hello, world!"});
}

TEST(Args, PrintEnumeration) {
  EXPECT_EQ(
      ::args::print_enumeration<::args::Importance>(::args::Importance::Optional), "Optional");
  EXPECT_EQ(
      ::args::print_enumeration<::args::Importance>(::args::Importance::Required), "Required");
  EXPECT_EQ(
      ::args::print_enumeration<::args::Importance>(static_cast<::args::Importance>(123)), "");
}

}  // namespace
