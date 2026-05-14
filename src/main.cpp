#include "csv_io.h"
#include "exported_product.h"
#include "sorts.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

using sorting_lab::ExportedProduct;
using Clock = std::chrono::steady_clock;

const std::vector<std::size_t>& benchmarkSizes() {
    static const std::vector<std::size_t> sizes{
        100, 250, 500, 1000, 2500, 5000, 10000, 25000, 50000, 110000,
    };
    return sizes;
}

std::string zeroPad(std::size_t value, int width) {
    std::ostringstream output;
    output << std::setw(width) << std::setfill('0') << value;
    return output.str();
}

fs::path inputPathForSize(std::size_t size) {
    return fs::path("data") / ("input_" + std::to_string(size) + ".csv");
}

fs::path outputPathFor(std::string algorithm, std::size_t size) {
    if (algorithm == "std::sort") {
        algorithm = "std_sort";
    }
    return fs::path("output") / ("sorted_" + algorithm + "_" + std::to_string(size) + ".csv");
}

void printUsage(const char* executable) {
    std::cerr << "Usage:\n"
              << "  " << executable << " generate [size]\n"
              << "  " << executable
              << " sort <input.csv> <output.csv> <bubble|shaker|merge|std>\n"
              << "  " << executable << " preview <input.csv> <bubble|shaker|merge|std> [rows]\n"
              << "  " << executable << " benchmark\n";
}

std::size_t parseSize(const std::string& value) {
    std::size_t parsed = 0;
    std::size_t processed = 0;

    try {
        parsed = static_cast<std::size_t>(std::stoull(value, &processed));
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid size: " + value);
    }

    if (processed != value.size() || parsed == 0) {
        throw std::runtime_error("Invalid size: " + value);
    }

    return parsed;
}

std::vector<ExportedProduct> generateProducts(std::size_t count) {
    const std::vector<std::string> countries{
        "Armenia", "Belarus", "China", "India", "Kazakhstan", "Serbia",
        "Turkey", "UAE", "Uzbekistan", "Vietnam",
    };

    std::vector<ExportedProduct> products;
    products.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const std::string name = "Product " + zeroPad(i, 6);
        const std::string country = countries[i % countries.size()];
        const double volume = 100.0 + static_cast<double>(i) * 0.5;
        const double amountRubles = volume * (1500.0 + static_cast<double>(i % 250));

        products.push_back(ExportedProduct{
            name,
            country,
            volume,
            amountRubles,
        });
    }

    const std::size_t step = std::max<std::size_t>(3, count / 1000);
    for (std::size_t i = 1; i + 1 < products.size(); i += step) {
        std::swap(products[i], products[i + 1]);
    }

    return products;
}

void generateOneDataset(std::size_t size) {
    const auto products = generateProducts(size);
    sorting_lab::writeProductsToCsv(inputPathForSize(size), products);
}

void generateCommand(int argc, char* argv[]) {
    if (argc == 3) {
        const auto size = parseSize(argv[2]);
        generateOneDataset(size);
        std::cout << "Generated " << inputPathForSize(size) << '\n';
        return;
    }

    if (argc != 2) {
        throw std::runtime_error("generate expects zero or one size argument");
    }

    for (const auto size : benchmarkSizes()) {
        generateOneDataset(size);
        std::cout << "Generated " << inputPathForSize(size) << '\n';
    }
}

void applySort(std::vector<ExportedProduct>& products, const std::string& algorithm) {
    if (algorithm == "bubble") {
        sorting_lab::bubbleSort(products);
    } else if (algorithm == "shaker") {
        sorting_lab::shakerSort(products);
    } else if (algorithm == "merge") {
        sorting_lab::mergeSort(products);
    } else if (algorithm == "std" || algorithm == "std_sort" || algorithm == "std::sort") {
        std::sort(products.begin(), products.end());
    } else {
        throw std::runtime_error("Unknown algorithm: " + algorithm);
    }
}

