// balst_stacktraceresolverimpl_dladdr.t.cpp                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_dladdr.h>

#include <balst_stacktrace.h>

#include <balst_objectfileformat.h>

#include <bdlt_dateutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_types.h>

#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_DLADDR

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
// ----------------------------------------------------------------------------
// CLASS METHoDS
// [ 2] static int resolve(balst::StackTrace *, bool);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] CONCERN: Resolving Invalid Input
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

typedef balst::StackTraceResolverImpl<balst::ObjectFileFormat::Dladdr> Obj;
typedef balst::StackTraceFrame                                        Frame;
typedef bsls::Types::UintPtr                                         UintPtr;

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

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
const void *addFixedOffset(bsls::Types::UintPtr funcAddress)
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

void stuffRandomAddresses(balst::StackTrace *stackTrace)
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

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // INVALID INPUT TEST
        //
        // Concerns:
        //: 1 That, given invalid data, the stack trace facility won't
        //:   segfault.
        //:
        //: 2 That, given invalid data, the stack trace facility won't fail an
        //:   assert.
        //:
        //: 3 That, given invalid data, the stack trace facility won't return a
        //:   non-zero status.
        //
        // Plan:
        //: 1 Seed a long stackTrace of StackTraceFrames with a combination of
        //:   random and maliciously chosen garbase addresses.
        //:
        //: 2 Resolve it.
        //:
        //: 3 Observe if it segfaults or fails any asserts.
        //:
        //: 4 Observe if it returns a non-zero return code.
        //:
        //: 5 Stream all the frames to a stringstream to see if the stream
        //:   operator causes any segfaults of failed asserts.
        // --------------------------------------------------------------------

        if (verbose) cout << "Invalid Input Test\n"
                             "==================\n";

        bslma::TestAllocator ta;

        balst::StackTrace stackTrace(&ta);
        stuffRandomAddresses(&stackTrace);

        int rc = Obj::resolve(&stackTrace, true);
        ASSERT(0 == rc);

        ASSERT(0 == defaultAllocator.numAllocations());

        // Note that the stringstream below will use temporaries using the
        // default allocator whether it's constructed with another allocator
        // or not.

        for (int vecIndex = 0; vecIndex < (int) stackTrace.length();
                                                             vecIndex += 128) {
            bsl::ostringstream oss(&ta);

            int vecIndexLim = bsl::min(vecIndex + 127,
                                       (int) stackTrace.length());
            for (int j = vecIndex; j < vecIndexLim; ++j) {
                oss << stackTrace[j] << endl;
            }
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: RESOLVE
        //
        // Concerns:
        //: 1 That 'resolve' can corectly resolve the address of alobal symbol
        //:   in this file.
        //:
        //: 2 That 'resolve' can corectly resolve the address of a static
        //:   non-inline function in this file.
        //:
        //: 3 That 'resolve' can corectly resolve the address of static inline
        //:   function in this file.
        //:
        //: 4 That 'resolve' can corectly resolve the address of a function in
        //:   a shared library.
        //:
        //: 5 That 'resolve' can corectly resolve the address of global
        //:   function in a different file.
        //:
        //: 6 That 'resolve' can corectly resolve the address of global inline
        //:   function in another file.
        //
        // Plan:
        //: 1 Populate a 'balst::StackTrace' object with several code pointers
        //:   of all the different types outlined in 'Concerns".
        //:
        //: 2 Verify all fields other than the 'address' fields are 'unknown'.
        //:
        //: 3 Call 'resolve' on the stack trace object
        //:
        //: 4 Verify some of the expected fields are known
        //:
        //: 5 Verify the 'libraryFileName' fields are as expected
        //:
        //: 6 Verify the 'sourceFileName' fields are as expected
        //:
        //: 7 Verify the 'mangledSymbolName' and 'symbolName' fields are as
        //:   expected.
        //:
        //: 8 For the non-static symbols, verify they demangled properly.
        //
        // TESTING:
        //    static int resolve(balst::StackTrace *, bool);
        // --------------------------------------------------------------------

        if (verbose) cout << "SUCCESSFUL RESOLVE TEST\n"
                             "=======================\n";

        // There seems to be a problem with taking a pointer to an function in
        // a shared library.  We'll leave the testing of symbols in shared
        // libraries to 'balst_stacktraceutil.t.cpp.

        typedef bsls::Types::UintPtr UintPtr;

        for (int ti = 0; ti < 2; ++ti) {
            const bool demangle = ti;
            balst::StackTrace stackTrace;
            stackTrace.resize(6);

            if (veryVerbose) {
                cout << "Pass " << (int) demangle << endl;
            }

            // Global symbol in this file

            stackTrace[0].setAddress(addFixedOffset((UintPtr) &funcGlobalOne));

            // Static out of line routine in this file

            stackTrace[1].setAddress(addFixedOffset((UintPtr) &funcStaticOne));

            // Static inline in this file

            UintPtr testFuncPtr = (UintPtr) &funcStaticInlineOne;

            // If we now just called through it, the optimizer would inline the
            // call.  So let's juggle (without actually changing it) a bit in a
            // way the optizer can't possibly figure out:

            testFuncPtr = foilOptimizer(testFuncPtr);

            int result = (* (int (*)(int)) testFuncPtr)(100);
            ASSERT(result > 10000);
            stackTrace[2].setAddress(addFixedOffset(testFuncPtr));

            // Symbol in a shared library

            {
                // make sure the component contain 'qsort' is loaded

                int ints[] = { 0, 1 };
                bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);
            }
            stackTrace[3].setAddress(addFixedOffset((UintPtr) &qsort));

            // Global symbol in a different file

            stackTrace[4].setAddress(addFixedOffset((UintPtr) &Obj::resolve));

            // Global inline in another file

            stackTrace[5].setAddress(addFixedOffset((UintPtr)
                                              &bdlt::DateUtil::nextDayOfWeek));

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

            int rc = Obj::resolve(&stackTrace, demangle);
            ASSERT(0 == rc);

            if (veryVerbose) {
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
            const char *thisLib = "balst_stacktraceresolverimpl_dladdr.t";
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
            GOOD_LIBNAME(safeStrStr,
                             stackTrace[5].libraryFileName().c_str(), thisLib);
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
                                 "balst_stacktraceresolverimpl_dladdr.t.cpp"));
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
            SM(stackTrace[5].mangledSymbolName(), "nextDayOfWeek");

            SM(stackTrace[0].symbolName(), "funcGlobalOne");
            SM(stackTrace[1].symbolName(), "funcStaticOne");
            SM(stackTrace[2].symbolName(), "funcStaticInlineOne");
            SM(stackTrace[3].symbolName(), "qsort");
            SM(stackTrace[4].symbolName(), "resolve");
            SM(stackTrace[5].symbolName(), "nextDayOfWeek");

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
                                      "balst::StackTraceResolverImpl"
                                      "<BloombergLP::"
                                      "balst::ObjectFileFormat::Dladdr>::"
                                      "resolve(";
                int resNameLen = (int) bsl::strlen(resName);
                const char *name4 = stackTrace[4].symbolName().c_str();
                LOOP2_ASSERT(name4, resName,
                                          safeCmp(name4, resName, resNameLen));
                resName = "BloombergLP::bdlt::DateUtil::nextDayOfWeek(";
                resNameLen = (int) bsl::strlen(resName);
                const char *name5 = stackTrace[5].symbolName().c_str();
                LOOP2_ASSERT(name5, resName,
                                          safeCmp(name5, resName, resNameLen));
            }
        }

        ASSERT(0 == defaultAllocator.numAllocations());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise basic functionality.
        //
        // Plan:
        //: 1 Load a single function pointer into a stack trace object.
        //: 2 Resolve it.
        //: 3 Check some of the symbol information for correctness.
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        balst::StackTrace stackTrace;
        stackTrace.resize(1);

        UintPtr funcUintPtr = (UintPtr) &funcGlobalOne;
        stackTrace[0].setAddress((void *) (funcUintPtr + 20));

        Obj::resolve(&stackTrace, false);

        ASSERT(1 == stackTrace.length());
        ASSERT(bsl::string::npos != stackTrace[0].symbolName().find(
                                                            "funcGlobalOne"));
        ASSERT(bsl::string::npos != stackTrace[0].libraryFileName().find(
                                    "balst_stacktraceresolverimpl_dladdr.t"));
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
