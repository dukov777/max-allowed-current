#bin/bash

rm -rf build/
mkdir build

cmake -B build/
cmake --build build/ -j 8
ctest --output-on-failure --test-dir build
./CapacitorTests
