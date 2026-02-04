Below is a prioritized, detailed ticket list for completion of the project. These tasks are structured with explicit **Context**, **Implementation Details**, and **Verification Steps** (Acceptance Criteria) to ensure the code compiles and meets the project's modularity requirements.

### Phase 1: Core Infrastructure & Interface

**Context:** We must establish the base classes before implementing specific complex logic. The `PriorityQueue` interface must be generic to support swapping heaps for the comparative analysis.

#### **Ticket 1.1: Define Abstract Base Class (`PriorityQueue.h`)**

* **Description:** Create a pure abstract template class `PriorityQueue<T>` that defines the contract for both Fibonacci and Pairing heaps.
* **Requirements:**
* Define virtual destructor `~PriorityQueue()`.
* Define pure virtual methods:
* `void insert(int key, T value)`
* `bool extractMin(int &outKey, T &outValue)`
* `bool isEmpty() const`
* `void decreaseKey(T value, int newKey)` — **Critical:** This function signatures relies on `value` (the vertex ID) being the lookup key, not a node pointer.
* 
`long getOperationCount() const` — For Part 4 Metrics.






* **Accuracy Check:** Code compiles. A dummy class inheriting from this can override all functions.

#### **Ticket 1.2: Implement Graph Class (`Graph.h`)**


* **Description:** Implement a weighted graph using an adjacency list with `std::vector` (per library constraints ).


* **Requirements:** 
* Structure `Edge` containing `{int target, int weight}`.
* Class `Graph` containing `std::vector<std::vector<Edge>> adjList`.
* Method `void addEdge(int u, int v, int weight)` that pushes to the vector.
* Constructor `Graph(int numVertices)` that resizes the vector.


* **Accuracy Check:** Create a Graph(5), add edge 0->1 with weight 10. Verify `adjList[0][0]` contains `{1, 10}`.

---

### Phase 2: Pairing Heap (The "Practice" Heap)

**Context:** Pairing heaps are "easier to implement in practice"  but require a specific "Two-Pass Merge" strategy for `extract-min`.

#### **Ticket 2.1: Pairing Heap Structure & Insert (`PairingHeap.h`)**

* **Description:** Define the `Node` struct and the `insert` logic.
* **Data Structure:**
* `struct Node`: `int key`, `T value`, `Node* child`, `Node* sibling`, `Node* prev` (for back-linking).
* Member `std::vector<Node*> nodeMapping`: Resized to graph size `N`. Maps `VertexID -> Node*` for O(1) lookup during decrease-key.




* **Logic:**
* `insert(key, value)`: Create node, save to `nodeMapping[value]`, call `merge(root, newNode)`.
* `merge(Node* A, Node* B)`: Compare keys. Make the larger root the *first child* of the smaller root.


* **Accuracy Check:** Insert 3 items. Verify `root` points to the min. Verify `nodeMapping` has pointers for all 3 values.

#### **Ticket 2.2: Pairing Heap Extract-Min & Decrease-Key**

* **Description:** Implement the "Two-Pass Merge" for extraction and cut logic for decrease-key.
* **Logic (`extractMin`):**
* Remove root.
* **Pass 1 (Left-to-Right):** Merge children in pairs (`child[0]` & `child[1]`, `child[2]` & `child[3]`, ...).
* **Pass 2 (Right-to-Left):** Merge the resulting trees from right to left into a single tree.


* **Logic (`decreaseKey`):**
* Lookup node using `nodeMapping`.
* If `newKey < parent->key`: Detach node from its siblings/parent (Standard pointer manipulation).
* `merge` the detached node with the root.


* **Accuracy Check:** Insert 10, 20, 30. Extract min (10). Verify new root is 20. Decrease 30 to 5. Verify root becomes 5.

---

### Phase 3: Fibonacci Heap (The "Advanced" Heap)

**Context:** Fibonacci Heaps utilize a circular doubly linked list for the root list and "lazy" operations. They require "marked" nodes to track history for cascading cuts.

