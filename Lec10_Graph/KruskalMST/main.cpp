#include <iostream>
#include <cstdio>
#include "WGraphPrim.h"
#include "WGraphDijkstra.h"

int main()
{
    WGraphPrim g; // WGraphMST를 확장한 WGraphPrim 클래스 사용
    g.load("graph.txt");

    printf("MST By Kruskal's Algorithm\n");
    g.Kruskal(); // 기존 Kruskal도 그대로 사용 가능합니다.
    printf("\n");

    printf("MST By Prim's Algorithm (Start from A):\n");
    g.Prim(0); // 0번 정점(A)부터 시작하여 Prim 알고리즘 작동
    printf("\n");

    WGraphDijkstra d;
    d.load_sp(); // 

    printf("Shortest Path By Dijkstra Algorithm\n");
    d.ShortestPath(0);

    return 0;
}
