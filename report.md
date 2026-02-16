# Comparative Analysis of Priority Queues in Graph Algorithms

**CS470 Project 1**  
Ellie Burton, William Mulhern, Justin Gomez

---

## 1. Problem Description & Background

Two fundamental graph problems drive this study: **single-source shortest paths** and **minimum spanning tree (MST)**. Both are solved by iteratively expanding a “frontier” of vertices, repeatedly choosing the next vertex via a **minimum** over some key (distance or edge weight). That minimum is exactly what a **priority queue** provides; the efficiency of the overall algorithm therefore depends on how fast the queue supports *insert*, *extract-min*, and *decrease-key*.

In theory, **Fibonacci heaps** give the best known amortized bounds for these operations and yield O(*m* + *n* log *n*) time for Dijkstra and Prim on a graph with *n* vertices and *m* edges [1]. In practice, their pointer-heavy structure and complex *decrease-key* (cascading cut) can make them slower than a simple **binary heap**, which has higher per-operation cost but much lower constant factors. **Pairing heaps** were proposed as a simpler, practically competitive alternative to Fibonacci heaps [2], with good amortized behavior in theory and often fewer operations in experiments, but their actual runtimes depend heavily on implementation and workload.

This project compares **Binary**, **Pairing**, and **Fibonacci** heaps as the priority queue in **Dijkstra’s algorithm** (shortest paths from one source) and **Prim’s algorithm** (MST), across **random** (sparse/dense), **grid**, and **synthetic worst-case** graphs. We measure both **wall-clock time** (microseconds) and **heap operation counts** to see where theory matches practice and where constant factors dominate.

---

## 2. Algorithm Explanation

### 2.1 Dijkstra’s Algorithm

Dijkstra’s algorithm [3] solves the single-source shortest-path problem on a graph with non-negative edge weights. From a source vertex *s*, it maintains a distance estimate *d*[*v*] for each vertex *v* and a priority queue of vertices keyed by *d*[*v*].

1. Initialize *d*[*s*] = 0 and *d*[*v*] = ∞ for *v* ≠ *s*. Insert *s* into the priority queue with key 0.
2. While the queue is non-empty:
   - **Extract-min** (*d*[*u*], *u*). The first time a vertex *u* is extracted, *d*[*u*] is the true shortest distance from *s* to *u*.
   - For each edge (*u*, *v*) with weight *w*: if *d*[*u*] + *w* < *d*[*v*], set *d*[*v*] = *d*[*u*] + *w*. If *v* is already in the queue, **decrease-key**(*v*, *d*[*v*]); otherwise **insert**(*v*, *d*[*v*]).

Each vertex is extracted at most once; each edge may trigger at most one decrease-key or insert. The algorithm therefore performs *O*(*n*) extract-mins and *O*(*m*) inserts plus decrease-keys, so the choice of heap directly affects total time.

### 2.2 Prim’s Algorithm

Prim’s algorithm (also attributed to Jarník [4] and Dijkstra [3]) builds a minimum spanning tree by growing a single tree from a source vertex. It keeps a key for each vertex: the minimum weight of any edge from that vertex into the current tree.

1. Choose a source *s*. Set *key*[*s*] = 0 and *key*[*v*] = ∞ for *v* ≠ *s*. Insert *s* with key 0 into the priority queue.
2. While the queue is non-empty:
   - **Extract-min** (*key*[*u*], *u*). The edge used to add *u* to the tree (the one achieving *key*[*u*]) is added to the MST.
   - For each edge (*u*, *v*) with weight *w*: if *v* is not yet in the tree and *w* < *key*[*v*], set *key*[*v*] = *w*. If *v* is in the queue, **decrease-key**(*v*, *key*[*v*]); otherwise **insert**(*v*, *key*[*v*]).

As with Dijkstra, we have *O*(*n*) extract-mins and *O*(*m*) inserts/decrease-keys. Prim tends to do slightly more decrease-keys than Dijkstra on the same graph because MST growth is driven by edge weights only, so the frontier can be updated more often.

### 2.3 Heap Implementations

**Binary heap (baseline).** A complete binary tree stored in an array; each node has key ≤ its children. Extract-min is O(log *n*) by replacing the root with the last element and bubbling down. Insert is O(log *n*) by appending and bubbling up. Decrease-key requires a *position* array (vertex id → index) so the key can be updated and the node bubbled up in O(log *n*). All operations have small constant factors and excellent cache locality [5, Ch. 6].

