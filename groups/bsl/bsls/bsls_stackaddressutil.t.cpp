// bsls_stackaddressutil.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bsls_stackaddressutil.h>

#include <bsls_atomic.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstddef>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <thread.h>  // thr_setconcurrency
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
// 'getStackAddresses' will not be able to trace through our stack frames if
// we're optimized on Windows

# pragma optimize("", off)

#endif

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides 2 primary functions, the lower-level
// 'getStackAddress' and the simple 'formatCheapStack' that builds on it.  Test
// cases are broken up in these levels, first testing 'getStackAddress' fully,
// then testing formatCheapStack and its usage example.
//
// ----------------------------------------------------------------------------
// [ 3]  int getStackAddresses(void **buffer, int maxFrames);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE #1
// [ 5] static void formatCheapStack(char*,int,char*);
// [ 6] CHEAPSTACK: test truncation
// [ 7] CHEAPSTACK: test stacks
// [ 8] CHEAPSTACK: test process name
// [ 9] MULTITHREADED TEST
// [10] USAGE EXAMPLE #2
// [ 2] getStackAddresses(0, 0)
// [-1] Speed benchmark of getStackAddresses

// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//               GLOBAL HELPER VARIABLES AND TYPES FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::UintPtr UintPtr;
int verbose;
int veryVerbose;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

int main(int, char **);    // forward declaration

namespace u {

// 'lamePlatform' -- on lame platforms where StackAddressUtil doesn't work or
// executables are stripped, disable some of the tests.
#if defined(BDE_BUILD_TARGET_OPT) && defined(BSLS_PLATFORM_OS_WINDOWS)
const bool lamePlatform = true;
#else
const bool lamePlatform = false;
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadType;
#else
typedef pthread_t ThreadType;
#endif

extern "C" {
typedef void* (*THREAD_ENTRY)(void *arg);
}

static int createThread(ThreadType *aHandle, THREAD_ENTRY aEntry, void *arg )
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    *aHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)aEntry,arg,0,0);
    return *aHandle ? 0 : -1;
#else
    return pthread_create(aHandle, 0, aEntry, arg);
#endif
}

static void  joinThread(ThreadType aHandle)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(aHandle,INFINITE);
    CloseHandle(aHandle);
#else
    pthread_join(aHandle,0);
#endif
}

template <class TYPE>
TYPE foilOptimizer(const TYPE funcPtr)
    // The function just returns 'funcPtr', but only after putting it through a
    // transform that the optimizer can't possibly understand that leaves it
    // with its original value.  'TYPE' is expected to be a function pointer
    // type.
    //
    // Note that it's still necessary to put a lot of the routines through
    // contortions to avoid the optimizer optimizing tail calls as jumps.
{
    TYPE ret, ret2 = funcPtr;

    UintPtr u = (UintPtr) funcPtr;

    const int loopGuard  = 0x8edf1000;    // garbage with a lot of trailing
                                          // 0's.
    const int toggleMask = 0xa72c3dca;    // pure garbage

    UintPtr u2 = u;
    for (int i = 0; !(i & loopGuard); ++i) {
        u ^= (i & toggleMask);
    }

    ret = (TYPE) u;

    // That previous loop toggled all the bits in 'u' that it touched an even
    // number of times, so 'ret == ret2', but I'm pretty sure the optimizer
    // can't figure that out.

    ASSERT(  u2 ==   u);
    ASSERT(ret2 == ret);

    return ret;
}

static std::string myHex(UintPtr up)
{
    std::stringstream ss;
    ss.setf(std::ios_base::hex, std::ios_base::basefield);
    ss.setf(std::ios_base::showbase);
    ss << up;
    return ss.str();
}

}  // close namespace u

                            // ------------------
                            // MULTITHREADED TEST
                            // ------------------

