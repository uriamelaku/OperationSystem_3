#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <functional>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <sys/select.h>

// Function declarations
void Newgraph(int numVertices, int numEdges);
void Newedge(int u, int v);
void Removeedge(int u, int v);
void Kosaraju(int client_fd);
void *handle_client_proactor(int client_fd);


#endif // SERVER_HPP