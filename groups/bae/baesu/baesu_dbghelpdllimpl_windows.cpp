// baesu_dbghelpdllimpl_windows.cpp                                   -*-C++-*-
#include <baesu_dbghelpdllimpl_windows.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_dbghelpdllimpl_windows_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM__OS_WINDOWS)

#include <bcemt_qlock.h>

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

#pragma optimize("", off)

#undef  TRACES
#define TRACES 0    // debugging traces off

#if TRACES == 1
# include <stdio.h>

#define eprintf printf
#define zprintf printf

#else

# define eprintf (void)    // only called on errors
# define zprintf (void)    // called on debug output - output is very
                           // voluminous if this is turned on
#endif

namespace {

                         // =========================
                         // local struct Dbghelp_Util
                         // =========================

struct Dbghelp_Util {
    // This struct contains the handle of the DLL and a collection of function
    // ptrs that will point to functions loaded at run time from it.  Only
    // one instance of this type is to exist, a static instance whose d'tor
    // will call 'SymCleanup' if the object has been initialized.

    // TYPES
    typedef DWORD __stdcall SymSetOptionsProc(DWORD);
    typedef BOOL  __stdcall SymInitializeProc(HANDLE, PCSTR, BOOL);
#ifdef BSLS_PLATFORM__CPU_32_BIT
    // For some reason, 'symFromAddr' doesn't work right on 64 bit, so we
    // #ifdef everything and use 'SymGetSymFromAddr64'.

    typedef BOOL  __stdcall SymFromAddrProc(HANDLE,
                                            DWORD64,
                                            PDWORD64,
                                            PSYMBOL_INFO);
#else
    typedef BOOL  __stdcall SymGetSymFromAddr64Proc(HANDLE,
                                                    DWORD64,
                                                    PDWORD64,
                                                    PIMAGEHLP_SYMBOL64);
#endif
    typedef BOOL  __stdcall SymGetLineFromAddr64Proc(HANDLE,
                                                     DWORD64,
                                                     PDWORD,
                                                     PIMAGEHLP_LINE64);
    typedef BOOL  __stdcall StackWalk64Proc(DWORD,
                                            HANDLE,
                                            HANDLE,
                                            LPSTACKFRAME64,
                                            PVOID,
                                            PREAD_PROCESS_MEMORY_ROUTINE64,
                                            PFUNCTION_TABLE_ACCESS_ROUTINE64,
                                            PGET_MODULE_BASE_ROUTINE64,
                                            PTRANSLATE_ADDRESS_ROUTINE64);
    typedef BOOL  __stdcall SymCleanupProc(HANDLE);

    // DATA
    HMODULE                          d_moduleHandle;  // handle of the DLL
                                                      // that we will load the
                                                      // functions from

    SymSetOptionsProc               *d_symSetOptions; // 'SymSetOptions' func

    SymInitializeProc               *d_symInitialize; // 'SymInitialize' func

#ifdef BSLS_PLATFORM__CPU_32_BIT
    SymFromAddrProc                 *d_symFromAddr;   // 'SymFromAddr' func
#else
    SymGetSymFromAddr64Proc         *d_symGetSymFromAddr64;
                                                      // 'SymGetSymFromAddr64'
                                                      // func
#endif
    SymGetLineFromAddr64Proc        *d_symGetLineFromAddr64;
                                                 // 'SymGetLineFromAddr64' func

    StackWalk64Proc                 *d_stackWalk64;   // 'StackWalk64' func

    SymCleanupProc                  *d_symCleanup;    // 'SymCleanup' func

    PFUNCTION_TABLE_ACCESS_ROUTINE64 d_symFunctionTableAccess64;
                 // Pointer to the Windows 'SymFunctionTableAccess64' function.

    PGET_MODULE_BASE_ROUTINE64       d_symGetModuleBase64;
                 // Pointer to the Windows 'SymGetModuleBase64' function.

    HANDLE                           d_hProcess;

  private:
    // PRIVATE MANIPULATORS
    int load();
        // Open the dll and get the function pointers, return 0 on success and
        // a non-zero value otherwise.

    void wipeClean();
        // Null out all the pointers owned by this class.

