#include "CommonHeader.h"
#include "TreeStruct.h"

// build SPTree using Dijkstra
void TreeStruct::buildSPTree() {
  std::bitset<maxnode> vis;
  pq que;

  for (int v = 0; v < numVertices; v++) {
    distance[v] = INF8;
  }
  vis.reset();

  que.push(Node(root, 0));
  distance[root] = 0;

  while (que.size()) {
    Node x = que.top();
    que.pop();
    int u = x.u;
    if (vis[u] == 1) {
      continue;
    }

    vis[u] = 1;
    if (u != root) {
      if (tree != NULL) {
        tree[parent[u]].push_back(u);
      }
    }

    for (auto &e : graph[u]) {
      int v = e.v;
      if (distance[v] > distance[u] + e.d) {
        distance[v] = distance[u] + e.d;
        if (parent != NULL) {
          parent[v] = u;
        }
        que.push(Node(v, distance[v]));
      }
    }
  }
}

void TreeStruct::dfs(int u, int fa) {
  label[u] = indexSize;
  level[indexSize++] = distance[u];
  treeNodesNums++;
  for (auto &v : tree[u]) {
    if (v == fa) {
      continue;
    }
    dfs(v, u);
    level[indexSize++] = distance[u];
  }
}

void TreeStruct::constructIndexGLS() {
  // tree = new std::vector<int>[numVertices];
  distance = new int[numVertices]; 
  // parent = new int[numVertices];
  
  buildSPTree();
}

void TreeStruct::constructIndexLLS() {
  level = new int[numVertices * 2];
  label = new int[numVertices];
  parent = new int[numVertices];
  tree = new std::vector<int>[numVertices];
  distance = new int[numVertices];   
  
  buildSPTree();
  
  indexSize = 0;
  treeNodesNums = 0;

  dfs(root, -1);

  DeleteArrPtr(distance);
  DeleteArrPtr(tree);
  DeleteArrPtr(parent);

//  Debug(nodesNumbers);
  blockSize = -1;
  while ((1ll << (blockSize + 1)) <= indexSize) {
    blockSize++;
  }

  blockSize >>= 1;
  blockSize = std::max(blockSize, 1);
  blockNums = (indexSize + blockSize - 1) / blockSize;
//  Debug(blockSize);
//  Debug(blockNums);

  maskMinIndex = new std::vector<std::vector<uint8_t>>[1 << blockSize];
  maskOfBlock = new short[blockNums];
  minIndexOnBlock = new std::vector<int>[blockNums];

  vis.reset();
  for (int index = 0; index < indexSize; index++) {

    int blockID = index / blockSize;
    if (index % blockSize == 0) {
      minIndexOnBlock[blockID].push_back(index);
      continue;
    }
    if (level[minIndexOnBlock[blockID][0]] > level[index]) {
      minIndexOnBlock[blockID][0] = index;
    }
  }


  // find the index of min element on block Array
  for (int step = 1; (1ll << step) <= blockNums; step++) {
    for (int blockID = 0; blockID + (1ll << step) < blockNums; blockID++) {
      if (level[minIndexOnBlock[blockID][step - 1]]
          > level[minIndexOnBlock[blockID + (1ll << (step - 1))][step - 1]]) {
        minIndexOnBlock[blockID].push_back(minIndexOnBlock[blockID + (1ll << (step - 1))][step - 1]);
      } else {
        minIndexOnBlock[blockID].push_back(minIndexOnBlock[blockID][step - 1]);
      }
    }
  }

  vis.reset();

  for (int blockStartIndex = 0, blockID = 0; blockStartIndex < indexSize; blockStartIndex += blockSize) {
    int mask = 0;

    for (int index = 1; blockStartIndex + index < indexSize && index < blockSize; index++) {
      if (level[index + blockStartIndex] > level[index - 1 + blockStartIndex]) {
        mask |= (1 << index);
      }
    }

    maskOfBlock[blockID++] = mask;

    if (vis[mask] == 1) {
      continue;
    }
    vis[mask] = 1;

    for (int index = 0; blockStartIndex + index < indexSize && index < blockSize; index++) {
      std::vector<uint8_t> minIndexSubArray;
      int minIndex = index;
      for (int indey = index; blockStartIndex + indey < indexSize && indey < blockSize; indey++) {
        if (level[blockStartIndex + minIndex] > level[blockStartIndex + indey]) {
          minIndex = indey;
        }
        minIndexSubArray.push_back(minIndex);
      }
      maskMinIndex[mask].push_back(minIndexSubArray);
    }
  }
}

int TreeStruct::RMQ(int x, int y) {
  if (x > y) {
    std::swap(x, y);
  }
  int xID = x / blockSize;
  int yID = y / blockSize;
  int ix = x + 1;
  int iy = y;
  while (ix <= y && ix % blockSize != 0) {
    ix++;
  }
  int maskx = maskOfBlock[xID];
  int masky = maskOfBlock[yID];
  int minIndex = maskMinIndex[maskx][x % blockSize][ix - 1 - x] + xID * blockSize;
  if (xID != yID) {
    while (iy % blockSize != 0) {
      iy--;
    }
    int yIndex = maskMinIndex[masky][iy % blockSize][y - iy] + iy;
    if (level[yIndex] < level[minIndex]) {
      minIndex = yIndex;
    }
    if (ix < iy) {
      xID = ix / blockSize;
      yID = (iy - 1) / blockSize;
      int bit = maxBits(yID - xID + 1);
      int aIndex = minIndexOnBlock[xID][bit];
      int bIndex = minIndexOnBlock[yID - (1 << bit) + 1][bit];
      int cIndex;
      if (level[aIndex] < level[bIndex]) {
        cIndex = aIndex;
      } else {
        cIndex = bIndex;
      }
      if (level[minIndex] > level[cIndex]) {
        minIndex = cIndex;
      }
    }
  }
  return minIndex;
}

int TreeStruct::getLcaDistance(int a, int b) {
  // node a and b don't have lca because they don't exit on the same SPTree
  int x = label[a], y = label[b];
  if (level[x] == INF8 || level[y] == INF8) {
    return INF8;
  }
  int minIndex = RMQ(x, y);
  return level[minIndex];
}

int TreeStruct::queryDistanceGLS(int x, int y) {
  if (x < 0 || y < 0 || x >= numVertices || y >= numVertices) {
    return INF8;
  }
  return distance[x] + distance[y];
}

int TreeStruct::queryDistanceLLS(int a, int b) {
  if (a < 0 || b < 0 || a >= numVertices || b >= numVertices) {
    return INF8;
  }
  int lcaDistance = getLcaDistance(a, b);
  return level[label[a]] + level[label[b]] - 2 * lcaDistance;
}

void TreeStruct::Free() {
  DeleteArrPtr(distance);
  DeleteArrPtr(level);
  DeleteArrPtr(label);
  DeleteArrPtr(parent);

  DeleteArrPtr(tree);
  DeleteArrPtr(maskMinIndex);
  DeleteArrPtr(maskOfBlock);
  DeleteArrPtr(minIndexOnBlock);
}

TreeStruct::~TreeStruct() {
  TreeStruct::Free();
}


