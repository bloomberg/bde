// bdlf_bind_testn.t.cpp                                              -*-C++-*-
#include <bdlf_bind_testn.h>

#include <bdlf_bind.h>
#include <bdlf_bind_test.h>
#include <bdlf_placeholder.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_nil.h>

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // 'atoi'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// The 'bdlf_bind' component defines a parameterized type for binding an
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
// placeholders, and 'bdlf_bind' objects), and that (2) the binder correctly
// propagates its allocator to all objects under its management (including all
// the copies of the function objects and parameters that it owns).
//
// The 'bdlf_bind' is a large component by the number of lines of code, and
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
// component into 14 pieces.  This test component exercises 'bdlf::Bind'
// objects with 14 parameters for invocables which are function objects (by
// value or by address).  In addition, we exercise the following restraint: We
// test only all placeholders or all bound arguments (a more elaborate mix of
// placeholders and arguments is explored in the 'bdlf_bind' test driver).
//
// Our test plan proceeds by checking that the 'bdlf::Bind' constructors work
// with and without allocators as they should, and that the 'bdlf::Bind' object
// can be constructed explicitly (case 1).  Then we check
// 'bdlf::BindUtil::bind' in conjunction with free functions pointers (case 2)
// and references (case 3), member function pointers (case 4), function objects
// passed by address (case 5) or by value (case 6), in all cases using either
// values or placeholders as parameters.  Next we check 'bdlf::BindUtil::bindA'
// in conjunction with free function pointers (case 7) or references (case 8),
// member function pointers (case 9), or function objects (cases 10 and 11) in
// the same way as cases 2--6.  Then, we check 'bdlf::BindUtil::bindR' again in
// the same way as 'bdlf::BindUtil::bind' (cases 12--16).
// ----------------------------------------------------------------------------
// [ 1] bdlf::Bind(func, list, bslma::Allocator *ba = 0);
// [ 1] bdlf::Bind(const bdlf::Bind& original, bslma::Allocator *ba = 0);
// [ 2] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 3] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 4] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 5] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 6] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 7] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [18] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [19] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [10] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [11] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [12] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [13] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [14] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [15] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [16] bdlf::BindUtil::bindR(FUNC const& func, ...);
// ----------------------------------------------------------------------------

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
//                    MACROS FOR VARIABLE ARGUMENT LISTS
// ----------------------------------------------------------------------------

// Count
#ifndef BBT_n
#define BBT_n 2
#endif

// S with parameter count appended
#ifndef BBT_C
#define BBT_C(S) S##2
#endif

// Repeat comma-separated S once per number of parameters with number appended
#ifndef BBT_N
#define BBT_N(S) S##1, S##2
#endif

// Repeat comma-separated S once per number of parameters
#ifndef BBT_R
#define BBT_R(S) S, S
#endif

#define BBT_FUNCAn           BBT_C(bdlf::Bind_TestFunctionsAlloc::func)
#define BBT_FUNCNAn          BBT_C(bdlf::Bind_TestFunctionsNoAlloc::func)
#define BBT_TESTFUNCAn       BBT_C(AllocTestType::testFunc)
#define BBT_TESTFUNCNAn      BBT_C(NoAllocTestType::testFunc)

#define BBT_ALLOCTESTARGn    BBT_N(AllocTestArg)
#define BBT_In               BBT_N(I)
#define BBT_NOALLOCTESTARGn  BBT_N(NoAllocTestArg)
#define BBT_NVn              BBT_N(NV)
#define BBT_PHn              BBT_N(PH)
#define BBT_Vn               BBT_N(V)
#define BBT_phn              BBT_N(_)

#define BBT_N1n              BBT_R(N1)

#define BBT_N1_N_N1                                                           \
    N1                                                                        \
    , ( 1 <= BBT_n ?  1 : N1) , ( 2 <= BBT_n ?  2 : N1)                       \
    , ( 3 <= BBT_n ?  3 : N1) , ( 4 <= BBT_n ?  4 : N1)                       \
    , ( 5 <= BBT_n ?  5 : N1) , ( 6 <= BBT_n ?  6 : N1)                       \
    , ( 7 <= BBT_n ?  7 : N1) , ( 8 <= BBT_n ?  8 : N1)                       \
    , ( 9 <= BBT_n ?  9 : N1) , (10 <= BBT_n ? 10 : N1)                       \
    , (11 <= BBT_n ? 11 : N1) , (12 <= BBT_n ? 12 : N1)                       \
    , (13 <= BBT_n ? 13 : N1) , (14 <= BBT_n ? 14 : N1)

