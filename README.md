# CS470 Project 1 
## Comparative Analysis of Priority Queues in Graph Algorithms

**Group:** Ellie Burton, William Mulhern, Justin Gomez

This project runs a comparative performance analysis across:

- Algorithms: Dijkstra vs Prim  
- Heaps: Binary (baseline), Pairing, Fibonacci  
- Graph sizes: small (n = 100-500) vs large (n = 5,000-10,000)
- Graph types: random graphs (sparse/dense), grid graphs, and synthetic worst-case (layered) graphs.  

It measures runtime and operation counts and checks correctness across heaps.

---

## File structure

| Path | Description |
|------|-------------|
| **Core source** | |
| `main.cpp` | Experiment driver: random (sparse/dense), grid, worst-case graphs; Dijkstra/Prim × Binary/Pairing/Fibonacci; writes CSV to `results.txt`. |
| `Graph.h` | Graph representation (adjacency list, `addEdge`, `addUndirectedEdge`, `numVertices`). |
| `Algorithms.h` | `runDijkstra` and `runPrim`; both take a `PriorityQueue<int>*` and are heap-agnostic. |
| **Priority queues** | |
| `PriorityQueue.h` | Abstract base: `insert`, `extractMin`, `isEmpty`, `decreaseKey`, `getOperationCount`. |
| `BinaryHeap.h` | Binary min-heap with position array for `decreaseKey`. |
| `PairingHeap.h` | Pairing heap with two-pass merge and tie-breaking. |
| `FibonacciHeap.h` | Fibonacci heap with root list, consolidate, and cascading cut. |
| **Output** | |
| `results.txt` | CSV from `main.exe`: `Algo`, `HeapType`, `GraphClass`, `GraphType`, `N`, `TimeUS`, `Ops`. Time in microseconds. |

---

## Prerequisites

- C++17 compiler (e.g. **g++**; on Windows with Cygwin, ensure `C:\cygwin64\bin` is on your PATH, or set it when building).

---

## Build

From the project root:

```bash
g++ -std=c++17 -O2 -o main.exe main.cpp
```

On Windows (PowerShell) with Cygwin g++ not on PATH:

```powershell
cd "c:\path\to\cs470-project1"
$env:PATH = "C:\cygwin64\bin;" + $env:PATH
g++ -std=c++17 -O2 -o main.exe main.cpp
```

---

## Run the main test execution

Run the main experiments (Dijkstra and Prim with Binary, Pairing, and Fibonacci heaps on sparse and dense graphs of several sizes):

```bash
./main.exe
```

- Prints a CSV header and one row per run to the console.
- Writes the same CSV to **`results.txt`** in the project root.
---

## Results

- **`results.txt`** — CSV with columns: `Algo`, `HeapType`, `GraphClass`, `GraphType`, `N`, `TimeUS`, `Ops`.  
  - **TimeUS** = time in **microseconds** (so sub-millisecond runs show non-zero values).  
  - **GraphClass:** `random`, `grid`, `worst_case`.
  - **GraphType:** e.g. `sparse`, `dense`, `grid_10x10`, `layered`.

-- -
