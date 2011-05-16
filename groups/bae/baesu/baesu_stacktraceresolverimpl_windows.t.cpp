// baesu_stacktraceresolverimpl_windows.t.cpp                         -*-C++-*-
#include <baesu_stacktraceresolverimpl_windows.h>

#include <baesu_objectfileformat.h>

#include <bdema_sequentialallocator.h>
#include <bdeu_random.h>

#include <bslma_testallocator.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
// TEST PLAN
//-----------------------------------------------------------------------------

//=============================================================================
// STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
// STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
// SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
// GLOBAL HELPER #DEFINES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER TYPES & CLASSES FOR TESTING
//-----------------------------------------------------------------------------

typedef baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Windows> Obj;
typedef baesu_StackTraceFrame                                         Frame;
typedef bsls_Types::UintPtr                                           UintPtr;
typedef bsls_Types::IntPtr                                            IntPtr;

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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
const void *addFixedOffset(bsls_Types::UintPtr funcAddress)
    // Given a function pointer stored in a 'UintPtr', add an offset to the
    // pointer and return it as a 'const void *'.
{
    const char *ptr = (const char *) funcAddress;

    return ptr + 4;
}

static
bool safeCmp(const char *a, const char *b)
    // Do '!strcmp', returning 'true' if the specified 'a' and 'b' point to
    // identical strings.  Return 'false' if either one is null.
{
    if (!a || !b) {
        return false;                                                 // RETURN
    }

    return !bsl::strcmp(a, b);
}

static
bool safeStrStr(const char *string, const char *target)
    // Return 'true' if the specified 'target' points to a string contained in
    // the specified 'string'.  Return 'false' if 'string' or 'target' is null.
{
    if (!string || !target) {
        return false;                                                 // RETURN
    }

    return bsl::strstr(string, target);
}

static
const char *ng(const char *str)
    // null guard -- substitute "(null)" for the specified 'str' if 'str' is
    // null.
{
    return str ? str : "(null)";
}

static bsls_Types::Uint64 bigRandSeed = 0;
static const bsls_Types::Uint64 randA = 6364136223846793005ULL;
static const bsls_Types::Uint64 randC = 1442695040888963407ULL;

static
UintPtr bigRand()
{
    typedef bsls_Types::Uint64 Uint64;

    Uint64 next = randA * bigRandSeed + randC;
    Uint64 lowBits = next >> 32;
    bigRandSeed = randA * next + randC;

    return (UintPtr) (bigRandSeed ^ lowBits);
}

