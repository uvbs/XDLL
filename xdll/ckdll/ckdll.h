
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CKDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CKDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CKDLL_EXPORTS
#define CKDLL_API __declspec(dllexport)
#else
#define CKDLL_API __declspec(dllimport)
#endif

extern "C" CKDLL_API int Check(void);

