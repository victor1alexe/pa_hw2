#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <stack>
#include <unordered_map>
#include <vector>

using std::ifstream;
using std::list;
using std::ofstream;
using std::pair;
using std::stack;
using std::unordered_map;
using std::vector;

#define DIE(assertion, call_description)                 \
  do {                                                   \
    if (assertion) {                                     \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
      perror(call_description);                          \
      exit(EXIT_FAILURE);                                \
    }                                                    \
  } while (0)

void cnt_desc(list<uint32_t> *tree, uint32_t node, uint32_t *desc) {
  desc[node] = 0;

  for (auto child = tree[node].begin(); child != tree[node].end(); child++) {
    cnt_desc(tree, *child, desc);

    desc[node] += desc[*child] + 1;
  }
}

void send_packets(list<uint32_t> *tree, const uint32_t *desc, uint32_t n_q,
                  uint32_t n_depo, ifstream &fin, ofstream &fout) {
  for (uint32_t i = 1; i <= n_q; i++) {
    uint32_t depo = 0;
    uint32_t n_packets = 0;
    uint32_t node = 0;
    uint32_t cnt = 0;
    stack<uint32_t> stack;

    fin >> depo >> n_packets;
    // DIE(fin.fail(), "read depo and n_packets");

    if (desc[depo] < n_packets) {
      fout << -1 << '\n';
      continue;
    }

    stack.push(depo);

    while (true) {
      if (stack.empty()) {
        break;
      }

      if (cnt > n_packets) {
        break;
      }

      node = stack.top();
      stack.pop();

      cnt++;

      if (cnt + desc[node] <= n_packets) {
        cnt += desc[node];
        continue;
      }

      std::reverse_iterator<list<uint32_t>::iterator> child;
      for (child = tree[node].rbegin(); child != tree[node].rend(); child++) {
        stack.push(*child);
      }
    }

    if (cnt <= n_packets) {
      fout << -1 << '\n';
    } else {
      fout << node << '\n';
    }
  }
}

int main(int argc, char ** /*argv*/, char ** /*enviro*/) {
  // DIE(argc != 1, "usage: ./magazin");

  std::ios_base::sync_with_stdio(false);

  ifstream fin;
  fin.open("magazin.in");
  // DIE(fin.fail(), "open magazin.in");

  ofstream fout;
  fout.open("magazin.out");
  // DIE(fout.fail(), "open magazin.out");

  uint32_t n_depo = 0;
  uint32_t n_q = 0;

  fin >> n_depo >> n_q;
  // DIE(fin.fail(), "read n_depo and n_q");

  auto *sup = new uint32_t[n_depo + 1];
  // DIE(sup == nullptr, "alloc sup");
  // memset(sup, 0, (n_depo + 1) * sizeof(uint32_t));

  auto *desc = new uint32_t[n_depo + 1];
  // DIE(desc == nullptr, "alloc desc");
  memset(desc, 0, (n_depo + 1) * sizeof(uint32_t));

  auto *tree = new list<uint32_t>[n_depo + 1];
  // DIE(tree == nullptr, "alloc tree");

  for (uint32_t i = 2; i <= n_depo; i++) {
    fin >> sup[i];

    tree[sup[i]].push_back(i);
  }

  cnt_desc(tree, 1, desc);

  send_packets(tree, desc, n_q, n_depo, fin, fout);

  fin.close();
  fout.close();

  delete[] sup;
  delete[] desc;
  delete[] tree;

  return 0;
}
