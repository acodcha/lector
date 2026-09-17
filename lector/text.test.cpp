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

TEST(Lector, CollateAndAlignCentreLeftBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_align_centre_left("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_left("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_left("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_left("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndAlignCentreLeftBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_align_centre_left("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_left("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_left("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_left("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndAlignCentreLeftFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_align_centre_left("", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "             the second\n"
            "              column.");
}

TEST(Lector, CollateAndAlignCentreLeftFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_align_centre_left(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "       This is the first column.          And this is\n"
            "                                           the second\n"
            "                                            column.");
}

TEST(Lector, CollateAndAlignCentreLeftFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_align_centre_left("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "  Hello.    And this is\n"
            "             the second\n"
            "              column.");
}

TEST(Lector, CollateAndAlignCentreLeftFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_align_centre_left("    ", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "             the second\n"
            "              column.");
}

TEST(Lector, CollateAndAlignCentreLeftTypical) {
  EXPECT_EQ(lector::collate_and_align_centre_left(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            "This is   And this is\n"
            "  the      the second\n"
            " first      column.\n"
            "column.");
}

TEST(Lector, CollateAndAlignCentreLeftVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_align_centre_left("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-  And this is\n"
      " g_word.    the second\n"
      "             column.");
}

TEST(Lector, CollateAndAlignCentreRightBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_align_centre_right("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_right("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_right("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_right("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndAlignCentreRightBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_align_centre_right("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_right("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_right("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_centre_right("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndAlignCentreRightFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_align_centre_right("", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "             the second\n"
            "               column.");
}

TEST(Lector, CollateAndAlignCentreRightFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_align_centre_right(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "        This is the first column.          And this is\n"
            "                                           the second\n"
            "                                             column.");
}

TEST(Lector, CollateAndAlignCentreRightFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_align_centre_right("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "  Hello.     And this is\n"
            "             the second\n"
            "               column.");
}

TEST(Lector, CollateAndAlignCentreRightFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_align_centre_right("    ", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "             the second\n"
            "               column.");
}

TEST(Lector, CollateAndAlignCentreRightTypical) {
  EXPECT_EQ(lector::collate_and_align_centre_right(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            " This is   And this is\n"
            "   the     the second\n"
            "  first      column.\n"
            " column.");
}

TEST(Lector, CollateAndAlignCentreRightVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_align_centre_right("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-   And this is\n"
      " g_word.    the second\n"
      "              column.");
}

TEST(Lector, CollateAndAlignLeftBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_align_left("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_left("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_left("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_left("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndAlignLeftBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_align_left("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_left("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_left("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_left("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndAlignLeftFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_align_left("", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CollateAndAlignLeftFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_align_left(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "This is the first column.                 And this is\n"
            "                                          the second\n"
            "                                          column.");
}

TEST(Lector, CollateAndAlignLeftFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_align_left("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "Hello.      And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CollateAndAlignLeftFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_align_left("    ", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CollateAndAlignLeftTypical) {
  EXPECT_EQ(lector::collate_and_align_left(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            "This is   And this is\n"
            "the       the second\n"
            "first     column.\n"
            "column.");
}

TEST(Lector, CollateAndAlignLeftVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_align_left("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-  And this is\n"
      "g_word.    the second\n"
      "           column.");
}

TEST(Lector, CollateAndAlignRightBothColumnsEmpty) {
  EXPECT_EQ(lector::collate_and_align_right("", 1, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_right("", 1, "", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_right("", 2, "", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_right("", 2, "", 2), std::string{});
}

TEST(Lector, CollateAndAlignRightBothColumnsWhitespace) {
  EXPECT_EQ(lector::collate_and_align_right("  ", 1, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_right("  ", 1, "  ", 2), std::string{});
  EXPECT_EQ(lector::collate_and_align_right("  ", 2, "  ", 1), std::string{});
  EXPECT_EQ(lector::collate_and_align_right("  ", 2, "  ", 2), std::string{});
}

TEST(Lector, CollateAndAlignRightFirstColumnEmpty) {
  EXPECT_EQ(lector::collate_and_align_right("", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "              the second\n"
            "                 column.");
}

TEST(Lector, CollateAndAlignRightFirstColumnLargeWidth) {
  EXPECT_EQ(lector::collate_and_align_right(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "               This is the first column.   And this is\n"
            "                                            the second\n"
            "                                               column.");
}

TEST(Lector, CollateAndAlignRightFirstColumnShort) {
  EXPECT_EQ(lector::collate_and_align_right("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "    Hello.   And this is\n"
            "              the second\n"
            "                 column.");
}

TEST(Lector, CollateAndAlignRightFirstColumnWhitespace) {
  EXPECT_EQ(lector::collate_and_align_right("    ", 10, "  And this is the second column.  ", 12),
            "             And this is\n"
            "              the second\n"
            "                 column.");
}

TEST(Lector, CollateAndAlignRightTypical) {
  EXPECT_EQ(lector::collate_and_align_right(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            " This is   And this is\n"
            "     the    the second\n"
            "   first       column.\n"
            " column.");
}

TEST(Lector, CollateAndAlignRightVeryLongWord) {
  EXPECT_EQ(
      lector::collate_and_align_right("  Very_long_word.  ", 9, "  And this is the second column.  ", 12),
      "Very_lon-   And this is\n"
      "  g_word.    the second\n"
      "                column.");
}

TEST(Lector, ContainsWhitespace) {
  EXPECT_FALSE(lector::contains_whitespace(""));
  EXPECT_TRUE(lector::contains_whitespace(" "));
  EXPECT_TRUE(lector::contains_whitespace("\t"));
  EXPECT_TRUE(lector::contains_whitespace("\n"));
  EXPECT_TRUE(lector::contains_whitespace("\v"));
  EXPECT_TRUE(lector::contains_whitespace("\f"));
  EXPECT_TRUE(lector::contains_whitespace("\r"));
  EXPECT_FALSE(lector::contains_whitespace("Hello!"));
  EXPECT_TRUE(lector::contains_whitespace("Hello, world!"));
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

TEST(Lector, JoinAndAlignCentreLeftEmpty) {
  EXPECT_EQ(lector::join_and_align_center_left(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndAlignCentreLeftMultiple) {
  EXPECT_EQ(lector::join_and_align_center_left(
                {"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
            "        Hello!  \n     Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndAlignCentreLeftSingle) {
  EXPECT_EQ(lector::join_and_align_center_left({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndAlignCentreLeftUtf8) {
  EXPECT_EQ(lector::join_and_align_center_left({"  Château.  ", "  Épée.  "}),
            "  Château.  \n   Épée.  ");
}

TEST(Lector, JoinAndAlignCentreRightEmpty) {
  EXPECT_EQ(lector::join_and_align_center_right(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndAlignCentreRightMultiple) {
  EXPECT_EQ(lector::join_and_align_center_right(
                {"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
            "         Hello!  \n     Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndAlignCentreRightSingle) {
  EXPECT_EQ(lector::join_and_align_center_right({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndAlignCentreRightUtf8) {
  EXPECT_EQ(lector::join_and_align_center_right({"  Château.  ", "  Épée.  "}),
            "  Château.  \n    Épée.  ");
}

TEST(Lector, JoinAndAlignLeftEmpty) {
  EXPECT_EQ(lector::join_and_align_left(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndAlignLeftMultiple) {
  EXPECT_EQ(
      lector::join_and_align_left({"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
      "  Hello!  \n  Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndAlignLeftSingle) {
  EXPECT_EQ(lector::join_and_align_left({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndAlignLeftUtf8) {
  EXPECT_EQ(lector::join_and_align_left({"  Château.  ", "  Épée.  "}), "  Château.  \n  Épée.  ");
}

TEST(Lector, JoinAndAlignRightEmpty) {
  EXPECT_EQ(lector::join_and_align_right(std::vector<std::string>{}), "");
}

TEST(Lector, JoinAndAlignRightMultiple) {
  EXPECT_EQ(
      lector::join_and_align_right({"  Hello!  ", "  Hello, world!  ", "  Hello again, world!  "}),
      "               Hello!  \n        Hello, world!  \n  Hello again, world!  ");
}

TEST(Lector, JoinAndAlignRightSingle) {
  EXPECT_EQ(lector::join_and_align_right({"  Hello, world!  "}), "  Hello, world!  ");
}

TEST(Lector, JoinAndAlignRightUtf8) {
  EXPECT_EQ(
      lector::join_and_align_right({"  Château.  ", "  Épée.  "}), "  Château.  \n     Épée.  ");
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

TEST(Lector, PadAndAlignCentreLeftEmpty) {
  EXPECT_EQ(lector::pad_and_align_centre_left("", 0), "");
  EXPECT_EQ(lector::pad_and_align_centre_left("", 1), " ");
  EXPECT_EQ(lector::pad_and_align_centre_left("", 2), "  ");
  EXPECT_EQ(lector::pad_and_align_centre_left("", 3), "   ");
}

TEST(Lector, PadAndAlignCentreLeftLengthLong) {
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 14), "Hello, world! ");
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 15), " Hello, world! ");
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 16), " Hello, world!  ");
}

TEST(Lector, PadAndAlignCentreLeftLengthShort) {
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 0), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 1), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 2), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_left("Hello, world!", 3), "Hello, world!");
}

TEST(Lector, PadAndAlignCentreLeftUtf8) {
  EXPECT_EQ(lector::pad_and_align_centre_left("château", 10), " château  ");
  EXPECT_EQ(lector::pad_and_align_centre_left("été", 10), "   été    ");
  EXPECT_EQ(lector::pad_and_align_centre_left("œuf", 10), "   œuf    ");
  EXPECT_EQ(lector::pad_and_align_centre_left("こんにちは", 10), "  こんにちは   ");
}

TEST(Lector, PadAndAlignCentreLeftWhitespace) {
  EXPECT_EQ(lector::pad_and_align_centre_left("  ", 10), "          ");
  EXPECT_EQ(lector::pad_and_align_centre_left("\t", 10), "    \t     ");
  EXPECT_EQ(lector::pad_and_align_centre_left("\n", 10), "    \n     ");
  EXPECT_EQ(lector::pad_and_align_centre_left(" \t\n \t\n", 10), "   \t\n \t\n  ");
}

TEST(Lector, PadAndAlignCentreRightEmpty) {
  EXPECT_EQ(lector::pad_and_align_centre_right("", 0), "");
  EXPECT_EQ(lector::pad_and_align_centre_right("", 1), " ");
  EXPECT_EQ(lector::pad_and_align_centre_right("", 2), "  ");
  EXPECT_EQ(lector::pad_and_align_centre_right("", 3), "   ");
}

TEST(Lector, PadAndAlignCentreRightLengthLong) {
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 14), " Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 15), " Hello, world! ");
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 16), "  Hello, world! ");
}

TEST(Lector, PadAndAlignCentreRightLengthShort) {
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 0), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 1), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 2), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_centre_right("Hello, world!", 3), "Hello, world!");
}

TEST(Lector, PadAndAlignCentreRightUtf8) {
  EXPECT_EQ(lector::pad_and_align_centre_right("château", 10), "  château ");
  EXPECT_EQ(lector::pad_and_align_centre_right("été", 10), "    été   ");
  EXPECT_EQ(lector::pad_and_align_centre_right("œuf", 10), "    œuf   ");
  EXPECT_EQ(lector::pad_and_align_centre_right("こんにちは", 10), "   こんにちは  ");
}

TEST(Lector, PadAndAlignCentreRightWhitespace) {
  EXPECT_EQ(lector::pad_and_align_centre_right("  ", 10), "          ");
  EXPECT_EQ(lector::pad_and_align_centre_right("\t", 10), "     \t    ");
  EXPECT_EQ(lector::pad_and_align_centre_right("\n", 10), "     \n    ");
  EXPECT_EQ(lector::pad_and_align_centre_right(" \t\n \t\n", 10), "   \t\n \t\n  ");
}

TEST(Lector, PadAndAlignLeftEmpty) {
  EXPECT_EQ(lector::pad_and_align_left("", 0), "");
  EXPECT_EQ(lector::pad_and_align_left("", 1), " ");
  EXPECT_EQ(lector::pad_and_align_left("", 2), "  ");
  EXPECT_EQ(lector::pad_and_align_left("", 3), "   ");
}

TEST(Lector, PadAndAlignLeftLengthLong) {
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 14), "Hello, world! ");
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 15), "Hello, world!  ");
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 16), "Hello, world!   ");
}

TEST(Lector, PadAndAlignLeftLengthShort) {
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 0), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 1), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 2), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_left("Hello, world!", 3), "Hello, world!");
}

TEST(Lector, PadAndAlignLeftUtf8) {
  EXPECT_EQ(lector::pad_and_align_left("château", 10), "château   ");
  EXPECT_EQ(lector::pad_and_align_left("été", 10), "été       ");
  EXPECT_EQ(lector::pad_and_align_left("œuf", 10), "œuf       ");
  EXPECT_EQ(lector::pad_and_align_left("こんにちは", 10), "こんにちは     ");
}

TEST(Lector, PadAndAlignLeftWhitespace) {
  EXPECT_EQ(lector::pad_and_align_left("  ", 10), "          ");
  EXPECT_EQ(lector::pad_and_align_left("\t", 10), "\t         ");
  EXPECT_EQ(lector::pad_and_align_left("\n", 10), "\n         ");
  EXPECT_EQ(lector::pad_and_align_left(" \t\n \t\n", 10), " \t\n \t\n    ");
}

TEST(Lector, PadAndAlignRightEmpty) {
  EXPECT_EQ(lector::pad_and_align_right("", 0), "");
  EXPECT_EQ(lector::pad_and_align_right("", 1), " ");
  EXPECT_EQ(lector::pad_and_align_right("", 2), "  ");
  EXPECT_EQ(lector::pad_and_align_right("", 3), "   ");
}

TEST(Lector, PadAndAlignRightLengthLong) {
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 14), " Hello, world!");
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 15), "  Hello, world!");
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 16), "   Hello, world!");
}

TEST(Lector, PadAndAlignRightLengthShort) {
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 0), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 1), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 2), "Hello, world!");
  EXPECT_EQ(lector::pad_and_align_right("Hello, world!", 3), "Hello, world!");
}

TEST(Lector, PadAndAlignRightUtf8) {
  EXPECT_EQ(lector::pad_and_align_right("château", 10), "   château");
  EXPECT_EQ(lector::pad_and_align_right("été", 10), "       été");
  EXPECT_EQ(lector::pad_and_align_right("œuf", 10), "       œuf");
  EXPECT_EQ(lector::pad_and_align_right("こんにちは", 10), "     こんにちは");
}

TEST(Lector, PadAndAlignRightWhitespace) {
  EXPECT_EQ(lector::pad_and_align_right("  ", 10), "          ");
  EXPECT_EQ(lector::pad_and_align_right("\t", 10), "         \t");
  EXPECT_EQ(lector::pad_and_align_right("\n", 10), "         \n");
  EXPECT_EQ(lector::pad_and_align_right(" \t\n \t\n", 10), "     \t\n \t\n");
}

TEST(Lector, QuoteBasicAlreadyContainsQuotes) {
  EXPECT_EQ(lector::quote("Hello, world!'"), "\"Hello, world!'\"");
  EXPECT_EQ(lector::quote("Hello, world!\""), "'Hello, world!\"'");
}

TEST(Lector, QuoteBasicAlreadyEnclosedInQuotes) {
  EXPECT_EQ(lector::quote("'Hello, world!'"), "'Hello, world!'");
  EXPECT_EQ(lector::quote("\"Hello, world!\""), "\"Hello, world!\"");
}

TEST(Lector, QuoteBasicEmpty) {
  EXPECT_EQ(lector::quote(""), "");
}

TEST(Lector, QuoteBasicInvalid) {
  EXPECT_ANY_THROW((void)lector::quote("'\""));
}

TEST(Lector, QuoteBasicPreferDoubleQuotes) {
  EXPECT_EQ(lector::quote("Hello, world!"), "\"Hello, world!\"");
}

TEST(Lector, QuoteIfContainsWhitespace) {
  EXPECT_EQ(lector::quote_if_contains_whitespace(""), "");
  EXPECT_EQ(lector::quote_if_contains_whitespace("Hello!"), "Hello!");
  EXPECT_EQ(lector::quote_if_contains_whitespace("Hello, world!"), "\"Hello, world!\"");
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

TEST(Lector, TruncateFromLeftAscii) {
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 14), "Hello, world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 12), "ello, world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 11), "llo, world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 10), "lo, world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 9), "o, world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 8), ", world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 7), " world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 6), "world!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 5), "orld!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 4), "rld!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 3), "ld!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 2), "d!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 1), "!");
  EXPECT_EQ(lector::truncate_from_left("Hello, world!", 0), "");
}

TEST(Lector, TruncateFromLeftEmpty) {
  EXPECT_EQ(lector::truncate_from_left("", 0), "");
  EXPECT_EQ(lector::truncate_from_left("", 1), "");
  EXPECT_EQ(lector::truncate_from_left("", 2), "");
  EXPECT_EQ(lector::truncate_from_left("", 3), "");
}

TEST(Lector, TruncateFromLeftUtf8) {
  EXPECT_EQ(lector::truncate_from_left("château", 8), "château");
  EXPECT_EQ(lector::truncate_from_left("château", 7), "château");
  EXPECT_EQ(lector::truncate_from_left("château", 6), "hâteau");
  EXPECT_EQ(lector::truncate_from_left("château", 5), "âteau");
  EXPECT_EQ(lector::truncate_from_left("château", 4), "teau");
  EXPECT_EQ(lector::truncate_from_left("château", 3), "eau");
  EXPECT_EQ(lector::truncate_from_left("château", 2), "au");
  EXPECT_EQ(lector::truncate_from_left("château", 1), "u");
  EXPECT_EQ(lector::truncate_from_left("château", 0), "");
  EXPECT_EQ(lector::truncate_from_left("été", 4), "été");
  EXPECT_EQ(lector::truncate_from_left("été", 3), "été");
  EXPECT_EQ(lector::truncate_from_left("été", 2), "té");
  EXPECT_EQ(lector::truncate_from_left("été", 1), "é");
  EXPECT_EQ(lector::truncate_from_left("été", 0), "");
  EXPECT_EQ(lector::truncate_from_left("œuf", 4), "œuf");
  EXPECT_EQ(lector::truncate_from_left("œuf", 3), "œuf");
  EXPECT_EQ(lector::truncate_from_left("œuf", 2), "uf");
  EXPECT_EQ(lector::truncate_from_left("œuf", 1), "f");
  EXPECT_EQ(lector::truncate_from_left("œuf", 0), "");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 6), "こんにちは");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 5), "こんにちは");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 4), "んにちは");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 3), "にちは");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 2), "ちは");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 1), "は");
  EXPECT_EQ(lector::truncate_from_left("こんにちは", 0), "");
}

