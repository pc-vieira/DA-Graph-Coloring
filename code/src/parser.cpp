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

        std::stringstream ss(line);
        std::string token;
        LiveRange currentRange;

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string possibleVarName = line.substr(0, colonPos);
            possibleVarName.erase(0, possibleVarName.find_first_not_of(" \t"));
            possibleVarName.erase(possibleVarName.find_last_not_of(" \t") + 1);
            
            if (!possibleVarName.empty()) {
                currentVariable = possibleVarName;
            }
            ss.str(line.substr(colonPos + 1));
        }

        while (std::getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            if (token.empty()) continue;

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

        bool merged = false;
        for (Web& existingWeb : allWebs) {
            if (existingWeb.variableName == currentVariable && existingWeb.overlaps(currentRange)) {
                existingWeb.merge(currentRange);
                merged = true;
                break;
            }
        }

        if (!merged) {
            Web newWeb;
            newWeb.id = "web" + std::to_string(webCounter++);
            newWeb.variableName = currentVariable;
            newWeb.merge(currentRange);
            allWebs.push_back(newWeb);
        }
    }

    file.close();
    return allWebs;
}