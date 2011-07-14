// bslma_testallocatormonitor.t.cpp
#include <bslma_testallocatormonitor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmf_assert.h>

#include <cstring>     // memset()
#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a ...
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bslma_TestAllocatorMonitor(const bslma_TestAllocator& tA);
//
// ACCESSORS
// [ 2] bool isInUseDown() const;
// [ 2] bool isInUseSame() const;
// [ 2] bool isInUseUp() const;
// [ 2] bool isMaxSame() const;
// [ 2] bool isMaxUp() const;
// [ 2] bool isTotalSame() const;
// [ 2] bool isTotalUp() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [  ] CONCERN: All accessor methods are declared 'const'.
// [  ] CONCERN: There is no temporary allocation from any allocator.

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma_TestAllocator        TestObj;
typedef bslma_TestAllocatorMonitor Obj;

// ============================================================================
//                  NON-STANDARD TEST MACROS
// ----------------------------------------------------------------------------

#define ALLOC_INUSE(MONITOR, STATE)                    \
    do {                                               \
               if (0 == std::strcmp("UP",   #STATE)) { \
                    ASSERT( (MONITOR).isInUseUp());    \
                    ASSERT(!(MONITOR).isInUseSame());  \
                    ASSERT(!(MONITOR).isInUseDown());  \
                                                       \
        } else if (0 == std::strcmp("SAME", #STATE)) { \
                    ASSERT(!(MONITOR).isInUseUp());    \
                    ASSERT( (MONITOR).isInUseSame());  \
                    ASSERT(!(MONITOR).isInUseDown());  \
                                                       \
        } else if (0 == std::strcmp("DOWN", #STATE)) { \
                    ASSERT(!(MONITOR).isInUseUp());    \
                    ASSERT(!(MONITOR).isInUseSame());  \
                    ASSERT( (MONITOR).isInUseDown());  \
                                                       \
        } else {                                       \
             assert("Unknown State: " #STATE);         \
        }                                              \
    } while (0);

#define ALLOC_MAX(MONITOR, STATE)  \
    do {                                               \
               if (0 == std::strcmp("UP",   #STATE)) { \
                    ASSERT( (MONITOR).isMaxUp());      \
                    ASSERT(!(MONITOR).isMaxSame());    \
                                                       \
        } else if (0 == std::strcmp("SAME", #STATE)) { \
                    ASSERT(!(MONITOR).isMaxUp());      \
                    ASSERT( (MONITOR).isMaxSame());    \
                                                       \
        } else {                                       \
             assert("Unknown State: " #STATE);         \
        }                                              \
    } while (0);

#define ALLOC_TOTAL(MONITOR, STATE)                    \
    do {                                               \
               if (0 == std::strcmp("UP",   #STATE)) { \
                    ASSERT( (MONITOR).isTotalUp());    \
                    ASSERT(!(MONITOR).isTotalSame());  \
                                                       \
        } else if (0 == std::strcmp("SAME", #STATE)) { \
                    ASSERT(!(MONITOR).isTotalUp());    \
                    ASSERT( (MONITOR).isTotalSame());  \
                                                       \
        } else {                                       \
             assert("Unknown State: " #STATE);         \
        }                                              \
    } while (0);

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Standard Usage
///- - - - - - - - - - - - -
// Classes taking 'bslma_allocator' objects have many requirements (and thus,
// many testing concerns) that other classes do not.  For example, the concerns
// of a (value-semantic) attribute class include:
//
//: 1 An object created with the default constructor (with or without a
//:   supplied allocator) has the contractually specified default value.
//:
//: 2 If an allocator is NOT supplied to the default constructor, the default
//:   allocator in effect at the time of construction becomes the object
//:   allocator for the resulting object.
//:
//: 3 If an allocator IS supplied to the default constructor, that allocator
//:   becomes the object allocator for the resulting object.
//:
//: 4 Supplying a null allocator address has the same effect as not supplying
//:   an allocator.
//:
//: 5 Supplying an allocator to the default constructor has no effect on
//:   subsequent object values.
//:
//: 6 Any memory allocation is from the object allocator.
//:
//: 7 There is no temporary allocation from any allocator.
//:
//: 8 Every object releases any allocated memory at destruction.
//:
//: 9 QoI: The default constructor allocates no memory.
//
// Note that some of these concerns (e.g., C-9) are not part of the class's
// contractual behavior.
//
// We here illustrate how 'bslma_TestAllocatorMonitor' objects (in conjunction
// with 'bslma_TestAllocator' objects, of course) can be used in a test driver
// to succinctly address the above concerns.
//
// As a test subject, we introduce 'MyClass', an unconstrained attribute class
// with a single attribute, 'description', an ascii string.  For the sake of
// brevity, 'MyClass' defines only a default constructor, a primary manipulator
// (the 'setDescription' method), and a basic accessor (the 'description'
// method).  These suffice for the purposes of these example.  Note that proper
// attribute class would also implement value and copy constructors,
// 'operator==', and other methods.
//
//..
    class MyClass {
        // This unconstrained (value-semantic) attribute class has a single
        // attribute, 'description', an ascii string.

        // DATA
        int              d_capacity;
        char            *d_description_p;
        bslma_Allocator *d_allocator_p;   // held, not owned

      public:
        // CREATORS
        MyClass(bslma_Allocator *basicAllocator = 0);
            // Create a 'MyClass' object having the (default) attribute values:
            //..
            //  description() == ""
            //..
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        ~MyClass();
           // Destroy this object.

        // MANIPULATORS
        void setDescription(const char* value);
            // Set the 'description' attribute of this object to data of the
            // specified 'length' at the specified 'value' address.

        // ACCESSORS
        const char *description() const;
            // Return a reference providing non-modifiable access to the
            // 'description' attribute of this object.
    };

    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

                            // -------------
                            // class MyClass
                            // -------------

    // CREATORS
    inline
    MyClass::MyClass(bslma_Allocator *basicAllocator)
    : d_capacity(0)
    , d_description_p(0)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    inline
    MyClass::~MyClass()
    {
        BSLS_ASSERT_SAFE(0 <= d_capacity);

        d_allocator_p->deallocate(d_description_p);
    }

    // MANIPULATORS
    inline
    void MyClass::setDescription(const char *value)
    {
        BSLS_ASSERT_SAFE(value);

        int length = std::strlen(value);
        int size   = length + 1;
        if (size > d_capacity) {
            d_allocator_p->deallocate(d_description_p);
            d_description_p = (char *) d_allocator_p->allocate(size);
            d_capacity      = size;
        }
        std::memcpy(d_description_p, value, length);
        d_description_p[length] = '\0';
    }

    // MANIPULATORS
    inline
    const char *MyClass::description() const
    {
        return d_description_p ? d_description_p : "";
    }
//..
//

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 3: {
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
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// First, following the typical testing pattern, we create a trio of
// 'bslma_TestAllocator' objects, and install two as the global and default
// allocators.  The remaining allocator will be supplied to the object.
//..
    {
        bslma_TestAllocator ga("global",  veryVeryVeryVerbose);
        bslma_TestAllocator da("default", veryVeryVeryVerbose);
        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma_Default::setGlobalAllocator(&ga);
        bslma_Default::setDefaultAllocatorRaw(&da);
//..
// Then, one creates a trio of 'bslma_TestAllocatorMonitor' objects to capture
// the current state (unused) of each of the 'bslma_TestAllocator' objects.
//..
        bslma_TestAllocatorMonitor gam(ga), dam(da), oam(oa);
//..
// Next, one creates an object of the class under test, supplying the remaining
// allocator, and subjects the object to the relevant operations.  In this
// first test, the object is just default constructed, and then destroyed.
//..
        {
            MyClass obj(&oa);
        }
//..
// Then, after the object is destroyed, the several monitor objects show that
// their respective allocators were not used.
//..
        ASSERT(gam.isTotalSame()); // no (de)allocations
        ASSERT(dam.isTotalSame()); // no (de)allocations
        ASSERT(oam.isTotalSame()); // no (de)allocations
//..
// Notice that we have addressed C-3.

// Next, we force the object into allocating memory, and confirm that memory
// was allocated.
//..
        {
            MyClass obj(&oa);

            const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            BSLMF_ASSERT(sizeof(obj) < sizeof(DESCRIPTION1));

            obj.setDescription(DESCRIPTION1);

            ASSERT(oam.isTotalUp());  // object allocator was used
            ASSERT(oam.isInUseUp());  // some outstanding allocation(s)
//..
// Then, we reset the attribute to a short value and test that the previously
// allocated memory is reused.  To measure changes in the object allocator
// relative to its current state, we introduce a second monitor object.
//..

            bslma_TestAllocatorMonitor oam2(oa);

            obj.setDescription("a");

            ASSERT(oam2.isTotalSame());  // no (de)allocations
//..
// Notice that this test addresses C-4.  Next, we assign a value that forces
// the object to allocate additional memory.
//..
            const char DESCRIPTION2[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            obj.setDescription(DESCRIPTION2);

            ASSERT(oam2.isTotalUp());    // Object allocator was used.
            ASSERT(oam2.isInUseSame());  // Outstanding allocation
                                                 // count (but not byte count)
                                                 // is unchanged.
        }
//..
// Now that the object has been destroyed.
//..
        ASSERT(oam.isTotalUp());   // Object allocator was used.
        ASSERT(oam.isInUseSame()); // All allocations were deallocated.
//..
// Notice that these tests confirm C-2.
//
// Finally, we examine the two monitors of the two non-object allocators to
// confirm that they were not used by any of these operations.
//..
        ASSERT(gam.isTotalSame()); // Global  allocator was never used.
        ASSERT(dam.isTotalSame()); // Default allocator was never used.
    }
//..
// Notice that the last tests, along with our observations of the use of the
// object allocator, address C-2 and C-5.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR MONITOR CTOR AND METHODS
        //
        // Concerns:
        //: 1 For any statistic tracked by the monitor (e.g., "InUse") only one
        //:   of the related methods returns 'true'.  Thus, if the 'inUseSame'
        //:   method returns 'true', then the 'inUseUp' and 'inUseDown' methods
        //:   return 'false'.
        //:
        //: 2 All of a monitor's "Same" methods return 'true' until it's
        //:   tracked allocator is used.
        //:
        //: 3 Any allocation(s) from the tracked allocator idempotently flip
        //:   the monitor's 'isMaxUp' and 'isTotalUp' methods return values to
        //:   'true'.
        //:
        //: 4 The return values of the "inUse" methods track the net number of
        //:   allocations and deallocations since the creation of the monitor.
        //:   Note that the monitors can be created for allocators with
        //:   outstanding allocations.  If so, there are scenarios where the
        //:   number of allocations "inUse" can decrease from that when the
        //:   monitor was created.
        //:
        // Plan:
        //: 1 Always test the return values as a "suite" (i.e., "InUse", "Max",
        //:   and "Total") and confirm that only one is 'true'.  Devise a set
        //:   of macros to do so concisely.  (C-1)
        //:
        //: 2 Create a test allocator, a tracking test allocator monitor, and
        //:   an confirm the return values of the monitor before the allocator
        //:   is used.  (C-2)
        //:
        //: 3 Put perform the following operations on the test allocator.
        //:   After each operation, check the return values of each monitor
        //:   method for expected results.
        //:   1 Allocate memory.  (C-3)
        //:   2 Allocate additional memory.  (C-3)
        //:   3 Deallocate first allocation.  (C-4)
        //:   4 Deallocate second allocation.  (C-4)
        //:
        //: 4 After step P-2.2, create a second monitor for the same test
        //:   allocator.  After operations, P-2.3 and P-2.4, check the return
        //:   values of the second monitor for the expected results.  (C-4)
        //
        // Testing:
        //   bslma_TestAllocatorMonitor(const bslma_TestAllocator& tA);
        //   bool isInUseDown() const;
        //   bool isInUseSame() const;
        //   bool isInUseUp() const;
        //   bool isMaxSame() const;
        //   bool isMaxUp() const;
        //   bool isTotalSame() const;
        //   bool isTotalUp() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST ALLOCATOR MONITOR CTOR AND METHODS" << endl
                          << "=======================================" << endl;

              TestObj  mta("test allocator", veryVeryVeryVerbose);
        const TestObj&  ta   = mta;

                  Obj  mtam1(ta);
        const     Obj&  tam1 = mtam1;

        ALLOC_INUSE(tam1, "SAME");
        ALLOC_MAX  (tam1, "SAME");
        ALLOC_TOTAL(tam1, "SAME");

        void *allocation1 = mta.allocate(1);

        ALLOC_INUSE(tam1, "UP");
        ALLOC_MAX  (tam1, "UP");
        ALLOC_TOTAL(tam1, "UP");

        void *allocation2 = mta.allocate(1);

        ALLOC_INUSE(tam1, "UP");
        ALLOC_MAX  (tam1, "UP");
        ALLOC_TOTAL(tam1, "UP");

                  Obj  mtam2(ta);
        const     Obj&  tam2 = mtam2;

                                    ALLOC_INUSE(tam2, "SAME");
                                    ALLOC_MAX  (tam2, "SAME");
                                    ALLOC_TOTAL(tam2, "SAME");

        mta.deallocate(allocation1);

        ALLOC_INUSE(tam1, "UP");    ALLOC_INUSE(tam2, "DOWN");
        ALLOC_MAX  (tam1, "UP");    ALLOC_MAX  (tam2, "SAME");
        ALLOC_TOTAL(tam1, "UP");    ALLOC_TOTAL(tam2, "SAME");

        mta.deallocate(allocation2);

        ALLOC_INUSE(tam1, "SAME"); ALLOC_INUSE(tam2, "DOWN");
        ALLOC_MAX  (tam1, "UP");   ALLOC_MAX  (tam2, "SAME");
        ALLOC_TOTAL(tam1, "UP");   ALLOC_TOTAL(tam2, "SAME");

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
        //: 1 Create a 'bslma_TestAllocator' object.
        //: 2 Create a 'bslma_TestAllocatorMonitor' object to track the
        //:   object created in P-1.
        //: 3 Perform several allocations from and deallocations to the
        //:   the test allocator created in P-1.  At each point, confirm that
        //:   the test allocator monitor object created in P-2 returns 'true'
        //:   from the appropriate methods.
        //: 4 Destroy the test allocator monitor.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma_TestAllocator        ta("test allocator", veryVeryVeryVerbose);
        bslma_TestAllocatorMonitor tam(ta);

        ASSERT(tam.isTotalSame());
        ASSERT(tam.isInUseSame());
        ASSERT(tam.isMaxSame());

        void *allocation = ta.allocate(1);

        ASSERT(tam.isTotalUp());
        ASSERT(tam.isInUseUp());
        ASSERT(tam.isMaxUp());

        ta.deallocate(allocation);

        ASSERT(tam.isTotalUp());
        ASSERT(tam.isInUseSame());
        ASSERT(tam.isMaxUp());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}
