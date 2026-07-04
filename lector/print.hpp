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

#ifndef LECTOR_PRINT_HPP
#define LECTOR_PRINT_HPP

#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

/// @brief The Lector library's namespace.
namespace lector {

/// @brief The canonical printed name for a value of a specified type.
/// @tparam Type The type of the value.
template <typename Type>
using Name = ::std::pair<Type, ::std::string_view>;

/// @brief Map of canonical printed names to their corresponding values of a specified type.
/// @tparam Type The type of the values.
template <typename Type>
inline constexpr ::std::array<::lector::Name<Type>, 0UL> Names{};

/// @brief Prints a value of a specified enumeration type as a string of text. The enumeration type
/// must define a specialization of the lector::Names constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] value The enumeration value to print.
/// @return The value as a printed string of text.
template <typename EnumerationType>
[[nodiscard]] constexpr ::std::string_view print_enumeration(const EnumerationType value) {
  for (const auto& [enumeration_value, name] : ::lector::Names<EnumerationType>) {
    if (enumeration_value == value) {
      return name;
    }
  }
  return ::std::string_view{};
}

/// @brief Prints a value of a specific type as a string of text.
/// @tparam Type The type of the value to print.
/// @param[in] value The value to print.
/// @return The value as a printed string of text.
template <typename Type>
[[nodiscard]] ::std::string print(const Type& value);

/// @brief Prints a boolean value as a string of text.
/// @param[in] value The boolean value to print.
/// @return The boolean value as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const bool& value) {
  if (value) {
    return "true";
  }
  return "false";
}

/// @brief Prints an 8-bit natural number as a string of text.
/// @param[in] value The 8-bit natural number to print.
/// @return The 8-bit natural number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint8_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 16-bit natural number as a string of text.
/// @param[in] value The 16-bit natural number to print.
/// @return The 16-bit natural number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint16_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 32-bit natural number as a string of text.
/// @param[in] value The 32-bit natural number to print.
/// @return The 32-bit natural number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint32_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 64-bit natural number as a string of text.
/// @param[in] value The 64-bit natural number to print.
/// @return The 64-bit natural number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint64_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints an 8-bit integer number as a string of text.
/// @param[in] value The 8-bit integer number to print.
/// @return The 8-bit integer number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int8_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 16-bit integer number as a string of text.
/// @param[in] value The 16-bit integer number to print.
/// @return The 16-bit integer number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int16_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 32-bit integer number as a string of text.
/// @param[in] value The 32-bit integer number to print.
/// @return The 32-bit integer number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int32_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 64-bit integer number as a string of text.
/// @param[in] value The 64-bit integer number to print.
/// @return The 64-bit integer number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int64_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a single-precision floating-point number as a string of text.
/// @param[in] value The single-precision floating-point number to print.
/// @return The single-precision floating-point number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const float& value) {
  if (value == 0.0F) {
    return "0";
  }
  if (!::std::isfinite(value)) {
    if (::std::isnan(value)) {
      return "nan";
    }
    if (value < 0.0F) {
      return "-inf";
    }
    return "inf";
  }
  const float absolute{::std::abs(value)};
  constexpr int digits{::std::numeric_limits<float>::max_digits10};
  ::std::ostringstream stream;
  if (absolute < 0.001F || absolute >= 10000.0F) {
    stream << ::std::scientific << ::std::setprecision(digits) << value;
  } else {
    stream << ::std::fixed;
    if (absolute < 0.01F) {
      stream << ::std::setprecision(digits + 3);
    } else if (absolute < 0.1F) {
      stream << ::std::setprecision(digits + 2);
    } else if (absolute < 1.0F) {
      stream << ::std::setprecision(digits + 1);
    } else if (absolute < 10.0F) {
      stream << ::std::setprecision(digits);
    } else if (absolute < 100.0F) {
      stream << ::std::setprecision(digits - 1);
    } else if (absolute < 1000.0F) {
      stream << ::std::setprecision(digits - 2);
    } else {
      stream << ::std::setprecision(digits - 3);
    }
    stream << value;
  }
  return stream.str();
}

/// @brief Prints a double-precision floating-point number as a string of text.
/// @param[in] value The double-precision floating-point number to print.
/// @return The double-precision floating-point number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const double& value) {
  if (value == 0.0) {
    return "0";
  }
  if (!::std::isfinite(value)) {
    if (::std::isnan(value)) {
      return "nan";
    }
    if (value < 0.0) {
      return "-inf";
    }
    return "inf";
  }
  const double absolute{::std::abs(value)};
  constexpr int digits{::std::numeric_limits<double>::max_digits10};
  ::std::ostringstream stream;
  if (absolute < 0.001 || absolute >= 10000.0) {
    stream << ::std::scientific << ::std::setprecision(digits) << value;
  } else {
    stream << ::std::fixed;
    if (absolute < 0.01) {
      stream << ::std::setprecision(digits + 3);
    } else if (absolute < 0.1) {
      stream << ::std::setprecision(digits + 2);
    } else if (absolute < 1.0) {
      stream << ::std::setprecision(digits + 1);
    } else if (absolute < 10.0) {
      stream << ::std::setprecision(digits);
    } else if (absolute < 100.0) {
      stream << ::std::setprecision(digits - 1);
    } else if (absolute < 1000.0) {
      stream << ::std::setprecision(digits - 2);
    } else {
      stream << ::std::setprecision(digits - 3);
    }
    stream << value;
  }
  return stream.str();
}

