#include "CommonHeader.h"
#include "GraphCompression.h"

#include <algorithm>
#include <vector>

// remove empty vertices and rank
// them by degrees, relabel them

int GraphCompression::queryDistanceOnNextCompressedGraph(int x, int y) const {
  int ans = INF8;
  if (nextGraph == NULL) {
    for (size_t i = 1; i < SPTree.size(); i++) {
      ans = std::min(ans, SPTree[i]->queryDistance(x, y));
    }
  } else {
    ans = std::min(ans, nextGraph->queryDistance(x, y));
  }
  return ans;
}

int GraphCompression::queryDistance(int x, int y) const {
  if (x < 0 || y < 0 || x >= numVertices || y >= numVertices) {
    return INF8;
  }

  if (x == y) {
    return 0;
  }

  int ans = INF8;
  if (nodesIndex[x].type == TreeNodeType && nodesIndex[y].type == TreeNodeType) {
    // x, y is on the same incident tree and calculate the
    // distance by index on uncompressed Graph
    if (nodesIndex[x].attr[0].v == nodesIndex[y].attr[0].v) {
      ans = SPTree[0]->queryDistance(x, y);
    } else {
      int nx = rank[nodesIndex[x].attr[0].v];
      int ny = rank[nodesIndex[y].attr[0].v];
      ans = nodesIndex[x].attr[0].d + nodesIndex[y].attr[0].d + queryDistanceOnNextCompressedGraph(nx, ny);
    }
  } else {
    for (auto& attrx : nodesIndex[x].attr) {
      for (auto& attry : nodesIndex[y].attr) {
        int ex = attrx.v;
        int ey = attry.v;
        ans = std::min(ans, attrx.d + attry.d + queryDistanceOnNextCompressedGraph(rank[ex], rank[ey]));
      }
    }
    if (nodesIndex[x].type == ChainNodeType && nodesIndex[y].type == ChainNodeType &&
        nodesIndex[x].chainNo == nodesIndex[y].chainNo) {
      for (int i = 0; i < 2; i++) {
        int r = std::abs(nodesIndex[x].attr[i].d - nodesIndex[y].attr[i].d);
        ans = std::min(ans, r);
      }
    }
  }
  return ans;
}

void GraphCompression::constructIndex(int NumSelectedLandmarks) {
  TreeStruct* ts = new TreeStruct(0, numVertices, graph);
  ts->constructIndex();
  SPTree.push_back(ts);

  // build NumSelectedLandmarks - 1 SPTree on compressedGraph
  // select NumSelectedLandmarks landmarks by degrees
  for (int i = 0; i < std::min(NumSelectedLandmarks - 1, cVertices); i++) {
    ts = new TreeStruct(i, cVertices, cGraph);
    ts->constructIndex();
    SPTree.push_back(ts);
  }
}

// rank vertices by degrees and relabel the compressed graph using the
// new node label
void GraphCompression::relabelGraph() {
  cGraph = new std::vector<Edge>[cVertices];
  std::vector<std::pair<float, int> > deg(cVertices);

  int verticesCnt = 0;

  for (int v = 0; v < numVertices; ++v) {
    // We add a random value here to diffuse nearby vertices
    if (nodesIndex[v].type == OtherNodeType) {
      deg[verticesCnt++] = std::make_pair(compressedGraph[v].size() + rand() / (RAND_MAX + 1.0), v);
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
  DeleteArrPtr(compressedGraph);
}

// go through all tree nodes, record the entry node and distance to it
void GraphCompression::dfsGoThroughTreeNodes(int root, int u, int fa, int dep) {
  for (auto& e : graph[u]) {
    int v = e.v;
    if (v != fa && nodesIndex[v].type == TreeNodeType) {
      nodesIndex[v].attr.push_back(Edge(root, dep + e.d));
      dfsGoThroughTreeNodes(root, v, u, dep + e.d);
    }
  }
}

// remove tree node and chain node, connect the end node of each chain
// relabel the compressed graph by degrees
int GraphCompression::compressGraph() {
  chainCnt = 0;

  nodesIndex = new IndexType[numVertices];
  compressedGraph = new std::vector<Edge>[numVertices];
  std::vector<int> deg(numVertices);
  std::queue<int> que;

  // remove Tree Nodes and record the distance to Entry Nodes
  {
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
    for (int v = 0; v < numVertices; v++) {
      if (deg[v] != 0) {
        assert(deg[v] > 1 && nodesIndex[v].type == OtherNodeType);
      }
      if (nodesIndex[v].type == OtherNodeType) {
        dfsGoThroughTreeNodes(v, v, -1, 0);
      }
    }
  }
  int treeNodes = 0;

  for (int v = 0; v < numVertices; v++) {
    treeNodes += nodesIndex[v].type == TreeNodeType;
    deg[v] = graph[v].size();
  }

  // remove Chain Nodes and record the distance to End Nodes and
  if (treeNodes != numVertices - 1) {

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

        // solve the big loop case
        if (endEdges.size() == 0) {
          nodesIndex[u].type = OtherNodeType;
          for (auto& e : graph[u]) {
            int w = e.v;
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
            nodesIndex[u].attr.push_back(Edge(end, d));
            nodesIndex[u].chainNo = chainCnt;
            for (auto& e : graph[u]) {
              int w = e.v;
              if (last != w) {
                last = u;
                u = w;
                d += e.d;
                break;
              }
            }
          }
        }
        chainCnt++;
      }
    }
  }

  // build compressed graph and relabel the remaining nodes
  {
    for (int v = 0; v < numVertices; v++) {
      if (nodesIndex[v].type == OtherNodeType) {
        cVertices++;

        for (auto& e : graph[v]) {
          int u = e.v;
          if (nodesIndex[u].type == OtherNodeType) {
            compressedGraph[v].push_back(e);
          } else if (nodesIndex[u].type == ChainNodeType) {
            int d = nodesIndex[u].attr[0].d + nodesIndex[u].attr[1].d;
            for (size_t i = 0; i < nodesIndex[u].attr.size(); i++) {
              int w = nodesIndex[u].attr[i].v;
              if (w != v) {
                compressedGraph[v].push_back(Edge(w, d));
                break;
              }
            }
          }
        }
        if (compressedGraph[v].size() > 0) {

          sort(compressedGraph[v].begin(), compressedGraph[v].end());
          compressedGraph[v].erase(std::unique(compressedGraph[v].begin(), compressedGraph[v].end()),
                                   compressedGraph[v].end());
        }
      }

/*      // order the end nodes
      if (nodesIndex[v].type == ChainNodeType) {
        if (nodesIndex[v].attr[1].v < nodesIndex[v].attr[0].v) {
          std::swap(nodesIndex[v].attr[0], nodesIndex[v].attr[1]);
        }
      }*/
    }
    // add the the remaing nodes as representative of itself
    for (int v = 0; v < numVertices; v++) {
      if (nodesIndex[v].type == OtherNodeType) {
        assert(nodesIndex[v].attr.size() == 0);
        nodesIndex[v].attr.push_back(Edge(v, 0));
      }
    }
  }

  relabelGraph();

  return cVertices;
}

void GraphCompression::Free() {
  DeleteArrPtr(cGraph);
  DeleteArrPtr(compressedGraph);
  DeleteArrPtr(nodesIndex);
  DeleteArrPtr(rank);
  DeletePtr(nextGraph);
  for (auto &ptr : SPTree) {
    DeletePtr(ptr);
  }
}

void GraphCompression::FreeNoUseMem() {
  DeleteArrPtr(graph);
}

GraphCompression::~GraphCompression() {
  Free();
}
