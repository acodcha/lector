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
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/// @brief The Lector library's namespace.
namespace lector {

/// @brief Returns whether a given character is the leading byte of a UTF-8 character. All UTF-8
/// characters measure either one, two, three, or four bytes. UTF-8 characters that measure only one
/// byte are the ASCII characters. UTF-8 character that measure two, three, or four bytes are
/// multi-byte characters and consist of a leading byte with a specific binary pattern and one or
/// more continuation bytes of the binary pattern 10xxxxxx.
///
/// 1. One-byte UTF-8 characters are the ASCII characters. Their first bit is 0 and their binary
///    pattern is therefore 0xxxxxxx.
///
/// 2. Two-byte UTF-8 characters have a leading byte with the binary pattern 110xxxxx and one
///    continuation byte with the binary pattern 10xxxxxx. Together, the two bytes therefore have
///    the binary pattern 110xxxxx 10xxxxxx.
///
/// 3. Three-byte UTF-8 characters have a leading byte with the binary pattern 1110xxxx and two
///    continuation bytes with the binary pattern 10xxxxxx. Together, the three bytes therefore have
///    the binary pattern 1110xxxx 10xxxxxx 10xxxxxx.
///
/// 4. Four-byte UTF-8 characters have a leading byte with the binary pattern 11110xxx and three
///    continuation bytes with the binary pattern 10xxxxxx. Together, the four bytes therefore have
///    the binary pattern 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.
/// @param[in] character The character to check.
/// @return True if the character is a leading byte; false if the character is a continuation byte.
[[nodiscard]] inline bool is_leading_byte(const char character) {
  // Cast to an unsigned character to avoid undefined behavior with bitwise operations on signed
  // characters. The binary pattern 10xxxxxx that identifies a continuation byte ranges from 0x80 to
  // 0xBF in hexadecimal notation.
  return (static_cast<unsigned char>(character) & 0xC0) != 0x80;
}

/// @brief Counts and returns the number of UTF-8 code points in a string of text. The number of
/// UTF-8 code points is a useful approximation of the number of graphemes in the string, where
/// ASCII characters and multi-byte UTF-8 characters are each counted as one unit of length.
/// @param[in] text The string of text whose UTF-8 code points are to be counted.
/// @return The number of UTF-8 code points in the string of text.
[[nodiscard]] inline std::size_t code_points(const std::string_view text) {
  std::size_t count{0UL};
  for (const char character : text) {
    if (lector::is_leading_byte(character)) {
      ++count;
    }
  }
  return count;
}

/// @brief Finds the exact byte [begin, end) index interval in a string of text where a specified
/// code point resides.
/// @param[in] text The string of text to parse.
/// @param[in] code_point_index The index of the code point in the string of text.
/// @return A pair that contains the begin and end byte indices of the specified code point. The end
/// index is the classical C++ "one past the end" index. If the specified code point index is out of
/// bounds, both returned indices are set to one past the end index of the string, which is the size
/// of the string.
[[nodiscard]] inline std::pair<std::size_t, std::size_t> byte_interval(
    const std::string_view text, const std::size_t code_point_index) {
  std::size_t current_code_point_index{0UL};
  std::size_t begin_byte_index{text.size()};
  for (std::size_t current_byte_index{0UL}; current_byte_index < text.size();
       ++current_byte_index) {
    if (lector::is_leading_byte(text.at(current_byte_index))) {
      if (current_code_point_index == code_point_index) {
        begin_byte_index = current_byte_index;
      } else if (current_code_point_index == code_point_index + static_cast<std::size_t>(1UL)) {
        // In this case, this is the start of the next code point, and therefore the end of the
        // requested code point.
        return std::pair<std::size_t, std::size_t>{begin_byte_index, current_byte_index};
      }
      ++current_code_point_index;
    }
  }
  if (begin_byte_index < text.size()) {
    // In this case, the requested code point is found, but it is the last code point in the string.
    return std::pair<std::size_t, std::size_t>{begin_byte_index, text.size()};
  }
  // In this case, the requested code point index is out of bounds.
  return std::pair<std::size_t, std::size_t>{text.size(), text.size()};
}

/// @brief Computes and returns the length of the longest word in a string of text. The length of a
/// word is measured by its number of UTF-8 code points.
/// @param[in] text The string of text whose longest word length is to be computed.
/// @return The length of the longest word in the string of text.
[[nodiscard]] inline std::size_t longest_word_length(const std::string_view text) {
  std::size_t current_longest_word_length{0UL};
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
    current_longest_word_length = std::max(current_longest_word_length, current_word_length);
  }
  return current_longest_word_length;
}

