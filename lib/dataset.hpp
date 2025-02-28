#pragma once
#include "rules.hpp"
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>
#include <vector>

namespace fuzzyrulesml::dataset {
class DataSet {
public:
  using RulesValuesContainer = std::vector<std::pair<fuzzyrulesml::rules::RuleTestingValues, std::string>>;
  using DatasetFeatures = std::map<std::string, double>;
  using DatasetTarget = std::string;
  using DatasetItem = std::pair<DatasetFeatures, DatasetTarget>;

  DataSet(nlohmann::json x_data, nlohmann::json y_data) {
    for (auto [x_item, y_item] : std::ranges::views::zip(x_data.items(), y_data.items())) {
      const nlohmann::json x_object = x_item.value();
      const nlohmann::json y_object = y_item.value();
      data.emplace_back(x_object.get<DatasetFeatures>(), y_object.at("class").get<DatasetTarget>());
    }
  };

  auto get_variables_names() const {
    std::vector<std::string> to_ret;
    for (const auto& [x, y] : data) {
      for (const auto& [key, value] : x) {
        if (std::ranges::find(to_ret, key) == to_ret.end()) {
          to_ret.push_back(key);
        }
      }
    }
    return to_ret;
  }

  template <typename... Targs>
  auto get_internal_items(auto item, auto variable_name_itself,
                          Targs... Fargs)
  {
    const auto [name, variable] = variable_name_itself;
    auto partial_map = get_internal_items(item, Fargs...);
    partial_map.emplace(variable, item.at(name));
    return partial_map;
  };

  auto get_internal_items(auto item, auto variable_name_itself)
  {
    const auto [name, variable] = variable_name_itself;
    return fuzzyrulesml::rules::RuleTestingValues{{variable, item.at(name)}};
  };

  template <typename... Targs> auto get_items(Targs... Fargs) {
    RulesValuesContainer to_ret;
    for (const auto& [item, y] : data) {
      auto rule_testing_values = get_internal_items(item, Fargs...);
      to_ret.emplace_back(rule_testing_values, y);
    }
    return to_ret;
  };

private:
  std::vector<DatasetItem> data;
};

void print(const DataSet::RulesValuesContainer& data) {
  std::print("------------------------- Printing dataset -------------------------\n");
  auto i = 1;
  for (const auto& [x, y] : data) {
    std::print("[{}]\t", i++);
    for (const auto& [key, value] : x) {
      std::print("{}: {:4}\t", fuzzyrulesml::rules::to_string(key), fuzzyrulesml::rules::to_string(value));
    }
    std::print("{}\n", y);
  }
  std::print("-------------------------   End of dataset -------------------------\n");
}

auto load_data(const std::string& input_file, const std::string& target_file) {
  nlohmann::json features_test, targets_test;
  std::ifstream ifs;
  ifs.open(input_file, std::ifstream::in);
  ifs >> features_test;
  ifs.close();
  ifs.open(target_file, std::ifstream::in);
  ifs >> targets_test;
  ifs.close();
  return std::pair{features_test, targets_test};
}

} // namespace fuzzyrulesml::dataset
