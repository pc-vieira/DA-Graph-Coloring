#include "GraphColoring.h"
#include <stack>
#include <set>
#include <algorithm>
#include <iostream>
#include <climits>
#include <numeric>

// ============================================================================
// Internal helper: basic greedy coloring (Figure 9 from project description)
// Returns which nodes were spilled during simplification.
// ============================================================================
ColoringResult GraphColoring::basicInternal(InterferenceGraph& ig, int maxRegisters) {
    ig.resetState();
    Graph<int>& g = ig.getGraph();
    auto& webs = ig.getWebs();
    int numWebs = webs.size();

    ColoringResult result;
    result.webColors.assign(numWebs, -1);
    result.success = true;
    result.registersUsed = 0;

    std::stack<int> simplifyStack;
    std::vector<bool> spilled(numWebs, false);

    // Phase 1: Simplification
    int enabledCount = numWebs;
    while (enabledCount > 0) {
        bool found = false;

        // Try to find a node with degree < N
        for (auto v : g.getVertexSet()) {
            if (!v->isEnabled()) continue;
            if (v->enabledDegree() < maxRegisters) {
                v->setEnabled(false);
                simplifyStack.push(v->getInfo());
                enabledCount--;
                found = true;
            }
        }

        if (!found) {
            // All remaining nodes have degree >= N
            // Select one to spill: highest degree, ties by smallest web size
            Vertex<int>* toSpill = nullptr;
            int maxDeg = -1;
            int minSize = INT_MAX;

            for (auto v : g.getVertexSet()) {
                if (!v->isEnabled()) continue;
                int deg = v->enabledDegree();
                int webSize = webs[v->getInfo()].lines.size();
                if (deg > maxDeg || (deg == maxDeg && webSize < minSize)) {
                    maxDeg = deg;
                    minSize = webSize;
                    toSpill = v;
                }
            }

            if (toSpill) {
                toSpill->setEnabled(false);
                spilled[toSpill->getInfo()] = true;
                enabledCount--;
                result.success = false;
            }
        }
    }

    // Phase 2: Coloring
    while (!simplifyStack.empty()) {
        int webId = simplifyStack.top();
        simplifyStack.pop();

        Vertex<int>* v = g.findVertex(webId);
        v->setEnabled(true);

        // Find colors used by enabled neighbors
        std::set<int> usedColors;
        for (auto edge : v->getAdj()) {
            Vertex<int>* neighbor = edge->getDest();
            if (neighbor->isEnabled() && neighbor->getColor() >= 0) {
                usedColors.insert(neighbor->getColor());
            }
        }

        // Assign lowest available color
        int assignedColor = 0;
        while (usedColors.count(assignedColor)) {
            assignedColor++;
        }
        v->setColor(assignedColor);
        result.webColors[webId] = assignedColor;
    }

    // Record spilled webs
    for (int i = 0; i < numWebs; i++) {
        if (spilled[i]) {
            result.spilledWebIds.push_back(i);
            result.webColors[i] = -1;
        }
    }

    // Count distinct registers used
    std::set<int> usedRegs;
    for (int c : result.webColors) {
        if (c >= 0) usedRegs.insert(c);
    }
    result.registersUsed = usedRegs.size();

    return result;
}

// ============================================================================
// T2.1 — Basic Register Allocation
// ============================================================================
ColoringResult GraphColoring::basic(InterferenceGraph& ig, int maxRegisters) {
    ColoringResult result = basicInternal(ig, maxRegisters);

    if (!result.success) {
        // In basic mode, if any node was spilled, allocation is infeasible
        // Assign ALL webs to memory
        std::cerr << "Warning: Register allocation with " << maxRegisters
                  << " registers was not possible." << std::endl;
        int numWebs = ig.getWebs().size();
        result.registersUsed = 0;
        result.webColors.assign(numWebs, -1);
        result.spilledWebIds.clear();
        for (int i = 0; i < numWebs; i++) {
            result.spilledWebIds.push_back(i);
        }
    }

    return result;
}

