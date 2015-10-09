// balst_dbghelpdllimpl_windows.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_dbghelpdllimpl_windows.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_dbghelpdllimpl_windows_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)

#include <bslmt_qlock.h>

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

#pragma optimize("", off)

// Optional debug traces, turned off in production, are done with 'debugPrintf
#undef  TRACES
#define TRACES 0    // debugging traces off

#if TRACES == 1
# include <stdio.h>

#define debugPrintf printf

#else

#define debugPrintf (void)
#endif

namespace {

                         // =========================
                         // local struct Dbghelp_Util
                         // =========================

struct Dbghelp_Util {
    // This struct contains the handle of the DLL and a collection of function
    // pointers that will point to functions loaded at run time from it.  Only
    // one instance of this type is to exist, a static instance whose
    // destructor will call 'SymCleanup' if the object has been initialized.

    // TYPES
    typedef DWORD __stdcall SymSetOptionsProc(DWORD);
    typedef BOOL  __stdcall SymInitializeProc(HANDLE, PCSTR, BOOL);
#ifdef BSLS_PLATFORM_CPU_32_BIT
    typedef BOOL  __stdcall SymFromAddrProc(HANDLE,
                                            DWORD64,
                                            PDWORD64,
                                            PSYMBOL_INFO);
#else
    // 'symFromAddr' doesn't work on 64 bit, so we conditionally compile to use
    // 'SymGetSymFromAddr64'.

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

#ifdef BSLS_PLATFORM_CPU_32_BIT
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
        // Load the dll and set all the function pointers in this class to
        // point into it.  Return 0 on success and a non-zero value otherwise.

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
        // otherwise.  Note that the initialization is broken up into 'init'
        // and 'load' so that 'init' is a small, fast inline function that, if
        // necessary, calls the large, out-of-line 'load' function.

    // ACCESSORS
    bool isLoaded() const;
        // Everything is fully loaded.
};

// PRIVATE MANIPULATORS
int Dbghelp_Util::load()
{
    BSLS_ASSERT(0 == d_moduleHandle);
    BSLS_ASSERT(0 == d_symSetOptions);
    BSLS_ASSERT(0 == d_symInitialize);
#ifdef BSLS_PLATFORM_CPU_32_BIT
    BSLS_ASSERT(0 == d_symFromAddr);
#else
    BSLS_ASSERT(0 == d_symGetSymFromAddr64);
#endif
    BSLS_ASSERT(0 == d_symGetLineFromAddr64);
    BSLS_ASSERT(0 == d_stackWalk64);
    BSLS_ASSERT(0 == d_symCleanup);
    BSLS_ASSERT(0 == d_symFunctionTableAccess64);
    BSLS_ASSERT(0 == d_symGetModuleBase64);
    BSLS_ASSERT(0 == d_hProcess);

    d_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (0 == d_moduleHandle) {
        debugPrintf("balst::DbghelpDllImpl_Windows: 'LoadLibraryA' failed\n");

        return -1;                                                    // RETURN
    }

    d_symSetOptions = (SymSetOptionsProc *)
                    GetProcAddress(d_moduleHandle, "SymSetOptions");
    d_symInitialize = (SymInitializeProc *)
                    GetProcAddress(d_moduleHandle, "SymInitialize");
#ifdef BSLS_PLATFORM_CPU_32_BIT
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
        debugPrintf("balst::DbghelpDllImpl_Windows: %s failed\n",
                                        0 == d_hProcess ? "'GetCurrentProcess'"
                                                        : "'GetProcAddress'");

        wipeClean();
        return -1;                                                    // RETURN
    }

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, this method will only load what is actually required

    BOOL rc = (*d_symInitialize)(d_hProcess, 0, TRUE);
    if (!rc) {
        debugPrintf("balst::DbghelpDllImpl_Windows: 'SymInitialize' failed\n");
        wipeClean();
        return -1;                                                    // RETURN
    }

    return 0;
}

// CREATORS
Dbghelp_Util::~Dbghelp_Util()
{
    BSLS_ASSERT_OPT(
              !BloombergLP::balst::DbghelpDllImpl_Windows::qLock().isLocked());

    if (isLoaded()) {
        (*d_symCleanup)(d_hProcess);
        FreeLibrary(d_moduleHandle);
    }
}

