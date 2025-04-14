#include "lib/dataset.hpp"
#include "lib/optimizer.hpp"
#include "lib/reasoner.hpp"
#include "lib/rules.hpp"
#include <CLI/CLI.hpp>
#include <string>
#include <tuple>

namespace fru = fuzzyrulesml::rules;
namespace fre = fuzzyrulesml::reasoner;
namespace fdd = fuzzyrulesml::dataset;

const int small = 0;
const int large = 1;
const int setosa = 0;
const int versicolor = 1;
const int virginica = 2;

namespace {
auto get_rules_set(std::vector<double> vector_variables) {
  fru::RulesSet rules_set;
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  if (vector_variables.empty()) {
    auto sepal_length = rules_set.add_input_variable("sepal_length", fru::initial_distribution::Uniform(4.3, 7.9, 2));
    auto sepal_width = rules_set.add_input_variable("sepal_width", fru::initial_distribution::Uniform(2.0, 4.4, 2));
    auto petal_length = rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(1.0, 6.9, 2));
    auto petal_width = rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(0.1, 2.5, 2));
    return std::tuple{rules_set, sepal_length, sepal_width, petal_length, petal_width};
  };
  auto sepal_length =
      rules_set.add_input_variable("sepal_length", fru::initial_distribution::Uniform(vector_variables[0], vector_variables[1], 2));
  auto sepal_width =
      rules_set.add_input_variable("sepal_width", fru::initial_distribution::Uniform(vector_variables[2], vector_variables[3], 2));
  auto petal_length =
      rules_set.add_input_variable("petal_length", fru::initial_distribution::Uniform(vector_variables[4], vector_variables[5], 2));
  auto petal_width =
      rules_set.add_input_variable("petal_width", fru::initial_distribution::Uniform(vector_variables[6], vector_variables[7], 2));
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return std::tuple{rules_set, sepal_length, sepal_width, petal_length, petal_width};
}
} // namespace

auto run_test(const auto features, const auto targets, const auto sepal_length, const auto sepal_width, const auto petal_length,
              const auto petal_width, const auto reasoner, const bool print) -> void {
  fdd::DataSet data_set(features, targets);
  const auto test_data = data_set.get_items(std::pair{"sepal length", sepal_length}, std::pair{"sepal width", sepal_width},
                                            std::pair{"petal length", petal_length}, std::pair{"petal width", petal_width});
  if (print) {
    test_data.print();
  }
  auto goal_func = calculate_one(test_data, reasoner, print);
  std::print("Goal function value : {}\n", goal_func);
}

