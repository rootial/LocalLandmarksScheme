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

char dataFile[][100] = {"E:\\GitHub\\Slashdot0811.txt", "E:\\GitHub\\web-Google.txt", "E:\\GitHub\\com-dblp.ungraph.txt", "E:\\GitHub\\com-amazon.ungraph.txt"};
char queryFile[][100] = {"E:\\GitHub\\Slashdot0811-queryPairs.txt", "E:\\GitHub\\web-Google-queryPairs.txt", "E:\\GitHub\\com-dblp.ungraph_queryPairs.txt", "E:\\GitHub\\com-amazon.ungraph_queryPairs.txt"};
char largestCom[][100] = {"E:\\GitHub\\Slashdot0811-largeCom.txt", "E:\\GitHub\\web-Google-largeCom.txt", "E:\\GitHub\\com-dblp.ungraph_largeCom.txt", "E:\\GitHub\\com-amazon.ungraph_largeCom.txt"};

char exactAnsFile[][100] = {"E:\\GitHub\\Slashdot0811-exactAns.txt", "E:\\GitHub\\web-Google-exactAns.txt", "E:\\GitHub\\com-dblp.ungraph_exactAns.txt", "E:\\GitHub\\com-amazon.ungraph_exactAns.txt"};
char LLSAnsFile[][100] = {"E:\\GitHub\\Slashdot0811-LLSAns.txt", "E:\\GitHub\\web-Google-LLSAns.txt", "E:\\GitHub\\com-dblp.ungraph_LLSAns.txt", "E:\\GitHub\\com-amazon.ungraph_LLSAns.txt"};
char GLSAnsFile[][100] = {"E:\\GitHub\\Slashdot0811-GLSAns.txt", "E:\\GitHub\\web-Google-GLSAns.txt", "E:\\GitHub\\com-dblp.ungraph_GLSAns.txt", "E:\\GitHub\\com-amazon.ungraph_GLSAns.txt"};
char relativeError[][100] = {"E:\\GitHub\\Slashdot0811-relaError.txt", "E:\\GitHub\\web-Google-relaError.txt", "E:\\GitHub\\com-dblp.ungraph_relaError.txt", "E:\\GitHub\\com-amazon.ungraph_relaError.txt"};


int top[] = {5, 10, 20, 50};

class TestLLS {
public:

