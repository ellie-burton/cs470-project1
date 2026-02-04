#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "Graph.h"
#include "PriorityQueue.h"
#include <vector>
#include <limits>

const int INF = std::numeric_limits<int>::max();

class Algorithms {
public:
    // Modular Dijkstra: insert when first seen, decreaseKey when already in PQ.
    static void runDijkstra(const Graph& g, int startNode, PriorityQueue<int>* pq, std::vector<int>& dist) {
        int n = g.numVertices;
        dist.assign(n, INF);
        dist[startNode] = 0;

        std::vector<bool> inPQ(n, false);
        std::vector<bool> visited(n, false);

        pq->insert(0, startNode);
        inPQ[startNode] = true;

        while (!pq->isEmpty()) {
            int d, u;
            pq->extractMin(d, u);
            inPQ[u] = false;
            if (visited[u]) continue;
            visited[u] = true;

            for (const auto& edge : g.adjList[u]) {
                int v = edge.target;
                int weight = edge.weight;
                int newDist = dist[u] + weight;

                if (!visited[v] && newDist < dist[v]) {
                    dist[v] = newDist;
                    if (inPQ[v])
                        pq->decreaseKey(v, dist[v]);
                    else {
                        pq->insert(dist[v], v);
                        inPQ[v] = true;
                    }
                }
            }
        }
    }

    // Prim's MST: minEdge[v] = weight of edge connecting v to MST; totalWeight = sum of minEdge.
    static void runPrim(const Graph& g, int startNode, PriorityQueue<int>* pq,
                        std::vector<int>& minEdge, int& totalWeight) {
        int n = g.numVertices;
        minEdge.assign(n, INF);
        minEdge[startNode] = 0;

        std::vector<bool> inPQ(n, false);
        std::vector<bool> inMST(n, false);

        pq->insert(0, startNode);
        inPQ[startNode] = true;
        totalWeight = 0;

        while (!pq->isEmpty()) {
            int key, u;
            pq->extractMin(key, u);
            inPQ[u] = false;
            if (inMST[u]) continue;
            inMST[u] = true;
            totalWeight += key;

            for (const auto& edge : g.adjList[u]) {
                int v = edge.target;
                int w = edge.weight;
                if (!inMST[v] && w < minEdge[v]) {
                    minEdge[v] = w;
                    if (inPQ[v])
                        pq->decreaseKey(v, minEdge[v]);
                    else {
                        pq->insert(minEdge[v], v);
                        inPQ[v] = true;
                    }
                }
            }
        }
    }
};

#endif
