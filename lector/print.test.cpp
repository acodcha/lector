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

#include "lector/print.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

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

  /// @brief Rectangle shape.
  Rectangle,
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

/// @brief Performs a round-trip test of printing a finite floating-point number, parsing the
/// printed value, and checking that the parsed value exactly matches the original floating-point
/// number. Called by test::print_floating().
/// @tparam FloatingPointType A floating-point number type: float, double, or long double.
/// @param[in] value The finite floating-point number to test.
template <typename FloatingPointType>
void print_floating_finite(const FloatingPointType value) {
  const std::string printed{lector::print<FloatingPointType>(value)};
  std::istringstream input_stream{printed};
  FloatingPointType parsed_value;
  input_stream >> parsed_value;
  EXPECT_EQ(value, parsed_value);
}

/// @brief Performs a round-trip test of printing a non-finite floating-point number, parsing the
/// printed value, and checking that the parsed value exactly matches the original floating-point
/// number. Called by test::print_floating().
/// @tparam FloatingPointType A floating-point number type: float, double, or long double.
/// @param[in] value The non-finite floating-point number to test.
template <typename FloatingPointType>
void print_floating_non_finite(const FloatingPointType value) {
  const std::string printed{lector::print<FloatingPointType>(value)};
  if (std::isnan(value)) {
    EXPECT_EQ(printed, "nan");
  } else if (value < 0.0F) {
    EXPECT_EQ(printed, "-inf");
  } else {
    EXPECT_EQ(printed, "inf");
  }
}

/// @brief Performs a round-trip test of printing a floating-point number, parsing the printed
/// value, and checking that the parsed value exactly matches the original floating-point number.
/// @tparam FloatingPointType A floating-point number type: float, double, or long double.
/// @param[in] value The floating-point number to test.
template <typename FloatingPointType>
void print_floating(const FloatingPointType value) {
  if (std::isfinite(value)) {
    test::print_floating_finite<FloatingPointType>(value);
  } else {
    test::print_floating_non_finite<FloatingPointType>(value);
  }
}

}  // namespace

}  // namespace test

namespace lector {

/// @brief Specialization of the lector::Names constant for the test::Shape enumeration.
/// Intentionally omit test::Shape::Rectangle to test that lector::print(test::Shape::Rectangle) and
/// lector::print_enumeration(test::Shape::Rectangle) each return an empty string.
template <>
inline constexpr std::array<lector::Name<test::Shape>, 3> Names<test::Shape>{
  {
   {test::Shape::Circle, "Circle"},
   {test::Shape::Triangle, "Triangle"},
   {test::Shape::Square, "Square"},
   }
};

}  // namespace lector

