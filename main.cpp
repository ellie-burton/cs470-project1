/**
 * Comparative performance analysis:
 *   - Algorithm: Dijkstra vs Prim
 *   - Heap type: Binary (baseline), Pairing, Fibonacci
 *   - Graph types: random (sparse/dense), grid, synthetic worst-case
 *   - Sizes: small vs large
 *
 * Output: CSV to results.txt and console (Algo, HeapType, GraphClass, GraphType, N, TimeUS, Ops).
 * Time in microseconds so sub-millisecond runs are visible.
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <cstdio>
#include "Graph.h"
#include "BinaryHeap.h"
#include "FibonacciHeap.h"
#include "PairingHeap.h"
#include "Algorithms.h"

// ---- Graph generators ----

/** Random sparse: ~ edgeMultiplier * N undirected edges (each trial random u,v,w). */
void generateRandomSparse(Graph& g, int edgeMultiplier) {
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

/** Random dense: target fraction of max possible edges (n*(n-1)/2). */
void generateRandomDense(Graph& g, double edgeFraction) {
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

/**
 * Grid graph: 2D grid of rows*cols = N. Vertex (r,c) = r*cols + c.
 * 4-neighbor connectivity; edge weights 1..100.
 */
void generateGrid(Graph& g, int rows, int cols) {
    int n = rows * cols;
    if (g.numVertices != n) return;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;
            int w;
            if (c + 1 < cols) {
                w = 1 + std::rand() % 100;
                g.addUndirectedEdge(u, r * cols + (c + 1), w);
            }
            if (r + 1 < rows) {
                w = 1 + std::rand() % 100;
                g.addUndirectedEdge(u, (r + 1) * cols + c, w);
            }
        }
    }
}

/**
 * Synthetic worst-case: layered graph to stress decreaseKey.
 * Vertices partitioned into L layers (L ~ sqrt(N)). Each vertex in layer i
 * is connected to each vertex in layer i+1 (undirected). Many edges and
 * many heap updates.
 */
void generateWorstCaseLayered(Graph& g) {
    int n = g.numVertices;
    if (n <= 1) return;
    int L = static_cast<int>(std::sqrt(n));
    if (L < 2) L = 2;
    std::vector<int> layerStart(L + 1);
    int perLayer = (n + L - 1) / L;
    for (int i = 0; i <= L; i++)
        layerStart[i] = std::min(i * perLayer, n);

    for (int i = 0; i < L - 1; i++) {
        int a = layerStart[i], aEnd = layerStart[i + 1];
        int b = layerStart[i + 1], bEnd = layerStart[i + 2];
        for (int u = a; u < aEnd; u++) {
            for (int v = b; v < bEnd; v++) {
                int w = 1 + std::rand() % 100;
                g.addUndirectedEdge(u, v, w);
            }
        }
    }
}

// ---- Experiment runners ----

static void writeLine(std::ostream& out, std::ostream& log,
                      const char* algo, const char* heap, const char* graphClass,
                      const char* graphType, int N, long timeUs, long ops) {
    out << algo << "," << heap << "," << graphClass << "," << graphType << "," << N << "," << timeUs << "," << ops << "\n";
    log << algo << "," << heap << "," << graphClass << "," << graphType << "," << N << "," << timeUs << "," << ops << "\n";
}

void runDijkstra(const Graph& g, int N, const char* graphClass, const char* graphType,
                 std::ostream& out, std::ostream& log) {
    std::vector<int> distB, distP, distF;
    int n = g.numVertices;

    BinaryHeap<int> bh(n);
    auto t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &bh, distB);
    auto t1 = std::chrono::high_resolution_clock::now();
    long usB = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    long opsB = bh.getOperationCount();

    PairingHeap<int> ph(n);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &ph, distP);
    t1 = std::chrono::high_resolution_clock::now();
    long usP = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    long opsP = ph.getOperationCount();

    FibonacciHeap<int> fh(n);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &fh, distF);
    t1 = std::chrono::high_resolution_clock::now();
    long usF = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    long opsF = fh.getOperationCount();

    writeLine(out, log, "Dijkstra", "Binary",   graphClass, graphType, N, usB, opsB);
    writeLine(out, log, "Dijkstra", "Pairing",  graphClass, graphType, N, usP, opsP);
    writeLine(out, log, "Dijkstra", "Fibonacci", graphClass, graphType, N, usF, opsF);

    if (distB != distP || distP != distF)
        std::cerr << "Correctness warning: Dijkstra dist mismatch for " << graphClass << " " << graphType << " N=" << N << "\n";
}

