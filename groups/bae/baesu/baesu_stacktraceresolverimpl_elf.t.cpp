// baesu_stacktraceresolverimpl_elf.t.cpp                             -*-C++-*-
#include <baesu_stacktraceresolverimpl_elf.h>

#include <baesu_objectfileformat.h>

#include <bdema_sequentialallocator.h>

#include <bslma_testallocator.h>
#include <bsls_types.h>

#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_ELF

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

typedef baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Elf> Obj;
typedef baesu_StackTraceFrame                                     Frame;
typedef bsls_Types::UintPtr                                       UintPtr;

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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
int funcStaticOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    for (int j = 0; j < 2; ++j) {
        i = i * i * i;
    }

    return 5 * i;
}

int funcGlobalOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    for (int j = 0; j < 2; ++j) {
        i = i * i * i * i;
    }

    return 6 * i;
}


static inline
int funcStaticInlineOne(int i)
    // Target function to be resolved.  Never called.  Do some arbitary
    // arithmetic so there will be some length of code in this routine.
{
    for (int j = 0; j < 5; ++j) {
        i = 75 * i * i + i / (j + 1);
    }

    return 5 * i;
}

static
const void *addFixedOffset(bsls_Types::UintPtr funcAddress)
    // Given a function pointer stored in a 'UintPtr', add an offset to the
    // pointer and return it as a 'const void *'.
{
#ifdef BSLS_PLATFORM__OS_HPUX
    // On HPUX, '&functionName' is a ptr to a ptr to a record consisting of 2
    // words, one of which is a pointer to the actual code.

    const char **ptr = (const char **) funcAddress;

#if defined(BSLS_PLATFORM__CPU_32_BIT)
    return ptr[1] + 4;
#else
    return ptr[0] + 4;
#endif

#else
    const char *ptr = (const char *) funcAddress;

    return ptr + 4;
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

            int vecIndexLim = bsl::min(vecIndex + 127, (int) traceVec.size());
            for (int j = vecIndex; j < vecIndexLim; ++j) {
                ss << traceVec[j] << endl;
            }
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // baesu_StackTraceResolverImp<Elf> BREATHING TEST
        //
        // Concerns: Exercise baesu_StackTrace basic functionality.
        //
        // Plan: Call 'printStackTrace()' to print a stack trace.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                          "baesu_StackTraceResolverImpl<Elf> breathing test\n"
                          "================================================\n";

        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  We'll leave the testing of symbols in shared
        // libraries to 'baesu_stacktrace.t.cpp.

        typedef bsls_Types::UintPtr UintPtr;

        for (bool demangle = false; true; demangle = true) {
            bsl::vector<Frame> frames(&sa);
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
            ASSERT(testFuncLine < 2000);
            frames.at(2).setAddress(addFixedOffset(testFuncPtr));

            testFuncPtr = (UintPtr) &funcStaticInlineOne;

            // If we now just called through it, the optimizer would inline the
            // call.  So let's juggle (without actually changing it) a bit in a
            // way the optizer can't possibly figure out:

            testFuncPtr = foilOptimizer(testFuncPtr);

            int result = (* (int (*)(int)) testFuncPtr)(100);
            ASSERT(result > 10000);
            frames.at(3).setAddress(addFixedOffset(testFuncPtr));

#if 0
            // Testing '&qsort' doesn't work.  The similar test in
            // baesu_stacktrace.t.cpp works.  I think what's happening is
            // &qsort doesn't properly point to 'qsort', it points to a thunk
            // that dynamically loads qsort and then calls it.  So when the
            // resolver tries to resolve the ptr to the thunk, it can't.

            {
                // make sure the component contain 'qsort' is loaded

                int ints[] = { 0, 1 };
                bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);
            }
            frames[4].setAddress(addFixedOffset((UintPtr) &bsl::qsort));
#endif

            frames[4].setAddress(addFixedOffset((UintPtr)
                   &baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Elf>::
                                                                     resolve));

            for (int i = 0; i < (int) frames.size(); ++i) {
                if (veryVerbose) {
                    cout << "frames[" << i << "].address() = " <<
                                         (UintPtr) frames[i].address() << endl;
                }

                LOOP_ASSERT(i, frames[i].address());
#undef  IS_INVALID
#define IS_INVALID(name) LOOP_ASSERT(i, !frames[i].is ## name ## Valid());
                IS_INVALID(LibraryFileName);
                IS_INVALID(LineNumber);
                IS_INVALID(MangledSymbolName);
                IS_INVALID(OffsetFromSymbol);
                IS_INVALID(SourceFileName);
                IS_INVALID(SymbolName);
#undef IS_INVALID
            }

            Obj::resolve(&frames,
                         demangle,
                         &sa);

            if (veryVerbose) {
                cout << "Pass " << (int) demangle << endl;

                for (unsigned i = 0; i < frames.size(); ++i) {
                    cout << '(' << i << "): " << frames[i] << endl;
                }
            }

            for (unsigned i = 0; i < frames.size(); ++i) {
#undef  IS_VALID
#define IS_VALID(name) LOOP_ASSERT(i, frames[i].is ## name ## Valid());
                IS_VALID(Address);
                IS_VALID(LibraryFileName);
                IS_VALID(MangledSymbolName);
                IS_VALID(OffsetFromSymbol);
                if (1 == i) {
                    IS_VALID(SourceFileName);    // static symbols only
                }
                IS_VALID(SymbolName);
            }
#undef IS_VALID

            const char *libName = ng(frames[0].libraryFileName());
            const char *thisLib = "baesu_stacktraceresolverimpl_elf.t";
#undef  GOOD_LIBNAME
#define GOOD_LIBNAME(func, exp, match) \
            LOOP3_ASSERT(#func, exp, ng(match), func(ng(exp), match));

            GOOD_LIBNAME(safeStrStr,  frames[0].libraryFileName(), thisLib);
            GOOD_LIBNAME(safeCmp,     frames[1].libraryFileName(), libName);
            GOOD_LIBNAME(safeCmp,     frames[2].libraryFileName(), libName);
            GOOD_LIBNAME(safeCmp,     frames[3].libraryFileName(), libName);
            GOOD_LIBNAME(safeCmp,     frames[4].libraryFileName(), libName);
#undef  GOOD_LIBNAME

            // frame[1] was pointing to a static, the ELF resolver should have
            // found this source file name.

#ifndef BSLS_PLATFORM__OS_LINUX
            for (unsigned i = 0; i < frames.size(); ++i) {
                if (0 == i || 2 == i || 4 == i) {
                    ASSERT(!frames[i].sourceFileName());
                    continue;
                }

                const char *name = ng(frames[i].sourceFileName());
                ASSERT(name && *name);
                if (name) {
                    const char *pc = name + bsl::strlen(name);
                    while (pc > name && '/' != pc[-1]) {
                        --pc;
                    }

                    LOOP2_ASSERT(i, pc, !bsl::strcmp(pc,
                                    "baesu_stacktraceresolverimpl_elf.t.cpp"));
                }
            }
#endif

#undef  SM
#define SM(nm, match) {                                             \
                const char *name = nm;                              \
                LOOP2_ASSERT(name, match, safeStrStr(name, match)); \
            }

            SM(frames[0].mangledSymbolName(), "funcGlobalOne");
            SM(frames[1].mangledSymbolName(), "funcStaticOne");
            SM(frames[2].mangledSymbolName(), "testFunc");
            SM(frames[3].mangledSymbolName(), "funcStaticInlineOne");
            SM(frames[4].mangledSymbolName(), "resolve");

            SM(frames[0].symbolName(), "funcGlobalOne");
            SM(frames[1].symbolName(), "funcStaticOne");
            SM(frames[2].symbolName(), "testFunc");
            SM(frames[3].symbolName(), "funcStaticInlineOne");
            SM(frames[4].symbolName(), "resolve");

            if (demangle) {
#undef  SM
#define SM(i, match) {                                         \
                    const char *name = frames[i].symbolName(); \
                    LOOP_ASSERT(name, safeCmp(name, match));   \
                }

                SM(0, "funcGlobalOne(int)");
                SM(1, "funcStaticOne(int)");
                SM(2, "BloombergLP::"
                      "baesu_StackTraceResolverImpl"
                      "<BloombergLP::"
                      "baesu_ObjectFileFormat::Elf>::"
                      "testFunc()");
                SM(3, "funcStaticInlineOne(int)");
                const char *resName = "BloombergLP::"
                                      "baesu_StackTraceResolverImpl"
                                      "<BloombergLP::"
                                      "baesu_ObjectFileFormat::Elf>::"
                                      "resolve(";
                int resNameLen = bsl::strlen(resName);
                const char *name4 = frames[4].symbolName();
                LOOP2_ASSERT(name4, resName,
                                          safeCmp(name4, resName, resNameLen));
                break;
            }

#if defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__CMP_GNU)
            // Sun CC, won't demangle
            break;
#endif
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
