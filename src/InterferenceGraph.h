#ifndef INTERFERENCEGRAPH_H
#define INTERFERENCEGRAPH_H

#include "Graph.h"
#include "Web.h"
#include <vector>

/**
 * @brief Wraps a Graph<int> to represent the interference graph for register allocation.
 *
 * Each vertex in the graph corresponds to a Web (identified by its integer ID).
 * An edge between two vertices means the corresponding webs interfere
 * (i.e., they are simultaneously alive on at least one program line).
 */
class InterferenceGraph {
public:
    /**
     * @brief Builds the interference graph from a list of webs.
     *
     * Creates a vertex for each web and adds a bidirectional edge between
     * every pair of webs that share at least one program line.
     *
     * @param webs Vector of Web objects to build the graph from.
     * @complexity O(W^2 * L) where W = number of webs, L = avg line-set size.
     */
    void build(const std::vector<Web>& webs);

    /**
     * @brief Returns a reference to the underlying Graph<int>.
     */
    Graph<int>& getGraph();

    /**
     * @brief Returns a const reference to the underlying Graph<int>.
     */
    const Graph<int>& getGraph() const;

    /**
     * @brief Returns the list of webs.
     */
    std::vector<Web>& getWebs();

    /**
     * @brief Returns a const reference to the list of webs.
     */
    const std::vector<Web>& getWebs() const;

    /**
     * @brief Resets all vertices to enabled and uncolored state.
     * @complexity O(V)
     */
    void resetState();

    /**
     * @brief Displays the interference graph to stdout.
     */
    void display() const;

private:
    Graph<int> graph;
    std::vector<Web> webs;
};

#endif // INTERFERENCEGRAPH_H
