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

                        // ======================
                        // struct Dbghelp_Util
                        // ======================

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
    static HMODULE                    s_moduleHandle;  // handle of the DLL
                                                       // that we will load the
                                                       // functions from

    static SymSetOptionsProc         *s_symSetOptions; // 'SymSetOptions' func

    static SymInitializeProc         *s_symInitialize; // 'SymInitialize' func

#ifdef BSLS_PLATFORM__CPU_32_BIT
    static SymFromAddrProc           *s_symFromAddr;   // 'SymFromAddr' func
#else
    static SymGetSymFromAddr64Proc   *s_symGetSymFromAddr64;
                                                       // 'SymGetSymFromAddr64'
                                                       // func
#endif
    static SymGetLineFromAddr64Proc  *s_symGetLineFromAddr64;
                                                 // 'SymGetLineFromAddr64' func

    StackWalk64Proc                  *s_stackWalk64;   // 'StackWalk64' func

    static SymCleanupProc            *s_symCleanup;    // 'SymCleanup' func

    static PFUNCTION_TABLE_ACCESS_ROUTINE64
                                      s_symFunctionTableAccess64;
        // Return a pointer to the Windows 'SymFunctionTableAccess64' function.

    static PGET_MODULE_BASE_ROUTINE64 s_symGetModuleBase64;

    static HANDLE                     s_hProcess;

  private:
    // PRIVATE CLASS METHODS
    int load();
        // Open the dll and get the function pointers, return 0 on success and
        // a non-zero value otherwise.

  public:
    // CLASS METHODS
    int init();
        // Ensure the dll is loaded.  Return 0 on success and a non-zero value
        // otherwise.  'init' is separated from 'load' to make it very small,
        // to make sure it is inlined, and to keep the code size smaller.

    // CREATORS
    ~DbghelpUtil();
        // Destroy this object.  Exactly one object of this type will be
        // created, and that object is static, so that resources can be cleaned
        // up upon process termination.

    // MANIPULATORS
    void wipeClean();
        // Null out all the pointers owned by this class.

    // ACCESSORS
    bool ok();
        // Everything is fully loaded.
} s_dbghelp_util_instance;

}  // close unnamed namespace

HMODULE                        Dbghelp_Util::s_moduleHandle         = NULL;
SymSetOptionsProc             *Dbghelp_Util::s_symSetOptions        = NULL;
SymInitializeProc             *Dbghelp_Util::s_symInitialize        = NULL;
#ifdef BSLS_PLATFORM__CPU_32_BIT
SymFromAddrProc               *Dbghelp_Util::s_symFromAddr          = NULL;
#else
SymGetSymFromAddr64Proc       *Dbghelp_Util::s_symGetSymFromAddr64  = NULL;
#endif
SymGetLineFromAddr64Proc      *Dbghelp_Util::s_symGetLineFromAddr64 = NULL;
StackWalk64Proc               *Dbghelp_Util::s_stackWalk64          = NULL;
static SymCleanupProc         *Dbghelp_Util::s_symCleanup           = NULL;
static PFUNCTION_TABLE_ACCESS_ROUTINE64
                               Dbghelp_Util::s_symFunctionTableAccess64 = NULL;
static PGET_MODULE_BASE_ROUTINE64
                               Dbghelp_Util::s_symGetModuleBase64   = NULL;
static Dbghelp_Util::CleanupHelper
                               Dbghelp_Util::s_cleanupHelper;

