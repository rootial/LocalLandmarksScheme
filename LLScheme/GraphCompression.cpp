#include "CommonHeader.h"
#include "GraphCompression.h"

#include <algorithm>
#include <vector>

void GraphCompression::dfsGoThroughTreeNodes(int root, int u, int fa, int dep) {
  for (auto& e : graph[u]) {
    int v = e.v;
    if (v != fa && nodesIndex[v].type == TreeNodeType) {
      nodesIndex[v].attr.push_back(PIU(root, dep + 1));
      dfsGoThroughTreeNodes(root, v, u, dep + 1);
    }
  }
}

int GraphCompression::compressGraph(std::vector<std::vector<Edge> >& compressGraph) {
  nodesIndex = new IndexType[numVertices];

  std::vector<int> deg(numVertices);
  std::queue<int> que;

  // remove Tree Nodes and record the distance to Entry Nodes
  for (size_t u = 0; u < numVertices; u++) {
    deg[u] = graph[u].size();
    assert(deg[u] != 0);
    if (deg[u] == 1) {
      que.push(u);
    }
  }
  int compressedGraphVertices = numVertices;
  while (que.size()) {
    int u = que.front();
    que.pop();
    if (deg[u] != 1) {
      continue;
    }
    compressedGraphVertices--;
    nodesIndex[u].type = TreeNodeType;
    deg[u] = 0;

    for (auto& e : graph[u]) {
      int v = e.v;
      if (deg[v] <= 0) {
        continue;
      }

      if (--deg[v] == 1) {
        que.push(v);
      }
    }
  }

  for (int v = 0; v < numVertices; v++) {
    if (deg[v] == 2) {
      compressedGraphVertices--;
    }
    if (nodesIndex[v].type == OtherNodeType) {
      dfsGoThroughTreeNodes(v, v, -1, 0);
    }
  }
  // remove Chain Nodes and record the distance to End Nodes and
  // can't solve graph which is a big loop
  for (int v = 0; v < numVertices; v++) {
    std::vector<int> endNodes;
    std::vector<int> adjEndNodes;

    if (deg[v] == 2 && nodesIndex[v].type == OtherNodeType) {
      que.push(v);
      while (que.size()) {
        int u = que.front();
        que.pop();
        if (nodesIndex[u].type == ChainNodeType) {
          continue;
        }
        nodesIndex[u].type = ChainNodeType;

        int isLoop = true;
        for (auto& e : graph[u]) {
          int x = e.v;
          if (nodesIndex[x].type != OtherNodeType) {
            continue;
          }

          isLoop = false;
          if (deg[x] > 2) {
            endNodes.push_back(x);
            adjEndNodes.push_back(u);
          } else {
            que.push(x);
          }
        }

        if (isLoop == true) {

          nodesIndex[u].type = OtherNodeType;
          for (auto& e : graph[u]) {
            int w = e.v;
            if (nodesIndex[w].type == TreeNodeType) {
              continue;
            }
            endNodes.push_back(u);
            adjEndNodes.push_back(w);
          }
        }
      }

      for (size_t i = 0; i < endNodes.size(); i++) {

        int end = endNodes[i];
        int last = end;
        int u = adjEndNodes[i];
        int d = 1;

        while (nodesIndex[u].type == ChainNodeType) {
          nodesIndex[u].attr.push_back(PIU(end, d));
          d++;
          for (auto& e : graph[u]) {
            int w = e.v;
            if (last != w) {
              last = u;
              u = w;
              break;
            }
          }
        }
      }
      if (!(endNodes.size() >= 1 && endNodes.size() <= 2)) {
        MSG("Error", endNodes.size());
      }
      assert(endNodes.size() >= 1 && endNodes.size() <= 2);
    }
  }

  // build compressed graph and relabel the remaining nodes



  {
    for (int v = 0; v < numVertices; v++) {
      if (nodesIndex[v].type == OtherNodeType) {
//           compressedGraphVertices++;
        for (auto& e : graph[v]) {
          int u = e.v;
          if (nodesIndex[u].type == OtherNodeType) {
            compressGraph[v].push_back(Edge(u, 1));
          } else if (nodesIndex[u].type == ChainNodeType) {
            for (size_t i = 0; i < nodesIndex[u].attr.size(); i++) {
              int w = nodesIndex[u].attr[i].first;
              int d = nodesIndex[u].attr[i].second;
              if (w != v) {
                compressGraph[v].push_back(Edge(w, d + 1));
                break;
              }
            }
          }
        }
      }

      if (compressGraph[v].size() > 0) {

        sort(compressGraph[v].begin(), compressGraph[v].end());
        compressGraph[v].erase(std::unique(compressGraph[v].begin(), compressGraph[v].end()), compressGraph[v].end());
      }
    }
  }
  return compressedGraphVertices;
}
