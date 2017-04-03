// balst_stacktraceresolverimpl_windows.t.cpp                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_windows.h>

#include <balst_objectfileformat.h>

#include <bdlb_random.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
// TEST PLAN
//-----------------------------------------------------------------------------
// [ 1] resolve
// [ 2] garbage test
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

// ============================================================================
//                    GLOBAL HELPER #DEFINES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                 GLOBAL HELPER TYPES & CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef balst::StackTraceResolverImpl<balst::ObjectFileFormat::Windows> Obj;
typedef balst::StackTraceFrame                                         Frame;
typedef bsls::Types::UintPtr                                          UintPtr;
typedef bsls::Types::IntPtr                                           IntPtr;

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

static const bsl::size_t npos = bsl::string::npos;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
int funcStaticOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    for (int j = 0; j < 2; ++j) {
        i = i * i * i * i;
    }

    return 5 * i;
}

int funcGlobalOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    for (int j = 0; j < 3; ++j) {
        i = i * i * i;
    }

    return 7 * i;
}

template <typename TYPE>
static TYPE abs(TYPE num)
{
    return num >= 0 ? num : -num;
}

inline
UintPtr foilOptimizer(const UintPtr u)
    // The function just returns 'u', but only after putting it through a
    // transform that the optimizer can't possibly understand that leaves it
    // with its original value.
{
    const int loopGuard = 0x8edf0000;    // garbage with a lot of trailing 0's.
    const int mask      = 0xa72c3dca;    // pure garbage

    UintPtr u2 = u;
    for (int i = 0; !(i & loopGuard); ++i) {
        u2 ^= (i & mask);
    }

    // That previous loop toggled all the bits in 'u2' that it touched an even
    // number of times, so 'u2 == u', but I'm pretty sure the optimizer can't
    // figure that out.

    ASSERT(u == u2);

    return u2;
}

static
const void *addFixedOffset(bsls::Types::UintPtr funcAddress)
    // Given a function pointer stored in a 'UintPtr', add an offset to the
    // pointer and return it as a 'const void *'.
{
    const char *ptr = (const char *) funcAddress;

    return ptr + 4;
}

static
const char *ng(const char *str)
    // null guard -- substitute "(null)" for the specified 'str' if 'str' is
    // null.
{
    return str ? str : "(null)";
}

static bsls::Types::Uint64 bigRandSeed = 0;
static const bsls::Types::Uint64 randA = 6364136223846793005ULL;
static const bsls::Types::Uint64 randC = 1442695040888963407ULL;

static
UintPtr bigRand()
{
    typedef bsls::Types::Uint64 Uint64;

    Uint64 next = randA * bigRandSeed + randC;
    Uint64 lowBits = next >> 32;
    bigRandSeed = randA * next + randC;

    return (UintPtr) (bigRandSeed ^ lowBits);
}

void stuffRandomAddresses(balst::StackTrace *v)
{
    const UintPtr delta = (UintPtr) 1 << ((sizeof(UintPtr) - 1) * 8);
    const int vecLength = 3 * 256 * 3 + 2048;

    v->removeAll();
    v->resize(vecLength);

    int vIndex = 0;
    for (int i = -1; i <= 1; ++i) {
        UintPtr u = i;
        for (int j = 0; j < 256; ++j, u += delta) {
            (*v)[vIndex++].setAddress((void *) u);
            (*v)[vIndex++].setAddress((void *) ~u);
            (*v)[vIndex++].setAddress((void *) - (IntPtr) u);
        }
    }

    ASSERT(vecLength - 2048 == vIndex);

    while (vIndex < vecLength) {
        (*v)[vIndex++].setAddress((void *) bigRand());
    }
}

