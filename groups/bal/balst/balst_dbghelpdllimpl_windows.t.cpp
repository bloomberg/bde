// balst_dbghelpdllimpl_windows.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_dbghelpdllimpl_windows.h>

#include <bsl_iostream.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslmf_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <bslmt_qlock.h>

#include <windows.h>
#include <dbghelp.h>
#endif


using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                 GLOBAL TYPEDEFS, VARIABLES AND FUNCTIONS
// ============================================================================

typedef bsls::Types::UintPtr UintPtr;

enum { FIRST_LINE = __LINE__ };

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
    int                 test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    // suppress 'unused variable' warnings

    veryVeryVeryVerbose &= verbose & veryVerbose & veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

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

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
        // This test is meaningless unless on Windows with debug enabled

        bslmt::QLockGuard guard(&balst::DbghelpDllImpl_Windows::qLock());

        balst::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(line);
        DWORD offsetFromLine;

        int rc = balst::DbghelpDllImpl_Windows::symGetLineFromAddr64(
                                                               (DWORD64) &main,
                                                               &offsetFromLine,
                                                               &line);
        ASSERT(rc);

        bsl::cout << "Source file: " << line.FileName << bsl::endl;
        bsl::cout << "Line #: " << line.LineNumber << bsl::endl;
#endif
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST RESOLVING FUNCTION NAME
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST RESOLVING FUNCTION NAME\n"
                             "============================\n";

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
        // This test is meaningless unless on Windows with debug enabled

        bslmt::QLockGuard guard(&balst::DbghelpDllImpl_Windows::qLock());

        balst::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        enum { MAX_SYMBOL_BUF_NAME_LENGTH = 2000 };
#ifdef BSLS_PLATFORM_CPU_32_BIT
        enum { SIZEOF_SEGMENT = sizeof(SYMBOL_INFO) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
        SYMBOL_INFO *sym = (SYMBOL_INFO*) ta.allocate(SIZEOF_SEGMENT);
#else
        enum { SIZEOF_SEGMENT = sizeof(IMAGEHLP_SYMBOL64) +
                                  MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR) };
        IMAGEHLP_SYMBOL64 *sym = (IMAGEHLP_SYMBOL64 *)
                                                   ta.allocate(SIZEOF_SEGMENT);
#endif

        DWORD64 offsetFromSymbol = 0;
        ZeroMemory(sym, SIZEOF_SEGMENT);
        sym->SizeOfStruct = sizeof(*sym);
#ifdef BSLS_PLATFORM_CPU_32_BIT
        sym->MaxNameLen = MAX_SYMBOL_BUF_NAME_LENGTH;
        int rc = balst::DbghelpDllImpl_Windows::symFromAddr(
                                                      (DWORD64) testFunction(),
                                                      &offsetFromSymbol,
                                                      sym);
#else
        BSLMF_ASSERT(sizeof(void *) == 8);
        sym->MaxNameLength = MAX_SYMBOL_BUF_NAME_LENGTH;
        int rc = balst::DbghelpDllImpl_Windows::symGetSymFromAddr64(
                                                      (DWORD64) testFunction(),
                                                      &offsetFromSymbol,
                                                      sym);
#endif
        ASSERT(rc);
        LOOP_ASSERT(sym->Name, !bsl::strncmp("testFunction", sym->Name, 12));

        ta.deallocate(sym);
#endif
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST RESOLVING LINE NUMBER AND SOURCE FILE NAME
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST RESOLVING LINE NUMBER\n"
                             "==========================\n";

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
        // This test is meaningless unless on Windows with debug enabled

        bslmt::QLockGuard guard(&balst::DbghelpDllImpl_Windows::qLock());

        balst::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(line);
        DWORD offsetFromLine;

        int rc = balst::DbghelpDllImpl_Windows::symGetLineFromAddr64(
                                                      (DWORD64) testFunction(),
                                                      &offsetFromLine,
                                                      &line);
        ASSERT(rc);

        const char *pc = line.FileName + bsl::strlen(line.FileName);
        while (pc > line.FileName && '\\' != pc[-1]) {
            --pc;
        }
        LOOP_ASSERT(pc, !bsl::strcmp(pc,"balst_dbghelpdllimpl_windows.t.cpp"));
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

        if (verbose) cout << "TEST DOING NOTHING\n"
                             "==================\n";
      }  break;
      case 1: {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        // --------------------------------------------------------------------
        // TEST LOADING OF DLL
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST LOADING OF DLL\n"
                             "===================\n";

        bslmt::QLockGuard guard(&balst::DbghelpDllImpl_Windows::qLock());

        balst::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                                    | SYMOPT_LOAD_LINES
                                                    | SYMOPT_DEFERRED_LOADS);

        ASSERT(balst::DbghelpDllImpl_Windows::isLoaded());
#endif
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    // suppress "unused variable" warnings

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
