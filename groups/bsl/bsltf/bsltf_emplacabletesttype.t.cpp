// bsltf_emplacabletesttype.t.cpp                                     -*-C++-*-
#include <bsltf_emplacabletesttype.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorproctor.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_objectbuffer.h>
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <new>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
|| (defined(BSLS_PLATFORM_CMP_CLANG) && !defined(__GXX_EXPERIMENTAL_CXX0X__)) \
|| (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR < 1800)

# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD:
// Global Concerns:
//: o No memory is every allocated from this component.
//-----------------------------------------------------------------------------
// CREATORS
// [  ] EmplacableTestType();
// [  ] EmplacableTestType(a01);
// [  ] EmplacableTestType(a01 .. a02);
// [  ] EmplacableTestType(a01 .. a03);
// [  ] EmplacableTestType(a01 .. a04);
// [  ] EmplacableTestType(a01 .. a05);
// [  ] EmplacableTestType(a01 .. a06);
// [  ] EmplacableTestType(a01 .. a07);
// [  ] EmplacableTestType(a01 .. a08);
// [  ] EmplacableTestType(a01 .. a09);
// [  ] EmplacableTestType(a01 .. a10);
// [  ] EmplacableTestType(a01 .. a11);
// [  ] EmplacableTestType(a01 .. a12);
// [  ] EmplacableTestType(a01 .. a13);
// [  ] EmplacableTestType(a01 .. a14);
// [  ] ~EmplacableTestType();
//
// ACCESSORS
// [  ] ArgType01 arg01() const;
// [  ] ArgType02 arg02() const;
// [  ] ArgType03 arg03() const;
// [  ] ArgType04 arg04() const;
// [  ] ArgType05 arg05() const;
// [  ] ArgType06 arg06() const;
// [  ] ArgType07 arg07() const;
// [  ] ArgType08 arg08() const;
// [  ] ArgType09 arg09() const;
// [  ] ArgType10 arg10() const;
// [  ] ArgType11 arg11() const;
// [  ] ArgType12 arg12() const;
// [  ] ArgType13 arg13() const;
// [  ] ArgType14 arg14() const;
// [  ] bool isEqual(rhs) const;
//
// FREE OPERATORS
// [  ] bool operator==(lhs, rhs);
// [  ] bool operator!=(lhs, rhs);
//
// CLASS METHODS
// [  ] static getNumDeletes();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ *] CONCERN: No memory is ever allocated.

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef EmplacableTestType Obj;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

static const EmplacableTestType::ArgType01 V01(1);
static const EmplacableTestType::ArgType02 V02(20);
static const EmplacableTestType::ArgType03 V03(23);
static const EmplacableTestType::ArgType04 V04(44);
static const EmplacableTestType::ArgType05 V05(66);
static const EmplacableTestType::ArgType06 V06(176);
static const EmplacableTestType::ArgType07 V07(878);
static const EmplacableTestType::ArgType08 V08(8);
static const EmplacableTestType::ArgType09 V09(912);
static const EmplacableTestType::ArgType10 V10(102);
static const EmplacableTestType::ArgType11 V11(111);
static const EmplacableTestType::ArgType12 V12(333);
static const EmplacableTestType::ArgType13 V13(712);
static const EmplacableTestType::ArgType14 V14(1414);

struct TestDriver {
    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type );
    template <class T>
    static const T&             testArg(T& t, bsl::false_type);

    template <class ALLOCATOR>
    static bslma::Allocator *extractBslma(ALLOCATOR basicAllocator) {
        return basicAllocator.testAllocator();
    }

    template <class ALLOCATOR>
    static bslma::Allocator *extractBslma(ALLOCATOR *basicAllocator) {
        return basicAllocator;
    }
    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase1();
        // Implement breathing test for the specified (template type parameter)
        // 'ALLOCATOR'.  See the test case function for documented concerns and
        // test plan.
};

template <class T>
inline
bslmf::MovableRef<T> TestDriver::testArg(T& t, bsl::true_type)
{
    return bslmf::MovableRefUtil::move(t);
}

