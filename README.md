Gzip C++ lib for gzip compression and decompression. Extracted from [mapnik-vector-tile](https://github.com/mapbox/mapnik-vector-tile) for light-weight modularity.

[![Build Status](https://travis-ci.com/mapbox/gzip-hpp.svg?branch=master)](https://travis-ci.com/mapbox/gzip-hpp)

## Usage
```cpp
#include <gzip.hpp>

// You can pass in a std::string
std::string data = "hello";

// Check if compressed, can check both gzip and zlib
// Accepts either "std::string" or "const char *"
bool c = gzip::is_compressed(data); // false

// Compress returns a std::string
std::string compressed_data = gzip::compress(data);

// Decompress returns a std::string and decodes both zlib and gzip
std::string decompressed_data = gzip::decompress(compressed_data);

// Other input options: You can also pass in a pointer of an immutable character sequence (aka a string in C)
// If using a pointer, be sure to include the size parameter
std::string data = "hello";
const char * pointer = data.data();
std::string value = gzip::compress(pointer, data.size());

// Or like so
std::string compressed_data = gzip::compress(tile->data(), tile->data.size());

// Or like so
std::string value = gzip::compress(node::Buffer::Data(obj), node::Buffer::Length(obj));

```
#### Compress
```cpp
// Compress using std::string
// Optionally include compression level and strategy
std::string data = "hello";
int level = Z_DEFAULT_COMPRESSION; // Z_DEFAULT_COMPRESSION is the default if no arg is passed
int strategy = Z_DEFAULT_STRATEGY; // Z_DEFAULT_STRATEGY is the defaul if no arg is passed

std::string compressed_data = gzip::compress(data, level, strategy);

// Compress using pointer
// Optionally include data size, compression level and strategy
std::size_t size; // No default value, but what happens when not passed??
int level = Z_DEFAULT_COMPRESSION; // Z_DEFAULT_COMPRESSION is the default if no arg is passed
int strategy = Z_DEFAULT_STRATEGY; // Z_DEFAULT_STRATEGY is the defaul if no arg is passed

std::string compressed_data = gzip::compress(tile->data(), size, level, strategy);
```
#### Decompress
```cpp
// Decompress using std::string
// No args other than the std:string
std::string data = "hello";
std::string compressed_data = gzip::compress(data);

std::string decompressed_data = gzip::decompress(compressed_data);

// Decompress using pointer
// Optionally include data size
const char * data = node::Buffer::Data(obj);
std::size_t size = node::Buffer::Length(obj);

std::string decompressed_data = gzip::decompress(data, size);

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
```cpp
#if GZIP_VERSION_MAJOR > 2
// use version 2 api
#else
// use older verion apis
#endif
```

Here's how to check the version string
```cpp
std::cout << "version: " << GZIP_VERSION_STRING << "/n";
// => version: 0.2.0
```

And lastly, mathematically checking for a specific version:
```cpp
#if GZIP_VERSION_CODE > 20001
// use feature provided in v2.0.1
#endif
```
