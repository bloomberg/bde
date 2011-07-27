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
// The component under test implements a mechanism which summarize changes in
// certain statistics of the test allocator object supplied at construction.
// The main testing concern is that the monitor captures the correct data when
// it is constructed and later compares those values to the correct current
// statistics from the test allocator.  There are many places in the
// implementation for mismatches between different statistics.  The overall
// test strategy is to create a test allocator and tracking monitors, then
// drive the allocator through scenarios that put the monitors through all
// possible states, and compared the monitor state actual with the its expected
// state.
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
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 2] CONCERN: All accessor methods are declared 'const'.
// [ *] CONCERN: There is no temporary allocation from any allocator.

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

#define ALLOC_INUSE(LINE, MONITOR, STATE)                           \
    do {                                                            \
               if (0 == std::strcmp("UP",   (STATE))) {             \
                    LOOP_ASSERT((LINE),  (MONITOR).isInUseUp());    \
                    LOOP_ASSERT((LINE), !(MONITOR).isInUseSame());  \
                    LOOP_ASSERT((LINE), !(MONITOR).isInUseDown());  \
                                                                    \
        } else if (0 == std::strcmp("SAME", (STATE))) {             \
                    LOOP_ASSERT((LINE), !(MONITOR).isInUseUp());    \
                    LOOP_ASSERT((LINE),  (MONITOR).isInUseSame());  \
                    LOOP_ASSERT((LINE), !(MONITOR).isInUseDown());  \
                                                                    \
        } else if (0 == std::strcmp("DOWN", (STATE))) {             \
                    LOOP_ASSERT((LINE), !(MONITOR).isInUseUp());    \
                    LOOP_ASSERT((LINE), !(MONITOR).isInUseSame());  \
                    LOOP_ASSERT((LINE),  (MONITOR).isInUseDown());  \
                                                                    \
        } else {                                                    \
             assert(!"MATCH: "#STATE);                              \
        }                                                           \
    } while (0);

#define ALLOC_MAX(LINE, MONITOR, STATE)                             \
    do {                                                            \
               if (0 == std::strcmp("UP",   (STATE))) {             \
                    LOOP_ASSERT((LINE),  (MONITOR).isMaxUp());      \
                    LOOP_ASSERT((LINE), !(MONITOR).isMaxSame());    \
                                                                    \
        } else if (0 == std::strcmp("SAME", (STATE))) {             \
                    LOOP_ASSERT((LINE), !(MONITOR).isMaxUp());      \
                    LOOP_ASSERT((LINE),  (MONITOR).isMaxSame());    \
                                                                    \
        } else {                                                    \
             assert(!"MATCH: "#STATE);                              \
        }                                                           \
    } while (0);

#define ALLOC_TOTAL(LINE, MONITOR, STATE)                           \
    do {                                                            \
               if (0 == std::strcmp("UP",   (STATE))) {             \
                    LOOP_ASSERT((LINE),  (MONITOR).isTotalUp());    \
                    LOOP_ASSERT((LINE), !(MONITOR).isTotalSame());  \
                                                                    \
        } else if (0 == std::strcmp("SAME", (STATE))) {             \
                    LOOP_ASSERT((LINE), !(MONITOR).isTotalUp());    \
                    LOOP_ASSERT((LINE),  (MONITOR).isTotalSame());  \
                                                                    \
        } else {                                                    \
             assert(!"MATCH: "#STATE);                              \
        }                                                           \
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
// many testing concerns) that other classes do not.  We here illustrate how
// 'bslma_TestAllocatorMonitor' objects (in conjunction with
// 'bslma_TestAllocator' objects, of course) can be used in a test driver to
// succinctly address many concerns of an object's use of allocators.
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
        explicit MyClass(bslma_Allocator *basicAllocator = 0);
            // Create a 'MyClass' object having the (default) attribute values:
            //..
            // description() == ""
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

    // ACCESSORS
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

    // CONCERN: In no case does memory come from the default allocator.
    bslma_TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocator(&defaultAllocator);

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

// Our allocator-related concerns for 'MyClass' include:
//..
// Concerns:
//: 1 Any memory allocation is from the object allocator.
//:
//: 2 There is no temporary allocation from any allocator.
//:
//: 3 Every object releases any allocated memory at destruction.
//:
//: 4 QoI: The default constructor allocates no memory.
//:
//: 5 QoI: When possible, memory is cached for reuse.
//..
// Note that some of these concerns (e.g., C-4, C-5) are not part of the
// class's documented, contractual behavior.  Hence, they are tagged
// a Quality of Implementation (QoI) concerns.
//
// A plan to address these concerns might be:
//..
// Plan:
//: 1 Create three 'bslma_TestAllocator' objects and for each of these a
//:   'bslma_TestAllocatorMonitor' object.  Install two allocators as the
//:   global and default allocators.  The remaining allocator will be
//:   passed to out test objects on construction.
//:
//: 2 In an inner block, default create a test object using the designated
//:   "object" allocator.  Then allow the object to go out of scope
//:   (destroyed).  Confirm that no memory has been allocated from any of the
//:   allocators.  (C-4).
//:
//: 3 Exercise a test object so that memory is allocated allocation, and and
//:   confirm that the object allocator (only) is used as expected:
//:   1 Create a new test object using the (as yet unused) object allocator.
//:   2 Force the test object to allocate memory by setting an attribute that
//:     exceeds the size of the object itself.  Confirm that the attribute was
//:     set and that memory was allocated.
//:   3 Change the attribute to a smaller value and confirm that the current
//:     memory was reused. (C-5)
//:   4 Force the test object to deallocate/allocate by setting a string
//:     that exceeds the capacity acquired earlier.  Confirm that the
//:     that the number of outstanding allocations is unchanged (one returned,
//:     one given) but the maximum number of allocations is unchanged so there
//:     were not extra (temporary) allocations. (C-2)
//:
//: 4 Destroy the test object and confirm that all allocations are returned.
//:   (C-3)
//: 5 Confirm that at no time were the global allocator or the default
//:   allocator were used. (C-1)
//..
// The implementation of the plan is shown below:
//
// First, we create the trio of test allocators, their respective test
// allocator monitors, and install two of the allocators as the global and
// default allocators:
//..
    {
        bslma_TestAllocator ga("global",  veryVeryVeryVerbose);
        bslma_TestAllocator da("default", veryVeryVeryVerbose);
        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma_TestAllocatorMonitor gam(ga), dam(da), oam(oa);

        bslma_Default::setGlobalAllocator(&ga);
        bslma_Default::setDefaultAllocatorRaw(&da);
//..
// Then, we default construct a test object using the object allocator, and
// then, immediately destroy it.  The object allocator monitor, 'oam', shows
// that the allocator was not used.
//..
        {
            MyClass obj(&oa);
        }
        ASSERT(oam.isTotalSame()); // object  allocator unused
//..
// Next, we pass the (still unused) object allocator to another test object.
// This time, we coerce the object into allocating memory by setting an
// attribute.  (Setting an attribute larger than the receiving object
// usually means that the object cannot store the data within its own footprint
// and must allocate memory.)
//..
        {
            MyClass obj(&oa);

            const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            BSLMF_ASSERT(sizeof(obj) < sizeof(DESCRIPTION1));

            obj.setDescription(DESCRIPTION1);
            ASSERT(0 == std::strcmp(DESCRIPTION1, obj.description()));

            ASSERT(oam.isTotalUp());  // object allocator was used
            ASSERT(oam.isInUseUp());  // some outstanding allocation(s)
            ASSERT(oam.isMaxUp());    // a maximum was set
//..
// Notice, as expected, memory was allocated from object allocator.
//
// Then, we create a second monitor to capture the current state of the test
// allocator, and reset the attribute of that same object, this time to a short
// string.
//..
            bslma_TestAllocatorMonitor oam2(oa);

            obj.setDescription("a");
            ASSERT(0 == std::strcmp("a", obj.description()));

            ASSERT(oam2.isTotalSame());  // no allocations
//..
// Notice that there are no allocations because the object had sufficient
// capacity in previously allocated memory to store the short string.
//
// Next, we make the object allocate additional memory by setting a longer
// attribute: one that exceeds the capacity allocated for 'DESCRIPTION1'.  Use
// the second monitor to confirm that some allocation was made.
//..
            const char DESCRIPTION2[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            obj.setDescription(DESCRIPTION2);
            ASSERT(0 == std::strcmp(DESCRIPTION2, obj.description()));

            ASSERT(oam2.isTotalUp());    // object allocator used
            ASSERT(oam2.isInUseSame());  // outstanding block (allocation)
                                         // count unchanged (even though byte
                                         // outsanding byte count increased)
            ASSERT(oam2.isMaxSame());    // no extra (temporary) allocations
        }
//..
// Notice that the number of outstanding allocations remained unchanged: one
// block was deallocated, one block (a larger block) was allocated.  Moreover,
// the lack of change in the maximum of outstanding blocks ('isMaxSame') shows
// there were no extra (temporary) allocations.
//
// Finally, check that none of these operations used the default or global
// allocators.
//..
        ASSERT(oam.isInUseSame()); // All object allocator memory deallocated.
        ASSERT(gam.isTotalSame()); // Global  allocator was never used.
        ASSERT(dam.isTotalSame()); // Default allocator was never used.
    }
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR AND ACCESSORS
        //   This case tests that the monitor captures the correct allocator
        //   statistics on construction and compares those to the correct
        //   allocator statistics over its lifetime.
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
        //:     o Note that the monitors can be created for allocators with
        //:       outstanding allocations.  If so, there are scenarios where
        //:       the number of allocations "inUse" can decrease from that when
        //:       the monitor was created.
        //:
        //: 5 The behavior is depends on the number of allocations and
        //:   deallocations, never on the size (in bytes) of the allocations.
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
        //:   3 Deallocate allocations in order of allocation (C-4)
        //:   5 Allocate memory.
        //:   6 Allocate memory.
        //:   7 Allocate memory.
        //:   8 Deallocate outstanding allocations in reserve order. (C-4)
        //:
        //: 4 After step P-2.2, create a second monitor for the same test
        //:   allocator.  After operations, P-2.3 and P-2.4, check the return
        //:   values of the second monitor for the expected results.  This
        //:   monitor object allows us to observe the 'isInUseDown' return
        //:   return 'true'.  (C-4)
        //:
        //: 5 Use a table driven test to repeat the P-1 to P-4 for different
        //:   allocation sizes: increasing, decreasing, large, small, etc.
        //:   Since zero-sized allocations are no-ops to the allocator, the
        //:   table has no zero-sized allocations.  Doing so would interfere
        //:   certain expected state changes.
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
                          << "CTOR AND ACCESSORS" << endl
                          << "==================" << endl;

        const bsls_Types::size_type ZERO = 0;
        const bsls_Types::size_type TEN0 = 1;
        const bsls_Types::size_type TEN1 = 10;
        const bsls_Types::size_type TEN2 = 100;
        const bsls_Types::size_type TEN3 = 1000;
        const bsls_Types::size_type TEN4 = 10000;
        const bsls_Types::size_type TEN5 = 100000;
        const bsls_Types::size_type TEN6 = 1000000;
        const bsls_Types::size_type TEN7 = 10000000;
        const bsls_Types::size_type TEN8 = 100000000;

        static const struct {
            int                   d_line;   // source line number
            bsls_Types::size_type d_size1;
            bsls_Types::size_type d_size2;
            bsls_Types::size_type d_size3;
            bsls_Types::size_type d_size4;
            bsls_Types::size_type d_size5;
        } DATA[] = {

            //LINE  SIZE1  SIZE2  SIZE3  SIZE4  SIZE5
            //----  -----  -----  -----  -----  -----
            { L_,   TEN0,  TEN0,  TEN0,  TEN0,  TEN0 },
            { L_,   TEN1,  TEN1,  TEN1,  TEN1,  TEN1 },
            { L_,   TEN2,  TEN2,  TEN2,  TEN2,  TEN2 },
            { L_,   TEN3,  TEN3,  TEN3,  TEN3,  TEN3 },
            { L_,   TEN4,  TEN4,  TEN4,  TEN4,  TEN4 },
            { L_,   TEN5,  TEN5,  TEN5,  TEN5,  TEN5 },
            { L_,   TEN6,  TEN6,  TEN6,  TEN6,  TEN6 },
            { L_,   TEN7,  TEN7,  TEN7,  TEN7,  TEN7 },
            { L_,   TEN8,  TEN8,  TEN8,  TEN8,  TEN8 },

            { L_,   TEN0,  TEN1,  TEN2,  TEN3,  TEN4 },
            { L_,   TEN5,  TEN6,  TEN7,  TEN8,  TEN0 },

            { L_,   TEN8,  TEN7,  TEN6,  TEN5,  TEN4 },
            { L_,   TEN3,  TEN2,  TEN1,  TEN0,  TEN8 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                   LINE  = DATA[ti].d_line;
            const bsls_Types::size_type SIZE1 = DATA[ti].d_size1;
            const bsls_Types::size_type SIZE2 = DATA[ti].d_size2;
            const bsls_Types::size_type SIZE3 = DATA[ti].d_size3;
            const bsls_Types::size_type SIZE4 = DATA[ti].d_size4;
            const bsls_Types::size_type SIZE5 = DATA[ti].d_size5;

            if (veryVerbose) { T_ P_(SIZE1)
                                  P_(SIZE2)
                                  P_(SIZE3)
                                  P_(SIZE4)
                                  P(SIZE5)
                             }

                  TestObj  mta("test allocator", veryVeryVeryVerbose);
            const TestObj&  ta   = mta;

                      Obj  mtam1(ta);
            const     Obj&  tam1 = mtam1;

            ALLOC_INUSE(LINE, tam1, "SAME");
            ALLOC_MAX  (LINE, tam1, "SAME");
            ALLOC_TOTAL(LINE, tam1, "SAME");

            void *allocation1 = mta.allocate(SIZE1);

            ALLOC_INUSE(LINE, tam1, "UP");
            ALLOC_MAX  (LINE, tam1, "UP");
            ALLOC_TOTAL(LINE, tam1, "UP");

            void *allocation2 = mta.allocate(SIZE2);

            ALLOC_INUSE(LINE, tam1, "UP");
            ALLOC_MAX  (LINE, tam1, "UP");
            ALLOC_TOTAL(LINE, tam1, "UP");

                      Obj  mtam2(ta);
            const     Obj&  tam2 = mtam2;

                                             ALLOC_INUSE(LINE, tam2, "SAME");
                                             ALLOC_MAX  (LINE, tam2, "SAME");
                                             ALLOC_TOTAL(LINE, tam2, "SAME");

            mta.deallocate(allocation1);

            ALLOC_INUSE(LINE, tam1, "UP");   ALLOC_INUSE(LINE, tam2, "DOWN");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "SAME");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "SAME");

            mta.deallocate(allocation2);

            ALLOC_INUSE(LINE, tam1, "SAME"); ALLOC_INUSE(LINE, tam2, "DOWN");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "SAME");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "SAME");

            void *allocation3 = mta.allocate(SIZE3);

            ALLOC_INUSE(LINE, tam1, "UP");   ALLOC_INUSE(LINE, tam2, "DOWN");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "SAME");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "UP");

            void *allocation4 = mta.allocate(SIZE4);

            ALLOC_INUSE(LINE, tam1, "UP");   ALLOC_INUSE(LINE, tam2, "SAME");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "SAME");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "UP");

            void *allocation5 = mta.allocate(SIZE5);

            ALLOC_INUSE(LINE, tam1, "UP");   ALLOC_INUSE(LINE, tam2, "UP");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "UP");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "UP");

            mta.deallocate(allocation5);

            ALLOC_INUSE(LINE, tam1, "UP");   ALLOC_INUSE(LINE, tam2, "SAME");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "UP");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "UP");

            mta.deallocate(allocation4);

            ALLOC_INUSE(LINE, tam1, "UP");   ALLOC_INUSE(LINE, tam2, "DOWN");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "UP");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "UP");

            mta.deallocate(allocation3);

            ALLOC_INUSE(LINE, tam1, "SAME"); ALLOC_INUSE(LINE, tam2, "DOWN");
            ALLOC_MAX  (LINE, tam1, "UP");   ALLOC_MAX  (LINE, tam2, "UP");
            ALLOC_TOTAL(LINE, tam1, "UP");   ALLOC_TOTAL(LINE, tam2, "UP");
        }
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

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}
