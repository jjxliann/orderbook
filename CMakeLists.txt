cmake_minimum_required(VERSION 3.20)
project(orderbook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(OB_SANITIZERS "Enable ASan/UBSan" ON)

add_library(ob_options INTERFACE)
target_compile_options(ob_options INTERFACE -Wall -Wextra -Wpedantic -Wconversion)
if(OB_SANITIZERS)
  target_compile_options(ob_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(ob_options INTERFACE -fsanitize=address,undefined)
endif()

include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz)
FetchContent_MakeAvailable(googletest)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.0.tar.gz)
FetchContent_MakeAvailable(benchmark)

enable_testing()

# Engine library (add sources as you write them)
add_library(ob INTERFACE)
target_include_directories(ob INTERFACE include)
target_link_libraries(ob INTERFACE ob_options)

add_executable(ob_tests tests/smoke_test.cpp)
target_link_libraries(ob_tests PRIVATE ob GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ob_tests)

add_executable(ob_bench bench/smoke_bench.cpp)
target_link_libraries(ob_bench PRIVATE ob benchmark::benchmark_main)