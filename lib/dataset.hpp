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
private:
  class RulesValuesContainer {
  public:
    void add(const fuzzyrulesml::rules::RuleTestingValues& rule_testing_values, const std::string& target) {
      data.emplace_back(rule_testing_values, target);
    }
    [[nodiscard]] auto size() const { return data.size(); }
    [[nodiscard]] auto begin() const { return data.begin(); }
    [[nodiscard]] auto end() const { return data.end(); }
    void print() const {
      std::print("------------------------- Printing dataset -------------------------\n");
      auto iter = 1;
      for (const auto& [external_key, external_value] : data) {
        std::print("[{}]\t", iter++);
        for (const auto& [key, value] : external_key) {
          std::print("{}: {:4}\t", key.to_string(), value.to_string());
        }
        std::print("{}\n", external_value);
      }
      std::print("-------------------------   End of dataset -------------------------\n");
    }

  private:
    std::vector<std::pair<fuzzyrulesml::rules::RuleTestingValues, std::string>> data;
  };

  class DatasetFeatures {
  public:
    using internal_type = std::map<std::string, double>;
    DatasetFeatures(const internal_type& data) : data(data) {}
    DatasetFeatures(internal_type&& data) : data(std::move(data)) {}
    [[nodiscard]] auto begin() const { return data.begin(); }
    [[nodiscard]] auto end() const { return data.end(); }
    [[nodiscard]] auto at(const std::string& key) const -> double {
      auto iter = data.find(key);
      if (iter != data.end()) {
        return iter->second;
      }
      throw std::out_of_range("Key not found in DatasetFeatures");
    }

  private:
    std::map<std::string, double> data;
  };

public:
  using DatasetTarget = std::string;
  using DatasetItem = std::pair<DatasetFeatures, DatasetTarget>;

  DataSet(nlohmann::json x_data, nlohmann::json y_data) {
    for (auto [x_item, y_item] : std::ranges::views::zip(x_data.items(), y_data.items())) {
      const nlohmann::json x_object = x_item.value();
      const nlohmann::json y_object = y_item.value();
      data.emplace_back(x_object.get<typename DatasetFeatures::internal_type>(), y_object.at("class").get<DatasetTarget>());
    }
  };

  [[nodiscard]] auto get_variables_names() const {
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

  template <typename... Targs> auto get_internal_items(auto item, auto variable_name_itself, Targs... Fargs) {
    const auto [name, variable] = variable_name_itself;
    auto partial_map = get_internal_items(item, Fargs...);
    partial_map.add(variable, item.at(name));
    return partial_map;
  };

  auto get_internal_items(auto item, auto variable_name_itself) {
    const auto [name, variable] = variable_name_itself;
    return fuzzyrulesml::rules::RuleTestingValues{fuzzyrulesml::rules::FuzzyVarUnion{variable},
                                                  fuzzyrulesml::rules::CrispValuesUnion{item.at(name)}};
  };

  template <typename... Targs> auto get_items(Targs... Fargs) {
    RulesValuesContainer to_ret;
    for (const auto& [item, y] : data) {
      auto rule_testing_values = get_internal_items(item, Fargs...);
      to_ret.add(rule_testing_values, y);
    }
    return to_ret;
  };

private:
  std::vector<DatasetItem> data;
};

auto load_data(const std::string& input_file, const std::string& target_file) -> std::pair<nlohmann::json, nlohmann::json>;

} // namespace fuzzyrulesml::dataset
