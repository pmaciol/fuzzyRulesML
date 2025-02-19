#pragma once
#include <string>
#include <string_view>

namespace fuzzyrulesml::data {
auto load_data(std::string_view filename) -> std::string;
}