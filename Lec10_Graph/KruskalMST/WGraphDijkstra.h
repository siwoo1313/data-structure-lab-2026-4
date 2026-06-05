#pragma once
#include <cstdio>
#include "WGraph.h"

// 교재 예시 화면의 무한대 값인 9999로 동기화합니다.
#ifndef INF
#define INF 9999
#endif

class WGraphDijkstra : public WGraphMST {
    int dist[MAX_VTXS];
    bool found[MAX_VTXS];

private:
    int chooseVertex() {
        int min = INF;
        int minpos = -1;
        for (int i = 0; i < size; i++) {
            if (dist[i] < min && !found[i]) {
                min = dist[i];
                minpos = i;
            }
        }
        return minpos;
    }

    void printDistance() {
        for (int i = 0; i < size; i++) {
            if (dist[i] == INF)
                printf("%5d", 9999); // 교재 화면처럼 INF 대신 9999로 출력
            else
                printf("%5d", dist[i]);
        }
        printf("\n");
    }

    int getEdgeWeight(int u, int w) {
        if (u == w) return 0;
        int weight = getEdge(u, w);
        if (weight == 0) return INF;
        return weight;
    }

public:
    // 교재의 최단 경로 그래프 데이터를 강제로 주입하는 전용 load 함수 추가
    void load_sp() {
        size = 7;
        const char* names = "ABCDEFG";
        for (int i = 0; i < size; i++) vertices[i] = names[i];

        // 인접 행렬 초기화 (자기 자신 외에는 모두 0)
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                adj[i][j] = 0;
            }
        }

        // 교재 다익스트라 전용 그래프 가중치 데이터 설정 (방향/무방향 여부에 맞춰 세팅)
        // 정점 인덱스: 0:A, 1:B, 2:C, 3:D, 4:E, 5:F, 6:G
        adj[0][1] = 7;  adj[1][0] = 7;  // A - B (7)
        adj[0][4] = 3;  adj[4][0] = 3;  // A - E (3)
        adj[0][5] = 10; adj[5][0] = 10; // A - F (10)

        adj[1][2] = 4;  adj[2][1] = 4;  // B - C (4)
        adj[1][4] = 2;  adj[4][1] = 2;  // B - E (2)

        adj[2][3] = 2;  adj[3][2] = 2;  // C - D (2)

        adj[3][4] = 11; adj[4][3] = 11; // D - E (11)
        adj[3][5] = 9;  adj[5][3] = 9;  // D - F (9)
        adj[3][6] = 4;  adj[6][3] = 4;  // D - G (4)

        adj[4][5] = 5;  adj[5][4] = 5;  // E - F (5)

        adj[5][6] = 6;  adj[6][5] = 6;  // F - G (6)
    }

    void ShortestPath(int start) {
        for (int i = 0; i < size; i++) {
            dist[i] = getEdgeWeight(start, i);
            found[i] = false;
        }

        found[start] = true;
        dist[start] = 0;

        for (int i = 0; i < size; i++) {
            printf("Step %d:\t", i + 1); // 교재 화면 출력 포맷 조정
            printDistance();

            int u = chooseVertex();
            if (u == -1) break;

            found[u] = true;

            for (int w = 0; w < size; w++) {
                if (found[w] == false) {
                    int edge_u_w = getEdgeWeight(u, w);
                    if (dist[u] + edge_u_w < dist[w]) {
                        dist[w] = dist[u] + edge_u_w;
                    }
                }
            }
        }
    }
};