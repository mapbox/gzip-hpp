#include <gzip/config.hpp>

// zlib
#include <zlib.h>
// std
#include <limits>
#include <stdexcept>
#include <string>

namespace gzip {

static const unsigned long MAX_SIZE_BEFORE_DECOMPRESS = 1000000000; // 1GB compressed is roughly 2GB decompressed

// decodes both zlib and gzip
// http://stackoverflow.com/a/1838702/2333354
std::string decompress(const char* data, std::size_t size)
{

    std::string output;
    z_stream inflate_s;

    inflate_s.zalloc = Z_NULL;
    inflate_s.zfree = Z_NULL;
    inflate_s.opaque = Z_NULL;
    inflate_s.avail_in = 0;
    inflate_s.next_in = Z_NULL;
    if (inflateInit2(&inflate_s, 32 + 15) != Z_OK)
    {
        throw std::runtime_error("inflate init failed");
    }
    inflate_s.next_in = reinterpret_cast<z_const Bytef*>(data);

#ifdef DEBUG
    // Verify if size (long type) input will fit into unsigned int, type used for zlib's avail_in
    std::uint64_t size_64 = size * 2;
    if (size_64 > std::numeric_limits<unsigned int>::max())
    {
        inflateEnd(&inflate_s);
        throw std::runtime_error("size arg is too large to fit into unsigned int type x2");
    }
#endif
    if (size > MAX_SIZE_BEFORE_DECOMPRESS)
    {
        inflateEnd(&inflate_s);
        throw std::runtime_error("size may use more memory than intended when decompressing");
    }
    inflate_s.avail_in = static_cast<unsigned int>(size);
    size_t length = 0;
    do
    {
        output.resize(length + 2 * size);
        inflate_s.avail_out = static_cast<unsigned int>(2 * size);
        inflate_s.next_out = reinterpret_cast<Bytef*>(&output[0] + length);
        int ret = inflate(&inflate_s, Z_FINISH);
        if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR)
        {
            std::string error_msg = inflate_s.msg;
            inflateEnd(&inflate_s);
            throw std::runtime_error(error_msg);
        }

        length += (2 * size - inflate_s.avail_out);
    } while (inflate_s.avail_out == 0);
    inflateEnd(&inflate_s);
    output.resize(length);

    // return the std::string
    return output;
}

} // end gzip namespace