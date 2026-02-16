// comparative run: dijkstra vs prim, binary/pairing/fib heaps, random/grid/worst-case graphs.
// output: results.txt + console (Algo, HeapType, GraphClass, GraphType, N, TimeUS, Ops)

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

using namespace std;

// graph generators

// ~ edgeMult*N undir edges, rand u,v,w
void generateRandomSparse(Graph& g, int edgeMultiplier) {
    int n = g.numVertices;
    int numEdges = n * edgeMultiplier;
    for (int i = 0; i < numEdges; i++) {
        int u = rand() % n;
        int v = rand() % n;
        if (u == v) continue;
        int w = 1 + rand() % 100;
        g.addUndirectedEdge(u, v, w);
    }
}

// frac of max edges n(n-1)/2
void generateRandomDense(Graph& g, double edgeFraction) {
    int n = g.numVertices;
    long maxEdges = static_cast<long>(n) * (n - 1) / 2;
    long target = static_cast<long>(maxEdges * edgeFraction);
    for (long i = 0; i < target; i++) {
        int u = rand() % n;
        int v = rand() % n;
        if (u == v) continue;
        int w = 1 + rand() % 100;
        g.addUndirectedEdge(u, v, w);
    }
}

// 2d grid rows*cols, 4-neighbor, w 1..100
void generateGrid(Graph& g, int rows, int cols) {
    int n = rows * cols;
    if (g.numVertices != n) return;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;
            int w;
            if (c + 1 < cols) {
                w = 1 + rand() % 100;
                g.addUndirectedEdge(u, r * cols + (c + 1), w);
            }
            if (r + 1 < rows) {
                w = 1 + rand() % 100;
                g.addUndirectedEdge(u, (r + 1) * cols + c, w);
            }
        }
    }
}

// layered: sqrt(n) layers, full bipartite between adjacent layers (stress decreaseKey)
void generateWorstCaseLayered(Graph& g) {
    int n = g.numVertices;
    if (n <= 1) return;
    int L = static_cast<int>(sqrt(n));
    if (L < 2) L = 2;
    vector<int> layerStart(L + 1);
    int perLayer = (n + L - 1) / L;
    for (int i = 0; i <= L; i++)
        layerStart[i] = min(i * perLayer, n);

    for (int i = 0; i < L - 1; i++) {
        int a = layerStart[i], aEnd = layerStart[i + 1];
        int b = layerStart[i + 1], bEnd = layerStart[i + 2];
        for (int u = a; u < aEnd; u++) {
            for (int v = b; v < bEnd; v++) {
                int w = 1 + rand() % 100;
                g.addUndirectedEdge(u, v, w);
            }
        }
    }
}

// one csv row
static void writeLine(ostream& out, ostream& log,
                      const char* algo, const char* heap, const char* graphClass,
                      const char* graphType, int N, long timeUs, long ops) {
    out << algo << "," << heap << "," << graphClass << "," << graphType << "," << N << "," << timeUs << "," << ops << "\n";
    log << algo << "," << heap << "," << graphClass << "," << graphType << "," << N << "," << timeUs << "," << ops << "\n";
}

void runDijkstra(const Graph& g, int N, const char* graphClass, const char* graphType,
                 ostream& out, ostream& log) {
    vector<int> distB, distP, distF;
    int n = g.numVertices;

    BinaryHeap<int> bh(n);
    auto t0 = chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &bh, distB);
    auto t1 = chrono::high_resolution_clock::now();
    long usB = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    long opsB = bh.getOperationCount();

    PairingHeap<int> ph(n);
    t0 = chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &ph, distP);
    t1 = chrono::high_resolution_clock::now();
    long usP = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    long opsP = ph.getOperationCount();

    FibonacciHeap<int> fh(n);
    t0 = chrono::high_resolution_clock::now();
    Algorithms::runDijkstra(g, 0, &fh, distF);
    t1 = chrono::high_resolution_clock::now();
    long usF = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    long opsF = fh.getOperationCount();

    writeLine(out, log, "Dijkstra", "Binary", graphClass, graphType, N, usB, opsB);
    writeLine(out, log, "Dijkstra", "Pairing", graphClass, graphType, N, usP, opsP);
    writeLine(out, log, "Dijkstra", "Fibonacci", graphClass, graphType, N, usF, opsF);

    if (distB != distP || distP != distF)
        cerr << "Correctness warning: Dijkstra dist mismatch for " << graphClass << " " << graphType << " N=" << N << "\n";
}

