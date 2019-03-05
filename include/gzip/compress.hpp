#include <gzip/config.hpp>

// zlib
#include <zlib.h>

// std
#include <limits>
#include <stdexcept>
#include <string>

namespace gzip {

inline void compress(const char* data,
                     std::size_t size,
                     std::string& output,
                     int level = Z_DEFAULT_COMPRESSION,
                     std::size_t buffering_size = 0)
{
    if (buffering_size == 0)
    {
        buffering_size = (size / 2) + (size / 4) + 16;
    }

    z_stream deflate_s;
    deflate_s.zalloc = Z_NULL;
    deflate_s.zfree = Z_NULL;
    deflate_s.opaque = Z_NULL;
    deflate_s.avail_in = 0;
    deflate_s.next_in = Z_NULL;

    // The windowBits parameter is the base two logarithm of the window size (the size of the history buffer).
    // It should be in the range 8..15 for this version of the library.
    // Larger values of this parameter result in better compression at the expense of memory usage.
    // This range of values also changes the decoding type:
    //  -8 to -15 for raw deflate
    //  8 to 15 for zlib
    // (8 to 15) + 16 for gzip
    // (8 to 15) + 32 to automatically detect gzip/zlib header (decompression/inflate only)
    constexpr int window_bits = 15 + 16; // gzip with windowbits of 15

    constexpr int mem_level = 8;
    // The memory requirements for deflate are (in bytes):
    // (1 << (window_bits+2)) +  (1 << (mem_level+9))
    // with a default value of 8 for mem_level and our window_bits of 15
    // this is 128Kb

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    if (deflateInit2(&deflate_s, level, Z_DEFLATED, window_bits, mem_level, Z_DEFAULT_STRATEGY) != Z_OK)
    {
        throw std::runtime_error("deflate init failed");
    }
#pragma GCC diagnostic pop

    std::string buffer;
    do
    {
        constexpr std::size_t max_step = static_cast<std::size_t>(std::numeric_limits<unsigned int>::max());
        unsigned int step_size = size > max_step ? max_step : static_cast<unsigned int>(size);
        size -= step_size;
        unsigned int buffer_size = buffering_size > step_size ? step_size : static_cast<unsigned int>(buffering_size);

        deflate_s.next_in = reinterpret_cast<z_const Bytef*>(data);
        data = data + step_size;
        deflate_s.avail_in = step_size;

        buffer.resize(static_cast<std::size_t>(buffer_size));
        do
        {
            deflate_s.avail_out = buffer_size;
            deflate_s.next_out = reinterpret_cast<Bytef*>(&buffer[0]);
            // From http://www.zlib.net/zlib_how.html
            // "deflate() has a return value that can indicate errors, yet we do not check it here.
            // Why not? Well, it turns out that deflate() can do no wrong here."
            // Basically only possible error is from deflateInit not working properly
            deflate(&deflate_s, Z_FINISH);
            output.append(buffer, 0, static_cast<std::size_t>(buffer_size - deflate_s.avail_out));
        } while (deflate_s.avail_out == 0);
    } while (size > 0);
    deflateEnd(&deflate_s);
}

inline void compress(std::string const& input,
                     std::string& output,
                     int level = Z_DEFAULT_COMPRESSION,
                     std::size_t buffering_size = 0)
{
    compress(input.data(), input.size(), output, level, buffering_size);
}

inline std::string compress(const char* data,
                            std::size_t size,
                            int level = Z_DEFAULT_COMPRESSION,
                            std::size_t buffering_size = 0)
{
    std::string output;
    compress(data, size, output, level, buffering_size);
    return output;
}

inline std::string compress(std::string const& input,
                            int level = Z_DEFAULT_COMPRESSION,
                            std::size_t buffering_size = 0)
{
    std::string output;
    compress(input.data(), input.size(), output, level, buffering_size);
    return output;
}

} // namespace gzip
