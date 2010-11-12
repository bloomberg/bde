// bdef_bind_test0.t.cpp         -*-C++-*-

#include <bdef_bind_test0.h>

#include <bdef_bind.h>
#include <bdef_bind_test.h>
#include <bdef_function.h>
#include <bdef_placeholder.h>

#include <bslalg_typetraits.h>

#include <bslmf_nil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The 'bdef_bind' component defines a parameterized type for binding an
// "invocable" (to respect the terminology of the component-level
// documentation), which is either a free function, a member function, or a
// function object (either by value or reference).  The binding produces a
// "bound object" (that can be passed by value) that encapsulates both the
// function called (by address for free functions, and by owning a copy of the
// underlying object otherwise) and arguments passed to it (by value).  Because
// it allocates space for storing all this information, there are a lot of
// concerns about memory allocation.  In addition, there are some concerns
// about properly passing the parameters by reference or by value to respect
// the signature of the invocable.
//
// Our main objectives are to make sure that (1) the bound object or function
// gets called appropriately with the correct arguments, in all various
// combinations of function types (free functions, member functions, function
// objects, and function object references) and parameters (value types,
// placeholders, and 'bdef_bind' objects), and that (2) the binder correctly
// propagates its allocator to all objects under its management (including all
// the copies of the function objects and parameters that it owns).
//
// The 'bdef_bind' is a large component by the number of lines of code, and
// even more so since it is a template and has at least *four* orthogonal
// dimensions: the family of factory methods ('bind', bindA', or 'bindR'), the
// nature of the bound object (free function pointer or reference, member
// function pointer, and function object by reference or by value), the number
// of arguments (from 0 up to 14 for function objects, and 0 up to 13 for free
// and member functions since the first argument is consumed by the test object
// pointer), and the invocation (any of the up to 2^14 possible combinations of
// placeholders or bound arguments).  In addition, in order to test the memory
// allocation model, we must use test classes that do and that do not allocate
// memory.
//
// Instead of restricting the test coverage, we split the testing of this
// component into 14 pieces.  This test component exercises 'bdef_Bind' objects
// with 0 parameters, or 0 parameters plus a function object in the first
// position (for free or member functions).  In addition, we exercise the
// following restraint: We test only all placeholders or all bound arguments (a
// more elaborate mix of placeholders and arguments is explored in the
// 'bdef_bind' test driver).
//
// Our test plan proceeds by checking that the 'bdef_Bind' constructors work
// with and without allocators as they should, and that the 'bdef_Bind' object
// can be constructed explicitly (case 1).  Then we check 'bdef_BindUtil::bind'
// in conjunction with free functions pointers (case 2) and references (case
// 3), member function pointers (case 4), function objects passed by address
// (case 5) or by value (case 6), in all cases using either all values or all
// placeholders as parameters.  Next we check 'bdef_BindUtil::bindA' in
// conjunction with free function pointers (case 7) or references (case 8),
// member function pointers (case 9), or function objects (cases 10 and 11) in
// the same way as cases 2--6.  Then, we check 'bdef_BindUtil::bindR' again in
// the same way as 'bdef_BindUtil::bind' (cases 12--16).
//-----------------------------------------------------------------------------
// [ 1] bdef_Bind(func, list, bslma_Allocator *ba = 0);
// [ 1] bdef_Bind(const bdef_Bind& original, bslma_Allocator *ba = 0);
// [ 2] bdef_BindUtil::bind(FUNC const& func, ...);
// [ 3] bdef_BindUtil::bind(FUNC const& func, ...);
// [ 4] bdef_BindUtil::bind(FUNC const& func, ...);
// [ 5] bdef_BindUtil::bind(FUNC const& func, ...);
// [ 6] bdef_BindUtil::bind(FUNC const& func, ...);
// [ 7] bdef_BindUtil::bindA(bslma_Allocator *ba, FUNC const& func, ...);
// [18] bdef_BindUtil::bindA(bslma_Allocator *ba, FUNC const& func, ...);
// [19] bdef_BindUtil::bindA(bslma_Allocator *ba, FUNC const& func, ...);
// [10] bdef_BindUtil::bindA(bslma_Allocator *ba, FUNC const& func, ...);
// [11] bdef_BindUtil::bindA(bslma_Allocator *ba, FUNC const& func, ...);
// [12] bdef_BindUtil::bindR(FUNC const& func, ...);
// [13] bdef_BindUtil::bindR(FUNC const& func, ...);
// [14] bdef_BindUtil::bindR(FUNC const& func, ...);
// [15] bdef_BindUtil::bindR(FUNC const& func, ...);
// [16] bdef_BindUtil::bindR(FUNC const& func, ...);
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