/// @brief Tokenizes a string of text into a vector of strings of text, where each string in the
/// vector corresponds to a word in the original string. Words are defined as sequences of
/// non-whitespace characters, and whitespace characters are used as delimiters. The function does
/// not modify the original string and returns views into it, so the original string must remain
/// valid for the lifetime of the returned vector.
/// @param[in] text The string of text to be tokenized.
/// @return A vector of strings of text, each corresponding to a word in the original string.
[[nodiscard]] inline std::vector<std::string_view> tokenize(const std::string_view text) {
  std::vector<std::string_view> words;
  std::size_t begin_index{0UL};
  while (begin_index < text.size()) {
    while (begin_index < text.size()
           && std::isspace(static_cast<unsigned char>(text[begin_index])) != 0) {
      ++begin_index;
    }
    if (begin_index == text.size()) {
      break;
    }
    std::size_t end_index{begin_index};
    while (
        end_index < text.size() && std::isspace(static_cast<unsigned char>(text[end_index])) == 0) {
      ++end_index;
    }
    words.push_back(text.substr(begin_index, end_index - begin_index));
    begin_index = end_index;
  }
  return words;
}

/// @brief Pads a string of text from the left with spaces to reach a specified length. If the
/// string of text is longer than the specified length, it is unchanged.
/// @param[in] text The string of text to pad from the left.
/// @param[in] length The desired length of the padded string.
/// @return The padded string of text.
[[nodiscard]] inline std::string pad_left(const std::string_view text, const std::size_t length) {
  const std::size_t text_length{lector::code_points(text)};
  if (text_length >= length) {
    return std::string(text);
  }
  return std::string(length - text_length, ' ') + std::string{text};
}

/// @brief Pads a string of text from the right with spaces to reach a specified length. If the
/// string of text is longer than the specified length, it is unchanged.
/// @param[in] text The string of text to pad from the right.
/// @param[in] length The desired length of the padded string.
/// @return The padded string of text.
[[nodiscard]] inline std::string pad_right(const std::string_view text, const std::size_t length) {
  const std::size_t text_length{lector::code_points(text)};
  if (text_length >= length) {
    return std::string(text);
  }
  return std::string{text} + std::string(length - text_length, ' ');
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
  // Calculate the exact total size.
  std::size_t total_size{0UL};
  for (const std::string& line : lines) {
    total_size += line.size();
  }
  // Add space for the newline separators (one less than the total number of lines).
  total_size += lines.size() - static_cast<std::size_t>(1UL);
  // Create and allocate the resulting text.
  std::string text;
  text.reserve(total_size);
  // Append the first line.
  text.append(lines.front());
  // Append subsequent lines prefixed by a newline.
  for (std::size_t line_index{1UL}; line_index < lines.size(); ++line_index) {
    text.push_back('\n');
    text.append(lines.at(line_index));
  }
  return text;
}

/// @brief Joins a vector of strings where each string corresponds to a line of text into a single
/// string of text, with newline characters inserted between the lines, and the lines right-aligned.
/// @param[in] lines Vector of strings to be joined and right-aligned.
/// @return The joined and right-aligned string of text.
[[nodiscard]] inline std::string join_and_right_align(const std::vector<std::string>& lines) {
  // Handle the empty case immediately to prevent underflow later.
  if (lines.empty()) {
    return std::string{};
  }
  // Compute the line lengths and find the maximum line length.
  std::vector<std::size_t> line_lengths;
  line_lengths.reserve(lines.size());
  std::size_t longest_line_length{0UL};
  for (const std::string& line : lines) {
    const std::size_t length{lector::code_points(line)};
    line_lengths.push_back(length);
    longest_line_length = std::max(length, longest_line_length);
  }
  // Compute the exact total byte size.
  std::size_t total_size{0UL};
  for (std::size_t line_index{0UL}; line_index < lines.size(); ++line_index) {
    const std::size_t padding{longest_line_length - line_lengths.at(line_index)};
    total_size += lines.at(line_index).size() + padding;
  }
  total_size += lines.size() - static_cast<std::size_t>(1UL);
  // Create and allocate the resulting text.
  std::string text;
  text.reserve(total_size);
  // Append lines with padding.
  for (std::size_t line_index{0UL}; line_index < lines.size(); ++line_index) {
    if (line_index > 0UL) {
      text.push_back('\n');
    }
    const std::size_t padding{longest_line_length - line_lengths.at(line_index)};
    text.append(padding, ' ');
    text.append(lines.at(line_index));
  }
  return text;
}

