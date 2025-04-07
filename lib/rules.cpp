#include "rules.hpp"

namespace fuzzyrulesml::rules {

auto all_variables_match(const std::multimap<FuzzyVarUnion, std::size_t>& variables_map,
                         const std::map<FuzzyVarUnion, std::size_t>& preconditions) -> bool {
  return (std::ranges::all_of(preconditions, [&variables_map](const auto& variable_index_pair) {
    return (std::ranges::any_of(variables_map, [variable_index_pair](const auto& query_variable_index_pair) {
      return variable_index_pair == query_variable_index_pair;
    }));
  }));
}

auto to_string(const FuzzyVarUnion& variable_container) -> std::string {
  return std::visit([](const auto& var) { return var.get_name(); }, variable_container);
}

auto get_matching_rules(const auto rules, const std::multimap<FuzzyVarUnion, std::size_t>& variables_map) -> std::vector<Rule> {
  return rules |
         std::views::filter([&variables_map](const auto& rule) { return all_variables_match(variables_map, rule.get_preconditions()); }) |
         std::ranges::to<std::vector<Rule>>();
}

auto RulesSet::add_output_variable(std::string_view variable_name, std::vector<std::string> output_categories) -> Conclusion {
  const auto [pointer, is_inserted] = output_variables.emplace(variable_name, output_categories);
  if (!is_inserted) {
    throw std::runtime_error("Output variable already exists");
  }
  return Conclusion{pointer->first, pointer->second};
}

void RulesSet::add_rule(const std::map<FuzzyVarUnion, std::size_t>& variables_map, const ConclusionChosen& conclusion) {
  if (not std::ranges::all_of(variables_map | std::views::keys,
                              [this](const auto& variable) { return input_variables.contains(variable); }))
    throw std::runtime_error("Input variable not found");

  const auto found = output_variables.find(conclusion.name);
  if (found == output_variables.end()) {
    throw std::runtime_error("Output variable not found");
  };
  const auto internal_found = std::ranges::find(found->second, conclusion.item);
  if (internal_found == found->second.end()) {
    throw std::runtime_error("Conclusion item not found");
  }
  rules.emplace_back(variables_map, conclusion);
}

auto RulesSet::get_rules(const RuleTestingValues& variables_map) -> std::vector<Rule> {
  std::multimap<FuzzyVarUnion, std::size_t> output_map;
  for (const auto& variable : variables_map) {
    const Membership memberships =
        std::visit([val = variable.second](const auto& var) { return var.operator()(val).get_membership(); }, variable.first);

    for (const auto& member : memberships) {
      output_map.insert(std::pair{variable.first, member.first});
    }
  }
  return get_matching_rules(this->rules, output_map);
}

auto RulesSet::get_input_variables_labels() const -> std::vector<std::string> {
  return input_variables |
         std::views::transform([](auto const& variable) { return std::visit([](const auto& var) { return var.get_name(); }, variable); }) |
         std::ranges::to<std::vector<std::string>>();
};

auto Rule::get_preconditions() const -> std::map<FuzzyVarMembership::first_type, FuzzyVarMembership::second_type> { return preconditions; }

auto Rule::get_conclusion() const -> ConclusionChosen { return conclusion; }

auto Conclusion::get_name() const -> std::string { return name; }

auto Conclusion::get_categories() const -> std::vector<ConclusionItem> { return categories; }

void RuleTestingValues::add(const fuzzyrulesml::rules::FuzzyVarUnion& fuzzy_value,
                            const fuzzyrulesml::rules::CrispValuesUnion& crisp_value) {
  crisp_values.emplace(fuzzy_value, crisp_value);
}

bool ConclusionChosen::operator<(const ConclusionChosen& other) const {
  if (name != other.name) {
    return name < other.name; // Compare by name first
  }
  return item < other.item; // If names are equal, compare by item
}

bool FuzzyVarMembership::operator<(const FuzzyVarMembership& other) const {
  if (fuzzy_variable != other.fuzzy_variable) {
    return fuzzy_variable < other.fuzzy_variable; // Compare by name first
  }
  return membership_index < other.membership_index; // If names are equal, compare by item
};
} // namespace fuzzyrulesml::rules
