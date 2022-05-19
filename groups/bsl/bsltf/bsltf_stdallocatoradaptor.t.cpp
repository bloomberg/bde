// bsltf_stdallocatoradaptor.t.cpp                                    -*-C++-*-
#include <bsltf_stdallocatoradaptor.h>

#include <bsltf_alloctesttype.h>
#include <bsltf_emplacabletesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_stdalloctesttype.h>
#include <bsltf_stdtestallocator.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_stdallocator.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

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
// The component under test is a value-semantic allocator adapter whose state
// is represented by the underlying allocator it is derived from.  As
// 'bsltf::StdAllocatorAdaptor' mostly delegates operations to the base class
// object, we usually need to verify that expected methods of associated
// allocator are invoked and the arguments passed are of the correct types and
// values.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 8] size_type
// [ 8] difference_type
// [ 8] pointer
// [ 8] const_pointer
// [ 8] reference
// [ 8] const_reference
// [ 8] value_type
// [ 8] rebind<U>::other
//
// CREATORS
// [ 2] StdAllocatorAdaptor();
// [ 2] StdAllocatorAdaptor(const ALLOCATOR& basicAllocator);
// [ 2] StdAllocatorAdaptor(const StdAllocatorAdaptor<ANY_TYPE>& other);
// [ 2] StdAllocatorAdaptor(const StdAllocatorAdaptor& origin) = default;
// [ 2] ~StdAllocatorAdaptor() = default;
//
// MANIPULATORS
// [ 6] AllocatorAdaptor& operator=(const AllocatorAdaptor& rh) = default;
// [ 4] void construct(ELEMENT_TYPE *address, Args&&... arguments);
//
// ACCESSORS
// [ 3] const ALLOCATOR& allocator() const;
// [ 7] StdAllocatorAdaptor select_on_container_copy_construction() const;
//
// FREE OPERATORS
// [ 5] bool operator==(const StdAllocatorAdaptor<TYPE1>& lhs, rhs);
// [ 5] bool operator!=(const StdAllocatorAdaptor<TYPE1>& lhs, rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE

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

//=============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsl::allocator<int>                 ObjType;
typedef bsl::allocator<float>               AnotherObjType;
typedef StdAllocatorAdaptor<ObjType>        Obj;
typedef StdAllocatorAdaptor<AnotherObjType> AnotherObj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                         // =======================
                         // class ConstructTestType
                         // =======================
template <class ALLOC = bsl::allocator<EmplacableTestType> >
class ConstructTestType {
    // This class provides a test type that is used to check that the arguments
    // passed for constructing an object of this type are of the correct types
    // and values.

  private:
    // TYPES
    typedef bsl::allocator_traits<ALLOC> AllocatorTraits;

    // DATA
    EmplacableTestType *d_value_p;    // pointer to value (owned)
    ALLOC               d_allocator;  // allocator

  public:
#ifndef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    ConstructTestType,
                    bslma::UsesBslmaAllocator,
                    (bsl::is_convertible<bslma::Allocator *, ALLOC>::value));
#endif

    // PUBLIC TYPES
    typedef ALLOC allocator_type;

    typedef EmplacableTestType::ArgType01 ArgType01;
    typedef EmplacableTestType::ArgType02 ArgType02;
    typedef EmplacableTestType::ArgType03 ArgType03;
    typedef EmplacableTestType::ArgType04 ArgType04;
    typedef EmplacableTestType::ArgType05 ArgType05;
    typedef EmplacableTestType::ArgType06 ArgType06;
    typedef EmplacableTestType::ArgType07 ArgType07;
    typedef EmplacableTestType::ArgType08 ArgType08;
    typedef EmplacableTestType::ArgType09 ArgType09;
    typedef EmplacableTestType::ArgType10 ArgType10;


    // CREATORS
    ConstructTestType(const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01 arg01, const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      ArgType05    arg05,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      ArgType05    arg05,
                      ArgType06    arg06,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      ArgType05    arg05,
                      ArgType06    arg06,
                      ArgType07    arg07,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      ArgType05    arg05,
                      ArgType06    arg06,
                      ArgType07    arg07,
                      ArgType08    arg08,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      ArgType05    arg05,
                      ArgType06    arg06,
                      ArgType07    arg07,
                      ArgType08    arg08,
                      ArgType09    arg09,
                      const ALLOC& allocator = ALLOC());
    ConstructTestType(ArgType01    arg01,
                      ArgType02    arg02,
                      ArgType03    arg03,
                      ArgType04    arg04,
                      ArgType05    arg05,
                      ArgType06    arg06,
                      ArgType07    arg07,
                      ArgType08    arg08,
                      ArgType09    arg09,
                      ArgType10    arg10,
                      const ALLOC& allocator = ALLOC());
        // Create an 'ConstructTestType' by initializing 'data' attribute with
        // the specified 'arg01'..'arg10'.  Optionally specify the 'allocator'
        // to initialize object allocator.  If 'allocator' is not supplied,
        // default-constructed object of the (template parameter) type 'ALLOC'
        // is used.

    ~ConstructTestType();
        // Destroy this object.

    // ACCESSORS
    const ArgType01& arg01() const;
    const ArgType02& arg02() const;
    const ArgType03& arg03() const;
    const ArgType04& arg04() const;
    const ArgType05& arg05() const;
    const ArgType06& arg06() const;
    const ArgType07& arg07() const;
    const ArgType08& arg08() const;
    const ArgType09& arg09() const;
    const ArgType10& arg10() const;
        // Return the value of the correspondingly numbered argument that was
        // passed to the constructor of this object.
};

                        // -----------------------
                        // class ConstructTestType
                        // -----------------------
    // CREATORS
