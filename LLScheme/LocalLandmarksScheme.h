#ifndef LOCALLANDMARKSSCHEME_H_INCLUDED
#define LOCALLANDMARKSSCHEME_H_INCLUDED

#include "CommonHeader.h"
#include "Graph.h"
#include "TreeStruct.h"
#include "SelectLandmarks.h"

#include <algorithm>
#include <cstdlib>
#include <bitset>
#include <queue>
#include <vector>
#include <iostream>

class LocalLandmarksScheme {
public:

  int NumSelectedLandmarks;
  // Constructs an index from a graph, given as a list of edges.
  // Vertices should be described by numbers starting from zero.
  // Returns |true| when successful.
  bool loadGraph(const std::vector<std::pair<int, int> > &es);
  bool loadGraph(std::istream &ifs);
  bool loadGraph(const char *filename);

  bool constructIndexLLS(int times, int selectionType);
  bool constructIndexGLS(int selectionType);

  // Returns distance between vertices |v| and |w| if they are connected.
  // Otherwise, returns |INT_MAX|.
  int queryDistanceLLS(int v, int w);
  int queryDistanceGLS(int v, int w);
  std::vector<int> queryDistance(int st);

  int queryDistanceExactDijk(int v, int w);
  int queryDistanceExact(int v, int w);

  void generateLargestComponent(const char* dataIn, const char* dataOut);

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

  double GetCurrentTimeSec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  }
  LocalLandmarksScheme()
    : numVertices(0) {}
  LocalLandmarksScheme(int num)
    : NumSelectedLandmarks(num), numVertices(0) {}
  virtual ~LocalLandmarksScheme();

private:

  int numVertices;
  Graph* graphPtr;

  std::bitset<maxnode> vis;
  GPtr iniGraph = NULL;

  int dfs(const std::vector<std::vector<Edge> >& graph, int u, int dep);

  std::vector<int> parseConnectedGraph(int root, std::vector<std::pair<int, Edge> >& edges);

  void rankNodes();

  bool isGraphConnected(ConstGPtr graph);
};

bool LocalLandmarksScheme
::loadGraph(char const *filename) {
  std::ifstream ifs(filename);
  return ifs && loadGraph(ifs);
}

bool LocalLandmarksScheme
::loadGraph(std::istream &ifs) {
  std::vector<std::pair<int, int> > es;

  ifs >> numVertices;

  for (int v, w; ifs >> v >> w; ) {
    es.push_back(std::make_pair(v, w));
  }
  puts("fuck");
  if (ifs.bad()) return false;
  loadGraph(es);
  return true;
}

bool LocalLandmarksScheme
::loadGraph(const std::vector<std::pair<int, int> > &es) {
  //
  // Prepare the adjacency list and index space
  //

  double timeLoad = -GetCurrentTimeSec();
  int V = 0;

  for (size_t i = 0; i < es.size(); ++i) {
    V = std::max(V, std::max(es[i].first, es[i].second) + 1);
  }

  if (numVertices == 0) {
    numVertices = V;
  }
//  Debug(V);
//  Debug(numVertices);
  assert(V == numVertices);

  iniGraph = new std::vector<Edge>[V];
  for (size_t i = 0; i < es.size(); ++i) {
    int v = es[i].first, w = es[i].second;
    if (w == v) {
      continue;
    }
    iniGraph[v].push_back(Edge(w, 1));
    iniGraph[w].push_back(Edge(v, 1));
  }

  // Order vertices by decreasing order of degree
  graphPtr = new Graph(V, iniGraph);
  // graphPtr->constructIndexLLS();
  timeLoad += GetCurrentTimeSec();

  printf("Load graph in time: %.6fs\n", timeLoad);
 //DeleteArrPtr(iniGraph);

  return true;
}

bool LocalLandmarksScheme
::constructIndexLLS(int times, int selectionType) {
  double timeIndex = -GetCurrentTimeSec();
  bool status = graphPtr->constructIndexLLS(times, NumSelectedLandmarks, selectionType);
  timeIndex += GetCurrentTimeSec();

  printf("constructIndexLLS status: %d, time: %.6fs\n", status, timeIndex);
  return status;
}

bool LocalLandmarksScheme
::constructIndexGLS(int selectionType) {
  double timeIndex = -GetCurrentTimeSec();
  bool status = graphPtr->constructIndexGLS(NumSelectedLandmarks, selectionType);
  timeIndex += GetCurrentTimeSec();

  printf("constructIndexGLS status: %d, time: %.6fs\n", status, timeIndex);
  return status;
}

int LocalLandmarksScheme
::queryDistanceGLS(int u, int v) {
  if (u < 0 || v < 0 || u >= numVertices || v >= numVertices) {
    return INF8;
  }
  if (u == v) {
    return 0;
  }
  return  graphPtr->queryDistanceGLS(u, v);
}

