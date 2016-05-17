#ifndef GRAPHCOMPRESSION_H_INCLUDED
#define GRAPHCOMPRESSION_H_INCLUDED
#include "CommonHeader.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

class GraphCompression {
public:
  int* rank = NULL;

  struct IndexType {
    int type;
    std::vector<PIU> attr;
    IndexType() {
      type = OtherNodeType;
    }
  };
  IndexType* nodesIndex = NULL;

  GraphCompression(int n, const std::vector<std::vector<Edge> >& g):
    numVertices(n), graph(g) {}

  int compressGraph(std::vector<std::vector<Edge> >& compressGraph);

private:
  int numVertices;
  const std::vector<std::vector<Edge> >& graph;

  void dfsGoThroughTreeNodes(int root, int u, int fa, int dep);
  void Free() {
    DeleteArrPtr(nodesIndex);
    DeleteArrPtr(rank);
  }
};

#endif // GRAPHCOMPRESSION_H_INCLUDED
