#ifndef GRAPHCOMPRESSION_H_INCLUDED
#define GRAPHCOMPRESSION_H_INCLUDED
#include "CommonHeader.h"
#include "TreeStruct.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

class GraphCompression {
public:
  // vertices of compressed graph
  int cVertices = 0;
  int chainCnt = 0;

  int* rank = NULL;
  std::vector<Edge>* cGraph = NULL;
  std::vector<TreeStruct*> SPTree;
  const GraphCompression* nextGraph = NULL;

  struct IndexType {
    int type;
    int chainNo;
    std::vector<Edge> attr;
    IndexType() {
      type = OtherNodeType;
    }
  };

  IndexType* nodesIndex = NULL;

  GraphCompression(int n, ConstGPtr g):
    numVertices(n), graph(g) {
    chainCnt = 0;
  }

  int compressGraph();

  int queryDistanceLLS(int x, int y) const;

  int queryDistanceOnNextCompressedGraph(int x, int y) const;

  void constructIndexLLS(int num);

  bool loadGraph(int n, ConstGPtr g) {
    if (g != NULL) {
      numVertices = n;
      graph = g;
      return true;
    }
    return false;
  }

  bool setNextGraph(const GraphCompression* ptr) {
    nextGraph = ptr;
    return ptr != NULL;
  }

  ~GraphCompression();

private:
  // vertices of initial graph
  int numVertices;
  ConstGPtr graph = NULL;
  GPtr compressedGraph = NULL;

  void relabelGraph();
  void dfsGoThroughTreeNodes(int root, int u, int fa, int dep);
  void Free();
};

#endif // GRAPHCOMPRESSION_H_INCLUDED
