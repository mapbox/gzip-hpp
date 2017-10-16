#include <gzip/config.hpp>

// zlib
#include <zlib.h>
// std
#include <limits>
#include <stdexcept>
#include <string>

#define USE_BOOST

#ifdef USE_BOOST
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/compose.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif

// https://blog.cppse.nl/deflate-and-gzip-compress-and-decompress-functions
// http://www.zlib.net/zpipe.c
// http://www.boost.org/doc/libs/1_50_0/libs/iostreams/doc/classes/gzip.html

namespace gzip {

static const unsigned long MAX_SIZE_BEFORE_COMPRESS = 2000000000; // 2GB decompressed

// Compress method that takes a pointer an immutable character sequence (aka a string in C)
std::string compress(const char* data,
                     std::size_t size,
                     int level = Z_DEFAULT_COMPRESSION,
                     int strategy = Z_DEFAULT_STRATEGY)
{

    std::string output;
#ifdef USE_BOOST
    boost::iostreams::array_source src(data, size);
    boost::iostreams::gzip_params params(level,
                                         boost::iostreams::zlib::deflated,
                                         15,
                                         8,
                                         strategy
                                         );
    boost::iostreams::gzip_compressor compressor(params);
    boost::iostreams::copy(boost::iostreams::compose(compressor, src),
                           boost::iostreams::back_inserter(output));
#else
    z_stream deflate_s;

    deflate_s.zalloc = Z_NULL;
    deflate_s.zfree = Z_NULL;
    deflate_s.opaque = Z_NULL;
    deflate_s.avail_in = 0;
    deflate_s.next_in = Z_NULL;

    // 4th arg is 31 because of...? Also what tippecanoe is doing
    // TODO: double check this
    if (deflateInit2(&deflate_s, level, Z_DEFLATED, 31, 8, strategy) != Z_OK)
    {
        throw std::runtime_error("deflate init failed");
    }
    deflate_s.next_in = reinterpret_cast<z_const Bytef*>(data);

#ifdef DEBUG
    // Verify if size input will fit into unsigned int, type used for zlib's avail_in
    if (size > std::numeric_limits<unsigned int>::max())
    {
        deflateEnd(&deflate_s); // explicitly end deflate to avoid memory leak
        throw std::runtime_error("size arg is too large to fit into unsigned int type");
    }
#endif
    if (size > MAX_SIZE_BEFORE_COMPRESS)
    {
        deflateEnd(&deflate_s); // explicitly end deflate to avoid memory leak
        throw std::runtime_error("size may use more memory than intended when decompressing");
    }

    deflate_s.avail_in = static_cast<unsigned int>(size);

    size_t length = 0;
    do
    {
        size_t increase = size / 2 + 1024;
        output.resize(length + increase);
        // There is no way we see that "increase" would not fit in an unsigned int,
        // hence we use static cast here to avoid -Wshorten-64-to-32 error
        deflate_s.avail_out = static_cast<unsigned int>(increase);
        deflate_s.next_out = reinterpret_cast<Bytef*>((&output[0] + length));
        // From http://www.zlib.net/zlib_how.html
        // "deflate() has a return value that can indicate errors, yet we do not check it here.
        // Why not? Well, it turns out that deflate() can do no wrong here."
        // Basically only possible error is from deflateInit not working properly
        deflate(&deflate_s, Z_FINISH);
        length += (increase - deflate_s.avail_out);
    } while (deflate_s.avail_out == 0);
    deflateEnd(&deflate_s);
    output.resize(length);
#endif
    // return the std::string
    return output;
}

} // end gzip namespace