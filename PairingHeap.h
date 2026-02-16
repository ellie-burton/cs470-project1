#ifndef PAIRING_HEAP_H
#define PAIRING_HEAP_H

#include "PriorityQueue.h"
#include <vector>
#include <unordered_set>
using std::vector;
using std::unordered_set;

template <typename T>
class PairingHeap : public PriorityQueue<T> {
private:
    struct Node {
        int key;
        T value;
        long long seq;  // tie-break by insert order
        Node* child;
        Node* sibling;
        Node* prev;
        Node(int k, T v, long long s) : key(k), value(v), seq(s), child(nullptr), sibling(nullptr), prev(nullptr) {}
    };

    Node* root;
    vector<Node*> nodeMapping;
    long opCount;
    long long nextSeq;

    Node* merge(Node* A, Node* B) {
        if (!A) return B;
        if (!B) return A;
        if (A == B) return A;
        // smaller key wins; tie = smaller seq first
        bool A_smaller = (A->key < B->key) || (A->key == B->key && A->seq < B->seq);
        if (A_smaller) {
            B->sibling = A->child;
            if (A->child) A->child->prev = B;
            A->child = B;
            B->prev = A;
            return A;
        } else {
            A->sibling = B->child;
            if (B->child) B->child->prev = A;
            B->child = A;
            A->prev = B;
            return B;
        }
    }

    // two-pass merge of sibling list (collect, clear links, pair merge then merge right-to-left)
    Node* twoPassMerge(Node* firstSibling) {
        if (!firstSibling) return nullptr;
        if (!firstSibling->sibling) return firstSibling;

        vector<Node*> nodes;
        unordered_set<Node*> seen;
        const size_t maxNodes = 100000;
        for (Node* p = firstSibling; p && nodes.size() < maxNodes; p = p->sibling) {
            if (seen.count(p)) break;
            seen.insert(p);
            nodes.push_back(p);
        }
        for (Node* p : nodes)
            p->sibling = nullptr;

        vector<Node*> pass1;
        for (size_t i = 0; i < nodes.size(); i += 2) {
            if (i + 1 < nodes.size())
                pass1.push_back(merge(nodes[i], nodes[i + 1]));
            else
                pass1.push_back(nodes[i]);
        }

        Node* result = pass1.back();
        for (int i = static_cast<int>(pass1.size()) - 2; i >= 0; --i)
            result = merge(pass1[i], result);
        return result;
    }

public:
    PairingHeap() : root(nullptr), opCount(0), nextSeq(0) {}
    explicit PairingHeap(int maxVertices) : root(nullptr), opCount(0), nextSeq(0) {
        nodeMapping.resize(maxVertices, nullptr);
    }

    void insert(int key, T value) override {
        Node* newNode = new Node(key, value, nextSeq++);
        if (value >= 0 && value < static_cast<int>(nodeMapping.size()))
            nodeMapping[value] = newNode;
        root = merge(root, newNode);
        if (root) root->prev = nullptr;
        opCount++;
    }

    bool extractMin(int &outKey, T &outValue) override {
        if (!root) return false;
        outKey = root->key;
        outValue = root->value;
        if (outValue >= 0 && outValue < static_cast<int>(nodeMapping.size()))
            nodeMapping[outValue] = nullptr;

        Node* oldRoot = root;
        Node* first = root->child;
        if (first) {
            Node* last = first;
            while (last->sibling) last = last->sibling;
            last->sibling = root->sibling;
        } else {
            first = root->sibling;
        }
        root = first ? twoPassMerge(first) : nullptr;
        if (root) root->prev = nullptr;
        delete oldRoot;
        opCount++;
        return true;
    }

    void decreaseKey(T value, int newKey) override {
        if (value < 0 || value >= static_cast<int>(nodeMapping.size())) return;
        Node* node = nodeMapping[value];
        if (!node) return;
        if (newKey >= node->key) return;

        node->key = newKey;
        Node* parent = node->prev;
        if (!parent) return;
        if (newKey >= parent->key) return;
        // unlink node from parent's child list
        if (parent->child == node)
            parent->child = node->sibling;
        else
            node->prev->sibling = node->sibling;
        if (node->sibling)
            node->sibling->prev = node->prev;
        node->sibling = nullptr;
        node->prev = nullptr;

        root = merge(node, root);
        if (root) root->prev = nullptr;
        opCount++;
    }

    bool isEmpty() const override { return root == nullptr; }
    long getOperationCount() const override { return opCount; }

    size_t countNodes() const {
        unordered_set<Node*> seen;
        vector<Node*> stack;
        if (root) stack.push_back(root);
        while (!stack.empty()) {
            Node* p = stack.back();
            stack.pop_back();
            if (!p || seen.count(p)) continue;
            seen.insert(p);
            if (p->child) stack.push_back(p->child);
            if (p->sibling) stack.push_back(p->sibling);
        }
        return seen.size();
    }
};

#endif
