#pragma once
#include <cstdio>
#include "MinHeap.h"
#include "VertexSets.h"

#define MAX_VTXS 100

class WGraph {
protected:
    int size;
    char vertices[MAX_VTXS];
    int adj[MAX_VTXS][MAX_VTXS];
public:
    WGraph() : size(0) {
        for (int i = 0; i < MAX_VTXS; i++)
            for (int j = 0; j < MAX_VTXS; j++)
                adj[i][j] = 0;
    }
    char getVertex(int i) { return vertices[i]; }
    bool hasEdge(int i, int j) { return adj[i][j] > 0; }
    int getEdge(int i, int j) { return adj[i][j]; }

    void load(const char* filename) {
        (void)filename;
        size = 7;
        const char* names = "ABCDEFG";
        for (int i = 0; i < size; i++) vertices[i] = names[i];

        adj[0][5] = 10; adj[5][0] = 10; // A - F (10)
        adj[2][3] = 12; adj[3][2] = 12; // C - D (12)
        adj[1][6] = 15; adj[6][1] = 15; // B - G (15)
        adj[1][2] = 16; adj[2][1] = 16; // B - C (16)
        adj[3][4] = 22; adj[4][3] = 22; // D - E (22)
        adj[4][5] = 27; adj[5][4] = 27; // E - F (27)

        adj[0][1] = 30; adj[1][0] = 30; // A - B (30)
        adj[4][6] = 35; adj[6][4] = 35; // E - G (35)
    }
};

class WGraphMST : public WGraph {
public:
    void Kruskal() {
        MinHeap heap;
        for (int i = 0; i < size - 1; i++)
            for (int j = i + 1; j < size; j++)
                if (hasEdge(i, j))
                    heap.insert(getEdge(i, j), i, j);

        VertexSets set(size);
        int edgeAccepted = 0;

        while (edgeAccepted < size - 1) {
            HeapNode e = heap.remove();
            int uset = set.findSet(e.getV1());
            int vset = set.findSet(e.getV2());
            if (uset != vset) {
                printf("간선 추가 : %c - %c (비용:%d)\n",
                    getVertex(e.getV1()), getVertex(e.getV2()), e.getKey());
                set.unionSets(uset, vset);
                edgeAccepted++;
            }
        }
    }
};