#include <gzip/config.hpp>

// zlib
#include <libdeflate.h>

// std
#include <limits>
#include <stdexcept>
#include <string>

namespace gzip {

class Compressor
{
    std::size_t max_;
    int level_;
    struct libdeflate_compressor* compressor_ = nullptr;
    // make noncopyable
    Compressor(Compressor const&) = delete;
    Compressor& operator=(Compressor const&) = delete;

  public:
    Compressor(int level = 6,
               std::size_t max_bytes = 2000000000) // by default refuse operation if uncompressed data is > 2GB
        : max_(max_bytes),
          level_(level)
    {
        compressor_ = libdeflate_alloc_compressor(level_);
        if (!compressor_)
        {
            throw std::runtime_error("libdeflate_alloc_compressor failed");
        }
    }

    ~Compressor()
    {
        if (compressor_)
        {
            libdeflate_free_compressor(compressor_);
        }
    }

    template <typename OutputType>
    void compress(OutputType& output,
                  char const* data,
                  std::size_t size) const
    {
        if (size > max_)
        {
            throw std::runtime_error("size may use more memory than intended when decompressing");
        }

        std::size_t max_compressed_size = libdeflate_gzip_compress_bound(compressor_, size);
        // TODO: sanity check this before allocating
        if (max_compressed_size > output.size())
        {
            output.resize(max_compressed_size);
        }

        std::size_t actual_compressed_size = libdeflate_gzip_compress(compressor_,
                                                                      data,
                                                                      size,
                                                                      const_cast<char*>(output.data()),
                                                                      max_compressed_size);
        if (actual_compressed_size == 0)
        {
            throw std::runtime_error("actual_compressed_size 0");
        }
        output.resize(actual_compressed_size);
    }
};

inline std::string compress(const char* data,
                            std::size_t size,
                            int level = 6)
{
    Compressor comp(level);
    std::string output;
    comp.compress(output, data, size);
    return output;
}

} // namespace gzip
