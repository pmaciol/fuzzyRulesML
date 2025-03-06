#pragma once

#include "variable.hpp"
#include <algorithm>
#include <map>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fuzzyrulesml::rules {
// FuzzyVarUnion stores any of available fuzzy variable; variable drescribes all possible range of crisp values and they
// mapping to the memebreship functions
using FuzzyVarUnion = std::variant<FuzzyVariable<double>, FuzzyVariable<int>>;
// FuzzyValueUnion stores ALL memberships for the crisp value
using FuzzyValueUnion = std::variant<FuzzyValue<double>, FuzzyValue<int>>;
// Conclusion is a label of a rule output
using Conclusion = std::string;
// Rule is a pair of the preconditions and the conclusion
using Rule = std::pair<std::map<FuzzyVarUnion, std::size_t>, Conclusion>;
// RuleTestingValues is a map of the fuzzy variables and their crisp values; it is used as an input for evaluation of
// the rule
using RuleTestingValues = std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>;

auto all_variables_match(const std::multimap<FuzzyVarUnion, std::size_t>& variables_map,
                         const std::map<FuzzyVarUnion, std::size_t>& preconditions) -> bool;

auto to_string(const FuzzyVarUnion& variable_container) -> std::string;


auto get_matching_rules(const auto rules, const std::multimap<FuzzyVarUnion, std::size_t>& variables_map) -> std::vector<Rule>;

class RulesSet {
public:
  template <typename VARIABLE_TYPE>
  auto add_input_variable(std::string_view name, initial_distribution::Uniform<VARIABLE_TYPE> dist) -> FuzzyVariable<VARIABLE_TYPE>;  
  [[nodiscard]] auto add_output_variable(std::string_view name, std::vector<std::string> categories) -> std::vector<std::string>;  
  
  void add_rule(const std::map<FuzzyVarUnion, std::size_t>& variables_map, const Conclusion& conclusion);
  [[nodiscard]] auto get_rules(const std::map<FuzzyVarUnion, CrispValuesUnion>& variables_map) -> std::vector<Rule>;
  [[nodiscard]] auto get_input_variables_labels() const -> std::vector<std::string>;

  private:  
  std::set<FuzzyVarUnion> input_variables;
  std::map<std::string, std::vector<std::string>> output_variables;
  std::vector<Rule> rules;

  // template <typename VARIABLE_TYPE> [[nodiscard]] auto get_input_variable(std::string_view name) -> FuzzyVariable<VARIABLE_TYPE>;
  // [[nodiscard]] auto get_input_variables_labels() const -> std::vector<std::string>;
};

template <typename VARIABLE_TYPE>
auto RulesSet::add_input_variable(std::string_view variable_name,
                                  initial_distribution::Uniform<VARIABLE_TYPE> distribution) -> FuzzyVariable<VARIABLE_TYPE> {
  const auto found = std::ranges::find(input_variables, variable_name, [](auto const& variable) {
    return std::visit([](const auto& var) { return var.get_name(); }, variable);
  });
  if (found != input_variables.end()) {
    throw std::runtime_error("Input variable already exists");
  }
  auto created_variable = FuzzyVariable<typename initial_distribution::Uniform<VARIABLE_TYPE>::UnderlyingType>(variable_name, distribution);
  input_variables.emplace(created_variable);
  return created_variable;
}

// template <typename VARIABLE_TYPE> auto RulesSet::get_input_variable(std::string_view variable_name) -> FuzzyVariable<VARIABLE_TYPE> {
//   const auto found = std::ranges::find(input_variables, variable_name, [](auto const& variable) {
//     return std::visit([](const auto& var) { return var.get_name(); }, variable);
//   });
//   if (found == input_variables.end()) {
//     throw std::runtime_error("Variable not found");
//   }
//   return std::get<FuzzyVariable<VARIABLE_TYPE>>(*found);
// }

} // namespace fuzzyrulesml::rules
