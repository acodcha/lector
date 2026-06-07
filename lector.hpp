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

#ifndef LECTOR_HPP
#define LECTOR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace lector {

/// @brief The official printing for a value of a specified type.
/// @tparam Type The type of the value.
template <typename Type>
using Printing = std::pair<Type, std::string_view>;

/// @brief A possible spelling for a value of a specified type.
/// @tparam Type The type of the value.
template <typename Type>
using Spelling = std::pair<std::string_view, Type>;

/// @brief Map of printings to their corresponding values of a specified type.
/// @tparam Type The type of the values.
template <typename Type>
inline constexpr std::array<lector::Printing<Type>, 0> Printings{};

/// \brief Map of spellings to their corresponding values of a specified type.
/// @tparam Type The type of the values.
template <typename Type>
inline constexpr std::array<lector::Spelling<Type>, 0> Spellings{};

/// @brief Prints a value of a specified enumeration type. The enumeration type must define a
/// specialization of the lector::Printings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] value The enumeration value to print.
/// @return The string view representing the printed value.
/// @throws std::invalid_argument if the value is not a valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] constexpr std::string_view print_enumeration(const EnumerationType value) {
  for (const auto& [enumeration_value, printing] : lector::Printings<EnumerationType>) {
    if (enumeration_value == value) {
      return printing;
    }
  }
  return std::string_view{};
}

/// @brief Parses a string view into an enumeration value. The enumeration type must define a
/// specialization of the lector::Spellings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] text The string view to parse.
/// @return The parsed enumeration value, or std::nullopt if the string view could not be parsed
/// into a valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] constexpr std::optional<EnumerationType> parse_enumeration(
    const std::string_view text) {
  for (const auto& [spelling, value] : lector::Spellings<EnumerationType>) {
    if (spelling == text) {
      return value;
    }
  }
  return std::nullopt;
}

/// @brief Importance of a command line argument.
enum class Importance : std::int8_t {
  /// @brief The command line argument is optional; it may or may not be provided by the user.
  Optional = 0,

  /// @brief The command line argument is required; it must be provided by the user.
  Required = 1,
};

/// @brief Specialization of the lector::Printings constant for the lector::Importance enumeration.
template <>
inline constexpr std::array<lector::Printing<lector::Importance>, 2> Printings<lector::Importance>{
  {
   {lector::Importance::Optional, "Optional"},
   {lector::Importance::Required, "Required"},
   }
};

/// @brief Specialization of the lector::Spellings constant for the lector::Importance enumeration.
template <>
inline constexpr std::array<lector::Spelling<lector::Importance>, 6> Spellings<lector::Importance>{
  {
   {"Optional", lector::Importance::Optional},
   {"Required", lector::Importance::Required},
   {"optional", lector::Importance::Optional},
   {"required", lector::Importance::Required},
   {"OPTIONAL", lector::Importance::Optional},
   {"REQUIRED", lector::Importance::Required},
   }
};

/// @brief Prints a value of a specific type as a string.
/// @tparam Type The type of the value to print.
/// @param[in] value The value to print.
/// @return The string that contains the printed value.
template <typename Type>
[[nodiscard]] std::string print(const Type& value);

/// @brief Prints a boolean value as a string.
/// @param[in] value The boolean value to print.
/// @return The string that contains the printed boolean value.
template <>
[[nodiscard]] std::string print(const bool& value) {
  if (value) {
    return "true";
  }
  return "false";
}

/// @brief Prints an 8-bit natural number as a string.
/// @param[in] value The 8-bit natural number to print.
/// @return The string that contains the printed 8-bit natural number.
template <>
[[nodiscard]] std::string print(const std::uint8_t& value) {
  return std::to_string(value);
}

/// @brief Prints a 16-bit natural number as a string.
/// @param[in] value The 16-bit natural number to print.
/// @return The string that contains the printed 16-bit natural number.
template <>
[[nodiscard]] std::string print(const std::uint16_t& value) {
  return std::to_string(value);
}

/// @brief Prints a 32-bit natural number as a string.
/// @param[in] value The 32-bit natural number to print.
/// @return The string that contains the printed 32-bit natural number.
template <>
[[nodiscard]] std::string print(const std::uint32_t& value) {
  return std::to_string(value);
}

/// @brief Prints a 64-bit natural number as a string.
/// @param[in] value The 64-bit natural number to print.
/// @return The string that contains the printed 64-bit natural number.
template <>
[[nodiscard]] std::string print(const std::uint64_t& value) {
  return std::to_string(value);
}

/// @brief Prints an 8-bit integer number as a string.
/// @param[in] value The 8-bit integer number to print.
/// @return The string that contains the printed 8-bit integer number.
template <>
[[nodiscard]] std::string print(const std::int8_t& value) {
  return std::to_string(value);
}

/// @brief Prints a 16-bit integer number as a string.
/// @param[in] value The 16-bit integer number to print.
/// @return The string that contains the printed 16-bit integer number.
template <>
[[nodiscard]] std::string print(const std::int16_t& value) {
  return std::to_string(value);
}