/// @brief Joins a vector of strings where each string corresponds to a line of text into a single
/// string of text, with newline characters inserted between the lines, and the lines
/// centre-aligned. If the total required centre-aligning padding is odd, the text is biased by one
/// space towards the left.
/// @param[in] lines Vector of strings to be joined and centre-aligned.
/// @return The joined and centre-aligned string of text.
[[nodiscard]] inline std::string join_and_centre_align_with_left_bias(
    const std::vector<std::string>& lines) {
  // Handle the empty case immediately to prevent underflow later.
  if (lines.empty()) {
    return std::string{};
  }
  // Compute the line lengths and find the maximum line length.
  std::vector<std::size_t> line_lengths;
  line_lengths.reserve(lines.size());
  std::size_t longest_line_length{0UL};
  for (const std::string& line : lines) {
    const std::size_t length{lector::code_points(line)};
    line_lengths.push_back(length);
    longest_line_length = std::max(length, longest_line_length);
  }
  // Compute the exact total byte size.
  std::size_t total_size{0UL};
  for (std::size_t line_index{0UL}; line_index < lines.size(); ++line_index) {
    const std::size_t total_padding{longest_line_length - line_lengths.at(line_index)};
    // Bias left. When the total number of padding spaces is odd, integer division rounds down,
    // giving one less padding space to the left.
    const std::size_t left_padding{total_padding / 2UL};
    total_size += lines.at(line_index).size() + left_padding;
  }
  total_size += lines.size() - static_cast<std::size_t>(1UL);
  // Create and allocate the resulting text.
  std::string text;
  text.reserve(total_size);
  // Append lines with padding.
  for (std::size_t line_index{0UL}; line_index < lines.size(); ++line_index) {
    if (line_index > 0UL) {
      text.push_back('\n');
    }
    const std::size_t total_padding{longest_line_length - line_lengths.at(line_index)};
    const std::size_t left_padding{total_padding / 2UL};
    text.append(left_padding, ' ');
    text.append(lines.at(line_index));
  }
  return text;
}

/// @brief Joins a vector of strings where each string corresponds to a line of text into a single
/// string of text, with newline characters inserted between the lines, and the lines
/// centre-aligned. If the total required centre-aligning padding is odd, the text is biased by one
/// space towards the right.
/// @param[in] lines Vector of strings to be joined and centre-aligned.
/// @return The joined and centre-aligned string of text.
[[nodiscard]] inline std::string join_and_centre_align_with_right_bias(
    const std::vector<std::string>& lines) {
  // Handle the empty case immediately to prevent underflow later.
  if (lines.empty()) {
    return std::string{};
  }
  // Compute the line lengths and find the maximum line length.
  std::vector<std::size_t> line_lengths;
  line_lengths.reserve(lines.size());
  std::size_t longest_line_length{0UL};
  for (const std::string& line : lines) {
    const std::size_t length{lector::code_points(line)};
    line_lengths.push_back(length);
    longest_line_length = std::max(length, longest_line_length);
  }
  // Compute the exact total byte size.
  std::size_t total_size{0UL};
  for (std::size_t line_index{0UL}; line_index < lines.size(); ++line_index) {
    const std::size_t total_padding{longest_line_length - line_lengths.at(line_index)};
    // Bias right. When the total number of padding spaces is odd, adding one more space before
    // performing the integer division rounds it up, giving one more padding space to the left.
    const std::size_t left_padding{(total_padding + 1UL) / 2UL};
    total_size += lines.at(line_index).size() + left_padding;
  }
  total_size += lines.size() - static_cast<std::size_t>(1UL);
  // Create and allocate the resulting text.
  std::string text;
  text.reserve(total_size);
  // Append lines with padding.
  for (std::size_t line_index{0UL}; line_index < lines.size(); ++line_index) {
    if (line_index > 0UL) {
      text.push_back('\n');
    }
    const std::size_t total_padding{longest_line_length - line_lengths.at(line_index)};
    const std::size_t left_padding{(total_padding + 1UL) / 2UL};
    text.append(left_padding, ' ');
    text.append(lines.at(line_index));
  }
  return text;
}

