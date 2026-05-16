#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include "graph.h" 

/**
 * @brief Configuration struct for register allocation mode.
 */
struct AllocSettings {
    int numRegisters = 0;
    std::string algorithm = "basic";
    int algoParam = 0;
};

/**
 * @brief Engine for allocating registers using Graph Colouring heuristics.
 */
class RegisterAllocator {
public:
    /**
     * @brief Allocates registers to webs, handling spilling and splitting dynamically.
     * @param graph The interference graph.
     * @param settings The configuration (K limit, algorithm mode).
     * @param webs The list of webs (passed by reference to allow splitting).
     * @param currentSplits Tracks how many splits have occurred across rebuilding iterations.
     * @return A map linking Web IDs to a Register index (or -1 for memory).
     * @complexity $O(|V|^2 + |E|)$ per iteration, where $|V|$ is the number of webs and $|E|$ is the number of interferences.
     */
    static std::map<std::string, int> allocateRegisters(Graph<std::string>& graph, const AllocSettings& settings, std::vector<Web>& webs, int& currentSplits) {
        std::map<std::string, int> degrees;
        std::map<std::string, bool> active;
        std::vector<std::string> stack;
        std::map<std::string, int> colors; 

        for (auto v : graph.getVertexSet()) {
            std::string id = v->getInfo();
            degrees[id] = v->getAdj().size();
            active[id] = true;
            colors[id] = -1; 
        }

        int activeCount = graph.getNumVertex();
        int currentSpills = 0;
        bool failed = false;

        // ==========================================
        // PHASE 1: SIMPLIFY, SPILL & SPLIT
        // ==========================================
        while (activeCount > 0) {
            bool removedAny = false;
            
            for (auto v : graph.getVertexSet()) {
                std::string id = v->getInfo();
                
                if (active[id] && degrees[id] < settings.numRegisters) {
                    active[id] = false;     
                    stack.push_back(id);    
                    activeCount--;
                    removedAny = true;

                    for (auto edge : v->getAdj()) {
                        std::string neighbour = edge->getDest()->getInfo();
                        if (active[neighbour]) degrees[neighbour]--;
                    }
                }
            }

            if (!removedAny && activeCount > 0) {
                
                std::string problematicNode = "";
                int maxMetric = -1; 
                
                // --- HEURISTIC SELECTION ---
                if (settings.algorithm == "free") {
                    for (auto v : graph.getVertexSet()) {
                        std::string id = v->getInfo();
                        if (active[id]) {
                            int lineCount = 0;
                            for (const Web& w : webs) {
                                if (w.id == id) {
                                    lineCount = w.lines.size();
                                    break;
                                }
                            }
                            if (lineCount > maxMetric) {
                                maxMetric = lineCount;
                                problematicNode = id;
                            }
                        }
                    }
                } else {
                    for (auto v : graph.getVertexSet()) {
                        std::string id = v->getInfo();
                        if (active[id] && degrees[id] > maxMetric) {
                            maxMetric = degrees[id];
                            problematicNode = id;
                        }
                    }
                }

                // --- EXECUTE INTERVENTION ---
                if (settings.algorithm == "splitting" && currentSplits < settings.algoParam) {
                    std::cout << "  -> Splitting " << problematicNode << " (Degree: " << maxMetric << ") to reduce interference.\n";
                    
                    std::vector<Web> newWebs;
                    for (const Web& w : webs) {
                        if (w.id == problematicNode) {
                            std::pair<Web, Web> splitWebs = w.split();
                            newWebs.push_back(splitWebs.first);
                            newWebs.push_back(splitWebs.second);
                        } else {
                            newWebs.push_back(w);
                        }
                    }
                    webs = newWebs; 
                    currentSplits++;
                    
                    return {{"__RESTART__", 1}}; 
                }
                else if ((settings.algorithm == "spilling" && currentSpills < settings.algoParam) || 
                          settings.algorithm == "free") {
                    
                    active[problematicNode] = false; 
                    colors[problematicNode] = -1; 
                    activeCount--;
                    currentSpills++;
                    
                    if (settings.algorithm == "free") {
                        std::cout << "  -> [Free Mode] Spilling " << problematicNode << " (Length: " << maxMetric << " lines) to memory.\n";
                    } else {
                        std::cout << "  -> Spilling " << problematicNode << " (Degree: " << maxMetric << ") to memory.\n";
                    }

                    Vertex<std::string>* spillVertex = graph.findVertex(problematicNode);
                    for (auto edge : spillVertex->getAdj()) {
                        std::string neighbour = edge->getDest()->getInfo();
                        if (active[neighbour]) degrees[neighbour]--;
                    }
                } 
                else {
                    failed = true;
                    break;
                }
            }
        } 

        if (failed) {
            std::cerr << "Warning: Assignment to the provided number of registers was not possible ";
            if (settings.algorithm == "spilling") std::cerr << "(even with allowed spills).\n";
            else if (settings.algorithm == "splitting") std::cerr << "(even with allowed splits).\n";
            else if (settings.algorithm == "free") std::cerr << "(despite unlimited spilling).\n";
            else std::cerr << "(using the basic algorithm).\n";
            
            return colors; 
        }

        // ==========================================
        // PHASE 2: COLOURING
        // ==========================================
        while (!stack.empty()) {
            std::string id = stack.back();
            stack.pop_back();

            std::set<int> usedColours;
            Vertex<std::string>* v = graph.findVertex(id);
            
            for (auto edge : v->getAdj()) {
                std::string neighbour = edge->getDest()->getInfo();
                if (colors[neighbour] != -1) {
                    usedColours.insert(colors[neighbour]);
                }
            }

            int assignedColour = -1;
            for (int c = 0; c < settings.numRegisters; ++c) {
                if (usedColours.find(c) == usedColours.end()) {
                    assignedColour = c;
                    break;
                }
            }

            colors[id] = assignedColour;
        }

        return colors;
    }
};

#endif // REGISTER_ALLOCATOR_H