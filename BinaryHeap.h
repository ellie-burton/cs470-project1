#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include "PriorityQueue.h"
#include <vector>
#include <utility>

template <typename T>
class BinaryHeap : public PriorityQueue<T> {
private:
    std::vector<std::pair<int, T>> heap;  // (key, value)
    std::vector<int> position;             // position[value] = index in heap, -1 if not present
    long opCount;

    void bubbleUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[parent].first <= heap[i].first) break;
            std::swap(heap[parent], heap[i]);
            position[heap[parent].second] = parent;
            position[heap[i].second] = i;
            i = parent;
        }
    }

    void bubbleDown(int i) {
        int n = static_cast<int>(heap.size());
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;
            if (left < n && heap[left].first < heap[smallest].first)
                smallest = left;
            if (right < n && heap[right].first < heap[smallest].first)
                smallest = right;
            if (smallest == i) break;
            std::swap(heap[i], heap[smallest]);
            position[heap[i].second] = i;
            position[heap[smallest].second] = smallest;
            i = smallest;
        }
    }

public:
    explicit BinaryHeap(int maxVertices) : opCount(0) {
        position.assign(maxVertices, -1);
    }

    void insert(int key, T value) override {
        int i = static_cast<int>(heap.size());
        heap.push_back({key, value});
        position[value] = i;
        bubbleUp(i);
        opCount++;
    }

    bool extractMin(int &outKey, T &outValue) override {
        if (heap.empty()) return false;
        outKey = heap[0].first;
        outValue = heap[0].second;
        position[outValue] = -1;

        if (heap.size() == 1) {
            heap.pop_back();
        } else {
            heap[0] = heap.back();
            heap.pop_back();
            position[heap[0].second] = 0;
            bubbleDown(0);
        }
        opCount++;
        return true;
    }

    void decreaseKey(T value, int newKey) override {
        int i = position[value];
        if (i < 0) return;  // value not in heap
        if (newKey >= heap[i].first) return;
        heap[i].first = newKey;
        bubbleUp(i);
        opCount++;
    }

    bool isEmpty() const override { return heap.empty(); }
    long getOperationCount() const override { return opCount; }
};

#endif
