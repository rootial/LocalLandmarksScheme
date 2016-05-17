#ifndef LOCALLANDMARKSSCHEME_H_INCLUDED
#define LOCALLANDMARKSSCHEME_H_INCLUDED

#include "CommonHeader.h"
#include "GraphCompression.h"
#include "TreeStruct.h"

#include <algorithm>
#include <cstdlib>
#include <queue>
#include <vector>
#include <iostream>


template<int NumSelectedLandmarks = 10>
class LocalLandmarksScheme {
public:
  // Constructs an index from a graph, given as a list of edges.
  // Vertices should be described by numbers starting from zero.
  // Returns |true| when successful.
  bool constructIndex(const std::vector<std::pair<int, int> > &es);
  bool constructIndex(std::istream &ifs);
  bool constructIndex(const char *filename);

  // Returns distance between vertices |v| and |w| if they are connected.
  // Otherwise, returns |INT_MAX|.
  int queryDistance(int v, int w);
  int queryDistanceGlobal(int v, int w);

  // Loads an index. Returns |true| when successful.
  bool loadIndex(std::istream &ifs);
  bool loadIndex(const char *filename);

  // Stores the index. Returns |true| when successful.
  bool storeIndex(std::ostream &ofs);
  bool storeIndex(const char *filename);

  int getNumVertices() {
    return numVertices;
  }

  void Free();
  void printStatistics();

  double GetCurrentTimeSec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  }

  LocalLandmarksScheme()
    : numVertices(0), timeLoad(0), timeIndexing(0) {}
  virtual ~LocalLandmarksScheme();

private:

  int numVertices;
  std::vector<TreeStruct*> SPTree;
  int* rank = NULL;

  // Statistics
  double timeLoad, timeIndexing;
};

const int maxnode = 2000000;

template<int NumSelectedLandmarks>
bool LocalLandmarksScheme<NumSelectedLandmarks>
::constructIndex(char const *filename) {
  std::ifstream ifs(filename);
  return ifs && constructIndex(ifs);
}

template<int NumSelectedLandmarks>
bool LocalLandmarksScheme<NumSelectedLandmarks>
::constructIndex(std::istream &ifs) {
  std::vector<std::pair<int, int> > es;
  for (int v, w; ifs >> v >> w; ) {
    es.push_back(std::make_pair(v, w));
  }
  if (ifs.bad()) return false;
  constructIndex(es);
  return true;
}

template<int NumSelectedLandmarks>
bool LocalLandmarksScheme<NumSelectedLandmarks>
::constructIndex(const std::vector<std::pair<int, int> > &es) {
  //
  // Prepare the adjacency list and index space
  //
  timeLoad = -GetCurrentTimeSec();
  int &V = numVertices;
  V = 0;
  for (size_t i = 0; i < es.size(); ++i) {
    V = std::max(V, std::max(es[i].first, es[i].second) + 1);
  }

  std::vector<std::vector<int> > adj(V);
  for (size_t i = 0; i < es.size(); ++i) {
    int v = es[i].first, w = es[i].second;
    assert(w != v);
    adj[v].push_back(w);
    adj[w].push_back(v);
  }

  for (int v = 0; v < V; v++) {
    sort(adj[v].begin(), adj[v].end());
    adj[v].erase(std::unique(adj[v].begin(), adj[v].end()), adj[v].end());
  }

  timeLoad += GetCurrentTimeSec();

  // Order vertices by decreasing order of degree
  //
  timeIndexing = -GetCurrentTimeSec();

  {
    // Order
    std::vector<std::pair<float, int> > deg(V);
    for (int v = 0; v < V; ++v) {
      // We add a random value here to diffuse nearby vertices
      deg[v] = std::make_pair(adj[v].size() + float(rand()) / RAND_MAX, v);
    }
    std::sort(deg.rbegin(), deg.rend());

    // Relabel the vertex IDs
    rank = new int[V];
    for (int i = 0; i < V; ++i) rank[deg[i].second] = i;
    std::vector<std::vector<int> > new_adj(V);
    for (int v = 0; v < V; ++v) {
      for (size_t i = 0; i < adj[v].size(); ++i) {
        new_adj[rank[v]].push_back(rank[adj[v][i]]);
      }
    }
    adj.swap(new_adj);
  }
  // select NumSelectedLandmarks landmarks by degrees
  for (int root = 0; root < NumSelectedLandmarks; root++) {
    std::bitset<maxnode> vis;
    std::queue<int> que;

    que.push(root);
    TreeStruct *treeResult = new TreeStruct(root, numVertices);

    std::vector<int>* _tree = treeResult->tree;
    int* dist = treeResult->distance;
    for (int v = 0; v < numVertices; v++) {
      dist[v] = INF8;
    }
    dist[root] = 0;
    vis[root] = 1;

    while (que.size()) {
      int u = que.front();
      que.pop();
      _tree[u].clear();

      for (auto &v : adj[u]) {
        if (vis[v]) {
          continue;
        }
        dist[v] = dist[u] + 1;
        _tree[u].push_back(v);
        vis[v] = 1;
        que.push(v);
      }
    }
    //  std::cout << treeResult << std::endl;
    treeResult->constructIndex();
    SPTree.push_back(treeResult);
    Debug(root);
  }
  GraphCompression GC(numVertices, adj);
  timeIndexing += GetCurrentTimeSec();
// Debug(1);
  return true;
}

template<int NumSelectedLandmarks>
int LocalLandmarksScheme<NumSelectedLandmarks>
::queryDistance(int u, int v) {
  int distance = INF8;
  if (u >= numVertices || v >= numVertices) {
    return distance;
  }
  for (auto treeResult : SPTree) {
    int val = treeResult->TreeStruct::queryDistance(rank[u], rank[v]);
    if (distance > val) {
      distance = val;
    }
  }
  return distance;
}

template<int NumSelectedLandmarks>
int LocalLandmarksScheme<NumSelectedLandmarks>
::queryDistanceGlobal(int u, int v) {
  int distance = INF8;
  if (u >= numVertices || v >= numVertices) {
    return distance;
  }
  for (auto treeResult : SPTree) {
    int val = treeResult->queryDistanceGlobal(rank[u], rank[v]);
    if (distance > val) {
      distance = val;
    }
  }
  return distance;
}

template<int NumSelectedLandmarks>
void LocalLandmarksScheme<NumSelectedLandmarks>
::printStatistics() {
  printf("Load Graph with %d vertices\n", numVertices);
  printf("TimeLoad: %.6fs TimeIndexed: %.6fs\n", timeLoad, timeIndexing);
}

template<int NumSelectedLandmarks>
void LocalLandmarksScheme<NumSelectedLandmarks>
::Free() {
  Delete(rank);
  for (auto &ptr : SPTree) {
    delete ptr;
  }
}

template<int NumSelectedLandmarks>
LocalLandmarksScheme<NumSelectedLandmarks>
::~LocalLandmarksScheme() {
  Free();
}


#endif // LOCALLANDMARKSSCHEME_H_INCLUDED
