#include "PCB.h"


PCB::PCB(bool mystatus, int myrows = 15, int mycols=15, int myBlocks=20, int mybid=1):
    rows(myrows), cols(mycols), numBlocks(myBlocks), curBid(mybid), targetFound(false),
    myTarget({0,0}),mySource({0,0})
{
// Sanity Check
if (myrows > 20) {rows = 20;}
if (mycols > 20) {cols = 20;}
if (numBlocks > myrows * mycols) {numBlocks = (myrows*mycols*3)/20;} // 15% max
if (curBid!=1){curBid=1;}


// Init setup
gridInit();
obstacleInit();
mySource = getOpenPt();
setSource(mySource);
myTarget = getOpenPt();
setTarget(myTarget);


// display visuals
printCellTypes();
printST(mySource,myTarget);

// define source as nearer to edge of board
bool swapST = centerHeuristic(mySource,myTarget);
if (swapST) {
    cout << "Switching source & target to reduce search time" << endl;
    swap(mySource,myTarget);
    // reprintables
    printCellTypes();
    printST(mySource,myTarget);
} else {cout << "No need to switch source & target" << endl;}

// Visuals again
L.clear();L1.clear();
L.push_back(mySource);
printL(); printL1();

}

//begin functions
mnPoint PCB::getRandomPt() {
    return {rand()%rows, rand()%cols};
}

mnPoint PCB::getOpenPt() {
    mnPoint temp = getRandomPt();
    if (grid[temp.m][temp.n].contents == open) {
        return temp;
    } else {
        return getOpenPt();
    }
}

void PCB::setSource(mnPoint P) {
    grid[P.m][P.n].contents=source;
    grid[P.m][P.n].viewStatus=explored;
    grid[P.m][P.n].myAkersWt=c;
    grid[P.m][P.n].prevAkersWt=b;
}

void PCB::setTarget(mnPoint P) {
    grid[P.m][P.n].contents=target;
}

void PCB::setObstacle(mnPoint P) {
    grid[P.m][P.n].contents=obstacle;
}

void PCB::gridInit() {
    grid.resize(rows);
    for (int i = 0; i < cols; ++i) {
        grid[i].resize(cols);
    }
    // now open cells
    for (int m = 0; m < rows; ++m) {
        for (int n = 0; n < cols; ++n) {
            grid[m][m] = Cell();
        }
    }
}

void PCB::obstacleInit() {
    mnPoint P = getRandomPt();
    for (int i = 0; i < numBlocks;) {
        P = getRandomPt();
        if (grid[P.m][P.n].contents == open) {
            setObstacle(P); ++i;
        }
    }

}

void PCB::printCoordinates() {
    std::cout << "Begin printing cell coordinates" << std::endl;
    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            cout << "(" << i << ", " << j << ")   ";
        }
        cout << "\n";
    }

    std::cout << "Done printing cell coordinates\n\n" << std::endl;
}

//print cell coordinates X(blocked),S(ource),T(arget),w(ired),_(open)
void PCB::printCellTypes() {
    std::cout << "Begin printing cell content" << std::endl;
    for (int m = 0; m < cols; ++m) {
        for (int n = 0; n < rows; ++n) {
            cellType x = grid[m][n].contents;
            if (x==obstacle){
                cout << "X  ";
            } else if (x==source) {
                cout << "S  ";
            } else if (x==target) {
                cout << "T  ";
            } else{ // x == open
                cout << "_  ";
            }

        }
        cout << "\n";
    }

    std::cout << "Done printing cell content\n" << std::endl;
}

bool PCB::onGrid(mnPoint temp) {
    return !(temp.m < 0 || temp.n < 0 || temp.m >= rows || temp.n >= cols);
}

// add cells to L1
bool PCB::exploreCell(mnPoint P) {
    // false if off grid or occupied
    if (onGrid(P) && grid[P.m][P.n].contents != obstacle) {
        // false if already looked at this cell
        if (grid[P.m][P.n].viewStatus == newCell) {
            // explore the cell...
            // false if we use the cell now
            if (grid[P.m][P.n].cost == 1) { // add if cellCost = current bid
                grid[P.m][P.n].leeWt = curBid;
                L1.push_back(P);
                grid[P.m][P.n].viewStatus = explored;
                if (grid[P.m][P.n].contents==target) {
                    targetFound = true;
                    cout << "\nTarget found with path cost = " << curBid;
                }
            }
                // true if we still need to consider this cell
            else { // this cell cost extra, but next round it is possible to add
                --grid[P.m][P.n].cost;
                return true;
            }
        }
    }
    return false;
}

