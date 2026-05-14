#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "LiveRange.h"

/**
 * @brief Configuration parsed from the registers input file.
 */
struct Config {
    int maxRegisters;           ///< Maximum number of registers available
    std::string algorithm;      ///< "basic", "spilling", "splitting", or "free"
    int algorithmParam = 0;     ///< K parameter for spilling/splitting (0 otherwise)
};

/**
 * @brief Parses input files for the register allocation tool.
 */
class Parser {
public:
    /**
     * @brief Parses a live ranges input file.
     * @param filename Path to the ranges file.
     * @return Vector of LiveRange objects.
     * @throws std::runtime_error on malformed input.
     * @complexity O(L) where L = total number of lines/points in the file.
     */
    static std::vector<LiveRange> parseRanges(const std::string& filename);

    /**
     * @brief Parses a registers/config input file.
     * @param filename Path to the registers file.
     * @return Config struct with register count and algorithm settings.
     * @throws std::runtime_error on malformed input.
     * @complexity O(1) — file has a constant number of relevant lines.
     */
    static Config parseConfig(const std::string& filename);
};

#endif // PARSER_H
