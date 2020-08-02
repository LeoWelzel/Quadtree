#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <SFML/Graphics.hpp>

#include "../defs.h"

class Application
{
public:
    Application();

    void initialise(), run();

private:
    size_t width, height, frameRate;

    bool initialised;

    std::string title;

    
};

#endif