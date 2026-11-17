#ifndef NEAREST_NEIGHBOR_HPP
#define NEAREST_NEIGHBOR_HPP

#include <cstddef>
#include <vector>

#include "instance.hpp"

/**
 * Nearest neighbor method: the tour starts at city 1 and goes each time to
 * the closest city not visited yet (the smaller number wins a tie).
 * Fast, but the tour is not optimal.
 * Returns the n city numbers, starting at 1.
 */
std::vector<size_t> nearest_neighbor_tour(const Instance& instance);

#endif
