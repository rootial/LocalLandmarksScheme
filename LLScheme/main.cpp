#include "LocalLandmarksScheme.h"
#include "TestUnit.h"

#include <malloc.h>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <climits>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>

TestLLS testUnit;

char GLSQueryTime[] = "E:\\GitHub\\GLSQueryTime.txt";
char LLSQueryTime[] = "E:\\GitHub\\LLSQueryTime.txt";

char GLSQueryError[] = "E:\\GitHub\\GLSQueryError.txt";
char LLSQueryError[] = "E:\\GitHub\\LLSQueryError.txt";

char GLSSearchRank[] = "E:\\GitHub\\GLSSearchRank.txt";
char LLSSearchRank[] = "E:\\GitHub\\LLSSearchRank.txt";

char GLSTime[] = "E:\\GitHub\\GLSTime.txt";
char LLSTime[] = "E:\\GitHub\\LLSTime.txt";

int num[] = {20, 50};

void testAverageError() {

  std::ofstream ofs(LLSQueryTime);
  for (int type = 0; type < 3; type++) {
    for (int index = 0; index < 4; index++) {
      for (int indey = 0; indey < 2; indey++) {
        double err = 0.0;
        for (int i = 0; i < 10; i++) {
          err +=  testUnit.testLLSAverageError(num[indey], 1, type, index);
        }
        err /= 10.0;
        char ans[100];
        sprintf(ans, "selection type: %d, err: %.6f, %d landmarks on graph %d\n", type, err, num[indey], index);
        ofs << ans << std::endl;
      }
    }
  }
}

void testSearchRank() {
  char ans[100];
  std::ofstream ofs(GLSSearchRank);
  for (int type = 0; type < 3; type++) {
    for (int index = 0; index < 4; index++) {
      for (int indey = 0; indey < 2; indey++) {
        sprintf(ans, "selection type: %d, %d landmarks on graph %d \n", type, num[indey], index);
        ofs << ans << std::endl;
        std::vector<double> VD = testUnit.testGLSSocialSearchRank(num[indey], type, index);
        for (auto accur : VD) {
          ofs << accur << ' ';
        }
        ofs << std::endl;
      }
    }
  }
}

void testLLSTime() {
  char ans[100];
  std::ofstream ofs(LLSTime);
  for (int index = 0; index < 4; index++) {
    for (int indey = 0; indey < 2; indey++) {
      double t = testUnit.testDistanceLLSTime(num[indey], 0, index);
      sprintf(ans, "takes %.6fms with %d landmarks on graph %d\n", t, num[indey], index);
      ofs << ans << std::endl;
    }
  }
}

void testGLSTime() {
  char ans[100];
  std::ofstream ofs(GLSTime);
  for (int index = 0; index < 4; index++) {
    for (int indey = 0; indey < 2; indey++) {
      double t = testUnit.testDistanceGLSTime(num[indey], 0, index);
      sprintf(ans, "takes %.6fms with %d landmarks on graph %d\n", t, num[indey], index);
      ofs << ans << std::endl;
    }
  }
}

int main() {
  srand(time(NULL));

//  testUnit.testCompressGraph();

  testUnit.testLLSAverageError(50, 3, 1, 2);

//  testUnit.testQueryDistanceLLS();

//  testUnit.generateRandomPairs(3, 334863);

//  testUnit.generateLargestComponent(3);

//  testUnit.outputExactAnswer(1);

//  testUnit.outputDistributionOfDist(0);
//  testLLSTime();
//  testGLSTime();

  return 0;
}
