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

/// @brief Prints a value of a specified enumeration type. The enumeration type must define a
/// specialization of the lector::Names constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] value The enumeration value to print.
/// @return The string view representing the printed value.
template <typename EnumerationType>
[[nodiscard]] constexpr ::std::string_view print_enumeration(const EnumerationType value) {
  for (const auto& [enumeration_value, name] : ::lector::Names<EnumerationType>) {
    if (enumeration_value == value) {
      return name;
    }
  }
  return ::std::string_view{};
}

/// @brief Prints a value of a specific type as a string.
/// @tparam Type The type of the value to print.
/// @param[in] value The value to print.
/// @return The string that contains the printed value.
template <typename Type>
[[nodiscard]] ::std::string print(const Type& value);

/// @brief Prints a boolean value as a string.
/// @param[in] value The boolean value to print.
/// @return The string that contains the printed boolean value.
template <>
[[nodiscard]] inline ::std::string print(const bool& value) {
  if (value) {
    return "true";
  }
  return "false";
}

/// @brief Prints an 8-bit natural number as a string.
/// @param[in] value The 8-bit natural number to print.
/// @return The string that contains the printed 8-bit natural number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint8_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 16-bit natural number as a string.
/// @param[in] value The 16-bit natural number to print.
/// @return The string that contains the printed 16-bit natural number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint16_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 32-bit natural number as a string.
/// @param[in] value The 32-bit natural number to print.
/// @return The string that contains the printed 32-bit natural number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint32_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 64-bit natural number as a string.
/// @param[in] value The 64-bit natural number to print.
/// @return The string that contains the printed 64-bit natural number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::uint64_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints an 8-bit integer number as a string.
/// @param[in] value The 8-bit integer number to print.
/// @return The string that contains the printed 8-bit integer number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int8_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 16-bit integer number as a string.
/// @param[in] value The 16-bit integer number to print.
/// @return The string that contains the printed 16-bit integer number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int16_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 32-bit integer number as a string.
/// @param[in] value The 32-bit integer number to print.
/// @return The string that contains the printed 32-bit integer number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int32_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a 64-bit integer number as a string.
/// @param[in] value The 64-bit integer number to print.
/// @return The string that contains the printed 64-bit integer number.
template <>
[[nodiscard]] inline ::std::string print(const ::std::int64_t& value) {
  return ::std::to_string(value);
}

/// @brief Prints a single-precision floating-point number as a string.
/// @param[in] value The single-precision floating-point number to print.
/// @return The string that contains the printed single-precision floating-point number.
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

/// @brief Prints a double-precision floating-point number as a string.
/// @param[in] value The double-precision floating-point number to print.
/// @return The string that contains the printed double-precision floating-point number.
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

/// @brief Prints an extended-precision floating-point number as a string.
/// @param[in] value The extended-precision floating-point number to print.
/// @return The string that contains the printed extended-precision floating-point number.
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

/// @brief Prints a string as a string.
/// @param[in] value The string to print.
/// @return The string that contains the printed string.
template <>
[[nodiscard]] inline ::std::string print(const ::std::string& value) {
  return value;
}

/// @brief Prints a string view as a string.
/// @param[in] value The string view to print.
/// @return The string that contains the printed string view.
template <>
[[nodiscard]] inline ::std::string print(const ::std::string_view& value) {
  return ::std::string(value);
}

/// @brief Prints a filesystem path as a string.
/// @param[in] value The filesystem path to print.
/// @return The string that contains the printed filesystem path.
template <>
[[nodiscard]] inline ::std::string print(const ::std::filesystem::path& value) {
  return value.string();
}

/// @brief Prints a value of a specific type as a string. If the type is an enumeration type, it
/// must define a specialization of the lector::Names constant for its type.
/// @tparam Type The type of the value to print.
/// @param[in] value The value to print.
/// @return The string that contains the printed value.
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

}  // namespace lector

#endif  // LECTOR_PRINT_HPP
