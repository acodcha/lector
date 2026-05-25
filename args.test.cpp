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
#include <optional>
#include <string>

namespace {

TEST(Args, ParseBoolean) {
  EXPECT_EQ(args::parse<bool>(""), std::nullopt);
  EXPECT_EQ(args::parse<bool>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<bool>("TRUE"), true);
  EXPECT_EQ(args::parse<bool>("True"), true);
  EXPECT_EQ(args::parse<bool>("true"), true);
  EXPECT_EQ(args::parse<bool>("1"), true);
  EXPECT_EQ(args::parse<bool>("FALSE"), false);
  EXPECT_EQ(args::parse<bool>("False"), false);
  EXPECT_EQ(args::parse<bool>("false"), false);
  EXPECT_EQ(args::parse<bool>("0"), false);
}

TEST(Args, ParseEnumeration) {
  EXPECT_EQ(args::parse_enumeration<args::Importance>(""), std::nullopt);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("OPT"), args::Importance::Optional);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("Opt"), args::Importance::Optional);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("opt"), args::Importance::Optional);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("OPTIONAL"), args::Importance::Optional);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("Optional"), args::Importance::Optional);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("optional"), args::Importance::Optional);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("REQ"), args::Importance::Required);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("Req"), args::Importance::Required);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("req"), args::Importance::Required);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("REQUIRED"), args::Importance::Required);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("Required"), args::Importance::Required);
  EXPECT_EQ(args::parse_enumeration<args::Importance>("required"), args::Importance::Required);
}

TEST(Args, ParseFilesystemPath) {
  EXPECT_EQ(args::parse<std::filesystem::path>(""), std::filesystem::path{""});
  EXPECT_EQ(args::parse<std::filesystem::path>("some_path"), std::filesystem::path{"some_path"});
  EXPECT_EQ(
      args::parse<std::filesystem::path>("some_file.txt"), std::filesystem::path{"some_file.txt"});
  EXPECT_EQ(args::parse<std::filesystem::path>("/some/path"), std::filesystem::path{"/some/path"});
  EXPECT_EQ(args::parse<std::filesystem::path>("/some/file.txt"),
            std::filesystem::path{"/some/file.txt"});
}

