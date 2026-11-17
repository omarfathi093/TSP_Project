#ifndef TOUR_WRITER_HPP
#define TOUR_WRITER_HPP

#include <cstddef>
#include <string>
#include <vector>

/**
 * Name written in the solution file: the instance name without a
 * trailing ".tsp", followed by ".tour" (example.tsp -> example.tour).
 */
std::string tour_name(const std::string& instance_name);

/**
 * Writes a tour in TSPLIB format (NAME, TYPE, DIMENSION, TOUR_SECTION,
 * the cities, -1, EOF).
 * Throws std::runtime_error if the file cannot be written.
 */
void write_tour(const std::string& path,
                const std::string& instance_name,
                const std::vector<size_t>& tour);

#endif
