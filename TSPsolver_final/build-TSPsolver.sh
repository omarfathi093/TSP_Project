#!/bin/bash
# Builds the project into the build/ folder with CMake:
#   build/TSPsolver  (the solver)
#   build/tests      (the tests)
# Usage: bash build-TSPsolver.sh

set -e

# Always work from the folder where this script is.
cd "$(dirname "$0")"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

echo "Build done: build/TSPsolver and build/tests"