// MANIPULATORS
inline
int Dbghelp_Util::init()
{
    return 0 == d_moduleHandle ? load() : 0;
}


void Dbghelp_Util::wipeClean()
{
    if (0 != d_moduleHandle) {
        FreeLibrary(d_moduleHandle);
    }

    d_moduleHandle             = 0;
    d_symSetOptions            = 0;
    d_symInitialize            = 0;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    d_symFromAddr              = 0;
#else
    d_symGetSymFromAddr64      = 0;
#endif
    d_symGetLineFromAddr64     = 0;
    d_stackWalk64              = 0;
    d_symCleanup               = 0;
    d_symFunctionTableAccess64 = 0;
    d_symGetModuleBase64       = 0;
    d_hProcess                 = 0;
}

// ACCESSORS
bool Dbghelp_Util::isLoaded() const
{
    return  0 != d_moduleHandle
         && 0 != d_symSetOptions
         && 0 != d_symInitialize
#ifdef BSLS_PLATFORM_CPU_32_BIT
         && 0 != d_symFromAddr
#else
         && 0 != d_symGetSymFromAddr64
#endif
         && 0 != d_symGetLineFromAddr64
         && 0 != d_stackWalk64
         && 0 != d_symCleanup
         && 0 != d_symFunctionTableAccess64
         && 0 != d_symGetModuleBase64
         && 0 != d_hProcess;
}

}  // close unnamed namespace

// DATA
static Dbghelp_Util dbghelp_util = {};    // all zeroes

namespace BloombergLP {

namespace balst {

                           // ----------------------
                           // DbghelpDllImpl_Windows
                           // ----------------------

// DATA
bslmt::QLock balst::DbghelpDllImpl_Windows::s_qLock = BSLMT_QLOCK_INITIALIZER;

// CLASS METHODS
bool DbghelpDllImpl_Windows::isLoaded()
{
    return dbghelp_util.isLoaded();
}

DWORD DbghelpDllImpl_Windows::symSetOptions(DWORD symOptions)
{
    int rc = dbghelp_util.init();
    BSLS_ASSERT_OPT(0 == rc);

    return (*dbghelp_util.d_symSetOptions)(symOptions);
}

#ifdef BSLS_PLATFORM_CPU_32_BIT

BOOL DbghelpDllImpl_Windows::symFromAddr(DWORD64      address,
                                               PDWORD64     displacement,
                                               PSYMBOL_INFO symbol)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return FALSE;                                                 // RETURN
    }

    return (*dbghelp_util.d_symFromAddr)(dbghelp_util.d_hProcess,
                                         address,
                                         displacement,
                                         symbol);
}

#else

BOOL DbghelpDllImpl_Windows::symGetSymFromAddr64(
                                               DWORD64            address,
                                               PDWORD64           displacement,
                                               PIMAGEHLP_SYMBOL64 symbol)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return FALSE;                                                 // RETURN
    }

    return (*dbghelp_util.d_symGetSymFromAddr64)(dbghelp_util.d_hProcess,
                                                 address,
                                                 displacement,
                                                 symbol);
}

#endif

BOOL DbghelpDllImpl_Windows::symGetLineFromAddr64(
                                              DWORD64          dwAddr,
                                              PDWORD           pdwDisplacement,
                                              PIMAGEHLP_LINE64 line)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return FALSE;                                                 // RETURN
    }

    return (*dbghelp_util.d_symGetLineFromAddr64)(dbghelp_util.d_hProcess,
                                                  dwAddr,
                                                  pdwDisplacement,
                                                  line);
}

BOOL DbghelpDllImpl_Windows::stackWalk64(DWORD          machineType,
                                               HANDLE         hThread,
                                               LPSTACKFRAME64 stackFrame,
                                               PVOID          contextRecord)
{
    int rc = dbghelp_util.init();
    if (0 != rc) {
        return FALSE;                                                 // RETURN
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
}  // close package namespace

}  // close namespace BloombergLP

// WINDOWS
#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