namespace {

TEST(Lector, PrintBoolean) {
  EXPECT_EQ(lector::print(true), "true");
  EXPECT_EQ(lector::print(false), "false");
}

TEST(Lector, PrintDataStructure) {
  EXPECT_EQ(lector::print<test::Point>(test::FirstPoint), "1 2 3");
  EXPECT_EQ(lector::print<test::Point>(test::SecondPoint), "4 5 6");
}

TEST(Lector, PrintEnumeration) {
  EXPECT_EQ(lector::print_enumeration<test::Shape>(test::Shape::Circle), "Circle");
  EXPECT_EQ(lector::print_enumeration<test::Shape>(test::Shape::Triangle), "Triangle");
  EXPECT_EQ(lector::print_enumeration<test::Shape>(test::Shape::Square), "Square");
  EXPECT_EQ(lector::print_enumeration<test::Shape>(test::Shape::Rectangle), "");
  EXPECT_EQ(lector::print<test::Shape>(test::Shape::Circle), "Circle");
  EXPECT_EQ(lector::print<test::Shape>(test::Shape::Triangle), "Triangle");
  EXPECT_EQ(lector::print<test::Shape>(test::Shape::Square), "Square");
  EXPECT_EQ(lector::print<test::Shape>(test::Shape::Rectangle), "");
}

TEST(Lector, PrintFilesystemPath) {
  EXPECT_EQ(lector::print<std::filesystem::path>(""), "");
  EXPECT_EQ(lector::print<std::filesystem::path>("/some/path"), "/some/path");
  EXPECT_EQ(lector::print<std::filesystem::path>("file.txt"), "file.txt");
  EXPECT_EQ(lector::print<std::filesystem::path>("/some/file.txt"), "/some/file.txt");
}

TEST(Lector, PrintNumberFloatingPointPrecisionDoubleExactMatch) {
  EXPECT_EQ(lector::print<double>(-std::numeric_limits<double>::quiet_NaN()), "nan");
  EXPECT_EQ(lector::print<double>(-std::numeric_limits<double>::infinity()), "-inf");
#ifdef __linux__
  EXPECT_EQ(lector::print<double>(-16384.0), "-1.63840000000000000e+04");
  EXPECT_EQ(lector::print<double>(-8192.0), "-8192.00000000000000");
  EXPECT_EQ(lector::print<double>(-4096.0), "-4096.00000000000000");
  EXPECT_EQ(lector::print<double>(-2048.0), "-2048.00000000000000");
  EXPECT_EQ(lector::print<double>(-1024.0), "-1024.00000000000000");
  EXPECT_EQ(lector::print<double>(-512.0), "-512.000000000000000");
  EXPECT_EQ(lector::print<double>(-256.0), "-256.000000000000000");
  EXPECT_EQ(lector::print<double>(-128.0), "-128.000000000000000");
  EXPECT_EQ(lector::print<double>(-64.0), "-64.0000000000000000");
  EXPECT_EQ(lector::print<double>(-32.0), "-32.0000000000000000");
  EXPECT_EQ(lector::print<double>(-16.0), "-16.0000000000000000");
  EXPECT_EQ(lector::print<double>(-8.0), "-8.00000000000000000");
  EXPECT_EQ(lector::print<double>(-4.0), "-4.00000000000000000");
  EXPECT_EQ(lector::print<double>(-2.0), "-2.00000000000000000");
  EXPECT_EQ(lector::print<double>(-1.0), "-1.00000000000000000");
  EXPECT_EQ(lector::print<double>(-0.5), "-0.500000000000000000");
  EXPECT_EQ(lector::print<double>(-0.25), "-0.250000000000000000");
  EXPECT_EQ(lector::print<double>(-0.125), "-0.125000000000000000");
  EXPECT_EQ(lector::print<double>(-0.0625), "-0.0625000000000000000");
  EXPECT_EQ(lector::print<double>(-0.03125), "-0.0312500000000000000");
  EXPECT_EQ(lector::print<double>(-0.015625), "-0.0156250000000000000");
  EXPECT_EQ(lector::print<double>(-0.0078125), "-0.00781250000000000000");
  EXPECT_EQ(lector::print<double>(-0.00390625), "-0.00390625000000000000");
  EXPECT_EQ(lector::print<double>(-0.001953125), "-0.00195312500000000000");
  EXPECT_EQ(lector::print<double>(-0.0009765625), "-9.76562500000000000e-04");
#endif  // __linux__
  EXPECT_EQ(lector::print<double>(-0.0), "0");
  EXPECT_EQ(lector::print<double>(0.0), "0");
#ifdef __linux__
  EXPECT_EQ(lector::print<double>(0.0009765625), "9.76562500000000000e-04");
  EXPECT_EQ(lector::print<double>(0.001953125), "0.00195312500000000000");
  EXPECT_EQ(lector::print<double>(0.00390625), "0.00390625000000000000");
  EXPECT_EQ(lector::print<double>(0.0078125), "0.00781250000000000000");
  EXPECT_EQ(lector::print<double>(0.015625), "0.0156250000000000000");
  EXPECT_EQ(lector::print<double>(0.03125), "0.0312500000000000000");
  EXPECT_EQ(lector::print<double>(0.0625), "0.0625000000000000000");
  EXPECT_EQ(lector::print<double>(0.125), "0.125000000000000000");
  EXPECT_EQ(lector::print<double>(0.25), "0.250000000000000000");
  EXPECT_EQ(lector::print<double>(0.5), "0.500000000000000000");
  EXPECT_EQ(lector::print<double>(1.0), "1.00000000000000000");
  EXPECT_EQ(lector::print<double>(2.0), "2.00000000000000000");
  EXPECT_EQ(lector::print<double>(4.0), "4.00000000000000000");
  EXPECT_EQ(lector::print<double>(8.0), "8.00000000000000000");
  EXPECT_EQ(lector::print<double>(16.0), "16.0000000000000000");
  EXPECT_EQ(lector::print<double>(32.0), "32.0000000000000000");
  EXPECT_EQ(lector::print<double>(64.0), "64.0000000000000000");
  EXPECT_EQ(lector::print<double>(128.0), "128.000000000000000");
  EXPECT_EQ(lector::print<double>(256.0), "256.000000000000000");
  EXPECT_EQ(lector::print<double>(512.0), "512.000000000000000");
  EXPECT_EQ(lector::print<double>(1024.0), "1024.00000000000000");
  EXPECT_EQ(lector::print<double>(2048.0), "2048.00000000000000");
  EXPECT_EQ(lector::print<double>(4096.0), "4096.00000000000000");
  EXPECT_EQ(lector::print<double>(8192.0), "8192.00000000000000");
  EXPECT_EQ(lector::print<double>(16384.0), "1.63840000000000000e+04");
#endif  // __linux__
  EXPECT_EQ(lector::print<double>(std::numeric_limits<double>::infinity()), "inf");
  EXPECT_EQ(lector::print<double>(std::numeric_limits<double>::quiet_NaN()), "nan");
}

TEST(Lector, PrintNumberFloatingPointPrecisionDoubleRoundTrip) {
  test::print_floating<double>(11111.111111111111111111111111111111);
  test::print_floating<double>(1111.111111111111111111111111111111);
  test::print_floating<double>(111.111111111111111111111111111111);
  test::print_floating<double>(11.111111111111111111111111111111);
  test::print_floating<double>(1.111111111111111111111111111111);
  test::print_floating<double>(0.111111111111111111111111111111);
  test::print_floating<double>(0.0111111111111111111111111111111);
  test::print_floating<double>(0.00111111111111111111111111111111);
  test::print_floating<double>(0.000111111111111111111111111111111);
  test::print_floating<double>(-std::numeric_limits<double>::quiet_NaN());
  test::print_floating<double>(-std::numeric_limits<double>::infinity());
  test::print_floating<double>(-16384.0);
  test::print_floating<double>(-8192.0);
  test::print_floating<double>(-4096.0);
  test::print_floating<double>(-2048.0);
  test::print_floating<double>(-1024.0);
  test::print_floating<double>(-512.0);
  test::print_floating<double>(-256.0);
  test::print_floating<double>(-128.0);
  test::print_floating<double>(-64.0);
  test::print_floating<double>(-32.0);
  test::print_floating<double>(-16.0);
  test::print_floating<double>(-8.0);
  test::print_floating<double>(-4.0);
  test::print_floating<double>(-2.0);
  test::print_floating<double>(-1.0);
  test::print_floating<double>(-0.5);
  test::print_floating<double>(-0.25);
  test::print_floating<double>(-0.125);
  test::print_floating<double>(-0.0625);
  test::print_floating<double>(-0.03125);
  test::print_floating<double>(-0.015625);
  test::print_floating<double>(-0.0078125);
  test::print_floating<double>(-0.00390625);
  test::print_floating<double>(-0.001953125);
  test::print_floating<double>(-0.0009765625);
  test::print_floating<double>(-0.0);
  test::print_floating<double>(0.0);
  test::print_floating<double>(0.0009765625);
  test::print_floating<double>(0.001953125);
  test::print_floating<double>(0.00390625);
  test::print_floating<double>(0.0078125);
  test::print_floating<double>(0.015625);
  test::print_floating<double>(0.03125);
  test::print_floating<double>(0.0625);
  test::print_floating<double>(0.125);
  test::print_floating<double>(0.25);
  test::print_floating<double>(0.5);
  test::print_floating<double>(1.0);
  test::print_floating<double>(2.0);
  test::print_floating<double>(4.0);
  test::print_floating<double>(8.0);
  test::print_floating<double>(16.0);
  test::print_floating<double>(32.0);
  test::print_floating<double>(64.0);
  test::print_floating<double>(128.0);
  test::print_floating<double>(256.0);
  test::print_floating<double>(512.0);
  test::print_floating<double>(1024.0);
  test::print_floating<double>(2048.0);
  test::print_floating<double>(4096.0);
  test::print_floating<double>(8192.0);
  test::print_floating<double>(16384.0);
  test::print_floating<double>(std::numeric_limits<double>::infinity());
  test::print_floating<double>(std::numeric_limits<double>::quiet_NaN());
  test::print_floating<double>(0.000111111111111111111111111111111);
  test::print_floating<double>(0.00111111111111111111111111111111);
  test::print_floating<double>(0.0111111111111111111111111111111);
  test::print_floating<double>(0.111111111111111111111111111111);
  test::print_floating<double>(1.111111111111111111111111111111);
  test::print_floating<double>(11.111111111111111111111111111111);
  test::print_floating<double>(111.111111111111111111111111111111);
  test::print_floating<double>(1111.111111111111111111111111111111);
  test::print_floating<double>(11111.111111111111111111111111111111);
}

TEST(Lector, PrintNumberFloatingPointPrecisionExtendedExactMatch) {
  EXPECT_EQ(lector::print<long double>(-std::numeric_limits<long double>::quiet_NaN()), "nan");
  EXPECT_EQ(lector::print<long double>(-std::numeric_limits<long double>::infinity()), "-inf");
#ifdef __linux__
  EXPECT_EQ(lector::print<long double>(-16384.0L), "-1.638400000000000000000e+04");
  EXPECT_EQ(lector::print<long double>(-8192.0L), "-8192.000000000000000000");
  EXPECT_EQ(lector::print<long double>(-4096.0L), "-4096.000000000000000000");
  EXPECT_EQ(lector::print<long double>(-2048.0L), "-2048.000000000000000000");
  EXPECT_EQ(lector::print<long double>(-1024.0L), "-1024.000000000000000000");
  EXPECT_EQ(lector::print<long double>(-512.0L), "-512.0000000000000000000");
  EXPECT_EQ(lector::print<long double>(-256.0L), "-256.0000000000000000000");
  EXPECT_EQ(lector::print<long double>(-128.0L), "-128.0000000000000000000");
  EXPECT_EQ(lector::print<long double>(-64.0L), "-64.00000000000000000000");
  EXPECT_EQ(lector::print<long double>(-32.0L), "-32.00000000000000000000");
  EXPECT_EQ(lector::print<long double>(-16.0L), "-16.00000000000000000000");
  EXPECT_EQ(lector::print<long double>(-8.0L), "-8.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(-4.0L), "-4.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(-2.0L), "-2.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(-1.0L), "-1.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.5L), "-0.5000000000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.25L), "-0.2500000000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.125L), "-0.1250000000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.0625L), "-0.06250000000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.03125L), "-0.03125000000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.015625L), "-0.01562500000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.0078125L), "-0.007812500000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.00390625L), "-0.003906250000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.001953125L), "-0.001953125000000000000000");
  EXPECT_EQ(lector::print<long double>(-0.0009765625L), "-9.765625000000000000000e-04");
