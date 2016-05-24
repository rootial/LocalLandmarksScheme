#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "GraphCompression.h"
#include <algorithm>
#include <cstdlib>

class Graph {
public:
  int numVertices;

  int queryDistanceLLS(int x, int y);

  int queryDistanceGLS(int x, int y);

  int queryDistanceExactDijk(int x, int y);

  int queryDistanceExact(int x, int y);

  bool constructIndexLLS(int times, int NumSelectedLandmarks);

  bool constructIndexGLS(int NumSelectedLandmarks);

  std::vector<std::pair<int, int> > outputEdges();

  Graph(const std::vector<int> &vertices, const std::vector<std::pair<int, Edge> >& edges,
        const std::vector<Edge>* iniGraph,  int* rank) {

    numVertices = vertices.size();
    std::vector<std::pair<float, int> > deg(numVertices);

    int i = 0;

    for (auto v : vertices) {
      deg[i++] = std::pair<float, int>(iniGraph[v].size() + rand() / (RAND_MAX + 1.0), v);
    }

    std::sort(deg.rbegin(), deg.rend());

    // Relabel the vertex IDs of of this graph
    for (int k = 0; k < numVertices; ++k) {
      rank[deg[k].second] = k;
    }

    graph = new std::vector<Edge>[numVertices];

    for (auto e : edges) {
      Edge E = e.second;
      int u = e.first;
      int v = E.v;
      assert(rank[u] < numVertices);
      graph[rank[u]].push_back(Edge(rank[v], E.d));
    }
  }

  Graph(int num, ConstGPtr iniGraph): numVertices(num) {
    graph = new std::vector<Edge>[numVertices];
    for (int u = 0; u < numVertices; u++) {
      for (auto e : iniGraph[u]) {
        graph[u].push_back(e);
      }
    }
  }

  virtual ~Graph();

private:
  std::vector<GraphCompression*> vecGCPtr;
  std::vector<TreeStruct*> GLSSPtree;

  int* dist = NULL;
  int* dist1 = NULL;
  int* dist2 = NULL;
  int* done1 = NULL;
  int* done2 = NULL;

  std::bitset<maxnode> vis;

  GraphCompression* headGraphPtr = NULL;
  GPtr graph = NULL;

  void Free();
};

#endif // GRAPH_H_INCLUDED