TEST(Args, ParseNumberDouble) {
  // Invalid.
  EXPECT_EQ(args::parse<double>(""), std::nullopt);
  EXPECT_EQ(args::parse<double>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<double>("1.0E1000000000000000000000000000000"), std::nullopt);
  EXPECT_EQ(args::parse<double>("-1.0E1000000000000000000000000000000"), std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(args::parse<double>("NAN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<double>("NAN").value()));
  ASSERT_TRUE(args::parse<double>("NaN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<double>("NaN").value()));
  ASSERT_TRUE(args::parse<double>("nan").has_value());
  EXPECT_TRUE(std::isnan(args::parse<double>("nan").value()));
  ASSERT_TRUE(args::parse<double>("-NAN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<double>("-NAN").value()));
  ASSERT_TRUE(args::parse<double>("-NaN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<double>("-NaN").value()));
  ASSERT_TRUE(args::parse<double>("-nan").has_value());
  EXPECT_TRUE(std::isnan(args::parse<double>("-nan").value()));

  // Infinity.
  EXPECT_EQ(args::parse<double>("INF"), std::numeric_limits<double>::infinity());
  EXPECT_EQ(args::parse<double>("Inf"), std::numeric_limits<double>::infinity());
  EXPECT_EQ(args::parse<double>("inf"), std::numeric_limits<double>::infinity());
  EXPECT_EQ(args::parse<double>("-INF"), -std::numeric_limits<double>::infinity());
  EXPECT_EQ(args::parse<double>("-Inf"), -std::numeric_limits<double>::infinity());
  EXPECT_EQ(args::parse<double>("-inf"), -std::numeric_limits<double>::infinity());

  // Zero.
  EXPECT_EQ(args::parse<double>("0F"), 0.0);
  EXPECT_EQ(args::parse<double>("0f"), 0.0);
  EXPECT_EQ(args::parse<double>("0"), 0.0);
  EXPECT_EQ(args::parse<double>("0L"), 0.0);
  EXPECT_EQ(args::parse<double>("0l"), 0.0);
  EXPECT_EQ(args::parse<double>("0.0F"), 0.0);
  EXPECT_EQ(args::parse<double>("0.0f"), 0.0);
  EXPECT_EQ(args::parse<double>("0.0"), 0.0);
  EXPECT_EQ(args::parse<double>("0.0L"), 0.0);
  EXPECT_EQ(args::parse<double>("0.0l"), 0.0);
  EXPECT_EQ(args::parse<double>("-0F"), 0.0);
  EXPECT_EQ(args::parse<double>("-0f"), 0.0);
  EXPECT_EQ(args::parse<double>("-0"), 0.0);
  EXPECT_EQ(args::parse<double>("-0L"), 0.0);
  EXPECT_EQ(args::parse<double>("-0l"), 0.0);
  EXPECT_EQ(args::parse<double>("-0.0F"), 0.0);
  EXPECT_EQ(args::parse<double>("-0.0f"), 0.0);
  EXPECT_EQ(args::parse<double>("-0.0"), 0.0);
  EXPECT_EQ(args::parse<double>("-0.0L"), 0.0);
  EXPECT_EQ(args::parse<double>("-0.0l"), 0.0);

  // Integer.
  EXPECT_EQ(args::parse<double>("-10F"), -10.0);
  EXPECT_EQ(args::parse<double>("-10f"), -10.0);
  EXPECT_EQ(args::parse<double>("-10"), -10.0);
  EXPECT_EQ(args::parse<double>("-10L"), -10.0);
  EXPECT_EQ(args::parse<double>("-10l"), -10.0);

  // Decimal notation.
  EXPECT_EQ(args::parse<double>("-3.14F"), -3.14);
  EXPECT_EQ(args::parse<double>("-3.14f"), -3.14);
  EXPECT_EQ(args::parse<double>("-3.14"), -3.14);
  EXPECT_EQ(args::parse<double>("-3.14L"), -3.14);
  EXPECT_EQ(args::parse<double>("-3.14l"), -3.14);

  // Scientific notation, small number.
  EXPECT_EQ(args::parse<double>("-3.14E-12F"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14E-12f"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14E-12"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14E-12L"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14E-12l"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14e-12F"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14e-12f"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14e-12"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14e-12L"), -3.14E-12);
  EXPECT_EQ(args::parse<double>("-3.14e-12l"), -3.14E-12);

  // Scientific notation, large number.
  EXPECT_EQ(args::parse<double>("-3.14E12F"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E12f"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E12"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E12L"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E12l"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e12F"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e12f"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e12"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e12L"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e12l"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E+12F"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E+12f"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E+12"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E+12L"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14E+12l"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e+12F"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e+12f"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e+12"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e+12L"), -3.14E12);
  EXPECT_EQ(args::parse<double>("-3.14e+12l"), -3.14E12);
}

TEST(Args, ParseNumberFloat) {
  // Invalid.
  EXPECT_EQ(args::parse<float>(""), std::nullopt);
  EXPECT_EQ(args::parse<float>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<float>("1.0E1000000000000000000000000000000"), std::nullopt);
  EXPECT_EQ(args::parse<float>("-1.0E1000000000000000000000000000000"), std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(args::parse<float>("NAN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<float>("NAN").value()));
  ASSERT_TRUE(args::parse<float>("NaN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<float>("NaN").value()));
  ASSERT_TRUE(args::parse<float>("nan").has_value());
  EXPECT_TRUE(std::isnan(args::parse<float>("nan").value()));
  ASSERT_TRUE(args::parse<float>("-NAN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<float>("-NAN").value()));
  ASSERT_TRUE(args::parse<float>("-NaN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<float>("-NaN").value()));
  ASSERT_TRUE(args::parse<float>("-nan").has_value());
  EXPECT_TRUE(std::isnan(args::parse<float>("-nan").value()));

  // Infinity.
  EXPECT_EQ(args::parse<float>("INF"), std::numeric_limits<float>::infinity());
  EXPECT_EQ(args::parse<float>("Inf"), std::numeric_limits<float>::infinity());
  EXPECT_EQ(args::parse<float>("inf"), std::numeric_limits<float>::infinity());
  EXPECT_EQ(args::parse<float>("-INF"), -std::numeric_limits<float>::infinity());
  EXPECT_EQ(args::parse<float>("-Inf"), -std::numeric_limits<float>::infinity());
  EXPECT_EQ(args::parse<float>("-inf"), -std::numeric_limits<float>::infinity());

  // Zero.
  EXPECT_EQ(args::parse<float>("0F"), 0.0F);
  EXPECT_EQ(args::parse<float>("0f"), 0.0F);
  EXPECT_EQ(args::parse<float>("0"), 0.0F);
  EXPECT_EQ(args::parse<float>("0L"), 0.0F);
  EXPECT_EQ(args::parse<float>("0l"), 0.0F);
  EXPECT_EQ(args::parse<float>("0.0F"), 0.0F);
  EXPECT_EQ(args::parse<float>("0.0f"), 0.0F);
  EXPECT_EQ(args::parse<float>("0.0"), 0.0F);
  EXPECT_EQ(args::parse<float>("0.0L"), 0.0F);
  EXPECT_EQ(args::parse<float>("0.0l"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0F"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0f"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0L"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0l"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0.0F"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0.0f"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0.0"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0.0L"), 0.0F);
  EXPECT_EQ(args::parse<float>("-0.0l"), 0.0F);

  // Integer.
  EXPECT_EQ(args::parse<float>("-10F"), -10.0F);
  EXPECT_EQ(args::parse<float>("-10f"), -10.0F);
  EXPECT_EQ(args::parse<float>("-10"), -10.0F);
  EXPECT_EQ(args::parse<float>("-10L"), -10.0F);
  EXPECT_EQ(args::parse<float>("-10l"), -10.0F);

  // Decimal notation.
  EXPECT_EQ(args::parse<float>("-3.14F"), -3.14F);
  EXPECT_EQ(args::parse<float>("-3.14f"), -3.14F);
  EXPECT_EQ(args::parse<float>("-3.14"), -3.14F);
  EXPECT_EQ(args::parse<float>("-3.14L"), -3.14F);
  EXPECT_EQ(args::parse<float>("-3.14l"), -3.14F);

  // Scientific notation, small number.
  EXPECT_EQ(args::parse<float>("-3.14E-12F"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14E-12f"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14E-12"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14E-12L"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14E-12l"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14e-12F"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14e-12f"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14e-12"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14e-12L"), -3.14E-12F);
  EXPECT_EQ(args::parse<float>("-3.14e-12l"), -3.14E-12F);

  // Scientific notation, large number.
  EXPECT_EQ(args::parse<float>("-3.14E12F"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E12f"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E12"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E12L"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E12l"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e12F"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e12f"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e12"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e12L"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e12l"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E+12F"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E+12f"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E+12"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E+12L"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14E+12l"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e+12F"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e+12f"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e+12"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e+12L"), -3.14E12F);
  EXPECT_EQ(args::parse<float>("-3.14e+12l"), -3.14E12F);
}

TEST(Args, ParseNumberInteger8) {
  EXPECT_EQ(args::parse<int8_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<int8_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<int8_t>("1000"), std::nullopt);
  EXPECT_EQ(args::parse<int8_t>("-1000"), std::nullopt);
  EXPECT_EQ(args::parse<int8_t>("0"), 0);
  EXPECT_EQ(args::parse<int8_t>("-0"), 0);
  EXPECT_EQ(args::parse<int8_t>("123"), 123);
  EXPECT_EQ(args::parse<int8_t>("-123"), -123);
}

TEST(Args, ParseNumberInteger16) {
  EXPECT_EQ(args::parse<int16_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<int16_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<int16_t>("1000000"), std::nullopt);
  EXPECT_EQ(args::parse<int16_t>("-1000000"), std::nullopt);
  EXPECT_EQ(args::parse<int16_t>("0"), 0);
  EXPECT_EQ(args::parse<int16_t>("-0"), 0);
  EXPECT_EQ(args::parse<int16_t>("123"), 123);
  EXPECT_EQ(args::parse<int16_t>("-123"), -123);
}

TEST(Args, ParseNumberInteger32) {
  EXPECT_EQ(args::parse<int32_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<int32_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<int32_t>("10000000000"), std::nullopt);
  EXPECT_EQ(args::parse<int32_t>("-10000000000"), std::nullopt);
  EXPECT_EQ(args::parse<int32_t>("0"), 0);
  EXPECT_EQ(args::parse<int32_t>("-0"), 0);
  EXPECT_EQ(args::parse<int32_t>("123"), 123);
  EXPECT_EQ(args::parse<int32_t>("-123"), -123);
}

TEST(Args, ParseNumberInteger64) {
  EXPECT_EQ(args::parse<int64_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<int64_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<int64_t>("1000000000000000000000000000000"), std::nullopt);
  EXPECT_EQ(args::parse<int64_t>("-1000000000000000000000000000000"), std::nullopt);
  EXPECT_EQ(args::parse<int64_t>("0"), 0);
  EXPECT_EQ(args::parse<int64_t>("-0"), 0);
  EXPECT_EQ(args::parse<int64_t>("123"), 123);
  EXPECT_EQ(args::parse<int64_t>("-123"), -123);
}

TEST(Args, ParseNumberLongDouble) {
  // Invalid.
  EXPECT_EQ(args::parse<long double>(""), std::nullopt);
  EXPECT_EQ(args::parse<long double>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<long double>("1.0E1000000000000000000000000000000"), std::nullopt);
  EXPECT_EQ(args::parse<long double>("-1.0E1000000000000000000000000000000"), std::nullopt);

  // Not-a-number.
  ASSERT_TRUE(args::parse<long double>("NAN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<long double>("NAN").value()));
  ASSERT_TRUE(args::parse<long double>("NaN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<long double>("NaN").value()));
  ASSERT_TRUE(args::parse<long double>("nan").has_value());
  EXPECT_TRUE(std::isnan(args::parse<long double>("nan").value()));
  ASSERT_TRUE(args::parse<long double>("-NAN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<long double>("-NAN").value()));
  ASSERT_TRUE(args::parse<long double>("-NaN").has_value());
  EXPECT_TRUE(std::isnan(args::parse<long double>("-NaN").value()));
  ASSERT_TRUE(args::parse<long double>("-nan").has_value());
  EXPECT_TRUE(std::isnan(args::parse<long double>("-nan").value()));

  // Infinity.
  EXPECT_EQ(args::parse<long double>("INF"), std::numeric_limits<long double>::infinity());
  EXPECT_EQ(args::parse<long double>("Inf"), std::numeric_limits<long double>::infinity());
  EXPECT_EQ(args::parse<long double>("inf"), std::numeric_limits<long double>::infinity());
  EXPECT_EQ(args::parse<long double>("-INF"), -std::numeric_limits<long double>::infinity());
  EXPECT_EQ(args::parse<long double>("-Inf"), -std::numeric_limits<long double>::infinity());
  EXPECT_EQ(args::parse<long double>("-inf"), -std::numeric_limits<long double>::infinity());

  // Zero.
  EXPECT_EQ(args::parse<long double>("0F"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0f"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0L"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0l"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0.0F"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0.0f"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0.0"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0.0L"), 0.0L);
  EXPECT_EQ(args::parse<long double>("0.0l"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0F"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0f"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0L"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0l"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0.0F"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0.0f"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0.0"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0.0L"), 0.0L);
  EXPECT_EQ(args::parse<long double>("-0.0l"), 0.0L);

  // Integer.
  EXPECT_EQ(args::parse<long double>("-10F"), -10.0L);
  EXPECT_EQ(args::parse<long double>("-10f"), -10.0L);
  EXPECT_EQ(args::parse<long double>("-10"), -10.0L);
  EXPECT_EQ(args::parse<long double>("-10L"), -10.0L);
  EXPECT_EQ(args::parse<long double>("-10l"), -10.0L);

  // Decimal notation.
  EXPECT_EQ(args::parse<long double>("-3.14F"), -3.14L);
  EXPECT_EQ(args::parse<long double>("-3.14f"), -3.14L);
  EXPECT_EQ(args::parse<long double>("-3.14"), -3.14L);
  EXPECT_EQ(args::parse<long double>("-3.14L"), -3.14L);
  EXPECT_EQ(args::parse<long double>("-3.14l"), -3.14L);

  // Scientific notation, small number.
  EXPECT_EQ(args::parse<long double>("-3.14E-12F"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14E-12f"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14E-12"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14E-12L"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14E-12l"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14e-12F"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14e-12f"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14e-12"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14e-12L"), -3.14E-12L);
  EXPECT_EQ(args::parse<long double>("-3.14e-12l"), -3.14E-12L);

  // Scientific notation, large number.
  EXPECT_EQ(args::parse<long double>("-3.14E12F"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E12f"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E12"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E12L"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E12l"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e12F"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e12f"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e12"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e12L"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e12l"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E+12F"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E+12f"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E+12"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E+12L"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14E+12l"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e+12F"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e+12f"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e+12"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e+12L"), -3.14E12L);
  EXPECT_EQ(args::parse<long double>("-3.14e+12l"), -3.14E12L);
}

TEST(Args, ParseNumberNatural8) {
  EXPECT_EQ(args::parse<uint8_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<uint8_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<uint8_t>("-0"), std::nullopt);
  EXPECT_EQ(args::parse<uint8_t>("-123"), std::nullopt);
  EXPECT_EQ(args::parse<uint8_t>("1000"), std::nullopt);
  EXPECT_EQ(args::parse<uint8_t>("0"), 0);
  EXPECT_EQ(args::parse<uint8_t>("123"), 123);
}

TEST(Args, ParseNumberNatural16) {
  EXPECT_EQ(args::parse<uint16_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<uint16_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<uint16_t>("-0"), std::nullopt);
  EXPECT_EQ(args::parse<uint16_t>("-123"), std::nullopt);
  EXPECT_EQ(args::parse<uint16_t>("1000000"), std::nullopt);
  EXPECT_EQ(args::parse<uint16_t>("0"), 0);
  EXPECT_EQ(args::parse<uint16_t>("123"), 123);
}

TEST(Args, ParseNumberNatural32) {
  EXPECT_EQ(args::parse<uint32_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<uint32_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<uint32_t>("-0"), std::nullopt);
  EXPECT_EQ(args::parse<uint32_t>("-123"), std::nullopt);
  EXPECT_EQ(args::parse<uint32_t>("10000000000"), std::nullopt);
  EXPECT_EQ(args::parse<uint32_t>("0"), 0);
  EXPECT_EQ(args::parse<uint32_t>("123"), 123);
}

TEST(Args, ParseNumberNatural64) {
  EXPECT_EQ(args::parse<uint64_t>(""), std::nullopt);
  EXPECT_EQ(args::parse<uint64_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse<uint64_t>("-0"), std::nullopt);
  EXPECT_EQ(args::parse<uint64_t>("-123"), std::nullopt);
  EXPECT_EQ(args::parse<uint64_t>("1000000000000000000000000000000"), std::nullopt);
  EXPECT_EQ(args::parse<uint64_t>("0"), 0);
  EXPECT_EQ(args::parse<uint64_t>("123"), 123);
}

TEST(Args, ParseString) {
  EXPECT_EQ(args::parse<std::string>(""), std::string{""});
  EXPECT_EQ(args::parse<std::string>("Hello, world!"), std::string{"Hello, world!"});
}

}  // namespace
