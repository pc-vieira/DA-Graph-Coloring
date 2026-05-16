#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <vector>
#include <string>
#include <algorithm>
#include "parser.h" 
#include "graph.h"  

/**
 * @brief Constructs the Interference Graph used for Register Allocation.
 */
class GraphBuilder {
public:
    /**
     * @brief Determines if two webs interfere and cannot share a register.
     * * Two webs interfere if they are simultaneously active on the same line,
     * unless every overlapping line represents a "clean handoff".
     *
     * @param a The first web.
     * @param b The second web.
     * @return true if the webs interfere, false otherwise.
     * @complexity $O(|L_a| + |L_b|)$ where $|L|$ is the number of active lines in the respective webs.
     */
    static bool interferes(const Web& a, const Web& b) {
        std::vector<int> commonLines;
        
        std::set_intersection(a.lines.begin(), a.lines.end(),
                              b.lines.begin(), b.lines.end(),
                              std::back_inserter(commonLines));
        
        if (commonLines.empty()) {
            return false;
        }

        for (int line : commonLines) {
            bool a_ends = (a.ends.find(line) != a.ends.end());
            bool b_starts = (b.starts.find(line) != b.starts.end());
            
            bool b_ends = (b.ends.find(line) != b.ends.end());
            bool a_starts = (a.starts.find(line) != a.starts.end());

            bool is_clean_handoff = (a_ends && b_starts) || (b_ends && a_starts);
            
            if (!is_clean_handoff) {
                return true; 
            }
        }
        
        return false;
    }

    /**
     * @brief Builds a bidirectional interference graph from the parsed webs.
     * @param webs The vector of webs to map.
     * @return A populated Graph where edges represent interferences.
     * @complexity $O(|W|^2 \times |L|)$ where $|W|$ is the number of webs and $|L|$ is the average number of lines per web.
     */
    static Graph<std::string> buildInterferenceGraph(const std::vector<Web>& webs) {
        Graph<std::string> interferenceGraph;

        for (const Web& w : webs) {
            interferenceGraph.addVertex(w.id);
        }

        for (size_t i = 0; i < webs.size(); ++i) {
            for (size_t j = i + 1; j < webs.size(); ++j) {
                if (interferes(webs[i], webs[j])) {
                    interferenceGraph.addEdge(webs[i].id, webs[j].id, 0.0);
                    interferenceGraph.addEdge(webs[j].id, webs[i].id, 0.0);
                }
            }
        }

        return interferenceGraph;
    }
};

#endif // GRAPH_BUILDER_H