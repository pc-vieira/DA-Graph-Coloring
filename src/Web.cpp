#include "Web.h"
#include <sstream>
#include <algorithm>

std::string Web::formatPoints() const {
    std::ostringstream oss;
    auto sortedPts = getSortedPoints();
    for (size_t i = 0; i < sortedPts.size(); i++) {
        if (i > 0) oss << ",";
        oss << sortedPts[i].line;
        if (sortedPts[i].isDef) oss << "+";
        if (sortedPts[i].isUse) oss << "-";
    }
    return oss.str();
}

bool Web::interferesWith(const Web& other) const {
    // Two webs interfere if they share a line where both are truly alive.
    // If the only shared lines are boundaries where one web ends (-)
    // and the other begins (+), they don't interfere — the use happens
    // before the def on the same instruction line.
    auto it1 = lines.begin();
    auto it2 = other.lines.begin();
    while (it1 != lines.end() && it2 != other.lines.end()) {
        if (*it1 == *it2) {
            int line = *it1;
            auto pt1 = pointMap.find(line);
            auto pt2 = other.pointMap.find(line);

            bool thisOnlyUse  = (pt1 != pointMap.end() && pt1->second.isUse && !pt1->second.isDef);
            bool otherOnlyDef = (pt2 != other.pointMap.end() && pt2->second.isDef && !pt2->second.isUse);
            bool thisOnlyDef  = (pt1 != pointMap.end() && pt1->second.isDef && !pt1->second.isUse);
            bool otherOnlyUse = (pt2 != other.pointMap.end() && pt2->second.isUse && !pt2->second.isDef);

            // Non-interfering boundary: one ends(-) and the other begins(+)
            bool boundaryOnly = (thisOnlyUse && otherOnlyDef) || (thisOnlyDef && otherOnlyUse);

            if (!boundaryOnly) {
                return true; // True overlap — they interfere
            }
            // Otherwise, this shared line is just a boundary — continue checking
            ++it1;
            ++it2;
        } else if (*it1 < *it2) {
            ++it1;
        } else {
            ++it2;
        }
    }
    return false;
}

std::vector<ProgramPoint> Web::getSortedPoints() const {
    std::vector<ProgramPoint> result;
    result.reserve(pointMap.size());
    for (const auto& [line, pt] : pointMap) {
        result.push_back(pt);
    }
    return result;
}
