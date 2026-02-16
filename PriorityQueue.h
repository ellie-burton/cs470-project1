#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <cstddef>

// abstract base so dijkstra/prim can take any heap impl
template <typename T>
class PriorityQueue {
public:
    virtual ~PriorityQueue() {}

    virtual void insert(int key, T value) = 0;
    // true if extracted, false if empty
    virtual bool extractMin(int &outKey, T &outValue) = 0;
    // need to find node by value (we use vertex id)
    virtual void decreaseKey(T value, int newKey) = 0;
    virtual bool isEmpty() const = 0;
    virtual long getOperationCount() const = 0;
};

#endif
