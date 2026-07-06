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
#include <string>
#include <string_view>
#include <vector>

/// @brief The Lector library's namespace.
namespace lector {

/// @brief Counts and returns the number of UTF-8 code points in a string of text. The number of
/// UTF-8 code points is a useful approximation of the number of graphemes in the string.
/// @param[in] text The string of text whose UTF-8 code points are to be counted.
/// @return The number of UTF-8 code points in the string of text.
[[nodiscard]] inline ::std::size_t code_points(const ::std::string_view text) {
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
  ::std::size_t count{0UL};
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
[[nodiscard]] inline ::std::string join_and_left_align(const ::std::vector<::std::string>& lines) {
  // Handle the empty case immediately to prevent underflow later.
  if (lines.empty()) {
    return ::std::string{};
  }
  // Calculate the exact required capacity.
  ::std::size_t total_length{0UL};
  for (const ::std::string& line : lines) {
    total_length += line.length();
  }
  // Add space for the newline separators (one less than the total number of lines).
  total_length += lines.size() - static_cast<::std::size_t>(1UL);
  // Create and allocate the result.
  ::std::string result;
  result.reserve(total_length);
  // Append the first line.
  result.append(lines.front());
  // Append subsequent lines prefixed by a newline.
  for (::std::size_t line_index{1UL}; line_index < lines.size(); ++line_index) {
    result.push_back('\n');
    result.append(lines.at(line_index));
  }
  return result;
}

/// @brief Computes and returns the length of the longest word in a string of text. The length of a
/// word is measured by its number of UTF-8 code points.
/// @param[in] text The string of text whose longest word length is to be computed.
/// @return The length of the longest word in the string of text.
[[nodiscard]] inline ::std::size_t longest_word_length(const ::std::string_view text) {
  ::std::size_t longest_word_length{0UL};
  ::std::size_t index{0UL};
  while (index < text.length()) {
    // Skip over any whitespaces.
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) != 0) {
      ++index;
    }
    // Return if the end of the string has been reached after skipping whitespaces.
    if (index >= text.length()) {
      break;
    }
    // The index now points to the start of the current word.
    const ::std::size_t current_word_start{index};
    // Find the end of the current word.
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) == 0) {
      ++index;
    }
    // Obtain the current word.
    const ::std::string_view current_word{
      text.substr(current_word_start, index - current_word_start)};
    // Compute the length of the current word.
    const ::std::size_t current_word_length{::lector::code_points(current_word)};
    // Update the longest word length.
    longest_word_length = ::std::max(longest_word_length, current_word_length);
  }
  return longest_word_length;
}