// sequence a,b,c,a,b,c...
akerWt PCB::nextAker(Cell cur){
    if (cur.myAkersWt == c) {
        return a;
    } else if (cur.myAkersWt == b) {
        return c;
    }
    return b;
}

// akers method version
bool PCB::exploreCellAker(mnPoint P, mnPoint cur) {
    // false if off grid or occupied
    if (onGrid(P) && grid[P.m][P.n].contents != obstacle) {
        // false if already looked at this cell
        if (grid[P.m][P.n].viewStatus == newCell) {
            // explore the cell...
            // false if we use the cell now
            if (grid[P.m][P.n].cost == 1) { // add if cellCost = current bid
                grid[P.m][P.n].myAkersWt = nextAker(grid[cur.m][cur.n]);
                grid[P.m][P.n].prevAkersWt = grid[cur.m][cur.n].myAkersWt;
                grid[P.m][P.n].leeWt = curBid;
                L1.push_back(P);
                grid[P.m][P.n].viewStatus = explored;
                if (grid[P.m][P.n].contents==target) {
                    targetFound = true;
                    cout << "\nTarget found with path cost = " << curBid;
                }
            }
                // true if we still need to consider this cell
            else { // this cell cost extra, but next round it is possible to add
                --grid[P.m][P.n].cost;
                return true;
            }
        }
    }
    return false;
}

// for a cell in L, decide what to put in L1;
void PCB::exploreL(mnPoint P) {
    mnPoint up = {P.m-1,P.n}; exploreCell(up);
    mnPoint down = {P.m+1,P.n}; exploreCell(down);
    mnPoint right = {P.m, P.n+1}; exploreCell(right);
    mnPoint left = {P.m, P.n-1}; exploreCell(left);
}

// for a cell in L, decide what to put in L1;
void PCB::exploreLAker(mnPoint P) {
    mnPoint up = {P.m-1,P.n}; exploreCellAker(up,P);
    mnPoint down = {P.m+1,P.n}; exploreCellAker(down,P);
    mnPoint right = {P.m, P.n+1}; exploreCellAker(right,P);
    mnPoint left = {P.m, P.n-1}; exploreCellAker(left,P);
}

void PCB::iterateL_Lee(){
    for (auto it: L) {
        exploreL(it); // use this if not doing akers method
    }
}

void PCB::iterateL_Aker() {
    for (auto it: L) {
        exploreLAker(it);
    }
}

bool PCB::termCriteriaMet() {
    return (targetFound || L.empty());
}

int PCB::manhattanDist(mnPoint P1, mnPoint P2){
    return (abs(P1.m - P2.m) + abs(P1.n - P2.n));
}

// move all elements of L1 into L, and then clear L1
void PCB::updateL() {
    list<mnPoint> temp;
    for (auto it: L) {
        if (Ligible(it))
            temp.push_back(it);
    }

    std::swap(temp,L);

    for (auto it: L1) {
        L.push_back(it);
    }
    cout << "\n";

    printL1();
    L1.clear();
}

void PCB::printExplored() {
    std::cout << "Begin printing cell content" << std::endl;
    for (int m = 0; m < cols; ++m) {
        for (int n = 0; n < rows; ++n) {
            auto x = grid[m][n].contents;
            if (x==obstacle){
                cout << "X  ";
            } else if (x==source) {
                cout << "S  ";
            } else if (x==target) {
                cout << "T  ";
            } else{ // x == open
                if (grid[m][n].viewStatus == explored) {
//                    cout << "v  ";
                    cout << grid[m][n].leeWt%10 << "  ";
                } else {
                    cout << "-  ";
                }
            }
        }
        cout << "\n";
    }

    std::cout << "Done printing cell content\n" << std::endl;
}

void PCB::printExploredAker() {
    std::cout << "Begin printing Akers Weight content" << std::endl;
    for (int m = 0; m < cols; ++m) {
        for (int n = 0; n < rows; ++n) {
            auto x = grid[m][n].contents;
            if (x==obstacle){
                cout << "X  ";
            } else if (x==source) {
                cout << "S  ";
            } else if (x==target) {
                cout << "T  ";
            } else{ // x == open
                if (grid[m][n].viewStatus == explored) {
//                    cout << "v  "; // will show all explored cells, not their weights
                    cout << grid[m][n].myAkersWt << "  ";
                } else {
                    cout << "-  ";
                }
            }
        }
        cout << "\n";
    }

    std::cout << "Done printing Akers Weights\n" << std::endl;
}

