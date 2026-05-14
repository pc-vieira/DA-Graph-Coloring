#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

/**
 * @brief Trims leading and trailing whitespace from a string.
 */
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<LiveRange> Parser::parseRanges(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Cannot open ranges file: " + filename);
    }

    std::vector<LiveRange> ranges;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Find the colon separator
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            throw std::runtime_error("Error: Malformed line (no colon): " + line);
        }

        LiveRange lr;
        lr.variable = trim(line.substr(0, colonPos));
        std::string pointsStr = trim(line.substr(colonPos + 1));

        // Parse comma-separated program points
        std::stringstream ss(pointsStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            if (token.empty()) continue;

            ProgramPoint pt;
            pt.isDef = false;
            pt.isUse = false;

            // Check for '+' or '-' suffix
            if (token.back() == '+') {
                pt.isDef = true;
                token.pop_back();
            } else if (token.back() == '-') {
                pt.isUse = true;
                token.pop_back();
            }

            try {
                pt.line = std::stoi(token);
            } catch (const std::exception& e) {
                throw std::runtime_error("Error: Invalid line number '" + token + "' in: " + line);
            }

            lr.points.push_back(pt);
            lr.lineSet.insert(pt.line);
        }

        if (lr.points.empty()) {
            throw std::runtime_error("Error: Empty live range for variable: " + lr.variable);
        }

        ranges.push_back(lr);
    }

    return ranges;
}

Config Parser::parseConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Cannot open registers file: " + filename);
    }

    Config config;
    config.maxRegisters = -1;
    config.algorithm = "";
    config.algorithmParam = 0;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));

        if (key == "registers") {
            try {
                config.maxRegisters = std::stoi(value);
            } catch (const std::exception& e) {
                throw std::runtime_error("Error: Invalid register count: " + value);
            }
        } else if (key == "algorithm") {
            // Could be "basic", "spilling, 2", "splitting, 3", "free"
            size_t commaPos = value.find(',');
            if (commaPos != std::string::npos) {
                config.algorithm = trim(value.substr(0, commaPos));
                std::string paramStr = trim(value.substr(commaPos + 1));
                try {
                    config.algorithmParam = std::stoi(paramStr);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Error: Invalid algorithm parameter: " + paramStr);
                }
            } else {
                config.algorithm = value;
            }
        }
    }

    if (config.maxRegisters < 0) {
        throw std::runtime_error("Error: Missing 'registers' field in config file.");
    }
    if (config.algorithm.empty()) {
        throw std::runtime_error("Error: Missing 'algorithm' field in config file.");
    }

    return config;
}