  // test with random generated queries to calculate the average error
  // with Global Landmarks Scheme
  double testLLSAverageError(int num, int times, int type, int fileID) {
    LocalLandmarksScheme LLS(num);

    printf("Start to build Graph Index on %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    LLS.constructIndexLLS(times, type);

    double avgErr = 0;
    std::ifstream queryIn(queryFile[fileID]);
    std::ifstream exactAnsFileIn(exactAnsFile[fileID]);

    double timeStart = -LLS.GetCurrentTimeSec();

    for (int u, v, d0, d, cnt = 0; queryIn >> u >> v, exactAnsFileIn >> d; cnt++) {
      d0 = LLS.queryDistanceLLS(u, v);
      assert(d0 >= d);
      if (d == 0) {
        continue;
      }
      avgErr += 1.0 * (d0 - d) / d;
      //  printf("%dth done! dist: %d\n", cnt, d);
    }

    avgErr /= maxTestRounds;

    timeStart += LLS.GetCurrentTimeSec();
    // compare result of LocalLandmarks Scheme with that of Global Landmarks Scheme...
    printf("Run %d QueryLLS tests in %.6fs, Average Error: %.6f\n", maxTestRounds, timeStart, avgErr);
    return avgErr;
  }

  double testGLSAverageError(int num, int type, int fileID) {
    LocalLandmarksScheme LLS(num);

    printf("Start to build Graph Index on %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    LLS.constructIndexGLS(type);

    double avgErr = 0;
    std::ifstream queryIn(queryFile[fileID]);
    std::ifstream exactAnsFileIn(exactAnsFile[fileID]);

    double timeStart = -LLS.GetCurrentTimeSec();

    for (int u, v, d0, d, cnt = 0; queryIn >> u >> v, exactAnsFileIn >> d; cnt++) {
      d0 = LLS.queryDistanceGLS(u, v);
      assert(d0 >= d);
      if (d == 0) {
        continue;
      }
      avgErr += 1.0 * (d0 - d) / d;
      //  printf("%dth done! dist: %d\n", cnt, d);
    }

    avgErr /= maxTestRounds;

    timeStart += LLS.GetCurrentTimeSec();
    // compare result of LocalLandmarks Scheme with that of Global Landmarks Scheme...
    printf("Run %d QueryGLS tests in %.6fs, Average Error: %.6f\n", maxTestRounds, timeStart, avgErr);
    return avgErr;
  }

  // select 100 nodes by random and apply LLS in social search rank using 100 nodes
  std::vector<double> testLLSSocialSearchRank(int numSelected, int times, int type, int fileID) {
    LocalLandmarksScheme LLS(numSelected);

    printf("Start to build Graph Index on %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    LLS.constructIndexLLS(times, type);

    int V = LLS.getNumVertices();

    std::vector<double> VD;
    std::vector<int> num;

    for (int i = 0; i < V; i++) {
      num.push_back(i);
    }
    std::bitset<maxnode> vis;

    for (int k = 0; k < 4; k++) {
      double accur = 0;
      for (int index = 0; index < 100; index++) {
        std::random_shuffle(num.begin(), num.end());
        int st = myrandom(V);

        vis.reset();
        for (int i = 0; i < 100; i++) {
          vis[num[i]] = 1;
        }

        std::vector<int> dist = LLS.queryDistance(st);
        std::vector<std::pair<int, int> > A, B;

        for (int i = 0; i < V; i++) {
          if (vis[i]) {
            A.push_back(std::make_pair(dist[i], i));
          }
        }

        for (int r = 0; r < 100; r++) {
          int d = LLS.queryDistanceLLS(st, num[r]);
          B.push_back(std::make_pair(d, num[r]));
        }

        std::sort(A.begin(), A.end());
        std::sort(B.begin(), B.end());

        vis.reset();
        for (int i = 0; i < top[k]; i++) {
          vis[A[i].second] = 1;
        }
        for (int i = top[k]; i < (int)A.size() && A[i].first == A[i - 1].first; i++) {
          vis[A[i].second] = 1;
        }
        int cnt = 0;
        for (int i = 0; i < top[k]; i++) {
          if (vis[B[i].second]) {
            cnt++;
          }
        }
        accur += 1.0 * cnt / top[k] / 100.0;
      }
      VD.push_back(accur);
    }
    return VD;
  }

  std::vector<double> testGLSSocialSearchRank(int numSelected, int type, int fileID) {
    LocalLandmarksScheme LLS(numSelected);

    printf("Start to build Graph Index on %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    LLS.constructIndexGLS(type);

    int V = LLS.getNumVertices();

    std::vector<double> VD;
    std::vector<int> num;

    for (int i = 0; i < V; i++) {
      num.push_back(i);
    }
    std::bitset<maxnode> vis;

    for (int k = 0; k < 4; k++) {
      double accur = 0;
      for (int index = 0; index < 100; index++) {
        std::random_shuffle(num.begin(), num.end());
        int st = myrandom(V);

        vis.reset();
        for (int i = 0; i < 100; i++) {
          vis[num[i]] = 1;
        }

        std::vector<int> dist = LLS.queryDistance(st);
        std::vector<std::pair<int, int> > A, B;

        for (int i = 0; i < V; i++) {
          if (vis[i]) {
            A.push_back(std::make_pair(dist[i], i));
          }
        }

        for (int r = 0; r < 100; r++) {
          int d = LLS.queryDistanceGLS(st, num[r]);
          B.push_back(std::make_pair(d, num[r]));
        }

        std::sort(A.begin(), A.end());
        std::sort(B.begin(), B.end());

        vis.reset();
        for (int i = 0; i < top[k]; i++) {
          vis[A[i].second] = 1;
        }
        for (int i = top[k]; i < (int)A.size() && A[i].first == A[i - 1].first; i++) {
          vis[A[i].second] = 1;
        }
        int cnt = 0;
        for (int i = 0; i < top[k]; i++) {
          if (vis[B[i].second]) {
            cnt++;
          }
        }
        accur += 1.0 * cnt / top[k] / 100.0;
      }
      VD.push_back(accur);
    }
    return VD;
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

    LocalLandmarksScheme LLS;
    LLS.loadGraph(es);

    if (LLS.constructIndexLLS(1, 0) == true) {
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

  void generateRandomPairs(int id, int V) {
    std::vector<PII> queryPairs;
    for (int i = 0; i < maxTestRounds; i++) {
      int u = myrandom(V);
      int v = myrandom(V);
      queryPairs.push_back(PII(u, v));
    }
    std::ofstream ofile(queryFile[id]);
    for (auto&e : queryPairs) {
      ofile << e.first << ' ' << e.second << std::endl;
    }
  }

  void outputExactAnswer(int fileID) {
    LocalLandmarksScheme LLS(2);
    printf("Start to build Graph Index on %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    double timeQuery = -LLS.GetCurrentTimeSec();

    std::ifstream queryIn(queryFile[fileID]);
    std::ofstream exactAnsOut(exactAnsFile[fileID]);
//    int cnt = 0;

    while (!queryIn.eof()) {
      int u, v;
      queryIn >> u >> v;
      int d = LLS.queryDistanceExact(u, v);
      int dd = LLS.queryDistanceExactDijk(u, v);
      assert(d == dd);
      exactAnsOut << d << std::endl;
//    int dd = LLS.queryDistanceExactDijk(u, v);
//      printf("%dth done! dist: %d\n", cnt++, d);
    }

    timeQuery += LLS.GetCurrentTimeSec();
    printf("%d query finished in time: %.6fs, average time: %.6fs\n",
           maxTestRounds, timeQuery, timeQuery / maxTestRounds);
  }

  void outputGLSAnswer(int type, int fileID) {
    LocalLandmarksScheme LLS(50);
    printf("Start to Load Graph %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    LLS.constructIndexGLS(type);

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

  void outputLLSAnswer(int num, int type, int fileID)  {
    LocalLandmarksScheme LLS(num);
    printf("Start to Load Graph %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);
    LLS.constructIndexLLS(1, type);

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
    LocalLandmarksScheme LLS;
    LLS.generateLargestComponent(dataFile[id], largestCom[id]);
  }

  void queryDistanceLLS(int num, int type, int fileID) {
    LocalLandmarksScheme LLS(num);
    printf("Start to Load Graph %s\n", largestCom[fileID]);

    LLS.loadGraph(largestCom[fileID]);

    LLS.constructIndexLLS(1, type);

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

  double testDistanceLLSTime(int num, int type, int fileID) {
    LocalLandmarksScheme LLS(num);
    printf("Start to Load Graph %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);

    LLS.constructIndexLLS(1, type);

    std::ifstream queryIn(queryFile[fileID]);

    double timeStart = -LLS.GetCurrentTimeSec();

    for (int u, v, d0; queryIn >> u >> v; ) {
      d0 = LLS.queryDistanceLLS(u, v);
    }
    timeStart += LLS.GetCurrentTimeSec();

    return timeStart / (maxTestRounds / 1000); // ms
  }

  double testDistanceGLSTime(int num, int type, int fileID) {
    LocalLandmarksScheme LLS(num);
    printf("Start to Load Graph %s\n", largestCom[fileID]);
    LLS.loadGraph(largestCom[fileID]);

    LLS.constructIndexGLS(type);

    std::ifstream queryIn(queryFile[fileID]);

    double timeStart = -LLS.GetCurrentTimeSec();

    for (int u, v, d0; queryIn >> u >> v; ) {
      d0 = LLS.queryDistanceGLS(u, v);
    }
    timeStart += LLS.GetCurrentTimeSec();

    return timeStart / (maxTestRounds / 1000); // ms
  }

  void outputDistributionOfDist(int fileID) {
    std::vector<int> distanceCnt(100, 0);
    std::ifstream ifs(exactAnsFile[fileID]);
    printf("Load exactAns from %s\n", exactAnsFile[fileID]);
    int maxDistance = 0;
    for (int dist; ifs >> dist; ) {
      distanceCnt[dist]++;
      maxDistance = std::max(maxDistance, dist);
    }
    printf("Max Distance: %d\n", maxDistance);
    for (int i = 0; i <= maxDistance; i++) {
      printf("Distance %d: %d times\n", i, distanceCnt[i]);
    }
  }
};

#endif // TESTUNIT_H_INCLUDED
