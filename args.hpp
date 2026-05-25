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

/// @brief Parses a spelling into an enumeration value. The enumeration type must define a
/// specialization of the args::Spellings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] spelling A spelling of the enumeration value to parse.
/// @return The parsed enumeration value, or std::nullopt if the spelling could not be parsed into a
/// valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] std::optional<EnumerationType> parse_enumeration(const std::string_view spelling) {
  const typename std::unordered_map<std::string_view, EnumerationType>::const_iterator found{
    args::Spellings<EnumerationType>.find(spelling)};

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

/// @brief Parses a spelling into a boolean value.
/// @param[in] spelling  A spelling of the boolean value to parse.
/// @return The parsed boolean value, or std::nullopt if the spelling could not be parsed into a
/// valid boolean value.
[[nodiscard]] std::optional<bool> parse_boolean(const std::string_view spelling) {
  if (spelling == "TRUE" || spelling == "True" || spelling == "true" || spelling == "1") {
    return true;
  }

  if (spelling == "FALSE" || spelling == "False" || spelling == "false" || spelling == "0") {
    return false;
  }

  return std::nullopt;
}

/// @brief Parses a spelling into a value of a specific type. If the type is an enumeration type, it
/// must define a specialization of the args::Spellings constant for its type.
/// @tparam Type The type in which to parse the spelling.
/// @param[in] spelling A spelling of the value to parse.
/// @return The parsed value, or std::nullopt if the spelling could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] std::optional<Type> parse_type(const std::string_view spelling) {
  if constexpr (std::is_same_v<Type, bool>) {
    return args::parse_boolean(spelling);
  }

  if constexpr (std::is_enum_v<Type>) {
    return args::parse_enumeration<Type>(spelling);
  }

  if constexpr (std::is_same_v<Type, std::string>) {
    return std::string{spelling};
  }

  if constexpr (std::is_same_v<Type, std::filesystem::path>) {
    return std::filesystem::path{spelling};
  }

  std::istringstream stream{std::string{spelling}};
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
