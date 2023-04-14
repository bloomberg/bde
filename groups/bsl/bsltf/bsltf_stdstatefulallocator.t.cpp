// bsltf_stdstatefulallocator.t.cpp                                   -*-C++-*-
#include <bsltf_stdstatefulallocator.h>

#include <bsltf_movestate.h>
#include <bsltf_simpletesttype.h>
#include <bsltf_templatetestfacility.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>

#include <limits>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;
using bsls::NameOf;

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
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
// The component under test implements a value-semantic type,
// 'bsltf::StdStatefulAllocator' whose value consists of a single pointer to a
// 'bslma::TestAllocator' object (its underlying "mechanism").    It cannot be
// reset, however, since normally an allocator does not change during the
// lifetime of an object.  A 'bsltf::StdStatefulAllocator' is parameterized by
// the type that it allocates, and that influences the behavior of several
// manipulators and accessors, mainly depending on the size of that type.  The
// same 'bsltf::StdStatefulAllocator' can be re-parameterized for another type
// ("rebound") using the 'rebind' nested template.
//
// Although 'bsltf::StdStatefulAllocator' is a value-semantic type, the fact
// that its value is fixed at construction and not permitted to change let us
// relax the usual concerns of a typical value-semantic type.  Our specific
// concerns are that an allocator constructed with a certain underlying
// mechanism actually uses that mechanism to allocate memory, and that its
// rebound versions also do.  Another concern is that the 'max_size' is the
// maximum possible size for that type (i.e., it is impossible to meaningfully
// pass in a larger size), and that the 'size_type' is unsigned, the
// 'difference_type' is signed, and generally all the requirements of C++
// standard allocators are met (20.1.2 [allocator.requirements]).
//-----------------------------------------------------------------------------
// class StdStatefulAllocator
//
// PUBLIC TYPES
// [13] size_type
// [13] difference_type
// [13] pointer
// [13] const_pointer
// [13] value_type
// [13] rebind<U>::other
// [14] propagate_on_container_copy_assignment
// [14] propagate_on_container_move_assignment
// [14] propagate_on_container_swap
// [14] is_always_equal
//
// CREATORS
// [ 2] StdStatefulAllocator(bslma::allocator *);
// [ 7] StdStatefulAllocator(const StdStatefulAllocator& original);
// [ 7] StdStatefulAllocator(const StdStatefulAllocator<OTHER>&);
// [ 2] ~StdStatefulAllocator();
//
// MANIPULATORS
// [ 9] StdStatefulAllocator& operator=(const StdStatefulAllocator& rhs);
// [11] pointer allocate(size_type numElements, const void *hint = 0);
// [11] void deallocate(pointer address, size_type numElements);
// [15] void construct(ELEMENT_TYPE *address, Args&&... arguments);
//
// ACCESSORS
// [ 4] bslma::Allocator *allocator() const;
// [12] StdStatefulAllocator select_on_container_copy_construction() const;
// [16] size_type max_size() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
// [ 6] bool operator!=(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

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

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE      BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
#define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR

// TBD
// For specific test cases, remove 'bsltf::TemplateTestFacility::FunctionPtr'
// from the list of types to test on Linux to avoid:
//   collect2: error: /opt/bb/bin/gnm returned 1 exit status
// which occurs when 'gnm' is run on 'bsltf_stdstatefulallocator.t.cpp.1.o'.
// Also see 'bslstl_deque.t.cpp'.

#if defined(BSLS_PLATFORM_OS_LINUX)
#define REDUCED_TEST_TYPES_REGULAR                                            \
    signed char,                                                              \
    size_t,                                                                   \
    bsltf::TemplateTestFacility::ObjectPtr,                                   \
    bsltf::TemplateTestFacility::MethodPtr,                                   \
    bsltf::EnumeratedTestType::Enum,                                          \
    bsltf::UnionTestType,                                                     \
    bsltf::SimpleTestType,                                                    \
    bsltf::AllocTestType,                                                     \
    bsltf::BitwiseCopyableTestType,                                           \
    bsltf::BitwiseMoveableTestType,                                           \
    bsltf::AllocBitwiseMoveableTestType,                                      \
    bsltf::NonTypicalOverloadsTestType
#else
#define REDUCED_TEST_TYPES_REGULAR                                            \
    BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
#endif

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
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef StdStatefulAllocator<int>   ObjI;
typedef StdStatefulAllocator<float> ObjF;

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                            TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class VALUE>
class TestDriver {
    // Test driver class for 'StdStatefulAllocator'

  private:
    // TYPES
    typedef bsltf::StdStatefulAllocator<VALUE> Obj;
        // type under testing

    // Legend
    // ------
    // 'CC'   - PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION
    // 'CA'   - PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT
    // 'SWAP' - PROPAGATE_ON_CONTAINER_SWAP,
    // 'MA'   - PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT
    // 'IAE'  - IS_ALWAYS_EQUAL
    //
    //                       TYPE   CC     CA     SWAP   MA     IAE
    //                       -----  -----  -----  -----  -----  ------
    typedef
        StdStatefulAllocator<VALUE, false, false, false, false, false> Obj0;
    typedef
        StdStatefulAllocator<VALUE, false, false, false, false, true>  Obj1;
    typedef
        StdStatefulAllocator<VALUE, false, false, false, true,  false> Obj2;
    typedef
        StdStatefulAllocator<VALUE, false, false, false, true,  true>  Obj3;
    typedef
        StdStatefulAllocator<VALUE, false, false, true,  false, false> Obj4;
    typedef
        StdStatefulAllocator<VALUE, false, false, true,  false, true>  Obj5;
    typedef
        StdStatefulAllocator<VALUE, false, false, true,  true,  false> Obj6;
    typedef
        StdStatefulAllocator<VALUE, false, false, true,  true,  true>  Obj7;
    typedef
        StdStatefulAllocator<VALUE, false, true,  false, false, false> Obj8;
    typedef
        StdStatefulAllocator<VALUE, false, true,  false, false, true>  Obj9;
    typedef
        StdStatefulAllocator<VALUE, false, true,  false, true,  false> Obj10;
    typedef
        StdStatefulAllocator<VALUE, false, true,  false, true,  true>  Obj11;
    typedef
        StdStatefulAllocator<VALUE, false, true,  true,  false, false> Obj12;
    typedef
        StdStatefulAllocator<VALUE, false, true,  true,  false, true>  Obj13;
    typedef
        StdStatefulAllocator<VALUE, false, true,  true,  true,  false> Obj14;
    typedef
        StdStatefulAllocator<VALUE, false, true,  true,  true,  true>  Obj15;
    typedef
        StdStatefulAllocator<VALUE, true, false, false, false, false>  Obj16;
    typedef
        StdStatefulAllocator<VALUE, true,  false, false, false, true>  Obj17;
    typedef
        StdStatefulAllocator<VALUE, true,  false, false, true,  false> Obj18;
    typedef
        StdStatefulAllocator<VALUE, true,  false, false, true,  true>  Obj19;
    typedef
        StdStatefulAllocator<VALUE, true,  false, true,  false, false> Obj20;
    typedef
        StdStatefulAllocator<VALUE, true,  false, true,  false, true>  Obj21;
    typedef
        StdStatefulAllocator<VALUE, true,  false, true,  true,  false> Obj22;
    typedef
        StdStatefulAllocator<VALUE, true,  false, true,  true,  true>  Obj23;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  false, false, false> Obj24;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  false, false, true>  Obj25;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  false, true,  false> Obj26;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  false, true,  true>  Obj27;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  true,  false, false> Obj28;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  true,  false, true>  Obj29;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  true,  true,  false> Obj30;
    typedef
        StdStatefulAllocator<VALUE, true,  true,  true,  true,  true>  Obj31;

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
        // Return the specified 't' moved.
    {
        return bslmf::MovableRefUtil::move(t);
    }

    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
        // Return a reference providing non-modifiable access to the specified
        // 't'.
    {
        return t;
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
              int N10>
    static void testCase15_RunTest(Obj *target);
        // Test forwarding of arguments in 'construct' method and 'construct'
        // method itself.

  public:

    // TEST CASES
    static void testCase16();
        // Test 'max_size'.

    static void testCase15();
        // Test 'construct'.

    static void testCase14();
        // Test traits propagation.

    static void testCase13();
        // Test types aliases.

    static void testCase12();
        // Test 'select_on_container_copy_construction' member.

    static void testCase11();
        // Test 'allocate' and 'deallocate' members.

    static void testCase9();
        // Test copy assignment operator.

    static void testCase7();
        // Test copy constructors.

    static void testCase6();
        // Test equality operators ('operator==' and 'operator!=').

    static void testCase4();
        // Test basic accessors ('allocator').

    static void testCase2();
        // Test primary manipulators (value constructor and destructor).

    static void testCase1();
        // Breathing test.
};

template <class VALUE>
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
          int N10>
void TestDriver<VALUE>::testCase15_RunTest(Obj *object)
{
    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    static const EmplacableTestType::ArgType01 V01(1);
    static const EmplacableTestType::ArgType02 V02(2);
    static const EmplacableTestType::ArgType03 V03(3);
    static const EmplacableTestType::ArgType04 V04(4);
    static const EmplacableTestType::ArgType05 V05(5);
    static const EmplacableTestType::ArgType06 V06(6);
    static const EmplacableTestType::ArgType07 V07(7);
    static const EmplacableTestType::ArgType08 V08(8);
    static const EmplacableTestType::ArgType09 V09(9);
    static const EmplacableTestType::ArgType10 V10(10);

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

    bsls::ObjectBuffer<EmplacableTestType> buffer;
    const EmplacableTestType&              EXP = buffer.object();

    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::TestAllocatorMonitor dam(da);
    switch (N_ARGS) {
      case 0: {
        object->construct(buffer.address());
      } break;
      case 1: {
        object->construct(buffer.address(), testArg(A01, MOVE_01));
      } break;
      case 2: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02));
      } break;
      case 3: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03));
      } break;
      case 4: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04));
      } break;
      case 5: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05));
      } break;
      case 6: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06));
      } break;
      case 7: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07));
      } break;
      case 8: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08));
      } break;
      case 9: {
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
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
        object->construct(buffer.address(),
                          testArg(A01, MOVE_01),
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
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Invalid # of args!");
      } break;
    }

    bslma::DestructorGuard<EmplacableTestType> guard(&buffer.object());

    // Verify forwarding of arguments.

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
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

    ASSERT(dam.isMaxSame());
    ASSERT(dam.isInUseSame());
}

                                // ----------
                                // TEST CASES
                                // ----------
