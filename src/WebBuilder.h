#ifndef WEBBUILDER_H
#define WEBBUILDER_H

#include <vector>
#include "LiveRange.h"
#include "Web.h"

/**
 * @brief Builds webs by merging live ranges that share program points.
 *
 * Uses a Union-Find (Disjoint Set Union) approach to transitively merge
 * live ranges of the same variable that share at least one line number.
 * Also fuses ranges where one ends at a line and another begins at the
 * same line (e.g., "i=i+1" pattern).
 */
class WebBuilder {
public:
    /**
     * @brief Merges live ranges into webs.
     * @param ranges Vector of all parsed LiveRange entries.
     * @return Vector of Web objects, each assigned a unique sequential ID.
     * @complexity O(V * R^2 * alpha(R)) where V = number of variables,
     *             R = max live ranges per variable, alpha = inverse Ackermann.
     */
    static std::vector<Web> buildWebs(const std::vector<LiveRange>& ranges);

private:
    /// Union-Find: find with path compression
    static int find(std::vector<int>& parent, int x);
    /// Union-Find: union by rank
    static void unite(std::vector<int>& parent, std::vector<int>& rank, int x, int y);
};

#endif // WEBBUILDER_H
