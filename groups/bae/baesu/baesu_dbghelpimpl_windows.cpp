// baesu_dbghelpimpl_windows.cpp                                      -*-C++-*-
#include <baesu_dbghelpimpl_windows.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_dbghelpimpl_windows_cpp,"$Id$ $CSID$")

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

                            // ===================
                            // struct Dbghelp_Util
                            // ===================

static
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
    bool ok();
        // Everything is fully loaded.
} dbghelp_util = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

}  // close unnamed namespace

// PRIVATE MANIPULATORS
int Dbghelp_Util::load()
{
    BSLS_ASSERT(NULL == d_moduleHandle);
    BSLS_ASSERT(NULL == d_symSetOptions);
    BSLS_ASSERT(NULL == d_symInitialize);
    BSLS_ASSERT(NULL == d_symFromAddr);
#ifdef BSLS_PLATFORM__CPU_32_BIT
    BSLS_ASSERT(NULL == d_symGetSymFromAddr64);
#else
    BSLS_ASSERT(NULL == d_symGetLineFromAddr64);
#endif
    BSLS_ASSERT(NULL == d_stackWalk64);
    BSLS_ASSERT(NULL == d_symCleanup);
    BSLS_ASSERT(NULL == d_symFunctionTableAccess64);
    BSLS_ASSERT(NULL == d_symGetModuleBase64);
    BSLS_ASSERT(NULL == d_hProcess);

    d_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (NULL == d_moduleHandle) {
        eprintf("baesu_Dbghelp: 'LoadLibraryA' failed\n");

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

    if (!ok()) {
        if (NULL == d_hProcess) {
            eprintf("baesu_Dbghelp: 'GetCurrentProcess' failed\n");
        }
        else {
            eprintf("baesu_Dbghelp: 'GetProcAddress' failed\n");
        }

        wipeClean();
        return -1;                                                    // RETURN
    }

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, this method will only load what is actually required

    BOOL rc = (*d_symInitialize)(d_hProcess, NULL, TRUE);
    if (!rc) {
        eprintf("baesu_Dbghelp: 'SymInitialize' failed\n");
        wipeClean();
        return -1;                                                    // RETURN
    }

    return 0;
}

// CLASS METHODS
// CREATORS
Dbghelp_Util::~Dbghelp_Util()
{
    BSLS_ASSERT_OPT(!BloombergLP::baesu_Dbghelp::qLock().isLocked());

    if (ok()) {
        (*d_symCleanup)(d_hProcess);
        FreeLibrary(d_moduleHandle);
    }
}

// MANIPULATORS
inline
int Dbghelp_Util::init()
{
    BSLS_ASSERT(BloombergLP::baesu_Dbghelp::qLock().isLocked());

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
bool Dbghelp_Util::ok()
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

                              // -------------
                              // baesu_Dbghelp
                              // -------------

namespace BloombergLP {

DWORD baesu_Dbghelp::symSetOptions(DWORD arg1)
{
    BSLS_ASSERT_OPT(0 == dbghelp_util.init());

    return (*dbghelp_util.d_symSetOptions)(arg1);
}

#ifdef BSLS_PLATFORM__CPU_32_BIT

BOOL baesu_Dbghelp::symFromAddr(NullArg,
                                DWORD64      arg2,
                                PDWORD64     arg3,
                                PSYMBOL_INFO arg4)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_symFromAddr)(dbghelp_util.d_hProcess,
                                         arg2,
                                         arg3,
                                         arg4);
}

#else

BOOL baesu_Dbghelp::symGetSymFromAddr64(NullArg,
                                        DWORD64            arg2,
                                        PDWORD64           arg3,
                                        PIMAGEHLP_SYMBOL64 arg4)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_symGetSymFromAddr64)(dbghelp_util.d_hProcess,
                                                 arg2,
                                                 arg3,
                                                 arg4);
}

#endif

BOOL baesu_Dbghelp::symGetLineFromAddr64(NullArg,
                                         DWORD64          arg2,
                                         PDWORD           arg3,
                                         PIMAGEHLP_LINE64 arg4)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_symGetLineFromAddr64)(dbghelp_util.d_hProcess,
                                                  arg2,
                                                  arg3,
                                                  arg4);
}

BOOL baesu_Dbghelp::stackWalk64(DWORD          arg1,
                                NullArg,
                                HANDLE         arg3,
                                LPSTACKFRAME64 arg4,
                                PVOID          arg5,
                                NullArg,
                                NullArg,
                                NullArg,
                                NullArg)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*dbghelp_util.d_stackWalk64)(
                                     arg1,
                                     dbghelp_util.d_hProcess,
                                     arg3,
                                     arg4,
                                     arg5,
                                     0,
                                     dbghelp_util.d_symFunctionTableAccess64,
                                     dbghelp_util.d_symGetModuleBase64,
                                     0);
}

bool baesu_Dbghelp::loadedOK()
{
    return dbghelp_util.ok();
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