void stuffRandomAddresses(bsl::vector<baesu_StackTraceFrame> *v)
{
    const UintPtr delta = (UintPtr) 1 << ((sizeof(UintPtr) - 1) * 8);
    const int vecLength = 3 * 256 * 3 + 2048;

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

//=============================================================================
// MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    // Note we are using SequentialAllocator here -- this component does not
    // free its memory, it relies upon its memory allocator to free it all
    // upon destruction.

    bslma_TestAllocator ta;
    bdema_SequentialAllocator sa(&ta);

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

        bsl::vector<baesu_StackTraceFrame> traceVec;
        stuffRandomAddresses(&traceVec);

        ASSERT(0 == Obj::resolve(&traceVec,
                                 true,
                                 &sa));
        
        for (int vecIndex = 0; vecIndex < (int) traceVec.size();
                                                             vecIndex += 128) {
            bsl::stringstream ss(&ta);

            int vecIndexLim = bsl::min(vecIndex + 128, (int) traceVec.size());
            for (int j = vecIndex; j < vecIndexLim; ++j) {
                ss << traceVec[vecIndex] << endl;
            }
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // baesu_StackTraceResolverImp<Windows> BREATHING TEST
        //
        // Concerns: Exercise baesu_StackTrace basic functionality.
        //
        // Plan: Call 'printStackTrace()' to print a stack trace.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                      "baesu_StackTraceResolverImpl<Windows> breathing test\n"
                      "====================================================\n";

#if defined(BSLS_PLATFORM__OS_WINDOWS) && !defined(BDE_BUILD_TARGET_DBG)
        cout << "Resolving not supported without debug symbols\n";
        break;
#else
        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  Also, apparently on Windows (but not on Linux,
        // though they use the same processor), if you take '&' of a global
        // function, you get a pointer to a thunk which will do a long jump to
        // the function.  So we'll leave the testing of symbols in shared
        // libraries and global symbols to 'baesu_stacktrace.t.cpp.

        typedef bsls_Types::UintPtr UintPtr;

        bsl::vector<Frame> frames;
        frames.resize(1);
        frames[0].setAddress(addFixedOffset((UintPtr) &funcStaticOne));

        // Optimizers can be just UNBELIEVABLY clever.  If you declare a
        // routine inline, it is VERY aggressive about figuring out a way to
        // inline it, even if you call it through a pointer.  'Obj::testFunc'
        // is an inline routine, but we force it out of line by taking a
        // function ptr to it.

        int testFuncLine = 0;
#if 0
        // inline function test on windows (see comment further down)
        // doesn't work

        typedef int (*TestFuncPtr)();
        TestFuncPtr tfp = &Obj::testFunc;
        UintPtr testFuncPtr = (UintPtr) tfp;

        // If we now just called through it, the optimizer would inline the
        // call.  So let's juggle (without actually changing it) a bit in a way
        // the optizer can't possibly figure out:

        testFuncPtr = foilOptimizer(testFuncPtr);

        testFuncLine = (* (TestFuncPtr) testFuncPtr)();
        frames[1].setAddress(addFixedOffset(testFuncPtr));
#endif

        for (unsigned i = 0; i < frames.size(); ++i) {
            const baesu_StackTraceFrame& frame = frames[i];
            const char *name;

            name = frame.mangledSymbolName();
            ASSERT(!name);
            name = frame.symbolName();
            ASSERT(!name);
            name = frame.libraryFileName();
            ASSERT(!name);
            name = frame.sourceFileName();
            ASSERT(!name);
            ASSERT(frame.lineNumber() < 0);
        }

        Obj::resolve(&frames,
                     true,
                     &sa);

        if (veryVerbose) {
            cout << "0): " << frames[0] << endl;
            cout << "1): " << frames[1] << endl;
        }

        for (unsigned i = 0; i < frames.size(); ++i) {
            const baesu_StackTraceFrame& frame = frames[i];
            const char *name;

            name = frame.mangledSymbolName();
            LOOP_ASSERT(ng(name), name && *name);
            name = frame.symbolName();
            LOOP_ASSERT(ng(name), name && *name);
            name = frame.libraryFileName();
            LOOP_ASSERT(ng(name), name && *name);
            LOOP_ASSERT(name, safeStrStr(frame.libraryFileName(),
                              "\\baesu_stacktraceresolverimpl_windows."));
            LOOP_ASSERT(name, safeStrStr(frame.libraryFileName(), ".exe"));
            if (0 == i) {
                name = frame.sourceFileName();
                ASSERT(name && *name);
                if (name) {
                    const char *pc = name + bsl::strlen(name);
                    while (pc > name && '\\' != pc[-1]) {
                        --pc;
                    }

                    LOOP_ASSERT(pc, !bsl::strcmp(pc,
                                "baesu_stacktraceresolverimpl_windows.t.cpp"));
                }
                ASSERT(safeCmp(frame.symbolName(),           "funcStaticOne"));
                ASSERT(safeStrStr(frame.mangledSymbolName(), "funcStaticOne"));
            } else if (false && 1 == i) {
                // *NONE* of these test work.  dbghelp.dll totally falls on its
                // face when there's an inline routine on the stack.  There's
                // nothing we can do about it except completely implement a
                // better dbghelp.dll ourselves.  And furthermore, it might
                // not be a problem with dbghelp.dll at all, it might be a
                // problem with the compiler, in which case we would have to
                // write our own CC++ compiler for Microsoft.  And since our
                // clients might not use that compiler, we would have to not
                // only write a compiler, but write one so good that it becomes
                // universally used on Windows.  Screw it.

                name = frame.sourceFileName();
                ASSERT(safeCmp(name,"baesu_stacktraceresolverimpl_windows.h"));

                LOOP_ASSERT(frame.lineNumber(),
                                  abs(frame.lineNumber() - testFuncLine) < 30);
                ASSERT(safeStrStr(frame.mangledSymbolName(), "testFunc"));
                LOOP_ASSERT(frame.symbolName(), safeCmp(frame.symbolName(),
                               "BloombergLP::baesu_StackTraceResolverImpl"
                               "<BloombergLP::baesu_ObjectFileFormat::Windows>"
                               "::testFunc"));
            }
        }
#endif
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