static int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                     // current Line number
#define T_ printf("\t");                // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class ConvertibleFromToInt {
    // Object that is convertible from and to 'int'.  Used to make sure that
    // 'bindR' can overwrite the return type of a bound object to an arbitrary
    // type that is convertible from the actual return type of the bound
    // object.  It is also convertible to 'int' to enable comparison to an int
    // for the verification of the return value of the binder invocation.

    int d_value; // int value of this object

    public:
    // CREATORS
    ConvertibleFromToInt() : d_value() { }
    ConvertibleFromToInt(int value) : d_value(value) { }

    // MANIPULATORS
    operator int&() { return d_value; }

    // ACCESSORS
    operator int() const { return d_value; }
};

// The whole bdef_bind component currently works with up to 14 arguments.

const int BIND_MAX_ARGUMENTS = 14;
const int NUM_SLOTS = BIND_MAX_ARGUMENTS+1;

// Nil value (uninitialized).

const int N1 = -1;

// Typedefs for slots, argument, and test types.

typedef bdef_Bind_TestSlotsNoAlloc    SlotsNoAlloc;

typedef bdef_Bind_TestArgNoAlloc<1>   NoAllocTestArg1;
typedef bdef_Bind_TestArgNoAlloc<2>   NoAllocTestArg2;
typedef bdef_Bind_TestArgNoAlloc<3>   NoAllocTestArg3;
typedef bdef_Bind_TestArgNoAlloc<4>   NoAllocTestArg4;
typedef bdef_Bind_TestArgNoAlloc<5>   NoAllocTestArg5;
typedef bdef_Bind_TestArgNoAlloc<6>   NoAllocTestArg6;
typedef bdef_Bind_TestArgNoAlloc<7>   NoAllocTestArg7;
typedef bdef_Bind_TestArgNoAlloc<8>   NoAllocTestArg8;
typedef bdef_Bind_TestArgNoAlloc<9>   NoAllocTestArg9;
typedef bdef_Bind_TestArgNoAlloc<10>  NoAllocTestArg10;
typedef bdef_Bind_TestArgNoAlloc<11>  NoAllocTestArg11;
typedef bdef_Bind_TestArgNoAlloc<12>  NoAllocTestArg12;
typedef bdef_Bind_TestArgNoAlloc<13>  NoAllocTestArg13;
typedef bdef_Bind_TestArgNoAlloc<14>  NoAllocTestArg14;

typedef bdef_Bind_TestTypeNoAlloc     NoAllocTestType;

typedef bdef_Bind_TestSlotsAlloc      SlotsAlloc;

typedef bdef_Bind_TestArgAlloc<1>     AllocTestArg1;
typedef bdef_Bind_TestArgAlloc<2>     AllocTestArg2;
typedef bdef_Bind_TestArgAlloc<3>     AllocTestArg3;
typedef bdef_Bind_TestArgAlloc<4>     AllocTestArg4;
typedef bdef_Bind_TestArgAlloc<5>     AllocTestArg5;
typedef bdef_Bind_TestArgAlloc<6>     AllocTestArg6;
typedef bdef_Bind_TestArgAlloc<7>     AllocTestArg7;
typedef bdef_Bind_TestArgAlloc<8>     AllocTestArg8;
typedef bdef_Bind_TestArgAlloc<9>     AllocTestArg9;
typedef bdef_Bind_TestArgAlloc<10>    AllocTestArg10;
typedef bdef_Bind_TestArgAlloc<11>    AllocTestArg11;
typedef bdef_Bind_TestArgAlloc<12>    AllocTestArg12;
typedef bdef_Bind_TestArgAlloc<13>    AllocTestArg13;
typedef bdef_Bind_TestArgAlloc<14>    AllocTestArg14;

typedef bdef_Bind_TestTypeAlloc       AllocTestType;

