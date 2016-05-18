#ifndef LOCALLANDMARKSSCHEME_H_INCLUDED
#define LOCALLANDMARKSSCHEME_H_INCLUDED

#include "CommonHeader.h"
#include "GraphCompression.h"
#include "Graph.h"
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
  int compressedGraphEdges = 0;
  std::vector<Graph*> GraphPtrVec;
  int* rank = NULL;
  int* whichComponent = NULL;
  std::bitset<maxnode> vis;
  GPtr iniGraph = NULL;

  // Statistics
  double timeLoad, timeIndexing;

  int dfs(const std::vector<std::vector<Edge> >& graph, int u, int dep);

  std::vector<int> parseConnectedGraph(int root, std::vector<std::pair<int, Edge> >& edges, int component);

  void rankNodes();

  bool isGraphConnected(ConstGPtr graph);
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

  iniGraph = new std::vector<Edge>[V];
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

  timeLoad += GetCurrentTimeSec();

  // Order vertices by decreasing order of degree

  timeIndexing = -GetCurrentTimeSec();
  {
    whichComponent = new int[V];
    rank = new int[V];
    for (int v = 0; v < V; v++) {
      rank[v] = -1;
      whichComponent[v] = -1;
    }

    vis.reset();
    int componentCnt = 0;
    int mx = 0;
    for (int v = 0; v < V; v++) {

      if (vis[v] == 0 && iniGraph[v].size() > 0) {
//        Debug(v);
        std::vector<std::pair<int, Edge> > edges;
        std::vector<int> vertices = parseConnectedGraph(v, edges, componentCnt);
        componentCnt++;
        Graph* graphPtr = new Graph(vertices, edges, iniGraph, rank);
        GraphPtrVec.push_back(graphPtr);
        if (GraphPtrVec[mx]->numVertices < GraphPtrVec.back()->numVertices) {
          mx = GraphPtrVec.size() - 1;
        }
      }
    }
    GraphPtrVec[0]->constructIndex(2);
    MSG("Parse graph: ", GraphPtrVec.size());
  }

  timeIndexing += GetCurrentTimeSec();
  printStatistics();
  return true;
}

template<int NumSelectedLandmarks>
int LocalLandmarksScheme<NumSelectedLandmarks>
::queryDistance(int u, int v) {
  int distance = INF8;
  if (u >= numVertices || v >= numVertices) {
    return distance;
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
std::vector<int> LocalLandmarksScheme<NumSelectedLandmarks>
::parseConnectedGraph(int root, std::vector<std::pair<int, Edge> >& edges, int component) {
  std::queue<int> que;
  que.push(root);
  vis[root] = 1;
  std::vector<int> vec;

  while (!que.empty()) {
    int u = que.front();
    que.pop();
    whichComponent[u] = component;
    vec.push_back(u);

    for (auto e : iniGraph[u]) {
      edges.push_back(std::pair<int, Edge>(u, e));
      if (vis[e.v] != 1) {
        que.push(e.v);
        vis[e.v] = 1;
      }
    }
  }
  return vec;
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
void LocalLandmarksScheme<NumSelectedLandmarks>
::rankNodes() {
  std::vector<std::pair<float, int> > deg(numVertices);

  int verticesCnt = 0;
  for (int v = 0; v < numVertices; ++v) {
    // We add a random value here to diffuse nearby vertices
    if (iniGraph[v].size() > 0) {
      deg[verticesCnt++] = std::make_pair(iniGraph[v].size() + float(rand()) / RAND_MAX, v);
    }
  }

  std::sort(deg.rbegin(), deg.rend());

  rank = new int[numVertices];
  for (int i = 0; i < numVertices; i++) {
    rank[i] = -1;
  }
  for (int i = 0; i < verticesCnt; ++i) {
    rank[deg[i].second] = i;
  }
}

template<int NumSelectedLandmarks>
bool LocalLandmarksScheme<NumSelectedLandmarks>
::isGraphConnected(ConstGPtr graph) {
  std::queue<int> que;
  que.push(0);
  int cnt = 1;
  vis.reset();
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
//  Debug(cnt);
  return cnt == numVertices;
}

template<int NumSelectedLandmarks>
void LocalLandmarksScheme<NumSelectedLandmarks>
::Free() {
  DeleteArrPtr(rank);
  DeleteArrPtr(iniGraph);
  DeleteArrPtr(whichComponent);

  for (auto &ptr : GraphPtrVec) {
    DeletePtr(ptr);
  }
}

template<int NumSelectedLandmarks>
LocalLandmarksScheme<NumSelectedLandmarks>
::~LocalLandmarksScheme() {
  Free();
}


#endif // LOCALLANDMARKSSCHEME_H_INCLUDED
