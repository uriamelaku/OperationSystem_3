#include "Kosaraju.hpp"
#include <iostream>
#include <list>
#include <algorithm>
#include <functional>
#include <vector>
#include <deque>

using namespace std;


void Kosaraju() {
    int vertices, edges;
    cout << "Enter total vertices and edges: ";
    cin >> vertices >> edges;

    if (vertices <= 0 || edges <= 0 || edges > 2 * vertices) {
        cout << "Input is not valid." << endl;
        return;
    }

    // Changed data structure from vector to deque for adjacency lists
    vector<deque<int>> graph(vertices), transposedGraph(vertices);
    cout << "Provide " << edges << " edges (u v format):" << endl;

    int from, to;
    for (int i = 0; i < edges; ++i) {
        cin >> from >> to;
        if (from < 0 || to < 0 || from >= vertices || to >= vertices) {
            cout << "Edge index out of bounds. Valid vertices: 0 to " << vertices - 1 << ". Try again." << endl;
            --i;
        } else {
            graph[from].push_back(to);
            transposedGraph[to].push_back(from);
            cout << "Added edge " << from << " -> " << to << endl;
        }
    }

    vector<bool> explored(vertices, false);
    vector<int> topoSort;

    // Renamed DFS function to traverse
    function<void(int)> traverse = [&](int node) {
        explored[node] = true;
        for (int neighbor : graph[node]) {
            if (!explored[neighbor]) {
                traverse(neighbor);
            }
        }
        topoSort.push_back(node);
    };

    // Run DFS for each unvisited node
    for (int i = 0; i < vertices; ++i) {
        if (!explored[i]) {
            traverse(i);
        }
    }

    reverse(topoSort.begin(), topoSort.end());
    vector<int> nodeComponent(vertices, -1);
    vector<deque<int>> scc; // Stores each strongly connected component

    // Another DFS function for SCC, renamed
    function<void(int, int)> assignComponent = [&](int node, int id) {
        nodeComponent[node] = id;
        scc[id].push_back(node);
        for (int neighbor : transposedGraph[node]) {
            if (nodeComponent[neighbor] == -1) {
                assignComponent(neighbor, id);
            }
        }
    };

    int id = 0;
    for (int node : topoSort) {
        if (nodeComponent[node] == -1) {
            scc.push_back(deque<int>()); // Initialize new SCC component
            assignComponent(node, id++);
        }
    }

    cout << "Total strongly connected components: " << id << endl;
    for (int i = 0; i < id; ++i) {
        cout << "SCC " << i + 1 << ": ";
        for (int element : scc[i]) {
            cout << element << " ";
        }
        cout << endl;
    }
}
