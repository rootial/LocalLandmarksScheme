//#include "GraphCompression.h"
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

char filename[30] = "E:\\GitHub\\web-Google.txt";
class TestLLS;

int main() {

  TestLLS testUnit;
  testUnit.testCompressGraphp();
  return 0;

//  srand(time(NULL));
//
//  if (LLS.constructIndex(filename)) {
//    LLS.printStatistics();
//      testUnit.testAverageError();
//  } else {
//    puts("Read Graph error!");
//    exit(EXIT_FAILURE);
//  }
  return 0;
}
