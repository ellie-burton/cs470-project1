#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include "PriorityQueue.h"
#include <cmath>
#include <vector>

template <typename T>
class FibonacciHeap : public PriorityQueue<T> {
private:
    struct Node {
        int key;
        T value;
        int degree;
        bool marked;

        Node* parent;
        Node* child;
        Node* left;
        Node* right;

        Node(int k, T v) : key(k), value(v), degree(0), marked(false),
                           parent(nullptr), child(nullptr), left(this), right(this) {}
    };

    Node* minNode;
    int n;
    std::vector<Node*> nodeMapping;
    long opCount;

    void addToRootList(Node* node) {
        if (!minNode) {
            minNode = node;
            node->left = node->right = node;
            node->parent = nullptr;
            return;
        }
        node->left = minNode;
        node->right = minNode->right;
        minNode->right->left = node;
        minNode->right = node;
        node->parent = nullptr;
    }

    void removeFromList(Node* node) {
        node->left->right = node->right;
        node->right->left = node->left;
    }

    void link(Node* y, Node* x) {
        removeFromList(y);
        if (x->child) {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        } else {
            x->child = y;
            y->left = y->right = y;
        }
        y->parent = x;
        x->degree++;
        y->marked = false;
    }

    void consolidate() {
        if (!minNode) return;
        int maxDegree = static_cast<int>(2 * std::log2(n + 1)) + 2;
        std::vector<Node*> A(static_cast<size_t>(maxDegree), nullptr);

        Node* start = minNode;
        std::vector<Node*> roots;
        Node* p = start;
        int count = 0;
        do {
            roots.push_back(p);
            p = p->right;
            if (++count > n + 10) break;  // safety: prevent infinite loop on corrupt list
        } while (p != start);

        for (Node* w : roots) {
            Node* x = w;
            int d = x->degree;
            while (true) {
                if (static_cast<size_t>(d) >= A.size())
                    A.resize(A.size() * 2 + 1, nullptr);
                if (!A[d]) break;
                Node* y = A[d];
                if (x->key > y->key) std::swap(x, y);
                link(y, x);
                A[d] = nullptr;
                d++;
            }
            if (static_cast<size_t>(d) >= A.size())
                A.resize(A.size() * 2 + 1, nullptr);
            A[d] = x;
        }

        minNode = nullptr;
        for (Node* node : A) {
            if (node) {
                node->left = node->right = node;
                node->parent = nullptr;
                if (!minNode) {
                    minNode = node;
                } else {
                    addToRootList(node);
                    if (node->key < minNode->key)
                        minNode = node;
                }
            }
        }
    }

    void cut(Node* x, Node* y) {
        if (x->right == x)
            y->child = nullptr;
        else {
            removeFromList(x);
            if (y->child == x)
                y->child = x->right;
        }
        y->degree--;
        addToRootList(x);
        x->marked = false;
    }

    void cascadingCut(Node* y) {
        Node* z = y->parent;
        if (!z) return;
        if (!y->marked) {
            y->marked = true;
        } else {
            cut(y, z);
            cascadingCut(z);
        }
    }

    void cascadingCutIterative(Node* y) {
        for (Node* z = y->parent; z; z = y->parent) {
            if (!y->marked) {
                y->marked = true;
                return;
            }
            cut(y, z);
            y = z;
        }
    }

public:
    FibonacciHeap() : minNode(nullptr), n(0), opCount(0) {}
    explicit FibonacciHeap(int maxVertices) : minNode(nullptr), n(0), opCount(0) {
        nodeMapping.resize(maxVertices, nullptr);
    }

    void insert(int key, T value) override {
        Node* newNode = new Node(key, value);
        if (!nodeMapping.empty() && value >= 0 && value < static_cast<int>(nodeMapping.size()))
            nodeMapping[value] = newNode;
        addToRootList(newNode);
        if (!minNode || key < minNode->key)
            minNode = newNode;
        n++;
        opCount++;
    }

    bool extractMin(int &outKey, T &outValue) override {
        if (!minNode) return false;
        outKey = minNode->key;
        outValue = minNode->value;
        if (!nodeMapping.empty() && outValue >= 0 && outValue < static_cast<int>(nodeMapping.size()))
            nodeMapping[outValue] = nullptr;

        if (minNode->child) {
            Node* c = minNode->child;
            do {
                Node* next = c->right;
                addToRootList(c);
                c = next;
            } while (c != minNode->child);
            minNode->child = nullptr;
        }

        Node* oldMin = minNode;
        if (minNode->right == minNode) {
            minNode = nullptr;
        } else {
            removeFromList(minNode);
            minNode = oldMin->right;
            consolidate();
        }
        delete oldMin;
        n--;
        opCount++;
        return true;
    }

    void decreaseKey(T value, int newKey) override {
        if (nodeMapping.empty() || value < 0 || value >= static_cast<int>(nodeMapping.size())) return;
        Node* node = nodeMapping[value];
        if (!node) return;
        if (newKey >= node->key) return;

        node->key = newKey;
        Node* parent = node->parent;
        if (parent && newKey < parent->key) {
            cut(node, parent);
            cascadingCutIterative(parent);
        }
        if (newKey < minNode->key)
            minNode = node;
        opCount++;
    }

    bool isEmpty() const override { return minNode == nullptr; }
    long getOperationCount() const override { return opCount; }
};

#endif
