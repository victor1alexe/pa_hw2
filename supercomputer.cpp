#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <vector>

using std::ifstream;
using std::ofstream;

using std::list;
using std::queue;

using std::min;

#define INPUT_FILE "supercomputer.in"
#define OUTPUT_FILE "supercomputer.out"

#define DIE(assertion, call_description)                 \
  do {                                                   \
    if (assertion) {                                     \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
      perror(call_description);                          \
      exit(EXIT_FAILURE);                                \
    }                                                    \
  } while (0)

enum dataset_t { RED = 1, BLACK = 2 };

void open_streams(ifstream &in, ofstream &out) {
  in.open(INPUT_FILE);
  DIE(in.fail(), "open " INPUT_FILE);

  out.open(OUTPUT_FILE);
  DIE(out.fail(), "open " OUTPUT_FILE);
}

void alloc_queue(queue<uint32_t> *&q) {
  q = new queue<uint32_t>();
  DIE(q == nullptr, "alloc q");
}

void alloc_mem(void **ptr, size_t size) {
  *ptr = malloc(size);
  DIE(*ptr == nullptr, "malloc");
}

void read_nodes_edges(uint32_t &n_nodes, uint32_t &n_edges, ifstream &in) {
  in >> n_nodes >> n_edges;
  DIE(in.fail(), "read n_nodes and n_edges");
}

void read_datasets(uint32_t n_nodes, uint32_t *dataset, ifstream &in) {
  for (uint32_t i = 1; i <= n_nodes; i++) {
    in >> dataset[i];
    DIE(in.fail(), "read dataset[i]");
  }
}

void read_edges(uint32_t n_edges, list<uint32_t> *adj_list,
                uint32_t *in_degr_red, uint32_t *in_degr_black, ifstream &in) {
  for (uint32_t i = 1; i <= n_edges; i++) {
    uint32_t x = 0;
    uint32_t y = 0;

    in >> x >> y;
    DIE(in.fail(), "read x and y");

    adj_list[x].push_back(y);

    in_degr_red[y]++;
    in_degr_black[y]++;
  }
}

void fill_queues(uint32_t n_nodes, const uint32_t *dataset,
                 const uint32_t *in_degr_black, queue<uint32_t> *q_red_1,
                 queue<uint32_t> *q_red_2, queue<uint32_t> *q_black_1,
                 queue<uint32_t> *q_black_2) {
  for (uint32_t i = 1; i <= n_nodes; i++) {
    if (in_degr_black[i] != 0) {
      continue;
    }

    if (dataset[i] == RED) {
      q_red_1->push(i);
      q_red_2->push(i);
      continue;
    }

    q_black_1->push(i);
    q_black_2->push(i);
  }
}

uint32_t get_switches(uint32_t n_nodes, const uint32_t *dataset,
                      const uint32_t *top_sort_red,
                      const uint32_t *top_sort_black, bool const empty_red_1,
                      bool const empty_black_2) {
  uint32_t n_switch_red = 0;
  uint32_t n_switch_black = 0;

  uint32_t last_red = top_sort_red[1];
  uint32_t last_black = top_sort_black[1];

  for (uint32_t i = 2; i <= n_nodes; i++) {
    if (dataset[top_sort_red[i]] != dataset[last_red]) {
      n_switch_red++;
      last_red = top_sort_red[i];
    }

    if (dataset[top_sort_black[i]] != dataset[last_black]) {
      n_switch_black++;
      last_black = top_sort_black[i];
    }
  }

  if (empty_red_1) {
    n_switch_red = INT32_MAX;
  }

  if (empty_black_2) {
    n_switch_black = INT32_MAX;
  }

  return min(n_switch_red, n_switch_black);
}

void add_neighbs(const uint32_t *dataset, const list<uint32_t> *adj_list,
                 uint32_t *in_degr, uint32_t *top_sort, queue<uint32_t> *q_red,
                 queue<uint32_t> *q_black, uint32_t i) {
  uint32_t const last = top_sort[i - 1];

  for (auto const &neigh : adj_list[last]) {
    in_degr[neigh]--;

    if (in_degr[neigh] != 0) {
      continue;
    }

    if (dataset[neigh] == RED) {
      q_red->push(neigh);
      continue;
    }

    q_black->push(neigh);
  }

  if (dataset[last] == RED) {
    if (!q_red->empty()) {
      top_sort[i] = q_red->front();
      q_red->pop();
    } else {
      top_sort[i] = q_black->front();
      q_black->pop();
    }
  }
  if (dataset[last] == BLACK) {
    if (!q_black->empty()) {
      top_sort[i] = q_black->front();
      q_black->pop();
    } else {
      top_sort[i] = q_red->front();
      q_red->pop();
    }
  }
}

