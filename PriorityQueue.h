#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <vector>
#include <limits>
#include <cstddef> // for size_t

// Abstract Base Class
// Allows Dijkstra/Prim to use "PriorityQueue*" without knowing if it's Fibonacci or Pairing.
template <typename T>
class PriorityQueue {
public:
    virtual ~PriorityQueue() {}

    // Core operations required by Project Part 2 [cite: 295]
    virtual void insert(int key, T value) = 0;
    
    // Returns true if a min was extracted, false if empty
    virtual bool extractMin(int &outKey, T &outValue) = 0;
    
    // Critical for Dijkstra/Prim performance [cite: 270]
    // You need a way to look up the node inside the heap to decrease its key.
    // Hint: In your graph nodes, store a pointer/handle to the heap node.
    virtual void decreaseKey(T value, int newKey) = 0;

    virtual bool isEmpty() const = 0;
    
    // For Metrics Part 4 [cite: 317]
    virtual long getOperationCount() const = 0;
};

#endif