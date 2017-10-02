#include <gzip.hpp>

#include <cassert>
#include <iostream>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <limits>

TEST_CASE("test version") {
    REQUIRE(GZIP_VERSION_STRING == std::string("1.0.0"));
}

TEST_CASE("successful compress - pointer") {
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();

    std::string value = gzip::compress(pointer, data.size());
    REQUIRE(!value.empty());
}

TEST_CASE("fail compress - throws max size limit") {
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();

    unsigned long l = 2000000001; 

    CHECK_THROWS_WITH(gzip::compress(pointer, l), Catch::Contains("size may use more memory than intended when decompressing"));
}

#ifdef DEBUG
TEST_CASE("fail compress - pointer, debug throws int overflow") {
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();
    // numeric_limit useful for integer conversion
    unsigned int i = std::numeric_limits<unsigned int>::max();
    // turn int i into a long, so we can add to it safely without overflow
    unsigned long l = static_cast<unsigned long>(i) + 1; 
    
    CHECK_THROWS_WITH(gzip::compress(pointer, l), Catch::Contains("size arg is too large to fit into unsigned int type"));

}
#endif

TEST_CASE("successful decompress - pointer") {
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();
    std::string compressed_data = gzip::compress(pointer, data.size());
    const char * compressed_pointer = compressed_data.data();

    std::string value = gzip::decompress(compressed_pointer, data.size());
    REQUIRE(data == value);
}

#ifdef DEBUG
TEST_CASE("fail decompress - pointer, debug throws int overflow") {
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();
    std::string compressed_data = gzip::compress(pointer, data.size());
    const char * compressed_pointer = compressed_data.data();

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
    const char * pointer = data.data();
    // data is not compressed but we will try to decompress it

    CHECK_THROWS(gzip::decompress(pointer, data.size()));
}

TEST_CASE("round trip compression - gzip")
{
    const std::string data("this is a sentence that will be compressed into something");
    const char * pointer = data.data();

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
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char * compressed_pointer = compressed_data.data();
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
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char * compressed_pointer = compressed_data.data();
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
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char * compressed_pointer = compressed_data.data();
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
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char * compressed_pointer = compressed_data.data();
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
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
            const char * compressed_pointer = compressed_data.data();
            CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
            std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(pointer, data.size(), level, strategy);
                const char * compressed_pointer = compressed_data.data();
                CHECK(gzip::is_compressed(compressed_pointer, compressed_data.size()));
                std::string new_data = gzip::decompress(compressed_pointer, compressed_data.size());
                CHECK(data == new_data);
            }
        }
    }
}