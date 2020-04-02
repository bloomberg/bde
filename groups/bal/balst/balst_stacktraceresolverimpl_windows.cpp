// balst_stacktraceresolverimpl_windows.cpp                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_windows.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolverimpl_windows_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS)

#include <bdlb_string.h>
#include <bdlma_heapbypassallocator.h>

#include <bslmf_assert.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_vector.h>

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

#pragma optimize("", off)

#undef  U_TRACES
#define U_TRACES 0  // 0 == debugging traces off, U_ZPRINTF does nothing
                    // 1 == debugging traces on,  U_ZPRINTF is printf

#if 0 == U_TRACES
# define U_ZPRINTF (void)
#else
# define U_ZPRINTF bsl::printf
#endif

namespace {
namespace u {

using namespace BloombergLP;

                         // ==========================
                         // local struct DbgHelpRecord
                         // ==========================

struct DbgHelpRecord {
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

    SymCleanupProc                  *d_symCleanup;    // 'SymCleanup' func

    HANDLE                           d_hProcess;

    bsls::AtomicOperations::AtomicTypes::Int            // Boolean to indicate
                                     d_isLoadCompleted; // that the library is
                                                        // successfully loaded.

  private:
    // PRIVATE MANIPULATORS
    int load();
        // Load the dll and set all the function pointers in this class to
        // point into it.  Return 0 on success and a non-zero value otherwise.

    void wipeClean();
        // Null out all the pointers owned by this class.

  public:
    // CLASS METHOD
    static bslmt::Mutex& mutexSingleton();
        // Return the mutex to guard all access to this record.

    // CREATORS
    ~DbgHelpRecord();
        // Destroy this object.  Exactly one object of this type will be
        // created, and that object is static, so that resources can be cleaned
        // up upon process termination.

    // MANIPULATORS
    bool isLoaded();
        // Everything is fully loaded.  Manipulator to make sure it's inlined
        // when called by 'init'.

    int init();
        // Ensure the dll is loaded.  Return 0 on success and a non-zero value
        // otherwise.  Note that the initialization is broken up into 'init'
        // and 'load' so that 'init' is a small, fast inline function that, if
        // necessary, calls the large, out-of-line 'load' function.

    // ACCESSORS
    HANDLE hProcess() const;
        // The handle to the process.

                    // *** Methods corresponding to Windows 'dbghelp.dll'
                    // functions ***

    BOOL symSetOptions(DWORD symOptions) const;
        // Invoke the 'SymSetOptions' function of 'dbghelp.dll' with the
        // specified 'symOptions' options.  Return 'TRUE' on success, and
        // 'FALSE' on failure.  The behavior is undefined if the mutex provided
        // by the 'lock' method is not held.
        //
        // Note that 'symOptions' (a bitwise-OR of 'SYMOPT_*' values) specifies
        // flags affecting subsequent calls to the '.dll'.  Also note that the
        // current options are returned.  Also note that the signature of this
        // function does not match that of 'SymSetOptions'.  Finally, note that
        // further details are available at 'http://msdn.com'.

#ifdef BSLS_PLATFORM_CPU_32_BIT
    // 'symFromAddr' doesn't work on 64-bit, so we conditionally compile and
    // use 'SymFromAddr' on 32-bit and 'SymGetSymFromAddr64' on 64-bit.

