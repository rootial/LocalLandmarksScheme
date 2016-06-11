#include "SelectLandmarks.h"

std::vector<int> dijkstra(int st, int cVertices, ConstGPtr graph) {
  std::vector<int> dist(cVertices, INF8);
  pq que;
  std::bitset<maxnode> vis;

  vis.reset();
  que.push(Node(st, 0));
  dist[st] = 0;

  while (que.size()) {
    Node x = que.top();
    que.pop();
    int u = x.u;
    if (vis[u]) {
      continue;
    }
    vis[u] = 1;
    for (auto e : graph[u]) {
      if (dist[e.v] > dist[u] + e.d) {
        dist[e.v] = dist[u] + e.d;
        que.push(Node(e.v, dist[e.v]));
      }
    }
  }
  return dist;
}

std::vector<int> selectLandmarks(ConstGPtr graph, int cVertices, int numSelected, int selectionType) {
  if (numSelected > cVertices) {
    numSelected = cVertices;
  }

  std::vector<int> selected;
  if (selectionType == 0) { // random
    std::vector<int> num;
    for (int i = 0; i < cVertices; i++) {
      num.push_back(i);
    }
    std::random_shuffle(num.begin(), num.end());

    for (int i = 0; i < numSelected; i++) {
      selected.push_back(num[i]);
    }
  } else if (selectionType == 1) { // by degrees
    for (int i = 0; i < numSelected; i++) {
      selected.push_back(i);
    }
  } else { // by closs centrality
    std::vector<int> num;
    for (int i = 0; i < cVertices; i++) {
      num.push_back(i);
    }
    std::random_shuffle(num.begin(), num.end());
    // use 100 random nodes to calculate the closs centrality
    std::vector<int> centrality(cVertices, 0);
    for (int i = 0; i < 100; i++) {
      std::vector<int> dist = dijkstra(num[i], cVertices, graph);
      for (int j = 0; j < cVertices; j++) {
        centrality[j] += dist[j];
      }
    }

    std::vector<std::pair<int, int> > rk;
    for (int i = 0; i < cVertices; i++) {
      rk.push_back(std::make_pair(centrality[i], i));
    }
    sort(rk.begin(), rk.end());

    for (int i = 0; i < numSelected; i++) {
      selected.push_back(rk[i].second);
    }
  }
  return selected;
}
