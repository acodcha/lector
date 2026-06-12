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

#include "lector/parse.hpp"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

namespace test {

namespace {

/// @brief Color. Enumeration type used for testing the parsing of enumeration command line
/// arguments.
enum class Color : ::std::int8_t {
  /// @brief Red color.
  Red,

  /// @brief Green color.
  Green,

  /// @brief Blue color.
  Blue,
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
inline constexpr bool operator==(const ::test::Point& first, const ::test::Point& second) {
  return first.x == second.x && first.y == second.y && first.z == second.z;
}

/// @brief Input stream operator for the test::Point data structure. Populates a test::Point data
/// structure from an input stream.
inline ::std::istream& operator>>(::std::istream& input_stream, ::test::Point& point) {
  input_stream >> point.x >> point.y >> point.z;
  return input_stream;
}

/// @brief Default point in three-dimensional space.
inline constexpr ::test::Point FirstPoint{1.0F, 2.0F, 3.0F};

/// @brief Another point in three-dimensional space. Different from the default point.
inline constexpr ::test::Point SecondPoint{4.0F, 5.0F, 6.0F};

}  // namespace

}  // namespace test

namespace lector {

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

namespace {

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

TEST(Lector, ParseDataStructure) {
  EXPECT_EQ(::lector::parse<::test::Point>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0.0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0 0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0.0 0.0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("1 2 3"), ::test::FirstPoint);
  EXPECT_EQ(::lector::parse<::test::Point>("1.0 2.0 3.0"), ::test::FirstPoint);
  EXPECT_EQ(::lector::parse<::test::Point>("1.0E+0 2.0E+0 3.0E+0"), ::test::FirstPoint);
  EXPECT_EQ(::lector::parse<::test::Point>("4 5 6"), ::test::SecondPoint);
  EXPECT_EQ(::lector::parse<::test::Point>("4.0 5.0 6.0"), ::test::SecondPoint);
  EXPECT_EQ(::lector::parse<::test::Point>("4.0E+0 5.0E+0 6.0E+0"), ::test::SecondPoint);
}

TEST(Lector, ParseEnumeration) {
  static_assert(!::lector::parse_enumeration<::test::Color>("").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>("Hello, world!").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>(" Red").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>("Red ").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>(" Green").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>("Green ").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>(" Blue").has_value());
  static_assert(!::lector::parse_enumeration<::test::Color>("Blue ").has_value());
  static_assert(::lector::parse_enumeration<::test::Color>("RED").value() == ::test::Color::Red);
  static_assert(::lector::parse_enumeration<::test::Color>("Red").value() == ::test::Color::Red);
  static_assert(::lector::parse_enumeration<::test::Color>("red").value() == ::test::Color::Red);
  static_assert(
      ::lector::parse_enumeration<::test::Color>("GREEN").value() == ::test::Color::Green);
  static_assert(
      ::lector::parse_enumeration<::test::Color>("Green").value() == ::test::Color::Green);
  static_assert(
      ::lector::parse_enumeration<::test::Color>("green").value() == ::test::Color::Green);
  static_assert(::lector::parse_enumeration<::test::Color>("BLUE").value() == ::test::Color::Blue);
  static_assert(::lector::parse_enumeration<::test::Color>("Blue").value() == ::test::Color::Blue);
  static_assert(::lector::parse_enumeration<::test::Color>("blue").value() == ::test::Color::Blue);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>(" Red"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Red "), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>(" Green"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Green "), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>(" Blue"), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Blue "), ::std::nullopt);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("RED"), ::test::Color::Red);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Red"), ::test::Color::Red);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("red"), ::test::Color::Red);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("GREEN"), ::test::Color::Green);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Green"), ::test::Color::Green);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("green"), ::test::Color::Green);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("BLUE"), ::test::Color::Blue);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("Blue"), ::test::Color::Blue);
  EXPECT_EQ(::lector::parse_enumeration<::test::Color>("blue"), ::test::Color::Blue);
  EXPECT_EQ(::lector::parse<::test::Color>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>(" Red"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>("Red "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>(" Green"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>("Green "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>(" Blue"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>("Blue "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Color>("RED"), ::test::Color::Red);
  EXPECT_EQ(::lector::parse<::test::Color>("Red"), ::test::Color::Red);
  EXPECT_EQ(::lector::parse<::test::Color>("red"), ::test::Color::Red);
  EXPECT_EQ(::lector::parse<::test::Color>("GREEN"), ::test::Color::Green);
  EXPECT_EQ(::lector::parse<::test::Color>("Green"), ::test::Color::Green);
  EXPECT_EQ(::lector::parse<::test::Color>("green"), ::test::Color::Green);
  EXPECT_EQ(::lector::parse<::test::Color>("BLUE"), ::test::Color::Blue);
  EXPECT_EQ(::lector::parse<::test::Color>("Blue"), ::test::Color::Blue);
  EXPECT_EQ(::lector::parse<::test::Color>("blue"), ::test::Color::Blue);
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

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleInfinity) {
  EXPECT_EQ(::lector::parse<double>("INF"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("Inf"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("inf"), ::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("-INF"), -::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("-Inf"), -::std::numeric_limits<double>::infinity());
  EXPECT_EQ(::lector::parse<double>("-inf"), -::std::numeric_limits<double>::infinity());
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleInvalid) {
  EXPECT_EQ(::lector::parse<double>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotANumber) {
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
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotationDecimal) {
  EXPECT_EQ(::lector::parse<double>("-3.14F"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14f"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14L"), -3.14);
  EXPECT_EQ(::lector::parse<double>("-3.14l"), -3.14);
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotationInteger) {
  EXPECT_EQ(::lector::parse<double>("-10F"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10f"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10L"), -10.0);
  EXPECT_EQ(::lector::parse<double>("-10l"), -10.0);
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotationScientific) {
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

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleZero) {
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
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedInfinity) {
  EXPECT_EQ(::lector::parse<long double>("INF"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("Inf"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("inf"), ::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("-INF"), -::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("-Inf"), -::std::numeric_limits<long double>::infinity());
  EXPECT_EQ(::lector::parse<long double>("-inf"), -::std::numeric_limits<long double>::infinity());
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedInvalid) {
  EXPECT_EQ(::lector::parse<long double>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotANumber) {
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
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotationInteger) {
  EXPECT_EQ(::lector::parse<long double>("-10F"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10f"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10L"), -10.0L);
  EXPECT_EQ(::lector::parse<long double>("-10l"), -10.0L);
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotationDecimal) {
  EXPECT_EQ(::lector::parse<long double>("-3.14F"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14f"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14L"), -3.14L);
  EXPECT_EQ(::lector::parse<long double>("-3.14l"), -3.14L);
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotationScientific) {
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

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedZero) {
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
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleInfinity) {
  EXPECT_EQ(::lector::parse<float>("INF"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("Inf"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("inf"), ::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("-INF"), -::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("-Inf"), -::std::numeric_limits<float>::infinity());
  EXPECT_EQ(::lector::parse<float>("-inf"), -::std::numeric_limits<float>::infinity());
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleInvalid) {
  EXPECT_EQ(::lector::parse<float>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("-1.0E1000000000000000000000000000000"), ::std::nullopt);
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotANumber) {
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
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotationInteger) {
  EXPECT_EQ(::lector::parse<float>("-10F"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10f"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10L"), -10.0F);
  EXPECT_EQ(::lector::parse<float>("-10l"), -10.0F);
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotationDecimal) {
  EXPECT_EQ(::lector::parse<float>("-3.14F"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14f"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14L"), -3.14F);
  EXPECT_EQ(::lector::parse<float>("-3.14l"), -3.14F);
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotationScientific) {
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

TEST(Lector, ParseNumberFloatingPointPrecisionSingleZero) {
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

}  // namespace
