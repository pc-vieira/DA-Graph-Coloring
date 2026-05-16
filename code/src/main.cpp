#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>

#include "parser.h"
#include "graphBuilder.h"
#include "registerAllocator.h"
#include "outputWriter.h"
#include "graph.h"

// Prototypes
void runInteractiveMenu();
void runBatchMode(const std::string& rangesFile, const std::string& registersFile, const std::string& outputFile);
AllocSettings parseRegistersFile(const std::string& filename);

/**
 * @brief Parses the registers.txt file to extract configuration data.
 * @complexity $O(N)$ where $N$ is the number of lines in the configuration file.
 */
AllocSettings parseRegistersFile(const std::string& filename) {
    AllocSettings settings;
    std::ifstream file(filename);
    std::string line;
    if (!file.is_open()) return settings;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.find("registers:") != std::string::npos) {
            size_t colonPos = line.find(':');
            settings.numRegisters = std::stoi(line.substr(colonPos + 1));
        }
        else if (line.find("algorithm:") != std::string::npos) {
            size_t colonPos = line.find(':');
            std::string algoPart = line.substr(colonPos + 1);
            
            size_t commaPos = algoPart.find(',');
            if (commaPos != std::string::npos) {
                std::string name = algoPart.substr(0, commaPos);
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                settings.algorithm = name;
                
                settings.algoParam = std::stoi(algoPart.substr(commaPos + 1));
            } else {
                algoPart.erase(0, algoPart.find_first_not_of(" \t"));
                algoPart.erase(algoPart.find_last_not_of(" \t") + 1);
                settings.algorithm = algoPart;
            }
        }
    }
    return settings;
}

/**
 * @brief Orchestrates the full pipeline for Register Allocation.
 * @complexity $O(I \times (|V|^2 + |E|))$ where $I$ is the number of splitting iterations.
 */
static void runPipeline(
    const std::string& rangesFile,
    const std::string& registersFile,
    const std::string& outputFile,
    bool verbose = true)
{
    AllocSettings settings = parseRegistersFile(registersFile);
    if (settings.numRegisters <= 0) {
        std::cerr << "Error: Could not parse a valid register count from " << registersFile << "\n";
        return;
    }

    std::vector<Web> webs = Parser::parseWebs(rangesFile);
    if (webs.empty()) {
        std::cerr << "Error: No webs parsed from " << rangesFile << "\n";
        return;
    }

    std::map<std::string, int> allocation;
    int currentSplits = 0;

    while (true) {
        Graph<std::string> interferenceGraph = GraphBuilder::buildInterferenceGraph(webs);
        
        if (verbose) {
            std::cout << "Built Interference Graph with " 
                      << interferenceGraph.getNumVertex() << " webs fighting for " 
                      << settings.numRegisters << " registers.\n";
        }

        allocation = RegisterAllocator::allocateRegisters(interferenceGraph, settings, webs, currentSplits);

        if (allocation.count("__RESTART__")) {
            continue; 
        }
        
        break; 
    }

    OutputWriter::writeAllocation(outputFile, webs, allocation);
}

/**
 * @brief Program entry point. Routes to interactive or batch mode.
 */
int main(int argc, char* argv[]) {
    if (argc >= 5 && std::strcmp(argv[1], "-b") == 0) {
        std::string rangesFile    = argv[2];
        std::string registersFile = argv[3];
        std::string outputFile    = argv[4];
        
        runBatchMode(rangesFile, registersFile, outputFile);
    } else if (argc == 1) {
        std::cout << "Starting Interactive Mode...\n";
        runInteractiveMenu();
    } else {
        std::cerr << "Usage:\n"
                  << "  Interactive:  " << argv[0] << "\n"
                  << "  Batch:        " << argv[0] << " -b <ranges.txt> <registers.txt> <output.txt>\n";
        return 1;
    }
    return 0;
}

void runBatchMode(const std::string& rangesFile, const std::string& registersFile, const std::string& outputFile) {
    runPipeline(rangesFile, registersFile, outputFile, false);
}

void runInteractiveMenu() {
    std::string rangesFile;
    std::string registersFile;
    bool dataLoaded = false;

    while (true) {
        std::cout << "\n=== Compiler Register Allocation Tool ===\n"
                  << "1. Load datasets (Ranges & Registers)\n"
                  << "2. Run Allocation\n"
                  << "0. Exit\n"
                  << "Choice: ";

        int choice = -1;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cerr << "Invalid input.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            std::string rangesInput, registersInput;
            
            std::cout << "Enter Ranges filename (e.g., ranges1.txt): ";
            std::cin >> rangesInput;
            rangesFile = "../data/ranges/" + rangesInput;
            
            std::cout << "Enter Registers filename (e.g., registers2.txt): ";
            std::cin >> registersInput;
            registersFile = "../data/registers/" + registersInput;
            
            dataLoaded = true;
            std::cout << "Paths loaded as:\n  -> " << rangesFile << "\n  -> " << registersFile << "\n";
            break;
        }

        case 2: {
            if (!dataLoaded) { 
                std::cout << "No files loaded yet. Please select option 1 first.\n"; 
                break; 
            }
            
            std::string outputInput;
            std::string outputFile;
            std::cout << "Enter Output filename (e.g., interactive_alloc.txt): ";
            std::cin >> outputInput;
            outputFile = "../data/output/" + outputInput;

            std::cout << "\n--- Running Register Allocation ---\n";
            runPipeline(rangesFile, registersFile, outputFile, true);
            break;
        }

        case 0:
            std::cout << "Goodbye!\n";
            return;

        default:
            std::cerr << "Invalid option.\n";
        }
    }
}