#ifndef KOSARAJU_HPP
#define KOSARAJU_HPP

#include <vector>
#include <deque>
#include <list>

void Kosaraju(const std::vector<std::vector<int>>& adj, const std::vector<std::vector<int>>& adjT, int vertices);
void KosarajuDeque(const std::deque<std::deque<int>>& adj, const std::deque<std::deque<int>>& adjT, int vertices);
void KosarajuList(const std::list<std::list<int>>& adj, const std::list<std::list<int>>& adjT, int vertices);

#endif // KOSARAJU_HPP
