#include "CommonHeader.h"
#include "TreeStruct.h"

void TreeStruct::dfs(int u, int fa) {
  label[u] = indexSize;
  trace[indexSize] = u;
  level[indexSize++] = distance[u];
  treeNodesNums++;
  for (auto &v : tree[u]) {
    if (v == fa) {
      continue;
    }
    dfs(v, u);
    trace[indexSize] = u;
    level[indexSize++] = distance[u];
  }
}

void TreeStruct::constructIndex() {
  indexSize = 0;
  treeNodesNums = 0;
  level = new int[numVertices * 2];
  trace = new int[numVertices * 2];
  label = new int[numVertices];
  dfs(root, -1);
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

  maskMinIndex = new std::vector<std::vector<int>>[1 << blockSize];
  maskOfBlock = new int[blockNums];
  minIndexOnBlock = new std::vector<int>[blockNums];

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


  for (int blockStartIndex = 0, blockID = 0; blockStartIndex < indexSize; blockStartIndex += blockSize) {
    int mask = 0;

    for (int index = 1; blockStartIndex + index < indexSize && index < blockSize; index++) {
      if (level[index + blockStartIndex] > level[index - 1 + blockStartIndex]) {
        mask |= (1 << index);
      }
    }
    maskOfBlock[blockID++] = (mask);
    if (visited[mask]) {
      continue;
    }
    visited[mask] = 1;

    for (int index = 0; blockStartIndex + index < indexSize && index < blockSize; index++) {
      std::vector<int> minIndexSubArray;
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

int TreeStruct::getLCA(int a, int b) {
  // node a and b don't have lca because they don't exit on the same SPTree
  if (distance[a] == INF8 || distance[b] == INF8 ||
      a >= numVertices || b >= numVertices) {
    return -1;
  }
  int x = label[a], y = label[b];
  int minIndex = RMQ(x, y);
  return trace[minIndex];
}

int TreeStruct::queryDistance(int a, int b) {
  int lca = getLCA(a, b);
  if (lca != -1) {
    return distance[a] + distance[b] - 2 * distance[lca];
  }
  return INF8;
}

int TreeStruct::queryDistanceGlobal(int a, int b) {
  if (a < numVertices && b < numVertices) {
    return distance[a] + distance[b];
  }
  return INF8;
}

void TreeStruct::Free() {
// puts("destroy TreeStruct");
  Delete(distance);
  Delete(level);
  Delete(trace);
  Delete(label);

  Delete(tree);
  Delete(maskMinIndex);
  Delete(maskOfBlock);
  Delete(minIndexOnBlock);
}

TreeStruct::~TreeStruct() {
  TreeStruct::Free();
}



