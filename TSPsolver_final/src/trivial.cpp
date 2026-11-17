#include "trivial.hpp"

std::vector<size_t> trivial_tour(const Instance& instance) {
    std::vector<size_t> tour;
    for (size_t city = 1; city <= instance.dimension(); ++city) {
        tour.push_back(city);
    }
    return tour;
}
