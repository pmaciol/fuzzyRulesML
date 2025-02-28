#pragma once

#include "membership_functions.hpp"
#include <map>
#include <print>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include <format>

namespace fuzzyrulesml::rules {
using CrispValuesU = std::variant<double, std::string>;

inline auto to_string(const CrispValuesU& value_container) -> std::string {
  return std::visit([](const auto& var) { return std::format("{}", var); }, value_container);
}


namespace initial_distribution {
class Uniform {
public:
  Uniform(double min, double max, int categories) : min(min), max(max), categories(categories) {}
  auto get_categories() const -> int { return categories; }
  // auto get_membership(double value) const -> std::map<int, double> { return std::map<int, double>{{0, 1.0}}; }
  auto get_min() const -> double { return min; }
  auto get_max() const -> double { return max; }

private:
  double min;
  double max;
  int categories;
};
} // namespace initial_distribution
template <typename VARIABLE> class FuzzyValue;
using Membership = std::map<int, double>;

template <typename UNDERLYING_TYPE> class FuzzyVariable {
public:
  using UnderlyingType = UNDERLYING_TYPE;
  FuzzyVariable(std::string_view name, const initial_distribution::Uniform& distribution);
  ~FuzzyVariable() = default;
  FuzzyVariable(const FuzzyVariable& other) : name(other.name), distribution{other.distribution} {}
  FuzzyVariable(FuzzyVariable&& other) noexcept : name(std::move(other.name)), distribution(std::move(other.distribution)) {}

  auto operator=(const FuzzyVariable& other) -> FuzzyVariable&;
  auto operator=(FuzzyVariable&& other) noexcept -> FuzzyVariable&;
  auto operator()(const UnderlyingType& value) const -> FuzzyValue<FuzzyVariable<UnderlyingType>>;
  auto operator()(const CrispValuesU& value) const -> FuzzyValue<FuzzyVariable<UnderlyingType>>;
  auto operator==(const FuzzyVariable& other) const -> bool;
  auto operator==(std::string_view name) const -> bool;
  auto operator<(const FuzzyVariable& other) const -> bool;

  [[nodiscard]] auto get_name() const -> std::string { return name; }
  [[nodiscard]] auto size() const -> int { return distribution.get_categories(); }

  void set_points(const std::vector<double>& characteristic_points) { distribution.set_points(characteristic_points); };
  auto get_points() const -> std::vector<double> { return distribution.get_points(); };

private:
  std::string name;
  fuzzyrulesml::mfunct::LinearDistribution distribution;
};

template <typename VARIABLE> class FuzzyValue {
public:
  FuzzyValue(const Membership& membership) : membership(membership) {}
  auto get_membership() const -> Membership { return membership; }

private:
  typename VARIABLE::UnderlyingType value;
  Membership membership;

  // ...
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////////////////////////

inline auto make_linear_distribution(const initial_distribution::Uniform& distribution) {
  const auto number_of_internal_functions = distribution.get_categories() - 2;
  const auto section_length = (distribution.get_max() - distribution.get_min()) / (distribution.get_categories() - 1);
  const auto left =
      fuzzyrulesml::mfunct::LinearMemberFunctDesc{distribution.get_min(), distribution.get_min() + section_length};
  const auto right =
      fuzzyrulesml::mfunct::LinearMemberFunctAsc(distribution.get_max() - section_length, distribution.get_max());
  std::vector<fuzzyrulesml::mfunct::LinearMemberFunct> functions;
  for (int i = 0; i < number_of_internal_functions; i++) {
    functions.push_back(fuzzyrulesml::mfunct::LinearMemberFunct{distribution.get_min() + i * section_length,
                                                                distribution.get_min() + (i + 1) * section_length,
                                                                distribution.get_min() + (i + 2) * section_length});
    const auto& added = functions.back();
    std::print("Adding midfunction {} {} {} {}\n", section_length, added.get_left_bound(), added.get_peak(),
               added.get_right_bound());
  }
  return fuzzyrulesml::mfunct::LinearDistribution{fuzzyrulesml::mfunct::LinearDistribution{left, functions, right}};
};

template <typename UNDERLYING_TYPE>
FuzzyVariable<UNDERLYING_TYPE>::FuzzyVariable(std::string_view name, const initial_distribution::Uniform& distribution)
    : name(name), distribution(make_linear_distribution(distribution)){};

template <typename UNDERLYING_TYPE>
auto FuzzyVariable<UNDERLYING_TYPE>::operator()(const UnderlyingType& value) const
    -> FuzzyValue<FuzzyVariable<UNDERLYING_TYPE>> {
  return FuzzyValue<FuzzyVariable<UNDERLYING_TYPE>>(this->distribution(value));
}

template <>
inline auto FuzzyVariable<std::string>::operator()(const UnderlyingType&) const -> FuzzyValue<FuzzyVariable<std::string>> {
  return FuzzyValue<FuzzyVariable<std::string>>({});
}

template <typename UNDERLYING_TYPE>
auto FuzzyVariable<UNDERLYING_TYPE>::operator()(const CrispValuesU& value) const
    -> FuzzyValue<FuzzyVariable<UNDERLYING_TYPE>> {
  const auto decoded_value = std::get<UNDERLYING_TYPE>(value);
  return this->operator()(decoded_value);
}

template <typename UNDERLYING_TYPE> auto FuzzyVariable<UNDERLYING_TYPE>::operator==(const FuzzyVariable& other) const -> bool {
  return this->name == other.name;
}

template <typename UNDERLYING_TYPE> auto FuzzyVariable<UNDERLYING_TYPE>::operator==(std::string_view name) const -> bool {
  return this->name == name;
}

template <typename UNDERLYING_TYPE> auto FuzzyVariable<UNDERLYING_TYPE>::operator<(const FuzzyVariable& other) const -> bool {
  return this->name < other.name;
}

template <typename UNDERLYING_TYPE>
auto FuzzyVariable<UNDERLYING_TYPE>::operator=(const FuzzyVariable<UNDERLYING_TYPE>& other) -> FuzzyVariable<UNDERLYING_TYPE>& {
  if (this != &other) {
    name = other.name;
  }
  return *this;
}
template <typename UNDERLYING_TYPE>
auto FuzzyVariable<UNDERLYING_TYPE>::operator=(FuzzyVariable<UNDERLYING_TYPE>&& other) noexcept -> FuzzyVariable<UNDERLYING_TYPE>& {
  if (this != &other) {
    name = std::move(other.name);
  }
  return *this;
}

} // namespace fuzzyrulesml::rules
