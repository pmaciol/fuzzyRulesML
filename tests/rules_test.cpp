#include "rules.hpp"
#include <map>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
namespace fru = fuzzyrulesml::rules;

TEST(RuleSetsVariables, add_input) {
  fru::RulesSet rules_set;
  rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  EXPECT_EQ(rules_set.get_input_variables_labels().size(), 1);
  EXPECT_STREQ(rules_set.get_input_variables_labels().front().c_str(), "petal_length");
}

TEST(RuleSetsVariables, created_and_get_match) {
  fru::RulesSet rules_set;
  const auto petal_created = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  const auto petal_got = rules_set.get_input_variables_labels();
  EXPECT_EQ(petal_created, petal_got[0]);
}

TEST(RuleSetsVariables, add_two_inputs) {
  fru::RulesSet rules_set;
  rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  const auto labels = rules_set.get_input_variables_labels();
  EXPECT_EQ(labels.size(), 2);
  EXPECT_THAT(labels, ::testing::Contains("petal_length"));
  EXPECT_THAT(labels, ::testing::Contains("petal_width"));
}

TEST(RuleSetsVariables, add_two_inputs_diff_types) {
  fru::RulesSet rules_set;
  rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(0, 10, 4));
  const auto labels = rules_set.get_input_variables_labels();
  EXPECT_EQ(labels.size(), 2);
  EXPECT_THAT(labels, ::testing::Contains("petal_length"));
  EXPECT_THAT(labels, ::testing::Contains("petal_width"));
}

TEST(RuleSetsVariables, fails_to_add_duplicate) {
  fru::RulesSet rules_set;
  rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  EXPECT_ANY_THROW(rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4)));
  const auto labels = rules_set.get_input_variables_labels();
  EXPECT_EQ(labels.size(), 1);
  EXPECT_THAT(labels, ::testing::Contains("petal_length"));
}

TEST(RuleSetsVariables, fails_to_add_duplicate_other_type) {
  fru::RulesSet rules_set;
  rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  EXPECT_ANY_THROW(rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4)));
  const auto labels = rules_set.get_input_variables_labels();
  EXPECT_EQ(labels.size(), 1);
  EXPECT_THAT(labels, ::testing::Contains("petal_length"));
}

TEST(RuleSetsVariables, variable_size) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  EXPECT_EQ(petal_length.size(), 4);
}

TEST(RuleSetsVariablesValues, get_membership_functions_boundaries) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 4));
  const auto valued_petal_length_1 = petal_length(0.0);
  const auto valued_petal_length_2 = petal_length(10.0);
  EXPECT_EQ(valued_petal_length_1.get_membership().size(), 1);
  EXPECT_EQ(valued_petal_length_2.get_membership().size(), 1);
  EXPECT_DOUBLE_EQ(valued_petal_length_1.get_membership()[0], 1.0);
  EXPECT_DOUBLE_EQ(valued_petal_length_2.get_membership()[3], 1.0);
}

TEST(RuleSetsVariablesValues, get_membership_only_two) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 2));
  const auto valued_petal_length_0 = petal_length(0.0);
  const auto valued_petal_length_5 = petal_length(5.0);
  const auto valued_petal_length_10 = petal_length(10.0);
  EXPECT_EQ(valued_petal_length_0.get_membership().size(), 1);
  EXPECT_DOUBLE_EQ(valued_petal_length_0.get_membership()[0], 1.0);
  EXPECT_EQ(valued_petal_length_5.get_membership().size(), 2);
  EXPECT_DOUBLE_EQ(valued_petal_length_5.get_membership()[0], 0.5);
  EXPECT_DOUBLE_EQ(valued_petal_length_5.get_membership()[1], 0.5);
  EXPECT_EQ(valued_petal_length_10.get_membership().size(), 1);
  EXPECT_DOUBLE_EQ(valued_petal_length_10.get_membership()[1], 1.0);
}

TEST(RuleSetsVariablesValues, get_membership_functions_mids) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto valued_petal_length_mid = petal_length(5.0);
  EXPECT_EQ(valued_petal_length_mid.get_membership().size(), 1);
  EXPECT_DOUBLE_EQ(valued_petal_length_mid.get_membership()[2], 1.0);
  const auto valued_petal_length_1 = petal_length(1.25);
  const auto valued_petal_length_2 = petal_length(8.75);
  EXPECT_EQ(valued_petal_length_1.get_membership().size(), 2);
  EXPECT_EQ(valued_petal_length_2.get_membership().size(), 2);
  EXPECT_DOUBLE_EQ(valued_petal_length_1.get_membership()[0], 0.5);
  EXPECT_DOUBLE_EQ(valued_petal_length_1.get_membership()[1], 0.5);
  EXPECT_DOUBLE_EQ(valued_petal_length_2.get_membership()[3], 0.5);
  EXPECT_DOUBLE_EQ(valued_petal_length_2.get_membership()[4], 0.5);
}

