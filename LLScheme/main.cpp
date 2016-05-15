#include <malloc.h>
#include <bitset>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <sys/time.h>
#include <climits>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>
#include <utility>


#define inf 0x0f0f0f0f
#define Delete(x) {if (x != NULL) { \
delete []x; x = NULL;}}
#define Debug(x) printf("Excuted here! %d\n", (x))

const int maxnode = 2000000;
const int maxLandmarksNum = 51;
const int maxBlockSize = 16;

typedef std::pair<int, int> PII;

static const uint8_t INF8 = 100;

inline int maxBits(int x) {
  if (x <= 1) {
    return 0;
  }
  int ret = 0;
  while ((1 << (ret + 1)) < x) {
    ret++;
  }
  return ret;
}

class TreeStruct {
public:
  std::vector<int> *tree = NULL;
  int *distance = NULL;

  void constructIndex();

  int queryDistance(int x, int y);

  int queryDistanceGlobal(int x, int y);

  TreeStruct(int root, int numVertices): root(root), numVertices(numVertices) {
    tree = new std::vector<int>[numVertices];
    distance = new int[numVertices];
    level = new int[numVertices * 2];
    trace = new int[numVertices * 2];
    label = new int[numVertices];
  }

  virtual ~TreeStruct() {
    Free();
  }

private:

  int root;
  int numVertices;
  int blockSize;
  int blockNums;
  int treeNodesNums;
  int indexSize;

  std::bitset < 1 << maxBlockSize > visited;

  int *level = NULL;
  int *trace = NULL;
  int *label = NULL;

  std::vector<std::vector<int>> *maskMinIndex = NULL;
  std::vector<int> *minIndexOnBlock = NULL;
  int *maskOfBlock = NULL;

  void dfs(int u, int fa);

  int getLCA(int x, int y);

  int RMQ(int x, int y);

  void Free();
};

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
  if (x > y) {std::swap(x, y);}
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
      if (level[aIndex] < level[bIndex]) {
        minIndex = aIndex;
      } else {
        minIndex = bIndex;
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
  Delete(distance);
  Delete(level);
  Delete(trace);
  Delete(label);

  Delete(tree);
  Delete(maskMinIndex);
  Delete(maskOfBlock);
  Delete(minIndexOnBlock);
}

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
  inline int queryDistance(int v, int w);
  inline int queryDistanceGlobal(int v, int w);

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

  LocalLandmarksScheme()
    : numVertices(0), timeLoad(0), timeIndexing(0) {}
  virtual ~LocalLandmarksScheme() {
    Free();
  }

private:

  int numVertices;
  std::vector<TreeStruct*> SPTree;
  int* rank = NULL;

//  indexType *indexStruct;

  double GetCurrentTimeSec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  }

  // Statistics
  double timeLoad, timeIndexing;
};

