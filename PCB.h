#ifndef LEEBASETHREE_PCB_H
#define LEEBASETHREE_PCB_H

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <time.h>
#include <stack>
#include <cmath>
#include "mnPoint.h"
#include "Cell.h"

using namespace std;
const int edgeWt = 4;
const int pathWt = 4;

class PCB {
public:
    // items
    vector<vector<Cell>> grid;
    list<mnPoint> L,L1;
    mnPoint mySource, myTarget;
    int rows, cols, numBlocks, curBid;
    bool targetFound;

    // Big Three
    PCB(bool mystatus, int myrows, int mycols, int myBlocks, int mybid); // ctor


    // functions
    mnPoint getRandomPt();
    mnPoint getOpenPt();
    void setSource(mnPoint P);
    void setTarget(mnPoint P);
    void setObstacle(mnPoint P);
    void gridInit();
    void obstacleInit();
    void printCoordinates();
    void printCellTypes();
    bool onGrid(mnPoint temp);
    bool exploreCell (mnPoint P);
    akerWt nextAker(Cell cur);
    bool exploreCellAker(mnPoint P, mnPoint cur);
    void exploreL (mnPoint P);
    void exploreLAker(mnPoint P);
    void iterateL_Lee ();
    void iterateL_Aker();
    bool termCriteriaMet();
    int manhattanDist(mnPoint P1, mnPoint P2);
    int radialDist(mnPoint P1, mnPoint P2);
    void updateL();
    void printExplored();
    void printExploredAker();
    void printL();
    void printL1();
    void printST(mnPoint source, mnPoint target);
    bool Ligible(mnPoint P);
    bool possiblePrev (mnPoint P,Cell cur);
    mnPoint findPrev(mnPoint P);
    mnPoint findPrevAker (mnPoint P);
    bool possiblePrevAker (mnPoint P,Cell cur);
    void printRoute();
    void printRouteAkers();
    void backTrace(mnPoint source, mnPoint target);
    void backTraceAker(mnPoint source, mnPoint target);
    bool centerHeuristic(mnPoint source, mnPoint target);
    void leeSolve();
    void akerSolve();
    void avoidCenter();
    void avoidRadial(mnPoint c);
    void avoidEdge();
    void addAvoidPath();
    void printCellWeights();

private:
    //nothing
};


#endif //LEEBASETHREE_PCB_H