TEST(Lector, TruncateFromLeftWhitespace) {
  EXPECT_EQ(lector::truncate_from_left("  ", 3), "  ");
  EXPECT_EQ(lector::truncate_from_left("  ", 2), "  ");
  EXPECT_EQ(lector::truncate_from_left("  ", 1), " ");
  EXPECT_EQ(lector::truncate_from_left("  ", 0), "");
  EXPECT_EQ(lector::truncate_from_left("\t\t", 3), "\t\t");
  EXPECT_EQ(lector::truncate_from_left("\t\t", 2), "\t\t");
  EXPECT_EQ(lector::truncate_from_left("\t\t", 1), "\t");
  EXPECT_EQ(lector::truncate_from_left("\t\t", 0), "");
  EXPECT_EQ(lector::truncate_from_left("\n\n", 3), "\n\n");
  EXPECT_EQ(lector::truncate_from_left("\n\n", 2), "\n\n");
  EXPECT_EQ(lector::truncate_from_left("\n\n", 1), "\n");
  EXPECT_EQ(lector::truncate_from_left("\n\n", 0), "");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 7), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 6), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 5), "\t\n \t\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 4), "\n \t\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 3), " \t\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 2), "\t\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 1), "\n");
  EXPECT_EQ(lector::truncate_from_left(" \t\n \t\n", 0), "");
}

