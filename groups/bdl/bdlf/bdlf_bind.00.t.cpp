// bdlf_bind.00.t.cpp                                                 -*-C++-*-
#include <bdlf_bind.h>

// This test driver contains common test machinery for all 'bdlf_bind' test
// drivers.  If '#include'd after defining the 'BDLF_BIND_00T_AS_INCLUDE'
// macro it will just provide those common pieces (and no 'main' function).  If
// '#include'd after defining the 'BDLF_BIND_00T_AS_GENERATOR' macro it will
// provide the common pieces, a 'main' function with the test cases described
// in the test plan below.  The number of function arguments used by the common
// parts (and the generated test cases) are determined by the 'BBT_n' macro.
// If not defined, we will assign it the value 2.  If defined, the following
// macros must also be defined for the same number of function arguments:
//..
//: BBT_C(s) -- S with parameter count appended (S##BBT_n)
//:
//: BBT_N(S) -- Repeat comma-separated S once per number of parameters with
//:             number appended (S##1,S##2...S##BBT_n)
//:
//: BBT_R(S) -- Repeat comma-separated S once per number of parameters

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic ignored "-Wnoexcept-type"
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <bdlf_placeholder.h>

#include <bsla_unused.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmf_nil.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 17 parts, 'bdlf_bind.00.t.cpp' (common code, generated code for
// repeated tests), 'bdlf_bind.01.t.cpp' (argument-number-independent tests,
// usage example), and 'bdlf_bind.02.t.cpp' to 'bdlf_bind.16.t.cpp' (repeated
// test from argument count 0 to 14).
//
//                                Overview
//                                --------
// The 'bdlf_bind' component defines a parameterized type for binding an
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
// placeholders, and 'bdlf_bind' objects), and that (2) the binder correctly
// propagates its allocator to all objects under its management (including all
// the copies of the function objects and parameters that it owns).  In
// addition, we would like to make sure that (3) the signature of the invocable
// is respected, in that arguments that are passed by reference are not copied,
// and that const-correctness is respected.
//
// This is a large component by the number of lines of code, and even more so
// since it is a template and has at least *four* orthogonal/ dimensions: the
// family of factory methods ('bind', bindA', or 'bindR'), the nature of the
// bound object (free function pointer or reference, member function pointer,
// and function object by reference or by value), the number of arguments (from
// 0 up to 14 for function objects, and 0 up to 13 for free and member
// functions since the first argument is consumed by the test object pointer),
// and the invocation (any of the up to 2^14 possible combinations of
// placeholders or bound arguments).  In addition, in order to test the memory
// allocation model, we must use test classes that do and that do not allocate
// memory.  This is clearly a huge number of template instantiations to test.
//
// Instead of restricting the test coverage, we split the testing of this
// component into 17 parts.  Part 'bdlf_bind.00.t.cpp' (this part) contains
// common code used by more than one other test part.  The test part
// 'bdlf_bind.01.t.cpp' contains testing of the common parts as well as
// concerns that do not explicitly depend on the number of arguments passed to
// the bound object.  The test driver parts 'bdlf_bind.02.t.cpp' to
// 'bdlf_bind.16.t.cpp' contain tests from 0 argument to 14 arguments for the
// combinations of the family of factory methods ('bind', bindR', or 'bindS'),
// the nature of the bound object (free function pointer or reference, member
// function pointer, and function object by reference or by value), and the
// allocation model.  In addition, we exercise the following restraint: We test
// only all placeholders or all bound arguments (a more elaborate mix of
// placeholders and arguments is explored in the 'bdlf_bind.01.t.cpp' test
// driver part).
//
// See the Test Plan for the argument-number-indepentent tests in the file
// 'bdlf_bind.01.t.cpp'.
//
// Our argument-number-dependent tests first check that the 'bdlf::Bind'
// constructors work with and without allocators as they should, and that the
// 'bdlf::Bind' object can be constructed explicitly (case 1).  Then we check
// 'bdlf::BindUtil::bind' in conjunction with free functions pointers (case 2)
// and references (case 3), member function pointers (case 4), function objects
// passed by address (case 5) or by value (case 6), in all cases using either
// values or placeholders as parameters.  Next we check 'bdlf::BindUtil::bindA'
// in conjunction with free function pointers (case 7) or references (case 8),
// member function pointers (case 9), or function objects (cases 10 and 11) in
// the same way as cases 2--6.  Then, we check 'bdlf::BindUtil::bindR' again in
// the same way as 'bdlf::BindUtil::bind' (cases 12--17).
// ----------------------------------------------------------------------------
// [ 1] bdlf::Bind(func, list, bslma::Allocator *ba = 0);
// [ 1] bdlf::Bind(const bdlf::Bind& original, bslma::Allocator *ba = 0);
// [ 2] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 3] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 4] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 5] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 6] bdlf::BindUtil::bind(FUNC const& func, ...);
// [ 7] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [ 8] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [ 9] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [10] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [11] bdlf::BindUtil::bindA(bslma::Allocator *ba, FUNC const& func, ...);
// [12] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [13] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [14] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [15] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [16] bdlf::BindUtil::bindR(FUNC const& func, ...);
// [17] bslmf::IsBitwiseMoveable<bdlf::Bind<RET,FUNC,LIST>>
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

#ifndef BDLF_BIND_00T_AS_INCLUDE
void aSsErT(bool condition, const char *message, int line) BSLA_UNUSED;
#endif

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
//                   MACRO FOR TEST CASE FUNCTION DEFINITION
// ----------------------------------------------------------------------------

#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)

// ============================================================================
//                     MACROS FOR VARIABLE ARGUMENT LISTS
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

#define BBT_FUNCAn           BBT_C(TestFunctionsAlloc::func)
#define BBT_FUNCNAn          BBT_C(TestFunctionsNoAlloc::func)
#define BBT_TESTFUNCAn       BBT_C(AllocTestType::testFunc)
#define BBT_TESTFUNCNAn      BBT_C(NoAllocTestType::testFunc)

#define BBT_FUNCNEAn         BBT_C(TestFunctionsAlloc::funcNE)
#define BBT_FUNCNENAn        BBT_C(TestFunctionsNoAlloc::funcNE)
#define BBT_TESTFUNCNEAn     BBT_C(AllocNETestType::testFunc)
#define BBT_TESTFUNCNENAn    BBT_C(NoAllocNETestType::testFunc)

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

const int PARAMS = BBT_n;

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
const int NUM_SLOTS          = BIND_MAX_ARGUMENTS+1;

// Nil value (uninitialized).

const int N1 = -1;

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
//                 MACRO EXPORTING INITIALIZATION OUT OF MAIN
// ----------------------------------------------------------------------------

#ifdef BDLF_BIND_00T_AS_GENERATOR

