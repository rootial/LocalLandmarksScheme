#include "CommonHeader.h"
#include "GraphCompression.h"
#include "Graph.h"

int Graph::queryDistance(int x, int y) {
  if (x < 0 || y < 0 || x >= numVertices || y >= numVertices) {
    return INF8;
  }

  if (x == y) {
    return 0;
  }
  return headGraphPtr->queryDistance(x, y);
}

int Graph::constructIndex(int times, int NumSelectedLandmarks) {
  int cVertices = numVertices;
  auto ptrGraph = graph;

  GraphCompression* lastGraphPtr = NULL;

  for (int k = 0; k < times; k++) {
    GraphCompression* GraphCompressionPtr = new GraphCompression(cVertices, ptrGraph);

    if (headGraphPtr == NULL) {
      headGraphPtr = GraphCompressionPtr;
    } else {
      lastGraphPtr->nextGraph = GraphCompressionPtr;
    }
    cVertices = GraphCompressionPtr->compressGraph();

    if (k == times - 1) {
//      printf("Start to Construct Index on %dth Compressed Graph\n", index);

      GraphCompressionPtr->constructIndex(NumSelectedLandmarks);

//      printf("Finish Construct Index on %dth Compressed Graph\n", index);
    } else {
//      printf("Start to Construct Index on %dth Compressed Graph\n", index);

      GraphCompressionPtr->constructIndex(1);

//      printf("Finish Construct Index on %dth Compressed Graph\n", index);
    }

    if (lastGraphPtr != NULL) {
      DeleteArrPtr(lastGraphPtr->cGraph);
    }
//    MSG("nodes", cVertices);
    ptrGraph = GraphCompressionPtr->cGraph;
    vecGCPtr.push_back(GraphCompressionPtr);
    lastGraphPtr = GraphCompressionPtr;
  }

  // construct NumSelectedLandmarks SPTree and index on compressedGraph
  DeleteArrPtr(graph);
  return times;
}

void Graph::Free() {
  DeleteArrPtr(graph);
  DeletePtr(headGraphPtr);
}

Graph::~Graph() {
  Free();
}
