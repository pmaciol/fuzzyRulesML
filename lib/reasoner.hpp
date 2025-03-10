#pragma once

#include "rules.hpp"
#include "variable.hpp"
#include <algorithm>

namespace fuzzyrulesml::reasoner {
class fuzzyLiteReasoner {};

auto calculate_one(const auto test_data, auto& reasoner, const auto print) {
  double goal_func{0.0};
  if (print) {
    std::print("------------------------- Printing all results -------------------------\n");
  }
  [[maybe_unused]] auto i = 1;
  for (const auto& [state, results] : test_data) {
    const auto reasoning_result = reasoner.do_reasoning(state);
    const auto biggest_val = std::ranges::max_element(reasoning_result, [](const auto& x, const auto& y) { return x.second < y.second; });

    if (print) {
      std::print("[{}]\t Match: {:6}\t Expected: {:20}\t inferred: {}\n", i++, results == biggest_val->first.second, results,
                 biggest_val->first.second);
    }
    if (results == biggest_val->first.second) {
      goal_func += 1.0;
    }
  }
  if (print)
    std::print("------------------------- End of results -------------------------\n");
  return goal_func;
}

auto get_membership(const auto fuzzy_variable, const auto crisp_value) {
  return std::visit([crisp_value](const auto& var) { return var(crisp_value).get_membership(); }, fuzzy_variable);
}

void set_rule_membership_value(auto& rule_memberships, const auto fuzzy_variable, const auto membership) {
  const auto found = rule_memberships.find(std::pair{fuzzy_variable, membership.first});
  if (found != rule_memberships.end()) {
    found->second = membership.second;
  }
}

class SimpleReasoner {
public:
  SimpleReasoner(const fuzzyrulesml::rules::RulesSet& rules) : stored_rules{rules} {};
  auto
  do_reasoning(const fuzzyrulesml::rules::RuleTestingValues& variables_map) -> std::map<fuzzyrulesml::rules::ConclusionChosen, double> {
    const auto usefeul_rules = stored_rules.get_rules(variables_map);
    std::multimap<fuzzyrulesml::rules::ConclusionChosen, double> conclusions;
    for (const auto& rule : usefeul_rules) {
      conclusions.insert(std::pair{rule.second, evaluate_rule(rule.first, variables_map)});
    }
    return std::accumulate(conclusions.begin(), conclusions.end(), std::map<fuzzyrulesml::rules::ConclusionChosen, double>{},
                           [](auto acc, const auto& pair) {
                             acc[pair.first] += pair.second;
                             return acc;
                           });
  }

  //                                                                                 };

private:
  auto evaluate_rule(const std::map<fuzzyrulesml::rules::FuzzyVarUnion, std::size_t>& rule_variables,
                     const fuzzyrulesml::rules::RuleTestingValues& crisp_values_for_rule_variables) -> double {
    std::map<fuzzyrulesml::rules::FuzzyVarMembership, double> rule_memberships{};
    for (const auto& variable : rule_variables) {
      rule_memberships[variable] = 0.0;
    }
    for (const auto& [fuzzy_variable, crisp_value] : crisp_values_for_rule_variables) {
      const auto memberships = get_membership(fuzzy_variable, crisp_value);
      for (const auto& membership : memberships) {
        set_rule_membership_value(rule_memberships, fuzzy_variable, membership);
      }
    }
    return std::accumulate(rule_memberships.begin(), rule_memberships.end(), 1.0,
                           [](auto acc, const auto& pair) { return acc * pair.second; });
  };
  fuzzyrulesml::rules::RulesSet stored_rules;
};

template <typename REASONER> auto create_reasoner() -> REASONER { return REASONER{}; }
} // namespace fuzzyrulesml::reasoner
