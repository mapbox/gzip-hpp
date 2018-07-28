#include <gzip/config.hpp>

// zlib
#include <libdeflate.h>

// std
#include <limits>
#include <stdexcept>
#include <string>

namespace gzip {

class Decompressor
{
    std::size_t max_;
    struct libdeflate_decompressor* decompressor_ = nullptr;

  public:
    Decompressor(std::size_t max_bytes = 1000000000) // by default refuse operation if compressed data is > 1GB
        : max_(max_bytes)
    {
        decompressor_ = libdeflate_alloc_decompressor();
        if (!decompressor_)
        {
            throw std::runtime_error("libdeflate_alloc_decompressor failed");
        }
    }

    ~Decompressor()
    {
        if (decompressor_)
        {
            libdeflate_free_decompressor(decompressor_);
        }
    }

    template <typename OutputType>
    void decompress(OutputType& output,
                    const char* data,
                    std::size_t size) const
    {

#ifdef DEBUG
        // Verify if size (long type) input will fit into unsigned int, type used for zlib's avail_in
        std::uint64_t size_64 = size * 2;
        if (size_64 > std::numeric_limits<unsigned int>::max())
        {
            throw std::runtime_error("size arg is too large to fit into unsigned int type x2");
        }
#endif
        if (size > max_ || (size * 2) > max_)
        {
            throw std::runtime_error("size may use more memory than intended when decompressing");
        }

        // https://github.com/kaorimatz/libdeflate-ruby/blob/0e33da96cdaad3162f03ec924b25b2f4f2847538/ext/libdeflate/libdeflate_ext.c#L340
        // https://github.com/ebiggers/libdeflate/commit/5a9d25a8922e2d74618fba96e56db4fe145510f4
        std::size_t actual_size;
        std::size_t uncompressed_size_guess = size * 4;
        output.resize(uncompressed_size_guess);
        enum libdeflate_result result = libdeflate_gzip_decompress(decompressor_,
                                                                   data,
                                                                   size,
                                                                   static_cast<void*>(&output[0]),
                                                                   uncompressed_size_guess, &actual_size);
        if (result == LIBDEFLATE_INSUFFICIENT_SPACE)
        {
            throw std::runtime_error("no space: did not succeed");
        }
        else if (result == LIBDEFLATE_SHORT_OUTPUT)
        {
            throw std::runtime_error("short output: did not succeed");
        }
        else if (result == LIBDEFLATE_BAD_DATA)
        {
            throw std::runtime_error("bad data: did not succeed");
        }
        else if (result != LIBDEFLATE_SUCCESS)
        {
            throw std::runtime_error("did not succeed");
        }
        output.resize(actual_size);
    }
};

inline std::string decompress(const char* data, std::size_t size)
{
    Decompressor decomp;
    std::string output;
    decomp.decompress(output, data, size);
    return output;
}

} // namespace gzip
