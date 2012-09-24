// baesu_stackaddressutil.t.cpp                                       -*-C++-*-
#include <baesu_stackaddressutil.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
// 'getStackAddresses' will not be able to trace through our stack frames if
// we're optimized on Windows

# pragma optimize("", off)

#endif

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
// TEST PLAN
//-----------------------------------------------------------------------------
// [ 3]  int getStackAddresses(void **buffer, int maxFrames);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 2] getStackAddresses(0, 0)
// [-1] Speed benchmark of getStackAddresses

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
//-----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
// GLOBAL HELPER VARIABLES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_Types::UintPtr UintPtr;
int verbose;
int veryVerbose;

// 'lamePlatform' -- on lame platforms where StackAddressUtil doesn't work or
// executables are stripped, disable some of the tests.
#if defined(BDE_BUILD_TARGET_OPT) && defined(BSLS_PLATFORM_OS_WINDOWS)
const bool lamePlatform = true;
#else
const bool lamePlatform = false;
#endif

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static bsl::string myHex(UintPtr up)
{
    bsl::stringstream ss;
    ss.setf(bsl::ios_base::hex, bsl::ios_base::basefield);
    ss.setf(bsl::ios_base::showbase);
    ss << up;
    return ss.str();
}

                                // ------
                                // CASE 4
                                // ------

namespace CASE_FOUR {

// First, we define 'AddressEntry', which will contain a pointer to the
// beginning of a function and an index corresponding to the function.  The '<'
// operator is defined so that a vector of address entries can be sorted in the
// order of the function addresses.  The address entries will be populated so
// that the entry containing '&funcN' when 'N' is an integer will have an index
// of 'N'.

struct AddressEntry {
    void *d_funcAddress;
    int   d_index;

    // CREATORS
    AddressEntry(void *funcAddress, int index)
    : d_funcAddress(funcAddress)
    , d_index(index)
    {}