/// @brief Wraps a string of text to a line length and returns the result as a sequence of strings
/// of text where each string in the sequence represents one line of text.
/// @param[in] text The string of text to wrap.
/// @param[in] line_length The desired line length to use when wrapping. Must be strictly greater
/// than zero. Very long words whose lengths exceed this line length are hyphenated.
/// @return The resulting sequence of strings of text that contains one string per line.
/// @throws std::invalid_argument if the desired line length is zero.
[[nodiscard]] inline std::vector<std::string> wrap(
    const std::string_view text, const std::size_t line_length) {
  // Ensure the line length is valid.
  if (line_length <= static_cast<std::size_t>(0UL)) {
    throw std::invalid_argument("Invalid line length. Must be strictly greater than zero.");
  }
  // Tokenize the input string of text.
  const std::vector<std::string_view> words{lector::tokenize(text)};
  // Process the tokenized input string of text and assemble the wrapped lines.
  std::vector<std::string> lines;
  std::string current_line;
  std::size_t current_line_code_point_size{0UL};
  for (const std::string_view current_word : words) {
    // Measure the current word.
    const std::size_t current_word_code_point_size{lector::code_points(current_word)};
    const std::size_t space_needed_for_hyphen{
      (current_line_code_point_size > static_cast<std::size_t>(0UL)) ?
          static_cast<std::size_t>(1UL) :
          static_cast<std::size_t>(0UL)};
    // Check if the current word fits on the current line.
    if (current_line_code_point_size > static_cast<std::size_t>(0UL)
        && current_line_code_point_size + current_word_code_point_size + space_needed_for_hyphen
               <= line_length) {
      // In this case, the current word fits on the current line.
      current_line.push_back(' ');
      current_line.append(current_word);
      current_line_code_point_size += current_word_code_point_size + space_needed_for_hyphen;
    } else {
      // In this case, the current word does not fit on the current line and must be wrapped to the
      // next line.
      if (current_line_code_point_size > static_cast<std::size_t>(0UL)) {
        lines.push_back(std::move(current_line));
        current_line.clear();
        current_line_code_point_size = static_cast<std::size_t>(0UL);
      }
      // Check if the current word needs to be hyphenated.
      if (current_word_code_point_size <= line_length) {
        // In this case, the current word fits completely on an empty line and does not need to be
        // hyphenated.
        current_line = current_word;
        current_line_code_point_size = current_word_code_point_size;
      } else {
        // In this case, the current word is too long and must be hyphenated.
        std::string_view remaining_word{current_word};
        std::size_t remaining_code_point_size{current_word_code_point_size};
        // Iterate until the remaining portion of the current word fits on a line, and repeat as
        // necessary; a very long word might need to be hyphenated multiple times.
        while (remaining_code_point_size > line_length) {
          // If the line_length is 1, no hyphen is used. Otherwise, take "line length - 1" code
          // points to save 1 character for the hyphen.
          const std::size_t chunk_code_point_size{line_length == static_cast<std::size_t>(1UL) ?
                                                      static_cast<std::size_t>(1UL) :
                                                      line_length - static_cast<std::size_t>(1UL)};
          const std::size_t split_byte_index{
            lector::byte_interval(remaining_word, chunk_code_point_size).first};
          std::string split_line(
              remaining_word.substr(static_cast<std::size_t>(0UL), split_byte_index));
          if (line_length > static_cast<std::size_t>(1UL)) {
            split_line.push_back('-');
          }
          lines.push_back(std::move(split_line));
          remaining_word = remaining_word.substr(split_byte_index);
          remaining_code_point_size -= chunk_code_point_size;
        }
        // The remaining slice of the word seeds the subsequent line.
        if (remaining_code_point_size > static_cast<std::size_t>(0UL)) {
          current_line = remaining_word;
          current_line_code_point_size = remaining_code_point_size;
        }
      }
    }
  }
  // Push the final built line if it is not empty.
  if (current_line_code_point_size > static_cast<std::size_t>(0UL)) {
    lines.push_back(std::move(current_line));
  }
  // Return the wrapped lines.
  return lines;
}

