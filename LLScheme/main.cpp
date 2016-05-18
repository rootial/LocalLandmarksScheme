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
//char filename[30] = "Slashdot0811.txt";
//char filename[400] = "E:\\SocialNetworks\\soc-Slashdot0902.txt\\Slashdot0902.txt";
//char filename[30] = "test.txt";
//char filename[100] = "E:\\GitHub\\com-youtube.all.cmty.txt\\com-youtube.all.cmty.txt";
LocalLandmarksScheme<50> LLS;

int main() {
//
//  TestLLS testUnit;
//  testUnit.testCompressGraph();
  std::cout << LLS.constructIndex(filename) << std::endl;
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