TEST(RuleSetsAddOutput, add_output_variable) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto iris_type = std::vector<std::string>{"Setosa", "Versicolor", "Virginica"};
  const auto output_variable = rules_set.add_output_variable("iris_type", iris_type);
  EXPECT_EQ(output_variable.get_name(), "iris_type");
  EXPECT_EQ(output_variable.get_categories().size(), 3);
  EXPECT_EQ(output_variable.get_categories()[0], "Setosa");
}

TEST(RuleSetsAddOutput, add_simple_rule) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto iris_type = std::vector<std::string>{"Setosa", "Versicolor", "Virginica"};
  const auto output_variable = rules_set.add_output_variable("iris_type", iris_type);
  const std::vector<fru::Rule> rules{{{{petal_length, 0}}, {"iris_type", "Setosa"}}};
  const auto filtered_rules = fru::get_matching_rules(rules, {{petal_length, 0}});
  EXPECT_EQ(filtered_rules.size(), 1);
  const auto empty_filtered_rules = fru::get_matching_rules(rules, {{petal_length, 1}});
  EXPECT_EQ(empty_filtered_rules.size(), 0);
}

TEST(RuleSetsAddOutput, add_double_rule) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto petal_width = rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto iris_type = std::vector<std::string>{"Setosa", "Versicolor", "Virginica"};
  const auto output_variable = rules_set.add_output_variable("iris_type", iris_type);

  const std::vector<fru::Rule> rules{{{{petal_length, 0}, {petal_width, 0}}, {"iris_type", "Setosa"}},
                                     {{{petal_length, 1}, {petal_width, 1}}, {"iris_type", "Versicolor"}}};

  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 0}}).size(), 0);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 1}}).size(), 0);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_width, 0}}).size(), 0);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_width, 1}}).size(), 0);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 0}, {petal_width, 0}}).size(), 1);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 1}, {petal_width, 1}}).size(), 1);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 0}, {petal_width, 1}}).size(), 0);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 0}, {petal_width, 1}}).size(), 0);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 0}, {petal_length, 1}, {petal_width, 0}, {petal_width, 1}}).size(), 2);
  EXPECT_EQ(fru::get_matching_rules(rules, {{petal_length, 0}, {petal_length, 1}, {petal_width, 0}}).size(), 1);
}

TEST(RuleSetsAddOutput, add_simple_valued_rule) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto iris_type = std::vector<std::string>{"Setosa", "Versicolor", "Virginica"};
  const auto output_variable = rules_set.add_output_variable("iris_type", iris_type);
  rules_set.add_rule({{petal_length, 0}}, {"iris_type", "Setosa"});
  const auto filtered_rules =
      rules_set.get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
          {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{0.0}}}));
  EXPECT_EQ(filtered_rules.size(), 1);
  const auto empty_filtered_rules =
      rules_set.get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
          {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{5.0}}}));
  EXPECT_EQ(empty_filtered_rules.size(), 0);
}

TEST(RuleSetsAddOutput, add_double_valued_rule) {
  fru::RulesSet rules_set;
  const auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto petal_width = rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto iris_type = std::vector<std::string>{"Setosa", "Versicolor", "Virginica"};
  const auto output_variable = rules_set.add_output_variable("iris_type", iris_type);
  rules_set.add_rule({{petal_length, 0}, {petal_width, 0}}, {"iris_type", "Setosa"});
  rules_set.add_rule({{petal_length, 1}, {petal_width, 1}}, {"iris_type", "Versicolor"});
  EXPECT_EQ(rules_set
                .get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
                    {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{0.0}}}))
                .size(),
            0);
  EXPECT_EQ(rules_set
                .get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
                    {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{0.0}},
                    {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{0.0}}}))
                .size(),
            1);
  EXPECT_EQ(rules_set
                .get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
                    {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{1.0}},
                    {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{1.0}}}))
                .size(),
            2);
  EXPECT_EQ(rules_set
                .get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
                    {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{9.0}},
                    {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{9.0}}}))
                .size(),
            0);
  EXPECT_EQ(rules_set
                .get_rules(fru::RuleTestingValues(std::map<fuzzyrulesml::rules::FuzzyVarUnion, fuzzyrulesml::rules::CrispValuesUnion>{
                    {fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{0.0}},
                    {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{9.0}}}))
                .size(),
            0);
}