namespace MULTITHREADED_TEST {

enum { k_NUM_THREADS = 100 };
enum { k_MAX_FRAMES_TO_CAPTURE = 30 };
enum Mode { e_CAPTURE_ADDRESSES, e_COMPARE_ADDRESSES };
Mode mode;
void *addressDataBase[10][k_MAX_FRAMES_TO_CAPTURE];
bsls::AtomicInt threadId(0);
bsls::TimeInterval start;
bsls::AtomicInt tracesDone(0);
int framesToCaptureBase = 0;

void topOfTheStack(int idMod)
{
    const int framesToCapture = framesToCaptureBase
                              ? framesToCaptureBase + idMod
                              : k_MAX_FRAMES_TO_CAPTURE;
    ASSERTV(framesToCapture, framesToCapture <= k_MAX_FRAMES_TO_CAPTURE);

    void *addresses[k_MAX_FRAMES_TO_CAPTURE];

    const int rc = bsls::StackAddressUtil::getStackAddresses(
                                                   addresses, framesToCapture);
    if (framesToCaptureBase) {
        ASSERTV(rc, framesToCapture, rc == framesToCapture);
    }

    switch (mode) {
      case e_CAPTURE_ADDRESSES: {
        ::memcpy(addressDataBase[idMod], addresses,
                                             framesToCapture * sizeof(void *));
        if (0 == idMod) {
            ASSERT(0 == framesToCaptureBase);

            // We want to 'framesToCaptureBase' so that it just barely includes
            // the function 'loopForEightSeconds' and everything above it.
            // We know that that function is just preceded by many identical
            // instances of 'recurseABunchOfTimes'.
            //
            // We know that the stack trace will consist of
            //: o (possibly) an ignored frame
            //: o '&topOfTheStack' + off
            //: o '&recurseABunchOfTimes' + off of call to 'topOfTheStack'
            //: o many identical instances of '&recurseABunchOfTimes' recurse
            //: o 'loopForEightSeconds' + off
            // So we probe the array to find the repeating addresses, and then
            // set 'framesToCaptureBase' to also capture the one frame after
            // them.

            int ii = 1;
            while (addresses[ii-1] != addresses[ii] &&
                                                ii < k_MAX_FRAMES_TO_CAPTURE) {
                ++ii;
            }
            while (addresses[ii-1] == addresses[ii] &&
                                                ii < k_MAX_FRAMES_TO_CAPTURE) {
                ++ii;
            }
            ASSERTV(ii, 10 <= ii);
            ASSERTV(ii, ii < k_MAX_FRAMES_TO_CAPTURE - 1);
            framesToCaptureBase = ii + 1;
        }
      } break;
      case e_COMPARE_ADDRESSES: {
        ASSERT(framesToCaptureBase);

        if (0 != ::memcmp(addresses, addressDataBase[idMod],
                                           framesToCapture * sizeof(void *))) {
            bool match = true;
            for (int ii = 0; match && ii < framesToCapture; ++ii) {
                ASSERTV(idMod, ii, framesToCapture,
                        (match = addresses[ii] == addressDataBase[idMod][ii]));
            }
        }
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "impossible switch");
      }
    }
}

void recurseABunchOfTimes(int *depth, int, void *, int, int idMod)
{
    if (--*depth <= 0) {
        topOfTheStack(idMod);
    }
    else {
        recurseABunchOfTimes(depth, 0, depth, 0, idMod);
    }

    ++*depth;
}

void *loopForEightSeconds(void *arg)
{
    const int idMod = threadId++ % 10;
    const int expDepth = 10 + idMod;
    const int iterations = arg ? 100 : 1;

    do {
        int depth = expDepth;

        for (int ii = 0; ii < iterations; ++ii, ++tracesDone) {
            recurseABunchOfTimes(&depth, 0, &ii, 0, idMod);
            ASSERT(expDepth == depth);
        }
    } while ((bsls::SystemTime::nowMonotonicClock() -
                                            start).totalSecondsAsDouble() < 8);

    return 0;
}

}  // close namespace MULTITHREADED_TEST

                                 // -----
                                 // Usage
                                 // -----

