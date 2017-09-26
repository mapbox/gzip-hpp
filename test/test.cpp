#include <gzip.hpp>

#include <exception>
#include <iostream>
#include <cassert>
// #define CATCH_CONFIG_MAIN
// #include <catch.hpp>
#include <limits>

static void test_memory_throw() {
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();

    unsigned long l = 2000000001; 
    
    try {
        std::string value = gzip::compress(pointer, l);
    } catch(std::exception const& ex) {
        std::cout << ex.what() << "\n";
        //assert(msg == "size may use more memory than intended when decompressing");
    }
}

int main() {
    test_memory_throw();
    // try {
    //     test_memory_throw();
    // } catch( const char* msg ) {
    //     std::cout << msg;
    // }
    return 0;
}