    BOOL symFromAddr(HANDLE       hProcess,
                     DWORD64      address,
                     PDWORD64     displacement,
                     PSYMBOL_INFO symbol) const;
        // Invoke the 'SymFromAddr' function of 'dbghelp.dll' with the
        // specified 'hProcess', 'address', 'displacement', and 'symbol', and
        // return the result.  The behavior is undefined unless the mutex
        // provided by the 'lock' method is held and 'symbol->MaxNameLen' is
        // set to the maximum length (a value of '2000' is recommended).
        //
        // Note that 'symbol' is loaded with a pointer to the symbol
        // information for the symbol at 'address', and 'displacement' is
        // loaded with the difference between 'address' and the address of the
        // symbol described at 'symbol'.  Also note that 'true' is returned if
        // a symbol is found for 'address', and 'false' is returned otherwise.
        // Finally, note that further details of 'SymFromAddr' are available at
        // 'http://msdn.com'.

#else
    BOOL symGetSymFromAddr64(HANDLE             hProcess,
                             DWORD64            address,
                             PDWORD64           displacement,
                             PIMAGEHLP_SYMBOL64 symbol) const;
        // Invoke the 'SymFromAddr64' function of 'dbghelp.dll' with the
        // specified 'hProcess, 'address', 'displacement', and 'symbol', and
        // return the result.  The behavior is undefined unless the mutex
        // provided by the 'lock' method is held and 'symbol->MaxNameLen' is
        // set to the maximum length (a value of '2000' is recommended).
        //
        // Note that 'symbol' is loaded with a pointer to the symbol
        // information for the symbol at 'address', and 'displacement' is
        // loaded with the difference between 'address' and the address of the
        // symbol described at 'symbol'.  Also note that 'true' is returned if
        // a symbol is found for 'address', and 'false' is returned otherwise.
        // Finally, note that further details of 'SymFromAddr64' are available
        // at 'http://msdn.com'.

#endif

    BOOL symGetLineFromAddr64(HANDLE           hProcess,
                              DWORD64          dwAddr,
                              PDWORD           pdwDisplacement,
                              PIMAGEHLP_LINE64 line) const;
        // Invoke the 'SymGetLineFromAddr64' function of 'dbghelp.dll' with the
        // specified 'dwAddr', 'pdwDisplacement', and 'line', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined if the mutex provided
        // by the 'lock' method is not held.
        //
        // Note that 'line' is loaded with a pointer to line information (e.g.,
        // source file name and line number) for the code at 'dwAddr' and
        // 'pdwDisplacement' is loaded with the displacement of that code from
        // the beginning of the line.  Also note that 'true' is returned if
        // information is found for 'dwAddr', and 'false' is returned
        // otherwise.  Finally, note that further details of
        // 'SymGetLineFromAddr64' are available at 'http://msdn.com'.
};

                            // --------------------
                            // struct DbgHelpRecord
                            // --------------------

// DATA
static DbgHelpRecord dbgHelpRecord = {};    // all zeroes

// PRIVATE MANIPULATORS
inline
bool DbgHelpRecord::isLoaded()
{
    return bsls::AtomicOperations::getInt(&d_isLoadCompleted);
}

int DbgHelpRecord::load()
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
    BSLS_ASSERT(0 == d_symCleanup);
    BSLS_ASSERT(0 == d_hProcess);

    d_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (0 == d_moduleHandle) {
        U_ZPRINTF("balst::DbghelpDllImpl_Windows: 'LoadLibraryA' failed\n");
        wipeClean();
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
    d_symCleanup = (SymCleanupProc *)
                    GetProcAddress(d_moduleHandle, "SymCleanup");
    d_hProcess = GetCurrentProcess();

    if  (0 == d_moduleHandle
      || 0 == d_symSetOptions
      || 0 == d_symInitialize
#ifdef BSLS_PLATFORM_CPU_32_BIT
      || 0 == d_symFromAddr
#else
      || 0 == d_symGetSymFromAddr64
#endif
      || 0 == d_symGetLineFromAddr64
      || 0 == d_symCleanup
      || 0 == d_hProcess) {
        U_ZPRINTF("balst::DbghelpDllImpl_Windows: %s failed\n",
                                        0 == d_hProcess ? "'GetCurrentProcess'"
                                                        : "'GetProcAddress'");
        wipeClean();
        return -1;                                                    // RETURN
    }

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, this method will only load what is actually required

    BOOL rc = (*d_symInitialize)(d_hProcess, 0, TRUE);
    if (!rc) {
        U_ZPRINTF("balst::DbghelpDllImpl_Windows: 'SymInitialize' failed\n");
        wipeClean();
        return -1;                                                    // RETURN
    }

    bsls::AtomicOperations::setInt(&d_isLoadCompleted, 1);

    return 0;
}

void DbgHelpRecord::wipeClean()
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
    d_symCleanup               = 0;
    d_hProcess                 = 0;

