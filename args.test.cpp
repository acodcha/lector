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

#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include <optional>
#include <string>

namespace {

TEST(Args, ParseBoolean) {
  EXPECT_EQ(args::parse_boolean(""), std::nullopt);
  EXPECT_EQ(args::parse_boolean("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_boolean("TRUE"), true);
  EXPECT_EQ(args::parse_boolean("True"), true);
  EXPECT_EQ(args::parse_boolean("true"), true);
  EXPECT_EQ(args::parse_boolean("1"), true);
  EXPECT_EQ(args::parse_boolean("FALSE"), false);
  EXPECT_EQ(args::parse_boolean("False"), false);
  EXPECT_EQ(args::parse_boolean("false"), false);
  EXPECT_EQ(args::parse_boolean("0"), false);
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

TEST(Args, ParseTypeFilesystemPath) {
  EXPECT_EQ(args::parse_type<std::filesystem::path>(""), std::filesystem::path{""});
  EXPECT_EQ(
      args::parse_type<std::filesystem::path>("some_path"), std::filesystem::path{"some_path"});
  EXPECT_EQ(args::parse_type<std::filesystem::path>("some_file.txt"),
            std::filesystem::path{"some_file.txt"});
  EXPECT_EQ(
      args::parse_type<std::filesystem::path>("/some/path"), std::filesystem::path{"/some/path"});
  EXPECT_EQ(args::parse_type<std::filesystem::path>("/some/file.txt"),
            std::filesystem::path{"/some/file.txt"});
}

TEST(Args, ParseTypeNumberDouble) {
  EXPECT_EQ(args::parse_type<double>(""), std::nullopt);
  EXPECT_EQ(args::parse_type<double>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_type<double>("0"), 0.0);
  EXPECT_EQ(args::parse_type<double>("0.0F"), 0.0);
  EXPECT_EQ(args::parse_type<double>("0.0"), 0.0);
  EXPECT_EQ(args::parse_type<double>("0.0L"), 0.0);
  EXPECT_EQ(args::parse_type<double>("-0"), 0.0);
  EXPECT_EQ(args::parse_type<double>("-0.0F"), 0.0);
  EXPECT_EQ(args::parse_type<double>("-0.0"), 0.0);
  EXPECT_EQ(args::parse_type<double>("-0.0L"), 0.0);
  EXPECT_EQ(args::parse_type<double>("1"), 1.0);
  EXPECT_EQ(args::parse_type<double>("1.0F"), 1.0);
  EXPECT_EQ(args::parse_type<double>("1.0"), 1.0);
  EXPECT_EQ(args::parse_type<double>("1.0L"), 1.0);
  EXPECT_EQ(args::parse_type<double>("-1.23456789"), -1.23456789);
  EXPECT_EQ(args::parse_type<double>("-1.23456789F"), -1.23456789);
  EXPECT_EQ(args::parse_type<double>("-1.23456789L"), -1.23456789);
  EXPECT_EQ(args::parse_type<double>("-1.23456789E-12"), -1.23456789E-12);
  EXPECT_EQ(args::parse_type<double>("-1.23456789E-12F"), -1.23456789E-12);
  EXPECT_EQ(args::parse_type<double>("-1.23456789E-12L"), -1.23456789E-12);
}

TEST(Args, ParseTypeNumberFloat) {
  EXPECT_EQ(args::parse_type<float>(""), std::nullopt);
  EXPECT_EQ(args::parse_type<float>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_type<float>("0"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("0.0F"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("0.0"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("0.0L"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("-0"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("-0.0F"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("-0.0"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("-0.0L"), 0.0F);
  EXPECT_EQ(args::parse_type<float>("1"), 1.0F);
  EXPECT_EQ(args::parse_type<float>("1.0F"), 1.0F);
  EXPECT_EQ(args::parse_type<float>("1.0"), 1.0F);
  EXPECT_EQ(args::parse_type<float>("1.0L"), 1.0F);
  EXPECT_EQ(args::parse_type<float>("-1.23456789"), -1.23456789F);
  EXPECT_EQ(args::parse_type<float>("-1.23456789F"), -1.23456789F);
  EXPECT_EQ(args::parse_type<float>("-1.23456789L"), -1.23456789F);
  EXPECT_EQ(args::parse_type<float>("-1.23456789E-12"), -1.23456789E-12F);
  EXPECT_EQ(args::parse_type<float>("-1.23456789E-12F"), -1.23456789E-12F);
  EXPECT_EQ(args::parse_type<float>("-1.23456789E-12L"), -1.23456789E-12F);
}

TEST(Args, ParseTypeNumberInteger) {
  EXPECT_EQ(args::parse_type<int64_t>(""), std::nullopt);
  EXPECT_EQ(args::parse_type<int64_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_type<int64_t>("0"), 0);
  EXPECT_EQ(args::parse_type<int64_t>("123"), 123);
  EXPECT_EQ(args::parse_type<int64_t>("-123"), -123);
}

TEST(Args, ParseTypeNumberLongDouble) {
  EXPECT_EQ(args::parse_type<long double>(""), std::nullopt);
  EXPECT_EQ(args::parse_type<long double>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_type<long double>("0"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("0.0F"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("0.0"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("0.0L"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("-0"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("-0.0F"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("-0.0"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("-0.0L"), 0.0L);
  EXPECT_EQ(args::parse_type<long double>("1"), 1.0L);
  EXPECT_EQ(args::parse_type<long double>("1.0F"), 1.0L);
  EXPECT_EQ(args::parse_type<long double>("1.0"), 1.0L);
  EXPECT_EQ(args::parse_type<long double>("1.0L"), 1.0L);
  EXPECT_EQ(args::parse_type<long double>("-1.23456789"), -1.23456789L);
  EXPECT_EQ(args::parse_type<long double>("-1.23456789F"), -1.23456789L);
  EXPECT_EQ(args::parse_type<long double>("-1.23456789L"), -1.23456789L);
  EXPECT_EQ(args::parse_type<long double>("-1.23456789E-12"), -1.23456789E-12L);
  EXPECT_EQ(args::parse_type<long double>("-1.23456789E-12F"), -1.23456789E-12L);
  EXPECT_EQ(args::parse_type<long double>("-1.23456789E-12L"), -1.23456789E-12L);
}

TEST(Args, ParseTypeNumberNatural) {
  EXPECT_EQ(args::parse_type<uint64_t>(""), std::nullopt);
  EXPECT_EQ(args::parse_type<uint64_t>("Hello, world!"), std::nullopt);
  EXPECT_EQ(args::parse_type<uint64_t>("0"), 0);
  EXPECT_EQ(args::parse_type<uint64_t>("123"), 123);
}

TEST(Args, ParseTypeString) {
  EXPECT_EQ(args::parse_type<std::string>(""), std::string{""});
  EXPECT_EQ(args::parse_type<std::string>("Hello, world!"), std::string{"Hello, world!"});
}

}  // namespace
