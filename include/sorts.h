#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

namespace sorting_lab {

/**
 * @brief Checks that values are sorted in nondecreasing order.
 */
template <typename T>
bool isSorted(const std::vector<T>& values) {
    return std::is_sorted(values.begin(), values.end());
}

/**
 * @brief Bubble sort.
 */
template <typename T>
void bubbleSort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    for (std::size_t right = values.size(); right > 1; --right) {
        bool swapped = false;

        for (std::size_t i = 0; i + 1 < right; ++i) {
            if (values[i] > values[i + 1]) {
                std::swap(values[i], values[i + 1]);
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }
    }
}

/**
 * @brief Cocktail shaker sort.
 */
template <typename T>
void shakerSort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    std::size_t left = 0;
    std::size_t right = values.size() - 1;
    bool swapped = true;

    while (swapped && left < right) {
        swapped = false;
        for (std::size_t i = left; i < right; ++i) {
            if (values[i] > values[i + 1]) {
                std::swap(values[i], values[i + 1]);
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }

        --right;
        swapped = false;

        for (std::size_t i = right; i > left; --i) {
            if (values[i - 1] > values[i]) {
                std::swap(values[i - 1], values[i]);
                swapped = true;
            }
        }

        ++left;
    }
}

namespace detail {

template <typename T>
void mergeSortRange(std::vector<T>& values,
                    std::vector<T>& buffer,
                    std::size_t left,
                    std::size_t right) {
    if (right - left < 2) {
        return;
    }

    const std::size_t middle = left + (right - left) / 2;
    mergeSortRange(values, buffer, left, middle);
    mergeSortRange(values, buffer, middle, right);

    std::size_t i = left;
    std::size_t j = middle;
    std::size_t k = left;

    while (i < middle && j < right) {
        if (!(values[j] < values[i])) {
            buffer[k++] = values[i++];
        } else {
            buffer[k++] = values[j++];
        }
    }

    while (i < middle) {
        buffer[k++] = values[i++];
    }
    while (j < right) {
        buffer[k++] = values[j++];
    }

    for (std::size_t index = left; index < right; ++index) {
        values[index] = buffer[index];
    }
}

}  // namespace detail

/**
 * @brief Stable merge sort.
 */
template <typename T>
void mergeSort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    std::vector<T> buffer(values.size());
    detail::mergeSortRange(values, buffer, 0, values.size());
}

}  // namespace sorting_lab
