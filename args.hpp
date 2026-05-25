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

#ifndef ARGS_HPP
#define ARGS_HPP

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace args {

/// \brief Map of spellings to their corresponding enumeration values.
/// @tparam EnumerationType The enumeration type.
template <typename EnumerationType>
inline const std::unordered_map<std::string_view, EnumerationType> Spellings;

/// @brief Parses a string into an enumeration value. The enumeration type must define a
/// specialization of the args::Spellings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] string The string to parse.
/// @return The parsed enumeration value, or std::nullopt if the string could not be parsed into a
/// valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] std::optional<EnumerationType> parse_enumeration(const std::string_view string) {
  const typename std::unordered_map<std::string_view, EnumerationType>::const_iterator found{
    args::Spellings<EnumerationType>.find(string)};
  if (found != args::Spellings<EnumerationType>.cend()) {
    return found->second;
  }
  return std::nullopt;
}

/// @brief Importance of a command line argument.
enum class Importance : int8_t {
  /// @brief The command line argument is optional; it may or may not be provided by the user.
  Optional = 0,

  /// @brief The command line argument is required; it must be provided by the user.
  Required = 1,
};

/// @brief Specialization of the args::Spellings constant for the args::Importance enumeration.
template <>
inline const std::unordered_map<std::string_view, args::Importance> Spellings<args::Importance>{
  {"OPT",      args::Importance::Optional},
  {"Opt",      args::Importance::Optional},
  {"opt",      args::Importance::Optional},
  {"OPTIONAL", args::Importance::Optional},
  {"Optional", args::Importance::Optional},
  {"optional", args::Importance::Optional},
  {"REQ",      args::Importance::Required},
  {"Req",      args::Importance::Required},
  {"req",      args::Importance::Required},
  {"REQUIRED", args::Importance::Required},
  {"Required", args::Importance::Required},
  {"required", args::Importance::Required},
};

/// @brief Parses a string into a value of a specific type. If the type is an enumeration type, it
/// must define a specialization of the args::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string.
/// @param[in] string The string to parse.
/// @return The parsed value, or std::nullopt if the string could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] std::optional<Type> parse(const std::string_view string);

/// @brief Parses a string into a boolean value. For example, the string "true" returns the boolean
/// value true.
/// @param[in] string  The string to parse.
/// @return The parsed boolean value, or std::nullopt if the string could not be parsed into a valid
/// boolean value.
template <>
[[nodiscard]] std::optional<bool> parse(const std::string_view string) {
  if (string == "TRUE" || string == "True" || string == "true" || string == "1") {
    return true;
  }
  if (string == "FALSE" || string == "False" || string == "false" || string == "0") {
    return false;
  }
  return std::nullopt;
}

/// @brief Parses a string into an 8-bit natural number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 8-bit natural number, or std::nullopt if the string could not be parsed into
/// a valid 8-bit natural number.
template <>
[[nodiscard]] std::optional<uint8_t> parse(const std::string_view string) {
  if (string.find('-') != std::string::npos) {
    return std::nullopt;
  }
  uint64_t number;
  try {
    number = std::stoull(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())) {
    return std::nullopt;
  }
  return static_cast<uint8_t>(number);
}

