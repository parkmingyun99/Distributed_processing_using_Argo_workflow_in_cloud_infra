#pragma once

#if defined(SM_MODEL_BUILD)
#ifdef _MSC_VER
        #define SM_MODEL_CLASS __declspec(dllexport)
    #elif defined __GNUC__
        #define SM_MODEL_CLASS __attribute__ ((visibility("default")))
    #endif
#else
#ifdef _MSC_VER
#define SM_MODEL_CLASS __declspec(dllimport)
        #ifdef _DEBUG
            #pragma comment(lib,"SmModeld")
        #elif NDEBUG
            #pragma comment(lib,"SmModel")
        #endif
#elif defined __GNUC__
#define SM_MODEL_CLASS __attribute__ ((visibility ("hidden")))
#endif
#endif


#pragma warning(disable : 4251)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#define MDL_LOG_TRACE_ENABLE 1