    bool operator<(const AddressEntry rhs) const
    {
        return d_funcAddress < rhs.d_funcAddress;
    }
};

// Then, we define 'entries', a vector of address entries.  This will be
// populated such that a given entry will contain function address '&funcN' and
// index 'N'.  The elements will be sorted according to function address.

bsl::vector<AddressEntry> entries;

// Next, we define 'findIndex':

static int findIndex(const void *retAddress)
    // Given the specfied 'retAddress' which should point to code within one of
    // the functions described by the sorted vector 'entries', identify the
    // index of the function containing that return address.
{
    unsigned u = 0;
    while (u < entries.size()-1 && retAddress >= entries[u+1].d_funcAddress) {
        ++u;
    }
    ASSERT(u < entries.size());
    ASSERT(retAddress >= entries[u].d_funcAddress);

    int ret = entries[u].d_index;

    if (veryVerbose) {
        P_(retAddress) P_(entries[u].d_funcAddress) P(ret);
    }

    return ret;
}

// Have a volatile global in calculations to discourange optimizers from
// inlining.

volatile int volatileGlobal = 2;

// Then, we define a chain of functions that will call each other and do some
// random calculation to generate some code, and eventually call 'func1' which
// will call 'getAddresses' and verify that the addresses returned correspond
// to the functions we expect them to.

static int func1();
static int func2()
{
    return volatileGlobal * 2 * func1();
}
static int func3()
{
    return volatileGlobal * 3 * func2();
}
static int func4()
{
    return volatileGlobal * 4 * func3();
}
static int func5()
{
    return volatileGlobal * 5 * func4();
}
static int func6()
{
    return volatileGlobal * 6 * func5();
}

// Next, we define the macro FUNC_ADDRESS, which will take as an arg a
// '&<function name>' and return a pointer to the actual beginning of the
// function's code, which is a non-trivial and platform-dependent exercise.
// (Note: this doesn't work on Windows for global routines).

#if   defined(BSLS_PLATFORM_OS_HPUX)
# define FUNC_ADDRESS(p) (((void **) (void *) (p))[sizeof(void *) == 4])
#elif defined(BSLS_PLATFORM_OS_AIX)
# define FUNC_ADDRESS(p) (((void **) (void *) (p))[0])
#else
# define FUNC_ADDRESS(p) ((void *) (p))
#endif

// Then, we define 'func1', which is the last of the chain of our functions
// that is called, which will do most of our work.

int func1()
    // Call 'getAddresses' and verify that the returned set of addresses
    // matches our expectations.
{
    // Next, we populate and sort the 'entries' table, a sorted array of
    // 'AddressEntry' objects that will allow 'findIndex' to look up within
    // which function a given return address can be found.

    entries.clear();
    entries.push_back(AddressEntry(0, 0));
    entries.push_back(AddressEntry(FUNC_ADDRESS(&func1), 1));
    entries.push_back(AddressEntry(FUNC_ADDRESS(&func2), 2));
    entries.push_back(AddressEntry(FUNC_ADDRESS(&func3), 3));
    entries.push_back(AddressEntry(FUNC_ADDRESS(&func4), 4));
    entries.push_back(AddressEntry(FUNC_ADDRESS(&func5), 5));
    entries.push_back(AddressEntry(FUNC_ADDRESS(&func6), 6));
    bsl::sort(entries.begin(), entries.end());

    // Then, we obtain the stack addresses with 'getStackAddresses'.

    enum { BUFFER_LENGTH = 100 };
    void *buffer[BUFFER_LENGTH];
    bsl::memset(buffer, 0, sizeof(buffer));
    int numAddresses = baesu_StackAddressUtil::getStackAddresses(
                                                                buffer,
                                                                BUFFER_LENGTH);
    ASSERT(numAddresses >= (int) entries.size());
    ASSERT(numAddresses < BUFFER_LENGTH);
    ASSERT(0 != buffer[numAddresses-1]);
    ASSERT(0 == buffer[numAddresses]);

    // Finally, we go through several of the first addresses returned in
    // 'buffer' and verify that each address corresponds to the routine we
    // expect it to.

    // Note that on some, but not all, platforms there is an extra 'narcissic'
    // frame describing 'getStackAddresses' itself at the beginning of
    // 'buffer'.  By starting our iteration through 'buffer' at
    // 'BAESU_IGNORE_FRAMES', we guarantee that the first address we examine
    // will be in 'func1' on all platforms.

    int funcIdx  = 1;
    int stackIdx = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES;
    for (; funcIdx < (int) entries.size(); ++funcIdx, ++stackIdx) {
        ASSERT(stackIdx < numAddresses);
        ASSERT(funcIdx == findIndex(buffer[stackIdx]));
    }

    if (testStatus || veryVerbose) {
        Q(Entries:);
        for (unsigned u = 0; u < entries.size(); ++u) {
            P_(u); P_((void *) entries[u].d_funcAddress);
            P(entries[u].d_index);
        }

        Q(Stack:);
        for (int i = 0; i < numAddresses; ++i) {
            P_(i); P(buffer[i]);
        }
    }

    return 3;    // random value
}

#undef FUNC_ADDRESS

}  // close namespace CASE_FOUR

                                // ------
                                // CASE 3
                                // ------

