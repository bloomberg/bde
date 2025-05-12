// balst_resolverimpl_elf.t.cpp                                       -*-C++-*-
#include <balst_resolverimpl_elf.h>

#include <balst_objectfileformat.h>
#include <balst_stacktrace.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>

#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_ELF

#include <unistd.h>
#include <sys/stat.h>    // chmod

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
// [ 1] resolve
// [ 2] garbage test
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

bsls::AtomicInt testStatus(0);

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define TAB cout << '\t';

#define PP(X) (cout << #X " = " << (X) << endl, false) // Print identifier and
                                         // value, return false, as expression.

// ============================================================================
//                    GLOBAL HELPER #DEFINES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                 GLOBAL HELPER TYPES & CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef balst::ResolverImpl<balst::ObjectFileFormat::Elf> Obj;
typedef balst::StackTraceFrame                            Frame;
typedef bsls::Types::UintPtr                              UintPtr;
typedef bsls::Types::Int64                                Int64;
typedef bsls::Types::Uint64                               Uint64;

#if defined(BSLS_PLATFORM_OS_LINUX)
enum { e_IS_LINUX = 1 };
#else
enum { e_IS_LINUX = 0 };
#endif
#if defined(BSLS_PLATFORM_CMP_CLANG)
enum { e_IS_CLANG = 1 };
#else
enum { e_IS_CLANG = 0 };
#endif
#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_DWARF) &&                         \
                                                 !defined(BDE_BUILD_TARGET_OPT)
enum { e_IS_DWARF = 1 };
#else
enum { e_IS_DWARF = 0 };
#endif

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

/// Return a pointer to the new delete allocator singleton.
bslma::Allocator *nda()
{
    // We use the new delete allocator here, because if we want to track down
    // accidental uses of the default allocator, we'll want to put a breakpoint
    // on `bslma::TestAllocator::allocate`.

    return &bslma::NewDeleteAllocator::singleton();
}

void checkExpendibleExecutableName(int topCaseIndex, char **argv)
{
    int test = bsl::atoi(argv[1]);

    ASSERTV(test, topCaseIndex, topCaseIndex < test);
    ASSERTV(test, topCaseIndex, test <= 2 * topCaseIndex);
    ASSERTV(argv[0], bsl::strstr(argv[0], "_expendible_"));
    ASSERTV(argv[0], test, bsl::strstr(argv[0], argv[1]));
}

/// Copy the file `fromFileName` to a new file `toFileName`.
void copyExecutable(const char *toFileName,
                    const char *fromFileName)
{
    using namespace bsl;
    typedef bdls::FilesystemUtil   Util;
    typedef bsl::char_traits<char> Traits;

    // `FilesystemUtil` functions use the default allocator on Windows.

    bslma::DefaultAllocatorGuard guard(u::nda());

    (void) Util::remove(toFileName);
    const Util::Offset fileSize = Util::getFileSize(fromFileName);

    {
        ofstream toStream(  toFileName,   ios_base::out | ios_base::binary);
        ifstream fromStream(fromFileName, ios_base::in  | ios_base::binary);

        ASSERT(toStream.  good());
        ASSERT(fromStream.good());

        streambuf *toSb   = toStream.  rdbuf();
        streambuf *fromSb = fromStream.rdbuf();

        copy(istreambuf_iterator<char>(fromSb),
             istreambuf_iterator<char>(),
             ostreambuf_iterator<char>(toSb));

        // Since we operated directly on the `streambuf`s, the `fstream`s were
        // uninformed about how it went.  Query the `streambuf`s directly to
        // verify that it went OK.

        const Traits::pos_type toOff = toSb->pubseekoff(0, ios_base::cur);
        ASSERTV(fileSize, toOff, fileSize == toOff);
        ASSERT(Traits::eof() == fromSb->sbumpc());
    }    // close the streams

    ASSERT(Util::getFileSize(toFileName) == fileSize);

#ifdef BSLS_PLATFORM_OS_UNIX
    int rc = ::chmod(toFileName, 0777);
    ASSERT(0 == rc);
#endif
}

