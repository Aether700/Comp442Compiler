#pragma once


//#ifdef DEBUG_MODE
    #define DEBUG_BREAK() __debugbreak()
    #define ASSERT(x) if (!(x)) { DEBUG_BREAK(); }
//#else
//    #define DEBUG_BREAK()
//    #define ASSERT(x)
//#endif