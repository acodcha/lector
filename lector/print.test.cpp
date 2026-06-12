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
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <limits>
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

/// @brief Specialization of the lector::Printings constant for the test::Color enumeration.
template <>
inline constexpr ::std::array<::lector::Printing<::test::Color>, 3> Printings<::test::Color>{
  {
   {::test::Color::Red, "Red"},
   {::test::Color::Green, "Green"},
   {::test::Color::Blue, "Blue"},
   }
};

}  // namespace lector

namespace {

TEST(Lector, PrintBoolean) {
  EXPECT_EQ(::lector::print(true), "true");
  EXPECT_EQ(::lector::print(false), "false");
}

TEST(Lector, PrintDataStructure) {
  EXPECT_EQ(::lector::print<::test::Point>(::test::FirstPoint), "1 2 3");
  EXPECT_EQ(::lector::print<::test::Point>(::test::SecondPoint), "4 5 6");
}

TEST(Lector, PrintEnumeration) {
  EXPECT_EQ(::lector::print_enumeration<::test::Color>(::test::Color::Red), "Red");
  EXPECT_EQ(::lector::print_enumeration<::test::Color>(::test::Color::Green), "Green");
  EXPECT_EQ(::lector::print_enumeration<::test::Color>(::test::Color::Blue), "Blue");
  EXPECT_EQ(::lector::print_enumeration<::test::Color>(static_cast<::test::Color>(123)), "");
  EXPECT_EQ(::lector::print<::test::Color>(::test::Color::Red), "Red");
  EXPECT_EQ(::lector::print<::test::Color>(::test::Color::Green), "Green");
  EXPECT_EQ(::lector::print<::test::Color>(::test::Color::Blue), "Blue");
  EXPECT_EQ(::lector::print<::test::Color>(static_cast<::test::Color>(123)), "");
}

TEST(Lector, PrintFilesystemPath) {
  EXPECT_EQ(::lector::print<::std::filesystem::path>(""), "");
  EXPECT_EQ(::lector::print<::std::filesystem::path>("/some/path"), "/some/path");
  EXPECT_EQ(::lector::print<::std::filesystem::path>("file.txt"), "file.txt");
  EXPECT_EQ(::lector::print<::std::filesystem::path>("/some/file.txt"), "/some/file.txt");
}

TEST(Lector, PrintNumberFloatingPointPrecisionDouble) {
#if defined(__linux__)
  EXPECT_EQ(::lector::print<double>(-std::numeric_limits<double>::quiet_NaN()), "-nan");
  EXPECT_EQ(::lector::print<double>(-std::numeric_limits<double>::infinity()), "-inf");
  EXPECT_EQ(::lector::print<double>(-4096.0), "-4096.00000000000000");
  EXPECT_EQ(::lector::print<double>(-512.0), "-512.000000000000000");
  EXPECT_EQ(::lector::print<double>(-64.0), "-64.0000000000000000");
  EXPECT_EQ(::lector::print<double>(-8.0), "-8.00000000000000000");
  EXPECT_EQ(::lector::print<double>(-1.0), "-1.00000000000000000");
  EXPECT_EQ(::lector::print<double>(-0.125), "-0.125000000000000000");
  EXPECT_EQ(::lector::print<double>(-0.015625), "-0.0156250000000000000");
#endif  // defined(__linux__)
  EXPECT_EQ(::lector::print<double>(-0.0), "0");
  EXPECT_EQ(::lector::print<double>(0.0), "0");
#if defined(__linux__)
  EXPECT_EQ(::lector::print<double>(0.015625), "0.0156250000000000000");
  EXPECT_EQ(::lector::print<double>(0.125), "0.125000000000000000");
  EXPECT_EQ(::lector::print<double>(1.0), "1.00000000000000000");
  EXPECT_EQ(::lector::print<double>(8.0), "8.00000000000000000");
  EXPECT_EQ(::lector::print<double>(64.0), "64.0000000000000000");
  EXPECT_EQ(::lector::print<double>(512.0), "512.000000000000000");
  EXPECT_EQ(::lector::print<double>(4096.0), "4096.00000000000000");
  EXPECT_EQ(::lector::print<double>(std::numeric_limits<double>::infinity()), "inf");
  EXPECT_EQ(::lector::print<double>(std::numeric_limits<double>::quiet_NaN()), "nan");
#endif  // defined(__linux__)
}

TEST(Lector, PrintNumberFloatingPointPrecisionExtended) {
#if defined(__linux__)
  EXPECT_EQ(::lector::print<long double>(-std::numeric_limits<long double>::quiet_NaN()), "-nan");
  EXPECT_EQ(::lector::print<long double>(-std::numeric_limits<long double>::infinity()), "-inf");
#endif  // defined(__linux__)
  EXPECT_EQ(::lector::print<long double>(-0.0L), "0");
  EXPECT_EQ(::lector::print<long double>(0.0L), "0");
#if defined(__linux__)
  EXPECT_EQ(::lector::print<long double>(std::numeric_limits<long double>::infinity()), "inf");
  EXPECT_EQ(::lector::print<long double>(std::numeric_limits<long double>::quiet_NaN()), "nan");
#endif  // defined(__linux__)
}

TEST(Lector, PrintNumberFloatingPointPrecisionSingle) {
#if defined(__linux__)
  EXPECT_EQ(::lector::print<float>(-std::numeric_limits<float>::quiet_NaN()), "-nan");
  EXPECT_EQ(::lector::print<float>(-std::numeric_limits<float>::infinity()), "-inf");
  EXPECT_EQ(::lector::print<float>(-4096.0F), "-4096.000000");
  EXPECT_EQ(::lector::print<float>(-512.0F), "-512.0000000");
  EXPECT_EQ(::lector::print<float>(-64.0F), "-64.00000000");
  EXPECT_EQ(::lector::print<float>(-8.0F), "-8.000000000");
  EXPECT_EQ(::lector::print<float>(-1.0F), "-1.000000000");
  EXPECT_EQ(::lector::print<float>(-0.125F), "-0.1250000000");
  EXPECT_EQ(::lector::print<float>(-0.015625F), "-0.01562500000");
#endif  // defined(__linux__)
  EXPECT_EQ(::lector::print<float>(-0.0F), "0");
  EXPECT_EQ(::lector::print<float>(0.0F), "0");
#if defined(__linux__)
  EXPECT_EQ(::lector::print<float>(0.015625F), "0.01562500000");
  EXPECT_EQ(::lector::print<float>(0.125F), "0.1250000000");
  EXPECT_EQ(::lector::print<float>(1.0F), "1.000000000");
  EXPECT_EQ(::lector::print<float>(8.0F), "8.000000000");
  EXPECT_EQ(::lector::print<float>(64.0F), "64.00000000");
  EXPECT_EQ(::lector::print<float>(512.0F), "512.0000000");
  EXPECT_EQ(::lector::print<float>(4096.0F), "4096.000000");
  EXPECT_EQ(::lector::print<float>(std::numeric_limits<float>::infinity()), "inf");
  EXPECT_EQ(::lector::print<float>(std::numeric_limits<float>::quiet_NaN()), "nan");
#endif  // defined(__linux__)
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
