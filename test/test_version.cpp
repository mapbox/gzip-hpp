#include <catch.hpp>
#include <gzip/version.hpp>

TEST_CASE("test version")
{
    REQUIRE(GZIP_VERSION_STRING == std::string("2.0.0"));
}
