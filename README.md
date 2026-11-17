# TSPsolver

Reads a TSP instance (TSPLIB format) and writes a tour file. Three methods:

- `trivial`: the tour is 1, 2, ..., n (default)
- `held-karp`: the shortest possible tour, only up to 24 cities
- `nearest-neighbor`: always go to the closest city not visited yet (fast, not optimal)

## Build

    bash build-TSPsolver.sh

This runs CMake (`CMakeLists.txt`) and creates `build/TSPsolver` and `build/tests` (needs `cmake` and a C++17 compiler such as `g++`).

## Run

    ./build/TSPsolver <instance_path> <output_path>
    ./build/TSPsolver <instance_path> <output_path> -m trivial|held-karp|nearest-neighbor

Without `-m`, the trivial method is used.
Examples:

    ./build/TSPsolver TSP-instances/coord/berlin52.tsp berlin52.tour -m nearest-neighbor
    ./build/TSPsolver TSP-instances/lower_diag/gr17.tsp gr17.tour -m held-karp

## Tests

    ./build/tests

## Check the tours with the course checker

`Solution_checker.py` only works from the university network (VPN).

    mkdir -p out
    for f in TSP-instances/*/*.tsp; do
        out="out/$(basename "$f" .tsp).tour"
        ./build/TSPsolver "$f" "$out"
        python3 Solution_checker.py "$f" "$out"
    done

## Files

- `src/main.cpp`: reads the arguments, calls the method, writes the tour
- `src/instance.hpp/.cpp`: class `Instance`, reads the file and gives the distances
- `src/trivial.hpp/.cpp`: the trivial method
- `src/held_karp.hpp/.cpp`: the Held-Karp method
- `src/nearest_neighbor.hpp/.cpp`: the nearest neighbor method
- `src/tour_writer.hpp/.cpp`: writes the tour file
- `tests/tests.cpp`: tests