/// @brief Prints an extended-precision floating-point number as a string of text.
/// @param[in] value The extended-precision floating-point number to print.
/// @return The extended-precision floating-point number as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const long double& value) {
  if (value == 0.0L) {
    return "0";
  }
  if (!::std::isfinite(value)) {
    if (::std::isnan(value)) {
      return "nan";
    }
    if (value < 0.0L) {
      return "-inf";
    }
    return "inf";
  }
  const long double absolute{::std::abs(value)};
  constexpr int digits{::std::numeric_limits<long double>::max_digits10};
  ::std::ostringstream stream;
  if (absolute < 0.001F || absolute >= 10000.0F) {
    stream << ::std::scientific << ::std::setprecision(digits) << value;
  } else {
    stream << ::std::fixed;
    if (absolute < 0.01L) {
      stream << ::std::setprecision(digits + 3);
    } else if (absolute < 0.1L) {
      stream << ::std::setprecision(digits + 2);
    } else if (absolute < 1.0L) {
      stream << ::std::setprecision(digits + 1);
    } else if (absolute < 10.0L) {
      stream << ::std::setprecision(digits);
    } else if (absolute < 100.0L) {
      stream << ::std::setprecision(digits - 1);
    } else if (absolute < 1000.0L) {
      stream << ::std::setprecision(digits - 2);
    } else {
      stream << ::std::setprecision(digits - 3);
    }
    stream << value;
  }
  return stream.str();
}

/// @brief Returns a string of text. No-op.
/// @param[in] value The string of text to print.
/// @return The string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::string& value) {
  return value;
}

/// @brief Prints a string view as a string of text.
/// @param[in] value The string view to print.
/// @return The string view as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::string_view& value) {
  return ::std::string(value);
}

/// @brief Prints a filesystem path as a string of text.
/// @param[in] value The filesystem path to print.
/// @return The filesystem path as a printed string of text.
template <>
[[nodiscard]] inline ::std::string print(const ::std::filesystem::path& value) {
  return value.string();
}

/// @brief Prints a value of a specific type as a string of text. If the type is an enumeration
/// type, it must define a specialization of the lector::Names constant for its type.
/// @tparam Type The type of the value to print.
/// @param[in] value The value to print.
/// @return The value as a printed string of text.
template <typename Type>
[[nodiscard]] inline ::std::string print(const Type& value) {
  if constexpr (::std::is_enum_v<Type>) {
    return ::std::string{::lector::print_enumeration(value)};
  } else {
    ::std::ostringstream stream;
    stream << value;
    return stream.str();
  }
}

/// @brief Counts and returns the number of UTF-8 code points in a string of text. The number of
/// code points is a useful approximation of the length of the string in terms of graphemes.
/// @param[in] text The string of text whose code points are to be counted.
/// @return The number of UTF-8 code points in the string of text.
inline ::std::size_t utf8_code_points(const ::std::string_view text) {
  // This function uses an optimized trick for counting code points. It works because of how UTF-8
  // is designed at the binary level:
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

/// @brief Left-aligns and wraps a string of text to a maximum line length.
/// @param[in] text The string of text to left-align and wrap.
/// @param[in] maximum_line_length The maximum line length to use when wrapping.
/// @return The resulting left-aligned and wrapped text.
inline ::std::string wrap(const ::std::string_view text, const ::std::size_t maximum_line_length) {
  // Adjust the maximum line length if needed. A specified maximum line length of 0 indicates that
  // the line length is unlimited.
  ::std::size_t actual_maximum_line_length{maximum_line_length};
  if (actual_maximum_line_length == static_cast<::std::size_t>(0UL)) {
    actual_maximum_line_length = ::std::string::npos;
  }
  // Pre-allocate memory to avoid multiple reallocations. Assume an average word and space
  // distribution.
  ::std::string result;
  result.reserve(text.length() + (text.length() / actual_maximum_line_length)
                 + static_cast<::std::size_t>(1UL));
  // Iterate through the string.
  ::std::size_t current_line_length{0};
  bool is_first_word{true};
  ::std::size_t index{0};
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
    // Find the boundaries of the next word.
    const ::std::size_t word_start_index{index};
    while (index < text.length() && ::std::isspace(static_cast<unsigned char>(text[index])) == 0) {
      ++index;
    }
    // Obtain the next word.
    const ::std::string_view word{text.substr(word_start_index, index - word_start_index)};
    const ::std::size_t word_length{utf8_code_points(word)};
    // Place the word in the result string.
    if (is_first_word) {
      result.append(word);
      current_line_length = word_length;
      is_first_word = false;
    } else {
      // Check if adding the word plus a space would exceed the line limit.
      if (current_line_length + 1 + word_length <= actual_maximum_line_length) {
        // In this case, the word can be safely added to the current line.
        result.push_back(' ');
        result.append(word);
        current_line_length += static_cast<::std::size_t>(1UL) + word_length;
      } else {
        // In this case, the word doesn't fit on this line, so drop down to the next line.
        result.push_back('\n');
        result.append(word);
        current_line_length = word_length;
      }
    }
  }
  return result;
}

}  // namespace lector

#endif  // LECTOR_PRINT_HPP
