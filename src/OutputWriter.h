#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include <string>
#include <vector>
#include "Web.h"
#include "GraphColoring.h"

/**
 * @brief Writes the register allocation results to an output file.
 */
class OutputWriter {
public:
    /**
     * @brief Writes the allocation result to a file in the required format.
     *
     * Output format:
     * - List of webs with their program points
     * - Number of registers used
     * - Register-to-web assignments (or M for memory-spilled webs)
     *
     * @param filename Path to the output file.
     * @param webs Vector of all webs.
     * @param result The coloring/allocation result.
     * @complexity O(W * P) where W = number of webs, P = avg points per web.
     */
    static void write(const std::string& filename,
                      const std::vector<Web>& webs,
                      const ColoringResult& result);

    /**
     * @brief Prints the allocation result to stdout.
     */
    static void print(const std::vector<Web>& webs,
                      const ColoringResult& result);

private:
    /**
     * @brief Formats the result as a string (used by both write and print).
     */
    static std::string format(const std::vector<Web>& webs,
                              const ColoringResult& result);
};

#endif // OUTPUTWRITER_H