namespace CASE_USAGE {

// In this example we demonstrate how to use 'formatCheapStack' to generate a
// string containing the current stack trace and instructions on how to print
// it out from showfunc.tsk
//
// First we define our function where we want to format the stack:
//..
struct MyTest {
    static void printCheapStack()
    {
        char str[128];
        bsls::StackAddressUtil::formatCheapStack(str, 128);
        printf("%s", str);
    }
};
//..
// Calling this function will then result in something like this being printed
// to standard output:
//..
//  Please run "/bb/bin/showfunc.tsk <binary_name_here> 403308 402641 ...
//                                ... 3710C1ED1D 400F49" to see the stack trace
//..
// Then, if you had encountered this output running the binary "mybinary.tsk",
// you could see your stack trace by running this command:
//..
//  /bb/bin/showfunc.tsk mybinary.tsk 403308 402641 3710C1ED1D 400F49
//..
// This will produce output like this:
//..
//  0x403308 _ZN6MyTest15printCheapStackEv + 30
//  0x402641 main + 265
//  0x3710c1ed1d ???
//  0x400f49 ???
//..
// Telling you that 'MyTest::printCheapStack' was called directly from 'main'.
// Note that if you had access to the binary name that was invoked then that
// could be provided that as the optional last argument to 'printCheapStack' to
// get a 'showfunc.tsk' command that can be more easily invoked, like this:
//..
struct MyTest2 {
    static void printCheapStack()
    {
        char str[128];
        bsls::StackAddressUtil::formatCheapStack(str, 128, "mybinary.tsk");
        printf("%s", str);
    }
};
//..
// resulting in output that looks like this:
//..
//  Please run "/bb/bin/showfunc.tsk mybinary.tsk 403308 402641 3710C1ED1D ...
//                                           ... 400F49" to see the stack trace
//..

}  // close namespace CASE_USAGE

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
        // Create an 'AddressEntry' object and initialize it with the specified
        // 'funcAddress' and 'index'.
    {}

    bool operator<(const AddressEntry& rhs) const
        // Return 'true' if the address stored in the object is lower than the
        // address stored in 'rhs' and 'false' otherwise.  Note that this is a
        // member function for brevity, it only exists to facilitate sorting
        // 'AddressEntry' objects in a vector.
    {

        return d_funcAddress < rhs.d_funcAddress;
    }
};
// Then, we define 'entries', a vector of address entries.  This will be
// populated such that a given entry will contain function address '&funcN' and
// index 'N'.  The elements will be sorted according to function address.

std::vector<AddressEntry> entries;

// Next, we define 'findIndex':

