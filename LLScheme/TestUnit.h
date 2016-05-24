#ifndef TESTUNIT_H_INCLUDED
#define TESTUNIT_H_INCLUDED

#include "CommonHeader.h"
#include "LocalLandmarksScheme.h"
#include "GraphCompression.h"
#include <cstdio>

const int maxTestRounds = 10000;
const double MinProb = 1.0 / (RAND_MAX + 1);

typedef std::pair<int, int> PII;

// generate intergers between [0, n-1]
bool happened(double probability) { //probability 0~1
  if(probability <= 0) {
    return false;
  }
  if(probability < MinProb) {
    return rand() == 0 && happened(probability * (RAND_MAX + 1));
  }
  if(rand() <= probability * (RAND_MAX + 1)) {
    return true;
  }
  return false;
}

long myrandom(long n) {
  int t = 0;
  if(n <= RAND_MAX) {
    long r = RAND_MAX - (RAND_MAX + 1) % n;
    t = rand();
    while ( t > r ) {
      t = rand();
    }
    return t % n;
  } else {
    long r = n % (RAND_MAX + 1); //
    if( happened( (double)r / n ) ) {
      return n - r + myrandom(r);
    } else {
      return rand() + myrandom(n / (RAND_MAX + 1)) * (RAND_MAX + 1);
    }
  }
}

//char dataFile[][100] = {"E:\\GitHub\\Slashdot0811.txt", "E:\\GitHub\\web-Google.txt"};
char dataFile[][100] = {"E:\\GitHub\\Slashdot0811-largeCom.txt", "E:\\GitHub\\web-Google-largeCom.txt"};
char queryFile[][100] = {"E:\\GitHub\\Slashdot0811-queryPairs.txt", "E:\\GitHub\\web-Google-queryPairs.txt"};
char largestCom[][100] = {"E:\\GitHub\\Slashdot0811-queryPairs.txt", "E:\\GitHub\\web-Google-queryPairs.txt"};

char exactAnsFile[][100] = {"E:\\GitHub\\Slashdot0811-exactAns.txt", "E:\\GitHub\\web-Google-exactAns.txt"};
char LLSAnsFile[][100] = {"E:\\GitHub\\Slashdot0811-LLSAns.txt", "E:\\GitHub\\web-Google-LLSAns.txt"};
char GLSAnsFile[][100] = {"E:\\GitHub\\Slashdot0811-GLSAns.txt", "E:\\GitHub\\web-Google-GLSAns.txt"};
char relativeError[][100] = {"E:\\GitHub\\Slashdot0811-relaError.txt", "E:\\GitHub\\web-Google-relaError.txt"};

class TestLLS {
public:

  // test with random generated queries to calculate the average error
  // with Global Landmarks Scheme
  void testLLSAverageError(int times, int fileID) {
    LocalLandmarksScheme<50> LLS;

    printf("Start to build Graph Index on %s\n", dataFile[fileID]);
    LLS.loadGraph(dataFile[fileID]);
    LLS.constructIndexLLS(times);

    double timeStart = -LLS.GetCurrentTimeSec();
    double avgErr = 0;

    std::ifstream queryIn(queryFile[fileID]);
    std::ifstream exactAnsFileIn(exactAnsFile[fileID]);

    for (int u, v, d0, d, cnt = 0; queryIn >> u >> v, exactAnsFileIn >> d; cnt++) {
      d0 = LLS.queryDistanceLLS(u, v);
      assert(d0 >= d);
      avgErr += 1.0 * (d0 - d) / d;
    //  printf("%dth done! dist: %d\n", cnt, d);
    }

    avgErr /= maxTestRounds;

    timeStart += LLS.GetCurrentTimeSec();
    // compare result of LocalLandmarks Scheme with that of Global Landmarks Scheme...
    printf("Run %d QueryLLS tests in %.6fs, Average Error: %.6f\n", maxTestRounds, timeStart, avgErr);
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
    puts("Passed all tests!\n");
  }

