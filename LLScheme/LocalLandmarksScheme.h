#ifndef LOCALLANDMARKSSCHEME_H_INCLUDED
#define LOCALLANDMARKSSCHEME_H_INCLUDED

#include "CommonHeader.h"
#include "GraphCompression.h"
#include "TreeStruct.h"

#include <algorithm>
#include <cstdlib>
#include <bitset>
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
  std::bitset<maxnode> vis;

  // Statistics
  double timeLoad, timeIndexing;

  int dfs(const std::vector<std::vector<Edge> >& graph, int u, int dep);

  bool isGraphConnected(const std::vector<std::vector<Edge> >& graph);
};

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

  std::vector<std::vector<Edge> > iniGraph(V);
  for (size_t i = 0; i < es.size(); ++i) {
    int v = es[i].first, w = es[i].second;
    if (w == v) {
      continue;
    }
    iniGraph[v].push_back(Edge(w, 1));
    iniGraph[w].push_back(Edge(v, 1));
  }

  for (int v = 0; v < V; v++) {
    sort(iniGraph[v].begin(), iniGraph[v].end());
    iniGraph[v].erase(std::unique(iniGraph[v].begin(), iniGraph[v].end()), iniGraph[v].end());

    for (size_t i = 1; i < iniGraph[v].size(); i++) {
      assert(iniGraph[v][i].v != iniGraph[v][i - 1].v);
    }
  }

//  assert(isGraphConnected(iniGraph) == true);
  timeLoad += GetCurrentTimeSec();

  // Order vertices by decreasing order of degree

  timeIndexing = -GetCurrentTimeSec();
  {
    // Order
    GraphCompression GC(numVertices, iniGraph);

    std::vector<std::vector<Edge> > compressedGraph(numVertices);
    int compressedGraphVertices = GC.compressGraph(compressedGraph);
    MSG("Compressed Nodes to: ", compressedGraphVertices);
    return true;

    std::vector<std::pair<float, int> > deg(compressedGraphVertices);
    int &VN = compressedGraphVertices;

    for (int v = 0; v < VN; ++v) {
      // We add a random value here to diffuse nearby vertices
      if (compressedGraph[v].size() > 0) {
        deg[v] = std::make_pair(compressedGraph[v].size() + float(rand()) / RAND_MAX, v);
      }
    }
    std::sort(deg.rbegin(), deg.rend());

    // Relabel the vertex IDs
    rank = new int[VN];
    for (int i = 0; i < VN; ++i) rank[deg[i].second] = i;
    std::vector<std::vector<Edge> > relabelGraph(VN);
    for (int v = 0; v < VN; ++v) {
      for (auto& e : compressedGraph[v]) {
        relabelGraph[rank[v]].push_back(Edge(rank[e.v], e.d));
      }
    }

    // construct NumSelectedLandmarks SPTree and index on compressedGraph
    // except the first one built on iniGraph
    {
      // build SPTree on iniGraph
      TreeStruct* ts = new TreeStruct(0, numVertices);
      ts->constructIndex(iniGraph);
      SPTree.push_back(ts);

      // build NumSelectedLandmarks - 1 SPTree on compressedGraph
      // select NumSelectedLandmarks landmarks by degrees

      for (int i = 0; i < NumSelectedLandmarks - 1; i++) {
        ts = new TreeStruct(i, compressedGraphVertices);
        ts->constructIndex(compressedGraph);
        SPTree.push_back(ts);
      }
    }
  }

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
void LocalLandmarksScheme<NumSelectedLandmarks>
::printStatistics() {
  printf("Load Graph with %d vertices\n", numVertices);
  printf("TimeLoad: %.6fs TimeIndexed: %.6fs\n", timeLoad, timeIndexing);
}
template<int NumSelectedLandmarks>
int LocalLandmarksScheme<NumSelectedLandmarks>
::dfs(const std::vector<std::vector<Edge> >& graph, int u, int dep) {
  int cnt = 1;
  vis[u] = 1;
//  assert(dep < 1000);
  for (auto& e : graph[u]) {
    int v = e.v;
    if (!vis[v]) {
      cnt += dfs(graph, v, dep + 1);
    }
  }
  return cnt;
}

template<int NumSelectedLandmarks>
bool LocalLandmarksScheme<NumSelectedLandmarks>
::isGraphConnected(const std::vector<std::vector<Edge> >& graph) {
  std::queue<int> que;
  que.push(0);
  int cnt = 1;
  vis[0] = 1;
  while (que.size()) {
    int u = que.front();
    que.pop();
    for (auto& e : graph[u]) {
      int v = e.v;
      if (vis[v]) {
        continue;
      }
      cnt++;
      vis[v] = 1;
      que.push(v);
    }
  }
  Debug(cnt);
  return cnt == 77360;
}

template<int NumSelectedLandmarks>
void LocalLandmarksScheme<NumSelectedLandmarks>
::Free() {
  DeleteArrPtr(rank);
  for (auto &ptr : SPTree) {
    DeletePtr(ptr);
  }
}

template<int NumSelectedLandmarks>
LocalLandmarksScheme<NumSelectedLandmarks>
::~LocalLandmarksScheme() {
  Free();
}


#endif // LOCALLANDMARKSSCHEME_H_INCLUDED
