#include "CommonHeader.h"
#include "GraphCompression.h"
#include "Graph.h"

int Graph::queryDistanceLLS(int x, int y) {
  if (x < 0 || y < 0 || x >= numVertices || y >= numVertices) {
    return INF8;
  }

  if (x == y) {
    return 0;
  }

  return headGraphPtr->queryDistanceLLS(x, y);
}

int Graph::queryDistanceGLS(int x, int y) {
  if (x < 0 || y < 0 || x >= numVertices || y >= numVertices) {
    return INF8;
  }

  if (GLSSPtree.size() == 0) {
    puts("Error! GLS Index not built!");
    return INF8;
  }

  int ans = INF8;
  for (auto ptr : GLSSPtree) {
    ans = std::min(ans, ptr->queryDistanceGLS(x, y));
  }
  return ans;
}

int Graph::queryDistanceExact(int x, int y) {
  pq que1, que2;
  if (dist1 == NULL) {
    dist1 = new int[numVertices];
    dist2 = new int[numVertices];
    done1 = new int[numVertices];
    done2 = new int[numVertices];
  }

  for (int i = 0; i < numVertices; i++) {
    dist1[i] = dist2[i] = INF8;
    done1[i] = done2[i] = 0;
  }

  que1.push(Node(x, 0));
  que2.push(Node(y, 0));

  dist1[x] = 0;
  dist2[y] = 0;
  int ans = INF8;

  while (que1.size() && que2.size()) {
    Node nx, ny;
    int u = 0, v = 0;
    while (que1.size()) {
      nx = que1.top();
      que1.pop();
      u = nx.u;
      if (done1[u]) {
        continue;
      }
      break;
    }

    while (que2.size()) {
      Node ny = que2.top();
      que2.pop();
      v = ny.u;
      if (done2[v]) {
        continue;
      }
      break;
    }

    done1[u] = 1;
    done2[v] = 1;

    if (done2[u]) {
      ans = std::min(ans, dist1[u] + dist2[u]);
    }

    for (auto e : graph[u]) {
      if (dist1[e.v] > dist1[u] + e.d) {
        dist1[e.v] = dist1[u] + e.d;
        que1.push(Node(e.v, dist1[e.v]));
      }
    }

    if (done1[v]) {
      ans = std::min(ans, dist1[v] + dist2[v]);
    }

    for (auto e : graph[v]) {
      if (dist2[e.v] > dist2[v] + e.d) {
        dist2[e.v] = dist2[v] + e.d;
        que2.push(Node(e.v, dist2[e.v]));
      }
    }

    if (ans <= dist1[u] || ans <= dist2[v]) {
      break;
    }
  }

  return ans;
}

int Graph::queryDistanceExactDijk(int x, int y) {
  pq que;
  if (dist == NULL) {
    dist = new int[numVertices];
  }
  for (int v = 0; v < numVertices; v++) {
    dist[v] = INF8;
  }
  vis.reset();

  que.push(Node(x, 0));
  dist[x] = 0;

  while (que.size()) {
    Node x = que.top();
    que.pop();
    int u = x.u;
    if (vis[u] == 1) {
      continue;
    }

    vis[u] = 1;
    if (u == y) {
      break;
    }

    for (auto &e : graph[u]) {
      int v = e.v;
      if (dist[v] > dist[u] + e.d) {
        dist[v] = dist[u] + e.d;
        que.push(Node(v, dist[v]));
      }
    }
  }
  return dist[y];
}

/*int Graph::queryDistanceExact(int x, int y) {
  dist = new int[numVertices];
  vis.reset();
  for (int i = 0; i < numVertices; i++) {
    dist[i] = INF8;
  }
  std::queue<int> q;
  dist[x] = 0;
  q.push(x);
  vis[x] = 1;
  while (q.size()) {
    int u = q.front();
    q.pop();
    vis[u] = 0;
    for (auto e : graph[u]) {
      if (dist[e.v] > dist[u] + e.d) {
        dist[e.v] = dist[u] + e.d;
        if (!vis[e.v]) {
          q.push(e.v);
          vis[e.v] = 1;
        }
      }
    }
  }
  return dist[y];
}*/

bool Graph::constructIndexGLS(int NumSelectedLandmarks) {
  for (int k = 0; k < NumSelectedLandmarks; k++) {
    TreeStruct* ts = new TreeStruct(k, numVertices, graph);
    ts->constructIndexGLS();
    GLSSPtree.push_back(ts);
  }
  return true;
}

bool Graph::constructIndexLLS(int times, int NumSelectedLandmarks) {
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
      GraphCompressionPtr->constructIndexLLS(NumSelectedLandmarks);
    } else {
      GraphCompressionPtr->constructIndexLLS(1);
    }

    if (lastGraphPtr != NULL) {
      DeleteArrPtr(lastGraphPtr->cGraph);
    }

    ptrGraph = GraphCompressionPtr->cGraph;
    vecGCPtr.push_back(GraphCompressionPtr);
    lastGraphPtr = GraphCompressionPtr;
  }
  return true;
}

std::vector<std::pair<int, int> > Graph::outputEdges() {
  std::vector<std::pair<int, int> > es;
  for (int u = 0; u < numVertices; u++) {
    for (auto& e : graph[u]) {
      int v = e.v;
      es.push_back(std::pair<int, int>(u, v));
    }
  }
  return es;
}

void Graph::Free() {
  DeleteArrPtr(graph);
  DeleteArrPtr(dist);
  DeletePtr(headGraphPtr);

  DeleteArrPtr(dist1);
  DeleteArrPtr(dist2);
  DeleteArrPtr(done1);
  DeleteArrPtr(done2);

  for (auto& ptr : GLSSPtree) {
    DeletePtr(ptr);
  }
}

Graph::~Graph() {
  Free();
}