void doEraseTest(int argc, char **argv)
{
    bsl::string cmd(argv[0], u::nda());
    cmd += "_expendible_";
    cmd += argv[1];            // `test` in text form
    cmd += ".t";

    // `cmd` is now the child exec name

    u::copyExecutable(cmd.c_str(), argv[0]);

    // command: run the test case

    cmd += ' ';
    cmd += argv[1];
    cmd += " --erase";
    for (int ii = 2; ii < argc; ++ii) {
        cmd += ' ';
        cmd += argv[ii];
    }
    int rc = ::system(cmd.c_str());
    ASSERT(0 == rc);
    if (0 < rc) {
        testStatus += rc;
    }
}

}  // close namespace u
}  // close unnamed namespace

template <class TYPE>
static TYPE abs(TYPE num)
{
    return num >= 0 ? num : -num;
}

const Int64 mask32 = 0xffffffffLL;

/// Target function to be resolved.  Never called.  Do some arbitary
/// arithmetic so there will be some length of code in this routine.
static
int funcStaticOne(int k)
{
    Uint64 ret = L_;
    unsigned i = k;
    for (int j = 0; j < 2; ++j) {
        i = i * i * i;
    }

    ret |= static_cast<Int64>(5 * i) << 14;
    return static_cast<int>(ret & mask32);
}

/// Target function to be resolved.  Never called.  Do some arbitary
/// arithmetic so there will be some length of code in this routine.
int funcGlobalOne(int k)
{
    Uint64 ret = L_;
    unsigned i = k;
    for (unsigned j = 0; j < 2; ++j) {
        i = i * i * i * i;
    }

    ret |= static_cast<Int64>(6 * i) << 14;
    return static_cast<int>(ret & mask32);
}

/// Target function to be resolved.  Never called.  Do some arbitary
/// arithmetic so there will be some length of code in this routine.
static inline
int funcStaticInlineOne(int k)
{
    Uint64 ret = L_;
    unsigned i = k;
    for (unsigned j = 0; j < 5; ++j) {
        i = 75 * i * i + i / (j + 1);
    }

    ret |= (5 * i) << 14;
    return static_cast<int>(ret & mask32);
}

struct S {
    static
    int funcGlobalMethod(int k);
};

int S::funcGlobalMethod(int k)
{
    Uint64 ret = L_;
    unsigned i = k;
    for (unsigned j = 0; j < 5; ++j) {
        i = 75 * i * i + i / (j + 1);
    }

    ret |= (5 * i) << 14;
    return static_cast<int>(ret & mask32);
}

/// Given a function pointer stored in a `UintPtr`, add an offset to the
/// pointer and return it as a `const void *`.
static
const void *addFixedOffset(bsls::Types::UintPtr funcAddress)
{
    const char *ptr = (const char *) funcAddress;

    return ptr + 4;
}

/// Do `!strcmp`, returning `true` if the specified `a` and `b` point to
/// identical strings.  Return `false` if either one is null.
static
bool safeCmp(const char *a, const char *b, int len = -1)
{
    if (!a || !b) {
        return false;                                                 // RETURN
    }

    return -1 == len ? !bsl::strcmp(a, b) : !bsl::strncmp(a, b, len);

}

/// Return `true` if the specified `target` points to a string contained in
/// the specified `string`.  Return `false` if `string` or `target` is null.
static
bool safeStrStr(const char *string, const char *target)
{
    if (!string || !target) {
        return false;                                                 // RETURN
    }

    return bsl::strstr(string, target);
}

/// null guard -- substitute "(null)" for the specified `str` if `str` is
/// null.
static
const char *ng(const char *str)
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
    UintPtr lowBits = static_cast<UintPtr>(next >> 32);
    bigRandSeed = randA * next + randC;

    return static_cast<UintPtr>(bigRandSeed) ^ lowBits;
}

