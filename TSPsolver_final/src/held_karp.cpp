#include "held_karp.hpp"

#include <climits>
#include <stdexcept>
#include <string>

static const size_t MAX_CITIES = 24;

std::vector<size_t> held_karp_tour(const Instance& instance) {
    size_t n = instance.dimension();
    if (n > MAX_CITIES) {
        throw std::runtime_error("held-karp supports at most " + std::to_string(MAX_CITIES) +
                                 " cities (this instance has " + std::to_string(n) + ")");
    }

    // With 3 cities or less every order is the same tour.
    std::vector<size_t> tour;
    if (n <= 3) {
        for (size_t city = 1; city <= n; ++city) {
            tour.push_back(city);
        }
        return tour;
    }

    // Copy the distances once: the loops below read them billions of times.
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, 0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            dist[i][j] = instance.distance(i, j);
        }
    }

    // City 1 (number 0 here) is the start. The other m = n - 1 cities are
    // the bits of a mask: bit j is city j + 1.
    // best[mask * m + j] = length of the shortest path that starts at city 1,
    // visits exactly the cities of the mask, and ends at the city of bit j.
    // parent[mask * m + j] = the city visited just before it on that path.
    size_t m = n - 1;
    size_t masks = static_cast<size_t>(1) << m;
    const int INF = INT_MAX / 2;
    const unsigned char NO_PARENT = 255;
    std::vector<int> best(masks * m, INF);
    std::vector<unsigned char> parent(masks * m, NO_PARENT);

    // Paths with one city: go from city 1 straight to it.
    for (size_t j = 0; j < m; ++j) {
        best[(static_cast<size_t>(1) << j) * m + j] = dist[0][j + 1];
    }

    // Extend the paths one city at a time.
    for (size_t mask = 1; mask < masks; ++mask) {
        for (size_t j = 0; j < m; ++j) {
            if (!(mask & (static_cast<size_t>(1) << j))) {
                continue;  // j is not in the mask
            }
            int here = best[mask * m + j];
            if (here >= INF) {
                continue;
            }
            for (size_t k = 0; k < m; ++k) {
                if (mask & (static_cast<size_t>(1) << k)) {
                    continue;  // k is already visited
                }
                size_t next_mask = mask | (static_cast<size_t>(1) << k);
                int length = here + dist[j + 1][k + 1];
                if (length < best[next_mask * m + k]) {
                    best[next_mask * m + k] = length;
                    parent[next_mask * m + k] = static_cast<unsigned char>(j);
                }
            }
        }
    }

    // Close the tour: the best last city, then back to city 1.
    size_t all = masks - 1;
    int best_length = INF;
    size_t last = 0;
    for (size_t j = 0; j < m; ++j) {
        int length = best[all * m + j] + dist[j + 1][0];
        if (length < best_length) {
            best_length = length;
            last = j;
        }
    }

    // Walk back through the parents to get the order of the cities.
    std::vector<size_t> reverse_order;
    size_t mask = all;
    size_t current = last;
    while (true) {
        reverse_order.push_back(current + 2);  // bit j is city j + 2 (numbered from 1)
        unsigned char before = parent[mask * m + current];
        mask &= ~(static_cast<size_t>(1) << current);
        if (before == NO_PARENT) {
            break;
        }
        current = before;
    }

    tour.push_back(1);
    for (size_t i = reverse_order.size(); i > 0; --i) {
        tour.push_back(reverse_order[i - 1]);
    }
    return tour;
}
