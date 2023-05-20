#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <unistd.h>

#include <iostream>
#include <list>
#include <stack>

using std::ifstream;
using std::list;
using std::ofstream;
using std::stack;

void count_descendants(list<int> *tree, int current_node, int *descendants_of)
{
	descendants_of[current_node] = 0;

	for (auto it = tree[current_node].begin(); it != tree[current_node].end(); it++)
	{
		count_descendants(tree, *it, descendants_of);

		descendants_of[current_node] += descendants_of[*it] + 1;
	}
}

int main()
{
	ifstream fin("magazin.in");
	ofstream fout("magazin.out");

	int nr_deposits = 0;
	int nr_questions = 0;

	fin >> nr_deposits >> nr_questions;

	int supplier_of[nr_deposits + 1] = {0};
	int descendants_of[nr_deposits + 1] = {0};

	auto *tree = new list<int>[nr_deposits + 1];

	for (int i = 2; i <= nr_deposits; i++)
	{
		fin >> supplier_of[i];

		tree[supplier_of[i]].push_back(i);
	}

	count_descendants(tree, 1, descendants_of);

	for (int i = 1; i <= nr_questions; i++)
	{
		int deposit;
		int nr_of_deliveries;

		fin >> deposit >> nr_of_deliveries;

		stack<int> stack;
		int node;
		int counter = 0;

		stack.push(deposit);

		while (!stack.empty() && counter <= nr_of_deliveries)
		{
			node = stack.top();
			stack.pop();

			counter++;

			if (counter + descendants_of[node] < nr_of_deliveries)
			{
				counter += descendants_of[node];
				continue;
			}

			for (auto node_neigh = tree[node].rbegin(); node_neigh != tree[node].rend(); node_neigh++)
			{
				stack.push(*node_neigh);
			}
		}

		if (counter <= nr_of_deliveries)
		{
			fout << -1 << '\n';
		}
		else
		{
			fout << node << '\n';
		}
	}

	fin.close();
	fout.close();

	return 0;
}
