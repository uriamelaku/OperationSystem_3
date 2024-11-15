#include <iostream>
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <thread>
#include <mutex>
#include "proactor.hpp"

using namespace std;

int n = 0, m = 0;
deque<deque<int>> adj;
deque<deque<int>> adjT;
std::mutex graph_mutex;

void Newgraph(int numVertices, int numEdges) {
    adj.clear();
    adj.resize(numVertices);
    adjT.clear();
    adjT.resize(numVertices);
    n = numVertices;
    m = numEdges;
    cout << "New graph created with " << numVertices << " vertices and " << numEdges << " edges." << endl;
}

void Newedge(int u, int v) {
    adj[u].push_back(v);
    adjT[v].push_back(u);
    cout << "Edge added: " << u << " -> " << v << endl;
}

void Removeedge(int u, int v) {
    auto it = find(adj[u].begin(), adj[u].end(), v);
    if (it != adj[u].end()) {
        adj[u].erase(it);
        cout << "Edge removed: " << u << " -> " << v << endl;
    }

    it = find(adjT[v].begin(), adjT[v].end(), u);
    if (it != adjT[v].end()) {
        adjT[v].erase(it);
    }
}

void Kosaraju(int client_fd) {
    if (n <= 0 || m <= 0 || m > 2 * n) {
        string msg = "Invalid input\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
        return;
    }

    vector<bool> visited(n, false);
    deque<int> order;

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

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) dfs1(i);
    }

    vector<int> component(n, -1);
    deque<deque<int>> components;
    int comp = 0;

    // Second DFS on the transposed graph
    std::function<void(int)> dfs2 = [&](int u) {
        component[u] = comp;
        components[comp].push_back(u);
        for (int v : adjT[u]) {
            if (component[v] == -1) dfs2(v);
        }
    };

    for (int u : order) {
        if (component[u] == -1) {
            components.push_back(deque<int>());
            dfs2(u);
            comp++;
        }
    }

    string result = "Number of strongly connected components: " + to_string(comp) + "\n";
    for (int i = 0; i < comp; ++i) {
        result += "Component " + to_string(i + 1) + ": ";
        for (int node : components[i]) result += to_string(node) + " ";
        result += "\n";
    }

    send(client_fd, result.c_str(), result.size(), 0);
    cout << "Kosaraju's algorithm executed. Result sent to client." << endl;
}

void *handle_client(int client_fd) {
    cout << "check client_fd " << client_fd << endl;
    char buf[1024];
    int numbytes;

    while ((numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[numbytes] = '\0';
        stringstream ss(buf);
        string cmd;
        ss >> cmd;

        string msg;
        if (cmd == "Newgraph") {
            int numVertices, numEdges;
            ss >> numVertices >> numEdges;
            {
                lock_guard<mutex> lock(graph_mutex);
                Newgraph(numVertices, numEdges);
            }
            msg = "Enter " + to_string(numEdges) + " edges:\n";
            send(client_fd, msg.c_str(), msg.size(), 0);

            for (int i = 0; i < numEdges; ++i) {
                numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
                if (numbytes <= 0) break;

                buf[numbytes] = '\0';
                stringstream edge_ss(buf);
                int u, v;
                edge_ss >> u >> v;
                Newedge(u, v);
            }

            msg = "Graph created with " + to_string(numVertices) + " vertices and " + to_string(numEdges) + " edges.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Kosaraju") {
            lock_guard<mutex> lock(graph_mutex);
            Kosaraju(client_fd);
        } else if (cmd == "Newedge") {
            int u, v;
            ss >> u >> v;
            {
                lock_guard<mutex> lock(graph_mutex);
                Newedge(u, v);
            }
            msg = "Edge " + to_string(u) + " " + to_string(v) + " added.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Removeedge") {
            int u, v;
            ss >> u >> v;
            {
                lock_guard<mutex> lock(graph_mutex);
                Removeedge(u, v);
            }
            msg = "Edge " + to_string(u) + " " + to_string(v) + " removed.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Exit") {
            msg = "Goodbye!\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            close(client_fd);
            break;
        } else {
            msg = "Invalid command\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        }
    }

    close(client_fd);
    return nullptr;
}

int main() {
    int listener;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(9034);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    cout << "Server is running on port 9034" << endl;

    while (true) {
        addrlen = sizeof(clientaddr);
        int client_fd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        cout << "New connection from " << inet_ntoa(clientaddr.sin_addr) << " on socket " << client_fd << endl;
        pthread_t client_thread = startProactor(client_fd, handle_client);
        if (client_thread == 0) {
            perror("startProactor");
            close(client_fd);
        }
    }

    close(listener);
    return 0;
}