/// @brief Prints a 32-bit integer number as a string.
/// @param[in] value The 32-bit integer number to print.
/// @return The string that contains the printed 32-bit integer number.
template <>
[[nodiscard]] std::string print(const std::int32_t& value) {
  return std::to_string(value);
}

/// @brief Prints a 64-bit integer number as a string.
/// @param[in] value The 64-bit integer number to print.
/// @return The string that contains the printed 64-bit integer number.
template <>
[[nodiscard]] std::string print(const std::int64_t& value) {
  return std::to_string(value);
}

/// @brief Prints a single-precision floating-point number as a string.
/// @param[in] value The single-precision floating-point number to print.
/// @return The string that contains the printed single-precision floating-point number.
template <>
[[nodiscard]] std::string print(const float& value) {
  const float absolute{std::abs(value)};
  std::ostringstream stream;
  if (absolute < 1.0F) {
    // Interval: [0, 1[
    if (absolute < 0.001F) {
      // Interval: [0, 0.001[
      if (absolute == 0.0F) {
        // Interval: [0, 0]
        stream << 0;
      } else {
        // Interval: ]0, 0.001[
        stream << std::scientific << std::setprecision(std::numeric_limits<float>::max_digits10)
               << value;
      }
    } else {
      // Interval: [0.001, 1[
      if (absolute < 0.1F) {
        // Interval: [0.001, 0.1[
        if (absolute < 0.01F) {
          // Interval: [0.001, 0.01[
          stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10 + 3)
                 << value;
        } else {
          // Interval: [0.01, 0.1[
          stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10 + 2)
                 << value;
        }
      } else {
        // Interval: [0.1, 1[
        stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10 + 1)
               << value;
      }
    }
  } else {
    // Interval: [1, +inf[
    if (absolute < 1000.0F) {
      // Interval: [1, 1000[
      if (absolute < 10.0F) {
        // Interval: [1, 10[
        stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10)
               << value;
      } else {
        // Interval: [10, 1000[
        if (absolute < 100.0F) {
          // Interval: [10, 100[
          stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10 - 1)
                 << value;
        } else {
          // Interval: [100, 1000[
          stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10 - 2)
                 << value;
        }
      }
    } else {
      // Interval: [1000, +inf[
      if (absolute < 10000.0F) {
        // Interval: [1000, 10000[
        stream << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10 - 3)
               << value;
      } else {
        // Interval: [10000, +inf[
        stream << std::scientific << std::setprecision(std::numeric_limits<float>::max_digits10)
               << value;
      }
    }
  }
  return stream.str();
}

/// @brief Prints a double-precision floating-point number as a string.
/// @param[in] value The double-precision floating-point number to print.
/// @return The string that contains the printed double-precision floating-point number.
template <>
[[nodiscard]] std::string print(const double& value) {
  const double absolute{std::abs(value)};
  std::ostringstream stream;
  if (absolute < 1.0) {
    // Interval: [0, 1[
    if (absolute < 0.001) {
      // Interval: [0, 0.001[
      if (absolute == 0.0) {
        // Interval: [0, 0]
        stream << 0;
      } else {
        // Interval: ]0, 0.001[
        stream << std::scientific << std::setprecision(std::numeric_limits<double>::max_digits10)
               << value;
      }
    } else {
      // Interval: [0.001, 1[
      if (absolute < 0.1) {
        // Interval: [0.001, 0.1[
        if (absolute < 0.01) {
          // Interval: [0.001, 0.01[
          stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10 + 3)
                 << value;
        } else {
          // Interval: [0.01, 0.1[
          stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10 + 2)
                 << value;
        }
      } else {
        // Interval: [0.1, 1[
        stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10 + 1)
               << value;
      }
    }
  } else {
    // Interval: [1, +inf[
    if (absolute < 1000.0) {
      // Interval: [1, 1000[
      if (absolute < 10.0) {
        // Interval: [1, 10[
        stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10)
               << value;
      } else {
        // Interval: [10, 1000[
        if (absolute < 100.0) {
          // Interval: [10, 100[
          stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10 - 1)
                 << value;
        } else {
          // Interval: [100, 1000[
          stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10 - 2)
                 << value;
        }
      }
    } else {
      // Interval: [1000, +inf[
      if (absolute < 10000.0) {
        // Interval: [1000, 10000[
        stream << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10 - 3)
               << value;
      } else {
        // Interval: [10000, +inf[
        stream << std::scientific << std::setprecision(std::numeric_limits<double>::max_digits10)
               << value;
      }
    }
  }
  return stream.str();
}