void sortCommand(int argc, char* argv[]) {
    if (argc != 5) {
        throw std::runtime_error("sort expects input, output and algorithm arguments");
    }

    auto products = sorting_lab::readProductsFromCsv(argv[2]);
    applySort(products, argv[4]);

    if (!sorting_lab::isSorted(products)) {
        throw std::runtime_error("Internal error: sort result is not ordered");
    }

    sorting_lab::writeProductsToCsv(argv[3], products);
    std::cout << "Sorted " << products.size() << " rows into " << argv[3] << '\n';
}

void printProductsPreview(const std::string& title,
                          const std::vector<ExportedProduct>& products,
                          std::size_t rows) {
    const std::size_t shown = std::min(rows, products.size());

    std::cout << title << " (" << shown << " of " << products.size() << " rows)\n";
    std::cout << std::left << std::setw(6) << "#"
              << std::setw(18) << "name"
              << std::setw(14) << "country"
              << std::right << std::setw(12) << "volume"
              << std::setw(16) << "amountRubles" << '\n';

    for (std::size_t i = 0; i < shown; ++i) {
        const auto& product = products[i];
        std::cout << std::left << std::setw(6) << i
                  << std::setw(18) << product.name
                  << std::setw(14) << product.country
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(12) << product.volume
                  << std::setw(16) << product.amountRubles << '\n';
    }
}

void previewCommand(int argc, char* argv[]) {
    if (argc != 4 && argc != 5) {
        throw std::runtime_error("preview expects input.csv, algorithm and optional row count");
    }

    const std::size_t rows = argc == 5 ? parseSize(argv[4]) : 10;
    const auto before = sorting_lab::readProductsFromCsv(argv[2]);
    auto after = before;

    applySort(after, argv[3]);
    if (!sorting_lab::isSorted(after)) {
        throw std::runtime_error("Internal error: preview result is not ordered");
    }

    printProductsPreview("Before sorting: " + std::string(argv[2]), before, rows);
    std::cout << '\n';
    printProductsPreview("After sorting by " + std::string(argv[3]), after, rows);
    std::cout << "\nSorted correctly: yes\n";
}

template <typename SortFunction>
double measureMilliseconds(SortFunction sortFunction) {
    const auto started = Clock::now();
    sortFunction();
    const auto finished = Clock::now();

    return std::chrono::duration<double, std::milli>(finished - started).count();
}

double benchmarkAlgorithm(const std::vector<ExportedProduct>& source,
                          const std::string& algorithm,
                          std::size_t size) {
    auto products = source;

    const double milliseconds = measureMilliseconds([&products, &algorithm]() {
        applySort(products, algorithm);
    });

    if (!sorting_lab::isSorted(products)) {
        throw std::runtime_error("Internal error: " + algorithm + " result is not ordered");
    }

    sorting_lab::writeProductsToCsv(outputPathFor(algorithm, size), products);
    return milliseconds;
}

void benchmarkCommand(int argc) {
    if (argc != 2) {
        throw std::runtime_error("benchmark does not accept arguments");
    }

    fs::create_directories("output");
    std::ofstream benchmark("output/benchmark.csv");
    if (!benchmark) {
        throw std::runtime_error("Cannot open output/benchmark.csv");
    }

    benchmark << "size,algorithm,milliseconds\n";

    const std::vector<std::string> algorithms{
        "bubble", "shaker", "merge", "std::sort",
    };

    for (const auto size : benchmarkSizes()) {
        generateOneDataset(size);
        const auto products = sorting_lab::readProductsFromCsv(inputPathForSize(size));

        for (const auto& algorithm : algorithms) {
            const double milliseconds = benchmarkAlgorithm(products, algorithm, size);
            benchmark << size << ',' << algorithm << ',' << std::fixed << std::setprecision(6)
                      << milliseconds << '\n';
            std::cout << std::setw(6) << size << " | " << std::setw(9) << algorithm
                      << " | " << std::fixed << std::setprecision(3)
                      << milliseconds << " ms\n";
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        const std::string command = argv[1];

        if (command == "generate") {
            generateCommand(argc, argv);
        } else if (command == "sort") {
            sortCommand(argc, argv);
        } else if (command == "preview") {
            previewCommand(argc, argv);
        } else if (command == "benchmark") {
            benchmarkCommand(argc);
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
