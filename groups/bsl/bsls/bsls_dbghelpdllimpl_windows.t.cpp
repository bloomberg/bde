// bsls_dbghelpdllimpl_windows.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bsls_dbghelpdllimpl_windows.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <dbghelp.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//               GLOBAL HELPER VARIABLES AND TYPES FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::UintPtr UintPtr;

enum { FIRST_LINE = __LINE__ };

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void *testFunction()
    // The function just returns a pointer into itself, cast to an unsigned int
    // but only after putting it through a transform that the optimizer can't
    // possibly understand that leaves it with its original value.
{
    const UintPtr u = ((UintPtr) &testFunction) + 4;
    const int loopGuard = 0x8edf0000;    // garbage with a lot of trailing 0's.
    const int mask      = 0xa72c3dca;    // pure garbage

    UintPtr u2 = u;
    for (int i = 0; !(i & loopGuard); ++i) {
        u2 ^= (i & mask);
    }

    // That previous loop toggled all the bits in 'u2' that it modified an even
    // number of times, so 'u2 == u', but I'm pretty sure the optimizer can't
    // figure that out.  Also, this routine is going to be longer than 4 bytes,
    // so the line number pointed to by the pointer returned will be between
    // 'FIRST_LINE' and 'SECOND_LINE'.

    ASSERT(u == u2);

    return (void *) u2;
}

enum { SECOND_LINE = __LINE__ };

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
    int     verbose = argc > 2;    (void) verbose;
    int veryVerbose = argc > 3;    (void) veryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // Usage Example
        //
        // Concern:
        //   Demonstrate usage of this class.
        //
        // Plan:
        //   Resolve line number of some code.
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
        // This test is meaningless unless on Windows with debug enabled

        bsls::BslLockGuard guard(&bsls::DbghelpDllImpl_Windows::lock());

        bsls::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(line);
        DWORD offsetFromLine;

        int rc = bsls::DbghelpDllImpl_Windows::symGetLineFromAddr64(
                                                               (DWORD64) &main,
                                                               &offsetFromLine,
                                                               &line);
        ASSERT(rc);

        printf("Source file: %s\n", line.FileName);
        printf("Line #: %d\n", line.LineNumber);
#endif
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST RESOLVING FUNCTION NAME
        // --------------------------------------------------------------------

        if (verbose) printf("TEST RESOLVING FUNCTION NAME\n"
                            "============================\n");

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
        // This test is meaningless unless on Windows with debug enabled

        bsls::BslLockGuard guard(&bsls::DbghelpDllImpl_Windows::lock());

        bsls::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        enum { MAX_SYMBOL_BUF_NAME_LENGTH = 2000 };
#ifdef BSLS_PLATFORM_CPU_32_BIT
        enum { SIZEOF_SEGMENT = sizeof(SYMBOL_INFO) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
        SYMBOL_INFO *sym = (SYMBOL_INFO*) malloc(SIZEOF_SEGMENT);
#else
        enum { SIZEOF_SEGMENT = sizeof(IMAGEHLP_SYMBOL64) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
        IMAGEHLP_SYMBOL64 *sym = (IMAGEHLP_SYMBOL64 *)
                                                   malloc(SIZEOF_SEGMENT);
#endif

        DWORD64 offsetFromSymbol = 0;
        ZeroMemory(sym, SIZEOF_SEGMENT);
        sym->SizeOfStruct = sizeof(*sym);
#ifdef BSLS_PLATFORM_CPU_32_BIT
        sym->MaxNameLen = MAX_SYMBOL_BUF_NAME_LENGTH;
        int rc = bsls::DbghelpDllImpl_Windows::symFromAddr(
                                                      (DWORD64) testFunction(),
                                                      &offsetFromSymbol,
                                                      sym);
#else
        ASSERT(sizeof(void *) == 8);
        sym->MaxNameLength = MAX_SYMBOL_BUF_NAME_LENGTH;
        int rc = bsls::DbghelpDllImpl_Windows::symGetSymFromAddr64(
                                                      (DWORD64) testFunction(),
                                                      &offsetFromSymbol,
                                                      sym);
#endif
        ASSERT(rc);
        ASSERTV(sym->Name, !strncmp("testFunction", sym->Name, 12));

        free(sym);
#endif
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST RESOLVING LINE NUMBER AND SOURCE FILE NAME
        // --------------------------------------------------------------------

        if (verbose) printf("TEST RESOLVING LINE NUMBER\n"
                            "==========================\n");

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
        // This test is meaningless unless on Windows with debug enabled

        bsls::BslLockGuard guard(&bsls::DbghelpDllImpl_Windows::lock());

        bsls::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(line);
        DWORD offsetFromLine;

        int rc = bsls::DbghelpDllImpl_Windows::symGetLineFromAddr64(
                                                      (DWORD64) testFunction(),
                                                      &offsetFromLine,
                                                      &line);
        ASSERT(rc);

        const char *pc = line.FileName + strlen(line.FileName);
        while (pc > line.FileName && '\\' != pc[-1]) {
            --pc;
        }
        ASSERTV(pc, !strcmp(pc,"bsls_dbghelpdllimpl_windows.t.cpp"));
        ASSERT(line.LineNumber > FIRST_LINE);
        ASSERT(line.LineNumber < SECOND_LINE);
#endif
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST DOING NOTHING
        //
        // Verify that the destructor of static 'dbghelp_util' in the imp file
        // doesn't malfunction and do something awful when 'dbghelp.dll' is
        // never loaded.
        // --------------------------------------------------------------------

        if (verbose) printf("TEST DOING NOTHING\n"
                            "==================\n");
      }  break;
      case 1: {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        // --------------------------------------------------------------------
        // TEST LOADING OF DLL
        // --------------------------------------------------------------------

        if (verbose) printf("TEST LOADING OF DLL\n"
                            "===================\n");

        bsls::BslLockGuard guard(&bsls::DbghelpDllImpl_Windows::lock());

        bsls::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        ASSERT(bsls::DbghelpDllImpl_Windows::isLoaded());
#endif
      }  break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

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
