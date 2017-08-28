Gzip C++ lib fro gzip compression and decompression. Extracted from [mapnik-vector-tile](https://github.com/mapbox/mapnik-vector-tile) for light-weight modularity.

[![Build Status](https://travis-ci.com/mapbox/gzip-hpp.svg?token=XUVmCBVxtg4Px9i4scss&branch=master)](https://travis-ci.com/mapbox/gzip-hpp)

## Usage
```cpp
#include <gzip.hpp>

// You can pass in a std::string
std::string data = "hello";

// compress returns a std::string
std::string compressed_data = gzip::compress(data);

// decompress returns a std::string and decodes both zlib and gzip
std::string decompressed_data = gzip::decompress(compressed_data);

// You can also pass in a pointer of an immutable character sequence (aka a string in C)
std::string compressed_data = gzip::compress(tile->data());
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
std::size_t size = node::Buffer::Length(obj); // Anything we should say about this?

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

## Publishing

We recommend publishing header files to [Mason](https://github.com/mapbox/mason), the C++ packaging manager. Binaries can be downloaded by project name and version number. In order to publish to Mason you must request the publish via a Pull Request to the [`scripts/` directory](https://github.com/mapbox/mason/tree/master/scripts) with your project materials.

Mason packages can be downloaded to your project by using the `mason install` command. This is best set up in a Makefile ([example](https://github.com/mapbox/geometry.hpp/blob/23b7fe66b11a4b7830c797817efe19660806d851/Makefile#L10)).

Of course, you can always copy and paste this repo into your vendor path for your project. :scissors:

## Versioning

This library is semantically versioned using the /include/hello_world/version.cpp file. This defines a number of macros that can be used to check the current major, minor, or patch versions, as well as the full version string.

Here's how a downstream client would check for a particular version to use specific API methods
```cpp
#if HELLOWORLD_VERSION_MAJOR > 2
// use version 2 api
#else
// use older verion apis
#endif
```

Here's how to check the version string
```cpp
std::cout << "version: " << HELLOWORLD_VERSION_STRING << "/n";
// => version: 0.2.0
```

And lastly, mathematically checking for a specific version:
```cpp
#if HELLOWORLD_VERSION_CODE > 20001
// use feature provided in v2.0.1
#endif
```
