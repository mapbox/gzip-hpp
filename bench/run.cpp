#include <benchmark/benchmark.h>
#include <fstream>
#include <gzip.hpp>

auto BM_compress = [](benchmark::State& state, const char * data, size_t length) // NOLINT google-runtime-references
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
};

auto BM_decompress = [](benchmark::State& state, const char * data, size_t length) // NOLINT google-runtime-references
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
};


auto BM_compress2 = [](benchmark::State& state, const char * data, size_t length) // NOLINT google-runtime-references
{
    gzip::Compressor comp;
    std::string arena;

    if (state.thread_index == 0)
    {

    }
    while (state.KeepRunning())
    {
        std::size_t compressed_size = comp.compress(arena, data, length);
        arena.resize(compressed_size);
        benchmark::DoNotOptimize(arena.data());
        benchmark::ClobberMemory();
    }
    if (state.thread_index == 0)
    {
        // Teardown code here.
    }
};

auto BM_decompress2 = [](benchmark::State& state, const char * data, size_t length) // NOLINT google-runtime-references
{

    gzip::Decompressor decomp;
    std::string arena;
    if (state.thread_index == 0)
    {

    }
    while (state.KeepRunning())
    {
        std::size_t uncompressed_size = decomp.decompress(arena, data, length);
        arena.resize(uncompressed_size);
        benchmark::DoNotOptimize(arena.data());
        benchmark::ClobberMemory();
    }
    if (state.thread_index == 0)
    {
        // Teardown code here.
    }
};

int main(int argc, char* argv[])
{
    std::string filename("./bench/14-4685-6265.mvt");
    std::ifstream stream(filename,std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + filename + "'");
    }
    std::string str_uncompressed((std::istreambuf_iterator<char>(stream.rdbuf())),
                    std::istreambuf_iterator<char>());
    stream.close();

    std::string str_compressed = gzip::compress(str_uncompressed.data(), str_uncompressed.size());

    benchmark::RegisterBenchmark("compress high level API", BM_compress, str_uncompressed.data(), str_uncompressed.size())->Threads(1)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::RegisterBenchmark("compress low level API", BM_compress2, str_uncompressed.data(), str_uncompressed.size())->Threads(1)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::RegisterBenchmark("decompress high level API", BM_decompress, str_compressed.data(), str_compressed.size())->Threads(1)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeak
    benchmark::RegisterBenchmark("decompress low level API", BM_decompress2, str_compressed.data(), str_compressed.size())->Threads(1)->Threads(2)->Threads(4)->Threads(8); // NOLINT clang-analyzer-cplusplus.NewDeleteLeak
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}