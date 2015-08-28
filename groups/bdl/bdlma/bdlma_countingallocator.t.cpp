// bdlma_countingallocator.t.cpp                                      -*-C++-*-
#include <bdlma_countingallocator.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <crtdbg.h>  // '_CrtSetReportMode', to suppress popups
#else
#include <pthread.h>
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// 'bdlma::CountingAllocator' is a special-purpose allocator mechanism that
// tracks the number of bytes currently in use and the cumulative number of
// bytes that have ever been allocated.  The primary concerns are that these
// two byte counts are correctly maintained, that 'allocate' returns maximally-
// aligned memory blocks of the expected size from the object allocator, and
// that 'deallocate' returns memory blocks back to the object allocator.  We
// make heavy use of the 'bslma::TestAllocator' to ensure that these concerns
// are satisfied.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] CountingAllocator(Allocator *ba = 0);
// [ 4] CountingAllocator(const char *name, Allocator *ba = 0);
// [ 2] ~CountingAllocator();
//
// MANIPULATORS
// [ 3] void *allocate(size_type size);
// [ 3] void deallocate(void *address);
//
// ACCESSORS
// [ 4] const char *name() const;
// [ 3] Int64 numBytesInUse() const;
// [ 3] Int64 numBytesTotal() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: There is no temporary allocation from any allocator.
// [ 4] CONCERN: Precondition violations are detected when enabled.
// [ 6] CONCERN: The 'allocate' and 'deallocate' methods are thread-safe.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlma::CountingAllocator Obj;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
ThreadId createThread(ThreadFunction function, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)function, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, function, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

namespace TestCase6 {

struct ThreadInfo {
    int  d_numIterations;
    Obj *d_obj_p;
};

extern "C" void *threadFunction1(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;  const Obj& X = mX;

    int n = 2;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p = mX.allocate(n);  bsl::memset(p, 0xff, n);

        ASSERT(0 < X.numBytesInUse());
        ASSERT(0 < X.numBytesTotal());
        ASSERT(X.numBytesInUse() <= X.numBytesTotal());

        mX.deallocate(p);

        if (n > 10000) {
            n = 2;
        }
        else {
            n *= 2;
        }
    }

    return arg;
}

extern "C" void *threadFunction2(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;  const Obj& X = mX;

    int n = 3;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      bsl::memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 2);  bsl::memset(p2, 0xff, n * 2);

        ASSERT(0 < X.numBytesInUse());
        ASSERT(0 < X.numBytesTotal());
        ASSERT(X.numBytesInUse() <= X.numBytesTotal());

        mX.deallocate(p1);
        mX.deallocate(p2);

        if (n > 10000) {
            n = 3;
        }
        else {
            n *= 3;
        }
    }

    return arg;
}

extern "C" void *threadFunction3(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;  const Obj& X = mX;

    int n = 5;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      bsl::memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 3);  bsl::memset(p2, 0xff, n * 3);
        void *p3 = mX.allocate(n * 7);  bsl::memset(p3, 0xff, n * 7);

        ASSERT(0 < X.numBytesInUse());
        ASSERT(0 < X.numBytesTotal());
        ASSERT(X.numBytesInUse() <= X.numBytesTotal());

        mX.deallocate(p3);
        mX.deallocate(p2);
        mX.deallocate(p1);

        if (n > 10000) {
            n = 5;
        }
        else {
            n *= 5;
        }
    }

    return arg;
}

}  // close namespace TestCase6

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tracking a Container's Dynamic Memory Use
///----------------------------------------------------
// In this example, we demonstrate how a counting allocator may be used to
// track the amount of dynamic memory used by a container.  The container used
// for illustration is 'DoubleStack', a stack of out-of-place 'double' values.
//
// First, we show the interface of the 'DoubleStack' class:
//..
    // doublestack.h

    class DoubleStack {
        // This class implements a stack of out-of-place 'double' values.

        // DATA
        double           **d_stack_p;      // dynamically allocated array of
                                           // 'd_capacity' elements

        int                d_capacity;     // physical capacity of the stack
                                           // (in elements)

        int                d_length;       // logical index of next available
                                           // stack element

        bslma::Allocator  *d_allocator_p;  // memory allocator (held, not
                                           // owned)

        // NOT IMPLEMENTED
        DoubleStack(const DoubleStack&);
        DoubleStack& operator=(const DoubleStack&);

      private:
        // PRIVATE MANIPULATORS
        void increaseCapacity();
            // Increase the capacity of this stack by at least one element.

      public:
        // CREATORS
        explicit
        DoubleStack(bslma::Allocator *basicAllocator = 0);
            // Create a stack for 'double' values having an initial capacity to
            // hold one element.  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~DoubleStack();
            // Delete this object.

        // MANIPULATORS
        void push(double value);
            // Add the specified 'value' to the top of this stack.

        void pop();
            // Remove the element at the top of this stack.  The behavior is
            // undefined unless this stack is non-empty.

        // ACCESSORS
        // ...
    };