TEST(Lector, TruncateFromRightAscii) {
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 14), "Hello, world!");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 12), "Hello, world");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 11), "Hello, worl");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 10), "Hello, wor");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 9), "Hello, wo");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 8), "Hello, w");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 7), "Hello, ");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 6), "Hello,");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 5), "Hello");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 4), "Hell");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 3), "Hel");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 2), "He");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 1), "H");
  EXPECT_EQ(lector::truncate_from_right("Hello, world!", 0), "");
}

TEST(Lector, TruncateFromRightEmpty) {
  EXPECT_EQ(lector::truncate_from_right("", 0), "");
  EXPECT_EQ(lector::truncate_from_right("", 1), "");
  EXPECT_EQ(lector::truncate_from_right("", 2), "");
  EXPECT_EQ(lector::truncate_from_right("", 3), "");
}

TEST(Lector, TruncateFromRightUtf8) {
  EXPECT_EQ(lector::truncate_from_right("château", 8), "château");
  EXPECT_EQ(lector::truncate_from_right("château", 7), "château");
  EXPECT_EQ(lector::truncate_from_right("château", 6), "châtea");
  EXPECT_EQ(lector::truncate_from_right("château", 5), "châte");
  EXPECT_EQ(lector::truncate_from_right("château", 4), "chât");
  EXPECT_EQ(lector::truncate_from_right("château", 3), "châ");
  EXPECT_EQ(lector::truncate_from_right("château", 2), "ch");
  EXPECT_EQ(lector::truncate_from_right("château", 1), "c");
  EXPECT_EQ(lector::truncate_from_right("château", 0), "");
  EXPECT_EQ(lector::truncate_from_right("été", 4), "été");
  EXPECT_EQ(lector::truncate_from_right("été", 3), "été");
  EXPECT_EQ(lector::truncate_from_right("été", 2), "ét");
  EXPECT_EQ(lector::truncate_from_right("été", 1), "é");
  EXPECT_EQ(lector::truncate_from_right("été", 0), "");
  EXPECT_EQ(lector::truncate_from_right("œuf", 4), "œuf");
  EXPECT_EQ(lector::truncate_from_right("œuf", 3), "œuf");
  EXPECT_EQ(lector::truncate_from_right("œuf", 2), "œu");
  EXPECT_EQ(lector::truncate_from_right("œuf", 1), "œ");
  EXPECT_EQ(lector::truncate_from_right("œuf", 0), "");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 6), "こんにちは");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 5), "こんにちは");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 4), "こんにち");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 3), "こんに");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 2), "こん");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 1), "こ");
  EXPECT_EQ(lector::truncate_from_right("こんにちは", 0), "");
}

