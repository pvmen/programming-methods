#pragma once

#include "exported_product.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sorting_lab {

/**
 * @brief Parses one CSV row with RFC 4180-style quoted fields.
 */
inline std::vector<std::string> parseCsvRow(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];

        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    if (inQuotes) {
        throw std::runtime_error("Malformed CSV row: unmatched quote");
    }

    fields.push_back(current);
    return fields;
}

/**
 * @brief Escapes one field for CSV output.
 */
inline std::string escapeCsvField(const std::string& field) {
    const bool needsQuotes = field.find_first_of(",\"\n\r") != std::string::npos;
    if (!needsQuotes) {
        return field;
    }

    std::string escaped;
    escaped.reserve(field.size() + 2);
    escaped.push_back('"');
    for (const char ch : field) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped.push_back(ch);
        }
    }
    escaped.push_back('"');
    return escaped;
}

inline double parseDouble(const std::string& value, const std::string& fieldName, std::size_t lineNumber) {
    std::size_t processed = 0;
    double parsed = 0.0;

    try {
        parsed = std::stod(value, &processed);
    } catch (const std::exception&) {
        throw std::runtime_error("CSV row " + std::to_string(lineNumber)
                                 + " has invalid " + fieldName + ": " + value);
    }

    if (processed != value.size()) {
        throw std::runtime_error("CSV row " + std::to_string(lineNumber)
                                 + " has invalid " + fieldName + ": " + value);
    }

    return parsed;
}

inline std::string formatDouble(double value) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(2) << value;
    return output.str();
}

/**
 * @brief Reads exported products from a CSV file.
 */
inline std::vector<ExportedProduct> readProductsFromCsv(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Cannot open input CSV: " + path.string());
    }

    std::vector<ExportedProduct> products;
    std::string line;

    if (!std::getline(input, line)) {
        return products;
    }

    std::size_t lineNumber = 1;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        auto fields = parseCsvRow(line);
        if (fields.size() != 4) {
            throw std::runtime_error("CSV row " + std::to_string(lineNumber)
                                     + " must contain 4 fields");
        }

        products.push_back(ExportedProduct{
            fields[0],
            fields[1],
            parseDouble(fields[2], "volume", lineNumber),
            parseDouble(fields[3], "amountRubles", lineNumber),
        });
    }

    return products;
}

/**
 * @brief Writes exported products to a CSV file.
 */
inline void writeProductsToCsv(const std::filesystem::path& path,
                               const std::vector<ExportedProduct>& products) {
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("Cannot open output CSV: " + path.string());
    }

    output << "name,country,volume,amount_rubles\n";
    for (const auto& product : products) {
        output << escapeCsvField(product.name) << ','
               << escapeCsvField(product.country) << ','
               << formatDouble(product.volume) << ','
               << formatDouble(product.amountRubles) << '\n';
    }
}

}  // namespace sorting_lab
