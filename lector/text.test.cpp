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

#include "lector/text.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

TEST(Lector, ByteIntervalAscii) {
  {
    constexpr std::pair<std::size_t, std::size_t> expected{0UL, 1UL};
    EXPECT_EQ(lector::byte_interval("Hi!", static_cast<std::size_t>(0UL)), expected);
  }
  {
    constexpr std::pair<std::size_t, std::size_t> expected{1UL, 2UL};
    EXPECT_EQ(lector::byte_interval("Hi!", static_cast<std::size_t>(1UL)), expected);
  }
  {
    constexpr std::pair<std::size_t, std::size_t> expected{2UL, 3UL};
    EXPECT_EQ(lector::byte_interval("Hi!", static_cast<std::size_t>(2UL)), expected);
  }
  {
    constexpr std::pair<std::size_t, std::size_t> expected{3UL, 3UL};
    EXPECT_EQ(lector::byte_interval("Hi!", static_cast<std::size_t>(3UL)), expected);
  }
  {
    constexpr std::pair<std::size_t, std::size_t> expected{3UL, 3UL};
    EXPECT_EQ(lector::byte_interval("Hi!", static_cast<std::size_t>(4UL)), expected);
  }
  {
    constexpr std::pair<std::size_t, std::size_t> expected{3UL, 3UL};
    EXPECT_EQ(lector::byte_interval("Hi!", static_cast<std::size_t>(5UL)), expected);
  }
}

TEST(Lector, ByteIntervalEmpty) {
  constexpr std::pair<std::size_t, std::size_t> expected{0UL, 0UL};
  EXPECT_EQ(lector::byte_interval("", static_cast<std::size_t>(0UL)), expected);
  EXPECT_EQ(lector::byte_interval("", static_cast<std::size_t>(1UL)), expected);
  EXPECT_EQ(lector::byte_interval("", static_cast<std::size_t>(2UL)), expected);
  EXPECT_EQ(lector::byte_interval("", static_cast<std::size_t>(3UL)), expected);
}

TEST(Lector, ByteIntervalUtf8) {
  {
    const std::pair<std::size_t, std::size_t> expected{0UL, 2UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(0UL)), expected);
  }
  {
    const std::pair<std::size_t, std::size_t> expected{2UL, 3UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(1UL)), expected);
  }
  {
    const std::pair<std::size_t, std::size_t> expected{3UL, 5UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(2UL)), expected);
  }
  {
    const std::pair<std::size_t, std::size_t> expected{5UL, 6UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(3UL)), expected);
  }
  {
    const std::pair<std::size_t, std::size_t> expected{6UL, 6UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(4UL)), expected);
  }
  {
    const std::pair<std::size_t, std::size_t> expected{6UL, 6UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(5UL)), expected);
  }
  {
    const std::pair<std::size_t, std::size_t> expected{6UL, 6UL};
    EXPECT_EQ(lector::byte_interval("Épée", static_cast<std::size_t>(6UL)), expected);
  }
}

TEST(Lector, CodePoints) {
  EXPECT_EQ(lector::code_points(""), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::code_points("Hello!!!"), static_cast<std::size_t>(8UL));
  EXPECT_EQ(lector::code_points("$5"), static_cast<std::size_t>(2UL));
  EXPECT_EQ(lector::code_points("¢25"), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::code_points("£5"), static_cast<std::size_t>(2UL));
  EXPECT_EQ(lector::code_points("¥500"), static_cast<std::size_t>(4UL));
  EXPECT_EQ(lector::code_points("5€"), static_cast<std::size_t>(2UL));
  EXPECT_EQ(lector::code_points("château"), static_cast<std::size_t>(7UL));
  EXPECT_EQ(lector::code_points("été"), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::code_points("œuf"), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::code_points("こんにちは"), static_cast<std::size_t>(5UL));
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "             the second\n"
            "              column.");
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "       This is the first column.          And this is\n"
            "                                           the second\n"
            "                                            column.");
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "  Hello.    And this is\n"
            "             the second\n"
            "              column.");
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias("    ", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "             the second\n"
            "              column.");
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasTypical) {
  EXPECT_EQ(lector::collate_and_centre_align_with_left_bias(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            "This is   And this is\n"
            "  the      the second\n"
            " first      column.\n"
            "column.");
}

TEST(Lector, CollateAndCentreAlignWithLeftBiasVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_centre_align_with_left_bias("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-  And this is\n"
      " g_word.    the second\n"
      "             column.");
}

TEST(Lector, CollateAndCentreAlignWithRightBiasBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndCentreAlignWithRightBiasBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndCentreAlignWithRightBiasFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "             the second\n"
            "               column.");
}

TEST(Lector, CollateAndCentreAlignWithRightBiasFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "        This is the first column.          And this is\n"
            "                                           the second\n"
            "                                             column.");
}

TEST(Lector, CollateAndCentreAlignWithRightBiasFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "  Hello.     And this is\n"
            "             the second\n"
            "               column.");
}

TEST(Lector, CollateAndCentreAlignWithRightBiasFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias("    ", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "             the second\n"
            "               column.");
}

