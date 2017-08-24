// zlib
#include <zlib.h>
// std
#include <stdexcept>

namespace gzip {

// decodes both zlib and gzip
// http://stackoverflow.com/a/1838702/2333354
std::string decompress(const char * data, std::size_t size) {

    std::string output;
    z_stream inflate_s;

    inflate_s.zalloc = Z_NULL;
    inflate_s.zfree = Z_NULL;
    inflate_s.opaque = Z_NULL;
    inflate_s.avail_in = 0;
    inflate_s.next_in = Z_NULL;
    inflateInit2(&inflate_s, 32 + 15);
    inflate_s.next_in = (Bytef *)data;
    inflate_s.avail_in = size;
    size_t length = 0;
    do {
        output.resize(length + 2 * size);
        inflate_s.avail_out = 2 * size;
        inflate_s.next_out = (Bytef *)(output.data() + length);
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

// Decompress method that takes a C++ std::string
std::string decompress(std::string const& input)
{
	// Call the decompress() function above that takes a const char pointer for code reuse
    return decompress(input.data(),input.size());
}

} // end gzip namespace