TEST(Lector, TruncateFromRightWhitespace) {
  EXPECT_EQ(lector::truncate_from_right("  ", 3), "  ");
  EXPECT_EQ(lector::truncate_from_right("  ", 2), "  ");
  EXPECT_EQ(lector::truncate_from_right("  ", 1), " ");
  EXPECT_EQ(lector::truncate_from_right("  ", 0), "");
  EXPECT_EQ(lector::truncate_from_right("\t\t", 3), "\t\t");
  EXPECT_EQ(lector::truncate_from_right("\t\t", 2), "\t\t");
  EXPECT_EQ(lector::truncate_from_right("\t\t", 1), "\t");
  EXPECT_EQ(lector::truncate_from_right("\t\t", 0), "");
  EXPECT_EQ(lector::truncate_from_right("\n\n", 3), "\n\n");
  EXPECT_EQ(lector::truncate_from_right("\n\n", 2), "\n\n");
  EXPECT_EQ(lector::truncate_from_right("\n\n", 1), "\n");
  EXPECT_EQ(lector::truncate_from_right("\n\n", 0), "");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 7), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 6), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 5), " \t\n \t");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 4), " \t\n ");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 3), " \t\n");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 2), " \t");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 1), " ");
  EXPECT_EQ(lector::truncate_from_right(" \t\n \t\n", 0), "");
}

