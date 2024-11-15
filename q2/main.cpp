/*

We tested the different implementations
We performed a time test to know which is more effective
We have drawn an unequivocal conclusion that DEQUE is more effective 
and therefore we will use it from now on in the rest of the exercise

*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <list>
#include <deque>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include "Kosaraju.hpp"


void generateRandomGraph(int vertices, int edges, std::vector<std::vector<int>>& adj, std::vector<std::vector<int>>& adjT) {
    adj.assign(vertices, std::vector<int>());
    adjT.assign(vertices, std::vector<int>());
    for (int i = 0; i < edges; ++i) {
        int u = rand() % vertices;
        int v = rand() % vertices;
        if (u != v) { // Avoid self-loops
            adj[u].push_back(v);
            adjT[v].push_back(u);
        }
    }
}

int main() {
    srand(time(nullptr)); // Seed random number generator
    int vertices = 100;  // Choose the number of vertices
    int edges = 300;     // Choose the number of edges

    std::vector<std::vector<int>> adj, adjT;

    // Generate a random graph
    generateRandomGraph(vertices, edges, adj, adjT);

    // Measure time for Kosaraju
    auto start = std::chrono::high_resolution_clock::now();
    Kosaraju(adj, adjT, vertices);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationKosaraju = end - start;
    std::cout << "Kosaraju (original) took: " << durationKosaraju.count() << " seconds\n";

    // Measure time for KosarajuDeque
    std::deque<std::deque<int>> dequeAdj(vertices), dequeAdjT(vertices);
    for (int i = 0; i < vertices; ++i) {
        dequeAdj[i] = std::deque<int>(adj[i].begin(), adj[i].end());
        dequeAdjT[i] = std::deque<int>(adjT[i].begin(), adjT[i].end());
    }
    start = std::chrono::high_resolution_clock::now();
    KosarajuDeque(dequeAdj, dequeAdjT, vertices);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationKosarajuDeque = end - start;
    std::cout << "Kosaraju with deque took: " << durationKosarajuDeque.count() << " seconds\n";

    // Measure time for KosarajuList
    std::list<std::list<int>> listAdj(vertices), listAdjT(vertices);
    auto adjIt = listAdj.begin();
    auto adjTIt = listAdjT.begin();
    for (int i = 0; i < vertices; ++i, ++adjIt, ++adjTIt) {
        *adjIt = std::list<int>(adj[i].begin(), adj[i].end());
        *adjTIt = std::list<int>(adjT[i].begin(), adjT[i].end());
    }
    start = std::chrono::high_resolution_clock::now();
    KosarajuList(listAdj, listAdjT, vertices);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationKosarajuList = end - start;
    std::cout << "Kosaraju with list took: " << durationKosarajuList.count() << " seconds\n";

    // Determine the most efficient method
    if (durationKosaraju < durationKosarajuDeque && durationKosaraju < durationKosarajuList) {
        std::cout << "The most efficient method was Kosaraju (original) with vector.\n";
    } else if (durationKosarajuDeque < durationKosaraju && durationKosarajuDeque < durationKosarajuList) {
        std::cout << "The most efficient method was Kosaraju with deque.\n";
    } else {
        std::cout << "The most efficient method was Kosaraju with list.\n";
    }

    return 0;
}
