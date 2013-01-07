// bdef_bind_test1.t.cpp         -*-C++-*-

#include <bdef_bind_test1.h>

#include <bdef_bind.h>
#include <bdef_bind_test.h>
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
// function called (by address for functions, and by owning a copy of the
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
// with 14 parameters for invocables which are function objects (by value or by
// address).  In addition, we exercise the following restraint: We test only
// all placeholders or all bound arguments (a more elaborate mix of
// placeholders and arguments is explored in the 'bdef_bind' test driver).
//
// Our test plan proceeds by checking that the 'bdef_Bind' constructors work
// with and without allocators as they should, and that the 'bdef_Bind' object
// can be constructed explicitly (case 1).  Then we check 'bdef_BindUtil::bind'
// in conjunction with free functions pointers (case 2) and references (case
// 3), member function pointers (case 4), function objects passed by address
// (case 5) or by value (case 6), in all cases using either values or
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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
        if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }   \

#define LOOP2_ASSERT(I,J,X) {                                                 \
        if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J);                 \
                            aSsErT(1, #X, __LINE__); } }                      \

#define LOOP3_ASSERT(I,J,K,X) {                                               \
        if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\n", #I, I, #J, J, #K, K);  \
                            aSsErT(1, #X, __LINE__); } }                      \

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
        if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n",                \
                      #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

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

// This test driver.

const int PARAMS = 1;

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

// Placeholder types for the corresponding _1, _2, etc.

typedef bdef_PlaceHolder<1>  PH1;
typedef bdef_PlaceHolder<2>  PH2;
typedef bdef_PlaceHolder<3>  PH3;
typedef bdef_PlaceHolder<4>  PH4;
typedef bdef_PlaceHolder<5>  PH5;
typedef bdef_PlaceHolder<6>  PH6;
typedef bdef_PlaceHolder<7>  PH7;
typedef bdef_PlaceHolder<8>  PH8;
typedef bdef_PlaceHolder<9>  PH9;
typedef bdef_PlaceHolder<10> PH10;
typedef bdef_PlaceHolder<11> PH11;
typedef bdef_PlaceHolder<12> PH12;
typedef bdef_PlaceHolder<13> PH13;
typedef bdef_PlaceHolder<14> PH14;
//=============================================================================
//               MACROS EXPORTING INITIALIZATION OUT OF MAIN
//-----------------------------------------------------------------------------
#define DECLARE_MAIN_VARIABLES                                                \
    /* The following machinery is for use in conjunction with the             \
    // 'SlotsNoAlloc::resetSlots' and 'SlotsNoAlloc::verifySlots' functions.  \
    // The slots are set when the corresponding function objector free function \
    // is called with 'NumArgs' arguments. */                                 \
                                                                              \
    const int NO_ALLOC_SLOTS[NUM_SLOTS]= {                                    \
        /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14 */       \
          N1,  1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1          \
    };                                                                        \
                                                                              \
    const int NO_ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {                           \
          N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1, N1          \
    };                                                                        \
                                                                              \
    /* Values that do not take an allocator. */                               \
                                                                              \
    const NoAllocTestArg1  I1  = 1;                                           \
    const NoAllocTestArg2  I2  = 2;                                           \
    const NoAllocTestArg3  I3  = 3;                                           \
    const NoAllocTestArg4  I4  = 4;                                           \
    const NoAllocTestArg5  I5  = 5;                                           \
    const NoAllocTestArg6  I6  = 6;                                           \
    const NoAllocTestArg7  I7  = 7;                                           \
    const NoAllocTestArg8  I8  = 8;                                           \
    const NoAllocTestArg9  I9  = 9;                                           \
    const NoAllocTestArg10 I10 = 10;                                          \
    const NoAllocTestArg11 I11 = 11;                                          \
    const NoAllocTestArg12 I12 = 12;                                          \
    const NoAllocTestArg13 I13 = 13;                                          \
    const NoAllocTestArg14 I14 = 14;                                          \
                                                                              \
    /* The following machinery is for use in conjunction with the             \
    // 'SlotsAlloc::resetSlots' and 'SlotsAlloc::verifySlots' functions.  The \
    // slots are set when the corresponding function objector free function is\
    // called with 'NumArgs' arguments. */                                    \
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
        /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14 */       \
          Z0, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0          \
    };                                                                        \
    const bslma_Allocator *ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {                 \
          Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0          \
    };                                                                        \
                                                                              \
    /* Values that do take an allocator (default allocator is used). */       \
                                                                              \
    const AllocTestArg1  V1(1),     NV1(-5);                                  \
    const AllocTestArg2  V2(20),    NV2(-20);                                 \
    const AllocTestArg3  V3(23),    NV3(-23);                                 \
    const AllocTestArg4  V4(44),    NV4(-44);                                 \
    const AllocTestArg5  V5(66),    NV5(-66);                                 \
    const AllocTestArg6  V6(176),   NV6(-176);                                \
    const AllocTestArg7  V7(878),   NV7(-878);                                \
    const AllocTestArg8  V8(8),     NV8(-8);                                  \
    const AllocTestArg9  V9(912),   NV9(-912);                                \
    const AllocTestArg10 V10(102),  NV10(-120);                               \
    const AllocTestArg11 V11(111),  NV11(-111);                               \
    const AllocTestArg12 V12(333),  NV12(-333);                               \
    const AllocTestArg13 V13(712),  NV13(-712);                               \
    const AllocTestArg14 V14(1414), NV14(-1414);                              \
                                                                              
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
        //   Call 'bdef_BindUtil::bindR' with an instance 'mX' of the class
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, invoked with 1 parameter.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   no allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdef_BindUtil::bindR' WITH FUNCTION OBJECT"
                   "\n===================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place regardless of whether place-holders are used or
        // not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                    mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                    mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // Since the bound function object is passed by value, no allocation
        // using 'Z1' should take place.  The copy of the function object into
        // the binder object requires 14 default allocations (plus another 14
        // for passing the 'func' argument by value for the
        // 'bdef_BindUtil::bind' method), and the PARAMS allocations triggered
        // by the invocation of the 'AllocTestType::testFunc*' methods will be
        // served by the allocator 'Z0' from the binder (who owns the copy of
        // the bound object) instead of the allocator 'Z1' of the bound object.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.  Since the bound function object is passed
            // by value, however, see the comment above.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE      = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                    mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS,
                         28+PARAMS == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur twice for each bound argument of
            // type 'AllocTestValue', as those are copied into the binder
            // object twice: once for making the tuple list type to be passed
            // to the binder constructor, and a second time for copying that
            // tuple into the binder.  Since the bound function object is
            // passed by value, however, see the comment above.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE  = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                    mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS = Z1->numAllocations() - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS,
                         28+3*PARAMS == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
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
        //   of the class 'NoAllocTestType' or 'AllocTestType', constructed
        //   with or without an allocator, invoked with 1 parameter.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindR' WITH FUNCTION OBJECT POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                   &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                   &mX, // invocation
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                   &mX, // invocation
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                                                   &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(14) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH MEMBER FUNCTION POINTER
        // Concerns:
        //   That the 'bdef_BindUtil::bindR' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bindR' with the 'testFunc1'
        //   method of the classes 'NoAllocTestType' or 'AllocTestType',
        //   applied to a function object 'mX' constructed with or without an
        //   allocator and 1 parameter.  Upon invocation,
        //   verify that the return value and state of 'mX' are as expected,
        //   and that no allocation (for objects 'mX' that do not require
        //   allocation) or the proper amount of allocation (for objects 'mX'
        //   that do require allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindR' WITH MEMBER FUNCTION POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                      &NoAllocTestType::testFunc1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                      &NoAllocTestType::testFunc1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                        &AllocTestType::testFunc1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
                        &AllocTestType::testFunc1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(13) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FREE FUNCTION REFERENCES
        // Concerns and plans:
        //   Identical to case 12, except passing 'func1' instead of
        //   '&func1'.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindR' WITH FREE FUNCTION REFERENCE"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
            bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
            bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
              bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
              bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(12) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FREE FUNCTION POINTER
        // Concerns:
        //   That the 'bdef_BindUtil::bindR' static method returns a
        //   'bdef_Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bindR' with the address of the class method
        //   'func1' of the utility class 'bdef_Bind_TestFunctionsNoAlloc' or
        //   'bdef_Bind_TestFunctionsAlloc', that takes a mutable function
        //   object 'mX', constructed with or without an allocator, and 1
        //   parameter.  Upon invocation, verify that the return value and
        //   state of 'mX' (as modified by the 'testFunc1' method) are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bindR(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
       printf("\nTESTING 'bdef_BindUtil::bindR' WITH FREE FUNCTION POINTER"
              "\n=========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
           &bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
           &bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
             &bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS,
                        PARAMS == bdef_BindUtil::bindR<ConvertibleFromToInt>(
             &bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(11) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY VALUE
        // Concerns and plan:
        //   Identical to case 7, except passing 'mX' instead of '&mX'.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
       printf("\nTESTING 'bdef_BindUtil::bindA' WITH FUNCTION OBJECT VALUE"
              "\n=========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                    mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                    mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In these two test sequences (using 'AllocTestType'), PARAMS
        // allocations should be requested (one per each argument) via the 'Z1'
        // allocator, as triggered in the 'AllocTestType::testFunc*' methods,
        // except if the function object is passed by value and copied into the
        // binder (with the optional allocator specified for the binder, 'Z2'),
        // in which case the copy uses 'Z2' instead of 'Z1' to supply memory.
        // All other allocations for the binder object should be requested via
        // 'Z2'.  In the case of passing a function object passed by value,
        // there will still be 14 allocations for passing the 'func' argument
        // to 'bdef_BindUtil::bindA' by value.  No other default allocation
        // should occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                    mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 14 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // The difference between this sequence and the preceding one is
            // that, by using no place-holders, the bound arguments will be
            // copied into the binder object and so the slots will be set to
            // 'Z2' instead of the default when invoking the
            // 'AllocTestType::testFunction*' methods.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                    mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 14 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
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
        //   The plan is similar to case 3 of 'bdef_BindUtil::bind': call
        //   'bdef_BindUtil::bindA' with the address of an instance 'mX' of
        //   the class 'NoAllocTestType' or 'AllocTestType', constructed with
        //   or without an allocator, invoked with 6 parameter.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.  In
        //   addition, we use two separate allocators for the binder and for
        //   the objects, and track (using the 'SlotsNoAlloc::setSlots' and
        //   'SlotsAlloc::setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindA' WITH FUNCTION OBJECT POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                   &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                   &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In these two test sequences (using 'AllocTestType'), PARAMS
        // allocations should be requested (one per each argument) via the 'Z1'
        // allocator, as triggered in the 'AllocTestType::testFunc*' methods,
        // except if the function object is passed by value and copied into the
        // binder (with the optional allocator specified for the binder, 'Z2'),
        // in which case the copy uses 'Z2' instead of 'Z1' to supply memory.
        // All other allocations for the binder object should be requested via
        // 'Z2'.  In the case of passing a function object (no pointer), there
        // will still be 14 allocations for passing the 'func' argument to
        // 'bdef_BindUtil::bindA' by value.  No other default allocation should
        // occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                   &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // The difference between this sequence and the preceding one is
            // that, by using no place-holders, the bound arguments will be
            // copied into the binder object and so the slots will be set to
            // 'Z2' instead of the default when invoking the
            // 'AllocTestType::testFunction*' methods.  In the case of passing
            // a function object (no pointer), there will still be 14
            // allocations for passing the 'func' argument to
            // 'bdef_BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                                                   &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
      }

DEFINE_TEST_CASE(9) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH MEMBER FUNCTION POINTER
        // Concerns:
        //   That the 'bdef_BindUtil::bindA' static method returns a
        //   'bdef_Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 4 of 'bdef_BindUtil::bind': call
        //   'bdef_BindUtil::bindA' with the 'testFunc1' method of the
        //   class 'NoAllocTestType' or 'AllocTestType', applied to a
        //   function object 'mX' constructed with or without an allocator and
        //   6 parameter.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation (for objects
        //   'mX' that do not require allocation) or the proper amount of
        //   allocation (for objects 'mX' that do require allocation) took
        //   place.  In addition, we use two separate allocators for the binder
        //   and for the objects, and track (using the 'SlotsNoAlloc::setSlots'
        //   and 'SlotsAlloc::setSlots' test functions) which allocator was
        //   used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindA' WITH MEMBER FUNCTION POINTER"
            "\n===========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
                const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                      &NoAllocTestType::testFunc1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                      &NoAllocTestType::testFunc1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In these two test sequences (using 'AllocTestType'), PARAMS
        // allocations should be requested (one per each argument) via the 'Z1'
        // allocator, as triggered in the 'AllocTestType::testFunc*' methods,
        // except if the function object is passed by value and copied into the
        // binder (with the optional allocator specified for the binder, 'Z2'),
        // in which case the copy uses 'Z2' instead of 'Z1' to supply memory.
        // All other allocations for the binder object should be requested via
        // 'Z2'.  In the case of passing a function object (no pointer), there
        // will still be 14 allocations for passing the 'func' argument to
        // 'bdef_BindUtil::bindA' by value.  No other default allocation should
        // occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                        &AllocTestType::testFunc1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // The difference between this sequence and the preceding one is
            // that, by using no place-holders, the bound arguments will be
            // copied into the binder object and so the slots will be set to
            // 'Z2' instead of the default when invoking the
            // 'AllocTestType::testFunction*' methods.  In the case of passing
            // a function object (no pointer), there will still be 14
            // allocations for passing the 'func' argument to
            // 'bdef_BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
                        &AllocTestType::testFunc1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
      }

DEFINE_TEST_CASE(8) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FREE FUNCTION REFERENCE
        // Concerns and plan:
        //   Identical to case 7, except passing 'func1' instead of
        //   '&func1'.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
     printf("\nTESTING 'bdef_BindUtil::bindA' WITH FREE FUNCTION REFERENCE"
            "\n===========================================================\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
            bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
            bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In these two test sequences (using 'AllocTestType'), PARAMS
        // allocations should be requested (one per each argument) via the 'Z1'
        // allocator, as triggered in the 'AllocTestType::testFunc*' methods,
        // except if the function object is passed by value and copied into the
        // binder (with the optional allocator specified for the binder, 'Z2'),
        // in which case the copy uses 'Z2' instead of 'Z1' to supply memory.
        // All other allocations for the binder object should be requested via
        // 'Z2'.  In the case of passing a function object (no pointer), there
        // will still be 14 allocations for passing the 'func' argument to
        // 'bdef_BindUtil::bindA' by value.  No other default allocation should
        // occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
              bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // The difference between this sequence and the preceding one is
            // that, by using no place-holders, the bound arguments will be
            // copied into the binder object and so the slots will be set to
            // 'Z2' instead of the default when invoking the
            // 'AllocTestType::testFunction*' methods.  In the case of passing
            // a function object (no pointer), there will still be 14
            // allocations for passing the 'func' argument to
            // 'bdef_BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
              bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
      }

DEFINE_TEST_CASE(7) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FREE FUNCTION POINTER
        // Concerns:
        //   That the 'bdef_BindUtil::bindA' static method returns a
        //   'bdef_Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 2 of 'bdef_BindUtil::bind': call
        //   'bdef_BindUtil::bindA' with the address of the 'func1' class
        //   method of the utility class 'bdef_Bind_TestFunctionsNoAlloc' or
        //   'bdef_Bind_TestFunctionsAlloc', that take an instance 'mX' of the
        //   class 'NoAllocTestType' or 'AllocTestType', constructed with or
        //   without an allocator, and 6 parameter.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   no allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.  In addition, we use two separate
        //   allocators for the binder and for the objects, and track (using
        //   the 'setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdef_BindUtil::bindA(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
       printf("\nTESTING 'bdef_BindUtil::bindA' WITH FREE FUNCTION POINTER"
              "\n=========================================================\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
           &bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
           &bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In these two test sequences (using 'AllocTestType'), PARAMS
        // allocations should be requested (one per each argument) via the 'Z1'
        // allocator, as triggered in the 'AllocTestType::testFunc*' methods,
        // except if the function object is passed by value and copied into the
        // binder (with the optional allocator specified for the binder, 'Z2'),
        // in which case the copy uses 'Z2' instead of 'Z1' to supply memory.
        // All other allocations for the binder object should be requested via
        // 'Z2'.  In the case of passing a function object (no pointer), there
        // will still be 14 allocations for passing the 'func' argument to
        // 'bdef_BindUtil::bindA' by value.  No other default allocation should
        // occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
             &bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // The difference between this sequence and the preceding one is
            // that, by using no place-holders, the bound arguments will be
            // copied into the binder object and so the slots will be set to
            // 'Z2' instead of the default when invoking the
            // 'AllocTestType::testFunction*' methods.  In the case of passing
            // a function object (no pointer), there will still be 14
            // allocations for passing the 'func' argument to
            // 'bdef_BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bindA(Z2,
             &bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdef_Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
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
        //   Call 'bdef_BindUtil::bind' with an instance 'mX' of the class
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, invoked with parameter.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   no allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdef_BindUtil::bind' WITH FUNCTION OBJECT"
                   "\n==================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place regardless of whether place-holders are used or
        // not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                    mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                    mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // Since the bound function object is passed by value, no allocation
        // using 'Z1' should take place.  The copy of the function object into
        // the binder object requires 14 default allocations (plus another 14
        // for passing the 'func' argument by value for the
        // 'bdef_BindUtil::bind' method), and the PARAMS allocations triggered
        // by the invocation of the 'AllocTestType::testFunc*' methods will be
        // served by the allocator 'Z0' from the binder (who owns the copy of
        // the bound object) instead of the allocator 'Z1' of the bound object.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.  Since the bound function object is passed
            // by value, however, see the comment above.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE  = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                    mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS,
                         28+PARAMS == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur twice for each bound argument of
            // type 'AllocTestValue', as those are copied into the binder
            // object twice: once for making the tuple list type to be passed
            // to the binder constructor, and a second time for copying that
            // tuple into the binder.  Since the bound function object is
            // passed by value, however, see the comment above.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE  = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                    mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS,
                         28+3*PARAMS == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(5) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY ADDRESS
        // Concerns and plan:
        //   Identical to case 4, except passing '&mX' instead of 'mX'.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with the address of an instance 'mX'
        //   of the class 'NoAllocTestType' or 'AllocTestType', constructed
        //   with or without an allocator, invoked with 6 parameter.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
      printf("\nTESTING 'bdef_BindUtil::bind' WITH FUNCTION OBJECT POINTER"
             "\n==========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                   &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                   &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                   &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                                                   &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(4) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH MEMBER FUNCTION POINTER
        // Concerns:
        //   That the 'bdef_BindUtil::bind' static method returns a 'bdef_Bind'
        //   object that returns the correct value, has the appropriate side
        //   effects, and performs the proper amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with the 'testFunc1' method of the
        //   class 'NoAllocTestType' or 'AllocTestType', applied to a function
        //   object 'mX' constructed with or without an allocator and 1
        //   parameter.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation (for objects
        //   'mX' that do not require allocation) or the proper amount of
        //   allocation (for objects 'mX' that do require allocation) took
        //   place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
      printf("\nTESTING 'bdef_BindUtil::bind' WITH MEMBER FUNCTION POINTER"
             "\n==========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                      &NoAllocTestType::testFunc1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                      &NoAllocTestType::testFunc1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                        &AllocTestType::testFunc1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
                        &AllocTestType::testFunc1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(3) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FREE FUNCTION REFERENCE
        // Concerns and plans:
        //    Identical to case 2, except passing 'func1' instead of
        //    '&func1'.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
      printf("\nTESTING 'bdef_BindUtil::bind' WITH FREE FUNCTION REFERENCE"
             "\n==========================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
            bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
            bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
              bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
              bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(2) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FREE FUNCTION POINTER
        // Concerns:
        //   That the 'bdef_BindUtil::bind' static method returns a 'bdef_Bind'
        //   object that returns the correct value, has the appropriate side
        //   effects, and performs the proper amount of memory allocation.
        //
        // Plan:
        //   Call 'bdef_BindUtil::bind' with the address of the 'func1' class
        //   method of the class 'bdef_Bind_TestFunctionsNoAlloc' or
        //   'bdef_Bind_TestFunctionsNoAlloc', that takes a mutable function
        //   object 'mX', constructed with or without an allocator, and no
        //   parameter.  Upon invocation, verify that the return value and
        //   state of 'mX' (as modified by the 'testFunc1' method) are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdef_BindUtil::bind(Func const&, ...);
        // ------------------------------------------------------------------

        if (verbose)
        printf("\nTESTING 'bdef_BindUtil::bind' WITH FREE FUNCTION POINTER"
               "\n========================================================");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
           &bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (I1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
           &bdef_Bind_TestFunctionsNoAlloc::func1, &mX, // invocable
                                              I1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocTestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocTestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdef_PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
             &bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              _1) // bound arguments
                                           (V1)); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            LOOP_ASSERT(PARAMS, EXPECTED != X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdef_BindUtil::bind(
             &bdef_Bind_TestFunctionsAlloc::func1, &mX, // invocable
                                              V1) // bound arguments
                                                   ()); // invocation arguments

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
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

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            const int NA = Z0->numAllocations();

            typedef NoAllocTestType                      *FUNC;
            typedef bdef_Bind_BoundTuple1<PH1>            ListType;
            typedef bdef_Bind<bslmf_Nil, FUNC, ListType>  Bind;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
                const NoAllocTestType  EXPECTED(I1);
            const NoAllocTestType  DEFAULT;
            ASSERT(EXPECTED != X);
            ASSERT(DEFAULT  == X);

                  ListType  mL(_1);
            const ListType& L = mL;

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1(I1));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, L, Z0);  const Bind& B2 = mB2;
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2(N1));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3(I1));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z0);
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4(N1,N1,N1,N1,N1,N1));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            ASSERT(NA == Z0->numAllocations());
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            const int NA = Z0->numAllocations();

            typedef NoAllocTestType                        *FUNC;
            typedef bdef_Bind_BoundTuple1<NoAllocTestArg1>  ListType;
            typedef bdef_Bind<bslmf_Nil, FUNC, ListType>    Bind;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED(I1);
            const NoAllocTestType  DEFAULT;
            ASSERT(EXPECTED != X);
            ASSERT(DEFAULT  == X);

                  ListType  mL(I1);
            const ListType& L = mL;
                  ListType  mM(N1);
            const ListType& M = mM;

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, M, Z0);  const Bind& B2 = mB2;
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z0);
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            ASSERT(NA == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            typedef AllocTestType                        *FUNC;
            typedef bdef_Bind_BoundTuple1<PH1>            ListType;
            typedef bdef_Bind<bslmf_Nil, FUNC, ListType>  Bind;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            const AllocTestType  DEFAULT(Z1, NV1);
            ASSERT(EXPECTED != X);
            ASSERT(DEFAULT  != X);

                  ListType  mL(_1);
            const ListType& L = mL;

            Bind mB1(&mX, L); const Bind& B1 = mB1;
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1(V1));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, L, Z2); const Bind& B2 = mB2;
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2(NV1));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3(V1));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z2);
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4(NV1));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            typedef AllocTestType                        *FUNC;
            typedef bdef_Bind_BoundTuple1<AllocTestArg1>  ListType;
            typedef bdef_Bind<bslmf_Nil, FUNC, ListType>  Bind;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1, V1);
            const AllocTestType  DEFAULT(Z1, NV1);
            ASSERT(EXPECTED != X);
            ASSERT(DEFAULT  != X);

                  ListType  mL(V1);
            const ListType& L = mL;
                  ListType  mM(NV1);
            const ListType& M = mM;

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, M, Z2);  const Bind& B2 = mB2;
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT(EXPECTED != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z2);
            ASSERT(DEFAULT != X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
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