//=============================================================================
//               MACROS EXPORTING INITIALIZATION OUT OF MAIN
//-----------------------------------------------------------------------------
#define DECLARE_MAIN_VARIABLES                                                \
    /* The following machinery is for use in conjunction with the             \
    // 'SlotsNoAlloc::resetSlots' and 'SlotsNoAlloc::verifySlots' functions.  \
    // The slots are set when the corresponding function object or free       \
    // function is called. */                                                 \
                                                                              \
    const int NO_ALLOC_SLOTS[NUM_SLOTS]= {                                    \
        /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  */      \
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1          \
    };                                                                        \
                                                                              \
    /* The following machinery is for use in conjunction with the             \
    // 'SlotsAlloc::resetSlots' and 'SlotsAlloc::verifySlots' functions.  The \
    // slots are set when the corresponding function object or free function is \
    // called. */                                                             \
                                                                              \
    bslma_TestAllocator allocator0(veryVeryVerbose);                          \
    bslma_TestAllocator allocator1(veryVeryVerbose);                          \
    bslma_TestAllocator allocator2(veryVeryVerbose);                          \
                                                                              \
    bslma_TestAllocator *Z0 = &allocator0;                                    \
    bslma_TestAllocator *Z1 = &allocator1;                                    \
    bslma_TestAllocator *Z2 = &allocator2;                                    \
                                                                              \
    bslma_DefaultAllocatorGuard allocGuard(Z0);                               \
    SlotsAlloc::setZ0(Z0);                                                    \
    SlotsAlloc::setZ1(Z1);                                                    \
    SlotsAlloc::setZ2(Z2);                                                    \
                                                                              \
    const bslma_Allocator *ALLOC_SLOTS[NUM_SLOTS] = {                         \
        /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  */      \
          Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0          \
    };

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)   

DEFINE_TEST_CASE(16) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FUNCTION OBJECT PASSED BY VALUE
        // Concerns:
        //   That the 'bdef_BindUtil::bindR' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bindR' with an instance 'mX' of the classes
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, invoked with no parameters.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   the proper amount of allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdef_BindUtil::bindR' WITH FUNCTION OBJECT"
                   "\n===================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
            // No allocation should take place.

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(mX) ());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
            // The function call does not modify the object other than for
            // copying the function object which should result in 28
            // allocations (twice for each member of the 'AllocTestType'
            // object, once for the temporary 'func' argument to
            // 'bdef_BindUtil::bind' and another for the privately held copy
            // inside the binder).

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS+28 == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS            == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(15) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FUNCTION OBJECT PASSED BY ADDRESS
        // Concerns:
        //   That the 'bdef_BindUtil::bindR' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bindR' with the address of an instance 'mX'
        //   of the classes 'NoAllocTestType' or 'AllocTestType', constructed
        //   with or without an allocator, invoked with no parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindR' WITH FUNCTION OBJECT POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(&mX) ());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(&mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(14) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH MEMBER FUNCTION POINTERS
        // Concerns:
        //   That the 'bdef_BindUtil::bindR' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bindR' with the 'testFunc0' method of the
        //   classes 'NoAllocTestType' or 'AllocTestType', applied to a
        //   function object 'mX' constructed with or without an allocator and
        //   no parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindR' WITH MEMBER FUNCTION POINTER"
            "\n===========================================================\n");

        const bool BOUND_OBJECT_MODIFIED = true;

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                         &NoAllocTestType::testFunc0, &mX) ());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                          &AllocTestType::testFunc0, &mX) () );
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(13) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FREE FUNCTION REFERENCES
        // Concerns and plan:
        //   Identical to case 12, except passing a reference to 'func0'
        //   instead of '&func0'.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindR' WITH FREE FUNCTION REFERENCE"
            "\n===========================================================\n");

        if (verbose) printf("\ttest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                bdef_Bind_TestFunctionsNoAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                  bdef_Bind_TestFunctionsAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(12) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FREE FUNCTION POINTERS
        // Concerns:
        //   That the 'bdef_BindUtil::bindR' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bindR' with the address of the 'func0'
        //   class method of the 'bdef_Bind_TestFunctionsNoAlloc' or
        //   'bdef_Bind_TestFunctionsAlloc' utility classes, that takes a
        //   mutable function object 'mX', constructed with or without an
        //   allocator, and no parameters.  Upon invocation, verify that the
        //   return value and state of 'mX' are as expected, and that no
        //   allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
       printf("\nTESTING 'bdef_BindUtil::bindR' WITH FREE FUNCTION POINTER"
              "\n=========================================================\n");

        const bool BOUND_OBJECT_MODIFIED = true;

        if (verbose) printf("\ttest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                               &bdef_Bind_TestFunctionsNoAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                 &bdef_Bind_TestFunctionsAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(11) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY VALUE
        // Concerns:
        //   That the 'bdef_BindUtil::bindA' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //  The plan is similar to case 6 of 'bdef_BindUtil::bind':  call
        //  'bdef_BindUtil::bindA' with an instance 'mX' of the classes
        //  'NoAllocTestType' or 'AllocTestType', constructed with or without
        //  an allocator, invoked with no parameters.  Upon invocation, verify
        //  that the return value and state of 'mX' are as expected, and that
        //  the proper amount of allocation took place.  In addition, we use
        //  two separate allocators for the binder and for the objects, and
        //  track (using the 'SlotsNoAlloc::setSlots' and
        //  'SlotsAlloc::setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
       printf("\nTESTING 'bdef_BindUtil::bindA' WITH FUNCTION OBJECT VALUE"
              "\n=========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
            // No allocation should take place.

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2, mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));
        }

        if (verbose) printf("\tTest types that do take an allocator.\n");

        {
            // For 0 arguments, the logic is slightly different from the
            // general case since the function call does not modify the object
            // other than for copying the function object which should result
            // in 14 allocations (once for each member of the 'AllocTestType'
            // object in the temporary 'func' argument to
            // 'bdef_BindUtil::bind'; note that the privately held copy inside
            // the binder uses 'Z2').

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2, mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS+14 == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS            == Z1->numAllocations());

            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));
        }
      }

