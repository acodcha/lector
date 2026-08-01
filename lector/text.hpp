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

#ifndef LECTOR_TEXT_HPP
#define LECTOR_TEXT_HPP

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

/// @brief The Lector library's namespace.
namespace lector {

/// @brief Counts and returns the number of UTF-8 code points in a string of text. The number of
/// UTF-8 code points is a useful approximation of the number of graphemes in the string.
/// @param[in] text The string of text whose UTF-8 code points are to be counted.
/// @return The number of UTF-8 code points in the string of text.
[[nodiscard]] inline std::size_t code_points(const std::string_view text) {
  // This function uses an optimized trick for counting UTF-8 code points. It works because of how
  // UTF-8 is designed at the binary level:
  // - The ASCII characters all use 1 byte and all start with 0. They are of the form 0xxxxxxx.
  // - The first byte of a UTF-8 multi-byte character is the lead byte. Lead bytes all start with
  //   the binary patterns 110xxxxx, 1110xxxx, or 11110xxx; these binary patterns indicate that the
  //   UTF-8 character is two, three, or four bytes long, respectively.
  // - The second, third, and fourth bytes of a UTF-8 multi-byte character are continuation bytes.
  //   These bytes always start with the binary pattern 10xxxxxx, which ranges from 0x80 to 0xBF in
  //   hexadecimal notation.
  // Therefore, instead of trying to parse the lead byte and then predicting the number of bytes in
  // the character, this function simply iterates through the string and ignores any byte that
  // starts with the binary pattern 10xxxxxx, which indicates a continuation byte, leaving only the
  // lead bytesto be counted. By only counting the lead bytes, this function perfectly counts the
  // number of valid code points, treating ASCII characters and UTF-8 multi-byte characters as
  // exactly one unit of length, which is a good approximation of the number of graphemes in the
  // string.
  std::size_t count{0UL};
  for (const char character : text) {
    // Cast to an unsigned character to avoid undefined behavior with bitwise operations on signed
    // characters. 0xC0 is 11000000 in binary. 0x80 is 10000000 in binary.
    if ((static_cast<unsigned char>(character) & 0xC0) != 0x80) {
      ++count;
    }
  }
  return count;
}

/// @brief Joins a vector of strings where each string corresponds to a line of text into a single
/// string of text, with newline characters inserted between the lines, and the lines left-aligned.
/// @param[in] lines Vector of strings to be joined and left-aligned.
/// @return The joined and left-aligned string of text.
[[nodiscard]] inline std::string join_and_left_align(const std::vector<std::string>& lines) {
  // Handle the empty case immediately to prevent underflow later.
  if (lines.empty()) {
    return std::string{};
  }
  // Calculate the exact required capacity.
  std::size_t total_length{0UL};
  for (const std::string& line : lines) {
    total_length += line.length();
  }
  // Add space for the newline separators (one less than the total number of lines).
  total_length += lines.size() - static_cast<std::size_t>(1UL);
  // Create and allocate the result.
  std::string result;
  result.reserve(total_length);
  // Append the first line.
  result.append(lines.front());
  // Append subsequent lines prefixed by a newline.
  for (std::size_t line_index{1UL}; line_index < lines.size(); ++line_index) {
    result.push_back('\n');
    result.append(lines.at(line_index));
  }
  return result;
}

/// @brief Computes and returns the length of the longest word in a string of text. The length of a
/// word is measured by its number of UTF-8 code points.
/// @param[in] text The string of text whose longest word length is to be computed.
/// @return The length of the longest word in the string of text.
[[nodiscard]] inline std::size_t longest_word_length(const std::string_view text) {
  std::size_t longest_word_length{0UL};
  std::size_t index{0UL};
  while (index < text.length()) {
    // Skip over any whitespaces.
    while (index < text.length() && std::isspace(static_cast<unsigned char>(text[index])) != 0) {
      ++index;
    }
    // Return if the end of the string has been reached after skipping whitespaces.
    if (index >= text.length()) {
      break;
    }
    // The index now points to the start of the current word.
    const std::size_t current_word_start{index};
    // Find the end of the current word.
    while (index < text.length() && std::isspace(static_cast<unsigned char>(text[index])) == 0) {
      ++index;
    }
    // Obtain the current word.
    const std::string_view current_word{
      text.substr(current_word_start, index - current_word_start)};
    // Compute the length of the current word.
    const std::size_t current_word_length{lector::code_points(current_word)};
    // Update the longest word length.
    longest_word_length = std::max(longest_word_length, current_word_length);
  }
  return longest_word_length;
}

/// @brief Helper function to process a word at the start of a line. If the word exceeds
/// the line length, it is hyphenated. Appends the chunk (and a hyphen, if applicable) to the
/// result, and updates the remaining word and length variables.
/// @param[in,out] result The string to append the text to.
/// @param[in,out] remaining_word The remaining portion of the word being processed.
/// @param[in,out] remaining_word_length The number of code points remaining in the word.
/// @param[in] line_length The desired line length to use when wrapping.
/// @return The new current line length (0 if a newline was appended, otherwise the length of the
/// word).
[[nodiscard]] inline std::size_t hyphenate(
    std::string& result, std::string_view& remaining_word, std::size_t& remaining_word_length,
    const std::size_t line_length) {
  // Check for an empty word or a zero line length.
  if (remaining_word.empty() || line_length == static_cast<std::size_t>(0UL)) {
    return static_cast<std::size_t>(0UL);
  }
  // If the word fits within the line length, no hyphen is needed.
  if (remaining_word_length <= line_length) {
    result.append(remaining_word);
    const std::size_t new_line_length{remaining_word_length};
    remaining_word = std::string_view{};
    remaining_word_length = 0UL;
    return new_line_length;
  }
  // The word exceeds the line length, so we must hyphenate it, leaving one character for the
  // hyphen. If the line limit is 1, we split without a hyphen to avoid an infinite loop.
  const std::size_t split_cps{line_length > static_cast<std::size_t>(1UL) ?
                                  line_length - static_cast<std::size_t>(1UL) :
                                  static_cast<std::size_t>(1UL)};
  // Find the correct byte boundary for the UTF-8 code points.
  std::size_t byte_index{0UL};
  for (std::size_t index{0UL}; index < split_cps && byte_index < remaining_word.length(); ++index) {
    const unsigned char character{static_cast<unsigned char>(remaining_word[byte_index])};
    std::size_t code_point_size{1UL};
    if ((character & 0x80) == 0) {
      code_point_size = 1UL;
    } else if ((character & 0xE0) == 0xC0) {
      code_point_size = 2UL;
    } else if ((character & 0xF0) == 0xE0) {
      code_point_size = 3UL;
    } else if ((character & 0xF8) == 0xF0) {
      code_point_size = 4UL;
    }
    byte_index += code_point_size;
  }
  // Prevent overshooting in case of a malformed string.
  byte_index = std::min(byte_index, remaining_word.length());
  // Append the chunk, add the hyphen, and drop to the next line.
  result.append(remaining_word.substr(0, byte_index));
  if (line_length > static_cast<std::size_t>(1UL)) {
    result.push_back('-');
  }
  result.push_back('\n');
  // Advance the remaining word segment.
  remaining_word = remaining_word.substr(byte_index);
  remaining_word_length -= split_cps;
  // The line ended with a newline, so the current line length is zero.
  return static_cast<std::size_t>(0UL);
}

/// @brief Left-aligns and wraps a string of text to a line length.
/// @param[in] text The string of text to left-align and wrap.
/// @param[in] line_length The desired line length to use when wrapping. Must be strictly greater
/// than zero. Very long words whose lengths exceed this line length are hyphenated.
/// @return The resulting left-aligned and wrapped text.
/// @throws std::invalid_argument if the desired line length is zero.
[[nodiscard]] inline std::string wrap_and_left_align(
    const std::string_view text, const std::size_t line_length) {
  // Validate the specified maximum line length.
  if (line_length <= static_cast<std::size_t>(0UL)) {
    throw std::invalid_argument("Invalid line length. Must be strictly greater than zero.");
  }
  // Check if the string of text is empty.
  if (text.empty()) {
    return std::string{};
  }
  // Pre-allocate memory to avoid multiple reallocations. Assume an average word and space
  // distribution, and account for potential hyphen and newline additions.
  std::string result;
  const std::size_t estimated_splits{line_length > static_cast<std::size_t>(1UL) ?
                                         line_length - static_cast<std::size_t>(1UL) :
                                         static_cast<std::size_t>(1UL)};
  result.reserve(text.length() + (static_cast<std::size_t>(2UL) * text.length() / estimated_splits)
                 + static_cast<std::size_t>(1UL));
  // Iterate through the string.
  std::size_t current_line_length{0UL};
  std::size_t index{0UL};
  while (index < text.length()) {
    // Skip over any whitespaces. This effectively treats consecutive spaces, tabs, and newlines as
    // one delimiter. Cast to an unsigned character to avoid undefined behavior with negative
    // character values in std::isspace.
    while (index < text.length() && std::isspace(static_cast<unsigned char>(text[index])) != 0) {
      ++index;
    }
    // Return if the end of the string has been reached after skipping whitespaces.
    if (index >= text.length()) {
      break;
    }
    // The index now points to the start of the current word.
    const std::size_t word_start_index{index};
    // Find the end of the current word.
    while (index < text.length() && std::isspace(static_cast<unsigned char>(text[index])) == 0) {
      ++index;
    }
    // Obtain the current word.
    const std::string_view current_word{text.substr(word_start_index, index - word_start_index)};
    const std::size_t current_word_length{lector::code_points(current_word)};
    std::string_view remaining_word{current_word};
    std::size_t remaining_word_length{current_word_length};
    // Process the word in chunks if it requires hyphenation.
    while (!remaining_word.empty()) {
      if (current_line_length == static_cast<std::size_t>(0UL)) {
        // In this case, we are at the start of a line (or the very first word).
        // The hyphenate function inherently checks if it fits, or if it needs to be split.
        current_line_length =
            lector::hyphenate(result, remaining_word, remaining_word_length, line_length);
      } else {
        // In this case, we are not at the start of a line, so a leading space would be required.
        // Check if adding a space plus the remaining word would exceed the line limit.
        if (current_line_length + static_cast<std::size_t>(1UL) + remaining_word_length
            <= line_length) {
          // The word can be safely added to the current line.
          result.push_back(' ');
          result.append(remaining_word);
          current_line_length += static_cast<std::size_t>(1UL) + remaining_word_length;
          remaining_word = std::string_view{};
        } else {
          // The word doesn't fit on this line, so drop down to the next line.
          // The next iteration will pick this up as the start of a line (and evaluate for
          // hyphenation).
          result.push_back('\n');
          current_line_length = 0UL;
        }
      }
    }
  }
  return result;
}

/// @brief Wraps a string of text to a line length and returns the result as a vector of strings
/// that contains one string per line.
/// @param[in] text The string of text to wrap.
/// @param[in] line_length The desired line length to use when wrapping. Must be strictly greater
/// than zero. Very long words whose lengths exceed this line length are hyphenated.
/// @return The resulting vector of strings that contains one string per line.
/// @throws std::invalid_argument if the desired line length is zero.
[[nodiscard]] inline std::vector<std::string> wrap(
    const std::string_view text, const std::size_t line_length) {
  // Validate the specified maximum line length.
  if (line_length <= static_cast<std::size_t>(0UL)) {
    throw std::invalid_argument("Invalid line length. Must be strictly greater than zero.");
  }
  // Check if the string of text is empty.
  if (text.empty()) {
    return std::vector<std::string>{};
  }
  // Generate the wrapped text string with embedded newlines, applying hyphenation rules.
  const std::string wrapped_text{lector::wrap_and_left_align(text, line_length)};

  // Split the wrapped text string line by line into a vector.
  std::vector<std::string> result;
  std::string_view view{wrapped_text};
  std::size_t pos{0UL};
  while ((pos = view.find('\n')) != std::string_view::npos) {
    result.emplace_back(view.substr(0, pos));
    view.remove_prefix(pos + 1UL);
  }
  if (!view.empty()) {
    result.emplace_back(view);
  }
  return result;
}

/// @brief Combines two strings of text, each representing a column, into a single vector of strings
/// that contains one line per string of text, with the lines formatted such that the two columns
/// are left-aligned and spaced a short distance apart.
/// @param[in] first_column_text The string of text for the first column.
/// @param[in] first_column_width The desired width of the first column. Very long words whose
/// length exceeds this width are hyphenated.
/// @param[in] second_column_text The string of text for the second column.
/// @param[in] second_column_width The desired width of the second column. Very long words whose
/// length exceeds this width are hyphenated.
/// @return The vector of strings that contains the combined text.
[[nodiscard]] inline std::string combine_and_left_align(
    const std::string_view first_column_text, const std::size_t first_column_width,
    const std::string_view second_column_text, const std::size_t second_column_width) {
  // Use a minimum gutter width of two spaces.
  constexpr std::size_t gutter_width{2UL};
  // Wrap and split both columns.
  const std::vector<std::string> first_column{lector::wrap(first_column_text, first_column_width)};
  const std::vector<std::string> second_column{
    lector::wrap(second_column_text, second_column_width)};
  // Determine the total number of rows required.
  const std::size_t rows{std::max(first_column.size(), second_column.size())};
  // Pre-allocate memory for the result. A safe and highly efficient upper bound is the byte size of
  // both original input strings, plus the maximum possible padding spaces and newlines per row.
  std::string result;
  result.reserve(first_column_text.length() + second_column_text.length()
                 + (rows * (first_column_width + gutter_width + static_cast<std::size_t>(1UL))));
  // Combine the rows line by line.
  for (std::size_t row_index{0UL}; row_index < rows; ++row_index) {
    // Append a newline character for every row after the first to separate them without leaving a
    // trailing newline at the very end of the string.
    if (row_index > static_cast<std::size_t>(0UL)) {
      result.push_back('\n');
    }
    // Grab the string for column 1 if it exists on this row; otherwise, use an empty string.
    const std::string_view first_cell{
      row_index < first_column.size() ? std::string_view{first_column.at(row_index)} :
                                        std::string_view{}};
    result.append(first_cell);
    // If column 2 has text on this row, pad column 1 and append column 2. Otherwise, if column 2 is
    // exhausted, skip this padding to avoid unnecessary trailing whitespace.
    if (row_index < second_column.size()) {
      const std::size_t first_cell_length{lector::code_points(first_cell)};
      const std::size_t padding{first_column_width + gutter_width - first_cell_length};
      result.append(padding, ' ');
      result.append(second_column.at(row_index));
    }
  }
  return result;
}

}  // namespace lector

#endif  // LECTOR_TEXT_HPP
