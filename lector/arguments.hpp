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

#ifndef LECTOR_ARGUMENTS_HPP
#define LECTOR_ARGUMENTS_HPP

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
#include <unordered_set>
#include <utility>
#include <vector>

#include "lector/parse.hpp"
#include "lector/print.hpp"

namespace lector {

/// @brief Importance of a command line argument.
enum class Importance : ::std::int8_t {
  /// @brief The command line argument is optional; it may or may not be provided by the user.
  Optional = 0,

  /// @brief The command line argument is required; it must be provided by the user.
  Required = 1,
};

/// @brief Specialization of the lector::Names constant for the lector::Importance enumeration.
template <>
inline constexpr ::std::array<::lector::Name<::lector::Importance>, 2> Names<::lector::Importance>{
  {
   {::lector::Importance::Optional, "Optional"},
   {::lector::Importance::Required, "Required"},
   }
};

/// @brief Specialization of the lector::Spellings constant for the lector::Importance enumeration.
template <>
inline constexpr ::std::array<::lector::Spelling<::lector::Importance>, 6>
    Spellings<::lector::Importance>{
      {
       {"Optional", ::lector::Importance::Optional},
       {"Required", ::lector::Importance::Required},
       {"optional", ::lector::Importance::Optional},
       {"required", ::lector::Importance::Required},
       {"OPTIONAL", ::lector::Importance::Optional},
       {"REQUIRED", ::lector::Importance::Required},
       }
};

/// @brief A command line argument, including its label, keys, description, importance, default
/// value, and parsed value.
/// @tparam LabelValue Value of this command line argument's label. The label is used to uniquely
/// identify the command line argument.
/// @tparam Type The type of the command line argument.
template <auto LabelValue, typename Type>
class Argument final {
public:
  using ValueType = Type;

  /// @brief Default constructor. Initializes the command line argument with no keys, an empty
  /// description, required importance, and no default value.
  Argument() noexcept = default;