  void testQueryDistanceLLS() {
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
    LLS.loadGraph(es);

    if (LLS.constructIndexLLS(1) == true) {
      for (int i = 0; i < 16; i++) {
        int x = queryPair[i].first;
        int y = queryPair[i].second;
        int d = LLS.queryDistanceLLS(x, y);
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
    puts("Query Distance result is correct!\n");
  }

  void generateRandomPairs(const char* filename, int V) {
    std::vector<PII> queryPairs;
    for (int i = 0; i < maxTestRounds; i++) {
      int u = myrandom(V);
      int v = myrandom(V);
      queryPairs.push_back(PII(u, v));
    }
    std::ofstream ofile(filename);
    for (auto&e : queryPairs) {
      ofile << e.first << ' ' << e.second << std::endl;
    }
  }

  void outputExactAnswer(int fileID) {
    LocalLandmarksScheme<50> LLS;
    printf("Start to build Graph Index on %s\n", dataFile[fileID]);
    LLS.loadGraph(dataFile[fileID]);

    double timeQuery = -LLS.GetCurrentTimeSec();

    std::ifstream queryIn(queryFile[fileID]);
//    std::ofstream exactAnsOut(exactAnsFile[fileID]);
    int cnt = 0;

    while (!queryIn.eof()) {
      int u, v;
      queryIn >> u >> v;
      int d = LLS.queryDistanceExact(u, v);
//      int dd = LLS.queryDistanceExactDijk(u, v);
//      assert(d == dd);
//      exactAnsOut << d << std::endl;
//    int dd = LLS.queryDistanceExactDijk(u, v);
//      printf("%dth done! dist: %d\n", cnt++, d);
    }

    timeQuery += LLS.GetCurrentTimeSec();
    printf("%d query finished in time: %.6fs, average time: %.6fs\n",
           maxTestRounds, timeQuery, timeQuery / maxTestRounds);
  }

  void outputGLSAnswer(int fileID) {
    LocalLandmarksScheme<50> LLS;
    printf("Start to Load Graph %s\n", dataFile[fileID]);
    LLS.loadGraph(dataFile[fileID]);
    LLS.constructIndexGLS();

    double timeQuery = -LLS.GetCurrentTimeSec();

    std::ifstream queryIn(queryFile[fileID]);
    std::ofstream GLSAnsOut(GLSAnsFile[fileID]);
    int cnt = 0;

    while (!queryIn.eof()) {
      int u, v;
      queryIn >> u >> v;
      int d = LLS.queryDistanceGLS(u, v);
//    int dd = LLS.queryDistanceExactDijk(u, v);
      printf("%dth done! dist: %d\n", cnt++, d);
      GLSAnsOut << d << std::endl;
    }
    timeQuery += LLS.GetCurrentTimeSec();

    printf("%d query finished in time: %.6fs, average time: %.6fs\n",
           maxTestRounds, timeQuery, timeQuery / maxTestRounds);
  }

  void outputLLSAnswer(int fileID)  {
    LocalLandmarksScheme<50> LLS;
    printf("Start to Load Graph %s\n", dataFile[fileID]);
    LLS.loadGraph(dataFile[fileID]);
    LLS.constructIndexLLS(1);

    double timeQuery = -LLS.GetCurrentTimeSec();

    std::ifstream queryIn(queryFile[fileID]);
    std::ofstream LLSAnsOut(LLSAnsFile[fileID]);
    int cnt = 0;

    while (!queryIn.eof()) {
      int u, v;
      queryIn >> u >> v;
      int d = LLS.queryDistanceLLS(u, v);
//    int dd = LLS.queryDistanceExactDijk(u, v);
      printf("%dth done! dist: %d\n", cnt++, d);
      LLSAnsOut << d << std::endl;
    }
    timeQuery += LLS.GetCurrentTimeSec();
    printf("%d query finished in time: %.6fs, average time: %.6fs\n",
           maxTestRounds, timeQuery, timeQuery / maxTestRounds);
  }

  void generateLargestComponent(int id) {
    LocalLandmarksScheme<10> LLS;
    LLS.generateLargestComponent(dataFile[id], largestCom[id]);
  }

  void queryDistanceLLS(int fileID) {
    LocalLandmarksScheme<50> LLS;
    printf("Start to Load Graph %s\n", dataFile[fileID]);

    LLS.loadGraph(dataFile[fileID]);

    LLS.constructIndexLLS(1);

    puts("Please enter the query pair nodes(separated by spaces)");
    int u, v;
    while (scanf("%d%d", &u, &v) != EOF) {
      int d = LLS.queryDistanceLLS(u, v);
      if (d == INF8) {
        printf("Query Pair: (%d, %d) not connected\n", u, v);
      } else {
        printf("Query Pair: (%d, %d)\tDistance: %d\n", u, v, d);
      }
    }
  }
};

#endif // TESTUNIT_H_INCLUDED