#define BBT_Z0_Z2_Z0                                                          \
    Z0                                                                        \
    , ( 1 <= BBT_n ? Z2 : Z0) , ( 2 <= BBT_n ? Z2 : Z0)                       \
    , ( 3 <= BBT_n ? Z2 : Z0) , ( 4 <= BBT_n ? Z2 : Z0)                       \
    , ( 5 <= BBT_n ? Z2 : Z0) , ( 6 <= BBT_n ? Z2 : Z0)                       \
    , ( 7 <= BBT_n ? Z2 : Z0) , ( 8 <= BBT_n ? Z2 : Z0)                       \
    , ( 9 <= BBT_n ? Z2 : Z0) , (10 <= BBT_n ? Z2 : Z0)                       \
    , (11 <= BBT_n ? Z2 : Z0) , (12 <= BBT_n ? Z2 : Z0)                       \
    , (13 <= BBT_n ? Z2 : Z0) , (14 <= BBT_n ? Z2 : Z0)

#if BBT_n > 0

// Parameter with extra arguments; elide final comma for 0 count
#define BBT_K(M, A)             M, A
// Initialized object declaration, avoiding function parse
#define BBT_OBJ(N, A)           N(A)
// Specialized Bind_BoundTuplen template, avoiding 0 specialization
#define BBT_BIND_BOUNDTUPLEn(A) bdlf::BBT_C(Bind_BoundTuple)<A>

#else

#define BBT_K(M, A)             M
#define BBT_OBJ(N, A)           N
#define BBT_BIND_BOUNDTUPLEn(A) bdlf::BBT_C(Bind_BoundTuple)

#endif

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

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

// The whole bdlf_bind component currently works with up to 14 arguments.

const int BIND_MAX_ARGUMENTS = 14;
const int NUM_SLOTS = BIND_MAX_ARGUMENTS+1;

// This test driver.

const int PARAMS = BBT_n;

// Nil value (uninitialized).

const int N1 = -1;

// Typedefs for slots, argument, and test types.

typedef bdlf::Bind_TestSlotsNoAlloc    SlotsNoAlloc;

typedef bdlf::Bind_TestArgNoAlloc<1>   NoAllocTestArg1;
typedef bdlf::Bind_TestArgNoAlloc<2>   NoAllocTestArg2;
typedef bdlf::Bind_TestArgNoAlloc<3>   NoAllocTestArg3;
typedef bdlf::Bind_TestArgNoAlloc<4>   NoAllocTestArg4;
typedef bdlf::Bind_TestArgNoAlloc<5>   NoAllocTestArg5;
typedef bdlf::Bind_TestArgNoAlloc<6>   NoAllocTestArg6;
typedef bdlf::Bind_TestArgNoAlloc<7>   NoAllocTestArg7;
typedef bdlf::Bind_TestArgNoAlloc<8>   NoAllocTestArg8;
typedef bdlf::Bind_TestArgNoAlloc<9>   NoAllocTestArg9;
typedef bdlf::Bind_TestArgNoAlloc<10>  NoAllocTestArg10;
typedef bdlf::Bind_TestArgNoAlloc<11>  NoAllocTestArg11;
typedef bdlf::Bind_TestArgNoAlloc<12>  NoAllocTestArg12;
typedef bdlf::Bind_TestArgNoAlloc<13>  NoAllocTestArg13;
typedef bdlf::Bind_TestArgNoAlloc<14>  NoAllocTestArg14;

typedef bdlf::Bind_TestTypeNoAlloc     NoAllocTestType;

typedef bdlf::Bind_TestSlotsAlloc      SlotsAlloc;

typedef bdlf::Bind_TestArgAlloc<1>     AllocTestArg1;
typedef bdlf::Bind_TestArgAlloc<2>     AllocTestArg2;
typedef bdlf::Bind_TestArgAlloc<3>     AllocTestArg3;
typedef bdlf::Bind_TestArgAlloc<4>     AllocTestArg4;
typedef bdlf::Bind_TestArgAlloc<5>     AllocTestArg5;
typedef bdlf::Bind_TestArgAlloc<6>     AllocTestArg6;
typedef bdlf::Bind_TestArgAlloc<7>     AllocTestArg7;
typedef bdlf::Bind_TestArgAlloc<8>     AllocTestArg8;
typedef bdlf::Bind_TestArgAlloc<9>     AllocTestArg9;
typedef bdlf::Bind_TestArgAlloc<10>    AllocTestArg10;
typedef bdlf::Bind_TestArgAlloc<11>    AllocTestArg11;
typedef bdlf::Bind_TestArgAlloc<12>    AllocTestArg12;
typedef bdlf::Bind_TestArgAlloc<13>    AllocTestArg13;
typedef bdlf::Bind_TestArgAlloc<14>    AllocTestArg14;

typedef bdlf::Bind_TestTypeAlloc       AllocTestType;

// Placeholder types for the corresponding _1, _2, etc.

