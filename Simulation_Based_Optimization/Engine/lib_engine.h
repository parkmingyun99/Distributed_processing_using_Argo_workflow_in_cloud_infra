#pragma once

#if defined(SM_SIM_BUILD)
#ifdef _MSC_VER
#define SM_SIM_EXT_CLASS __declspec(dllexport)
#elif defined __GNUC__
        #define SM_SIM_EXT_CLASS __attribute__ ((visibility("default")))
#endif
#else
#ifdef _MSC_VER
        #define SM_SIM_EXT_CLASS __declspec(dllimport)
#ifdef _DEBUG
            #pragma comment(lib,"Engined")
#elif NDEBUG
            #pragma comment(lib,"Engine")
#endif
#elif defined __GNUC__
        #define SM_SIM_EXT_CLASS __attribute__ ((visibility ("hidden")))
#endif
#endif


#pragma warning(disable : 4251)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#define _SCL_SECURE_NO_WARNINGS 1

#define EN_EV_COUNT  1
#define ENG_LOG_TRACE_ENABLE
