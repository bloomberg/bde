// bslma_testallocatormonitor.t.cpp
#include <bslma_testallocatormonitor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <cstdlib>     // 'atoi'
#include <cstring>     // 'strlen'
#include <iostream>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a mechanism that summarize changes in
// certain statistics of the test allocator object supplied at construction.
// The main testing concerns are that the monitor captures the correct data
// when it is constructed and later compares those values to the correct
// current statistics from the test allocator.  There are many places in the
// implementation for mismatches between different statistics.  The overall
// test strategy is to create a test allocator and tracking monitors, then
// drive the allocator through scenarios that put the monitors through all
// possible states, and compared the monitor's actual actual state with its
// expected state.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR pointer parameter is declared 'const'.
//: o No memory is ever allocated by this object.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o Any explicit memory allocations would use the global or default
//:   allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bslma_TestAllocatorMonitor(const bslma_TestAllocator *tA);
// [ 2] ~bslma_TestAllocatorMonitor();
//
// ACCESSORS
// [ 3] bool isInUseDown() const;
// [ 3] bool isInUseSame() const;
// [ 3] bool isInUseUp() const;
// [ 3] bool isMaxSame() const;
// [ 3] bool isMaxUp() const;
// [ 3] bool isTotalSame() const;
// [ 3] bool isTotalUp() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 3] CONCERN: All accessor methods are declared 'const'.
// [ *] CONCERN: There is no memory allocation from any allocator.

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma_TestAllocator        Ta;
typedef bslma_TestAllocatorMonitor Tam;

// ============================================================================
//                  NON-STANDARD TEST MACROS
// ----------------------------------------------------------------------------

#define A(X) ASSERT(X)

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Standard Usage
///- - - - - - - - - - - - -
// Classes taking 'bslma_allocator' objects have many requirements (and thus,
// many testing concerns) that other classes do not.  Here we illustrate how
// 'bslma_TestAllocatorMonitor' objects (in conjunction with
// 'bslma_TestAllocator' objects) can be used in a test driver to succinctly
// address many concerns of an object's use of allocators.
//
// First, for a test subject, we introduce 'MyClass', an unconstrained
// attribute class having a single, null-terminated ascii string attribute,
// 'description'.  For the sake of brevity, 'MyClass' defines only a default
// constructor, a primary manipulator (the 'setDescription' method), and a
// basic accessor (the 'description' method).  These suffice for the purposes
// of these example.  Note that a proper attribute class would also implement
// value and copy constructors, 'operator==', an accessor for the allocator,
// and other methods.
//..
    class MyClass {
        // This unconstrained (value-semantic) attribute class has a single,
        // null-terminated ascii string attribute, 'description'.

        // DATA
        int              d_capacity;      // available memory
        char            *d_description_p; // string data
        bslma_Allocator *d_allocator_p;   // held, not owned

      public:
        // CREATORS
        explicit MyClass(bslma_Allocator *basicAllocator = 0);
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
        void setDescription(const char *value);
            // Set the null-terminated ascii string 'description' attribute of
            // this object to the specified 'value'.  On completion, the
            // 'description' method returns the address of a copy of the ascii
            // string at 'value'.

        // ACCESSORS
        const char *description() const;
            // Return the value of the null-terminated ascii string
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

        int size = std::strlen(value) + 1;
        if (size > d_capacity) {
            char *newMemory = (char *) d_allocator_p->allocate(size);
            d_allocator_p->deallocate(d_description_p);
            d_description_p = newMemory;
            d_capacity      = size;

        }
        std::memcpy(d_description_p, value, size);
    }
//..
// Notice that the implementation of the manipulator allocates/deallocates
// memory *before* updating the object.  This ordering leaves the object
// unchanged in case the allocator throws an exception (part of the strong
// exception guarantee).  This is an implementation detail, not a part
// of the contract (in this example).
//..

    // ACCESSORS
    inline
    const char *MyClass::description() const
    {
        return d_description_p ? d_description_p : "";
    }
