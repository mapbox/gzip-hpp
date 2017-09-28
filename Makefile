# Whether to turn compiler warnings into errors
export WERROR ?= true

default: release

release:
	mkdir -p build/Release && cd build/Release && cmake ../../ -DCMAKE_BUILD_TYPE=Release -DWERROR=$(WERROR) && VERBOSE=1 cmake --build .

debug:
	mkdir -p build/Debug && cd build/Debug && cmake ../../ -DCMAKE_BUILD_TYPE=Debug -DWERROR=$(WERROR) && VERBOSE=1 cmake --build .

test: release debug
	@echo "Running tests for Debug mode"
	./build/Debug/unit-tests
	@echo "Running tests for Release mode"
	./build/Release/unit-tests

bench:
	@if [ -f ./build/bench-tests ]; then ./build/bench-tests; else echo "Please run 'make release' or 'make debug' first" && exit 1; fi

tidy:
	./scripts/clang-tidy.sh

coverage:
	./scripts/coverage.sh

clean:
	rm -rf build

format:
	./scripts/format.sh

.PHONY: test bench

# TODO: once gzip has its own repo, consider adding `-Wold-style-cast` as a debug flag and moving to C-style casting to static_cast
# Not sure where this should live, now that flags are no longer in the Makefile. Perhaps in setup.sh?