/// @brief Left-aligns and wraps a string of text to a line length.
/// @param[in] text The string of text to wrap and left-align.
/// @param[in] line_length The desired line length to use when wrapping. Must be strictly greater
/// than zero. Very long words whose lengths exceed this line length are hyphenated.
/// @return The resulting wrapped and left-aligned string of text.
/// @throws std::invalid_argument if the desired line length is zero.
[[nodiscard]] inline std::string wrap_and_left_align(
    const std::string_view text, const std::size_t line_length) {
  return lector::join_and_left_align(lector::wrap(text, line_length));
}

/// @brief Right-aligns and wraps a string of text to a line length.
/// @param[in] text The string of text to wrap and right-align.
/// @param[in] line_length The desired line length to use when wrapping. Must be strictly greater
/// than zero. Very long words whose lengths exceed this line length are hyphenated.
/// @return The resulting wrapped and right-aligned string of text.
/// @throws std::invalid_argument if the desired line length is zero.
[[nodiscard]] inline std::string wrap_and_right_align(
    const std::string_view text, const std::size_t line_length) {
  return lector::join_and_right_align(lector::wrap(text, line_length));
}

/// @brief Collates two strings of text, each representing a column, into a single string that
/// contains newline-separated lines of text, with the lines formatted such that the two columns are
/// left-aligned and spaced a short distance apart.
/// @param[in] first_column_text The string of text for the first column.
/// @param[in] first_column_width The desired width of the first column. Very long words whose
/// length exceeds this width are hyphenated.
/// @param[in] second_column_text The string of text for the second column.
/// @param[in] second_column_width The desired width of the second column. Very long words whose
/// length exceeds this width are hyphenated.
/// @return The string that contains the collated text.
/// @throws std::invalid_argument if either desired column width is zero.
[[nodiscard]] inline std::string collate_and_left_align(
    const std::string_view first_column_text, const std::size_t first_column_width,
    const std::string_view second_column_text, const std::size_t second_column_width) {
  // Use a gutter width of two spaces.
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
  // Collate the rows line by line.
  for (std::size_t row_index{0UL}; row_index < rows; ++row_index) {
    // Append a newline character for every row after the first to separate them without leaving a
    // trailing newline at the very end of the string.
    if (row_index > static_cast<std::size_t>(0UL)) {
      result.push_back('\n');
    }
    // Grab the string for the first column if it exists on this row; otherwise, use an empty
    // string.
    const std::string_view first_cell{
      row_index < first_column.size() ? std::string_view{first_column.at(row_index)} :
                                        std::string_view{}};
    result.append(first_cell);
    // If the second column has text on this row, pad the first column and append the second column.
    // Otherwise, if the second column is exhausted, skip this padding to avoid unnecessary trailing
    // whitespace.
    if (row_index < second_column.size()) {
      const std::size_t first_cell_length{lector::code_points(first_cell)};
      const std::size_t padding{first_column_width + gutter_width - first_cell_length};
      result.append(padding, ' ');
      result.append(second_column.at(row_index));
    }
  }
  return result;
}