/// @brief Wraps a string of text to a line length and returns the result as a vector of strings
/// that contains one string per line.
/// @param[in] text The string of text to wrap.
/// @param[in] line_length The desired line length to use when wrapping. The actual line length may
/// be longer if the string of text contains very long words whose lengths exceed the desired line
/// length.
/// @return The resulting vector of strings that contains one string per line.
[[nodiscard]] inline ::std::vector<::std::string> wrap(
    const ::std::string_view text, const ::std::size_t line_length) {
  // Adjust the specified maximum line length if needed, increasing it to the longest word length if
  // it is greater than the specified maximum line length. A specified maximum line length of 0
  // indicates that the line length is unlimited.
  const ::std::size_t maximum_line_length{
    line_length == static_cast<::std::size_t>(0UL) ?
        ::std::string::npos :
        ::std::max(line_length, ::lector::longest_word_length(text))};
  // Pre-allocate memory to avoid multiple reallocations. Assume an average word and space
  // distribution.
  ::std::vector<::std::string> result;
  result.reserve((text.length() / maximum_line_length) + static_cast<::std::size_t>(1UL));
  // Iterate through the string.
  ::std::size_t current_line_length{0UL};
  ::std::size_t index{0UL};
  bool is_first_word{true};
  while (index < text.length()) {
    // Skip over any whitespaces. This effectively treats consecutive spaces, tabs, and newlines as
    // one delimiter. Cast to an unsigned character to avoid undefined behavior with negative
    // character values in std::isspace.
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) != 0) {
      ++index;
    }
    // Return if the end of the string has been reached after skipping whitespaces.
    if (index >= text.length()) {
      break;
    }
    // The index now points to the start of the current word.
    const ::std::size_t word_start_index{index};
    // Find the end of the current word.
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) == 0) {
      ++index;
    }
    // Obtain the current word.
    const ::std::string_view current_word{text.substr(word_start_index, index - word_start_index)};
    const ::std::size_t current_word_length{::lector::code_points(current_word)};
    // Place the word in the result vector.
    if (is_first_word) {
      // In this case, this is the first word in the result string, so it can be safely added
      // without a leading space.
      result.emplace_back(current_word);
      current_line_length = current_word_length;
      is_first_word = false;
    } else {
      // In this case, this is not the first word in the result string, so a leading space must be
      // added. Check if adding a space plus the word would exceed the line limit.
      if (current_line_length + 1 + current_word_length <= maximum_line_length) {
        // In this case, the word can be safely added to the current line. Access the current line
        // via result.back() and append to it.
        result.back().push_back(' ');
        result.back().append(current_word);
        current_line_length += static_cast<::std::size_t>(1UL) + current_word_length;
      } else {
        // In this case, the word doesn't fit on this line, so push a new line into the vector.
        result.emplace_back(current_word);
        current_line_length = current_word_length;
      }
    }
  }
  return result;
}

/// @brief Left-aligns and wraps a string of text to a line length.
/// @param[in] text The string of text to left-align and wrap.
/// @param[in] line_length The desired line length to use when wrapping. The actual line length may
/// be longer if the string of text contains very long words whose lengths exceed the desired line
/// length.
/// @return The resulting left-aligned and wrapped text.
[[nodiscard]] inline ::std::string wrap_and_left_align(
    const ::std::string_view text, const ::std::size_t line_length) {
  // Adjust the specified maximum line length if needed, increasing it to the longest word length if
  // it is greater than the specified maximum line length. A specified maximum line length of 0
  // indicates that the line length is unlimited.
  const ::std::size_t maximum_line_length{
    line_length == static_cast<::std::size_t>(0UL) ?
        ::std::string::npos :
        ::std::max(line_length, ::lector::longest_word_length(text))};
  // Pre-allocate memory to avoid multiple reallocations. Assume an average word and space
  // distribution.
  ::std::string result;
  result.reserve(
      text.length() + (text.length() / maximum_line_length) + static_cast<::std::size_t>(1UL));
  // Iterate through the string.
  ::std::size_t current_line_length{0UL};
  ::std::size_t index{0UL};
  bool is_first_word{true};
  while (index < text.length()) {
    // Skip over any whitespaces. This effectively treats consecutive spaces, tabs, and newlines as
    // one delimiter. Cast to an unsigned character to avoid undefined behavior with negative
    // character values in std::isspace.
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) != 0) {
      ++index;
    }
    // Return if the end of the string has been reached after skipping whitespaces.
    if (index >= text.length()) {
      break;
    }
    // The index now points to the start of the current word.
    const ::std::size_t word_start_index{index};
    // Find the end of the current word.
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) == 0) {
      ++index;
    }
    // Obtain the current word.
    const ::std::string_view current_word{text.substr(word_start_index, index - word_start_index)};
    const ::std::size_t current_word_length{::lector::code_points(current_word)};
    // Place the word in the result string.
    if (is_first_word) {
      // In this case, this is the first word in the result string, so it can be safely added
      // without a leading space.
      result.append(current_word);
      current_line_length = current_word_length;
      is_first_word = false;
    } else {
      // In this case, this is not the first word in the result string, so a leading space must be
      // added. Check if adding a space plus the word would exceed the line limit.
      if (current_line_length + 1 + current_word_length <= maximum_line_length) {
        // In this case, the word can be safely added to the current line.
        result.push_back(' ');
        result.append(current_word);
        current_line_length += static_cast<::std::size_t>(1UL) + current_word_length;
      } else {
        // In this case, the word doesn't fit on this line, so drop down to the next line.
        result.push_back('\n');
        result.append(current_word);
        current_line_length = current_word_length;
      }
    }
  }
  return result;
}

