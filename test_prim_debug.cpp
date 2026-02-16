// compare prim extract (key,value) order: binary vs pairing on same graph. first diff = where they diverge.
// build: g++ -std=c++17 -o test_prim_debug.exe test_prim_debug.cpp

#include <iostream>
#include <vector>
#include <cstdlib>
#include "Graph.h"
#include "BinaryHeap.h"
#include "PairingHeap.h"
#include "Algorithms.h"

using namespace std;

// run prim but record every extract into extracted[]
static void runPrimTrace(const Graph& g, int startNode, PriorityQueue<int>* pq,
                         vector<pair<int,int>>& extracted, int& totalWeight) {
    int n = g.numVertices;
    vector<int> minEdge(n, INF);
    minEdge[startNode] = 0;
    vector<bool> inPQ(n, false);
    vector<bool> inMST(n, false);

    pq->insert(0, startNode);
    inPQ[startNode] = true;
    totalWeight = 0;
    extracted.clear();

    while (!pq->isEmpty()) {
        int key, u;
        pq->extractMin(key, u);
        extracted.push_back({key, u});
        inPQ[u] = false;
        if (inMST[u]) {
            cerr << "  [DUPLICATE extract vertex " << u << " key=" << key << "]\n";
            continue;
        }
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

int main() {
    srand(42);

    // fixed small graph
    {
        const int N = 6;
        Graph g(N);
        g.addUndirectedEdge(0, 1, 4);
        g.addUndirectedEdge(0, 2, 3);
        g.addUndirectedEdge(1, 2, 1);
        g.addUndirectedEdge(1, 3, 2);
        g.addUndirectedEdge(2, 3, 4);
        g.addUndirectedEdge(3, 4, 2);
        g.addUndirectedEdge(4, 5, 6);
        g.addUndirectedEdge(2, 5, 5);

        vector<pair<int,int>> extB, extP;
        int totalB = 0, totalP = 0;
        BinaryHeap<int> bh(N);
        runPrimTrace(g, 0, &bh, extB, totalB);
        PairingHeap<int> ph(N);
        runPrimTrace(g, 0, &ph, extP, totalP);
        cout << "[Fixed graph N=6] Binary total=" << totalB << " Pairing total=" << totalP;
        cout << (totalB == totalP && extB == extP ? " OK\n" : " MISMATCH\n");
    }

    // rand sparse N=50 (seed 42)
    {
        const int N = 50;
        Graph g(N);
        for (int i = 0; i < N * 5; i++) {
            int u = rand() % N, v = rand() % N;
            if (u == v) continue;
            g.addUndirectedEdge(u, v, 1 + rand() % 100);
        }
        vector<pair<int,int>> extB, extP;
        int totalB = 0, totalP = 0;
        BinaryHeap<int> bh(N);
        runPrimTrace(g, 0, &bh, extB, totalB);
        PairingHeap<int> ph(N);
        runPrimTrace(g, 0, &ph, extP, totalP);
        cout << "[Random sparse N=50] Binary total=" << totalB << " Pairing total=" << totalP;
        if (totalB != totalP) cout << " TOTAL MISMATCH\n";
        else if (extB != extP) {
            cout << " OK (sequence diff)\n";
            size_t i = 0;
            for (; i < extB.size() && i < extP.size(); i++)
                if (extB[i] != extP[i]) break;
            cout << "  First diff at index " << i << ": Binary(" << extB[i].first << "," << extB[i].second
                 << ") Pairing(" << extP[i].first << "," << extP[i].second << ")\n";
        } else
            cout << " OK\n";
    }

    // N=1000 sparse
    {
        srand(42);
        const int N = 1000;
        Graph g(N);
        for (int i = 0; i < N * 5; i++) {
            int u = rand() % N, v = rand() % N;
            if (u == v) continue;
            g.addUndirectedEdge(u, v, 1 + rand() % 100);
        }
        vector<pair<int,int>> extB, extP;
        int totalB = 0, totalP = 0;
        BinaryHeap<int> bh(N);
        runPrimTrace(g, 0, &bh, extB, totalB);
        PairingHeap<int> ph(N);
        runPrimTrace(g, 0, &ph, extP, totalP);
        cout << "[Random sparse N=1000] Binary total=" << totalB << " Pairing total=" << totalP;
        cout << " | Binary extracts=" << extB.size() << " Pairing extracts=" << extP.size();
        if (totalB != totalP) {
            cout << " TOTAL MISMATCH\n";
            size_t i = 0;
            for (; i < extB.size() && i < extP.size(); i++)
                if (extB[i] != extP[i]) {
                    cout << "  First diff at " << i << ": B(" << extB[i].first << "," << extB[i].second
                         << ") P(" << extP[i].first << "," << extP[i].second << ")\n";
                    break;
                }
        } else
            cout << " OK\n";
    }

    return 0;
}
