# CS470 Project 1 — Comparative Analysis of Priority Queues in Graph Algorithms

Priority queue implementations (Binary, Pairing, Fibonacci) used with Dijkstra and Prim on sparse/dense graphs.

## Build & run

- **Compiler:** C++17 (e.g. `g++` from Cygwin: `C:\cygwin64\bin\g++.exe`).
- **Build:** `g++ -std=c++17 -O2 -o main.exe main.cpp`
- **Run:** `./main.exe` — writes CSV to `results.txt` and prints to console.

## Project state and known bugs

See **To_fix.md** for current status, the Pairing-heap + Prim bug, and suggested next steps.

## Files

- `PriorityQueue.h`, `BinaryHeap.h`, `PairingHeap.h`, `FibonacciHeap.h` — heap implementations
- `Graph.h`, `Algorithms.h` — graph and Dijkstra/Prim
- `main.cpp` — experiment driver
- `test_prim_debug.cpp` — diagnostic (Binary vs Pairing extract sequence for Prim)
- `requirements.md`, `plan.md` — project spec and task list
- `report.md` — report outline (to be filled)
- `results.txt` — saved experiment output (CSV)