/// @brief Combines two strings of text, each representing a column, into a single vector of strings
/// that contains one line per string of text, with the lines formatted such that the two columns
/// are left-aligned and spaced a short distance apart.
/// @param[in] first_column_text The string of text for the first column.
/// @param[in] first_column_width The desired width of the first column. The actual width may be
/// larger if the string of text contains very long words whose lengths exceed the desired column
/// width.
/// @param[in] second_column_text The string of text for the second column.
/// @param[in] second_column_width The desired width of the second column. The actual width may be
/// larger if the string of text contains very long words whose lengths exceed the desired column
/// width.
/// @return The vector of strings that contains the combined text.
[[nodiscard]] inline ::std::string combine_and_left_align(
    const ::std::string_view first_column_text, const ::std::size_t first_column_width,
    const ::std::string_view second_column_text, const ::std::size_t second_column_width) {
  // Use a minimum gutter width of two spaces.
  constexpr ::std::size_t gutter_width{2UL};
  // Wrap and split both columns.
  const ::std::vector<::std::string> first_column{
    ::lector::wrap(first_column_text, first_column_width)};
  const ::std::vector<::std::string> second_column{
    ::lector::wrap(second_column_text, second_column_width)};
  // Find the length of the longest line in the first column.
  ::std::size_t first_column_longest_line_length{0UL};
  for (const ::std::string& line : first_column) {
    first_column_longest_line_length =
        ::std::max(first_column_longest_line_length, ::lector::code_points(line));
  }
  // Determine the actual width of the first column. This automatically handles the case where the
  // desired first column width is zero because in this case, the first column longest line length
  // is necessarily greater than or equal to zero.
  const ::std::size_t first_column_actual_width{
    ::std::max(first_column_width, first_column_longest_line_length)};
  // Determine the total number of rows required.
  const ::std::size_t rows{::std::max(first_column.size(), second_column.size())};
  // Pre-allocate memory for the result. A safe and highly efficient upper bound is the byte size of
  // both original input strings, plus the maximum possible padding spaces and newlines per row.
  ::std::string result;
  result.reserve(
      first_column_text.length() + second_column_text.length()
      + (rows * (first_column_actual_width + gutter_width + static_cast<::std::size_t>(1UL))));
  // Combine the rows line by line.
  for (::std::size_t row_index{0UL}; row_index < rows; ++row_index) {
    // Append a newline character for every row after the first to separate them without leaving a
    // trailing newline at the very end of the string.
    if (row_index > static_cast<::std::size_t>(0UL)) {
      result.push_back('\n');
    }
    // Grab the string for column 1 if it exists on this row; otherwise, use an empty string.
    const ::std::string_view first_cell{
      row_index < first_column.size() ? ::std::string_view{first_column.at(row_index)} :
                                        ::std::string_view{}};
    result.append(first_cell);
    // If column 2 has text on this row, pad column 1 and append column 2. Otherwise, if column 2 is
    // exhausted, skip this padding to avoid unnecessary trailing whitespace.
    if (row_index < second_column.size()) {
      const ::std::size_t first_cell_length{::lector::code_points(first_cell)};
      const ::std::size_t padding{first_column_actual_width - first_cell_length + gutter_width};
      result.append(padding, ' ');
      result.append(second_column.at(row_index));
    }
  }
  return result;
}

}  // namespace lector

#endif  // LECTOR_TEXT_HPP
