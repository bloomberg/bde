// baesu_stacktraceresolverimpl_xcoff.t.cpp                           -*-C++-*-
#include <baesu_stacktraceresolverimpl_xcoff.h>

#include <baesu_objectfileformat.h>

#include <bdema_sequentialallocator.h>

#include <bslma_testallocator.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF

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

typedef baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Xcoff> Obj;
typedef baesu_StackTraceFrame                                       Frame;
typedef bsls_Types::UintPtr                                         UintPtr;

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static const bsl::size_t npos = bsl::string::npos;

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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
const void *addFixedOffset(bsls_Types::UintPtr funcAddress)
    // Given a function pointer stored in a 'UintPtr', add an offset to the
    // pointer and return it as a 'const void *'.
{
#ifdef BSLS_PLATFORM__OS_AIX
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

static bsls_Types::Uint64 bigRandSeed = 0;
static const bsls_Types::Uint64 randA = 6364136223846793005ULL;
static const bsls_Types::Uint64 randC = 1442695040888963407ULL;

static
UintPtr bigRand()
{
    typedef bsls_Types::Uint64 Uint64;

    Uint64 next = randA * bigRandSeed + randC;
    UintPtr lowBits = next >> 32;
    bigRandSeed = randA * next + randC;

    return (UintPtr) bigRandSeed ^ lowBits;
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
            (*v)[vIndex++].setAddress((void *) -u);
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
        // baesu_StackTraceResolverImp<Xcoff> BREATHING TEST
        //
        // Concerns: Exercise baesu_StackTrace basic functionality.
        //
        // Plan: Call 'printStackTrace()' to print a stack trace.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "baesu_StackTraceResolverImpl<Xcoff> breathing test\n"
                        "==================================================\n";

        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  We'll leave the testing of symbols in shared
        // libraries to 'baesu_stacktrace.t.cpp.

        typedef bsls_Types::UintPtr UintPtr;

        // We don't have source file name or line number info if we're not
        // built with debug
#ifdef BDE_BUILD_TARGET_DBG
        const bool debug = true;
#else
        const bool debug = false;
#endif

        for (bool demangle = false; true; demangle = true) {
            bsl::vector<Frame> frames(&sa);
            frames.resize(5);
            frames.at(0).setAddress(addFixedOffset((UintPtr) &funcGlobalOne));
            frames.at(1).setAddress(addFixedOffset((UintPtr) &funcStaticOne));

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
            frames.at(2).setAddress(addFixedOffset(testFuncPtr));

            frames[3].setAddress(addFixedOffset((UintPtr)
                 &baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Xcoff>::
                                                                     resolve));

            // make sure 'qsort' is loaded

            int ints[] = { 0, 1 };
            bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);
            frames[4].setAddress(addFixedOffset((UintPtr) &qsort));

            int line;
            const char *name;

            for (int i = 0; i < frames.size(); ++i) {
                ASSERT(!frames.at(i).isMangledSymbolNameValid());
                ASSERT(!frames.at(i).isSymbolNameValid());
                ASSERT(!frames.at(i).isLibraryFileNameValid());
                ASSERT(!frames.at(i).isSourceFileNameValid());
                ASSERT(!frames.at(i).isLineNumberValid());
            }

            Obj::resolve(&frames,
                         demangle,
                         &sa);

            if (veryVerbose) {
                cout << "Pass " << (int) demangle << endl;

                for (int i = 0; i < frames.size(); ++i) {
                    cout << '(' << i << "): " << frames.at(i) << endl;
                }
            }

            for (int i = 0; i < frames.size(); ++i) {
                LOOP_ASSERT(i, frames.at(i).isMangledSymbolNameValid());
                LOOP_ASSERT(i, frames.at(i).isSymbolNameValid());
                LOOP_ASSERT(i, frames.at(i).isLibraryFileNameValid());
                if (debug && 4 != i) {
                    LOOP_ASSERT(i, frames.at(i).isSourceFileNameValid());
                    line = frames.at(i).lineNumber();
                    LOOP2_ASSERT(i, line, line > 0);
                    if (2 == i) {
                        // check that the line number we got within 'testFunc'
                        // is near the '__LINE__' we took within that routine.

                        LOOP2_ASSERT(line, testFuncLine,
                                                abs(line - testFuncLine) < 35);
                    }
                }
            }

            if (verbose) P(frames.at(0).libraryFileName());
            ASSERT(safeCmp(frames.at(0).libraryFileName().c_str(),
                           "baesu_stacktraceresolverimpl_xcoff.t.",
                           32));
            for (int i = 1; i < frames.size(); ++i) {
                if (4 == i) {
                    if (verbose) P(frames.at(4).libraryFileName());
                    LOOP2_ASSERT(i, frames.at(4).libraryFileName(),
                             npos != frames.at(4).libraryFileName().find(
                                                                "/lib/libc."));
                    ASSERT('/' == frames.at(4).libraryFileName()[0]);
                }
                else {
                    LOOP2_ASSERT(i, frames.at(i).libraryFileName(),
                                      frames.at(0).libraryFileName() ==
                                               frames.at(i).libraryFileName());
                }
            }

            if (debug) {
                for (int i = 0; i < frames.size(); ++i) {
                    name = frames.at(i).sourceFileName().c_str();
                    LOOP_ASSERT(i, 4 == i || (name && *name));
                    if (name) {
                        const char *pc = name + bsl::strlen(name);
                        while (pc > name && '/' != pc[-1]) {
                            --pc;
                        }
                        switch (i) {
                          case 2: {
                            LOOP_ASSERT(pc, !bsl::strcmp(pc,
                                  "baesu_stacktraceresolverimpl_xcoff.h"));
                          }  break;
                          case 3: {
                            LOOP_ASSERT(pc, !bsl::strcmp(pc,
                                  "baesu_stacktraceresolverimpl_xcoff.cpp"));
                          }  break;
                          case 4: {
                             ; // so nothing
                          }  break;
                          default: {
                            LOOP2_ASSERT(i, pc, !bsl::strcmp(pc,
                                  "baesu_stacktraceresolverimpl_xcoff.t.cpp"));
                          }  break;
                        }
                    }
                }
            }

            LOOP_ASSERT(frames.at(0).symbolName(),
                      npos != frames.at(0).symbolName().find("funcGlobalOne"));
            LOOP_ASSERT(frames.at(1).symbolName(),
                      npos != frames.at(1).symbolName().find("funcStaticOne"));
            LOOP_ASSERT(frames.at(0).mangledSymbolName(),
               npos != frames.at(0).mangledSymbolName().find("funcGlobalOne"));
            LOOP_ASSERT(frames.at(1).mangledSymbolName(),
               npos != frames.at(1).mangledSymbolName().find("funcStaticOne"));
            LOOP_ASSERT(frames.at(2).symbolName(),
                           npos != frames.at(2).symbolName().find("testFunc"));
            LOOP_ASSERT(frames.at(2).mangledSymbolName(),
                    npos != frames.at(2).mangledSymbolName().find("testFunc"));
            LOOP_ASSERT(frames.at(3).mangledSymbolName(),
                     npos != frames.at(3).mangledSymbolName().find("resolve"));
            LOOP_ASSERT(frames.at(4).mangledSymbolName(),
                       npos != frames.at(4).mangledSymbolName().find("qsort"));

            if (demangle) {
                LOOP_ASSERT(frames.at(0).symbolName(),
                           frames.at(0).symbolName() == ".funcGlobalOne(int)");
                LOOP_ASSERT(frames.at(1).symbolName(),
                           frames.at(1).symbolName() == ".funcStaticOne(int)");
                LOOP_ASSERT(frames.at(2).symbolName(),
                             frames.at(2).symbolName() == "BloombergLP::"
                                   "baesu_StackTraceResolverImpl<BloombergLP::"
                               "baesu_ObjectFileFormat::Xcoff>::.testFunc()");
                {
                    const char *match =
                                   "BloombergLP::"
                                   "baesu_StackTraceResolverImpl<BloombergLP::"
                                   "baesu_ObjectFileFormat::Xcoff>::.resolve(";
                    int matchLen = bsl::strlen(match);
                    LOOP_ASSERT(frames.at(3).symbolName(),
                                     safeCmp(frames.at(3).symbolName().c_str(),
                                             match,
                                             matchLen));
                }
                LOOP_ASSERT(frames.at(4).symbolName(),
                                        frames.at(4).symbolName() == ".qsort");

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