/// @brief Prints an extended-precision floating-point number as a string.
/// @param[in] value The extended-precision floating-point number to print.
/// @return The string that contains the printed extended-precision floating-point number.
template <>
[[nodiscard]] std::string print(const long double& value) {
  const long double absolute{std::abs(value)};
  std::ostringstream stream;
  if (absolute < 1.0L) {
    // Interval: [0, 1[
    if (absolute < 0.001L) {
      // Interval: [0, 0.001[
      if (absolute == 0.0L) {
        // Interval: [0, 0]
        stream << 0;
      } else {
        // Interval: ]0, 0.001[
        stream << std::scientific
               << std::setprecision(std::numeric_limits<long double>::max_digits10) << value;
      }
    } else {
      // Interval: [0.001, 1[
      if (absolute < 0.1L) {
        // Interval: [0.001, 0.1[
        if (absolute < 0.01L) {
          // Interval: [0.001, 0.01[
          stream << std::fixed
                 << std::setprecision(std::numeric_limits<long double>::max_digits10 + 3) << value;
        } else {
          // Interval: [0.01, 0.1[
          stream << std::fixed
                 << std::setprecision(std::numeric_limits<long double>::max_digits10 + 2) << value;
        }
      } else {
        // Interval: [0.1, 1[
        stream << std::fixed
               << std::setprecision(std::numeric_limits<long double>::max_digits10 + 1) << value;
      }
    }
  } else {
    // Interval: [1, +inf[
    if (absolute < 1000.0L) {
      // Interval: [1, 1000[
      if (absolute < 10.0L) {
        // Interval: [1, 10[
        stream << std::fixed << std::setprecision(std::numeric_limits<long double>::max_digits10)
               << value;
      } else {
        // Interval: [10, 1000[
        if (absolute < 100.0L) {
          // Interval: [10, 100[
          stream << std::fixed
                 << std::setprecision(std::numeric_limits<long double>::max_digits10 - 1) << value;
        } else {
          // Interval: [100, 1000[
          stream << std::fixed
                 << std::setprecision(std::numeric_limits<long double>::max_digits10 - 2) << value;
        }
      }
    } else {
      // Interval: [1000, +inf[
      if (absolute < 10000.0L) {
        // Interval: [1000, 10000[
        stream << std::fixed
               << std::setprecision(std::numeric_limits<long double>::max_digits10 - 3) << value;
      } else {
        // Interval: [10000, +inf[
        stream << std::scientific
               << std::setprecision(std::numeric_limits<long double>::max_digits10) << value;
      }
    }
  }
  return stream.str();
}

/// @brief Prints a string as a string.
/// @param[in] value The string to print.
/// @return The string that contains the printed string.
template <>
[[nodiscard]] std::string print(const std::string& value) {
  return value;
}

/// @brief Prints a string view as a string.
/// @param[in] value The string view to print.
/// @return The string that contains the printed string view.
template <>
[[nodiscard]] std::string print(const std::string_view& value) {
  return std::string(value);
}

/// @brief Prints a filesystem path as a string.
/// @param[in] value The filesystem path to print.
/// @return The string that contains the printed filesystem path.
template <>
[[nodiscard]] std::string print(const std::filesystem::path& value) {
  return value.string();
}

/// @brief Prints a value of a specific type as a string. If the type is an enumeration type, it
/// must define a specialization of the lector::Printings constant for its type.
/// @tparam Type The type of the value to print.
/// @param[in] value The value to print.
/// @return The string that contains the printed value.
template <typename Type>
[[nodiscard]] std::string print(const Type& value) {
  if constexpr (std::is_enum_v<Type>) {
    return std::string{lector::print_enumeration(value)};
  } else {
    std::ostringstream stream;
    stream << value;
    return stream.str();
  }
}

/// @brief Parses a string view into a value of a specific type. If the type is an enumeration type,
/// it must define a specialization of the lector::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string view.
/// @param[in] text The string view to parse.
/// @return The parsed value, or std::nullopt if the string view could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] std::optional<Type> parse(const std::string_view text);

/// @brief Parses a string view into a boolean value. For example, the string view "true" returns
/// the boolean value true.
/// @param[in] text  The string view to parse.
/// @return The parsed boolean value, or std::nullopt if the string view could not be parsed into a
/// valid boolean value.
template <>
[[nodiscard]] std::optional<bool> parse(const std::string_view text) {
  if (text == "TRUE" || text == "True" || text == "true") {
    return true;
  }
  if (text == "FALSE" || text == "False" || text == "false") {
    return false;
  }
  return std::nullopt;
}

