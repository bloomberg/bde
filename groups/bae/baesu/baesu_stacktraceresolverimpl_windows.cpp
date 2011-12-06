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

static
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

namespace BloombergLP {

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

    bcemt_QLockGuard guard(&baesu_Dbghelp::qLock());

    baesu_Dbghelp::symSetOptions(SYMOPT_NO_PROMPTS
                                 | SYMOPT_LOAD_LINES
                                 | SYMOPT_DEFERRED_LOADS);

    //                           | SYMOPT_DEBUG);

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
        rc = baesu_Dbghelp::symGetLineFromAddr64(baesu_Dbghelp::NullArg(),
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
#ifdef BSLS_PLATFORM__CPU_32_BIT
        sym->MaxNameLen = MAX_SYMBOL_BUF_NAME_LENGTH;
        rc = baesu_Dbghelp::symFromAddr(baesu_Dbghelp::NullArg(),
                                        address,
                                        &offsetFromSymbol,
                                        sym);
#else
        BSLS_ASSERT(sizeof(void *) == 8);
        sym->MaxNameLength = MAX_SYMBOL_BUF_NAME_LENGTH;
        rc = baesu_Dbghelp::symGetSymFromAddr64(baesu_Dbghelp::NullArg(),
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
