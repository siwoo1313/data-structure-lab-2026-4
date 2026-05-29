#pragma once
#include "HeapNode.h"
#define MAX_VTXS 100

class MinHeap {
    HeapNode node[MAX_VTXS * MAX_VTXS];
    int size;
public:
    MinHeap() : size(0) {}
    bool isFull() { return size >= (MAX_VTXS * MAX_VTXS - 1); }
    bool isEmpty() { return size == 0; }

    HeapNode getParent(int i) { return node[i / 2]; }

    void insert(int key, int u, int v) {
        if (isFull()) return;
        int i = ++size;
        while (i != 1 && key < getParent(i).getKey()) {
            node[i] = node[i / 2];
            i /= 2;
        }
        node[i].setKey(key, u, v);
    }

    HeapNode remove() {
        if (isEmpty()) return HeapNode();
        HeapNode item = node[1];
        HeapNode last = node[size--];
        int parentIdx = 1;
        int childIdx = 2;
        while (childIdx <= size) {
            if (childIdx < size && node[childIdx].getKey() > node[childIdx + 1].getKey()) {
                childIdx++;
            }
            if (last.getKey() <= node[childIdx].getKey()) break;
            node[parentIdx] = node[childIdx];
            parentIdx = childIdx;
            childIdx *= 2;
        }
        node[parentIdx] = last;
        return item;
    }
};
