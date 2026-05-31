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
#include <vector>

namespace args {

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
inline constexpr std::array<args::Printing<Type>, 0> Printings{};

/// \brief Map of spellings to their corresponding values of a specified type.
/// @tparam Type The type of the values.
template <typename Type>
inline constexpr std::array<args::Spelling<Type>, 0> Spellings{};

/// @brief Prints a value of a specified enumeration type. The enumeration type must define a
/// specialization of the args::Printings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] value The enumeration value to print.
/// @return The string view representing the printed value.
/// @throws std::invalid_argument if the value is not a valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] constexpr std::string_view print_enumeration(const EnumerationType value) {
  for (const auto& [enumeration_value, printing] : args::Printings<EnumerationType>) {
    if (enumeration_value == value) {
      return printing;
    }
  }
  return std::string_view{};
}

/// @brief Parses a string view into an enumeration value. The enumeration type must define a
/// specialization of the args::Spellings constant for its type.
/// @tparam EnumerationType The enumeration type.
/// @param[in] text The string view to parse.
/// @return The parsed enumeration value, or std::nullopt if the string view could not be parsed
/// into a valid enumeration value.
template <typename EnumerationType>
[[nodiscard]] constexpr std::optional<EnumerationType> parse_enumeration(
    const std::string_view text) {
  for (const auto& [spelling, value] : args::Spellings<EnumerationType>) {
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

/// @brief Specialization of the args::Printings constant for the args::Importance enumeration.
template <>
inline constexpr std::array<args::Printing<args::Importance>, 2> Printings<args::Importance>{
  {
   {args::Importance::Optional, "Optional"},
   {args::Importance::Required, "Required"},
   }
};

/// @brief Specialization of the args::Spellings constant for the args::Importance enumeration.
template <>
inline constexpr std::array<args::Spelling<args::Importance>, 6> Spellings<args::Importance>{
  {
   {"Optional", args::Importance::Optional},
   {"Required", args::Importance::Required},
   {"optional", args::Importance::Optional},
   {"required", args::Importance::Required},
   {"OPTIONAL", args::Importance::Optional},
   {"REQUIRED", args::Importance::Required},
   }
};

/// @brief Parses a string view into a value of a specific type. If the type is an enumeration type,
/// it must define a specialization of the args::Spellings constant for its type.
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
/// it must define a specialization of the args::Spellings constant for its type.
/// @tparam Type The type of the value in which to parse the string view.
/// @param[in] text The string view to parse.
/// @return The parsed value, or std::nullopt if the string view could not be parsed into a valid
/// value.
template <typename Type>
[[nodiscard]] std::optional<Type> parse(const std::string_view text) {
  if constexpr (std::is_enum_v<Type>) {
    return args::parse_enumeration<Type>(text);
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
        std::is_same_v<Type, bool> ? args::Importance::Optional : args::Importance::Required} {
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
    : keys_{keys}, description_{description}, importance_{args::Importance::Optional},
      default_value_{default_value} {
    validate_keys();
    validate_description();
    validate_default_value();
  }

  /// @brief Destructor. Destroys this command line argument.
  ~Argument() noexcept = default;

  /// @brief Copy constructor. Constructs this command line argument by copying another one.
  Argument(const Argument&) = default;

  /// @brief Copy assignment operator. Assigns this command line argument by copying another one.
  /// @return This command line argument after the assignment.
  Argument& operator=(const Argument&) = default;

  /// @brief Move constructor. Constructs this command line argument by moving another one.
  Argument(Argument&&) noexcept = default;

  /// @brief Move assignment operator. Assigns this command line argument by moving another one.
  /// @return This command line argument after the assignment.
  Argument& operator=(Argument&&) noexcept = default;

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

  /// @brief Importance of this command line argument. Required arguments must be provided by the
  /// user, while optional arguments may or may not be provided by the user. Set at construction.
  /// @return The importance of this command line argument.
  [[nodiscard]] args::Importance importance() const noexcept {
    return importance_;
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

  /// @brief Value of this command line argument. Returns the parsed value if it exists; otherwise,
  /// returns the default value.
  /// @return The value of this command line argument.
  [[nodiscard]] const Type& parsed_or_default_value() const {
    if (parsed_value_.has_value()) {
      return parsed_value_.value();
    }
    if (default_value_.has_value()) {
      return default_value_.value();
    }
    throw std::logic_error("No parsed or default value.");
  }

  /// @brief Sets the parsed value of this command line argument.
  /// @param[in] value The parsed value to set.
  void set(const Type& value) {
    parsed_value_ = value;
  }

  /// @brief Prints the keys and value types of this command line argument as a string.
  /// @return The string that contains the keys and value types.
  std::string print_keys_and_value_types() const {
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
  void validate_keys() const {
    if (keys_.empty()) {
      throw std::invalid_argument("An argument must have at least one key.");
    }
    for (const std::string& key : keys_) {
      if (key.empty()) {
        throw std::invalid_argument("An argument cannot have an empty key.");
      }
    }
    for (std::size_t first{0}; first < keys_.size(); ++first) {
      for (std::size_t second{first + 1}; second < keys_.size(); ++second) {
        if (keys_[first] == keys_[second]) {
          throw std::invalid_argument("An argument cannot have duplicate keys. The key '"
                                      + keys_[first] + "' is duplicated.");
        }
      }
    }
  };

  /// @brief Validates the description of this command line argument. Called by both constructors.
  void validate_description() const {
    if (description_.empty()) {
      throw std::invalid_argument("An argument cannot have an empty description.");
    }
  };

  /// @brief Validates the default value of this command line argument. Called by the constructor
  /// that specifies a default value.
  constexpr void validate_default_value() const {
    if constexpr (std::is_same_v<Type, bool>) {
      throw std::invalid_argument("A boolean argument cannot have a default value.");
    }
  }

  /// @brief Prints the value type of this command line argument as a string.
  /// @return The string that contains the value type.
  std::string print_value_type() const {
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

  /// @brief Keys that can be used to specify this command line argument. Set at construction.
  std::vector<std::string> keys_;

  /// @brief Description of this command line argument. Used when printing usage information. Set at
  /// construction.
  std::string description_;

  /// @brief Importance of this command line argument. Required arguments must be provided by the
  /// user, while optional arguments may or may not be provided by the user. Set at construction.
  args::Importance importance_{args::Importance::Required};

  /// @brief Default value of this command line argument. Only relevant for optional non-boolean
  /// arguments. Set at construction.
  std::optional<Type> default_value_;

  /// @brief Parsed value of this command line argument. Set when this command line argument is
  /// parsed.
  std::optional<Type> parsed_value_;
};

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

template <typename... ArgumentTypes>
class Arguments {
public:
  // Move the variadic arguments into our tuple.
  explicit Arguments(ArgumentTypes... arguments) : arguments_(std::move(arguments)...) {}

  ~Arguments() noexcept = default;

  Arguments(const Arguments&) = default;

  Arguments& operator=(const Arguments&) = default;

  Arguments(Arguments&&) noexcept = default;

  Arguments& operator=(Arguments&&) noexcept = default;

  [[nodiscard]] const std::filesystem::path& executable_path() const {
    return executable_path_;
  }

  // Type-safe getter using the compile-time label.
  template <auto Label>
  [[nodiscard]] const auto& get() const {
    using Type = typename args::FindArgumentByLabel<Label, ArgumentTypes...>::type;
    return std::get<Type>(arguments_);
  }

  void print_execution() const {
    std::cout << "Execution: ";
    for (const std::string& argument : raw_arguments_) {
      std::cout << argument << " ";
    }
    std::cout << std::endl;
  }

  [[nodiscard]] std::string print_usage_information() const {
    std::ostringstream stream;
    stream << "Usage: " << executable_path_.filename().string() << " [options]" << std::endl
           << "Options:" << std::endl;

    // Fold expression to iterate over the tuple elements.
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            stream << "  ";
            for (std::size_t index{0}; index < argument.keys().size(); ++index) {
              stream << argument.keys()[index] << (index + 1 < argument.keys().size() ? ", " : "");
            }

            // If the parameter takes a value (isn't a pure bool flag), hint it.
            if constexpr (
                !std::is_same_v<typename std::decay_t<decltype(argument)>::ValueType, bool>) {
              stream << " <value>";
            }

            stream << std::endl << "      " << argument.description();
            if (argument.importance() == args::Importance::Required) {
              stream << " [Required]";
            } else if (argument.default_value()) {
              stream << " [Optional]";
            }
            stream << std::endl;
          }());
        },
        arguments_);

    return stream.str();
  }

  bool parse(const int argc, char* argv[]) {
    if (argc > 0) {
      executable_path_ = argv[0];
    }

    for (std::size_t index{0}; index < static_cast<std::size_t>(argc); ++index) {
      raw_arguments_.emplace_back(argv[index]);
    }

    bool success{true};

    for (std::size_t index{1}; index < static_cast<std::size_t>(argc); ++index) {
      std::string current_arg{argv[index]};
      bool matched{false};

      std::apply(
          [&](auto&... argument) {
            // Fold expression trick to simulate iterating and short-circuiting over the tuple.
            (..., [&]() {
              if (matched) {
                // Skip if we already matched this token.
                return;
              }

              if (std::find(argument.keys().begin(), argument.keys().end(), current_arg)
                  != argument.keys().end()) {
                matched = true;

                using Type = typename std::decay_t<decltype(argument)>::ValueType;

                // If it's a bool, its presence implies true (no trailing value required).
                if constexpr (std::is_same_v<Type, bool>) {
                  argument.set(true);
                } else {
                  if (index + 1 < static_cast<std::size_t>(argc)) {
                    const std::string value_str{argv[++index]};
                    const auto parsed{args::parse<Type>(value_str)};
                    if (parsed) {
                      argument.set(parsed.value());
                    } else {
                      std::cerr << "Error: Failed to parse value '" << value_str
                                << "' for argument '" << current_arg << "'." << std::endl;
                      success = false;
                    }
                  } else {
                    std::cerr << "Error: Missing value for argument '" << current_arg << "'."
                              << std::endl;
                    success = false;
                  }
                }
              }
            }());
          },
          arguments_);

      if (!matched) {
        std::cerr << "Error: Unknown argument '" << current_arg << "'." << std::endl;
        success = false;
      }
    }

    // Check if all required arguments were provided.
    std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == args::Importance::Required
                && !argument.parsed_value().has_value()) {
              std::cerr << "Error: Missing required argument '" << argument.keys().front() << "'."
                        << std::endl;
              success = false;
            }
          }());
        },
        arguments_);

    return success;
  }

private:
  std::filesystem::path executable_path_;

  std::tuple<ArgumentTypes...> arguments_;

  std::vector<std::string> raw_arguments_;
};

}  // namespace args

#endif  // ARGS_HPP
