#include <iostream>

#include "ui/application.h"

void runApplication()
{
    Application application = Application(500, 500, 300, "Application", 256, 30, 30, sf::Color(0, 11, 45), 16384, 16384);
    application.run();
}

int main()
{
    runApplication();
    std::cout << "Done.\n";
}