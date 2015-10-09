// balst_stacktraceresolverimpl_xcoff.t.cpp                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_xcoff.h>

#include <balst_objectfileformat.h>

#include <bdlma_sequentialallocator.h>

#include <bslma_testallocator.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF

#include <unistd.h>

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
// [ 3] memory heap leak test
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

typedef balst::StackTraceResolverImpl<balst::ObjectFileFormat::Xcoff> Obj;
typedef balst::StackTraceFrame                                        Frame;
typedef bsls::Types::UintPtr                                          UintPtr;

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

static const bsl::size_t npos = bsl::string::npos;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int phonyCompare(const void *, const void*)
{
    return 0;
}

static
int funcStaticOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    int j, k;

    for (j = 0; j < 2; ++j) {
        i = i * i * i * i;
    }

    for (k = 0; k < 2; ++k) {
        i = i * i * i * i;
    }

    return 5 * i * j * k;
}

int funcGlobalOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    int j, k;

    for (j = 0; j < 2; ++j) {
        i = i * i * i * i;
    }

    for (k = 0; k < 2; ++k) {
        i = i * i * i * i;
    }

    return 5 * i * j * k;
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

template <typename TYPE>
static TYPE abs(TYPE num)
{
    return num >= 0 ? num : -num;
}

static
const void *addFixedOffset(bsls::Types::UintPtr funcAddress)
    // Given a function pointer stored in a 'UintPtr', add an offset to the
    // pointer and return it as a 'const void *'.
{
#ifdef BSLS_PLATFORM_OS_AIX
    // when you take '&' of a function on AIX, you get a pointer to a record
    // describing the function, which has a pointer in it to the actual code.

    const char **ptr = (const char **) funcAddress;

    return *ptr + 1;
#else
# error unsupported platform
#endif
}

static
bool safeCmp(const char *a, const char *b, int len = -1)
    // Do '!strcmp', returning 'true' if the specified 'a' and 'b' point to
    // identical strings.  Return 'false' if either one is null.
{
    if (!a || !b) {
        return false;                                                 // RETURN
    }

    return -1 == len ? !bsl::strcmp(a, b) : !bsl::strncmp(a, b, len);
}

static
const char *ng(const char *str)
    // null guard -- substitute "(null)" for the specified 'str' if 'str' is
    // null.
{
    return str ? str : "(null)";
}

// knuth random number generator

static bsls::Types::Uint64 bigRandSeed = 0;
static const bsls::Types::Uint64 randA = 6364136223846793005ULL;
static const bsls::Types::Uint64 randC = 1442695040888963407ULL;

static
UintPtr bigRand()
{
    typedef bsls::Types::Uint64 Uint64;

    Uint64 next = randA * bigRandSeed + randC;
    UintPtr lowBits = next >> 32;
    bigRandSeed = randA * next + randC;

    return (UintPtr) bigRandSeed ^ lowBits;
}