#endif  // __linux__
  EXPECT_EQ(lector::print<long double>(-0.0L), "0");
  EXPECT_EQ(lector::print<long double>(0.0L), "0");
#ifdef __linux__
  EXPECT_EQ(lector::print<long double>(0.0009765625L), "9.765625000000000000000e-04");
  EXPECT_EQ(lector::print<long double>(0.001953125L), "0.001953125000000000000000");
  EXPECT_EQ(lector::print<long double>(0.00390625L), "0.003906250000000000000000");
  EXPECT_EQ(lector::print<long double>(0.0078125L), "0.007812500000000000000000");
  EXPECT_EQ(lector::print<long double>(0.015625L), "0.01562500000000000000000");
  EXPECT_EQ(lector::print<long double>(0.03125L), "0.03125000000000000000000");
  EXPECT_EQ(lector::print<long double>(0.0625L), "0.06250000000000000000000");
  EXPECT_EQ(lector::print<long double>(0.125L), "0.1250000000000000000000");
  EXPECT_EQ(lector::print<long double>(0.25L), "0.2500000000000000000000");
  EXPECT_EQ(lector::print<long double>(0.5L), "0.5000000000000000000000");
  EXPECT_EQ(lector::print<long double>(1.0L), "1.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(2.0L), "2.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(4.0L), "4.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(8.0L), "8.000000000000000000000");
  EXPECT_EQ(lector::print<long double>(16.0L), "16.00000000000000000000");
  EXPECT_EQ(lector::print<long double>(32.0L), "32.00000000000000000000");
  EXPECT_EQ(lector::print<long double>(64.0L), "64.00000000000000000000");
  EXPECT_EQ(lector::print<long double>(128.0L), "128.0000000000000000000");
  EXPECT_EQ(lector::print<long double>(256.0L), "256.0000000000000000000");
  EXPECT_EQ(lector::print<long double>(512.0L), "512.0000000000000000000");
  EXPECT_EQ(lector::print<long double>(1024.0L), "1024.000000000000000000");
  EXPECT_EQ(lector::print<long double>(2048.0L), "2048.000000000000000000");
  EXPECT_EQ(lector::print<long double>(4096.0L), "4096.000000000000000000");
  EXPECT_EQ(lector::print<long double>(8192.0L), "8192.000000000000000000");
  EXPECT_EQ(lector::print<long double>(16384.0L), "1.638400000000000000000e+04");
#endif  // __linux__
  EXPECT_EQ(lector::print<long double>(std::numeric_limits<long double>::infinity()), "inf");
  EXPECT_EQ(lector::print<long double>(std::numeric_limits<long double>::quiet_NaN()), "nan");
}

