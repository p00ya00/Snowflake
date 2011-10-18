#ifndef SF_SHARED_LIB_HPP
#define SF_SHARED_LIB_HPP

#ifdef _WIN32

#include <Windows.h>

typedef HINSTANCE LibHandle;
#define LoadLib(lib) LoadLibrary(TEXT(lib))
#define CloseLib(lib) FreeLibrary(lib)
#define LoadFunc(lib, fname, type)  (type) GetProcAddress(lib, fname)
#define Loaded(fPtr) (fPtr != NULL)
#define LibErrorMsg  "Cannot load DLL or function"

#else

#include <dlfcn.h>

typedef void * LibHandle;
#define LoadLib(lib) dlopen(lib, RTLD_LAZY)
#define CloseLib(lib) dlclose(lib)
#define LoadFunc(lib, fname, type)  dlerror();\ //reset errors first
                                    (type) dlsym(lib, fname)
#define Loaded(fPtr) !dlerror()
#define LibErrorMsg dlerror()

#endif

#endif