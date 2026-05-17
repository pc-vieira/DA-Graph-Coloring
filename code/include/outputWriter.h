#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "parser.h"

/**
 * @brief Handles formatting and writing the final allocation results to a text file.
 */
class OutputWriter {
public:
    /**
     * @brief Writes the web summaries and their assigned registers/memory.
     * @param filename Target output file name.
     * @param webs The final vector of webs (including any split derivatives).
     * @param allocation The map linking Web IDs to their assigned Register (or -1 for Memory).
     * @complexity $O(|W| \times |L|)$ where $|W|$ is the number of webs and $|L|$ is the number of lines per web.
     */
    static void writeAllocation(const std::string& filename, const std::vector<Web>& webs, const std::map<std::string, int>& allocation) {
        std::ofstream outFile("../data/output/" + filename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open output file " << "../data/output/" + filename << std::endl;
            return;
        }

        outFile << "# Total number of webs followed by the listing of the program points of each one\n";
        outFile << "# program points in each web are sorted in ascending order\n";
        outFile << "webs: " << webs.size() << "\n";

        for (const Web& w : webs) {
            outFile << w.id << ": ";
            bool first = true;
            for (int line : w.lines) {
                if (!first) outFile << ",";
                outFile << line;
                
                bool isStart = (w.starts.find(line) != w.starts.end());
                bool isEnd = (w.ends.find(line) != w.ends.end());
                
                if (isStart && !isEnd) outFile << "+";
                else if (isEnd && !isStart) outFile << "-";
                
                first = false;
            }
            outFile << "\n";
        }

        int maxRegisterUsed = -1;
        int spilledCount = 0;
        
        for (const auto& pair : allocation) {
            if (pair.second == -1) {
                spilledCount++;
            } else if (pair.second > maxRegisterUsed) {
                maxRegisterUsed = pair.second;
            }
        }
        
        bool totalFailure = (spilledCount == allocation.size());
        int totalRegistersUsed = maxRegisterUsed + 1;

        outFile << "# Total number of registers used, followed by assignment to webs\n";
        outFile << "registers: " << totalRegistersUsed << "\n";

        for (const Web& w : webs) {
            int reg = allocation.at(w.id);
            if (reg == -1) {
                outFile << "M: " << w.id << "\n";
            } else {
                outFile << "r" << reg << ": " << w.id << "\n";
            }
        }

        outFile.close();
        std::cout << "Successfully wrote final allocation to " << "../data/output/" + filename << std::endl;
    }
};

#endif // OUTPUT_WRITER_H