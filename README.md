# CS470 Project 1 — Comparative Analysis of Priority Queues in Graph Algorithms

**Authors:** Ellie Burton, William Mulhern, Justin Gomez

This project runs a **comparative performance analysis** across:

- **Algorithm:** Dijkstra vs Prim  
- **Heap type:** Binary (baseline), Pairing, Fibonacci  
- **Graph characteristics:** sparse vs dense, small vs large  

**Graph types:** random graphs (sparse/dense), grid graphs, and synthetic worst-case (layered) graphs.  

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
| **Testing / diagnostics** | |
| `test_prim_debug.cpp` | Diagnostic: compares Prim’s extract sequence and total weight (Binary vs Pairing). |
| **Documentation & specs** | |
| `requirements.md` | Project requirements and deliverables. |
| `plan.md` | Task breakdown and plan. |
| `report.md` | Report outline and analysis (to be filled). |
| `To_fix.md` | Project state, resolved bugs, and suggested next steps. |
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
- If Prim total weight differs between heaps for any graph, a correctness warning is printed to stderr.

---

## Where results are found

- **`results.txt`** — CSV with columns: `Algo`, `HeapType`, `GraphClass`, `GraphType`, `N`, `TimeUS`, `Ops`.  
  - **TimeUS** = time in **microseconds** (so sub-millisecond runs show non-zero values).  
  - **GraphClass:** `random`, `grid`, `worst_case`. **GraphType:** e.g. `sparse`, `dense`, `grid_10x10`, `layered`.
- Console — Same CSV plus any correctness warnings (stderr).

---

## Analysis (overview of initial results)

The following summarizes patterns from **`results.txt`** (one run per configuration; times in microseconds).

### Time (wall-clock)

- **Binary heap** is usually the **fastest** in wall-clock time across graph types and sizes. It has the lowest constant-factor overhead per operation.
- **Pairing heap** is typically the **slowest** in time, even though it performs fewer heap operations (see below). Per-operation cost (pointer updates, merging) dominates.
- **Fibonacci heap** is **in between**: often 2–3× slower than Binary and faster than Pairing. On large dense and worst-case graphs it can approach Binary.

Example (random dense, *N* = 5000): Dijkstra — Binary 15.3 ms, Pairing 31.5 ms, Fibonacci 19.7 ms. Prim — Binary 15.2 ms, Pairing 27.9 ms, Fibonacci 18.0 ms.

### Operation counts (Ops)

- **Pairing heap** consistently has the **fewest** operations (insert + extractMin + decreaseKey). This matches theory: pairing heaps can do fewer comparisons/updates per decreaseKey in practice.
- **Binary** and **Fibonacci** have **similar** op counts (often within a few percent). Fibonacci’s amortized behavior doesn’t always show up as fewer total ops in these runs.

So: Pairing wins on *number* of operations; Binary wins on *time*; Fibonacci is a middle ground.

### By graph type

- **Random (sparse / dense):** Same relative ordering (Binary fastest, Pairing slowest). Dense graphs increase both time and ops for all heaps.
- **Grid:** Same ordering. Grids have limited degree, so ops and time grow more gently than in dense random graphs.
- **Worst-case (layered):** Stresses decreaseKey-heavy behavior. Pairing still has the fewest ops (e.g. ~16k–19k vs ~25k–29k for Binary/Fibonacci at *N* = 5000) but remains slowest in time.

### By size (small vs large)

- **Small (*N* = 100–500):** Differences in time are small (tens to hundreds of µs). Binary’s advantage is clear but not dramatic.
- **Large (*N* = 1000–5000):** Gaps widen. Binary stays relatively fast; Pairing’s time grows faster (e.g. 2× Binary or more on dense and worst-case).

### Dijkstra vs Prim

- **Prim** generally has **more ops** than Dijkstra on the same graph (MST adds more edges to the frontier). Time trends are similar: Binary fastest, Pairing slowest, Fibonacci in between.

For detailed tables, plots, and conclusions, see **`report.md`** (and re-run experiments as needed for multiple trials or additional sizes).

---

## Optional: Prim diagnostic (Binary vs Pairing)

To compare Prim’s extract sequence and total weight between Binary and Pairing heaps (e.g. for debugging):

```bash
g++ -std=c++17 -O2 -o test_prim_debug.exe test_prim_debug.cpp
./test_prim_debug.exe
```

Reports totals and extract counts for fixed and random graphs; on mismatch, reports the first differing extract index and values.

---

## Further information

- **Project state and fixes:** See **To_fix.md** for implementation status and resolved bugs (e.g. Pairing heap + Prim).
- **Requirements and deliverables:** See **requirements.md**.
- **Report:** See **report.md** for the written report outline and analysis.
