cmake_minimum_required(VERSION 3.8)
project(hpp_skel LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mason.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wsign-compare -Wconversion -Wshadow")

# mason_use is a mason function within the mason.cmake file and provides ready-to-go vars, like "STATIC_LIBS" and "INCLUDE_DIRS"
mason_use(catch VERSION 1.9.6 HEADER_ONLY)
include_directories(SYSTEM ${MASON_PACKAGE_catch_INCLUDE_DIRS})

mason_use(zlib VERSION 1.2.8)
include_directories(SYSTEM ${MASON_PACKAGE_zlib_INCLUDE_DIRS})

# headers
mason_use(boost VERSION 1.63.0 HEADER_ONLY)
include_directories(SYSTEM ${MASON_PACKAGE_boost_INCLUDE_DIRS})
# iostreams library which has gzip coding support
mason_use(boost_libiostreams VERSION 1.63.0)

include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB TEST_SOURCES test/*.cpp)
add_executable(unit-tests ${TEST_SOURCES})
# link zlib static library to the unit-tests binary so the tests know where to find the zlib impl code
target_link_libraries(unit-tests ${MASON_PACKAGE_zlib_STATIC_LIBS} ${MASON_PACKAGE_boost_libiostreams_STATIC_LIBS})