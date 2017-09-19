#include <benchmark/benchmark.h>
#include <gzip.hpp>
#include <fstream>

static void BM_compress(benchmark::State& state, const char * data, size_t length) // NOLINT google-runtime-references
{

    if (state.thread_index == 0)
    {

    }
    while (state.KeepRunning())
    {
        std::string value = gzip::compress(data, length);
        benchmark::DoNotOptimize(value.data());
        benchmark::ClobberMemory();
    }
    if (state.thread_index == 0)
    {
        // Teardown code here.
    }
}

static void BM_decompress(benchmark::State& state, const char * data, size_t length) // NOLINT google-runtime-references
{

    if (state.thread_index == 0)
    {

    }
    while (state.KeepRunning())
    {
        std::string value = gzip::decompress(data, length);
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
    std::ifstream t("14-4685-6265.mvt");
    std::string str_uncompressed((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

    std::string str_compressed = gzip::compress(str_uncompressed.data(), str_uncompressed.size());

    benchmark::RegisterBenchmark("BM_compress", BM_compress, str_uncompressed.data(), str_uncompressed.size())->Threads(1)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::RegisterBenchmark("BM_decompress", BM_decompress, str_compressed.data(), str_compressed.size())->Threads(1)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}