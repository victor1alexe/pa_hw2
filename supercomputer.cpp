#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <unistd.h>
#include <vector>

using std::ifstream;
using std::list;
using std::min;
using std::ofstream;
using std::queue;
using std::vector;

const int MAX_NODES = 100001;

int main()
{
	// Open streams
	ifstream fin("supercomputer.in");
	ofstream fout("supercomputer.out");

	int nr_nodes;
	int nr_edges;

	int dataset[MAX_NODES] = {0};

	int in_degr1[MAX_NODES] = {0};
	int in_degr2[MAX_NODES] = {0};

	// Read nr of nodes and edges
	fin >> nr_nodes >> nr_edges;

	auto *graph = new list<int>[nr_nodes + 1];

	// Read datasets
	for (int i = 1; i <= nr_nodes; i++)
		fin >> dataset[i];

	// Read edges
	for (int i = 1; i <= nr_edges; i++)
	{
		int x;
		int y;

		fin >> x >> y;

		graph[x].push_back(y);

		// Update in degrees
		in_degr1[y]++;
		in_degr2[y]++;
	}

	// Answer vectors
	vector<int> ans1 = vector<int>(nr_nodes + 1);
	vector<int> ans2 = vector<int>(nr_nodes + 1);

	// Queues for dataset 1
	queue<int> q1_1 = queue<int>();
	queue<int> q1_2 = queue<int>();

	// Queues for dataset 2
	queue<int> q2_1 = queue<int>();
	queue<int> q2_2 = queue<int>();

	for (int i = 1; i <= nr_nodes; i++)
	{
		if (in_degr1[i] != 0)
			continue;

		// Push nodes with in degree 0 and dataset 1
		if (dataset[i] == 1)
		{
			q1_1.push(i);
			q1_2.push(i);
			continue;
		}

		// Push nodes with in degree 0 and dataset 2
		q2_1.push(i);
		q2_2.push(i);
	}

	// Push first element of each dataset
	if (!q1_1.empty())
	{
		ans1[1] = q1_1.front();
		q1_1.pop();
	}
	if (!q2_1.empty())
	{
		ans2[1] = q2_2.front();
		q2_2.pop();
	}

	// For the rest of the nodes
	for (int i = 2; i <= nr_nodes; i++)
	{
		int last_added_node_1 = ans1[i - 1];
		int last_added_node_2 = ans2[i - 1];

		// For each neighbour of the last added node
		for (int &node : graph[last_added_node_1])
		{
			in_degr1[node]--;

			// If the in degree is 0
			// Push to the queue
			if (in_degr1[node] == 0)
			{
				if (dataset[node] == 1)
				{
					q1_1.push(node);
					continue;
				}

				q2_1.push(node);
			}
		}

		// For each neighbour of the last added node
		for (int &node : graph[last_added_node_2])
		{
			in_degr2[node]--;

			// If the in degree is 0
			// Push to the queue
			if (in_degr2[node] == 0)
			{
				if (dataset[node] == 1)
				{
					q1_2.push(node);
					continue;
				}

				q2_2.push(node);
			}
		}

		if (dataset[last_added_node_1] == 1)
		{
			if (!q1_1.empty())
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
		else
		{
			if (!q2_1.empty())
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

		if (dataset[last_added_node_2] == 1)
		{
			if (!q1_2.empty())
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
		else
		{
			if (!q2_2.empty())
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
	}

	int count1 = 0;
	int count2 = 0;

	int last1 = dataset[ans1[1]];
	int last2 = dataset[ans2[1]];

	for (int i = 1; i <= nr_nodes; i++)
	{
		if (dataset[ans1[i]] != last1)
		{
			count1++;
			last1 = dataset[ans1[i]];
		}
		if (dataset[ans2[i]] != last2)
		{
			count2++;
			last2 = dataset[ans2[i]];
		}
	}

	int const count = min(count1, count2);

	fout << count << '\n';

	fin.close();
	fout.close();

	return 0;
}