TEST(Lector, PrintNumberFloatingPointPrecisionExtendedRoundTrip) {
  test::print_floating<long double>(11111.111111111111111111111111111111L);
  test::print_floating<long double>(1111.111111111111111111111111111111L);
  test::print_floating<long double>(111.111111111111111111111111111111L);
  test::print_floating<long double>(11.111111111111111111111111111111L);
  test::print_floating<long double>(1.111111111111111111111111111111L);
  test::print_floating<long double>(0.111111111111111111111111111111L);
  test::print_floating<long double>(0.0111111111111111111111111111111L);
  test::print_floating<long double>(0.00111111111111111111111111111111L);
  test::print_floating<long double>(0.000111111111111111111111111111111L);
  test::print_floating<long double>(-std::numeric_limits<long double>::quiet_NaN());
  test::print_floating<long double>(-std::numeric_limits<long double>::infinity());
  test::print_floating<long double>(-16384.0L);
  test::print_floating<long double>(-8192.0L);
  test::print_floating<long double>(-4096.0L);
  test::print_floating<long double>(-2048.0L);
  test::print_floating<long double>(-1024.0L);
  test::print_floating<long double>(-512.0L);
  test::print_floating<long double>(-256.0L);
  test::print_floating<long double>(-128.0L);
  test::print_floating<long double>(-64.0L);
  test::print_floating<long double>(-32.0L);
  test::print_floating<long double>(-16.0L);
  test::print_floating<long double>(-8.0L);
  test::print_floating<long double>(-4.0L);
  test::print_floating<long double>(-2.0L);
  test::print_floating<long double>(-1.0L);
  test::print_floating<long double>(-0.5L);
  test::print_floating<long double>(-0.25L);
  test::print_floating<long double>(-0.125L);
  test::print_floating<long double>(-0.0625L);
  test::print_floating<long double>(-0.03125L);
  test::print_floating<long double>(-0.015625L);
  test::print_floating<long double>(-0.0078125L);
  test::print_floating<long double>(-0.00390625L);
  test::print_floating<long double>(-0.001953125L);
  test::print_floating<long double>(-0.0009765625L);
  test::print_floating<long double>(-0.0L);
  test::print_floating<long double>(0.0L);
  test::print_floating<long double>(0.0009765625L);
  test::print_floating<long double>(0.001953125L);
  test::print_floating<long double>(0.00390625L);
  test::print_floating<long double>(0.0078125L);
  test::print_floating<long double>(0.015625L);
  test::print_floating<long double>(0.03125L);
  test::print_floating<long double>(0.0625L);
  test::print_floating<long double>(0.125L);
  test::print_floating<long double>(0.25L);
  test::print_floating<long double>(0.5L);
  test::print_floating<long double>(1.0L);
  test::print_floating<long double>(2.0L);
  test::print_floating<long double>(4.0L);
  test::print_floating<long double>(8.0L);
  test::print_floating<long double>(16.0L);
  test::print_floating<long double>(32.0L);
  test::print_floating<long double>(64.0L);
  test::print_floating<long double>(128.0L);
  test::print_floating<long double>(256.0L);
  test::print_floating<long double>(512.0L);
  test::print_floating<long double>(1024.0L);
  test::print_floating<long double>(2048.0L);
  test::print_floating<long double>(4096.0L);
  test::print_floating<long double>(8192.0L);
  test::print_floating<long double>(16384.0L);
  test::print_floating<long double>(std::numeric_limits<long double>::infinity());
  test::print_floating<long double>(std::numeric_limits<long double>::quiet_NaN());
  test::print_floating<long double>(0.000111111111111111111111111111111L);
  test::print_floating<long double>(0.00111111111111111111111111111111L);
  test::print_floating<long double>(0.0111111111111111111111111111111L);
  test::print_floating<long double>(0.111111111111111111111111111111L);
  test::print_floating<long double>(1.111111111111111111111111111111L);
  test::print_floating<long double>(11.111111111111111111111111111111L);
  test::print_floating<long double>(111.111111111111111111111111111111L);
  test::print_floating<long double>(1111.111111111111111111111111111111L);
  test::print_floating<long double>(11111.111111111111111111111111111111L);
}