template <class T>
inline
const T& TestDriver::testArg(T& t, bsl::false_type)
{
    return  t;
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void TestDriver::testCase1()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::TestAllocatorMonitor dam(da);

    // In C++17 these become the simpler to name 'bool_constant'
    static const bsl::integral_constant<bool, N01 == 1> MOVE_01 = {};
    static const bsl::integral_constant<bool, N02 == 1> MOVE_02 = {};
    static const bsl::integral_constant<bool, N03 == 1> MOVE_03 = {};
    static const bsl::integral_constant<bool, N04 == 1> MOVE_04 = {};
    static const bsl::integral_constant<bool, N05 == 1> MOVE_05 = {};
    static const bsl::integral_constant<bool, N06 == 1> MOVE_06 = {};
    static const bsl::integral_constant<bool, N07 == 1> MOVE_07 = {};
    static const bsl::integral_constant<bool, N08 == 1> MOVE_08 = {};
    static const bsl::integral_constant<bool, N09 == 1> MOVE_09 = {};
    static const bsl::integral_constant<bool, N10 == 1> MOVE_10 = {};
    static const bsl::integral_constant<bool, N11 == 1> MOVE_11 = {};
    static const bsl::integral_constant<bool, N12 == 1> MOVE_12 = {};
    static const bsl::integral_constant<bool, N13 == 1> MOVE_13 = {};
    static const bsl::integral_constant<bool, N14 == 1> MOVE_14 = {};

    EmplacableTestType::ArgType01 A01(V01);
    EmplacableTestType::ArgType02 A02(V02);
    EmplacableTestType::ArgType03 A03(V03);
    EmplacableTestType::ArgType04 A04(V04);
    EmplacableTestType::ArgType05 A05(V05);
    EmplacableTestType::ArgType06 A06(V06);
    EmplacableTestType::ArgType07 A07(V07);
    EmplacableTestType::ArgType08 A08(V08);
    EmplacableTestType::ArgType09 A09(V09);
    EmplacableTestType::ArgType10 A10(V10);
    EmplacableTestType::ArgType11 A11(V11);
    EmplacableTestType::ArgType12 A12(V12);
    EmplacableTestType::ArgType13 A13(V13);
    EmplacableTestType::ArgType14 A14(V14);

    bsls::ObjectBuffer<Obj> buffer;
    const Obj& EXP = buffer.object();

    switch (N_ARGS) {
      case 0: {
        new(buffer.address()) Obj();
      } break;
      case 1: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01));
      } break;
      case 2: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02));
      } break;
      case 3: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03));
      } break;
      case 4: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04));
      } break;
      case 5: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05));
      } break;
      case 6: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06));
      } break;
      case 7: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07));
      } break;
      case 8: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08));
      } break;
      case 9: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09));
      } break;
      case 10: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10));
      } break;
      case 11: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11));
      } break;
      case 12: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11),
                                   testArg(A12, MOVE_12));
      } break;
      case 13: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11),
                                   testArg(A12, MOVE_12),
                                   testArg(A13, MOVE_13));
      } break;
      case 14: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11),
                                   testArg(A12, MOVE_12),
                                   testArg(A13, MOVE_13),
                                   testArg(A14, MOVE_14));
      } break;
      default: {
        ASSERTV(0);
      } break;
    }
    bslma::DestructorProctor<Obj> proctor(&buffer.object());

    ASSERTV(MOVE_01, A01.movedFrom(), MOVE_01 == A01.movedFrom());
    ASSERTV(MOVE_02, A02.movedFrom(), MOVE_02 == A02.movedFrom());
    ASSERTV(MOVE_03, A03.movedFrom(), MOVE_03 == A03.movedFrom());
    ASSERTV(MOVE_04, A04.movedFrom(), MOVE_04 == A04.movedFrom());
    ASSERTV(MOVE_05, A05.movedFrom(), MOVE_05 == A05.movedFrom());
    ASSERTV(MOVE_06, A06.movedFrom(), MOVE_06 == A06.movedFrom());
    ASSERTV(MOVE_07, A07.movedFrom(), MOVE_07 == A07.movedFrom());
    ASSERTV(MOVE_08, A08.movedFrom(), MOVE_08 == A08.movedFrom());
    ASSERTV(MOVE_09, A09.movedFrom(), MOVE_09 == A09.movedFrom());
    ASSERTV(MOVE_10, A10.movedFrom(), MOVE_10 == A10.movedFrom());
    ASSERTV(MOVE_11, A11.movedFrom(), MOVE_11 == A11.movedFrom());
    ASSERTV(MOVE_12, A12.movedFrom(), MOVE_12 == A12.movedFrom());
    ASSERTV(MOVE_13, A13.movedFrom(), MOVE_13 == A13.movedFrom());
    ASSERTV(MOVE_14, A14.movedFrom(), MOVE_14 == A14.movedFrom());

    ASSERTV(V01, EXP.arg01(), V01 == EXP.arg01() || 2 == N01);
    ASSERTV(V02, EXP.arg02(), V02 == EXP.arg02() || 2 == N02);
    ASSERTV(V03, EXP.arg03(), V03 == EXP.arg03() || 2 == N03);
    ASSERTV(V04, EXP.arg04(), V04 == EXP.arg04() || 2 == N04);
    ASSERTV(V05, EXP.arg05(), V05 == EXP.arg05() || 2 == N05);
    ASSERTV(V06, EXP.arg06(), V06 == EXP.arg06() || 2 == N06);
    ASSERTV(V07, EXP.arg07(), V07 == EXP.arg07() || 2 == N07);
    ASSERTV(V08, EXP.arg08(), V08 == EXP.arg08() || 2 == N08);
    ASSERTV(V09, EXP.arg09(), V09 == EXP.arg09() || 2 == N09);
    ASSERTV(V10, EXP.arg10(), V10 == EXP.arg10() || 2 == N10);
    ASSERTV(V11, EXP.arg11(), V11 == EXP.arg11() || 2 == N11);
    ASSERTV(V12, EXP.arg12(), V12 == EXP.arg12() || 2 == N12);
    ASSERTV(V13, EXP.arg13(), V13 == EXP.arg13() || 2 == N13);
    ASSERTV(V14, EXP.arg14(), V14 == EXP.arg14() || 2 == N14);

    ASSERT(dam.isMaxSame());
    ASSERT(dam.isInUseSame());
}
//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------


//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
        if (verbose) printf("\nTesting contructor with no arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase1<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 1 argument"
                            "\n----------------------------------\n");
        TestDriver::testCase1<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 2 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 3 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 4 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 5 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 6 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 7 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        TestDriver::testCase1<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 8 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        TestDriver::testCase1<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        TestDriver::testCase1<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 9 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        TestDriver::testCase1<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        TestDriver::testCase1<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 10 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        TestDriver::testCase1<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 11 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        TestDriver::testCase1<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting contructor with 12 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        TestDriver::testCase1<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting contructor with 13 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        TestDriver::testCase1<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

        if (verbose) printf("\nTesting contructor with 14 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        TestDriver::testCase1<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();
#else
        TestDriver::testCase1< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase1< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase1< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase1<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase1<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase1<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase1<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase1<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