    bsls::AtomicOperations::setInt(&d_isLoadCompleted, 0);
}

// CLASS METHOD
bslmt::Mutex& DbgHelpRecord::mutexSingleton()
{
    static bslmt::Mutex *ret = 0;

    BSLMT_ONCE_DO {
        static bslmt::Mutex mutex;

        ret = &mutex;
    }

    return *ret;
}

// CREATORS
DbgHelpRecord::~DbgHelpRecord()
{
    if (isLoaded()) {
        (*d_symCleanup)(d_hProcess);
        FreeLibrary(d_moduleHandle);
    }
}

// MANIPULATORS
inline
int DbgHelpRecord::init()
{
    int rc;

    // Sometimes, under multithreaded conditions, 'load' spuriously fails and
    // returns non-zero.  In that case, try again a few more times.

    for (int ii = 0; (rc = !isLoaded()) && ii < 20; ++ii) {
        load();
    }

    return rc;
}

// ACCESSORS
inline
HANDLE DbgHelpRecord::hProcess() const
{
    return d_hProcess;
}

inline
BOOL DbgHelpRecord::symSetOptions(DWORD symOptions) const
{
    const DWORD result = (*d_symSetOptions)(symOptions);

    return result == symOptions;
}

#ifdef BSLS_PLATFORM_CPU_32_BIT

inline
BOOL DbgHelpRecord::symFromAddr(HANDLE       hProcess,
                                DWORD64      address,
                                PDWORD64      displacement,
                                PSYMBOL_INFO symbol) const
{
    return (*d_symFromAddr)(hProcess,
                            address,
                            displacement,
                            symbol);
}

#else

inline
BOOL DbgHelpRecord::symGetSymFromAddr64(HANDLE             hProcess,
                                        DWORD64            address,
                                        PDWORD64           displacement,
                                        PIMAGEHLP_SYMBOL64 symbol) const
{
    return (*d_symGetSymFromAddr64)(hProcess,
                                    address,
                                    displacement,
                                    symbol);
}

#endif

inline
BOOL DbgHelpRecord::symGetLineFromAddr64(HANDLE           hProcess,
                                         DWORD64          dwAddr,
                                         PDWORD           pdwDisplacement,
                                         PIMAGEHLP_LINE64 line) const
{
    return (*d_symGetLineFromAddr64)(hProcess,
                                     dwAddr,
                                     pdwDisplacement,
                                     line);
}

void reportError(const char *string)
    // If an environment variable is set, report the result of 'GetLastError'
    // to 'cerr', but only do it a limited number of times (test case 2 causes
    // a huge number of these errors).
{
    DWORD lastError = GetLastError();

    static int reportTimes = 0;
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;

        const char *nrString =
              bsl::getenv("BALST_STACKTRACERESOLVERIMPL_WINDOWS_REPORT_TIMES");
        if (nrString) {
            reportTimes = bsl::atoi(nrString);
        }
    }