auto run_training(const auto features, const auto targets, auto sepal_length, auto sepal_width, auto petal_length, auto petal_width,
                  auto reasoner, const bool print) -> void {
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  const auto lower_bounds = Eigen::Matrix<double, 8, 1>(0.0, 7.9, 0.0, 4.4, 0.0, 6.9, -1.0, 2.5);
  const auto upper_bounds = Eigen::Matrix<double, 8, 1>(4.3, 15.0, 2.0, 15.0, 1.0, 15.0, 0.1, 15.0);
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  fdd::DataSet data_set(features, targets);
  // NOLINTBEGIN(performance-unnecessary-value-param)
  auto dataset_opt_fn = [sepal_length, sepal_width, petal_length, petal_width, &reasoner, &data_set, lower_bounds, upper_bounds,
                         print](const Eigen::VectorXd local_vals_inp, Eigen::VectorXd *, void *) {
    // NOLINTEND(performance-unnecessary-value-param)
    auto local_reasoner = reasoner;
    auto sepal_length_local = sepal_length;
    auto sepal_width_local = sepal_width;
    auto petal_length_local = petal_length;
    auto petal_width_local = petal_width;

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    sepal_length_local.set_points({local_vals_inp(0), local_vals_inp(1)});
    sepal_width_local.set_points({local_vals_inp(2), local_vals_inp(3)});
    petal_length_local.set_points({local_vals_inp(4), local_vals_inp(5)});
    petal_width_local.set_points({local_vals_inp(6), local_vals_inp(7)});
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    const auto test_data = data_set.get_items(std::pair{"sepal length", sepal_length_local}, std::pair{"sepal width", sepal_width_local},
                                              std::pair{"petal length", petal_length_local}, std::pair{"petal width", petal_width_local});
    if (print) {
      test_data.print();
    }
    static auto gfunc = static_cast<double>(test_data.size());
    const auto goal_func = calculate_one(test_data, local_reasoner, print);
    auto opt_target = double(test_data.size()) - goal_func;
    const auto min_span = 0.5;
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
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
                 opt_target, goal_func, local_vals_inp(0), local_vals_inp(1), local_vals_inp(2), local_vals_inp(3), local_vals_inp(4),
                 local_vals_inp(5), local_vals_inp(6), local_vals_inp(7));
      gfunc = opt_target;
      if (opt_target <= 3.1) {
        std::print("Computations finished\n");
        exit(0);
      }
      // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    }
    return opt_target;
  };

  const auto sepal_length_points = sepal_length.get_points();
  const auto sepal_width_points = sepal_width.get_points();
  const auto petal_length_points = petal_length.get_points();
  const auto petal_width_points = petal_width.get_points();

  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  Eigen::VectorXd start_vector =
      Eigen::Matrix<double, 8, 1>(sepal_length_points[0], sepal_length_points[1], sepal_width_points[0], sepal_width_points[1],
                                  petal_length_points[0], petal_length_points[1], petal_width_points[0], petal_width_points[1]);

  optim::algo_settings_t settings;
  settings.de_settings.check_freq = 10;
  settings.print_level = 3;
  settings.de_settings.n_pop = 20;
  settings.de_settings.n_gen = 100;
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  optim::de(start_vector, dataset_opt_fn, &settings);
}

auto main(int argc, char **argv) -> int {
  try {
    CLI::App app{"App description"};
    argv = app.ensure_utf8(argv);

    std::string input_file;
    app.add_option("-i,--input", input_file, "Input file");
    std::string target_file;
    app.add_option("-t,--target", target_file, "Target file");
    std::vector<double> test_vector{};
    app.add_option<std::vector<double>>("--test_vector", test_vector, "Vector of model params");
    bool train = false;
    app.add_option("--train", train, "Train the model, false - test the model");
    bool print = false;
    app.add_option("--print", print, "Print internal results");

    CLI11_PARSE(app, argc, argv);

    auto [rules_set, sepal_length, sepal_width, petal_length, petal_width] = get_rules_set(test_vector);
    const auto output_variable = rules_set.add_output_variable("iris_type", {"Iris-setosa", "Iris-versicolor", "Iris-virginica"});

    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    auto add_rule = [&rules_set, sepal_length, sepal_width, petal_length, petal_width,
                     output_variable](size_t sepal_lenght_val, size_t sepal_width_val, size_t petal_length_val, size_t petal_width_val,
                                      size_t output_val) {
      // NOLINTEND(bugprone-easily-swappable-parameters)
      rules_set.add_rule({{sepal_length, sepal_lenght_val},
                          {sepal_width, sepal_width_val},
                          {petal_length, petal_length_val},
                          {petal_width, petal_width_val}},
                         {"iris_type", output_variable.get_categories()[output_val]});
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

    const fre::SimpleReasoner reasoner{rules_set};
    const auto [features, targets] = fdd::load_data(input_file, target_file);

    if (train) {
      run_training(features, targets, sepal_length, sepal_width, petal_length, petal_width, reasoner, print);
    } else {
      run_test(features, targets, sepal_length, sepal_width, petal_length, petal_width, reasoner, print);
    }
  } catch (const CLI::ParseError& parse_error) {
    std::cerr << "Parse error: " << parse_error.what() << "\n";
    return 1;
  } catch (const std::exception& except) {
    std::cerr << "Error: " << except.what() << '\n';
    return 1;
  } catch (...) {
    std::cerr << "Unknown error occurred\n";
    return 1;
  }

  return 0;
}
