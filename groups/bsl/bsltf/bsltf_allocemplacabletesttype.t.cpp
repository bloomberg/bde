// bsltf_allocemplacabletesttype.t.cpp                                -*-C++-*-
#include <bsltf_allocemplacabletesttype.h>

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
//: o No memory is every allocated from the global allocator.
//-----------------------------------------------------------------------------
// CREATORS
// [  ] AllocEmplacableTestType(Allocator *ba);
// [  ] AllocEmplacableTestType(a01, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a02, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a03, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a04, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a05, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a06, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a07, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a08, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a09, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a10, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a11, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a12, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a13, Allocator *ba);
// [  ] AllocEmplacableTestType(a01 .. a14, Allocator *ba);
// [  ] ~AllocEmplacableTestType();
//
// ACCESSORS
// [  ] AllocArgType01 arg01() const;
// [  ] AllocArgType02 arg02() const;
// [  ] AllocArgType03 arg03() const;
// [  ] AllocArgType04 arg04() const;
// [  ] AllocArgType05 arg05() const;
// [  ] AllocArgType06 arg06() const;
// [  ] AllocArgType07 arg07() const;
// [  ] AllocArgType08 arg08() const;
// [  ] AllocArgType09 arg09() const;
// [  ] AllocArgType10 arg10() const;
// [  ] AllocArgType11 arg11() const;
// [  ] AllocArgType12 arg12() const;
// [  ] AllocArgType13 arg13() const;
// [  ] AllocArgType14 arg14() const;
// [  ] bslma::Allocator *getAllocator() const;
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
// [ *] CONCERN: No memory is ever allocated from the global allocator.

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

typedef AllocEmplacableTestType Obj;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// GLOBAL VERBOSITY FLAGS
bool             verbose = false;
bool         veryVerbose = false;
bool     veryVeryVerbose = false;
bool veryVeryVeryVerbose = false;