typedef bdlf::PlaceHolder<1>  PH1;
typedef bdlf::PlaceHolder<2>  PH2;
typedef bdlf::PlaceHolder<3>  PH3;
typedef bdlf::PlaceHolder<4>  PH4;
typedef bdlf::PlaceHolder<5>  PH5;
typedef bdlf::PlaceHolder<6>  PH6;
typedef bdlf::PlaceHolder<7>  PH7;
typedef bdlf::PlaceHolder<8>  PH8;
typedef bdlf::PlaceHolder<9>  PH9;
typedef bdlf::PlaceHolder<10> PH10;
typedef bdlf::PlaceHolder<11> PH11;
typedef bdlf::PlaceHolder<12> PH12;
typedef bdlf::PlaceHolder<13> PH13;
typedef bdlf::PlaceHolder<14> PH14;

// ============================================================================
//                MACROS EXPORTING INITIALIZATION OUT OF MAIN
// ----------------------------------------------------------------------------
#define DECLARE_MAIN_VARIABLES                                                \
    /*                                                                        \
    // The following machinery is for use in conjunction with the             \
    // 'SlotsNoAlloc::resetSlots' and 'SlotsNoAlloc::verifySlots' functions.  \
    // The slots are set when the corresponding function objector free        \
    // function is called with 'NumArgs' arguments.
    */                                                                        \
                                                                              \
    const int NO_ALLOC_SLOTS[NUM_SLOTS]= {                                    \
        /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14 */       \
          BBT_N1_N_N1                                                         \
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
    /*                                                                        \
    // The following machinery is for use in conjunction with the             \
    // 'SlotsAlloc::resetSlots' and 'SlotsAlloc::verifySlots' functions.  The \
    // slots are set when the corresponding function objector free function is\
    // called with 'NumArgs' arguments.
    */                                                                        \
                                                                              \
    bslma::TestAllocator allocator0(veryVeryVerbose);                         \
    bslma::TestAllocator allocator1(veryVeryVerbose);                         \
    bslma::TestAllocator allocator2(veryVeryVerbose);                         \
                                                                              \
    bslma::TestAllocator *Z0 = &allocator0;                                   \
    bslma::TestAllocator *Z1 = &allocator1;                                   \
    bslma::TestAllocator *Z2 = &allocator2;                                   \
                                                                              \
    bslma::DefaultAllocatorGuard allocGuard(Z0);                              \
    SlotsAlloc::setZ0(Z0);                                                    \
    SlotsAlloc::setZ1(Z1);                                                    \
    SlotsAlloc::setZ2(Z2);                                                    \
                                                                              \
    const bslma::Allocator *ALLOC_SLOTS[NUM_SLOTS] = {                        \
        /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14 */       \
          BBT_Z0_Z2_Z0                                                        \
    };                                                                        \
    const bslma::Allocator *ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {                \
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
    const AllocTestArg14 V14(1414), NV14(-1414);

// ============================================================================
//                                TEST CASES
// ----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)

DEFINE_TEST_CASE(17) {
        // ------------------------------------------------------------------
        // TESTING 'bslmf::IsBitwiseMoveable<bdlf::Bind<R,F,L>>'
        //
        // Concerns:
        //: 1 The bitwise moveable trait is true for 'Bind<RET,FUNC,LIST>'
        //:   if the bitwise moveable trait is true for 'FUNC' and for all
        //:   of the types in 'LIST'.
        //: 2 Placeholders are treated as bitwise moveable.
        //: 3 The bitwise moveable trait is false for
        //:   'Bind<RET,FUNC,LIST>' if the bitwise moveable trait is
        //:   false for 'FUNC'.
        //: 4 The bitwise moveable trait is false for
        //:   'Bind<RET,FUNC,LIST>' if the bitwise moveable trait is
        //:   false for any of the types in 'LIST'.
        //: 5 The 'RET' type has no affect on whether
        //:   'Bind<RET,FUNC,LIST>' has the bitwise moveable trait.
        //: 6 The concerns above apply to the types as returned from
        //:   'BindUtil::bind', 'BindUtil::bindA', and
        //:   'BindUtil::bindR'.
        //
        // Plan:
        //: 1 For concern 1, instantate 'Bind' with a bitwise moveable
        //:   'FUNC' and a 'LIST' type comprising types that are all bitwise
        //:   moveable.  Verify that the resulting specialization has the
        //:   bitwise moveable trait.
        //: 2 For concern 2, repeat step 1 except replace one of the types in
        //:   'LIST' with a placeholder.  Verify that the resulting
        //:   specialization has the bitwise moveable trait.
        //: 3 For concern 3, repeat step 2 except replace 'FUNC' with a type
        //:   that is not bitwise moveable.  Verify that the resulting
        //:   specialization *does not* have the bitwise moveable trait.
        //: 4 For concern 4, repeat step 2 except replace one of the types in
        //:   'LIST' with a type that is not bitwise moveable.  Verify that
        //:   the resulting specialization *does not* have the bitwise
        //:   moveable trait.
        //: 5 For concern 5, repeat step 2 except replace 'RET' with a type
        //:   that is not bitwise moveable.  Verify that the resulting
        //:   specialization *still has* the bitwise moveable trait.
        //: 6 For concern 6, create a function template,
        //:   'isBitwiseMoveableArg(const T&)', that returns true iff 'T' has
        //:   the bitwise moveable trait. Invoke 'isBitwiseMoveableArg' on
        //:   calls to
        //:   'BindUtil::bind', 'BindUtil::bindA', and
        //:   'BindUtil::bindR' with arguments that would result in the
        //:   various combinations above.
        //
        // Testing:
        //      bslmf::IsBitwiseMoveable<bdlf::Bind<RET,FUNC,LIST>>

        if (verbose)
            printf("\nTESTING 'bslmf::IsBitwiseMoveable<bdlf::Bind<R,F,L>>'"
                   "\n====================================================\n");

#if BBT_n > 0
        (void) veryVeryVerbose;

        typedef bdlf::Bind_TestArgNoAlloc<1> BitwiseArg;
        typedef bdlf::Bind_TestArgNoAlloc<2> BitwiseRet;
        typedef bdlf::Bind_TestTypeNoAlloc   BitwiseInvocable;

        typedef bdlf::Bind_TestArgAlloc<1>   NonBitwiseArg;
        typedef bdlf::Bind_TestArgAlloc<2>   NonBitwiseRet;
        typedef bdlf::Bind_TestTypeAlloc     NonBitwiseInvocable;

#if   BBT_n ==  1
    #define BWM_ARGS(F,L) L
#elif BBT_n ==  2
    #define BWM_ARGS(F,L) F,L
#elif BBT_n ==  3
    #define BWM_ARGS(F,L) F,2,L
#elif BBT_n ==  4
    #define BWM_ARGS(F,L) F,2,3,L
#elif BBT_n ==  5
    #define BWM_ARGS(F,L) F,2,3,4,L
#elif BBT_n ==  6
    #define BWM_ARGS(F,L) F,2,3,4,5,L
#elif BBT_n ==  7
    #define BWM_ARGS(F,L) F,2,3,4,5,6,L
#elif BBT_n ==  8
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,L
#elif BBT_n ==  9
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,8,L
#elif BBT_n == 10
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,8,9,L
#elif BBT_n == 11
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,8,9,10,L
#elif BBT_n == 12
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,8,9,10,11,L
#elif BBT_n == 13
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,8,9,10,11,12,L
#elif BBT_n == 14
    #define BWM_ARGS(F,L) F,2,3,4,5,6,7,8,9,10,11,12,13,L
#else
    #error Out of range value for BBT_n
#endif

        typedef BBT_BIND_BOUNDTUPLEn(BBT_R(int)) IntList;
        struct Bitwise {
            typedef BitwiseArg BBT_C(PH);
            typedef BBT_BIND_BOUNDTUPLEn(BBT_PHn) List;
        };
        typedef Bitwise::List  BitwiseList;
        struct NonBitwise {
            typedef NonBitwiseArg BBT_C(PH);
            typedef BBT_BIND_BOUNDTUPLEn(BBT_PHn) List;
        };
        typedef NonBitwise::List NonBitwiseList;

        typedef bdlf::Bind_TestUtil TU;

        BitwiseArg           bwFirstArg(1);
        BitwiseArg           bwLastArg(BBT_n);
        BitwiseInvocable     bwFunc;
        NonBitwiseArg        nonBwFirstArg(1);
        NonBitwiseArg        nonBwLastArg(BBT_n);
        NonBitwiseInvocable  nonBwFunc;
        bslma::TestAllocator alloc0(veryVeryVerbose);

        using namespace bdlf::PlaceHolders;

#define BWM_TEST(R,F,L,EXP) \
        ASSERT(!EXP == !(bslmf::IsBitwiseMoveable<bdlf::Bind<R,F,L> >::VALUE));

        if (veryVerbose) printf("Testing Bind\n");
        //       Return type    Invocable type       Args list       Exp
        //       =============  ===================  ==============  =====
        BWM_TEST(bslmf::Nil   , BitwiseInvocable   , IntList       , true );
        BWM_TEST(BitwiseRet   , BitwiseInvocable   , BitwiseList   , true );
        BWM_TEST(BitwiseRet   , NonBitwiseInvocable, BitwiseList   , false);
        BWM_TEST(BitwiseRet   , BitwiseInvocable   , NonBitwiseList, false);
        BWM_TEST(NonBitwiseRet, BitwiseInvocable   , BitwiseList   , true );
#undef BWM_TEST

#define BWM_TEST(RET,FUNC,FIRST,LAST,EXPECT)                                  \
        ASSERT(!EXPECT == !(TU::isBitwiseMoveableType(                        \
                  bdlf::BindUtil::bind(FUNC,BWM_ARGS(FIRST,LAST)))));         \
        ASSERT(!EXPECT == !(TU::isBitwiseMoveableType(                        \
            bdlf::BindUtil::bindR<RET>(FUNC,BWM_ARGS(FIRST,LAST)))));         \
//        ASSERT(!EXPECT == !(TU::isBitwiseMoveableType(                        \
//            bdlf::BindUtil::bindA(&alloc0,FUNC,BWM_ARGS(FIRST,LAST)))));

        if (veryVerbose) printf("Testing BindUtil::bind[RA]\n");
        //       Return type    Invocable  First Arg      Last Arg      Exp
        //       =============  =========  =============  ============  =====
        BWM_TEST(bslmf::Nil   , bwFunc   , bwFirstArg   , bwLastArg   , true );
        BWM_TEST(BitwiseRet   , bwFunc   , _1           , bwLastArg   , true );
        BWM_TEST(BitwiseRet   , nonBwFunc, _1           , bwLastArg   , false);
        BWM_TEST(BitwiseRet   , bwFunc   , _1           , nonBwLastArg, false);
        BWM_TEST(NonBitwiseRet, bwFunc   , _1           , bwLastArg   , true );
#undef BWM_TEST
#undef BWM_ARGS
#endif
      }

DEFINE_TEST_CASE(16) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FUNCTION OBJECT PASSED BY VALUE
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindR' static method returns a
        //   'bdlf::Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bindR' with an instance 'mX' of the class
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, invoked with N parameters.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   no allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindR' WITH FUNCTION OBJECT"
                   "\n====================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place regardless of whether place-holders are used or
        // not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                  BBT_K(mX, BBT_phn))(BBT_In));

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

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                         BBT_K(mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // Since the bound function object is passed by value, no allocation
        // using 'Z1' should take place.  The copy of the function object into
        // the binder object requires 14 default allocations (plus another 14
        // for passing the 'func' argument by value for the
        // 'bdlf::BindUtil::bind' method), and the PARAMS allocations triggered
        // by the invocation of the 'AllocTestType::testFunc*' methods will be
        // served by the allocator 'Z0' from the binder (who owns the copy of
        // the bound object) instead of the allocator 'Z1' of the bound object.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

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

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                  BBT_K(mX, BBT_phn))(BBT_Vn));

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

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                         BBT_K(mX, BBT_Vn))());

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
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FUNCTION OBJECT PASSED BY ADDRESS
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindR' static method returns a
        //   'bdlf::Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bindR' with the address of an instance 'mX'
        //   of the class 'NoAllocTestType' or 'AllocTestType', constructed
        //   with or without an allocator, invoked with N parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindR' WITH FUNCTION OBJECT "
                   "POINTER"
                   "\n====================================================="
                   "=======\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_K(&mX, BBT_Vn))());

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
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH MEMBER FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindR' static method returns a
        //   'bdlf::Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bindR' with the 'testFuncN'
        //   method of the classes 'NoAllocTestType' or 'AllocTestType',
        //   applied to a function object 'mX' constructed with or without an
        //   allocator and N parameters.  Upon invocation,
        //   verify that the return value and state of 'mX' are as expected,
        //   and that no allocation (for objects 'mX' that do not require
        //   allocation) or the proper amount of allocation (for objects 'mX'
        //   that do require allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindR' WITH MEMBER FUNCTION "
                   "POINTER"
                   "\n====================================================="
                   "=======\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tmember function pointer because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_TESTFUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_TESTFUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_TESTFUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_TESTFUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