template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator, d_value_p);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator, d_value_p, arg01);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator, d_value_p, arg01, arg02);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator, d_value_p, arg01, arg02, arg03);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            ArgType05    arg05,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04,
                               arg05);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            ArgType05    arg05,
                                            ArgType06    arg06,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04,
                               arg05,
                               arg06);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            ArgType05    arg05,
                                            ArgType06    arg06,
                                            ArgType07    arg07,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04,
                               arg05,
                               arg06,
                               arg07);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            ArgType05    arg05,
                                            ArgType06    arg06,
                                            ArgType07    arg07,
                                            ArgType08    arg08,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04,
                               arg05,
                               arg06,
                               arg07,
                               arg08);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            ArgType05    arg05,
                                            ArgType06    arg06,
                                            ArgType07    arg07,
                                            ArgType08    arg08,
                                            ArgType09    arg09,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04,
                               arg05,
                               arg06,
                               arg07,
                               arg08,
                               arg09);
}

template <class ALLOC>
ConstructTestType<ALLOC>::ConstructTestType(ArgType01    arg01,
                                            ArgType02    arg02,
                                            ArgType03    arg03,
                                            ArgType04    arg04,
                                            ArgType05    arg05,
                                            ArgType06    arg06,
                                            ArgType07    arg07,
                                            ArgType08    arg08,
                                            ArgType09    arg09,
                                            ArgType10    arg10,
                                            const ALLOC& allocator)
: d_allocator(allocator)
{
    d_value_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(d_allocator,
                               d_value_p,
                               arg01,
                               arg02,
                               arg03,
                               arg04,
                               arg05,
                               arg06,
                               arg07,
                               arg08,
                               arg09,
                               arg10);
}

template <class ALLOC>
ConstructTestType<ALLOC>::~ConstructTestType()
{
    AllocatorTraits::destroy(d_allocator, d_value_p);
    AllocatorTraits::deallocate(d_allocator,d_value_p, 1);
}

