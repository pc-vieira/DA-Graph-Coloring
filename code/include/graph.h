#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

template <class T> class Edge;
template <class T> class Graph;

/**
 * @brief Represents a vertex (node) in the graph.
 * @tparam T The data type of the vertex information.
 */
template <class T>
class Vertex {
    T info;
    std::vector<Edge<T>*> adj;
public:
    explicit Vertex(T in) : info(in) {}
    
    T getInfo() const { return info; }
    std::vector<Edge<T>*> getAdj() const { return adj; }
    
    /**
     * @brief Adds an outgoing edge from this vertex to a destination vertex.
     * @complexity $O(1)$
     */
    void addEdge(Vertex<T>* dest, double w = 0.0) {
        adj.push_back(new Edge<T>(dest, w));
    }
};

/**
 * @brief Represents a directed edge between two vertices.
 * @tparam T The data type of the vertex information.
 */
template <class T>
class Edge {
    Vertex<T>* dest;
    double weight; // Kept purely for compatibility with standard graph signatures
public:
    Edge(Vertex<T>* d, double w = 0.0) : dest(d), weight(w) {}
    
    Vertex<T>* getDest() const { return dest; }
};

/**
 * @brief A generic Graph data structure using adjacency lists.
 * @tparam T The data type of the vertex information (e.g., std::string for Web IDs).
 */
template <class T>
class Graph {
    std::vector<Vertex<T>*> vertexSet;
public:
    /**
     * @brief Destructor to safely clean up allocated vertices and edges.
     * @complexity $O(|V| + |E|)$ where $|V|$ is the number of vertices and $|E|$ is the number of edges.
     */
    ~Graph() {
        for (auto v : vertexSet) {
            for (auto e : v->getAdj()) {
                delete e;
            }
            delete v;
        }
    }

    int getNumVertex() const { return vertexSet.size(); }
    
    std::vector<Vertex<T>*> getVertexSet() const { return vertexSet; }

    /**
     * @brief Finds a vertex in the graph by its info.
     * @complexity $O(|V|)$ in the worst case.
     */
    Vertex<T>* findVertex(const T& in) const {
        for (auto v : vertexSet) {
            if (v->getInfo() == in) return v;
        }
        return nullptr;
    }

    /**
     * @brief Adds a new vertex to the graph.
     * @complexity $O(|V|)$ due to the uniqueness check.
     */
    bool addVertex(const T& in) {
        if (findVertex(in) != nullptr) return false; 
        vertexSet.push_back(new Vertex<T>(in));
        return true;
    }

    /**
     * @brief Adds a directed edge between two existing vertices.
     * @complexity $O(|V|)$ to find the source and destination vertices.
     */
    bool addEdge(const T& sourc, const T& dest, double w = 0.0) {
        auto v1 = findVertex(sourc);
        auto v2 = findVertex(dest);
        if (v1 == nullptr || v2 == nullptr) return false;
        
        v1->addEdge(v2, w);
        return true;
    }
};

#endif // GRAPH_H