#endif
      }

DEFINE_TEST_CASE(13) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FREE FUNCTION REFERENCES
        //
        // Concerns and plans:
        //   Identical to case 12, except passing 'funcN' instead of
        //   '&funcN'.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindR' WITH FREE FUNCTION "
                   "REFERENCE"
                   "\n==================================================="
                   "=========\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tfree function reference because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
#endif
      }

DEFINE_TEST_CASE(12) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDR WITH FREE FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindR' static method returns a
        //   'bdlf::Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bindR' with the address of the class method
        //   'funcN' of the utility class 'bdlf::Bind_TestFunctionsNoAlloc' or
        //   'bdlf::Bind_TestFunctionsAlloc', that takes a mutable function
        //   object 'mX', constructed with or without an allocator, and N
        //   parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' (as modified by the 'testFuncN' method) are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING 'bdlf::BindUtil::bindR' WITH FREE FUNCTION POINTER"
                "\n=========================================================="
                "\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tfree function pointer because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
#endif
      }

DEFINE_TEST_CASE(11) {
#if 0
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY VALUE
        //
        // Concerns and plan:
        //   Identical to case 7, except passing 'mX' instead of '&mX'.
        //
        // Testing:
        //   bdlf::BindUtil::bindA(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING 'bdlf::BindUtil::bindA' WITH FUNCTION OBJECT VALUE"
                "\n=========================================================="
                "\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                  Z2,
                                                  BBT_K(mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
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
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                         Z2,
                                                         BBT_K(mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
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
        // to 'bdlf::BindUtil::bindA' by value.  No other default allocation
        // should occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(Z1);
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                  Z2,
                                                  BBT_K(mX, BBT_phn))(BBT_Vn));

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
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
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

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                         Z2,
                                                         BBT_K(mX, BBT_Vn))());

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
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // #if 0
      }

