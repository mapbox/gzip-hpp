#include <benchmark/benchmark.h>
#include <gzip.hpp>

static void BM_compress(benchmark::State& state) // NOLINT google-runtime-references
{
    if (state.thread_index == 0)
    {

    }
    while (state.KeepRunning())
    {
        std::string data = "hello";
        const char * pointer = data.data();
        std::string value = gzip::compress(pointer, data.size());
        benchmark::DoNotOptimize(value.data());
        benchmark::ClobberMemory();
    }
    if (state.thread_index == 0)
    {
        // Teardown code here.
    }
}

int main(int argc, char* argv[])
{
    benchmark::RegisterBenchmark("BM_compress", BM_compress)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}