#ifndef TREESTRUCT_H_INCLUDED
#define TREESTRUCT_H_INCLUDED

#include "CommonHeader.h"

#include <bitset>
#include <cstdio>
#include <sys/time.h>
#include <queue>
#include <vector>

inline int maxBits(int x) {
  if (x <= 1) {
    return 0;
  }
  int ret = 0;
  while ((1 << (ret + 1)) < x) {
    ret++;
  }
  return ret;
}

struct Node {
  int u, d;
  Node() {}
  Node(int u, int d): u(u), d(d) {}
  bool operator < (const Node& o) const {
    return d > o.d;
  }
};

typedef std::priority_queue<Node> pq;

class TreeStruct {
public:
  std::vector<int> *tree = NULL;
  int *distance = NULL;

  void constructIndex(const std::vector<std::vector<Edge> >& inGrapph);

  int queryDistance(int x, int y);

  TreeStruct(int root, int numVertices): root(root), numVertices(numVertices) {
    tree = new std::vector<int>[numVertices];
    distance = new int[numVertices];
    level = new int[numVertices * 2];
    trace = new int[numVertices * 2];
    label = new int[numVertices];
    parent = new int[numVertices];
  }

  virtual ~TreeStruct();

private:

  int root;
  int numVertices;
  int blockSize;
  int blockNums;
  int treeNodesNums;
  int indexSize;

  std::bitset < 1 << maxBlockSize > visited;

  int *level = NULL;
  int *trace = NULL;
  int *label = NULL;
  int *parent = NULL;

  std::vector<std::vector<int>> *maskMinIndex = NULL;
  std::vector<int> *minIndexOnBlock = NULL;
  int *maskOfBlock = NULL;

  void buildSPTree(const std::vector<std::vector<Edge> >& inGrapph);

  void dfs(int u, int fa);

  int getLCA(int x, int y);

  int RMQ(int x, int y);

  void Free();
};



#endif // TREESTRUCT_H_INCLUDED
