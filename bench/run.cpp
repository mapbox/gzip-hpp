#include <benchmark/benchmark.h>
#include <fstream>
#include <gzip.hpp>

static std::string open_file(std::string const& filename)
{
    std::ifstream stream(filename,std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + filename + "'");
    }
    std::string data((std::istreambuf_iterator<char>(stream.rdbuf())),
                    std::istreambuf_iterator<char>());
    stream.close();
    return data;
}

static void BM_compress(benchmark::State& state) // NOLINT google-runtime-references
{
    std::string buffer = open_file("./bench/14-4685-6265.mvt");
    while (state.KeepRunning())
    {
        std::string value = gzip::compress(buffer.data(), buffer.size());
        benchmark::DoNotOptimize(value.data());
    }
};

BENCHMARK(BM_compress);

static void BM_decompress(benchmark::State& state) // NOLINT google-runtime-references
{
    std::string buffer_uncompressed = open_file("./bench/14-4685-6265.mvt");
    std::string buffer = gzip::compress(buffer_uncompressed.data(), buffer_uncompressed.size());
    while (state.KeepRunning())
    {
        std::string value = gzip::decompress(buffer.data(), buffer.size());
        benchmark::DoNotOptimize(value.data());
        benchmark::ClobberMemory();
    }
};

BENCHMARK(BM_decompress);

static void BM_compress2(benchmark::State& state) // NOLINT google-runtime-references
{
    std::string buffer = open_file("./bench/14-4685-6265.mvt");
    gzip::Compressor comp;

    while (state.KeepRunning())
    {
        std::string output;
        comp.compress(output, buffer.data(), buffer.size());
    }
};

BENCHMARK(BM_compress2);

static void BM_decompress2(benchmark::State& state) // NOLINT google-runtime-references
{

    std::string buffer_uncompressed = open_file("./bench/14-4685-6265.mvt");
    std::string buffer = gzip::compress(buffer_uncompressed.data(), buffer_uncompressed.size());
    gzip::Decompressor decomp;
    while (state.KeepRunning())
    {
        std::string output;
        decomp.decompress(output, buffer.data(), buffer.size());
    }
};

BENCHMARK(BM_decompress2);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
BENCHMARK_MAIN();
#pragma GCC diagnostic pop