// ACCESSORS
template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType01&
ConstructTestType<ALLOC>::arg01() const
{
    return d_value_p->arg01();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType02&
ConstructTestType<ALLOC>::arg02() const
{
    return d_value_p->arg02();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType03&
ConstructTestType<ALLOC>::arg03() const
{
    return d_value_p->arg03();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType04&
ConstructTestType<ALLOC>::arg04() const
{
    return d_value_p->arg04();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType05&
ConstructTestType<ALLOC>::arg05() const
{
    return d_value_p->arg05();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType06&
ConstructTestType<ALLOC>::arg06() const
{
    return d_value_p->arg06();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType07&
ConstructTestType<ALLOC>::arg07() const
{
    return d_value_p->arg07();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType08&
ConstructTestType<ALLOC>::arg08() const
{
    return d_value_p->arg08();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType09&
ConstructTestType<ALLOC>::arg09() const
{
    return d_value_p->arg09();
}

template <class ALLOC>
const typename ConstructTestType<ALLOC>::ArgType10&
ConstructTestType<ALLOC>::arg10() const
{
    return d_value_p->arg10();
}

template <class T>
bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
{
    return bslmf::MovableRefUtil::move(t);
}

template <class T>
const T&             testArg(T& t, bsl::false_type)
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
void testCase4_RunTest()
    // Test forwarding of arguments in 'construct' method and 'construct'
    // method itself.
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

    typedef EmplacableTestType                   OBJ_VALUE_TYPE;
    typedef bsl::allocator<OBJ_VALUE_TYPE>       ADAPTOR_ALLOCATOR;

    typedef StdTestAllocator<OBJ_VALUE_TYPE>     FIRST_ALLOCATOR;
    typedef ConstructTestType<FIRST_ALLOCATOR>   FIRST_OBJ_TYPE;
    typedef bsl::allocator<FIRST_OBJ_TYPE>       FIRST_OBJ_ALLOCATOR;

    typedef ADAPTOR_ALLOCATOR                    SECOND_ALLOCATOR;
    typedef ConstructTestType<SECOND_ALLOCATOR>  SECOND_OBJ_TYPE;
    typedef bsl::allocator<SECOND_OBJ_TYPE>      SECOND_OBJ_ALLOCATOR;

    static const FIRST_OBJ_TYPE::ArgType01 V01(1);
    static const FIRST_OBJ_TYPE::ArgType02 V02(2);
    static const FIRST_OBJ_TYPE::ArgType03 V03(3);
    static const FIRST_OBJ_TYPE::ArgType04 V04(4);
    static const FIRST_OBJ_TYPE::ArgType05 V05(5);
    static const FIRST_OBJ_TYPE::ArgType06 V06(6);
    static const FIRST_OBJ_TYPE::ArgType07 V07(7);
    static const FIRST_OBJ_TYPE::ArgType08 V08(8);
    static const FIRST_OBJ_TYPE::ArgType09 V09(9);
    static const FIRST_OBJ_TYPE::ArgType10 V10(10);

    FIRST_OBJ_TYPE::ArgType01 A01(V01), B01(V01);
    FIRST_OBJ_TYPE::ArgType02 A02(V02), B02(V02);
    FIRST_OBJ_TYPE::ArgType03 A03(V03), B03(V03);
    FIRST_OBJ_TYPE::ArgType04 A04(V04), B04(V04);
    FIRST_OBJ_TYPE::ArgType05 A05(V05), B05(V05);
    FIRST_OBJ_TYPE::ArgType06 A06(V06), B06(V06);
    FIRST_OBJ_TYPE::ArgType07 A07(V07), B07(V07);
    FIRST_OBJ_TYPE::ArgType08 A08(V08), B08(V08);
    FIRST_OBJ_TYPE::ArgType09 A09(V09), B09(V09);
    FIRST_OBJ_TYPE::ArgType10 A10(V10), B10(V10);

    FIRST_OBJ_ALLOCATOR   objAllocator1;
    SECOND_OBJ_ALLOCATOR  objAllocator2;
    FIRST_OBJ_TYPE       *obj1;
    SECOND_OBJ_TYPE      *obj2;

    obj1 = bsl::allocator_traits<FIRST_OBJ_ALLOCATOR>::allocate(objAllocator1,
                                                                1);
    obj2 = bsl::allocator_traits<SECOND_OBJ_ALLOCATOR>::allocate(objAllocator2,
                                                                 1);
    bslma::TestAllocator ta1;
    bslma::TestAllocator ta2;

    ADAPTOR_ALLOCATOR adaptorAllocator1(&ta1);
    ADAPTOR_ALLOCATOR adaptorAllocator2(&ta2);

    StdAllocatorAdaptor<ADAPTOR_ALLOCATOR> adaptor1(adaptorAllocator1);
    StdAllocatorAdaptor<ADAPTOR_ALLOCATOR> adaptor2(adaptorAllocator2);

    ASSERTV(N_ARGS, 0 == ta1.numBytesInUse());
    ASSERTV(N_ARGS, 0 == ta2.numBytesInUse());

    switch (N_ARGS) {
      case 0: {
        adaptor1.construct(obj1);
        adaptor2.construct(obj2);
      } break;
      case 1: {
        adaptor1.construct(obj1, testArg(A01, MOVE_01));
        adaptor2.construct(obj2, testArg(B01, MOVE_01));
      } break;
      case 2: {
        adaptor1.construct(obj1, testArg(A01, MOVE_01), testArg(A02, MOVE_02));
        adaptor2.construct(obj2, testArg(B01, MOVE_01), testArg(B02, MOVE_02));
      } break;
      case 3: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03));
        adaptor2.construct(obj2,
                            testArg(B01, MOVE_01),
                            testArg(B02, MOVE_02),
                            testArg(B03, MOVE_03));
      } break;
      case 4: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03),
                           testArg(A04, MOVE_04));
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04));
      } break;
      case 5: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03),
                           testArg(A04, MOVE_04),
                           testArg(A05, MOVE_05));
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04),
                           testArg(B05, MOVE_05));
      } break;
      case 6: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03),
                           testArg(A04, MOVE_04),
                           testArg(A05, MOVE_05),
                           testArg(A06, MOVE_06));
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04),
                           testArg(B05, MOVE_05),
                           testArg(B06, MOVE_06));
      } break;
      case 7: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03),
                           testArg(A04, MOVE_04),
                           testArg(A05, MOVE_05),
                           testArg(A06, MOVE_06),
                           testArg(A07, MOVE_07));
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04),
                           testArg(B05, MOVE_05),
                           testArg(B06, MOVE_06),
                           testArg(B07, MOVE_07));
      } break;
      case 8: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03),
                           testArg(A04, MOVE_04),
                           testArg(A05, MOVE_05),
                           testArg(A06, MOVE_06),
                           testArg(A07, MOVE_07),
                           testArg(A08, MOVE_08));
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04),
                           testArg(B05, MOVE_05),
                           testArg(B06, MOVE_06),
                           testArg(B07, MOVE_07),
                           testArg(B08, MOVE_08));
      } break;
      case 9: {
        adaptor1.construct(obj1,
                           testArg(A01, MOVE_01),
                           testArg(A02, MOVE_02),
                           testArg(A03, MOVE_03),
                           testArg(A04, MOVE_04),
                           testArg(A05, MOVE_05),
                           testArg(A06, MOVE_06),
                           testArg(A07, MOVE_07),
                           testArg(A08, MOVE_08),
                           testArg(A09, MOVE_09));
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04),
                           testArg(B05, MOVE_05),
                           testArg(B06, MOVE_06),
                           testArg(B07, MOVE_07),
                           testArg(B08, MOVE_08),
                           testArg(B09, MOVE_09));
      } break;
      case 10: {
        adaptor1.construct(obj1,
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
        adaptor2.construct(obj2,
                           testArg(B01, MOVE_01),
                           testArg(B02, MOVE_02),
                           testArg(B03, MOVE_03),
                           testArg(B04, MOVE_04),
                           testArg(B05, MOVE_05),
                           testArg(B06, MOVE_06),
                           testArg(B07, MOVE_07),
                           testArg(B08, MOVE_08),
                           testArg(B09, MOVE_09),
                           testArg(B10, MOVE_10));
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Invalid # of args!");
      } break;
    }

    const FIRST_OBJ_TYPE&  OBJ1 = *obj1;
    const SECOND_OBJ_TYPE& OBJ2 = *obj2;

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

    ASSERTV(MOVE_01, B01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == B01.movedFrom()));
    ASSERTV(MOVE_02, B02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == B02.movedFrom()));
    ASSERTV(MOVE_03, B03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == B03.movedFrom()));
    ASSERTV(MOVE_04, B04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == B04.movedFrom()));
    ASSERTV(MOVE_05, B05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == B05.movedFrom()));
    ASSERTV(MOVE_06, B06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == B06.movedFrom()));
    ASSERTV(MOVE_07, B07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == B07.movedFrom()));
    ASSERTV(MOVE_08, B08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == B08.movedFrom()));
    ASSERTV(MOVE_09, B09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == B09.movedFrom()));
    ASSERTV(MOVE_10, B10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == B10.movedFrom()));

    ASSERTV(V01, OBJ1.arg01(), V01 == OBJ1.arg01() || 2 == N01);
    ASSERTV(V02, OBJ1.arg02(), V02 == OBJ1.arg02() || 2 == N02);
    ASSERTV(V03, OBJ1.arg03(), V03 == OBJ1.arg03() || 2 == N03);
    ASSERTV(V04, OBJ1.arg04(), V04 == OBJ1.arg04() || 2 == N04);
    ASSERTV(V05, OBJ1.arg05(), V05 == OBJ1.arg05() || 2 == N05);
    ASSERTV(V06, OBJ1.arg06(), V06 == OBJ1.arg06() || 2 == N06);
    ASSERTV(V07, OBJ1.arg07(), V07 == OBJ1.arg07() || 2 == N07);
    ASSERTV(V08, OBJ1.arg08(), V08 == OBJ1.arg08() || 2 == N08);
    ASSERTV(V09, OBJ1.arg09(), V09 == OBJ1.arg09() || 2 == N09);
    ASSERTV(V10, OBJ1.arg10(), V10 == OBJ1.arg10() || 2 == N10);

    ASSERTV(V01, OBJ2.arg01(), V01 == OBJ2.arg01() || 2 == N01);
    ASSERTV(V02, OBJ2.arg02(), V02 == OBJ2.arg02() || 2 == N02);
    ASSERTV(V03, OBJ2.arg03(), V03 == OBJ2.arg03() || 2 == N03);
    ASSERTV(V04, OBJ2.arg04(), V04 == OBJ2.arg04() || 2 == N04);
    ASSERTV(V05, OBJ2.arg05(), V05 == OBJ2.arg05() || 2 == N05);
    ASSERTV(V06, OBJ2.arg06(), V06 == OBJ2.arg06() || 2 == N06);
    ASSERTV(V07, OBJ2.arg07(), V07 == OBJ2.arg07() || 2 == N07);
    ASSERTV(V08, OBJ2.arg08(), V08 == OBJ2.arg08() || 2 == N08);
    ASSERTV(V09, OBJ2.arg09(), V09 == OBJ2.arg09() || 2 == N09);
    ASSERTV(V10, OBJ2.arg10(), V10 == OBJ2.arg10() || 2 == N10);

    // Verify allocator propagation.

    ASSERTV(N_ARGS, ta1.numBytesInUse(),
            0 == ta1.numBytesInUse());
    ASSERTV(N_ARGS, ta2.numBytesInUse(),
            sizeof(OBJ_VALUE_TYPE) == ta2.numBytesInUse());

    // Reclaim dynamically allocated objects.

    bsl::allocator_traits<FIRST_OBJ_ALLOCATOR>::destroy(objAllocator1,
                                                        obj1);
    bsl::allocator_traits<SECOND_OBJ_ALLOCATOR>::destroy(objAllocator2,
                                                         obj2);

    bsl::allocator_traits<FIRST_OBJ_ALLOCATOR>::deallocate(
                                                         objAllocator1,
                                                         obj1,
                                                         1);
    bsl::allocator_traits<SECOND_OBJ_ALLOCATOR>::deallocate(
                                                         objAllocator2,
                                                         obj2,
                                                         1);
}

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Allocator Propagation
/// - - - - - - - - - - - - - - - -
// 'bslma::ConstructionUtil' propagates 'bslma::Allocator', wrapped by C++
// standard style allocator, to the constructor, if type, being constructed,
// supports 'UsesBslmaAllocator' trait.  'bsltf::StdAllocatorAdaptor' is used
// in test drivers to get the same behavior for the types that do not support
// that trait.
//
// Suppose, we want to adopt a test for a component that uses 'bslma'-style
// allocation to test that this component correctly works with standard
// allocators.  For simplicity the test below constructs an object of the
// (template parameter) type 'TYPE' by calling allocator's 'construct' method.
// We want to test that allocator is correctly propagated to the object
// constructor.  First, we define the test implementation:
//..
    template<class TYPE, class ALLOC = bsl::allocator<TYPE> >
    class TestDriver
    {
      public:
        static void testCase()
        {
            bslma::TestAllocator oa("object");
            ALLOC                xoa(&oa);

            bsls::ObjectBuffer<TYPE> buffer;

            xoa.construct(buffer.address(), 1);

            bslma::DestructorGuard<TYPE> guard(&buffer.object());

            const TYPE& X = buffer.object();

            ASSERT(1   == X.data());
            ASSERT(&oa == X.allocator());
        }
    };