static bslma::TestAllocator g_argAlloc("global arguments allocator");
static const AllocEmplacableTestType::ArgType01 V01(1,    &g_argAlloc);
static const AllocEmplacableTestType::ArgType02 V02(20,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType03 V03(23,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType04 V04(44,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType05 V05(66,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType06 V06(176,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType07 V07(878,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType08 V08(8,    &g_argAlloc);
static const AllocEmplacableTestType::ArgType09 V09(912,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType10 V10(102,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType11 V11(111,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType12 V12(333,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType13 V13(712,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType14 V14(1414, &g_argAlloc);

class TestDriver {
  private:
    // TYPES

    // Shorthands
    typedef bsltf::MoveState                      MoveState;

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

  public:
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

    bslma::TestAllocator aa("arguments allocator", veryVeryVeryVerbose);

    // 14 arguments, all using a local test allocator
    AllocEmplacableTestType::ArgType01 A01(V01, &aa);
    AllocEmplacableTestType::ArgType02 A02(V02, &aa);
    AllocEmplacableTestType::ArgType03 A03(V03, &aa);
    AllocEmplacableTestType::ArgType04 A04(V04, &aa);
    AllocEmplacableTestType::ArgType05 A05(V05, &aa);
    AllocEmplacableTestType::ArgType06 A06(V06, &aa);
    AllocEmplacableTestType::ArgType07 A07(V07, &aa);
    AllocEmplacableTestType::ArgType08 A08(V08, &aa);
    AllocEmplacableTestType::ArgType09 A09(V09, &aa);
    AllocEmplacableTestType::ArgType10 A10(V10, &aa);
    AllocEmplacableTestType::ArgType11 A11(V11, &aa);
    AllocEmplacableTestType::ArgType12 A12(V12, &aa);
    AllocEmplacableTestType::ArgType13 A13(V13, &aa);
    AllocEmplacableTestType::ArgType14 A14(V14, &aa);

    bsls::ObjectBuffer<Obj> buffer;
    Obj& mX = buffer.object(); const Obj& X = mX;

    bslma::TestAllocator ta("test allocator", veryVeryVerbose);
    bslma::TestAllocator *objAllocator = 0;

    bslma::TestAllocatorMonitor dam(da);

    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
        const char CONFIG = cfg;
        switch (cfg) {
          case 'a': {
            dam.reset(da);
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
                ASSERTV(!"Invalid # of args!");
              } break;
            }
            objAllocator = da;
          } break;
          case 'b': {
            dam.reset(da);
            switch (N_ARGS) {
              case 0: {
                new(buffer.address()) Obj(&ta);
              } break;
              case 1: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                          &ta);
              } break;
              case 2: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                          &ta);
              } break;
              case 3: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                           testArg(A03, MOVE_03),
                                          &ta);
              } break;
              case 4: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                           testArg(A03, MOVE_03),
                                           testArg(A04, MOVE_04),
                                          &ta);
              } break;
              case 5: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                           testArg(A03, MOVE_03),
                                           testArg(A04, MOVE_04),
                                           testArg(A05, MOVE_05),
                                          &ta);
              } break;
              case 6: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                           testArg(A03, MOVE_03),
                                           testArg(A04, MOVE_04),
                                           testArg(A05, MOVE_05),
                                           testArg(A06, MOVE_06),
                                          &ta);
              } break;
              case 7: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                           testArg(A03, MOVE_03),
                                           testArg(A04, MOVE_04),
                                           testArg(A05, MOVE_05),
                                           testArg(A06, MOVE_06),
                                           testArg(A07, MOVE_07),
                                          &ta);
              } break;
              case 8: {
                new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                           testArg(A02, MOVE_02),
                                           testArg(A03, MOVE_03),
                                           testArg(A04, MOVE_04),
                                           testArg(A05, MOVE_05),
                                           testArg(A06, MOVE_06),
                                           testArg(A07, MOVE_07),
                                           testArg(A08, MOVE_08),
                                          &ta);
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
                                           testArg(A09, MOVE_09),
                                          &ta);
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
                                           testArg(A10, MOVE_10),
                                          &ta);
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
                                           testArg(A11, MOVE_11),
                                          &ta);
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
                                           testArg(A12, MOVE_12),
                                          &ta);
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
                                           testArg(A13, MOVE_13),
                                          &ta);
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
                                           testArg(A14, MOVE_14),
                                          &ta);
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
            objAllocator = &ta;
          } break;
          default: {
            ASSERTV(!"Invalid config spec!");
          }
        }
        bslma::DestructorProctor<Obj> proctor(&buffer.object());

        bslma::TestAllocator& oa = *objAllocator;

        ASSERTV(CONFIG, &aa == A01.getAllocator());
        ASSERTV(CONFIG, &aa == A02.getAllocator());
        ASSERTV(CONFIG, &aa == A03.getAllocator());
        ASSERTV(CONFIG, &aa == A04.getAllocator());
        ASSERTV(CONFIG, &aa == A05.getAllocator());
        ASSERTV(CONFIG, &aa == A06.getAllocator());
        ASSERTV(CONFIG, &aa == A07.getAllocator());
        ASSERTV(CONFIG, &aa == A08.getAllocator());
        ASSERTV(CONFIG, &aa == A09.getAllocator());
        ASSERTV(CONFIG, &aa == A10.getAllocator());
        ASSERTV(CONFIG, &aa == A11.getAllocator());
        ASSERTV(CONFIG, &aa == A12.getAllocator());
        ASSERTV(CONFIG, &aa == A13.getAllocator());
        ASSERTV(CONFIG, &aa == A14.getAllocator());

        ASSERTV(CONFIG, &oa == X.arg01().getAllocator() || 2 == N01);
        ASSERTV(CONFIG, &oa == X.arg02().getAllocator() || 2 == N02);
        ASSERTV(CONFIG, &oa == X.arg03().getAllocator() || 2 == N03);
        ASSERTV(CONFIG, &oa == X.arg04().getAllocator() || 2 == N04);
        ASSERTV(CONFIG, &oa == X.arg05().getAllocator() || 2 == N05);
        ASSERTV(CONFIG, &oa == X.arg06().getAllocator() || 2 == N06);
        ASSERTV(CONFIG, &oa == X.arg07().getAllocator() || 2 == N07);
        ASSERTV(CONFIG, &oa == X.arg08().getAllocator() || 2 == N08);
        ASSERTV(CONFIG, &oa == X.arg09().getAllocator() || 2 == N09);
        ASSERTV(CONFIG, &oa == X.arg10().getAllocator() || 2 == N10);
        ASSERTV(CONFIG, &oa == X.arg11().getAllocator() || 2 == N11);
        ASSERTV(CONFIG, &oa == X.arg12().getAllocator() || 2 == N12);
        ASSERTV(CONFIG, &oa == X.arg13().getAllocator() || 2 == N13);
        ASSERTV(CONFIG, &oa == X.arg14().getAllocator() || 2 == N14);


        ASSERTV(MOVE_01, A01.movedFrom(),
                           MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
        ASSERTV(MOVE_02, A02.movedFrom(),
                           MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
        ASSERTV(MOVE_03, A03.movedFrom(),
                           MOVE_03 ==(MoveState::e_MOVED ==  A03.movedFrom()));
        ASSERTV(MOVE_04, A04.movedFrom(),
                           MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
        ASSERTV(MOVE_05, A05.movedFrom(),
                           MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
        ASSERTV(MOVE_06, A06.movedFrom(),
                           MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
        ASSERTV(MOVE_07, A07.movedFrom(),
                           MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
        ASSERTV(MOVE_08, A08.movedFrom(),
                           MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
        ASSERTV(MOVE_09, A09.movedFrom(),
                           MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
        ASSERTV(MOVE_10, A10.movedFrom(),
                           MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
        ASSERTV(MOVE_11, A11.movedFrom(),
                           MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
        ASSERTV(MOVE_12, A12.movedFrom(),
                           MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
        ASSERTV(MOVE_13, A13.movedFrom(),
                           MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
        ASSERTV(MOVE_14, A14.movedFrom(),
                           MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));

        ASSERTV(V01, X.arg01(), V01 == X.arg01() || 2 == N01);
        ASSERTV(V02, X.arg02(), V02 == X.arg02() || 2 == N02);
        ASSERTV(V03, X.arg03(), V03 == X.arg03() || 2 == N03);
        ASSERTV(V04, X.arg04(), V04 == X.arg04() || 2 == N04);
        ASSERTV(V05, X.arg05(), V05 == X.arg05() || 2 == N05);
        ASSERTV(V06, X.arg06(), V06 == X.arg06() || 2 == N06);
        ASSERTV(V07, X.arg07(), V07 == X.arg07() || 2 == N07);
        ASSERTV(V08, X.arg08(), V08 == X.arg08() || 2 == N08);
        ASSERTV(V09, X.arg09(), V09 == X.arg09() || 2 == N09);
        ASSERTV(V10, X.arg10(), V10 == X.arg10() || 2 == N10);
        ASSERTV(V11, X.arg11(), V11 == X.arg11() || 2 == N11);
        ASSERTV(V12, X.arg12(), V12 == X.arg12() || 2 == N12);
        ASSERTV(V13, X.arg13(), V13 == X.arg13() || 2 == N13);
        ASSERTV(V14, X.arg14(), V14 == X.arg14() || 2 == N14);

        if (&oa == &ta) {
            ASSERTV(CONFIG, dam.isMaxSame());
            ASSERTV(CONFIG, dam.isInUseSame());
        }
    }
}
//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------


//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

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
        if (verbose) printf("\nTesting ctor with 1..14 arguments, move=0"
                            "\n-----------------------------------------\n");
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

    // CONCERN: No memory is ever allocated from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