void runPrim(const Graph& g, int N, const char* graphClass, const char* graphType,
             ostream& out, ostream& log) {
    vector<int> minEdgeB, minEdgeP, minEdgeF;
    int totalB = 0, totalP = 0, totalF = 0;
    int n = g.numVertices;

    BinaryHeap<int> bh(n);
    auto t0 = chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &bh, minEdgeB, totalB);
    auto t1 = chrono::high_resolution_clock::now();
    long usB = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    long opsB = bh.getOperationCount();

    PairingHeap<int> ph(n);
    t0 = chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &ph, minEdgeP, totalP);
    t1 = chrono::high_resolution_clock::now();
    long usP = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    long opsP = ph.getOperationCount();

    FibonacciHeap<int> fh(n);
    t0 = chrono::high_resolution_clock::now();
    Algorithms::runPrim(g, 0, &fh, minEdgeF, totalF);
    t1 = chrono::high_resolution_clock::now();
    long usF = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    long opsF = fh.getOperationCount();

    writeLine(out, log, "Prim", "Binary", graphClass, graphType, N, usB, opsB);
    writeLine(out, log, "Prim", "Pairing", graphClass, graphType, N, usP, opsP);
    writeLine(out, log, "Prim", "Fibonacci", graphClass, graphType, N, usF, opsF);

    if (totalB != totalP || totalP != totalF)
        cerr << "Correctness warning: Prim total weight mismatch for " << graphClass << " " << graphType << " N=" << N
             << " (Binary=" << totalB << " Pairing=" << totalP << " Fibonacci=" << totalF << ")\n";
}

int main() {
    srand(42);

    ofstream out("results.txt");
    if (!out) {
        cerr << "Could not open results.txt for writing.\n";
        return 1;
    }

    const char* header = "Algo,HeapType,GraphClass,GraphType,N,TimeUS,Ops\n";
    out << header;
    cout << header;

    const int smallSizes[] = { 100, 500 };
    const int largeSizes[] = { 1000, 5000 };
    const int numSmall = sizeof(smallSizes) / sizeof(smallSizes[0]);
    const int numLarge = sizeof(largeSizes) / sizeof(largeSizes[0]);

    // random sparse/dense
    for (int i = 0; i < numSmall; i++) {
        int N = smallSizes[i];
        Graph sparse(N);
        generateRandomSparse(sparse, 5);
        Graph dense(N);
        generateRandomDense(dense, 0.15);
        runDijkstra(sparse, N, "random", "sparse", out, cout);
        runPrim(sparse, N, "random", "sparse", out, cout);
        runDijkstra(dense, N, "random", "dense", out, cout);
        runPrim(dense, N, "random", "dense", out, cout);
    }
    for (int i = 0; i < numLarge; i++) {
        int N = largeSizes[i];
        Graph sparse(N);
        generateRandomSparse(sparse, 5);
        Graph dense(N);
        generateRandomDense(dense, 0.15);
        runDijkstra(sparse, N, "random", "sparse", out, cout);
        runPrim(sparse, N, "random", "sparse", out, cout);
        runDijkstra(dense, N, "random", "dense", out, cout);
        runPrim(dense, N, "random", "dense", out, cout);
    }

    // grids
    auto runGrid = [&](int rows, int cols) {
        int N = rows * cols;
        Graph g(N);
        generateGrid(g, rows, cols);
        char typeBuf[32];
        snprintf(typeBuf, sizeof(typeBuf), "grid_%dx%d", rows, cols);
        runDijkstra(g, N, "grid", typeBuf, out, cout);
        runPrim(g, N, "grid", typeBuf, out, cout);
    };
    runGrid(10, 10);
    runGrid(22, 23);
    runGrid(32, 32);
    runGrid(70, 72);

    // worst-case layered
    for (int i = 0; i < numSmall; i++) {
        int N = smallSizes[i];
        Graph g(N);
        generateWorstCaseLayered(g);
        runDijkstra(g, N, "worst_case", "layered", out, cout);
        runPrim(g, N, "worst_case", "layered", out, cout);
    }
    for (int i = 0; i < numLarge; i++) {
        int N = largeSizes[i];
        Graph g(N);
        generateWorstCaseLayered(g);
        runDijkstra(g, N, "worst_case", "layered", out, cout);
        runPrim(g, N, "worst_case", "layered", out, cout);
    }

    out.close();
    cout << "Results written to results.txt\n";
    return 0;
}