static int findIndex(const void *retAddress)
    // Return the index of the address entry whose function uses an instruction
    // located at specified 'retAddress'.  The behavior is undefined unless
    // 'retAddress' is the address of an instruction in use by a function
    // referred to by an address entry in 'entries'.
{
    unsigned int u = 0;
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

// Then, we define a volatile global variable that we will use in calculation
// to discourage compiler optimizers from inlining:

volatile unsigned int volatileGlobal = 1;

// Next, we define a set of functions that will be called in a nested fashion
// -- 'func5' calls 'func4' who calls 'fun3' and so on.  In each function, we
// will perform some inconsequential instructions to prevent the compiler from
// inlining the functions.
//
// Note that we know the 'if' conditions in these 5 subroutines never evaluate
// to 'true', however, the optimizer cannot figure that out, and that will
// prevent it from inlining here.

static unsigned int func1();
static unsigned int func2()
{
    if (volatileGlobal > 10) {
        return (volatileGlobal -= 100) * 2 * func2();                 // RETURN
    }
    else {
        return volatileGlobal * 2 * func1();                          // RETURN
    }
}
static unsigned int func3()
{
    if (volatileGlobal > 10) {
        return (volatileGlobal -= 100) * 2 * func3();                 // RETURN
    }
    else {
        return volatileGlobal * 3 * func2();                          // RETURN
    }
}
static unsigned int func4()
{
    if (volatileGlobal > 10) {
        return (volatileGlobal -= 100) * 2 * func4();                 // RETURN
    }
    else {
        return volatileGlobal * 4 * func3();                          // RETURN
    }
}
static unsigned int func5()
{
    if (volatileGlobal > 10) {
        return (volatileGlobal -= 100) * 2 * func5();                 // RETURN
    }
    else {
        return volatileGlobal * 5 * func4();                          // RETURN
    }
}
static unsigned int func6()
{
    if (volatileGlobal > 10) {
        return (volatileGlobal -= 100) * 2 * func6();                 // RETURN
    }
    else {
        return volatileGlobal * 6 * func5();                          // RETURN
    }
}

// Next, we define the macro FUNC_ADDRESS, which will take a parameter of
// '&<function name>' and return a pointer to the actual beginning of the
// function's code, which is a non-trivial and platform-dependent exercise.
// Note: this doesn't work on Windows for global routines.

#if   defined(BSLS_PLATFORM_OS_HPUX)
# define FUNC_ADDRESS(p) (((void **) (void *) (p))[sizeof(void *) == 4])
#elif defined(BSLS_PLATFORM_OS_AIX)
# define FUNC_ADDRESS(p) (((void **) (void *) (p))[0])
#else
# define FUNC_ADDRESS(p) ((void *) (p))
#endif

// Then, we define 'func1', the last function to be called in the chain of
// nested function calls.  'func1' uses
// 'bsls::StackAddressUtil::getStackAddresses' to get an ordered sequence of
// return addresses from the current thread's function call stack and uses the
// previously defined 'findIndex' function to verify those address are correct.

unsigned int func1()
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
    std::sort(entries.begin(), entries.end());

    // Then, we obtain the stack addresses with 'getStackAddresses'.

    enum { BUFFER_LENGTH = 100 };
    void *buffer[BUFFER_LENGTH];
    memset(buffer, 0, sizeof(buffer));
    int numAddresses = bsls::StackAddressUtil::getStackAddresses(
                                                                buffer,
                                                                BUFFER_LENGTH);
    ASSERT(numAddresses >= (int) entries.size());
    ASSERT(numAddresses < BUFFER_LENGTH);
    ASSERT(0 != buffer[numAddresses-1]);
    ASSERT(0 == buffer[numAddresses]);

    // Finally, we go through several of the first addresses returned in
    // 'buffer' and verify that each address corresponds to the routine we
    // expect it to.

    // Note that on some, but not all, platforms there is an extra
    // "narcissistic" frame describing 'getStackAddresses' itself at the
    // beginning of 'buffer'.  By starting our iteration through 'buffer' at
    // 'k_IGNORE_FRAMES', we guarantee that the first address we examine will
    // be in 'func1' on all platforms.

    int funcIdx  = 1;
    int stackIdx = bsls::StackAddressUtil::k_IGNORE_FRAMES;
    for (; funcIdx < (int) entries.size(); ++funcIdx, ++stackIdx) {
        ASSERT(stackIdx < numAddresses);
        ASSERT(funcIdx == findIndex(buffer[stackIdx]));
    }

    if (testStatus || veryVerbose) {
        Q(Entries:);
        for (unsigned int u = 0; u < entries.size(); ++u) {
            P_(u); P_((void *) entries[u].d_funcAddress);
            P(entries[u].d_index);
        }

        Q(Stack:);
        for (int i = 0; i < numAddresses; ++i) {
            P_(i); P(buffer[i]);
        }
    }

    return volatileGlobal;
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
        P_(u::myHex(funcP).c_str()) P_(u::myHex(retP).c_str()) P(ret);
    }

    return ret;
}

#define CASE3_FUNC(nMinus1, n)                                               \
    void func ## n(int *pi)                                                  \
    {                                                                        \
        ++*pi;                                                               \
        if (*pi > 100) {                                                     \
            func ## n(pi);                                                   \
        }                                                                    \
        else if (*pi < 100) {                                                \
            func ## nMinus1(pi);                                             \
        }                                                                    \
                                                                             \
        ++*pi;                                                               \
    }

void func0(int *pi);
CASE3_FUNC(0, 1)
CASE3_FUNC(1, 2)
CASE3_FUNC(2, 3)
CASE3_FUNC(3, 4)
CASE3_FUNC(4, 5)