/// @brief Parses a string into a 16-bit natural number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 16-bit natural number, or std::nullopt if the string could not be parsed into
/// a valid 16-bit natural number.
template <>
[[nodiscard]] std::optional<uint16_t> parse(const std::string_view string) {
  if (string.find('-') != std::string::npos) {
    return std::nullopt;
  }
  uint64_t number;
  try {
    number = std::stoull(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<uint64_t>(std::numeric_limits<uint16_t>::max())) {
    return std::nullopt;
  }
  return static_cast<uint16_t>(number);
}

/// @brief Parses a string into a 32-bit natural number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 32-bit natural number, or std::nullopt if the string could not be parsed into
/// a valid 32-bit natural number.
template <>
[[nodiscard]] std::optional<uint32_t> parse(const std::string_view string) {
  if (string.find('-') != std::string::npos) {
    return std::nullopt;
  }
  uint64_t number;
  try {
    number = std::stoull(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())) {
    return std::nullopt;
  }
  return static_cast<uint32_t>(number);
}

/// @brief Parses a string into a 64-bit natural number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 64-bit natural number, or std::nullopt if the string could not be parsed into
/// a valid 64-bit natural number.
template <>
[[nodiscard]] std::optional<uint64_t> parse(const std::string_view string) {
  if (string.find('-') != std::string::npos) {
    return std::nullopt;
  }
  uint64_t number;
  try {
    number = std::stoull(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string into an 8-bit integer number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 8-bit integer number, or std::nullopt if the string could not be parsed into
/// a valid 8-bit integer number.
template <>
[[nodiscard]] std::optional<int8_t> parse(const std::string_view string) {
  int64_t number;
  try {
    number = std::stoll(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<int64_t>(std::numeric_limits<int8_t>::max())) {
    return std::nullopt;
  }
  if (number < static_cast<int64_t>(std::numeric_limits<int8_t>::lowest())) {
    return std::nullopt;
  }
  return static_cast<int8_t>(number);
}

/// @brief Parses a string into a 16-bit integer number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 16-bit integer number, or std::nullopt if the string could not be parsed into
/// a valid 16-bit integer number.
template <>
[[nodiscard]] std::optional<int16_t> parse(const std::string_view string) {
  int64_t number;
  try {
    number = std::stoll(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<int64_t>(std::numeric_limits<int16_t>::max())) {
    return std::nullopt;
  }
  if (number < static_cast<int64_t>(std::numeric_limits<int16_t>::lowest())) {
    return std::nullopt;
  }
  return static_cast<int16_t>(number);
}

/// @brief Parses a string into a 32-bit integer number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 32-bit integer number, or std::nullopt if the string could not be parsed into
/// a valid 32-bit integer number.
template <>
[[nodiscard]] std::optional<int32_t> parse(const std::string_view string) {
  int64_t number;
  try {
    number = std::stoll(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<int64_t>(std::numeric_limits<int32_t>::max())) {
    return std::nullopt;
  }
  if (number < static_cast<int64_t>(std::numeric_limits<int32_t>::lowest())) {
    return std::nullopt;
  }
  return static_cast<int32_t>(number);
}

/// @brief Parses a string into a 64-bit integer number. For example, the string "42" returns the
/// number 42.
/// @param[in] string The string to parse.
/// @return The parsed 64-bit integer number, or std::nullopt if the string could not be parsed into
/// a valid 64-bit integer number.
template <>
[[nodiscard]] std::optional<int64_t> parse(const std::string_view string) {
  int64_t number;
  try {
    number = std::stoll(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string into a single-precision floating-point number. For example, the string
/// "3.14" returns the number 3.14.
/// @param[in] string The string to parse.
/// @return The parsed single-precision floating-point number, or std::nullopt if the string could
/// not be parsed into a valid single-precision floating-point number.
template <>
[[nodiscard]] std::optional<float> parse(const std::string_view string) {
  float number;
  try {
    number = std::stof(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string into a double-precision floating-point number. For example, the string
/// "3.14" returns the number 3.14.
/// @param[in] string The string to parse.
/// @return The parsed double-precision floating-point number, or std::nullopt if the string could
/// not be parsed into a valid double-precision floating-point number.
template <>
[[nodiscard]] std::optional<double> parse(const std::string_view string) {
  double number;
  try {
    number = std::stod(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string into an extended-precision floating-point number. For example, the string
/// "3.14" returns the number 3.14.
/// @param[in] string The string to parse.
/// @return The parsed extended-precision floating-point number, or std::nullopt if the string could
/// not be parsed into a valid extended-precision floating-point number.
template <>
[[nodiscard]] std::optional<long double> parse(const std::string_view string) {
  long double number;
  try {
    number = std::stold(std::string{string});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string into a string. Does not mutate the string in any way.
/// @param[in] string The string to parse.
/// @return The parsed string.
template <>
[[nodiscard]] std::optional<std::string> parse(const std::string_view string) {
  return std::string{string};
}

/// @brief Parses a string into a filesystem path.
/// @param[in] string The string to parse.
/// @return The parsed filesystem path, or std::nullopt if the string could not be parsed into a
/// valid filesystem path.
template <>
[[nodiscard]] std::optional<std::filesystem::path> parse(const std::string_view string) {
  return std::filesystem::path{string};
}

/// @brief Parses a string into a value of a specific type. If the type is an enumeration type, it
/// must define a specialization of the args::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string.
/// @param[in] string The string to parse.
/// @return The parsed value, or std::nullopt if the string could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] std::optional<Type> parse(const std::string_view string) {
  if constexpr (std::is_enum_v<Type>) {
    return args::parse_enumeration<Type>(string);
  }

  std::istringstream stream{std::string{string}};
  Type value;
  if (stream >> value) {
    return value;
  }

  return std::nullopt;
}

/// @brief Forward declaration of the args::Argument class.
/// @tparam Label The label of the command line argument.
/// @tparam Type The type of the command line argument.
template <auto Label, typename Type = bool>
class Argument;

/// @brief Type trait used to extract the exact args::Argument<Label, Type> from a variadic pack of
/// args::Arguments, using only the Label.
/// @tparam Label The label of the command line argument.
/// @tparam Types The list of types of the command line arguments.
template <auto Label, typename... Types>
struct FindArgumentByLabel;

template <auto Label, typename Type, typename... Rest>
struct FindArgumentByLabel<Label, args::Argument<Label, Type>, Rest...> {
  using type = args::Argument<Label, Type>;
};

template <auto Label, auto OtherLabel, typename OtherType, typename... Rest>
struct FindArgumentByLabel<Label, args::Argument<OtherLabel, OtherType>, Rest...> {
  using type = typename args::FindArgumentByLabel<Label, Rest...>::type;
};

}  // namespace args

#endif  // ARGS_HPP