void PCB::printL() {
    cout << "L contains: ";
    for (auto it: L) {
        cout << "   " << it;
    }
}

void PCB::printL1() {
    cout << "L1 contains: ";
    for (auto it: L1) {
        cout << "   " << it;
    }
    cout << endl;
}

void PCB::printST(mnPoint source, mnPoint target) {
    cout << "center: " << mnPoint(rows/2,cols/2) << endl;
    cout << "Source: " << source << endl;
    cout << "Target: " << target << endl;
}

bool PCB::Ligible(mnPoint P) {
    bool keepup(true),keepdown(true),keepright(true),keepleft(true);
    mnPoint upP(P.m-1,P.n),downP(P.m+1,P.n),rightP(P.m,P.n+1),leftP(P.m,P.n-1);

    if (!onGrid(upP) || grid[upP.m][upP.n].contents == obstacle || grid[upP.m][upP.n].viewStatus == explored) {
        keepup = false;
    }

    if (!onGrid(downP) || grid[downP.m][downP.n].contents == obstacle || grid[downP.m][downP.n].viewStatus == explored) {
        keepdown = false;
    }


    if (!onGrid(rightP) || grid[rightP.m][rightP.n].contents == obstacle || grid[rightP.m][rightP.n].viewStatus == explored) {
        keepright = false;
    }


    if (!onGrid(leftP) || grid[leftP.m][leftP.n].contents == obstacle || grid[leftP.m][leftP.n].viewStatus == explored) {
        keepleft = false;
    }

    return (keepup || keepdown || keepleft || keepright);

}

bool PCB::possiblePrev (mnPoint P, Cell cur) {
    int m(P.m),n(P.n);
    if (!onGrid(P)) {
        return false;
    }

    Cell c = grid[m][n];
    if (c.contents == obstacle) {
        return false;
    }

    if (c.viewStatus == newCell) {
        return false;
    }

    if(c.leeWt >= cur.leeWt) {
        return false;
    }

    return true;
}

bool PCB::possiblePrevAker (mnPoint P,Cell cur) {
    int m(P.m),n(P.n);
    if (!onGrid(P)) {
        return false;
    }

    Cell thisCell = grid[m][n];
    if (thisCell.contents == obstacle) {
        return false;
    }

    if (thisCell.viewStatus == newCell) {
        return false;
    }

    if(cur.myAkersWt == c){
        if (thisCell.myAkersWt == b) { return true;}
    } else if (cur.myAkersWt == b) {
        if (thisCell.myAkersWt == a) { return true;}
    } else if (cur.myAkersWt == a) {
        if (thisCell.myAkersWt == c) { return true;}
    }

    return false;
}

mnPoint PCB::findPrevAker (mnPoint P) {
    Cell cur = grid[P.m][P.n];
    mnPoint up(P.m -1,P.n);
    if (possiblePrevAker(up,cur)) {return up;}

    mnPoint down(P.m +1,P.n);
    if (possiblePrevAker(down,cur)) {return down;}

    mnPoint right(P.m,P.n+1);
    if (possiblePrevAker(right,cur)) {return right;}

    mnPoint left(P.m, P.n-1);
    return left;
}

mnPoint PCB::findPrev(mnPoint P) {
    Cell cur = grid[P.m][P.n];
    mnPoint up(P.m -1,P.n);
    if (possiblePrev(up,cur)) {return up;}

    mnPoint down(P.m +1,P.n);
    if (possiblePrev(down,cur)) {return down;}

    mnPoint right(P.m,P.n+1);
    if (possiblePrev(right,cur)) {return right;}

    mnPoint left(P.m, P.n-1);
    return left;
}

void PCB::printRoute() {
    std::cout << "Begin Lee's route" << std::endl;
    for (int m = 0; m < cols; ++m) {
        for (int n = 0; n < rows; ++n) {
            cellType x = grid[m][n].contents;
            if (x==obstacle){
                cout << "X  ";
            } else if (x==source) {
                cout << "S  ";
            } else if (x==target) {
                cout << "T  ";
            } else{ // x == open
                bool included = grid[m][n].onRoute;
                if (included) {
                    cout << grid[m][n].leeWt%10 << "  ";
                } else {
                    cout << "_  ";
                }
            }

        }
        cout << "\n";
    }

    std::cout << "Finished printing Lee's route\n" << std::endl;
}