#### **Ticket 3.1: Fibonacci Heap Structure & Insert (`FibonacciHeap.h`)**

* **Description:** Define the complex Node struct and lazy insert.
* **Data Structure:**
* 
`struct Node`: `parent`, `child`, `left`, `right`, `degree` (int), `marked` (bool).


* Roots are stored in a **circular doubly linked list**.


* `minNode` pointer always points to the minimum root.


* **Logic (`insert`):**
* Create node. Splice it into the circular root list next to `minNode`.
* Update `minNode` if `newNode->key < minNode->key`.
* Time complexity must be O(1).




* **Accuracy Check:** Insert 5 nodes. Traverse `minNode->right` 5 times; ensure you return to `minNode`.

#### **Ticket 3.2: Fibonacci Heap Extract-Min (Consolidate)**

* **Description:** The cleanup phase. When min is removed, trees of equal degree must be merged.
* **Logic:**
* Remove `minNode`. Promote all its children to the root list.
* **Consolidate:**
* Create an array `A[0..D]` (where D is approx `2*log(N)`).
* Iterate through root list. If `A[degree]` is occupied, **Link** the two trees (make larger root child of smaller). Clear `A[degree]` and continue with the merged tree until a slot is empty.




* Rebuild root list from `A`. Update `minNode`.


* **Accuracy Check:** Insert elements causing degrees 0, 0, 1. Call `extractMin`. Ensure result has no duplicate degrees in root list.

#### **Ticket 3.3: Fibonacci Heap Decrease-Key (Cascading Cut)**

* **Description:** The critical operation for Dijkstra’s speed.
* **Logic:**
* Lookup node via `nodeMapping`. Check if `newKey < parent->key`.
* **Cut:** Detach node from parent, add to root list, set `marked = false`.
* **Cascading Cut:** If parent is NOT marked, mark it. If parent IS marked, `Cut` the parent and recurse up.




* **Accuracy Check:** Trigger a cascading cut (decrease a grandchild node such that its parent and grandparent are affected). Verify multiple nodes move to root list.

---

### Phase 4: Algorithms & Integration

**Context:** Implement the algorithms specifically to utilize the modular `PriorityQueue` interface so heaps can be swapped instantly.

#### **Ticket 4.1: Generic Dijkstra Implementation (`Algorithms.h`)**

* **Description:** Implement Dijkstra's algorithm accepting a `PriorityQueue*`.
* **Logic:**
* Initialize `dist[]` to infinity, `visited[]` to false.
* `pq->insert(0, startNode)`.
* While PQ not empty:
* `extractMin(d, u)`.
* For each neighbor `v` of `u`:
* If `newDist < dist[v]`:
* `dist[v] = newDist`.
* If `v` was already seen, call `pq->decreaseKey(v, newDist)`.
* Else, call `pq->insert(newDist, v)`.








* **Accuracy Check:** Run on a known 5-node graph. Compare `dist[]` output against manual calculation.

#### **Ticket 4.2: Prim's Algorithm (`Algorithms.h`)**

* **Description:** Implement Prim's MST algorithm.
* 
**Logic:** Similar to Dijkstra, but `dist[v]` represents the weight of the cheapest edge connecting `v` to the MST, not the cumulative path length.


* **Accuracy Check:** Run on a graph where the MST is obvious (e.g., a line or ring). Verify total weight.

---

### Phase 5: Experiments & Reporting

**Context:** Generate data for the report.

#### **Ticket 5.1: Experiment Driver (`main.cpp`)**

* **Description:** Create a harness to run head-to-head comparisons.
* **Requirements:**
* Generate a **Dense Graph** (Edges ≈ V^2) to stress `decrease-key`.
* Generate a **Sparse Graph** (Edges ≈ V) to favor standard operations.
* Run `Dijkstra(Pairing)` vs `Dijkstra(Fibonacci)`.
* Record: Runtime (ms), Total Heap Operations (`getOperationCount`).


* **Accuracy Check:** Outputs a CSV-like format to console: `Algo, HeapType, GraphSize, TimeMS, Ops`.