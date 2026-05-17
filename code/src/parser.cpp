#include "parser.h"

std::vector<Web> Parser::parseWebs(const std::string& filename) {
    std::vector<Web> allWebs;
    std::ifstream file(filename);
    std::string line;
    std::string currentVariable = ""; // We absolutely need this to track across lines!
    int webCounter = 0;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return allWebs;
    }

    while (std::getline(file, line)) {
        // Skip empty lines or comments
        if (line.empty() || line[0] == '#') continue;

        // Find the colon. If a line doesn't even have a colon, it's invalid.
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        // 1. Check if there is a variable name before the colon
        std::string possibleVarName = line.substr(0, colonPos);
        possibleVarName.erase(0, possibleVarName.find_first_not_of(" \t"));
        possibleVarName.erase(possibleVarName.find_last_not_of(" \t") + 1);
        
        // If it's not empty (like "sum:"), update our tracker. 
        // If it is empty (like "   :"), we just keep using the old currentVariable.
        if (!possibleVarName.empty()) {
            currentVariable = possibleVarName;
        }

        // 2. Extract the numbers and create the stringstream exactly once (Optimized!)
        std::string dataToParse = line.substr(colonPos + 1);
        std::stringstream ss(dataToParse);
        std::string token;
        LiveRange currentRange;

        // Parse the comma-separated line numbers
        while (std::getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);

            bool isStart = false;
            bool isEnd = false;

            if (token.back() == '+') {
                isStart = true;
                token.pop_back();
            } else if (token.back() == '-') {
                isEnd = true;
                token.pop_back();
            }

            int lineNum = std::stoi(token);
            currentRange.lines.insert(lineNum);
            if (isStart) currentRange.starts.insert(lineNum);
            if (isEnd) currentRange.ends.insert(lineNum);
        }



        // 3. Merge or create the web
        std::vector<int> overlappingIndices;
        
        // Find ALL existing webs that this new range overlaps with
        for (size_t i = 0; i < allWebs.size(); ++i) {
            if (allWebs[i].variableName == currentVariable && allWebs[i].overlaps(currentRange)) {
                overlappingIndices.push_back(i);
            }
        }

        if (overlappingIndices.empty()) {
            // It overlaps with nothing, so it becomes a brand new web
            Web newWeb;
            newWeb.id = "web" + std::to_string(webCounter++);
            newWeb.variableName = currentVariable;
            newWeb.merge(currentRange);
            allWebs.push_back(newWeb);
        } else {
            // It overlaps with at least one existing web.
            // Merge the new range into the FIRST overlapping web.
            int primaryIdx = overlappingIndices[0];
            allWebs[primaryIdx].merge(currentRange);

            // If it overlapped with MORE than one web, it means this new range
            // acts as a "bridge" connecting previously separate webs!
            // We must absorb the other webs into the primary one, and then delete them.
            // We loop backwards (from end to 1) so erasing doesn't mess up our remaining indices.
            for (int i = overlappingIndices.size() - 1; i >= 1; --i) {
                int secondaryIdx = overlappingIndices[i];
                
                // Absorb the secondary web's data into the primary web
                allWebs[primaryIdx].lines.insert(allWebs[secondaryIdx].lines.begin(), allWebs[secondaryIdx].lines.end());
                allWebs[primaryIdx].starts.insert(allWebs[secondaryIdx].starts.begin(), allWebs[secondaryIdx].starts.end());
                allWebs[primaryIdx].ends.insert(allWebs[secondaryIdx].ends.begin(), allWebs[secondaryIdx].ends.end());
                
                // Delete the secondary web since it is now safely fused into the primary
                allWebs.erase(allWebs.begin() + secondaryIdx);
            }
        }

    }

    file.close();
    return allWebs;
}