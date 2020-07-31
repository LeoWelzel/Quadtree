#include <iostream>

#include "tests.h"

void tests()
{
    // ParentContainer<int, 8> test;

    FreeStack<int, 8> test;

    for (int i = 0; i < 8; i++) test.reallocate();

    std::cout << "Function completed...\n";
}

int main()
{
    // Tests::runTests();
    // std::cout << "Tests completed.\n";
    tests();

    std::cout << "Function exited.\n";

    std::cout << "Done.\n";
    std::cin.ignore();
}