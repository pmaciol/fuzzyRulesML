#pragma once

#include "membership_functions.hpp"
#include <format>
#include <map>
#include <print>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fuzzyrulesml::rules {

class CrispValuesUnion {
public:
  template <typename T> CrispValuesUnion(const T& value) : payload(value) {}
  [[nodiscard]] auto to_string() const -> std::string {
    return std::visit([](const auto& var) { return std::format("{}", var); }, payload);
  }
  template <typename T> [[nodiscard]] auto get() const -> T { return std::get<T>(payload); }

private:
  std::variant<double, int> payload;
};

namespace initial_distribution {
template <typename UNDERLYING_TYPE = double> class Uniform {
public:
  using UnderlyingType = UNDERLYING_TYPE;
  Uniform(UnderlyingType min, UnderlyingType max, std::size_t categories) : min(min), max(max), categories(categories) {}
  [[nodiscard]] auto get_categories() const -> std::size_t { return categories; }
  // auto get_membership(double value) const -> std::map<int, double> { return std::map<int, double>{{0, 1.0}}; }
  [[nodiscard]] auto get_min() const -> UnderlyingType { return min; }
  [[nodiscard]] auto get_max() const -> UnderlyingType { return max; }

private:
  UnderlyingType min;
  UnderlyingType max;
  std::size_t categories;
};
} // namespace initial_distribution
template <typename VARIABLE> class FuzzyValue;

template <typename UNDERLYING_TYPE> class FuzzyVariable {
public:
  using UnderlyingType = UNDERLYING_TYPE;
  FuzzyVariable(std::string_view name, const initial_distribution::Uniform<UnderlyingType>& distribution);
  ~FuzzyVariable() = default;
  FuzzyVariable(const FuzzyVariable& other) : name(other.name), distribution{other.distribution} {}
  FuzzyVariable(FuzzyVariable&& other) noexcept : name(std::move(other.name)), distribution(std::move(other.distribution)) {}

  [[nodiscard]] auto operator=(const FuzzyVariable& other) -> FuzzyVariable&;
  [[nodiscard]] auto operator=(FuzzyVariable&& other) noexcept -> FuzzyVariable&;
  [[nodiscard]] auto operator()(const UnderlyingType& value) const -> FuzzyValue<UnderlyingType>;
  [[nodiscard]] auto operator()(const CrispValuesUnion& value) const -> FuzzyValue<UnderlyingType>;
  [[nodiscard]] auto operator==(const FuzzyVariable& other) const -> bool;
  [[nodiscard]] auto operator==(std::string_view name) const -> bool;
  [[nodiscard]] auto operator<(const FuzzyVariable& other) const -> bool;
  [[nodiscard]] auto compare(const auto& other) const -> bool { return this->name < other.get_name(); }

  [[nodiscard]] auto get_name() const -> std::string { return name; }
  [[nodiscard]] auto size() const -> int { return distribution.get_categories(); }

  void set_points(const std::vector<double>& characteristic_points) { distribution.set_points(characteristic_points); };
  [[nodiscard]] auto get_points() const -> std::vector<double> { return distribution.get_points(); };

private:
  std::string name;
  fuzzyrulesml::mfunct::LinearDistribution<UNDERLYING_TYPE> distribution;
};

template <typename UnderlyingType> class FuzzyValue {
public:
  FuzzyValue(const Membership& membership) : membership(membership) {}
  [[nodiscard]] auto get_membership() const -> Membership { return membership; }

private:
  typename FuzzyVariable<UnderlyingType>::UnderlyingType value;
  Membership membership;

  // ...
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename UNDERLYING_TYPE>
inline auto make_linear_distribution(const initial_distribution::Uniform<UNDERLYING_TYPE>& distribution) {
  const auto number_of_internal_functions = distribution.get_categories() - 2;
  const auto section_length =
      static_cast<UNDERLYING_TYPE>(distribution.get_max() - distribution.get_min()) / (distribution.get_categories() - 1);
  const auto left = fuzzyrulesml::mfunct::LinearMemberFunctDesc{distribution.get_min(),
                                                                distribution.get_min() + static_cast<UNDERLYING_TYPE>(section_length)};
  const auto right = fuzzyrulesml::mfunct::LinearMemberFunctAsc(distribution.get_max() - static_cast<UNDERLYING_TYPE>(section_length),
                                                                distribution.get_max());
  std::vector<fuzzyrulesml::mfunct::LinearMemberFunct<UNDERLYING_TYPE>> functions;
  for (std::size_t i = 0; i < number_of_internal_functions; i++) {
    functions.push_back(
        fuzzyrulesml::mfunct::LinearMemberFunct{distribution.get_min() + static_cast<UNDERLYING_TYPE>(i * section_length),
                                                distribution.get_min() + static_cast<UNDERLYING_TYPE>((i + 1) * section_length),
                                                distribution.get_min() + static_cast<UNDERLYING_TYPE>((i + 2) * section_length)});
    const auto& added = functions.back();
    std::print("Adding midfunction {} {} {} {}\n", section_length, added.get_left_bound(), added.get_peak(), added.get_right_bound());
  }
  return fuzzyrulesml::mfunct::LinearDistribution{fuzzyrulesml::mfunct::LinearDistribution{left, functions, right}};
};

template <typename UNDERLYING_TYPE>
FuzzyVariable<UNDERLYING_TYPE>::FuzzyVariable(std::string_view name, const initial_distribution::Uniform<UNDERLYING_TYPE>& distribution)
    : name(name), distribution(make_linear_distribution(distribution)){};

template <typename UNDERLYING_TYPE>
auto FuzzyVariable<UNDERLYING_TYPE>::operator()(const UnderlyingType& value) const -> FuzzyValue<UNDERLYING_TYPE> {
  return FuzzyValue<UNDERLYING_TYPE>(this->distribution(value));
}

template <> inline auto FuzzyVariable<int>::operator()(const UnderlyingType& /*unused*/) const -> FuzzyValue<int> {
  return FuzzyValue<int>({}); // NOLINT(modernize-return-braced-init-list)
}

template <typename UNDERLYING_TYPE>
auto FuzzyVariable<UNDERLYING_TYPE>::operator()(const CrispValuesUnion& value) const -> FuzzyValue<UNDERLYING_TYPE> {
  const auto decoded_value = value.get<UNDERLYING_TYPE>();
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
