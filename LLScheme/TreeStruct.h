#ifndef TREESTRUCT_H_INCLUDED
#define TREESTRUCT_H_INCLUDED

#include "CommonHeader.h"

#include <bitset>
#include <cstdio>
#include <sys/time.h>
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

class TreeStruct {
public:
  std::vector<int> *tree = NULL;
  int *distance = NULL;

  void constructIndex();

  int queryDistance(int x, int y);

  int queryDistanceGlobal(int x, int y);

  TreeStruct(int root, int numVertices): root(root), numVertices(numVertices) {
    tree = new std::vector<int>[numVertices];
    distance = new int[numVertices];
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

  std::vector<std::vector<int>> *maskMinIndex = NULL;
  std::vector<int> *minIndexOnBlock = NULL;
  int *maskOfBlock = NULL;

  void dfs(int u, int fa);

  int getLCA(int x, int y);

  int RMQ(int x, int y);

  void Free();
};



#endif // TREESTRUCT_H_INCLUDED
