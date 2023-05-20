#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <unistd.h>
#include <vector>

using std::ifstream;
using std::ofstream;
using std::pair;
using std::priority_queue;
using std::vector;

void dfs1_iter(int v, vector<int> g[], bool good[])
{
	vector<int> stack;
	stack.push_back(v);
	while (!stack.empty())
	{
		int node = stack.back();
		stack.pop_back();
		good[node] = true;
		for (auto to : g[node])
		{
			if (!good[to])
			{
				stack.push_back(to);
			}
		}
	}
}

void dfs2(int v, vector<int> g[], bool good[], bool vis[], int &cnt)
{
	vis[v] = true;
	++cnt;
	for (long unsigned int i = 0; i < g[v].size(); ++i)
	{
		if (good[g[v][i]] || vis[g[v][i]])
			continue;

		dfs2(g[v][i], g, good, vis, cnt);
	}
}

class Compare
{
public:
	bool operator()(pair<int, int> a, pair<int, int> b)
	{
		return a.first < b.first;
	}
};

int main()
{
	const int N = 100001;

	int n = 0;
	int m = 0;
	int x = 0;
	int i = 0;
	int from = 0;
	int to = 0;
	int cnt = 0;

	bool vis[N];
	bool good[N];

	vector<int> g[N];

	ifstream f("ferate.in");
	ofstream fout("ferate.out");

	f >> n >> m >> x;

	for (i = 1; i <= m; i++)
	{
		f >> from >> to;
		g[from].push_back(to);
	}

	dfs1_iter(x, g, good);
	priority_queue<pair<int, int>, vector<pair<int, int>>, Compare> pq;
	for (int i = 1; i <= n; ++i)
	{
		if (!good[i])
		{
			cnt = 0;
			memset(vis, 0, N);
			dfs2(i, g, good, vis, cnt);
			pq.emplace(cnt, i);
		}
	}
	int ans = 0;
	while (!pq.empty())
	{
		auto it = pq.top();
		pq.pop();
		if (!good[it.second])
		{
			if (it.second != x)
			{
				++ans;
			}
			dfs1_iter(it.second, g, good);
		}
	}

	fout << ans;

	return 0;
}