void runPrim(const Graph& g, int N, const char* graphClass, const char* graphType,
             std::ostream& out, std::ostream& log) {
    std::vector<int> minEdgeB, minEdgeP, minEdgeF;
    int totalB = 0, totalP = 0, totalF = 0;
    int n = g.numVertices;

    BinaryHeap<int> bh(n);
    auto t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &bh, minEdgeB, totalB);
    auto t1 = std::chrono::high_resolution_clock::now();
    long usB = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    long opsB = bh.getOperationCount();

    PairingHeap<int> ph(n);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &ph, minEdgeP, totalP);
    t1 = std::chrono::high_resolution_clock::now();
    long usP = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    long opsP = ph.getOperationCount();

    FibonacciHeap<int> fh(n);
    t0 = std::chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &fh, minEdgeF, totalF);
    t1 = std::chrono::high_resolution_clock::now();
    long usF = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    long opsF = fh.getOperationCount();

    writeLine(out, log, "Prim", "Binary",   graphClass, graphType, N, usB, opsB);
    writeLine(out, log, "Prim", "Pairing",  graphClass, graphType, N, usP, opsP);
    writeLine(out, log, "Prim", "Fibonacci", graphClass, graphType, N, usF, opsF);

    if (totalB != totalP || totalP != totalF)
        std::cerr << "Correctness warning: Prim total weight mismatch for " << graphClass << " " << graphType << " N=" << N
                  << " (Binary=" << totalB << " Pairing=" << totalP << " Fibonacci=" << totalF << ")\n";
}

int main() {
    std::srand(42);

    std::ofstream out("results.txt");
    if (!out) {
        std::cerr << "Could not open results.txt for writing.\n";
        return 1;
    }

    const char* header = "Algo,HeapType,GraphClass,GraphType,N,TimeUS,Ops\n";
    out << header;
    std::cout << header;

    // Small vs large sizes
    const int smallSizes[] = { 100, 500 };
    const int largeSizes[] = { 1000, 5000 };
    const int numSmall = sizeof(smallSizes) / sizeof(smallSizes[0]);
    const int numLarge = sizeof(largeSizes) / sizeof(largeSizes[0]);

    // ---- Random graphs (sparse / dense) ----
    for (int i = 0; i < numSmall; i++) {
        int N = smallSizes[i];
        Graph sparse(N);
        generateRandomSparse(sparse, 5);
        Graph dense(N);
        generateRandomDense(dense, 0.15);

        runDijkstra(sparse, N, "random", "sparse", out, std::cout);
        runPrim(sparse, N, "random", "sparse", out, std::cout);
        runDijkstra(dense, N, "random", "dense", out, std::cout);
        runPrim(dense, N, "random", "dense", out, std::cout);
    }
    for (int i = 0; i < numLarge; i++) {
        int N = largeSizes[i];
        Graph sparse(N);
        generateRandomSparse(sparse, 5);
        Graph dense(N);
        generateRandomDense(dense, 0.15);

        runDijkstra(sparse, N, "random", "sparse", out, std::cout);
        runPrim(sparse, N, "random", "sparse", out, std::cout);
        runDijkstra(dense, N, "random", "dense", out, std::cout);
        runPrim(dense, N, "random", "dense", out, std::cout);
    }

    // ---- Grid graphs (small vs large) ----
    auto runGrid = [&](int rows, int cols) {
        int N = rows * cols;
        Graph g(N);
        generateGrid(g, rows, cols);
        char typeBuf[32];
        std::snprintf(typeBuf, sizeof(typeBuf), "grid_%dx%d", rows, cols);
        runDijkstra(g, N, "grid", typeBuf, out, std::cout);
        runPrim(g, N, "grid", typeBuf, out, std::cout);
    };

    runGrid(10, 10);    // 100
    runGrid(22, 23);    // 506 ~ small
    runGrid(32, 32);    // 1024
    runGrid(70, 72);    // 5040 ~ large

    // ---- Synthetic worst-case (layered) ----
    for (int i = 0; i < numSmall; i++) {
        int N = smallSizes[i];
        Graph g(N);
        generateWorstCaseLayered(g);
        runDijkstra(g, N, "worst_case", "layered", out, std::cout);
        runPrim(g, N, "worst_case", "layered", out, std::cout);
    }
    for (int i = 0; i < numLarge; i++) {
        int N = largeSizes[i];
        Graph g(N);
        generateWorstCaseLayered(g);
        runDijkstra(g, N, "worst_case", "layered", out, std::cout);
        runPrim(g, N, "worst_case", "layered", out, std::cout);
    }

    out.close();
    std::cout << "Results written to results.txt\n";

    return 0;
}
