cmake_minimum_required(VERSION 3.28)
project(fuzzyRulesML VERSION 0.0.1 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
find_package(GTest CONFIG REQUIRED COMPONENTS GTest GMock)

file(GLOB LIB_SRC "${CMAKE_SOURCE_DIR}/lib/*.cpp")
file(GLOB LIB_H "${CMAKE_SOURCE_DIR}/lib/*.hpp")
set(MAIN_FILE "${CMAKE_SOURCE_DIR}/main.cpp")
set(ALL_FILES ${MAIN_FILE} ${LIB_SRC} ${LIB_H}) 
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

set(TESTS_DIR "${CMAKE_SOURCE_DIR}/tests")
file(GLOB TEST_SRC "${TESTS_DIR}/*.cpp")

add_custom_target(tidy
    COMMAND
        run-clang-tidy -p ./ ${ALL_FILES} -header-filter=..*
    VERBATIM
)

add_custom_target(format
    COMMAND
        clang-format -i ${ALL_FILES}
    VERBATIM
)

add_executable(fuzzyRulesML ${MAIN_FILE} ${LIB_SRC} ${LIB_H})
target_compile_options(fuzzyRulesML PRIVATE -Werror -Wall -Wextra)
target_include_directories(fuzzyRulesML PUBLIC
/usr/include/eigen3/ /usr/local/include/optim/
)
target_link_libraries(fuzzyRulesML optim)

add_executable(fuzzyRulesML_tests ${TEST_SRC} ${LIB_SRC})
target_link_libraries(fuzzyRulesML_tests GTest::gtest GTest::gtest_main)
target_include_directories(fuzzyRulesML_tests PUBLIC
${CMAKE_SOURCE_DIR}/lib/ /usr/include/eigen3/ ~/src/fuzzyRulesML/lib/
)
add_test(NAME fuzzyRulesML_tests COMMAND fuzzyRulesML_tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

