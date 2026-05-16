#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>

/**
 * @brief Represents a single contiguous range of lines for a variable.
 */
struct LiveRange {
    std::set<int> lines;
    std::set<int> starts; 
    std::set<int> ends;   
};

/**
 * @brief Represents a fully merged web of overlapping ranges for a single variable.
 */
struct Web {
    std::string id;           
    std::string variableName; 
    std::set<int> lines;
    std::set<int> starts;
    std::set<int> ends;
    
    /**
     * @brief Checks if this web overlaps with a given live range.
     * @complexity $O(|L_w| + |L_r|)$ set intersection.
     */
    bool overlaps(const LiveRange& other) const {
        std::vector<int> intersection;
        std::set_intersection(lines.begin(), lines.end(),
                              other.lines.begin(), other.lines.end(),
                              std::back_inserter(intersection));
        return !intersection.empty();
    }

    /**
     * @brief Merges a live range into this web.
     * @complexity $O(|L_r| \log(|L_w|))$ to insert elements.
     */
    void merge(const LiveRange& other) {
        lines.insert(other.lines.begin(), other.lines.end());
        starts.insert(other.starts.begin(), other.starts.end());
        ends.insert(other.ends.begin(), other.ends.end());
    }

    /**
     * @brief Splits this web into two chronological halves.
     * @return A pair containing the two new derived Webs.
     * @complexity $O(|L_w|)$ as it iterates through all lines to divide them.
     */
    std::pair<Web, Web> split() const {
        Web w1, w2;
        w1.id = this->id + "_1";
        w1.variableName = this->variableName;
        w2.id = this->id + "_2";
        w2.variableName = this->variableName;

        int mid = lines.size() / 2;
        int count = 0;
        
        for (int line : lines) {
            if (count < mid) {
                w1.lines.insert(line);
                if (starts.find(line) != starts.end()) w1.starts.insert(line);
                if (ends.find(line) != ends.end()) w1.ends.insert(line);
            } else {
                w2.lines.insert(line);
                if (starts.find(line) != starts.end()) w2.starts.insert(line);
                if (ends.find(line) != ends.end()) w2.ends.insert(line);
            }
            count++;
        }
        
        if (!w1.lines.empty() && !w2.lines.empty()) {
            w1.ends.insert(*w1.lines.rbegin());
            w2.starts.insert(*w2.lines.begin());
        }
        
        return {w1, w2};
    }
};

/**
 * @brief Parses input files into usable data structures.
 */
class Parser {
public:
    /**
     * @brief Reads a ranges.txt file and constructs the initial Webs.
     * @complexity $O(N \times |W| \times |L|)$ where $N$ is lines in the file, $|W|$ is current webs, and $|L|$ is lines per web.
     */
    static std::vector<Web> parseWebs(const std::string& filename);
};

#endif // PARSER_H