#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <vector>

using std::list;
using std::pair;
using std::priority_queue;
using std::vector;

#define INF std::numeric_limits<uint32_t>::max()

struct Node {
  int n;
  uint32_t cost;
  bool temp;
};

int iddfs(int start, int end, int initial_cost, int target, int limit,
          list<Node> *graph, int goal);

// depth limited search with revisiting nodes, to check if there is a path
// from start to end with an exact cost
uint32_t dls(int start, int end, uint32_t cost, int target, int limit,
             list<Node> *graph, int goal) {
  // cout << start << " " << end << ": " << cost << endl;
  if (start == end && cost % target == 0) {
    return cost;
  }

  if (limit <= 0) {
    return -1;
  }

  for (auto it = graph[start].begin(); it != graph[start].end(); it++) {
    if (!it->temp) {
      int const ret =
          dls(it->n, end, cost + it->cost, target, limit - 1, graph, goal);
      if (ret != -1) {
        return ret;
      }
    }
    // if (it->temp) {
    //   int ret = iddfs(it->n, it->n, cost, it->cost, limit, graph, goal);
    //   if (ret != -1) {
    //     return ret;
    //   }
    // }
  }
  return -1;
}

// IDDFS
int iddfs(int start, int end, int initial_cost, int target, int limit,
          list<Node> *graph, int goal) {
  for (int i = 1; i <= limit; i++) {
    int const ret = dls(start, end, initial_cost, target, i, graph, goal);
    if (ret != -1) {
      return ret;
    }
  }

  return -1;
}

struct Compare {
  bool operator()(const pair<int, int> &p1, const pair<int, int> &p2) {
    return p1.second > p2.second;
  }
};

// djikstra
uint32_t djikstra(int start, int end, list<Node> *graph, int n) {
  auto *dist = new uint32_t[end + 1];
  bool *visited = new bool[end + 1];

  for (int i = 0; i <= end; i++) {
    dist[i] = INF;
    visited[i] = false;
  }

  dist[start] = 0;
  // queue<int> q;
  //  create priority queue of pairs, and compare by second element
  priority_queue<pair<int, int>, vector<pair<int, int>>, Compare> pq;

  // q.push(start);
  pq.emplace(start, 0);

  while (!pq.empty()) {
    // getchar();
    //  int node = q.front();
    //  q.pop();
    int const node = pq.top().first;
    pq.pop();

    // cout << "node taken out: " << node << endl;

    if (node == end) {
      break;
    }

    if (visited[node]) {
      continue;
    }
    visited[node] = true;

    for (auto it = graph[node].begin(); it != graph[node].end(); it++) {
      // cout << "checking node: " << it->n << endl;
      if (node == start && it->temp && dist[it->n] > 1) {
        // cout << "starting with teleport" << endl;
        dist[it->n] = dist[node] + 1;
        // q.push(it->n);
        pq.emplace(it->n, dist[it->n]);
        // cout << "dist: ";
        // for (int i = 1; i <= end; i++) {
        //   cout << dist[i] << " ";
        // }
        // cout << endl;
        continue;
      }
      if (it->temp && dist[node] % it->cost == 0 &&
          dist[it->n] > dist[node] + it->cost) {
        // cout << "can take shortcut" << endl;
        dist[it->n] = dist[node] + 1;
        // visited[it->n] = false;
        pq.emplace(it->n, dist[it->n]);
      } else if (it->temp && dist[node] < dist[it->n]) {
        // cout << "checking if can teleport" << endl;
        int const ret = iddfs(node, node, dist[node], it->cost, n, graph, end);
        if (ret != -1) {
          // cout << "can teleport" << endl;
          if (dist[it->n] > ret + 1) {
            dist[it->n] = ret + 1;
            // visited[it->n] = false;
            pq.emplace(it->n, dist[it->n]);
          }
        }
      } else if (!it->temp && dist[it->n] > dist[node] + it->cost) {
        // cout << "found better" << endl;
        dist[it->n] = dist[node] + it->cost;
        pq.emplace(it->n, dist[it->n]);
      }
      // cout << "dist: ";
      // for (int i = 1; i <= end; i++) {
      //   cout << dist[i] << " ";
      // }
      // cout << endl;
    }
  }

  // print dist
  // for (int i = 1; i <= end; i++) {
  //   cout << dist[i] << " ";
  // }
  // cout << endl;

  uint32_t const ret = dist[end];
  delete[] dist;
  delete[] visited;

  return ret;
}

int main() {
  // read from file
  int m;
  int n;
  int k;
  FILE *f = fopen("teleportare.in", "r");
  // FILE *f = fopen("public_tests/teleportare/input/4-teleportare.in", "r");
  FILE *out = fopen("teleportare.out", "w");
  fscanf(f, "%d %d %d", &n, &m, &k);

  // read graph
  auto *graph = new list<Node>[n + 1];
  for (int i = 0; i < m; i++) {
    int x;
    int y;
    int c;
    fscanf(f, "%d %d %d", &x, &y, &c);
    Node node;
    node.n = y;
    node.cost = c;
    node.temp = false;
    graph[x].push_back(node);
    Node node2;
    node2.n = x;
    node2.cost = c;
    node2.temp = false;
    graph[y].push_back(node2);
  }

  // read teleportation
  for (int i = 0; i < k; i++) {
    int x;
    int y;
    int c;
    fscanf(f, "%d %d %d", &x, &y, &c);
    Node node;
    node.n = y;
    node.cost = c;
    node.temp = true;
    graph[x].push_back(node);
    Node node2;
    node2.n = x;
    node2.cost = c;
    node2.temp = true;
    graph[y].push_back(node2);
  }

  // print graph
  // for (int i = 1; i <= n; i++) {
  //   cout << i << ": ";
  //   for (auto it = graph[i].begin(); it != graph[i].end(); it++) {
  //     if (it->temp)
  //       cout << "(" << it->n << ") ";
  //     else
  //       cout << it->n << " ";
  //   }
  //   cout << endl;
  // }

  // cout << djikstra(1, n, graph, n) << endl;
  fprintf(out, "%d", djikstra(1, n, graph, n));

  fclose(f);
  fclose(out);

  delete[] graph;

  return 0;
}