TEST(Lector, TruncateToCentreLeftAscii) {
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 14), "Hello, world!");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 12), "Hello, world");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 11), "ello, world");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 10), "ello, worl");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 9), "llo, worl");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 8), "llo, wor");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 7), "lo, wor");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 6), "lo, wo");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 5), "o, wo");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 4), "o, w");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 3), ", w");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 2), ", ");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 1), " ");
  EXPECT_EQ(lector::truncate_to_centre_left("Hello, world!", 0), "");
}

TEST(Lector, TruncateToCentreLeftEmpty) {
  EXPECT_EQ(lector::truncate_to_centre_left("", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left("", 1), "");
  EXPECT_EQ(lector::truncate_to_centre_left("", 2), "");
  EXPECT_EQ(lector::truncate_to_centre_left("", 3), "");
}

TEST(Lector, TruncateToCentreLeftUtf8) {
  EXPECT_EQ(lector::truncate_to_centre_left("château", 8), "château");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 7), "château");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 6), "châtea");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 5), "hâtea");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 4), "hâte");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 3), "âte");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 2), "ât");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 1), "t");
  EXPECT_EQ(lector::truncate_to_centre_left("château", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left("été", 4), "été");
  EXPECT_EQ(lector::truncate_to_centre_left("été", 3), "été");
  EXPECT_EQ(lector::truncate_to_centre_left("été", 2), "ét");
  EXPECT_EQ(lector::truncate_to_centre_left("été", 1), "t");
  EXPECT_EQ(lector::truncate_to_centre_left("été", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left("œuf", 4), "œuf");
  EXPECT_EQ(lector::truncate_to_centre_left("œuf", 3), "œuf");
  EXPECT_EQ(lector::truncate_to_centre_left("œuf", 2), "œu");
  EXPECT_EQ(lector::truncate_to_centre_left("œuf", 1), "u");
  EXPECT_EQ(lector::truncate_to_centre_left("œuf", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 6), "こんにちは");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 5), "こんにちは");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 4), "こんにち");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 3), "んにち");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 2), "んに");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 1), "に");
  EXPECT_EQ(lector::truncate_to_centre_left("こんにちは", 0), "");
}

