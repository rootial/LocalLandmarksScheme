#ifndef GRAPHCOMPRESSION_H_INCLUDED
#define GRAPHCOMPRESSION_H_INCLUDED
#include "CommonHeader.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

class GraphCompression {
public:
  // vertices of compressed graph
  int cVertices = 0;

  int* rank = NULL;
  std::vector<Edge>* cGraph = NULL;

  struct IndexType {
    int type;
    std::vector<PIU> attr;
    IndexType() {
      type = OtherNodeType;
    }
  };

  IndexType* nodesIndex = NULL;

  GraphCompression(int n, ConstGPtr g):
    numVertices(n), graph(g) {}

  int compressGraph();

  bool loadGraph(int n, ConstGPtr g) {
    if (g != NULL) {
      numVertices = n;
      graph = g;
      return true;
    }
    return false;
  }

private:

  // vertices of initial graph
  int numVertices;
  ConstGPtr graph = NULL;
  GPtr compressedGraph = NULL;

  void relabelGraph();
  void dfsGoThroughTreeNodes(int root, int u, int fa, int dep);
  void Free() {
    DeleteArrPtr(cGraph);
    DeleteArrPtr(compressedGraph);
    DeleteArrPtr(nodesIndex);
    DeleteArrPtr(rank);
  }
};

#endif // GRAPHCOMPRESSION_H_INCLUDED
