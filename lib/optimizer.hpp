#define OPTIM_ENABLE_EIGEN_WRAPPERS
#include "optim/optim.hpp"

inline auto ackley_fn(const Eigen::VectorXd& vals_inp, Eigen::VectorXd * /*unused*/, void * /*unused*/) -> double {
  const double x_value = vals_inp(0);
  const double y_value = vals_inp(1);

  const double obj_val = 20 + std::numbers::e - 20 * std::exp(-0.2 * std::sqrt(0.5 * (x_value * x_value + y_value * y_value))) -
                         std::exp(0.5 * (std::cos(2 * std::numbers::pi * x_value) + std::cos(2 * std::numbers::pi * y_value)));

  return obj_val;
}