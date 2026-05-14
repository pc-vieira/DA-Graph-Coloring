#include "OutputWriter.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <set>

std::string OutputWriter::format(const std::vector<Web>& webs,
                                  const ColoringResult& result) {
    std::ostringstream oss;

    // Section 1: Webs listing
    oss << "webs: " << webs.size() << "\n";
    for (size_t i = 0; i < webs.size(); i++) {
        oss << "web" << webs[i].id << ": " << webs[i].formatPoints() << "\n";
    }

    // Section 2: Register assignment
    oss << "registers: " << result.registersUsed << "\n";

    // Group webs by their assigned register
    // First output register assignments, then memory assignments
    std::set<int> spilledSet(result.spilledWebIds.begin(), result.spilledWebIds.end());

    if (result.registersUsed > 0) {
        // Group by register
        std::map<int, std::vector<int>> regToWebs;
        for (size_t i = 0; i < result.webColors.size(); i++) {
            if (result.webColors[i] >= 0) {
                regToWebs[result.webColors[i]].push_back(i);
            }
        }

        for (auto& [reg, webIds] : regToWebs) {
            for (int wid : webIds) {
                oss << "r" << reg << ": web" << wid << "\n";
            }
        }
    }

    // Memory-spilled webs
    for (int wid : result.spilledWebIds) {
        oss << "M: web" << wid << "\n";
    }

    // If all webs are spilled and registersUsed is 0, output M for all
    if (result.registersUsed == 0 && result.spilledWebIds.empty()) {
        for (size_t i = 0; i < webs.size(); i++) {
            oss << "M: web" << i << "\n";
        }
    }

    return oss.str();
}

void OutputWriter::write(const std::string& filename,
                          const std::vector<Web>& webs,
                          const ColoringResult& result) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open output file: " << filename << std::endl;
        return;
    }
    file << format(webs, result);
    file.close();
}

void OutputWriter::print(const std::vector<Web>& webs,
                          const ColoringResult& result) {
    std::cout << "\n" << format(webs, result) << std::endl;
}
