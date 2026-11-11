#include <benchmark/benchmark.h>

static void BM_Smoke(benchmark::State& state) {
    int x = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(x += 1);
    }
}
BENCHMARK(BM_Smoke);