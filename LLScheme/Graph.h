#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED
#include <algorithm>
#include <cstdlib>

class Graph {
public:
  int numVertices;

  int queryDistance(int x, int y);

  int constructIndex(int times);

  Graph(const std::vector<int> &vertices, const std::vector<std::pair<int, Edge> >& edges,
        const std::vector<Edge>* iniGraph,  int* rank): rank(rank) {

    numVertices = vertices.size();
    std::vector<std::pair<float, int> > deg(numVertices);
    std::bitset<maxnode> vis;
    vis.reset();
    for (auto v : vertices) {
      vis[v] = 1;
    }

    int i = 0;
    for (int v = 0; v < maxnode; v++) {
      if (!vis[v]) {
        continue;
      }
      deg[i++] = std::pair<float, int>(iniGraph[v].size() + float(rand()) / RAND_MAX, v);
    }

    std::sort(deg.rbegin(), deg.rend());

    // Relabel the vertex IDs

    for (int k = 0; k < numVertices; ++k) {
      rank[deg[k].second] = k;
    }

    graph = new std::vector<Edge>[numVertices];
    for (auto e : edges) {
      Edge E = e.second;
      int u = e.first;
      int v = E.v;
      assert(rank[u] < numVertices);
      assert(vis[v]);
      graph[rank[u]].push_back(Edge(rank[v], E.d));
    }
  }

private:

  GPtr graph = NULL;
  int* rank = NULL;
  void Free();
};

#endif // GRAPH_H_INCLUDED
