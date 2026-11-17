cmake_minimum_required(VERSION 3.10)
project(TSPsolver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimized build by default (-O2).
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
add_compile_options(-Wall -Wextra)

# The two programs are written directly in the build/ folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Everything except main() and the tests, shared by the two programs.
add_library(tsp STATIC
    src/instance.cpp
    src/tour_writer.cpp
    src/trivial.cpp
    src/held_karp.cpp
    src/nearest_neighbor.cpp)
target_include_directories(tsp PUBLIC src)

add_executable(TSPsolver src/main.cpp)
target_link_libraries(TSPsolver tsp)

add_executable(tests tests/tests.cpp)
target_link_libraries(tests tsp)
