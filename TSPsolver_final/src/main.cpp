#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "held_karp.hpp"
#include "instance.hpp"
#include "nearest_neighbor.hpp"
#include "tour_writer.hpp"
#include "trivial.hpp"

static void print_usage(const std::string& program) {
    std::cerr << "Usage: " << program
              << " <instance_path> <output_path> [-m <algorithm>]\n"
              << "Available algorithms: trivial (default), held-karp, nearest-neighbor\n";
}

int main(int argc, char* argv[]) {
    // Two mandatory arguments, and optionally "-m <algorithm>".
    if (argc != 3 && argc != 5) {
        print_usage(argv[0]);
        return 1;
    }

    std::string instance_path = argv[1];
    std::string output_path = argv[2];
    std::string method = "trivial";
    if (argc == 5) {
        if (std::string(argv[3]) != "-m") {
            print_usage(argv[0]);
            return 1;
        }
        method = argv[4];
    }

    try {
        Instance instance(instance_path);

        std::vector<size_t> tour;
        if (method == "trivial") {
            tour = trivial_tour(instance);
        } else if (method == "held-karp") {
            tour = held_karp_tour(instance);
        } else if (method == "nearest-neighbor") {
            tour = nearest_neighbor_tour(instance);
        } else {
            throw std::runtime_error("unknown method: " + method);
        }

        write_tour(output_path, instance.name(), tour);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}
