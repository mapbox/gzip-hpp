#include <iostream>
// zlib
#include <zlib.h>
// std
#include <stdexcept>

namespace gzip {


// Compress method that takes a pointer an immutable character sequence (aka a string in C)
std::string compress (const char * data,
                      std::size_t size,
                      int level=Z_DEFAULT_COMPRESSION, 
                      int strategy=Z_DEFAULT_STRATEGY) {

    std::string output;
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
    deflate_s.next_in = (Bytef *)data;
    deflate_s.avail_in = size;
    size_t length = 0;
    do {
        size_t increase = size / 2 + 1024;
        output.resize(length + increase);
        deflate_s.avail_out = increase;
        deflate_s.next_out = (Bytef *)(output.data() + length);
        // From http://www.zlib.net/zlib_how.html
        // "deflate() has a return value that can indicate errors, yet we do not check it here. 
        // Why not? Well, it turns out that deflate() can do no wrong here."
        // Basically only possible error is from deflateInit not working properly
        deflate(&deflate_s, Z_FINISH);
        length += (increase - deflate_s.avail_out);
    } while (deflate_s.avail_out == 0);
    deflateEnd(&deflate_s);
    output.resize(length);
    
    // return the std::string
    return output; 
}

// Compress method that takes a C++ std::string
std::string compress (std::string const& input, 
                      int level=Z_DEFAULT_COMPRESSION, 
                      int strategy=Z_DEFAULT_STRATEGY) {
    
    // Call the compress() function above that takes a const char pointer for code reuse
    return compress(input.data(), input.size(), level, strategy);
}


} // end gzip namespace