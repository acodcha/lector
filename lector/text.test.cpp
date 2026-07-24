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
#include <vector>

namespace {

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

TEST(Lector, CombineAndLeftAlignBothColumnsEmpty) {
  EXPECT_EQ(lector::combine_and_left_align("", 1, "", 1), "");
  EXPECT_EQ(lector::combine_and_left_align("", 1, "", 2), "");
  EXPECT_EQ(lector::combine_and_left_align("", 2, "", 1), "");
  EXPECT_EQ(lector::combine_and_left_align("", 2, "", 2), "");
}

TEST(Lector, CombineAndLeftAlignBothColumnsWhitespace) {
  EXPECT_EQ(lector::combine_and_left_align("  ", 1, "  ", 1), "");
  EXPECT_EQ(lector::combine_and_left_align("  ", 1, "  ", 2), "");
  EXPECT_EQ(lector::combine_and_left_align("  ", 2, "  ", 1), "");
  EXPECT_EQ(lector::combine_and_left_align("  ", 2, "  ", 2), "");
}

TEST(Lector, CombineAndLeftAlignFirstColumnEmpty) {
  EXPECT_EQ(lector::combine_and_left_align("", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CombineAndLeftAlignFirstColumnLargeWidth) {
  EXPECT_EQ(lector::combine_and_left_align(
                "  This is the first column.  ", 40, "  And this is the second column.  ", 12),
            "This is the first column.                 And this is\n"
            "                                          the second\n"
            "                                          column.");
}

TEST(Lector, CombineAndLeftAlignFirstColumnShort) {
  EXPECT_EQ(lector::combine_and_left_align("  Hello.  ", 10, "  And this is the second column.  ", 12),
            "Hello.      And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CombineAndLeftAlignFirstColumnWhitespace) {
  EXPECT_EQ(lector::combine_and_left_align("    ", 10, "  And this is the second column.  ", 12),
            "            And this is\n"
            "            the second\n"
            "            column.");
}

TEST(Lector, CombineAndLeftAlignTypical) {
  EXPECT_EQ(lector::combine_and_left_align(
                "  This is the first column.  ", 8, "  And this is the second column.  ", 12),
            "This is   And this is\n"
            "the       the second\n"
            "first     column.\n"
            "column.");
}

TEST(Lector, CombineAndLeftAlignVeryLongWord) {
  EXPECT_EQ(
      lector::combine_and_left_align("  Very_long_word.  ", 1, "  And this is the second column.  ", 12),
      "Very_long_word.  And this is\n"
      "                 the second\n"
      "                 column.");
}

TEST(Lector, JoinAndLeftAlignEmpty) {
  EXPECT_EQ(lector::join_and_left_align({}), "");
}

TEST(Lector, JoinAndLeftAlignMultiple) {
  EXPECT_EQ(lector::join_and_left_align({"  Hello, ", "world!  "}), "  Hello, \nworld!  ");
}

TEST(Lector, JoinAndLeftAlignSingle) {
  EXPECT_EQ(lector::join_and_left_align({"  Hello, world!  "}), "  Hello, world!  ");
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

TEST(Lector, WrapAndLeftAlignExcessiveWhitespaceEmpty) {
  EXPECT_EQ(lector::wrap_and_left_align("", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align(" ", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align("  ", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align("\t", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align("\t\t", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align("\n", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align("\n\n", static_cast<std::size_t>(100UL)), "");
  EXPECT_EQ(lector::wrap_and_left_align(" \t\n \t\n", static_cast<std::size_t>(100UL)), "");
}

TEST(Lector, WrapAndLeftAlignExcessiveWhitespaceTypical) {
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

TEST(Lector, WrapAndLeftAlignMaximumLineLengthSmall) {
  const std::string expected{"The\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog."};
  EXPECT_EQ(lector::wrap_and_left_align("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                                        static_cast<std::size_t>(1UL)),
            expected);
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
  const std::string expected{"The word\nsupercalifragilisticexpialidocious\nis my favorite word!"};
  EXPECT_EQ(lector::wrap_and_left_align(
                "  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                static_cast<std::size_t>(10UL)),
            expected);
}

TEST(Lector, WrapToVectorExcessiveWhitespaceEmpty) {
  const std::vector<std::string> expected;
  EXPECT_EQ(lector::wrap("", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap(" ", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("  ", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\t", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\t\t", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\n", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap("\n\n", static_cast<std::size_t>(100UL)), expected);
  EXPECT_EQ(lector::wrap(" \t\n \t\n", static_cast<std::size_t>(100UL)), expected);
}

TEST(Lector, WrapToVectorExcessiveWhitespaceTypical) {
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

TEST(Lector, WrapToVectorInvalid) {
  EXPECT_ANY_THROW((void)lector::wrap("Hello, world!", static_cast<std::size_t>(0UL)));
}

TEST(Lector, WrapToVectorMaximumLineLengthSmall) {
  const std::vector<std::string> expected{
    "The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog."};
  EXPECT_EQ(lector::wrap("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                         static_cast<std::size_t>(1UL)),
            expected);
}

TEST(Lector, WrapToVectorMultipleLines) {
  const std::vector<std::string> expected{
    "The quick", "brown fox", "jumps over", "the lazy", "dog."};
  EXPECT_EQ(lector::wrap("  The  quick  brown  fox  jumps  over  the  lazy  dog.  ",
                         static_cast<std::size_t>(12UL)),
            expected);
}

TEST(Lector, WrapToVectorUtf8Characters) {
  const std::vector<std::string> expected{"J'ai hâte à", "l'été!"};
  EXPECT_EQ(lector::wrap("  J'ai  hâte  à  l'été!  ", static_cast<std::size_t>(11UL)), expected);
}

TEST(Lector, WrapToVectorVeryLongWord) {
  const std::vector<std::string> expected{
    "The word", "supercalifragilisticexpialidocious", "is my favorite word!"};
  EXPECT_EQ(
      lector::wrap("  The  word  supercalifragilisticexpialidocious  is  my  favorite  word!  ",
                   static_cast<std::size_t>(10UL)),
      expected);
}

}  // namespace
