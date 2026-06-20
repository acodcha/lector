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

#ifndef LECTOR_PARSE_HPP
#define LECTOR_PARSE_HPP

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

#ifdef __APPLE__
  #include <cstddef>
#endif  // __APPLE__

namespace lector {

/// @brief A possible spelling for a value of a specified type.
/// @tparam Type The type of the value.
template <typename Type>
using Spelling = ::std::pair<::std::string_view, Type>;

/// \brief Map of spellings to their corresponding values of a specified type.
/// @tparam Type The type of the values.
template <typename Type>
inline constexpr ::std::array<::lector::Spelling<Type>, 0UL> Spellings{};

/// @brief Parses a string view into an enumeration value. The enumeration type must define a
/// specialization of the lector::Spellings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] text The string view to parse.
/// @return The parsed enumeration value, or std::nullopt if the string view could not be parsed
/// into a valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] constexpr ::std::optional<EnumerationType> parse_enumeration(
    const ::std::string_view text) {
  for (const auto& [spelling, value] : ::lector::Spellings<EnumerationType>) {
    if (spelling == text) {
      return value;
    }
  }
  return ::std::nullopt;
}

/// @brief Parses a string view into a value of a specific type. If the type is an enumeration type,
/// it must define a specialization of the lector::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string view.
/// @param[in] text The string view to parse.
/// @return The parsed value, or std::nullopt if the string view could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] ::std::optional<Type> parse(::std::string_view text);

/// @brief Parses a string view into a boolean value. For example, the string view "true" returns
/// the boolean value true.
/// @param[in] text  The string view to parse.
/// @return The parsed boolean value, or std::nullopt if the string view could not be parsed into a
/// valid boolean value.
template <>
[[nodiscard]] inline ::std::optional<bool> parse(const ::std::string_view text) {
  if (text == "true" || text == "True" || text == "TRUE") {
    return true;
  }
  if (text == "false" || text == "False" || text == "FALSE") {
    return false;
  }
  return ::std::nullopt;
}

/// @brief Parses a string view into an 8-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 8-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 8-bit natural number.
template <>
[[nodiscard]] inline ::std::optional<::std::uint8_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' or '-' sign, if any, and make note of the sign.
  bool is_negative{false};
  if (modified_text.front() == '-') {
    is_negative = true;
    modified_text.remove_prefix(1);
  } else if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::uint8_t number{0U};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  // The only allowed negative number is -0, which parses to 0.
  if (is_negative && number != 0U) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a 16-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 16-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 16-bit natural number.
template <>
[[nodiscard]] inline ::std::optional<::std::uint16_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' or '-' sign, if any, and make note of the sign.
  bool is_negative{false};
  if (modified_text.front() == '-') {
    is_negative = true;
    modified_text.remove_prefix(1);
  } else if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::uint16_t number{0U};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  // The only allowed negative number is -0, which parses to 0.
  if (is_negative && number != 0U) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a 32-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 32-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 32-bit natural number.
template <>
[[nodiscard]] inline ::std::optional<::std::uint32_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' or '-' sign, if any, and make note of the sign.
  bool is_negative{false};
  if (modified_text.front() == '-') {
    is_negative = true;
    modified_text.remove_prefix(1);
  } else if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::uint32_t number{0U};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  // The only allowed negative number is -0, which parses to 0.
  if (is_negative && number != 0U) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a 64-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 64-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 64-bit natural number.
template <>
[[nodiscard]] inline ::std::optional<::std::uint64_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' or '-' sign, if any, and make note of the sign.
  bool is_negative{false};
  if (modified_text.front() == '-') {
    is_negative = true;
    modified_text.remove_prefix(1);
  } else if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::uint64_t number{0UL};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  // The only allowed negative number is -0, which parses to 0.
  if (is_negative && number != 0UL) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into an 8-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 8-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 8-bit integer number.
template <>
[[nodiscard]] inline ::std::optional<::std::int8_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::int8_t number{0};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a 16-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 16-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 16-bit integer number.
template <>
[[nodiscard]] inline ::std::optional<::std::int16_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::int16_t number{0};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a 32-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 32-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 32-bit integer number.
template <>
[[nodiscard]] inline ::std::optional<::std::int32_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::int32_t number{0};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a 64-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 64-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 64-bit integer number.
template <>
[[nodiscard]] inline ::std::optional<::std::int64_t> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
  // Parse the text into a number using std::from_chars().
  ::std::int64_t number{0L};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a single-precision floating-point number. For example, the
/// string view "3.14" returns the number 3.14.
/// @param[in] text The string view to parse.
/// @return The parsed single-precision floating-point number, or std::nullopt if the string view
/// could not be parsed into a valid single-precision floating-point number.
template <>
[[nodiscard]] inline ::std::optional<float> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
#ifdef __APPLE__
  // Check if the first character is a '+' sign. This is needed because std::stod() accepts "++1.0".
  if (!modified_text.empty() && modified_text.front() == '+') {
    return ::std::nullopt;
  }
  // Parse the text into a number using std::stof() instead of std::from_chars(). The <charconv>
  // library on macOS has an older version of std::from_chars() that only supports integral types
  // and does not support floating-point types.
  float number{0.0F};
  ::std::size_t position{0};
  try {
    const ::std::string modified_text_string{modified_text};
    number = ::std::stof(modified_text_string, &position);
    if (position != modified_text_string.size()) {
      return ::std::nullopt;
    }
  } catch (...) {
    return ::std::nullopt;
  }
  return number;
