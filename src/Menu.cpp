#include "Menu.h"
#include "WebBuilder.h"
#include "OutputWriter.h"
#include <iostream>
#include <limits>

void Menu::displayMenu() const {
    std::cout << "\n===== Register Allocation Tool =====" << std::endl;
    std::cout << "1. Load ranges file" << std::endl;
    std::cout << "2. Load registers/config file" << std::endl;
    std::cout << "3. Display parsed live ranges" << std::endl;
    std::cout << "4. Display webs" << std::endl;
    std::cout << "5. Display interference graph" << std::endl;
    std::cout << "6. Run register allocation" << std::endl;
    std::cout << "7. Display results" << std::endl;
    std::cout << "8. Save results to file" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "Option: ";
}

void Menu::run() {
    int option;
    do {
        displayMenu();
        if (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (option) {
            case 1: loadRanges(); break;
            case 2: loadConfig(); break;
            case 3: displayRanges(); break;
            case 4: displayWebs(); break;
            case 5: displayGraph(); break;
            case 6: runAllocation(); break;
            case 7: displayResults(); break;
            case 8: saveResults(); break;
            case 0: std::cout << "Exiting." << std::endl; break;
            default: std::cout << "Invalid option." << std::endl;
        }
    } while (option != 0);
}

void Menu::loadRanges() {
    std::string filename;
    std::cout << "Enter ranges file path: ";
    std::getline(std::cin, filename);

    try {
        ranges = Parser::parseRanges(filename);
        rangesLoaded = true;
        websBuilt = false;
        graphBuilt = false;
        allocationDone = false;
        std::cout << "Loaded " << ranges.size() << " live range(s)." << std::endl;

        // Automatically build webs
        webs = WebBuilder::buildWebs(ranges);
        websBuilt = true;
        std::cout << "Built " << webs.size() << " web(s)." << std::endl;

        // Automatically build interference graph
        ig = InterferenceGraph();
        ig.build(webs);
        graphBuilt = true;
        std::cout << "Interference graph built." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Menu::loadConfig() {
    std::string filename;
    std::cout << "Enter registers/config file path: ";
    std::getline(std::cin, filename);

    try {
        config = Parser::parseConfig(filename);
        configLoaded = true;
        allocationDone = false;
        std::cout << "Config loaded: " << config.maxRegisters << " registers, algorithm: "
                  << config.algorithm;
        if (config.algorithmParam > 0) {
            std::cout << " (param: " << config.algorithmParam << ")";
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Menu::displayRanges() const {
    if (!rangesLoaded) {
        std::cout << "No ranges loaded. Use option 1 first." << std::endl;
        return;
    }

    std::cout << "\n=== Live Ranges ===" << std::endl;
    for (const auto& lr : ranges) {
        std::cout << lr.variable << ": ";
        for (size_t i = 0; i < lr.points.size(); i++) {
            if (i > 0) std::cout << ",";
            std::cout << lr.points[i].line;
            if (lr.points[i].isDef) std::cout << "+";
            if (lr.points[i].isUse) std::cout << "-";
        }
        std::cout << std::endl;
    }
    std::cout << "===================\n" << std::endl;
}

void Menu::displayWebs() const {
    if (!websBuilt) {
        std::cout << "No webs built. Load ranges first." << std::endl;
        return;
    }

    std::cout << "\n=== Webs ===" << std::endl;
    for (const auto& web : webs) {
        std::cout << "web" << web.id << " (" << web.variable << "): "
                  << web.formatPoints() << std::endl;
    }
    std::cout << "============\n" << std::endl;
}

void Menu::displayGraph() const {
    if (!graphBuilt) {
        std::cout << "No interference graph built. Load ranges first." << std::endl;
        return;
    }
    ig.display();
}

void Menu::runAllocation() {
    if (!graphBuilt) {
        std::cout << "No interference graph built. Load ranges first." << std::endl;
        return;
    }
    if (!configLoaded) {
        std::cout << "No config loaded. Use option 2 first." << std::endl;
        return;
    }

    // Re-build the graph to ensure clean state
    ig = InterferenceGraph();
    ig.build(webs);

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
        return;
    }

    allocationDone = true;
    // Update webs from ig (splitting may have changed them)
    webs = ig.getWebs();

    std::cout << "\nAllocation complete." << std::endl;
    if (result.success) {
        std::cout << "Success: " << result.registersUsed << " register(s) used." << std::endl;
    } else {
        std::cout << "Allocation infeasible with " << config.maxRegisters << " register(s)." << std::endl;
    }
}

void Menu::displayResults() const {
    if (!allocationDone) {
        std::cout << "No allocation done. Use option 6 first." << std::endl;
        return;
    }
    OutputWriter::print(webs, result);
}

void Menu::saveResults() {
    if (!allocationDone) {
        std::cout << "No allocation done. Use option 6 first." << std::endl;
        return;
    }

    std::cout << "Enter output file path: ";
    std::getline(std::cin, outputFile);
    OutputWriter::write(outputFile, webs, result);
    std::cout << "Results saved to: " << outputFile << std::endl;
}
