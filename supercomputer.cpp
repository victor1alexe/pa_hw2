#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <queue>
#include <vector>

using std::cout;
using std::endl;
using std::list;
using std::min;
using std::queue;
using std::vector;

int main()
{
  // read from file
  FILE *f = fopen("supercomputer.in", "r");
  FILE *out = fopen("supercomputer.out", "w");
  int n, m;
  fscanf(f, "%d %d", &n, &m);
  int *a = (int *)malloc((n + 1) * sizeof(int));
  for (int i = 1; i <= n; i++)
  {
    fscanf(f, "%d", &a[i]);
  }
  // declare vector of n arrays
  list<int> *v = new list<int>[n + 1];

  // vector of in-degrees
  int *in_degr1 = (int *)malloc((n + 1) * sizeof(int));
  int *in_degr2 = (int *)malloc((n + 1) * sizeof(int));
  memset(in_degr1, 0, (n + 1) * sizeof(int));
  memset(in_degr2, 0, (n + 1) * sizeof(int));
  for (int i = 0; i < m; i++)
  {
    int x, y;
    fscanf(f, "%d %d", &x, &y);
    v[x].push_back(y);
    in_degr1[y]++;
    in_degr2[y]++;
  }

  // topological sort
  vector<int> ans1 = vector<int>(n + 1);
  vector<int> ans2 = vector<int>(n + 1);

  queue<int> q1_1 = queue<int>();
  queue<int> q2_1 = queue<int>();
  queue<int> q1_2 = queue<int>();
  queue<int> q2_2 = queue<int>();

  for (int i = 1; i <= n; i++)
  {
    if (in_degr1[i] == 0)
    {
      if (a[i] == 1)
      {
        q1_1.push(i);
        q1_2.push(i);
      }
      else
      {
        q2_1.push(i);
        q2_2.push(i);
      }
    }
  }
  bool is1 = true;
  bool is2 = true;
  if (q1_1.empty())
  {
    is1 = false;
  }
  if (q2_1.empty())
  {
    is2 = false;
  }
  if (is1)
  {
    ans1[1] = q1_1.front();
    q1_1.pop();
  }
  if (is2)
  {
    ans2[1] = q2_2.front();
    q2_2.pop();
  }
  for (int i = 2; i <= n; i++)
  {
    int last1, last2;
    if (is1)
      last1 = ans1[i - 1];
    if (is2)
      last2 = ans2[i - 1];
    // update in degrees
    if (is1)
      for (auto it = v[last1].begin(); it != v[last1].end(); it++)
      {
        in_degr1[*it]--;
        if (in_degr1[*it] == 0)
        {
          if (a[*it] == 1)
          {
            q1_1.push(*it);
          }
          else
          {
            q2_1.push(*it);
          }
        }
      }

    if (is2)
      for (auto it = v[last2].begin(); it != v[last2].end(); it++)
      {
        in_degr2[*it]--;
        if (in_degr2[*it] == 0)
        {
          if (a[*it] == 1)
          {
            q1_2.push(*it);
          }
          else
          {
            q2_2.push(*it);
          }
        }
      }
    if (is1)
    {
      if (a[last1] == 1)
      {
        if (q1_1.empty())
        {
          ans1[i] = q2_1.front();
          q2_1.pop();
        }
        else
        {
          ans1[i] = q1_1.front();
          q1_1.pop();
        }
      }
      else
      {
        if (q2_1.empty())
        {
          ans1[i] = q1_1.front();
          q1_1.pop();
        }
        else
        {
          ans1[i] = q2_1.front();
          q2_1.pop();
        }
      }
    }
    if (is2)
    {
      if (a[last2] == 1)
      {
        if (q1_2.empty())
        {
          ans2[i] = q2_2.front();
          q2_2.pop();
        }
        else
        {
          ans2[i] = q1_2.front();
          q1_2.pop();
        }
      }
      else
      {
        if (q2_2.empty())
        {
          ans2[i] = q1_2.front();
          q1_2.pop();
        }
        else
        {
          ans2[i] = q2_2.front();
          q2_2.pop();
        }
      }
    }
  }

  // look for number switching
  int count1 = 0;
  int count2 = 0;
  int last1 = a[ans1[1]];
  int last2 = a[ans2[1]];
  for (int i = 1; i <= n; i++)
  {
    if (is1 && a[ans1[i]] != last1)
    {
      count1++;
      last1 = a[ans1[i]];
    }
    if (is2 && a[ans2[i]] != last2)
    {
      count2++;
      last2 = a[ans2[i]];
    }
  }
  if (!is1)
  {
    count1 = 1000000000;
  }
  if (!is2)
  {
    count2 = 1000000000;
  }
  int count = min(count1, count2);
  fprintf(out, "%d\n", count);

  fclose(f);
  fclose(out);

  return 0;
}