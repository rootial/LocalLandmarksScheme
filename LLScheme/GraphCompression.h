#ifndef GRAPHCOMPRESSION_H_INCLUDED
#define GRAPHCOMPRESSION_H_INCLUDED
#include "CommonHeader.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

class GraphCompression {
public:
  int numVertices;

  std::vector<std::vector<int> >& graph;

  struct IndexType {
    int type;
    std::vector<PIU> attr;
    IndexType() {
      type = 3;
    }
  };
  IndexType* nodesIndex;

  GraphCompression(int n, std::vector<std::vector<int> >&g): numVertices(n), graph(g) {}

  void compressGraph();

private:
  void dfsGoThroughTreeNodes(int root, int u, int fa, int dep);
  void Free() {
    Delete(nodesIndex);
  }
};

#endif // GRAPHCOMPRESSION_H_INCLUDED