//..
// Next, we show the (elided) implementation of 'DoubleStack'.
//
// The default constructor creates a stack having the capacity for one element
// (the implementation of the destructor is not shown):
//..
    // doublestack.cpp
    // ...

    // TYPES
    enum { k_INITIAL_CAPACITY = 1, k_GROWTH_FACTOR = 2 };

    // CREATORS
    DoubleStack::DoubleStack(bslma::Allocator *basicAllocator)
    : d_stack_p(0)
    , d_capacity(k_INITIAL_CAPACITY)
    , d_length(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_stack_p = (double **)
                       d_allocator_p->allocate(d_capacity * sizeof *d_stack_p);
    }
//..
// The 'push' method first ensures that the array has sufficient capacity to
// accommodate an additional value, then allocates a block in which to store
// that value:
//..
    // MANIPULATORS
    void DoubleStack::push(double value)
    {
        if (d_length >= d_capacity) {
            increaseCapacity();
        }
        double *stackValue = (double *)d_allocator_p->allocate(sizeof(double));
        *stackValue = value;
        d_stack_p[d_length] = stackValue;
        ++d_length;
    }
//..
// The 'pop' method asserts that the stack is not empty before deallocating the
// block used to store the element at the top of the stack:
//..
    void DoubleStack::pop()
    {
        BSLS_ASSERT(0 < d_length);

        d_allocator_p->deallocate(d_stack_p[d_length - 1]);
        --d_length;
    }
//..
// The 'push' method (above) made use of the private 'increaseCapacity' method,
// which, in turn, makes use of the 'reallocate' helper function ('static' to
// the '.cpp' file).  Note that 'increaseCapacity' (below) increases the
// capacity of the 'double *' array by a factor of 2 each time that it is
// called:
//..
    // HELPER FUNCTIONS
    static
    void reallocate(double           ***array,
                    int                 newCapacity,
                    int                 length,
                    bslma::Allocator   *allocator)
        // Reallocate memory in the specified 'array' to accommodate the
        // specified 'newCapacity' elements using the specified 'allocator'.
        // The specified 'length' number of leading elements are preserved.
        // The behavior is undefined unless 'newCapacity > length'.
    {
        BSLS_ASSERT(newCapacity > length);

        double **tmp = *array;
        *array = (double **)allocator->allocate(newCapacity * sizeof **array);
        bsl::memcpy(*array, tmp, length * sizeof **array);  // commit
        allocator->deallocate(tmp);
    }

    // PRIVATE MANIPULATORS
    void DoubleStack::increaseCapacity()
    {
        const int newCapacity = d_capacity * k_GROWTH_FACTOR;
                                                        // reallocate can throw
        reallocate(&d_stack_p, newCapacity, d_length, d_allocator_p);
        d_capacity = newCapacity;                       // commit
    }
//..

DoubleStack::~DoubleStack()
{
    // CLASS INVARIANTS
    BSLS_ASSERT(d_stack_p);
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(0 <  d_capacity);
    BSLS_ASSERT(d_length <= d_capacity);

    for (int i = d_length - 1; i >= 0; --i) {
        d_allocator_p->deallocate(d_stack_p[i]);
    }
    d_allocator_p->deallocate(d_stack_p);
}

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

// Now, we are ready to employ a 'CountingAllocator' to illustrate the dynamic
// memory use of 'DoubleStack'.  We first define two constants that facilitate
// portability of this example across 32- and 64-bit platforms:
//..
    const int DBLSZ = sizeof(double);
    const int PTRSZ = sizeof(double *);
//..
// First, we define a 'CountingAllocator', 'ca'.  At construction, a counting
// allocator can be configured with an optional name and an optional allocator.
// In this case, we give 'ca' a name to distinguish it from other counting
// allocators, but settle for using the default allocator:
//..
    bdlma::CountingAllocator ca("'DoubleStack' Allocator");
//..
// Next, we create a 'DoubleStack', supplying it with 'ca', and assert the
// expected memory use incurred by the default constructor:
//..
    DoubleStack stack(&ca);
                           ASSERT(1 * PTRSZ             == ca.numBytesInUse());
                           ASSERT(1 * PTRSZ             == ca.numBytesTotal());
