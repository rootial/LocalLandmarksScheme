#ifndef COMMONHEADER_H_INCLUDED
#define COMMONHEADER_H_INCLUDED

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <bitset>
#include <cstdint>
#include <fstream>
#include <queue>
#include <vector>

const int maxnode = 2000000;
const int maxBlockSize = 16;
const uint8_t INF8 = 100;
const int TreeNodeType = 0;
const int ChainNodeType = 1;
const int OtherNodeType = 2;

#define DeletePtr(x) {if ((x) != NULL) {delete (x); x = NULL;}}
#define DeleteArrPtr(x) {if ((x) != NULL) {delete [](x); x = NULL;}}
#define Debug(x) printf("Excute here %d lines\n", (x))
#define MSG(str, x) printf("%s: %d\n", str, x)

struct Edge {
  int v;
  uint8_t d;
  Edge() {}
  Edge(int v, uint8_t d): v(v), d(d) {}
  bool operator < (const Edge& oe) const {
    return v < oe.v || (v == oe.v && d < oe.d);
  }

  bool operator == (const Edge& oe) const {
    return v == oe.v;
  }
};

struct Node {
  int u, d;
  Node() {}
  Node(int u, int d): u(u), d(d) {}
  bool operator < (const Node& o) const {
    return d > o.d;
  }
};

typedef std::vector<Edge>(*GPtr);
typedef const std::vector<Edge>(*ConstGPtr);
typedef std::vector<std::vector<Edge> > GraphType;

typedef std::priority_queue<Node> pq;

#endif // COMMONHEADER_H_INCLUDED