  public:
    // CREATORS
    ~Dbghelp_Util();
        // Destroy this object.  Exactly one object of this type will be
        // created, and that object is static, so that resources can be cleaned
        // up upon process termination.

    // MANIPULATORS
    int init();
        // Ensure the dll is loaded.  Return 0 on success and a non-zero value
        // otherwise.  'init' is separated from 'load' to make it very small,
        // to make sure it is inlined, and to keep the code size smaller.

    // ACCESSORS
    bool isLoaded();
        // Everything is fully loaded.
};

// PRIVATE MANIPULATORS
int Dbghelp_Util::load()
{
    BSLS_ASSERT(NULL == d_moduleHandle);
    BSLS_ASSERT(NULL == d_symSetOptions);
    BSLS_ASSERT(NULL == d_symInitialize);
#ifdef BSLS_PLATFORM__CPU_32_BIT
    BSLS_ASSERT(NULL == d_symFromAddr);
#else
    BSLS_ASSERT(NULL == d_symGetSymFromAddr64);
#endif
    BSLS_ASSERT(NULL == d_symGetLineFromAddr64);
    BSLS_ASSERT(NULL == d_stackWalk64);
    BSLS_ASSERT(NULL == d_symCleanup);
    BSLS_ASSERT(NULL == d_symFunctionTableAccess64);
    BSLS_ASSERT(NULL == d_symGetModuleBase64);
    BSLS_ASSERT(NULL == d_hProcess);

    d_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (NULL == d_moduleHandle) {
        eprintf("baesu_DbghelpDllImpl_Windows: 'LoadLibraryA' failed\n");

        return -1;                                                    // RETURN
    }

    d_symSetOptions = (SymSetOptionsProc *)
                    GetProcAddress(d_moduleHandle, "SymSetOptions");
    d_symInitialize = (SymInitializeProc *)
                    GetProcAddress(d_moduleHandle, "SymInitialize");
#ifdef BSLS_PLATFORM__CPU_32_BIT
    d_symFromAddr = (SymFromAddrProc *)
                    GetProcAddress(d_moduleHandle, "SymFromAddr");
#else
    d_symGetSymFromAddr64 = (SymGetSymFromAddr64Proc *)
                    GetProcAddress(d_moduleHandle, "SymGetSymFromAddr64");
#endif
    d_symGetLineFromAddr64 = (SymGetLineFromAddr64Proc *)
                    GetProcAddress(d_moduleHandle, "SymGetLineFromAddr64");
    d_stackWalk64 = (StackWalk64Proc *)
                    GetProcAddress(d_moduleHandle, "StackWalk64");
    d_symCleanup = (SymCleanupProc *)
                    GetProcAddress(d_moduleHandle, "SymCleanup");
    d_symFunctionTableAccess64 = (PFUNCTION_TABLE_ACCESS_ROUTINE64)
                    GetProcAddress(d_moduleHandle, "SymFunctionTableAccess64");
    d_symGetModuleBase64 = (PGET_MODULE_BASE_ROUTINE64)
                    GetProcAddress(d_moduleHandle, "SymGetModuleBase64");
    d_hProcess = GetCurrentProcess();

    if (!isLoaded()) {
        eprintf("baesu_DbghelpDllImpl_Windows: %s failed\n",
                                     NULL == d_hProcess ? "'GetCurrentProcess'"
                                                        : "'GetProcAddress'";

        wipeClean();
        return -1;                                                    // RETURN
    }

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, this method will only load what is actually required

    BOOL rc = (*d_symInitialize)(d_hProcess, NULL, TRUE);
    if (!rc) {
        eprintf("baesu_DbghelpDllImpl_Windows: 'SymInitialize' failed\n");
        wipeClean();
        return -1;                                                    // RETURN
    }

    return 0;
}

// CREATORS
Dbghelp_Util::~Dbghelp_Util()
{
    BSLS_ASSERT_OPT(
               !BloombergLP::baesu_DbghelpDllImpl_Windows::qLock().isLocked());

    if (isLoaded()) {
        (*d_symCleanup)(d_hProcess);
        FreeLibrary(d_moduleHandle);
    }
}

// MANIPULATORS
inline
int Dbghelp_Util::init()
{
    BSLS_ASSERT(BloombergLP::baesu_DbghelpDllImpl_Windows::qLock().isLocked());

    return NULL == d_moduleHandle ? load() : 0;
}


void Dbghelp_Util::wipeClean()
{
    if (NULL != d_moduleHandle) {
        FreeLibrary(d_moduleHandle);
    }

    d_moduleHandle             = NULL;
    d_symSetOptions            = NULL;
    d_symInitialize            = NULL;
#ifdef BSLS_PLATFORM__CPU_32_BIT
    d_symFromAddr              = NULL;
#else
    d_symGetSymFromAddr64      = NULL;
#endif
    d_symGetLineFromAddr64     = NULL;
    d_stackWalk64              = NULL;
    d_symCleanup               = NULL;
    d_symFunctionTableAccess64 = NULL;
    d_symGetModuleBase64       = NULL;
    d_hProcess                 = NULL;
}

// ACCESSORS
bool Dbghelp_Util::isLoaded()
{
    return  NULL != d_moduleHandle
         && NULL != d_symSetOptions
         && NULL != d_symInitialize
#ifdef BSLS_PLATFORM__CPU_32_BIT
         && NULL != d_symFromAddr
#else
         && NULL != d_symGetSymFromAddr64
#endif
         && NULL != d_symGetLineFromAddr64
         && NULL != d_stackWalk64
         && NULL != d_symCleanup
         && NULL != d_symFunctionTableAccess64
         && NULL != d_symGetModuleBase64
         && NULL != d_hProcess;
}

}  // close unnamed namespace

// DATA
static Dbghelp_Util dbghelp_util = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

