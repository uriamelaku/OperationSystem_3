#include "reactor.hpp"
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
#include <csignal>

using namespace std;

// Global variables for graph representation
int n = 0, m = 0;
deque<deque<int>> adj;
deque<deque<int>> adjT;

// Function to initialize a new graph
void Newgraph(int numVertices, int numEdges) {
    n = numVertices;
    m = numEdges;

    adj.clear();
    adj.resize(n);
    adjT.clear();
    adjT.resize(n);
    cout << "New graph created with " << numVertices << " vertices and " << numEdges << " edges." << endl;
}

// Function to add a new edge
void Newedge(int u, int v) {
    adj[u].push_back(v);
    adjT[v].push_back(u);
    cout << "Edge added: " << u << " -> " << v << endl;
}

// Function to remove an edge
void Removeedge(int u, int v) {
    auto it = find(adj[u].begin(), adj[u].end(), v);
    if (it != adj[u].end()) {
        adj[u].erase(it);
    }

    it = find(adjT[v].begin(), adjT[v].end(), u);
    if (it != adjT[v].end()) {
        adjT[v].erase(it);
    }
    cout << "Edge removed: " << u << " -> " << v << endl;
}

// Kosaraju's algorithm function
void Kosaraju(int client_fd) {
    if (n <= 0 || m <= 0 || m > 2 * n) {
        string msg = "Invalid input\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
        return;
    }

    vector<bool> visited(n, false);
    deque<int> order;

    function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        for (int v : adj[u]) {
            if (!visited[v]) {
                dfs1(v);
            }
        }
        order.push_front(u);
    };

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            dfs1(i);
        }
    }

    vector<int> component(n, -1);
    deque<deque<int>> components; // To store the nodes of each component

    function<void(int, int)> dfs2 = [&](int u, int comp) {
        component[u] = comp;
        components[comp].push_back(u);
        for (int v : adjT[u]) {
            if (component[v] == -1) {
                dfs2(v, comp);
            }
        }
    };

    int comp = 0;
    for (int u : order) {
        if (component[u] == -1) {
            components.push_back(deque<int>()); // Add a new component
            dfs2(u, comp++);
        }
    }

    string result = "Number of strongly connected components: " + to_string(comp) + "\n";
    for (int i = 0; i < comp; ++i) {
        result += "Component " + to_string(i + 1) + ": ";
        for (int node : components[i]) {
            result += to_string(node) + " ";
        }
        result += "\n";
    }

    send(client_fd, result.c_str(), result.size(), 0);
}

void handle_client(int client_fd) {
    char buf[1024];
    int numbytes;
    string command;
    stringstream ss;

    while ((numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0)) > 0 ) {
        buf[numbytes] = '\0';
        command.append(buf);
        ss.clear();
        ss.str(command);
        string cmd;
        ss >> cmd;

        if (cmd == "Newgraph") {
            int numVertices, numEdges;
            ss >> numVertices >> numEdges;
            Newgraph(numVertices, numEdges);

            string msg = "Enter " + to_string(numEdges) + " edges:\n";
            send(client_fd, msg.c_str(), msg.size(), 0);

            for (int i = 0; i < numEdges; ++i) {
                numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
                if (numbytes <= 0) {
                    cerr << "Error reading edge input" << endl;
                    return;
                }
                buf[numbytes] = '\0';
                stringstream edge_ss(buf);
                int u, v;
                edge_ss >> u >> v;
                Newedge(u, v);
            }

            msg = "Graph created with " + to_string(numVertices) + " vertices and " + to_string(numEdges) + " edges.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            break;
        } else if (cmd == "Kosaraju") {
            Kosaraju(client_fd);
            break;
        } else if (cmd == "Newedge") {
            int u, v;
            ss >> u >> v;
            Newedge(u, v);
            string msg = "Edge " + to_string(u) + " " + to_string(v) + " added.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            break;
        } else if (cmd == "Removeedge") {
            int u, v;
            ss >> u >> v;
            Removeedge(u, v);
            string msg = "Edge " + to_string(u) + " " + to_string(v) + " removed.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            break;
        } else if (cmd == "Exit") {
            close(client_fd);
            cout << "Closing client: " << client_fd << endl;
            break;
        } else {
            string msg = "Invalid command\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        }

        command.clear();
    }
}

int main() {
    int listener;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;
    const int PORT = 9034;

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }
    cout << "Server is running on port " << PORT << endl;

    Reactor reactor;
    reactor.addFd(listener, [&](int fd) {
        addrlen = sizeof(clientaddr);
        int newfd = accept(fd, (struct sockaddr*)&clientaddr, &addrlen);
        if (newfd == -1) {
            perror("accept");
            return;
        }

        cout << "New connection from " << inet_ntoa(clientaddr.sin_addr) << " on socket " << newfd << endl;
        reactor.addFd(newfd, handle_client);
    });

    reactor.run();

    return 0;
}
