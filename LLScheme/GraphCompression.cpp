#include "CommonHeader.h"
#include "GraphCompression.h"

void GraphCompression::dfsGoThroughTreeNodes(int root, int u, int fa, int dep) {
  for (auto& v : graph[u]) {
    if (v != fa && nodesIndex[v].type == 0) {
      nodesIndex[v].attr.push_back(PIU(root, dep + 1));
      dfsGoThroughTreeNodes(root, v, u, dep + 1);
    }
  }
}

void GraphCompression::compressGraph() {
  nodesIndex = new IndexType[numVertices];

  std::vector<int> deg(numVertices);
  std::queue<int> que;

  for (size_t u = 0; u < graph.size(); u++) {
    deg[u] = graph[u].size();
    if (deg[u] == 1) {
      que.push(u);
    }
  }

  while (que.size()) {
    int u = que.front();
    que.pop();
    nodesIndex[u].type = 0;

    for (auto& v : graph[u]) {
      if (--deg[v] == 1) {
        que.push(v);
      }
    }
  }

  for (int v = 0; v < numVertices; v++) {
    if (nodesIndex[v].type == 3) {
      dfsGoThroughTreeNodes(v, v, -1, 0);
    }
  }
}
