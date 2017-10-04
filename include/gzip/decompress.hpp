// zlib
#include <zlib.h>
// std
#include <limits>
#include <stdexcept>
#include <libdeflate.h>

namespace gzip {

static const unsigned long MAX_SIZE_BEFORE_DECOMPRESS = 1000000000; // 1GB compressed is roughly 2GB decompressed

/*
static uint32_t
load_u32_gzip(const char *p)
{
    return ((uint32_t)p[0] << 0) | ((uint32_t)p[1] << 8) |
        ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
*/


// decodes both zlib and gzip
// http://stackoverflow.com/a/1838702/2333354
std::string decompress(const char* data, std::size_t size)
{

#ifdef DEBUG
    // Verify if size (long type) input will fit into unsigned int, type used for zlib's avail_in
    std::uint64_t size_64 = size * 2;
    if (size_64 > std::numeric_limits<unsigned int>::max())
    {
        throw std::runtime_error("size arg is too large to fit into unsigned int type x2");
    }
#endif
    if (size > MAX_SIZE_BEFORE_DECOMPRESS)
    {
        throw std::runtime_error("size may use more memory than intended when decompressing");
    }

    // not working since load_u32_gzip returns a bogus value likely due to https://stackoverflow.com/questions/9209138/uncompressed-file-size-using-zlibs-gzip-file-access-function/9213826#9213826
    /*

    std::size_t uncompressed_size = load_u32_gzip(&data[size - 4]);
    // sanity check this first before allocating?
    std::string uncompressed_data(uncompressed_size, '\0');
    auto decompressor = libdeflate_alloc_decompressor();
    // https://github.com/kaorimatz/libdeflate-ruby/blob/0e33da96cdaad3162f03ec924b25b2f4f2847538/ext/libdeflate/libdeflate_ext.c#L340
    std::size_t final_size;
    enum libdeflate_result result;
    for (;;) {
        std::size_t actual_size;
        result = libdeflate_gzip_decompress(decompressor,
                                            data,
                                            size,
                                            (void*)uncompressed_data.data(),
                                            uncompressed_size, &actual_size);
        if (result != LIBDEFLATE_INSUFFICIENT_SPACE) {
            break;
        }

    }
    if (result == LIBDEFLATE_BAD_DATA) {
        std::clog << "res: " << result << "\n";
    }
    else if (result != LIBDEFLATE_SUCCESS) {
        throw std::runtime_error("did not succeed");        
    }
    uncompressed_data.resize(final_size);
    return uncompressed_data;
*/
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
    inflate_s.next_in = (Bytef*)data;

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
        inflate_s.next_out = (Bytef*)(output.data() + length);
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