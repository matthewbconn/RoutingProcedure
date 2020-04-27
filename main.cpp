#include <iostream>
#include "PCB.h"

int main() {
    std::cout << "Starting Demo Runs" << std::endl;
    int x = 11;

    int i(1),j(5);
    std::cout << "###################################################" << std::endl;
    PCB run1(false,x,x,x,1);
    run1.leeSolve();
    std::cout << "Demo run " << i << "/" << j << " complete" << std::endl;
    ++i;

    std::cout << "###################################################" << std::endl;
    PCB run2(false,x,x,x,1);
    run2.avoidCenter();
    run2.leeSolve();
    std::cout << "Demo run " << i << "/" << j << " complete" << std::endl;
    i++;

    std::cout << "###################################################" << std::endl;
    PCB run3(false,x,x,x,1);
    run3.addAvoidPath();
    run3.akerSolve();
    std::cout << "Demo run " << i << "/" << j << " complete" << std::endl;
    ++i;

    std::cout << "###################################################" << std::endl;
    PCB run4(false,x,x,7,1);
    run4.avoidEdge();
    run4.akerSolve();
    std::cout << "Demo run " << i << "/" << j << " complete" << std::endl;
    ++i;

    std::cout << "###################################################" << std::endl;
    PCB run5(false,x,x,x,1);
    run5.avoidRadial({x/2,x/2});
    run5.leeSolve();
    std::cout << "Demo run " << i << "/" << j << " complete" << std::endl;

    return 0;
}
