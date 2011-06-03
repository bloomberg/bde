// baesu_stackaddressutil.t.cpp                                       -*-C++-*-
#include <baesu_stackaddressutil.h>

#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

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
// GLOBAL HELPER VARIABLES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_Types::UintPtr UintPtr;
int verbose;
int veryVerbose;

// 'lamePlatform' -- on lame platforms where StackAddressUtil doesn't work or
// executables are stripped, disable some of the tests.
#if defined(BDE_BUILD_TARGET_OPT) && defined(BSLS_PLATFORM__OS_WINDOWS)
const bool lamePlatform = true;
#else
const bool lamePlatform = false;
#endif

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace CASE_ONE {

volatile int recurseDepth = 50;

enum {
    BUFFER_LENGTH = 1000
};

struct ChainLink {
    int        d_i;
    ChainLink *d_next;

    ChainLink(int i, ChainLink *next) : d_i(i), d_next(next) {}
        // Create a 'ChainLink' object with a value of the specified 'i' and
        // and a link pointing at the specified '*next'.
};

int recurser(volatile int *depth, ChainLink *cl_p)
{
    int sum = 0;

    if (--*depth <= 0) {
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
    else {
        ChainLink cl(2, cl_p);
        for (ChainLink *p = &cl; p; p = p->d_next) {
             sum += p->d_i;
             p->d_i *= sum - 5;
        }

        sum *= recurser(depth, &cl);
    }

    ++*depth;           // Prevent compilers from optimizing tail recursion as
                        // a loop.
    return sum;
}

}  // close namespace CASE_ONE

bsl::string myHex(UintPtr up)
{
    bsl::stringstream ss;
    ss.setf(bsl::ios_base::hex, bsl::ios_base::basefield);
    ss.setf(bsl::ios_base::showbase);
    ss << up;
    return ss.str();
}

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

#if    defined(BSLS_PLATFORM__OS_HPUX) && defined(BSLS_PLATFORM__CPU_32_BIT)
# define FUNC_ADDRESS(p) (((UintPtr *) (UintPtr) (p))[1])
#elif (defined(BSLS_PLATFORM__OS_HPUX) && defined(BSLS_PLATFORM__CPU_64_BIT)) \
    || defined(BSLS_PLATFORM__OS_AIX)
# define FUNC_ADDRESS(p) (((UintPtr *) (UintPtr) (p))[0])
#else
# define FUNC_ADDRESS(p) ((UintPtr) (p))
#endif

int func0()
{
    enum { BUFFER_LENGTH = 100,
           IGNORE_FRAMES = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES
     };

    void *buffer[BUFFER_LENGTH];
    AddressEntry entries[BUFFER_LENGTH];

    UintPtr funcAddrs[] = { FUNC_ADDRESS(&func0),
                            FUNC_ADDRESS(&func1),
                            FUNC_ADDRESS(&func2),
                            FUNC_ADDRESS(&func3),
                            FUNC_ADDRESS(&func4),
                            FUNC_ADDRESS(&func5) };
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

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // FINDING RIGHT FUNCTIONS TEST CASE
        //
        // Concerns:
        //  That 'getStackAddresses' finds the functions we expect it to.
        //
        // Plan:
        //  Make sure that the addresses we obtain are between the address
        //  of the function we expect it to be in and the address of the next
        //  known function.
        //  This test case just seems to fail on Windows, something to do with
        //  '&' not working correctly, possibly because the compiler is
        //  creating 'thunks' which just call the routine.  I wish they
        //  wouldn't do that.
        // --------------------------------------------------------------------

        if (verbose) cout << "Finding Right Functions Test\n"
                             "============================\n";

#ifndef BSLS_PLATFORM__OS_WINDOWS
        ASSERT(CASE_THREE::func5() > 0);
#endif
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // ZEROES TEST CASE
        //
        // Concerns:
        //   That 'getStackAddresses(0, 0)' doesn't segFault.
        //
        // Plan:
        //   Call 'getStackAddresses(0, 0)'.  In the debugger, verify that on
        //   Linux, the first call calls 'backtrace' and the second call calls
        //   neither 'dlopen' nor 'malloc'.
        // --------------------------------------------------------------------

        if (verbose) cout << "getStackAddresses(0, 0) TEST\n"
                             "============================\n";

        baesu_StackAddressUtil::getStackAddresses(0, 0);
        baesu_StackAddressUtil::getStackAddresses(0, 0);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE THREE
        //
        // Concerns:
        //   That the usage example that uses 'getStackAddresses' and
        //   'initializeFromAddressArray' works.
        //
        // Plan:
        //   Call the routines in the usage example to observe that the stack
        //   trace works.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        namespace TC = CASE_ONE;

        // Call 'recurseAndPrintExample3' with will recurse 'depth' times, then
        // print a stack trace.

        veryVerbose = bsl::max(0, veryVerbose);
        TC::recurseDepth += veryVerbose;
        int depth = TC::recurseDepth;
        TC::ChainLink cl(15, 0);
        TC::recurser(&depth, &cl);
        ASSERT(TC::recurseDepth == depth);
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // BENCHMARK OF getStackAddresses
        // --------------------------------------------------------------------

        namespace TD = CASE_MINUS_ONE;

        int depth = TD::RECURSION_DEPTH;

#if   defined(BSLS_PLATFORM__OS_WINDOWS)
        const int iterations = 100;
#elif defined(BSLS_PLATFORM__OS_SOLARIS)
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
