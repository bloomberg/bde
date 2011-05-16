// baesu_stacktraceresolverimpl_windows.cpp                           -*-C++-*-
#include <baesu_stacktraceresolverimpl_windows.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceresolverimpl_windows_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>

#include <bdeu_string.h>

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)

#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_map.h>
#include <bsl_vector.h>

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

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

namespace BloombergLP {

namespace {

                        // ======================
                        // struct Resolver_DllApi
                        // ======================

struct Resolver_DllApi {
    // This struct contains the handle of the DLL and a collection of function
    // ptrs that will point to functions loaded at run time from it.

    // TYPES
    typedef DWORD __stdcall SymSetOptionsProc(DWORD);
    typedef BOOL  __stdcall SymInitializeProc(HANDLE, PCSTR, BOOL);
    typedef BOOL  __stdcall SymFromAddrProc(HANDLE,
                                            DWORD64,
                                            PDWORD64,
                                            PSYMBOL_INFO);
    typedef BOOL  __stdcall SymGetLineFromAddr64Proc(HANDLE,
                                                     DWORD64,
                                                     PDWORD,
                                                     PIMAGEHLP_LINE64);
    typedef BOOL  __stdcall SymCleanupProc(HANDLE);

    // DATA
    HMODULE                           d_moduleHandle;  // handle of the DLL
                                                       // that we will load the
                                                       // functions from

    SymSetOptionsProc                *d_symSetOptions; // 'SymSetOptions' func

    SymInitializeProc                *d_symInitialize; // 'SymInitialize' func

    SymFromAddrProc                  *d_symFromAddr;   // 'SymFromAddr' func

    SymGetLineFromAddr64Proc         *d_symGetLineFromAddr64;
                                                 // 'SymGetLineFromAddr64' func

    SymCleanupProc                   *d_symCleanup;    // 'SymCleanup' func

    // CREATORS
    Resolver_DllApi();
        // Open the dll and get the function pointers.  After construction,
        // 'loaded' will return 'true' if construction succeeded and 'false'
        // otherwise.

    ~Resolver_DllApi();
        // Close the dll.

    // ACCESSOR
    bool loaded();
        // Return 'true' if the c'tor successfully loaded the function pointers
        // and 'false' otherwise.
};

// MANIPULATORS
Resolver_DllApi::Resolver_DllApi()
{
    // 'LoadLibraryA' increments the reference count for the number of entities
    // referring to the dll.  Since we never unload it, the dll will never be
    // unloaded.

    d_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (!d_moduleHandle) {
        return;                                                       // RETURN
    }

    d_symSetOptions = (SymSetOptionsProc *)
                               GetProcAddress(d_moduleHandle, "SymSetOptions");
    d_symInitialize = (SymInitializeProc *)
                               GetProcAddress(d_moduleHandle, "SymInitialize");
    d_symFromAddr = (SymFromAddrProc *)
                                 GetProcAddress(d_moduleHandle, "SymFromAddr");
    d_symGetLineFromAddr64 = (SymGetLineFromAddr64Proc *)
                        GetProcAddress(d_moduleHandle, "SymGetLineFromAddr64");
    d_symCleanup = (SymCleanupProc *)
                                  GetProcAddress(d_moduleHandle, "SymCleanup");

    if   (NULL == d_symSetOptions
       || NULL == d_symInitialize
       || NULL == d_symFromAddr
       || NULL == d_symGetLineFromAddr64
       || NULL == d_symCleanup) {
        FreeLibrary(d_moduleHandle);
        d_moduleHandle = NULL;
        return;                                                       // RETURN
    }
}

Resolver_DllApi::~Resolver_DllApi()
{
    // Since we might want to call this dll again, don't unload it.

    //    if (d_moduleHandle) {
    //        FreeLibrary(d_moduleHandle);
    //    }
}

bool Resolver_DllApi::loaded()
{
    return NULL != d_moduleHandle;
}

}  // close unnamed namespace

