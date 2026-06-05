#pragma once
#include "WGraph.h"

#ifndef INF
#define INF 999999
#endif

// 기존 WGraphMST를 상속받아 Prim 알고리즘까지 통합한 클래스 정의
class WGraphPrim : public WGraphMST {
private:
    // MST에 포함되지 않은 정점들 중에서 dist가 최소인 정점 선택
    int getMinVertex(bool* selected, int* dist) {
        int minv = 0;
        int mindist = INF;
        for (int v = 0; v < size; v++) {
            if (!selected[v] && dist[v] < mindist) {
                mindist = dist[v];
                minv = v;
            }
        }
        return minv;
    }

    // WGraph의 adj[][]가 간선 없을 때 0을 가지므로, 
    // 이미지 코드의 '!= INF' 판단을 위해 래핑 함수 작성
    int getEdgeWeight(int u, int v) {
        int weight = getEdge(u, v);
        if (weight == 0) return INF; // 간선이 없으면 INF 반환
        return weight;
    }

public:
    // Prim의 MST 알고리즘 구현
    void Prim(int s) {
        bool selected[MAX_VTXS]; // 정점이 이미 포함되었는가?
        int dist[MAX_VTXS];      // 거리

        // 배열 초기화
        for (int i = 0; i < size; i++) {
            dist[i] = INF;
            selected[i] = false;
        }

        dist[s] = 0; // 시작 정점

        for (int i = 0; i < size; i++) {
            int u = getMinVertex(selected, dist);
            selected[u] = true;

            if (dist[u] == INF) return;
            printf("%c ", getVertex(u));

            for (int v = 0; v < size; v++) {
                if (getEdgeWeight(u, v) != INF) {
                    if (!selected[v] && getEdgeWeight(u, v) < dist[v]) {
                        dist[v] = getEdgeWeight(u, v);
                    }
                }
            }
        }
        printf("\n");
    }
};