  /// @brief Constructor for a required command line argument or a boolean command line argument. No
  /// default value is needed. Boolean command line arguments are always optional and always default
  /// to false.
  /// @param[in] keys The keys that can be used to specify the command line argument.
  /// @param[in] description The description of the command line argument. Used when printing usage
  /// information. Set at construction.
  /// @throws std::invalid_argument if any of the parameters are invalid.
  Argument(const ::std::initializer_list<::std::string>& keys, const ::std::string& description)
    : keys_{keys}, description_{description},
      importance_{::std::is_same_v<Type, bool> ? ::lector::Importance::Optional :
                                                 ::lector::Importance::Required} {
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
  Argument(const ::std::initializer_list<::std::string>& keys, const ::std::string& description,
           const Type& default_value)
    : keys_{keys}, description_{description}, default_value_{default_value},
      importance_{::lector::Importance::Optional} {
    validate_keys();
    validate_description();
    validate_default_value();
  }

  /// @brief Destructor. Destroys this command line argument.
  ~Argument() noexcept = default;

  /// @brief Copy constructor. Constructs a command line argument by copying another one.
  Argument(const ::lector::Argument<LabelValue, Type>&) = default;

  /// @brief Copy assignment operator. Assigns this command line argument by copying another one.
  /// @return This command line argument after the assignment.
  ::lector::Argument<LabelValue, Type>& operator=(
      const ::lector::Argument<LabelValue, Type>&) = default;

  /// @brief Move constructor. Constructs a command line argument by moving another one.
  Argument(::lector::Argument<LabelValue, Type>&&) noexcept = default;

  /// @brief Move assignment operator. Assigns this command line argument by moving another one.
  /// @return This command line argument after the assignment.
  ::lector::Argument<LabelValue, Type>& operator=(
      ::lector::Argument<LabelValue, Type>&&) noexcept = default;

  /// @brief Label of this command line argument. Used to uniquely identify this command line
  /// argument. Set at construction.
  /// @return The label of this command line argument.
  [[nodiscard]] static constexpr auto label() noexcept {
    return LabelValue;
  }

  /// @brief Keys that can be used to specify this command line argument. Set at construction.
  /// @return The keys that can be used to specify this command line argument.
  [[nodiscard]] const ::std::vector<::std::string>& keys() const noexcept {
    return keys_;
  }

  /// @brief Description of this command line argument. Used when printing usage information. Set at
  /// construction.
  /// @return The description of this command line argument.
  [[nodiscard]] ::std::string_view description() const noexcept {
    return description_;
  }

  /// @brief Default value of this command line argument. Only relevant for optional non-boolean
  /// arguments. Set at construction.
  /// @return The default value of this command line argument.
  [[nodiscard]] const ::std::optional<Type>& default_value() const noexcept {
    return default_value_;
  }

  /// @brief Parsed value of this command line argument. Set when this command line argument is
  /// parsed.
  /// @return The parsed value of this command line argument.
  [[nodiscard]] const ::std::optional<Type>& parsed_value() const noexcept {
    return parsed_value_;
  }

  /// @brief Importance of this command line argument. Required arguments must be provided by the
  /// user, while optional arguments may or may not be provided by the user. Set at construction.
  /// @return The importance of this command line argument.
  [[nodiscard]] ::lector::Importance importance() const noexcept {
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
    throw ::std::logic_error(
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
  [[nodiscard]] ::std::string longest_key_with_value_type() const {
    ::std::string result{longest_key()};
    const ::std::string type{value_type()};
    if (!type.empty()) {
      result += " " + type;
    }
    return result;
  }

  /// @brief Prints the keys of this command line argument with their associated value types as a
  /// string.
  /// @return The string that contains the keys and value types.
  [[nodiscard]] ::std::string keys_with_value_types() const {
    ::std::string result;
    for (::std::size_t index{0}; index < keys_.size(); ++index) {
      const ::std::string type{value_type()};
      result += keys_[index];
      if (!type.empty()) {
        result += " " + type;
      }
      if (index + 1 < keys_.size()) {
        result += ", ";
      }
    }
    return result;
  }

  /// @brief Prints the execution of this command line argument as a string.
  /// @return The string that contains the execution of this command line argument.
  [[nodiscard]] ::std::string execution() const {
    if constexpr (::std::is_same_v<Type, bool>) {
      if (parsed_value_.has_value() && parsed_value_.value()) {
        return longest_key();
      }
      return "";
    } else {
      if (parsed_value_.has_value()) {
        return longest_key() + " " + ::lector::print<Type>(parsed_value_.value());
      }
      return "";
    }
  }

private:
  /// @brief If this command line argument is a boolean argument, sets its default value to false.
  /// Called by the constructor that does not specify a default value. Boolean arguments are always
  /// optional and always default to false.
  void set_boolean_default() {
    if constexpr (::std::is_same_v<Type, bool>) {
      default_value_ = false;
    }
  }

  /// @brief Validates the keys of this command line argument. Called by both constructors.
  /// @throws std::logic_error if this command line argument has no keys or if any of its keys are
  /// invalid.
  void validate_keys() const {
    if (keys_.empty()) {
      throw ::std::logic_error("All arguments must each have at least one key.");
    }
    for (const ::std::string& key : keys_) {
      if (key.empty()) {
        if (longest_key().empty()) {
          throw ::std::logic_error("Arguments cannot have empty keys.");
        }
        throw ::std::logic_error("Empty key in argument '" + longest_key_with_value_type()
                                 + "'. Arguments cannot have empty keys.");
      }
    }
    const ::std::size_t keys_size{keys_.size()};
    for (::std::size_t first{0}; first < keys_size; ++first) {
      for (::std::size_t second{first + 1}; second < keys_size; ++second) {
        if (keys_[first] == keys_[second]) {
          throw ::std::logic_error(
              "Duplicated key '" + keys_[first] + "' in argument '" + longest_key_with_value_type()
              + "'. Arguments cannot have duplicate keys.");
        }
      }
    }
  };

  /// @brief Validates the description of this command line argument. Called by both constructors.
  /// @throws std::logic_error if this command line argument's description is empty.
  void validate_description() const {
    if (description_.empty()) {
      throw ::std::logic_error("Empty description in argument '" + longest_key_with_value_type()
                               + "'. All arguments must have descriptions.");
    }
  };

  /// @brief Validates the default value of this command line argument. Called by the constructor
  /// that specifies a default value.
  /// @throws std::logic_error if this command line argument is boolean but specifies a default
  /// value.
  constexpr void validate_default_value() const {
    if constexpr (::std::is_same_v<Type, bool>) {
      throw ::std::logic_error(
          "Specified default value for boolean argument '" + longest_key_with_value_type()
          + "'. Boolean arguments are always false by default and cannot specify default values.");
    }
  }

  /// @brief Prints the value type of this command line argument as a string.
  /// @return The string that contains the value type.
  [[nodiscard]] constexpr ::std::string_view value_type() const {
    if constexpr (::std::is_same_v<Type, bool>) {
      return "";
    } else if constexpr (::std::numeric_limits<Type>::is_integer) {
      return "<number>";
    } else if constexpr (::std::is_floating_point_v<Type>) {
      return "<value>";
    } else if constexpr (
        ::std::is_same_v<Type, ::std::string> || ::std::is_same_v<Type, ::std::string_view>) {
      return "<text>";
    } else if constexpr (::std::is_same_v<Type, ::std::filesystem::path>) {
      return "<path>";
    } else {
      return "<value>";
    }
  }

  /// @brief Returns the longest key of this command line argument.
  /// @return The longest key of this command line argument.
  /// @throws std::logic_error if this command line argument has no keys.
  [[nodiscard]] const ::std::string& longest_key() const {
    if (keys_.empty()) {
      throw ::std::logic_error("All arguments must each have at least one key.");
    }
    ::std::size_t longest_key_index{0};
    for (::std::size_t index{1}; index < keys_.size(); ++index) {
      if (keys_[index].size() > keys_[longest_key_index].size()) {
        longest_key_index = index;
      }
    }
    return keys_[longest_key_index];
  }

  /// @brief Keys that can be used to specify this command line argument. Set at construction.
  ::std::vector<::std::string> keys_;

  /// @brief Description of this command line argument. Used when printing usage information. Set at
  /// construction.
  ::std::string description_;

  /// @brief Default value of this command line argument. Only relevant for optional non-boolean
  /// arguments. Set at construction.
  ::std::optional<Type> default_value_;

  /// @brief Parsed value of this command line argument. Set when this command line argument is
  /// parsed.
  ::std::optional<Type> parsed_value_;

  /// @brief Importance of this command line argument. Required arguments must be provided by the
  /// user, while optional arguments may or may not be provided by the user. Set at construction.
  ::lector::Importance importance_{::lector::Importance::Required};
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
struct FindArgumentByLabel<Label, ::lector::Argument<Label, Type>, OtherArgumentTypes...> {
  using type = ::lector::Argument<Label, Type>;
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
struct FindArgumentByLabel<Label, ::lector::Argument<OtherLabel, OtherType>,
                           RemainingArgumentTypes...>
    final {
  using type = typename ::lector::FindArgumentByLabel<Label, RemainingArgumentTypes...>::type;
};

/// @brief Data structure that validates at compilation time that a specified variadic list of types
/// are unique. Base data structure that contains an empty list of types and returns true.
/// @tparam ...Types Variadic list of types to check for uniqueness.
template <auto... Types>
struct AreUnique : std::true_type {};

/// @brief Data structure that validates at compilation time that a specified variadic list of types
/// are unique. Recursively compares a first type against the remaining variadic list of types.
/// @tparam FirstType The first type to compare.
/// @tparam ...RemainingTypes The remaining types in the variadic list of types to compare.
template <auto FirstType, auto... RemainingTypes>
struct AreUnique<FirstType, RemainingTypes...>
  : std::bool_constant<((FirstType != RemainingTypes) && ...)
                       && AreUnique<RemainingTypes...>::value> {};

/// @brief A collection of command line arguments that can be parsed from argc and argv.
/// @tparam ...ArgumentTypes Variadic list of the types of the command line arguments in this
/// collection.
template <typename... ArgumentTypes>
class Arguments final {
public:
  /// @brief Compile-time check that all arguments have unique labels.
  static_assert(AreUnique<ArgumentTypes::label()...>::value,
                "Duplicate argument labels detected. Each argument must have a unique label.");

  /// @brief Constructor. Constructs a collection of command line arguments by moving a variadic
  /// list of command line arguments.
  /// @param[in] ...arguments The variadic list of command line arguments.
  explicit Arguments(ArgumentTypes... arguments) : arguments_{::std::move(arguments)...} {
    validate_keys();
  }

  /// @brief Destructor. Destroys this collection of command line arguments.
  ~Arguments() noexcept = default;

  /// @brief Copy constructor. Constructs a collection of command line argument by copying another
  /// one.
  Arguments(const ::lector::Arguments<ArgumentTypes...>&) = default;

  /// @brief Copy assignment operator. Assigns this collection of command line argument by copying
  /// another one.
  /// @return This collection of command line argument after the assignment.
  ::lector::Arguments<ArgumentTypes...>& operator=(
      const ::lector::Arguments<ArgumentTypes...>&) = default;

  /// @brief Move constructor. Constructs a collection of command line argument by moving another
  /// one.
  Arguments(::lector::Arguments<ArgumentTypes...>&&) noexcept = default;

  /// @brief Move assignment operator. Assigns this collection of command line argument by moving
  /// another one.
  /// @return This collection of command line argument after the assignment.
  ::lector::Arguments<ArgumentTypes...>& operator=(
      ::lector::Arguments<ArgumentTypes...>&&) noexcept = default;

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
  [[nodiscard]] const ::std::filesystem::path& executable_path() const {
    return executable_path_;
  }

  /// @brief Returns a specified command line argument from this collection.
  /// @tparam Label The label of the command line argument to return.
  /// @return The specified command line argument.
  template <auto Label>
  [[nodiscard]] const auto& get() const {
    using Type = typename ::lector::FindArgumentByLabel<Label, ArgumentTypes...>::type;
    return ::std::get<Type>(arguments_);
  }

  /// @brief Prints the execution of this collection of command line argument as a string.
  /// @return The string that contains the execution of this collection of command line argument.
  [[nodiscard]] ::std::string execution() const {
    ::std::string printed_execution_arguments;
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            const std::string argument_execution{argument.execution()};
            if (!printed_execution_arguments.empty() && !argument_execution.empty()) {
              printed_execution_arguments += " ";
            }
            printed_execution_arguments += argument_execution;
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
  [[nodiscard]] ::std::string usage_command() const {
    ::std::string result;
    result += executable_path_.filename().string();
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == ::lector::Importance::Required) {
              result += " " + argument.longest_key_with_value_type();
            }
          }());
        },
        arguments_);
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == ::lector::Importance::Optional) {
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
  [[nodiscard]] ::std::string usage_options() const {
    ::std::ostringstream stream;
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == ::lector::Importance::Required) {
              stream << argument.keys_with_value_types() << "  " << argument.description()
                     << ::std::endl;
            }
          }());
        },
        arguments_);
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == ::lector::Importance::Optional) {
              stream << "[" << argument.keys_with_value_types() << "]  " << argument.description()
                     << ::std::endl;
            }
          }());
        },
        arguments_);
    return stream.str();
  }

