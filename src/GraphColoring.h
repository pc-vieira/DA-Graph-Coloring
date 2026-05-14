#ifndef GRAPHCOLORING_H
#define GRAPHCOLORING_H

#include "InterferenceGraph.h"
#include <vector>
#include <string>

/**
 * @brief Result of a graph coloring / register allocation attempt.
 */
struct ColoringResult {
    bool success;                        ///< True if all webs were colored (no forced spills in basic)
    int registersUsed;                   ///< Number of distinct colors/registers used (0 if infeasible)
    std::vector<int> webColors;          ///< webColors[i] = color of web i (-1 = spilled to memory)
    std::vector<int> spilledWebIds;      ///< IDs of webs assigned to memory
};

/**
 * @brief Implements graph coloring algorithms for register allocation.
 */
class GraphColoring {
public:
    /**
     * @brief T2.1 — Basic greedy graph coloring (Figure 9 from project description).
     *
     * Uses the simplification + coloring two-phase approach.
     * If coloring is infeasible with the given number of registers,
     * all webs are assigned to memory.
     *
     * @param ig The interference graph (state will be modified).
     * @param maxRegisters Maximum number of colors/registers.
     * @return ColoringResult with the allocation.
     * @complexity O(V^2 + V*E) where V = number of webs, E = number of edges.
     */
    static ColoringResult basic(InterferenceGraph& ig, int maxRegisters);

    /**
     * @brief T2.2 — Register allocation with web spilling.
     *
     * Tries basic coloring first. If it fails, greedily selects up to
     * maxSpills webs to remove from the graph and retries.
     * Spill selection heuristic: highest degree, ties broken by fewest points.
     *
     * @param ig The interference graph.
     * @param maxRegisters Maximum number of colors/registers.
     * @param maxSpills Maximum number of webs to spill (K parameter).
     * @return ColoringResult with the allocation.
     * @complexity O(K * (V^2 + V*E))
     */
    static ColoringResult spilling(InterferenceGraph& ig, int maxRegisters, int maxSpills);

    /**
     * @brief T2.3 — Register allocation with web splitting.
     *
     * Tries basic coloring first. If it fails, selects up to maxSplits
     * webs to split at their largest gap, rebuilds the graph, and retries.
     *
     * @param ig The interference graph (webs may be modified).
     * @param maxRegisters Maximum number of colors/registers.
     * @param maxSplits Maximum number of webs to split (K parameter).
     * @return ColoringResult with the allocation.
     * @complexity O(K * (W^2 * L + V^2 + V*E))
     */
    static ColoringResult splitting(InterferenceGraph& ig, int maxRegisters, int maxSplits);

    /**
     * @brief T2.4 — Free algorithm using DSatur (Degree of Saturation) heuristic.
     *
     * DSatur dynamically selects the vertex with highest saturation
     * (number of distinct colors among neighbors) to color next.
     * Proven optimal for bipartite graphs, near-optimal in general.
     * Falls back to spilling if more colors needed than available.
     *
     * @param ig The interference graph.
     * @param maxRegisters Maximum number of colors/registers.
     * @return ColoringResult with the allocation.
     * @complexity O(V^2 + V*E)
     */
    static ColoringResult free(InterferenceGraph& ig, int maxRegisters);

private:
    /**
     * @brief Internal basic coloring that reports which nodes were spilled
     *        during simplification (rather than assigning all to memory).
     */
    static ColoringResult basicInternal(InterferenceGraph& ig, int maxRegisters);
};

#endif // GRAPHCOLORING_H