#if    defined(BSLS_PLATFORM_OS_HPUX) && defined(BSLS_PLATFORM_CPU_32_BIT)
# define FUNC_ADDRESS_NUM(p) (((UintPtr *) (UintPtr) (p))[1])
#elif (defined(BSLS_PLATFORM_OS_HPUX) && defined(BSLS_PLATFORM_CPU_64_BIT)) \
    || defined(BSLS_PLATFORM_OS_AIX)
# define FUNC_ADDRESS_NUM(p) (((UintPtr *) (UintPtr) (p))[0])
#else
# define FUNC_ADDRESS_NUM(p) ((UintPtr) (p))
#endif

void func0(int *pi)
{
    enum { BUFFER_LENGTH = 100,
           IGNORE_FRAMES = bsls::StackAddressUtil::k_IGNORE_FRAMES
    };

    *pi += 2;

    void        *buffer[BUFFER_LENGTH];
    AddressEntry entries[BUFFER_LENGTH];

    UintPtr funcAddrs[] = { FUNC_ADDRESS_NUM(&func0),
                            FUNC_ADDRESS_NUM(&func1),
                            FUNC_ADDRESS_NUM(&func2),
                            FUNC_ADDRESS_NUM(&func3),
                            FUNC_ADDRESS_NUM(&func4),
                            FUNC_ADDRESS_NUM(&func5) };
    enum { NUM_FUNC_ADDRS = sizeof funcAddrs / sizeof *funcAddrs };

    memset(buffer, 0, sizeof(buffer));
    int numAddresses = bsls::StackAddressUtil::getStackAddresses(
                                                                buffer,
                                                                BUFFER_LENGTH);

    for (int toIndex = 0, fromIndex = IGNORE_FRAMES;
                            fromIndex < numAddresses; ++toIndex, ++fromIndex) {
        entries[toIndex].d_returnAddress = (UintPtr) buffer[fromIndex];
        entries[toIndex].d_traceIndex    = toIndex;
    }
    numAddresses -= IGNORE_FRAMES;

    std::sort(entries, entries + numAddresses);

    for (int i = 0; i < numAddresses - 1; ++i) {
        const AddressEntry *e = &entries[i];

        UintPtr lhs = e[0].d_returnAddress;
        UintPtr rhs = e[1].d_returnAddress;
        ASSERTV(i, lhs, rhs, lhs < rhs);
    }

    bool problem = false;
    for (int i = 0; i < NUM_FUNC_ADDRS; ++i) {
        int index = findIndex(entries, numAddresses, funcAddrs[i]);
        if (i != index) {
            problem = true;
        }
        ASSERTV(i, index, u::myHex(funcAddrs[i]).c_str(), i == index);
    }

    if (problem || veryVerbose) {
        for (int i = 0; i < NUM_FUNC_ADDRS; ++i) {
            P_(i);    P(u::myHex(funcAddrs[i]).c_str());
        }

        for (int i = 0; i < numAddresses; ++i) {
            const AddressEntry *e = &entries[i];

            printf("(%d): addr = %s, ti = %d\n", i,
                   u::myHex(e->d_returnAddress).c_str(), e->d_traceIndex);
        }
    }
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
        int   numAddresses;

        memset(buffer, 0, sizeof(buffer));
        numAddresses = bsls::StackAddressUtil::getStackAddresses(
                                                                buffer,
                                                                BUFFER_LENGTH);
        ASSERTV(numAddresses, u::lamePlatform || numAddresses > recurseDepth);
        for (int i = 0; i < numAddresses; ++i) {
            ASSERT(0 != buffer[i]);
        }
        for (int i = numAddresses; i < BUFFER_LENGTH; ++i) {
            ASSERT(0 == buffer[i]);
        }

        memset(buffer, 0, sizeof(buffer));
        numAddresses = bsls::StackAddressUtil::getStackAddresses(buffer, 10);
        ASSERTV(numAddresses, u::lamePlatform || 10 == numAddresses);
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

enum CustomValues {
    // This enum has a name to placate the IBM C++03 compiler which requires
    // that all template parameters to be named types.
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
            ASSERTV(RECURSION_DEPTH, frames, RECURSION_DEPTH < frames);
            ASSERTV(RECURSION_DEPTH + 10, frames,
                                                RECURSION_DEPTH + 10 > frames);
        }
    }
    else {
        recurser(iterations, depth);
    }

    ++*depth;         // prevent tail recursion optimization
}

}  // close namespace CASE_MINUS_ONE

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose  = argc > 2;
    veryVerbose = argc > 3 ? (atoi(argv[3]) ? atoi(argv[3]) : 1) : 0;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #2
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 The usage example is included above to be sure it compiles.
        //: 2
        //
        // Testing:
        //   USAGE EXAMPLE #2
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE #2"
                            "\n================\n");

        CASE_USAGE::MyTest::printCheapStack();
        CASE_USAGE::MyTest2::printCheapStack();

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST
        //
        // Concerns:
        //: 1 We want to test that the stack walkback is thread safe.  Of
        //:   particular concern is the Windows implementation, where we use
        //:   'RtlCaptureStackBackTrace' which is not explicitly documented on
        //:   MSDN as thread-safe, but it is argued that MSDN always calls out
        //:   non-thread-safe functions as such.
        //
        // Plan:
        //: 1 We do 10 different stack traces where we capture a stack trace to
        //:   10 buffers, a different stack trace each time.
        //:
        //: 2 Then we repeat, in 10 threads, repeatedly doing stack traces,
        //:   the stack traces in each thread matching one of those ten
        //:   different traces, and check that the stack trace matches the
        //:   so no two threads are doing matching traces.  Do this repeatedly
        //:   for 8 seconds.
        //
        // Testing:
        //   MULTITHREADED TEST
        // --------------------------------------------------------------------

        namespace TC = MULTITHREADED_TEST;

        TC::mode = TC::e_CAPTURE_ADDRESSES;
        TC::threadId = 0;
        TC::start = bsls::SystemTime::nowMonotonicClock() - 20;
        for (int ii = 0; ii < 10; ++ii) {
            TC::loopForEightSeconds(0);
        }
        ASSERT(10 == TC::threadId);

        // Make sure the 'addressDataBase' is as we expect it to be.

        {
            ASSERT(10 < TC::framesToCaptureBase);

            void **stack = &TC::addressDataBase[0][0];
            int   jj     = TC::framesToCaptureBase;

            const void *p = stack[jj - 1];
            const void *q = stack[jj - 2];
            ASSERT(p != q);
            for (int ii = 0; ii < 10; ++ii) {
                stack = &TC::addressDataBase[ii][0];
                jj    = bsls::StackAddressUtil::k_IGNORE_FRAMES;
                ASSERT(stack[jj] != stack[jj + 1]);

                jj    = TC::framesToCaptureBase + ii;

                ASSERT(p == stack[jj - 1]);
                for (int kk = 2; kk < 11; ++kk) {
                    ASSERT(q == stack[jj - kk]);
                }
            }
        }

        u::ThreadType threadHandles[TC::k_NUM_THREADS];

        TC::mode = TC::e_COMPARE_ADDRESSES;
        TC::threadId = 0;
        TC::tracesDone = 0;
        TC::start = bsls::SystemTime::nowMonotonicClock();
        for (int ii = 0; ii < TC::k_NUM_THREADS; ++ii) {
            u::createThread(&threadHandles[ii], &TC::loopForEightSeconds, &ii);
        }

        for (int ii = 0; ii < TC::k_NUM_THREADS; ++ii) {
            u::joinThread(threadHandles[ii]);
        }

        if (verbose) P(TC::tracesDone);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CHEAPSTACK PROCESS NAME TEST
        //
        // Concerns:
        //: 1 'formatCheapStack' when not passed a 'taskName' parameter should
        //:   get the process name, which for this test driver should contain
        //:   the component name "bsls_stackaddressutil" somewhere in it.
        //:
        // Plan:
        //: 1 Call 'formatCheapStack'.
        //:
        //: 2 Verify the result contains "bsls_stackaddressutil".
        //
        // Testing:
        //   CHEAPSTACK: test process name
        // --------------------------------------------------------------------

          if (verbose) printf( "\nCHEAPSTACK PROCESS NAME TEST"
                               "\n============================\n" );

          char res[1024];

          bsls::StackAddressUtil::formatCheapStack( res, 1024 );

          if (verbose) printf("\nCheapstack output:%s", res);

          const char *tpos = std::strstr(res, "bsls_stackaddressutil");

          ASSERT(tpos != NULL);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CHEAPSTACK STACK TEST
        //
        // Concerns:
        //: 1 'formatCheapStack' should produce the same output other than just
        //:   one return point when called consecutively.
        //:
        // Plan:
        //: 1 Call 'formatCheapStack' twice.
        //:
        //: 2 Compare the outputs.
        //
        // Testing:
        //   CHEAPSTACK: test stacks
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCHEAPSTACK STACK TEST"
                             "\n=====================\n" );

        if (verbose) P(bsls::StackAddressUtil::k_IGNORE_FRAMES + 0);

        char res1[1024];
        char res2[1024];

        const char *taskName = "TaSkNaMe!";
        std::size_t taskNameLen = std::strlen(taskName);

        bsls::StackAddressUtil::formatCheapStack( res1, 1024, taskName );
        bsls::StackAddressUtil::formatCheapStack( res2, 1024, taskName );

        if (verbose) printf("\nCheapstack 1 output:%s", res1);
        if (verbose) printf("\nCheapstack 2 output:%s", res2);

        // the first return address after the task name should be the only
        // difference between the two results.

        const char *t1 = std::strstr(res1, taskName);
        const char *t2 = std::strstr(res2, taskName);

        ASSERT( t1 != NULL );
        ASSERT( t2 != NULL );
        if (t1 == NULL || t2 == NULL) { break; }

        t1 += taskNameLen;
        t2 += taskNameLen;

        ASSERT( *t1 == ' ' );
        ASSERT( *t2 == ' ' );

        if (*t1 != ' ' || *t2 != ' ') { break; }

        t1 += 1;
        t2 += 1;

        long headLen1 = (t1 - res1);
        long headLen2 = (t2 - res2);

        LOOP2_ASSERT( headLen1, headLen2, headLen1 == headLen2);

        LOOP2_ASSERT( res1, res2, 0 == std::strncmp(res1, res2, headLen1) );

        const char *u1 = std::strstr(t1, " ");
        const char *u2 = std::strstr(t2, " ");

        ASSERT(u1 != NULL);
        ASSERT(u2 != NULL);
        if (u1 == NULL || u2 == NULL) { break; }

        LOOP2_ASSERT( u1, u2, 0 == std::strcmp(u1,u2) );

        char ret1[1024];
        std::memset(ret1,0,1024);
        char ret2[1024];
        std::memset(ret2,0,1024);

        std::strncpy( ret1, t1, u1-t1 );
        std::strncpy( ret2, t2, u2-t2 );

        if (verbose) printf("\nVerifying that tops of stack traces are "
                            "different (%s != %s)\n", ret1, ret2);

        LOOP2_ASSERT( ret1, ret2, 0 != std::strcmp( ret1, ret2 ) );

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CHEAPSTACK TRUNCATION TEST
        //
        // Concerns:
        //: 1 'formatCheapStack' should work properly even when given buffers
        //:   too small for its full output.
        //
        // Plan:
        //: 1 Repeatedly call formatCheapStack from the same place in a loop.
        //:
        //: 2 Increase the buffer size given to formatCheapStack in each
        //:   iteration of the loop.
        //:
        //: 3 Verify that each consecutive call extends the output of the
        //:   previous invocation.
        //:
        //: 4 Verify that the 0 case does not alter the buffer.
        //
        // Testing:
        //   CHEAPSTACK: test truncation
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCHEAPSTACK TRUNCATION TEST"
                             "\n==========================\n" );

        char prev[1024];
        std::memset(prev,0,1024);
        char next[1024];
        std::memset(next,0,1024);


        for (int i = 1; i < 1024; ++i)
        {
            // place a marker in next to be sure it gets written to or not
            // appropriately.
            next[0] = 'X';

            bsls::StackAddressUtil::formatCheapStack(next,i);

            if (veryVerbose) printf("\nCheapstack(%d) output: %s", i, next);

            if (i == 0)
            {
                ASSERT(next[0] == 'X');
            }
            else
            {
                ASSERT(next[0] != 'X');

                ASSERT( std::strlen(next) < static_cast<std::size_t>(i) );

                if (i > 2)
                {
                    // allow 3 characters leeway because some platforms don't
                    // use the last characters "efficiently" to make sure that
                    // they always null-terminate properly.
                    LOOP3_ASSERT( i, prev, next, 0 ==
                                  std::strncmp( prev, next, i-3) );
                }

                std::memcpy( prev, next, 1024);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CHEAPSTACK TEST
        //
        // Concerns:
        //: 1 Want to observe the basic call to cheapstack works.
        //
        // Plan:
        //: 1 Call 'formatCheapStack' with a sufficiently large buffer.
        //:
        //: 2 Call 'formatCheapStack' again with an explicit taskname to be
        //:   sure that the taskname is part of the output.
        //:
        // Testing
        //   static void formatCheapStack(char*,int,char*);
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCHEAPSTACK TEST"
                             "\n===============\n" );

        {
            char buf[1024];
            bsls::StackAddressUtil::formatCheapStack(buf, 1024);

            if (verbose) printf("\nCheapstack output:%s", buf);

            ASSERT( NULL != std::strstr(buf, "showfunc.tsk") );
            ASSERT( NULL != std::strstr(buf, "to see the stack trace") );
        }

        {
            const char *taskName = "TaSkNaMe!";
            char        buf[1024];
            bsls::StackAddressUtil::formatCheapStack(buf, 1024, taskName);

            if (verbose) printf("\nCheapstack output:%s", buf);

            ASSERT( NULL != std::strstr(buf, "showfunc.tsk") );
            ASSERT( NULL != std::strstr(buf, "to see the stack trace") );
            ASSERT( NULL != std::strstr(buf, taskName ) );
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1
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
        //   USAGE EXAMPLE #1
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE #1"
                            "\n================\n");

#ifndef BSLS_PLATFORM_OS_WINDOWS
        // This test case just seems to fail on Windows, something to do with
        // '&' not working correctly, possibly because the compiler is creating
        // 'thunk' functions that just call the actual routine.  I wish they
        // wouldn't do that.

        unsigned int result = CASE_FOUR::func6();
        ASSERTV(result, 6 * 5 * 4 * 3 * 2, result == 6 * 5 * 4 * 3 * 2);
#endif
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

        if (verbose) printf("\nCLASS METHOD 'getStackAddresses'"
                            "\n================================\n");

#ifndef BSLS_PLATFORM_OS_WINDOWS
        // This test case just seems to fail on Windows, something to do with
        // '&' not working correctly, possibly because the compiler is creating
        // 'thunk' functions that just call the actual routine.  I wish they
        // wouldn't do that.

        int i = 0;
        CASE_THREE::func5(&i);
        ASSERTV(i, 12 == i);
#endif
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

        if (verbose) printf("\nZEROES TEST CASE"
                            "\n================\n");

        if (verbose) printf("getStackAddresses(0, 0) TEST.");

        bsls::StackAddressUtil::getStackAddresses(0, 0);
        bsls::StackAddressUtil::getStackAddresses(0, 0);
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
        //:   enough to accommodate the entire stack depth, and once in the
        //:   case where the array length passed is too short to hold the
        //:   entire stack, and verify that elements past the specified length
        //:   of the array are unaffected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        namespace TC = CASE_ONE;

        // Call 'recurseAndPrintExample3' with will recurse 'depth' times, then
        // print a stack trace.

        veryVerbose = std::max(0, veryVerbose);
        TC::recurseDepth += veryVerbose;
        int depth = TC::recurseDepth;
        TC::recurser(&depth);
        ASSERT(TC::recurseDepth == depth);

        if (verbose) P(BSLS_PLATFORM_CMP_VERSION);      // Used to calculate
                                                        // 'k_IGNORE_FRAMES'.
      }  break;
      default: {
          fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