/// @brief Parses a string view into an 8-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 8-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 8-bit natural number.
template <>
[[nodiscard]] std::optional<std::uint8_t> parse(const std::string_view text) {
  if (text.find('-') != std::string::npos) {
    return std::nullopt;
  }
  std::uint64_t number;
  try {
    number = std::stoull(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<std::uint64_t>(std::numeric_limits<std::uint8_t>::max())) {
    return std::nullopt;
  }
  return static_cast<std::uint8_t>(number);
}

/// @brief Parses a string view into a 16-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 16-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 16-bit natural number.
template <>
[[nodiscard]] std::optional<std::uint16_t> parse(const std::string_view text) {
  if (text.find('-') != std::string::npos) {
    return std::nullopt;
  }
  std::uint64_t number;
  try {
    number = std::stoull(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<std::uint64_t>(std::numeric_limits<std::uint16_t>::max())) {
    return std::nullopt;
  }
  return static_cast<std::uint16_t>(number);
}

/// @brief Parses a string view into a 32-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 32-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 32-bit natural number.
template <>
[[nodiscard]] std::optional<std::uint32_t> parse(const std::string_view text) {
  if (text.find('-') != std::string::npos) {
    return std::nullopt;
  }
  std::uint64_t number;
  try {
    number = std::stoull(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max())) {
    return std::nullopt;
  }
  return static_cast<std::uint32_t>(number);
}

/// @brief Parses a string view into a 64-bit natural number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 64-bit natural number, or std::nullopt if the string view could not be parsed
/// into a valid 64-bit natural number.
template <>
[[nodiscard]] std::optional<std::uint64_t> parse(const std::string_view text) {
  if (text.find('-') != std::string::npos) {
    return std::nullopt;
  }
  std::uint64_t number;
  try {
    number = std::stoull(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into an 8-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 8-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 8-bit integer number.
template <>
[[nodiscard]] std::optional<std::int8_t> parse(const std::string_view text) {
  std::int64_t number;
  try {
    number = std::stoll(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<std::int64_t>(std::numeric_limits<std::int8_t>::max())) {
    return std::nullopt;
  }
  if (number < static_cast<std::int64_t>(std::numeric_limits<std::int8_t>::lowest())) {
    return std::nullopt;
  }
  return static_cast<std::int8_t>(number);
}

/// @brief Parses a string view into a 16-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 16-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 16-bit integer number.
template <>
[[nodiscard]] std::optional<std::int16_t> parse(const std::string_view text) {
  std::int64_t number;
  try {
    number = std::stoll(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<std::int64_t>(std::numeric_limits<std::int16_t>::max())) {
    return std::nullopt;
  }
  if (number < static_cast<std::int64_t>(std::numeric_limits<std::int16_t>::lowest())) {
    return std::nullopt;
  }
  return static_cast<std::int16_t>(number);
}

/// @brief Parses a string view into a 32-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 32-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 32-bit integer number.
template <>
[[nodiscard]] std::optional<std::int32_t> parse(const std::string_view text) {
  std::int64_t number;
  try {
    number = std::stoll(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  if (number > static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max())) {
    return std::nullopt;
  }
  if (number < static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::lowest())) {
    return std::nullopt;
  }
  return static_cast<std::int32_t>(number);
}

/// @brief Parses a string view into a 64-bit integer number. For example, the string view "42"
/// returns the number 42.
/// @param[in] text The string view to parse.
/// @return The parsed 64-bit integer number, or std::nullopt if the string view could not be parsed
/// into a valid 64-bit integer number.
template <>
[[nodiscard]] std::optional<std::int64_t> parse(const std::string_view text) {
  std::int64_t number;
  try {
    number = std::stoll(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a single-precision floating-point number. For example, the
/// string view "3.14" returns the number 3.14.
/// @param[in] text The string view to parse.
/// @return The parsed single-precision floating-point number, or std::nullopt if the string view
/// could not be parsed into a valid single-precision floating-point number.
template <>
[[nodiscard]] std::optional<float> parse(const std::string_view text) {
  float number;
  try {
    number = std::stof(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a double-precision floating-point number. For example, the
/// string view "3.14" returns the number 3.14.
/// @param[in] text The string view to parse.
/// @return The parsed double-precision floating-point number, or std::nullopt if the string view
/// could not be parsed into a valid double-precision floating-point number.
template <>
[[nodiscard]] std::optional<double> parse(const std::string_view text) {
  double number;
  try {
    number = std::stod(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into an extended-precision floating-point number. For example, the
/// string view "3.14" returns the number 3.14.
/// @param[in] text The string view to parse.
/// @return The parsed extended-precision floating-point number, or std::nullopt if the string view
/// could not be parsed into a valid extended-precision floating-point number.
template <>
[[nodiscard]] std::optional<long double> parse(const std::string_view text) {
  long double number;
  try {
    number = std::stold(std::string{text});
  } catch (...) {
    return std::nullopt;
  }
  return number;
}

/// @brief Parses a string view into a string. Does not mutate the string view in any way.
/// @param[in] text The string view to parse.
/// @return The parsed string.
template <>
[[nodiscard]] std::optional<std::string> parse(const std::string_view text) {
  return std::string{text};
}

/// @brief Parses a string view into a string view. Does not mutate the string view in any way.
/// @param[in] text The string view to parse.
/// @return The parsed string view.
template <>
[[nodiscard]] std::optional<std::string_view> parse(const std::string_view text) {
  return text;
}

/// @brief Parses a string view into a filesystem path.
/// @param[in] text The string view to parse.
/// @return The parsed filesystem path, or std::nullopt if the string view could not be parsed into
/// a valid filesystem path.
template <>
[[nodiscard]] std::optional<std::filesystem::path> parse(const std::string_view text) {
  return std::filesystem::path{text};
}

/// @brief Parses a string view into a value of a specific type. If the type is an enumeration type,
/// it must define a specialization of the lector::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string view.
/// @param[in] text The string view to parse.
/// @return The parsed value, or std::nullopt if the string view could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] std::optional<Type> parse(const std::string_view text) {
  if constexpr (std::is_enum_v<Type>) {
    return lector::parse_enumeration<Type>(text);
  } else {
    std::istringstream stream{std::string{text}};
    Type value;
    if (stream >> value) {
      if (stream.eof()) {
        return value;
      }
    }
    return std::nullopt;
  }
}

/// @brief A command line argument, including its label, keys, description, importance, default
/// value, and parsed value.
/// @tparam LabelValue Value of this command line argument's label. The label is used to uniquely
/// identify the command line argument.
/// @tparam Type The type of the command line argument.
template <auto LabelValue, typename Type>
class Argument {
public:
  using ValueType = Type;

  /// @brief Default constructor. Initializes the command line argument with no keys, an empty
  /// description, required importance, and no default value.
  Argument() noexcept {};

  /// @brief Constructor for a required command line argument or a boolean command line argument. No
  /// default value is needed. Boolean command line arguments are always optional and always default
  /// to false.
  /// @param[in] keys The keys that can be used to specify the command line argument.
  /// @param[in] description The description of the command line argument. Used when printing usage
  /// information. Set at construction.
  /// @throws std::invalid_argument if any of the parameters are invalid.
  Argument(const std::initializer_list<std::string>& keys, const std::string& description)
    : keys_{keys}, description_{description},
      importance_{
        std::is_same_v<Type, bool> ? lector::Importance::Optional : lector::Importance::Required} {
    set_boolean_default();
    validate_keys();
    validate_description();
  }

  /// @brief Constructor for an optional non-boolean command line argument. A default value must be
  /// provided.
  /// @param[in] keys The keys that can be used to specify the command line argument.
  /// @param[in] description The description of the command line argument. Used when printing usage
  /// information. Set at construction.
  /// @param[in] default_value The default value of the command line argument.
  /// @throws std::invalid_argument if any of the parameters are invalid.
  Argument(const std::initializer_list<std::string>& keys, const std::string& description,
           const Type& default_value)
    : keys_{keys}, description_{description}, default_value_{default_value},
      importance_{lector::Importance::Optional} {
    validate_keys();
    validate_description();
    validate_default_value();
  }

  /// @brief Destructor. Destroys this command line argument.
  ~Argument() noexcept = default;

  /// @brief Copy constructor. Constructs a command line argument by copying another one.
  Argument(const lector::Argument<LabelValue, Type>&) = default;

  /// @brief Copy assignment operator. Assigns this command line argument by copying another one.
  /// @return This command line argument after the assignment.
  lector::Argument<LabelValue, Type>& operator=(
      const lector::Argument<LabelValue, Type>&) = default;

  /// @brief Move constructor. Constructs a command line argument by moving another one.
  Argument(lector::Argument<LabelValue, Type>&&) noexcept = default;

  /// @brief Move assignment operator. Assigns this command line argument by moving another one.
  /// @return This command line argument after the assignment.
  lector::Argument<LabelValue, Type>& operator=(
      lector::Argument<LabelValue, Type>&&) noexcept = default;

  /// @brief Label of this command line argument. Used to uniquely identify this command line
  /// argument. Set at construction.
  /// @return The label of this command line argument.
  [[nodiscard]] static constexpr auto label() noexcept {
    return LabelValue;
  }

  /// @brief Keys that can be used to specify this command line argument. Set at construction.
  /// @return The keys that can be used to specify this command line argument.
  [[nodiscard]] const std::vector<std::string>& keys() const noexcept {
    return keys_;
  }

  /// @brief Description of this command line argument. Used when printing usage information. Set at
  /// construction.
  /// @return The description of this command line argument.
  [[nodiscard]] std::string_view description() const noexcept {
    return description_;
  }

  /// @brief Default value of this command line argument. Only relevant for optional non-boolean
  /// arguments. Set at construction.
  /// @return The default value of this command line argument.
  [[nodiscard]] const std::optional<Type>& default_value() const noexcept {
    return default_value_;
  }

  /// @brief Parsed value of this command line argument. Set when this command line argument is
  /// parsed.
  /// @return The parsed value of this command line argument.
  [[nodiscard]] const std::optional<Type>& parsed_value() const noexcept {
    return parsed_value_;
  }

  /// @brief Importance of this command line argument. Required arguments must be provided by the
  /// user, while optional arguments may or may not be provided by the user. Set at construction.
  /// @return The importance of this command line argument.
  [[nodiscard]] lector::Importance importance() const noexcept {
    return importance_;
  }

  /// @brief Value of this command line argument. Returns the parsed value if it exists; otherwise,
  /// returns the default value.
  /// @return The value of this command line argument.
  /// @throws std::logic_error if this command line argument is missing both its parsed value and
  /// its default value.
  [[nodiscard]] const Type& parsed_or_default_value() const {
    if (parsed_value_.has_value()) {
      return parsed_value_.value();
    }
    if (default_value_.has_value()) {
      return default_value_.value();
    }
    throw std::logic_error(
        "No parsed or default value for argument '" + longest_key_with_value_type() + "'.");
  }

  /// @brief Sets the parsed value of this command line argument.
  /// @param[in] value The parsed value to set.
  void set_parsed_value(const Type& value) {
    parsed_value_ = value;
  }

  /// @brief Prints the longest key of this command line argument with its associated value type as
  /// a string.
  /// @return The string that contains the longest key and its associated value type.
  std::string longest_key_with_value_type() const {
    std::string result{longest_key()};
    const std::string value_type{print_value_type()};
    if (!value_type.empty()) {
      result += " " + value_type;
    }
    return result;
  }

  /// @brief Prints the keys of this command line argument with their associated value types as a
  /// string.
  /// @return The string that contains the keys and value types.
  std::string keys_with_value_types() const {
    std::string result;
    for (std::size_t index{0}; index < keys_.size(); ++index) {
      const std::string value_type{print_value_type()};
      result += keys_[index];
      if (!value_type.empty()) {
        result += " " + value_type;
      }
      if (index + 1 < keys_.size()) {
        result += ", ";
      }
    }
    return result;
  }

  /// @brief Prints the execution of this command line argument as a string.
  /// @return The string that contains the execution of this command line argument.
  std::string execution() const {
    if constexpr (std::is_same_v<Type, bool>) {
      if (parsed_value_.has_value() && parsed_value_.value()) {
        return longest_key();
      }
      return "";
    } else {
      if (parsed_value_.has_value()) {
        return longest_key() + " " + lector::print<Type>(parsed_value_.value());
      }
      return "";
    }
  }

private:
  /// @brief If this command line argument is a boolean argument, sets its default value to false.
  /// Called by the constructor that does not specify a default value. Boolean arguments are always
  /// optional and always default to false.
  void set_boolean_default() {
    if constexpr (std::is_same_v<Type, bool>) {
      default_value_ = false;
    }
  }

  /// @brief Validates the keys of this command line argument. Called by both constructors.
  /// @throws std::invalid_argument if any of this command line argument's keys are invalid.
  void validate_keys() const {
    if (keys_.empty()) {
      throw std::logic_error("All arguments must each have at least one key.");
    }
    for (const std::string& key : keys_) {
      if (key.empty()) {
        if (longest_key().empty()) {
          throw std::logic_error("Arguments cannot have empty keys.");
        }
        throw std::logic_error("Empty key in argument '" + longest_key_with_value_type()
                               + "'. Arguments cannot have empty keys.");
      }
    }
    for (std::size_t first{0}; first < keys_.size(); ++first) {
      for (std::size_t second{first + 1}; second < keys_.size(); ++second) {
        if (keys_[first] == keys_[second]) {
          throw std::logic_error(
              "Duplicated key '" + keys_[first] + "' in argument '" + longest_key_with_value_type()
              + "'. Arguments cannot have duplicate keys.");
        }
      }
    }
  };

  /// @brief Validates the description of this command line argument. Called by both constructors.
  /// @throws std::invalid_argument if this command line argument's description is empty.
  void validate_description() const {
    if (description_.empty()) {
      throw std::logic_error("Empty description in argument '" + longest_key_with_value_type()
                             + "'. All arguments must have descriptions.");
    }
  };

  /// @brief Validates the default value of this command line argument. Called by the constructor
  /// that specifies a default value.
  /// @throws std::invalid_argument if this command line argument is boolean but has a default
  /// value.
  constexpr void validate_default_value() const {
    if constexpr (std::is_same_v<Type, bool>) {
      throw std::logic_error(
          "Specified default value for boolean argument '" + longest_key_with_value_type()
          + "'. Boolean arguments are always false by default and cannot specify default values.");
    }
  }

  /// @brief Prints the value type of this command line argument as a string.
  /// @return The string that contains the value type.
  std::string_view print_value_type() const {
    if (std::is_same_v<Type, bool>) {
      return "";
    } else if constexpr (std::numeric_limits<Type>::is_integer) {
      return "<number>";
    } else if constexpr (std::is_floating_point_v<Type>) {
      return "<value>";
    } else if constexpr (
        std::is_same_v<Type, std::string> || std::is_same_v<Type, std::string_view>) {
      return "<text>";
    } else if constexpr (std::is_same_v<Type, std::filesystem::path>) {
      return "<path>";
    } else {
      return "<value>";
    }
  }

  /// @brief Returns the longest key of this command line argument.
  /// @return The longest key of this command line argument.
  const std::string& longest_key() const {
    std::size_t longest_key_index{0};
    for (std::size_t index{1}; index < keys_.size(); ++index) {
      if (keys_[index].size() > keys_[longest_key_index].size()) {
        longest_key_index = index;
      }
    }
    return keys_[longest_key_index];
  }

  /// @brief Keys that can be used to specify this command line argument. Set at construction.
  std::vector<std::string> keys_;

  /// @brief Description of this command line argument. Used when printing usage information. Set at
  /// construction.
  std::string description_;

  /// @brief Default value of this command line argument. Only relevant for optional non-boolean
  /// arguments. Set at construction.
  std::optional<Type> default_value_;

  /// @brief Parsed value of this command line argument. Set when this command line argument is
  /// parsed.
  std::optional<Type> parsed_value_;

  /// @brief Importance of this command line argument. Required arguments must be provided by the
  /// user, while optional arguments may or may not be provided by the user. Set at construction.
  lector::Importance importance_{lector::Importance::Required};
};

/// @brief Type trait used to extract a command line argument from a collection of command line
/// arguments, using only its Label.
/// @tparam Label The label of the command line argument to extract.
/// @tparam ...ArgumentTypes The variadic list of argument types in the collection of command line
/// arguments.
template <auto Label, typename... ArgumentTypes>
struct FindArgumentByLabel;

/// @brief Type trait specialization used to extract a command line argument from a collection of
/// command line arguments, using its Label, its type, and the types of the remaining command line
/// arguments in the collection.
/// @tparam Label The label of the command line argument to extract.
/// @tparam Type The type of the command line argument to extract.
/// @tparam ...OtherArgumentTypes The variadic list of argument types in the collection of command
/// line arguments, excluding the command line argument to extract.
template <auto Label, typename Type, typename... OtherArgumentTypes>
struct FindArgumentByLabel<Label, lector::Argument<Label, Type>, OtherArgumentTypes...> {
  using type = lector::Argument<Label, Type>;
};

/// @brief Type trait specialization used to extract a command line argument from a collection of
/// command line arguments, using its Label and the types of the remaining command line arguments in
/// the collection.
/// @tparam Label The label of the command line argument to extract.
/// @tparam OtherLabel The label of the command line argument to compare against.
/// @tparam OtherType The type of the command line argument to compare against.
/// @tparam ...RemainingArgumentTypes The variadic list of argument types in the collection of
/// command line arguments, excluding the command line argument to extract.
template <auto Label, auto OtherLabel, typename OtherType, typename... RemainingArgumentTypes>
struct FindArgumentByLabel<Label, lector::Argument<OtherLabel, OtherType>,
                           RemainingArgumentTypes...> {
  using type = typename lector::FindArgumentByLabel<Label, RemainingArgumentTypes...>::type;
};

/// @brief A collection of command line arguments that can be parsed from argc and argv.
/// @tparam ...ArgumentTypes Variadic list of the types of the command line arguments in this
/// collection.
template <typename... ArgumentTypes>
class Arguments {
public:
  /// @brief Constructor. Constructs a collection of command line arguments by moving a variadic
  /// list of command line arguments.
  /// @param ...arguments The variadic list of command line arguments.
  explicit Arguments(ArgumentTypes... arguments) : arguments_{std::move(arguments)...} {}

  /// @brief Destructor. Destroys this collection of command line arguments.
  ~Arguments() noexcept = default;

  /// @brief Copy constructor. Constructs a collection of command line argument by copying another
  /// one.
  Arguments(const lector::Arguments<ArgumentTypes...>&) = default;

  /// @brief Copy assignment operator. Assigns this collection of command line argument by copying
  /// another one.
  /// @return This collection of command line argument after the assignment.
  lector::Arguments<ArgumentTypes...>& operator=(
      const lector::Arguments<ArgumentTypes...>&) = default;

  /// @brief Move constructor. Constructs a collection of command line argument by moving another
  /// one.
  Arguments(lector::Arguments<ArgumentTypes...>&&) noexcept = default;

  /// @brief Move assignment operator. Assigns this collection of command line argument by moving
  /// another one.
  /// @return This collection of command line argument after the assignment.
  lector::Arguments<ArgumentTypes...>& operator=(
      lector::Arguments<ArgumentTypes...>&&) noexcept = default;

  /// @brief Parses argc and argv to populate the parsed values of the command line arguments in
  /// this collection.
  /// @param[in] argc The number of command line arguments, including the executable path.
  /// @param[in] argv The array of C-strings that represents the command line arguments, starting
  /// with the executable path.
  /// @throws std::invalid_argument if an invalid, unknown, duplicated, or missing argument is
  /// encountered.
  void parse(const int argc, char* argv[]) {
    parse_executable_path(argc, argv);
    parse_arguments(argc, argv);
    validate_all_required_arguments_have_parsed_values();
  }

  /// @brief Returns the executable path of this collection of command line arguments. If the
  /// command line arguments have not yet been parsed from argc and argv, this path is empty.
  /// @return The executable path of this collection of command line arguments.
  [[nodiscard]] const std::filesystem::path& executable_path() const {
    return executable_path_;
  }

  /// @brief Returns a specified command line argument from this collection.
  /// @tparam Label The label of the command line argument to return.
  /// @return The specified command line argument.
  template <auto Label>
  [[nodiscard]] const auto& get() const {
    using Type = typename lector::FindArgumentByLabel<Label, ArgumentTypes...>::type;
    return std::get<Type>(arguments_);
  }

  /// @brief Prints the execution of this collection of command line argument as a string.
  /// @return The string that contains the execution of this collection of command line argument.
  std::string print_execution() const {
    std::string printed_execution_arguments;
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (!printed_execution_arguments.empty()) {
              printed_execution_arguments += " ";
            }
            printed_execution_arguments += argument.execution();
          }());
        },
        arguments_);
    if (printed_execution_arguments.empty()) {
      return executable_path_.string();
    }
    return executable_path_.string() + " " + printed_execution_arguments;
  }

  /// @brief Prints the usage command of this collection of command line argument as a string.
  /// @return The string that contains the usage command of this collection of command line
  /// argument.
  [[nodiscard]] std::string print_usage_command() const {
    std::string result;
    result += executable_path_.filename().string();
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == lector::Importance::Required) {
              result += " " + argument.longest_key_with_value_type();
            }
          }());
        },
        arguments_);
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == lector::Importance::Optional) {
              result += " [" + argument.longest_key_with_value_type() + "]";
            }
          }());
        },
        arguments_);
    return result;
  }

  /// @brief Prints the usage options of this collection of command line argument as a string.
  /// @return The string that contains the usage options of this collection of command line
  /// argument.
  [[nodiscard]] std::string print_usage_options() const {
    std::ostringstream stream;
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == lector::Importance::Required) {
              stream << argument.keys_with_value_types() << "  " << argument.description()
                     << std::endl;
            }
          }());
        },
        arguments_);
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == lector::Importance::Optional) {
              stream << "[" << argument.keys_with_value_types() << "]  " << argument.description()
                     << std::endl;
            }
          }());
        },
        arguments_);
    return stream.str();
  }

