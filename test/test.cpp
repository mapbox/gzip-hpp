#include <gzip.hpp>

#include <cassert>
#include <iostream>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <limits>
#include <fstream>

TEST_CASE("test version")
{
    REQUIRE(GZIP_VERSION_STRING == std::string("1.0.0"));
}

TEST_CASE("successful compress - pointer")
{
    std::string data = "hello hello hello hello";
    const char* pointer = data.data();

    std::string value = gzip::compress(pointer, data.size());
    REQUIRE(!value.empty());
}

TEST_CASE("fail compress - throws max size limit")
{
    std::string data = "hello hello hello hello";
    const char* pointer = data.data();

    unsigned long l = 2000000001;

    CHECK_THROWS_WITH(gzip::compress(pointer, l), Catch::Contains("size may use more memory than intended when decompressing"));
}

#ifdef DEBUG
TEST_CASE("fail compress - pointer, debug throws int overflow")
{
    std::string data = "hello hello hello hello";
    const char* pointer = data.data();
    // numeric_limit useful for integer conversion
    unsigned int i = std::numeric_limits<unsigned int>::max();
    // turn int i into a long, so we can add to it safely without overflow
    unsigned long l = static_cast<unsigned long>(i) + 1;

    CHECK_THROWS_WITH(gzip::compress(pointer, l), Catch::Contains("size arg is too large to fit into unsigned int type"));
}
#endif

TEST_CASE("successful decompress - pointer")
{
    std::string data = "hello hello hello hello";
    const char* pointer = data.data();
    std::string compressed_data = gzip::compress(pointer, data.size());
    const char* compressed_pointer = compressed_data.data();

    std::string value = gzip::decompress(compressed_pointer, data.size());
    REQUIRE(data == value);
}

#ifdef DEBUG
TEST_CASE("fail decompress - pointer, debug throws int overflow")
{
    std::string data = "hello hello hello hello";
    const char* pointer = data.data();
    std::string compressed_data = gzip::compress(pointer, data.size());
    const char* compressed_pointer = compressed_data.data();

    // numeric_limit useful for integer conversion
    unsigned int i = std::numeric_limits<unsigned int>::max();
    // turn int i into a long, so we can add to it safely without overflow
    unsigned long l = static_cast<unsigned long>(i) + 1;

    CHECK_THROWS_WITH(gzip::decompress(compressed_pointer, l), Catch::Contains("size arg is too large to fit into unsigned int type x2"));
}
#endif

TEST_CASE("invalid decompression")
{
    std::string data("this is a string that should be compressed data");
    const char* pointer = data.data();
    // data is not compressed but we will try to decompress it

    CHECK_THROWS(gzip::decompress(pointer, data.size()));
}

TEST_CASE("round trip compression - gzip")
{
    const std::string data("this is a sentence that will be compressed into something");
    const char* pointer = data.data();

    CHECK(!gzip::is_compressed(pointer, data.size()));

    int strategy;

    SECTION("strategy - invalid compression")
    {
        strategy = 99;
        int level = Z_DEFAULT_COMPRESSION;

        CHECK_THROWS(gzip::compress(pointer, data.size(), level, strategy));
    }

    SECTION("compression level - invalid")
    {
        strategy = Z_DEFAULT_STRATEGY;
        int level = 99;

        CHECK_THROWS(gzip::compress(pointer, data.size(), level, strategy));
    }

    SECTION("strategy - default")
    {
        strategy = Z_DEFAULT_STRATEGY;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char* compressed_pointer = compressed_data.data();
                CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
                std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - filtered")
    {
        strategy = Z_FILTERED;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char* compressed_pointer = compressed_data.data();
                CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
                std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - huffman only")
    {
        strategy = Z_HUFFMAN_ONLY;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char* compressed_pointer = compressed_data.data();
                CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
                std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - rle")
    {
        strategy = Z_RLE;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char* compressed_pointer = compressed_data.data();
                CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
                std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - fixed")
    {
        strategy = Z_FIXED;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char* compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char* compressed_pointer = compressed_data.data();
                CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
                std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
                CHECK(data == new_data);
            }
        }
    }
}

TEST_CASE("low level interface works as expected")
{
    // compress
    std::string data = "hello hello hello hello 8923uropasjdflajhsi hp97yd98fy93kjnn a asdfasdf0000000 asdfasdfasd f2rqwrtddddddddd ";
    std::string result1 = gzip::compress(data.data(),data.size());
    gzip::Compressor comp;
    std::string result2;
    comp.compress(result2,data.data(),data.size());
    std::size_t compressed_size = result2.size();
    CHECK(result1.size() == compressed_size);
    std::string result2_trimmed = result2.substr(0,compressed_size);
    CHECK(result1 == result2_trimmed);
    CHECK(result1.size() == result2_trimmed.size());

    // test re-used scratch space/arena during compression
    std::size_t HEADER_LENGTH = 20;
    std::vector<std::pair<std::string,std::size_t>> strings = {
        {"", 0 + HEADER_LENGTH}
        ,{" ", 1 + HEADER_LENGTH}
        ,{"  ", 2 + HEADER_LENGTH}
        ,{"", 0 + HEADER_LENGTH}
    };
    std::string arena;
    std::size_t last_size = 0;
    for (auto const& d : strings)
    {
        comp.compress(arena,d.first.data(),d.first.size());
        std::size_t c_size = arena.size();
        CHECK(c_size == d.second);
        if (last_size > 0) {
           CHECK(arena.size() >= last_size);
        }
        last_size = arena.size();
    }

    // decompress
    gzip::Decompressor decomp;
    // test re-used scratch space/arena during decompression
    std::vector<std::pair<std::string,std::size_t>> decomp_strings = {
        {"", 0}
        ,{" ", 1}
        ,{"  ", 2}
        ,{"", 0}
    };
    std::string decomp_arena;
    std::size_t decomp_last_size = 0;
    std::size_t decomp_last_capacity = 0;
    std::size_t reserve_size = 50;
    std::size_t RESERVE_EXTRA = 13;
    decomp_arena.reserve(reserve_size);
    for (auto const& d : decomp_strings)
    {
        std::string value = gzip::compress(d.first.data(),d.first.size());
        decomp.decompress(decomp_arena,value.data(),value.size());
        std::size_t c_size = decomp_arena.size();
        CHECK(c_size == d.first.size());
        CHECK(decomp_arena.size() == d.second);
        decomp_last_size = decomp_arena.size();
        decomp_last_capacity = decomp_arena.capacity();
        CHECK(decomp_last_capacity == reserve_size+RESERVE_EXTRA);
    }

}

TEST_CASE("test decompression size limit")
{
    std::string filename("./test/data/highly_compressed.gz");
    std::ifstream stream(filename,std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + filename + "'");
    }
    std::string str_compressed((std::istreambuf_iterator<char>(stream.rdbuf())),
                    std::istreambuf_iterator<char>());
    stream.close();

    std::size_t limit = 20 * 1024 * 1024; // 20 Mb
    // file should be about 500 mb uncompressed
    gzip::Decompressor decomp(limit);
    std::string output;
    CHECK_THROWS(decomp.decompress(output, str_compressed.data(), str_compressed.size()));
    CHECK(output.size() < limit);
}