void stuffRandomAddresses(balst::StackTrace *st)
{
    const UintPtr delta = (UintPtr) 1 << ((sizeof(UintPtr) - 1) * 8);
    const int stLength = 3 * 256 * 3 + 2048;

    st->removeAll();
    st->resize(stLength);

    int stIndex = 0;
    for (int i = -1; i <= 1; ++i) {
        UintPtr u = i;
        for (int j = 0; j < 256; ++j, u += delta) {
            (*st)[stIndex++].setAddress((void *) u);
            (*st)[stIndex++].setAddress((void *) ~u);
            (*st)[stIndex++].setAddress((void *) -u);
        }
    }

    ASSERT(stLength - 2048 == stIndex);

    while (stIndex < stLength) {
        (*st)[stIndex++].setAddress((void *) bigRand());
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: Potential Memory Leak (see DRQS 42134199)
        //
        // Concerns:
        //: 1 That heap memory allocated when resolving symbols is reclaimed.
        //
        // Plan:
        //: 1 Resolve symbols repeatedly and observe, using 'sbrk', that the
        //:   stack top remains constant (note that the stack top will grow
        //:   for the first several iterations due to memory fragmentation,
        //:   but should eventually settle down into 100% of memory being
        //:   reclaimed.
        // --------------------------------------------------------------------

        if (verbose) cout << "Memory Leak Test\n"
                             "================\n";

        bslma::TestAllocator ta;

        balst::StackTrace frames(&ta);
        long heapTop = 0;
        int iterations = verbose ? 500 : 50;

        for (int ti = 0; ti < iterations;  ++ti) {
            for (int tj = 0; tj < 10; ++tj) {
                frames.resize(1);
                frames[0].setAddress(addFixedOffset((UintPtr) &Obj::resolve));

                ASSERT(0 == Obj::resolve(&frames, true));
                frames.resize(0);
            }

            if (4 == ti) {
                heapTop = (long) sbrk(0);
            }
            else if (ti > 4) {
                LOOP2_ASSERT((long) sbrk(0), heapTop,
                                                    (long) sbrk(0) == heapTop);
            }

            if (verbose) P((long) sbrk(0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GARBAGE TEST
        //
        // Concerns: That the resolver won't segfault given garbage data.
        //
        // Plan: seed a stack trace with many garbase addresses, then resolve
        //  the stack trace, seeing if it segfaults or returns an error.  Then
        //  stream out the frames to see if any problems are encountered
        //  streaming out.
        // --------------------------------------------------------------------

        if (verbose) cout << "Garbage Test\n"
                             "============\n";

        balst::StackTrace st;
        stuffRandomAddresses(&st);

        ASSERT(0 == Obj::resolve(&st,
                                 true));

        for (int stIndex = 0; stIndex < (int) st.length(); stIndex += 128) {
            bsl::stringstream ss;

            int stIndexLim = bsl::min(stIndex + 128, (int) st.length());
            for (int j = stIndex; j < stIndexLim; ++j) {
                ss << st[stIndex] << endl;
            }
        }

      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // balst::StackTraceResolverImp<Xcoff> BREATHING TEST
        //
        // Concerns: Exercise balst::StackTrace basic functionality.
        //
        // Plan: Call 'printStackTrace()' to print a stack trace.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                       "balst::StackTraceResolverImpl<Xcoff> breathing test\n"
                       "===================================================\n";

        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  We'll leave the testing of symbols in shared
        // libraries to 'balst_stacktrace.t.cpp.

        typedef bsls::Types::UintPtr UintPtr;

        // We don't have source file name or line number info if we're not
        // built with debug
#ifdef BDE_BUILD_TARGET_DBG
        const bool debug = true;
#else
        const bool debug = false;
#endif

        for (bool demangle = false; true; demangle = true) {
            balst::StackTrace frames;
            frames.resize(5);
            frames[0].setAddress(addFixedOffset((UintPtr) &funcGlobalOne));
            frames[1].setAddress(addFixedOffset((UintPtr) &funcStaticOne));

            // The optizer is just UNBELIEVABLY clever.  If you declare a
            // routine inline, it is VERY aggressive about figuring out a way
            // to inline it, even if you call it through a pointer.
            // 'Obj::testFunc' is an inline routine, but we force it out of
            // line by taking a function ptr to it.

            UintPtr testFuncPtr = (UintPtr) &Obj::testFunc;

            // If we now just called through it, the optimizer would inline the
            // call.  So let's juggle (without actually changing it) a bit in a
            // way the optizer can't possibly figure out:

            testFuncPtr = foilOptimizer(testFuncPtr);

            int testFuncLine = (* (int (*)()) testFuncPtr)();
            frames[2].setAddress(addFixedOffset(testFuncPtr));

            frames[3].setAddress(addFixedOffset((UintPtr) &Obj::resolve));

            // make sure 'qsort' is loaded

            int ints[] = { 0, 1 };
            bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);
            frames[4].setAddress(addFixedOffset((UintPtr) &qsort));

            int line;
            const char *name;

            for (int i = 0; i < frames.length(); ++i) {
                ASSERT(!frames[i].isMangledSymbolNameKnown());
                ASSERT(!frames[i].isSymbolNameKnown());
                ASSERT(!frames[i].isLibraryFileNameKnown());
                ASSERT(!frames[i].isSourceFileNameKnown());
                ASSERT(!frames[i].isLineNumberKnown());
            }

            Obj::resolve(&frames,
                         demangle);

            if (veryVerbose) {
                cout << "Pass " << (int) demangle << endl;

                for (int i = 0; i < frames.length(); ++i) {
                    cout << '(' << i << "): " << frames[i] << endl;
                }
            }

            for (int i = 0; i < frames.length(); ++i) {
                LOOP_ASSERT(i, frames[i].isMangledSymbolNameKnown());
                LOOP_ASSERT(i, frames[i].isSymbolNameKnown());
                LOOP_ASSERT(i, frames[i].isLibraryFileNameKnown());
                if (debug && 4 != i) {
                    LOOP_ASSERT(i, frames[i].isSourceFileNameKnown());
                    line = frames[i].lineNumber();
                    LOOP2_ASSERT(i, line, line > 0);
                    if (2 == i) {
                        // check that the line number we got within 'testFunc'
                        // is near the '__LINE__' we took within that routine.

                        LOOP2_ASSERT(line, testFuncLine,
                                                abs(line - testFuncLine) < 35);
                    }
                }
            }

            if (verbose) P(frames[0].libraryFileName());
            ASSERT(safeCmp(frames[0].libraryFileName().c_str(),
                           "balst_stacktraceresolverimpl_xcoff.t.",
                           32));
            for (int i = 1; i < frames.length(); ++i) {
                if (4 == i) {
                    if (verbose) P(frames[4].libraryFileName());
                    LOOP2_ASSERT(i, frames[4].libraryFileName(),
                             npos != frames[4].libraryFileName().find(
                                                                "/lib/libc."));
                    ASSERT('/' == frames[4].libraryFileName()[0]);
                }
                else {
                    LOOP2_ASSERT(i, frames[i].libraryFileName(),
                                      frames[0].libraryFileName() ==
                                               frames[i].libraryFileName());
                }
            }

            if (debug) {
                for (int i = 0; i < frames.length(); ++i) {
                    name = frames[i].sourceFileName().c_str();
                    LOOP_ASSERT(i, 4 == i || (name && *name));
                    if (name) {
                        const char *pc = name + bsl::strlen(name);
                        while (pc > name && '/' != pc[-1]) {
                            --pc;
                        }
                        switch (i) {
                          case 2: {
                            LOOP_ASSERT(pc, !bsl::strcmp(pc,
                                  "balst_stacktraceresolverimpl_xcoff.h"));
                          }  break;
                          case 3: {
                            LOOP_ASSERT(pc, !bsl::strcmp(pc,
                                  "balst_stacktraceresolverimpl_xcoff.cpp"));
                          }  break;
                          case 4: {
                             ; // so nothing
                          }  break;
                          default: {
                            LOOP2_ASSERT(i, pc, !bsl::strcmp(pc,
                                  "balst_stacktraceresolverimpl_xcoff.t.cpp"));
                          }  break;
                        }
                    }
                }
            }

            LOOP_ASSERT(frames[0].symbolName(),
                      npos != frames[0].symbolName().find("funcGlobalOne"));
            LOOP_ASSERT(frames[1].symbolName(),
                      npos != frames[1].symbolName().find("funcStaticOne"));
            LOOP_ASSERT(frames[0].mangledSymbolName(),
               npos != frames[0].mangledSymbolName().find("funcGlobalOne"));
            LOOP_ASSERT(frames[1].mangledSymbolName(),
               npos != frames[1].mangledSymbolName().find("funcStaticOne"));
            LOOP_ASSERT(frames[2].symbolName(),
                           npos != frames[2].symbolName().find("testFunc"));
            LOOP_ASSERT(frames[2].mangledSymbolName(),
                    npos != frames[2].mangledSymbolName().find("testFunc"));
            LOOP_ASSERT(frames[3].mangledSymbolName(),
                     npos != frames[3].mangledSymbolName().find("resolve"));
            LOOP_ASSERT(frames[4].mangledSymbolName(),
                       npos != frames[4].mangledSymbolName().find("qsort"));

            if (demangle) {
                LOOP_ASSERT(frames[0].symbolName(),
                           frames[0].symbolName() == ".funcGlobalOne(int)");
                LOOP_ASSERT(frames[1].symbolName(),
                           frames[1].symbolName() == ".funcStaticOne(int)");
                LOOP_ASSERT(frames[2].symbolName(),
                             frames[2].symbolName() == "BloombergLP::"
                                  "balst::StackTraceResolverImpl<BloombergLP::"
                               "balst::ObjectFileFormat::Xcoff>::.testFunc()");
                {
                    const char *match =
                                  "BloombergLP::"
                                  "balst::StackTraceResolverImpl<BloombergLP::"
                                  "balst::ObjectFileFormat::Xcoff>::.resolve(";
                    int matchLen = bsl::strlen(match);
                    LOOP_ASSERT(frames[3].symbolName(),
                                     safeCmp(frames[3].symbolName().c_str(),
                                             match,
                                             matchLen));
                }
                LOOP_ASSERT(frames[4].symbolName(),
                                        frames[4].symbolName() == ".qsort");

                break;
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