private:
  /// @brief The best matching argument for a command line argument token during parsing. The best
  /// matching argument is the argument with the longest matching key, and if there are multiple
  /// arguments with keys of the same length that match, then the best matching argument is the one
  /// that is matched by a non-inline key rather than an inline key. Used to avoid shadowing when
  /// multiple arguments have keys that are prefixes of each other, and to prefer non-inline matches
  /// over inline matches when the key lengths are equal.
  struct BestArgument final {
    /// @brief Index of this argument in the tuple of arguments. Used to identify this argument
    /// during parsing.
    ::std::size_t index{0};

    /// @brief Length of the longest matching key for this argument. Used to avoid shadowing when
    /// multiple arguments have keys that are prefixes of each other. For example, if one argument
    /// has the key "key" and another argument has the key "key_long", then the argument with the
    /// key "key_long" should be matched for the command line argument "key_long=value", not the
    /// argument with the key "key".
    ::std::size_t key_length{0};

    /// @brief Whether this argument was matched by an inline key of the form "key=value" rather
    /// than a whitespace-separated key-value pair of the form "key value". Used to prefer
    /// non-inline matches over inline matches when the key lengths are equal. For example, if one
    /// argument has the key "key" and another argument has the key "key_long", then the argument
    /// with the key "key" should be matched for the command line argument "key=value", not the
    /// argument with the key "key_long".
    bool is_inline{false};
  };

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
    const ::std::size_t count{static_cast<::std::size_t>(argc)};
    for (::std::size_t argv_index{1}; argv_index < count; ++argv_index) {
      const ::std::string_view token{argv[argv_index]};
      const BestArgument best_argument{find_best_argument(token)};
      ::std::size_t current_index{0};
      ::std::apply(
          [&](auto&... argument) {
            (..., [&]() {
              if (current_index == best_argument.index) {
                populate_argument(argument, best_argument, argc, argv, argv_index);
              }
              ++current_index;
            }());
          },
          arguments_);
    }
  }

  /// @brief Finds the best matching argument for a command line argument token.
  /// @param[in] token The command line argument token to match.
  /// @return The best matching argument.
  /// @throws std::invalid_argument if an unknown argument is encountered.
  [[nodiscard]] BestArgument find_best_argument(const ::std::string_view token) const {
    ::std::optional<BestArgument> best;
    ::std::size_t argument_index{0};
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            for (const ::std::string& argument_key : argument.keys()) {
              const ::std::optional<BestArgument> exact_match{
                try_exact_match(token, argument_index, argument_key)};
              if (exact_match.has_value()) {
                if (!best.has_value() || exact_match->key_length > best->key_length
                    || (exact_match->key_length == best->key_length && best->is_inline)) {
                  best = exact_match;
                }
                continue;
              }
              const ::std::optional<BestArgument> inline_match{
                try_inline_match<decltype(argument)>(token, argument_index, argument_key)};
              if (inline_match.has_value()
                  && (!best.has_value() || inline_match->key_length > best->key_length)) {
                best = inline_match;
              }
            }
            ++argument_index;
          }());
        },
        arguments_);
    if (!best.has_value()) {
      throw ::std::invalid_argument("Unknown argument '" + ::std::string{token} + "'.");
    }
    return best.value();
  }

  /// @brief Checks whether a token is the exact match of an argument's key. Called by
  /// lector::Arguments::find_best_argument().
  /// @param[in] token The token to check.
  /// @param[in] argument_index The index of the argument that has the key.
  /// @param[in] argument_key The argument key against which to compare.
  /// @return A populated lector::Arguments::BestArgument data structure if the specified token is
  /// an exact match for the key, or std::nullopt otherwise.
  [[nodiscard]] static ::std::optional<BestArgument> try_exact_match(
      const ::std::string_view token, const ::std::size_t argument_index,
      const ::std::string_view argument_key) noexcept {
    if (token == argument_key) {
      return BestArgument{argument_index, argument_key.size(), false};
    }
    return ::std::nullopt;
  }

  /// @brief Checks whether a token contains an inline match of an argument's key of the form
  /// "key=value". Called by lector::Arguments::find_best_argument().
  /// @tparam Argument The type of the argument that has the key to be used in the comparison.
  /// @param[in] token The token to check.
  /// @param[in] argument_index The index of the argument that has the key to be used in the
  /// comparison.
  /// @param[in] argument_key The argument key against which to compare.
  /// @return A populated lector::Arguments::BestArgument data structure if the specified token
  /// contains an inline match for the key, or std::nullopt otherwise.
  template <typename Argument>
  [[nodiscard]] static ::std::optional<BestArgument> try_inline_match(
      const ::std::string_view token, const ::std::size_t argument_index,
      const ::std::string_view argument_key) noexcept {
    using ArgumentType = typename ::std::decay_t<Argument>::ValueType;
    // Inline matching is strictly disabled for boolean arguments because they are key-only flags
    // that do not have values.
    if constexpr (!::std::is_same_v<ArgumentType, bool>) {
      if (token.size() > argument_key.size()
          && token.compare(0, argument_key.size(), argument_key) == 0
          && token[argument_key.size()] == '=') {
        return BestArgument{argument_index, argument_key.size(), true};
      }
    }
    return ::std::nullopt;
  }

  /// @brief Populates an argument with its parsed value. Called by
  /// lector::Arguments::parse_arguments().
  /// @tparam Argument The type of the argument to be populated.
  /// @param[in,out] argument The argument to be populated.
  /// @param[in] best_argument The lector::Arguments::BestArgument data structure that corresponds
  /// to the argument to be populated.
  /// @param[in] argc The number of command line arguments, including the executable path.
  /// @param[in] argv The array of C-strings that represents the command line arguments, starting
  /// with the executable path.
  /// @param[in,out] argv_index The index of the argument in the array of C-string command line
  /// arguments whose value is to be extracted and used to populate the argument.
  /// @throws std::invalid_argument if the parsed value is invalid for this argument type.
  template <typename Argument>
  static void populate_argument(Argument& argument, const BestArgument& best_argument,
                                const int argc, char* argv[], ::std::size_t& argv_index) {
    if (argument.parsed_value().has_value()) {
      throw ::std::invalid_argument(
          "Duplicated argument '" + argument.longest_key_with_value_type() + "'.");
    }
    using Type = typename ::std::decay_t<Argument>::ValueType;
    if constexpr (::std::is_same_v<Type, bool>) {
      // Boolean arguments are key-only flags; their presence implies true.
      argument.set_parsed_value(true);
    } else {
      // This is a non-boolean argument. Extract and parse its value.
      const ::std::string raw_value{extract_raw_value(
          best_argument, argument.longest_key_with_value_type(), argc, argv, argv_index)};
      const ::std::optional<Type> parsed_value{::lector::parse<Type>(raw_value)};
      if (parsed_value.has_value()) {
        argument.set_parsed_value(parsed_value.value());
      } else {
        throw ::std::invalid_argument("Invalid value '" + raw_value + "' for argument '"
                                      + argument.longest_key_with_value_type() + "'.");
      }
    }
  }

  /// @brief Extracts the raw string value from an argv token for a non-boolean best argument.
  /// Called by lector::Arguments::populate_argument().
  /// @param[in] best_argument The best argument.
  /// @param[in] best_argument_longest_key_with_value_type The longest key with value type of the
  /// best argument.
  /// @param[in] argc The number of command line arguments, including the executable path.
  /// @param[in] argv The array of C-strings that represents the command line arguments, starting
  /// with the executable path.
  /// @param[in,out] argv_index The index of the argument in the array of C-string command line
  /// arguments whose raw string value is to be extracted.
  /// @return The extracted raw string value.
  /// @throws std::invalid_argument if the command line arguments are missing the value for this
  /// best argument.
  [[nodiscard]] static ::std::string extract_raw_value(
      const BestArgument& best_argument,
      const ::std::string& best_argument_longest_key_with_value_type, const int argc, char* argv[],
      ::std::size_t& argv_index) {
    const ::std::string_view token{argv[argv_index]};
    if (best_argument.is_inline) {
      // The token contains an inline value of the form "key=value". Extract the last portion of the
      // token.
      return ::std::string{token.substr(best_argument.key_length + 1)};
    }
    if (argv_index + 1 < static_cast<::std::size_t>(argc)) {
      // The token is a standalone value of the form "key", and the next token is of the form
      // "value". Advance the argv index to consume the next argv element that contains the value.
      ++argv_index;
      return ::std::string{argv[argv_index]};
    }
    throw ::std::invalid_argument(
        "Missing value for argument '" + best_argument_longest_key_with_value_type + "'.");
  }

  /// @brief Validates that the same key is never duplicated across two or more arguments. Called by
  /// the constructor.
  /// @throws std::logic_error if the same key is duplicated across two or more arguments.
  void validate_keys() const {
    ::std::unordered_set<::std::string> unique_keys;
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            for (const ::std::string& key : argument.keys()) {
              const ::std::pair<::std::unordered_set<::std::string>::const_iterator, bool> result{
                unique_keys.insert(key)};
              if (!result.second) {
                throw std::logic_error("Duplicate key '" + key + "' across two arguments.");
              }
            }
          }());
        },
        arguments_);
  }

  /// @brief Validates that all required arguments have each successfully parsed a value from argc
  /// and argv. Called by the lector::Arguments::parse method.
  /// @throws std::invalid_argument if one or more required arguments are missing a parsed value.
  void validate_all_required_arguments_have_parsed_values() const {
    ::std::apply(
        [&](const auto&... argument) {
          (..., [&]() {
            if (argument.importance() == ::lector::Importance::Required
                && !argument.parsed_value().has_value()) {
              throw ::std::invalid_argument(
                  "Missing required argument '" + argument.longest_key_with_value_type() + "'.");
            }
          }());
        },
        arguments_);
  }

  /// @brief Variadic collection of command line arguments.
  ::std::tuple<ArgumentTypes...> arguments_;

  /// @brief Executable path of this collection of command line arguments. If the command line
  /// arguments have not yet been parsed from argc and argv, this path is empty.
  ::std::filesystem::path executable_path_;
};

}  // namespace lector

#endif  // LECTOR_ARGUMENTS_HPP