//..
// Now, parametrize 'TestDriver' class with 'StdAllocatorAdaptor' explicitly
// to expand 'testCase' behavior for types, that don't support bslma
// allocators:
//..
    template<class TYPE>
    class StdBslmaTestDriver : public TestDriver<TYPE,
                            bsltf::StdAllocatorAdaptor<bsl::allocator<TYPE> > >
    {
    };
//..

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
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
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

// Finally, run the test for types that use 'bslma' and standard allocators:
//..
    TestDriver<AllocTestType>::testCase();
    StdBslmaTestDriver<StdAllocTestType<bsl::allocator<int> > >::testCase();
//..
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SPURIOUS NESTED TYPES
        //   There are a number of frequently encountered type aliases that are
        //   often defined by an allocator type.  As 'StdAllocatorAdaptor' is
        //   only a facade of an allocator, these aliases are only "facades"
        //   for the allocator's aliases.
        //
        // Concerns:
        //: 1 The 'typedef' aliases defined in this component are the same as
        //:   in associated allocator.
        //:
        //: 2 'rebind<BDE_OTHER_TYPE>::other' defines a template instance for
        //:   'StdAllocatorAdaptor' parameterized on the 'BDE_OTHER_TYPE' type.
        //
        // Plan:
        //: 1 Define two aliases for 'StdAllocatorAdaptor' parameterized on
        //:   'bsl::allocator<int>' and 'bsl::allocator<float>'.
        //:
        //: 2 For each alias defines in P-1:
        //:
        //:   1 For all type aliases, use 'bsl::is_same' to verify that
        //:     they are equal to the assosiated allocator's types.  (C-1)
        //:
        //:   2 Verify using 'bsl::is_same' that 'rebind<U>::other', where 'U'
        //:     is two aliases defined by P-1, defines the correct type.  (C-3)
        //
        // Testing:
        //   size_type
        //   difference_type
        //   pointer
        //   const_pointer
        //   reference
        //   const_reference
        //   value_type
        //   rebind<U>::other
        // --------------------------------------------------------------------

        if (verbose) printf("\nSPURIOUS NESTED TYPES"
                            "\n=====================\n");

        typedef bsl::allocator<int>     AI;
        typedef bsl::allocator<float>   AF;
        typedef StdAllocatorAdaptor<AI> AAI;
        typedef StdAllocatorAdaptor<AF> AAF;

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT((bsl::is_same<AI::size_type, AAI::size_type>::value));
            ASSERT((bsl::is_same<AF::size_type, AAF::size_type>::value));
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT((bsl::is_same<AI::difference_type,
                                 AAI::difference_type>::value));
            ASSERT((bsl::is_same<AF::difference_type,
                                 AAF::difference_type>::value));
        }

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::pointer, AAI::pointer>::value));
            ASSERT((bsl::is_same<AF::pointer, AAF::pointer>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer,
                                 AAI::const_pointer>::value));
            ASSERT((bsl::is_same<AF::const_pointer,
                                 AAF::const_pointer>::value));
        }

        if (verbose) printf("\tTesting 'reference'.\n");
        {
            ASSERT((bsl::is_same<AI::reference,AAI::reference>::value));
            ASSERT((bsl::is_same<AF::reference,AAF::reference>::value));
        }

        if (verbose) printf("\tTesting 'const_reference'.\n");
        {
            ASSERT((bsl::is_same<AI::const_reference,
                                 AAI::const_reference>::value));
            ASSERT((bsl::is_same<AF::const_reference,
                                 AAF::const_reference>::value));
        }

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<AI::value_type,AAI::value_type>::value));
            ASSERT((bsl::is_same<AF::value_type,AAF::value_type>::value));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AAI::rebind<int  >::other, AAI>::value));
            ASSERT((bsl::is_same<AAI::rebind<float>::other, AAF>::value));
            ASSERT((bsl::is_same<AAF::rebind<int  >::other, AAI>::value));
            ASSERT((bsl::is_same<AAF::rebind<float>::other, AAF>::value));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'select_on_container_copy_construction'
        //
        // Concerns:
        //: 1 The 'select_on_container_copy_construction' invokes the
        //:   appropriate method of allocator_traits interface and passes there
        //:   address of this object.
        //
        // Plan:
        //: 1 Create an object, adapting an allocator that supports
        //:   'select_on_container_copy_construction' method.
        //:
        //: 2 Verify, that allocator's method is called on adaptor's
        //:   'select_on_container_copy_construction' method invocation.
        //:
        //: 3 Create an object, adapting an allocator that doesn't support
        //:   'select_on_container_copy_construction' method.
        //:
        //: 4 Verify, that this object is returned from
        //:   'select_on_container_copy_construction' method of
        //:   allocator_traits interface.
        //
        // Testing:
        //   StdAllocatorAdaptor select_on_container_copy_construction() const;
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nTESTING 'select_on_container_copy_construction'"
                         "\n==============================================\n");

        typedef bsl::allocator<int>                   AllocType;
        typedef StdAllocatorAdaptor<AllocType>        AdaptorType;
        typedef bslma::StdTestAllocator<int>          AnotherAllocType;
        typedef StdAllocatorAdaptor<AnotherAllocType> AnotherAdaptorType;

        // Adapting an allocator, having
        // 'select_on_container_copy_construction' method.

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);
        AllocType            bslAllocator(&ta);
        AdaptorType          mX1(bslAllocator);
        const AdaptorType&   X1 = mX1;

        AdaptorType        mY1 = X1.select_on_container_copy_construction();
        const AdaptorType& Y1 = mY1;

        // 'bsl::allocator' returns default constructed object from
        // 'select_on_container_copy_construction' method, so it shouldn't be
        // equal to the origin one, created with value constructor.

        ASSERT(X1          != Y1);
        ASSERT(AllocType() == Y1);

        // Adapting an allocator, not having
        // 'select_on_container_copy_construction' method.

        bslma::TestAllocator      ata("another test", veryVeryVeryVerbose);
        AnotherAllocType          anotherBslAllocator(&ata);
        AnotherAdaptorType        mX2(anotherBslAllocator);
        const AnotherAdaptorType& X2 = mX2;

        AnotherAdaptorType mY2 = X2.select_on_container_copy_construction();

        // In that case the same object is returned from
        // 'select_on_container_copy_construction' method of 'allocator_traits'
        // interface, so copy of origin adaptor is received.

        ASSERT(X2 == mY2);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
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
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Use default constructor to create a source object 'X' and copy
        //:   constructor to create control object 'Z' as a copy of 'X'.
        //:
        //: 3 Use value constructor to create target object 'Y', having the
        //:   same type as  the source object, but the different value.
        //:
        //: 4 Assign 'Y' from 'X'.
        //:
        //: 5 Verify that the address of the return value is the same as
        //:   that of target object.  (C-3)
        //:
        //: 6 Use the equal-comparison operator to verify that target object
        //:   have the same values as 'X'.  (C-1,3)
        //:
        //: 7 Use the equal-comparison operator to verify that after each
        //:   assignment 'X' has the same value as 'Z'.  (C-4)
        //:
        //: 8 Assign 'X' from 'X'.
        //:
        //: 9 Use the equal-comparison operator to verify that 'X' has the
        //:   same value as 'Z'.  (C-5)
        //
        // Testing:
        //   AllocatorAdaptor& operator=(const AllocatorAdaptor& rh) = default;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose)
            printf("\tSignature and return type verification.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }

        if (verbose)
            printf("\tTesting behavior.\n");
        {
            // Source object creation.

            Obj        mX;
            const Obj& X = mX;

            // Control object creation.

            Obj        mZ(X);
            const Obj& Z = mZ;

            // Target objects creation.

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            ObjType              allocator(&ta);
            Obj                  mY(allocator);
            const Obj&           Y = mY;

            ASSERT(X  != Y);

            Obj *mR = &(mY = X);

            ASSERT(mR == &mY);
            ASSERT(X  == Y);
            ASSERT(Z  == X);
        }

        if (verbose)
            printf("\tSelf-assignment.\n");
        {
            // Source object creation.

            Obj        mX;
            const Obj& X = mX;

            // Control object creation.

            Obj        mZ;
            const Obj& Z = mZ;

            // Assignment.

            mX = X;

            ASSERT(Z == X);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   associated allocator instances are equal.
        //:
        //: 2 'true  == (X == X)'  (i.e., identity)
        //:
        //: 3 'false == (X != X)'  (i.e., identity)
        //:
        //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //
        // Plan:
        //: 1 Use value constructor to create two equal objects 'X1' and 'X2'
        //:   having the same type.
        //:
        //: 2 Use value constructor to create an object 'Y1', having another
        //:   type and equal to X1 and X2.
        //:
        //: 3 Use value constructor to create an object 'X3', having the same
        //:   type as 'X1' and 'X2', but different value.
        //:
        //: 4 Use value constructor to create an object 'Y2', having different
        //:   type than 'X1' and 'X2', but the same value.
        //:
        //: 5 Compare all objects to verify the commutativity properties and
        //:   the expected return values for both '==' and '!='.  (C-1..8)
        //
        // Testing:
        //   bool operator==(const StdAllocatorAdaptor<TYPE1>& lhs, rhs);
        //   bool operator!=(const StdAllocatorAdaptor<TYPE1>& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);
        bslma::TestAllocator ata("another", veryVeryVeryVerbose);
        ObjType              allocator(&ta);
        AnotherObjType       anotherAllocator(&ata);

        Obj               mX1(allocator);
        const Obj&        X1 = mX1;
        Obj               mX2(allocator);
        const Obj&        X2 = mX2;
        Obj               mX3(anotherAllocator);
        const Obj&        X3 = mX3;
        AnotherObj        mY1(allocator);
        const AnotherObj& Y1 = mY1;
        AnotherObj        mY2(anotherAllocator);
        const AnotherObj& Y2 = mY2;

        ASSERT(  X1 == X1);
        ASSERT(  X1 == X2);
        ASSERT(!(X1 == X3));
        ASSERT(  X1 == Y1);
        ASSERT(!(X1 == Y2));

        ASSERT(  X2 == X1);
        ASSERT(  X2 == X2);
        ASSERT(!(X2 == X3));
        ASSERT(  X2 == Y1);
        ASSERT(!(X2 == Y2));

        ASSERT(!(X3 == X1));
        ASSERT(!(X3 == X2));
        ASSERT(  X3 == X3);
        ASSERT(!(X3 == Y1));
        ASSERT(  X3 == Y2);

        ASSERT(  Y1 == X1);
        ASSERT(  Y1 == X2);
        ASSERT(!(Y1 == X3));
        ASSERT(  Y1 == Y1);
        ASSERT(!(Y1 == Y2));

        ASSERT(!(Y2 == X1));
        ASSERT(!(Y2 == X2));
        ASSERT(  Y2 == X3);
        ASSERT(!(Y2 == Y1));
        ASSERT(  Y2 == Y2);

        ASSERT(!(X1 != X1));
        ASSERT(!(X1 != X2));
        ASSERT(  X1 != X3);
        ASSERT(!(X1 != Y1));
        ASSERT(  X1 != Y2);

        ASSERT(!(X2 != X1));
        ASSERT(!(X2 != X2));
        ASSERT(  X2 != X3);
        ASSERT(!(X2 != Y1));
        ASSERT(  X2 != Y2);

        ASSERT(  X3 != X1);
        ASSERT(  X3 != X2);
        ASSERT(!(X3 != X3));
        ASSERT(  X3 != Y1);
        ASSERT(!(X3 != Y2));

        ASSERT(!(Y1 != X1));
        ASSERT(!(Y1 != X2));
        ASSERT(  Y1 != X3);
        ASSERT(!(Y1 != Y1));
        ASSERT(  Y1 != Y2);

        ASSERT(  Y2 != X1);
        ASSERT(  Y2 != X2);
        ASSERT(!(Y2 != X3));
        ASSERT(  Y2 != Y1);
        ASSERT(!(Y2 != Y2));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCT
        //  'bsltf::EmplacableTestType' allows to ensure that arguments are
        //  forwarded correctly to functions and methods taking an arbitrary
        //  number of arguments.  Special class 'ConstructTestType' uses this
        //  feature, but also adds allocator support to be valuable for
        //  'StdAllocatorAdaptor' testing.
        //
        // Concerns:
        //: 1 The 'construct' invokes appropriate 'construct' method of
        //:   allocator traits interface in accordance with constructed
        //:   object's properties.
        //:
        //: 2 The 'construct' correctly forwards arguments to the 'construct'
        //:   method of allocator traits interface, up to 10 arguments.
        //
        // Plan:
        //: 1 This test makes material use of template method
        //:   'testCase4_RunTest' with first integer template parameter
        //:   indicating the number of arguments to use, the next 10 integer
        //:   template parameters indicating '0' for copy, '1' for move, and
        //:   '2' for not-applicable (i.e., beyond the number of arguments).
        //:
        //:   1 Create 10 unique argument values.
        //:
        //:   2 Create two allocators and use each of them to supply memory,
        //:     sufficient for the object, being constructed with 'construct()'
        //:     method.
        //:
        //:   3 Create two adaptors having associated allocators of different
        //:     types. One type is equal to the type of constructing object's
        //:     allocator. Another is different.
        //:
        //:   4 Based on (first) template parameter indicating the number of
        //:     args to pass in, call 'construct' with the corresponding
        //:     argument values, performing an explicit move of the argument if
        //:     so indicated by the template parameter corresponding to the
        //:     argument, all in the presence of injected exceptions.
        //:
        //:   5 Verify that the argument values were passed correctly.
        //:
        //:   6 Verify that the move-state for each argument is as expected.
        //:     (C-2)
        //:
        //:   7 Verify that memory is supplied by the allocator, associated
        //:     with adaptor, if it has the same type as object's allocator
        //:     has, and isn't supplied otherwise.  (C-1)
        //
        // Testing:
        //   void construct(ELEMENT_TYPE *address, Args&&... arguments);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCT"
                            "\n=================\n");

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
        if (verbose) printf("\tTesting 'construct' with no arguments\n");
        testCase4_RunTest<0,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 1 argument\n");
        testCase4_RunTest<1,0,2,2,2,2,2,2,2,2,2>();
        testCase4_RunTest<1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 2 arguments\n");
        testCase4_RunTest<2,0,0,2,2,2,2,2,2,2,2>();
        testCase4_RunTest<2,0,1,2,2,2,2,2,2,2,2>();
        testCase4_RunTest<2,1,0,2,2,2,2,2,2,2,2>();
        testCase4_RunTest<2,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 3 arguments\n");
        testCase4_RunTest<3,0,0,0,2,2,2,2,2,2,2>();
        testCase4_RunTest<3,1,0,0,2,2,2,2,2,2,2>();
        testCase4_RunTest<3,0,1,0,2,2,2,2,2,2,2>();
        testCase4_RunTest<3,0,0,1,2,2,2,2,2,2,2>();
        testCase4_RunTest<3,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 4 arguments\n");
        testCase4_RunTest<4,0,0,0,0,2,2,2,2,2,2>();
        testCase4_RunTest<4,1,0,0,0,2,2,2,2,2,2>();
        testCase4_RunTest<4,0,1,0,0,2,2,2,2,2,2>();
        testCase4_RunTest<4,0,0,1,0,2,2,2,2,2,2>();
        testCase4_RunTest<4,0,0,0,1,2,2,2,2,2,2>();
        testCase4_RunTest<4,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 5 arguments\n");
        testCase4_RunTest<5,0,0,0,0,0,2,2,2,2,2>();
        testCase4_RunTest<5,1,0,0,0,0,2,2,2,2,2>();
        testCase4_RunTest<5,0,1,0,0,0,2,2,2,2,2>();
        testCase4_RunTest<5,0,0,1,0,0,2,2,2,2,2>();
        testCase4_RunTest<5,0,0,0,1,0,2,2,2,2,2>();
        testCase4_RunTest<5,0,0,0,0,1,2,2,2,2,2>();
        testCase4_RunTest<5,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 6 arguments\n");
        testCase4_RunTest<6,0,0,0,0,0,0,2,2,2,2>();
        testCase4_RunTest<6,1,0,0,0,0,0,2,2,2,2>();
        testCase4_RunTest<6,0,1,0,0,0,0,2,2,2,2>();
        testCase4_RunTest<6,0,0,1,0,0,0,2,2,2,2>();
        testCase4_RunTest<6,0,0,0,1,0,0,2,2,2,2>();
        testCase4_RunTest<6,0,0,0,0,1,0,2,2,2,2>();
        testCase4_RunTest<6,0,0,0,0,0,1,2,2,2,2>();
        testCase4_RunTest<6,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 7 arguments\n");
        testCase4_RunTest<7,0,0,0,0,0,0,0,2,2,2>();
        testCase4_RunTest<7,1,0,0,0,0,0,0,2,2,2>();
        testCase4_RunTest<7,0,1,0,0,0,0,0,2,2,2>();
        testCase4_RunTest<7,0,0,1,0,0,0,0,2,2,2>();
        testCase4_RunTest<7,0,0,0,1,0,0,0,2,2,2>();
        testCase4_RunTest<7,0,0,0,0,1,0,0,2,2,2>();
        testCase4_RunTest<7,0,0,0,0,0,1,0,2,2,2>();
        testCase4_RunTest<7,0,0,0,0,0,0,1,2,2,2>();
        testCase4_RunTest<7,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\tTesting 'construct' with 8 arguments\n");
        testCase4_RunTest<8,0,0,0,0,0,0,0,0,2,2>();
        testCase4_RunTest<8,1,0,0,0,0,0,0,0,2,2>();
        testCase4_RunTest<8,0,1,0,0,0,0,0,0,2,2>();
        testCase4_RunTest<8,0,0,1,0,0,0,0,0,2,2>();
        testCase4_RunTest<8,0,0,0,1,0,0,0,0,2,2>();
        testCase4_RunTest<8,0,0,0,0,1,0,0,0,2,2>();
        testCase4_RunTest<8,0,0,0,0,0,1,0,0,2,2>();
        testCase4_RunTest<8,0,0,0,0,0,0,1,0,2,2>();
        testCase4_RunTest<8,0,0,0,0,0,0,0,1,2,2>();
        testCase4_RunTest<8,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\tTesting 'construct' with 9 arguments\n");
        testCase4_RunTest<9,0,0,0,0,0,0,0,0,0,2>();
        testCase4_RunTest<9,1,0,0,0,0,0,0,0,0,2>();
        testCase4_RunTest<9,0,1,0,0,0,0,0,0,0,2>();
        testCase4_RunTest<9,0,0,1,0,0,0,0,0,0,2>();
        testCase4_RunTest<9,0,0,0,1,0,0,0,0,0,2>();
        testCase4_RunTest<9,0,0,0,0,1,0,0,0,0,2>();
        testCase4_RunTest<9,0,0,0,0,0,1,0,0,0,2>();
        testCase4_RunTest<9,0,0,0,0,0,0,1,0,0,2>();
        testCase4_RunTest<9,0,0,0,0,0,0,0,1,0,2>();
        testCase4_RunTest<9,0,0,0,0,0,0,0,0,1,2>();
        testCase4_RunTest<9,1,1,1,1,1,1,1,1,1,2>();

        if (verbose) printf("\tTesting 'construct' with 10 arguments\n");
        testCase4_RunTest<10,0,0,0,0,0,0,0,0,0,0>();
        testCase4_RunTest<10,1,0,0,0,0,0,0,0,0,0>();
        testCase4_RunTest<10,0,1,0,0,0,0,0,0,0,0>();
        testCase4_RunTest<10,0,0,1,0,0,0,0,0,0,0>();
        testCase4_RunTest<10,0,0,0,1,0,0,0,0,0,0>();
        testCase4_RunTest<10,0,0,0,0,1,0,0,0,0,0>();
        testCase4_RunTest<10,0,0,0,0,0,1,0,0,0,0>();
        testCase4_RunTest<10,0,0,0,0,0,0,1,0,0,0>();
        testCase4_RunTest<10,0,0,0,0,0,0,0,1,0,0>();
        testCase4_RunTest<10,0,0,0,0,0,0,0,0,1,0>();
        testCase4_RunTest<10,0,0,0,0,0,0,0,0,0,1>();
        testCase4_RunTest<10,1,1,1,1,1,1,1,1,1,1>();

#else
        testCase4_RunTest< 0,2,2,2,2,2,2,2,2,2,2>();
        testCase4_RunTest< 1,0,2,2,2,2,2,2,2,2,2>();
        testCase4_RunTest< 2,0,0,2,2,2,2,2,2,2,2>();
        testCase4_RunTest< 3,0,0,0,2,2,2,2,2,2,2>();
        testCase4_RunTest< 4,0,0,0,0,2,2,2,2,2,2>();
        testCase4_RunTest< 5,0,0,0,0,0,2,2,2,2,2>();
        testCase4_RunTest< 6,0,0,0,0,0,0,2,2,2,2>();
        testCase4_RunTest< 7,0,0,0,0,0,0,0,2,2,2>();
        testCase4_RunTest< 8,0,0,0,0,0,0,0,0,2,2>();
        testCase4_RunTest< 9,0,0,0,0,0,0,0,0,0,2>();
        testCase4_RunTest<10,0,0,0,0,0,0,0,0,0,0>();
#endif

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The 'allocator' method returns a reference, providing
        //:   non-modifiable access to the underlying allocator.
        //:
        //: 2  The 'allocator' method method is declared 'const'.
        //
        // Plan:
        //: 1 Construct the object using value constructor.
        //:
        //: 2 Use the 'allocator()' method to access underlying allocator and
        //:   verify it's value and address.
        //
        // Testing:
        //   const ALLOCATOR& allocator() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);
        ObjType              allocator(&ta);
        const ObjType        VALUE(allocator);
        Obj                  mX(VALUE);
        const Obj&           X = mX;

        ASSERT(VALUE == X.allocator());
        ASSERT(&mX   == &X.allocator());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 Objects, constructed with various constructors, have expected
        //:   values.
        //:
        //: 2 Appropriate parent class constructors are called during
        //:   construction.
        //:
        //: 3 Adaptor object doesn't occupy any memory itself, only underlying
        //:   allocator does.
        //
        // Plan:
        //: 1 Execute an inner loop creating four distinct objects, in turn,
        //:   using different constructors identified by 'CONFIG':
        //:   'a': using default constructor;
        //:
        //:   'b': using a value constructor, passing an allocator;
        //:
        //:   'c': using a copy constructor;
        //:
        //:   'd': using a copy constructor, passing an adaptor, adapting an
        //:        allocator of another type.
        //:
        //: 2 For each of the four iterations in P-1:
        //:
        //:   1 Create two 'bslma_TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Choose the constructor depending on 'CONFIG' to dynamically
        //:     create an object using a distinct test allocator for the
        //:     object's footprint.
        //:
        //:   3 Use the appropriate test allocator to verify that no
        //:     additional memory is allocated by the target object.  (C-3)
        //:
        //:   4 Use untested functionality to access underlying allocator and
        //:     check it's value to verify that correct constructor has been
        //:     called.  (C-2)
        //:
        //:   5 Use untested functionality to access underlying allocator and
        //:     verify that it has expected value.  (C-1)
        //
        // Testing:
        //   StdAllocatorAdaptor();
        //   StdAllocatorAdaptor(const ALLOCATOR& basicAllocator);
        //   StdAllocatorAdaptor(const StdAllocatorAdaptor<ANY_TYPE>& other);
        //   StdAllocatorAdaptor(const StdAllocatorAdaptor& origin) = default;
        //   ~StdAllocatorAdaptor() = default;
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the constructor

            if (veryVerbose)
            switch (CONFIG) {
              case 'a': {
                printf("\tTesting default constructor.\n");
              } break;
              case 'b': {
                printf("\tTesting value constructor.\n");
              } break;
              case 'c': {
                printf("\tTesting copy constructor.\n");
              } break;
              case 'd': {
                printf("\tTesting construction from another type adapter.\n");
              } break;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN("Bad constructor config.");
              } break;
            }
            // Test allocators.

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

            // Install default allocator.

            bslma::DefaultAllocatorGuard dag(&da);

            // Source objects creation.

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            ObjType              allocator(&ta);
            Obj                  mX(allocator);
            const Obj&           X = mX;

            bslma::TestAllocator ata("another", veryVeryVeryVerbose);
            ObjType              anotherAllocator(&ata);
            AnotherObj           mAX(anotherAllocator);
            const AnotherObj&    AX = mAX;

            // Objects to compare result with.

            const ObjType DEFAULT;
            const ObjType VALUE(&ta);
            const ObjType ANOTHER(&ata);

            Obj           *objPtr = 0;
            const ObjType *expected = &DEFAULT;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                expected = &DEFAULT;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(VALUE);
                expected = &VALUE;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(X);
                expected = &X.allocator();
              } break;
              case 'd': {
                objPtr = new (fa) Obj(AX);
                expected = &ANOTHER;
              } break;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN("Bad constructor config.");
              } break;
            }

            ASSERTV(CONFIG, sizeof(ObjType), sizeof(Obj),
                    sizeof(ObjType) == sizeof(Obj));

            ASSERTV(CONFIG, sizeof(Obj), fa.numBytesInUse(),
                    fa.numBytesInUse() == sizeof(Obj));

            Obj&       mY = *objPtr;
            const Obj& Y = mY;

            const ObjType& EXPECTED = *expected;

            ASSERTV(CONFIG, EXPECTED == Y.allocator());

            // Testing destructor.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);
        ObjType              allocator(&ta);
        Obj                  mX(allocator);

        int *pointer = mX.allocate(1);

        mX.construct(pointer, 2);

        ASSERT(2           == *pointer);
        ASSERT(sizeof(int) == ta.numBytesInUse());

        mX.deallocate(pointer, 1);

        ASSERT(0 == ta.numBytesInUse());

      } break;
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
