#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <cstddef>
#include <string>
#include <vector>

/**
 * A TSP instance read from a TSPLIB file.
 *
 * The file gives either the coordinates of the cities (EUC_2D, ATT or GEO)
 * or the lower triangle of the distance matrix (EXPLICIT, LOWER_DIAG_ROW).
 * In both cases the distances between all the cities are stored.
 */
class Instance {
public:
    /**
     * Reads the instance file.
     * Throws std::runtime_error if the file cannot be opened, if NAME or
     * DIMENSION is missing or invalid, or if the data has the wrong size.
     */
    explicit Instance(const std::string& path);

    /** Name of the instance, as written in the NAME entry. */
    const std::string& name() const;

    /** Number of cities, as written in the DIMENSION entry. */
    size_t dimension() const;

    /**
     * Distance between city i and city j (cities are numbered from 0 here).
     * Throws std::runtime_error if the file had no distance data.
     */
    int distance(size_t i, size_t j) const;

private:
    std::string _name;
    size_t _dimension;
    std::vector<std::vector<int>> _distances;
};

#endif
