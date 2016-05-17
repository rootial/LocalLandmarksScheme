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

  void testCompressGraphp() {
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
//    Debug(sz);
    std::vector<std::vector<int> > graph(sz);
    for (int i = 0; i < 17; i++) {
      int u = (int)edges[i][0] - 'a';
      int v = (int)edges[i][1] - 'a';
      graph[u].push_back(v);
      graph[v].push_back(u);
    }
    GraphCompression GC(sz, graph);
    GC.compressGraph();

    PIU answers[] = {PIU(0, 0), PIU(0, 1), PIU(0, 2), PIU(0, 1), PIU(0, 2),
                     PIU(0, 2), PIU(0, 0), PIU(0, 0), PIU(0, 0), PIU(0, 0),
                     PIU(0, 0), PIU(0, 0), PIU(0, 0), PIU(0, 0), PIU(13, 1)
                    };

    for (int u = 0; u < sz; u++) {
      if ((GC.nodesIndex[u].type != 0 && answers[u].second == 0) ||
          (GC.nodesIndex[u].attr[0].first == answers[u].first && GC.nodesIndex[u].attr[0].second == answers[u].second)) {
      } else {
        printf("Failed tests at Node %d\n", u);
        printf("Output (%d, %d)\n", GC.nodesIndex[u].attr[0].first, GC.nodesIndex[u].attr[0].second);
        printf("Answer (%d, %d)\n", answers[u].first, answers[u].second);
        return;
      }
    }
    puts("Passed all tests!");
  }
};


#endif // TESTUNIT_H_INCLUDED
