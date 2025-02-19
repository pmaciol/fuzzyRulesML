#pragma once

#include <map>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

namespace fuzzyrulesml::mfunct {
class LinearMemberFunct {
public:
  LinearMemberFunct(double left_bound, double peak, double right_bound)
      : left_bound(left_bound), peak(peak), right_bound(right_bound) {
    if (left_bound >= peak || peak >= right_bound) {
      throw std::runtime_error("Invalid bounds");
    }
  }

  auto operator()(double value) const -> std::optional<double> {
    if (value <= left_bound || value >= right_bound) {
      return std::nullopt;
    }
    if (value == peak) {
      return 1.0;
    }
    if (value < peak) {
      return (value - left_bound) / (peak - left_bound);
    }
    return (right_bound - value) / (right_bound - peak);
  }
  auto get_left_bound() const -> double { return left_bound; }
  auto get_peak() const -> double { return peak; }
  auto get_right_bound() const -> double { return right_bound; }

  auto get_points() const -> std::vector<double> { return {left_bound, peak, right_bound}; }
  void set_points(const double left_bound, const double peak, const double right_bound) {
    this->left_bound = left_bound;
    this->peak = peak;
    this->right_bound = right_bound;
  }

private:
  double left_bound;
  double peak;
  double right_bound;
};

class LinearMemberFunctAsc {
public:
  LinearMemberFunctAsc(double left_bound, double right_bound) : left_bound(left_bound), right_bound(right_bound) {
    if (left_bound >= right_bound) {
      throw std::runtime_error("Invalid bounds");
    }
  }

  auto operator()(double value) const -> std::optional<double> {
    if (value <= left_bound || value > right_bound) {
      return std::nullopt;
    }
    return (value - left_bound) / (right_bound - left_bound);
  }
  auto get_points() const -> std::vector<double> { return {left_bound, right_bound}; }
  void set_points(const double left_bound, const double right_bound) {
    this->left_bound = left_bound;
    this->right_bound = right_bound;
  }

private:
  double left_bound;
  double right_bound;
};

class LinearMemberFunctDesc {
public:
  LinearMemberFunctDesc(double left_bound, double right_bound) : left_bound(left_bound), right_bound(right_bound) {
    if (left_bound >= right_bound) {
      throw std::runtime_error("Invalid bounds");
    }
  }

  auto operator()(double value) const -> std::optional<double> {
    if (value < left_bound || value >= right_bound) {
      return std::nullopt;
    }
    return (right_bound - value) / (right_bound - left_bound);
  }

  auto get_points() const -> std::vector<double> { return {left_bound, right_bound}; }
  void set_points(const double left_bound, const double right_bound) {
    this->left_bound = left_bound;
    this->right_bound = right_bound;
  }

private:
  double left_bound;
  double right_bound;
};

class LinearDistribution {
public:
  LinearDistribution(const LinearMemberFunctDesc& lfunction, const std::vector<LinearMemberFunct>& functions,
                     const LinearMemberFunctAsc& rfunction)
      : left_function{lfunction}, mid_functions(functions), right_function{rfunction} {}

  auto operator()(double value) const -> std::map<int, double> {
    std::map<int, double> memberships;
    const auto left_boundary = left_function.get_points()[0];
    if (value < left_boundary) {
      memberships[0] = 1.0;
      return memberships;
    }
    
    if (left_function(value)) {
      memberships[0] = left_function(value).value();
    }
    int index = 1;
    memberships =
        std::accumulate(mid_functions.begin(), mid_functions.end(), memberships,
                        [&value, &index](std::map<int, double> memberships, const LinearMemberFunct& function) {
                          if (function(value)) {
                            memberships[index++] = function(value).value();
                          } else {
                            ++index;
                          }
                          return memberships;
                        });
    if (right_function(value)) {
      memberships[mid_functions.size() + 1] = right_function(value).value();
    }
    const auto right_boundary = right_function.get_points()[1];
    if (value >= right_boundary) {
      memberships[mid_functions.size() + 1] = 1.0;
    }
    return memberships;
  };
  auto get_categories() const -> std::size_t { return mid_functions.size() + 2; }
  void set_points(const std::vector<double>& characteristic_points) {
    if (characteristic_points.size() != mid_functions.size() + 2) {
      throw std::runtime_error("Invalid number of characteristic points");
    }
    left_function.set_points(characteristic_points[0], characteristic_points[1]);
    for (std::size_t i = 0; i < mid_functions.size(); ++i) {
      mid_functions[i].set_points(characteristic_points[i], characteristic_points[i + 1], characteristic_points[i + 2]);
    }
    right_function.set_points(characteristic_points[characteristic_points.size() - 2], characteristic_points.back());
  }

  auto get_points() const -> std::vector<double> {
    std::vector<double> points;
    points.push_back(left_function.get_points()[0]);
    for (const auto& function : mid_functions) {
      points.push_back(function.get_peak());
    }
    points.push_back(right_function.get_points()[1]);
    return points;
  }

private:
  LinearMemberFunctDesc left_function;
  std::vector<LinearMemberFunct> mid_functions;
  LinearMemberFunctAsc right_function;
};
}; // namespace fuzzyrulesml::mfunct