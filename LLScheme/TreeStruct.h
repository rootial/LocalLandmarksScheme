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

class TreeStruct {
public:
  std::vector<int> *tree = NULL;

  void constructIndexLLS();

  void constructIndexGLS();

  int distanceOfNode(int x);

  int queryDistanceGLS(int x, int y);

  int queryDistanceLLS(int x, int y);

  TreeStruct(int root, int numVertices,
             ConstGPtr g): graph(g), root(root), numVertices(numVertices) {

  }

  virtual ~TreeStruct();

private:
  ConstGPtr graph;
  std::bitset < 1 << maxBlockSize > vis;

  int root;
  int numVertices;
  int blockSize;
  int blockNums;
  int treeNodesNums;
  int indexSize;

  int *distance = NULL;
  int *level = NULL;
  int *label = NULL;
  int *parent = NULL;

  std::vector<std::vector<uint8_t>> *maskMinIndex = NULL;
  std::vector<int> *minIndexOnBlock = NULL;
  short *maskOfBlock = NULL;

  void buildSPTree();

  void dfs(int u, int fa);

  int getLcaDistance(int x, int y);

  int RMQ(int x, int y);

  void Free();
};



#endif // TREESTRUCT_H_INCLUDED
