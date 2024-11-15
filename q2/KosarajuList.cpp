#include "Kosaraju.hpp"
#include <iostream>
#include <list>
#include <algorithm>
#include <functional>
#include <vector>
#include <deque>

using namespace std;

void KosarajuList(const std::list<std::list<int>>& adj, const std::list<std::list<int>>& adjT, int vertices) {
    std::vector<bool> visited(vertices, false);
    std::list<int> order;

    // First DFS to find the finishing order
    std::function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        auto it = std::next(adj.begin(), u);
        for (int v : *it) {
            if (!visited[v]) {
                dfs1(v);
            }
        }
        order.push_front(u);
    };

    for (int i = 0; i < vertices; ++i) {
        if (!visited[i]) {
            dfs1(i);
        }
    }

    std::vector<int> component(vertices, -1);
    int comp = 0;

    // Second DFS on the transposed graph
    std::function<void(int)> dfs2 = [&](int u) {
        component[u] = comp;
        auto it = std::next(adjT.begin(), u);
        for (int v : *it) {
            if (component[v] == -1) {
                dfs2(v);
            }
        }
    };

    for (int u : order) {
        if (component[u] == -1) {
            dfs2(u);
            comp++;
        }
    }

    std::cout << "Total strongly connected components (list): " << comp << std::endl;
}