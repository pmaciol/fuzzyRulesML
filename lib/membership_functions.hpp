#pragma once

#include <map>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

namespace fuzzyrulesml {
class Membership {
public:
  [[nodiscard]] auto begin() const { return memberships.begin(); };
  [[nodiscard]] auto end() const { return memberships.end(); }
  [[nodiscard]] auto operator[](std::size_t index) -> double& { return memberships[index]; }
  [[nodiscard]] auto size() const -> std::size_t { return memberships.size(); }
  [[nodiscard]] auto get_membership(std::size_t index) const -> double {
    auto found = memberships.find(index);
    if (found != memberships.end()) {
      return found->second;
    }
    return 0.0;
  }

  [[nodiscard]] auto get_memberships() const -> std::map<std::size_t, double> { return memberships; }

private:
  std::map<std::size_t, double> memberships;
};
} // namespace fuzzyrulesml
namespace fuzzyrulesml::mfunct {
template <typename VARIABLE> class LinearMemberFunct {
public:
  LinearMemberFunct(VARIABLE left_bound, VARIABLE peak, VARIABLE right_bound)
      : left_bound(left_bound), peak(peak), right_bound(right_bound) {
    if (left_bound >= peak || peak >= right_bound) {
      throw std::runtime_error("Invalid bounds");
    }
  }

  auto operator()(VARIABLE value) const -> std::optional<VARIABLE> {
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
  [[nodiscard]] auto get_left_bound() const -> VARIABLE { return left_bound; }
  [[nodiscard]] auto get_peak() const -> VARIABLE { return peak; }
  [[nodiscard]] auto get_right_bound() const -> VARIABLE { return right_bound; }

  [[nodiscard]] auto get_points() const -> std::vector<VARIABLE> { return {left_bound, peak, right_bound}; }
  void set_points(const VARIABLE left_bound, const VARIABLE peak, const VARIABLE right_bound) {
    this->left_bound = left_bound;
    this->peak = peak;
    this->right_bound = right_bound;
  }

private:
  VARIABLE left_bound;
  VARIABLE peak;
  VARIABLE right_bound;
};

template <typename VARIABLE> class LinearMemberFunctAsc {
public:
  LinearMemberFunctAsc(VARIABLE left_bound, VARIABLE right_bound) : left_bound(left_bound), right_bound(right_bound) {
    if (left_bound >= right_bound) {
      throw std::runtime_error("Invalid bounds");
    }
  }

  [[nodiscard]] auto operator()(VARIABLE value) const -> std::optional<VARIABLE> {
    if (value <= left_bound || value > right_bound) {
      return std::nullopt;
    }
    return (value - left_bound) / (right_bound - left_bound);
  }
  [[nodiscard]] auto get_points() const -> std::vector<VARIABLE> { return {left_bound, right_bound}; }
  void set_points(const VARIABLE left_bound, const VARIABLE right_bound) {
    this->left_bound = left_bound;
    this->right_bound = right_bound;
  }

private:
  VARIABLE left_bound;
  VARIABLE right_bound;
};

template <typename VARIABLE> class LinearMemberFunctDesc {
public:
  LinearMemberFunctDesc(VARIABLE left_bound, VARIABLE right_bound) : left_bound(left_bound), right_bound(right_bound) {
    if (left_bound >= right_bound) {
      throw std::runtime_error("Invalid bounds");
    }
  }

  [[nodiscard]] auto operator()(VARIABLE value) const -> std::optional<VARIABLE> {
    if (value < left_bound || value >= right_bound) {
      return std::nullopt;
    }
    return (right_bound - value) / (right_bound - left_bound);
  }

  [[nodiscard]] auto get_points() const -> std::vector<VARIABLE> { return {left_bound, right_bound}; }
  void set_points(const double left_bound, const double right_bound) {
    this->left_bound = left_bound;
    this->right_bound = right_bound;
  }

private:
  VARIABLE left_bound;
  VARIABLE right_bound;
};

template <typename VARIABLE> class LinearDistribution {
public:
  LinearDistribution(const LinearMemberFunctDesc<VARIABLE>& lfunction, const std::vector<LinearMemberFunct<VARIABLE>>& functions,
                     const LinearMemberFunctAsc<VARIABLE>& rfunction)
      : left_function{lfunction}, mid_functions(functions), right_function{rfunction} {}

  [[nodiscard]] auto operator()(VARIABLE value) const -> Membership {
    Membership memberships;
    const auto left_boundary = left_function.get_points()[0];
    if (value < left_boundary) {
      memberships[0] = 1.0;
      return memberships;
    }
    const auto& left_function_value = left_function(value);
    if (left_function_value.has_value()) {
      memberships[0] = left_function_value.value();
    }
    int index = 1;
    memberships = std::accumulate(mid_functions.begin(), mid_functions.end(), memberships,
                                  [&value, &index](Membership memberships, const LinearMemberFunct<VARIABLE>& function) {
                                    if (function(value)) {
                                      memberships[index++] = function(value).value();
                                    } else {
                                      ++index;
                                    }
                                    return memberships;
                                  });
    const auto& right_function_value = right_function(value);
    if (right_function_value.has_value()) {
      memberships[mid_functions.size() + 1] = right_function_value.value();
    }
    const auto right_boundary = right_function.get_points()[1];
    if (value >= right_boundary) {
      memberships[mid_functions.size() + 1] = 1.0;
    }
    return memberships;
  };
  [[nodiscard]] auto get_categories() const -> std::size_t { return mid_functions.size() + 2; }
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

  [[nodiscard]] auto get_points() const -> std::vector<double> {
    std::vector<double> points;
    points.push_back(left_function.get_points()[0]);
    for (const auto& function : mid_functions) {
      points.push_back(function.get_peak());
    }
    points.push_back(right_function.get_points()[1]);
    return points;
  }

private:
  LinearMemberFunctDesc<VARIABLE> left_function;
  std::vector<LinearMemberFunct<VARIABLE>> mid_functions;
  LinearMemberFunctAsc<VARIABLE> right_function;
};
}; // namespace fuzzyrulesml::mfunct