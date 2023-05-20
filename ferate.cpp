// C++ implementation of the approach
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

using namespace std;
using std::vector;

const int N = 501000;

int n, x;

vector<int> g[N];

// To check if the vertex has been
// visited or not
bool vis[N];

// To store if vertex is reachable
// from source or not
bool good[N];

int cnt;

void ADD_EDGE(int u, int v) { g[u].push_back(v); }

// Function to find all good vertices
void dfs1(int v)
{
  good[v] = true;
  for (auto to : g[v])
    if (!good[to])
      dfs1(to);
}

// Function to find cnt of all unreachable vertices
void dfs2(int v)
{
  vis[v] = true;
  ++cnt;
  for (auto to : g[v])
    if (!vis[to] && !good[to])
      dfs2(to);
}

// Function to return the minimum edges required
int Minimum_Edges()
{
  // Find all vertices reachable from the source
  dfs1(x);

  // To store all vertices with their cnt value
  vector<pair<int, int>> val;

  for (int i = 1; i <= n; ++i)
  {
    // If vertex is bad i.e. not reachable
    if (!good[i])
    {
      cnt = 0;
      memset(vis, false, sizeof(vis));

      // Find cnt of this vertex
      dfs2(i);
      val.push_back(make_pair(cnt, i));
    }
  }

  // Sort all unreachable vertices in
  // non-decreasing order of their cnt values
  sort(val.begin(), val.end());
  reverse(val.begin(), val.end());

  // Find the minimum number of edges
  // needed to be added
  int ans = 0;
  for (auto it : val)
  {
    if (!good[it.second])
    {
      if (it.second != x)
        ++ans;
      dfs1(it.second);
    }
  }

  return ans;
}

// Driver code
int main()
{
  // Number of nodes and source node
  FILE *f = fopen("ferate.in", "r");
  // FILE *f = fopen("public_tests/ferate/input/7-ferate.in", "r");
  FILE *out = fopen("ferate.out", "w");

  int m;
  fscanf(f, "%d %d %d", &n, &m, &x);
  for (int i = 0; i < m; i++)
  {
    int x, y;
    fscanf(f, "%d %d", &x, &y);
    ADD_EDGE(x, y);
  }
  fprintf(out, "%d", Minimum_Edges());
  return 0;
}