void PCB::printRouteAkers() {
    std::cout << "Begin Aker's route" << std::endl;
    for (int m = 0; m < cols; ++m) {
        for (int n = 0; n < rows; ++n) {
            cellType x = grid[m][n].contents;
            if (x==obstacle){
                cout << "X  ";
            } else if (x==source) {
                cout << "S  ";
            } else if (x==target) {
                cout << "T  ";
            } else{ // x == open
                bool included = grid[m][n].onRoute;
                if (included) {
                    cout << grid[m][n].myAkersWt << "  ";
                } else {
                    cout << "_  ";
                }
            }

        }
        cout << "\n";
    }
    std::cout << "Done printing Aker's route\n" << std::endl;
}

void PCB::backTrace(mnPoint source, mnPoint target) {
    stack<mnPoint> route;
    route.push(target);
    while (route.top() != source) {
        route.push(findPrev(route.top()));
    }

    while (!route.empty()) {
        mnPoint rtPt = route.top();
        route.pop();
        grid[rtPt.m][rtPt.n].onRoute = true;
    }

    printRoute();
}

void PCB::backTraceAker(mnPoint source, mnPoint target) {
    stack<mnPoint> route;
    route.push(target);
    while (route.top() != source) {
        route.push(findPrevAker(route.top()));
    }

    while (!route.empty()) {
        mnPoint rtPt = route.top();
        route.pop();
        grid[rtPt.m][rtPt.n].onRoute = true;
    }

    printRoute();
    printRouteAkers();
}

bool PCB::centerHeuristic(mnPoint mysource, mnPoint mytarget) {
    mnPoint center(rows/2,cols/2);
    int targetDist = manhattanDist(center,mytarget);
    int sourceDist = manhattanDist(center,mysource);

    // Select the one closer to the edge as the source
    if (targetDist > sourceDist) {
        // need to flip source and target
        swap(grid[mysource.m][mysource.n],grid[mytarget.m][mytarget.n]);
        return true;
    }
    return false;
}

void PCB::leeSolve(){
    while (!termCriteriaMet()){
        cout << "-------------------------------------------" << endl;
        ++curBid;
        cout << "Begin iteration for cost = " << curBid << endl;
        printL();
        iterateL_Lee();
        updateL();
        printExplored();
        cout << "-------------------------------------------" << endl;
    }

    printExplored();
    backTrace(mySource, myTarget);

}

void PCB::akerSolve(){
    while (!termCriteriaMet()){
        cout << "-------------------------------------------" << endl;
        ++curBid;
        cout << "Begin iteration for cost = " << curBid << endl;
        printL();
        iterateL_Aker();
        updateL();
        printExploredAker();
        cout << "-------------------------------------------" << endl;
    }

    printExploredAker();
    backTraceAker(mySource,myTarget);
}

void PCB::avoidCenter() {
    mnPoint center({rows/2,cols/2}),top({0,0});
    int maxDist = manhattanDist(center,top);

    for (int m = 0; m < rows; ++m) {
        for (int n = 0; n < cols; ++n) {
            grid[m][n].cost = maxDist - manhattanDist(center,{m,n});
        }
    }

    printCellWeights();
}

void PCB::avoidEdge() {
    // top
    for (int n = 0; n < cols; ++n) {
        grid[0][n].cost = edgeWt;
    }

    // left
    for (int m = 0; m < rows; ++m) {
        grid[m][0].cost = edgeWt;
    }

    // right
    for (int m = 0; m < rows; ++m) {
        grid[m][cols-1].cost = edgeWt;
    }

    // bottom
    for (int n = 0; n < cols; ++n) {
        grid[rows-1][n].cost = edgeWt;
    }

    printCellWeights();
}

void PCB::addAvoidPath() {
    int centerCol = cols/2;
    int centerRow = rows/2;

    for (int m = 0; m < rows; ++m) {
        grid[m][centerCol].cost = pathWt;
    }

    for (int n = 0; n < cols; ++n) {
        grid[centerRow][n].cost = pathWt;
    }

    printCellWeights();

};

void PCB::printCellWeights() {
    std::cout << "Begin printing cell weights" << std::endl;
    for (int m = 0; m < cols; ++m) {
        for (int n = 0; n < rows; ++n) {
            cellType x = grid[m][n].contents;
            if (x==obstacle){
                cout << "X  ";
            } else if (x==source) {
                cout << "S  ";
            } else if (x==target) {
                cout << "T  ";
            } else{ // x == open
                int formattedCost = grid[m][n].cost;
                if (formattedCost < 10) {cout << grid[m][n].cost <<"  ";}
                else {cout << grid[m][n].cost <<" ";}
            }

        }
        cout << "\n";
    }

    std::cout << "Done printing cell weights\n" << std::endl;
}