                              // -------------
                              // baesu_DbghelpDllImpl_Windows
                              // -------------

namespace BloombergLP {

// DATA
static bcemt_QLock baesu_DbghelpDllImpl_Windows::s_qLock =
                                                       BCEMT_QLOCK_INITIALIZER;

// CLASS METHODS
bool baesu_DbghelpDllImpl_Windows::isLoaded()
{
    return dbghelp_util.isLoaded();
}

DWORD baesu_DbghelpDllImpl_Windows::symSetOptions(DWORD symOptions)
{
    BSLS_ASSERT_OPT(0 == dbghelp_util.init());

    return (*dbghelp_util.d_symSetOptions)(symOptions);
}

#ifdef BSLS_PLATFORM__CPU_32_BIT

BOOL baesu_DbghelpDllImpl_Windows::symFromAddr(DWORD64      address,
                                               PDWORD64     displacement,
                                               PSYMBOL_INFO symbol)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_symFromAddr)(dbghelp_util.d_hProcess,
                                         address,
                                         displacement,
                                         symbol);
}

#else

BOOL baesu_DbghelpDllImpl_Windows::symGetSymFromAddr64(
                                               DWORD64            address,
                                               PDWORD64           displacement,
                                               PIMAGEHLP_SYMBOL64 symbol)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_symGetSymFromAddr64)(dbghelp_util.d_hProcess,
                                                 address,
                                                 displacement,
                                                 symbol);
}

#endif

BOOL baesu_DbghelpDllImpl_Windows::symGetLineFromAddr64(
                                              DWORD64          dwAddr,
                                              PDWORD           pdwDisplacement,
                                              PIMAGEHLP_LINE64 line)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_symGetLineFromAddr64)(dbghelp_util.d_hProcess,
                                                  dwAddr,
                                                  pdwDisplacement,
                                                  line);
}

BOOL baesu_DbghelpDllImpl_Windows::stackWalk64(DWORD          machineType,
                                               HANDLE         hThread,
                                               LPSTACKFRAME64 stackFrame,
                                               PVOID          contextRecord)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_stackWalk64)(
                                       machineType,
                                       dbghelp_util.d_hProcess,
                                       hThread,
                                       stackFrame,
                                       contextRecord,
                                       0,
                                       dbghelp_util.d_symFunctionTableAccess64,
                                       dbghelp_util.d_symGetModuleBase64,
                                       0);
}

}  // close namespace BloombergLP

// WINDOWS
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