/// @brief Collates two strings of text, each representing a column, into a single string that
/// contains newline-separated lines of text, with the lines formatted such that the two columns are
/// right-aligned and spaced a short distance apart.
/// @param[in] first_column_text The string of text for the first column.
/// @param[in] first_column_width The desired width of the first column. Very long words whose
/// length exceeds this width are hyphenated.
/// @param[in] second_column_text The string of text for the second column.
/// @param[in] second_column_width The desired width of the second column. Very long words whose
/// length exceeds this width are hyphenated.
/// @return The string that contains the collated text.
/// @throws std::invalid_argument if either desired column width is zero.
[[nodiscard]] inline std::string collate_and_right_align(
    const std::string_view first_column_text, const std::size_t first_column_width,
    const std::string_view second_column_text, const std::size_t second_column_width) {
  // Use a gutter width of two spaces.
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
                 + (rows
                    * (first_column_width + gutter_width + second_column_width
                       + static_cast<std::size_t>(1UL))));
  // Collate the rows line by line.
  for (std::size_t row_index{0UL}; row_index < rows; ++row_index) {
    // Append a newline character for every row after the first to separate them without leaving a
    // trailing newline at the very end of the string.
    if (row_index > static_cast<std::size_t>(0UL)) {
      result.push_back('\n');
    }
    // Grab the string for the first column if it exists on this row; otherwise, use an empty
    // string.
    const std::string_view first_cell{
      row_index < first_column.size() ? std::string_view{first_column.at(row_index)} :
                                        std::string_view{}};
    const std::size_t first_cell_length{lector::code_points(first_cell)};
    // Calculate the leading padding. The ternary operator protects against std::size_t underflow in
    // the extremely unlikely event a cell exceeds the column width.
    const std::size_t first_cell_padding{
      first_column_width > first_cell_length ? first_column_width - first_cell_length :
                                               static_cast<std::size_t>(0UL)};
    // Right-align the first column by prepending the required padding.
    result.append(first_cell_padding, ' ');
    result.append(first_cell);
    // If the second column has text on this row, append the gutter, pad the second column, and
    // append it. Otherwise, if the second column is exhausted, skip this padding to avoid
    // unnecessary trailing whitespace.
    if (row_index < second_column.size()) {
      const std::string_view second_cell{second_column.at(row_index)};
      const std::size_t second_cell_length{lector::code_points(second_cell)};
      const std::size_t second_cell_padding{
        second_column_width > second_cell_length ? second_column_width - second_cell_length :
                                                   static_cast<std::size_t>(0UL)};
      result.append(gutter_width, ' ');
      result.append(second_cell_padding, ' ');
      result.append(second_cell);
    }
  }
  return result;
}

/// @brief Collates two strings of text, each representing a column, into a single string that
/// contains newline-separated lines of text, with the lines formatted such that the two columns are
/// centre-aligned and spaced a short distance apart. If the total required centre-aligning padding
/// is odd, the text is biased by one space towards the left.
/// @param[in] first_column_text The string of text for the first column.
/// @param[in] first_column_width The desired width of the first column. Very long words whose
/// length exceeds this width are hyphenated.
/// @param[in] second_column_text The string of text for the second column.
/// @param[in] second_column_width The desired width of the second column. Very long words whose
/// length exceeds this width are hyphenated.
/// @return The string that contains the collated text.
/// @throws std::invalid_argument if either desired column width is zero.
[[nodiscard]] inline std::string collate_and_centre_align_with_left_bias(
    const std::string_view first_column_text, const std::size_t first_column_width,
    const std::string_view second_column_text, const std::size_t second_column_width) {
  // Use a gutter width of two spaces.
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
                 + (rows
                    * (first_column_width + gutter_width + second_column_width
                       + static_cast<std::size_t>(1UL))));
  // Collate the rows line by line.
  for (std::size_t row_index{0UL}; row_index < rows; ++row_index) {
    // Append a newline character for every row after the first to separate them without leaving a
    // trailing newline at the very end of the string.
    if (row_index > static_cast<std::size_t>(0UL)) {
      result.push_back('\n');
    }
    // Grab the string for the first column if it exists on this row; otherwise, use an empty
    // string.
    const std::string_view first_cell{
      row_index < first_column.size() ? std::string_view{first_column.at(row_index)} :
                                        std::string_view{}};
    const std::size_t first_cell_length{lector::code_points(first_cell)};
    // Calculate the total padding. The ternary operator protects against std::size_t underflow in
    // the extremely unlikely event a cell exceeds the column width.
    const std::size_t first_cell_total_padding{
      first_column_width > first_cell_length ? first_column_width - first_cell_length :
                                               static_cast<std::size_t>(0UL)};
    // Bias left. When the total number of padding spaces is odd, integer division rounds down,
    // giving one less padding space to the left.
    const std::size_t first_cell_left_padding{first_cell_total_padding / 2UL};
    const std::size_t first_cell_right_padding{first_cell_total_padding - first_cell_left_padding};
    // Append the left padding and the first cell.
    result.append(first_cell_left_padding, ' ');
    result.append(first_cell);
    // If the second column has text on this row, calculate its padding, append the central padding
    // (first cell right padding + gutter + second cell left padding), and append the second cell.
    if (row_index < second_column.size()) {
      const std::string_view second_cell{second_column.at(row_index)};
      const std::size_t second_cell_length{lector::code_points(second_cell)};
      const std::size_t second_cell_total_padding{
        second_column_width > second_cell_length ? second_column_width - second_cell_length :
                                                   static_cast<std::size_t>(0UL)};
      const std::size_t second_cell_left_padding{second_cell_total_padding / 2UL};
      const std::size_t central_padding{
        first_cell_right_padding + gutter_width + second_cell_left_padding};
      result.append(central_padding, ' ');
      result.append(second_cell);
    }
  }
  return result;
}

