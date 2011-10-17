// bslstl_rbtree.t.cpp                                                -*-C++-*-
#include <bslstl_rbtree.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P_(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P(I) P_(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P(I) P(J) P_(K) \
                aSsErT(!(X), #X, __LINE__); } }

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::RbTreePrimitives Op;
typedef bslalg::RbTreeNode       Node;

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

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------
// JSL: REMOVE THIS after it is moved to the test allocator.
// JSL: change the name to 'bslma_TestAllocatorMonitor'.

class bslma_TestAllocatorMonitor {
    // TBD

    // DATA
    int                              d_lastInUse;
    int                              d_lastMax;
    int                              d_lastTotal;
    const bslma_TestAllocator *const d_allocator_p;

  public:
    // CREATORS
    bslma_TestAllocatorMonitor(const bslma_TestAllocator& basicAllocator);
        // TBD

    ~bslma_TestAllocatorMonitor();
        // TBD

    // ACCESSORS
    bool isInUseSame() const;
        // TBD

    bool isInUseUp() const;
        // TBD

    bool isMaxSame() const;
        // TBD

    bool isMaxUp() const;
        // TBD

    bool isTotalSame() const;
        // TBD

    bool isTotalUp() const;
        // TBD
};

// CREATORS
inline
bslma_TestAllocatorMonitor::bslma_TestAllocatorMonitor(
                                     const bslma_TestAllocator& basicAllocator)
: d_lastInUse(basicAllocator.numBlocksInUse())
, d_lastMax(basicAllocator.numBlocksMax())
, d_lastTotal(basicAllocator.numBlocksTotal())
, d_allocator_p(&basicAllocator)
{
}

inline
bslma_TestAllocatorMonitor::~bslma_TestAllocatorMonitor()
{
}

// ACCESSORS
inline
bool bslma_TestAllocatorMonitor::isInUseSame() const
{
    return d_allocator_p->numBlocksInUse() == d_lastInUse;
}

inline
bool bslma_TestAllocatorMonitor::isInUseUp() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() != d_lastInUse;
}

inline
bool bslma_TestAllocatorMonitor::isMaxSame() const
{
    return d_allocator_p->numBlocksMax() == d_lastMax;
}

inline
bool bslma_TestAllocatorMonitor::isMaxUp() const
{
    return d_allocator_p->numBlocksMax() != d_lastMax;
}

inline
bool bslma_TestAllocatorMonitor::isTotalSame() const
{
    return d_allocator_p->numBlocksTotal() == d_lastTotal;
}

inline
bool bslma_TestAllocatorMonitor::isTotalUp() const
{
    return d_allocator_p->numBlocksTotal() != d_lastTotal;
}

// ============================================================================
//                          BREATHING TEST
// ----------------------------------------------------------------------------

