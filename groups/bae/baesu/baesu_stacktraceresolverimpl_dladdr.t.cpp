// baesu_stacktraceresolverimpl_dladdr.t.cpp                          -*-C++-*-
#include <baesu_stacktraceresolverimpl_dladdr.h>

#include <baesu_stacktrace.h>

#include <baesu_objectfileformat.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_types.h>

#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_DLADDR

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
// TEST PLAN
//
// The component has only one public function, 'resolve'.  There are two ways
// to test it:
//: 1 supplying the stack trace with valid code addresses, and after resolution
//:   we will confirm that the symbol names it has resolved are correct.
//: 2 supplying the stack trace with a large number of garbage addresses, to
//:   see if any addresses cause undefined behavior such as a segfault or
//:   crash.  This is important because most of the work is being done by the
//:   'dladdr' function, which is supplied to us by the operating system, and
//:   for which we have no source and over which we have no control.
//-----------------------------------------------------------------------------
// [ 2] resolve maliciously chosen garbage code addresses
// [ 1] resolve valid code addresses
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

typedef baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Dladdr> Obj;
typedef baesu_StackTraceFrame                                        Frame;
typedef bsls_Types::UintPtr                                          UintPtr;

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int phonyCompare(const void *, const void*)
{
    return 0;
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
#ifdef BSLS_PLATFORM_OS_HPUX
    // On HPUX, '&functionName' is a ptr to a ptr to a record consisting of 2
    // words, one of which is a pointer to the actual code.

    const char **ptr = (const char **) funcAddress;

#if defined(BSLS_PLATFORM_CPU_32_BIT)
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
const char *safeBaseName(const char *str)
{
    if (!str) {
        return "";                                                    // RETURN
    }

    const char *ret = str;
    for (const char *next; (next = bsl::strchr(ret, '/')); ) {
        ret = next + 1;
    }

    return ret;
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

void stuffRandomAddresses(baesu_StackTrace *stackTrace)
{
    const UintPtr delta = (UintPtr) 1 << ((sizeof(UintPtr) - 1) * 8);
    const int vecLength = 3 * 256 * 3 * 2;

    UintPtr values[vecLength];

    stackTrace->resize(vecLength);

    int vIndex = 0;
    UintPtr u = 0;
    for (int j = 0; j < 256; ++j, u += delta, ++vIndex) {
        values[vIndex] = u;
    }
    for (int j = 0; j < 256; ++j, ++vIndex) {
        values[vIndex] = values[vIndex - 256] - 1;
    }
    for (int j = 0; j < 256; ++j, ++vIndex) {
        values[vIndex] = values[vIndex - 512] + 1;
    }

    const int three256 = 3 * 256;
    ASSERT(three256 == vIndex);
    for (int j = 0; j < three256; ++j, ++vIndex) {
        values[vIndex] = ~values[vIndex - three256];
    }

    const int six256 = 6 * 256;
    ASSERT(six256 == vIndex);
    for (int j = 0; j < three256; ++j, ++vIndex) {
        values[vIndex] = - values[vIndex - six256];
    }

    const UintPtr hiMask = (UintPtr) 0xff << ((sizeof(UintPtr) - 1) * 8);
    const UintPtr loMask = ~hiMask;

    ASSERT(9 * 256 == vIndex);
    for (int j = 0; vIndex < vecLength; j = (j + 1) & 0xff, ++vIndex) {
        u = values[j] & hiMask;
        values[vIndex] = u | (bigRand() & loMask);
    }

    for (int j = 0; j < vecLength; ++j) {
        (*stackTrace)[j].setAddress((void *) values[j]);
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

    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // GARBAGE TEST
        //
        // Concerns:
        //   That the resolver won't segfault given garbage data.
        //
        // Plan:
        //   Seed a long stackTrace of StackTraceFrames with maliciously chosen
        //   garbase addresses, then resolve the stackTrace, seeing if it
        //   segfaults or fails an assert.  Then stream out the frames to see
        //   if any problems are encountered streaming out.
        // --------------------------------------------------------------------

        if (verbose) cout << "Garbage Test\n"
                             "============\n";

        bslma_TestAllocator ta;

        baesu_StackTrace stackTrace(&ta);
        stuffRandomAddresses(&stackTrace);

        (void) Obj::resolve(&stackTrace, true);

        ASSERT(0 == defaultAllocator.numAllocations());

        // Note that the stringstream below will use temporaries using the
        // default allocator whether it's constructed with another allocator
        // or not.

        for (int vecIndex = 0; vecIndex < (int) stackTrace.length();
                                                             vecIndex += 128) {
            bsl::ostringstream ss(&ta);

            int vecIndexLim = bsl::min(vecIndex + 127,
                                       (int) stackTrace.length());
            for (int j = vecIndex; j < vecIndexLim; ++j) {
                ss << stackTrace[j] << endl;
            }
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // baesu_StackTraceResolverImp<Dladdr> BREATHING TEST
        //
        // Concerns:
        //   That 'resolve' can corectly resolve code pointers into symbol
        //   names
        //
        // Plan:
        //   Seed a 'baesu_StackTrace' object with several code pointers
        //   pointeing into known functions, call 'resolve', and verify that
        //   the data fields calculated are as they should be.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                       "baesu_StackTraceResolverImpl<Dladdr> breathing test\n"
                       "===================================================\n";

        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  We'll leave the testing of symbols in shared
        // libraries to 'baesu_stacktraceutil.t.cpp.

        typedef bsls_Types::UintPtr UintPtr;

        for (bool demangle = false; true; demangle = true) {
            baesu_StackTrace stackTrace;
            stackTrace.resize(5);
            stackTrace[0].setAddress(addFixedOffset((UintPtr) &funcGlobalOne));
            stackTrace[1].setAddress(addFixedOffset((UintPtr) &funcStaticOne));

            UintPtr testFuncPtr = (UintPtr) &funcStaticInlineOne;

            // If we now just called through it, the optimizer would inline the
            // call.  So let's juggle (without actually changing it) a bit in a
            // way the optizer can't possibly figure out:

            testFuncPtr = foilOptimizer(testFuncPtr);

            int result = (* (int (*)(int)) testFuncPtr)(100);
            ASSERT(result > 10000);
            stackTrace[2].setAddress(addFixedOffset(testFuncPtr));

            // Testing '&qsort' doesn't work.  The similar test in
            // baesu_stacktraceutil.t.cpp works.  I think what's happening is
            // &qsort doesn't properly point to 'qsort', it points to a thunk
            // that dynamically loads qsort and then calls it.  So when the
            // resolver tries to resolve the ptr to the thunk, it can't.

            {
                // make sure the component contain 'qsort' is loaded

                int ints[] = { 0, 1 };
                bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);
            }
            stackTrace[3].setAddress(addFixedOffset((UintPtr) &qsort));
            stackTrace[4].setAddress(addFixedOffset((UintPtr) &Obj::resolve));

            for (int i = 0; i < (int) stackTrace.length(); ++i) {
                if (veryVerbose) {
                    cout << "stackTrace[" << i << "].address() = " <<
                                     (UintPtr) stackTrace[i].address() << endl;
                }

                LOOP_ASSERT(i, stackTrace[i].address());
#undef  IS_UNKNOWN
#define IS_UNKNOWN(name) LOOP_ASSERT(i, !stackTrace[i].is ## name ## Known());
                IS_UNKNOWN(LibraryFileName);
                IS_UNKNOWN(LineNumber);
                IS_UNKNOWN(MangledSymbolName);
                IS_UNKNOWN(OffsetFromSymbol);
                IS_UNKNOWN(SourceFileName);
                IS_UNKNOWN(SymbolName);
#undef IS_UNKNOWN
            }

            Obj::resolve(&stackTrace,
                         demangle);

            if (veryVerbose) {
                cout << "Pass " << (int) demangle << endl;

                for (int i = 0; i < stackTrace.length(); ++i) {
                    cout << '(' << i << "): " << stackTrace[i] << endl;
                }
            }

            for (int i = 0; i < stackTrace.length(); ++i) {
#undef  IS_KNOWN
#define IS_KNOWN(name) LOOP_ASSERT(i, stackTrace[i].is ## name ## Known());
                IS_KNOWN(Address);
                IS_KNOWN(LibraryFileName);
                IS_KNOWN(MangledSymbolName);
                IS_KNOWN(OffsetFromSymbol);
                IS_KNOWN(SymbolName);
            }
#undef IS_KNOWN

            const char *libName = stackTrace[0].libraryFileName().c_str();
            libName = safeBaseName(libName);
            const char *thisLib = "baesu_stacktraceresolverimpl_dladdr.t";
            const char *qsLib = "libsystem_c";
#undef  GOOD_LIBNAME
#define GOOD_LIBNAME(func, exp, match)                                        \
            LOOP3_ASSERT(#func, safeBaseName(exp), ng(match),                 \
                                               func(safeBaseName(exp), match));

            GOOD_LIBNAME(safeStrStr,
                             stackTrace[0].libraryFileName().c_str(), thisLib);
            GOOD_LIBNAME(safeCmp,
                             stackTrace[1].libraryFileName().c_str(), libName);
            GOOD_LIBNAME(safeCmp,
                             stackTrace[2].libraryFileName().c_str(), libName);
            GOOD_LIBNAME(safeStrStr,
                             stackTrace[3].libraryFileName().c_str(), qsLib);
            GOOD_LIBNAME(safeStrStr,
                             stackTrace[4].libraryFileName().c_str(), thisLib);
#undef  GOOD_LIBNAME

            // frame[1] was pointing to a static, the ELF resolver should have
            // found this source file name.

#if 0
            for (int i = 0; i < stackTrace.length(); ++i) {
                if (0 == i || 3 == i) {
                    ASSERT(!stackTrace[i].isSourceFileNameKnown());
                    continue;
                }

                const char *name = stackTrace[i].sourceFileName().c_str();
                ASSERT(name && *name);
                if (name) {
                    const char *pc = name + bsl::strlen(name);
                    while (pc > name && '/' != pc[-1]) {
                        --pc;
                    }

                    LOOP2_ASSERT(i, pc, !bsl::strcmp(pc,
                                 "baesu_stacktraceresolverimpl_dladdr.t.cpp"));
                }
            }
#endif

#undef  SM
#define SM(nm, match) {                                             \
                const char *name = nm.c_str();                      \
                LOOP2_ASSERT(name, match, safeStrStr(name, match)); \
            }

            SM(stackTrace[0].mangledSymbolName(), "funcGlobalOne");
            SM(stackTrace[1].mangledSymbolName(), "funcStaticOne");
            SM(stackTrace[2].mangledSymbolName(), "funcStaticInlineOne");
            SM(stackTrace[3].mangledSymbolName(), "qsort");
            SM(stackTrace[4].mangledSymbolName(), "resolve");

            SM(stackTrace[0].symbolName(), "funcGlobalOne");
            SM(stackTrace[1].symbolName(), "funcStaticOne");
            SM(stackTrace[2].symbolName(), "funcStaticInlineOne");
            SM(stackTrace[3].symbolName(), "qsort");
            SM(stackTrace[4].symbolName(), "resolve");

            if (demangle) {
#undef  SM
#define SM(i, match) {                                                     \
                    const char *name = stackTrace[i].symbolName().c_str(); \
                    LOOP_ASSERT(name, safeCmp(name, match));               \
                }

                SM(0, "funcGlobalOne(int)");
//              SM(1, "funcStaticOne(int)");    // Darwin doesn't demangle
                                                // statics
//              SM(2, "funcStaticInlineOne(int)");    // Darwin doesn't
                                                      // demangle statics
                SM(3, "qsort");
                const char *resName = "BloombergLP::"
                                      "baesu_StackTraceResolverImpl"
                                      "<BloombergLP::"
                                      "baesu_ObjectFileFormat::Dladdr>::"
                                      "resolve(";
                int resNameLen = (int) bsl::strlen(resName);
                const char *name4 = stackTrace[4].symbolName().c_str();
                LOOP2_ASSERT(name4, resName,
                                          safeCmp(name4, resName, resNameLen));
                break;
            }

#if defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CMP_GNU)
            // Sun CC, won't demangle
            break;
#endif
        }

        ASSERT(0 == defaultAllocator.numAllocations());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        baesu_StackTrace stackTrace;
        stackTrace.resize(1);

        UintPtr funcUintPtr = (UintPtr) &funcGlobalOne;
        stackTrace[0].setAddress((void *) (funcUintPtr + 20));

        Obj::resolve(&stackTrace, false);

        ASSERT(1 == stackTrace.length());
        ASSERT(bsl::string::npos != stackTrace[0].symbolName().find(
                                                            "funcGlobalOne"));
        ASSERT(bsl::string::npos != stackTrace[0].libraryFileName().find(
                                    "baesu_stacktraceresolverimpl_dladdr.t"));
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
