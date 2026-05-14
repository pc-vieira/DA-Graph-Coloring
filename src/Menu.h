#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "LiveRange.h"
#include "Web.h"
#include "InterferenceGraph.h"
#include "GraphColoring.h"
#include "Parser.h"

/**
 * @brief Interactive command-line menu for the Register Allocation Tool.
 */
class Menu {
public:
    /**
     * @brief Runs the interactive menu loop.
     */
    void run();

private:
    std::vector<LiveRange> ranges;
    std::vector<Web> webs;
    InterferenceGraph ig;
    Config config;
    ColoringResult result;

    bool rangesLoaded = false;
    bool configLoaded = false;
    bool websBuilt = false;
    bool graphBuilt = false;
    bool allocationDone = false;

    std::string outputFile;

    void displayMenu() const;
    void loadRanges();
    void loadConfig();
    void displayRanges() const;
    void displayWebs() const;
    void displayGraph() const;
    void runAllocation();
    void displayResults() const;
    void saveResults();
};

#endif // MENU_H
