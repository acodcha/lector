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

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <istream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

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

/// @brief Default point in three-dimensional space.
inline constexpr ::test::Point FirstPoint{1.0F, 2.0F, 3.0F};

/// @brief Another point in three-dimensional space. Different from the default point.
inline constexpr ::test::Point SecondPoint{4.0F, 5.0F, 6.0F};

}  // namespace

}  // namespace test

namespace lector {

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

namespace {

TEST(Lector, ParseBoolean) {
  EXPECT_EQ(::lector::parse<bool>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>(" TRUE"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("TRUE "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>(" True"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("True "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>(" true"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("true "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>(" FALSE"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("FALSE "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>(" False"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("False "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>(" false"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<bool>("false "), ::std::nullopt);
  {
    const ::std::optional<bool> parsed{::lector::parse<bool>("TRUE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value());
  }
  {
    const ::std::optional<bool> parsed{::lector::parse<bool>("True")};
    EXPECT_TRUE(parsed.has_value() && parsed.value());
  }
  {
    const ::std::optional<bool> parsed{::lector::parse<bool>("true")};
    EXPECT_TRUE(parsed.has_value() && parsed.value());
  }
  {
    const ::std::optional<bool> parsed{::lector::parse<bool>("FALSE")};
    EXPECT_TRUE(parsed.has_value() && !parsed.value());
  }
  {
    const ::std::optional<bool> parsed{::lector::parse<bool>("False")};
    EXPECT_TRUE(parsed.has_value() && !parsed.value());
  }
  {
    const ::std::optional<bool> parsed{::lector::parse<bool>("false")};
    EXPECT_TRUE(parsed.has_value() && !parsed.value());
  }
}

TEST(Lector, ParseDataStructure) {
  EXPECT_EQ(::lector::parse<::test::Point>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0.0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0 0"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Point>("0.0 0.0"), ::std::nullopt);
  {
    const ::std::optional<::test::Point> parsed{::lector::parse<::test::Point>("1 2 3")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::FirstPoint);
  }
  {
    const ::std::optional<::test::Point> parsed{::lector::parse<::test::Point>("1.0 2.0 3.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::FirstPoint);
  }
  {
    const ::std::optional<::test::Point> parsed{
      ::lector::parse<::test::Point>("1.0E+00 2.0E+00 3.0E+00")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::FirstPoint);
  }
  {
    const ::std::optional<::test::Point> parsed{::lector::parse<::test::Point>("4 5 6")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::SecondPoint);
  }
  {
    const ::std::optional<::test::Point> parsed{::lector::parse<::test::Point>("4.0 5.0 6.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::SecondPoint);
  }
  {
    const ::std::optional<::test::Point> parsed{
      ::lector::parse<::test::Point>("4.0E+00 5.0E+00 6.0E+00")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::SecondPoint);
  }
}

TEST(Lector, ParseEnumeration) {
  static_assert(::lector::parse_enumeration<::test::Shape>("") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("Hello, world!") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("CiRcLe") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>(" Circle") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("Circle ") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("TrIaNgLe") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>(" Triangle") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("Triangle ") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("SqUaRe") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>(" Square") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("Square ") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("ReCtAnGlE") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>(" Rectangle") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("Rectangle ") == ::std::nullopt);
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("CIRCLE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Circle);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("Circle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Circle);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("circle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Circle);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("TRIANGLE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Triangle);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("Triangle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Triangle);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("triangle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Triangle);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("SQUARE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Square);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("Square")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Square);
  }
  {
    constexpr ::std::optional<::test::Shape> parsed{
      ::lector::parse_enumeration<::test::Shape>("square")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Square);
  }
  static_assert(::lector::parse_enumeration<::test::Shape>("RECTANGLE") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("Rectangle") == ::std::nullopt);
  static_assert(::lector::parse_enumeration<::test::Shape>("rectangle") == ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("CiRcLe"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>(" Circle"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("Circle "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("TrIaNgLe"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>(" Triangle"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("Triangle "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("SqUaRe"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>(" Square"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("Square "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("ReCtAnGlE"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>(" Rectangle"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::test::Shape>("Rectangle "), ::std::nullopt);
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("CIRCLE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Circle);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("Circle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Circle);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("circle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Circle);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("TRIANGLE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Triangle);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("Triangle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Triangle);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("triangle")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Triangle);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("SQUARE")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Square);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("Square")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Square);
  }
  {
    const ::std::optional<::test::Shape> parsed{::lector::parse<::test::Shape>("square")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::test::Shape::Square);
  }
}

TEST(Lector, ParseFilesystemPath) {
  {
    const ::std::optional<::std::filesystem::path> parsed{
      ::lector::parse<::std::filesystem::path>("")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::filesystem::path{});
  }
  {
    const ::std::optional<::std::filesystem::path> parsed{
      ::lector::parse<::std::filesystem::path>("some_path")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::filesystem::path{"some_path"});
  }
  {
    const ::std::optional<::std::filesystem::path> parsed{
      ::lector::parse<::std::filesystem::path>("some_file.txt")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::filesystem::path{"some_file.txt"});
  }
  {
    const ::std::optional<::std::filesystem::path> parsed{
      ::lector::parse<::std::filesystem::path>("/some/path")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::filesystem::path{"/some/path"});
  }
  {
    const ::std::optional<::std::filesystem::path> parsed{
      ::lector::parse<::std::filesystem::path>("/some/file.txt")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::filesystem::path{"/some/file.txt"});
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleInfinity) {
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+INF")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+Inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("INF")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("Inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-INF")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-Inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -::std::numeric_limits<double>::infinity());
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -::std::numeric_limits<double>::infinity());
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleInvalid) {
  EXPECT_EQ(::lector::parse<double>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("1.23E--4"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("1.23E++4"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<double>("  123 45   "), ::std::nullopt);
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotANumber) {
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotationDecimal) {
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotationInteger) {
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleNotationScientific) {
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23E-04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23e-04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23E-4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23e-4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23E04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23e04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23E4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23e4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23E+04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23e+04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23E+4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-1.23e+4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23E-04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23e-04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23E-4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23e-4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23E04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23e04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23E4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23e4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23E+04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23e+04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23E+4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -1.23e+4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionDoubleZero) {
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("-0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  -0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("+0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
  {
    const ::std::optional<double> parsed{::lector::parse<double>("  +0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedInfinity) {
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+INF")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == ::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+Inf")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == ::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+inf")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == ::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("INF")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == ::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("Inf")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == ::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("inf")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == ::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-INF")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == -::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-Inf")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == -::std::numeric_limits<long double>::infinity());
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-inf")};
    EXPECT_TRUE(
        parsed.has_value() && parsed.value() == -::std::numeric_limits<long double>::infinity());
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedInvalid) {
  EXPECT_EQ(::lector::parse<long double>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("-1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("1.23E--4"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("1.23E++4"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<long double>("  123 45   "), ::std::nullopt);
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotANumber) {
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotationDecimal) {
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23L);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotationInteger) {
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0L);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedNotationScientific) {
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23E-04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23e-04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23E-4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23e-4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23E04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23e04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23E4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23e4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23E+04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23e+04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23E+4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-1.23e+4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23E-04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23e-04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23E-4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23e-4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23E04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23e04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23E4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23e4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23E+04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23e+04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23E+4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -1.23e+4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4L);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionExtendedZero) {
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("-0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  -0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("+0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
  {
    const ::std::optional<long double> parsed{::lector::parse<long double>("  +0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0L);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleInfinity) {
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+INF")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+Inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("INF")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("Inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == ::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-INF")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-Inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -::std::numeric_limits<float>::infinity());
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-inf")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -::std::numeric_limits<float>::infinity());
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleInvalid) {
  EXPECT_EQ(::lector::parse<float>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("-1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("1.0E1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("1.23E--4"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("1.23E++4"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<float>("  123 45   "), ::std::nullopt);
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotANumber) {
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+NAN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+NaN")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+nan")};
    EXPECT_TRUE(parsed.has_value() && ::std::isnan(parsed.value()));
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotationDecimal) {
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +1.23")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +1.23   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 1.23F);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotationInteger) {
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +10")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +10   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 10.0F);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleNotationScientific) {
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23E-04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23e-04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23E-4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23e-4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23E04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23e04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23E4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23e4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23E+04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23e+04")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23E+4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-1.23e+4")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23E-04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23e-04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23E-4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23e-4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E-4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23E04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23e04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23E4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23e4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23E+04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23e+04   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23E+4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -1.23e+4   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == -1.23E4F);
  }
}

TEST(Lector, ParseNumberFloatingPointPrecisionSingleZero) {
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("-0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  -0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +0.0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("+0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
  {
    const ::std::optional<float> parsed{::lector::parse<float>("  +0.0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == 0.0F);
  }
}

TEST(Lector, ParseNumberIntegerBits08) {
  EXPECT_EQ(::lector::parse<::std::int8_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("-1000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("1000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int8_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(0));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("-123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(-123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  -123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(-123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("-123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(-123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  -123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(-123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
  {
    const ::std::optional<::std::int8_t> parsed{::lector::parse<::std::int8_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int8_t>(123));
  }
}

TEST(Lector, ParseNumberIntegerBits16) {
  EXPECT_EQ(::lector::parse<::std::int16_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("-100000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("100000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int16_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(0));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("-123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(-123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  -123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(-123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("-123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(-123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  -123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(-123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
  {
    const ::std::optional<::std::int16_t> parsed{::lector::parse<::std::int16_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int16_t>(123));
  }
}

TEST(Lector, ParseNumberIntegerBits32) {
  EXPECT_EQ(::lector::parse<::std::int32_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("-10000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("10000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int32_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(0));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("-123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(-123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  -123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(-123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("-123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(-123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  -123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(-123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
  {
    const ::std::optional<::std::int32_t> parsed{::lector::parse<::std::int32_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int32_t>(123));
  }
}

TEST(Lector, ParseNumberIntegerBits64) {
  EXPECT_EQ(::lector::parse<::std::int64_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("-1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("--123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  --123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("--123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  --123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::int64_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(0));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("-123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(-123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  -123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(-123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("-123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(-123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  -123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(-123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
  {
    const ::std::optional<::std::int64_t> parsed{::lector::parse<::std::int64_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::int64_t>(123));
  }
}

TEST(Lector, ParseNumberNaturalBits08) {
  EXPECT_EQ(::lector::parse<::std::uint8_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("1000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  -123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("-123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  -123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint8_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(0));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
  {
    const ::std::optional<::std::uint8_t> parsed{::lector::parse<::std::uint8_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint8_t>(123));
  }
}

TEST(Lector, ParseNumberNaturalBits16) {
  EXPECT_EQ(::lector::parse<::std::uint16_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("100000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  -123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("-123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  -123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint16_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(0));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
  {
    const ::std::optional<::std::uint16_t> parsed{::lector::parse<::std::uint16_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint16_t>(123));
  }
}

TEST(Lector, ParseNumberNaturalBits32) {
  EXPECT_EQ(::lector::parse<::std::uint32_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("10000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  -123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("-123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  -123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint32_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(0));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
  {
    const ::std::optional<::std::uint32_t> parsed{::lector::parse<::std::uint32_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint32_t>(123));
  }
}

TEST(Lector, ParseNumberNaturalBits64) {
  EXPECT_EQ(::lector::parse<::std::uint64_t>(""), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("Hello, world!"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("1000000000000000000000000000000"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("-123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  -123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("-123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  -123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  ++123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  ++123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  123abc"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  123abc   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  abc123"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  abc123   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  123 45"), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("123 45   "), ::std::nullopt);
  EXPECT_EQ(::lector::parse<::std::uint64_t>("  123 45   "), ::std::nullopt);
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("-0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  -0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("-0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  -0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("+0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  +0")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("+0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  +0   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(0));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("+123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  +123")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("+123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
  {
    const ::std::optional<::std::uint64_t> parsed{::lector::parse<::std::uint64_t>("  +123   ")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == static_cast<::std::uint64_t>(123));
  }
}

TEST(Lector, ParseString) {
  {
    const ::std::optional<::std::string> parsed{::lector::parse<::std::string>("")};
    EXPECT_TRUE(parsed.has_value() && parsed.value().empty());
  }
  {
    const ::std::optional<::std::string> parsed{::lector::parse<::std::string>("Hello, world!")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == "Hello, world!");
  }
}

TEST(Lector, ParseStringView) {
  {
    const ::std::optional<::std::string_view> parsed{::lector::parse<::std::string_view>("")};
    EXPECT_TRUE(parsed.has_value() && parsed.value().empty());
  }
  {
    const ::std::optional<::std::string_view> parsed{
      ::lector::parse<::std::string_view>("Hello, world!")};
    EXPECT_TRUE(parsed.has_value() && parsed.value() == "Hello, world!");
  }
}

}  // namespace
