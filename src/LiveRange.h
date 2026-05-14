#ifndef LIVERANGE_H
#define LIVERANGE_H

#include <string>
#include <vector>
#include <set>

/**
 * @brief Represents a single program point within a live range.
 *
 * A program point corresponds to a line number in the source code,
 * optionally annotated with '+' (definition/write) or '-' (last use/read).
 */
struct ProgramPoint {
    int line;              ///< Program line number
    bool isDef  = false;   ///< '+' marker: variable is defined (written) at this line
    bool isUse  = false;   ///< '-' marker: variable is last used (read) at this line
};

/**
 * @brief Represents a single live range for a variable as parsed from the input file.
 *
 * Each line in the input file (e.g., "i: 1+,2,3,4,5,6-") produces one LiveRange.
 * A variable may have multiple LiveRange entries that are later merged into Webs.
 */
struct LiveRange {
    std::string variable;              ///< Variable name (e.g., "i", "sum", "t0")
    std::vector<ProgramPoint> points;  ///< Ordered list of program points in this range
    std::set<int> lineSet;             ///< Set of all line numbers for fast overlap checks
};

#endif // LIVERANGE_H
