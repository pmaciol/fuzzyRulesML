# Simple fuzzy logic reasoner

This is a simple and still incomplete fuzzy logic reasoner. It is developed mainly for fun and training, so it is not a coprehensive solution. It is not intended for production use, but rather to be a learning tool.

## Features

* Focused on performance, so the data structures are optimized for speed.

* Modern C++ language, generally C++23 compiler is required

* Contains three main components:
  * fuzzy reasoner
  * machine learning example
  * unit tests

* Currently, fuzzy numbers might represent ints and doubles, but the data structures are designed to support other types, including non-POD types as well
* There is **a lot of room** for improvements, starting from codestyle and code completeness, from examples, to more features.

## Getting Started

### Prerequisites

* **CMake v3.23+** - found at [https://cmake.org/](https://cmake.org/)

* **C++ Compiler** - needs to support the **C++23** standard, i.e. *GCC-14*, *Clang-18*

* **Ninja** generator

* Tested on Ubuntu 24.04, but should work on any other Linux distribution providing required compilers. Windows should do as well, however the WSL support is recommended

### Installing & running

* download from the github
* prepare the environment with `cmake --preset gcc14_debug | clang18_debug | clang18_release`; other presets might be configured in [CMakePresets](./CMakePresets.json)
* build with `cmake --build ./build_clang18/ --target fuzzyRulesML | fuzzyRulesML_tests`
* run tests `./build_clang18/fuzzyRulesML_tests`
* for running an example with ML of iris database:
  * download the dataset `python3 ./download_iris.py`
  * train with `./build_clang18/fuzzyRulesML -i ./iris_features_train.json -t ./iris_targets_train.json --train 1 --print 0`
  * test with `./build_clang18/fuzzyRulesML -i ./iris_features_train.json -t ./iris_targets_train.json --train 0 --print 1 --test_vector __values__of__the__test__vector`; eg. `--test_vector 4.198039 13.111611 1.560437 12.724000 0.636190 7.633797 -0.786421 2.586373`
* some other tools for developers:
  * formatting the code: `cmake --build ./build_clang18/ --target format`; the style is defined in [.clang-format](./.clang-format)
  * runnig static checks: `cmake --build ./build_clang18/ --target tidy`; the style is defined in [.clang-tidy](./.clang-tidy)


## Contributing

If you would like to contribute in any way, I would suggest to contatct me to discuss the paths to enhance this tiny tool

## Author

* **Piotr Macioł** - [@pmaciol](https://github.com/pmaciol)

## License

This project is licensed under the [GNU Lesser General Public License v2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) - see the
[LICENSE](LICENSE.md) file for details