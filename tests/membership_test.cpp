#include "membership_functions.hpp"
#include "variable.hpp"
#include "gtest/gtest.h"

using namespace fuzzyrulesml::mfunct;
namespace fru = fuzzyrulesml::rules;

TEST(LinearMemberFunction, valid_function) {
  const auto linear_function = LinearMemberFunct{0.0, 1.0, 2.0};
  EXPECT_FALSE(linear_function(0.0));
  EXPECT_DOUBLE_EQ(linear_function(0.5).value(), 0.5);
  EXPECT_DOUBLE_EQ(linear_function(1.0).value(), 1.0);
  EXPECT_DOUBLE_EQ(linear_function(1.5).value(), 0.5);
  EXPECT_FALSE(linear_function(2.0));
}

TEST(LinearMemberFunction, descending_1) {
  const auto linear_function = LinearMemberFunctDesc{0.0, 1.0};
  EXPECT_TRUE(linear_function(0.0));
  EXPECT_DOUBLE_EQ(linear_function(0.0).value(), 1.0);
  EXPECT_DOUBLE_EQ(linear_function(0.5).value(), 0.5);
  EXPECT_FALSE(linear_function(1.0));
}

TEST(LinearMemberFunction, descending_2) {
  const auto linear_function = LinearMemberFunctDesc{1.0, 2.0};
  EXPECT_TRUE(linear_function(1.0));
  EXPECT_DOUBLE_EQ(linear_function(1.0).value(), 1.0);
  EXPECT_DOUBLE_EQ(linear_function(1.5).value(), 0.5);
  EXPECT_FALSE(linear_function(2.0));
}

TEST(LinearMemberFunction, ascending_1) {
  const auto linear_function = LinearMemberFunctAsc{0.0, 1.0};
  EXPECT_FALSE(linear_function(0.0));  
  EXPECT_TRUE(linear_function(1.0));
  EXPECT_DOUBLE_EQ(linear_function(0.5).value(), 0.5);
  EXPECT_DOUBLE_EQ(linear_function(1.0).value(), 1.0);
  
}

TEST(LinearMemberFunction, ascending_2) {
  const auto linear_function = LinearMemberFunctAsc{1.0, 2.0};
  EXPECT_FALSE(linear_function(1.0));  
  EXPECT_TRUE(linear_function(2.0));
  EXPECT_DOUBLE_EQ(linear_function(1.5).value(), 0.5);
  EXPECT_DOUBLE_EQ(linear_function(2.0).value(), 1.0);
  
}

TEST(LinearMemberFunction, bad_initialization) { 
    EXPECT_ANY_THROW(LinearMemberFunct(0.0, 0.0, 2.0)); 
    EXPECT_ANY_THROW(LinearMemberFunct(0.0, 1.0, 1.0)); 
    EXPECT_ANY_THROW(LinearMemberFunct(2.0, 1.0, 0.0)); 
    EXPECT_ANY_THROW(LinearMemberFunct(0.0, 1.0, 0.0)); 
    }

TEST(CreateLinearDistribution, exact_values) {
  const auto uniform_distribution = fru::initial_distribution::Uniform(0.0, 10.0, 5);
  const auto ld = fru::make_linear_distribution(uniform_distribution);
  EXPECT_EQ(ld.get_categories(),5 );
  EXPECT_EQ(ld(0.0).size(),1);
  EXPECT_EQ(ld(2.5).size(),1);
  EXPECT_EQ(ld(5.0).size(),1);
  EXPECT_EQ(ld(7.5).size(),1);
  EXPECT_EQ(ld(10.0).size(),1);
}

TEST(CreateLinearDistribution, mid_values) {
  const auto uniform_distribution = fru::initial_distribution::Uniform(0.0, 10.0, 5);
  const auto linear_distribution_0_10_5 = fru::make_linear_distribution(uniform_distribution);
  EXPECT_EQ(linear_distribution_0_10_5(1.25).size(),2);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(1.25)[0], 0.5);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(1.25)[1], 0.5);
  EXPECT_EQ(linear_distribution_0_10_5(3.75).size(),2);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(3.75)[1], 0.5);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(3.75)[2], 0.5);
  EXPECT_EQ(linear_distribution_0_10_5(6.25).size(),2);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(6.25)[2], 0.5);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(6.25)[3], 0.5);
  EXPECT_EQ(linear_distribution_0_10_5(8.75).size(),2);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(8.75)[3], 0.5);
  EXPECT_DOUBLE_EQ(linear_distribution_0_10_5(8.75)[4], 0.5);
}

TEST(CreateLinearDistribution, valid_distribution) {
    const auto linear_distr_3_func = LinearDistribution{LinearMemberFunctDesc{0.0, 1.0}, {LinearMemberFunct{0.0, 1.0, 2.0}}, LinearMemberFunctAsc{1.0, 2.0}};
    auto memberships_00 = linear_distr_3_func(0.0);
    auto memberships_05 = linear_distr_3_func(0.5);
    auto memberships_10 = linear_distr_3_func(1.0);
    auto memberships_15 = linear_distr_3_func(1.5);
    auto memberships_20 = linear_distr_3_func(2.0);
    
    EXPECT_EQ(memberships_00.size(), 1);
    EXPECT_DOUBLE_EQ(memberships_00[0], 1.0);
    
    EXPECT_EQ(memberships_05.size(), 2);
    EXPECT_DOUBLE_EQ(memberships_05[0], 0.5);
    EXPECT_DOUBLE_EQ(memberships_05[1], 0.5);

    EXPECT_EQ(memberships_10.size(), 1);
    EXPECT_DOUBLE_EQ(memberships_10[1], 1.0);

    EXPECT_EQ(memberships_15.size(), 2);
    EXPECT_DOUBLE_EQ(memberships_15[1], 0.5);
    EXPECT_DOUBLE_EQ(memberships_15[2], 0.5);

    EXPECT_EQ(memberships_20.size(), 1);
    EXPECT_DOUBLE_EQ(memberships_20[2], 1.0);

}