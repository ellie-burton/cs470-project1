# To Fix — Project State & Known Bugs

**Last updated:** After Pairing heap + Prim fix. Use this file to see what works, what’s fixed, and what’s left to do.

---

## Project completeness

- **Correctness:** All three heaps (Binary, Pairing, Fibonacci) are correct for both Dijkstra and Prim. No open correctness bugs.
- **Remaining work:** Report (`report.md`), testing section in README, optional refresh of `results.txt` after re-running full experiments.

---

## Current state

### Implemented and working

- **PriorityQueue.h** — Abstract base with `insert`, `extractMin`, `isEmpty`, `decreaseKey`, `getOperationCount`.
- **Graph.h** — Adjacency list, `addEdge`, `addUndirectedEdge`, `numVertices`.
- **BinaryHeap.h** — Baseline heap with `nodeMapping` (vector by vertex ID), all operations. **Correct for both algorithms.**
- **PairingHeap.h** — Two-pass merge, tie-breaking by `seq`, **correct for both Dijkstra and Prim** (see *Resolved bugs* below).
- **FibonacciHeap.h** — Full implementation with root list, consolidate, cascading cut. **Correct for both Dijkstra and Prim.**
- **Algorithms.h** — `runDijkstra` and `runPrim`; both take a `PriorityQueue<int>*` and are heap-agnostic.
- **main.cpp** — Builds graphs (sparse/dense), runs Dijkstra and Prim with Binary, Pairing, and Fibonacci heaps, writes CSV to `results.txt` and logs correctness warnings (Prim total weight mismatch).
- **test_prim_debug.cpp** — Diagnostic that compares Prim extract sequence and total weight (Binary vs Pairing); used to verify the Pairing heap fix.
- **report.md** — Report outline/placeholders (to be filled with analysis and results).

---

## Resolved bugs

### 1. Pairing heap + Prim: wrong MST total and missing extracts — **FIXED**

**Symptom (before fix)**

- Prim with Pairing heap gave a different (usually lower) total MST weight than Binary and Fibonacci on the same graph.
- On N=1000 sparse (seed 42), Binary did 1000 `extractMin` calls, Pairing did 977 → 23 nodes never extracted (stuck in the heap).

**Root cause**

- After `extractMin`, the new root is the result of `twoPassMerge(first)`. In `merge()`, only the node that becomes a **child** gets its `prev` pointer set; the node that becomes the **parent** (and thus the new root) keeps its old `prev`, which still pointed at the **old root**.
- The old root is then deleted, so the new root’s `prev` became a dangling pointer.
- Later, when `decreaseKey` ran on that root (or any node that had become a parent in a merge), it did `node->prev->sibling = ...`, i.e. **use-after-free**, corrupting the heap and orphaning nodes.

**Solution implemented**

- Ensure the root always has `prev == nullptr` by clearing it whenever we assign a new root:
  1. **extractMin:** After `root = twoPassMerge(first);`, add `if (root) root->prev = nullptr;`
  2. **decreaseKey:** After `root = merge(node, root);`, add `if (root) root->prev = nullptr;`
  3. **insert:** After `root = merge(root, newNode);`, add `if (root) root->prev = nullptr;`

**Verification**

- `test_prim_debug.exe`: N=1000 sparse — Binary total = Pairing total (12680), both 1000 extracts.
- `main.exe`: No “Correctness warning: Prim total weight mismatch” on any run; Prim totals match across Binary, Pairing, and Fibonacci for all graph types and sizes.

---

## Repo layout and how to run

- **Build (Cygwin g++):**  
  `$env:PATH = "C:\cygwin64\bin;" + $env:PATH; g++ -std=c++17 -O2 -o main.exe main.cpp`  
  (Or ensure `C:\cygwin64\bin` is on PATH.)
- **Run experiments:**  
  `./main.exe`  
  Writes CSV to `results.txt`. Correctness warnings appear if Prim total weight differs between heaps.
- **Diagnostic (Prim Binary vs Pairing):**  
  `g++ -std=c++17 -O2 -o test_prim_debug.exe test_prim_debug.cpp`  
  `./test_prim_debug.exe`  
  Compares extract (key, value) sequence and total weight; reports first difference and extract counts.

---

## Requirements / deliverables (from requirements.md)

- **Correctness – 30%:** Binary, Pairing, and Fibonacci are correct for both Dijkstra and Prim.
- **Testing:** Include testing and **results of test cases in repo** (e.g. keep `results.txt` or equivalent).
- **Report:** 8–12 pages, in repo as `.md` (see `report.md`).
- **No Python;** C/C++ preferred; minimal use of libraries (vector OK).

---

## Suggested order of work

1. ~~Fix Pairing heap + Prim~~ **Done.**
2. Fill in `report.md` (experiments, tables, analysis, conclusions).
3. Add a short testing section to README and point to `results.txt` and how to reproduce.
4. Optional: Re-run full experiments and refresh `results.txt`.
5. Final pass: README, .gitignore, remove any leftover temp files.
