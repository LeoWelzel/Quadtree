#include <iostream>

#include "tests.h"
#include "temptest.h"

void test()
{
    TempTest tempTest;

    for (int i = 0; i < 15; i++)
        tempTest.doubleSize();
    std::cout << "Done.\n";
    std::cout.flush();
}

int main()
{
    Tests::runTests();
    std::cout << "Tests completed.\n";
}