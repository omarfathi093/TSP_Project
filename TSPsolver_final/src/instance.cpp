#include "instance.hpp"

#include <cmath>
#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>

/** Removes spaces, tabs and line ends at both ends of a string. */
static std::string trim(const std::string& text) {
    const std::string blanks = " \t\r\n";
    size_t first = text.find_first_not_of(blanks);
    if (first == std::string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(blanks);
    return text.substr(first, last - first + 1);
}

/** True if the keyword starts a data section, like NODE_COORD_SECTION. */
static bool is_section(const std::string& keyword) {
    const std::string ending = "_SECTION";
    if (keyword.size() < ending.size()) {
        return false;
    }
    return keyword.compare(keyword.size() - ending.size(), ending.size(), ending) == 0;
}

/** Converts the value of the DIMENSION entry to a number. */
static size_t parse_dimension(const std::string& value) {
    size_t used = 0;
    long number = 0;
    try {
        number = std::stol(value, &used);
    } catch (const std::exception&) {
        throw std::runtime_error("invalid DIMENSION value: '" + value + "'");
    }
    if (used != value.size() || number <= 0) {
        throw std::runtime_error("invalid DIMENSION value: '" + value + "'");
    }
    return static_cast<size_t>(number);
}

/** TSPLIB rounds to the nearest integer like this. */
static int nint(double x) {
    return static_cast<int>(x + 0.5);
}

/** EUC_2D: straight-line distance, rounded. */
static int euc_2d(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    return nint(std::sqrt(dx * dx + dy * dy));
}

/** ATT: special distance of the att48 instance. */
static int att(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    double r = std::sqrt((dx * dx + dy * dy) / 10.0);
    int t = nint(r);
    if (t < r) {
        return t + 1;
    }
    return t;
}

/** GEO coordinates are written as degrees.minutes; this gives radians. */
static double geo_radians(double x) {
    const double pi = 3.141592;
    int degrees = static_cast<int>(x);
    double minutes = x - degrees;
    return pi * (degrees + 5.0 * minutes / 3.0) / 180.0;
}

/** GEO: distance on the earth between two (latitude, longitude) points. */
static int geo(double x1, double y1, double x2, double y2) {
    const double earth_radius = 6378.388;
    double lat1 = geo_radians(x1);
    double lon1 = geo_radians(y1);
    double lat2 = geo_radians(x2);
    double lon2 = geo_radians(y2);
    double q1 = std::cos(lon1 - lon2);
    double q2 = std::cos(lat1 - lat2);
    double q3 = std::cos(lat1 + lat2);
    double value = 0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3);
    if (value > 1.0) {
        value = 1.0;
    }
    if (value < -1.0) {
        value = -1.0;
    }
    return static_cast<int>(earth_radius * std::acos(value) + 1.0);
}

Instance::Instance(const std::string& path) : _name(), _dimension(0), _distances() {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("cannot open instance file: " + path);
    }

    bool has_name = false;
    bool has_dimension = false;
    std::string weight_type;
    std::string weight_format;
    std::string section;
    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<int> weights;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        // The keyword is what comes before the first ':' (or the whole line
        // if there is no ':'). Some files write "NAME : x", others "NAME: x".
        size_t colon = line.find(':');
        std::string keyword = trim(line.substr(0, colon));

        if (keyword == "EOF") {
            break;
        }
        if (is_section(keyword)) {
            section = keyword;
            continue;
        }

        // A line without ':' is a line of the data part.
        if (colon == std::string::npos) {
            std::istringstream numbers(line);
            if (section == "NODE_COORD_SECTION") {
                double index = 0;
                double x = 0;
                double y = 0;
                if (!(numbers >> index >> x >> y)) {
                    throw std::runtime_error("bad coordinate line: '" + line + "'");
                }
                xs.push_back(x);
                ys.push_back(y);
            } else if (section == "EDGE_WEIGHT_SECTION") {
                int weight = 0;
                while (numbers >> weight) {
                    weights.push_back(weight);
                }
            }
            continue;
        }

        std::string value = trim(line.substr(colon + 1));
        if (keyword == "NAME") {
            _name = value;
            has_name = true;
        } else if (keyword == "DIMENSION") {
            _dimension = parse_dimension(value);
            has_dimension = true;
        } else if (keyword == "EDGE_WEIGHT_TYPE") {
            weight_type = value;
        } else if (keyword == "EDGE_WEIGHT_FORMAT") {
            weight_format = value;
        }
    }

    if (!has_name) {
        throw std::runtime_error("missing NAME in the header of " + path);
    }
    if (!has_dimension) {
        throw std::runtime_error("missing DIMENSION in the header of " + path);
    }

    size_t n = _dimension;
    if (weight_type == "EUC_2D" || weight_type == "ATT" || weight_type == "GEO") {
        // Distances computed from the coordinates.
        if (xs.size() != n) {
            throw std::runtime_error("expected " + std::to_string(n) + " coordinates in " +
                                     path + " but found " + std::to_string(xs.size()));
        }
        _distances.assign(n, std::vector<int>(n, 0));
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i == j) {
                    continue;
                }
                if (weight_type == "EUC_2D") {
                    _distances[i][j] = euc_2d(xs[i], ys[i], xs[j], ys[j]);
                } else if (weight_type == "ATT") {
                    _distances[i][j] = att(xs[i], ys[i], xs[j], ys[j]);
                } else {
                    _distances[i][j] = geo(xs[i], ys[i], xs[j], ys[j]);
                }
            }
        }
    } else if (weight_type == "EXPLICIT" && weight_format == "LOWER_DIAG_ROW") {
        // The lower triangle of the matrix, row by row, with the diagonal.
        if (weights.size() != n * (n + 1) / 2) {
            throw std::runtime_error("expected " + std::to_string(n * (n + 1) / 2) +
                                     " distances in " + path + " but found " +
                                     std::to_string(weights.size()));
        }
        _distances.assign(n, std::vector<int>(n, 0));
        size_t next = 0;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                _distances[i][j] = weights[next];
                _distances[j][i] = weights[next];
                ++next;
            }
        }
    }
}

const std::string& Instance::name() const {
    return _name;
}

size_t Instance::dimension() const {
    return _dimension;
}

int Instance::distance(size_t i, size_t j) const {
    if (_distances.empty()) {
        throw std::runtime_error("this instance has no distance data");
    }
    if (i >= _dimension || j >= _dimension) {
        throw std::runtime_error("city number out of range");
    }
    return _distances[i][j];
}
