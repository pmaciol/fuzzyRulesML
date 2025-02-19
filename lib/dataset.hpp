#pragma once
#include "rules.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>
#include <vector>
#include <fstream>

namespace fuzzyrulesml::dataset {
class DataSet {
public:
  using Output = std::vector<
      std::pair<std::map<fuzzyrulesml::rules::VariableContainer, fuzzyrulesml::rules::ValuesContainer>, std::string>>;

  DataSet(nlohmann::json x_data, nlohmann::json y_data) {
    for (auto [x_item, y_item] : std::ranges::views::zip(x_data.items(), y_data.items())) {
      nlohmann::json x_object = x_item.value();
      nlohmann::json y_object = y_item.value();
      // for (const auto& [key, value] : x_object.get<std::map<std::string, double>>()) {
      //   std::print("test read {}: {} \t", key, value);
      // }
      // std::print("{}\n", y_object.dump());
      data.emplace_back(std::pair(x_object.get<std::map<std::string, double>>(), y_object["class"].get<std::string>()));
    }
  };

  template <typename T1, typename T2, typename T3, typename T4>
  auto get_items(const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
    std::vector<
        std::pair<std::map<fuzzyrulesml::rules::VariableContainer, fuzzyrulesml::rules::ValuesContainer>, std::string>>
        to_ret;
    for (const auto& [x, y] : data) {
      to_ret.emplace_back(
          std::map<fuzzyrulesml::rules::VariableContainer, fuzzyrulesml::rules::ValuesContainer>{
              {t1, x.at("sepal length")},
              {t2, x.at("sepal width")},
              {t3, x.at("petal length")},
              {t4, x.at("petal width")}},
          y);
    }
    return to_ret;
  }

private:
  std::vector<std::pair<std::map<std::string, double>, std::string>> data;
};

void print(const DataSet::Output& data) {
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
