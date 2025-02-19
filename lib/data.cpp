#include "data.hpp"
#include <string>
#include <string_view>

namespace fuzzyrulesml::data {
auto load_data(const std::string_view /*unused*/) -> std::string { return "hello modules!"; }
}; // namespace fuzzyrulesml::data