template <class VALUE>
void TestDriver<VALUE>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING 'max_size'
    //
    // Concerns:
    //: 1 The result of 'max_size' fits and represents the maximum possible
    //:   number of bytes in a 'bslma::Allocator::size_type' for
    //:   'StdStatefulAllocator' parametrized by type, having size equal to 1
    //:   byte.
    //:
    //: 2 The result of 'max_size' represents the maximum possible number of
    //:   objects of (template parameter) 'TYPE', that occupy no more memory
    //:   (in bytes) than 'bslma::Allocator::size_type' maximum value.
    //
    // Plan:
    //: 1 Create a 'StdStatefulAllocator' object, parameterized with 'char'.
    //:   Verify, that 'max_size' returns maximum value of
    //:   'bslma::Allocator::size_type'.  (C-1)
    //:
    //: 2 Create a 'StdStatefulAllocator' object, parameterized with (function
    //:   template parameter) 'VALUE'.  Verify, that 'max_size' returns
    //:   positive value, that represents the number of objects that occupy no
    //:   more memory(in bytes) than 'bslma::Allocator::size_type' maximum
    //:   value.  Verify that 'max_size + 1' number of objects occupy more
    //:   memory(in bytes) than 'bslma::Allocator::size_type' maximum value.
    //:   (C-2)
    //
    // Testing:
    //   size_type max_size() const;
    // ------------------------------------------------------------------------
#if !defined(BSLSTL_ALLOCATOR_TRAITS_SUPPORTS_ALL_CPP11_DEDUCTIONS)

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    typedef bslma::Allocator::size_type SizeType;
    typedef StdStatefulAllocator<char>  ObjC;

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ObjC                 mZ(&oa);
    const ObjC&          Z = mZ;
    const SizeType       CMS = Z.max_size();

    // Verify that max_size() is the largest positive integer of type
    // size_type.

    ASSERTV(CMS, 0 < CMS);
    ASSERTV(CMS, std::numeric_limits<SizeType>::max() == CMS);

    Obj        mX(&oa);
    const Obj& X = mX;

    // Detect problem with MSVC in 64-bit mode, which can't do 64-bit int
    // arithmetic correctly for enums.

    ASSERT(X.max_size() <= Z.max_size());

    SizeType vms      = X.max_size();
    SizeType vmss     = vms  * sizeof(VALUE);
    SizeType vmssplus = vmss + sizeof(VALUE);

    ASSERTV(vms,      0   <  vms);
    ASSERTV(vmss,     CMS >= vmss);
    ASSERTV(vmss,     vms <= vmss);      // no overflow
    ASSERTV(vmssplus, vms >  vmssplus);  // overflow

#endif
}

template <class VALUE>
void TestDriver<VALUE>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING 'construct'
    //
    // Concerns:
    //: 1 The 'construct' correctly forwards arguments to the constructor of
    //:   the object being constructed (up to 10 arguments).
    //:
    //: 2 No memory is allocated from default allocator or from the
    //:   'StdStatefulAllocator' object.
    //:
    // Plan:
    //: 1 This test makes material use of template method
    //:   'testCase15_RunTest' with first integer template parameter
    //:   indicating the number of arguments to use, the next 10 integer
    //:   template parameters indicating '0' for copy, '1' for move, and
    //:   '2' for not-applicable (i.e., beyond the number of arguments).
    //:
    //:   1 Create 'StdStatefulAllocator' object.
    //:
    //:   2 Create 10 unique argument values.
    //:
    //:   3 Based on (first) template parameter indicating the number of
    //:     args to pass in, call 'construct' with the corresponding
    //:     argument values, performing an explicit move of the argument if
    //:     so indicated by the template parameter corresponding to the
    //:     argument, all in the presence of injected exceptions.
    //:
    //:   4 Verify that the argument values were passed correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.  (C-1)
    //:
    //:   6 Verify that no memory is allocated from default allocator.
    //:
    //:   7 Verify that no memory is allocated from the allocator, associated
    //:     with current 'StdStatefulAllocator' object.  (C-2)
    //
    // Testing:
    //   void construct(ELEMENT_TYPE *address, Args&&... arguments);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
    Obj                          mX(&oa);
    bslma::TestAllocatorMonitor  oam(&oa);

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\tTesting 'construct' with no arguments\n");
    testCase15_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 1 argument\n");
    testCase15_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 2 arguments\n");
    testCase15_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 3 arguments\n");
    testCase15_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 4 arguments\n");
    testCase15_RunTest<4,0,0,0,0,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<4,1,0,0,0,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<4,0,1,0,0,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<4,0,0,1,0,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<4,0,0,0,1,2,2,2,2,2,2>(&mX);
    testCase15_RunTest<4,1,1,1,1,2,2,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 5 arguments\n");
    testCase15_RunTest<5,0,0,0,0,0,2,2,2,2,2>(&mX);
    testCase15_RunTest<5,1,0,0,0,0,2,2,2,2,2>(&mX);
    testCase15_RunTest<5,0,1,0,0,0,2,2,2,2,2>(&mX);
    testCase15_RunTest<5,0,0,1,0,0,2,2,2,2,2>(&mX);
    testCase15_RunTest<5,0,0,0,1,0,2,2,2,2,2>(&mX);
    testCase15_RunTest<5,0,0,0,0,1,2,2,2,2,2>(&mX);
    testCase15_RunTest<5,1,1,1,1,1,2,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 6 arguments\n");
    testCase15_RunTest<6,0,0,0,0,0,0,2,2,2,2>(&mX);
    testCase15_RunTest<6,1,0,0,0,0,0,2,2,2,2>(&mX);
    testCase15_RunTest<6,0,1,0,0,0,0,2,2,2,2>(&mX);
    testCase15_RunTest<6,0,0,1,0,0,0,2,2,2,2>(&mX);
    testCase15_RunTest<6,0,0,0,1,0,0,2,2,2,2>(&mX);
    testCase15_RunTest<6,0,0,0,0,1,0,2,2,2,2>(&mX);
    testCase15_RunTest<6,0,0,0,0,0,1,2,2,2,2>(&mX);
    testCase15_RunTest<6,1,1,1,1,1,1,2,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 7 arguments\n");
    testCase15_RunTest<7,0,0,0,0,0,0,0,2,2,2>(&mX);
    testCase15_RunTest<7,1,0,0,0,0,0,0,2,2,2>(&mX);
    testCase15_RunTest<7,0,1,0,0,0,0,0,2,2,2>(&mX);
    testCase15_RunTest<7,0,0,1,0,0,0,0,2,2,2>(&mX);
    testCase15_RunTest<7,0,0,0,1,0,0,0,2,2,2>(&mX);
    testCase15_RunTest<7,0,0,0,0,1,0,0,2,2,2>(&mX);
    testCase15_RunTest<7,0,0,0,0,0,1,0,2,2,2>(&mX);
    testCase15_RunTest<7,0,0,0,0,0,0,1,2,2,2>(&mX);
    testCase15_RunTest<7,1,1,1,1,1,1,1,2,2,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 8 arguments\n");
    testCase15_RunTest<8,0,0,0,0,0,0,0,0,2,2>(&mX);
    testCase15_RunTest<8,1,0,0,0,0,0,0,0,2,2>(&mX);
    testCase15_RunTest<8,0,1,0,0,0,0,0,0,2,2>(&mX);
    testCase15_RunTest<8,0,0,1,0,0,0,0,0,2,2>(&mX);
    testCase15_RunTest<8,0,0,0,1,0,0,0,0,2,2>(&mX);
    testCase15_RunTest<8,0,0,0,0,1,0,0,0,2,2>(&mX);
    testCase15_RunTest<8,0,0,0,0,0,1,0,0,2,2>(&mX);
    testCase15_RunTest<8,0,0,0,0,0,0,1,0,2,2>(&mX);
    testCase15_RunTest<8,0,0,0,0,0,0,0,1,2,2>(&mX);
    testCase15_RunTest<8,1,1,1,1,1,1,1,1,2,2>(&mX);

