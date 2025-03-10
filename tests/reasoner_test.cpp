#include "reasoner.hpp"
#include "rules.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
namespace fru = fuzzyrulesml::rules;
namespace fre = fuzzyrulesml::reasoner;

TEST(SimpleReasoning, add_double_rule) {
  fru::RulesSet rules_set;
  const auto petal_length =
      rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto petal_width =
      rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(0.0, 10.0, 5));
  const auto iris_type = std::vector<std::string>{"Setosa", "Versicolor", "Virginica"};
  const auto output_variable = rules_set.add_output_variable("iris_type", iris_type);
  rules_set.add_rule({{petal_length, 0}, {petal_width, 0}}, {"iris_type", "Setosa"});
  rules_set.add_rule({{petal_length, 1}, {petal_width, 1}}, {"iris_type", "Versicolor"});
  fre::SimpleReasoner reasoner{rules_set};

  const auto result = reasoner.do_reasoning({{fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{0.0}},
                                             {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{0.0}}});
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.at({"iris_type", "Setosa"}), 1.0);

  const auto result2 = reasoner.do_reasoning({{fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{1.0}},
                                              {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{1.0}}});
  EXPECT_EQ(result2.size(), 2);
  EXPECT_GT(result2.at({"iris_type", "Setosa"}), result2.at({"iris_type", "Versicolor"}));
  const auto result3 = reasoner.do_reasoning({{fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{2.0}},
                                              {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{2.0}}});

  EXPECT_EQ(result3.size(), 2);
  EXPECT_LT(result3.at({"iris_type", "Setosa"}), result3.at({"iris_type", "Versicolor"}));
  const auto result4 = reasoner.do_reasoning({{fru::FuzzyVarUnion{petal_length}, fru::CrispValuesUnion{3.0}},
                                              {fru::FuzzyVarUnion{petal_width}, fru::CrispValuesUnion{3.0}}});

  EXPECT_EQ(result4.size(), 1);
  EXPECT_GT(result4.at({"iris_type", "Versicolor"}), 0.0);
}