#else
  // Parse the text into a number using std::from_chars().
  float number{0.0F};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
#endif  // __APPLE__
}

/// @brief Parses a string view into a double-precision floating-point number. For example, the
/// string view "3.14" returns the number 3.14.
/// @param[in] text The string view to parse.
/// @return The parsed double-precision floating-point number, or std::nullopt if the string view
/// could not be parsed into a valid double-precision floating-point number.
template <>
[[nodiscard]] inline ::std::optional<double> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
#ifdef __APPLE__
  // Check if the first character is a '+' sign. This is needed because std::stod() accepts "++1.0".
  if (!modified_text.empty() && modified_text.front() == '+') {
    return ::std::nullopt;
  }
  // Parse the text into a number using std::stod() instead of std::from_chars(). The <charconv>
  // library on macOS has an older version of std::from_chars() that only supports integral types
  // and does not support floating-point types.
  double number{0.0};
  ::std::size_t position{0};
  try {
    const ::std::string modified_text_string{modified_text};
    number = ::std::stod(modified_text_string, &position);
    if (position != modified_text_string.size()) {
      return ::std::nullopt;
    }
  } catch (...) {
    return ::std::nullopt;
  }
  return number;
#else
  // Parse the text into a number using std::from_chars().
  double number{0.0};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
#endif  // __APPLE__
}

/// @brief Parses a string view into an extended-precision floating-point number. For example, the
/// string view "3.14" returns the number 3.14.
/// @param[in] text The string view to parse.
/// @return The parsed extended-precision floating-point number, or std::nullopt if the string view
/// could not be parsed into a valid extended-precision floating-point number.
template <>
[[nodiscard]] inline ::std::optional<long double> parse(const ::std::string_view text) {
  ::std::string_view modified_text{text};
  // Strip leading and trailing whitespace.
  modified_text.remove_prefix(
      ::std::min(modified_text.find_first_not_of(" \t\r\n"), modified_text.size()));
  modified_text.remove_suffix(
      modified_text.size() - (modified_text.find_last_not_of(" \t\r\n") + 1));
  if (modified_text.empty()) {
    return ::std::nullopt;
  }
  // Strip the leading '+' sign, if any.
  if (modified_text.front() == '+') {
    modified_text.remove_prefix(1);
  }
#ifdef __APPLE__
  // Check if the first character is a '+' sign. This is needed because std::stod() accepts "++1.0".
  if (!modified_text.empty() && modified_text.front() == '+') {
    return ::std::nullopt;
  }
  // Parse the text into a number using std::stod() instead of std::from_chars(). The <charconv>
  // library on macOS has an older version of std::from_chars() that only supports integral types
  // and does not support floating-point types.
  long double number{0.0L};
  ::std::size_t position{0};
  try {
    const ::std::string modified_text_string{modified_text};
    number = ::std::stold(modified_text_string, &position);
    if (position != modified_text_string.size()) {
      return ::std::nullopt;
    }
  } catch (...) {
    return ::std::nullopt;
  }
  return number;
#else
  // Parse the text into a number using std::from_chars().
  long double number{0.0L};
  const ::std::from_chars_result result{
    ::std::from_chars(modified_text.data(), modified_text.data() + modified_text.size(), number)};
  if (result.ec != ::std::errc{}) {
    return ::std::nullopt;
  }
  if (result.ptr != modified_text.data() + modified_text.size()) {
    return ::std::nullopt;
  }
  return number;
#endif  // __APPLE__
}

/// @brief Parses a string view into a string. Does not mutate the string view in any way.
/// @param[in] text The string view to parse.
/// @return The parsed string.
template <>
[[nodiscard]] inline ::std::optional<::std::string> parse(const ::std::string_view text) {
  return ::std::string{text};
}

/// @brief Parses a string view into a string view. Does not mutate the string view in any way.
/// @param[in] text The string view to parse.
/// @return The parsed string view.
template <>
[[nodiscard]] inline ::std::optional<::std::string_view> parse(const ::std::string_view text) {
  return text;
}

/// @brief Parses a string view into a filesystem path.
/// @param[in] text The string view to parse.
/// @return The parsed filesystem path, or std::nullopt if the string view could not be parsed into
/// a valid filesystem path.
template <>
[[nodiscard]] inline ::std::optional<::std::filesystem::path> parse(const ::std::string_view text) {
  return ::std::filesystem::path{text};
}

/// @brief Parses a string view into a value of a specific type. If the type is an enumeration type,
/// it must define a specialization of the lector::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string view.
/// @param[in] text The string view to parse.
/// @return The parsed value, or std::nullopt if the string view could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] inline ::std::optional<Type> parse(const ::std::string_view text) {
  if constexpr (::std::is_enum_v<Type>) {
    return ::lector::parse_enumeration<Type>(text);
  } else {
    ::std::istringstream stream{::std::string{text}};
    Type value;
    if (stream >> value) {
      if (stream.eof()) {
        return value;
      }
    }
    return ::std::nullopt;
  }
}

}  // namespace lector

#endif  // LECTOR_PARSE_HPP