#if 0
    // This section is disabled to avoid excessive template instantiation
    // that leads to build error. Temporary fix.
    if (verbose) printf("\tTesting 'construct' with 9 arguments\n");
    testCase15_RunTest<9,0,0,0,0,0,0,0,0,0,2>(&mX);
    testCase15_RunTest<9,1,0,0,0,0,0,0,0,0,2>(&mX);
    testCase15_RunTest<9,0,1,0,0,0,0,0,0,0,2>(&mX);
    testCase15_RunTest<9,0,0,1,0,0,0,0,0,0,2>(&mX);
    testCase15_RunTest<9,0,0,0,1,0,0,0,0,0,2>(&mX);
    testCase15_RunTest<9,0,0,0,0,1,0,0,0,0,2>(&mX);
    testCase15_RunTest<9,0,0,0,0,0,1,0,0,0,2>(&mX);
    testCase15_RunTest<9,0,0,0,0,0,0,1,0,0,2>(&mX);
    testCase15_RunTest<9,0,0,0,0,0,0,0,1,0,2>(&mX);
    testCase15_RunTest<9,0,0,0,0,0,0,0,0,1,2>(&mX);
    testCase15_RunTest<9,1,1,1,1,1,1,1,1,1,2>(&mX);

    if (verbose) printf("\tTesting 'construct' with 10 arguments\n");
    testCase15_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    testCase15_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX);
    testCase15_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX);
    testCase15_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX);
    testCase15_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX);
    testCase15_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX);
    testCase15_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX);
    testCase15_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX);
    testCase15_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX);
    testCase15_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX);
    testCase15_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX);
    testCase15_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
#endif

#else
    testCase15_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
    testCase15_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
    testCase15_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
    testCase15_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
    testCase15_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
    testCase15_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
    testCase15_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
    testCase15_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
#endif

    ASSERT(oam.isMaxSame());
    ASSERT(oam.isInUseSame());
}

