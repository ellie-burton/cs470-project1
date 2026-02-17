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

For the measurement data that was gathered during this project, we collected data using a single run per configuration. The execution time was measured in microseconds and can be done using the <chrono> library, using the std::chrono::high_resolution_clock in microseconds (TimeUS).
This covers the enter algorithm execution
All heap ops
Logic Loops
Edge relaxations
Comparison.
Pointer or array updated

The logical counter or operation count was computed as count = insert + extractMin + decreaseKey.This covered how much abstract heap work each implementation performed.
Four graph type families were tested in this project.
Random sparse: Contained approximately 5·n edges. 
Random dense: Contained approximately 15% of all possible edges. 
Grid: Used a 2D(4-neighbor) lattice.
Layered: Constructed with  √n layers and complete bipartite connections between 		adjacent layers to increase the frequency of decrease key operations.
Graph sizes ranged from n = 100 and 500 for smaller tests and n =1000 and 5000 for larger tests.Grid sizes ranged from 10  by 10 all the way up to 70 by 70. For every graph, Dijkstra and Prim were compared across all heap implementations. No discrepancies were found. 

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

Across all graph classes(and sizes)the binary heap consistently produced the lowest wall clock times. The pairing heap consistently produced the highest wall clock times. While the Fibonacci heap fell between the two, never the fastest. This ordering held true for both Dihstra and Prim and did not change with graph density or structure. 
Operation counts showed a different pattern for the heaps that were being viewed. Pairing heaps executed fewer total operations than both the binary and Fibonacci heap in every configuration tested. While Binary and Fibonacci heaps showed nearly identical operations counts, which only differed by a small inconceivable margin(often within a few percent). Lower operation counts for painting did not translate into faster processed times. 
When looking at the increase in graph size, the time gap between binary and pairing heaps grew. This effect was visible in both sparse and dense random graphs, as well as layered graphs that were designed to stress the decreased key behavior. Grid graphs also showed lower tuntimes for all heaps due to the bounded degree, but overall performance did remain similar. 

- **Correctness:** All three heaps produced identical Dijkstra distances and identical Prim MST total weights on every run; no correctness warnings.

---

## 5. Comparative Discussion

**Do Fibonacci heaps provide practical benefits?**  
Throughout each instance of this project, Fibonacci heaps did not seem to provide any practical performance benefit. Even though they were theorized that amortized bounds are better for decreasing key operations, their measured run times in our practice were consistently higher than those of binary heaps. The type of reduction that the Fibonacci heaps are designed to produce was cheaper in theory, but because we did not look at graphs large enough for the asymptotic operations advantage to dominate, we did not see that in our study.
**How do pairing heaps compare in practice?**  
Pairing heaps reduced the amount of heap operations more than the alternative heaps of binary or fibonacci. In practice, this suggests that their structure that is self adjusting does limit their restructuring work at an algorithmic level. But, for each individual operation that it does iterate through it appears to be more expensive. With pointer traversal, repeated merges, and its weaker memory locality, in this practice it contributed to its higher run time.

**Which algorithm benefits more from advanced heaps?**  
Neither of the algorithms showed/ produced a more meaningful runtime than the other when using the Fibonacci or pairing heap instead of a binary heap. Differences between these two algorithms were modest, meaning prim triggered more key decrease operations and did not change the relative ranking of heap performance.

**How does graph structure affect performance?**  
Graph structure influenced absolute cost but not relative outcomes. The dense graphs increased the overall total work off the heaps. Grid graphs focused on reducing the work due to its bounded degree. Layered graphs increased the decrease in key frequency. But in all cases binary heaps still remained fastest. The pairing heaps the slowest, and Fibonacci in the middle.


**Why theory and practice differ**  
Why theory and practice differ
When comparing theory and practice, the results suggest a gap between asymptotic guarantees and observed performance at the tested problem sizes. Theoretical models do not account for many factors that affect instruction level cost, including cache behavior, memory access patterns, and pointer overhead. In this setting, simpler data structures with predictable access patterns appeared to be more effective, even when they performed more abstract operations.

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
