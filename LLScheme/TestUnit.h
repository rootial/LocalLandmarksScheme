#ifndef TESTUNIT_H_INCLUDED
#define TESTUNIT_H_INCLUDED

#include "CommonHeader.h"
#include "LocalLandmarksScheme.h"
#include "GraphCompression.h"
#include <cstdio>

const int maxTestRounds = 10000;

typedef std::pair<int, int> PII;

class TestLLS {
public:

  // test with random generated queries to calculate the average error
  // with Global Landmarks Scheme
  void testAverageError() {
    LocalLandmarksScheme<20> LLS;

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

    Edge answers[][2] = {{Edge(0, 0), Edge(0, 0)}, {Edge(0, 1), Edge(0, 0)}, {Edge(0, 2), Edge(0, 0)},
      {Edge(0, 1), Edge(0, 0)}, {Edge(0, 2), Edge(0, 0)}, {Edge(0, 2), Edge(0, 0)},
      {Edge(0, 1), Edge(7, 1)}, {Edge(7, 0), Edge(7, 0)}, {Edge(7, 1), Edge(10, 2)}, {Edge(7, 2), Edge(10, 1)},
      {Edge(10, 0), Edge(10, 0)}, {Edge(10, 1), Edge(10, 2)}, {Edge(10, 1), Edge(10, 2)},
      {Edge(13, 0), Edge(13, 0)}, {Edge(13, 1), Edge(13, 1)}
    };

    int sz = 0;
    for (int i = 0; i < 17; i++) {
      int u = (int)edges[i][0] - 'a';
      int v = (int)edges[i][1] - 'a';
      sz = std::max(sz, std::max(u, v) + 1);
    }

    auto graph = new std::vector<Edge>[sz];
    //  std::vector<Edge> graph[20];

    for (int i = 0; i < 17; i++) {
      int u = (int)edges[i][0] - 'a';
      int v = (int)edges[i][1] - 'a';
      graph[u].push_back(Edge(v, 1));
      graph[v].push_back(Edge(u, 1));
    }


    GraphCompression GC(sz, graph);
    int compressedGraphVertices = GC.compressGraph();

    puts("Check index");
    for (int u = 0; u < sz; u++) {
      int i = 0;
      std::sort(GC.nodesIndex[u].attr.begin(), GC.nodesIndex[u].attr.end());

      for (auto& attr : GC.nodesIndex[u].attr) {
        if (!(attr.v == answers[u][i].v && attr.d == answers[u][i].d)) {
          puts("Failed building index!");
          printf("Should be Node: %d, type: %d, Representative: %d, dist: %d\n", u, GC.nodesIndex[u].type,
                 answers[u][i].v, answers[u][i].d);
          printf("Ouput Node: %d, type: %d, Representative: %d, dist: %d\n", u, GC.nodesIndex[u].type,
                 attr.v, attr.d);
          return;
        }
        i++;
      }
    }
    puts("Build Index correctly!");
    printf("Graph has been compressed into %d vertices\n", compressedGraphVertices);
    puts("Passed all tests!");
  }

  void testQueryDistance() {
    char edges[][4] = {
      "ag", "ah", "gh", "ab", "bc",
      "ad", "de", "df", "hi", "ij",
      "jk", "hn", "nk", "no", "kl",
      "lm", "km",
    };

    int sz = 0;
    std::vector<PII> es;

    for (int i = 0; i < 17; i++) {
      int u = (int)edges[i][0] - 'a';
      int v = (int)edges[i][1] - 'a';
      es.push_back(PII(u, v));
      sz = std::max(sz, std::max(u, v) + 1);
    }
    PII queryPair[] = {PII(0, 1), PII(1, 2), PII(2, 3), PII(2, 4), PII(2, 5),
                       PII(2, 6), PII(2, 7), PII(2, 9), PII(2, 10), PII(8, 9),
                       PII(8, 10), PII(14, 8), PII(14, 10), PII(14, 11), PII(11, 12),
                       PII(14, 12),
                      };
    int ans[] = {1, 1, 3, 4, 4,
                 3, 3, 5, 5, 1,
                 2, 3, 2, 3, 1, 3
                };

    LocalLandmarksScheme<2> LLS;

    if (LLS.constructIndex(es) == true) {
      for (int i = 0; i < 16; i++) {
        int x = queryPair[i].first;
        int y = queryPair[i].second;
        int d = LLS.queryDistance(x, y);
        if (d != ans[i]) {
          printf("Failed at pair(%d, %d)\n", x, y);
          printf("Output distance: %d\n", d);
          printf("Should be: %d\n", ans[i]);
          return;
        }
      }
    } else {
      puts("Failed test query distance");
      return;
    }
    puts("Check Query Distance Result");
    puts("Query Distance result is correct!");
  }
};


#endif // TESTUNIT_H_INCLUDED