DEFINE_TEST_CASE(10) {
#if 0
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY ADDRESS
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindA' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 5 of 'bdlf::BindUtil::bind': call
        //   'bdlf::BindUtil::bindA' with the address of an instance 'mX' of
        //   the class 'NoAllocTestType' or 'AllocTestType', constructed with
        //   or without an allocator, invoked with N parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.  In
        //   addition, we use two separate allocators for the binder and for
        //   the objects, and track (using the 'SlotsNoAlloc::setSlots' and
        //   'SlotsAlloc::setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdlf::BindUtil::bindA(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindA' WITH FUNCTION OBJECT "
                   "POINTER"
                   "\n====================================================="
                   "=======\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
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
        // 'bdlf::BindUtil::bindA' by value.  No other default allocation
        // should occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsAlloc::verifySlots(
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
            // 'bdlf::BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_K(&mX, BBT_Vn))());

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
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // #if 0
}

DEFINE_TEST_CASE(9) {
#if 0
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH MEMBER FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindA' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 4 of 'bdlf::BindUtil::bind': call
        //   'bdlf::BindUtil::bindA' with the 'testFunc6' method of the
        //   class 'NoAllocTestType' or 'AllocTestType', applied to a
        //   function object 'mX' constructed with or without an allocator and
        //   N parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation (for objects
        //   'mX' that do not require allocation) or the proper amount of
        //   allocation (for objects 'mX' that do require allocation) took
        //   place.  In addition, we use two separate allocators for the binder
        //   and for the objects, and track (using the 'SlotsNoAlloc::setSlots'
        //   and 'SlotsAlloc::setSlots' test functions) which allocator was
        //   used.
        //
        // Testing:
        //   bdlf::BindUtil::bindA(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindA' WITH MEMBER FUNCTION "
                   "POINTER"
                   "\n====================================================="
                   "=======\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tmember function pointer because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
                const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_TESTFUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_TESTFUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
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
        // 'bdlf::BindUtil::bindA' by value.  No other default allocation
        // should occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_TESTFUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsAlloc::verifySlots(
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
            // 'bdlf::BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_TESTFUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif
#endif // #if 0
      }