DEFINE_TEST_CASE(10) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY ADDRESS
        // Concerns:
        //   That the 'bdef_BindUtil::bindA' static method returns a
        //   'bdef_Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 5 of 'bdef_BindUtil::bind': call
        //   'bdef_BindUtil::bindA' with the address of an instance 'mX' of the
        //   classes 'NoAllocTestType' or 'AllocTestType', constructed with or
        //   without an allocator, invoked with no parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation took place.  In addition, we use
        //   two separate allocators for the binder and for the objects, and
        //   track (using the 'SlotsNoAlloc::setSlots' and
        //   'SlotsAlloc::setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindA' WITH FUNCTION OBJECT POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2, &mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2, &mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());

            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));
        }
      }

DEFINE_TEST_CASE(9) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH MEMBER FUNCTION POINTERS
        // Concerns:
        //   That the 'bdef_BindUtil::bindA' static method returns a
        //   'bdef_Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 4 of 'bdef_BindUtil::bind': call
        //   'bdef_BindUtil::bindA' with the 'testFunc0' method of the classes
        //   'NoAllocTestType' or 'AllocTestType', applied to a function object
        //   'mX' constructed with or without an allocator and no parameters.
        //   Upon invocation, verify that the return value and state of 'mX'
        //   are as expected, and that no allocation took place.  In addition,
        //   we use two separate allocators for the binder and for the objects,
        //   and track (using the 'SlotsNoAlloc::setSlots' and
        //   'SlotsAlloc::setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindA' WITH MEMBER FUNCTION POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2,
                                         &NoAllocTestType::testFunc0, &mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2,
                                           &AllocTestType::testFunc0, &mX) ());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());

            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));
        }
      }

DEFINE_TEST_CASE(8) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FREE FUNCTION REFERENCES
        // Concerns and plan:
        //   Identical to case 7, except passing a reference to 'func0' instead
        //   of '&func0'.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindA' WITH FREE FUNCTION REFERENCE"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2,
                                bdef_Bind_TestFunctionsNoAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2,
                                  bdef_Bind_TestFunctionsAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());

            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));
        }
      }

