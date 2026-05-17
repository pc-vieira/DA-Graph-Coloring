#include "parser.h"

std::vector<Web> Parser::parseWebs(const std::string& filename) {
    std::vector<Web> allWebs;
    std::ifstream file(filename);
    std::string line;
    std::string currentVariable = "";
    int webCounter = 0;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return allWebs;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string possibleVarName = line.substr(0, colonPos);
        possibleVarName.erase(0, possibleVarName.find_first_not_of(" \t"));
        possibleVarName.erase(possibleVarName.find_last_not_of(" \t") + 1);
        
        if (!possibleVarName.empty()) {
            currentVariable = possibleVarName;
        }

        std::string dataToParse = line.substr(colonPos + 1);
        std::stringstream ss(dataToParse);
        std::string token;
        LiveRange currentRange;

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

        std::vector<int> overlappingIndices;

        for (size_t i = 0; i < allWebs.size(); ++i) {
            if (allWebs[i].variableName == currentVariable && allWebs[i].overlaps(currentRange)) {
                overlappingIndices.push_back(i);
            }
        }

        if (overlappingIndices.empty()) {
            Web newWeb;
            newWeb.id = "web" + std::to_string(webCounter++);
            newWeb.variableName = currentVariable;
            newWeb.merge(currentRange);
            allWebs.push_back(newWeb);
        } else {
            int primaryIdx = overlappingIndices[0];
            allWebs[primaryIdx].merge(currentRange);

            for (int i = overlappingIndices.size() - 1; i >= 1; --i) {
                int secondaryIdx = overlappingIndices[i];
                
                allWebs[primaryIdx].lines.insert(allWebs[secondaryIdx].lines.begin(), allWebs[secondaryIdx].lines.end());
                allWebs[primaryIdx].starts.insert(allWebs[secondaryIdx].starts.begin(), allWebs[secondaryIdx].starts.end());
                allWebs[primaryIdx].ends.insert(allWebs[secondaryIdx].ends.begin(), allWebs[secondaryIdx].ends.end());
                
                allWebs.erase(allWebs.begin() + secondaryIdx);
            }
        }

    }

    file.close();
    return allWebs;
}