// ============================================================================
// T2.2 — Register Allocation with Web Spilling
// ============================================================================
ColoringResult GraphColoring::spilling(InterferenceGraph& ig, int maxRegisters, int maxSpills) {
    // First try basic coloring
    ColoringResult result = basicInternal(ig, maxRegisters);
    if (result.success) return result;

    // Try spilling 1, 2, ..., up to maxSpills webs
    auto& webs = ig.getWebs();
    int numWebs = webs.size();

    // Rank webs by degree (descending) for spill candidates
    // Compute degrees from a fresh state
    ig.resetState();
    std::vector<std::pair<int, int>> webDegrees; // (degree, webId)
    for (auto v : ig.getGraph().getVertexSet()) {
        webDegrees.push_back({v->enabledDegree(), v->getInfo()});
    }
    std::sort(webDegrees.rbegin(), webDegrees.rend());

    for (int numSpills = 1; numSpills <= maxSpills && numSpills <= numWebs; numSpills++) {
        ig.resetState();

        // Spill the top numSpills highest-degree webs
        std::vector<int> toSpill;
        for (int i = 0; i < numSpills && i < (int)webDegrees.size(); i++) {
            toSpill.push_back(webDegrees[i].second);
        }

        // Disable spilled webs
        for (int id : toSpill) {
            Vertex<int>* v = ig.getGraph().findVertex(id);
            if (v) v->setEnabled(false);
        }

        // Try coloring the remaining graph
        Graph<int>& g = ig.getGraph();
        std::stack<int> simplifyStack;
        std::vector<bool> inStack(numWebs, false);

        // Count enabled vertices
        int enabledCount = 0;
        for (auto v : g.getVertexSet()) {
            if (v->isEnabled()) enabledCount++;
        }

        bool spilledMore = false;
        int tempEnabledCount = enabledCount;

        // Simplification phase (on non-spilled vertices only)
        while (tempEnabledCount > 0) {
            bool found = false;
            for (auto v : g.getVertexSet()) {
                if (!v->isEnabled() || inStack[v->getInfo()]) continue;
                if (v->enabledDegree() < maxRegisters) {
                    v->setEnabled(false);
                    simplifyStack.push(v->getInfo());
                    inStack[v->getInfo()] = true;
                    tempEnabledCount--;
                    found = true;
                }
            }
            if (!found) {
                // Still stuck even after spilling — this attempt fails
                spilledMore = true;
                break;
            }
        }

        if (spilledMore) continue;

        // Coloring phase
        result.webColors.assign(numWebs, -1);
        result.spilledWebIds = toSpill;
        bool coloringOk = true;

        while (!simplifyStack.empty()) {
            int webId = simplifyStack.top();
            simplifyStack.pop();

            Vertex<int>* v = g.findVertex(webId);
            v->setEnabled(true);

            std::set<int> usedColors;
            for (auto edge : v->getAdj()) {
                Vertex<int>* neighbor = edge->getDest();
                if (neighbor->isEnabled() && neighbor->getColor() >= 0) {
                    usedColors.insert(neighbor->getColor());
                }
            }

            int assignedColor = 0;
            while (usedColors.count(assignedColor)) {
                assignedColor++;
            }

            if (assignedColor >= maxRegisters) {
                coloringOk = false;
                break;
            }

            v->setColor(assignedColor);
            result.webColors[webId] = assignedColor;
        }

        if (coloringOk) {
            result.success = true;
            std::set<int> usedRegs;
            for (int c : result.webColors) {
                if (c >= 0) usedRegs.insert(c);
            }
            result.registersUsed = usedRegs.size();
            return result;
        }
    }

    // All attempts failed — assign everything to memory
    std::cerr << "Warning: Register allocation with " << maxRegisters
              << " registers was not possible even with " << maxSpills << " spills." << std::endl;
    result.registersUsed = 0;
    result.webColors.assign(numWebs, -1);
    result.spilledWebIds.clear();
    for (int i = 0; i < numWebs; i++) {
        result.spilledWebIds.push_back(i);
    }
    result.success = false;
    return result;
}

// ============================================================================
// T2.3 — Register Allocation with Web Splitting
// ============================================================================
ColoringResult GraphColoring::splitting(InterferenceGraph& ig, int maxRegisters, int maxSplits) {
    // First try basic coloring
    ColoringResult result = basicInternal(ig, maxRegisters);
    if (result.success) return result;

    auto& webs = ig.getWebs();

    for (int numSplits = 1; numSplits <= maxSplits; numSplits++) {
        // Find the best web to split: highest degree in original graph
        ig.resetState();
        int bestWebIdx = -1;
        int maxDeg = -1;

        for (auto v : ig.getGraph().getVertexSet()) {
            int deg = v->enabledDegree();
            int webId = v->getInfo();
            // Only split webs that have enough points to split
            if (deg > maxDeg && webs[webId].lines.size() >= 2) {
                maxDeg = deg;
                bestWebIdx = webId;
            }
        }

        if (bestWebIdx < 0) break; // Nothing to split

        // Split the web at its largest gap
        Web& toSplit = webs[bestWebIdx];
        std::vector<int> sortedLines(toSplit.lines.begin(), toSplit.lines.end());

        int bestGap = -1;
        int bestGapIdx = -1;
        for (int i = 0; i + 1 < (int)sortedLines.size(); i++) {
            int gap = sortedLines[i + 1] - sortedLines[i];
            if (gap > bestGap) {
                bestGap = gap;
                bestGapIdx = i;
            }
        }

        if (bestGapIdx < 0 || bestGap <= 0) break; // Can't split

        // Create two new webs from the split
        Web webA, webB;
        webA.id = toSplit.id;  // Keep original ID
        webA.variable = toSplit.variable;
        webB.id = webs.size(); // New ID
        webB.variable = toSplit.variable;

        int splitPoint = sortedLines[bestGapIdx];
        for (const auto& [line, pt] : toSplit.pointMap) {
            if (line <= splitPoint) {
                webA.lines.insert(line);
                webA.pointMap[line] = pt;
            } else {
                webB.lines.insert(line);
                webB.pointMap[line] = pt;
            }
        }

        // Replace the original web and add the new one
        webs[bestWebIdx] = webA;
        webs.push_back(webB);

        // Rebuild the interference graph
        ig.build(webs);

        // Try basic coloring on the new graph
        result = basicInternal(ig, maxRegisters);
        if (result.success) return result;
    }

    // All attempts failed
    std::cerr << "Warning: Register allocation with " << maxRegisters
              << " registers was not possible even with " << maxSplits << " splits." << std::endl;
    int numWebs = webs.size();
    result.registersUsed = 0;
    result.webColors.assign(numWebs, -1);
    result.spilledWebIds.clear();
    for (int i = 0; i < numWebs; i++) {
        result.spilledWebIds.push_back(i);
    }
    result.success = false;
    return result;
}

