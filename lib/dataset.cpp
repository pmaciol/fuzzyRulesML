#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
namespace fuzzyrulesml::dataset {
auto load_data(const std::string& input_file, const std::string& target_file) {
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

} // namespace fuzzyrulesml::dataset