// baesu_stacktraceresolverimpl_windows.cpp                           -*-C++-*-
#include <baesu_stacktraceresolverimpl_windows.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceresolverimpl_windows_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>

#include <bdeu_string.h>

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)

#include <bdema_heapbypassallocator.h>

#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
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
#ifdef BSLS_PLATFORM__CPU_32_BIT
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
    typedef BOOL  __stdcall SymCleanupProc(HANDLE);

    // DATA
    HMODULE                           d_moduleHandle;  // handle of the DLL
                                                       // that we will load the
                                                       // functions from

    SymSetOptionsProc                *d_symSetOptions; // 'SymSetOptions' func

    SymInitializeProc                *d_symInitialize; // 'SymInitialize' func

#ifdef BSLS_PLATFORM__CPU_32_BIT
	SymFromAddrProc                  *d_symFromAddr;   // 'SymFromAddr' func
#else
    SymGetSymFromAddr64Proc          *d_symGetSymFromAddr64;
	                                                   // 'SymGetSymFromAddr64'
                                                       // func
#endif
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
#ifdef BSLS_PLATFORM__CPU_32_BIT
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

    if   (NULL == d_symSetOptions
       || NULL == d_symInitialize
#ifdef BSLS_PLATFORM__CPU_32_BIT
	   || NULL == d_symFromAddr
#else
	   || NULL == d_symGetSymFromAddr64
#endif
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

static void reportError(const char *string)
{
    DWORD lastError = GetLastError();

    static int reportTimes = 0;
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;

        const char *nrString =
              bsl::getenv("BAESU_STACKTRACERESOLVERIMPL_WINDOWS_REPORT_TIMES");
        if (nrString) {
            reportTimes = bsl::atoi(nrString);
        }
    }

    if (reportTimes > 0) {
        --reportTimes;

        bsl::cerr << string << lastError << bsl::endl;
    }
}

       // =============================================================
       // baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Windows>
       // =============================================================

int baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Windows>::resolve(
                                                  baesu_StackTrace *stackTrace,
                                                  bool              demangle)
    // Given a specified stack trace object 'stackTrace' of stack trace frames
    // with only their 'address' fields valid, set as many other fields of the
    // frames as possible.  The 'demangle' argument is ignored, demangling
    // always happens on Windows.  Return 0 if successful and a non-zero value
    // otherwise.
{
    typedef bsl::map<HMODULE, const char *> LibNameMap;

    bdema_HeapBypassAllocator hbpAlloc;

    Resolver_DllApi api;
    if (!api.loaded()) {
        return -291;                                                  // RETURN
    }

    (*api.d_symSetOptions)(SYMOPT_NO_PROMPTS | SYMOPT_LOAD_LINES
                                       | SYMOPT_DEFERRED_LOADS);

    //                                 | SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);

    HANDLE hProcess = GetCurrentProcess();

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, 'api' will only load what is actually required

    BOOL rc = (*api.d_symInitialize)(hProcess, NULL, TRUE);
    if (!rc) {
        eprintf("Could not init symbols for %p (%08x)\n",
                                                     hProcess, GetLastError());
        return -301;                                                  // RETURN
    }

    int numFrames = stackTrace->length();
    LibNameMap libNameMap(&hbpAlloc);
    char *libNameBuf = (char *) hbpAlloc.allocate(MAX_PATH);

	enum { MAX_SYMBOL_BUF_NAME_LENGTH = 2000 };
#ifdef BSLS_PLATFORM__CPU_32_BIT
	enum { SIZEOF_SEGMENT = sizeof(SYMBOL_INFO) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
    SYMBOL_INFO *sym = (SYMBOL_INFO*) hbpAlloc.allocate(SIZEOF_SEGMENT);
#else
	enum { SIZEOF_SEGMENT = sizeof(IMAGEHLP_SYMBOL64) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
    IMAGEHLP_SYMBOL64 *sym = (IMAGEHLP_SYMBOL64 *)
                                             hbpAlloc.allocate(SIZEOF_SEGMENT);
#endif

    for(int i = 0; i < numFrames; ++i) {
        baesu_StackTraceFrame *frame = &(*stackTrace)[i];
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
            frame->setSourceFileName(line.FileName);
            frame->setLineNumber(line.LineNumber);
        }
        else {
            reportError("stack trace resovler error: symGetLineFromAddr64"
                        " error code: ");
        }
        DWORD64 offsetFromSymbol = 0;
		ZeroMemory(sym, SIZEOF_SEGMENT);
        sym->SizeOfStruct = sizeof(*sym);
		sym->MaxNameLength = MAX_SYMBOL_BUF_NAME_LENGTH;
#ifdef BSLS_PLATFORM__CPU_32_BIT
        rc = (*api.d_symFromAddr)(hProcess,
                                  address,
                                  &offsetFromSymbol,
                                  sym);
#else
		BSLS_ASSERT(sizeof(void *) == 8);
		rc = (*api.d_symGetSymFromAddr64)(hProcess,
			                              address,
					    	    		  &offsetFromSymbol,
						    			  sym);
#endif
		if (rc) {
            // windows is always demangled

            ((TCHAR *) sym)[SIZEOF_SEGMENT - 1] = 0;
            frame->setMangledSymbolName(sym->Name);
            frame->setSymbolName(frame->mangledSymbolName());
            frame->setOffsetFromSymbol((bsl::size_t) offsetFromSymbol);
        }
        else {
#ifdef BSLS_PLATFORM__CPU_32_BIT
            reportError("stack trace resovler error: SymFromAddr"
                        " error code: ");
#else
			reportError("stack trace resovler error: SymGetSymFromAddr64"
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
            rc = GetModuleFileNameA(hModule,
                                    libNameBuf,
                                    MAX_PATH);
            libNameBuf[MAX_PATH-1] = 0;
            if (!rc) {
                // Failed.  Put a null lib name into the map so we won't waste
                // time looking up the same library again.  Leave the
                // libraryFileName in the frame null.

                libNameMap[hModule] = bdeu_String::copy("", &hbpAlloc);
            }
            else {
                frame->setLibraryFileName(libNameBuf);
                libNameMap[hModule] = frame->libraryFileName().c_str();
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
