#include <iostream>
#include <deque>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <functional>


std::deque<std::deque<int>> adj, adjT; 
int vertices = 0, edges = 0;

void Newgraph(int n, int m) {
    vertices = n;
    edges = m;
    adj.assign(vertices, std::deque<int>());
    adjT.assign(vertices, std::deque<int>());

    for (int i = 0; i < edges; ++i) {
        int u, v;
        std::cin >> u >> v;
        adj[u].push_back(v);
        adjT[v].push_back(u);
    }
    std::cout << "Graph created with " << vertices << " vertices and " << edges << " edges.\n";
}



void KosarajuDeque(const std::deque<std::deque<int>>& adj, const std::deque<std::deque<int>>& adjT, int vertices) {
    std::vector<bool> visited(vertices, false);
    std::deque<int> order;

    // First DFS to find the finishing order
    std::function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        for (int v : adj[u]) {
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
        for (int v : adjT[u]) {
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

    std::cout << "Total strongly connected components (deque): " << comp << std::endl;
}

void KosarajuCommand() {
    KosarajuDeque(adj, adjT, vertices);
}

void Newedge(int u, int v) {
    if (u >= 0 && u < vertices && v >= 0 && v < vertices) {
        adj[u].push_back(v);
        adjT[v].push_back(u);
        std::cout << "Edge " << u << " -> " << v << " added.\n";
    } else {
        std::cout << "Invalid edge\n";
    }
}

void Removeedge(int u, int v) {
    if (u >= 0 && u < vertices && v >= 0 && v < vertices) {
        // Remove edge u -> v
        adj[u].erase(std::remove(adj[u].begin(), adj[u].end(), v), adj[u].end());
        adjT[v].erase(std::remove(adjT[v].begin(), adjT[v].end(), u), adjT[v].end());
        std::cout << "Edge " << u << " -> " << v << " removed.\n";
    } else {
        std::cout << "Invalid edge\n";
    }
}

int main() {
    std::string command;
    while (std::getline(std::cin, command)) {
        std::istringstream iss(command);
        std::string operation;
        iss >> operation;

        if (operation == "Newgraph") {
            int n, m;
            iss >> n >> m;
            Newgraph(n, m);
        } else if (operation == "Kosaraju") {
            KosarajuCommand();
        } else if (operation == "Newedge") {
            int u, v;
            iss >> u >> v;
            Newedge(u, v);
        } else if (operation == "Removeedge") {
            int u, v;
            iss >> u >> v;
            Removeedge(u, v);
        } else {
            std::cout << "Invalid command\n";
        }
    }

    return 0;
}
