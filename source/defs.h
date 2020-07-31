#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define DEBUGGING

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