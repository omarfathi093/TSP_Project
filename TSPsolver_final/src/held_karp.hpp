#ifndef HELD_KARP_HPP
#define HELD_KARP_HPP

#include <cstddef>
#include <vector>

#include "instance.hpp"

/**
 * Held-Karp method: dynamic programming over the subsets of cities.
 * Returns the shortest possible tour, starting at city 1.
 * The memory doubles with each city, so it only works up to 24 cities:
 * for more cities it throws std::runtime_error.
 */
std::vector<size_t> held_karp_tour(const Instance& instance);

#endif