namespace CASE_THREE {

struct AddressEntry {
    UintPtr d_returnAddress;
    int     d_traceIndex;
};

bool operator<(const AddressEntry lhs, const AddressEntry rhs)
{
    return lhs.d_returnAddress < rhs.d_returnAddress;
}

static int findIndex(AddressEntry *entries, int numAddresses, UintPtr funcP)
    // 'buffer' is a sorted list of 'numAddresses' addresses, whose highest
    // entry is the max possible value for a pointer.  Search the buffer and
    // return the index of the first entry at or before 'p'.
{
    int i = 0;
    while (i < numAddresses && funcP > entries[i].d_returnAddress) {
        ++i;
    }
    ASSERT(i < numAddresses);
    UintPtr retP = entries[i].d_returnAddress;

    ASSERT(retP > funcP);
    int ret = entries[i].d_traceIndex;

    if (veryVerbose) {
        P_(myHex(funcP)) P_(myHex(retP)) P(ret);
    }

    return ret;
}

#define CASE3_FUNC(nMinus1, n)                       \
    int func ## n()                                  \
    {                                                \
        int i = (n);                                 \
                                                     \
        i += func ## nMinus1();                      \
                                                     \
        return i * (n) + i / bsl::max(1, (nMinus1)); \
    }

int func0();
CASE3_FUNC(0, 1)
CASE3_FUNC(1, 2)
CASE3_FUNC(2, 3)
CASE3_FUNC(3, 4)
CASE3_FUNC(4, 5)

#if    defined(BSLS_PLATFORM_OS_HPUX) && defined(BSLS_PLATFORM_CPU_32_BIT)
# define FUNC_ADDRESS(p) (((UintPtr *) (UintPtr) (p))[1])
#elif (defined(BSLS_PLATFORM_OS_HPUX) && defined(BSLS_PLATFORM_CPU_64_BIT)) \
    || defined(BSLS_PLATFORM_OS_AIX)
# define FUNC_ADDRESS(p) (((UintPtr *) (UintPtr) (p))[0])
#else
# define FUNC_ADDRESS_NUM(p) ((UintPtr) (p))
#endif

int func0()
{
    enum { BUFFER_LENGTH = 100,
           IGNORE_FRAMES = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES
     };

    void *buffer[BUFFER_LENGTH];
    AddressEntry entries[BUFFER_LENGTH];

    UintPtr funcAddrs[] = { FUNC_ADDRESS_NUM(&func0),
                            FUNC_ADDRESS_NUM(&func1),
                            FUNC_ADDRESS_NUM(&func2),
                            FUNC_ADDRESS_NUM(&func3),
                            FUNC_ADDRESS_NUM(&func4),
                            FUNC_ADDRESS_NUM(&func5) };
    enum { NUM_FUNC_ADDRS = sizeof funcAddrs / sizeof *funcAddrs };

    bsl::memset(buffer, 0, sizeof(buffer));
    int numAddresses = baesu_StackAddressUtil::getStackAddresses(
                                                                buffer,
                                                                BUFFER_LENGTH);

    for (int toIndex = 0, fromIndex = IGNORE_FRAMES;
                            fromIndex < numAddresses; ++toIndex, ++fromIndex) {
        entries[toIndex].d_returnAddress = (UintPtr) buffer[fromIndex];
        entries[toIndex].d_traceIndex    = toIndex;
    }
    numAddresses -= IGNORE_FRAMES;

    bsl::sort(entries, entries + numAddresses);

    for (int i = 0; i < numAddresses - 1; ++i) {
        const AddressEntry *e = &entries[i];

        UintPtr lhs = e[0].d_returnAddress;
        UintPtr rhs = e[1].d_returnAddress;
        LOOP3_ASSERT(i, lhs, rhs, lhs < rhs);
    }

    bool problem = false;
    for (int i = 0; i < NUM_FUNC_ADDRS; ++i) {
        int index = findIndex(entries, numAddresses, funcAddrs[i]);
        if (i != index) {
            problem = true;
        }
        LOOP3_ASSERT(i, index, myHex(funcAddrs[i]), i == index);
    }

    if (problem || veryVerbose) {
        for (int i = 0; i < NUM_FUNC_ADDRS; ++i) {
            P_(i);    P(myHex(funcAddrs[i]));
        }

        for (int i = 0; i < numAddresses; ++i) {
            const AddressEntry *e = &entries[i];

            cout << "(" << i << "): addr = " <<
             myHex(e->d_returnAddress) << ", ti = " << e->d_traceIndex << endl;
        }
    }

    return 0;
}

}  // close namespace CASE_THREE

                                // ------
                                // Case 1
                                // ------

