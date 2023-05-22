#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <vector>

using std::ifstream;
using std::list;
using std::ofstream;
using std::pair;
using std::priority_queue;
using std::vector;

#define INPUT_FILE "teleportare.in"
#define OUTPUT_FILE "teleportare.out"

#define INF std::numeric_limits<uint32_t>::max()

#define DIE(assertion, call_description)                 \
  do {                                                   \
    if (assertion) {                                     \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
      perror(call_description);                          \
      exit(EXIT_FAILURE);                                \
    }                                                    \
  } while (0)

struct Room {
  uint32_t id;
  uint32_t cost;
  bool is_tp;
};

struct Compare_by_dist {
  bool operator()(const pair<uint32_t, uint32_t> &p1,
                  const pair<uint32_t, uint32_t> &p2) {
    return p1.second > p2.second;
  }
};

void open_streams(ifstream &in, ofstream &out) {
  in.open(INPUT_FILE);
  DIE(in.fail(), "open " INPUT_FILE);

  out.open(OUTPUT_FILE);
  DIE(out.fail(), "open " OUTPUT_FILE);
}

uint32_t dls(uint32_t start, uint32_t end, uint32_t cost, uint32_t target,
             uint32_t depth, list<Room> *neigh_rooms) {
  bool const at_end = start == end;
  bool const can_tp = cost % target == 0;

  if (at_end && can_tp) {
    return cost;
  }

  if (depth <= 0) {
    return -1;
  }

  for (auto neigh : neigh_rooms[start]) {
    if (neigh.is_tp) {
      continue;
    }

    uint32_t const move_around_dist =
        dls(neigh.id, end, cost + neigh.cost, target, depth - 1, neigh_rooms);
    if (move_around_dist != -1) {
      return move_around_dist;
    }
  }

  return -1;
}

uint32_t iddfs(uint32_t start, uint32_t end, uint32_t cost_init,
               uint32_t target, uint32_t depth, list<Room> *neigh_rooms) {
  for (uint32_t i = 1; i <= depth; i++) {
    uint32_t const min_move_around_dist =
        dls(start, end, cost_init, target, i, neigh_rooms);

    if (min_move_around_dist != -1) {
      return min_move_around_dist;
    }
  }

  return -1;
}

struct Compare {
  bool operator()(const pair<uint32_t, uint32_t> &p1,
                  const pair<uint32_t, uint32_t> &p2) {
    return p1.second > p2.second;
  }
};

uint32_t dijkstra(uint32_t start, uint32_t end, list<Room> *neigh_rooms,
                  uint32_t n_rooms) {
  auto *dist = new uint32_t[end + 1];
  DIE(dist == nullptr, "alloc dist");
  memset(dist, UINT32_MAX, sizeof(uint32_t) * (end + 1));

  auto *vis = new bool[end + 1];
  DIE(vis == nullptr, "alloc vis");
  memset(vis, 0, sizeof(bool) * (end + 1));

  dist[start] = 0;

  priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t>>,
                 Compare_by_dist>
      pq;

  pq.emplace(start, 0);

  while (true) {
    if (pq.empty()) {
      break;
    }

    uint32_t const room = pq.top().first;
    pq.pop();

    if (room == end) {
      break;
    }

    if (vis[room]) {
      continue;
    }

    vis[room] = true;

    for (auto neigh : neigh_rooms[room]) {
      uint32_t const potential_dist = dist[room] + neigh.cost;
      bool const worth_it = potential_dist < dist[neigh.id];
      bool const can_tp = dist[room] % neigh.cost == 0;
      bool const am_start = room == start;

      if (am_start && neigh.is_tp && dist[neigh.id] > 1) {
        dist[neigh.id] = 1;
        pq.emplace(neigh.id, dist[neigh.id]);
        continue;
      }

      if (neigh.is_tp && can_tp && worth_it) {
        dist[neigh.id] = dist[room] + 1;
        pq.emplace(neigh.id, dist[neigh.id]);
        continue;
      }

      if (neigh.is_tp && !can_tp) {
        uint32_t const move_around_dist =
            iddfs(room, room, dist[room], neigh.cost, n_rooms, neigh_rooms);

        if (move_around_dist == -1) {
          continue;
        }

        if (move_around_dist + 1 < dist[neigh.id]) {
          dist[neigh.id] = move_around_dist + 1;
          pq.emplace(neigh.id, dist[neigh.id]);
        }

        continue;
      }

      if (worth_it && !neigh.is_tp) {
        dist[neigh.id] = potential_dist;
        pq.emplace(neigh.id, dist[neigh.id]);
      }
    }
  }

  uint32_t const ret = dist[end];

  delete[] dist;
  delete[] vis;

  return ret;
}

int main(int argc, char ** /*argv*/, char ** /*envp*/) {
  DIE(argc != 1, "usage: ./teleportare");

  ifstream in;
  ofstream out;
  open_streams(in, out);

  uint32_t n_rooms = 0;
  uint32_t n_halls = 0;
  uint32_t n_tp = 0;

  in >> n_rooms >> n_halls >> n_tp;
  DIE(in.fail(), "read n_rooms, n_halls, n_tp");

  auto *neigh_rooms = new list<Room>[n_rooms + 1];
  DIE(neigh_rooms == nullptr, "alloc neigh_rooms");

  for (uint32_t i = 1; i <= n_halls; i++) {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t c = 0;

    in >> x >> y >> c;
    DIE(in.fail(), "read x, y, c");

    Room room;
    room.id = y;
    room.cost = c;
    room.is_tp = false;

    neigh_rooms[x].push_back(room);

    room.id = x;
    neigh_rooms[y].push_back(room);
  }

  for (uint32_t i = 1; i <= n_tp; i++) {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t c = 0;

    in >> x >> y >> c;
    DIE(in.fail(), "read x, y, c");

    Room room;
    room.id = y;
    room.cost = c;
    room.is_tp = true;

    neigh_rooms[x].push_back(room);

    room.id = x;
    neigh_rooms[y].push_back(room);
  }

  // out << djikstra(1, n_rooms, neigh_rooms, n_rooms);
  out << dijkstra(1, n_rooms, neigh_rooms, n_rooms);

  in.close();
  out.close();

  delete[] neigh_rooms;

  return 0;
}
