#ifndef WEB_H
#define WEB_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "LiveRange.h"

/**
 * @brief Represents a web: a collection of merged live ranges for the same variable.
 *
 * Two live ranges of the same variable belong to the same web if they share
 * at least one program line (or connect transitively through shared lines).
 * Webs become the nodes of the interference graph.
 */
class Web {
public:
    int id;                              ///< Web identifier (web0, web1, ...)
    std::string variable;                ///< Original variable name
    std::set<int> lines;                 ///< Union of all line numbers in this web
    std::map<int, ProgramPoint> pointMap; ///< Map from line number to annotated ProgramPoint

    int assignedRegister = -1;           ///< -1 = unassigned, -2 = memory (M)

    /**
     * @brief Formats the web's program points for output.
     * @return String like "1+,2,3,4,5,6-" with points sorted in ascending order.
     * @complexity O(P) where P = number of program points.
     */
    std::string formatPoints() const;

    /**
     * @brief Checks if this web interferes with another web.
     * Two webs interfere if their line sets have a non-empty intersection.
     * @param other The other web to check against.
     * @return True if the webs interfere.
     * @complexity O(min(|this.lines|, |other.lines|) * log(max(|this.lines|, |other.lines|)))
     */
    bool interferesWith(const Web& other) const;

    /**
     * @brief Gets the sorted list of program points.
     * @return Vector of ProgramPoints sorted by line number.
     * @complexity O(P) where P = number of points.
     */
    std::vector<ProgramPoint> getSortedPoints() const;
};

#endif // WEB_H
