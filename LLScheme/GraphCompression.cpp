#include "CommonHeader.h"
#include "GraphCompression.h"

#include <algorithm>
#include <vector>

// remove empty vertices and rank
// them by degrees, relabel them
void GraphCompression::relabelGraph() {
  cGraph = new std::vector<Edge>[cVertices];
  std::vector<std::pair<float, int> > deg(cVertices);

  int verticesCnt = 0;

  for (int v = 0; v < numVertices; ++v) {
    // We add a random value here to diffuse nearby vertices
    if (compressedGraph[v].size() > 0) {
      deg[verticesCnt++] = std::make_pair(compressedGraph[v].size() + float(rand()) / RAND_MAX, v);
    }
  }
  std::sort(deg.rbegin(), deg.rend());

  // Relabel the vertex IDs
  rank = new int[numVertices];
  for (int i = 0; i < numVertices; i++) {
    rank[i] = -1;
  }
  for (int i = 0; i < cVertices; ++i) {
    rank[deg[i].second] = i;
  }
  for (int v = 0; v < numVertices; ++v) {
    for (auto& e : compressedGraph[v]) {
      cGraph[rank[v]].push_back(Edge(rank[e.v], e.d));
    }
  }
}

void GraphCompression::dfsGoThroughTreeNodes(int root, int u, int fa, int dep) {
  for (auto& e : graph[u]) {
    int v = e.v;
    if (v != fa && nodesIndex[v].type == TreeNodeType) {
//      if (numVertices == 36315) {
//        Debug(nodesIndex[v].type);
//      }
      nodesIndex[v].attr.push_back(PIU(root, dep + e.d));
      dfsGoThroughTreeNodes(root, v, u, dep + e.d);
    }
  }
}

int GraphCompression::compressGraph() {
  nodesIndex = new IndexType[numVertices];
  compressedGraph = new std::vector<Edge>[numVertices];
  std::vector<int> deg(numVertices);
  std::queue<int> que;

  // remove Tree Nodes and record the distance to Entry Nodes
  {
    Debug(numVertices);
    for (int u = 0; u < numVertices; u++) {
      deg[u] = graph[u].size();
      // assert(deg[u] != 0);
      if (deg[u] == 1) {
        que.push(u);
      }
    }

    while (que.size() != 0) {
      int u = que.front();
      que.pop();
      if (deg[u] != 1) {
        continue;
      }
//    compressedGraphVertices--;
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
//    if (numVertices == 36315) {
//      for (int v = 0; v < numVertices; v++) {
//        assert(nodesIndex[v].type != TreeNodeType);
//      }
//    }
    for (int v = 0; v < numVertices; v++) {
      if (deg[v] != 0) {
        assert(deg[v] > 1);
      }
      if (nodesIndex[v].type == OtherNodeType) {
        dfsGoThroughTreeNodes(v, v, -1, 0);
      }
    }
  }

  // remove Chain Nodes and record the distance to End Nodes and
  {

    for (int v = 0; v < numVertices; v++) {
      std::vector<std::pair<Edge, int> > endEdges;

      if (deg[v] == 2 && nodesIndex[v].type == OtherNodeType) {
        que.push(v);
        nodesIndex[v].type = ChainNodeType;
        int u;

        while (que.size()) {
          u = que.front();
          que.pop();

          for (auto& e : graph[u]) {
            int x = e.v;
            if (nodesIndex[x].type != OtherNodeType) {
              continue;
            }

            if (deg[x] > 2) {
              endEdges.push_back(std::pair<Edge, int>(e, u));
            } else {
              que.push(x);
              nodesIndex[x].type = ChainNodeType;
            }
          }
        }

        if (endEdges.size() == 0) {
          nodesIndex[u].type = OtherNodeType;
          for (auto& e : graph[u]) {
            int w = e.v;
            if (nodesIndex[w].type == TreeNodeType) {
              continue;
            }
            endEdges.push_back(std::pair<Edge, int>(Edge(u, e.d), w));
          }
        }
        assert(endEdges.size() == 2);

        for (size_t i = 0; i < endEdges.size(); i++) {

          int end = endEdges[i].first.v;
          int last = end;
          int u = endEdges[i].second;
          int d = endEdges[i].first.d;

          while (nodesIndex[u].type == ChainNodeType) {
            nodesIndex[u].attr.push_back(PIU(end, d));
            for (auto& e : graph[u]) {
              int w = e.v;
              if (nodesIndex[w].type == TreeNodeType) {
                continue;
              }
              if (last != w) {
                last = u;
                u = w;
                d += e.d;
                break;
              }
            }
          }
        }
      }
    }
  }

  // build compressed graph and relabel the remaining nodes
  {
    for (int v = 0; v < numVertices; v++) {
      if (nodesIndex[v].type == OtherNodeType) {
//      compressedGraphVertices++;
        for (auto& e : graph[v]) {
          int u = e.v;
          if (nodesIndex[u].type == OtherNodeType) {
            compressedGraph[v].push_back(e);
          } else if (nodesIndex[u].type == ChainNodeType) {
            int d = nodesIndex[u].attr[0].second + nodesIndex[u].attr[1].second;
            for (size_t i = 0; i < nodesIndex[u].attr.size(); i++) {
              int w = nodesIndex[u].attr[i].first;
              if (w != v) {
                compressedGraph[v].push_back(Edge(w, d));
                break;
              }
            }
          }
        }
        if (compressedGraph[v].size() > 0) {
          cVertices++;
          sort(compressedGraph[v].begin(), compressedGraph[v].end());
          compressedGraph[v].erase(std::unique(compressedGraph[v].begin(), compressedGraph[v].end()),
                                   compressedGraph[v].end());
        }
      }
    }
  }

  relabelGraph();

  return cVertices;
}
