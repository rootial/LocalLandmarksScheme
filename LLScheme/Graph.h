#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "GraphCompression.h"
#include <algorithm>
#include <cstdlib>

class Graph {
public:
  int numVertices;

  int queryDistance(int x, int y);

  int constructIndex(int times, int NumSelectedLandmarks);

  Graph(const std::vector<int> &vertices, const std::vector<std::pair<int, Edge> >& edges,
        const std::vector<Edge>* iniGraph,  int* rank): rank(rank) {

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

  virtual ~Graph();

private:
  std::vector<GraphCompression*> vecGCPtr;

  GraphCompression* headGraphPtr = NULL;
  GPtr graph = NULL;
  int* rank = NULL;
  void Free();
};

#endif // GRAPH_H_INCLUDED
