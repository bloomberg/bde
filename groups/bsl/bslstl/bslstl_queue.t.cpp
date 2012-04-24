//bslstl_queue.t.cpp                                                  -*-C++-*-

#include <bslstl_queue.h>

#include <bslstl_allocator.h>
#include <bslstl_iterator.h>
#include <bslstl_forwarditerator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatormonitor.h>    // for testing only
#include <bslma_testallocatorexception.h>  // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_stopwatch.h>                // for testing only
#include <bsls_util.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P_(I) P(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P_(I) P_(J) P(K) \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { P_(I) P_(J) P_(K) P(L)\
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// Define DEFAULT DATA used in multiple test cases.

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}


// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

template <class VALUE, class CONTAINER>
struct QTestDriver {

    static void testCase1(VALUE *testValues, size_t numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};

template <class VALUE, class CONTAINER>
void QTestDriver<VALUE, CONTAINER>::testCase1(VALUE  *testValues,
                                              size_t  numValues)
{
    typedef bsl::queue<VALUE, CONTAINER>  Obj;
    bslma_TestAllocator         defaultAllocator("defaultAllocator");
    bslma_DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma_TestAllocator objectAllocator("objectAllocator");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Default construct an empty queue.\n");
    }
    {
        Obj o1; const Obj& O1 = o1;
        ASSERTV(0    == O1.size());
        ASSERTV(true == O1.empty());
        ASSERTV(0    <  defaultAllocator.numBytesInUse());

        bslma_TestAllocatorMonitor monitor(&defaultAllocator);
        Obj o2(&objectAllocator); const Obj& O2 = o2;
        ASSERTV(0    == O2.size());
        ASSERTV(true == O2.empty());
        ASSERTV(monitor.isInUseSame());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test use of allocators.\n");
    }
    {
        bslma_TestAllocator objectAllocator1("objectAllocator1");
        bslma_TestAllocator objectAllocator2("objectAllocator2");

        CONTAINER container(&objectAllocator1);
        Obj o1(container, &objectAllocator1); const Obj& O1 = o1;

        for (size_t i = 0; i < numValues; ++i) {
            o1.push(testValues[i]);
        }
        ASSERTV(numValues == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());

        bslma_TestAllocatorMonitor monitor1(&objectAllocator1);
        Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        Obj o3(&objectAllocator1); const Obj& O3 = o3;

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0         == O3.size());
        ASSERTV(monitor1.isInUseUp());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        bslma_TestAllocatorMonitor monitor2(&objectAllocator1);
        o1.swap(o3);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(monitor2.isInUseSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        o3.swap(o2);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(monitor2.isInUseSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());
    }

    if (veryVerbose) {
        printf("Test primary manipulators/accessors.\n");
    }
    {
        Obj x(&objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            
            Obj y(X, &objectAllocator); const Obj& Y = y;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            // Test 'push'.
            
            x.push(testValues[i]);

            // Test size, empty, front, back.

            ASSERTV(i + 1 == X.size());
            ASSERTV(false == X.empty());
            ASSERTV(testValues[0] == X.front());
            ASSERTV(testValues[i] == X.back());

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        for (size_t i = 0; i < numValues; --i) {
            
            // Test 'pop'.
            
            x.pop();

            // Test size, front, back.

            ASSERTV(numValues - i - 1 == X.size ());
            if (i < numValues - 1) {
                ASSERTV(testValues[i + 1]         == X.front());
                ASSERTV(testValues[numValues - 1] == X.back ());
            } else {
                ASSERTV(false == X.empty());
            }
        }

        ASSERTV(false == X.empty());
        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());
    }

    /*

    priority_queue<int> intPQueue(&objectAllocator);
    ASSERT(intPQueue.empty());
    intPQueue.push(101);
    ASSERT(!intPQueue.empty());
    intPQueue.push(102);
    intPQueue.push(103);
    intPQueue.push(104);
    ASSERT(!intPQueue.empty());
    ASSERT(4   == intPQueue.size());
    ASSERT(104 == intPQueue.top());
    intPQueue.pop();
    ASSERT(3   == intPQueue.size());
    ASSERT(103 == intPQueue.top());
    intPQueue.pop();
    ASSERT(2   == intPQueue.size());
    ASSERT(102 == intPQueue.top());
    intPQueue.pop();
    ASSERT(1   == intPQueue.size());
    ASSERT(101 == intPQueue.top());
    intPQueue.pop();
    ASSERT(0   == intPQueue.size());
    ASSERT(intPQueue.empty());
    */
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        // 
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        
        int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
        int NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

        QTestDriver<int, deque<int> >::testCase1(INT_VALUES, NUM_INT_VALUES);
        //TODO: uncomment when 'bsl::list' is available
        //QTestDriver<int,  list<int> >::testCase1(INT_VALUES, NUM_INT_VALUES);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}