DEFINE_TEST_CASE(8) {
#if 0
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FREE FUNCTION REFERENCE
        //
        // Concerns and plan:
        //   Identical to case 7, except passing 'funcN' instead of
        //   '&funcN'.
        //
        // Testing:
        //   bdlf::BindUtil::bindA(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bindA' WITH FREE FUNCTION "
                   "REFERENCE"
                   "\n==================================================="
                   "=========\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tfree function reference because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
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
        // 'bdlf::BindUtil::bindA' by value.  No other default allocation
        // should occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsAlloc::verifySlots(
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
            // 'bdlf::BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif
#endif // #if 0
      }

DEFINE_TEST_CASE(7) {
#if 0
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BINDA WITH FREE FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindA' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   The plan is similar to case 2 of 'bdlf::BindUtil::bind': call
        //   'bdlf::BindUtil::bindA' with the address of the 'funcN' class
        //   method of the utility class 'bdlf::Bind_TestFunctionsNoAlloc' or
        //   'bdlf::Bind_TestFunctionsAlloc', that take an instance 'mX' of the
        //   class 'NoAllocTestType' or 'AllocTestType', constructed with or
        //   without an allocator, and N parameters.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   no allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.  In addition, we use two separate
        //   allocators for the binder and for the objects, and track (using
        //   the 'setSlots' test functions) which allocator was used.
        //
        // Testing:
        //   bdlf::BindUtil::bindA(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING 'bdlf::BindUtil::bindA' WITH FREE FUNCTION POINTER"
                "\n=========================================================="
                "\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tfree function pointer because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place since the call should be logically equivalent to a
        // call to the bound object, with the arguments either passed directly
        // (with placeholders) or copied as bound arguments inside the binder
        // object and the copies passed to the bound object invocation.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                                 NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsNoAlloc::verifySlots(
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
        // 'bdlf::BindUtil::bindA' by value.  No other default allocation
        // should occur ever, since all allocators are passed explicitly.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_DFLT_ALLOCS = Z0->numAllocations()
                                      - NUM_DFLT_ALLOCS_BEFORE;
            const int NUM_ALLOCS      = Z1->numAllocations()
                                      - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::Bind_TestSlotsAlloc::verifySlots(
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
            // 'bdlf::BindUtil::bindA' by value.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::Bind_TestSlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
            LOOP_ASSERT(PARAMS, bdlf::Bind_TestSlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif
#endif // #if 0
      }

DEFINE_TEST_CASE(6) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY VALUE
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bind' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bind' with an instance 'mX' of the class
        //   'NoAllocTestType' or 'AllocTestType', constructed with or without
        //   an allocator, invoked with parameters.  Upon invocation, verify
        //   that the return value and state of 'mX' are as expected, and that
        //   no allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bind(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bdlf::BindUtil::bind' WITH FUNCTION OBJECT"
                   "\n==================================================\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case (using 'NoAllocTestType'), no allocation at all
        // should take place regardless of whether place-holders are used or
        // not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                  BBT_K(mX, BBT_phn))(BBT_In));

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

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                         BBT_K(mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // Since the bound function object is passed by value, no allocation
        // using 'Z1' should take place.  The copy of the function object into
        // the binder object requires 14 default allocations (plus another 14
        // for passing the 'func' argument by value for the
        // 'bdlf::BindUtil::bind' method), and the PARAMS allocations triggered
        // by the invocation of the 'AllocTestType::testFunc*' methods will be
        // served by the allocator 'Z0' from the binder (who owns the copy of
        // the bound object) instead of the allocator 'Z1' of the bound object.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

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

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                  BBT_K(mX, BBT_phn))(BBT_Vn));

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

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                         BBT_K(mX, BBT_Vn))());

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
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY ADDRESS
        //
        // Concerns and plan:
        //   Identical to case 4, except passing '&mX' instead of 'mX'.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bind' with the address of an instance 'mX'
        //   of the class 'NoAllocTestType' or 'AllocTestType', constructed
        //   with or without an allocator, invoked with N parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bind(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING 'bdlf::BindUtil::bind' WITH FUNCTION OBJECT POINTER"
                "\n==========================================================="
                "\n");

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_K(&mX, BBT_Vn))());

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
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH MEMBER FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bind' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bind' with the 'testFuncN' method of the
        //   class 'NoAllocTestType' or 'AllocTestType', applied to a function
        //   object 'mX' constructed with or without an allocator and N
        //   parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' are as expected, and that no allocation (for objects
        //   'mX' that do not require allocation) or the proper amount of
        //   allocation (for objects 'mX' that do require allocation) took
        //   place.
        //
        // Testing:
        //   bdlf::BindUtil::bind(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING 'bdlf::BindUtil::bind' WITH MEMBER FUNCTION POINTER"
                "\n==========================================================="
                "\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tmember function pointer because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_TESTFUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_TESTFUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_TESTFUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_TESTFUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
