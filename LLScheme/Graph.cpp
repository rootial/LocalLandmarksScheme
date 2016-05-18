#include "CommonHeader.h"
#include "GraphCompression.h"
#include "Graph.h"

int Graph::queryDistance(int x, int y) {
  return 1;
}

int Graph::constructIndex(int times) {

  GraphCompression GC(numVertices, graph);
  int cVertices = GC.compressGraph();
  auto ptrGraph = GC.cGraph;
  printf("Compressed to %d nodes in %d round\n", cVertices, 0);
  std::vector<GraphCompression*> vecGCPtr;

  for (int k = 0; k < times; k++) {
    GraphCompression* GCPtr = new GraphCompression(cVertices, ptrGraph);

    cVertices = GCPtr->compressGraph();
//      Debug(k);
    printf("Compressed to %d nodes in %d round\n", cVertices, k + 1);
    ptrGraph = GCPtr->cGraph;
    vecGCPtr.push_back(GCPtr);
  }

  for (int k = 0; k < times; k++) {
    delete vecGCPtr[k];
  }
//  // construct NumSelectedLandmarks SPTree and index on compressedGraph
//  // except the first one built on iniGraph
//  {
//    // build SPTree on iniGraph
//    TreeStruct* ts = new TreeStruct(0, numVertices, iniGraph);
//    ts->constructIndex();
//    SPTree.push_back(ts);
//
//    // build NumSelectedLandmarks - 1 SPTree on compressedGraph
//    // select NumSelectedLandmarks landmarks by degrees
//
//    for (int i = 0; i < NumSelectedLandmarks - 1; i++) {
//      ts = new TreeStruct(i, compressedGraphVertices, relabelGraph);
//      ts->constructIndex();
//      SPTree.push_back(ts);
//    }
//  }
  return times;
}

void Graph::Free() {
  DeleteArrPtr(graph);
}
