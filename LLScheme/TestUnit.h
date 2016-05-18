#ifndef TESTUNIT_H_INCLUDED
#define TESTUNIT_H_INCLUDED

#include "CommonHeader.h"
#include "LocalLandmarksScheme.h"
#include "GraphCompression.h"
#include <cstdio>

const int maxTestRounds = 10000;

class TestLLS {
public:
  LocalLandmarksScheme<20> LLS;
  // test with random generated queries to calculate the average error
  // with Global Landmarks Scheme
  void testAverageError() {
    double timeStart = -LLS.GetCurrentTimeSec();
    int numV = LLS.getNumVertices();
    double avgErr = 0;
    for (int t = 0; t < maxTestRounds; ) {
      int u = (int)(1.0 * rand() / RAND_MAX * numV);
      int v = (int)(1.0 * rand() / RAND_MAX * numV);
      if (u >= numV || v >= numV) {
        continue;
      }
      t++;
      int d0 = LLS.queryDistanceGlobal(u, v);
      int d1 = LLS.queryDistance(u, v);
      assert(d0 >= d1);

      avgErr += d0 - d1;
    }

    avgErr = avgErr * 1.0 / maxTestRounds;
    timeStart += LLS.GetCurrentTimeSec();

    // compare result of LocalLandmarks Scheme with that of Global Landmarks Scheme...
    printf("Run %d test cases in %.6fs, Average Error: %.6f\n", maxTestRounds, timeStart, avgErr);
  }

  void testCompressGraph() {
    char edges[][4] = {
      "ag", "ah", "gh", "ab", "bc",
      "ad", "de", "df", "hi", "ij",
      "jk", "hn", "nk", "no", "kl",
      "lm", "km",
    };
    int sz = 0;
    for (int i = 0; i < 17; i++) {
      int u = (int)edges[i][0] - 'a';
      int v = (int)edges[i][1] - 'a';
      sz = std::max(sz, std::max(u, v) + 1);
    }
    GPtr graph = new std::vector<Edge>[sz];
//    auto graph = std::vector<std::vector<Edge> >(sz);
    for (int i = 0; i < 17; i++) {
      int u = (int)edges[i][0] - 'a';
      int v = (int)edges[i][1] - 'a';
      graph[u].push_back(Edge(v, 1));
      graph[v].push_back(Edge(u, 1));
    }


    GraphCompression GC(sz, graph);
    int compressedGraphVertices = GC.compressGraph();

    DeleteArrPtr(graph);

    PIU answers[][2] = {{PIU(7, 1), PIU(7, 2)}, {PIU(0, 1), PIU(0, 0)}, {PIU(0, 2), PIU(0, 0)},
      {PIU(0, 1), PIU(0, 0)}, {PIU(0, 2), PIU(0, 0)}, {PIU(0, 2), PIU(0, 0)},
      {PIU(7, 1), PIU(7, 2)}, {PIU(0, 0), PIU(0, 0)}, {PIU(7, 1), PIU(10, 2)}, {PIU(7, 2), PIU(10, 1)},
      {PIU(0, 0), PIU(0, 0)}, {PIU(10, 1), PIU(10, 2)}, {PIU(10, 1), PIU(10, 2)},
      {PIU(7, 1), PIU(10, 1)}, {PIU(13, 1), PIU(0, 0)}
    };

    for (int u = 0; u < sz; u++) {
      if (GC.nodesIndex[u].type == TreeNodeType) {
        if (!(GC.nodesIndex[u].attr[0].first == answers[u][0].first &&
              GC.nodesIndex[u].attr[0].second == answers[u][0].second)) {
          printf("Should be Node: %d, EntryNode: %d, dist: %d\n", u,
                 answers[u][0].first, answers[u][0].second);
          printf("Ouput  Node: %d, EntryNode: %d, dist: %d\n", u,
                 GC.nodesIndex[u].attr[0].first, GC.nodesIndex[u].attr[0].second);
          return;
        }
      } else if (GC.nodesIndex[u].type == ChainNodeType) {
        int i = 0;
        sort(GC.nodesIndex[u].attr.begin(), GC.nodesIndex[u].attr.end());
        for (auto& attr : GC.nodesIndex[u].attr) {
          if (!(attr.first == answers[u][i].first && attr.second == answers[u][i].second)) {
            printf("Should be Node: %d, EndNode: %d, dist: %d\n", u,
                   answers[u][i].first, answers[u][i].second);
            printf("Ouput  Node: %d, EndNode: %d, dist: %d\n", u,
                   attr.first, attr.second);
            return;
          }
          i++;
        }
      }
    }
    printf("Graph has been compressed into %d vertices\n", compressedGraphVertices);
    puts("Passed all tests!");
  }
};


#endif // TESTUNIT_H_INCLUDED
