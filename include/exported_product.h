#pragma once

#include <string>
#include <tuple>

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

/**
 * @brief Builds the lexicographic comparison key required by variant 9.
 */
inline auto comparisonKey(const ExportedProduct& product) {
    return std::tie(product.name, product.volume, product.country);
}

inline bool operator<(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    return comparisonKey(lhs) < comparisonKey(rhs);
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

inline bool operator==(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    return comparisonKey(lhs) == comparisonKey(rhs);
}

inline bool operator!=(const ExportedProduct& lhs, const ExportedProduct& rhs) {
    return !(lhs == rhs);
}

}  // namespace sorting_lab
