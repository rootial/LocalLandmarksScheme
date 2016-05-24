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

int main() {
  srand(time(NULL));

//  testUnit.testCompressGraph();
//  testUnit.testQueryDistanceLLS();
//  testUnit.generateLargestComponent(0);
//  testUnit.outputExactAnswer(1);
  testUnit.testLLSAverageError(3, 1);
  return 0;
}