template<int NumSelectedLandmarks>
bool LocalLandmarksScheme<NumSelectedLandmarks>
::constructIndex(const char *filename) {
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

  std::vector<std::vector<int> > adj(V);
  for (size_t i = 0; i < es.size(); ++i) {
    int v = es[i].first, w = es[i].second;
    adj[v].push_back(w);
    // adj[w].push_back(v);
  }
  timeLoad += GetCurrentTimeSec();

  // Order vertices by decreasing order of degree
  //
  timeIndexing = -GetCurrentTimeSec();
  std::vector<int> inv(V);  // new label -> old label
  {
    // Order
    std::vector<std::pair<float, int> > deg(V);
    for (int v = 0; v < V; ++v) {
      // We add a random value here to diffuse nearby vertices
      deg[v] = std::make_pair(adj[v].size() + float(rand()) / RAND_MAX, v);
    }
    std::sort(deg.rbegin(), deg.rend());
    for (int i = 0; i < V; ++i) inv[i] = deg[i].second;

    // Relabel the vertex IDs
    rank = new int[V];
    for (int i = 0; i < V; ++i) rank[deg[i].second] = i;
    std::vector<std::vector<int> > new_adj(V);
    for (int v = 0; v < V; ++v) {
      for (size_t i = 0; i < adj[v].size(); ++i) {
        new_adj[rank[v]].push_back(rank[adj[v][i]]);
      }
    }
    adj.swap(new_adj);
  }
  // select NumSelectedLandmarks landmarks by degrees
  for (int root = 0; root < NumSelectedLandmarks; root++) {
    std::bitset<maxnode> vis;
    std::queue<int> que;

    que.push(root);
    TreeStruct *treeResult = new TreeStruct(root, numVertices);

    std::vector<int>* _tree = treeResult->tree;
    int* dist = treeResult->distance;
    for (int v = 0; v < numVertices; v++) {
      dist[v] = INF8;
    }
    dist[root] = 0;
    vis[root] = 1;

    while (que.size()) {
      int u = que.front();
      que.pop();
      _tree[u].clear();

      for (auto &v : adj[u]) {
        if (vis[v]) {
          continue;
        }
        dist[v] = dist[u] + 1;
        _tree[u].push_back(v);
        vis[v] = 1;
        que.push(v);
        assert(que.size() < 78000);
      }
    }
  //  std::cout << treeResult << std::endl;
    treeResult->constructIndex();
    SPTree.push_back(treeResult);
  }
  timeIndexing += GetCurrentTimeSec();
 // Debug(1);
  return true;
}

template<int NumSelectedLandmarks>
int LocalLandmarksScheme<NumSelectedLandmarks>
::queryDistance(int u, int v) {
  int distance = INF8;
  if (u >= numVertices || v >= numVertices) {
    return distance;
  }
  for (auto treeResult : SPTree) {
    int val = treeResult->TreeStruct::queryDistance(rank[u], rank[v]);
    if (distance > val) {
      distance = val;
    }
  }
  return distance;
}

template<int NumSelectedLandmarks>
int LocalLandmarksScheme<NumSelectedLandmarks>
::queryDistanceGlobal(int u, int v) {
  int distance = INF8;
  if (u >= numVertices || v >= numVertices) {
    return distance;
  }
  for (auto treeResult : SPTree) {
    int val = treeResult->queryDistanceGlobal(rank[u], rank[v]);
    if (distance > val) {
      distance = val;
    }
  }
  return distance;
}

template<int NumSelectedLandmarks>
void LocalLandmarksScheme<NumSelectedLandmarks>
::Free() {
  Delete(rank);
//  Debug(SPTree.size());
  for (auto &ptr : SPTree) {
  //  assert(ptr != NULL);
//  std::cout << "ptr: " << ptr << std::endl;
    Delete(ptr);
  }
}

template<int NumSelectedLandmarks>
void LocalLandmarksScheme<NumSelectedLandmarks>
::printStatistics() {
  printf("Load Graph with %d vertices\n", numVertices);
  printf("TimeLoad: %.6fs TimeIndexed: %.6fs\n", timeLoad, timeIndexing);
}

char filename[20] = "Slashdot0811.txt";

int main() {
  LocalLandmarksScheme<50> LLS;

  if (LLS.constructIndex(filename)) {

  } else {
    puts("Read Graph error!");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  int relaErr = 0;
  for (int t = 0; t <= 100000; t++) {
    int u = (int)(1.0 * rand() / RAND_MAX * 77360);
    int v = (int)(1.0 * rand() / RAND_MAX * 77360);
   // std::cout << u << " " << v << std::endl;
    int d0 = LLS.queryDistanceGlobal(u, v);
    int d1 = LLS.queryDistance(u, v);
    assert(d0 >= d1);
    relaErr += d0 - d1;
  }
  LLS.printStatistics();
  std::cout << "Average Error: " << relaErr/ 100000.0 << std::endl;
  return 0;
}
