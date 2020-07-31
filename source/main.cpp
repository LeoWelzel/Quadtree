#include <iostream>

#include "freelist/freestack.h"

int main()
{
    FreeStack<int> freeStack;

    std::cout << "Freestack instantiated with " << freeStack.size() << " elements.\n";
}