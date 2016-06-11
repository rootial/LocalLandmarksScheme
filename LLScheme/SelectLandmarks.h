#ifndef SELECTLANDMARKS_H_INCLUDED
#define SELECTLANDMARKS_H_INCLUDED

#include "CommonHeader.h"

std::vector<int> dijkstra(int st, int cVertices, ConstGPtr graph);
std::vector<int> selectLandmarks(ConstGPtr graph, int cVertices, int numSelected, int selectionType);



#endif // SELECTLANDMARKS_H_INCLUDED