//..

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
      case 4: {
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

// Then, we design a test-driver for 'MyClass'.  Our allocator-related concerns
// for 'MyClass' include:
//..
// Concerns:
//: 1 Any memory allocation is from the object allocator.
//:
//: 2 Every object releases any allocated memory at destruction.
//:
//: 3 No accessor allocates any memory.
//:
//: 4 All memory allocation is exception neutral.
//:
//: 5 QoI: The default constructor allocates no memory.
//:
//: 6 QoI: When possible, memory is cached for reuse.
//..
// Notice that some of these concerns (e.g., C-5..6) are not part of the
// class's documented, contractual behavior.  These are classified as Quality
// of Implementation (QoI) concerns.
//
// Next, we define a test plan.  For example, a plan to test these concerns is:
//..
// Plan:
//: 1 Setup global and default allocators:
//:
//:   1 Create two 'bslma_TestAllocator' objects and, for each of these, create
//:     an associated 'bslma_TestAllocatorMonitor' object.
//:
//:   2 Install the two allocators as the global and default allocators.
//:
//: 2 Confirm that default construction allocates no memory: (C-5)
//:
//:   1 Construct a 'bslma_TestAllocatorMonitor' object to be used passed to
//:     test objects on their construction, and an associated
//:
//:   2 In an inner block, default construct an object of 'MyClass' using the
//:     designated "object" test allocator.
//:
//:   3 Allow the object to go out of scope (destroyed).  Confirm that no
//:     memory has been allocated from any of the allocators.
//:
//: 3 Exercise an object of 'MyClass' such that memory should be allocated, and
//:   then confirm that the object allocator (only) is used: (C-2..4,6)
//:
//:   1 In another inner block, default construct a new test object using the
//:     (as yet unused) object allocator.
//:
//:   2 Force the test object to allocate memory by setting its 'descriptor'
//:     attribute to a value whose size exceeds the size of the object itself.
//:     Confirm that the attribute was set and that memory was allocated.
//:
//:   3 Confirm the the primary manipulator (the 'setDescription' method) is
//:     exception neutral (i.e., exceptions from the allocator are propagated
//:     and no memory is leaked).  Use the
//:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros to manage the test,
//:     and use the test allocator monitor to confirm that memory is allocated
//:     on the no-exception code path.  (C-4)
//:
//:   4 When the object is holding memory, create an additional test allocator
//:     monitor allocator for the object allocator.  Use the basic accessor
//:     (i.e., the 'description' method) to confirm that the object has the
//:     expected value.  Check this test allocator monitor to confirm that
//:     accessor allocated no memory.  (C-3)
//:
//:   5 Change the attribute to a smaller value and confirm that the current
//:     memory was reused (i.e., no memory is allocated). (C-6)
//:
//:   6 Destroy the test object by allowing it to go out of scope, and confirm
//:     that all allocations are returned.  (C-2)
//:
//: 4 Confirm that at no time were the global allocator or the default
//:   allocator were used.  (C-1)
//..
// The implementation of the plan is shown below:
//
// Then, we implement the first portion of the plan.  We create the trio of
// test allocators, their respective test allocator monitors, and install two
// of the allocators as the global and default allocators:
//..
    {
        if (verbose) cout << "Setup global and default allocators" << endl;

        bslma_TestAllocator        ga("global",  veryVeryVeryVerbose);
        bslma_TestAllocator        da("default", veryVeryVeryVerbose);
        bslma_TestAllocatorMonitor gam(&ga);
        bslma_TestAllocatorMonitor dam(&da);

        bslma_Default::setGlobalAllocator(&ga);
        bslma_Default::setDefaultAllocatorRaw(&da);
//..
// Then, we default construct a test object using the object allocator, and
// then, immediately destroy it.  The object allocator monitor, 'oam', shows
// that the allocator was not used.
//..
        if (verbose) cout << "No allocation by Default Constructor " << endl;

        bslma_TestAllocator        oa("object", veryVeryVeryVerbose);
        bslma_TestAllocatorMonitor oam(&oa);

        {
            MyClass obj(&oa);
            ASSERT(oam.isTotalSame()); // object allocator unused
        }
//..
// Next, we pass the (still unused) object allocator to another test object.
// This time, we coerce the object into allocating memory by setting an
// attribute.  (Setting an attribute larger than the receiving object
// means that the object cannot store the data within its own footprint and
// must allocate memory.)
//..
        if (verbose) cout << "Exercise object" << endl;

        {
            MyClass obj(&oa);

            const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            ASSERT(sizeof(obj) < sizeof(DESCRIPTION1));

            if (veryVerbose) cout << "\tPrimary Manipulator Allocates" << endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                obj.setDescription(DESCRIPTION1);
                ASSERT(oam.isTotalUp());  // object allocator was used
                ASSERT(oam.isInUseUp());  // some outstanding allocation(s)
                ASSERT(oam.isMaxUp());    // a maximum was set
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
//..
// Notice, as expected, memory was allocated from object allocator.
//
// Now that the allocator has been used, we create a second monitor to capture
// the that state.  Confirm that the basic accessor (the 'description' method)
// does not use the allocator.
//..
            if (veryVerbose) cout << "\tBasic Accessor does not allocate"
                                  << endl;

            bslma_TestAllocatorMonitor oam2(&oa); // Captures state of 'oa'
                                                  // with outstanding
                                                  // allocations.

            ASSERT(0 == strcmp(DESCRIPTION1, obj.description()));
            ASSERT(oam2.isTotalSame());  // object allocator was not used
//..
// Next, confirm that when a shorter value is assigned, the existing memory
// is reused.
//..

            obj.setDescription("a");
            ASSERT(0 == std::strcmp("a", obj.description()));

            ASSERT(oam2.isTotalSame());  // no allocations
//..
// Notice that there are no allocations because the object had sufficient
// capacity in previously allocated memory to store the short string.
//
// Next, as an additional test, we make the object allocate additional memory
// by setting a longer attribute: one that exceeds the capacity allocated for
// 'DESCRIPTION1'.  Use the second monitor to confirm that an allocation was
// performed.
//
// There are tests where using a test allocator monitor does not suffice.
// Our test object is currently holding memory, if we assign a value that
// exceeds its current capacity there will be two operations on the object
// allocator: the allocation of larger memory, and the deallocation of its
// current memory: in that order, as part of the strong exception guarantee.
// Thus, the maximum number of allocations should go up by one, and no more.
//
// Note that absence of memory leaks due to exceptions (the other part of the
// strong exception guarantee is confirmed during the destruction of the object
// test allocator at the end of this test, which featured exceptions.
//..
            bsls_Types::Int64 maxBeforeSet   = oa.numBlocksMax();
            const char        DESCRIPTION2[] = "abcdefghijklmnopqrstuvwyz"
                                               "abcdefghijklmnopqrstuvwyz"
                                               "abcdefghijklmnopqrstuvwyz"
                                               "abcdefghijklmnopqrstuvwyz"
                                               "abcdefghijklmnopqrstuvwyz";
            ASSERT(sizeof(DESCRIPTION1) < sizeof(DESCRIPTION2));

            obj.setDescription(DESCRIPTION2);
            ASSERT(0 == std::strcmp(DESCRIPTION2, obj.description()));

            ASSERT(oam2.isTotalUp());    // The object allocator used.

            ASSERT(oam2.isInUseSame());  // The outstanding block (allocation)
                                         // count unchanged (even though byte
                                         // outstanding byte count increased).

            ASSERT(oam2.isMaxUp());      // Max increased as expected, but was
                                         // did it change only by one?  The
                                         // monitor cannot answer that
                                         // question.

            bsls_Types::Int64 maxAfterSet = oa.numBlocksMax();

            ASSERT(1 == maxAfterSet - maxBeforeSet);
//..
// Notice that our test allocator monitor cannot confirm that the allocator's
// maximum increased by exactly one.  In this case, we must extract our
// statistics directly from the test allocator.
//
// Note that increment in "max" occurs only the first time through the the
// allocate/deallocate scenario in 'setDescription'.
//..
            bslma_TestAllocatorMonitor oam3(&oa);

            const char DESCRIPTION3[] = "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz"
                                        "abcdefghijklmnopqrstuvwyz";
            ASSERT(sizeof(DESCRIPTION2) < sizeof(DESCRIPTION3));

            obj.setDescription(DESCRIPTION3);
            ASSERT(0 == std::strcmp(DESCRIPTION3, obj.description()));

            ASSERT(oam3.isTotalUp());    // The object allocator used.

            ASSERT(oam3.isInUseSame());  // The outstanding block (allocation)
                                         // count unchanged (even though byte
                                         // outstanding byte count increased).

            ASSERT(oam3.isMaxSame());    // A repeat of the scenario for
                                         // 'DESCRIPTION2', so no change in the
                                         // allocator's maximum.
//..
// Now, we close scope and check that all object memory was deallocated
//..
        }

        if (veryVerbose) cout << "\tAll memory returned object allocator"
                              << endl;

        ASSERT(oam.isInUseSame());
//..
// Finally, we check that none of these operations used the default or global
// allocators.
//..
        if (verbose) cout << "Global and Default allocators never used"
                          << endl;

        ASSERT(gam.isTotalSame());
        ASSERT(dam.isTotalSame());
    }
//..

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ACCESSORS
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
        //: 2 Any allocation(s) from the tracked allocator idempotently flip
        //:   the monitor's 'isMaxUp' and 'isTotalUp' methods return values to
        //:   'true'.
        //:
        //: 3 The "inUse" statistic tracks changes both above and below the
        //:   number of allocations recorded when the monitor was created.
        //:
        //: 4 The return values depend for each statistic depend on the number
        //:   of allocations and deallocations, never on the size (in bytes) of
        //:   the allocations.
        //:
        // Plan:
        //: 1 Always test related test values in concert.
        //:   1 For the "Max" and "Total" statistics, check both "Same" and
        //:     "Up" accessors.  Only one should be 'true'.  To express this
        //:     concisely, the standard 'ASSERT' test macro is used via the
        //:     'A', macro.
        //:   2 For the "InUse" statistics, check "Same", "Up" and "Down". one
        //:     should be 'true'.  (C-1)
        //:
        //: 2 Create a test allocator, perform several allocations (retaining
        //:   the returned memory addresses), and then create a test allocator
        //:   monitor.  Exercise the test allocator and check the return values
        //:   from the accessors.  The scenarios should include:
        //:   1 Multiple allocations and deallocations, to confirm that once
        //:     "isMaxUp' and 'isTotal' up return 'true', they remain 'true'.
        //:     (C-2)
        //:   2 Multiple of deallocations and reallocations so that the "InUse"
        //:     statistic can be observed to go from "Same" to "Down" and back.
        //:     (C-3)
        //:   3 The return of an allocation followed by another allocation of
        //:     greater size.  (C-4)
        //
        // Testing:
        //   bool isInUseDown() const;
        //   bool isInUseSame() const;
        //   bool isInUseUp() const;
        //   bool isMaxSame() const;
        //   bool isMaxUp() const;
        //   bool isTotalSame() const;
        //   bool isTotalUp() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ACCESSORS" << endl
                          << "=========" << endl;

        if (veryVerbose) cout << "\tSetup Test Allocator and Monitor."
                              << endl;

        Ta ta("testAllocator", veryVeryVeryVerbose);

        bsls_Types::size_type sizeSmall  =    1;
        bsls_Types::size_type sizeMedium =   10;
        bsls_Types::size_type sizeLarge  =  100;
        bsls_Types::size_type sizeLarger = 1000;

        ASSERT(sizeSmall  < sizeMedium);
        ASSERT(sizeMedium < sizeLarge);
        ASSERT(sizeLarge  < sizeLarger);

        void *prior1 = ta.allocate(sizeSmall);
        void *prior2 = ta.allocate(sizeMedium);
        void *prior3 = ta.allocate(sizeLarge);

        Tam tam(&ta);

        if (veryVerbose) cout << "\tCheck Initial State." << endl;

        ASSERT( tam.isTotalSame());  // Same Total
        ASSERT(!tam.isTotalUp());

        ASSERT( tam.isInUseSame());  // Same InUse
        ASSERT(!tam.isInUseUp());
        ASSERT(!tam.isInUseDown());

        ASSERT( tam.isMaxSame());    // Same Max
        ASSERT(!tam.isMaxUp());

        if (veryVerbose) cout << "\tStatics track blocks not bytes." << endl;

        ta.deallocate(prior3); prior3 = 0;

        ASSERT( tam.isTotalSame());  // Same Total
        ASSERT(!tam.isTotalUp());    // deallocs not counted

        ASSERT(!tam.isInUseSame());
        ASSERT(!tam.isInUseUp());
        ASSERT( tam.isInUseDown());  // Down InUse (changed)

        ASSERT( tam.isMaxSame());    // Same Max
        ASSERT(!tam.isMaxUp());

        void *post1 = ta.allocate(sizeLarger);  // "Balances" 'prior3'.
                                                // First alloc w.r.t. monitor.

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total (changed)

        ASSERT( tam.isInUseSame());  // Same InUse (changed)
        ASSERT(!tam.isInUseUp());
        ASSERT(!tam.isInUseDown());

        ASSERT( tam.isMaxSame());    // Same Max
        ASSERT(!tam.isMaxUp());

        if (veryVerbose) cout << "\t\"TotalUp\" and \"MaxUp\" remain 'true'."
                              << endl;

        void *post2 = ta.allocate(sizeMedium);

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total (changed)

        ASSERT(!tam.isInUseSame());
        ASSERT( tam.isInUseUp());    // Up InUse (changed)
        ASSERT(!tam.isInUseDown());

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max (changed)

        void *post3 = ta.allocate(sizeSmall);

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT(!tam.isInUseSame());
        ASSERT( tam.isInUseUp());    // Up InUse
        ASSERT(!tam.isInUseDown());

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        if (veryVerbose) cout
                          << "\t\"inUse\" reported w.r.t. initial block count"
                          << endl;

        ta.deallocate(post2); post2 = 0;
        ta.deallocate(post3); post3 = 0;

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT( tam.isInUseSame());  // Same InUse (changed)
        ASSERT(!tam.isInUseUp());
        ASSERT(!tam.isInUseDown());

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        // Now deallocate 'post1', which had balanced the deallocated 'prior3'.
        ta.deallocate(post1); post1 = 0;

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT(!tam.isInUseSame());
        ASSERT(!tam.isInUseUp());
        ASSERT( tam.isInUseDown());  // Down InUse

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        // Drive "inUse" count further below initial value.
        ta.deallocate(prior1); prior1 = 0;

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT(!tam.isInUseSame());
        ASSERT(!tam.isInUseUp());
        ASSERT( tam.isInUseDown());  // Down InUse

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        // Drive "inUse" count even further below initial value.
        ta.deallocate(prior2); prior2 = 0;

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT(!tam.isInUseSame());
        ASSERT(!tam.isInUseUp());
        ASSERT( tam.isInUseDown());  // Down InUse

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        // Three allocations drive the "InUse" count to initial value;
        void *final1 = ta.allocate(sizeSmall);
        void *final2 = ta.allocate(sizeSmall);
        void *final3 = ta.allocate(sizeSmall);

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT( tam.isInUseSame());  // Same InUse (changed)
        ASSERT(!tam.isInUseUp());
        ASSERT(!tam.isInUseDown());

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        // As a final test, one more allocation drives it "Up" yet again.
        void *final4 = ta.allocate(sizeSmall);

        ASSERT(!tam.isTotalSame());
        ASSERT( tam.isTotalUp());    // Up Total

        ASSERT(!tam.isInUseSame());
        ASSERT( tam.isInUseUp());    // Up InUse (changed)
        ASSERT(!tam.isInUseDown());

        ASSERT(!tam.isMaxSame());
        ASSERT( tam.isMaxUp());      // Up Max

        if (veryVerbose) cout << "Clean up." << endl;

        ta.deallocate(final1); final1 = 0;
        ta.deallocate(final2); final2 = 0;
        ta.deallocate(final3); final3 = 0;
        ta.deallocate(final4); final4 = 0;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR and DTOR
        //   Ensure that we can use the constructor to create a object that
        //   monitors the intended test allocator, and use the destructor to
        //   destroy it safely.
        //
        // Concerns:
        //: 1 A monitor object is associated with the specified test allocator.
        //:
        //: 2 Object can be safely destroyed.
        //:
        //: 3 The constructor accurately captures the current state of the
        //:   tracked allocator, be it a newly created allocator or one that
        //:   has experienced arbitrary use.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a test allocator object and, before it handles any
        //:   allocation request, create an associated test allocator monitor
        //:   object.  Use the (as yet unproven) "same" accessors to confirm
        //:   that the monitor is the expected initial state.  Exercise the
        //:   allocator and check for the expected change of state in the the
        //:   associated monitor.  (C-1) Finally, destroy the monitor by
        //:   allowing it to go out of scope.  (C-2)
        //:
        //: 2 For each in a set of two test allocators, one newly created and
        //:   the other having experienced arbitrary use, create a tracking
        //:   test allocator monitor, exercise the test allocator and check for
        //:   the expected change in return value from the (as yet unproved)
        //:   accessors.  Note that the pointer returned in prior use, if any,
        //:   must be retained for later deallocation.
        //:    1 Confirm that the newly created monitor's "Same" allocators
        //:      all return 'true'.
        //:    2 Confirm that after an arbitrary allocation
        //:      the all the "Up" accessors return 'true'.
        //:    3 Confirm that after this recent allocation is deallocated,
        //:      the "InUseSame" accessor returns 'true' and the "Up"
        //:      accessors for "Max" and "Total" return 'true'.
        //:    4 Confirm that after deallocation of the memory allocated before
        //:      creation of the monitor, the "InUseDown" accessor returns
        //:     'true'  and the "Up" accessors for "Max" and "Total" return
        //:     'true'.
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   bslma_TestAllocatorMonitor(const bslma_TestAllocator *tA);
        //   ~bslma_TestAllocatorMonitor();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CTOR & DTOR" << endl
                          << "===========" << endl;

        if (verbose) cout << "\nCtor, Associated Ta, & Dtor." << endl;
        {
            Ta  ta("testAllocator", veryVeryVeryVerbose);
            Tam tam(&ta);

            ASSERT(tam.isTotalSame());
            ASSERT(tam.isInUseSame());
            ASSERT(tam.isMaxSame());

            void *allocation1 = ta.allocate(1);

            ASSERT(tam.isTotalUp());
            ASSERT(tam.isInUseUp());
            ASSERT(tam.isMaxUp());

            ta.deallocate(allocation1);

            ASSERT(tam.isTotalUp());
            ASSERT(tam.isInUseSame());
            ASSERT(tam.isMaxUp());
        }

        if (verbose) cout << "\nProper State Saved on Construction." << endl;
        {
            Ta    ta1("ta1", veryVeryVeryVerbose);  // Do not use yet.
            Ta    ta2("ta2", veryVeryVeryVerbose);
            void *p2  = ta2.allocate((bsls_Types::size_type) &ta2 & 0xFFFF);

            const struct {
                int   d_line;
                Ta*   d_ta_p;
                void* d_mem_p;
            } DATA[] = {

              //LINE  ALLOCP  PRIOR
              //----  ------  -----
              { L_,   &ta1,   0    },
              { L_,   &ta2,   p2   }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  =  DATA[ti].d_line;
                Ta&       TA    = *DATA[ti].d_ta_p;
                void     *PRIOR =  DATA[ti].d_mem_p;

                Tam tam(&TA);

                ASSERT(tam.isTotalSame());
                ASSERT(tam.isInUseSame());
                ASSERT(tam.isMaxSame());

                void *p = TA.allocate((bsls_Types::size_type) &TA & 0xFFFF);

                ASSERT(tam.isTotalUp());
                ASSERT(tam.isInUseUp());
                ASSERT(tam.isMaxUp());

                TA.deallocate(p);

                ASSERT(tam.isTotalUp());
                ASSERT(tam.isInUseSame());
                ASSERT(tam.isMaxUp());

                if (PRIOR) {
                    TA.deallocate(PRIOR);

                    ASSERT(tam.isTotalUp());
                    ASSERT(tam.isInUseDown());
                    ASSERT(tam.isMaxUp());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'constructor'" << endl;
            {
                Ta ta("testAllocator", veryVeryVeryVerbose);
                ASSERT_SAFE_PASS((Tam)(&ta));
                ASSERT_SAFE_FAIL(Tam((const bslma_TestAllocator *)0));
            }
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
        //: 1 Create a 'bslma_TestAllocator' object and a
        //:   'bslma_TestAllocatorMonitor' object to track the
        //:   'bslma_TestAllocator' object and a
        //: 2 Perform several allocations from and deallocations to the the
        //:   test allocator created in P-1.  Once the test allocator has been
        //:   used, create a second test allocator monitor to trace changes
        //:   from that non-initial state.  At each point, confirm that the
        //:   test allocator monitor object created in P-2 returns 'true' from
        //:   the appropriate methods.
        //: 3 Destroy the test allocator monitors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma_TestAllocator        ta("test allocator", veryVeryVeryVerbose);
        bslma_TestAllocatorMonitor tamA(&ta);

        ASSERT(tamA.isTotalSame());
        ASSERT(tamA.isInUseSame());
        ASSERT(tamA.isMaxSame());

        void *allocation1 = ta.allocate(1);

        bslma_TestAllocatorMonitor tamB(&ta);

        ASSERT(tamA.isTotalUp());   ASSERT(tamB.isTotalSame());
        ASSERT(tamA.isInUseUp());   ASSERT(tamB.isInUseSame());
        ASSERT(tamA.isMaxUp());     ASSERT(tamB.isMaxSame());

        void *allocation2 = ta.allocate(2);

        ASSERT(tamA.isTotalUp());   ASSERT(tamB.isTotalUp());
        ASSERT(tamA.isInUseUp());   ASSERT(tamB.isInUseUp());
        ASSERT(tamA.isMaxUp());     ASSERT(tamB.isMaxUp());

        ta.deallocate(allocation2);

        ASSERT(tamA.isTotalUp());   ASSERT(tamB.isTotalUp());
        ASSERT(tamA.isInUseUp());   ASSERT(tamB.isInUseSame());
        ASSERT(tamA.isMaxUp());     ASSERT(tamB.isMaxUp());

        ta.deallocate(allocation1);

        ASSERT(tamA.isTotalUp());   ASSERT(tamB.isTotalUp());
        ASSERT(tamA.isInUseSame()); ASSERT(tamB.isInUseDown());
        ASSERT(tamA.isMaxUp());     ASSERT(tamB.isMaxUp());

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
