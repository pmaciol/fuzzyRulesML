#include "lib/data.hpp"
#include "lib/dataset.hpp"
#include "lib/optimizer.hpp"
#include "lib/reasoner.hpp"
#include "lib/rules.hpp"
#include <CLI/CLI.hpp>
#include <string>
#include <tuple>

namespace fd = fuzzyrulesml::data;
namespace fru = fuzzyrulesml::rules;
namespace fre = fuzzyrulesml::reasoner;
namespace fdd = fuzzyrulesml::dataset;

const int small = 0;
const int large = 1;
const int setosa = 0;
const int versicolor = 1;
const int virginica = 2;

auto get_rules_set(std::vector<double> v1) {
  fru::RulesSet rules_set;
  if (v1.empty()) {
    auto sepal_length =
        rules_set.add_input_variable<double>("sepal_length", fru::initial_distribution::Uniform(4.3, 7.9, 2));
    auto sepal_width =
        rules_set.add_input_variable<double>("sepal_width", fru::initial_distribution::Uniform(2.0, 4.4, 2));
    auto petal_length =
        rules_set.add_input_variable<double>("petal_length", fru::initial_distribution::Uniform(1.0, 6.9, 2));
    auto petal_width =
        rules_set.add_input_variable<double>("petal_width", fru::initial_distribution::Uniform(0.1, 2.5, 2));
    return std::tuple{rules_set, sepal_length, sepal_width, petal_length, petal_width};
  };
  auto sepal_length =
      rules_set.add_input_variable<double>("sepal_length", fru::initial_distribution::Uniform(v1[0], v1[1], 2));
  auto sepal_width =
      rules_set.add_input_variable<double>("sepal_width", fru::initial_distribution::Uniform(v1[2], v1[3], 2));
  auto petal_length =
      rules_set.add_input_variable<double>("petal_length", fru::initial_distribution::Uniform(v1[4], v1[5], 2));
  auto petal_width =
      rules_set.add_input_variable<double>("petal_width", fru::initial_distribution::Uniform(v1[6], v1[7], 2));
  return std::tuple{rules_set, sepal_length, sepal_width, petal_length, petal_width};
}

