# To Fix — Project State & Known Bugs

**Last updated:** For team handoff. Use this file to see what works, what’s broken, and what’s left to do.

---

## Current state

### Implemented and working

- **PriorityQueue.h** — Abstract base with `insert`, `extractMin`, `isEmpty`, `decreaseKey`, `getOperationCount`.
- **Graph.h** — Adjacency list, `addEdge`, `addUndirectedEdge`, `numVertices`.
- **BinaryHeap.h** — Baseline heap with `nodeMapping` (vector by vertex ID), all operations. **Correct for both algorithms.**
- **FibonacciHeap.h** — Full implementation with root list, consolidate, cascading cut. **Correct for both Dijkstra and Prim.**
- **Algorithms.h** — `runDijkstra` and `runPrim`; both take a `PriorityQueue<int>*` and are heap-agnostic.
- **main.cpp** — Builds graphs (sparse/dense), runs Dijkstra and Prim with Binary, Pairing, and Fibonacci heaps, writes CSV to `results.txt` and logs correctness warnings (Prim total weight mismatch).
- **report.md** — Report outline/placeholders (to be filled with analysis and results).

### Partially working

- **PairingHeap.h** — Works for **Dijkstra** (correct distances, no known bugs). **Fails for Prim** (wrong MST total, fewer extracts than vertices).

---

## Known bugs

### 1. Pairing heap + Prim: wrong MST total and missing extracts (open)

**Symptom**

- Prim with Pairing heap gives a **different (usually lower) total MST weight** than Binary and Fibonacci on the same graph.
- Diagnostic: on N=1000 sparse (seed 42), Binary does **1000** `extractMin` calls, Pairing does **977** → **23 nodes never extracted** (stuck in the heap).
- First difference in extract sequence can be early (e.g. index 14: Binary extracts vertex 254, Pairing extracts 244 for same key 11).

**What’s already been tried**

- **Tie-breaking:** Added `seq` (insertion order) in `merge()` so equal keys have deterministic order; helped small cases (e.g. N=50 totals match) but N=1000 still fails.
- **Two-pass merge:** Sibling links are cleared before pass 1 so `merge()` doesn’t overwrite the child list. Two-pass merge itself is not the sole cause (sequential merge made things worse).
- **Child list collection:** Collecting root’s children by following `sibling` until `nullptr` or a cycle yields **977 nodes** at the failing extract — so the root’s child list is already missing 23 nodes (either broken list or orphaning elsewhere).
- **decreaseKey:** Simplified to single `root = merge(node, root)` (no sibling chain). Bug persists.
- **extractMin:** Tried including `root->sibling` in the list to merge when present; no change (root effectively has no sibling in this design).

**Current hypothesis**

- Somewhere, a node’s `sibling` pointer is overwritten or the child list is otherwise broken, so 23 nodes are no longer reachable from `root->child` when they should be top-level children. Not yet located.

**Suggested next steps (for whoever debugs next)**

1. **Instrument `twoPassMerge`:** Log `nodes.size()` after collection (e.g. when N=1000). If it’s 977 at the bad extract, the bug is before/during extract (list shape or orphaning); if it’s 1000, the bug is in the merge/rest of extract.
2. **AddressSanitizer:** Build with `-fsanitize=address` and run the Prim + Pairing test; look for use-after-free or out-of-bounds writes.
3. **Single-step logging:** For one N=1000 run, log before/after each `extractMin` (e.g. root key/value, maybe a full heap node count). See at which extract the count or behavior first diverges.
4. **Report workaround:** For the report, Prim results can use Binary vs Fibonacci (they agree). Note Pairing as “correct for Dijkstra; Prim under investigation” or similar.

---

## Repo layout and how to run

- **Build (Cygwin g++):**  
  `g++ -std=c++17 -O2 -o main.exe main.cpp`  
  (Ensure `C:\cygwin64\bin` is on PATH, or use full path to `g++.exe`.)
- **Run experiments:**  
  `./main.exe`  
  Writes CSV to `results.txt` and prints to console. Correctness warnings appear when Prim total weight differs between heaps.
- **Diagnostic (Prim Binary vs Pairing):**  
  `g++ -std=c++17 -O2 -o test_prim_debug.exe test_prim_debug.cpp`  
  `./test_prim_debug.exe`  
  Compares extract (key, value) sequence and total weight; reports first difference and extract counts (e.g. 1000 vs 977).

---

## Requirements / deliverables (from requirements.md)

- **Correctness – 30%:** Binary and Fibonacci are correct for both algorithms; Pairing is correct for Dijkstra, incorrect for Prim until the bug is fixed.
- **Testing:** Include testing and **results of test cases in repo** (e.g. keep `results.txt` or equivalent).
- **Report:** 8–12 pages, in repo as `.md` (see `report.md`).
- **No Python;** C/C++ preferred; minimal use of libraries (vector OK).

---

## Suggested order of work

1. Fix Pairing heap + Prim (using To_fix suggestions above), then re-run full experiments and refresh `results.txt`.
2. Fill in `report.md` (experiments, tables, analysis, conclusions).
3. Add a short testing section to README and point to `results.txt` and how to reproduce.
4. Final pass: README, .gitignore, remove any leftover temp files.
