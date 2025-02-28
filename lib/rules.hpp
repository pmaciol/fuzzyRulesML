#pragma once

#include "variable.hpp"
#include <algorithm>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include <map>

namespace fuzzyrulesml::rules {
using OutputVariableContainer = FuzzyVariable<std::string>;

using FuzzyVariableU = std::variant<FuzzyVariable<double>, FuzzyVariable<std::string>>;
using FuzzyValueU = std::variant<FuzzyValue<FuzzyVariable<double>>, FuzzyValue<FuzzyVariable<std::string>>>;
using Conclusion = std::string;
using Rule = std::pair<std::map<FuzzyVariableU, std::size_t>, Conclusion>;
using RuleTestingValues = std::map<fuzzyrulesml::rules::FuzzyVariableU, fuzzyrulesml::rules::CrispValuesU>;

inline auto all_variables_match(const std::multimap<FuzzyVariableU, std::size_t>& variables_map,
                                const std::map<FuzzyVariableU, std::size_t>& preconditions) -> bool {
  return (std::ranges::all_of(preconditions, [&variables_map](const auto& variable_index_pair) {
    return (std::ranges::any_of(variables_map, [variable_index_pair](const auto& query_variable_index_pair) {
      return variable_index_pair == query_variable_index_pair;
    }));
  }));
}

inline auto to_string(const FuzzyVariableU& variable_container) -> std::string {
  return std::visit([](const auto& var) { return var.get_name(); }, variable_container);
}

class RulesSet {
public:
  template <typename VARIABLE_TYPE>
  auto add_input_variable(std::string_view name, initial_distribution::Uniform dist) -> FuzzyVariable<VARIABLE_TYPE>;

  template <typename VARIABLE_TYPE> auto get_input_variable(std::string_view name) -> FuzzyVariable<VARIABLE_TYPE>;

  auto add_output_variable(std::string_view name, std::vector<std::string> categories) -> std::vector<std::string>;

  void add_rule(const std::map<FuzzyVariableU, std::size_t>& variables_map, const Conclusion& conclusion) {
    rules.emplace_back(std::pair{variables_map, conclusion});
  }

  auto get_rules(const std::multimap<FuzzyVariableU, std::size_t>& variables_map) -> std::vector<Rule> {
    return rules | std::views::filter([&variables_map](const auto& rule) {
             return all_variables_match(variables_map, rule.first);
           }) |
           std::ranges::to<std::vector<Rule>>();
  }

  auto get_rules(const std::map<FuzzyVariableU, CrispValuesU>& variables_map) -> std::vector<Rule> {
      std::multimap<FuzzyVariableU, std::size_t> output_map;
    for (const auto& variable : variables_map) {
      const Membership memberships = std::visit([val = variable.second](const auto& var) { 
        return var.operator()(val).get_membership(); }, variable.first);
        
        for (const auto& member: memberships) {
          output_map.insert(std::pair{variable.first,member.first});
        }
    }
    return get_rules(output_map);
  }

  template <typename DATA_SET, typename REASONER>
  void optimize_rules(const DATA_SET& data_set, const REASONER& reasoner);

  [[nodiscard]] inline auto get_input_variables_labels() const -> std::vector<std::string>;

private:
  std::set<FuzzyVariableU> input_variables;
  std::map<std::string, std::vector<std::string>> output_variables;
  std::vector<Rule> rules;
};

inline auto RulesSet::get_input_variables_labels() const -> std::vector<std::string> {
  return input_variables | std::views::transform([](auto const& variable) {
           return std::visit([](const auto& var) { return var.get_name(); }, variable);
         }) |
         std::ranges::to<std::vector<std::string>>();
};

template <typename VARIABLE_TYPE>
auto RulesSet::add_input_variable(std::string_view variable_name,
                                  initial_distribution::Uniform distribution) -> FuzzyVariable<VARIABLE_TYPE> {
  const auto found = std::ranges::find(input_variables, variable_name, [](auto const& variable) {
    return std::visit([](const auto& var) { return var.get_name(); }, variable);
  });
  if (found != input_variables.end()) {
    throw std::runtime_error("Input variable already exists");
  }
  auto created_variable = FuzzyVariable<VARIABLE_TYPE>(variable_name, distribution);
  input_variables.emplace(created_variable);
  return created_variable;
}

template <typename VARIABLE_TYPE>
auto RulesSet::get_input_variable(std::string_view variable_name) -> FuzzyVariable<VARIABLE_TYPE> {
  const auto found = std::ranges::find(input_variables, variable_name, [](auto const& variable) {
    return std::visit([](const auto& var) { return var.get_name(); }, variable);
  });
  if (found == input_variables.end()) {
    throw std::runtime_error("Variable not found");
  }
  return std::get<FuzzyVariable<VARIABLE_TYPE>>(*found);
}

inline auto RulesSet::add_output_variable(std::string_view variable_name,
                                          std::vector<std::string> output_categories) -> std::vector<std::string> {
  const auto found = output_variables.find(std::string{variable_name});
  if (found != output_variables.end()) {
    throw std::runtime_error("Output variable already exists");
  }
  output_variables[std::string{variable_name}] = output_categories;
  return output_categories;
}

template <typename DATA_SET, typename REASONER> void optimize_rules(const DATA_SET&, const REASONER&) {};

} // namespace fuzzyrulesml::rules
