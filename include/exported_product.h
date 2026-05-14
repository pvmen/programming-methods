#pragma once

#include <string>

namespace sorting_lab {

/**
 * @brief Exported product record for variant 9.
 *
 * Sorting key for the lab variant is:
 * product name, then supply volume, then destination country.
 * Amount in rubles is stored and written back, but it is not part of comparisons.
 */
struct ExportedProduct {
    std::string name;
    std::string country;
    double volume = 0.0;
    double amountRubles = 0.0;
};

inline bool operator<(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    if (lhs.name != rhs.name) {
        return lhs.name < rhs.name;
    }
    if (lhs.volume != rhs.volume) {
        return lhs.volume < rhs.volume;
    }
    return lhs.country < rhs.country;
}

inline bool operator>(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    return rhs < lhs;
}

inline bool operator<=(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    return !(rhs < lhs);
}

inline bool operator>=(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    return !(lhs < rhs);
}

}  // namespace sorting_lab
