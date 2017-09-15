#include <benchmark/benchmark.h>
#include <gzip.hpp>
#include <fstream>

static void BM_compress(benchmark::State& state) // NOLINT google-runtime-references
{
    const char * pointer;
    size_t size;

    if (state.thread_index == 0)
    {
        std::ifstream t("test.txt");
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        pointer = str.data();
        size = str.size();
    }
    while (state.KeepRunning())
    {
        std::string value = gzip::compress(pointer, size);
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