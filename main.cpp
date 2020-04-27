#include <iostream>
#include "PCB.h"

int main() {
    std::cout << "Starting Demo Runs" << std::endl;

    int i = 1;
    PCB run1(false,15,15,15,1);
    run1.leeSolve();
    std::cout << "Demo run " << i << "/2 complete" << std::endl;
    ++i;

    PCB run2(false,15,15,15,1);
    run2.avoidCenter();
    run2.leeSolve();
    std::cout << "Demo run " << i << "/2 complete" << std::endl;

    return 0;
}