auto main(int argc, char **argv) -> int {
  CLI::App app{"App description"};
  argv = app.ensure_utf8(argv);

  std::string input_file = "";
  app.add_option("-i,--input", input_file, "Input file");
  std::string target_file = "";
  app.add_option("-t,--target", target_file, "Target file");
  std::vector<double> v1{};
  app.add_option<std::vector<double>>("--test_vector", v1, "Vector of model params");
  bool train = false;
  app.add_option("--train", train, "Train the model, false - test the model");
  bool print = false;
  app.add_option("--print", print, "Print internal results");

  CLI11_PARSE(app, argc, argv);

  auto [rules_set, sepal_length, sepal_width, petal_length, petal_width] = get_rules_set(v1);
  const auto output_variable =
      rules_set.add_output_variable("iris_type", {"Iris-setosa", "Iris-versicolor", "Iris-virginica"});

  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  auto add_rule = [&rules_set, sepal_length, sepal_width, petal_length, petal_width,
                   output_variable](size_t sepal_lenght_val, size_t sepal_width_val, size_t petal_length_val,
                                    size_t petal_width_val, size_t output_val) {
    // NOLINTEND(bugprone-easily-swappable-parameters)
    rules_set.add_rule({{sepal_length, sepal_lenght_val},
                        {sepal_width, sepal_width_val},
                        {petal_length, petal_length_val},
                        {petal_width, petal_width_val}},
                       output_variable[output_val]);
  };

  add_rule(small, small, small, small, setosa);     // 1
  add_rule(small, small, small, large, versicolor); // 2
  add_rule(small, small, large, small, versicolor); // 3
  add_rule(small, small, large, large, virginica);  // 4
  add_rule(small, large, small, small, setosa);     // 5
  add_rule(small, large, small, large, versicolor); // 6
  add_rule(small, large, large, small, setosa);     // 7
  add_rule(small, large, large, large, versicolor); // 8
  add_rule(large, small, small, small, versicolor); // 9
  add_rule(large, small, small, large, virginica);  // 10
  add_rule(large, small, large, small, versicolor); // 11
  add_rule(large, small, large, large, virginica);  // 12
  add_rule(large, large, small, small, versicolor); // 13
  add_rule(large, large, small, large, versicolor); // 14
  add_rule(large, large, large, small, versicolor); // 15
  add_rule(large, large, large, large, virginica);  // 16

  fre::SimpleReasoner reasoner{rules_set};
  const auto [features, targets] = fdd::load_data(input_file, target_file);

  if (!train) {
    fdd::DataSet data_set(features, targets);
    // const auto test_data = data_set.get_items(sepal_length, sepal_width, petal_length, petal_width);
    const auto test_data =
        data_set.get_items(std::pair{"sepal length", sepal_length}, std::pair{"sepal width", sepal_width},
                           std::pair{"petal length", petal_length}, std::pair{"petal width", petal_width});
    if (print) {
      fdd::print(test_data);
    }
    auto goal_func = calculate_one(test_data, reasoner, print);
    std::print("Goal function value : {}\n", goal_func);
  } else {
    const auto lower_bounds = Eigen::Matrix<double, 8, 1>(0.0, 7.9, 0.0, 4.4, 0.0, 6.9, -1.0, 2.5);
    const auto upper_bounds = Eigen::Matrix<double, 8, 1>(4.3, 15.0, 2.0, 15.0, 1.0, 15.0, 0.1, 15.0);

    fdd::DataSet data_set(features, targets);
    auto dataset_opt_fn = [sepal_length, sepal_width, petal_length, petal_width, &reasoner, &data_set, lower_bounds,
                           upper_bounds, print](const Eigen::VectorXd local_vals_inp, Eigen::VectorXd *, void *) {
      auto local_reasoner = reasoner;
      sepal_length.set_points(std::vector<double>{local_vals_inp(0), local_vals_inp(1)});
      sepal_width.set_points(std::vector<double>{local_vals_inp(2), local_vals_inp(3)});
      petal_length.set_points(std::vector<double>{local_vals_inp(4), local_vals_inp(5)});
      petal_width.set_points(std::vector<double>{local_vals_inp(6), local_vals_inp(7)});

      const auto test_data =
          data_set.get_items(std::pair{"sepal length", sepal_length}, std::pair{"sepal width", sepal_width},
                             std::pair{"petal length", petal_length}, std::pair{"petal width", petal_width});
      if (print)
        fdd::print(test_data);
      static double gfunc = double(test_data.size());
      const auto goal_func = calculate_one(test_data, local_reasoner, print);
      auto opt_target = double(test_data.size()) - goal_func;
      const auto min_span = 0.5;
      if (local_vals_inp(1) - local_vals_inp(0) < min_span || local_vals_inp(3) - local_vals_inp(2) < min_span ||
          local_vals_inp(5) - local_vals_inp(4) < min_span || local_vals_inp(7) - local_vals_inp(6) < min_span) {
        opt_target += 30.0;
      }
      for (int i = 0; i < 8; ++i) {
        if (local_vals_inp(i) < lower_bounds(i) || local_vals_inp(i) > upper_bounds(i)) {
          opt_target += 30.0;
        }
      }

      if (opt_target < gfunc) {
        std::print("Opt_target : {:.6f}\t goal_func {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t "
                   "{:.6f}\t {:.6f}\t\n ",
                   opt_target, goal_func, local_vals_inp(0), local_vals_inp(1), local_vals_inp(2), local_vals_inp(3),
                   local_vals_inp(4), local_vals_inp(5), local_vals_inp(6), local_vals_inp(7));
        gfunc = opt_target;
      }
      return opt_target;
    };

    const auto sepal_length_points = sepal_length.get_points();
    const auto sepal_width_points = sepal_width.get_points();
    const auto petal_length_points = petal_length.get_points();
    const auto petal_width_points = petal_width.get_points();

    Eigen::VectorXd x = Eigen::Matrix<double, 8, 1>(
        sepal_length_points[0], sepal_length_points[1], sepal_width_points[0], sepal_width_points[1],
        petal_length_points[0], petal_length_points[1], petal_width_points[0], petal_width_points[1]);

    optim::algo_settings_t settings;
    settings.de_settings.check_freq = 10;
    settings.print_level = 3;
    settings.de_settings.n_pop = 20;
    settings.de_settings.n_gen = 100;
    optim::de(x, dataset_opt_fn, &settings);
  }

  return 0;
}