namespace CASE_ONE {

volatile int recurseDepth = 50;

enum {
    BUFFER_LENGTH = 1000
};

void recurser(volatile int *depth)
{

    if (--*depth > 0) {
        recurser(depth);
    }
    else {
        void *buffer[BUFFER_LENGTH];
        int numAddresses;

        bsl::memset(buffer, 0, sizeof(buffer));
        numAddresses = baesu_StackAddressUtil::getStackAddresses(
                                                                buffer,
                                                                BUFFER_LENGTH);
        LOOP_ASSERT(numAddresses, lamePlatform || numAddresses > recurseDepth);
        for (int i = 0; i < numAddresses; ++i) {
            ASSERT(0 != buffer[i]);
        }
        for (int i = numAddresses; i < BUFFER_LENGTH; ++i) {
            ASSERT(0 == buffer[i]);
        }

        bsl::memset(buffer, 0, sizeof(buffer));
        numAddresses = baesu_StackAddressUtil::getStackAddresses(buffer, 10);
        LOOP_ASSERT(numAddresses, lamePlatform || 10 == numAddresses);
        for (int i = 0; i < numAddresses; ++i) {
            ASSERT(0 != buffer[i]);
        }
        for (int i = numAddresses; i < BUFFER_LENGTH; ++i) {
            ASSERT(0 == buffer[i]);
        }
    }

    ++*depth;           // Prevent compilers from optimizing tail recursion as
                        // a loop.
}

}  // close namespace CASE_ONE

                            // ------------------
                            // case -1: benchmark
                            // ------------------

namespace CASE_MINUS_ONE {

typedef int (*GetStackPointersFunc)(void **buffer,
                                    int    maxFrames);
GetStackPointersFunc funcPtr;

enum {
    RECURSION_DEPTH = 40,
    MAX_FRAMES = 100
};

void recurser(int  iterations,
              int *depth)
{
    if (--*depth <= 0) {
        void *addresses[MAX_FRAMES];
        for (int i = iterations; i > 0; --i) {
            int frames = (*funcPtr)(addresses, MAX_FRAMES);
            LOOP2_ASSERT(RECURSION_DEPTH, frames, RECURSION_DEPTH < frames);
            LOOP2_ASSERT(RECURSION_DEPTH + 10, frames,
                                                RECURSION_DEPTH + 10 > frames);
        }
    }
    else {
        recurser(iterations, depth);
    }

    ++*depth;         // prevent tail recursion optimization
}

}  // close namespace CASE_MINUS_ONE

//=============================================================================
//                                   MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose  = argc > 2;
    veryVerbose = argc > 3 ? (bsl::atoi(argv[3]) ? bsl::atoi(argv[3]) : 1) : 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Have a sequence of records, each of which contains the address of
        //:   the beginning of a function's code and an index corresponding to
        //:   that function.
        //:
        //: 2 These functions are arranged in a chain to call each other and
        //:   the final one of the chain calls 'getStackAddresses' and looks up
        //:   the return addresses obtain in the sequence of records to verify
        //:   that the addresses are within the functions we expect them to be
        //:   in.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "Finding Right Functions Test\n"
                             "============================\n";

// #ifndef BSLS_PLATFORM_OS_WINDOWS
        // This test case just seems to fail on Windows, something to do with
        // '&' not working correctly, possibly because the compiler is creating
        // 'thunk' functions which just call the actual routine.  I wish they
        // wouldn't do that.

        ASSERT(CASE_FOUR::func6() != 0);
