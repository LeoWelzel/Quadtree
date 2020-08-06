#ifndef DEFS_HPP_INCLUDED
#define DEFS_HPP_INCLUDED

#define DEBUGGING
#define APPLICATION

#ifdef DEBUGGING
    #define ASSERTIONS
    #define TO_STRING

    #ifdef ASSERTIONS
        #include <cassert>
    #endif

    #ifdef TO_STRING
        #include <iostream>
        #include <string>
    #endif
#endif

#endif