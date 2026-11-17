#ifndef TRIVIAL_HPP
#define TRIVIAL_HPP

#include <cstddef>
#include <vector>

#include "instance.hpp"

/**
 * Trivial method: the tour visits the cities in the order of their indices,
 * 1, 2, ..., n (and then goes back to 1).
 * Returns the n city numbers, starting at 1.
 */
std::vector<size_t> trivial_tour(const Instance& instance);

#endif
