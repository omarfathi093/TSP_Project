#include "nearest_neighbor.hpp"

std::vector<size_t> nearest_neighbor_tour(const Instance& instance) {
    size_t n = instance.dimension();
    std::vector<size_t> tour;
    std::vector<bool> visited(n, false);

    // Cities are numbered from 0 here, and from 1 in the tour.
    size_t current = 0;
    visited[current] = true;
    tour.push_back(current + 1);

    while (tour.size() < n) {
        size_t closest = n;  // n means "none found yet"
        int closest_distance = 0;
        for (size_t city = 0; city < n; ++city) {
            if (visited[city]) {
                continue;
            }
            int d = instance.distance(current, city);
            // "<" keeps the first city found when two are at the same distance.
            if (closest == n || d < closest_distance) {
                closest = city;
                closest_distance = d;
            }
        }
        visited[closest] = true;
        tour.push_back(closest + 1);
        current = closest;
    }
    return tour;
}
