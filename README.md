Gzip C++ lib for gzip compression and decompression.

This library is designed for **non streaming** gzip decompression and compression using C++ strings.

[![Build Status](https://travis-ci.com/mapbox/gzip-hpp.svg?branch=master)](https://travis-ci.com/mapbox/gzip-hpp)
[![hpp-skel badge](https://raw.githubusercontent.com/mapbox/cpp/master/assets/hpp-skel-badge_blue.svg)](https://github.com/mapbox/hpp-skel)

## Usage
```c++
// Include the specific gzip headers your code needs, for example...
#include <gzip/compress.hpp>
#include <gzip/config.hpp>
#include <gzip/decompress.hpp>
#include <gzip/utils.hpp>
#include <gzip/version.hpp>

// All function calls must pass in a pointer of an 
// immutable character sequence (aka a string in C) and its size
std::string data = "hello";

// Check if compressed. Can check both gzip and zlib.
bool c = gzip::is_compressed(data); // false

// Compress returns a std::string
std::string compressed_data = gzip::compress(data);

// Decompress returns a std::string and decodes both zlib and gzip
std::string decompressed_data = gzip::decompress(compressed_data);

// Can also compress or decompress from a const char * and size
std::string compressed_data = gzip::compress(data.data(), data.size());
std::string decompressed_data = gzip::decompress(compressed_data.data(), data.size());

// You can pass in an existing string as well to be modified for compression or decompression
// both of which will have data appended to the end.
std::string compressed_data;
gzip::compress(data, compressed_data);
std::string decompressed_data
gzip::decompress(compressed_data, decompressed_data);

// This also works using pointers and sizes
std::string compressed_data;
gzip::compress(data.data(), data.size(), compressed_data);
std::string decompressed_data
gzip::decompress(compressed_data.data(), compressed_data.size(), decompressed_data);
```
#### Compress

All forms of compressed as shown above support the following optional arguments:
 * Compression Level
 * Buffering Size

Compression level is a number 0 to 9, based on the zlib compression levels. This increases the time spent in compression
and may result in higher levels of compression for higher compression levels. The default is the default level for the
zlib compression library of `Z_DEFAULT_COMPRESSION`.

Buffering size controls the amount of memory allocated as a buffer during compression with zlib. This by default is a buffer
that is 75% of the size of the data provided. A `0` passed to buffer size is the default and forces this 75% calculation.

```c++
const int level = Z_DEFAULT_COMPRESSION;
const std::size_t buffer_size = 1024; // 1 KB

std::string compressed_data = gzip::compress(data, level, buffer_size);
```

#### Decompress

All forms of decompressed as shown above support the following optional arguments:
 * Maximum Uncompressed Size
 * Buffering Size

Maximum uncompressed size limits the total amount of memory that may be used during decompression. This is provided to prevent heavily
compressed malicious files from causing issues in an application. By default this value is set to `0` which disables this protection.

Buffering size controls the amount of memory allocated as a buffer during deccompression with zlib. This by default is a buffer
that is 150% of the size of the compressed data provided. A `0` passed to buffer size is the default and forces this 150% calculation.

```c++
const std::size_t max_decompressed_size = 1024 * 1024 * 1024; // 1 GB
const std::size_t buffer_size = 1024; // 1 kB
std::string decompressed_data = gzip::decompress(data, max_decompressed_size, buffer_size);
```

## Test

```shell
# build test binaries
make

# run tests
make test
```

You can make Release test binaries as well
```shell
BUILDTYPE=Release make
BUILDTYPE=Release make test
```

## Versioning

This library is semantically versioned using the /include/gzip/version.cpp file. This defines a number of macros that can be used to check the current major, minor, or patch versions, as well as the full version string.

Here's how you can check for a particular version to use specific API methods
```c++
#if GZIP_VERSION_MAJOR > 2
// use version 2 api
#else
// use older verion apis
#endif
```

Here's how to check the version string
```c++
std::cout << "version: " << GZIP_VERSION_STRING << "/n";
// => version: 0.2.0
```

And lastly, mathematically checking for a specific version:
```c++
#if GZIP_VERSION_CODE > 20001
// use feature provided in v2.0.1
#endif
```