void logMessageHandler(bsls::LogSeverity::Enum  severity,
                       const char              *file,
                       int                      line,
                       const char              *message)
    // Message handler in case the resolver logs any errors while running.
    // Stream a message containing the specified 'severity', 'file', 'line',
    // and 'message' to 'cout'.
{
    bsl::cout << "Resolver logged: " << bsls::LogSeverity::toAscii(severity) <<
                 ' ' << file << ':' << line << ' ' << message << bsl:: endl;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    // The resolver reports problems with 'bsl::log'.  Route these to 'cout' so
    // they will be visible in matrix builds.

    bsls::Log::setLogMessageHandler(&logMessageHandler);
    bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);

    bslma::TestAllocator ta;
    bslma::TestAllocator da;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // GARBAGE TEST
        //
        // Concerns: That the resolver won't segfault given garbage data.
        //
        // Plan: seed a long vector of StackTraceFrames with garbase addresses,
        //  then resolve the vector, seeing if it segfaults or returns an
        //  error.  Then stream out the frames to see if any problems are
        //  encountered streaming out.
        // --------------------------------------------------------------------

        if (verbose) cout << "Garbage Test\n"
                             "============\n";

        balst::StackTrace st;
        stuffRandomAddresses(&st);

        ASSERT(0 == Obj::resolve(&st, true));

        for (int si = 0; si < (int) st.length(); si += 128) {
            bsl::stringstream ss(&ta);

            int siLim = bsl::min(si + 128, (int) st.length());
            for (int j = si; j < siLim; ++j) {
                ss << st[si] << endl;
            }
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // balst::StackTraceResolverImp<Windows> BREATHING TEST
        //
        // Concerns:
        //   Exercise balst::StackTrace basic functionality.  Note that this
        //   test initially was also testing resolving a function that was an
        //   inline function, but either 'dbghelf.ddl' or the MSVC compiler
        //   failed miserably at this, which is beyond our means to fix.
        //
        // Plan:
        //: 1 Declare a stack trace.
        //:
        //: 2 Assign function pointers to the addresses in the stack trace.
        //:
        //: 3 Run the resolver.
        //:
        //: 4 Observe the 'symbolName', 'mangledSymbolName', and
        //:   'sourceFileName' fields generated by the resolver are correct.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                      "balst::StackTraceResolverImpl<Windows> breathing test\n"
                      "====================================================\n";

#if defined(BSLS_PLATFORM_OS_WINDOWS) && !defined(BDE_BUILD_TARGET_DBG)
        cout << "Resolving not supported without debug symbols\n";
        break;
#endif
        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  Also, apparently on Windows (but not on Linux,
        // though they use the same processor), if you take '&' of a global
        // function, you get a pointer to a thunk which will do a long jump to
        // the function.  So we'll leave the testing of symbols in shared
        // libraries and global symbols to 'balst_stacktrace.t.cpp.

        typedef bsls::Types::UintPtr UintPtr;

        balst::StackTrace st;
        st.resize(3);
        st[0].setAddress(addFixedOffset((UintPtr) &funcStaticOne));
        st[1].setAddress(addFixedOffset((UintPtr) &funcGlobalOne));
        st[2].setAddress(addFixedOffset((UintPtr) &Obj::resolve));

        // Optimizers can be just UNBELIEVABLY clever.  If you declare a
        // routine inline, it is VERY aggressive about figuring out a way to
        // inline it, even if you call it through a pointer.  'Obj::testFunc'
        // is an inline routine, but we force it out of line by taking a
        // function ptr to it.

        int testFuncLine = 0;

        for (unsigned int i = 0; i < st.length(); ++i) {
            const balst::StackTraceFrame& frame = st[i];

            ASSERT(!frame.isMangledSymbolNameKnown());
            ASSERT(!frame.isSymbolNameKnown());
            ASSERT(!frame.isLibraryFileNameKnown());
            ASSERT(!frame.isSourceFileNameKnown());
            ASSERT(!frame.isLineNumberKnown());
        }

        if (veryVerbose) {
            cout << "Before resolving:\n" << st;
        }

        Obj::resolve(&st, true);

        if (veryVerbose) {
            cout << "After  resolving:\n" << st;
        }

        for (unsigned int i = 0; i < st.length(); ++i) {
            const balst::StackTraceFrame& frame = st[i];
            bsl::string libName;

            const int iterationTestStatus = testStatus;

            ASSERT(frame.isMangledSymbolNameKnown());
            ASSERT(frame.isSymbolNameKnown());
            ASSERT(frame.isLibraryFileNameKnown());
            libName = frame.libraryFileName();
            LOOP_ASSERT(libName, npos != libName.find(
                                   "\\balst_stacktraceresolverimpl_windows."));
            LOOP_ASSERT(libName, npos != libName.find(".exe"));

            const char *sourceName, *funcName;
            switch (i) {
              case 0: {
                funcName = "funcStaticOne";
                sourceName = "balst_stacktraceresolverimpl_windows.t.cpp";
              } break;
              case 1: {
                funcName = "funcGlobalOne";
                sourceName = "balst_stacktraceresolverimpl_windows.t.cpp";
              } break;
              case 2: {
                funcName = "resolve";
                sourceName = "balst_stacktraceresolverimpl_windows.cpp";
              } break;
            }

#if defined(BSLS_PLATFORM_CMP_MSVC)
# if  (BSLS_PLATFORM_CMP_VERSION >= 1700 && BSLS_PLATFORM_CMP_VERSION < 2000) \
    || BSLS_PLATFORM_CMP_VERSION < 1600
            if (0 == i) {
                // Statics are invisible on most versions of the MSVC compiler.

                continue;
            }

            enum { e_SOURCE = false };
            enum { e_DEMANGLE = false };
# else
            enum { e_SOURCE = true };
            enum { e_DEMANGLE = true };
# endif
#endif

            if (e_SOURCE) {
                bsl::size_t fnIdx;
                LOOP_ASSERT(i, npos !=
                            (fnIdx = frame.sourceFileName().find(sourceName)));
                fnIdx = npos == fnIdx ? 0 : fnIdx;
                LOOP3_ASSERT(i, frame.sourceFileName(), sourceName,
                             !strcmp(frame.sourceFileName().c_str() + fnIdx,
                                     sourceName));
            }

            if (e_DEMANGLE) {
                LOOP3_ASSERT(i, frame.symbolName(), funcName,
                                     i >= 2 || frame.symbolName() == funcName);
                LOOP3_ASSERT(i, frame.symbolName(), funcName,
                                    npos != frame.symbolName().find(funcName));
                LOOP3_ASSERT(i, frame.mangledSymbolName(), funcName,
                             npos != frame.mangledSymbolName().find(funcName));
            }
            else {
                LOOP3_ASSERT(i, frame.symbolName(), funcName,
                                    npos != frame.symbolName().find(funcName));
                LOOP3_ASSERT(i, frame.mangledSymbolName(), funcName,
                             npos != frame.mangledSymbolName().find(funcName));
            }

            if (testStatus > iterationTestStatus) {
                P(st[i]);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

#else

int main()
{
    return -1;
}

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