       // =============================================================
       // baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Windows>
       // =============================================================

int baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Windows>::resolve(
                        bsl::vector<baesu_StackTraceFrame> *frames,
                        bool                                demangle,
                        bslma_Allocator                    *basicAllocator)
    // Given the specified vector 'frames' of frames from the stack trace
    // object which have the 'address()' fields valid and all other fields
    // invalid, resolve the rest of the fields in the frames.  Each 'address'
    // field must be initialized to the return instruction pointer from a
    // subroutine that was on the stack when
    // 'baesu_StackTrace::getStackPointers' was run.  Return 0 on success and a
    // non-zero value otherwise.  Note that on Windows, demangling is not an
    // option -- it always happens.
{
    typedef bsl::map<HMODULE, const char *> LibNameMap;

    Resolver_DllApi api;
    if (!api.loaded()) {
        return -291;                                                  // RETURN
    }

    // TBD: remove SYMOPT_DEBUG

    (*api.d_symSetOptions)(SYMOPT_NO_PROMPTS | SYMOPT_LOAD_LINES
                                       | SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);
    HANDLE hProcess = GetCurrentProcess();

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, 'api' will only load what is actually required

    BOOL rc = (*api.d_symInitialize)(hProcess, NULL, TRUE);
    if (!rc) {
        eprintf("Could not init symbols for %p (%08x)\n",
                                                     hProcess, GetLastError());
        return -301;                                                  // RETURN
    }

    int numFrames = frames->size();
    LibNameMap libNameMap(basicAllocator);
    char *libNameBuf = (char *) basicAllocator->allocate(MAX_PATH);

    enum {
        MAX_SYMBOL_BUF_NAME_LENGTH = 2000,
        SIZEOF_SEGMENT = sizeof(SYMBOL_INFO) +
                                    MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR),
        SIZEOF_STRUCT = SIZEOF_SEGMENT - sizeof(TCHAR)
    };
    SYMBOL_INFO *sym = (SYMBOL_INFO*) basicAllocator->allocate(SIZEOF_SEGMENT);

    for(int i = 0; i < numFrames; ++i) {
        baesu_StackTraceFrame *frame = &(*frames)[i];
        DWORD64 address = (DWORD64) frame->address();

        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));

        line.SizeOfStruct = sizeof(line);
        DWORD offsetFromLine;
        rc = (*api.d_symGetLineFromAddr64)(hProcess,
                                           address,
                                           &offsetFromLine,
                                           &line);
        if (rc) {
            frame->setSourceFileName(bdeu_String::copy(line.FileName,
                                                       basicAllocator));
            frame->setLineNumber(line.LineNumber);
        }
        ZeroMemory(sym, SIZEOF_SEGMENT);
        sym->SizeOfStruct = SIZEOF_STRUCT;
        sym->MaxNameLen = MAX_SYMBOL_BUF_NAME_LENGTH;
        DWORD64 offsetFromSymbol;
        rc = (*api.d_symFromAddr)(hProcess,
                                  address,
                                  &offsetFromSymbol,
                                  sym);
        if (rc) {
            // windows is always demangled

            ((TCHAR *) sym)[SIZEOF_SEGMENT - 1] = 0;
            frame->setMangledSymbolName(
                                bdeu_String::copy(sym->Name, basicAllocator));
            frame->setSymbolName(frame->mangledSymbolName());
            frame->setOffsetFromSymbol((bsl::size_t) offsetFromSymbol);
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
            rc = GetModuleFileNameA(hModule,
                                    libNameBuf,
                                    MAX_PATH);
            libNameBuf[MAX_PATH-1] = 0;
            if (!rc) {
                // Failed.  Put a null lib name into the map so we won't waste
                // time looking up the same library again.  Leave the
                // libraryFileName in the frame null.

                libNameMap[hModule] = bdeu_String::copy("", basicAllocator);
            }
            else {
                frame->setLibraryFileName(bdeu_String::copy(libNameBuf,
                                                            basicAllocator));
                libNameMap[hModule] = frame->libraryFileName();
            }
        }
    }

    (*api.d_symCleanup)(hProcess);

    return 0;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