    if (reportTimes > 0) {
        --reportTimes;

        fprintf(stderr, "%s: %d", string, static_cast<int>(lastError));
    }
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace balst {

           // =================================================
           // StackTraceResolverImpl<ObjectFileFormat::Windows>
           // =================================================

int StackTraceResolverImpl<ObjectFileFormat::Windows>::resolve(
                                                        StackTrace *stackTrace,
                                                        bool        demangle)
    // Given a specified stack trace object 'stackTrace' of stack trace frames
    // with only their 'address' fields valid, set as many other fields of the
    // frames as possible.  The 'demangle' argument is ignored, demangling
    // always happens on Windows.  Return 0 if successful and a non-zero value
    // otherwise.
{
    typedef bsl::map<HMODULE, const char *> LibNameMap;

    bslmt::LockGuard<bslmt::Mutex> guard(&u::DbgHelpRecord::mutexSingleton());

    int rc = u::dbgHelpRecord.init();
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    HANDLE hProcess = u::dbgHelpRecord.hProcess();

    // Note that some of these functions return 'bool' and some 'DWORD', but
    // whenever the variable 'success' is used, 0 means failure.

    bool success = u::dbgHelpRecord.symSetOptions(SYMOPT_NO_PROMPTS
                                                | SYMOPT_LOAD_LINES
                                                | SYMOPT_DEFERRED_LOADS);
                                             // | SYMOPT_DEBUG);
    if (!success) {
        return -1;                                                    // RETURN
    }

    bdlma::HeapBypassAllocator hbpAlloc;

    int numFrames = stackTrace->length();
    LibNameMap libNameMap(&hbpAlloc);
    char *libNameBuf = (char *) hbpAlloc.allocate(MAX_PATH);

    enum { MAX_SYMBOL_BUF_NAME_LENGTH = 2000 };
#ifdef BSLS_PLATFORM_CPU_32_BIT
    enum { SIZEOF_SEGMENT = sizeof(SYMBOL_INFO) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
    SYMBOL_INFO *sym = (SYMBOL_INFO*) hbpAlloc.allocate(SIZEOF_SEGMENT);
#else
    enum { SIZEOF_SEGMENT = sizeof(IMAGEHLP_SYMBOL64) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
    IMAGEHLP_SYMBOL64 *sym = (IMAGEHLP_SYMBOL64 *)
                                             hbpAlloc.allocate(SIZEOF_SEGMENT);
#endif

    for (int i = 0; i < numFrames; ++i) {
        StackTraceFrame *frame = &(*stackTrace)[i];
        DWORD64 address = (DWORD64) frame->address();

        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));

        line.SizeOfStruct = sizeof(line);
        DWORD offsetFromLine;
        success = u::dbgHelpRecord.symGetLineFromAddr64(hProcess,
                                                        address,
                                                        &offsetFromLine,
                                                        &line);
        if (success) {
            frame->setSourceFileName(line.FileName);
            frame->setLineNumber(line.LineNumber);
        }
        else {
            u::reportError("stack trace resolver error: symGetLineFromAddr64"
                           " error code: ");
        }
        DWORD64 offsetFromSymbol = 0;
        ZeroMemory(sym, SIZEOF_SEGMENT);
        sym->SizeOfStruct = sizeof(*sym);
#ifdef BSLS_PLATFORM_CPU_32_BIT
        sym->MaxNameLen = MAX_SYMBOL_BUF_NAME_LENGTH;
        success = u::dbgHelpRecord.symFromAddr(hProcess,
                                               address,
                                               &offsetFromSymbol,
                                               sym);
#else
        BSLMF_ASSERT(sizeof(void *) == 8);
        sym->MaxNameLength = MAX_SYMBOL_BUF_NAME_LENGTH;
        success = u::dbgHelpRecord.symGetSymFromAddr64(hProcess,
                                                       address,
                                                       &offsetFromSymbol,
                                                       sym);
#endif
        if (success) {
            // windows is always demangled

            ((TCHAR *) sym)[SIZEOF_SEGMENT - 1] = 0;
            frame->setMangledSymbolName(sym->Name);
            frame->setSymbolName(frame->mangledSymbolName());
            frame->setOffsetFromSymbol((bsl::size_t) offsetFromSymbol);
        }
        else {
#ifdef BSLS_PLATFORM_CPU_32_BIT
            u::reportError("stack trace resolver error: SymFromAddr"
                           " error code: ");
#else
            u::reportError("stack trace resolver error: SymGetSymFromAddr64"
                           " error code: ");
#endif
        }

        HMODULE hModule = NULL;
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((LPCVOID) address, &mbi, sizeof(mbi))) {
            hModule = (HMODULE)(mbi.AllocationBase);
        }
        LibNameMap::iterator it = libNameMap.find(hModule);
        if (libNameMap.end() != it) {
            // If the library name in the map is "", leave the library file
            // name in the frame null.

            if (*it->second) {
                frame->setLibraryFileName(it->second);
            }
        }
        else {
            DWORD success2 = GetModuleFileNameA(hModule,
                                                libNameBuf,
                                                MAX_PATH);
            libNameBuf[MAX_PATH-1] = 0;
            if (!success2) {
                // Failed.  Put a null lib name into the map so we won't waste
                // time looking up the same library again.  Leave the
                // libraryFileName in the frame null.

                libNameMap[hModule] = bdlb::String::copy("", &hbpAlloc);
            }
            else {
                frame->setLibraryFileName(libNameBuf);
                libNameMap[hModule] = frame->libraryFileName().c_str();
            }
        }
    }

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

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