TEST(Lector, TruncateToCentreLeftWhitespace) {
  EXPECT_EQ(lector::truncate_to_centre_left("  ", 3), "  ");
  EXPECT_EQ(lector::truncate_to_centre_left("  ", 2), "  ");
  EXPECT_EQ(lector::truncate_to_centre_left("  ", 1), " ");
  EXPECT_EQ(lector::truncate_to_centre_left("  ", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left("\t\t", 3), "\t\t");
  EXPECT_EQ(lector::truncate_to_centre_left("\t\t", 2), "\t\t");
  EXPECT_EQ(lector::truncate_to_centre_left("\t\t", 1), "\t");
  EXPECT_EQ(lector::truncate_to_centre_left("\t\t", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left("\n\n", 3), "\n\n");
  EXPECT_EQ(lector::truncate_to_centre_left("\n\n", 2), "\n\n");
  EXPECT_EQ(lector::truncate_to_centre_left("\n\n", 1), "\n");
  EXPECT_EQ(lector::truncate_to_centre_left("\n\n", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 7), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 6), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 5), " \t\n \t");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 4), "\t\n \t");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 3), "\t\n ");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 2), "\n ");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 1), "\n");
  EXPECT_EQ(lector::truncate_to_centre_left(" \t\n \t\n", 0), "");
}

TEST(Lector, TruncateToCentreRightAscii) {
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 14), "Hello, world!");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 13), "Hello, world!");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 12), "ello, world!");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 11), "ello, world");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 10), "llo, world");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 9), "llo, worl");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 8), "lo, worl");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 7), "lo, wor");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 6), "o, wor");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 5), "o, wo");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 4), ", wo");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 3), ", w");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 2), " w");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 1), " ");
  EXPECT_EQ(lector::truncate_to_centre_right("Hello, world!", 0), "");
}

TEST(Lector, TruncateToCentreRightEmpty) {
  EXPECT_EQ(lector::truncate_to_centre_right("", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right("", 1), "");
  EXPECT_EQ(lector::truncate_to_centre_right("", 2), "");
  EXPECT_EQ(lector::truncate_to_centre_right("", 3), "");
}

TEST(Lector, TruncateToCentreRightUtf8) {
  EXPECT_EQ(lector::truncate_to_centre_right("château", 8), "château");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 7), "château");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 6), "hâteau");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 5), "hâtea");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 4), "âtea");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 3), "âte");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 2), "te");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 1), "t");
  EXPECT_EQ(lector::truncate_to_centre_right("château", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right("été", 4), "été");
  EXPECT_EQ(lector::truncate_to_centre_right("été", 3), "été");
  EXPECT_EQ(lector::truncate_to_centre_right("été", 2), "té");
  EXPECT_EQ(lector::truncate_to_centre_right("été", 1), "t");
  EXPECT_EQ(lector::truncate_to_centre_right("été", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right("œuf", 4), "œuf");
  EXPECT_EQ(lector::truncate_to_centre_right("œuf", 3), "œuf");
  EXPECT_EQ(lector::truncate_to_centre_right("œuf", 2), "uf");
  EXPECT_EQ(lector::truncate_to_centre_right("œuf", 1), "u");
  EXPECT_EQ(lector::truncate_to_centre_right("œuf", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 6), "こんにちは");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 5), "こんにちは");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 4), "んにちは");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 3), "んにち");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 2), "にち");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 1), "に");
  EXPECT_EQ(lector::truncate_to_centre_right("こんにちは", 0), "");
}

TEST(Lector, TruncateToCentreRightWhitespace) {
  EXPECT_EQ(lector::truncate_to_centre_right("  ", 3), "  ");
  EXPECT_EQ(lector::truncate_to_centre_right("  ", 2), "  ");
  EXPECT_EQ(lector::truncate_to_centre_right("  ", 1), " ");
  EXPECT_EQ(lector::truncate_to_centre_right("  ", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right("\t\t", 3), "\t\t");
  EXPECT_EQ(lector::truncate_to_centre_right("\t\t", 2), "\t\t");
  EXPECT_EQ(lector::truncate_to_centre_right("\t\t", 1), "\t");
  EXPECT_EQ(lector::truncate_to_centre_right("\t\t", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right("\n\n", 3), "\n\n");
  EXPECT_EQ(lector::truncate_to_centre_right("\n\n", 2), "\n\n");
  EXPECT_EQ(lector::truncate_to_centre_right("\n\n", 1), "\n");
  EXPECT_EQ(lector::truncate_to_centre_right("\n\n", 0), "");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 7), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 6), " \t\n \t\n");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 5), "\t\n \t\n");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 4), "\t\n \t");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 3), "\n \t");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 2), "\n ");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 1), " ");
  EXPECT_EQ(lector::truncate_to_centre_right(" \t\n \t\n", 0), "");
}

