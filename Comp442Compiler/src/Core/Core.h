#pragma once


#ifdef DEBUG_MODE
    #define DEBUG_BREAK() __debugbreak()
#else
    #define DEBUG_BREAK()
#endif