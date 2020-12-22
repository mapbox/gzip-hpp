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
    std::size_t const max_;
    struct libdeflate_decompressor* decompressor_ = nullptr;
    // make noncopyable
    Decompressor(Decompressor const&) = delete;
    Decompressor& operator=(Decompressor const&) = delete;

  public:
    Decompressor(std::size_t max_bytes = 2147483648u) // by default refuse operation if required uutput buffer is > 2GB
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
                    char const* data,
                    std::size_t size) const
    {
        // https://github.com/kaorimatz/libdeflate-ruby/blob/0e33da96cdaad3162f03ec924b25b2f4f2847538/ext/libdeflate/libdeflate_ext.c#L340
        // https://github.com/ebiggers/libdeflate/commit/5a9d25a8922e2d74618fba96e56db4fe145510f4
        std::size_t actual_size;
        std::size_t uncompressed_size_guess = std::min(size * 4, max_);
        output.resize(uncompressed_size_guess);
        enum libdeflate_result result;
        for (;;)
        {
            result = libdeflate_gzip_decompress(decompressor_,
                                                data,
                                                size,
                                                const_cast<char*>(output.data()),
                                                output.size(), &actual_size);
            if (result != LIBDEFLATE_INSUFFICIENT_SPACE)
            {
                break;
            }
            if (output.size() == max_)
            {
                throw std::runtime_error("request to resize output buffer can't exceed maximum limit");
            }
            std::size_t new_size = std::min((output.capacity() << 1) - output.size(), max_);
            output.resize(new_size);
        }

        if (result == LIBDEFLATE_SHORT_OUTPUT)
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