// PRIVATE CLASS METHODS
int Dbghelp_Util::load()
{
    BSLS_ASSERT(NULL == s_moduleHandle);

    s_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (NULL == s_moduleHandle) {
        eprintf("baesu_Dbghelp: 'LoadLibraryA' failed\n");

        return -1;                                                    // RETURN
    }

    s_symSetOptions = (SymSetOptionsProc *)
                    GetProcAddress(s_moduleHandle, "SymSetOptions");
    s_symInitialize = (SymInitializeProc *)
                    GetProcAddress(s_moduleHandle, "SymInitialize");
#ifdef BSLS_PLATFORM__CPU_32_BIT
    s_symFromAddr = (SymFromAddrProc *)
                    GetProcAddress(s_moduleHandle, "SymFromAddr");
#else
    s_symGetSymFromAddr64 = (SymGetSymFromAddr64Proc *)
                    GetProcAddress(s_moduleHandle, "SymGetSymFromAddr64");
#endif
    s_symGetLineFromAddr64 = (SymGetLineFromAddr64Proc *)
                    GetProcAddress(s_moduleHandle, "SymGetLineFromAddr64");
    s_stackWalk64 = (StackWalk64Proc *)
                    GetProcAddress(s_moduleHandle, "StackWalk64");
    s_symCleanup = (SymCleanupProc *)
                    GetProcAddress(s_moduleHandle, "SymCleanup");
    s_symFunctionTableAccess64 = (PFUNCTION_TABLE_ACCESS_ROUTINE64)
                    GetProcAddress(s_moduleHandle, "SymFunctionTableAccess64");
    s_symGetModuleBase64 = (PGET_MODULE_BASE_ROUTINE64)
                    GetProcAddress(s_moduleHandle, "SymGetModuleBase64");
    s_hProcess = GetCurrentProcess();

    if (!ok()) {
        if (NULL == s_hProcess) {
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

    BOOL rc = (*s_symInitialize)(s_hProcess, NULL, TRUE);
    if (!rc) {
        eprintf("baesu_Dbghelp: 'SymInitialize' failed\n");
        wipeClean();
        return -1;                                                    // RETURN
    }

    return 0;
}

// CLASS METHODS
inline
int Dbghelp_Util::init()
{
    BSLS_ASSERT(baesu_Dbghelp::qLock().isLocked());

    return NULL == s_moduleHandle ? load() : 0;
}

// CREATORS
DbghelpUtil::~DbghelpUtil()
{
    BSLS_ASSERT_OPT(!baesu_Dbghelp::qLock().isLocked());

    if (ok()) {
        (*s_symCleanup)(s_hProcess);
    }
}

// MANIPULATORS
void DbghelpUtil::wipeClean()
{
    if (NULL != s_moduleHandle) {
        FreeLibrary(s_moduleHandle);
    }

    s_moduleHandle = NULL;
    s_symSetOptions = NULL;
    s_symInitialize = NULL;
#ifdef BSLS_PLATFORM__CPU_32_BIT
    s_symFromAddr = NULL;
#else
    s_symGetSymFromAddr64 = NULL;
#endif
    s_symGetLineFromAddr64 = NULL;
    s_stackWalk64 = NULL;
    s_symCleanup = NULL;
    s_symFunctionTableAccess64 = NULL;
    s_symGetModuleBase64 = NULL;
    s_hProcess = NULL;
}

// ACCESSORS
bool DbghelpUtil::ok()
{
    return  NULL != s_moduleHandle
         && NULL != s_symSetOptions
         && NULL != s_symInitialize
#ifdef BSLS_PLATFORM__CPU_32_BIT
         && NULL != s_symFromAddr
#else
         && NULL != s_symGetSymFromAddr64
#endif
         && NULL != s_symGetLineFromAddr64
         && NULL != s_stackWalk64
         && NULL != s_symCleanup
         && NULL != s_symFunctionTableAccess64
         && NULL != s_symGetModuleBase64
         && NULL != s_hProcess;
}

                              // -------------
                              // baesu_Dbghelp
                              // -------------

namespace BloombergLP {

DWORD baesu_Dbghelp::symSetOptions(DWORD arg1)
{
    BSLS_ASSERT_OPT(0 == Dbghelp_Util::init());

    return (*Dbghelp_Util::s_symSetOptions)(arg1);
}

#ifdef BSLS_PLATFORM__CPU_32_BIT

BOOL baesu_Dbghelp::symFromAddr(NullArg,
                                DWORD64      arg2,
                                PDWORD64     arg3,
                                PSYMBOL_INFO arg4)
{
    int rc = Dbghelp_Util::init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*Dbghelp_Util::s_symFromAddr)(Dbghelp_Util::s_hProcess,
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
    int rc = Dbghelp_Util::init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*Dbghelp_Util::s_symGetSymFromAddr64)(Dbghelp_Util::s_hProcess,
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
    int rc = Dbghelp_Util::init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*Dbghelp_Util::s_symGetLineFromAddr64)(Dbghelp_Util::s_hProcess,
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
    int rc = Dbghelp_Util::init();
    if (0 != rc) {
        return false;                                                 // RETURN
    }

    return (*Dbghelp_Util::s_stackWalk64)(
                                      arg1,
                                      Dbghelp_Util::s_hProcess,
                                      arg3,
                                      arg4,
                                      arg5,
                                      0,
                                      Dbghelp_Util::s_symFunctionTableAccess64,
                                      Dbghelp_Util::s_symGetModuleBase64,
                                      0);
}

bool baesu_Dbghelp::loadedOK()
{
    return s_dbghelp_util_instance.ok();
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
