#include <iostream>
#include <string>
#include "Parser.h"
#include "WebBuilder.h"
#include "InterferenceGraph.h"
#include "GraphColoring.h"
#include "OutputWriter.h"
#include "Menu.h"

/**
 * @brief Entry point for the Register Allocation Tool.
 *
 * Supports two modes:
 * - Batch mode: ./myProg -b ranges.txt registers.txt allocation.txt
 * - Interactive mode: ./myProg (no arguments)
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return 0 on success, 1 on error.
 */
int main(int argc, char* argv[]) {
    if (argc == 5 && std::string(argv[1]) == "-b") {
        // Batch mode
        std::string rangesFile = argv[2];
        std::string registersFile = argv[3];
        std::string outputFile = argv[4];

        try {
            // Parse input files
            auto ranges = Parser::parseRanges(rangesFile);
            auto config = Parser::parseConfig(registersFile);

            // Build webs
            auto webs = WebBuilder::buildWebs(ranges);

            // Build interference graph
            InterferenceGraph ig;
            ig.build(webs);

            // Run the appropriate algorithm
            ColoringResult result;
            if (config.algorithm == "basic") {
                result = GraphColoring::basic(ig, config.maxRegisters);
            } else if (config.algorithm == "spilling") {
                result = GraphColoring::spilling(ig, config.maxRegisters, config.algorithmParam);
            } else if (config.algorithm == "splitting") {
                result = GraphColoring::splitting(ig, config.maxRegisters, config.algorithmParam);
            } else if (config.algorithm == "free") {
                result = GraphColoring::free(ig, config.maxRegisters);
            } else {
                std::cerr << "Error: Unknown algorithm: " << config.algorithm << std::endl;
                return 1;
            }

            // Update webs (splitting may have modified them)
            webs = ig.getWebs();

            // Write output
            OutputWriter::write(outputFile, webs, result);

            // Also print to stdout
            OutputWriter::print(webs, result);

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    } else if (argc == 1) {
        // Interactive mode
        Menu menu;
        menu.run();
    } else {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  Batch mode:       " << argv[0] << " -b ranges.txt registers.txt allocation.txt" << std::endl;
        std::cerr << "  Interactive mode:  " << argv[0] << std::endl;
        return 1;
    }

    return 0;
}