**Pairing heap [2].** A self-adjusting heap represented as a tree; the minimum is at the root. Melding two heaps is O(1) by making the larger root a child of the smaller. Insert is meld with a single node. Extract-min removes the root and *pairs* the children (e.g., two-pass merge) to form a new heap. Decrease-key is implemented by cutting the node from its parent and melding it with the root. In theory, amortized bounds are O(log *n*) for extract-min and O(2↑↑(−*c*)) or similar for decrease-key under certain analyses; in practice, pairing heaps often perform *fewer* comparisons than binary heaps, but pointer chasing and merge overhead can increase wall-clock time.

**Fibonacci heap [1].** A collection of heap-ordered trees with lazy consolidation. Insert and meld are O(1) amortized; decrease-key is O(1) amortized (with cascading cut to preserve structure); extract-min does consolidation and is O(log *n*) amortized. This yields O(*m* + *n* log *n*) for Dijkstra and Prim in the RAM model. The implementation is more complex (root list, child lists, marked bits, consolidation by degree), so constant factors and cache behavior are worse than for a binary heap in many practical settings.

---

## 3. Complexity Analysis

| Structure      | Insert (amort.) | Extract-min (amort.) | Decrease-key (amort.) |
|----------------|----------------|----------------------|------------------------|
| Binary heap    | O(log *n*)     | O(log *n*)           | O(log *n*)             |
| Pairing heap   | O(1) [2]       | O(log *n*)           | O(log log *n*) or similar [2,6] |
| Fibonacci heap | O(1) [1]       | O(log *n*)           | O(1) [1]               |

**Dijkstra / Prim with *n* vertices and *m* edges:**

- Binary heap: *O*((*m* + *n*) log *n*) total.
- Fibonacci heap: *O*(*m* + *n* log *n*) total, which is asymptotically better for dense graphs (*m* = Ω(*n* log *n*)).

Space: *O*(*n*) for all three heaps (each vertex has at most one heap node at a time in our usage).

---

## 4. Evaluation & Results

### 4.1 Experimental Setup

- **Platform:** Single run per configuration; time measured with `std::chrono::high_resolution_clock` in **microseconds** (TimeUS); operation count = insert + extractMin + decreaseKey.
- **Graph types:**
  - **Random sparse:** ~5·*n* undirected edges, weights 1–100.
  - **Random dense:** ~15% of *n*(*n*−1)/2 edges, weights 1–100.
  - **Grid:** 2D grid (4-neighbor), weights 1–100; sizes 10×10, 22×23, 32×32, 70×72.
  - **Worst-case (layered):** √*n* layers, full bipartite edges between adjacent layers to stress decrease-key.
- **Sizes:** *n* = 100, 500 (small), 1000, 5000 (large) for random and worst-case; grid *n* = 100, 506, 1024, 5040.
- **Correctness:** For each graph, Dijkstra distances and Prim MST total weight are compared across the three heaps; no mismatches were reported (all heaps agreed).

### 4.2 Results Summary

Data below are from **results.txt** (time in µs, Ops = heap operation count).

**Random sparse, *n* = 5000**

| Algo     | Heap      | Time (µs) | Ops   |
|----------|-----------|-----------|-------|
| Dijkstra | Binary    | 1,040     | 14,777 |
| Dijkstra | Pairing   | 14,670    | 12,342 |
| Dijkstra | Fibonacci | 4,288     | 14,766 |
| Prim     | Binary    | 799       | 17,566 |
| Prim     | Pairing   | 8,213     | 15,109 |
| Prim     | Fibonacci | 4,059     | 17,564 |

**Random dense, *n* = 5000**

| Algo     | Heap      | Time (µs) | Ops   |
|----------|-----------|-----------|-------|
| Dijkstra | Binary    | 15,317    | 27,937 |
| Dijkstra | Pairing   | 31,541    | 16,840 |
| Dijkstra | Fibonacci | 19,711    | 28,049 |
| Prim     | Binary    | 15,221    | 30,908 |
| Prim     | Pairing   | 27,895    | 18,112 |
| Prim     | Fibonacci | 17,990    | 30,930 |

**Grid 70×72 (*n* = 5040)**

| Algo     | Heap      | Time (µs) | Ops   |
|----------|-----------|-----------|-------|
| Dijkstra | Binary    | 330       | 11,594 |
| Dijkstra | Pairing   | 8,572     | 10,869 |
| Dijkstra | Fibonacci | 3,223     | 11,600 |
| Prim     | Binary    | 616       | 13,290 |
| Prim     | Pairing   | 4,487     | 12,585 |
| Prim     | Fibonacci | 3,338     | 13,288 |

**Worst-case (layered), *n* = 5000**

| Algo     | Heap      | Time (µs) | Ops   |
|----------|-----------|-----------|-------|
| Dijkstra | Binary    | 2,772     | 25,737 |
| Dijkstra | Pairing   | 13,585    | 16,346 |
| Dijkstra | Fibonacci | 6,532     | 25,734 |
| Prim     | Binary    | 3,840     | 29,123 |
| Prim     | Pairing   | 11,676    | 19,297 |
| Prim     | Fibonacci | 7,619     | 29,244 |

