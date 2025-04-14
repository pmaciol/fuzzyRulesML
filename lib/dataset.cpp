#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include "dataset.hpp"
namespace fuzzyrulesml::dataset {
  
auto load_data(const std::string& input_file, const std::string& target_file)  -> std::pair<nlohmann::json, nlohmann::json>{
  nlohmann::json features_test;
  nlohmann::json targets_test;
  std::ifstream ifs;
  ifs.open(input_file, std::ifstream::in);
  ifs >> features_test;
  ifs.close();
  ifs.open(target_file, std::ifstream::in);
  ifs >> targets_test;
  ifs.close();
  return std::pair{features_test, targets_test};
}

DataSet::DataSet(nlohmann::json x_data, nlohmann::json y_data) {
  for (auto [x_item, y_item] : std::ranges::views::zip(x_data.items(), y_data.items())) {
    const nlohmann::json x_object = x_item.value();
    const nlohmann::json y_object = y_item.value();
    data.emplace_back(x_object.get<typename DatasetFeatures::InternalStorage>(), y_object.at("class").get<DatasetTarget>());
  }
};

auto DataSet::get_variables_names() const -> std::vector<std::string>{
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

} // namespace fuzzyrulesml::dataset