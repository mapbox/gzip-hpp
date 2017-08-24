default: release

release:
	mkdir -p build && cd build && cmake ../ -DCMAKE_BUILD_TYPE=Release && VERBOSE=1 cmake --build .

debug:
	mkdir -p build && cd build && cmake ../ -DCMAKE_BUILD_TYPE=Debug && VERBOSE=1 cmake --build .

test: default 
	./build/unit-tests

coverage:
	./scripts/coverage.sh

clean:
	rm -rf build

# TODO: once gzip has its own repo, consider adding `-Wold-style-cast` as a debug flag and moving to C-style casting to static_cast
# Not sure where this should live, now that flags are no longer in the Makefile. Perhaps in setup.sh?