/// @brief Collates two strings of text, each representing a column, into a single string that
/// contains newline-separated lines of text, with the lines formatted such that the two columns are
/// centre-aligned and spaced a short distance apart. If the total required centre-aligning padding
/// is odd, the text is biased by one space towards the right.
/// @param[in] first_column_text The string of text for the first column.
/// @param[in] first_column_width The desired width of the first column. Very long words whose
/// length exceeds this width are hyphenated.
/// @param[in] second_column_text The string of text for the second column.
/// @param[in] second_column_width The desired width of the second column. Very long words whose
/// length exceeds this width are hyphenated.
/// @return The string that contains the collated text.
/// @throws std::invalid_argument if either desired column width is zero.
[[nodiscard]] inline std::string collate_and_centre_align_with_right_bias(
    const std::string_view first_column_text, const std::size_t first_column_width,
    const std::string_view second_column_text, const std::size_t second_column_width) {
  // Use a gutter width of two spaces.
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
                 + (rows
                    * (first_column_width + gutter_width + second_column_width
                       + static_cast<std::size_t>(1UL))));
  // Collate the rows line by line.
  for (std::size_t row_index{0UL}; row_index < rows; ++row_index) {
    // Append a newline character for every row after the first to separate them without leaving a
    // trailing newline at the very end of the string.
    if (row_index > static_cast<std::size_t>(0UL)) {
      result.push_back('\n');
    }
    // Grab the string for the first column if it exists on this row; otherwise, use an empty
    // string.
    const std::string_view first_cell{
      row_index < first_column.size() ? std::string_view{first_column.at(row_index)} :
                                        std::string_view{}};
    const std::size_t first_cell_length{lector::code_points(first_cell)};
    // Calculate the total padding. The ternary operator protects against std::size_t underflow in
    // the extremely unlikely event a cell exceeds the column width.
    const std::size_t first_cell_total_padding{
      first_column_width > first_cell_length ? first_column_width - first_cell_length :
                                               static_cast<std::size_t>(0UL)};
    // Bias right. When the total number of padding spaces is odd, adding one more space before
    // performing the integer division rounds it up, giving one more padding space to the left.
    const std::size_t first_cell_left_padding{(first_cell_total_padding + 1UL) / 2UL};
    const std::size_t first_cell_right_padding{first_cell_total_padding - first_cell_left_padding};
    // Append the left padding and the first cell.
    result.append(first_cell_left_padding, ' ');
    result.append(first_cell);
    // If the second column has text on this row, calculate its padding, append the central padding
    // (first cell right padding + gutter + second cell left padding), and append the second cell.
    if (row_index < second_column.size()) {
      const std::string_view second_cell{second_column.at(row_index)};
      const std::size_t second_cell_length{lector::code_points(second_cell)};
      const std::size_t second_cell_total_padding{
        second_column_width > second_cell_length ? second_column_width - second_cell_length :
                                                   static_cast<std::size_t>(0UL)};
      const std::size_t second_cell_left_padding{(second_cell_total_padding + 1UL) / 2UL};
      const std::size_t central_padding{
        first_cell_right_padding + gutter_width + second_cell_left_padding};
      result.append(central_padding, ' ');
      result.append(second_cell);
    }
  }
  return result;
}

}  // namespace lector

#endif  // LECTOR_TEXT_HPP