void add_fst_nodes(uint32_t *top_sort_red, uint32_t *top_sort_black,
                   queue<uint32_t> *q_red_1, queue<uint32_t> *q_black_2,
                   bool const empty_red_1, bool const empty_black_2 /* */) {
  if (!empty_red_1) {
    top_sort_red[1] = q_red_1->front();
    q_red_1->pop();
  }

  if (!empty_black_2) {
    top_sort_black[1] = q_black_2->front();
    q_black_2->pop();
  }
}

void top_sorts(uint32_t n_nodes, const uint32_t *dataset,
               const list<uint32_t> *adj_list, uint32_t *in_degr_red,
               uint32_t *in_degr_black, uint32_t *top_sort_red,
               uint32_t *top_sort_black, queue<uint32_t> *q_red_1,
               queue<uint32_t> *q_black_1, queue<uint32_t> *q_red_2,
               queue<uint32_t> *q_black_2, bool const empty_red_1,
               bool const empty_black_2 /* */) {
  for (uint32_t i = 2; i <= n_nodes; i++) {
    if (!empty_red_1) {
      add_neighbs(dataset, adj_list, in_degr_red, top_sort_red, q_red_1,
                  q_black_1, i);
    }

    if (!empty_black_2) {
      add_neighbs(dataset, adj_list, in_degr_black, top_sort_black, q_red_2,
                  q_black_2, i);
    }
  }
}

int main(int argc, char * /*argv*/[], char * /*envp*/[]) {
  DIE(argc != 1, "usage: ./supercomputer");

  ifstream in;
  ofstream out;
  open_streams(in, out);

  uint32_t n_nodes = 0;
  uint32_t n_edges = 0;
  read_nodes_edges(n_nodes, n_edges, in);

  uint32_t *dataset = nullptr;
  alloc_mem(reinterpret_cast<void **>(&dataset),
            (n_nodes + 1) * sizeof(uint32_t));

  read_datasets(n_nodes, dataset, in);

  auto *adj_list = new list<uint32_t>[n_nodes + 1];
  DIE(adj_list == nullptr, "alloc adj_list");

  uint32_t *in_degr_red = nullptr;
  uint32_t *in_degr_black = nullptr;

  alloc_mem(reinterpret_cast<void **>(&in_degr_red),
            (n_nodes + 1) * sizeof(uint32_t));
  alloc_mem(reinterpret_cast<void **>(&in_degr_black),
            (n_nodes + 1) * sizeof(uint32_t));

  read_edges(n_edges, adj_list, in_degr_red, in_degr_black, in);

  uint32_t *top_sort_red = nullptr;
  uint32_t *top_sort_black = nullptr;

  alloc_mem(reinterpret_cast<void **>(&top_sort_red),
            (n_nodes + 1) * sizeof(uint32_t));
  alloc_mem(reinterpret_cast<void **>(&top_sort_black),
            (n_nodes + 1) * sizeof(uint32_t));

  queue<uint32_t> *q_red_1 = nullptr;
  queue<uint32_t> *q_black_1 = nullptr;
  queue<uint32_t> *q_red_2 = nullptr;
  queue<uint32_t> *q_black_2 = nullptr;

  alloc_queue(q_red_1);
  alloc_queue(q_black_1);
  alloc_queue(q_red_2);
  alloc_queue(q_black_2);

  fill_queues(n_nodes, dataset, in_degr_black, q_red_1, q_red_2, q_black_1,
              q_black_2);

  bool const empty_red_1 = q_red_1->empty();
  bool const empty_black_2 = q_black_2->empty();

  add_fst_nodes(top_sort_red, top_sort_black, q_red_1, q_black_2, empty_red_1,
                empty_black_2);

  top_sorts(n_nodes, dataset, adj_list, in_degr_red, in_degr_black,
            top_sort_red, top_sort_black, q_red_1, q_black_1, q_red_2,
            q_black_2, empty_red_1, empty_black_2);

  uint32_t const ans = get_switches(n_nodes, dataset, top_sort_red,
                                    top_sort_black, empty_red_1, empty_black_2);

  out << ans << "\n";

  in.close();
  out.close();

  free(dataset);
  free(in_degr_red);
  free(in_degr_black);
  free(top_sort_red);
  free(top_sort_black);

  delete[] adj_list;
  delete q_red_1;
  delete q_red_2;
  delete q_black_1;
  delete q_black_2;

  return 0;
}
