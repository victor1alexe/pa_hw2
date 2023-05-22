#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

using std::ifstream;
using std::ofstream;
using std::pair;
using std::priority_queue;
using std::vector;

#define DIE(assertion, call_description)                 \
  do {                                                   \
    if (assertion) {                                     \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
      perror(call_description);                          \
      exit(EXIT_FAILURE);                                \
    }                                                    \
  } while (0)

void cnt_desc(uint32_t v, vector<uint32_t> **g, bool *reachable, bool *visited,
              uint32_t &cnt) {
  visited[v] = true;
  ++cnt;

  for (uint32_t i = 0; i < g[v]->size(); ++i) {
    uint32_t const j = g[v]->at(i);

    if (reachable[j] || visited[j]) {
      continue;
    }

    cnt_desc(j, g, reachable, visited, cnt);
  }
}

void mark_reachable(uint32_t v, vector<uint32_t> **g, bool *reachable) {
  vector<uint32_t> stack;

  stack.push_back(v);

  while (true) {
    if (stack.empty()) {
      break;
    }

    uint32_t const node = stack.back();
    stack.pop_back();

    reachable[node] = true;

    for (uint32_t const neigh : *g[node]) {
      if (!reachable[neigh]) {
        stack.push_back(neigh);
      }
    }
  }
}

void cnt_addable(
    uint32_t src, vector<uint32_t> **g, bool *reachable,
    priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t>>,
                   std::less<>> &pq,
    uint32_t &ans) {
  while (true) {
    if (pq.empty()) {
      break;
    }

    auto pair = pq.top();
    pq.pop();

    uint32_t const node = pair.second;

    if (!reachable[node]) {
      if (node != src) {
        ++ans;
      }

      mark_reachable(node, g, reachable);
    }
  }
}

void sort_by_desc(
    vector<uint32_t> **g, uint32_t const n_depo, bool *reachable, bool *visited,
    priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t>>,
                   std::less<>> &pq) {
  for (uint32_t i = 1; i <= n_depo; i++) {
    if (!reachable[i]) {
      uint32_t cnt = 0;

      memset(visited, 0, n_depo + 1);
      cnt_desc(i, g, reachable, visited, cnt);

      pq.emplace(cnt, i);
    }
  }
}

void alloc_mem(void **ptr, size_t size) {
  *ptr = malloc(size);
  DIE(*ptr == nullptr, "malloc");
}

int main(int argc, char ** /*argv*/, char ** /*envp*/) {
  DIE(argc != 1, "usage: ./ferate");

  ifstream fin;
  ofstream fout;

  uint32_t n_depo = 0;
  uint32_t n_roads = 0;
  uint32_t src = 0;

  bool *reachable = nullptr;
  bool *visited = nullptr;

  fin.open("ferate.in");
  DIE(fin.fail(), "open ferate.in");

  fout.open("ferate.out");
  DIE(fout.fail(), "open ferate.out");

  fin >> n_depo >> n_roads >> src;

  alloc_mem(reinterpret_cast<void **>(&reachable), (n_depo + 1) * sizeof(bool));
  alloc_mem(reinterpret_cast<void **>(&visited), (n_depo + 1) * sizeof(bool));

  auto **g = new vector<uint32_t> *[n_depo + 1];
  DIE(g == nullptr, "alloc g");
  for (uint32_t i = 1; i <= n_depo; i++) {
    g[i] = new vector<uint32_t>();
    DIE(g[i] == nullptr, "alloc g[i]");
  }

  for (uint32_t i = 1; i <= n_roads; i++) {
    uint32_t x = 0;
    uint32_t y = 0;

    fin >> x >> y;
    DIE(fin.fail(), "read x and y");

    g[x]->push_back(y);
  }

  mark_reachable(src, g, reachable);

  priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t>>,
                 std::less<>>
      pq;

  sort_by_desc(g, n_depo, reachable, visited, pq);

  uint32_t ans = 0;
  cnt_addable(src, g, reachable, pq, ans);

  fout << ans;

  fin.close();
  fout.close();

  free(reachable);
  free(visited);
  for (uint32_t i = 1; i <= n_depo; i++) {
    delete g[i];
  }
  delete[] g;

  return 0;
}