TEST(Lector, PrintNumberFloatingPointPrecisionSingleExactMatch) {
  EXPECT_EQ(lector::print<float>(-std::numeric_limits<float>::quiet_NaN()), "nan");
  EXPECT_EQ(lector::print<float>(-std::numeric_limits<float>::infinity()), "-inf");
#ifdef __linux__
  EXPECT_EQ(lector::print<float>(-16384.0F), "-1.638400000e+04");
  EXPECT_EQ(lector::print<float>(-8192.0F), "-8192.000000");
  EXPECT_EQ(lector::print<float>(-4096.0F), "-4096.000000");
  EXPECT_EQ(lector::print<float>(-2048.0F), "-2048.000000");
  EXPECT_EQ(lector::print<float>(-1024.0F), "-1024.000000");
  EXPECT_EQ(lector::print<float>(-512.0F), "-512.0000000");
  EXPECT_EQ(lector::print<float>(-256.0F), "-256.0000000");
  EXPECT_EQ(lector::print<float>(-128.0F), "-128.0000000");
  EXPECT_EQ(lector::print<float>(-64.0F), "-64.00000000");
  EXPECT_EQ(lector::print<float>(-32.0F), "-32.00000000");
  EXPECT_EQ(lector::print<float>(-16.0F), "-16.00000000");
  EXPECT_EQ(lector::print<float>(-8.0F), "-8.000000000");
  EXPECT_EQ(lector::print<float>(-4.0F), "-4.000000000");
  EXPECT_EQ(lector::print<float>(-2.0F), "-2.000000000");
  EXPECT_EQ(lector::print<float>(-1.0F), "-1.000000000");
  EXPECT_EQ(lector::print<float>(-0.5F), "-0.5000000000");
  EXPECT_EQ(lector::print<float>(-0.25F), "-0.2500000000");
  EXPECT_EQ(lector::print<float>(-0.125F), "-0.1250000000");
  EXPECT_EQ(lector::print<float>(-0.0625F), "-0.06250000000");
  EXPECT_EQ(lector::print<float>(-0.03125F), "-0.03125000000");
  EXPECT_EQ(lector::print<float>(-0.015625F), "-0.01562500000");
  EXPECT_EQ(lector::print<float>(-0.0078125F), "-0.007812500000");
  EXPECT_EQ(lector::print<float>(-0.00390625F), "-0.003906250000");
  EXPECT_EQ(lector::print<float>(-0.001953125F), "-0.001953125000");
  EXPECT_EQ(lector::print<float>(-0.0009765625F), "-9.765625000e-04");
#endif  // __linux__
  EXPECT_EQ(lector::print<float>(-0.0F), "0");
  EXPECT_EQ(lector::print<float>(0.0F), "0");
#ifdef __linux__
  EXPECT_EQ(lector::print<float>(0.0009765625F), "9.765625000e-04");
  EXPECT_EQ(lector::print<float>(0.001953125F), "0.001953125000");
  EXPECT_EQ(lector::print<float>(0.00390625F), "0.003906250000");
  EXPECT_EQ(lector::print<float>(0.0078125F), "0.007812500000");
  EXPECT_EQ(lector::print<float>(0.015625F), "0.01562500000");
  EXPECT_EQ(lector::print<float>(0.03125F), "0.03125000000");
  EXPECT_EQ(lector::print<float>(0.0625F), "0.06250000000");
  EXPECT_EQ(lector::print<float>(0.125F), "0.1250000000");
  EXPECT_EQ(lector::print<float>(0.25F), "0.2500000000");
  EXPECT_EQ(lector::print<float>(0.5F), "0.5000000000");
  EXPECT_EQ(lector::print<float>(1.0F), "1.000000000");
  EXPECT_EQ(lector::print<float>(2.0F), "2.000000000");
  EXPECT_EQ(lector::print<float>(4.0F), "4.000000000");
  EXPECT_EQ(lector::print<float>(8.0F), "8.000000000");
  EXPECT_EQ(lector::print<float>(16.0F), "16.00000000");
  EXPECT_EQ(lector::print<float>(32.0F), "32.00000000");
  EXPECT_EQ(lector::print<float>(64.0F), "64.00000000");
  EXPECT_EQ(lector::print<float>(128.0F), "128.0000000");
  EXPECT_EQ(lector::print<float>(256.0F), "256.0000000");
  EXPECT_EQ(lector::print<float>(512.0F), "512.0000000");
  EXPECT_EQ(lector::print<float>(1024.0F), "1024.000000");
  EXPECT_EQ(lector::print<float>(2048.0F), "2048.000000");
  EXPECT_EQ(lector::print<float>(4096.0F), "4096.000000");
  EXPECT_EQ(lector::print<float>(8192.0F), "8192.000000");
  EXPECT_EQ(lector::print<float>(16384.0F), "1.638400000e+04");
#endif  // __linux__
  EXPECT_EQ(lector::print<float>(std::numeric_limits<float>::infinity()), "inf");
  EXPECT_EQ(lector::print<float>(std::numeric_limits<float>::quiet_NaN()), "nan");
}