#endif
      }

DEFINE_TEST_CASE(3) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FREE FUNCTION REFERENCE
        //
        // Concerns and plans:
        //    Identical to case 2, except passing 'funcN' instead of
        //    '&funcN'.
        //
        // Testing:
        //   bdlf::BindUtil::bind(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING 'bdlf::BindUtil::bind' WITH FREE FUNCTION REFERENCE"
                "\n==========================================================="
                "\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tfree function reference because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
#endif
      }

DEFINE_TEST_CASE(2) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BINDUTIL::BIND WITH FREE FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bind' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bind' with the address of the 'funcN' class
        //   method of the class 'bdlf::Bind_TestFunctionsNoAlloc' or
        //   'bdlf::Bind_TestFunctionsNoAlloc', that takes a mutable function
        //   object 'mX', constructed with or without an allocator, and no
        //   parameters.  Upon invocation, verify that the return value and
        //   state of 'mX' (as modified by the 'testFuncN' method) are as
        //   expected, and that no allocation (for objects 'mX' that do not
        //   require allocation) or the proper amount of allocation (for
        //   objects 'mX' that do require allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bind(Func const&, ...);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                  "\nTESTING 'bdlf::BindUtil::bind' WITH FREE FUNCTION POINTER"
                  "\n========================================================="
                  "\n");
#if BBT_n == 14
        if (verbose)
            printf(
                "\n\tThis test driver cannot exercise 14 parameters on a"
                "\n\tfree function pointer because the test apparatus"
                "\n\trequires that the first argument be a pointer to the test"
                "\n\tobject, leaving only room for up to 13 parameters.\n");