// #endif
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'getStackAddresses'
        //
        // Note: this was the original test that 'getStackAddresses' was
        // finding the proper functions.  This test was later simplified into
        // case 4 for use as a usage example.
        //
        // Concerns:
        //: 1 The method finds the functions we expect it to.
        //:
        //: 2 The returned addresses are in the right order.
        //:
        //: 3 The method returns non-zero on error.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Have a sequence of functions on the stack, and take pointers to
        //:   those functions using '&<function name>', put those pointers into
        //:   an array.
        //:
        //: 2 Collect the return addresses from the stack and put those
        //:   addresses into an array of records, also storing an int in each
        //:   record to identify which routine we expect the address to be in.
        //:   Sort the array of records.  It then becomes possible to verify
        //:   which return address is within which routine.
        //
        // Testing:
        //   int getStackAddresses(void **buffer, int maxFrames);
        // --------------------------------------------------------------------

        if (verbose) cout << "Finding Right Functions Test\n"
                             "============================\n";

#ifndef BSLS_PLATFORM_OS_WINDOWS
        // This test case just seems to fail on Windows, something to do with
        // '&' not working correctly, possibly because the compiler is creating
        // 'thunk' functions which just call the actual routine.  I wish they
        // wouldn't do that.

        ASSERT(CASE_THREE::func5() > 0);
#endif

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tgetStackAddresses" << endl;
            {
                void *buf[1];
                ASSERT_PASS(baesu_StackAddressUtil::getStackAddresses(buf, 0));
                ASSERT_FAIL(
                           baesu_StackAddressUtil::getStackAddresses(buf, -1));
            }
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // ZEROES TEST CASE
        //
        // Concerns:
        //: 1 'getStackAddresses(0, 0)' doesn't segFault.
        //
        // Plan:
        //: 1 Call 'getStackAddresses(0, 0)'.  In the debugger, verify that on
        //:   Linux, the first call calls 'backtrace' and the second call calls
        //:   neither 'dlopen' nor 'malloc'.
        //
        // Testing:
        //   CONCERN: 'getStackAddresses(0, 0)' doesn't segFault.
        // --------------------------------------------------------------------

        if (verbose) cout << "getStackAddresses(0, 0) TEST\n"
                             "============================\n";

        baesu_StackAddressUtil::getStackAddresses(0, 0);
        baesu_StackAddressUtil::getStackAddresses(0, 0);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //:
        //: 2 The method won't write past the end of the array it is passed.
        //
        // Plan:
        //: 1 Recurse many times, call 'getStackAddresses', and check that the
        //:   elements are either null or non-null as expected.
        //:
        //: 2 Do this twice, once in the case where the array is more than long
        //:   enough to accomodate the entire stack depth, and once in the case
        //:   where the array length passed is too short to hold the entire
        //:   stack, and verify that elements past the specified length of the
        //:   array are unaffected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        namespace TC = CASE_ONE;

        // Call 'recurseAndPrintExample3' with will recurse 'depth' times, then
        // print a stack trace.

        veryVerbose = bsl::max(0, veryVerbose);
        TC::recurseDepth += veryVerbose;
        int depth = TC::recurseDepth;
        TC::recurser(&depth);
        ASSERT(TC::recurseDepth == depth);
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // BENCHMARK OF getStackAddresses
        // --------------------------------------------------------------------

        namespace TD = CASE_MINUS_ONE;

        int depth = TD::RECURSION_DEPTH;

#if   defined(BSLS_PLATFORM_OS_WINDOWS)
        const int iterations = 100;
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
        const int iterations = 1000;
#else
        const int iterations = 100 * 1000;
#endif

        bsls_Stopwatch sw;
        TD::funcPtr = &baesu_StackAddressUtil::getStackAddresses;

        sw.start(true);
        TD::recurser(iterations, &depth);
        sw.stop();

        ASSERT(TD::RECURSION_DEPTH == depth);

        cout << "getStackAddresses: user: " <<
                                 sw.accumulatedUserTime() / iterations <<
                   ", wall: " << sw.accumulatedWallTime() / iterations << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