TEST(Lector, PrintNumberFloatingPointPrecisionSingleRoundTrip) {
  test::print_floating<float>(11111.111111111111111111111111111111F);
  test::print_floating<float>(1111.111111111111111111111111111111F);
  test::print_floating<float>(111.111111111111111111111111111111F);
  test::print_floating<float>(11.111111111111111111111111111111F);
  test::print_floating<float>(1.111111111111111111111111111111F);
  test::print_floating<float>(0.111111111111111111111111111111F);
  test::print_floating<float>(0.0111111111111111111111111111111F);
  test::print_floating<float>(0.00111111111111111111111111111111F);
  test::print_floating<float>(0.000111111111111111111111111111111F);
  test::print_floating<float>(-std::numeric_limits<float>::quiet_NaN());
  test::print_floating<float>(-std::numeric_limits<float>::infinity());
  test::print_floating<float>(-16384.0F);
  test::print_floating<float>(-8192.0F);
  test::print_floating<float>(-4096.0F);
  test::print_floating<float>(-2048.0F);
  test::print_floating<float>(-1024.0F);
  test::print_floating<float>(-512.0F);
  test::print_floating<float>(-256.0F);
  test::print_floating<float>(-128.0F);
  test::print_floating<float>(-64.0F);
  test::print_floating<float>(-32.0F);
  test::print_floating<float>(-16.0F);
  test::print_floating<float>(-8.0F);
  test::print_floating<float>(-4.0F);
  test::print_floating<float>(-2.0F);
  test::print_floating<float>(-1.0F);
  test::print_floating<float>(-0.5F);
  test::print_floating<float>(-0.25F);
  test::print_floating<float>(-0.125F);
  test::print_floating<float>(-0.0625F);
  test::print_floating<float>(-0.03125F);
  test::print_floating<float>(-0.015625F);
  test::print_floating<float>(-0.0078125F);
  test::print_floating<float>(-0.00390625F);
  test::print_floating<float>(-0.001953125F);
  test::print_floating<float>(-0.0009765625F);
  test::print_floating<float>(-0.0F);
  test::print_floating<float>(0.0F);
  test::print_floating<float>(0.0009765625F);
  test::print_floating<float>(0.001953125F);
  test::print_floating<float>(0.00390625F);
  test::print_floating<float>(0.0078125F);
  test::print_floating<float>(0.015625F);
  test::print_floating<float>(0.03125F);
  test::print_floating<float>(0.0625F);
  test::print_floating<float>(0.125F);
  test::print_floating<float>(0.25F);
  test::print_floating<float>(0.5F);
  test::print_floating<float>(1.0F);
  test::print_floating<float>(2.0F);
  test::print_floating<float>(4.0F);
  test::print_floating<float>(8.0F);
  test::print_floating<float>(16.0F);
  test::print_floating<float>(32.0F);
  test::print_floating<float>(64.0F);
  test::print_floating<float>(128.0F);
  test::print_floating<float>(256.0F);
  test::print_floating<float>(512.0F);
  test::print_floating<float>(1024.0F);
  test::print_floating<float>(2048.0F);
  test::print_floating<float>(4096.0F);
  test::print_floating<float>(8192.0F);
  test::print_floating<float>(16384.0F);
  test::print_floating<float>(std::numeric_limits<float>::infinity());
  test::print_floating<float>(std::numeric_limits<float>::quiet_NaN());
  test::print_floating<float>(0.000111111111111111111111111111111F);
  test::print_floating<float>(0.00111111111111111111111111111111F);
  test::print_floating<float>(0.0111111111111111111111111111111F);
  test::print_floating<float>(0.111111111111111111111111111111F);
  test::print_floating<float>(1.111111111111111111111111111111F);
  test::print_floating<float>(11.111111111111111111111111111111F);
  test::print_floating<float>(111.111111111111111111111111111111F);
  test::print_floating<float>(1111.111111111111111111111111111111F);
  test::print_floating<float>(11111.111111111111111111111111111111F);
}