template <class VALUE, class COMPARATOR> 
void executeBreathingTest(const COMPARATOR&  comparator,
                          VALUE             *testValues,
                          int                numValues)
{ 
    typedef bslstl::RbTree<VALUE, COMPARATOR>       Obj;
    typedef std::pair<typename Obj::iterator, bool> InsertResult;

    bslma_TestAllocator defaultAllocator("defaultAllocator");    
    bslma_DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma_TestAllocator objectAllocator("objectAllocator");    

    ASSERT(0 < numValues);
    ASSERT(8 > numValues);
    
    // Default construct an empty set.
    {
        Obj x(&objectAllocator); const Obj& X = x;
        ASSERT(0 == X.size());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == objectAllocator.numBytesInUse());
    }

    // Test use of allocators
    {
        bslma_TestAllocator objectAllocator1("objectAllocator1");
        bslma_TestAllocator objectAllocator2("objectAllocator2");

        Obj o1(&objectAllocator1); const Obj& O1 = o1;
        ASSERT(&objectAllocator1 == O1.allocator().mechanism());

        for (int i = 0; i < numValues; ++i) {
            typename Obj::iterator it;
            o1.insert(testValues[i]);
        }
        ASSERT(numValues == O1.size());            
        ASSERT(0 <  objectAllocator1.numBytesInUse());
        ASSERT(0 == objectAllocator2.numBytesInUse());
          
        bslma_TestAllocatorMonitor monitor1(objectAllocator1);
        Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
        ASSERT(&objectAllocator2 == O2.allocator().mechanism());
        
        ASSERT(numValues == O1.size());            
        ASSERT(numValues == O2.size());            
        ASSERT(monitor1.isInUseSame())
            ASSERT(monitor1.isTotalSame());
        ASSERT(0 <  objectAllocator1.numBytesInUse());
        ASSERT(0 <  objectAllocator2.numBytesInUse());
  
        Obj o3(&objectAllocator1); const Obj& O3 = o3;
        ASSERT(&objectAllocator1 == O3.allocator().mechanism());
  
        ASSERT(numValues == O1.size());
        ASSERT(numValues == O2.size());
        ASSERT(0         == O3.size());
        ASSERT(monitor1.isInUseSame());
        ASSERT(monitor1.isTotalSame());
        ASSERT(0 <  objectAllocator1.numBytesInUse());
        ASSERT(0 <  objectAllocator2.numBytesInUse());
  
        o1.swap(o3);
        ASSERT(0         == O1.size());
        ASSERT(numValues == O2.size());
        ASSERT(numValues == O3.size());
        ASSERT(monitor1.isInUseSame());
        ASSERT(monitor1.isTotalSame());
        ASSERT(0 <  objectAllocator1.numBytesInUse());
        ASSERT(0 <  objectAllocator2.numBytesInUse());
          
        o3.swap(o2);
        ASSERT(0         == O1.size());
        ASSERT(numValues == O2.size());
        ASSERT(numValues == O3.size());
        ASSERT(monitor1.isInUseSame());
        ASSERT(monitor1.isTotalUp());
        ASSERT(0 <  objectAllocator1.numBytesInUse());
        ASSERT(0 <  objectAllocator2.numBytesInUse());
  
        ASSERT(&objectAllocator1 == O1.allocator().mechanism());
        ASSERT(&objectAllocator2 == O2.allocator().mechanism());
        ASSERT(&objectAllocator1 == O3.allocator().mechanism());
    }

    // For each possible permuation of values, insert and remove values from a
    // set.
    std::sort(testValues, testValues + numValues, comparator);
    do {
        // Insert unique elements into a RbTree. 
        Obj x(&objectAllocator); const Obj& X = x;
        for (int i = 0; i < numValues; ++i) {

            Obj y(X, &objectAllocator); const Obj& Y = y;

            ASSERT(X == Y);
            ASSERT(!(X != Y));

            ASSERT(X.end() == X.find(testValues[i]));

            InsertResult result = x.insertUnique(testValues[i]);
            ASSERT(X.end()       != result.first); 
            ASSERT(true          == result.second);
            ASSERT(testValues[i] == *result.first);
            ASSERT(result.first == X.find(testValues[i]));

            ASSERT(result.first == x.insertUnique(testValues[i]).first);
            ASSERT(false        == x.insertUnique(testValues[i]).second);

            ASSERT(i + 1 == X.size());

            ASSERT(X != Y);
            ASSERT(!(X == Y));

            y = x;
            ASSERT(X == Y);
            ASSERT(!(X != Y));
        }

        ASSERT(0 != objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        // Verify sorted order of elements.
        {
            typename Obj::const_iterator last = X.begin();
            typename Obj::const_iterator it   = ++(X.begin());
            while (it != X.end()) {
                ASSERT(comparator(*last, *it));
                ++it;
            }
        }
       
        // Use erase(iterator) on all the elements.
        for (int i = 0; i < numValues; ++i) {
            typename Obj::const_iterator it     = x.find(testValues[i]);
            typename Obj::const_iterator nextIt = it;
            ++nextIt;

            ASSERT(X.end()       != it);
            ASSERT(testValues[i] == *it);


            typename Obj::const_iterator resIt = x.erase(it);
            ASSERT(resIt             == nextIt);
            ASSERT(numValues - i - 1 == X.size());
            if (resIt != X.end()) {
                ASSERT(comparator(testValues[i], *resIt));
            }
        }
    } while (std::next_permutation(testValues, 
                                   testValues + numValues, 
                                   comparator));
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

    bslma_TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
      } break;
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
        int NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);


        executeBreathingTest(std::less<int>(), 
                             INT_VALUES, 
                             NUM_INT_VALUES);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
