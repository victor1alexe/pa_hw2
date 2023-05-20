#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <stack>

using std::cout;
using std::endl;
using std::list;

int dfsLimit(list<int> *tree, int treeSize, int root, int x, int *dp)
{
  std::stack<int> stack;
  int counter = 0;

  stack.push(root);
  int node;

  while (!stack.empty() && counter <= x)
  {
    node = stack.top();
    stack.pop();
    counter++;

    // push children
    int nodeChildren = dp[node];
    if (counter + nodeChildren < x)
    {
      counter += nodeChildren;
      continue;
    }
    for (auto it = tree[node].rbegin(); it != tree[node].rend(); it++)
    {
      stack.push(*it);
    }
  }

  if (counter <= x)
    return -1;
  else
    return node;
}

void count_descendants(list<int> *tree, int treeSize, int root, int *dp)
{
  dp[root] = 0;
  for (auto it = tree[root].begin(); it != tree[root].end(); it++)
  {
    count_descendants(tree, treeSize, *it, dp);
    dp[root] += dp[*it] + 1;
  }
}

int main()
{
  // read from file
  FILE *f = fopen("magazin.in", "r");
  FILE *out = fopen("magazin.out", "w");
  int n, q;
  fscanf(f, "%d %d", &n, &q);
  int *in = (int *)malloc((n + 1) * sizeof(int));
  list<int> *tree = new list<int>[n + 1];
  for (int i = 2; i <= n; i++)
  {
    fscanf(f, "%d", &in[i]);
    tree[in[i]].push_back(i);
  }

  int *dp = (int *)malloc((n + 1) * sizeof(int));
  count_descendants(tree, n + 1, 1, dp);

  for (int i = 0; i < q; i++)
  {
    int x, root;
    fscanf(f, "%d %d", &root, &x);
    fprintf(out, "%d\n", dfsLimit(tree, n + 1, root, x, dp));
  }
  fclose(f);
  fclose(out);
  return 0;
}
