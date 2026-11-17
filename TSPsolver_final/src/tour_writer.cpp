#include "tour_writer.hpp"

#include <fstream>
#include <stdexcept>

std::string tour_name(const std::string& instance_name) {
    const std::string extension = ".tsp";
    std::string base = instance_name;
    bool ends_with_tsp =
        base.size() >= extension.size() &&
        base.compare(base.size() - extension.size(), extension.size(), extension) == 0;
    if (ends_with_tsp) {
        base = base.substr(0, base.size() - extension.size());
    }
    return base + ".tour";
}

void write_tour(const std::string& path,
                const std::string& instance_name,
                const std::vector<size_t>& tour) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("cannot open output file: " + path);
    }

    file << "NAME : " << tour_name(instance_name) << "\n";
    file << "TYPE : TOUR\n";
    file << "DIMENSION : " << tour.size() << "\n";
    file << "TOUR_SECTION\n";
    for (size_t i = 0; i < tour.size(); ++i) {
        file << tour[i] << "\n";
    }
    file << "-1\n";
    file << "EOF\n";

    file.close();
    if (file.fail()) {
        throw std::runtime_error("error while writing output file: " + path);
    }
}