#define DECLARE_BBTN_MAIN_VARIABLES                                           \
    /*
    // The following machinery is for use in conjunction with the
    // 'SlotsNoAlloc::resetSlots' and 'SlotsNoAlloc::verifySlots' functions.
    // The slots are set when the corresponding function objector free
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
    (void)NO_ALLOC_SLOTS;                                                     \
    (void)NO_ALLOC_SLOTS_DEFAULT;                                             \
                                                                              \
    /*                                                                        \
    // The following machinery is for use in conjunction with the
    // 'SlotsAlloc::resetSlots' and 'SlotsAlloc::verifySlots' functions.  The
    // slots are set when the corresponding function objector free function is
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
    (void)ALLOC_SLOTS;                                                        \
    (void)ALLOC_SLOTS_DEFAULT;                                                \
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

#endif

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                   MACROS FOR MULTIPLE ARGUMENT EXPANSION

// Numbered items separated by given separator

#define S1(FN,SN)                FN(1)
#define S2(FN,SN)  S1(FN,SN)  SN FN(2)
#define S3(FN,SN)  S2(FN,SN)  SN FN(3)
#define S4(FN,SN)  S3(FN,SN)  SN FN(4)
#define S5(FN,SN)  S4(FN,SN)  SN FN(5)
#define S6(FN,SN)  S5(FN,SN)  SN FN(6)
#define S7(FN,SN)  S6(FN,SN)  SN FN(7)
#define S8(FN,SN)  S7(FN,SN)  SN FN(8)
#define S9(FN,SN)  S8(FN,SN)  SN FN(9)
#define S10(FN,SN) S9(FN,SN)  SN FN(10)
#define S11(FN,SN) S10(FN,SN) SN FN(11)
#define S12(FN,SN) S11(FN,SN) SN FN(12)
#define S13(FN,SN) S12(FN,SN) SN FN(13)
#define S14(FN,SN) S13(FN,SN) SN FN(14)

// Comma-separated numbered items

#define C1(FN)           FN(1)
#define C2(FN)  C1(FN),  FN(2)
#define C3(FN)  C2(FN),  FN(3)
#define C4(FN)  C3(FN),  FN(4)
#define C5(FN)  C4(FN),  FN(5)
#define C6(FN)  C5(FN),  FN(6)
#define C7(FN)  C6(FN),  FN(7)
#define C8(FN)  C7(FN),  FN(8)
#define C9(FN)  C8(FN),  FN(9)
#define C10(FN) C9(FN),  FN(10)
#define C11(FN) C10(FN), FN(11)
#define C12(FN) C11(FN), FN(12)
#define C13(FN) C12(FN), FN(13)
#define C14(FN) C13(FN), FN(14)

// Space-separated numbered items

#define L1(FN)          FN(1)
#define L2(FN)  L1(FN)  FN(2)
#define L3(FN)  L2(FN)  FN(3)
#define L4(FN)  L3(FN)  FN(4)
#define L5(FN)  L4(FN)  FN(5)
#define L6(FN)  L5(FN)  FN(6)
#define L7(FN)  L6(FN)  FN(7)
#define L8(FN)  L7(FN)  FN(8)
#define L9(FN)  L8(FN)  FN(9)
#define L10(FN) L9(FN)  FN(10)
#define L11(FN) L10(FN) FN(11)
#define L12(FN) L11(FN) FN(12)
#define L13(FN) L12(FN) FN(13)
#define L14(FN) L13(FN) FN(14)

#define PN(n) Arg##n const& a##n
#define VN(n) Arg##n a##n
#define AN(n) a##n

                          // ==============
                          // class TestUtil
                          // ==============

struct TestUtil {
    // Utility class for static functions useful in 'bdlf_bind' testing.

    template <class T>
    static bool isBitwiseMoveableType(const T&);
        // Return true if the specified parameter type 'T' has the
        // 'bslmf::IsBitwiseMovable' trait and false otherwise.
};

                             // ===============
                             // class SlotsBase
                             // ===============

template <class VALUE>
struct SlotsBase {
    // This 'struct' defines an array of 'VALUE' to keep track (in conjunction
    // with the 'NoAllocTestType' or 'AllocTestType' classes) of which value or
    // allocator is being passed to which argument of the test function by a
    // 'bdlf_bind' object.

    // ENUMERATIONS
    enum {
        k_NUM_SLOTS = 15
    };

  private:
    // PRIVATE CLASS DATA
    static VALUE s_slots[k_NUM_SLOTS];

  public:
    // CLASS METHODS
    static VALUE getSlot(int index);
        // Get the value of the slot at the specified 'index'.

    static void resetSlots(VALUE value = 0);
        // Reset all the slots in this instance to the optionally specified
        // 'value'.

    static void setSlot(VALUE value, int index);
        // Set the slot at the specified 'index' to the specified 'value'.

    static bool verifySlots(const VALUE *EXPECTED, bool verboseFlag = true)
#ifndef BDLF_BIND_00T_AS_INCLUDE
                                                                    BSLA_UNUSED
#endif
    ;
        // Verify that all the slots in this instance compare equal to those in
        // the specified 'EXPECTED' value.  If the optionally specified
        // 'verboseFlag' is not set, output diagnostic only in case the slots
        // do not compare equal.
};

                           // =================
                           // type SlotsNoAlloc
                           // =================

typedef SlotsBase<int> SlotsNoAlloc;
    // When used within the methods of 'NoAllocTestType', the 'VALUE' type
    // will be 'int' and will keep track of which arguments have been assigned
    // a value (in case 'bdlf_bind' accesses fields that it should not).

                         // ====================
                         // class NoAllocTestArg
                         // ====================

template <int ID>
class NoAllocTestArg {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters due to the fact that 'NoAllocTestArg<ID1>' is a
    // different type than 'NoAllocTestArg<ID2>' is ID1 != ID2.  This class
    // does not take an optional allocator.

    // INSTANCE DATA
    int d_value; // value held by this object

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NoAllocTestArg,
                                   bslmf::IsBitwiseMoveable);

    // CREATORS
    NoAllocTestArg(int value);                                 // IMPLICIT
        // Create an object having the specified 'value'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    NoAllocTestArg(const NoAllocTestArg&) = default;
#endif

    // MANIPULATORS
    NoAllocTestArg& operator=(const NoAllocTestArg &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    int value() const;
};

// FREE OPERATORS
template <int ID>
inline
bool operator==(NoAllocTestArg<ID> const& lhs,
                NoAllocTestArg<ID> const& rhs);

template <int ID>
inline
bool operator!=(NoAllocTestArg<ID> const& lhs,
                NoAllocTestArg<ID> const& rhs);

                         // =====================
                         // class NoAllocTestType
                         // =====================

class NoAllocTestType {
    // This 'struct' provides a test class capable of holding up to 14 bound
    // parameters of types 'TestArgNoAlloc[1--14]', with full (non-streamable)
    // value semantics defined by the 'operator=='.  By default, a
    // 'NoAllocTestType' is constructed with nil ('N1') values, but instances
    // can be constructed with actual values (e.g., for creating expected
    // values).  A 'NoAllocTestType' can be invoked with up to 14 parameters,
    // via member functions 'testFunc[1--14]'.  These functions are also called
    // by the overloaded member 'operator()' of the same signatures, and
    // similar global functions 'testFunc[1--14]'.  All invocations support the
    // above 'SlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
#undef  FN
#define FN(n) typedef NoAllocTestArg<n> Arg##n;
    L14(FN)
        // Argument types for shortcut.

    // PRIVATE DATA
#undef  FN
#define FN(n) mutable Arg##n d_a##n;
    L14(FN)

    // FRIEND
    friend bool operator==(const NoAllocTestType& lhs,
                           const NoAllocTestType& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NoAllocTestType,
                                   bslmf::IsBitwiseMoveable);

    // TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    enum {
        k_N1 = -1   // default value for all private data
    };

    // CREATORS
#undef  FN
#define FN(n) Arg##n a##n = k_N1
    explicit NoAllocTestType(C14(FN));
        // Create a test object having the same value as the specified
        // 'original'.

    NoAllocTestType(const NoAllocTestType& original);
        // Create a test object having the same value as the specified
        // 'original'.

    // MANIPULATORS
    NoAllocTestType& operator=(const NoAllocTestType &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const;
        // Test operators invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int operator()(C##n(PN)) const;
    L14(FN)

    int testFunc0() const;
        // Test methods invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int testFunc##n(C##n(PN)) const;
    L14(FN)

    // ACCESSORS
    void print() const;
        // Output the value of this instance to the standard output.
};

// FREE OPERATORS
inline
bool operator==(const NoAllocTestType& lhs,
                const NoAllocTestType& rhs);

inline
bool operator!=(const NoAllocTestType& lhs,
                const NoAllocTestType& rhs)
#ifndef BDLF_BIND_00T_AS_INCLUDE
                BSLA_UNUSED
#endif
                ;

                        // =======================
                        // class NoAllocNETestType
                        // =======================

class NoAllocNETestType {
    // This 'struct' provides a test class capable of holding up to 14 bound
    // parameters of types 'TestArgNoAllocNE[1--14]', with full
    // (non-streamable) value semantics defined by the 'operator=='.  By
    // default, a 'NoAllocNETestType' is constructed with nil ('N1') values,
    // but instances can be constructed with actual values (e.g., for creating
    // expected values).  A 'NoAllocNETestType' can be invoked with up to 14
    // parameters, via member functions 'testFunc[1--14]'.  These functions are
    // also called by the overloaded member 'operator()' of the same
    // signatures, and similar global functions 'testFunc[1--14]'.  All
    // invocations support the above 'SlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
#undef  FN
#define FN(n) typedef NoAllocTestArg<n> Arg##n;
    L14(FN)
        // Argument types for shortcut.

    // PRIVATE DATA
#undef  FN
#define FN(n) mutable Arg##n d_a##n;
    L14(FN)

    // FRIEND
    friend bool operator==(const NoAllocNETestType& lhs,
                           const NoAllocNETestType& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NoAllocNETestType,
                                   bslmf::IsBitwiseMoveable);

    // TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    enum {
        k_N1 = -1   // default value for all private data
    };

    // CREATORS
#undef  FN
#define FN(n) Arg##n a##n = k_N1
    explicit NoAllocNETestType(C14(FN));
        // Create a test object having the same value as the specified
        // 'original'.

    NoAllocNETestType(const NoAllocNETestType& original);
        // Create a test object having the same value as the specified
        // 'original'.

    // MANIPULATORS
    NoAllocNETestType& operator=(const NoAllocNETestType &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const BSLS_KEYWORD_NOEXCEPT;
        // Test operators invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int operator()(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT;
    L14(FN)

    int testFunc0() const BSLS_KEYWORD_NOEXCEPT;
        // Test methods invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int testFunc##n(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT;
    L14(FN)

    // ACCESSORS
    void print() const;
        // Output the value of this instance to the standard output.
};

// FREE OPERATORS
inline
bool operator==(const NoAllocNETestType& lhs,
                const NoAllocNETestType& rhs);

inline
bool operator!=(const NoAllocNETestType& lhs,
                const NoAllocNETestType& rhs)
#ifndef BDLF_BIND_00T_AS_INCLUDE
                BSLA_UNUSED
#endif
                ;

                      // ==========================
                      // class TestFunctionsNoAlloc
                      // ==========================

struct TestFunctionsNoAlloc {
    // Global versions of 'NoAllocTestType' member functions.

    // TYPES
#undef  FN
#define FN(n) typedef NoAllocTestArg<n> Arg##n;
    L14(FN)
        // Types for shortcut.

    // CLASS METHODS
    static int func0(NoAllocTestType *object);
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
#undef  FN
#define FN(n) static int func##n(NoAllocTestType *object, C##n(PN));
    L14(FN)

    static int funcNE0(NoAllocNETestType *object) BSLS_KEYWORD_NOEXCEPT;
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
#undef  FN
#define FN(n) static int funcNE##n(NoAllocNETestType *object, C##n(PN))    \
                                                         BSLS_KEYWORD_NOEXCEPT;
    L14(FN)
};

                         // ====================
                         // class SlotsAllocBase
                         // ====================

template <class AllocPtr>
struct SlotsAllocBase
{
    // The point of this base class is that we would like it to be a class
    // template, so that the class data does not pollute the library object
    // files.  Since there is no reason to make 'SlotsAlloc' a template, we
    // separate the static pointers into this base class.  These pointers are
    // needed for the pretty printing in the 'verifySlots' method.

    // PRIVATE CLASS DATA
    static AllocPtr s_Z0;
    static AllocPtr s_Z1;
    static AllocPtr s_Z2;

  public:
    // CLASS METHODS
    static void setZ0(AllocPtr Z0);
    static void setZ1(AllocPtr Z1);
    static void setZ2(AllocPtr Z2);
    static AllocPtr getZ0();
    static AllocPtr getZ1();
    static AllocPtr getZ2();
};

                           // ================
                           // class SlotsAlloc
                           // ================

class SlotsAlloc
: public SlotsBase<const bslma::Allocator*>
, public SlotsAllocBase<const bslma::Allocator*>
{
    // When used within the methods of 'AllocTestType', the 'VALUE' type will
    // be 'const bslma::Allocator*' and will keep track of which allocator is
    // used with which argument of the 'Bind' object.  The sole purpose of
    // deriving a new class is to use pretty printing of the allocators in
    // 'verifySlots'.

  public:
    static bool verifySlots(const bslma::Allocator *const *EXPECTED,
                            bool                           verboseFlag = true)
#ifndef BDLF_BIND_00T_AS_INCLUDE
                                                                    BSLA_UNUSED
#endif
    ;
        // Verify that all the slots in this instance compare equal to those in
        // the specified 'EXPECTED' value.  If the optionally specified
        // 'verboseFlag' is not set, output diagnostic only in case the slots
        // do not compare equal.
};

                            // ==================
                            // class AllocTestArg
                            // ==================

template <int ID>
class AllocTestArg {
    // This class is used to disambiguate types in passing parameters due to
    // the fact that 'AllocTestArg<ID1>' is a different type than
    // 'AllocTestArg<ID2>' is ID1 != ID2.  This class is used for testing
    // memory allocator issues.

    // PRIVATE DATA
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)
    int              *d_value;        // value allocated from 'd_allocator_p'
        // NOTE: *Must* be declared and initialized in that order.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AllocTestArg,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    AllocTestArg(int value, bslma::Allocator *allocator = 0);  // IMPLICIT
        // Create an object having the specified 'value'.  Use the specified
        // 'allocator' to supply memory.  If 'allocator' is 0, use the
        // currently installed default allocator.

    AllocTestArg(const AllocTestArg&  original,
                 bslma::Allocator    *allocator = 0);
        // Create a copy of the specified non-modifiable 'original'.  Use the
        // specified 'allocator' to supply memory.  If 'allocator' is 0, use
        // the currently installed default allocator.

    ~AllocTestArg();
        // Destroy this object.

    // MANIPULATORS
    AllocTestArg& operator=(const AllocTestArg &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    const bslma::Allocator *allocator() const;
        // Return the allocator specified at construction of this object.

    int value() const;
        // Return the value of this object.
};

// FREE OPERATORS
template <int ID>
inline
bool operator==(const AllocTestArg<ID>& lhs,
                const AllocTestArg<ID>& rhs);

template <int ID>
inline
bool operator!=(const AllocTestArg<ID>& lhs,
                const AllocTestArg<ID>& rhs);

                            // ===================
                            // class AllocTestType
                            // ===================

class AllocTestType {
    // This class provides a test class capable of holding up to 14 bound
    // parameters of types 'AllocTestArg[1--14]', with full (non-streamable)
    // value semantics defined by the 'operator=='.  By default, a
    // 'AllocTestType' is constructed with nil ('k_N1') values, but objects can
    // be constructed with actual values (e.g., for creating expected values).
    // A 'AllocTestType' can be invoked with up to 14 parameters, via member
    // functions 'testFunc[1--14]'.  These functions are also called by the
    // overloaded member 'operator()' of the same signatures, and similar
    // global functions 'testFunc1--14'.  All invocations support the above
    // 'SlotsAlloc' mechanism.
    //
    // This class intentionally *does* take an allocator.

    // PRIVATE TYPES
#undef  FN
#define FN(n) typedef AllocTestArg<n> Arg##n;
    L14(FN)
        // Argument types for shortcut.

    enum {
        k_N1 = -1   // default value for all private data
    };

    // PRIVATE DATA
#undef  FN
#define FN(n) mutable Arg##n d_a##n;
    L14(FN)

    // FRIEND
    friend bool operator==(const AllocTestType& lhs,
                           const AllocTestType& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AllocTestType,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    // CREATORS
#undef  FN
#define FN(n) Arg##n a##n = k_N1
    explicit AllocTestType(bslma::Allocator *allocator = 0, C14(FN));
        // This constructor does *not* participate in the
        // 'UsesBdemaAllocatorTraits' contract, it is here simply to allow to
        // construct expected values with a specified 'allocator' as the first
        // argument (otherwise there would need to be fourteen different
        // constructors with 'allocator' as the last argument).

#undef  FN
#define FN(n) Arg##n a##n
    AllocTestType(C14(FN), bslma::Allocator *allocator = 0);

    AllocTestType(const AllocTestType&  original,
                  bslma::Allocator     *allocator = 0);

    // MANIPULATORS
    AllocTestType& operator=(const AllocTestType &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const;
        // Test operators invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int operator()(C##n(PN)) const;
    L14(FN)

    int testFunc0() const;
        // Test methods invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int testFunc##n(C##n(PN)) const;
    L14(FN)

    void setSlots();
        // Set slots with allocator values of internal data members (as opposed
        // to with allocator value of invocation arguments as in the operators
        // and 'testFunc*' functions above).

    // ACCESSORS
    void print() const;
        // Output the value of this object to the standard output.
};

// FREE OPERATORS
inline
bool operator==(AllocTestType const& lhs, AllocTestType const& rhs);

inline
bool operator!=(AllocTestType const& lhs, AllocTestType const& rhs)
#ifndef BDLF_BIND_00T_AS_INCLUDE
                BSLA_UNUSED
#endif
                ;

                         // =====================
                         // class AllocNETestType
                         // =====================

class AllocNETestType {
    // This class provides a test class capable of holding up to 14 bound
    // parameters of types 'AllocTestArg[1--14]', with full (non-streamable)
    // value semantics defined by the 'operator=='.  By default, a
    // 'AllocNETestType' is constructed with nil ('k_N1') values, but objects
    // can be constructed with actual values (e.g., for creating expected
    // values).  A 'AllocNETestType' can be invoked with up to 14 parameters,
    // via member functions 'testFunc[1--14]'.  These functions are also called
    // by the overloaded member 'operator()' of the same signatures, and
    // similar global functions 'testFunc1--14'.  All invocations support the
    // above 'SlotsAlloc' mechanism.
    //
    // This class intentionally *does* take an allocator.

    // PRIVATE TYPES
#undef  FN
#define FN(n) typedef AllocTestArg<n> Arg##n;
    L14(FN)
        // Argument types for shortcut.

    enum {
        k_N1 = -1   // default value for all private data
    };

    // PRIVATE DATA
#undef  FN
#define FN(n) mutable Arg##n d_a##n;
    L14(FN)

    // FRIEND
    friend bool operator==(const AllocNETestType& lhs,
                           const AllocNETestType& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AllocNETestType,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    // CREATORS
#undef  FN
#define FN(n) Arg##n a##n = k_N1
    explicit AllocNETestType(bslma::Allocator *allocator = 0, C14(FN));
        // This constructor does *not* participate in the
        // 'UsesBdemaAllocatorTraits' contract, it is here simply to allow to
        // construct expected values with a specified 'allocator' as the first
        // argument (otherwise there would need to be fourteen different
        // constructors with 'allocator' as the last argument).

#undef  FN
#define FN(n) Arg##n a##n
    AllocNETestType(C14(FN), bslma::Allocator *allocator = 0);

    AllocNETestType(const AllocNETestType&  original,
                    bslma::Allocator       *allocator = 0);

    // MANIPULATORS
    AllocNETestType& operator=(const AllocNETestType &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const BSLS_KEYWORD_NOEXCEPT;
        // Test operators invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int operator()(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT;
    L14(FN)

    int testFunc0() const BSLS_KEYWORD_NOEXCEPT;
        // Test methods invoking this test type with 0 up to 14 arguments.
#undef  FN
#define FN(n) int testFunc##n(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT;
    L14(FN)

    void setSlots();
        // Set slots with allocator values of internal data members (as opposed
        // to with allocator value of invocation arguments as in the operators
        // and 'testFunc*' functions above).

    // ACCESSORS
    void print() const;
        // Output the value of this object to the standard output.
};

// FREE OPERATORS
inline
bool operator==(AllocNETestType const& lhs,
                AllocNETestType const& rhs);

inline
bool operator!=(AllocNETestType const& lhs,
                AllocNETestType const& rhs)
#ifndef BDLF_BIND_00T_AS_INCLUDE
                BSLA_UNUSED
#endif
                ;

                       // ========================
                       // class TestFunctionsAlloc
                       // ========================

struct TestFunctionsAlloc {
    // Global versions of 'AllocTestType' member functions.

    // TYPES
#undef  FN
#define FN(n) typedef AllocTestArg<n> Arg##n;
    L14(FN)

    // CLASS METHODS
    static int func0(AllocTestType *o);
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
#undef  FN
#define FN(n) static int func##n(AllocTestType *o, C##n(PN));
    L14(FN)

    static int funcNE0(AllocNETestType *o) BSLS_KEYWORD_NOEXCEPT;
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
#undef  FN
#define FN(n) static int funcNE##n(AllocNETestType *o, C##n(PN))              \
                                                         BSLS_KEYWORD_NOEXCEPT;
    L14(FN)
};

                            // --------------
                            // class TestUtil
                            // --------------

template <class T>
inline bool TestUtil::isBitwiseMoveableType(const T&) {
    return bslmf::IsBitwiseMoveable<T>::VALUE;
}

                            // ---------------
                            // class SlotsBase
                            // ---------------

// PRIVATE CLASS DATA
template <class VALUE>
VALUE SlotsBase<VALUE>::s_slots[k_NUM_SLOTS];

// CLASS METHODS
template <class VALUE>
inline
VALUE SlotsBase<VALUE>::getSlot(int index)
{
    return s_slots[index];
}

template <class VALUE>
void SlotsBase<VALUE>::resetSlots(VALUE value)
{
    for (int i = 0; i < k_NUM_SLOTS; ++i) {
        s_slots[i] = value;
    }
}

template <class VALUE>
inline
void SlotsBase<VALUE>::setSlot(VALUE value, int index)
{
    s_slots[index] = value;
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( push )
#pragma warning( disable : 4100 ) // Verify we still need separate test paths
                                  // for MSVC
#endif

template <class VALUE>
bool SlotsBase<VALUE>::verifySlots(const VALUE *EXPECTED,
                                   bool         verboseFlag)
{
    bool equalFlag = true;
#if !defined(BSLS_PLATFORM_CMP_MSVC)
    // Note: the calls to 'verifyAllocSlots' are disabled on Windows.  Their
    // success depends on the "Return Value Optimization" (RVO) which Windows
    // does not seem to be applying.

    for (int i = 0; i < k_NUM_SLOTS; ++i) {
        if (EXPECTED[i] != getSlot(i)) {
            equalFlag = false;
            break;
        }
    }
#endif // !defined(BSLS_PLATFORM_CMP_MSVC)

    if (verboseFlag || !equalFlag) {
        bsl::printf("\tSlots:");
        for (int i = 0; i < k_NUM_SLOTS; ++i) {
            bsl::printf(" %d", getSlot(i));
        }
        bsl::printf("\n");
    }

    return equalFlag;
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( pop )
#endif
                          // ----------------
                          // class SlotsAlloc
                          // ----------------

// CLASS METHODS
bool SlotsAlloc::verifySlots(const bslma::Allocator* const *EXPECTED,
                             bool                           verboseFlag)
{
    bool equalFlag = true;
#if !defined(BSLS_PLATFORM_CMP_MSVC)
    // Note: the calls to 'verifyAllocSlots' are disabled on Windows.  Their
    // success depends on the "Return Value Optimization" (RVO) which Windows
    // does not seem to be applying.

    for (int i = 0; i < k_NUM_SLOTS; ++i) {
        if (EXPECTED[i] != getSlot(i)) {
            equalFlag = false;
            break;
        }
    }
#endif // !defined(BSLS_PLATFORM_CMP_MSVC)

    if (verboseFlag || !equalFlag) {
        bsl::printf("\tAllocSlots:");
        for (int i = 0; i < k_NUM_SLOTS; ++i) {
            bsl::printf(" %s", (getSlot(i) == getZ0()) ? "Z0"
                            : ((getSlot(i) == getZ1()) ? "Z1"
                            : ((getSlot(i) == getZ2()) ? "Z2"
                            : "Z?")));
        }
        bsl::printf("\n");
    }

    return equalFlag;
}

                           // --------------------
                           // class NoAllocTestArg
                           // --------------------

// CREATORS
template <int ID>
inline
NoAllocTestArg<ID>::NoAllocTestArg(int value)
: d_value(value)
{
}

// MANIPULATORS
template <int ID>
inline
NoAllocTestArg<ID>&
NoAllocTestArg<ID>::operator=(const NoAllocTestArg &rhs)
{
    d_value = rhs.d_value;
    return *this;
}

// ACCESSORS
template <int ID>
inline
int NoAllocTestArg<ID>::value() const
{
    return d_value;
}

// FREE OPERATORS
template <int ID>
bool operator==(NoAllocTestArg<ID> const& lhs,
                NoAllocTestArg<ID> const& rhs)
{
    return lhs.value() == rhs.value();
}

template <int ID>
bool operator!=(NoAllocTestArg<ID> const& lhs,
                NoAllocTestArg<ID> const& rhs)
{
    return !(lhs == rhs);
}

                           // ---------------------
                           // class NoAllocTestType
                           // ---------------------

// CREATORS
#undef  FN
#define FN(n) d_a##n(a##n)
inline
NoAllocTestType::NoAllocTestType(C14(VN))
: C14(FN)
{
}

#undef  FN
#define FN(n) d_a##n(original.d_a##n)
inline
NoAllocTestType::NoAllocTestType(const NoAllocTestType& original)
: C14(FN)
{
}

// MANIPULATORS
inline
NoAllocTestType&
NoAllocTestType::operator=(const NoAllocTestType& rhs)
{
#undef  FN
#define FN(n) d_a##n = rhs.d_a##n;
    L14(FN)
    return *this;
}

// ACCESSORS
inline
int NoAllocTestType::operator()() const
{
    return testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int NoAllocTestType::operator()(C##n(PN)) const                               \
{                                                                             \
    return testFunc##n(C##n(AN));                                             \
}
L14(FN)

inline
int NoAllocTestType::testFunc0() const
{
    return 0;
}

#undef  GN
#define GN(n) d_a##n = a##n; SlotsNoAlloc::setSlot(a##n.value(), n);
#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int NoAllocTestType::testFunc##n(C##n(PN)) const                              \
{                                                                             \
    S##n(GN,)                                                                 \
    return n;                                                                 \
}
L14(FN)

#undef  FN
#define FN(n) d_a##n.value()
inline
void NoAllocTestType::print() const
{
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                C14(FN));
}

// FREE OPERATORS
inline
bool operator==(const NoAllocTestType& lhs,
                const NoAllocTestType& rhs)
{
#undef  FN
#define FN(n) lhs.d_a##n.value() == rhs.d_a##n.value()
    return S14(FN,&&);
}

inline
bool operator!=(const NoAllocTestType& lhs,
                const NoAllocTestType& rhs)
{
    return !(lhs == rhs);
}

                          // -----------------------
                          // class NoAllocNETestType
                          // -----------------------

// CREATORS
#undef  FN
#define FN(n) d_a##n(a##n)
inline
NoAllocNETestType::NoAllocNETestType(C14(VN))
: C14(FN)
{
}

#undef  FN
#define FN(n) d_a##n(original.d_a##n)
inline
NoAllocNETestType::NoAllocNETestType(const NoAllocNETestType& original)
: C14(FN)
{
}

// MANIPULATORS
inline
NoAllocNETestType&
NoAllocNETestType::operator=(const NoAllocNETestType& rhs)
{
#undef  FN
#define FN(n) d_a##n = rhs.d_a##n;
    L14(FN)
    return *this;
}

// ACCESSORS
inline
int NoAllocNETestType::operator()() const BSLS_KEYWORD_NOEXCEPT
{
    return testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int NoAllocNETestType::operator()(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT       \
{                                                                             \
    return testFunc##n(C##n(AN));                                             \
}
L14(FN)

inline
int NoAllocNETestType::testFunc0() const BSLS_KEYWORD_NOEXCEPT
{
    return 0;
}

#undef  GN
#define GN(n) d_a##n = a##n; SlotsNoAlloc::setSlot(a##n.value(), n);
#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int NoAllocNETestType::testFunc##n(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT      \
{                                                                             \
    S##n(GN,)                                                                 \
    return n;                                                                 \
}
L14(FN)

#undef  FN
#define FN(n) d_a##n.value()
inline
void NoAllocNETestType::print() const
{
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                C14(FN));
}

// FREE OPERATORS
inline
bool operator==(const NoAllocNETestType& lhs,
                const NoAllocNETestType& rhs)
{
#undef  FN
#define FN(n) lhs.d_a##n.value() == rhs.d_a##n.value()
    return S14(FN,&&);
}

inline
bool operator!=(const NoAllocNETestType& lhs,
                const NoAllocNETestType& rhs)
{
    return !(lhs == rhs);
}

                       // --------------------------
                       // class TestFunctionsNoAlloc
                       // --------------------------

// CLASS METHODS
inline
int TestFunctionsNoAlloc::func0(NoAllocTestType *object)
{
    return object->testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int TestFunctionsNoAlloc::func##n(NoAllocTestType *object, C##n(PN))          \
{                                                                             \
    return object->testFunc##n(C##n(AN));                                     \
}
L14(FN)

inline
int TestFunctionsNoAlloc::funcNE0(NoAllocNETestType *object)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return object->testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int TestFunctionsNoAlloc::funcNE##n(NoAllocNETestType *object,                \
                                         C##n(PN)) BSLS_KEYWORD_NOEXCEPT      \
{                                                                             \
    return object->testFunc##n(C##n(AN));                                     \
}
L14(FN)

                          // --------------------
                          // class SlotsAllocBase
                          // --------------------

// CLASS DATA
template <class AllocPtr>
AllocPtr SlotsAllocBase<AllocPtr>::s_Z0 = 0;

template <class AllocPtr>
AllocPtr SlotsAllocBase<AllocPtr>::s_Z1 = 0;

template <class AllocPtr>
AllocPtr SlotsAllocBase<AllocPtr>::s_Z2 = 0;

// CLASS METHODS
template <class AllocPtr>
inline
void SlotsAllocBase<AllocPtr>::setZ0(AllocPtr Z0)
{
    s_Z0 = Z0;
}

template <class AllocPtr>
inline
void SlotsAllocBase<AllocPtr>::setZ1(AllocPtr Z1)
{
    s_Z1 = Z1;
}

template <class AllocPtr>
inline
void SlotsAllocBase<AllocPtr>::setZ2(AllocPtr Z2)
{
    s_Z2 = Z2;
}

template <class AllocPtr>
inline
AllocPtr SlotsAllocBase<AllocPtr>::getZ0()
{
    return s_Z0;
}

template <class AllocPtr>
inline
AllocPtr SlotsAllocBase<AllocPtr>::getZ1()
{
    return s_Z1;
}

template <class AllocPtr>
inline
AllocPtr SlotsAllocBase<AllocPtr>::getZ2()
{
    return s_Z2;
}

                           // ------------------
                           // class AllocTestArg
                           // ------------------

// CREATORS

template <int ID>
AllocTestArg<ID>::AllocTestArg(int               value,
                               bslma::Allocator *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_value(new (*d_allocator_p) int(value))
{
}

template <int ID>
AllocTestArg<ID>::AllocTestArg(const AllocTestArg&  original,
                               bslma::Allocator    *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_value(new (*d_allocator_p) int(original.value()))
{
}

template <int ID>
AllocTestArg<ID>::~AllocTestArg()
{
    d_allocator_p->deleteObjectRaw(d_value);
}

// MANIPULATORS
template <int ID>
AllocTestArg<ID>&
AllocTestArg<ID>::operator=(const AllocTestArg& rhs)
{
    if (this != &rhs) {
        d_allocator_p->deleteObjectRaw(d_value);
        d_value = new (*d_allocator_p) int(rhs.value());
    }

    return *this;
}

// ACCESSORS
template <int ID>
const bslma::Allocator *AllocTestArg<ID>::allocator() const
{
    return d_allocator_p;
}

template <int ID>
int AllocTestArg<ID>::value() const
{
    return *d_value;
}

// FREE OPERATORS
template <int ID>
inline
bool operator==(const AllocTestArg<ID>& lhs,
                const AllocTestArg<ID>& rhs)
{
    return lhs.value() == rhs.value();
}

template <int ID>
inline
bool operator!=(const AllocTestArg<ID>& lhs,
                const AllocTestArg<ID>& rhs)
{
    return !(lhs == rhs);
}

                        // -------------------
                        // class AllocTestType
                        // -------------------

// CREATORS
#undef  FN
#define FN(n) d_a##n(a##n, allocator)
inline
AllocTestType::AllocTestType(bslma::Allocator *allocator, C14(VN))
: C14(FN)
{
}

#undef  FN
#define FN(n) d_a##n(a##n, allocator)
inline
AllocTestType::AllocTestType(C14(VN), bslma::Allocator *allocator)
: C14(FN)
{
}

#undef  FN
#define FN(n) d_a##n(original.d_a##n, allocator)
inline
AllocTestType::AllocTestType(const AllocTestType&  original,
                             bslma::Allocator     *allocator)
: C14(FN)
{
}

// MANIPULATORS

#undef  FN
#define FN(n) d_a##n = rhs.d_a##n;
inline
AllocTestType&
AllocTestType::operator=(const AllocTestType& rhs)
{
    L14(FN)
    return *this;
}

// ACCESSORS
inline
int AllocTestType::operator()() const
{
    return testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int AllocTestType::operator()(C##n(PN)) const                                 \
{                                                                             \
    return testFunc##n(C##n(AN));                                             \
}
L14(FN)

inline
int AllocTestType::testFunc0() const
{
    return 0;
}

#undef  GN
#define GN(n) d_a##n = a##n; SlotsAlloc::setSlot(a##n.allocator(), n);
#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int AllocTestType::testFunc##n(C##n(PN)) const                                \
{                                                                             \
    S##n(GN,)                                                                 \
    return n;                                                                 \
}
L14(FN)

inline
void AllocTestType::setSlots()
{
#undef  FN
#define FN(n) SlotsAlloc::setSlot(d_a##n.allocator(), n);
    L14(FN)
}

inline
void AllocTestType::print() const
{
#undef  FN
#define FN(n) d_a##n.value()
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                C14(FN));
}

// FREE OPERATORS
inline
bool operator==(const AllocTestType& lhs,
                const AllocTestType& rhs)
{
#undef  FN
#define FN(n) lhs.d_a##n.value()  == rhs.d_a##n.value()
    return S14(FN,&&);
}

inline
bool operator!=(const AllocTestType& lhs,
                const AllocTestType& rhs)
{
    return !(lhs == rhs);
}

                          // ---------------------
                          // class AllocNETestType
                          // ---------------------

// CREATORS
#undef  FN
#define FN(n) d_a##n(a##n, allocator)
inline
AllocNETestType::AllocNETestType(bslma::Allocator *allocator, C14(VN))
: C14(FN)
{
}

#undef  FN
#define FN(n) d_a##n(a##n, allocator)
inline
AllocNETestType::AllocNETestType(C14(VN), bslma::Allocator *allocator)
: C14(FN)
{
}

#undef  FN
#define FN(n) d_a##n(original.d_a##n, allocator)
inline
AllocNETestType::AllocNETestType(const AllocNETestType&  original,
                                 bslma::Allocator       *allocator)
: C14(FN)
{
}

// MANIPULATORS

#undef  FN
#define FN(n) d_a##n = rhs.d_a##n;
inline
AllocNETestType&
AllocNETestType::operator=(const AllocNETestType& rhs)
{
    L14(FN)
    return *this;
}

// ACCESSORS
inline
int AllocNETestType::operator()() const BSLS_KEYWORD_NOEXCEPT
{
    return testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int AllocNETestType::operator()(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT         \
{                                                                             \
    return testFunc##n(C##n(AN));                                             \
}
L14(FN)

inline
int AllocNETestType::testFunc0() const BSLS_KEYWORD_NOEXCEPT
{
    return 0;
}

#undef  GN
#define GN(n) d_a##n = a##n; SlotsAlloc::setSlot(a##n.allocator(), n);
#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int AllocNETestType::testFunc##n(C##n(PN)) const BSLS_KEYWORD_NOEXCEPT        \
{                                                                             \
    S##n(GN,)                                                                 \
    return n;                                                                 \
}
L14(FN)

inline
void AllocNETestType::setSlots()
{
#undef  FN
#define FN(n) SlotsAlloc::setSlot(d_a##n.allocator(), n);
    L14(FN)
}

inline
void AllocNETestType::print() const
{
#undef  FN
#define FN(n) d_a##n.value()
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                C14(FN));
}

// FREE OPERATORS
inline
bool operator==(const AllocNETestType& lhs,
                const AllocNETestType& rhs)
{
#undef  FN
#define FN(n) lhs.d_a##n.value()  == rhs.d_a##n.value()
    return S14(FN,&&);
}

inline
bool operator!=(const AllocNETestType& lhs,
                const AllocNETestType& rhs)
{
    return !(lhs == rhs);
}

                          // ------------------------
                          // class TestFunctionsAlloc
                          // ------------------------

inline
int TestFunctionsAlloc::func0(AllocTestType *o)
{
    return o->testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int TestFunctionsAlloc::func##n(AllocTestType *o, C##n(PN))                   \
{                                                                             \
    return o->testFunc##n(C##n(AN));                                          \
}
L14(FN)

inline
int TestFunctionsAlloc::funcNE0(AllocNETestType *o) BSLS_KEYWORD_NOEXCEPT
{
    return o->testFunc0();
}

#undef  FN
#define FN(n)                                                                 \
inline                                                                        \
int TestFunctionsAlloc::funcNE##n(AllocNETestType *o, C##n(PN))               \
                                                        BSLS_KEYWORD_NOEXCEPT \
{                                                                             \
    return o->testFunc##n(C##n(AN));                                          \
}
L14(FN)

// ----------------------------------------------------------------------------
//                       Typedefs for argument types

typedef NoAllocTestArg<1>   NoAllocTestArg1;
typedef NoAllocTestArg<2>   NoAllocTestArg2;
typedef NoAllocTestArg<3>   NoAllocTestArg3;
typedef NoAllocTestArg<4>   NoAllocTestArg4;
typedef NoAllocTestArg<5>   NoAllocTestArg5;
typedef NoAllocTestArg<6>   NoAllocTestArg6;
typedef NoAllocTestArg<7>   NoAllocTestArg7;
typedef NoAllocTestArg<8>   NoAllocTestArg8;
typedef NoAllocTestArg<9>   NoAllocTestArg9;
typedef NoAllocTestArg<10>  NoAllocTestArg10;
typedef NoAllocTestArg<11>  NoAllocTestArg11;
typedef NoAllocTestArg<12>  NoAllocTestArg12;
typedef NoAllocTestArg<13>  NoAllocTestArg13;
typedef NoAllocTestArg<14>  NoAllocTestArg14;

typedef AllocTestArg<1>     AllocTestArg1;
typedef AllocTestArg<2>     AllocTestArg2;
typedef AllocTestArg<3>     AllocTestArg3;
typedef AllocTestArg<4>     AllocTestArg4;
typedef AllocTestArg<5>     AllocTestArg5;
typedef AllocTestArg<6>     AllocTestArg6;
typedef AllocTestArg<7>     AllocTestArg7;
typedef AllocTestArg<8>     AllocTestArg8;
typedef AllocTestArg<9>     AllocTestArg9;
typedef AllocTestArg<10>    AllocTestArg10;
typedef AllocTestArg<11>    AllocTestArg11;
typedef AllocTestArg<12>    AllocTestArg12;
typedef AllocTestArg<13>    AllocTestArg13;
typedef AllocTestArg<14>    AllocTestArg14;

// ----------------------------------------------------------------------------
//                              MACO CLEANUP

#undef  S1
#undef  S2
#undef  S3
#undef  S4
#undef  S5
#undef  S6
#undef  S7
#undef  S8
#undef  S9
#undef  S10
#undef  S11
#undef  S12
#undef  S13
#undef  S14

#undef  C1
#undef  C2
#undef  C3
#undef  C4
#undef  C5
#undef  C6
#undef  C7
#undef  C8
#undef  C9
#undef  C10
#undef  C11
#undef  C12
#undef  C13
#undef  C14

#undef  L1
#undef  L2
#undef  L3
#undef  L4
#undef  L5
#undef  L6
#undef  L7
#undef  L8
#undef  L9
#undef  L10
#undef  L11
#undef  L12
#undef  L13
#undef  L14

#undef  AN
#undef  FN
#undef  GN
#undef  PN
#undef  VN

// ============================================================================
//                            COMMON TEST VALUES
// ----------------------------------------------------------------------------

// Values that do not take an allocator.

const NoAllocTestArg1  I1  BSLA_UNUSED =  1;
const NoAllocTestArg2  I2  BSLA_UNUSED =  2;
const NoAllocTestArg3  I3  BSLA_UNUSED =  3;
const NoAllocTestArg4  I4  BSLA_UNUSED =  4;
const NoAllocTestArg5  I5  BSLA_UNUSED =  5;
const NoAllocTestArg6  I6  BSLA_UNUSED =  6;
const NoAllocTestArg7  I7  BSLA_UNUSED =  7;
const NoAllocTestArg8  I8  BSLA_UNUSED =  8;
const NoAllocTestArg9  I9  BSLA_UNUSED =  9;
const NoAllocTestArg10 I10 BSLA_UNUSED = 10;
const NoAllocTestArg11 I11 BSLA_UNUSED = 11;
const NoAllocTestArg12 I12 BSLA_UNUSED = 12;
const NoAllocTestArg13 I13 BSLA_UNUSED = 13;
const NoAllocTestArg14 I14 BSLA_UNUSED = 14;

// ============================================================================
//                                TEST CASES
// ----------------------------------------------------------------------------

#ifdef BDLF_BIND_00T_AS_GENERATOR

DEFINE_TEST_CASE(17) {
        // ------------------------------------------------------------------
        // TESTING 'bslmf::IsBitwiseMoveable<bdlf::Bind<R,F,L>>'
        //
        // Concerns:
        //: 1 The bitwise moveable trait is true for 'Bind<RET,FUNC,LIST>' if
        //:   the bitwise moveable trait is true for 'FUNC' and for all of the
        //:   types in 'LIST'.
        //: 2 Placeholders are treated as bitwise moveable.
        //: 3 The bitwise moveable trait is false for 'Bind<RET,FUNC,LIST>' if
        //:   the bitwise moveable trait is false for 'FUNC'.
        //: 4 The bitwise moveable trait is false for 'Bind<RET,FUNC,LIST>' if
        //:   the bitwise moveable trait is false for any of the types in
        //:   'LIST'.
        //: 5 The 'RET' type has no affect on whether 'Bind<RET,FUNC,LIST>' has
        //:   the bitwise moveable trait.
        //: 6 The concerns above apply to the types as returned from
        //:   'BindUtil::bind', 'BindUtil::bindA', and 'BindUtil::bindR'.
        //
        // Plan:
        //: 1 For concern 1, instantate 'Bind' with a bitwise moveable 'FUNC'
        //:   and a 'LIST' type comprising types that are all bitwise moveable.
        //:   Verify that the resulting specialization has the bitwise moveable
        //:   trait.
        //: 2 For concern 2, repeat step 1 except replace one of the types in
        //:   'LIST' with a placeholder.  Verify that the resulting
        //:   specialization has the bitwise moveable trait.
        //: 3 For concern 3, repeat step 2 except replace 'FUNC' with a type
        //:   that is not bitwise moveable.  Verify that the resulting
        //:   specialization *does not* have the bitwise moveable trait.
        //: 4 For concern 4, repeat step 2 except replace one of the types in
        //:   'LIST' with a type that is not bitwise moveable.  Verify that the
        //:   resulting specialization *does not* have the bitwise moveable
        //:   trait.
        //: 5 For concern 5, repeat step 2 except replace 'RET' with a type
        //:   that is not bitwise moveable.  Verify that the resulting
        //:   specialization *still has* the bitwise moveable trait.
        //: 6 For concern 6, create a function template,
        //:   'isBitwiseMoveableArg(const T&)', that returns true iff 'T' has
        //:   the bitwise moveable trait.  Invoke 'isBitwiseMoveableArg' on
        //:   calls to 'BindUtil::bind', 'BindUtil::bindA', and
        //:   'BindUtil::bindR' with arguments that would result in the various
        //:   combinations above.
        //
        // Testing:
        //      bslmf::IsBitwiseMoveable<bdlf::Bind<RET,FUNC,LIST>>

        if (verbose)
            printf("\nTESTING 'bslmf::IsBitwiseMoveable<bdlf::Bind<R,F,L>>'"
                   "\n====================================================\n");

        (void)veryVerbose;
        (void)veryVeryVerbose;

#if BBT_n > 0
        typedef NoAllocTestArg<1> BitwiseArg;
        typedef NoAllocTestArg<2> BitwiseRet;
        typedef NoAllocTestType   BitwiseInvocable;

        typedef AllocTestArg<1>   NonBitwiseArg;
        typedef AllocTestArg<2>   NonBitwiseRet;
        typedef AllocTestType     NonBitwiseInvocable;

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

        typedef TestUtil         TU;

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
//        ASSERT(!EXPECT == !(TU::isBitwiseMoveableType(
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
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDR WITH FUNCTION OBJECT PASSED BY VALUE
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                  BBT_K(mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS =
                                      Z0->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                         BBT_K(mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS =
                                      Z0->numAllocations() - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                  BBT_K(mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                         BBT_K(mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS,
                         28+3*PARAMS == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(15) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDR WITH FUNCTION OBJECT PASSED BY ADDRESS
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(14) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BIND WITH MEMBER FUNCTION POINTER
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_TESTFUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_TESTFUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_TESTFUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_TESTFUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
#endif // BBT_n == 14
      }

DEFINE_TEST_CASE(13) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDR WITH FREE FUNCTION REFERENCES
        //
        // Concerns and plans:
        //   Identical to case 12, except passing 'funcN' instead of
        //   '&funcN'.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
#endif // BBT_n == 14
      }

DEFINE_TEST_CASE(12) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDR WITH FREE FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bindR' static method returns a
        //   'bdlf::Bind' object that returns the correct value of the correct
        //   type, has the appropriate side effects, and performs the proper
        //   amount of memory allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bindR' with the address of the class method
        //   'funcN' of the utility class 'TestFunctionsNoAlloc' or
        //   'TestFunctionsAlloc', that takes a mutable function object 'mX',
        //   constructed with or without an allocator, and N parameters.  Upon
        //   invocation, verify that the return value and state of 'mX' (as
        //   modified by the 'testFuncN' method) are as expected, and that no
        //   allocation (for objects 'mX' that do not require allocation) or
        //   the proper amount of allocation (for objects 'mX' that do require
        //   allocation) took place.
        //
        // Testing:
        //   bdlf::BindUtil::bindR(Func const&, ...);
        // --------------------------------------------------------------------

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                 &BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS ==
                bdlf::BindUtil::bindR<ConvertibleFromToInt>(
                                                        &BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
#endif // BBT_n == 14
      }

DEFINE_TEST_CASE(11) {

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;

#if 0
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY VALUE
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                  Z2,
                                                  BBT_K(mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                         Z2,
                                                         BBT_K(mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                  Z2,
                                                  BBT_K(mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 14 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                         Z2,
                                                         BBT_K(mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 14 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::SlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // #if 0
      }

DEFINE_TEST_CASE(10) {

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;

#if 0
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDA WITH FUNCTION OBJECT PASSED BY ADDRESS
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::SlotsAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // #if 0
}

DEFINE_TEST_CASE(9) {

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;

#if 0
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDA WITH MEMBER FUNCTION POINTER
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_TESTFUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_TESTFUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_TESTFUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::SlotsAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_TESTFUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // BBT_n == 14
#endif // #if 0
      }

DEFINE_TEST_CASE(8) {

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;

#if 0
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDA WITH FREE FUNCTION REFERENCE
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::SlotsAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // BBT_n == 14
#endif // #if 0
      }

DEFINE_TEST_CASE(7) {

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;

#if 0
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BINDA WITH FREE FUNCTION POINTER
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
        //   method of the utility class 'TestFunctionsNoAlloc' or
        //   'TestFunctionsAlloc', that take an instance 'mX' of the
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            SlotsNoAlloc::resetSlots(N1);
            LOOP_ASSERT(PARAMS, SlotsNoAlloc::verifySlots(
                                         NO_ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  SlotsNoAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                 Z2,
                                                 &BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS,  bdlf::SlotsAlloc::verifySlots(
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            bdlf::SlotsAlloc::resetSlots(Z0);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                            ALLOC_SLOTS_DEFAULT, veryVerbose));

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bindA(
                                                        Z2,
                                                        &BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
            LOOP_ASSERT(PARAMS, bdlf::SlotsAlloc::verifySlots(
                                                    ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) {
                printf("\tLINE: %d PARAMS: %d X: ", L_, PARAMS); X.print();
            }
        }
#endif // BBT_n == 14
#endif // #if 0
      }

DEFINE_TEST_CASE(6) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY VALUE
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                  BBT_K(mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  EXPECTED;
            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                         BBT_K(mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE  = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                  BBT_K(mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE  = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                         BBT_K(mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BIND WITH FUNCTION OBJECT PASSED BY ADDRESS
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest 'noexcept' types that do *not* "
                            "take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\t'noexcept' With placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest 'noexcept'  types that *do* "
                            "take an allocator.\n");

        // In this and the next test sequence (using 'AllocNETestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocNETestType::testFunc*' methods.

        if (verbose) printf("\t\t'noexcept' With placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
      }

DEFINE_TEST_CASE(4) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BIND WITH MEMBER FUNCTION POINTER
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_TESTFUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_TESTFUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_TESTFUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_TESTFUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest 'noexcept' types that do *not* "
                            "take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_TESTFUNCNENAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_TESTFUNCNENAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest 'noexcept' types that *do* "
                            "take an allocator.\n");

        // In this and the next test sequence (using 'AllocNETestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocNETestType::testFunc*' methods.

        if (verbose) printf("\t\tWith placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_TESTFUNCNEAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_TESTFUNCNEAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
#endif // BBT_n == 14
      }

DEFINE_TEST_CASE(3) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BIND WITH FREE FUNCTION REFERENCE
        //
        // Concerns and plans:
        //    Identical to case 2, except passing 'funcN' instead of
        //    '&funcN'.
        //
        // Testing:
        //   bdlf::BindUtil::bind(Func const&, ...);
        // --------------------------------------------------------------------

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest 'noexcept' types that do *not* "
                            "take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\t'noexcept' With placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_FUNCNENAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_FUNCNENAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest types that *do* take an allocator.\n");

        // In this and the next test sequence (using 'AllocNETestType'), PARAMS
        // allocations should take place via the 'Z1' allocator (one per each
        // argument) as triggered in the 'AllocNETestType::testFunc*' methods.

        if (verbose) printf("\t\t'noexcept' With placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 BBT_FUNCNEAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        BBT_FUNCNEAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
#endif // BBT_n == 14
      }

DEFINE_TEST_CASE(2) {
        DECLARE_BBTN_MAIN_VARIABLES
        // --------------------------------------------------------------------
        // TESTING BDLF::BINDUTIL::BIND WITH FREE FUNCTION POINTER
        //
        // Concerns:
        //   That the 'bdlf::BindUtil::bind' static method returns a
        //   'bdlf::Bind' object that returns the correct value, has the
        //   appropriate side effects, and performs the proper amount of memory
        //   allocation.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bind' with the address of the 'funcN' class
        //   method of the class 'TestFunctionsNoAlloc' or
        //   'TestFunctionsNoAlloc', that takes a mutable function
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

        (void)veryVerbose;

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

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_FUNCNAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\tWithout placeholder.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;
            const NoAllocTestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_FUNCNAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_FUNCAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
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

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_FUNCAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\tTest 'noexcept' types that do *not* "
                            "take an allocator.\n");

        // In this test case sequences (using 'NoAllocTestType'), no allocation
        // at all should take place regardless of whether place-holders are
        // used or not.

        if (verbose) printf("\t\t'noexcept' With placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_FUNCNENAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_In));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\'noexcept' tWithout placeholder.\n");
        {
                  NoAllocNETestType  mX;
            const NoAllocNETestType& X = mX;
            const NoAllocNETestType  BBT_OBJ(EXPECTED, BBT_In);
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z0->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_FUNCNENAn,
                                                        BBT_K(&mX, BBT_In))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_ALLOCS = Z0->numAllocations()
                                                - NUM_ALLOCS_BEFORE;
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' With placeholder.\n");
        {
            using namespace bdlf::PlaceHolders;

            // No default allocation should take place if placeholders are
            // used, because the 'AllocTestValue' arguments will be passed only
            // at invocation time.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                 &BBT_FUNCNEAn,
                                                 BBT_K(&mX, BBT_phn))(BBT_Vn));

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 0 == NUM_DFLT_ALLOCS);
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }

        if (verbose) printf("\t\t'noexcept' Without placeholder.\n");
        {
            // Default allocation should occur for each bound argument of type
            // 'AllocTestValue', as those are copied into the binder object
            // twice: once for making the tuple list type to be passed to the
            // binder constructor, and a second time for copying that tuple
            // into the binder.

                  AllocNETestType  mX(Z1);
            const AllocNETestType& X = mX;
            const AllocNETestType  EXPECTED(BBT_K(Z1, BBT_Vn));
            LOOP_ASSERT(PARAMS, (BBT_n > 0) == (EXPECTED != X));

            const bsls::Types::Int64 NUM_DFLT_ALLOCS_BEFORE =
                                                          Z0->numAllocations();
            const bsls::Types::Int64 NUM_ALLOCS_BEFORE = Z1->numAllocations();

            LOOP_ASSERT(PARAMS, PARAMS == bdlf::BindUtil::bind(
                                                        &BBT_FUNCNEAn,
                                                        BBT_K(&mX, BBT_Vn))());

            LOOP_ASSERT(PARAMS, EXPECTED == X);

            const bsls::Types::Int64 NUM_DFLT_ALLOCS = Z0->numAllocations()
                                                     - NUM_DFLT_ALLOCS_BEFORE;
            const bsls::Types::Int64 NUM_ALLOCS      = Z1->numAllocations()
                                                     - NUM_ALLOCS_BEFORE;

#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO
            LOOP2_ASSERT(PARAMS, NUM_DFLT_ALLOCS, 2*PARAMS== NUM_DFLT_ALLOCS);
#endif
            LOOP2_ASSERT(PARAMS, NUM_ALLOCS, PARAMS == NUM_ALLOCS);
        }
#endif // BBT_n == 14
      }

DEFINE_TEST_CASE(1) {
        DECLARE_BBTN_MAIN_VARIABLES
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
            const bsls::Types::Int64 NA = Z0->numAllocations();

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
            const bsls::Types::Int64 NA = Z0->numAllocations();

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

#elif !defined(BDLF_BIND_00T_AS_INCLUDE)

int main()
{
    (void)PARAMS;
    (void)NUM_SLOTS;
    (void)N1;

    return -1;
}

#endif  // ifdef BBT_n

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
