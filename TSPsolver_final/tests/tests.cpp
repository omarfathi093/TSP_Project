// Simple tests, written with assert (no test framework).
// Run them with: ./build/tests

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "held_karp.hpp"
#include "instance.hpp"
#include "nearest_neighbor.hpp"
#include "tour_writer.hpp"
#include "trivial.hpp"

static const std::string TMP_INSTANCE = "tests_tmp_instance.tsp";
static const std::string TMP_OUTPUT = "tests_tmp_output.tour";

static void write_text(const std::string& path, const std::string& text) {
    std::ofstream file(path);
    file << text;
}

static std::string read_text(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Header written like "NAME : x" (spaces around the colon), coordinates.
static void test_header_with_spaces() {
    write_text(TMP_INSTANCE,
               "NAME : example\n"
               "TYPE : TSP\n"
               "COMMENT : 5 cities\n"
               "DIMENSION : 5\n"
               "EDGE_WEIGHT_TYPE : EUC_2D\n"
               "NODE_COORD_SECTION\n"
               "1 565.0 575.0\n"
               "2 25.0 185.0\n"
               "3 345.0 750.0\n"
               "4 945.0 685.0\n"
               "5 845.0 655.0\n"
               "EOF\n");
    Instance instance(TMP_INSTANCE);
    assert(instance.name() == "example");
    assert(instance.dimension() == 5);
}

// Header written like "NAME: x" (no space before the colon).
static void test_header_without_spaces() {
    write_text(TMP_INSTANCE,
               "NAME: example.tsp\n"
               "TYPE: TSP\n"
               "DIMENSION: 5\n"
               "EDGE_WEIGHT_TYPE: EUC_2D\n"
               "NODE_COORD_SECTION\n"
               "1 565.0 575.0\n"
               "2 25.0 185.0\n"
               "3 345.0 750.0\n"
               "4 945.0 685.0\n"
               "5 845.0 655.0\n"
               "EOF\n");
    Instance instance(TMP_INSTANCE);
    assert(instance.name() == "example.tsp");
    assert(instance.dimension() == 5);
}

// Lower triangle format, keywords in another order, spaces at the end of
// the lines, and an indented EOF.
static void test_header_lower_diag_and_odd_spacing() {
    write_text(TMP_INSTANCE,
               "COMMENT : odd file\n"
               "TYPE : TSP\n"
               "DIMENSION : 3   \n"
               "NAME : small3 \n"
               "EDGE_WEIGHT_TYPE : EXPLICIT\n"
               "EDGE_WEIGHT_FORMAT : LOWER_DIAG_ROW \n"
               "EDGE_WEIGHT_SECTION\n"
               " 0 5 0 8 4 0\n"
               " EOF\n");
    Instance instance(TMP_INSTANCE);
    assert(instance.name() == "small3");
    assert(instance.dimension() == 3);
}

static void test_missing_file_throws() {
    bool thrown = false;
    try {
        Instance instance("this_file_does_not_exist.tsp");
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

static void test_missing_dimension_throws() {
    write_text(TMP_INSTANCE, "NAME : x\nTYPE : TSP\nNODE_COORD_SECTION\n1 0 0\nEOF\n");
    bool thrown = false;
    try {
        Instance instance(TMP_INSTANCE);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

static void test_bad_dimension_throws() {
    write_text(TMP_INSTANCE, "NAME : x\nDIMENSION : abc\nNODE_COORD_SECTION\nEOF\n");
    bool thrown = false;
    try {
        Instance instance(TMP_INSTANCE);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

// The trivial tour is 1, 2, ..., n.
static void test_trivial_tour() {
    write_text(TMP_INSTANCE,
               "NAME : example\nTYPE : TSP\nDIMENSION : 5\nNODE_COORD_SECTION\nEOF\n");
    Instance instance(TMP_INSTANCE);
    std::vector<size_t> tour = trivial_tour(instance);
    assert(tour.size() == 5);
    for (size_t i = 0; i < tour.size(); ++i) {
        assert(tour[i] == i + 1);
    }
}

static void test_tour_name() {
    assert(tour_name("example.tsp") == "example.tour");
    assert(tour_name("berlin52") == "berlin52.tour");
    assert(tour_name("ulysses16.tsp") == "ulysses16.tour");
}

// The output file must match the format of the course exactly.
static void test_write_tour_format() {
    std::vector<size_t> tour = {1, 3, 5, 2, 4};
    write_tour(TMP_OUTPUT, "example.tsp", tour);
    std::string expected =
        "NAME : example.tour\n"
        "TYPE : TOUR\n"
        "DIMENSION : 5\n"
        "TOUR_SECTION\n"
        "1\n"
        "3\n"
        "5\n"
        "2\n"
        "4\n"
        "-1\n"
        "EOF\n";
    assert(read_text(TMP_OUTPUT) == expected);
}

static void test_write_tour_bad_path_throws() {
    std::vector<size_t> tour = {1, 2, 3};
    bool thrown = false;
    try {
        write_tour("no_such_folder/out.tour", "example", tour);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

// A file with two cities and the given distance type.
static std::string two_cities(const std::string& type, const std::string& points) {
    return "NAME : t\nDIMENSION : 2\nEDGE_WEIGHT_TYPE : " + type +
           "\nNODE_COORD_SECTION\n" + points + "EOF\n";
}

// Distances worked out by hand for each type of the archive.
static void test_distance_formulas() {
    write_text(TMP_INSTANCE, two_cities("EUC_2D", "1 0 0\n2 3 4\n"));
    assert(Instance(TMP_INSTANCE).distance(0, 1) == 5);
    write_text(TMP_INSTANCE, two_cities("ATT", "1 0 0\n2 10 0\n"));
    assert(Instance(TMP_INSTANCE).distance(0, 1) == 4);
    write_text(TMP_INSTANCE, two_cities("GEO", "1 0.0 0.0\n2 0.0 90.0\n"));
    Instance geo(TMP_INSTANCE);
    assert(geo.distance(0, 1) == geo.distance(1, 0));
    assert(geo.distance(0, 1) > 9000 && geo.distance(0, 1) < 10100);
}

// The lower triangle gives a symmetric matrix.
static void test_explicit_distances() {
    write_text(TMP_INSTANCE,
               "NAME : small3\nDIMENSION : 3\nEDGE_WEIGHT_TYPE : EXPLICIT\n"
               "EDGE_WEIGHT_FORMAT : LOWER_DIAG_ROW\nEDGE_WEIGHT_SECTION\n"
               " 0 5 0 8 4 0\nEOF\n");
    Instance instance(TMP_INSTANCE);
    assert(instance.distance(1, 0) == 5);
    assert(instance.distance(0, 1) == 5);
    assert(instance.distance(2, 0) == 8);
    assert(instance.distance(1, 2) == 4);
}

// A random instance with n cities, as a lower triangle.
static std::string random_instance(size_t n) {
    std::string text = "NAME : random\nDIMENSION : " + std::to_string(n) +
                       "\nEDGE_WEIGHT_TYPE : EXPLICIT\nEDGE_WEIGHT_FORMAT : LOWER_DIAG_ROW\n"
                       "EDGE_WEIGHT_SECTION\n";
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            text += std::to_string(i == j ? 0 : 1 + std::rand() % 50) + " ";
        }
        text += "\n";
    }
    return text + "EOF\n";
}

// Length of the tour, going back to the first city at the end.
static long tour_length(const Instance& instance, const std::vector<size_t>& tour) {
    long total = 0;
    for (size_t i = 0; i < tour.size(); ++i) {
        total += instance.distance(tour[i] - 1, tour[(i + 1) % tour.size()] - 1);
    }
    return total;
}

// Held-Karp must find the same length as a search of all the orders.
static void test_held_karp_matches_brute_force() {
    std::srand(1);
    for (size_t n = 4; n <= 8; ++n) {
        for (int round = 0; round < 5; ++round) {
            write_text(TMP_INSTANCE, random_instance(n));
            Instance instance(TMP_INSTANCE);
            std::vector<size_t> tour = held_karp_tour(instance);

            std::vector<size_t> order;
            for (size_t city = 1; city <= n; ++city) {
                order.push_back(city);
            }
            long shortest = tour_length(instance, order);
            while (std::next_permutation(order.begin() + 1, order.end())) {
                shortest = std::min(shortest, tour_length(instance, order));
            }
            assert(tour.size() == n);
            assert(tour[0] == 1);
            assert(tour_length(instance, tour) == shortest);
        }
    }
}

// More than 24 cities is refused with an error.
static void test_held_karp_city_limit() {
    write_text(TMP_INSTANCE, random_instance(25));
    Instance instance(TMP_INSTANCE);
    bool thrown = false;
    try {
        held_karp_tour(instance);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

// Cities on a line at x = 0, 10, 1, 3 (cities 1 to 4): from city 1 the
// closest is city 3, then city 4, then city 2.
static void test_nearest_neighbor() {
    write_text(TMP_INSTANCE,
               "NAME : line\nDIMENSION : 4\nEDGE_WEIGHT_TYPE : EUC_2D\nNODE_COORD_SECTION\n"
               "1 0 0\n2 10 0\n3 1 0\n4 3 0\nEOF\n");
    Instance instance(TMP_INSTANCE);
    std::vector<size_t> expected = {1, 3, 4, 2};
    assert(nearest_neighbor_tour(instance) == expected);
}

int main() {
    test_header_with_spaces();
    test_header_without_spaces();
    test_header_lower_diag_and_odd_spacing();
    test_missing_file_throws();
    test_missing_dimension_throws();
    test_bad_dimension_throws();
    test_trivial_tour();
    test_tour_name();
    test_write_tour_format();
    test_write_tour_bad_path_throws();
    test_distance_formulas();
    test_explicit_distances();
    test_held_karp_matches_brute_force();
    test_held_karp_city_limit();
    test_nearest_neighbor();

    std::remove(TMP_INSTANCE.c_str());
    std::remove(TMP_OUTPUT.c_str());

    std::cout << "All tests passed." << std::endl;
    return 0;
}