#else
        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const int NUM_ALLOCS = Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

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
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

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
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const int NUM_DFLT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

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
#endif
      }

DEFINE_TEST_CASE(1) {
        DECLARE_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDEF_BIND CONSTRUCTORS
        //
        // Concerns:
        //   That the constructors build a 'bdlf::Bind' object that has the
        //   proper members so that, when the bound object is invoked, it is
        //   put into the correct state and returns the proper value.
        //
        // Plan:
        //   Construct a 'bdlf::Bind' object using the four constructors (the
        //   two that take a list of arguments and an allocator or not, and the
        //   copy constructors that take an allocator or not).  Then invoke the
        //   bound object and verify that it is put into the correct state and
        //   returns the proper value.
        //
        // Testing:
        //   bdlf::Bind(func, list, bslma::Allocator *ba = 0);
        //   bdlf::Bind(const bdlf::Bind& original, bslma::Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDEF_BIND CONSTRUCTORS"
                            "\n==============================\n");

        using namespace bdlf::PlaceHolders;

        if (verbose) printf("\tTest types that do *not* take an allocator.\n");

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            const int NA = Z0->numAllocations();

            typedef NoAllocTestType                        *FUNC;
            typedef BBT_BIND_BOUNDTUPLEn(BBT_PHn)           ListType;
            typedef bdlf::Bind<bslmf::Nil, FUNC, ListType>  Bind;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
                const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            const NoAllocTestType  DEFAULT;
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            ASSERT(DEFAULT  == X);

                  ListType  BBT_OBJ(mL, BBT_phn);
            const ListType& L = mL;

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1(BBT_In));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, L, Z0);  const Bind& B2 = mB2;
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2(BBT_N1n));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3(BBT_In));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z0);
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4(BBT_N1n));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            ASSERT(NA == Z0->numAllocations());
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            const int NA = Z0->numAllocations();

            typedef NoAllocTestType                           *FUNC;
            typedef BBT_BIND_BOUNDTUPLEn(BBT_NOALLOCTESTARGn)  ListType;
            typedef bdlf::Bind<bslmf::Nil, FUNC, ListType>     Bind;

                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            const NoAllocTestType  DEFAULT;
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            ASSERT(DEFAULT  == X);

                  ListType  BBT_OBJ(mL, BBT_In);
            const ListType& L = mL;
                  ListType  BBT_OBJ(mM, BBT_N1n);
            const ListType& M = mM;

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, M, Z0);  const Bind& B2 = mB2;
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z0);
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            ASSERT(NA == Z0->numAllocations());
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            typedef AllocTestType                          *FUNC;
            typedef BBT_BIND_BOUNDTUPLEn(BBT_PHn)           ListType;
            typedef bdlf::Bind<bslmf::Nil, FUNC, ListType>  Bind;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            const AllocTestType  DEFAULT(BBT_K(Z1, BBT_NVn));
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            ASSERT((BBT_n > 0) == (DEFAULT  != X));

                  ListType  BBT_OBJ(mL, BBT_phn);
            const ListType& L = mL;

            Bind mB1(&mX, L); const Bind& B1 = mB1;
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1(BBT_Vn));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, L, Z2); const Bind& B2 = mB2;
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2(BBT_NVn));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3(BBT_Vn));
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z2);
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4(BBT_NVn));
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
            typedef AllocTestType                           *FUNC;
            typedef BBT_BIND_BOUNDTUPLEn(BBT_ALLOCTESTARGn)  ListType;
            typedef bdlf::Bind<bslmf::Nil, FUNC, ListType>   Bind;

                  AllocTestType  mX(Z1);
            const AllocTestType& X = mX;
            const AllocTestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            const AllocTestType  DEFAULT(BBT_K(Z1, BBT_NVn));
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            ASSERT((BBT_n > 0) == (DEFAULT  != X));

                  ListType  BBT_OBJ(mL, BBT_Vn);
            const ListType& L = mL;
                  ListType  BBT_OBJ(mM, BBT_NVn);
            const ListType& M = mM;

            Bind mB1(&mX, L);  const Bind& B1 = mB1;
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB1());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB2(&mX, M, Z2);  const Bind& B2 = mB2;
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB2());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB3(B1);
            ASSERT((BBT_n > 0) == (EXPECTED != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB3());
            ASSERT(EXPECTED == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }

            Bind mB4(B2, Z2);
            ASSERT((BBT_n > 0) == (DEFAULT != X));
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
            ASSERT(PARAMS == mB4());
            ASSERT(DEFAULT == X);
            if (veryVerbose) { printf("%d: X=", L_); X.print(); }
        }
      }

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                          \
      case NUMBER: {                                                          \
        testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose);              \
      } break
      CASE(17);
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
