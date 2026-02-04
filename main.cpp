#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Graph.h"
#include "BinaryHeap.h"
#include "FibonacciHeap.h"
#include "PairingHeap.h"
#include "Algorithms.h"

void generateSparseGraph(Graph& g, int edgeMultiplier) {
    int n = g.numVertices;
    int numEdges = n * edgeMultiplier;
    for (int i = 0; i < numEdges; i++) {
        int u = std::rand() % n;
        int v = std::rand() % n;
        if (u == v) continue;
        int w = 1 + std::rand() % 100;
        g.addUndirectedEdge(u, v, w);
    }
}

void generateDenseGraph(Graph& g, double edgeFraction) {
    int n = g.numVertices;
    long maxEdges = static_cast<long>(n) * (n - 1) / 2;
    long target = static_cast<long>(maxEdges * edgeFraction);
    for (long i = 0; i < target; i++) {
        int u = std::rand() % n;
        int v = std::rand() % n;
        if (u == v) continue;
        int w = 1 + std::rand() % 100;
        g.addUndirectedEdge(u, v, w);
    }
}

void runDijkstraExperiments(const Graph& g, int N, const char* graphType,
                            std::ostream& out, std::ostream& log) {
    std::vector<int> distB, distP, distF;

    BinaryHeap<int> bh(N);
    auto t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &bh, distB);
    auto t1 = std::chrono::high_resolution_clock::now();
    long opsB = bh.getOperationCount();
    long msB = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    PairingHeap<int> ph(N);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &ph, distP);
    t1 = std::chrono::high_resolution_clock::now();
    long opsP = ph.getOperationCount();
    long msP = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    FibonacciHeap<int> fh(N);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &fh, distF);
    t1 = std::chrono::high_resolution_clock::now();
    long opsF = fh.getOperationCount();
    long msF = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    auto line = [&](const char* algo, const char* heap, long ms, long ops) {
        out << algo << "," << heap << "," << graphType << "," << N << "," << ms << "," << ops << "\n";
        log << algo << "," << heap << "," << graphType << "," << N << "," << ms << "," << ops << "\n";
    };
    line("Dijkstra", "Binary", msB, opsB);
    line("Dijkstra", "Pairing", msP, opsP);
    line("Dijkstra", "Fibonacci", msF, opsF);

    if (distB != distP || distP != distF)
        std::cerr << "Correctness warning: Dijkstra dist mismatch for " << graphType << " N=" << N << "\n";
}

void runPrimExperiments(const Graph& g, int N, const char* graphType,
                        std::ostream& out, std::ostream& log) {
    std::vector<int> minEdgeB, minEdgeP, minEdgeF;
    int totalB = 0, totalP = 0, totalF = 0;

    BinaryHeap<int> bh(N);
    auto t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &bh, minEdgeB, totalB);
    auto t1 = std::chrono::high_resolution_clock::now();
    long opsB = bh.getOperationCount();
    long msB = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    PairingHeap<int> ph(N);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &ph, minEdgeP, totalP);
    t1 = std::chrono::high_resolution_clock::now();
    long opsP = ph.getOperationCount();
    long msP = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    FibonacciHeap<int> fh(N);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &fh, minEdgeF, totalF);
    t1 = std::chrono::high_resolution_clock::now();
    long opsF = fh.getOperationCount();
    long msF = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    auto line = [&](const char* algo, const char* heap, long ms, long ops) {
        out << algo << "," << heap << "," << graphType << "," << N << "," << ms << "," << ops << "\n";
        log << algo << "," << heap << "," << graphType << "," << N << "," << ms << "," << ops << "\n";
    };
    line("Prim", "Binary", msB, opsB);
    line("Prim", "Pairing", msP, opsP);
    line("Prim", "Fibonacci", msF, opsF);

    if (totalB != totalP || totalP != totalF)
        std::cerr << "Correctness warning: Prim total weight mismatch for " << graphType << " N=" << N
                  << " (Binary=" << totalB << " Pairing=" << totalP << " Fibonacci=" << totalF << ")\n";
}

int main() {
    std::srand(42);

    std::ofstream out("results.txt");
    if (!out) {
        std::cerr << "Could not open results.txt for writing.\n";
        return 1;
    }

    out << "Algo,HeapType,GraphType,GraphSize,TimeMS,Ops\n";
    std::cout << "Algo,HeapType,GraphType,GraphSize,TimeMS,Ops\n";

    // Requirements: "sparse vs dense, small vs large" (no specific sizes). Document parameters.
    int sizes[] = { 50, 100, 200, 1000, 5000};  // small vs large; adjust as needed for report
    const int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int si = 0; si < numSizes; si++) {
        int N = sizes[si];

        Graph sparse(N);
        generateSparseGraph(sparse, 5);   // ~5*V undirected edges

        Graph dense(N);
        generateDenseGraph(dense, 0.15); // ~15% of max edges (V*(V-1)/2)

        runDijkstraExperiments(sparse, N, "sparse", out, std::cout);
        runDijkstraExperiments(dense, N, "dense", out, std::cout);
        runPrimExperiments(sparse, N, "sparse", out, std::cout);
        runPrimExperiments(dense, N, "dense", out, std::cout);
    }

    out.close();
    std::cout << "Results written to results.txt\n";

    return 0;
}
