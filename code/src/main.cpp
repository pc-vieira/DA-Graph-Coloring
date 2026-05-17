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

    if ((settings.algorithm == "spilling" || settings.algorithm == "splitting") && settings.algoParam == 0) {
        settings.algorithm = "basic";
        settings.algoParam = -1;
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
    const std::string& outputFile)
{
    AllocSettings settings = parseRegistersFile("../data/registers/" + registersFile);
    
    if (settings.numRegisters <= 0) {
        std::cerr << "Error: Invalid or missing register count (" 
                  << settings.numRegisters << "). Must be greater than 0.\n";
        return;
    }

    if ((settings.algorithm == "spilling" || settings.algorithm == "splitting") && settings.algoParam < 0) {
        std::cerr << "Error: Algorithm '" << settings.algorithm << "' requires a numeric positive parameter.\n";
        return;
    }

    std::vector<Web> webs = Parser::parseWebs("../data/ranges/" + rangesFile);
    if (webs.empty()) {
        std::cerr << "Error: No webs parsed from " << "../data/ranges/" + rangesFile << "\n";
        return;
    }

    std::map<std::string, int> allocation;
    int currentSplits = 0;

    while (true) {
        Graph<std::string> interferenceGraph = GraphBuilder::buildInterferenceGraph(webs);
        
        std::cout << "Built Interference Graph with " 
                    << interferenceGraph.getNumVertex() << " webs fighting for " 
                    << settings.numRegisters << " registers.\n";

        allocation = RegisterAllocator::allocateRegisters(interferenceGraph, settings, webs, currentSplits);

        if (allocation.count("__RESTART__")) {
            continue; 
        }
        
        break; 
    }

    OutputWriter::writeAllocation(outputFile, webs, allocation);
}

void runBatchMode(const std::string& rangesFile, const std::string& registersFile, const std::string& outputFile) {
    runPipeline(rangesFile, registersFile, outputFile);
}

void runInteractiveMenu() {
    bool dataLoaded = false;
    std::string rangesInput, registersInput;

    while (true) {
        std::cout << "\n=== Compiler Register Allocation Tool ===\n"
                  << "1. Load datasets (Ranges & Registers)\n"
                  << "2. Run Allocation\n"
                  << "3. Show Parsed Webs\n"
                  << "4. Show Interference Graph\n"
                  << "0. Exit\n"
                  << "Choice: ";

        int choice = -1;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid input.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            std::cout << "Enter Ranges filename (e.g., ranges1.txt): ";
            std::cin >> rangesInput;
            
            std::cout << "Enter Registers filename (e.g., registers1.txt): ";
            std::cin >> registersInput;
            
            dataLoaded = true;
            std::cout << "Paths loaded as:\n  -> " << "../data/ranges/" + rangesInput << "\n  -> " << "../data/registers/" + registersInput << "\n";
            break;
        }

        case 2: {
            if (!dataLoaded) { 
                std::cout << "No files loaded yet. Please select option 1 first.\n"; 
                break; 
            }
            
            std::string outputInput;
            std::cout << "Enter Output filename (e.g., allocation1.txt): ";
            std::cin >> outputInput;

            std::cout << "\n--- Running Register Allocation ---\n";
            runPipeline(rangesInput, registersInput, outputInput);
            break;
        }

        case 3: {
            if (!dataLoaded) {
                std::cout << "No files loaded yet. Please select option 1 first.\n";
                break;
            }
            std::vector<Web> webs = Parser::parseWebs("../data/ranges/" + rangesInput);
            std::cout << "\n--- Parsed Webs (" << webs.size() << ") ---\n";
            for (const Web& w : webs) {
                std::cout << w.id << " (" << w.variableName << "): ";
                bool first = true;
                for (int line : w.lines) {
                    if (!first) std::cout << ", ";
                    std::cout << line;
                    first = false;
                }
                std::cout << "\n";
            }
            break;
        }
        
        case 4: {
            if (!dataLoaded) {
                std::cout << "No files loaded yet. Please select option 1 first.\n";
                break;
            }
            std::vector<Web> webs = Parser::parseWebs("../data/ranges/" + rangesInput);
            Graph<std::string> graph = GraphBuilder::buildInterferenceGraph(webs);
            
            int numEdges = 0;
            for (auto v : graph.getVertexSet()) {
                numEdges += v->getAdj().size();
            }
            numEdges /= 2;

            std::cout << "\n--- Interference Graph ---\n";
            std::cout << "Nodes (Webs): " << graph.getNumVertex() << "\n";
            std::cout << "Edges (Interferences): " << numEdges << "\n";

            std::cout << "\nDetailed Interferences:\n";
            for (auto v : graph.getVertexSet()) {
                std::cout << "  " << v->getInfo() << " collides with: [";
                
                auto adj = v->getAdj();
                if (adj.empty()) {
                    std::cout << "";
                } else {
                    for (size_t i = 0; i < adj.size(); ++i) {
                        std::cout << adj[i]->getDest()->getInfo();
                        if (i < adj.size() - 1) std::cout << ", ";
                    }
                }
                std::cout << "]\n";
            }
            break;
        }

        case 0:
            std::cout << "Goodbye!\n";
            return;

        default:
            std::cerr << "Invalid option.\n";
            break;
        }
    }
}

/**
 * @brief Program entry point. Routes to interactive or batch mode.
 */
int main(int argc, char* argv[]) {
    if (argc == 5 && std::strcmp(argv[1], "-b") == 0) {
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