//..
// Next, we push an element onto the stack.  The first push incurs an
// additional allocation to store (out-of-place) the value being inserted:
//..
    stack.push(1.54);      ASSERT(1 * PTRSZ + 1 * DBLSZ == ca.numBytesInUse());
                           ASSERT(1 * PTRSZ + 1 * DBLSZ == ca.numBytesTotal());
//..
// Next, we push a second element onto the stack.  In this case, two
// allocations result, one due to the resizing of the internal array and one
// required to store the new value out-of-place:
//..
    stack.push(0.99);      ASSERT(2 * PTRSZ + 2 * DBLSZ == ca.numBytesInUse());
                           ASSERT(3 * PTRSZ + 2 * DBLSZ == ca.numBytesTotal());
//..
// Next, we pop the top-most element from the stack.  The number of bytes in
// use decreases by the amount used to store the popped element out-of-place:
//..
    stack.pop();           ASSERT(2 * PTRSZ + 1 * DBLSZ == ca.numBytesInUse());
                           ASSERT(3 * PTRSZ + 2 * DBLSZ == ca.numBytesTotal());
//..
// Finally, we print the state of 'ca' to standard output:
//..
if (veryVerbose)
    ca.print(bsl::cout);
//..
// which displays the following on a 32-bit platform:
//..
//  ----------------------------------------
//                          Counting Allocator State
//  ----------------------------------------
//  Allocator name: 'DoubleStack' Allocator
//  Bytes in use:   16
//  Bytes in total: 28
//..

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CONCURRENCY
        //   Ensure that 'allocate' and 'deallocate' are thread-safe.
        //
        // Concerns:
        //: 1 That 'allocate' and 'deallocate' are thread-safe.  (Note that
        //:   although all methods of 'bdlma::CountingAllocator' are
        //:   thread-safe, the thread safety of 'allocate' and 'deallocate' are
        //:   of paramount concern.)
        //
        // Plan:
        //: 1 Create a 'bdlma::CountingAllocator'.
        //:
        //: 2 Within a loop, create three threads that iterate a specified
        //:   number of times and that perform a different sequence of
        //:   allocation and deallocation operations on the allocator from P-1.
        //:
        //: 3 After each iteration, use the accessors to verify the expected
        //:   state of the counting allocator.  (C-1)
        //
        // Testing:
        //   CONCERN: The 'allocate' and 'deallocate' methods are thread-safe.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCURRENCY" << endl
                          << "===========" << endl;

        using namespace TestCase6;

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&sa);  const Obj& X = mX;

        const int NUM_TEST_ITERATIONS   =   10;
        const int NUM_THREAD_ITERATIONS = 1000;

        ThreadInfo info = { NUM_THREAD_ITERATIONS, &mX };

        for (int ti = 0; ti < NUM_TEST_ITERATIONS; ++ti) {
            ThreadId id1 = createThread(&threadFunction1, &info);
            ThreadId id2 = createThread(&threadFunction2, &info);
            ThreadId id3 = createThread(&threadFunction3, &info);

            joinThread(id1);
            joinThread(id2);
            joinThread(id3);

            ASSERT(0 == X.numBytesInUse());
            ASSERT(0  < X.numBytesTotal());

            ASSERT(0 == sa.numBlocksInUse());
            ASSERT(0 == da.numBlocksTotal());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT METHOD
        //   Ensure that the counting allocator properties can be formatted
        //   appropriately on an 'ostream' in a reasonable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the allocator properties to the
        //:   specified 'ostream'.
        //:
        //: 2 The 'print' method writes the allocator properties in the
        //:   intended format.
        //:
        //: 3 The 'print' method has the expected signature and return type.
        //:
        //: 4 The 'print' method returns the supplied 'ostream'.
        //:
        //: 5 The 'print' method allocates no memory from the object
        //:   allocator.
        //
        // Plan:
        //: 1 Use the address of the 'print' member function to initialize
        //:   a member-function pointer having the appropriate signature and
        //:   return type.  (C-3)
        //:
        //: 2 Using brute force, configure three objects for printing: the
        //:   first is *unnamed* and is in the default constructed state, the
        //:   second is *unnamed* and has one outstanding memory block out of
        //:   two total allocations, and the third is *named* and has one
        //:   outstanding memory block out of two total allocations.
        //:
        //: 3 Print each of the objects defined in P-2 to a supplied
        //:   'ostringstream' and verify that the output matches what is
        //:   expected, and that the method returns a reference to the
        //:   'ostream' that was supplied.  (C-1..2, 4)
        //:
        //: 4 Use a 'bslma::TestAllocatorMonitor' to verify that 'print'
        //:   allocates no memory from the object allocator.  (C-5)
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT METHOD" << endl
                          << "============" << endl;

        if (veryVerbose) cout << "\tVerify 'print' signature and return type."
                              << endl;
        {
            typedef bsl::ostream& (Obj::*funcPtr)(bsl::ostream&) const;

            funcPtr printMember = &Obj::print;

            (void)printMember;  // quash potential compiler warnings
        }

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (veryVerbose) cout << "\tUnnamed allocator w/o allocation." << endl;
        {
            bsl::ostringstream os;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            bslma::TestAllocatorMonitor sam(&sa);

            ASSERT(&os == &X.print(os));

            ASSERT(sam.isInUseSame());
            ASSERT(sam.isTotalSame());

            const char *EXPECTED =
                "----------------------------------------\n"
                "        Counting Allocator State\n"
                "----------------------------------------\n"
                "Bytes in use:   0\n"
                "Bytes in total: 0\n";

            if (veryVerbose) {
                cout << "ACTUAL:"   << endl << os.str() << endl;
                cout << "EXPECTED:" << endl << EXPECTED << endl;
            }

            ASSERT(EXPECTED == os.str());
        }

        if (veryVerbose) cout << "\tUnnamed allocator w/allocation." << endl;
        {
            bsl::ostringstream os;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            void *p1 = mX.allocate( 8);
            void *p2 = mX.allocate(56);  mX.deallocate(p2);

            bslma::TestAllocatorMonitor sam(&sa);

            ASSERT(&os == &X.print(os));

            ASSERT(sam.isInUseSame());
            ASSERT(sam.isTotalSame());

            const char *EXPECTED =
                "----------------------------------------\n"
                "        Counting Allocator State\n"
                "----------------------------------------\n"
                "Bytes in use:   8\n"
                "Bytes in total: 64\n";

            if (veryVerbose) {
                cout << "ACTUAL:"   << endl << os.str() << endl;
                cout << "EXPECTED:" << endl << EXPECTED << endl;
            }

            ASSERT(EXPECTED == os.str());

            mX.deallocate(p1);
        }

        if (veryVerbose) cout << "\tNamed allocator w/allocation." << endl;
        {
            bsl::ostringstream os;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX("<named>", &sa);  const Obj& X = mX;

            void *p1 = mX.allocate(101);
            void *p2 = mX.allocate(202);  mX.deallocate(p2);

            bslma::TestAllocatorMonitor sam(&sa);

            ASSERT(&os == &X.print(os));

            ASSERT(sam.isInUseSame());
            ASSERT(sam.isTotalSame());

            const char *EXPECTED =
                "----------------------------------------\n"
                "        Counting Allocator State\n"
                "----------------------------------------\n"
                "Allocator name: <named>\n"
                "Bytes in use:   101\n"
                "Bytes in total: 303\n";

            if (veryVerbose) {
                cout << "ACTUAL:"   << endl << os.str() << endl;
                cout << "EXPECTED:" << endl << EXPECTED << endl;
            }

            ASSERT(EXPECTED == os.str());

            mX.deallocate(p1);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // NAMING CTOR AND NAME ACCESSOR
        //   Ensure that a counting allocator can be named and that the name is
        //   accessible through the 'name' method.
        //
        // Concerns:
        //: 1 If an allocator is NOT supplied to the naming constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 2 If an allocator IS supplied to the naming constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 3 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 4 The naming constructor allocates no memory.
        //:
        //: 5 Any memory allocation is from the object allocator.
        //:
        //: 6 There is no temporary allocation from any allocator.
        //:
        //: 7 The byte counts are initialized to 0.
        //:
        //: 8 An object created using the naming constructor has the supplied
        //:   name, which is accessible via the 'name' method.
        //:
        //: 9 The 'name' accessor allocates no memory.
        //:
        //:10 The 'name' accessor returns 0 for an object created using the
        //:   default constructor.
        //:
        //:11 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a loop-based approach, name-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..9)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the naming constructor to dynamically create an object
        //:     'mX', with its object allocator configured appropriately (see
        //:     P-1); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the naming constructor.  (C-4)
        //:
        //:   4 Use the 'numBytesInUse' and 'numBytesTotal' accessors to verify
        //:     that the byte counts are initialized to 0.  (C-7)
        //:
        //:   5 Verify that the 'name' accessor returns the name supplied at
        //:     construction and that it allocates no memory.  (C-8..9)
        //:
        //:   6 Allocate a block from 'mX' and verify that the memory is
        //:     allocated from the object allocator.  (C-1..3, 5)
        //:
        //:   7 Verify that no temporary memory is allocated from any
        //:     allocator.  (C-6)
        //:
        //: 2 Provide a separate test that 'name' returns 0 for an object
        //:   created using the default constructor.  (C-10)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered.  (C-11)
        //
        // Testing:
        //   CountingAllocator(const char *name, Allocator *ba = 0);
        //   const char *name() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NAMING CTOR AND NAME ACCESSOR" << endl
                          << "=============================" << endl;

        const char *NAME = "Test Name";

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj(NAME);
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(NAME, 0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(NAME, &sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }
            LOOP_ASSERT(CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify byte counts are initialized to 0.

            LOOP_ASSERT(CONFIG, 0 == X.numBytesInUse());
            LOOP_ASSERT(CONFIG, 0 == X.numBytesTotal());

            // Verify 'name' accessor.

            LOOP2_ASSERT(CONFIG, X.name(), 0 == bsl::strcmp(NAME, X.name()));

            // Verify no allocation from the object allocator.

            LOOP2_ASSERT(CONFIG, oa.numBlocksTotal(),
                         0 == oa.numBlocksTotal());

            // Verify no allocation from the non-object allocator.

            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Verify we can allocate from the object and write to the returned
            // memory block.

            void *p = mX.allocate(1);
            LOOP2_ASSERT(CONFIG, p, p);
            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         1 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            *static_cast<char *>(p) = 'x';  // sanity check

            // Deallocate to avoid memory leak.

            mX.deallocate(p);

            LOOP2_ASSERT(CONFIG,  oa.numBlocksInUse(),
                         0 ==  oa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Destroy object and reclaim dynamically allocated footprint.

            fa.deleteObject(objPtr);

            // Verify all memory has been released.

            LOOP2_ASSERT(CONFIG,  fa.numBlocksInUse(),
                         0 ==  fa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG,  oa.numBlocksInUse(),
                         0 ==  oa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());
        }

        if (veryVerbose) cout << "\tUnnamed allocator." << endl;
        {
            Obj mX;  const Obj& X = mX;

            ASSERT(0 == X.name());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Obj(name, *ba)'" << endl;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            ASSERT_SAFE_FAIL_RAW(Obj((const char *)0     ));
            ASSERT_SAFE_FAIL_RAW(Obj(              0,   0));
            ASSERT_SAFE_FAIL_RAW(Obj(              0, &sa));

            ASSERT_SAFE_PASS_RAW(Obj(            "x",   0));
            ASSERT_SAFE_PASS_RAW(Obj(            "x", &sa));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALLOCATE, DEALLOCATE, AND BYTE COUNTS
        //   Ensure that 'allocate' and 'deallocate' work as expected, and that
        //   the byte counts are correctly maintained.
        //
        // Concerns:
        //: 1 Memory blocks returned by 'allocate' are obtained from the object
        //:   allocator.
        //:
        //: 2 Memory blocks returned by 'allocate' are of at least the
        //:   requested size (in bytes).
        //:
        //: 3 Memory blocks returned by 'allocate' are maximally aligned.
        //:
        //: 4 Calling 'allocate' with 0 returns 0 and has no effect.
        //:
        //: 5 'deallocate' returns memory to the object allocator.
        //:
        //: 6 Calling 'deallocate' with 0 has no effect.
        //:
        //: 7 Immediately following construction, the 'numBytesInUse' and
        //:   'numBytesTotal' accessors each return 0.
        //:
        //: 8 The 'allocate' and 'deallocate' methods each correctly update the
        //:   two byte counts ('numBytesInUse' and 'numBytesTotal').
        //:
        //: 9 There is no temporary allocation from any allocator.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of allocation/deallocation request sequences,
        //:     together with the in-use and total byte/block counts that are
        //:     expected to result from exercising each sequence on a newly-
        //:     constructed counting allocator.
        //:
        //: 2 For each row 'R' (representing an allocation/deallocation request
        //:   sequence, 'S') in the table described in P-1:  (C-1..3, 5, 7..9)
        //:
        //:   1 Create a 'bslma::TestAllocator' object and install it as the
        //:     current default allocator.
        //:
        //:   2 Create a 'bslma::TestAllocator' object 'sa'.
        //:
        //:   3 Use the default constructor and 'sa' to create a modifiable
        //:     'Obj' 'mX'.
        //:
        //:   4 Using the 'numBytesInUse' and 'numBytesTotal' accessors, verify
        //:     that the byte counts are initially 0.  (C-7)
        //:
        //:   5 Perform the sequence of allocation and deallocation requests
        //:     defined by 'S' (from P-2).
        //:
        //:   6 For each call to 'allocate' in P-2.5, verify that the returned
        //:     memory address is non-null and maximally aligned, that the
        //:     entire extent of the memory block can be written to, and that
        //:     the memory was allocated from the object allocator.  (C-1..3)
        //:
        //:   7 Upon completion of the allocation and deallocation sequence
        //:     'S', verify that the 'numBytesInUse' and 'numBytesTotal'
        //:     methods report the expected values for the two byte counts.
        //:     (P-8)
        //:
        //:   8 Verify, using the 'sa' test allocator, that 'deallocate'
        //:     returns memory to the object allocator.  (C-5)
        //:
        //:   9 Verify that no temporary memory is allocated from any
        //:     allocator.  (C-9)
        //:
        //: 3 Perform a separate brute-force test to verify that
        //:   'mX.allocate(0)' returns 0 and has no effect on any allocator.
        //:   (C-4)
        //:
        //: 4 Perform a separate brute-force test to verify that
        //:   'mX.deallocate(0)' has no effect.  (C-6)
        //
        // Testing:
        //   void *allocate(size_type size);
        //   void deallocate(void *address);
        //   Int64 numBytesInUse() const;
        //   Int64 numBytesTotal() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ALLOCATE, DEALLOCATE, AND BYTE COUNTS" << endl
                          << "=====================================" << endl;

        const int MAX_SEQ = 6;

        struct {
            int d_line;               // line number
            int d_numBytesInUse;      // # bytes in use
            int d_numBytesTotal;      // # bytes total
            int d_numBlocksInUse;     // # blocks in use
            int d_numBlocksTotal;     // # blocks total
            int d_sequence[MAX_SEQ];  // allocation/deallocation sequence
        } DATA[] = {
            //         BYTES          BLOCKS
            //LINE  IN USE TOTAL  IN USE TOTAL        TEST SEQUENCE
            //----  ------ -----  ------ -----  ------------------------
            { L_,       0,    0,     0,    0,   {                  -99 } },

            { L_,       1,    1,     1,    1,   {   1,             -99 } },
            { L_,       2,    2,     1,    1,   {   2,             -99 } },
            { L_,       3,    3,     1,    1,   {   3,             -99 } },
            { L_,       4,    4,     1,    1,   {   4,             -99 } },
            { L_,       7,    7,     1,    1,   {   7,             -99 } },
            { L_,       8,    8,     1,    1,   {   8,             -99 } },
            { L_,      15,   15,     1,    1,   {  15,             -99 } },
            { L_,      16,   16,     1,    1,   {  16,             -99 } },
            { L_,      31,   31,     1,    1,   {  31,             -99 } },
            { L_,      32,   32,     1,    1,   {  32,             -99 } },
            { L_,      33,   33,     1,    1,   {  33,             -99 } },

            { L_,     313,  313,     2,    2,   { 301, 12,         -99 } },
            { L_,       0,   17,     0,    1,   {  17,  0,         -99 } },

            { L_,      40,   40,     3,    3,   {  22, 11,  7,     -99 } },
            { L_,      12,   35,     1,    2,   {  23, 12,  0,     -99 } },
            { L_,      24,   37,     1,    2,   {  24, 13, -1,     -99 } },
            { L_,       7,   32,     1,    2,   {  25,  0,  7,     -99 } },

            { L_,     120,  120,     4,    4,   {   8, 16, 32, 64, -99 } },
            { L_,      48,   56,     2,    3,   {   8, 16, 32,  0, -99 } },
            { L_,      40,   56,     2,    3,   {   8, 16, 32, -1, -99 } },
            { L_,      24,   56,     2,    3,   {   8, 16, 32, -2, -99 } },
            { L_,      80,   88,     2,    3,   {   8, 16,  0, 64, -99 } },
            { L_,      72,   88,     2,    3,   {   8, 16, -1, 64, -99 } },
            { L_,      96,  104,     2,    3,   {   8,  0, 32, 64, -99 } },
            { L_,       0,   24,     0,    2,   {   8, 16,  0, -1, -99 } },
            { L_,       0,   24,     0,    2,   {   8, 16, -1,  0, -99 } },
            { L_,       0,   40,     0,    2,   {   8,  0, 32, -2, -99 } },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // Interpret each 'd_sequence[i]' value, 'N', as follows:
        //
        //: -99 == N -- terminate the allocation/deallocation sequence
        //:
        //:   0 <  N -- allocate 'N' bytes
        //:
        //:   0 >= N -- deallocate the block allocated by 'd_sequence[abs(N)]'

        if (verbose) {
            cout << "\nTesting 'allocate', 'deallocate', and byte counts."
                 << endl;
        }

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE        = DATA[ti].d_line;
            const int  BYTESINUSE  = DATA[ti].d_numBytesInUse;
            const int  BYTESTOTAL  = DATA[ti].d_numBytesTotal;
            const int  BLOCKSINUSE = DATA[ti].d_numBlocksInUse;
            const int  BLOCKSTOTAL = DATA[ti].d_numBlocksTotal;
            const int *SEQUENCE    = DATA[ti].d_sequence;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&sa);  const Obj& X = mX;

            void *p[MAX_SEQ];

            for (int i = 0; i < MAX_SEQ; ++i) {
                p[i] = 0;
            }

            for (int i = 0; i < MAX_SEQ; ++i) {
                const int N = SEQUENCE[i];

                if (-99 == N) {
                    break;
                }
                else if (0 < N) {
                    p[i] = mX.allocate(N);
                    LOOP2_ASSERT(LINE, ti, p[i]);

                    bsl::memset(p[i], 0xff, N);  // overwrite entire block

                    // verify maximal alignment

                    typedef bsls::AlignmentUtil U;
                    const int offset = U::calculateAlignmentOffset(
                                                        p[i],
                                                        U::BSLS_MAX_ALIGNMENT);
                    LOOP2_ASSERT(LINE, ti, 0 == offset);
                }
                else {
                    const int INDEX = bsl::abs(N);
                    LOOP2_ASSERT(LINE, ti, INDEX < MAX_SEQ);
                    LOOP2_ASSERT(LINE, ti, p[INDEX]);

                    mX.deallocate(p[INDEX]);
                    p[INDEX] = 0;
                }
            }

            LOOP2_ASSERT(LINE, ti, BYTESINUSE  ==  X.numBytesInUse());
            LOOP2_ASSERT(LINE, ti, BYTESTOTAL  ==  X.numBytesTotal());

            LOOP2_ASSERT(LINE, ti, BLOCKSINUSE == sa.numBlocksInUse());
            LOOP2_ASSERT(LINE, ti, BLOCKSTOTAL == sa.numBlocksTotal());
            LOOP2_ASSERT(LINE, ti,           0 == da.numBlocksTotal());

            for (int i = 0; i < MAX_SEQ; ++i) {
                mX.deallocate(p[i]);
            }

            LOOP2_ASSERT(LINE, ti, 0 ==  X.numBytesInUse());
            LOOP2_ASSERT(LINE, ti, 0 == sa.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting 'allocate(0)'." << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&sa);  const Obj& X = mX;

            ASSERT(0 ==  X.numBytesInUse());
            ASSERT(0 ==  X.numBytesTotal());

            void *p = mX.allocate(0);

            ASSERT(0 == p);
            ASSERT(0 ==  X.numBytesInUse());
            ASSERT(0 ==  X.numBytesTotal());

            ASSERT(0 == sa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting 'deallocate(0)'." << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&sa);  const Obj& X = mX;

            ASSERT(0 ==  X.numBytesInUse());
            ASSERT(0 ==  X.numBytesTotal());

            mX.deallocate(0);

            ASSERT(0 ==  X.numBytesInUse());
            ASSERT(0 ==  X.numBytesTotal());

            ASSERT(0 == sa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());

            void *p = mX.allocate(5);

            ASSERT(5 ==  X.numBytesInUse());
            ASSERT(5 ==  X.numBytesTotal());

            ASSERT(1 == sa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());

            mX.deallocate(0);

            ASSERT(5 ==  X.numBytesInUse());
            ASSERT(5 ==  X.numBytesTotal());

            ASSERT(1 == sa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());

            mX.deallocate(p);

            ASSERT(0 ==  X.numBytesInUse());
            ASSERT(5 ==  X.numBytesTotal());

            ASSERT(0 == sa.numBlocksInUse());
            ASSERT(1 == sa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object, that we can 'allocate' memory from the object, and that we
        //   can safely destroy the object.
        //
        // Concerns:
        //: 1 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 2 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 3 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 4 The default constructor allocates no memory.
        //:
        //: 5 Any memory allocation is from the object allocator.
        //:
        //: 6 There is no temporary allocation from any allocator.
        //:
        //: 7 The byte counts are initialized to 0.
        //:
        //: 8 An object created using the default constructor is unnamed.
        //:
        //: 9 The destructor has no effect on any outstanding allocated memory.
        //
        // Plan:
        //: 1 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..8)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'mX', with its object allocator configured appropriately (see
        //:     P-1); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-4)
        //:
        //:   4 Use the (as yet untested) 'numBytesInUse', 'numBytesTotal', and
        //:     'name' accessors to verify that the byte counts are initialized
        //:     to 0 and that the object is unnamed.  (C-7..8)
        //:
        //:   5 Allocate a block from 'mX' and verify that the memory is
        //:     allocated from the object allocator.  (C-1..3, 5)
        //:
        //:   6 Verify that no temporary memory is allocated from any
        //:     allocator.  (C-6)
        //:
        //: 2 Perform a separate test to verify that the destructor has no
        //:   effect on any outstanding allocated memory.  (C-9)
        //
        // Testing:
        //   CountingAllocator(Allocator *ba = 0);
        //   ~CountingAllocator();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT CTOR & DTOR" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting constructor." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(static_cast<bslma::Allocator *>(0));
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }
            LOOP_ASSERT(CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify byte counts are initialized to 0.

            LOOP_ASSERT(CONFIG, 0 == X.numBytesInUse());
            LOOP_ASSERT(CONFIG, 0 == X.numBytesTotal());

            // Verify unnamed (using untested 'name' accessor).

            LOOP_ASSERT(CONFIG, 0 == X.name());

            // Verify no allocation from the object allocator.

            LOOP2_ASSERT(CONFIG, oa.numBlocksTotal(),
                         0 == oa.numBlocksTotal());

            // Verify no allocation from the non-object allocator.

            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Verify we can allocate from the object and write to the returned
            // memory block.

            void *p = mX.allocate(1);
            LOOP2_ASSERT(CONFIG, p, p);
            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         1 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            *static_cast<char *>(p) = 'x';  // sanity check

            // Deallocate to avoid memory leak.

            mX.deallocate(p);

            LOOP2_ASSERT(CONFIG,  oa.numBlocksInUse(),
                         0 ==  oa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Destroy object and reclaim dynamically allocated footprint.

            fa.deleteObject(objPtr);

            // Verify all memory has been released.

            LOOP2_ASSERT(CONFIG,  fa.numBlocksInUse(),
                         0 ==  fa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG,  oa.numBlocksInUse(),
                         0 ==  oa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            void *p;

            {
                Obj mX(&sa);

                p = mX.allocate(1);

                ASSERT(p);
                ASSERT(1 == sa.numBlocksInUse());
                ASSERT(1 == sa.numBlocksTotal());

                *static_cast<char *>(p) = 'x';
            }

            ASSERT(1 == sa.numBlocksInUse());
            ASSERT(1 == sa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());

            ASSERT('x' == *static_cast<const char *>(p));

            // Suppress diagnostic on memory leak from 'mX'.

            sa.setQuiet(true);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create a modifiable object 'mX'.
        //: 2 Allocate a block 'b1' from 'mX'.
        //: 3 Deallocate block 'b1'.
        //: 4 Allocate a block 'b2' from 'mX'.
        //: 5 Allocate a block 'b3' from 'mX'.
        //: 6 Deallocate block 'b2'.
        //: 7 Deallocate block 'b3'.
        //: 8 Allow 'mX' to go out of scope.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const int N1 =  8;
        const int N2 = 13;
        const int N3 = 56;

        {
            // P-1
            Obj mX;  const Obj& X = mX;
                                     ASSERT(           0 == X.numBytesInUse());
                                     ASSERT(           0 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-1:\n";  X.print(bsl::cout); }

            // P-2
            void *p1 = mX.allocate(N1);
                                     ASSERT(          N1 == X.numBytesInUse());
                                     ASSERT(          N1 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-2:\n";  X.print(bsl::cout); }

            // P-3
            mX.deallocate(p1);
                                     ASSERT(           0 == X.numBytesInUse());
                                     ASSERT(          N1 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-3:\n";  X.print(bsl::cout); }

            // P-4
            void *p2 = mX.allocate(N2);
                                     ASSERT(          N2 == X.numBytesInUse());
                                     ASSERT(     N1 + N2 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-4:\n";  X.print(bsl::cout); }

            // P-5
            void *p3 = mX.allocate(N3);
                                     ASSERT(     N2 + N3 == X.numBytesInUse());
                                     ASSERT(N1 + N2 + N3 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-5:\n";  X.print(bsl::cout); }

            // P-6
            mX.deallocate(p2);
                                     ASSERT(          N3 == X.numBytesInUse());
                                     ASSERT(N1 + N2 + N3 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-6:\n";  X.print(bsl::cout); }

            // P-7
            mX.deallocate(p3);
                                     ASSERT(           0 == X.numBytesInUse());
                                     ASSERT(N1 + N2 + N3 == X.numBytesTotal());

            if (veryVerbose) { bsl::cout << "\nP-7:\n";  X.print(bsl::cout); }
        }   // P-8

        ASSERT(0 == da.numBlocksInUse());
        ASSERT(3 == da.numBlocksTotal());

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