void stuffRandomAddresses(balst::StackTrace *stackTrace)
{
    const UintPtr delta = (UintPtr) 1 << ((sizeof(UintPtr) - 1) * 8);
    const int vecLength = 3 * 256 * 3 + 2048;

    stackTrace->resize(vecLength);

    int vIndex = 0;
    for (int i = -1; i <= 1; ++i) {
        UintPtr u = i;
        for (int j = 0; j < 256; ++j, u += delta) {
            (*stackTrace)[vIndex++].setAddress((void *) u);
            (*stackTrace)[vIndex++].setAddress((void *) ~u);
            (*stackTrace)[vIndex++].setAddress((void *) -u);
        }
    }

    ASSERT(vecLength - 2048 == vIndex);

    while (vIndex < vecLength) {
        (*stackTrace)[vIndex++].setAddress((void *) bigRand());
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
    int veryVeryVerbose = argc > 4;

    enum { k_TOP_TEST_CASE_INDEX = 2 };

    const bool eraseExecutable = verbose && !bsl::strcmp("--erase", argv[2]);
    if (eraseExecutable) {
        // `FilesystemUtil` functions uses the default allocator on Windows.

        bslma::DefaultAllocatorGuard guard(u::nda());

        u::checkExpendibleExecutableName(k_TOP_TEST_CASE_INDEX, argv);

        int rc = bdls::FilesystemUtil::remove(argv[0]);
        ASSERT(0 == rc);
        ASSERT(!bdls::FilesystemUtil::exists(argv[0]));

        test        -= k_TOP_TEST_CASE_INDEX;

        verbose     =  veryVerbose;
        veryVerbose =  veryVeryVerbose;
    }

    cout << "TEST " << __FILE__ << " CASE " << test <<
                                   (eraseExecutable ? " --erase" : "") << endl;

    if (verbose && eraseExecutable) {
        cout << argv[0] << " erased\n";
    }

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case k_TOP_TEST_CASE_INDEX: {
        // --------------------------------------------------------------------
        // GARBAGE TEST
        //
        // Concerns: That the resolver won't segfault given garbage data.
        //
        // Plan: seed a long stackTrace of StackTraceFrames with garbase
        // addresses, then resolve the stackTrace, seeing if it segfaults or
        // returns an error.  Then stream out the frames to see if any problems
        // are encountered streaming out.
        // --------------------------------------------------------------------

        if (verbose) cout << "Garbage Test\n"
                             "============\n";

        balst::StackTrace stackTrace;
        stuffRandomAddresses(&stackTrace);

        ASSERT(0 == Obj::resolve(&stackTrace,
                                 true));

        ASSERT(0 == defaultAllocator.numAllocations());

        // Note that the stringstream below will use temporaries using the
        // default allocator whether it's constructed with another allocator
        // or not.

        bslma::DefaultAllocatorGuard guard(u::nda());

        for (int vecIndex = 0; vecIndex < (int) stackTrace.length();
                                                             vecIndex += 128) {
            bsl::ostringstream ss;

            int vecIndexLim = bsl::min(vecIndex + 127,
                                       (int) stackTrace.length());
            for (int j = vecIndex; j < vecIndexLim; ++j) {
                ss << stackTrace[j] << endl;
            }
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // balst::ResolverImp<Elf> BREATHING TEST
        //
        // Concerns: Exercise balst::StackTrace basic functionality.
        //
        // Plan: Call `printStackTrace()` to print a stack trace.
        // --------------------------------------------------------------------

        if (verbose) cout << "balst::ResolverImpl<Elf> breathing test\n"
                             "=======================================\n";

        // On some platforms, taking a ptr to a function in a shared library
        // gives you a ptr to a thunk that will load the lib and jump to the
        // function.  On gcc-13 C++20, taking a ptr to a global in another
        // module apparently does that same thing.  We'll leave the testing of
        // symbols in shared libraries and other components to
        // 'balst_stacktraceprintutil.t.cpp and balst_stacktraceutil.t.cpp.

        typedef bsls::Types::UintPtr UintPtr;

        Int64 lineResults[5];
        {
            const Int64 lineResultsMask = (1 << 14) - 1;
            lineResults[0] = funcGlobalOne(3)       & lineResultsMask;
            lineResults[1] = funcStaticOne(3)       & lineResultsMask;
            lineResults[2] = funcStaticInlineOne(3) & lineResultsMask;
            lineResults[3] = S::funcGlobalMethod(3) & lineResultsMask;
            lineResults[4] = Obj::test()            & lineResultsMask;
        }

        for (int demangle = 0; demangle < 2; ++demangle) {
            if (verbose) cout << "Trace with" << (demangle ? "" : "out") <<
                                                              " demangling.\n";

            balst::StackTrace stackTrace;
            stackTrace.resize(5);
            stackTrace[0].setAddress(addFixedOffset((UintPtr) &funcGlobalOne));
            stackTrace[1].setAddress(addFixedOffset((UintPtr) &funcStaticOne));

            // The optizer is just UNBELIEVABLY clever.  If you declare a
            // routine inline, it is VERY aggressive about figuring out a way
            // to inline it, even if you call it through a pointer.
            // `Obj::testFunc` is an inline routine, but we force it out of
            // line by taking a function ptr to it.

            UintPtr testFuncPtr = (UintPtr) &funcStaticInlineOne;

            // If we now just called through it, the optimizer would inline the
            // call.  So let's juggle (without actually changing it) a bit in a
            // way the optizer can't possibly figure out:

            testFuncPtr = bslim::TestUtil::makeFunctionCallNonInline(
                                                                  testFuncPtr);

            int result = (* (int (*)(int)) testFuncPtr)(100);
            ASSERT(result > 10000);
            stackTrace[2].setAddress(addFixedOffset(testFuncPtr));

#if 0
            // Testing `&qsort` doesn't work.  The similar test in
            // balst_stacktraceutil.t.cpp works.  I think what's happening is
            // &qsort doesn't properly point to `qsort`, it points to a thunk
            // that dynamically loads qsort and then calls it.  So when the
            // resolver tries to resolve the ptr to the thunk, it can't.

            {
                // make sure the component contain `qsort` is loaded

                int ints[] = { 0, 1 };
                bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);
            }
            stackTrace[3].setAddress(addFixedOffset((UintPtr) &bsl::qsort));
#endif

#if 0
            // Testing '&Obj::resolve' doesn't work on gcc-13 C++20, apparently
            // applying '&' to a global function in another source file yields
            // a pointer to a 'thunk' rather than directly to the function
            // itself, so we have replaced this test with a pointer to
            // 'S::funcGlobalMethod'.

            stackTrace[3].setAddress(addFixedOffset((UintPtr) &Obj::resolve));
#endif

            stackTrace[3].setAddress(addFixedOffset(
                                              (UintPtr) &S::funcGlobalMethod));
            stackTrace[4].setAddress(addFixedOffset((UintPtr) &Obj::test));

            for (int i = 0; i < (int) stackTrace.length(); ++i) {
                if (veryVerbose) {
                    cout << "stackTrace[" << i << "].address() = " <<
                                               stackTrace[i].address() << endl;
                }

                ASSERTV(i, stackTrace[i].address());
#undef  IS_UNKNOWN
#define IS_UNKNOWN(name) ASSERTV(i, !stackTrace[i].is ## name ## Known());
                IS_UNKNOWN(LibraryFileName);
                IS_UNKNOWN(LineNumber);
                IS_UNKNOWN(MangledSymbolName);
                IS_UNKNOWN(OffsetFromSymbol);
                IS_UNKNOWN(SourceFileName);
                IS_UNKNOWN(SymbolName);
#undef IS_UNKNOWN
            }

            Obj::resolve(&stackTrace, demangle);

            if (veryVerbose) {
                cout << "Pass " << (int) demangle << endl;

                for (int i = 0; i < stackTrace.length(); ++i) {
                    cout << '(' << i << "): " << stackTrace[i] << endl;
                }
            }

            for (int i = 0; i < stackTrace.length(); ++i) {
#undef  IS_KNOWN
#define IS_KNOWN(name) ASSERTV(i, stackTrace[i],                              \
                                          stackTrace[i].is ## name ## Known());
                IS_KNOWN(Address);
                IS_KNOWN(LibraryFileName);
                IS_KNOWN(MangledSymbolName);
                IS_KNOWN(OffsetFromSymbol);
                if (e_IS_DWARF || 1 == i || 2 == i) {
                    IS_KNOWN(SourceFileName);    // static symbols only
                }
                IS_KNOWN(SymbolName);
            }
#undef IS_KNOWN

            const char *libName = stackTrace[0].libraryFileName().c_str();
            bsl::string progName(u::nda());
            bdls::PathUtil::getBasename(&progName, argv[0]);
            const char *thisLib = progName.c_str();
#undef  GOOD_LIBNAME
#define GOOD_LIBNAME(func, exp, match) \
            ASSERTV(#func, exp, ng(match), func(ng(exp), match));

            GOOD_LIBNAME(safeStrStr,
                             stackTrace[0].libraryFileName().c_str(), thisLib);
            GOOD_LIBNAME(safeCmp,
                             stackTrace[1].libraryFileName().c_str(), libName);
            GOOD_LIBNAME(safeCmp,
                             stackTrace[2].libraryFileName().c_str(), libName);
            GOOD_LIBNAME(safeCmp,
                             stackTrace[3].libraryFileName().c_str(), libName);
#undef  GOOD_LIBNAME

            for (int i = 0; i < stackTrace.length(); ++i) {
                const char *sym = stackTrace[i].symbolName().c_str();

                const char *name = stackTrace[i].sourceFileName().c_str();
                ASSERTV(i, name, !e_IS_DWARF || '/' == *name);
                ASSERTV(i, name, !e_IS_DWARF ||
                     bdls::FilesystemUtil::exists(name) ||
                                                  (eraseExecutable && 0 == i));

                const char *pc = name + bsl::strlen(name);
                while (pc > name && '/' != pc[-1]) {
                    --pc;
                }

                int line = stackTrace[i].lineNumber();
                ASSERTV(line, 0 < line || -1 == line);
                ASSERTV(line, !e_IS_DWARF || 0 < line);

                if (e_IS_DWARF) {
                    if (3 == i) {
                        ASSERTV(line, 50 < line);
                        ASSERTV(line, line < 300);
                    }
                    else {
                        const int fudge = 4 == i ? 4 : 2;
                        ASSERTV(i, lineResults[i], line,
                                          abs(lineResults[i] - line) <= fudge);
                    }
                }

                if (veryVerbose) cout << i << ", " << sym << ", " << name <<
                                                           ':' << line << endl;

                // frame[1] and frame[2] point to a statics, the ELF resolver
                // should have found this source file name, even without DWARF.

                if (e_IS_DWARF || 1 == i || 2 == i) {
                    ASSERTV(i, stackTrace[i].isSourceFileNameKnown());

                    ASSERTV(i, pc, !bsl::strcmp(
                         pc,
                         4 == i ? "balst_resolverimpl_elf.h"
                                : "balst_resolverimpl_elf.t.cpp"));
                }
            }

#undef  SM
#define SM(ii, match) {                                                       \
                const char *msn = stackTrace[ii].mangledSymbolName().c_str(); \
                ASSERTV(msn, match, safeStrStr(msn, match));                  \
                const char *sn = stackTrace[ii].symbolName().c_str();         \
                ASSERTV(sn, match, safeStrStr(sn, match));                    \
            }

            SM(0, "funcGlobalOne");
            SM(1, "funcStaticOne");
            SM(2, "funcStaticInlineOne");
            SM(3, "funcGlobalMethod");
            SM(4, "test");
#undef  SM

            // Note that we skip after the first space in `name`, if any is
            // found, because on Solaris CC `name` begins with the return type
            // declaration.  No other spaces are expected.

            if (demangle) {
#define SM(ii, match) {                                                       \
                    const char *name = stackTrace[ii].symbolName().c_str();   \
                    const char *sp = bsl::strchr(name, ' ');                  \
                    if (sp) {                                                 \
                        name = sp + 1;                                        \
                    }                                                         \
                    ASSERTV(name, match, safeCmp(name, match));               \
                }

                SM(0, "funcGlobalOne(int)");
                SM(3, "S::funcGlobalMethod(int)");
                SM(4, "BloombergLP::balst::ResolverImpl"
                      "<BloombergLP::balst::ObjectFileFormat::Elf>::"
                      "test()");
                if (!e_IS_LINUX || !e_IS_CLANG) {
                    // The linux clang demangler has a bug where it fails on
                    // file-scope statics.

                    SM(1, "funcStaticOne(int)");
                    SM(2, "funcStaticInlineOne(int)");
                }
#undef  SM
            }
        }
      } break;
      default: {
        BSLS_ASSERT_OPT(!eraseExecutable);

        ASSERTV(test, k_TOP_TEST_CASE_INDEX < test);

        if (test <= 2 * k_TOP_TEST_CASE_INDEX) {
            u::doEraseTest(argc, argv);
        }
        else {
            cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
            testStatus = -1;
        }
      }
    }

    ASSERTV(defaultAllocator.numAllocations(),
                                       0 == defaultAllocator.numAllocations());

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