### 4.3 Observations

- **Time:** Binary is fastest in every configuration; Pairing is slowest; Fibonacci is in between (typically ~2–3× Binary, and faster than Pairing).
- **Ops:** Pairing has the fewest operations in every configuration; Binary and Fibonacci have nearly the same op counts (often within a few percent).
- **Scaling:** As *n* and *m* grow, the time gap between Binary and Pairing widens (e.g., random dense *n*=5000: Binary ~15 ms, Pairing ~28–31 ms).
- **Correctness:** All three heaps produced identical Dijkstra distances and identical Prim MST total weights on every run; no correctness warnings.

---

## 5. Comparative Discussion

**Do Fibonacci heaps provide practical benefits?**  
In our experiments, **no**: Fibonacci heaps were always slower than the binary heap in wall-clock time, despite having better asymptotic complexity. The gain from fewer operations (which was small; op counts were close to Binary) did not offset the cost of pointer manipulation, consolidation, and cascading cut. For the graph sizes and platforms we used, the binary heap’s simplicity and cache friendliness dominate.

**How do pairing heaps compare in practice?**  
Pairing heaps did **fewer** heap operations than both Binary and Fibonacci in every test, which is consistent with their design as a “lazy” self-adjusting structure. Nevertheless, they were the **slowest** in time. Each operation (especially merge and decrease-key) does more pointer work and has worse locality than the binary heap’s array-based operations. So in our implementation, the constant factors of the pairing heap outweigh its advantage in operation count.

**Which algorithm benefits more from advanced heaps?**  
Neither Dijkstra nor Prim benefited in *time* from using Pairing or Fibonacci instead of Binary; both algorithms ran fastest with the binary heap. In terms of *operation count*, Pairing reduced ops for both algorithms by a similar proportion (roughly 10–40% fewer ops than Binary/Fibonacci depending on graph). So the “benefit” of advanced heaps here shows up only in op count, not in runtime.

**How does graph structure affect performance?**  
- **Sparse vs dense:** Dense graphs increase time and ops for all heaps; the relative ordering (Binary &lt; Fibonacci &lt; Pairing in time) stayed the same.  
- **Grid vs random:** Grids have bounded degree, so ops and time grow more gently; the same ordering holds.  
- **Worst-case (layered):** Designed to stress decrease-key. Pairing still did the fewest ops but remained slowest in time; Binary stayed fastest. So graph structure did not change which heap was practically best.

**Why theory and practice differ**  
Asymptotic analysis assumes a RAM model where pointer updates and comparisons have unit cost and ignores cache effects. In practice, the binary heap’s array layout yields good cache behavior and few instructions per operation. Fibonacci and pairing heaps incur more pointer chasing, more branches, and more memory traffic per operation. For *n* in the thousands and typical *m*, log *n* is small (e.g., ~12 for *n*=5000), so the difference between O(*m* log *n*) and O(*m* + *n* log *n*) is not large, and constant factors decide the winner. Our results align with the common observation that Fibonacci (and in our case pairing) heaps are often slower than binary heaps for graph algorithms at moderate scale.

---

## 6. References & Report Organization

### References

[1] M. L. Fredman and R. E. Tarjan. Fibonacci heaps and their uses in improved network optimization algorithms. *Journal of the ACM*, 34(3):596–615, July 1987.

[2] M. L. Fredman, R. Sedgewick, D. D. Sleator, and R. E. Tarjan. The pairing heap: A new form of self-adjusting heap. *Algorithmica*, 1(1):111–129, November 1986.

[3] E. W. Dijkstra. A note on two problems in connexion with graphs. *Numerische Mathematik*, 1(1):269–271, 1959.

[4] R. C. Prim. Shortest connection networks and some generalizations. *Bell System Technical Journal*, 36(6):1389–1401, 1957.

[5] T. H. Cormen, C. E. Leiserson, R. L. Rivest, and C. Stein. *Introduction to Algorithms*, 3rd ed. MIT Press, 2009.

[6] J. Iacono. Improved upper bounds for pairing heaps. *Algorithmica*, 32(2):209–224, 2002.

### Academic integrity

The implementation (binary, pairing, and Fibonacci heaps; Dijkstra and Prim; graph generators) was developed for this course project. The report text and analysis are our own; algorithms and complexity are described using standard sources cited above. Data in Section 4 comes from a single run of our program; results are reproducible by running `main.exe` as described in the project README.

---

*Full experimental data are in **results.txt**; one row per (algorithm, heap, graph class, graph type, *n*), with columns Algo, HeapType, GraphClass, GraphType, N, TimeUS, Ops.*