// ============================================================================
// T2.4 — Free Algorithm: DSatur (Degree of Saturation)
// ============================================================================
ColoringResult GraphColoring::free(InterferenceGraph& ig, int maxRegisters) {
    ig.resetState();
    Graph<int>& g = ig.getGraph();
    auto& webs = ig.getWebs();
    int numWebs = webs.size();

    ColoringResult result;
    result.webColors.assign(numWebs, -1);
    result.success = true;

    std::vector<int> saturation(numWebs, 0);
    std::vector<bool> colored(numWebs, false);

    // Compute initial degrees
    std::vector<int> degree(numWebs, 0);
    for (auto v : g.getVertexSet()) {
        degree[v->getInfo()] = v->getAdj().size();
    }

    for (int iter = 0; iter < numWebs; iter++) {
        // Select uncolored vertex with highest saturation
        // Ties: highest degree, then lowest id
        int bestId = -1;
        int bestSat = -1;
        int bestDeg = -1;

        for (int i = 0; i < numWebs; i++) {
            if (colored[i]) continue;
            if (saturation[i] > bestSat ||
                (saturation[i] == bestSat && degree[i] > bestDeg) ||
                (saturation[i] == bestSat && degree[i] == bestDeg && (bestId == -1 || i < bestId))) {
                bestSat = saturation[i];
                bestDeg = degree[i];
                bestId = i;
            }
        }

        if (bestId < 0) break;

        // Find colors used by colored neighbors
        Vertex<int>* v = g.findVertex(bestId);
        std::set<int> usedColors;
        for (auto edge : v->getAdj()) {
            int neighborId = edge->getDest()->getInfo();
            if (colored[neighborId]) {
                usedColors.insert(result.webColors[neighborId]);
            }
        }

        // Assign lowest available color
        int assignedColor = 0;
        while (usedColors.count(assignedColor)) {
            assignedColor++;
        }

        // If we exceed maxRegisters, spill this node instead
        if (assignedColor >= maxRegisters) {
            result.spilledWebIds.push_back(bestId);
            colored[bestId] = true;
            // Don't update saturation of neighbors (spilled node has no color)
            continue;
        }

        result.webColors[bestId] = assignedColor;
        colored[bestId] = true;
        v->setColor(assignedColor);

        // Update saturation of uncolored neighbors
        for (auto edge : v->getAdj()) {
            int neighborId = edge->getDest()->getInfo();
            if (!colored[neighborId]) {
                // Recompute saturation (number of distinct colors among colored neighbors)
                Vertex<int>* nv = g.findVertex(neighborId);
                std::set<int> neighborColors;
                for (auto ne : nv->getAdj()) {
                    int nnId = ne->getDest()->getInfo();
                    if (colored[nnId] && result.webColors[nnId] >= 0) {
                        neighborColors.insert(result.webColors[nnId]);
                    }
                }
                saturation[neighborId] = neighborColors.size();
            }
        }
    }

    // Check if any nodes were spilled
    if (!result.spilledWebIds.empty()) {
        result.success = false;
        std::cerr << "Warning: DSatur spilled " << result.spilledWebIds.size()
                  << " webs to memory." << std::endl;
    }

    // Count distinct registers used
    std::set<int> usedRegs;
    for (int c : result.webColors) {
        if (c >= 0) usedRegs.insert(c);
    }
    result.registersUsed = usedRegs.size();

    return result;
}