TEST(Lector, CollateAndCentreAlignWithRightBiasTypical) {
  EXPECT_EQ(lector::collate_and_centre_align_with_right_bias(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            " This is   And this is\n"
            "   the     the second\n"
            "  first      column.\n"
            " column.");
}

TEST(Lector, CollateAndCentreAlignWithRightBiasVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_centre_align_with_right_bias("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-   And this is\n"
      " g_word.    the second\n"
      "              column.");
}

TEST(Lector, CollateAndLeftAlignBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_left_align("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_left_align("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_left_align("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_left_align("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndLeftAlignBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_left_align("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_left_align("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_left_align("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_left_align("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndLeftAlignFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_left_align("", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CollateAndLeftAlignFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_left_align(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "This is the first column.                 And this is\n"
            "                                          the second\n"
            "                                          column.");
}

TEST(Lector, CollateAndLeftAlignFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_left_align("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "Hello.      And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CollateAndLeftAlignFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_left_align("    ", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CollateAndLeftAlignTypical) {
  EXPECT_EQ(lector::collate_and_left_align(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            "This is   And this is\n"
            "the       the second\n"
            "first     column.\n"
            "column.");
}

TEST(Lector, CollateAndLeftAlignVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_left_align("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-  And this is\n"
      "g_word.    the second\n"
      "           column.");
}

TEST(Lector, CollateAndRightAlignBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_right_align("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_right_align("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_right_align("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_right_align("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndRightAlignBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_right_align("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_right_align("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_right_align("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_right_align("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndRightAlignFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_right_align("", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "              the second\n"
            "                 column.");
}

TEST(Lector, CollateAndRightAlignFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_right_align(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "               This is the first column.   And this is\n"
            "                                            the second\n"
            "                                               column.");
}

TEST(Lector, CollateAndRightAlignFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_right_align("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "    Hello.   And this is\n"
            "              the second\n"
            "                 column.");
}

TEST(Lector, CollateAndRightAlignFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_right_align("    ", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "              the second\n"
            "                 column.");
}

TEST(Lector, CollateAndRightAlignTypical) {
  EXPECT_EQ(lector::collate_and_right_align(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            " This is   And this is\n"
            "     the    the second\n"
            "   first       column.\n"
            " column.");
}

TEST(Lector, CollateAndRightAlignVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_right_align("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-   And this is\n"
      "  g_word.    the second\n"
      "                column.");
}

TEST(Lector, IsLeadingByte) {
  EXPECT_TRUE(lector::is_leading_byte('\0'));
  EXPECT_TRUE(lector::is_leading_byte('\x01'));
  EXPECT_TRUE(lector::is_leading_byte('\x02'));
  EXPECT_TRUE(lector::is_leading_byte('\x03'));
  EXPECT_TRUE(lector::is_leading_byte('\x04'));
  EXPECT_TRUE(lector::is_leading_byte('\x05'));
  EXPECT_TRUE(lector::is_leading_byte('\x06'));
  EXPECT_TRUE(lector::is_leading_byte('\x07'));
  EXPECT_TRUE(lector::is_leading_byte('\x08'));
  EXPECT_TRUE(lector::is_leading_byte('\t'));
  EXPECT_TRUE(lector::is_leading_byte('\n'));
  EXPECT_TRUE(lector::is_leading_byte('\v'));
  EXPECT_TRUE(lector::is_leading_byte('\f'));
  EXPECT_TRUE(lector::is_leading_byte('\r'));
  EXPECT_TRUE(lector::is_leading_byte('\x0E'));
  EXPECT_TRUE(lector::is_leading_byte('\x0F'));
  EXPECT_TRUE(lector::is_leading_byte('\x10'));
  EXPECT_TRUE(lector::is_leading_byte('\x11'));
  EXPECT_TRUE(lector::is_leading_byte('\x12'));
  EXPECT_TRUE(lector::is_leading_byte('\x13'));
  EXPECT_TRUE(lector::is_leading_byte('\x14'));
  EXPECT_TRUE(lector::is_leading_byte('\x15'));
  EXPECT_TRUE(lector::is_leading_byte('\x16'));
  EXPECT_TRUE(lector::is_leading_byte('\x17'));
  EXPECT_TRUE(lector::is_leading_byte('\x18'));
  EXPECT_TRUE(lector::is_leading_byte('\x19'));
  EXPECT_TRUE(lector::is_leading_byte('\x1A'));
  EXPECT_TRUE(lector::is_leading_byte('\x1B'));
  EXPECT_TRUE(lector::is_leading_byte('\x1C'));
  EXPECT_TRUE(lector::is_leading_byte('\x1D'));
  EXPECT_TRUE(lector::is_leading_byte('\x1E'));
  EXPECT_TRUE(lector::is_leading_byte('\x1F'));
  EXPECT_TRUE(lector::is_leading_byte(' '));
  EXPECT_TRUE(lector::is_leading_byte('!'));
  EXPECT_TRUE(lector::is_leading_byte('"'));
  EXPECT_TRUE(lector::is_leading_byte('#'));
  EXPECT_TRUE(lector::is_leading_byte('$'));
  EXPECT_TRUE(lector::is_leading_byte('%'));
  EXPECT_TRUE(lector::is_leading_byte('&'));
  EXPECT_TRUE(lector::is_leading_byte('\''));
  EXPECT_TRUE(lector::is_leading_byte('('));
  EXPECT_TRUE(lector::is_leading_byte(')'));
  EXPECT_TRUE(lector::is_leading_byte('*'));
  EXPECT_TRUE(lector::is_leading_byte('+'));
  EXPECT_TRUE(lector::is_leading_byte(','));
  EXPECT_TRUE(lector::is_leading_byte('-'));
  EXPECT_TRUE(lector::is_leading_byte('.'));
  EXPECT_TRUE(lector::is_leading_byte('/'));
  EXPECT_TRUE(lector::is_leading_byte('0'));
  EXPECT_TRUE(lector::is_leading_byte('1'));
  EXPECT_TRUE(lector::is_leading_byte('2'));
  EXPECT_TRUE(lector::is_leading_byte('3'));
  EXPECT_TRUE(lector::is_leading_byte('4'));
  EXPECT_TRUE(lector::is_leading_byte('5'));
  EXPECT_TRUE(lector::is_leading_byte('6'));
  EXPECT_TRUE(lector::is_leading_byte('7'));
  EXPECT_TRUE(lector::is_leading_byte('8'));
  EXPECT_TRUE(lector::is_leading_byte('9'));
  EXPECT_TRUE(lector::is_leading_byte(':'));
  EXPECT_TRUE(lector::is_leading_byte(';'));
  EXPECT_TRUE(lector::is_leading_byte('<'));
  EXPECT_TRUE(lector::is_leading_byte('='));
  EXPECT_TRUE(lector::is_leading_byte('>'));
  EXPECT_TRUE(lector::is_leading_byte('?'));
  EXPECT_TRUE(lector::is_leading_byte('@'));
  EXPECT_TRUE(lector::is_leading_byte('A'));
  EXPECT_TRUE(lector::is_leading_byte('B'));
  EXPECT_TRUE(lector::is_leading_byte('C'));
  EXPECT_TRUE(lector::is_leading_byte('D'));
  EXPECT_TRUE(lector::is_leading_byte('E'));
  EXPECT_TRUE(lector::is_leading_byte('F'));
  EXPECT_TRUE(lector::is_leading_byte('G'));
  EXPECT_TRUE(lector::is_leading_byte('H'));
  EXPECT_TRUE(lector::is_leading_byte('I'));
  EXPECT_TRUE(lector::is_leading_byte('J'));
  EXPECT_TRUE(lector::is_leading_byte('K'));
  EXPECT_TRUE(lector::is_leading_byte('L'));
  EXPECT_TRUE(lector::is_leading_byte('M'));
  EXPECT_TRUE(lector::is_leading_byte('N'));
  EXPECT_TRUE(lector::is_leading_byte('O'));
  EXPECT_TRUE(lector::is_leading_byte('P'));
  EXPECT_TRUE(lector::is_leading_byte('Q'));
  EXPECT_TRUE(lector::is_leading_byte('R'));
  EXPECT_TRUE(lector::is_leading_byte('S'));
  EXPECT_TRUE(lector::is_leading_byte('T'));
  EXPECT_TRUE(lector::is_leading_byte('U'));
  EXPECT_TRUE(lector::is_leading_byte('V'));
  EXPECT_TRUE(lector::is_leading_byte('W'));
  EXPECT_TRUE(lector::is_leading_byte('X'));
  EXPECT_TRUE(lector::is_leading_byte('Y'));
  EXPECT_TRUE(lector::is_leading_byte('Z'));
  EXPECT_TRUE(lector::is_leading_byte('['));
  EXPECT_TRUE(lector::is_leading_byte('\\'));
  EXPECT_TRUE(lector::is_leading_byte(']'));
  EXPECT_TRUE(lector::is_leading_byte('^'));
  EXPECT_TRUE(lector::is_leading_byte('_'));
  EXPECT_TRUE(lector::is_leading_byte('`'));
  EXPECT_TRUE(lector::is_leading_byte('a'));
  EXPECT_TRUE(lector::is_leading_byte('b'));
  EXPECT_TRUE(lector::is_leading_byte('c'));
  EXPECT_TRUE(lector::is_leading_byte('d'));
  EXPECT_TRUE(lector::is_leading_byte('e'));
  EXPECT_TRUE(lector::is_leading_byte('f'));
  EXPECT_TRUE(lector::is_leading_byte('g'));
  EXPECT_TRUE(lector::is_leading_byte('h'));
  EXPECT_TRUE(lector::is_leading_byte('i'));
  EXPECT_TRUE(lector::is_leading_byte('j'));
  EXPECT_TRUE(lector::is_leading_byte('k'));
  EXPECT_TRUE(lector::is_leading_byte('l'));
  EXPECT_TRUE(lector::is_leading_byte('m'));
  EXPECT_TRUE(lector::is_leading_byte('n'));
  EXPECT_TRUE(lector::is_leading_byte('o'));
  EXPECT_TRUE(lector::is_leading_byte('p'));
  EXPECT_TRUE(lector::is_leading_byte('q'));
  EXPECT_TRUE(lector::is_leading_byte('r'));
  EXPECT_TRUE(lector::is_leading_byte('s'));
  EXPECT_TRUE(lector::is_leading_byte('t'));
  EXPECT_TRUE(lector::is_leading_byte('u'));
  EXPECT_TRUE(lector::is_leading_byte('v'));
  EXPECT_TRUE(lector::is_leading_byte('w'));
  EXPECT_TRUE(lector::is_leading_byte('x'));
  EXPECT_TRUE(lector::is_leading_byte('y'));
  EXPECT_TRUE(lector::is_leading_byte('z'));
  EXPECT_TRUE(lector::is_leading_byte('{'));
  EXPECT_TRUE(lector::is_leading_byte('|'));
  EXPECT_TRUE(lector::is_leading_byte('}'));
  EXPECT_TRUE(lector::is_leading_byte('~'));
  EXPECT_TRUE(lector::is_leading_byte('\x7F'));
  EXPECT_FALSE(lector::is_leading_byte('\x80'));
  EXPECT_FALSE(lector::is_leading_byte('\x81'));
  EXPECT_FALSE(lector::is_leading_byte('\x82'));
  EXPECT_FALSE(lector::is_leading_byte('\x83'));
  EXPECT_FALSE(lector::is_leading_byte('\x84'));
  EXPECT_FALSE(lector::is_leading_byte('\x85'));
  EXPECT_FALSE(lector::is_leading_byte('\x86'));
  EXPECT_FALSE(lector::is_leading_byte('\x87'));
  EXPECT_FALSE(lector::is_leading_byte('\x88'));
  EXPECT_FALSE(lector::is_leading_byte('\x89'));
  EXPECT_FALSE(lector::is_leading_byte('\x8A'));
  EXPECT_FALSE(lector::is_leading_byte('\x8B'));
  EXPECT_FALSE(lector::is_leading_byte('\x8C'));
  EXPECT_FALSE(lector::is_leading_byte('\x8D'));
  EXPECT_FALSE(lector::is_leading_byte('\x8E'));
  EXPECT_FALSE(lector::is_leading_byte('\x8F'));
  EXPECT_FALSE(lector::is_leading_byte('\x90'));
  EXPECT_FALSE(lector::is_leading_byte('\x91'));
  EXPECT_FALSE(lector::is_leading_byte('\x92'));
  EXPECT_FALSE(lector::is_leading_byte('\x93'));
  EXPECT_FALSE(lector::is_leading_byte('\x94'));
  EXPECT_FALSE(lector::is_leading_byte('\x95'));
  EXPECT_FALSE(lector::is_leading_byte('\x96'));
  EXPECT_FALSE(lector::is_leading_byte('\x97'));
  EXPECT_FALSE(lector::is_leading_byte('\x98'));
  EXPECT_FALSE(lector::is_leading_byte('\x99'));
  EXPECT_FALSE(lector::is_leading_byte('\x9A'));
  EXPECT_FALSE(lector::is_leading_byte('\x9B'));
  EXPECT_FALSE(lector::is_leading_byte('\x9C'));
  EXPECT_FALSE(lector::is_leading_byte('\x9D'));
  EXPECT_FALSE(lector::is_leading_byte('\x9E'));
  EXPECT_FALSE(lector::is_leading_byte('\x9F'));
  EXPECT_FALSE(lector::is_leading_byte('\xA0'));
  EXPECT_FALSE(lector::is_leading_byte('\xA1'));
  EXPECT_FALSE(lector::is_leading_byte('\xA2'));
  EXPECT_FALSE(lector::is_leading_byte('\xA3'));
  EXPECT_FALSE(lector::is_leading_byte('\xA4'));
  EXPECT_FALSE(lector::is_leading_byte('\xA5'));
  EXPECT_FALSE(lector::is_leading_byte('\xA6'));
  EXPECT_FALSE(lector::is_leading_byte('\xA7'));
  EXPECT_FALSE(lector::is_leading_byte('\xA8'));
  EXPECT_FALSE(lector::is_leading_byte('\xA9'));
  EXPECT_FALSE(lector::is_leading_byte('\xAA'));
  EXPECT_FALSE(lector::is_leading_byte('\xAB'));
  EXPECT_FALSE(lector::is_leading_byte('\xAC'));
  EXPECT_FALSE(lector::is_leading_byte('\xAD'));
  EXPECT_FALSE(lector::is_leading_byte('\xAE'));
  EXPECT_FALSE(lector::is_leading_byte('\xAF'));
  EXPECT_FALSE(lector::is_leading_byte('\xB0'));
  EXPECT_FALSE(lector::is_leading_byte('\xB1'));
  EXPECT_FALSE(lector::is_leading_byte('\xB2'));
  EXPECT_FALSE(lector::is_leading_byte('\xB3'));
  EXPECT_FALSE(lector::is_leading_byte('\xB4'));
  EXPECT_FALSE(lector::is_leading_byte('\xB5'));
  EXPECT_FALSE(lector::is_leading_byte('\xB6'));
  EXPECT_FALSE(lector::is_leading_byte('\xB7'));
  EXPECT_FALSE(lector::is_leading_byte('\xB8'));
  EXPECT_FALSE(lector::is_leading_byte('\xB9'));
  EXPECT_FALSE(lector::is_leading_byte('\xBA'));
  EXPECT_FALSE(lector::is_leading_byte('\xBB'));
  EXPECT_FALSE(lector::is_leading_byte('\xBC'));
  EXPECT_FALSE(lector::is_leading_byte('\xBD'));
  EXPECT_FALSE(lector::is_leading_byte('\xBE'));
  EXPECT_FALSE(lector::is_leading_byte('\xBF'));
  EXPECT_TRUE(lector::is_leading_byte('\xC0'));
  EXPECT_TRUE(lector::is_leading_byte('\xC1'));
  EXPECT_TRUE(lector::is_leading_byte('\xC2'));
  EXPECT_TRUE(lector::is_leading_byte('\xC3'));
  EXPECT_TRUE(lector::is_leading_byte('\xC4'));
  EXPECT_TRUE(lector::is_leading_byte('\xC5'));
  EXPECT_TRUE(lector::is_leading_byte('\xC6'));
  EXPECT_TRUE(lector::is_leading_byte('\xC7'));
  EXPECT_TRUE(lector::is_leading_byte('\xC8'));
  EXPECT_TRUE(lector::is_leading_byte('\xC9'));
  EXPECT_TRUE(lector::is_leading_byte('\xCA'));
  EXPECT_TRUE(lector::is_leading_byte('\xCB'));
  EXPECT_TRUE(lector::is_leading_byte('\xCC'));
  EXPECT_TRUE(lector::is_leading_byte('\xCD'));
  EXPECT_TRUE(lector::is_leading_byte('\xCE'));
  EXPECT_TRUE(lector::is_leading_byte('\xCF'));
  EXPECT_TRUE(lector::is_leading_byte('\xD0'));
  EXPECT_TRUE(lector::is_leading_byte('\xD1'));
  EXPECT_TRUE(lector::is_leading_byte('\xD2'));
  EXPECT_TRUE(lector::is_leading_byte('\xD3'));
  EXPECT_TRUE(lector::is_leading_byte('\xD4'));
  EXPECT_TRUE(lector::is_leading_byte('\xD5'));
  EXPECT_TRUE(lector::is_leading_byte('\xD6'));
  EXPECT_TRUE(lector::is_leading_byte('\xD7'));
  EXPECT_TRUE(lector::is_leading_byte('\xD8'));
  EXPECT_TRUE(lector::is_leading_byte('\xD9'));
  EXPECT_TRUE(lector::is_leading_byte('\xDA'));
  EXPECT_TRUE(lector::is_leading_byte('\xDB'));
  EXPECT_TRUE(lector::is_leading_byte('\xDC'));
  EXPECT_TRUE(lector::is_leading_byte('\xDD'));
  EXPECT_TRUE(lector::is_leading_byte('\xDE'));
  EXPECT_TRUE(lector::is_leading_byte('\xDF'));
  EXPECT_TRUE(lector::is_leading_byte('\xE0'));
  EXPECT_TRUE(lector::is_leading_byte('\xE1'));
  EXPECT_TRUE(lector::is_leading_byte('\xE2'));
  EXPECT_TRUE(lector::is_leading_byte('\xE3'));
  EXPECT_TRUE(lector::is_leading_byte('\xE4'));
  EXPECT_TRUE(lector::is_leading_byte('\xE5'));
  EXPECT_TRUE(lector::is_leading_byte('\xE6'));
  EXPECT_TRUE(lector::is_leading_byte('\xE7'));
  EXPECT_TRUE(lector::is_leading_byte('\xE8'));
  EXPECT_TRUE(lector::is_leading_byte('\xE9'));
  EXPECT_TRUE(lector::is_leading_byte('\xEA'));
  EXPECT_TRUE(lector::is_leading_byte('\xEB'));
  EXPECT_TRUE(lector::is_leading_byte('\xEC'));
  EXPECT_TRUE(lector::is_leading_byte('\xED'));
  EXPECT_TRUE(lector::is_leading_byte('\xEE'));
  EXPECT_TRUE(lector::is_leading_byte('\xEF'));
  EXPECT_TRUE(lector::is_leading_byte('\xF0'));
  EXPECT_TRUE(lector::is_leading_byte('\xF1'));
  EXPECT_TRUE(lector::is_leading_byte('\xF2'));
  EXPECT_TRUE(lector::is_leading_byte('\xF3'));
  EXPECT_TRUE(lector::is_leading_byte('\xF4'));
  EXPECT_TRUE(lector::is_leading_byte('\xF5'));
  EXPECT_TRUE(lector::is_leading_byte('\xF6'));
  EXPECT_TRUE(lector::is_leading_byte('\xF7'));
  EXPECT_TRUE(lector::is_leading_byte('\xF8'));
  EXPECT_TRUE(lector::is_leading_byte('\xF9'));
  EXPECT_TRUE(lector::is_leading_byte('\xFA'));
  EXPECT_TRUE(lector::is_leading_byte('\xFB'));
  EXPECT_TRUE(lector::is_leading_byte('\xFC'));
  EXPECT_TRUE(lector::is_leading_byte('\xFD'));
  EXPECT_TRUE(lector::is_leading_byte('\xFE'));
  EXPECT_TRUE(lector::is_leading_byte('\xFF'));
}

TEST(Lector, JoinAndCentreAlignWithLeftBiasEmpty) {
  EXPECT_EQ(lector::join_and_centre_align_with_left_bias(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndCentreAlignWithLeftBiasMultiple) {
  EXPECT_EQ(lector::join_and_centre_align_with_left_bias(
                {"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
            "        Hello!  \n     Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndCentreAlignWithLeftBiasSingle) {
  EXPECT_EQ(
      lector::join_and_centre_align_with_left_bias({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndCentreAlignWithLeftBiasUtf8) {
  EXPECT_EQ(lector::join_and_centre_align_with_left_bias({"  Château.  ", "  Épée.  "}),
            "  Château.  \n   Épée.  ");
}

TEST(Lector, JoinAndCentreAlignWithRightBiasEmpty) {
  EXPECT_EQ(lector::join_and_centre_align_with_right_bias(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndCentreAlignWithRightBiasMultiple) {
  EXPECT_EQ(lector::join_and_centre_align_with_right_bias(
                {"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
            "         Hello!  \n     Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndCentreAlignWithRightBiasSingle) {
  EXPECT_EQ(
      lector::join_and_centre_align_with_right_bias({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndCentreAlignWithRightBiasUtf8) {
  EXPECT_EQ(lector::join_and_centre_align_with_right_bias({"  Château.  ", "  Épée.  "}),
            "  Château.  \n    Épée.  ");
}

TEST(Lector, JoinAndLeftAlignEmpty) {
  EXPECT_EQ(lector::join_and_left_align(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndLeftAlignMultiple) {
  EXPECT_EQ(
      lector::join_and_left_align({"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
      "  Hello!  \n  Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndLeftAlignSingle) {
  EXPECT_EQ(lector::join_and_left_align({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndLeftAlignUtf8) {
  EXPECT_EQ(lector::join_and_left_align({"  Château.  ", "  Épée.  "}), "  Château.  \n  Épée.  ");
}

TEST(Lector, JoinAndRightAlignEmpty) {
  EXPECT_EQ(lector::join_and_right_align(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndRightAlignMultiple) {
  EXPECT_EQ(
      lector::join_and_right_align({"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
      "               Hello!  \n        Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndRightAlignSingle) {
  EXPECT_EQ(lector::join_and_right_align({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndRightAlignUtf8) {
  EXPECT_EQ(
      lector::join_and_right_align({"  Château.  ", "  Épée.  "}), "  Château.  \n     Épée.  ");
}

TEST(Lector, LongestWordLengthEmpty) {
  EXPECT_EQ(lector::longest_word_length(""), static_cast<std::size_t>(0UL));
}

TEST(Lector, LongestWordLengthExcessiveWhitespace) {
  EXPECT_EQ(lector::longest_word_length("  Hello there!  "), static_cast<std::size_t>(6UL));
  EXPECT_EQ(lector::longest_word_length("  It is $5.  "), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("  It is ¢25.  "), static_cast<std::size_t>(4UL));
  EXPECT_EQ(lector::longest_word_length("  It is £5.  "), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("  It is ¥500.  "), static_cast<std::size_t>(5UL));
  EXPECT_EQ(lector::longest_word_length("  It is 5€.  "), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("  C'est un château.  "), static_cast<std::size_t>(8UL));
  EXPECT_EQ(lector::longest_word_length("  J'ai hâte à l'été!  "), static_cast<std::size_t>(6UL));
  EXPECT_EQ(lector::longest_word_length("  Un œuf.  "), static_cast<std::size_t>(4UL));
  EXPECT_EQ(lector::longest_word_length("  こんにちは、友よ！  "), static_cast<std::size_t>(9UL));
}

TEST(Lector, LongestWordLengthMultiple) {
  EXPECT_EQ(lector::longest_word_length("Hello there!"), static_cast<std::size_t>(6UL));
  EXPECT_EQ(lector::longest_word_length("It is $5."), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("It is ¢25."), static_cast<std::size_t>(4UL));
  EXPECT_EQ(lector::longest_word_length("It is £5."), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("It is ¥500."), static_cast<std::size_t>(5UL));
  EXPECT_EQ(lector::longest_word_length("It is 5€."), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("C'est un château."), static_cast<std::size_t>(8UL));
  EXPECT_EQ(lector::longest_word_length("J'ai hâte à l'été!"), static_cast<std::size_t>(6UL));
  EXPECT_EQ(lector::longest_word_length("Un œuf."), static_cast<std::size_t>(4UL));
  EXPECT_EQ(lector::longest_word_length("こんにちは、友よ！"), static_cast<std::size_t>(9UL));
}

TEST(Lector, LongestWordLengthSingle) {
  EXPECT_EQ(lector::longest_word_length("Hello!!!"), static_cast<std::size_t>(8UL));
  EXPECT_EQ(lector::longest_word_length("$5"), static_cast<std::size_t>(2UL));
  EXPECT_EQ(lector::longest_word_length("¢25"), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("£5"), static_cast<std::size_t>(2UL));
  EXPECT_EQ(lector::longest_word_length("¥500"), static_cast<std::size_t>(4UL));
  EXPECT_EQ(lector::longest_word_length("5€"), static_cast<std::size_t>(2UL));
  EXPECT_EQ(lector::longest_word_length("château"), static_cast<std::size_t>(7UL));
  EXPECT_EQ(lector::longest_word_length("été"), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("œuf"), static_cast<std::size_t>(3UL));
  EXPECT_EQ(lector::longest_word_length("こんにちは"), static_cast<std::size_t>(5UL));
}

TEST(Lector, LongestWordLengthWhitespaceOnly) {
  EXPECT_EQ(lector::longest_word_length(" "), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::longest_word_length("  "), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::longest_word_length("\t"), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::longest_word_length("\t\t"), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::longest_word_length("\n"), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::longest_word_length("\n\n"), static_cast<std::size_t>(0UL));
  EXPECT_EQ(lector::longest_word_length(" \t\n \t\n"), static_cast<std::size_t>(0UL));
}

TEST(Lector, PadLeftEmpty) {
  EXPECT_EQ(lector::pad_left("", 0), "");
  EXPECT_EQ(lector::pad_left("", 1), " ");
  EXPECT_EQ(lector::pad_left("", 2), "  ");
  EXPECT_EQ(lector::pad_left("", 3), "   ");
}

TEST(Lector, PadLeftLengthLong) {
  EXPECT_EQ(lector::pad_left("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::pad_left("Hello, world!", 14), " Hello, world!");
  EXPECT_EQ(lector::pad_left("Hello, world!", 15), "  Hello, world!");
  EXPECT_EQ(lector::pad_left("Hello, world!", 16), "   Hello, world!");
}

TEST(Lector, PadLeftLengthShort) {
  EXPECT_EQ(lector::pad_left("Hello, world!", 0), "Hello, world!");
  EXPECT_EQ(lector::pad_left("Hello, world!", 1), "Hello, world!");
  EXPECT_EQ(lector::pad_left("Hello, world!", 2), "Hello, world!");
  EXPECT_EQ(lector::pad_left("Hello, world!", 3), "Hello, world!");
}

TEST(Lector, PadLeftUtf8) {
  EXPECT_EQ(lector::pad_left("château", 10), "   château");
  EXPECT_EQ(lector::pad_left("été", 10), "       été");
  EXPECT_EQ(lector::pad_left("œuf", 10), "       œuf");
  EXPECT_EQ(lector::pad_left("こんにちは", 10), "     こんにちは");
}

TEST(Lector, PadLeftWhitespace) {
  EXPECT_EQ(lector::pad_left("  ", 10), "          ");
  EXPECT_EQ(lector::pad_left("\t", 10), "         \t");
  EXPECT_EQ(lector::pad_left("\n", 10), "         \n");
  EXPECT_EQ(lector::pad_left(" \t\n \t\n", 10), "     \t\n \t\n");
}

TEST(Lector, PadRightEmpty) {
  EXPECT_EQ(lector::pad_right("", 0), "");
  EXPECT_EQ(lector::pad_right("", 1), " ");
  EXPECT_EQ(lector::pad_right("", 2), "  ");
  EXPECT_EQ(lector::pad_right("", 3), "   ");
}

TEST(Lector, PadRightLengthLong) {
  EXPECT_EQ(lector::pad_right("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::pad_right("Hello, world!", 14), "Hello, world! ");
  EXPECT_EQ(lector::pad_right("Hello, world!", 15), "Hello, world!  ");
  EXPECT_EQ(lector::pad_right("Hello, world!", 16), "Hello, world!   ");
}

TEST(Lector, PadRightLengthShort) {
  EXPECT_EQ(lector::pad_right("Hello, world!", 0), "Hello, world!");
  EXPECT_EQ(lector::pad_right("Hello, world!", 1), "Hello, world!");
  EXPECT_EQ(lector::pad_right("Hello, world!", 2), "Hello, world!");
  EXPECT_EQ(lector::pad_right("Hello, world!", 3), "Hello, world!");
}

TEST(Lector, PadRightUtf8) {
  EXPECT_EQ(lector::pad_right("château", 10), "château   ");
  EXPECT_EQ(lector::pad_right("été", 10), "été       ");
  EXPECT_EQ(lector::pad_right("œuf", 10), "œuf       ");
  EXPECT_EQ(lector::pad_right("こんにちは", 10), "こんにちは     ");
}

TEST(Lector, PadRightWhitespace) {
  EXPECT_EQ(lector::pad_right("  ", 10), "          ");
  EXPECT_EQ(lector::pad_right("\t", 10), "\t         ");
  EXPECT_EQ(lector::pad_right("\n", 10), "\n         ");
  EXPECT_EQ(lector::pad_right(" \t\n \t\n", 10), " \t\n \t\n    ");
}

TEST(Lector, Tokenize) {
  EXPECT_EQ(lector::tokenize(""), std::vector<std::string_view>{});
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("Hello, world!"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("  Hello,    world!   "), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("Hello,\tworld!"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("\t\tHello,\t\t\t\tworld!\t\t\t"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("Hello,\vworld!"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("\v\vHello,\v\v\v\vworld!\v\v\v"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("Hello,\nworld!"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("\n\nHello,\n\n\n\nworld!\n\n\n"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("Hello,\fworld!"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("\f\fHello,\f\f\f\fworld!\f\f\f"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("Hello,\rworld!"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize("\r\rHello,\r\r\r\rworld!\r\r\r"), expected);
  }
  {
    const std::vector<std::string_view> expected{"Hello,", "world!"};
    EXPECT_EQ(lector::tokenize(" \t\v\n\f\rHello, \t\v\n\f\rworld! \t\v\n\f\r"), expected);
  }
}

TEST(Lector, WrapAndLeftAlignExcessiveWhitespace) {
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align(" Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("  Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align(" Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("  Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world!\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("\n\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world!\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("Hello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("\t\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_left_align(
                " \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
}

TEST(Lector, WrapAndLeftAlignInvalid) {
  EXPECT_ANY_THROW(
      (void)lector::wrap_and_left_align("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapAndLeftAlignLineLengthFive) {
  const std::string expected{"The\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog."};
  EXPECT_EQ(lector::wrap_and_left_align("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                        static_cast<std::size_t>(5UL)),
            expected);
}

TEST(Lector, WrapAndLeftAlignLineLengthFour) {
  const std::string expected{"Hel-\nlo,\nwor-\nld!"};
  EXPECT_EQ(
      lector::wrap_and_left_align("  Hello, world!  ", static_cast<std::size_t>(4UL)), expected);
}

TEST(Lector, WrapAndLeftAlignLineLengthOne) {
  const std::string expected{"H\ne\nl\nl\no\n,\nw\no\nr\nl\nd\n!"};
  EXPECT_EQ(
      lector::wrap_and_left_align("  Hello, world!  ", static_cast<std::size_t>(1UL)), expected);
}

TEST(Lector, WrapAndLeftAlignLineLengthThree) {
  const std::string expected{"He-\nll-\no,\nwo-\nrl-\nd!"};
  EXPECT_EQ(
      lector::wrap_and_left_align("  Hello, world!  ", static_cast<std::size_t>(3UL)), expected);
}

TEST(Lector, WrapAndLeftAlignLineLengthTwo) {
  const std::string expected{"H-\ne-\nl-\nl-\no,\nw-\no-\nr-\nl-\nd!"};
  EXPECT_EQ(
      lector::wrap_and_left_align("  Hello, world!  ", static_cast<std::size_t>(2UL)), expected);
}

TEST(Lector, WrapAndLeftAlignMultipleLines) {
  const std::string expected{"The quick\nbrown fox\njumps over\nthe lazy\ndog."};
  EXPECT_EQ(lector::wrap_and_left_align("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                        static_cast<std::size_t>(12UL)),
            expected);
}

TEST(Lector, WrapAndLeftAlignUtf8Characters) {
  const std::string expected{"J'ai hâte à\nl'été!"};
  EXPECT_EQ(
      lector::wrap_and_left_align("  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)),
      expected);
}

TEST(Lector, WrapAndLeftAlignVeryLongWord) {
  const std::string expected{
    "The word\nsupercali-\nfragilist-\nicexpiali-\ndocious is\nmy\nfavorite\nword!"};
  EXPECT_EQ(lector::wrap_and_left_align(
                "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                static_cast<std::size_t>(10UL)),
            expected);
}

TEST(Lector, WrapAndLeftAlignWhitespaceOnly) {
  EXPECT_EQ(lector::wrap_and_left_align("", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align(" ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align("  ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align("\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align("\t\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align("\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align("\n\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_left_align(" \t\n", static_cast<std::size_t>(100UL)), std::string{});
}

TEST(Lector, WrapAndRightAlignExcessiveWhitespace) {
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align(" Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("  Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align(" Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("  Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world!\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("\n\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world!\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("Hello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("\t\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_right_align(
                " \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
}

TEST(Lector, WrapAndRightAlignInvalid) {
  EXPECT_ANY_THROW(
      (void)lector::wrap_and_right_align("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapAndRightAlignLineLengthFive) {
  const std::string expected{"  The\nquick\nbrown\n  fox\njumps\n over\n  the\n lazy\n dog."};
  EXPECT_EQ(lector::wrap_and_right_align("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                         static_cast<std::size_t>(5UL)),
            expected);
}

TEST(Lector, WrapAndRightAlignLineLengthFour) {
  const std::string expected{"Hel-\n lo,\nwor-\n ld!"};
  EXPECT_EQ(
      lector::wrap_and_right_align("  Hello, world!  ", static_cast<std::size_t>(4UL)), expected);
}

TEST(Lector, WrapAndRightAlignLineLengthOne) {
  const std::string expected{"H\ne\nl\nl\no\n,\nw\no\nr\nl\nd\n!"};
  EXPECT_EQ(
      lector::wrap_and_right_align("  Hello, world!  ", static_cast<std::size_t>(1UL)), expected);
}

TEST(Lector, WrapAndRightAlignLineLengthThree) {
  const std::string expected{"He-\nll-\n o,\nwo-\nrl-\n d!"};
  EXPECT_EQ(
      lector::wrap_and_right_align("  Hello, world!  ", static_cast<std::size_t>(3UL)), expected);
}

TEST(Lector, WrapAndRightAlignLineLengthTwo) {
  const std::string expected{"H-\ne-\nl-\nl-\no,\nw-\no-\nr-\nl-\nd!"};
  EXPECT_EQ(
      lector::wrap_and_right_align("  Hello, world!  ", static_cast<std::size_t>(2UL)), expected);
}

TEST(Lector, WrapAndRightAlignMultipleLines) {
  const std::string expected{" The quick\n brown fox\njumps over\n  the lazy\n      dog."};
  EXPECT_EQ(lector::wrap_and_right_align("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                         static_cast<std::size_t>(12UL)),
            expected);
}

TEST(Lector, WrapAndRightAlignUtf8Characters) {
  const std::string expected{"J'ai hâte à\n     l'été!"};
  EXPECT_EQ(
      lector::wrap_and_right_align("  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)),
      expected);
}

TEST(Lector, WrapAndRightAlignVeryLongWord) {
  const std::
      string
          expected{
            "  The word\nsupercali-\nfragilist-\nicexpiali-\ndocious is\n        my\n  favorite\n"
            "     word!"};
  EXPECT_EQ(lector::wrap_and_right_align(
                "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                static_cast<std::size_t>(10UL)),
            expected);
}

TEST(Lector, WrapAndRightAlignWhitespaceOnly) {
  EXPECT_EQ(lector::wrap_and_right_align("", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align(" ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align("  ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align("\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align("\t\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align("\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align("\n\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_right_align(" \t\n", static_cast<std::size_t>(100UL)), std::string{});
}

TEST(Lector, WrapOnlyExcessiveWhitespace) {
  const std::vector<std::string> expected{"Hello, world!"};
  EXPECT_EQ(lector::wrap("Hello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("Hello, world! ", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("Hello, world!  ", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap(" Hello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("  Hello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap(" Hello, world! ", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("  Hello, world!  ", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("Hello, world!\n", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("Hello, world!\n\n", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\nHello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\n\nHello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("Hello, world!\t", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("Hello, world!\t\t", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\tHello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\t\tHello, world!", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(
      lector::wrap(" \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)), expected);
}

TEST(Lector, WrapOnlyInvalid) {
  EXPECT_ANY_THROW((void)lector::wrap("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapOnlyLineLengthFive) {
  const std::vector<std::string> expected{
    "The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog."};
  EXPECT_EQ(lector::wrap("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                         static_cast<std::size_t>(5UL)),
            expected);
}

TEST(Lector, WrapOnlyLineLengthFour) {
  const std::vector<std::string> expected{"Hel-", "lo,", "wor-", "ld!"};
  EXPECT_EQ(lector::wrap("  Hello,  world!  ", static_cast<std::size_t>(4UL)), expected);
}

TEST(Lector, WrapOnlyLineLengthOne) {
  const std::vector<std::string> expected{
    "H", "e", "l", "l", "o", ",", "w", "o", "r", "l", "d", "!"};
  EXPECT_EQ(lector::wrap("  Hello,  world!  ", static_cast<std::size_t>(1UL)), expected);
}

TEST(Lector, WrapOnlyLineLengthThree) {
  const std::vector<std::string> expected{"He-", "ll-", "o,", "wo-", "rl-", "d!"};
  EXPECT_EQ(lector::wrap("  Hello,  world!  ", static_cast<std::size_t>(3UL)), expected);
}

TEST(Lector, WrapOnlyLineLengthTwo) {
  const std::vector<std::string> expected{
    "H-", "e-", "l-", "l-", "o,", "w-", "o-", "r-", "l-", "d!"};
  EXPECT_EQ(lector::wrap("  Hello,  world!  ", static_cast<std::size_t>(2UL)), expected);
}

TEST(Lector, WrapOnlyMultipleLines) {
  const std::vector<std::string> expected{
    "The quick", "brown fox", "jumps over", "the lazy", "dog."};
  EXPECT_EQ(lector::wrap("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                         static_cast<std::size_t>(12UL)),
            expected);
}

TEST(Lector, WrapOnlyUtf8Characters) {
  const std::vector<std::string> expected{"J'ai hâte à", "l'été!"};
  EXPECT_EQ(lector::wrap("  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)), expected);
}

TEST(Lector, WrapOnlyVeryLongWord) {
  const std::vector<std::string> expected{
    "The word", "supercali-", "fragilist-", "icexpiali-", "docious is", "my", "favorite", "word!"};
  EXPECT_EQ(
      lector::wrap("  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                   static_cast<std::size_t>(10UL)),
      expected);
}

TEST(Lector, WrapOnlyWhitespaceOnly) {
  EXPECT_EQ(lector::wrap("", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap(" ", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap("  ", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap("\t", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap("\t\t", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap("\n", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap("\n\n", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
  EXPECT_EQ(lector::wrap(" \t\n", static_cast<std::size_t>(100UL)), std::vector<std::string>{});
}

}  // namespace