private:
  /// @brief Parses the executable path from argc and argv. Called by the lector::Arguments::parse
  /// method.
  /// @param[in] argc The number of command line arguments, including the executable path.
  /// @param[in] argv The array of C-strings that represents the command line arguments, starting
  /// with the executable path.
  void parse_executable_path(const int argc, char* argv[]) {
    if (argc > 0) {
      executable_path_ = argv[0];
    }
  }

  /// @brief Parses the command line arguments from argc and argv, except for the executable path.
  /// Starts at the second argument in argv. Called by the lector::Arguments::parse method.
  /// @param[in] argc The number of command line arguments, including the executable path.
  /// @param[in] argv The array of C-strings that represents the command line arguments, starting
  /// with the executable path.
  /// @throws std::invalid_argument if an invalid, unknown, duplicated, or missing argument is
  /// encountered.
  void parse_arguments(const int argc, char* argv[]) {
    for (std::size_t index{1}; index < static_cast<std::size_t>(argc); ++index) {
      std::string key{argv[index]};
      bool matched{false};
      std::apply(
          [&](auto&... argument) {
            (..., [&]() {
              if (matched) {
                // This key has already been matched to an argument. Skip all remaining arguments.
                return;
              }
              if (std::find(argument.keys().begin(), argument.keys().end(), key)
                  != argument.keys().cend()) {
                if (argument.parsed_value().has_value()) {
                  throw std::invalid_argument(
                      "Duplicated argument '" + argument.longest_key_with_value_type() + "'.");
                }
                matched = true;
                using Type = typename std::decay_t<decltype(argument)>::ValueType;
                if constexpr (std::is_same_v<Type, bool>) {
                  // A boolean argument's value is indicated by its presence.
                  argument.set_parsed_value(true);
                } else {
                  if (index + 1 < static_cast<std::size_t>(argc)) {
                    ++index;
                    const std::string raw_value{argv[index]};
                    const auto parsed_value{lector::parse<Type>(raw_value)};
                    if (parsed_value.has_value()) {
                      argument.set_parsed_value(parsed_value.value());
                    } else {
                      throw std::invalid_argument("Invalid value '" + raw_value + "' for argument '"
                                                  + argument.longest_key_with_value_type() + "'.");
                    }
                  } else {
                    throw std::invalid_argument("Missing value for argument '"
                                                + argument.longest_key_with_value_type() + "'.");
                  }
                }
              }
            }());
          },
          arguments_);
      if (!matched) {
        throw std::invalid_argument("Unknown argument '" + key + "'.");
      }
    }
  }

  /// @brief Validates that all required arguments have each successfully parsed a value from argc
  /// and argv. Called by the lector::Arguments::parse method.
  /// @throws std::invalid_argument if one or more required arguments are missing a parsed value.
  void validate_all_required_arguments_have_parsed_values() const {
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == lector::Importance::Required
                && !argument.parsed_value().has_value()) {
              throw std::invalid_argument(
                  "Missing required argument '" + argument.longest_key_with_value_type() + "'.");
            }
          }());
        },
        arguments_);
  }

  /// @brief Variadic collection of command line arguments.
  std::tuple<ArgumentTypes...> arguments_;

  /// @brief Executable path of this collection of command line arguments. If the command line
  /// arguments have not yet been parsed from argc and argv, this path is empty.
  std::filesystem::path executable_path_;
};

}  // namespace lector

#endif  // LECTOR_HPP