TEST(Lector, PrintNumberIntegerBits08) {
  EXPECT_EQ(lector::print<std::int8_t>(static_cast<std::int8_t>(-123)), "-123");
  EXPECT_EQ(lector::print<std::int8_t>(static_cast<std::int8_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::int8_t>(static_cast<std::int8_t>(0)), "0");
  EXPECT_EQ(lector::print<std::int8_t>(static_cast<std::int8_t>(123)), "123");
}

TEST(Lector, PrintNumberIntegerBits16) {
  EXPECT_EQ(lector::print<std::int16_t>(static_cast<std::int16_t>(-123)), "-123");
  EXPECT_EQ(lector::print<std::int16_t>(static_cast<std::int16_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::int16_t>(static_cast<std::int16_t>(0)), "0");
  EXPECT_EQ(lector::print<std::int16_t>(static_cast<std::int16_t>(123)), "123");
}

TEST(Lector, PrintNumberIntegerBits32) {
  EXPECT_EQ(lector::print<std::int32_t>(static_cast<std::int32_t>(-123)), "-123");
  EXPECT_EQ(lector::print<std::int32_t>(static_cast<std::int32_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::int32_t>(static_cast<std::int32_t>(0)), "0");
  EXPECT_EQ(lector::print<std::int32_t>(static_cast<std::int32_t>(123)), "123");
}

TEST(Lector, PrintNumberIntegerBits64) {
  EXPECT_EQ(lector::print<std::int64_t>(static_cast<std::int64_t>(-123)), "-123");
  EXPECT_EQ(lector::print<std::int64_t>(static_cast<std::int64_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::int64_t>(static_cast<std::int64_t>(0)), "0");
  EXPECT_EQ(lector::print<std::int64_t>(static_cast<std::int64_t>(123)), "123");
}

TEST(Lector, PrintNumberNaturalBits08) {
  EXPECT_EQ(lector::print<std::uint8_t>(static_cast<std::uint8_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::uint8_t>(static_cast<std::uint8_t>(0)), "0");
  EXPECT_EQ(lector::print<std::uint8_t>(static_cast<std::uint8_t>(123)), "123");
}

TEST(Lector, PrintNumberNaturalBits16) {
  EXPECT_EQ(lector::print<std::uint16_t>(static_cast<std::uint16_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::uint16_t>(static_cast<std::uint16_t>(0)), "0");
  EXPECT_EQ(lector::print<std::uint16_t>(static_cast<std::uint16_t>(123)), "123");
}

TEST(Lector, PrintNumberNaturalBits32) {
  EXPECT_EQ(lector::print<std::uint32_t>(static_cast<std::uint32_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::uint32_t>(static_cast<std::uint32_t>(0)), "0");
  EXPECT_EQ(lector::print<std::uint32_t>(static_cast<std::uint32_t>(123)), "123");
}

TEST(Lector, PrintNumberNaturalBits64) {
  EXPECT_EQ(lector::print<std::uint64_t>(static_cast<std::uint64_t>(-0)), "0");
  EXPECT_EQ(lector::print<std::uint64_t>(static_cast<std::uint64_t>(0)), "0");
  EXPECT_EQ(lector::print<std::uint64_t>(static_cast<std::uint64_t>(123)), "123");
}

TEST(Lector, PrintString) {
  EXPECT_EQ(lector::print<std::string>(""), "");
  EXPECT_EQ(lector::print<std::string>("Hello, world!"), "Hello, world!");
}

TEST(Lector, PrintStringView) {
  EXPECT_EQ(lector::print<std::string_view>(""), "");
  EXPECT_EQ(lector::print<std::string_view>("Hello, world!"), "Hello, world!");
}

}  // namespace
