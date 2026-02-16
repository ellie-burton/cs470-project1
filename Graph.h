#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
using std::vector;

struct Edge {
    int target;
    int weight;
};

class Graph {
public:
    int numVertices;
    vector<vector<Edge>> adjList;

    Graph(int n) : numVertices(n) {
        adjList.resize(n);
    }

    void addEdge(int u, int v, int weight) {
        adjList[u].push_back({v, weight});
    }

    void addUndirectedEdge(int u, int v, int weight) {
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }
};

#endif
