#include "tests.h"

namespace Tests
{
    const int ITERATIONS = /* 65537 */ 1025;

    void stackPushTest()
    {
        FreeStack<int, 2> stack;

        for (int i = 0; i < ITERATIONS; i++)
        {
            stack.pushBack(i);
            assert(stack.size() == i + 1);
        }

        std::cout << "Stack push test succeeded.\n";
    }

    void stackVisualPushTest()
    {
        FreeStack<int, 2> stack;

        for (int i = 0; i < 258; i++)
        {
            stack.pushBack(i);
        }
        
        std::cout << stack.toString() << '\n';
        std::cout << "Stack visual push test succeeded.\n";
    }

    void stackPopTest()
    {
        FreeStack<int> stack;

        const int numEntries = ITERATIONS;
        for (int i = 0; i < numEntries; i++)
            stack.pushBack(i);

        for (int i = 0; i < numEntries; i++)
        {
            assert(stack.size() == numEntries - i);
            stack.popBack();
            assert(stack.size() == numEntries - i - 1);
        }
        
        std::cout << "Stack pop test succeeded.\n";
    }

    void stackTests()
    {
        stackVisualPushTest();
        stackPushTest();
        stackPopTest();
    }

    void listInsertTest()
    {
        FreeList<int> list;

        for (int i = 0; i < ITERATIONS; i++)
            list.insert(i);

        std::cout << "Insertion test complete.\n";
    }

    void listRemovalTest()
    {
        FreeList<int> list;

        for (int i = 0; i < ITERATIONS; i++)
            list.insert(i);

        #ifdef DEBUGGING
        const int initialCapacity = list.getCapacity(),
            initialSize = list.size();
        #endif

        for (int i = 0; i < ITERATIONS; i += 7)
            list.erase(i);

        for (int i = 0; i < ITERATIONS; i += 7)
            list.insert(i * 7);

        #ifdef DEBUGGING
        const int finalCapacity = list.getCapacity(),
            finalSize = list.size();
        
        assert(initialCapacity == finalCapacity);
        assert(initialSize == finalSize);
        #endif

        std::cout << "Removal test complete.\n";
    }

    void listVisualTest()
    {
        FreeList<int> list;

        const int localIterations = 258;
        for (int i = 0; i < localIterations; i++)
            list.insert(i);
        for (int i = 0; i < localIterations; i += 7)
            list.erase(i);
        for (int i = 0; i < localIterations; i += 7)
            list.insert(i * 7);

        std::cout << list.toString() << '\n';
        std::cout << "List visual test complete.\n";
    }

    void listTests()
    {
        listInsertTest();
        listRemovalTest();
        listVisualTest();

        std::cout << "List tests completed.\n";
    }

    void runTests()
    {
        stackTests();
        listTests();
    }
}