int LocalLandmarksScheme
::queryDistanceLLS(int u, int v) {
  if (u < 0 || v < 0 || u >= numVertices || v >= numVertices) {
    return INF8;
  }
  if (u == v) {
    return 0;
  }
  return  graphPtr->queryDistanceLLS(u, v);
}

std::vector<int> LocalLandmarksScheme
::queryDistance(int st) {
  return dijkstra(st, numVertices, iniGraph);
}

int LocalLandmarksScheme
::queryDistanceExactDijk(int u, int v) {
  if (u < 0 || v < 0 || u >= numVertices || v >= numVertices) {
    return INF8;
  }
  if (u == v) {
    return 0;
  }
  return graphPtr->queryDistanceExactDijk(u, v);
}

int LocalLandmarksScheme
::queryDistanceExact(int u, int v) {
  if (u < 0 || v < 0 || u >= numVertices || v >= numVertices) {
    return INF8;
  }
  if (u == v) {
    return 0;
  }
  return graphPtr->queryDistanceExact(u, v);
}

void LocalLandmarksScheme
::generateLargestComponent(const char* dataIn, const char* dataOut) {
  std::ifstream ifs(dataIn);

  std::vector<std::pair<int, int> > es;
  int V = 0;

  for (int v, w; ifs >> v >> w ;) {
    es.push_back(std::pair<int, int>(v, w));
    V = std::max(V, std::max(v, w) + 1);
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

  // remove the duplicate edges between two nodes
  // remain the shortest edge
  for (int v = 0; v < V; v++) {
    if (iniGraph[v].size() == 0) {
      continue;
    }
    sort(iniGraph[v].begin(), iniGraph[v].end());
    iniGraph[v].erase(std::unique(iniGraph[v].begin(), iniGraph[v].end()), iniGraph[v].end());

    for (size_t i = 1; i < iniGraph[v].size(); i++) {
      assert(iniGraph[v][i].v != iniGraph[v][i - 1].v);
    }
  }

  int* rank = new int[V];
  std::vector<Graph*> GraphPtrVec;
  int mx = 0;

  // Order vertices by decreasing order of degree
  {
    for (int v = 0; v < V; v++) {
      rank[v] = -1;
    }

    vis.reset();
    int cnt = 0;

    for (int v = 0; v < V; v++) {
      if (vis[v] == 0 && iniGraph[v].size() > 0) {
        std::vector<std::pair<int, Edge> > edges;
        std::vector<int> vertices = parseConnectedGraph(v, edges);

        Graph* graphPtr = new Graph(vertices, edges, iniGraph, rank);
        GraphPtrVec.push_back(graphPtr);
        if (GraphPtrVec[mx]->numVertices < GraphPtrVec.back()->numVertices) {
          mx = GraphPtrVec.size() - 1;
        }
        cnt++;
      }
    }
  }

  V = 0;
  es = GraphPtrVec[mx]->outputEdges();
  for (auto& e : es) {
    if (e.first > e.second) {
      std::swap(e.first, e.second);
    }
    V = std::max(V, e.second);
  }
  sort(es.begin(), es.end());
  es.erase(unique(es.begin(), es.end()), es.end());

  std::ofstream ofs(dataOut);

  ofs << V + 1 << std::endl;

  for (auto e : es) {
    ofs << e.first << ' ' << e.second << std:: endl;
  }

  printf("Parse largest component with %d nodes\n", V + 1);

  for (auto &ptr : GraphPtrVec) {
    DeletePtr(ptr);
  }
  DeleteArrPtr(iniGraph);
  DeleteArrPtr(rank);
}

std::vector<int> LocalLandmarksScheme
::parseConnectedGraph(int root, std::vector<std::pair<int, Edge> >& edges) {
  std::queue<int> que;
  que.push(root);
  vis[root] = 1;
  std::vector<int> vec;

  while (!que.empty()) {
    int u = que.front();
    que.pop();
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

int LocalLandmarksScheme
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

void LocalLandmarksScheme
::rankNodes() {
  int* rank = new int[numVertices];

  std::vector<std::pair<float, int> > deg(numVertices);

  int verticesCnt = 0;
  for (int v = 0; v < numVertices; ++v) {
    // We add a random value here to diffuse nearby vertices
    if (iniGraph[v].size() > 0) {
      deg[verticesCnt++] = std::make_pair(iniGraph[v].size() + rand() / (RAND_MAX + 1.0), v);
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

  DeleteArrPtr(rank);
}

bool LocalLandmarksScheme
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
  return cnt == numVertices;
}

void LocalLandmarksScheme
::Free() {
  DeleteArrPtr(iniGraph);
  DeletePtr(graphPtr);
}

LocalLandmarksScheme
::~LocalLandmarksScheme() {
  Free();
}


#endif // LOCALLANDMARKSSCHEME_H_INCLUDED
