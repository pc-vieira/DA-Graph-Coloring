#include "InterferenceGraph.h"
#include <iostream>

void InterferenceGraph::build(const std::vector<Web>& inputWebs) {
    webs = inputWebs;

    // Add a vertex for each web
    for (const auto& web : webs) {
        graph.addVertex(web.id);
    }

    // Add edges between interfering webs
    for (size_t i = 0; i < webs.size(); i++) {
        for (size_t j = i + 1; j < webs.size(); j++) {
            if (webs[i].interferesWith(webs[j])) {
                graph.addBidirectionalEdge(webs[i].id, webs[j].id, 1.0);
            }
        }
    }
}

Graph<int>& InterferenceGraph::getGraph() {
    return graph;
}

const Graph<int>& InterferenceGraph::getGraph() const {
    return graph;
}

std::vector<Web>& InterferenceGraph::getWebs() {
    return webs;
}

const std::vector<Web>& InterferenceGraph::getWebs() const {
    return webs;
}

void InterferenceGraph::resetState() {
    for (auto v : graph.getVertexSet()) {
        v->setEnabled(true);
        v->setColor(-1);
    }
}

void InterferenceGraph::display() const {
    std::cout << "\n=== Interference Graph ===" << std::endl;
    std::cout << "Vertices (webs): " << graph.getNumVertex() << std::endl;

    for (auto v : graph.getVertexSet()) {
        int id = v->getInfo();
        std::cout << "  web" << id << " (" << webs[id].variable << "): ";
        std::cout << webs[id].formatPoints();

        auto adj = v->getAdj();
        if (!adj.empty()) {
            std::cout << "  -> interferes with: ";
            for (size_t i = 0; i < adj.size(); i++) {
                if (i > 0) std::cout << ", ";
                std::cout << "web" << adj[i]->getDest()->getInfo();
            }
        }
        std::cout << std::endl;
    }
    std::cout << "=========================\n" << std::endl;
}