TEST(Lector, WrapAndAlignCentreLeftExcessiveWhitespace) {
  EXPECT_EQ(lector::wrap_and_align_centre_left("Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left(" Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("  Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left(" Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("  Hello, world!  ", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("Hello, world!\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("Hello, world!\n\n", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\n\nHello, world!", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("Hello, world!\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("Hello, world!\t\t", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left("\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\t\tHello, world!", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_left(
                " \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
}

TEST(Lector, WrapAndAlignCentreLeftInvalid) {
  EXPECT_ANY_THROW(
      (void)lector::wrap_and_align_centre_left("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapAndAlignCentreLeftLineLengthFive) {
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                         static_cast<std::size_t>(5UL)),
      " The\nquick\nbrown\n fox\njumps\nover\n the\nlazy\ndog.");
}

TEST(Lector, WrapAndAlignCentreLeftLineLengthFour) {
  EXPECT_EQ(lector::wrap_and_align_centre_left("  Hello, world!  ", static_cast<std::size_t>(4UL)),
            "Hel-\nlo,\nwor-\nld!");
}

TEST(Lector, WrapAndAlignCentreLeftLineLengthOne) {
  EXPECT_EQ(lector::wrap_and_align_centre_left("  Hello, world!  ", static_cast<std::size_t>(1UL)),
            "H\ne\nl\nl\no\n,\nw\no\nr\nl\nd\n!");
}

TEST(Lector, WrapAndAlignCentreLeftLineLengthThree) {
  EXPECT_EQ(lector::wrap_and_align_centre_left("  Hello, world!  ", static_cast<std::size_t>(3UL)),
            "He-\nll-\no,\nwo-\nrl-\nd!");
}

TEST(Lector, WrapAndAlignCentreLeftLineLengthTwo) {
  EXPECT_EQ(lector::wrap_and_align_centre_left("  Hello, world!  ", static_cast<std::size_t>(2UL)),
            "H-\ne-\nl-\nl-\no,\nw-\no-\nr-\nl-\nd!");
}

TEST(Lector, WrapAndAlignCentreLeftMultipleLines) {
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                         static_cast<std::size_t>(12UL)),
      "The quick\nbrown fox\njumps over\n the lazy\n   dog.");
}

TEST(Lector, WrapAndAlignCentreLeftUtf8Characters) {
  EXPECT_EQ(lector::wrap_and_align_centre_left(
                "  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)),
            "J'ai hâte à\n  l'été!");
}

TEST(Lector, WrapAndAlignCentreLeftVeryLongWord) {
  EXPECT_EQ(
      lector::wrap_and_align_centre_left(
          "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
          static_cast<std::size_t>(10UL)),
      " The word\nsupercali-\nfragilist-\nicexpiali-\ndocious is\n    my\n favorite\n  word!");
}

TEST(Lector, WrapAndAlignCentreLeftWhitespaceOnly) {
  EXPECT_EQ(lector::wrap_and_align_centre_left("", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left(" ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("  ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\t\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left("\n\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_left(" \t\n", static_cast<std::size_t>(100UL)), std::string{});
}

TEST(Lector, WrapAndAlignCentreRightExcessiveWhitespace) {
  EXPECT_EQ(lector::wrap_and_align_centre_right("Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right(" Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("  Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right(" Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("  Hello, world!  ", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("Hello, world!\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("Hello, world!\n\n", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\n\nHello, world!", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("Hello, world!\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("Hello, world!\t\t", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right("\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\t\tHello, world!", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)),
      "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_centre_right(
                " \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
}

TEST(Lector, WrapAndAlignCentreRightInvalid) {
  EXPECT_ANY_THROW(
      (void)lector::wrap_and_align_centre_right("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapAndAlignCentreRightLineLengthFive) {
  EXPECT_EQ(lector::wrap_and_align_centre_right(
                "  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                static_cast<std::size_t>(5UL)),
            " The\nquick\nbrown\n fox\njumps\n over\n the\n lazy\n dog.");
}

TEST(Lector, WrapAndAlignCentreRightLineLengthFour) {
  EXPECT_EQ(lector::wrap_and_align_centre_right("  Hello, world!  ", static_cast<std::size_t>(4UL)),
            "Hel-\n lo,\nwor-\n ld!");
}

TEST(Lector, WrapAndAlignCentreRightLineLengthOne) {
  EXPECT_EQ(lector::wrap_and_align_centre_right("  Hello, world!  ", static_cast<std::size_t>(1UL)),
            "H\ne\nl\nl\no\n,\nw\no\nr\nl\nd\n!");
}

TEST(Lector, WrapAndAlignCentreRightLineLengthThree) {
  EXPECT_EQ(lector::wrap_and_align_centre_right("  Hello, world!  ", static_cast<std::size_t>(3UL)),
            "He-\nll-\n o,\nwo-\nrl-\n d!");
}

TEST(Lector, WrapAndAlignCentreRightLineLengthTwo) {
  EXPECT_EQ(lector::wrap_and_align_centre_right("  Hello, world!  ", static_cast<std::size_t>(2UL)),
            "H-\ne-\nl-\nl-\no,\nw-\no-\nr-\nl-\nd!");
}

TEST(Lector, WrapAndAlignCentreRightMultipleLines) {
  EXPECT_EQ(lector::wrap_and_align_centre_right(
                "  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                static_cast<std::size_t>(12UL)),
            " The quick\n brown fox\njumps over\n the lazy\n   dog.");
}

TEST(Lector, WrapAndAlignCentreRightUtf8Characters) {
  EXPECT_EQ(lector::wrap_and_align_centre_right(
                "  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)),
            "J'ai hâte à\n   l'été!");
}

TEST(Lector, WrapAndAlignCentreRightVeryLongWord) {
  EXPECT_EQ(
      lector::wrap_and_align_centre_right(
          "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
          static_cast<std::size_t>(10UL)),
      " The word\nsupercali-\nfragilist-\nicexpiali-\ndocious is\n    my\n favorite\n   word!");
}

TEST(Lector, WrapAndAlignCentreRightWhitespaceOnly) {
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right(" ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("  ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\t\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right("\n\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(
      lector::wrap_and_align_centre_right(" \t\n", static_cast<std::size_t>(100UL)), std::string{});
}

TEST(Lector, WrapAndAlignLeftExcessiveWhitespace) {
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left(" Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("  Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left(" Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("  Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world!\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("\n\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world!\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("Hello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("\t\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_left(
                " \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
}

TEST(Lector, WrapAndAlignLeftInvalid) {
  EXPECT_ANY_THROW(
      (void)lector::wrap_and_align_left("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapAndAlignLeftLineLengthFive) {
  EXPECT_EQ(lector::wrap_and_align_left("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                        static_cast<std::size_t>(5UL)),
            "The\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog.");
}

TEST(Lector, WrapAndAlignLeftLineLengthFour) {
  EXPECT_EQ(lector::wrap_and_align_left("  Hello, world!  ", static_cast<std::size_t>(4UL)),
            "Hel-\nlo,\nwor-\nld!");
}

TEST(Lector, WrapAndAlignLeftLineLengthOne) {
  EXPECT_EQ(lector::wrap_and_align_left("  Hello, world!  ", static_cast<std::size_t>(1UL)),
            "H\ne\nl\nl\no\n,\nw\no\nr\nl\nd\n!");
}

TEST(Lector, WrapAndAlignLeftLineLengthThree) {
  EXPECT_EQ(lector::wrap_and_align_left("  Hello, world!  ", static_cast<std::size_t>(3UL)),
            "He-\nll-\no,\nwo-\nrl-\nd!");
}

TEST(Lector, WrapAndAlignLeftLineLengthTwo) {
  EXPECT_EQ(lector::wrap_and_align_left("  Hello, world!  ", static_cast<std::size_t>(2UL)),
            "H-\ne-\nl-\nl-\no,\nw-\no-\nr-\nl-\nd!");
}

TEST(Lector, WrapAndAlignLeftMultipleLines) {
  EXPECT_EQ(lector::wrap_and_align_left("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                        static_cast<std::size_t>(12UL)),
            "The quick\nbrown fox\njumps over\nthe lazy\ndog.");
}

TEST(Lector, WrapAndAlignLeftUtf8Characters) {
  EXPECT_EQ(
      lector::wrap_and_align_left("  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)),
      "J'ai hâte à\nl'été!");
}

TEST(Lector, WrapAndAlignLeftVeryLongWord) {
  EXPECT_EQ(lector::wrap_and_align_left(
                "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                static_cast<std::size_t>(10UL)),
            "The word\nsupercali-\nfragilist-\nicexpiali-\ndocious is\nmy\nfavorite\nword!");
}

TEST(Lector, WrapAndAlignLeftWhitespaceOnly) {
  EXPECT_EQ(lector::wrap_and_align_left("", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left(" ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left("  ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left("\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left("\t\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left("\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left("\n\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_left(" \t\n", static_cast<std::size_t>(100UL)), std::string{});
}

TEST(Lector, WrapAndAlignRightExcessiveWhitespace) {
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right(" Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("  Hello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right(" Hello, world! ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("  Hello, world!  ", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world!\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("\n\nHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("\n\nHello, world!\n\n", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world!\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("Hello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("\t\tHello, world!", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right("\t\tHello, world!\t\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
  EXPECT_EQ(lector::wrap_and_align_right(
                " \n\t \n\tHello, world! \n\t \n\t", static_cast<std::size_t>(100UL)),
            "Hello, world!");
}

TEST(Lector, WrapAndAlignRightInvalid) {
  EXPECT_ANY_THROW(
      (void)lector::wrap_and_align_right("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapAndAlignRightLineLengthFive) {
  EXPECT_EQ(lector::wrap_and_align_right("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                         static_cast<std::size_t>(5UL)),
            "  The\nquick\nbrown\n  fox\njumps\n over\n  the\n lazy\n dog.");
}

TEST(Lector, WrapAndAlignRightLineLengthFour) {
  EXPECT_EQ(lector::wrap_and_align_right("  Hello, world!  ", static_cast<std::size_t>(4UL)),
            "Hel-\n lo,\nwor-\n ld!");
}

TEST(Lector, WrapAndAlignRightLineLengthOne) {
  EXPECT_EQ(lector::wrap_and_align_right("  Hello, world!  ", static_cast<std::size_t>(1UL)),
            "H\ne\nl\nl\no\n,\nw\no\nr\nl\nd\n!");
}

TEST(Lector, WrapAndAlignRightLineLengthThree) {
  EXPECT_EQ(lector::wrap_and_align_right("  Hello, world!  ", static_cast<std::size_t>(3UL)),
            "He-\nll-\n o,\nwo-\nrl-\n d!");
}

TEST(Lector, WrapAndAlignRightLineLengthTwo) {
  EXPECT_EQ(lector::wrap_and_align_right("  Hello, world!  ", static_cast<std::size_t>(2UL)),
            "H-\ne-\nl-\nl-\no,\nw-\no-\nr-\nl-\nd!");
}

TEST(Lector, WrapAndAlignRightMultipleLines) {
  EXPECT_EQ(lector::wrap_and_align_right("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                         static_cast<std::size_t>(12UL)),
            " The quick\n brown fox\njumps over\n  the lazy\n      dog.");
}

TEST(Lector, WrapAndAlignRightUtf8Characters) {
  EXPECT_EQ(
      lector::wrap_and_align_right("  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)),
      "J'ai hâte à\n     l'été!");
}

TEST(Lector, WrapAndAlignRightVeryLongWord) {
  EXPECT_EQ(lector::wrap_and_align_right(
                "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                static_cast<std::size_t>(10UL)),
            "  The word\nsupercali-\nfragilist-\nicexpiali-\ndocious is\n        my\n  favorite\n  "
            "   word!");
}

TEST(Lector, WrapAndAlignRightWhitespaceOnly) {
  EXPECT_EQ(lector::wrap_and_align_right("", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right(" ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right("  ", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right("\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right("\t\t", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right("\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right("\n\n", static_cast<std::size_t>(100UL)), std::string{});
  EXPECT_EQ(lector::wrap_and_align_right(" \t\n", static_cast<std::size_t>(100UL)), std::string{});
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