template <class VALUE>
void TestDriver<VALUE>::testCase14()
{
    // ------------------------------------------------------------------------
    // PROPAGATION TRAITS
    //   The copy, move and swap behavior of the allocators of C++11
    //   containers is determined by three "propagation traits" that may
    //   be defined by the allocator, but are otherwise assumed to be
    //   'false' i.e., do not propagate.  This component provides the
    //   ability to configure these traits for any given instantiation of
    //   this template.  These traits have no effect on the behavior of the
    //   allocator type itself, but exist solely to be queried by higher
    //   level components, such as containers.  If defined, they must be
    //   an alias to 'bsl::true_type', 'bsl::false_type', or a type
    //   publicly and unambiguously derived from one of these two classes.
    //
    // Concerns:
    //: 1 Template parameters are correctly transformed to the type traits.
    //:
    // Plan:
    //: 1 Define several adaptor types with various combinations of template
    //:   parameters and verify that traits are configured correctly.  (C-1)
    //
    // Testing:
    //   propagate_on_container_copy_assignment
    //   propagate_on_container_move_assignment
    //   propagate_on_container_swap
    //   is_always_equal
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    //                                    CA     SWAP   MA     IAE
    //                                    -----  -----  -----  -----
    // typedef StdStatefulAllocator<...,  false, false, false, false> Obj0;

    ASSERT(!Obj0::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj0::propagate_on_container_swap::value);
    ASSERT(!Obj0::propagate_on_container_move_assignment::value);
    ASSERT(!Obj0::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, false, true>  Obj1;

    ASSERT(!Obj1::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj1::propagate_on_container_swap::value);
    ASSERT(!Obj1::propagate_on_container_move_assignment::value);
    ASSERT( Obj1::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, true, false>  Obj2;

    ASSERT(!Obj2::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj2::propagate_on_container_swap::value);
    ASSERT( Obj2::propagate_on_container_move_assignment::value);
    ASSERT(!Obj2::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, true, true>   Obj3;

    ASSERT(!Obj3::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj3::propagate_on_container_swap::value);
    ASSERT( Obj3::propagate_on_container_move_assignment::value);
    ASSERT( Obj3::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  false, false> Obj4;

    ASSERT(!Obj4::propagate_on_container_copy_assignment::value);
    ASSERT( Obj4::propagate_on_container_swap::value);
    ASSERT(!Obj4::propagate_on_container_move_assignment::value);
    ASSERT(!Obj4::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  false, true>  Obj5;

    ASSERT(!Obj5::propagate_on_container_copy_assignment::value);
    ASSERT( Obj5::propagate_on_container_swap::value);
    ASSERT(!Obj5::propagate_on_container_move_assignment::value);
    ASSERT( Obj5::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  true, false>  Obj6;

    ASSERT(!Obj6::propagate_on_container_copy_assignment::value);
    ASSERT( Obj6::propagate_on_container_swap::value);
    ASSERT( Obj6::propagate_on_container_move_assignment::value);
    ASSERT(!Obj6::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  true, true>   Obj7;

    ASSERT(!Obj7::propagate_on_container_copy_assignment::value);
    ASSERT( Obj7::propagate_on_container_swap::value);
    ASSERT( Obj7::propagate_on_container_move_assignment::value);
    ASSERT( Obj7::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, false, false> Obj8;

    ASSERT( Obj8::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj8::propagate_on_container_swap::value);
    ASSERT(!Obj8::propagate_on_container_move_assignment::value);
    ASSERT(!Obj8::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, false, true>  Obj9;

    ASSERT( Obj9::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj9::propagate_on_container_swap::value);
    ASSERT(!Obj9::propagate_on_container_move_assignment::value);
    ASSERT( Obj9::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, true, false>  Obj10;

    ASSERT( Obj10::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj10::propagate_on_container_swap::value);
    ASSERT( Obj10::propagate_on_container_move_assignment::value);
    ASSERT(!Obj10::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, true, true>   Obj11;

    ASSERT( Obj11::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj11::propagate_on_container_swap::value);
    ASSERT( Obj11::propagate_on_container_move_assignment::value);
    ASSERT( Obj11::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  false, false> Obj12;

    ASSERT( Obj12::propagate_on_container_copy_assignment::value);
    ASSERT( Obj12::propagate_on_container_swap::value);
    ASSERT(!Obj12::propagate_on_container_move_assignment::value);
    ASSERT(!Obj12::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  false, true>  Obj13;

    ASSERT( Obj13::propagate_on_container_copy_assignment::value);
    ASSERT( Obj13::propagate_on_container_swap::value);
    ASSERT(!Obj13::propagate_on_container_move_assignment::value);
    ASSERT( Obj13::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  true, false>  Obj14;

    ASSERT( Obj14::propagate_on_container_copy_assignment::value);
    ASSERT( Obj14::propagate_on_container_swap::value);
    ASSERT( Obj14::propagate_on_container_move_assignment::value);
    ASSERT(!Obj14::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  true, true>   Obj15;

    ASSERT( Obj15::propagate_on_container_copy_assignment::value);
    ASSERT( Obj15::propagate_on_container_swap::value);
    ASSERT( Obj15::propagate_on_container_move_assignment::value);
    ASSERT( Obj15::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, false, false> Obj16;

    ASSERT(!Obj16::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj16::propagate_on_container_swap::value);
    ASSERT(!Obj16::propagate_on_container_move_assignment::value);
    ASSERT(!Obj16::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, false, true>  Obj17;

    ASSERT(!Obj17::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj17::propagate_on_container_swap::value);
    ASSERT(!Obj17::propagate_on_container_move_assignment::value);
    ASSERT( Obj17::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, true, false>  Obj18;

    ASSERT(!Obj18::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj18::propagate_on_container_swap::value);
    ASSERT( Obj18::propagate_on_container_move_assignment::value);
    ASSERT(!Obj18::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, false, true, true>   Obj19;

    ASSERT(!Obj19::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj19::propagate_on_container_swap::value);
    ASSERT( Obj19::propagate_on_container_move_assignment::value);
    ASSERT( Obj19::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  false, false> Obj20;

    ASSERT(!Obj20::propagate_on_container_copy_assignment::value);
    ASSERT( Obj20::propagate_on_container_swap::value);
    ASSERT(!Obj20::propagate_on_container_move_assignment::value);
    ASSERT(!Obj20::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  false, true>  Obj21;

    ASSERT(!Obj21::propagate_on_container_copy_assignment::value);
    ASSERT( Obj21::propagate_on_container_swap::value);
    ASSERT(!Obj21::propagate_on_container_move_assignment::value);
    ASSERT( Obj21::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  true, false>  Obj22;

    ASSERT(!Obj22::propagate_on_container_copy_assignment::value);
    ASSERT( Obj22::propagate_on_container_swap::value);
    ASSERT( Obj22::propagate_on_container_move_assignment::value);
    ASSERT(!Obj22::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  false, true,  true, true>   Obj23;

    ASSERT(!Obj23::propagate_on_container_copy_assignment::value);
    ASSERT( Obj23::propagate_on_container_swap::value);
    ASSERT( Obj23::propagate_on_container_move_assignment::value);
    ASSERT( Obj23::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, false, false> Obj24;

    ASSERT( Obj24::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj24::propagate_on_container_swap::value);
    ASSERT(!Obj24::propagate_on_container_move_assignment::value);
    ASSERT(!Obj24::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, false, true>  Obj25;

    ASSERT( Obj25::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj25::propagate_on_container_swap::value);
    ASSERT(!Obj25::propagate_on_container_move_assignment::value);
    ASSERT( Obj25::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, true, false>  Obj26;

    ASSERT( Obj26::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj26::propagate_on_container_swap::value);
    ASSERT( Obj26::propagate_on_container_move_assignment::value);
    ASSERT(!Obj26::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  false, true, true>   Obj27;

    ASSERT( Obj27::propagate_on_container_copy_assignment::value);
    ASSERT(!Obj27::propagate_on_container_swap::value);
    ASSERT( Obj27::propagate_on_container_move_assignment::value);
    ASSERT( Obj27::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  false, false> Obj28;

    ASSERT( Obj28::propagate_on_container_copy_assignment::value);
    ASSERT( Obj28::propagate_on_container_swap::value);
    ASSERT(!Obj28::propagate_on_container_move_assignment::value);
    ASSERT(!Obj28::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  false, true>  Obj29;

    ASSERT( Obj29::propagate_on_container_copy_assignment::value);
    ASSERT( Obj29::propagate_on_container_swap::value);
    ASSERT(!Obj29::propagate_on_container_move_assignment::value);
    ASSERT( Obj29::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  true, false>  Obj30;

    ASSERT( Obj30::propagate_on_container_copy_assignment::value);
    ASSERT( Obj30::propagate_on_container_swap::value);
    ASSERT( Obj30::propagate_on_container_move_assignment::value);
    ASSERT(!Obj30::is_always_equal::value);

    // typedef StdStatefulAllocator<...,  true,  true,  true, true>   Obj31;

    ASSERT( Obj31::propagate_on_container_copy_assignment::value);
    ASSERT( Obj31::propagate_on_container_swap::value);
    ASSERT( Obj31::propagate_on_container_move_assignment::value);
    ASSERT( Obj31::is_always_equal::value);
}

template <class VALUE>
void TestDriver<VALUE>::testCase13()
{
    // ------------------------------------------------------------------------
    // SPURIOUS NESTED TYPES
    //   There are a number of frequently encountered type aliases that are
    //   often defined by an allocator type.  For a minimal C++11 allocator
    //   these will be supplied automatically by the 'allocator_traits'
    //   template and so should *not* be defined for our minimal allocator.
    //   However, the current 'bsl' implementation of 'allocator_traits'
    //   does not perform the necessary template metaprogramming to deduce
    //   these aliases if missing, so our initial implementation must also
    //   provide these names.  Once we have a more complete implementation
    //   of 'allocator_traits', this test will instead confirm that these
    //   popular type aliases do *not* exist for our template.
    //
    // Concerns:
    //: 1 The 'typedef' aliases defined in this component are as specified
    //:   by the C++11 standard for template instances parameterized on the
    //:   different types.
    //:
    //: 2 'size_type' is unsigned while 'difference_type' is signed.
    //:
    //: 3 'rebind<BDE_OTHER_TYPE>::other' defines a template instance for
    //:   'StdStatefulAllocator' parameterized on the 'BDE_OTHER_TYPE' type.
    //
    // Plan:
    //: 1 Use the 'sizeof' operator to verify that 'size_type' and
    //:   'difference_type' are the right size and verify they are
    //:   unsigned and signed values.  (C-2)
    //:
    //: 2 For all other type aliases, use 'bsl::is_same' to verify that
    //:   they are the expected types.  (C-1)
    //:
    //: 3 Verify using 'bsl::is_same' that 'rebind<U>::other' defines the
    //:   correct type.  (C-3)
    //
    // Testing:
    //   value_type
    //   size_type
    //   difference_type
    //   pointer
    //   const_pointer
    //   rebind<U>::other
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose) printf("\tTesting 'value_type'.\n");
    {
        ASSERT((bsl::is_same<typename Obj::value_type, VALUE>::value));
    }

    if (verbose) printf("\tTesting 'size_type'.\n");
    {
        ASSERT(sizeof(typename Obj::size_type) == sizeof(VALUE*));
        ASSERT(0 < ~(typename Obj::size_type)0);
    }

    if (verbose) printf("\tTesting 'difference_type'.\n");
    {
        ASSERT(sizeof(typename Obj::difference_type) == sizeof(VALUE*));
        ASSERT(0 > ~(typename Obj::difference_type)0);
    }

    if (verbose) printf("\tTesting 'pointer'.\n");
    {
        ASSERT((bsl::is_same<typename Obj::pointer, VALUE*>::value));
    }

    if (verbose) printf("\tTesting 'const_pointer'.\n");
    {
        ASSERT((bsl::is_same<typename Obj::const_pointer,
                             const VALUE*>::value));
    }

    if (verbose) printf("\tTesting 'rebind'.\n");
    {
        typedef typename Obj::template rebind<int  >::other ObjRebindI;
        typedef typename Obj::template rebind<float>::other ObjRebindF;

        ASSERT((bsl::is_same<ObjRebindI, ObjI>::value));
        ASSERT((bsl::is_same<ObjRebindF, ObjF>::value));
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING 'select_on_container_copy_construction'
    //
    // Concerns:
    //: 1 Return value depends only on
    //:   'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' template parameter.
    //:
    //: 2 If 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' template parameter is
    //:   equal to 'true', copy of current object is returned.
    //:
    //: 3 If 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' template parameter is
    //:   equal to 'false', temporary object, delegating requests to the
    //:   default allocator is returned.
    //:
    //: 4 If 'IS_ALWAYS_EQUAL' template parameter is
    //:   equal to 'true', returned objects will always compare as equal to the
    //:   current object, regardless of other parameter values.
    //
    // Plan:
    //: 1 Create several 'StdStatefulAllocator' objects with different template
    //:   parameters.
    //:
    //: 2 Use 'select_on_container_copy_construction' method to create new
    //:   'StdStatefulAllocator' objects.
    //:
    //: 3 Verify that 'select_on_container_copy_construction' method returns a
    //:   copy of origin object if the 'bool' template parameter
    //:   'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' is true, and a copy of a
    //:   'StdStatefulAllocator' object wrapping the default allocator
    //:   otherwise (although this can only be determined when the 'bool'
    //:   template parameter 'IS_ALWAYS_EQUAL' is false).  (C-1..3)
    //
    // Testing:
    //   StdStatefulAllocator select_on_container_copy_construction() const
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::TestAllocator         allocators[32];

                                       //  TYPE   CC  CA  SWAP  MA  IAE
                                       //  -----  --  --  ----  --  ---
    const Obj0  X0 (&allocators[0 ]);  // <VALUE, F,  F,  F,    F,  F>
    const Obj1  X1 (&allocators[1 ]);  // <VALUE, F,  F,  F,    F,  T>
    const Obj2  X2 (&allocators[2 ]);  // <VALUE, F,  F,  F,    T,  F>
    const Obj3  X3 (&allocators[3 ]);  // <VALUE, F,  F,  F,    T,  T>
    const Obj4  X4 (&allocators[4 ]);  // <VALUE, F,  F,  T,    F,  F>
    const Obj5  X5 (&allocators[5 ]);  // <VALUE, F,  F,  T,    F,  T>
    const Obj6  X6 (&allocators[6 ]);  // <VALUE, F,  F,  T,    T,  F>
    const Obj7  X7 (&allocators[7 ]);  // <VALUE, F,  F,  T,    T,  T>
    const Obj8  X8 (&allocators[8 ]);  // <VALUE, F,  T,  F,    F,  F>
    const Obj9  X9 (&allocators[9 ]);  // <VALUE, F,  T,  F,    F,  T>
    const Obj10 X10(&allocators[10]);  // <VALUE, F,  T,  F,    T,  F>
    const Obj11 X11(&allocators[11]);  // <VALUE, F,  T,  F,    T,  T>
    const Obj12 X12(&allocators[12]);  // <VALUE, F,  T,  T,    F,  F>
    const Obj13 X13(&allocators[13]);  // <VALUE, F,  T,  T,    F,  T>
    const Obj14 X14(&allocators[14]);  // <VALUE, F,  T,  T,    T,  F>
    const Obj15 X15(&allocators[15]);  // <VALUE, F,  T,  T,    T,  T>
    const Obj16 X16(&allocators[16]);  // <VALUE, T,  F,  F,    F,  F>
    const Obj17 X17(&allocators[17]);  // <VALUE, T,  F,  F,    F,  T>
    const Obj18 X18(&allocators[18]);  // <VALUE, T,  F,  F,    T,  F>
    const Obj19 X19(&allocators[19]);  // <VALUE, T,  F,  F,    T,  T>
    const Obj20 X20(&allocators[20]);  // <VALUE, T,  F,  T,    F,  F>
    const Obj21 X21(&allocators[21]);  // <VALUE, T,  F,  T,    F,  T>
    const Obj22 X22(&allocators[22]);  // <VALUE, T,  F,  T,    T,  F>
    const Obj23 X23(&allocators[23]);  // <VALUE, T,  F,  T,    T,  T>
    const Obj24 X24(&allocators[24]);  // <VALUE, T,  T,  F,    F,  F>
    const Obj25 X25(&allocators[25]);  // <VALUE, T,  T,  F,    F,  T>
    const Obj26 X26(&allocators[26]);  // <VALUE, T,  T,  F,    T,  F>
    const Obj27 X27(&allocators[27]);  // <VALUE, T,  T,  F,    T,  T>
    const Obj28 X28(&allocators[28]);  // <VALUE, T,  T,  T,    F,  F>
    const Obj29 X29(&allocators[29]);  // <VALUE, T,  T,  T,    F,  T>
    const Obj30 X30(&allocators[30]);  // <VALUE, T,  T,  T,    T,  F>
    const Obj31 X31(&allocators[31]);  // <VALUE, T,  T,  T,    T,  T>

    const Obj0  COPY0  = X0.select_on_container_copy_construction();
    const Obj1  COPY1  = X1.select_on_container_copy_construction();
    const Obj2  COPY2  = X2.select_on_container_copy_construction();
    const Obj3  COPY3  = X3.select_on_container_copy_construction();
    const Obj4  COPY4  = X4.select_on_container_copy_construction();
    const Obj5  COPY5  = X5.select_on_container_copy_construction();
    const Obj6  COPY6  = X6.select_on_container_copy_construction();
    const Obj7  COPY7  = X7.select_on_container_copy_construction();
    const Obj8  COPY8  = X8.select_on_container_copy_construction();
    const Obj9  COPY9  = X9.select_on_container_copy_construction();
    const Obj10 COPY10 = X10.select_on_container_copy_construction();
    const Obj11 COPY11 = X11.select_on_container_copy_construction();
    const Obj12 COPY12 = X12.select_on_container_copy_construction();
    const Obj13 COPY13 = X13.select_on_container_copy_construction();
    const Obj14 COPY14 = X14.select_on_container_copy_construction();
    const Obj15 COPY15 = X15.select_on_container_copy_construction();
    const Obj16 COPY16 = X16.select_on_container_copy_construction();
    const Obj17 COPY17 = X17.select_on_container_copy_construction();
    const Obj18 COPY18 = X18.select_on_container_copy_construction();
    const Obj19 COPY19 = X19.select_on_container_copy_construction();
    const Obj20 COPY20 = X20.select_on_container_copy_construction();
    const Obj21 COPY21 = X21.select_on_container_copy_construction();
    const Obj22 COPY22 = X22.select_on_container_copy_construction();
    const Obj23 COPY23 = X23.select_on_container_copy_construction();
    const Obj24 COPY24 = X24.select_on_container_copy_construction();
    const Obj25 COPY25 = X25.select_on_container_copy_construction();
    const Obj26 COPY26 = X26.select_on_container_copy_construction();
    const Obj27 COPY27 = X27.select_on_container_copy_construction();
    const Obj28 COPY28 = X28.select_on_container_copy_construction();
    const Obj29 COPY29 = X29.select_on_container_copy_construction();
    const Obj30 COPY30 = X30.select_on_container_copy_construction();
    const Obj31 COPY31 = X31.select_on_container_copy_construction();

    // For the following cases, 'propogate_on_container_copy' is false, so a
    // newly default constructed allocator is returned by
    // 'select_on_container_copy_construction'.  As 'is_always_equal' is false,
    // we can use the equality operator to test that the objects are different.
    ASSERT(X0  != COPY0 );
    ASSERT(X2  != COPY2 );
    ASSERT(X4  != COPY4 );
    ASSERT(X6  != COPY6 );
    ASSERT(X8  != COPY8 );
    ASSERT(X10 != COPY10);
    ASSERT(X12 != COPY12);
    ASSERT(X14 != COPY14);

    // For the following cases, 'propogate_on_container_copy' is false, so a
    // newly default constructed allocator is returned by
    // 'select_on_container_copy_construction'.  However, as the
    // 'is_always_equal' trait is true, the equality operator will return
    // 'true' in all cases.
    ASSERT(X1  == COPY1 ); // is_always_equal
    ASSERT(X3  == COPY3 ); // is_always_equal
    ASSERT(X5  == COPY5 ); // is_always_equal
    ASSERT(X7  == COPY7 ); // is_always_equal
    ASSERT(X9  == COPY9 ); // is_always_equal
    ASSERT(X11 == COPY11); // is_always_equal
    ASSERT(X13 == COPY13); // is_always_equal
    ASSERT(X15 == COPY15); // is_always_equal

    // For the following cases, 'propogate_on_container_copy' is false, so a
    // copy of the allocator (obtained using the copy constructor) is returned
    // by 'select_on_container_copy_construction'.  As they are the same
    // object, the equality operator will return true.
    ASSERT(X16 == COPY16); // propagate_on_container_copy_construction
    ASSERT(X18 == COPY18); // propagate_on_container_copy_construction
    ASSERT(X20 == COPY20); // propagate_on_container_copy_construction
    ASSERT(X22 == COPY22); // propagate_on_container_copy_construction
    ASSERT(X24 == COPY24); // propagate_on_container_copy_construction
    ASSERT(X26 == COPY26); // propagate_on_container_copy_construction
    ASSERT(X28 == COPY28); // propagate_on_container_copy_construction
    ASSERT(X30 == COPY30); // propagate_on_container_copy_construction

    // For the following cases, 'propogate_on_container_copy' is false, so a
    // copy of the allocator (obtained using the copy constructor) is returned
    // by 'select_on_container_copy_construction'.  As they are the same
    // object, the equality operator will return true.  Note, however that, as
    // the 'is_always_equal' trait is true, the equality operator would have
    // returned true regardless of whether the objects were the same.
    ASSERT(X17 == COPY17); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X19 == COPY19); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X21 == COPY21); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X23 == COPY23); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X25 == COPY25); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X27 == COPY27); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X29 == COPY29); // propagate_on_container_copy_construction
                           // && is_always_equal
    ASSERT(X31 == COPY31); // propagate_on_container_copy_construction
                           // && is_always_equal

    ASSERT(&da                 == COPY0.allocator());
    ASSERT(&da                 == COPY1.allocator());
    ASSERT(&da                 == COPY2.allocator());
    ASSERT(&da                 == COPY3.allocator());
    ASSERT(&da                 == COPY4.allocator());
    ASSERT(&da                 == COPY5.allocator());
    ASSERT(&da                 == COPY6.allocator());
    ASSERT(&da                 == COPY7.allocator());
    ASSERT(&da                 == COPY8.allocator());
    ASSERT(&da                 == COPY9.allocator());
    ASSERT(&da                 == COPY10.allocator());
    ASSERT(&da                 == COPY11.allocator());
    ASSERT(&da                 == COPY12.allocator());
    ASSERT(&da                 == COPY13.allocator());
    ASSERT(&da                 == COPY14.allocator());
    ASSERT(&da                 == COPY15.allocator());
    ASSERT(X16.allocator() == COPY16.allocator());
    ASSERT(X17.allocator() == COPY17.allocator());
    ASSERT(X18.allocator() == COPY18.allocator());
    ASSERT(X19.allocator() == COPY19.allocator());
    ASSERT(X20.allocator() == COPY20.allocator());
    ASSERT(X21.allocator() == COPY21.allocator());
    ASSERT(X22.allocator() == COPY22.allocator());
    ASSERT(X23.allocator() == COPY23.allocator());
    ASSERT(X24.allocator() == COPY24.allocator());
    ASSERT(X25.allocator() == COPY25.allocator());
    ASSERT(X26.allocator() == COPY26.allocator());
    ASSERT(X27.allocator() == COPY27.allocator());
    ASSERT(X28.allocator() == COPY28.allocator());
    ASSERT(X29.allocator() == COPY29.allocator());
    ASSERT(X30.allocator() == COPY30.allocator());
    ASSERT(X31.allocator() == COPY31.allocator());
}

template <class VALUE>
void TestDriver<VALUE>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING 'allocate' AND 'deallocate'
    //
    // Concerns:
    //: 1 The 'allocate' method forwards allocation requests to the appropriate
    //:   delegate allocator.
    //:
    //: 2 The 'deallocate' method forwards the deallocation requests to the
    //:   appropriate delegate allocator.
    //:
    //: 3 The 'allocate' method returns the address that was returned by the
    //:   allocation request to the appropriate delegate allocator.
    //:
    //: 4 The 'allocate' method allocates memory enough for storing requested
    //:   number of elements.
    //:
    //: 5 The allocation request for zero elements has no effect on memory
    //:   allocation and null pointer is returned.
    //:
    //: 6 The 'deallocate' method ignores 'numElements' parameter.
    //
    // Plan:
    //: 1 Create a 'bslma::Allocator' object and install it as the delegate
    //:   allocator for 'StdStatefulAllocator'.
    //:
    //: 2 Create a new 'StdStatefulAllocator' object and invoke the 'allocate'
    //:   method.  Verify that the correct amount of memory has been allocated
    //:   from the delegate allocator and address of allocated memory has been
    //:   returned.  (C-1, 3..5)
    //:
    //: 3 Invoke the 'deallocate' method and verify that the correct amount of
    //:   memory has been deallocated from the delegate allocator. (C-2, 6)
    //
    // Testing:
    //   pointer allocate(size_type numElements, const void *hint = 0);
    //   void deallocate(pointer address, size_type numElements = 1);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    Obj mX(&oa);

    if (verbose) {
        printf("\tAllocating and deallocating zero bytes.\n");
    }
    {
        VALUE *ptr = mX.allocate(0);

        ASSERTV(ptr,                0 == ptr);
        ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), 0 == oa.numBytesTotal());

        mX.deallocate(ptr, 0);

        ASSERTV(ptr,                0 == ptr);
        ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), 0 == oa.numBytesTotal());
    }

    if (verbose) {
        printf("\tAllocating and deallocating non-zero number of bytes.\n");
    }
    {
        const size_t        ALLOCATIONS_NUM = 5;
        bsls::Types::Int64  numBytesInUse = 0;
        VALUE              *ptrs[ALLOCATIONS_NUM];

        for (size_t i = 1; i <= ALLOCATIONS_NUM; ++i) {
            ptrs[i-1] = mX.allocate(i);

            size_t numBytesAllocated = i * sizeof(VALUE);
            numBytesInUse += numBytesAllocated;

            if (veryVerbose) {
                T_ T_ P(numBytesAllocated);
            }

            ASSERTV(oa.lastAllocatedAddress(),  ptrs[i-1],
                    oa.lastAllocatedAddress() == ptrs[i-1]);
            ASSERTV(numBytesAllocated, oa.lastAllocatedNumBytes(),
                    numBytesAllocated == oa.lastAllocatedNumBytes());
            ASSERTV(oa.numBytesInUse(), numBytesInUse,
                    numBytesInUse == oa.numBytesInUse());
        }

        for (size_t i = 1; i <= ALLOCATIONS_NUM; ++i) {
            mX.deallocate(ptrs[i-1], i);

            size_t numBytesDeallocated = i * sizeof(VALUE);
            numBytesInUse -= numBytesDeallocated;

            if (veryVerbose) {
                T_ T_ P(numBytesDeallocated);
            }

            ASSERTV(oa.lastDeallocatedAddress(),  ptrs[i-1],
                    oa.lastDeallocatedAddress() == ptrs[i-1]);
            ASSERTV(numBytesDeallocated, oa.lastDeallocatedNumBytes(),
                    numBytesDeallocated == oa.lastDeallocatedNumBytes());
            ASSERTV(oa.numBytesInUse(), numBytesInUse,
                    numBytesInUse == oa.numBytesInUse());
        }
    }

    ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());

    if (verbose) {
        printf("\tVerifying that 'numElements' parameter is ignored.\n");
    }
    {
        const size_t  ALLOCATIONS_NUM = 5;

        for (size_t i = 1; i <= ALLOCATIONS_NUM; ++i) {
            VALUE *ptr = mX.allocate(i);
            mX.deallocate(ptr, ALLOCATIONS_NUM - i + 1);

            size_t numBytesDeallocated = i * sizeof(VALUE);

            if (veryVerbose) {
                T_ T_ P(i);
            }

            ASSERTV(oa.lastDeallocatedAddress(),  ptr,
                    oa.lastDeallocatedAddress() == ptr);
            ASSERTV(numBytesDeallocated, oa.lastDeallocatedNumBytes(),
                    numBytesDeallocated == oa.lastDeallocatedNumBytes());
        }
    }

    ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
}

template <class VALUE>
void TestDriver<VALUE>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR
    //
    // Concerns:
    //: 1 The signature and return type are standard.
    //:
    //: 2 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 3 The reference returned is to the target object (i.e., '*this').
    //:
    //: 4 The value of the source object is not modified.
    //:
    //: 5 Assigning an object to itself behaves as expected (alias-safety).
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-1)
    //:
    //: 2 Create two 'StdStatefulAllocator' objects having the same value
    //:   (source and control ones)
    //:
    //: 3 Use a const reference of the source object to assign it to itself and
    //:   verify that it's value hasn't been changed.  (C-5)
    //:
    //: 4 Create a 'StdStatefulAllocator' object (target), having different
    //:   value than the origin one.
    //:
    //: 5 Use a const reference of the source object to assign it to the target
    //:   object.
    //:
    //: 6 Verify that the address of the return value is the same as that of
    //:   target object.  (C-3)
    //:
    //: 7 Use equality-comparison operator to verify, that the target object
    //:   has the same value as the source one.  (C-2)
    //:
    //: 8 Use equality-comparison operator to verify that source object remains
    //:   unchanged.  (C-4)
    //
    // Testing:
    //   StdStatefulAllocator& operator=(const StdStatefulAllocator& rhs);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose)
        printf("\tTesting signatures.\n");
    {
        typedef Obj& (Obj::*operatorPtr)(const Obj&);

        // Verify that the signature and return type are standard.

        operatorPtr operatorAssignment = &Obj::operator=;

        (void) operatorAssignment;  // quash potential compiler warning
    }

    if (verbose)
        printf("\tTesting behavior.\n");
    {
        bslma::TestAllocator allocX("X", veryVeryVeryVerbose);
        bslma::TestAllocator allocY("Y", veryVeryVeryVerbose);

        Obj mX(&allocX); const Obj& X = mX;
        Obj mZ(&allocX); const Obj& Z = mZ;

        // Self-assignment.

        ASSERT(Z == X);

        mX = X;

        ASSERT(Z == X);

        // Another object assignment.

        Obj mY(&allocY); const Obj& Y = mY;

        ASSERT(X != Y);

        Obj *mR = &(mY = X);

        ASSERTV(mR, &mX, mR == &mY);
        ASSERT (         X  == Y);
        ASSERT (         Z  == X);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase7()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTORS
    //
    // Concerns:
    //: 1 A 'StdStatefulAllocator' object can be copy constructed from a const
    //:   reference of another object of the same type.
    //:
    //: 2 A 'StdStatefulAllocator' object can be copy constructed from a const
    //:   reference of another object of the 'StdStatefulAllocator' template
    //:   instance parameterized on a different type.
    //:
    //: 3 The value of the original object is unchanged.
    //
    // Plan:
    //: 1 Create two 'StdStatefulAllocator' objects having the same value
    //:   (source and control ones).  Use a const reference of the source
    //:   object to copy construct an object of the same type.
    //:
    //: 2 Use equality-comparison operator to verify, that created object has
    //:   the same value as the source one.  (C-1)
    //:
    //: 3 Use equality-comparison operator to verify that source object remains
    //:   unchanged.
    //:
    //: 4 Create two 'StdStatefulAllocator' objects having the same value
    //:   (source and control ones).  Use a const reference of the source
    //:   object to copy construct an object of the 'StdStatefulAllocator'
    //:   template instant parameterized on a different type.
    //:
    //: 5 Use basic accessor to verify, that created object has the same value
    //:   as the source one.  (C-2)
    //:
    //: 6 Use equality-comparison operator to verify that source object remains
    //:   unchanged.  (C-3)
    //
    // Testing:
    //   StdStatefulAllocator(const StdStatefulAllocator& original);
    //   StdStatefulAllocator(const StdStatefulAllocator<OTHER>&);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    bslma::TestAllocator allocX("X", veryVeryVeryVerbose);

    if (verbose)
        printf("\tTesting construction of object of the same type.\n");
    {
        Obj mX(&allocX); const Obj& X = mX;
        Obj mZ(&allocX); const Obj& Z = mZ;

        Obj mY(X); const Obj& Y = mY;
        ASSERT(X == Y);
        ASSERT(Z == X);
    }

    if (verbose)
        printf("\tTesting construction of object of different type.\n");
    {
        Obj mX(&allocX); const Obj& X = mX;
        Obj mZ(&allocX); const Obj& Z = mZ;

        ObjI mY1(X); const ObjI& Y1 = mY1;
        ASSERT(X.allocator() == Y1.allocator());
        ASSERT(Z == X);

        ObjF mY2(X); const ObjF& Y2 = mY2;
        ASSERT(X.allocator() == Y2.allocator());
        ASSERT(Z == X);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase6()
{
    // ------------------------------------------------------------------------
    // EQUALITY-COMPARISON OPERATORS
    //   Ensure that '==' and '!=' are the operational definition of value.
    //
    // Concerns:
    //: 1 The equality operator's signature and return type are standard.
    //:
    //: 2 The inequality operator's signature and return type are standard.
    //:
    //: 3 Two objects, 'X' and 'Y', compare equal if and only if their
    //:   underlying allocator instances are equal.
    //:
    //: 4 'true == (X == X)'  (i.e., identity)
    //:
    //: 5 'false == (X != X)'  (i.e., identity)
    //:
    //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
    //:
    //: 8 'X != Y' if and only if '!(X == Y)'
    //:
    //: 9 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //:10 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality-comparison
    //:   operators defined in this component.  (C-1..2)
    //:
    //: 2 Create a few 'StdStatefulAllocator' objects of different template
    //:   instances and verify the commutativity properties and the expected
    //:   return values for both '==' and '!='.  (C-3..10)
    //
    // Testing:
    //   bool operator==(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
    //   bool operator!=(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose)
        printf("\tTesting signatures.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        // quash potential compiler warnings

        (void) operatorEq;
        (void) operatorNe;
    }

    if (verbose)
        printf("\tTesting behavior.\n");
    {
        bslma::TestAllocator allocX("X", veryVeryVeryVerbose);
        bslma::TestAllocator allocY("Y", veryVeryVeryVerbose);

        const Obj X1(&allocX);
        const Obj X2(&allocX);
        const Obj Y1(&allocY);

        ASSERT(  X1 == X1 );
        ASSERT(  X1 == X2 );
        ASSERT(!(X1 == Y1));

        ASSERT(  X2 == X1 );
        ASSERT(  X2 == X2 );
        ASSERT(!(X2 == Y1));

        ASSERT(!(Y1 == X1));
        ASSERT(!(Y1 == X2));
        ASSERT(  Y1 == Y1 );

        ASSERT(!(X1 != X1));
        ASSERT(!(X1 != X2));
        ASSERT(  X1 != Y1 );

        ASSERT(!(X2 != X1));
        ASSERT(!(X2 != X2));
        ASSERT(  X2 != Y1 );

        ASSERT(  Y1 != X1 );
        ASSERT(  Y1 != X2 );
        ASSERT(!(Y1 != Y1));
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor properly interprets object state.
    //
    // Concerns:
    //: 1 The 'allocator' accessor returns the value of the corresponding
    //:   attribute of the object.
    //:
    //: 2 The 'allocator' accessor method is declared 'const'.
    //
    // Plan:
    //: 1 Use the value constructor, create an object having the expected
    //:   attribute values.  Verify that the accessor for the 'allocator'
    //:   attribute invoked on a reference providing non-modifiable access to
    //:   the object return the expected value.  (C-1..2)
    //
    // Testing:
    //   bslma::TestAllocator *allocator() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    bslma::TestAllocator allocX("X", veryVeryVeryVerbose);
    const Obj            X(&allocX);
    bslma::Allocator&    allocY = bslma::NewDeleteAllocator::singleton();
    const Obj            Y(&allocY);

    ASSERTV(&allocX, X.allocator(), &allocX == X.allocator());
    ASSERTV(&allocY, Y.allocator(), &allocY == Y.allocator());
}

template <class VALUE>
void TestDriver<VALUE>::testCase2()
{
    // ------------------------------------------------------------------------
    // PRIMARY MANIPULATORS
    //
    // Concerns:
    //: 1 The constructor sets the bslma::TestAllocator to which object
    //:   delegates.
    //:
    //: 2 Object destruction has no effect on the underlying
    //:   bslma::TestAllocator.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create three 'bslma::TestAllocator' objects, and install one as the
    //:   current default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 2 Use the value constructor to dynamically create an object using
    //:   second allocator in P-1 as a parameter; use third allocator in P-1
    //:   for the object's footprint.
    //:
    //: 3 Verify that underlying allocator is set correctly.  (C-1)
    //:
    //: 4 Destroy the object and verify that underlying allocator remains
    //:   unharmed.  (C-2)
    //:
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid values, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-3)
    //
    // Testing:
    //   StdStatefulAllocator(bslma::TestAllocator *);
    //   ~StdStatefulAllocator();
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    if (verbose)
        printf("\tTesting behavior\n");
    {
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",    veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj *objPtr;
        objPtr = new (fa) Obj(&oa);

        Obj&       mX = *objPtr;
        const Obj& X = mX;

        ASSERT(sizeof(Obj) == fa.numBytesInUse());
        ASSERT(0           == da.numBytesInUse());
        ASSERT(&oa         == X.allocator());

        fa.deleteObject(objPtr);

        ASSERT(0            == fa.numBytesInUse());
        ASSERT(0            == oa.numBytesInUse());
    }

    if (verbose)
        printf("\tNegative Testing\n");
    {
        bsls::AssertTestHandlerGuard hG;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        ASSERT_SAFE_PASS((Obj(&oa)));
        ASSERT_SAFE_PASS((Obj(0)));
        ASSERT_SAFE_PASS((Obj()));
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase1()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive testing
    //:   in subsequent test cases.
    //
    // Plan:
    //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nVALUE: %s\n", NameOf<VALUE>().name());

    bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
    StdStatefulAllocator<VALUE>  sa(&oa);
    VALUE                       *ptr = sa.allocate(2);

    ASSERT(oa.numBytesInUse() == 2*sizeof(VALUE));
    sa.deallocate(ptr, 2);
    ASSERT(oa.numBytesInUse() == 0);
}
//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing The Support for STL-Compliant Allocator
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will verify that a type supports the use of a
// STL-compliant allocator.
//
// First we define a simple container type intended to be used with a C++11
// standard compliant allocator:
//..
    template <class TYPE, class ALLOCATOR>
    class MyContainer {
        // This container type is parameterized on a standard allocator type
        // and contains a single object, always initialized, which can be
        // replaced and accessed.

        // DATA MEMBERS
        ALLOCATOR  d_allocator;  // allocator used to supply memory (held, not
                                 // owned)

        TYPE      *d_object_p;   // pointer to the contained object

      public:
        // CREATORS
        MyContainer(const TYPE& object, const ALLOCATOR& allocator);
            // Create an container containing the specified 'object', using the
            // specified 'allocator' to supply memory.

        ~MyContainer();
            // Destroy this container.

        // MANIPULATORS
        TYPE& object();
            // Return a reference providing modifiable access to the object
            // contained in this container.

        // ACCESSORS
        const TYPE& object() const;
            // Return a reference providing non-modifiable access to the object
            // contained in this container.
    };
//..
// Then, we define the member functions of 'MyContainer':
//..
    // CREATORS
    template <class TYPE, class ALLOCATOR>
    MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE&      object,
                                              const ALLOCATOR& allocator)
    : d_allocator(allocator)
    {
        d_object_p = d_allocator.allocate(1);
        new (static_cast<void *>(d_object_p)) TYPE(object);
    }

    template <class TYPE, class ALLOCATOR>
    MyContainer<TYPE, ALLOCATOR>::~MyContainer()
    {
        d_object_p->~TYPE();
        d_allocator.deallocate(d_object_p, 1);
    }

    // MANIPULATORS
    template <class TYPE, class ALLOCATOR>
    TYPE& MyContainer<TYPE, ALLOCATOR>::object()
    {
        return *d_object_p;
    }

    // ACCESSORS
    template <class TYPE, class ALLOCATOR>
    const TYPE& MyContainer<TYPE, ALLOCATOR>::object() const
    {
        return *d_object_p;
    }
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;

                 verbose = argc > 2;
             veryVerbose = argc > 3;
         veryVeryVerbose = argc > 4;
     veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

//..
// Now, we use 'bsltf::StdStatefulAllocator' to implement a simple test for
// 'MyContainer' to verify it correctly uses a parameterized allocator using
// only the C++11 standard methods:
//..
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    {
        typedef MyContainer<int, bsltf::StdStatefulAllocator<int> > Obj;

        Obj        mX(2, bsltf::StdStatefulAllocator<int>(&oa));
        const Obj& X = mX;
        ASSERT(sizeof(int) == oa.numBytesInUse());

        ASSERT(X.object() == 2);

        mX.object() = -10;
        ASSERT(X.object() == -10);
    }

    ASSERT(0 == oa.numBytesInUse());
//..
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'max_size'
        // --------------------------------------------------------------------

#if !defined(BSLSTL_ALLOCATOR_TRAITS_SUPPORTS_ALL_CPP11_DEDUCTIONS)
        if (verbose) printf("\nTESTING 'max_size'"
                            "\n==================\n");

        RUN_EACH_TYPE(TestDriver, testCase16, TEST_TYPES_REGULAR);
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'construct'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'construct'"
                            "\n===================\n");

        RUN_EACH_TYPE(TestDriver, testCase15, REDUCED_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PROPAGATION TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nPROPAGATION TRAITS"
                            "\n==================\n");

        RUN_EACH_TYPE(TestDriver, testCase14, TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // SPURIOUS NESTED TYPES
        // --------------------------------------------------------------------

        if (verbose) printf("\nSPURIOUS NESTED TYPES"
                            "\n=====================\n");

        RUN_EACH_TYPE(TestDriver, testCase13, TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'select_on_container_copy_construction'
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "\nTESTING 'select_on_container_copy_construction'"
                        "\n===============================================\n");

        RUN_EACH_TYPE(TestDriver, testCase12, TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'allocate' AND 'deallocate'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'allocate' AND 'deallocate'"
                            "\n===================================\n");

        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");
        RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");

        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing the test machinery
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES_REGULAR);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        RUN_EACH_TYPE(TestDriver, testCase1, TEST_TYPES_REGULAR);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