DEFINE_TEST_CASE(7) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FREE FUNCTION POINTERS
        // Concerns:
        //   That the 'bdef_BindUtil::bindA' static method returns a
        //   'bdef_Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 2 of 'bdef_BindUtil::bind': call
        //   'bdef_BindUtil::bindA' with the 'func0' function of the
        //   'bdef_Bind_TestFunctionsNoAlloc' or 'bdef_Bind_TestFunctionsAlloc'
        //   utility classes, that takes an instance 'mX' of the classes
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, and no parameters.  Upon invocation, verify that the
        //   return value and state of 'mX' are as expected, and that no
        //   allocation took place.  In addition, we use two separate
        //   allocators for the binder and for the objects, and track (using
        //   the 'setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
       printf("\nTESTING 'bdef_BindUtil::bindA' WITH FREE FUNCTION POINTER"
              "\n=========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2,
                               &bdef_Bind_TestFunctionsNoAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));

            ASSERT(0 == bdef_BindUtil::bindA(Z2,
                                 &bdef_Bind_TestFunctionsAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());

            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));
        }
      }

DEFINE_TEST_CASE(6) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY VALUE
        // Concerns:
        //   That the 'bdef_BindUtil::bind' static method returns a 'bdef_Bind'
        //   object that returns the correct value, has the appropriate side
        //   effects, and performs the proper amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with an instance 'mX' of the classes
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, invoked with no parameters.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   the proper amount of allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdef_BindUtil::bind' WITH FUNCTION OBJECT"
                   "\n==================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
            // No allocation should take place.

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(mX)());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
            // For 0 arguments, the logic is slightly different from the
            // general case since the function call does not modify the object
            // other than for copying the function object which should result
            // in 28 allocations (twice for each member of the 'AllocTestType'
            // object, once for the temporary 'func' argument to
            // 'bdef_BindUtil::bind' and another for the privately held copy
            // inside the binder).

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS      = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            ASSERT(NUM_DFLT_ALLOCS+28 == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(5) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY ADDRESS
        // Concerns:
        //   That the 'bdef_BindUtil::bind' static method returns a 'bdef_Bind'
        //   object that returns the correct value, has the appropriate side
        //   effects, and performs the proper amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with the address of an instance 'mX' of
        //   the classes 'NoAllocTestType' or 'AllocTestType', constructed with
        //   or without an allocator, invoked with no parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
      printf("\nTESTING 'bdef_BindUtil::bind' WITH FUNCTION OBJECT POINTER"
             "\n==========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(&mX)());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(&mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(4) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH MEMBER FUNCTION POINTERS
        // Concerns:
        //   That the 'bdef_BindUtil::bind' static method returns a 'bdef_Bind'
        //   object that returns the correct value, has the appropriate side
        //   effects, and performs the proper amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with the 'testFunc0' method of
        //   the classes 'NoAllocTestType' or 'AllocTestType', applied to a
        //   function object 'mX' constructed with or without an allocator and
        //   no parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
      printf("\nTESTING 'bdef_BindUtil::bind' WITH MEMBER FUNCTION POINTER"
             "\n==========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(
                                         &NoAllocTestType::testFunc0, &mX) ());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(
                                          &AllocTestType::testFunc0, &mX) () );
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(3) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FREE FUNCTION REFERENCES
        // Concerns and plan:
        //   Identical to case 2, except passing a reference to 'func0'
        //   instead of '&func0'.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
      printf("\nTESTING 'bdef_BindUtil::bind' WITH FREE FUNCTION REFERENCE"
             "\n==========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(
                                bdef_Bind_TestFunctionsNoAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(
                                  bdef_Bind_TestFunctionsAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(2) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FREE FUNCTION POINTERS
        // Concerns:
        //   That the 'bdef_BindUtil::bind' static method returns a 'bdef_Bind'
        //   object that returns the correct value, has the appropriate side
        //   effects, and performs the proper amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with the address of the 'func0' class
        //   method of the utility classes 'bdef_Bind_TestFunctionsNoAlloc' or
        //   'bdef_Bind_TestFunctionsAlloc', that takes a mutable function
        //   object 'mX', constructed with or without an allocator, and no
        //   parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
        printf("\nTESTING 'bdef_BindUtil::bind' WITH FREE FUNCTION POINTER"
               "\n========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            ASSERT(EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(
                               &bdef_Bind_TestFunctionsNoAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        {
                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            ASSERT(EXPECTED == X);

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations();
            const int NUM_ALLOCS         = Z1->numAllocations();

            ASSERT(0 == bdef_BindUtil::bind(
                                 &bdef_Bind_TestFunctionsAlloc::func0, &mX)());
            ASSERT(EXPECTED == X);

#ifndef BSLS_PLATFORM__CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            ASSERT(NUM_DEFAULT_ALLOCS == Z0->numAllocations());
#endif
            ASSERT(NUM_ALLOCS         == Z1->numAllocations());
        }
      }

DEFINE_TEST_CASE(1) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BIND CONSTRUCTORS
        // Concerns:
        //   That the constructors build a 'bdef_Bind' object that has the
        //   proper members so that, when the bound object is invoked, it is
        //   put into the correct state and returns the proper value.
        //
        // Plan:
        //   Construct a 'bdef_Bind' object using the four constructors (the
        //   two that take a list of arguments and an allocator or not, and the
        //   copy constructors that take an allocator or not).  Then invoke the
        //   bound object and verify that it is put into the correct state and
        //   returns the proper value.
        //
        // Testing:
        //   bdef_Bind(func, list, bslma_Allocator *ba = 0);
        //   bdef_Bind(const bdef_Bind& original, bslma_Allocator *ba = 0);
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDEF_BIND CONSTRUCTORS"
                            "\n==============================\n");

        using namespace bdef_PlaceHolders;

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // 0 arguments to function object.
        {
            const int NA = Z0->numAllocations();

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED_X; // expected value of 'X'
                                               // *after* the binder is invoked
            const NoAllocTestType  DEFAULT_X;  // default value of 'X'
                                               // *after* the binder is invoked
            ASSERT(EXPECTED_X == X);
            ASSERT(DEFAULT_X == X);

            typedef NoAllocTestType *FUNC;
            typedef bdef_Bind_BoundTuple0 ListType;
            typedef bdef_Bind<bslmf_Nil, FUNC, ListType> Bind;

            // For passing to the constructor of 'bdef_Bind'.

            ListType        mL;     // list of arguments
            const ListType& L = mL; // non-modifiable list of arguments

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT(EXPECTED_X == X);
            ASSERT(0 == mB1());
            ASSERT(EXPECTED_X == X);

            Bind mB2(&mX, L, Z0);  const Bind& B2 = mB2;
            ASSERT(DEFAULT_X == X);
            ASSERT(0 == mB2());
            ASSERT(DEFAULT_X == X);

            Bind mB3(B1);
            ASSERT(EXPECTED_X == X);
            ASSERT(0 == mB3());
            ASSERT(EXPECTED_X == X);

            Bind mB4(B2, Z0);
            ASSERT(DEFAULT_X == X);
            ASSERT(0 == mB4());
            ASSERT(DEFAULT_X == X);

            ASSERT(NA == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // 0 arguments to function object.
        {
                  AllocTestType mX(Z1);
            const AllocTestType X = mX;
            const AllocTestType EXPECTED_X(Z1);
            const AllocTestType DEFAULT_X(Z1);
            ASSERT(EXPECTED_X == X);
            ASSERT(DEFAULT_X == X);

            typedef AllocTestType *FUNC;
            typedef bdef_Bind_BoundTuple0 ListType;
            typedef bdef_Bind<bslmf_Nil, FUNC, ListType> Bind;

            // For passing to the constructor of 'bdef_Bind'.

            ListType        mL;     // list of arguments
            const ListType& L = mL; // non-modifiable list of arguments

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT(EXPECTED_X == X);
            ASSERT(0 == mB1());
            ASSERT(EXPECTED_X == X);

            Bind mB2(&mX, L, Z2);  const Bind& B2 = mB2;
            ASSERT(DEFAULT_X == X);
            ASSERT(0 == mB2());
            ASSERT(DEFAULT_X == X);

            Bind mB3(B1);
            ASSERT(EXPECTED_X == X);
            ASSERT(0 == mB3());
            ASSERT(EXPECTED_X == X);

            Bind mB4(B2, Z2);
            ASSERT(DEFAULT_X == X);
            ASSERT(0 == mB4());
            ASSERT(DEFAULT_X == X);
        }
      }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                     \
  case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose); break
        CASE(16);
        CASE(15);
        CASE(14);
        CASE(13);
        CASE(12);
        CASE(11);
        CASE(10);
        CASE(9);
        CASE(8);
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
