#ifndef COMMONHEADER_H_INCLUDED
#define COMMONHEADER_H_INCLUDED

#include <cassert>
#include <cstdint>
#include <queue>
#include <vector>

typedef std::pair<int, uint8_t> PIU;

const int maxBlockSize = 16;
const uint8_t INF8 = 100;
#define Delete(x) {if (x != NULL) {delete []x; x = NULL;}}
#define Debug(x) printf("Excute here %d lines\n", (x))

#endif // COMMONHEADER_H_INCLUDED
