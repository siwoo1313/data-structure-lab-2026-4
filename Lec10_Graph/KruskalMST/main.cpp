#include <iostream>
#include <cstdio>
#include "WGraph.h"

int main()
{
    WGraphMST g;
    g.load("graph.txt");

    printf("MST By Kruskal's Algorithm\n");
    g.Kruskal();

    return 0;
}