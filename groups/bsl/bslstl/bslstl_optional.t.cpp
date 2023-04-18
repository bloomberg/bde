// bslstl_optional.t.cpp                                              -*-C++-*-
#include <bslstl_optional.h>

#include <bslstl_string.h>

#include <bsla_maybeunused.h>

#include <bslalg_constructorproxy.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_managedptr.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_buildtarget.h>
#include <bsls_bsltestutil.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h> // 'bsls::Types::Int64'

// A list of disabled tests :
//
//BSLSTL_OPTIONAL_TEST_BAD_VALUE
//      Tests in this group verify that value category and cv qualification
//      of the return value from value() member function is correct.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT
//      Tests in this group verify that assignments is not possible if the
//      'value_type' is not both assignable and constructible from the source
//      type.
//
//BSLSTL_OPTIONAL_TEST_BAD_IL_EMPLACE
//      Tests in this group verify that emplace is not possible for const
//      optional types.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
//      Tests in this group verify that assignments is not possible to
//      an optional of const type or to a const qualified optional

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The object under test is a type whose interface and contract is dictated by
// the C++ standard.  The general concern is compliance with the standard.  In
// C++03 mode, the concern is to test all the features of the standard type
// that can be supported using C++03 features.  This type is implemented in the
// form of a class template, and thus its proper instantiation for several
// types is a concern.  The purpose of this type is to represent a value object
// that may or may not exist.  If the value object is allocator-aware, this
// type has an additional interface to allow for specifying the allocator and
// for retrieving the allocator in use.  This is implemented by having a
// different class template specialisation depending on whether the value type
// is allocator-aware or not, and thus the behaviour needs to be tested for
// both allocator-aware and non allocator-aware value types.  One of the
// guarantees this type provides is that no unnecessary copies of the value
// type object are created when using this type as opposed to using a raw value
// type object.  In order to test this guarantee, we use test types designed to
// count the number of instances created.
//
// TYPEDEFS
// [15] typedef TYPE ValueType;
// [15] typename bsl::allocator<char> allocator_type;
// [26] bsl::optional<bslma::ManagedPtr<void>>
//
// TRAITS
// [15] bsl::is_trivially_copyable
// [15] bsl::is_trivially_destructible
// [15] BloombergLP::bslma::UsesBslmaAllocator
// [15] BloombergLP::bslmf::UsesAllocatorArgT
//
// CREATORS
// [ 2] optional();
// [ 2] optional(nullopt_t);
// [ 7] optional(const optional&);
// [ 7] optional(optional&&);
// [ 3] optional(const TYPE&);
// [ 3] optional(TYPE&&);
// [ 7] optional(const optional<ANY_TYPE> &);
// [ 7] optional(optional<ANY_TYPE>&&);
// [ 7] optional(const std::optional<ANY_TYPE> &);
// [ 7] optional(std::optional<ANY_TYPE>&&);
// [ 3] optional(const ANY_TYPE&);
// [ 3] optional(ANY_TYPE&&);
// [ 3] optional(in_place_t, ARGS&&...);
// [ 3] optional(in_place_t, std::initializer_list, ARGS&&...);
// [ 2] optional(alloc_arg, alloc);
// [ 2] optional(alloc_arg, alloc, nullopt_t);
// [ 7] optional(alloc_arg, alloc, const optional&);
// [ 7] optional(alloc_arg, alloc, optional&&);
// [ 3] optional(alloc_arg, alloc, const TYPE&);
// [ 3] optional(alloc_arg, alloc, TYPE&&);
// [ 7] optional(alloc_arg, alloc, const optional<ANY_TYPE> &);
// [ 7] optional(alloc_arg, alloc, optional<ANY_TYPE>&&);
// [ 7] optional(alloc_arg, alloc, const std::optional<ANY_TYPE> &);
// [ 7] optional(alloc_arg, alloc, std::optional<ANY_TYPE>&&);
// [ 3] optional(alloc_arg, alloc, const ANY_TYPE&);
// [ 3] optional(alloc_arg, alloc, ANY_TYPE&&);
// [ 3] optional(alloc_arg, alloc, in_place_t, ARGS&&...);
// [ 3] optional(alloc_arg, alloc, in_place_t, init_list, ARGS&&...);
// [ 2] ~optional(nullopt_t);
//
// MANIPULATORS
// [13] T& emplace(ARGS&&...);
// [13] T& emplace(std::initializer_list<INIT_LIST_TYPE>, ARGS&&...);
// [ 2] void reset();
// [ 9] void swap(optional& other);
// [ 4] TYPE&  value() &;
// [ 4] TYPE&& value() &&;
// [ 4] TYPE value_or(ANY_TYPE&&) &&;
// [ 4] TYPE value_or(alloc_arg, alloc, ANY_TYPE&&) &&
// [10] optional& operator=(bsl::nullopt_t);
// [10] optional& operator=(const optional&);
// [10] optional& operator=(optional&&);
// [10] optional& operator=(const optional<ANY_TYPE>&);
// [10] optional& operator=(optional<ANY_TYPE>&&);
// [10] optional& operator=(const std::optional<ANY_TYPE>&);
// [10] optional& operator=(std::optional<ANY_TYPE>&&);
// [10] optional& operator=(const TYPE&);
// [10] optional& operator=(TYPE&&);
// [10] optional& operator=(const ANY_TYPE&);
// [10] optional& operator=(ANY_TYPE&&);
// [22] operator=(optional<bdef_Function>);
// [ 4] TYPE *operator->();
// [ 4] TYPE&  operator*() &;
// [ 4] TYPE&& operator*() &&;
//
// ACCESSORS
// [ 2] allocator_type get_allocator() const;
// [ 4] bool has_value() const;
// [ 4] const TYPE&  value() const &;
// [ 4] const TYPE&&  value() const &&;
// [ 4] const TYPE *operator->() const;
// [ 4] const TYPE&  operator*() const &;
// [ 4] const TYPE&&  operator*() const &&;
// [ 4] TYPE value_or(ANY_TYPE&&) const&;
// [ 4] TYPE value_or(alloc_arg, alloc, ANY_TYPE&&) const&;
// [ 4] explicit operator bool() const;
//
// FREE OPERATORS
// [ 9] void swap(bsl::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
// [ 9] void swap(std::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
// [ 9] void swap(bsl::optional<TYPE>& lhs, std::optional<TYPE>& rhs);
// [14] void hashAppend(HASHALG& hashAlg, const optional<TYPE>& input);
// [ 6] bool operator==(const optional<LHS>&, nullopt_t&);
// [ 6] bool operator!=(const optional<LHS>&, nullopt_t&);
// [ 6] bool operator< (const optional<LHS>&, nullopt_t&);
// [ 6] bool operator<=(const optional<LHS>&, nullopt_t&);
// [ 6] bool operator> (const optional<LHS>&, nullopt_t&);
// [ 6] bool operator>=(const optional<LHS>&, nullopt_t&);
// [ 6] bool operator==(const optional<LHS>&, const RHS&);
// [ 6] bool operator!=(const optional<LHS>&, const RHS&);
// [ 6] bool operator< (const optional<LHS>&, const RHS&);
// [ 6] bool operator<=(const optional<LHS>&, const RHS&);
// [ 6] bool operator> (const optional<LHS>&, const RHS&);
// [ 6] bool operator>=(const optional<LHS>&, const RHS&);
// [ 6] bool operator==(const optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator!=(const optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator< (const optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator<=(const optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator>=(const optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator> (const optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator==(const nullopt_t&, const optional<RHS>&);
// [ 6] bool operator!=(const nullopt_t&, const optional<RHS>&);
// [ 6] bool operator< (const nullopt_t&, const optional<RHS>&);
// [ 6] bool operator<=(const nullopt_t&, const optional<RHS>&);
// [ 6] bool operator> (const nullopt_t&, const optional<RHS>&);
// [ 6] bool operator>=(const nullopt_t&, const optional<RHS>&);
// [ 6] bool operator==(const LHS&, const optional<RHS>&);
// [ 6] bool operator!=(const LHS&, const optional<RHS>&);
// [ 6] bool operator< (const LHS&, const optional<RHS>&);
// [ 6] bool operator<=(const LHS&, const optional<RHS>&);
// [ 6] bool operator> (const LHS&, const optional<RHS>&);
// [ 6] bool operator>=(const LHS&, const optional<RHS>&);
// [ 6] bool operator==(const std::optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator!=(const std::optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator< (const std::optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator<=(const std::optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator>=(const std::optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator> (const std::optional<LHS>&, const optional<RHS>&);
// [ 6] bool operator==(const optional<LHS>&, const std::optional<RHS>&);
// [ 6] bool operator!=(const optional<LHS>&, const std::optional<RHS>&);
// [ 6] bool operator< (const optional<LHS>&, const std::optional<RHS>&);
// [ 6] bool operator<=(const optional<LHS>&, const std::optional<RHS>&);
// [ 6] bool operator>=(const optional<LHS>&, const std::optional<RHS>&);
// [ 6] bool operator> (const optional<LHS>&, const std::optional<RHS>&);
// [ 6] auto operator<=>(const optional<LHS>&, const optional<RHS>&);
// [ 6] auto operator<=>(const optional<LHS>&, const RHS&);
// [ 6] auto operator<=>(const optional<LHS>&, nullopt_t);
// [ 6] auto operator<=>(const optional<LHS>&, const std::optional<RHS>&);
// [17] optional make_optional(alloc_arg, const alloc&, TYPE&&);
// [17] optional make_optional(alloc_arg, const alloc&, ARGS&&...);
// [17] optional make_optional(alloc_arg, const alloc&, init_list, ARGS&&...);
// [16] optional make_optional();
// [16] optional make_optional(TYPE&&);
// [16] optional make_optional(ARG&&, ARGS&&...);
// [16] optional make_optional(initializer_list, ARGS&&...);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] DRQS 169300521
// [19] DRQS 165776192
// [21] CLASS TEMPLATE DEDUCTION GUIDES
// [23] TESTING DERIVED -- 'TYPE' ALLOCATES
// [24] TESTING DERIVED -- 'TYPE' DOES NOT ALLOCATE
// [25] IMPLICIT/EXPLICIT C'TORS TEST
// [27] CONCEPTS

// Further, there are a number of behaviors that explicitly should not compile
// by accident that we will provide tests for.  These tests should fail to
// compile if the appropriate macro is defined.  Each such test will use a
// unique macro for its feature test, and provide a commented-out definition of
// that macro immediately above the test, to easily enable compiling that test
// while in development.  Below is the list of all macros that control the
// availability of these tests:
//  #define BSLSTL_OPTIONAL_COMPILE_FAIL_NOT_AN_ALLOCATOR_AWARE_TYPE
//  #define BSLSTL_OPTIONAL_COMPILE_FAIL_NOT_AN_ALLOCATOR

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

using namespace BloombergLP;
using namespace bsl;

typedef bslmf::MovableRefUtil MoveUtil;

typedef bsltf::TemplateTestFacility TTF;
typedef bsltf::MoveState            MoveState;

namespace {
    enum { k_MOVED_FROM_VAL = 0x01d };
    enum { k_DESTROYED = 0x05c };

}  // close unnamed namespace

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

#define ASSERT_IS_MOVED_FROM(val)                                             \
    ASSERT(TTF::getMovedFromState(val) == MoveState::e_MOVED ||               \
           TTF::getMovedFromState(val) == MoveState::e_UNKNOWN);

#define ASSERT_IS_MOVED_INTO(val)                                             \
    ASSERT(TTF::getMovedIntoState(val) == MoveState::e_MOVED ||               \
           TTF::getMovedIntoState(val) == MoveState::e_UNKNOWN);

#define ASSERT_IS_NOT_MOVED_FROM(val)                                         \
    ASSERT(TTF::getMovedFromState(val) == MoveState::e_NOT_MOVED ||           \
           TTF::getMovedFromState(val) == MoveState::e_UNKNOWN);

#define ASSERT_IS_NOT_MOVED_INTO(val)                                         \
    ASSERT(TTF::getMovedIntoState(val) == MoveState::e_NOT_MOVED ||           \
           TTF::getMovedIntoState(val) == MoveState::e_UNKNOWN);

#define BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING                          \
    MyClass1, MyClass1a, MyClass2, MyClass2a

#define BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS                              \
    ConstructTestTypeNoAlloc, ConstructTestTypeAlloc,                         \
        ConstructTestTypeAllocArgT

#define BSLSTL_OPTIONAL_TEST_NESTED_TYPE(X)                                   \
     bsl::optional<X>

#define BSLSTL_OPTIONAL_TEST_NESTED_TYPES                                     \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(signed char),                              \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(size_t),                                   \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(const char *),                             \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::TemplateTestFacility::ObjectPtr),   \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::TemplateTestFacility::FunctionPtr), \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::TemplateTestFacility::MethodPtr),   \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::EnumeratedTestType::Enum),          \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::UnionTestType),                     \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::SimpleTestType),                    \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::AllocTestType),                     \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::BitwiseCopyableTestType),           \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::BitwiseMoveableTestType),           \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::AllocBitwiseMoveableTestType),      \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::MovableTestType),                   \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::MovableAllocTestType),              \
  BSLSTL_OPTIONAL_TEST_NESTED_TYPE(bsltf::NonTypicalOverloadsTestType)

                              // ================
                              // class MyClassDef
                              // ================

struct MyClassDef {
    // Data members that give MyClassX size and alignment.  This class is a
    // simple aggregate, use to provide a common data layout to subsequent test
    // types.  There are no semantics associated with any of the members, in
    // particular the allocator pointer is not used directly by this aggregate
    // to allocate storage owned by this class.

    // DATA (exceptionally public, only in test driver)
    int               d_value;
    int              *d_data_p;
    bslma::Allocator *d_allocator_p;

    // In optimized builds, some compilers will elide some of the operations in the
    // destructors of the test classes defined below.  In order to force the
    // compiler to retain all of the code in the destructors, we provide the
    // following function that can be used to (conditionally) print out the state
    // of a 'MyClassDef' data member.  If the destructor calls this function as its
    // last operation, then all values set in the destructor have visible
    // side-effects, but non-verbose test runs do not have to be burdened with
    // additional output.
    static bool s_forceDestructorCall;

    void dumpState();
};


bool MyClassDef::s_forceDestructorCall = false;

void MyClassDef::dumpState()
{
    if (s_forceDestructorCall) {
        printf("%p: %d %p %p\n",
               this,
               this->d_value,
               this->d_data_p,
               this->d_allocator_p);
    }
}

                               // ==============
                               // class MyClass1
                               // ==============

struct MyClass1 {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'MyClassDef' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer, while
    // the 'd_data_p' pointer is never initialized.  This class supports move,
    // copy, and destructor counters and can be used in tests that check for
    // unnecessary copies.  A signal value, 'k_MOVED_FROM_VAL', is used to detect
    // an object in a moved-from state.

    // DATA
    MyClassDef d_def;

    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_lvalueConstructorInvocations;
    static int s_rvalueConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;
    static int s_destructorInvocations;

    // CREATORS
    MyClass1(int v = 0)  // IMPLICIT
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }
    MyClass1(const MyClass1& rhs)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++s_copyConstructorInvocations;
    }

    MyClass1(bslmf::MovableRef<MyClass1> other)
    {
        MyClass1& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        d_def.d_allocator_p    = 0;
        ++s_moveConstructorInvocations;
    }

    ~MyClass1()
    {
        ASSERT(d_def.d_value != k_DESTROYED);
        d_def.d_value       = k_DESTROYED;
        d_def.d_allocator_p = 0;
        d_def.dumpState();
        ++s_destructorInvocations;
    }

    MyClass1& operator=(const MyClass1& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        ++s_copyAssignmentInvocations;
        return *this;
    }

    MyClass1& operator=(bslmf::MovableRef<MyClass1> rhs)
    {
        MyClass1& otherRef     = MoveUtil::access(rhs);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        ++s_moveAssignmentInvocations;
        return *this;
    }

    MyClass1& operator=(int rhs)
    {
        d_def.d_value = rhs;
        return *this;
    }
    // ACCESSORS
    int value() const { return d_def.d_value; }
};
bool operator==(const MyClass1& lhs, const MyClass1& rhs)
{
    return (lhs.value() == rhs.value());
}
// CLASS DATA
int MyClass1::s_copyConstructorInvocations   = 0;
int MyClass1::s_moveConstructorInvocations   = 0;
int MyClass1::s_lvalueConstructorInvocations = 0;
int MyClass1::s_rvalueConstructorInvocations = 0;
int MyClass1::s_copyAssignmentInvocations    = 0;
int MyClass1::s_moveAssignmentInvocations    = 0;
int MyClass1::s_destructorInvocations        = 0;

                              // ===============
                              // class MyClass1a
                              // ===============

struct MyClass1a {
    // This 'class' is the same as MyClass1, except it also supports
    // conversion from MyClass1. This allows for testing of converting
    // constructors and assignment from a type convertible to value type.

    MyClass1   d_data;
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_lvalueConstructorInvocations;
    static int s_rvalueConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;
    static int s_destructorInvocations;

    // CREATORS

    MyClass1a(int v = 0)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass1a(const MyClass1& v)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass1a(bslmf::MovableRef<MyClass1> v)  // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(v)))
    {
        ++s_rvalueConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1> v)
    : d_data(MoveUtil::access(v))
    {
        ++s_lvalueConstructorInvocations;
    }   // IMPLICIT
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass1a(const MyClass1a& rhs)
    : d_data(rhs.d_data)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass1a(bslmf::MovableRef<MyClass1a> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
        ++s_moveConstructorInvocations;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1a> rhs)
    : d_data(MoveUtil::access(rhs).d_data)
    {
        // a move from a const object is a copy
        ++s_copyConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // MANIPULATORS
    MyClass1a& operator=(const MyClass1a& rhs)
    {
        d_data. operator=(rhs.d_data);
        ++s_copyAssignmentInvocations;
        return *this;
    }

    MyClass1a& operator=(bslmf::MovableRef<MyClass1a> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        ++s_moveAssignmentInvocations;
        return *this;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a& operator=(bslmf::MovableRef<const MyClass1a> rhs)
    {
        d_data. operator=(MoveUtil::access(rhs).d_data);
        ++s_copyAssignmentInvocations;
        return *this;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass1a& operator=(int rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }

    ~MyClass1a()
    {
        ++s_destructorInvocations;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
// CLASS DATA
int MyClass1a::s_copyConstructorInvocations   = 0;
int MyClass1a::s_moveConstructorInvocations   = 0;
int MyClass1a::s_lvalueConstructorInvocations = 0;
int MyClass1a::s_rvalueConstructorInvocations = 0;
int MyClass1a::s_copyAssignmentInvocations    = 0;
int MyClass1a::s_moveAssignmentInvocations    = 0;
int MyClass1a::s_destructorInvocations        = 0;

bool operator==(const MyClass1a& lhs, const MyClass1a& rhs)
{
    return (lhs.value() == rhs.value());
}

                               // ==============
                               // class MyClass2
                               // ==============
struct MyClass2 {
    // This 'class' supports the 'bslma::UsesBslmaAllocator' trait, providing
    // an allocator-aware version of every constructor.  While it holds an
    // allocator and has the expected allocator propagation properties of a
    // 'bslma::Allocator'-aware type, it does not actually allocate any memory.
    // This class supports move, copy, and destructor counters and can be used
    // in tests that check for unnecessary copies and correct destructor
    // invocation.  A signal value, 'k_MOVED_FROM_VAL', is used to detect an
    // object in a moved-from state.  This class is convertable and assignable
    // from an object of type 'MyClass1', which allows testing of converting
    // constructors and assignment from a type convertible to value type.

    // DATA
    MyClassDef d_def;

    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_lvalueConstructorInvocations;
    static int s_rvalueConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;
    static int s_destructorInvocations;


    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit MyClass2(bslma::Allocator *a = 0)
    {
        d_def.d_value       = 0;
        d_def.d_allocator_p = a;
    }

    MyClass2(int v, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = a;
    }
    MyClass2(const MyClass2& rhs, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        s_copyConstructorInvocations++;
    }

    MyClass2(bslmf::MovableRef<MyClass2> other)
    {
        // IMPLICIT
        MyClass2& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        s_moveConstructorInvocations++;
    }
    MyClass2(bslmf::MovableRef<MyClass2> other, bslma::Allocator *a)
    {
        // IMPLICIT
        MyClass2& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        d_def.d_allocator_p = a;

        s_moveConstructorInvocations++;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass2> other)
    {  // IMPLICIT

        const MyClass2& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        // a move from a const object is a copy
        s_copyConstructorInvocations++;
    }
    MyClass2(bslmf::MovableRef<const MyClass2> other, bslma::Allocator *a)
    {  // IMPLICIT

        const MyClass2& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p = a;

        // a move from a const object is a copy
        s_copyConstructorInvocations++;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2(const MyClass1& rhs, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        s_lvalueConstructorInvocations++;
    }

    MyClass2(bslmf::MovableRef<MyClass1> other, bslma::Allocator *a = 0)
        // IMPLICIT
    {
        MyClass1& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        d_def.d_allocator_p    = a;
        s_rvalueConstructorInvocations++;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass1> other, bslma::Allocator *a = 0)
    {  // IMPLICIT

        const MyClass1& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p      = a;
        s_lvalueConstructorInvocations++;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~MyClass2()
    {
        ASSERT(d_def.d_value != k_DESTROYED);
        d_def.d_value       = k_DESTROYED;
        d_def.d_allocator_p = 0;
        d_def.dumpState();
        ++s_destructorInvocations;
    }

    // MANIPULATORS
    MyClass2& operator=(const MyClass2& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        // do not touch allocator!
        ++s_copyAssignmentInvocations;
        return *this;
    }

    MyClass2& operator=(bslmf::MovableRef<MyClass2> rhs)
    {
        MyClass2& otherRef     = MoveUtil::access(rhs);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        // do not touch allocator!
        ++s_moveAssignmentInvocations;
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2& operator=(bslmf::MovableRef<const MyClass2> rhs)
    {
        const MyClass2& otherRef = MoveUtil::access(rhs);
        d_def.d_value            = otherRef.d_def.d_value;
        // do not touch allocator!
        ++s_copyAssignmentInvocations;
        return *this;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2& operator=(int rhs)
    {
        d_def.d_value = rhs;
        // do not touch allocator!
        return *this;
    }

    // ACCESSORS

    int value() const { return d_def.d_value; }

    bsl::allocator<char> get_allocator() const { return d_def.d_allocator_p; }
};
// CLASS DATA
int MyClass2::s_copyConstructorInvocations   = 0;
int MyClass2::s_moveConstructorInvocations   = 0;
int MyClass2::s_lvalueConstructorInvocations = 0;
int MyClass2::s_rvalueConstructorInvocations = 0;
int MyClass2::s_copyAssignmentInvocations    = 0;
int MyClass2::s_moveAssignmentInvocations    = 0;
int MyClass2::s_destructorInvocations        = 0;

bool operator==(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() == rhs.value());
}
bool operator==(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() == rhs);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

auto operator<=>(const MyClass2& lhs, const MyClass2& rhs)
{
    return lhs.value() <=> rhs.value();
}
auto operator<=>(const MyClass2& lhs, const int& rhs)
{
    return lhs.value() <=> rhs;
}

#else
bool operator==(const int& lhs, const MyClass2& rhs)
{
    return (lhs == rhs.value());
}
bool operator!=(const MyClass2& lhs, const MyClass2& rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const int& lhs, const MyClass2& rhs)
{
    return !(lhs == rhs);
}
bool operator!=(const MyClass2& lhs, const int& rhs)
{
    return !(lhs == rhs);
}
bool operator<(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() < rhs.value());
}

bool operator<(const int& lhs, const MyClass2& rhs)
{
    return (lhs < rhs.value());
}
bool operator<(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() < rhs);
}
bool operator>(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() > rhs.value());
}

bool operator>(const int& lhs, const MyClass2& rhs)
{
    return (lhs > rhs.value());
}
bool operator>(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() > rhs);
}
bool operator<=(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() <= rhs.value());
}

bool operator<=(const int& lhs, const MyClass2& rhs)
{
    return (lhs <= rhs.value());
}
bool operator<=(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() <= rhs);
}
bool operator>=(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() >= rhs.value());
}

bool operator>=(const int& lhs, const MyClass2& rhs)
{
    return (lhs >= rhs.value());
}
bool operator>=(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() >= rhs);
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON


                                 // =========
                                 // MyClass2a
                                 // =========

struct MyClass2a {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.  This
    // class is constructible and assignable from MyClass2

    MyClass2   d_data;

    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_lvalueConstructorInvocations;
    static int s_rvalueConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;
    static int s_destructorInvocations;


    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2a, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2a, bslmf::UsesAllocatorArgT);

    // CREATORS
    MyClass2a()
    : d_data()
    {
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    explicit MyClass2a(int v)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass2a(const MyClass2& rhs)  // IMPLICIT
    : d_data(rhs)
    {
        ++s_lvalueConstructorInvocations;
    }

    MyClass2a(bslmf::MovableRef<MyClass2> rhs)  // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(rhs)))
    {
        ++s_rvalueConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2> rhs)  // IMPLICIT
    : d_data(MoveUtil::access(rhs))
    {
        ++s_lvalueConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(const MyClass2a& rhs)
    : d_data(rhs.d_data)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass2a(bslmf::MovableRef<MyClass2a> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
        ++s_moveConstructorInvocations;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2a> rhs)
    : d_data(MoveUtil::access(rhs).d_data)
    {
        ++s_copyConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a)
    {
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2& v)
    : d_data(v, a)
    {
        ++s_lvalueConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator            *a,
              bslmf::MovableRef<MyClass2>  v)
    : d_data(MoveUtil::move(MoveUtil::access(v)), a)
    {
        ++s_rvalueConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator                  *a,
              bslmf::MovableRef<const MyClass2>  v)
    : d_data(MoveUtil::access(v), a)
    {
        ++s_lvalueConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2a& rhs)
    : d_data(rhs.d_data, a)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator             *a,
              bslmf::MovableRef<MyClass2a>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
        ++s_moveConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator                   *a,
              bslmf::MovableRef<const MyClass2a>  rhs)
    : d_data(MoveUtil::access(rhs).d_data, a)
    {
        ++s_copyConstructorInvocations;
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~MyClass2a()
    {
        ++s_destructorInvocations;
    }

    // MANIPULATORS
    MyClass2a& operator=(const MyClass2a& rhs)
    {
        d_data. operator=(rhs.d_data);
        ++s_copyAssignmentInvocations;
        return *this;
    }

    MyClass2a& operator=(bslmf::MovableRef<MyClass2a> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        ++s_moveAssignmentInvocations;
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(bslmf::MovableRef<const MyClass2a> rhs)
    {
        d_data. operator=(MoveUtil::access(rhs).d_data);
        ++s_copyAssignmentInvocations;
        return *this;
    }

#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(int rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }

    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};

bool operator==(const MyClass2a& lhs, const MyClass2a& rhs)
{
    return (lhs.value() == rhs.value());
}
int MyClass2a::s_copyConstructorInvocations   = 0;
int MyClass2a::s_moveConstructorInvocations   = 0;
int MyClass2a::s_lvalueConstructorInvocations = 0;
int MyClass2a::s_rvalueConstructorInvocations = 0;
int MyClass2a::s_copyAssignmentInvocations    = 0;
int MyClass2a::s_moveAssignmentInvocations    = 0;
int MyClass2a::s_destructorInvocations        = 0;


                                 // =========
                                 // MyClass2b
                                 // =========

class MyClass2b {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.  This
    // class is assignable from MyClass2, but not constructible from MyClass2
  public:
    MyClass2 d_data;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2b, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2b, bslmf::UsesAllocatorArgT);

    // CREATORS
    MyClass2b()
    : d_data()
    {
    }

    MyClass2b(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    MyClass2b(const MyClass2b& rhs)
    : d_data(rhs.d_data)
    {
    }

    MyClass2b(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2b& rhs)
    : d_data(rhs.d_data, a)
    {
    }

    MyClass2b(bslmf::MovableRef<MyClass2b> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
    }

    MyClass2b(bsl::allocator_arg_t,
              bslma::Allocator             *a,
              bslmf::MovableRef<MyClass2b>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
    }

    // MANIPULATORS
    MyClass2b& operator=(const MyClass2b& rhs)
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass2b& operator=(bslmf::MovableRef<MyClass2b> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

    MyClass2b& operator=(const MyClass2& rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }

    MyClass2b& operator=(bslmf::MovableRef<MyClass2> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs)));
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }

    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};
bool operator==(const MyClass2b& lhs, const MyClass2b& rhs)
{
    return (lhs.value() == rhs.value());
}


                                 // =========
                                 // MyClass2c
                                 // =========

class MyClass2c {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.  This
    // class is constructable from 'MyClass2', but not assignable from 'MyClass2'.

  public:
    MyClass2 d_data;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2c, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(MyClass2c, bslmf::UsesAllocatorArgT);


    // CREATORS
    MyClass2c()
    : d_data()
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    explicit MyClass2c(int v)
    : d_data(v)
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a)
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2& v)
    : d_data(v, a)
    {
    }

    MyClass2c(bsl::allocator_arg_t,
              bslma::Allocator            *a,
              bslmf::MovableRef<MyClass2>  v)
    : d_data(MoveUtil::move(v), a)
    {
    }

    MyClass2c(const MyClass2c& rhs)
    : d_data(rhs.d_data)
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2c& rhs)
    : d_data(rhs.d_data, a)
    {
    }

    MyClass2c(bslmf::MovableRef<MyClass2c> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
    }

    MyClass2c(bsl::allocator_arg_t,
              bslma::Allocator             *a,
              bslmf::MovableRef<MyClass2c>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
    }

    // MANIPULATORS
    MyClass2c& operator=(const MyClass2c& rhs)
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass2c& operator=(bslmf::MovableRef<MyClass2c> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

    MyClass2c& operator=(int rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }

    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};
bool operator==(const MyClass2c& lhs, const MyClass2c& rhs)
{
    return (lhs.value() == rhs.value());
}

                           // ======================
                           // class ConstructTestArg
                           // ======================

template <int ID>
struct ConstructTestArg {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters to 'construct' due to the fact that
    // 'ConstructTestArg<ID1>' is a different type than 'ConstructTestArg<ID2>'
    // if 'ID1 != ID2'.

    // PUBLIC DATA
    const int d_value;
    int       d_copyCount;
        // A counter tracking the number of copy constructions leading to this
        // particular instance.

    // CREATORS
    ConstructTestArg(int value = -1);
        // Create an object having the specified 'value'.

    ConstructTestArg(const ConstructTestArg& other);
    ConstructTestArg(bslmf::MovableRef<ConstructTestArg> other);
};

// CREATORS
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(int value)
: d_value(value)
, d_copyCount(0)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(const ConstructTestArg& other)
: d_value(other.d_value)
, d_copyCount(other.d_copyCount + 1)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(
                                     bslmf::MovableRef<ConstructTestArg> other)
: d_value(MoveUtil::access(other).d_value)
, d_copyCount(MoveUtil::access(other).d_copyCount)
{
}
                       // ==============================
                       // class ConstructTestTypeNoAlloc
                       // ==============================

class ConstructTestTypeNoAlloc {
    // This 'struct' provides a test class capable of holding up to 14
    // parameters of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeNoAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  A 'ConstructTestTypeNoAlloc' can be invoked with up
    // to 14 parameters, via member functions 'testFunc[1--14]'.  These
    // functions are also called by the overloaded member 'operator()' of the
    // same signatures, and similar global functions 'testFunc[1--14]'.  All
    // invocations support the above 'ConstructTestSlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1  // default value for all private data
    };

  public:
    // PUBLIC DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_lvalueConstructorInvocations;
    static int s_rvalueConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;

    int        d_ilsum;  // sum of initializer_list argument values

    Arg1       d_a1;
    Arg2       d_a2;
    Arg3       d_a3;
    Arg4       d_a4;
    Arg5       d_a5;
    Arg6       d_a6;
    Arg7       d_a7;
    Arg8       d_a8;
    Arg9       d_a9;
    Arg10      d_a10;
    Arg11      d_a11;
    Arg12      d_a12;
    Arg13      d_a13;
    Arg14      d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)

    ConstructTestTypeNoAlloc()
    : d_ilsum(0){};

    explicit ConstructTestTypeNoAlloc(const Arg1& a1)
    : d_ilsum(0)
    , d_a1(a1)
    {
    }
    explicit ConstructTestTypeNoAlloc(bslmf::MovableRef<Arg1> a1)
    : d_ilsum(0)
    , d_a1(MoveUtil::move(a1))
    {
    }

    template <class ARG1, class ARG2>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
    }

    template <class ARG1, class ARG2, class ARG3>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
    }

    ConstructTestTypeNoAlloc(const ConstructTestTypeNoAlloc& other)
    : d_ilsum(0)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeNoAlloc(
                            bslmf::MovableRef<ConstructTestTypeNoAlloc>& other)
    : d_ilsum(0)
    , d_a1(MoveUtil::access(other).d_a1)
    , d_a2(MoveUtil::access(other).d_a2)
    , d_a3(MoveUtil::access(other).d_a3)
    , d_a4(MoveUtil::access(other).d_a4)
    , d_a5(MoveUtil::access(other).d_a5)
    , d_a6(MoveUtil::access(other).d_a6)
    , d_a7(MoveUtil::access(other).d_a7)
    , d_a8(MoveUtil::access(other).d_a8)
    , d_a9(MoveUtil::access(other).d_a9)
    , d_a10(MoveUtil::access(other).d_a10)
    , d_a11(MoveUtil::access(other).d_a11)
    , d_a12(MoveUtil::access(other).d_a12)
    , d_a13(MoveUtil::access(other).d_a13)
    , d_a14(MoveUtil::access(other).d_a14)
    {
        ++s_moveConstructorInvocations;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    ConstructTestTypeNoAlloc(std::initializer_list<int> il)
    : d_ilsum(0)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
        for (int i : il)
            d_ilsum += i;
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
};

int ConstructTestTypeNoAlloc::s_copyConstructorInvocations   = 0;
int ConstructTestTypeNoAlloc::s_moveConstructorInvocations   = 0;
int ConstructTestTypeNoAlloc::s_lvalueConstructorInvocations = 0;
int ConstructTestTypeNoAlloc::s_rvalueConstructorInvocations = 0;
int ConstructTestTypeNoAlloc::s_copyAssignmentInvocations    = 0;
int ConstructTestTypeNoAlloc::s_moveAssignmentInvocations    = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeNoAlloc& lhs,
                const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_ilsum == rhs.d_ilsum &&
           lhs.d_a1.d_value == rhs.d_a1.d_value &&
           lhs.d_a2.d_value == rhs.d_a2.d_value &&
           lhs.d_a3.d_value == rhs.d_a3.d_value &&
           lhs.d_a4.d_value == rhs.d_a4.d_value &&
           lhs.d_a5.d_value == rhs.d_a5.d_value &&
           lhs.d_a6.d_value == rhs.d_a6.d_value &&
           lhs.d_a7.d_value == rhs.d_a7.d_value &&
           lhs.d_a8.d_value == rhs.d_a8.d_value &&
           lhs.d_a9.d_value == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}
bool createdAlike(const ConstructTestTypeNoAlloc& lhs,
                  const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_a1.d_copyCount == rhs.d_a1.d_copyCount &&
           lhs.d_a2.d_copyCount == rhs.d_a2.d_copyCount &&
           lhs.d_a3.d_copyCount == rhs.d_a3.d_copyCount &&
           lhs.d_a4.d_copyCount == rhs.d_a4.d_copyCount &&
           lhs.d_a5.d_copyCount == rhs.d_a5.d_copyCount &&
           lhs.d_a6.d_copyCount == rhs.d_a6.d_copyCount &&
           lhs.d_a7.d_copyCount == rhs.d_a7.d_copyCount &&
           lhs.d_a8.d_copyCount == rhs.d_a8.d_copyCount &&
           lhs.d_a9.d_copyCount == rhs.d_a9.d_copyCount &&
           lhs.d_a10.d_copyCount == rhs.d_a10.d_copyCount &&
           lhs.d_a11.d_copyCount == rhs.d_a11.d_copyCount &&
           lhs.d_a12.d_copyCount == rhs.d_a12.d_copyCount &&
           lhs.d_a13.d_copyCount == rhs.d_a13.d_copyCount &&
           lhs.d_a14.d_copyCount == rhs.d_a14.d_copyCount;
}
                        // ============================
                        // class ConstructTestTypeAlloc
                        // ============================

class ConstructTestTypeAlloc {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class intentionally *does* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1  // default value for all private data
    };

  public:

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructTestTypeAlloc, bslma::UsesBslmaAllocator);

    // PUBLIC DATA
    static int        s_copyConstructorInvocations;
    static int        s_moveConstructorInvocations;
    static int        s_lvalueConstructorInvocations;
    static int        s_rvalueConstructorInvocations;
    static int        s_copyAssignmentInvocations;
    static int        s_moveAssignmentInvocations;

    int               d_ilsum;  // sum of initializer_list argument values

    bslma::Allocator *d_allocator_p;
    Arg1              d_a1;
    Arg2              d_a2;
    Arg3              d_a3;
    Arg4              d_a4;
    Arg5              d_a5;
    Arg6              d_a6;
    Arg7              d_a7;
    Arg8              d_a8;
    Arg9              d_a9;
    Arg10             d_a10;
    Arg11             d_a11;
    Arg12             d_a12;
    Arg13             d_a13;
    Arg14             d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    explicit ConstructTestTypeAlloc(bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    {
    }
    ConstructTestTypeAlloc(const ConstructTestTypeAlloc&  other,
                           bslma::Allocator              *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeAlloc(
                      bslmf::MovableRef<ConstructTestTypeAlloc>  other,
                      bslma::Allocator                          *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(MoveUtil::move(MoveUtil::access(other).d_a1))
    , d_a2(MoveUtil::move(MoveUtil::access(other).d_a2))
    , d_a3(MoveUtil::move(MoveUtil::access(other).d_a3))
    , d_a4(MoveUtil::move(MoveUtil::access(other).d_a4))
    , d_a5(MoveUtil::move(MoveUtil::access(other).d_a5))
    , d_a6(MoveUtil::move(MoveUtil::access(other).d_a6))
    , d_a7(MoveUtil::move(MoveUtil::access(other).d_a7))
    , d_a8(MoveUtil::move(MoveUtil::access(other).d_a8))
    , d_a9(MoveUtil::move(MoveUtil::access(other).d_a9))
    , d_a10(MoveUtil::move(MoveUtil::access(other).d_a10))
    , d_a11(MoveUtil::move(MoveUtil::access(other).d_a11))
    , d_a12(MoveUtil::move(MoveUtil::access(other).d_a12))
    , d_a13(MoveUtil::move(MoveUtil::access(other).d_a13))
    , d_a14(MoveUtil::move(MoveUtil::access(other).d_a14))
    {
        ++s_moveConstructorInvocations;
    }

    // In order to distinguish between an optional allocator parameter and a
    // non-optional 'Arg' parameter, we need to explicitly call out the type
    // of the parameter before the optional allocator.
    explicit ConstructTestTypeAlloc(const Arg1&       a1,
                                    bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(a1)
    {
    }
    explicit ConstructTestTypeAlloc(bslmf::MovableRef<Arg1>  a1,
                                    bslma::Allocator        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(MoveUtil::move(a1))
    {
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        const Arg2&                              a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(a2)
    {
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        bslmf::MovableRef<Arg2>                  a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(MoveUtil::move(a2))
    {
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        const Arg3&                              a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(a3)
    {
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        bslmf::MovableRef<Arg3>                  a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(MoveUtil::move(a3))
    {
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        const Arg4&                              a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(a4)
    {
    }
    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        bslmf::MovableRef<Arg4>                  a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(MoveUtil::move(a4))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        const Arg5&                              a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(a5)
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        bslmf::MovableRef<Arg5>                  a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(MoveUtil::move(a5))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        const Arg6&                              a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(a6)
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        bslmf::MovableRef<Arg6>                  a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(MoveUtil::move(a6))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           const Arg7&                             a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(a7)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           bslmf::MovableRef<Arg7>                 a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(MoveUtil::move(a7))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           const Arg8&                             a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(a8)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           bslmf::MovableRef<Arg8>                 a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(MoveUtil::move(a8))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           const Arg9&                             a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(a9)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           bslmf::MovableRef<Arg9>                 a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(MoveUtil::move(a9))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           const Arg10&                            a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(a10)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           bslmf::MovableRef<Arg10>                a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(MoveUtil::move(a10))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           const Arg11&                             a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(a11)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           bslmf::MovableRef<Arg11>                 a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(MoveUtil::move(a11))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           const Arg12&                             a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(a12)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           bslmf::MovableRef<Arg12>                 a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(MoveUtil::move(a12))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           const Arg13&                             a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(a13)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           bslmf::MovableRef<Arg13>                 a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(MoveUtil::move(a13))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           const Arg14&                             a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(a14)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           bslmf::MovableRef<Arg14>                 a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(MoveUtil::move(a14))
    {
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    explicit ConstructTestTypeAlloc(std::initializer_list<int>  il,
                                    bslma::Allocator           *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    {
        for (int i : il)
            d_ilsum += i;
    }
    explicit ConstructTestTypeAlloc(std::initializer_list<int>  il,
                                    const Arg1&                 a1,
                                    bslma::Allocator           *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(a1)
    {
        for (int i : il)
            d_ilsum += i;
    }
    explicit ConstructTestTypeAlloc(std::initializer_list<int>  il,
                                    bslmf::MovableRef<Arg1>     a1,
                                    bslma::Allocator           *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(MoveUtil::move(a1))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        const Arg2&                              a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(a2)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        bslmf::MovableRef<Arg2>                  a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(MoveUtil::move(a2))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        const Arg3&                              a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(a3)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        bslmf::MovableRef<Arg3>                  a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(MoveUtil::move(a3))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        const Arg4&                              a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(a4)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        bslmf::MovableRef<Arg4>                  a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(MoveUtil::move(a4))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        const Arg5&                              a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(a5)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        bslmf::MovableRef<Arg5>                  a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(MoveUtil::move(a5))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        const Arg6&                              a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(a6)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        bslmf::MovableRef<Arg6>                  a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(MoveUtil::move(a6))
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           const Arg7&                             a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(a7)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           bslmf::MovableRef<Arg7>                 a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(MoveUtil::move(a7))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           const Arg8&                             a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(a8)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           bslmf::MovableRef<Arg8>                 a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(MoveUtil::move(a8))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           const Arg9&                             a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(a9)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           bslmf::MovableRef<Arg9>                 a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(MoveUtil::move(a9))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           const Arg10&                            a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(a10)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           bslmf::MovableRef<Arg10>                a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(MoveUtil::move(a10))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           const Arg11&                             a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(a11)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           bslmf::MovableRef<Arg11>                 a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(MoveUtil::move(a11))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           const Arg12&                             a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(a12)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           bslmf::MovableRef<Arg12>                 a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(MoveUtil::move(a12))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           const Arg13&                             a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(a13)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           bslmf::MovableRef<Arg13>                 a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(MoveUtil::move(a13))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           const Arg14&                             a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(a14)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           bslmf::MovableRef<Arg14>                 a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(MoveUtil::move(a14))
    {
        for (int i : il)
            d_ilsum += i;
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    bsl::allocator<char> get_allocator() const { return d_allocator_p; }
};

int ConstructTestTypeAlloc::s_copyConstructorInvocations   = 0;
int ConstructTestTypeAlloc::s_moveConstructorInvocations   = 0;
int ConstructTestTypeAlloc::s_lvalueConstructorInvocations = 0;
int ConstructTestTypeAlloc::s_rvalueConstructorInvocations = 0;
int ConstructTestTypeAlloc::s_copyAssignmentInvocations    = 0;
int ConstructTestTypeAlloc::s_moveAssignmentInvocations    = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeAlloc& lhs,
                const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_ilsum == rhs.d_ilsum &&
           lhs.d_a1.d_value == rhs.d_a1.d_value &&
           lhs.d_a2.d_value == rhs.d_a2.d_value &&
           lhs.d_a3.d_value == rhs.d_a3.d_value &&
           lhs.d_a4.d_value == rhs.d_a4.d_value &&
           lhs.d_a5.d_value == rhs.d_a5.d_value &&
           lhs.d_a6.d_value == rhs.d_a6.d_value &&
           lhs.d_a7.d_value == rhs.d_a7.d_value &&
           lhs.d_a8.d_value == rhs.d_a8.d_value &&
           lhs.d_a9.d_value == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}
bool createdAlike(const ConstructTestTypeAlloc& lhs,
                  const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_a1.d_copyCount == rhs.d_a1.d_copyCount &&
           lhs.d_a2.d_copyCount == rhs.d_a2.d_copyCount &&
           lhs.d_a3.d_copyCount == rhs.d_a3.d_copyCount &&
           lhs.d_a4.d_copyCount == rhs.d_a4.d_copyCount &&
           lhs.d_a5.d_copyCount == rhs.d_a5.d_copyCount &&
           lhs.d_a6.d_copyCount == rhs.d_a6.d_copyCount &&
           lhs.d_a7.d_copyCount == rhs.d_a7.d_copyCount &&
           lhs.d_a8.d_copyCount == rhs.d_a8.d_copyCount &&
           lhs.d_a9.d_copyCount == rhs.d_a9.d_copyCount &&
           lhs.d_a10.d_copyCount == rhs.d_a10.d_copyCount &&
           lhs.d_a11.d_copyCount == rhs.d_a11.d_copyCount &&
           lhs.d_a12.d_copyCount == rhs.d_a12.d_copyCount &&
           lhs.d_a13.d_copyCount == rhs.d_a13.d_copyCount &&
           lhs.d_a14.d_copyCount == rhs.d_a14.d_copyCount;
}
                      // ================================
                      // class ConstructTestTypeAllocArgT
                      // ================================

class ConstructTestTypeAllocArgT {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAllocArgT' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class takes an allocator using the
    // 'allocator_arg_t' protocol.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1  // default value for all private data
    };

  public:

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructTestTypeAllocArgT, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructTestTypeAllocArgT, bslmf::UsesAllocatorArgT);

    // PUBLIC DATA
    static int        s_copyConstructorInvocations;
    static int        s_moveConstructorInvocations;
    static int        s_lvalueConstructorInvocations;
    static int        s_rvalueConstructorInvocations;
    static int        s_copyAssignmentInvocations;
    static int        s_moveAssignmentInvocations;


    int               d_ilsum;
    bslma::Allocator *d_allocator_p;
    Arg1              d_a1;
    Arg2              d_a2;
    Arg3              d_a3;
    Arg4              d_a4;
    Arg5              d_a5;
    Arg6              d_a6;
    Arg7              d_a7;
    Arg8              d_a8;
    Arg9              d_a9;
    Arg10             d_a10;
    Arg11             d_a11;
    Arg12             d_a12;
    Arg13             d_a13;
    Arg14             d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    ConstructTestTypeAllocArgT()
    : d_ilsum(0)
    , d_allocator_p(0)
    {
    }

    ConstructTestTypeAllocArgT(ConstructTestTypeAllocArgT const& other)
    : d_ilsum(0)
    , d_allocator_p(0)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(
                           bslmf::MovableRef<ConstructTestTypeAllocArgT> other)
    : d_ilsum(0)
    , d_allocator_p(MoveUtil::access(other).d_allocator_p)
    , d_a1(MoveUtil::move(MoveUtil::access(other).d_a1))
    , d_a2(MoveUtil::move(MoveUtil::access(other).d_a2))
    , d_a3(MoveUtil::move(MoveUtil::access(other).d_a3))
    , d_a4(MoveUtil::move(MoveUtil::access(other).d_a4))
    , d_a5(MoveUtil::move(MoveUtil::access(other).d_a5))
    , d_a6(MoveUtil::move(MoveUtil::access(other).d_a6))
    , d_a7(MoveUtil::move(MoveUtil::access(other).d_a7))
    , d_a8(MoveUtil::move(MoveUtil::access(other).d_a8))
    , d_a9(MoveUtil::move(MoveUtil::access(other).d_a9))
    , d_a10(MoveUtil::move(MoveUtil::access(other).d_a10))
    , d_a11(MoveUtil::move(MoveUtil::access(other).d_a11))
    , d_a12(MoveUtil::move(MoveUtil::access(other).d_a12))
    , d_a13(MoveUtil::move(MoveUtil::access(other).d_a13))
    , d_a14(MoveUtil::move(MoveUtil::access(other).d_a14))
    {
        ++s_moveConstructorInvocations;
    }
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                  *alloc,
                               ConstructTestTypeAllocArgT const&  other)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(
                          bsl::allocator_arg_t,
                          bslma::Allocator                              *alloc,
                          bslmf::MovableRef<ConstructTestTypeAllocArgT>  other)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(MoveUtil::move(MoveUtil::access(other).d_a1))
    , d_a2(MoveUtil::move(MoveUtil::access(other).d_a2))
    , d_a3(MoveUtil::move(MoveUtil::access(other).d_a3))
    , d_a4(MoveUtil::move(MoveUtil::access(other).d_a4))
    , d_a5(MoveUtil::move(MoveUtil::access(other).d_a5))
    , d_a6(MoveUtil::move(MoveUtil::access(other).d_a6))
    , d_a7(MoveUtil::move(MoveUtil::access(other).d_a7))
    , d_a8(MoveUtil::move(MoveUtil::access(other).d_a8))
    , d_a9(MoveUtil::move(MoveUtil::access(other).d_a9))
    , d_a10(MoveUtil::move(MoveUtil::access(other).d_a10))
    , d_a11(MoveUtil::move(MoveUtil::access(other).d_a11))
    , d_a12(MoveUtil::move(MoveUtil::access(other).d_a12))
    , d_a13(MoveUtil::move(MoveUtil::access(other).d_a13))
    , d_a14(MoveUtil::move(MoveUtil::access(other).d_a14))
    {
        ++s_moveConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t, bslma::Allocator *alloc)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    {
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator     *alloc,
                               const Arg1&           a1)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(a1)
    {
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator        *alloc,
                               bslmf::MovableRef<Arg1>  a1)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(MoveUtil::move(a1))
    {
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14)  a14)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator           *alloc,
                               std::initializer_list<int>  il)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14)  a14)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
        for (int i : il)
            d_ilsum += i;
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    bsl::allocator<char> get_allocator() const { return d_allocator_p; }
};


int ConstructTestTypeAllocArgT::s_copyConstructorInvocations   = 0;
int ConstructTestTypeAllocArgT::s_moveConstructorInvocations   = 0;
int ConstructTestTypeAllocArgT::s_lvalueConstructorInvocations = 0;
int ConstructTestTypeAllocArgT::s_rvalueConstructorInvocations = 0;
int ConstructTestTypeAllocArgT::s_copyAssignmentInvocations    = 0;
int ConstructTestTypeAllocArgT::s_moveAssignmentInvocations    = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeAllocArgT& lhs,
                const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_value == rhs.d_a1.d_value &&
           lhs.d_a2.d_value == rhs.d_a2.d_value &&
           lhs.d_a3.d_value == rhs.d_a3.d_value &&
           lhs.d_a4.d_value == rhs.d_a4.d_value &&
           lhs.d_a5.d_value == rhs.d_a5.d_value &&
           lhs.d_a6.d_value == rhs.d_a6.d_value &&
           lhs.d_a7.d_value == rhs.d_a7.d_value &&
           lhs.d_a8.d_value == rhs.d_a8.d_value &&
           lhs.d_a9.d_value == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}
bool createdAlike(const ConstructTestTypeAllocArgT& lhs,
                  const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_copyCount == rhs.d_a1.d_copyCount &&
           lhs.d_a2.d_copyCount == rhs.d_a2.d_copyCount &&
           lhs.d_a3.d_copyCount == rhs.d_a3.d_copyCount &&
           lhs.d_a4.d_copyCount == rhs.d_a4.d_copyCount &&
           lhs.d_a5.d_copyCount == rhs.d_a5.d_copyCount &&
           lhs.d_a6.d_copyCount == rhs.d_a6.d_copyCount &&
           lhs.d_a7.d_copyCount == rhs.d_a7.d_copyCount &&
           lhs.d_a8.d_copyCount == rhs.d_a8.d_copyCount &&
           lhs.d_a9.d_copyCount == rhs.d_a9.d_copyCount &&
           lhs.d_a10.d_copyCount == rhs.d_a10.d_copyCount &&
           lhs.d_a11.d_copyCount == rhs.d_a11.d_copyCount &&
           lhs.d_a12.d_copyCount == rhs.d_a12.d_copyCount &&
           lhs.d_a13.d_copyCount == rhs.d_a13.d_copyCount &&
           lhs.d_a14.d_copyCount == rhs.d_a14.d_copyCount;
}

const MyClass1       V1(1);
const MyClass2       V2(2);

ConstructTestArg<1>  VA1(1);
ConstructTestArg<2>  VA2(2);
ConstructTestArg<3>  VA3(3);
ConstructTestArg<4>  VA4(4);
ConstructTestArg<5>  VA5(5);
ConstructTestArg<6>  VA6(6);
ConstructTestArg<7>  VA7(7);
ConstructTestArg<8>  VA8(8);
ConstructTestArg<9>  VA9(9);
ConstructTestArg<10> VA10(10);
ConstructTestArg<11> VA11(11);
ConstructTestArg<12> VA12(12);
ConstructTestArg<13> VA13(13);
ConstructTestArg<14> VA14(14);

struct Swappable {
    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    int d_value;

    // CLASS METHODS
    static bool swapCalled() { return 0 != s_swapCalled; }

    static void swapReset() { s_swapCalled = 0; }

    // CREATORS
    explicit Swappable(int v)
    : d_value(v)
    {
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.d_value == rhs.d_value;
}

// PUBLIC CLASS DATA
int Swappable::s_swapCalled = 0;

void swap(Swappable& a, Swappable& b)
{
    ++Swappable::s_swapCalled;

    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

struct SwappableAA {
    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    MyClassDef d_def;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SwappableAA, bslma::UsesBslmaAllocator);


    // CLASS METHODS
    static bool swapCalled() { return 0 != s_swapCalled; }

    static void swapReset() { s_swapCalled = 0; }

    // CREATORS
    explicit SwappableAA(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = a;
    }

    SwappableAA(const SwappableAA& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    SwappableAA(bslmf::MovableRef<SwappableAA> other, bslma::Allocator *a = 0)
    {
        SwappableAA& otherRef  = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = k_MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }
};

// FREE OPERATORS
bool operator==(const SwappableAA& lhs, const SwappableAA& rhs)
{
    return lhs.d_def.d_value == rhs.d_def.d_value;
}

// PUBLIC CLASS DATA
int SwappableAA::s_swapCalled = 0;

void swap(SwappableAA& a, SwappableAA& b)
{
    ++SwappableAA::s_swapCalled;

    bslalg::SwapUtil::swap(&a.d_def.d_value, &b.d_def.d_value);
}

                              // ============================
                              // class ThrowMoveConstructible
                              // ============================

template <bool SWAP_NOEXCEPT>
struct ThrowMoveConstructible {
    // Type with throwing move constructor.

    // CREATORS
    ThrowMoveConstructible()
        // Create a 'ThrowMoveConstructible' object.
    {
    }
    ThrowMoveConstructible(bslmf::MovableRef<ThrowMoveConstructible> )
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowMoveConstructible' object.
    {
    }

    // MANIPULATORS
    void swap(ThrowMoveConstructible& other)
                             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(SWAP_NOEXCEPT)
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        (void) other;
    }
    // FREE FUNCTIONS
    friend void swap(ThrowMoveConstructible& a,
                     ThrowMoveConstructible& b)
                             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(SWAP_NOEXCEPT)
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        (void) a;
        (void) b;
    }
};

                                // ------------
                                // Test Case 26
                                // ------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) &&               \
   !(defined(BSLS_PLATFORM_CMP_CLANG) &&                                      \
     defined(BSLS_LIBRARYFEATURES_STDCPP_GNU))               &&               \
   !(defined(BSLS_PLATFORM_CMP_CLANG) &&                                      \
     defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM))
// Ensure that we can do 'bsl::optional<bslma::ManagedPtr<void> > on C++17.  As
// per DRQS 168171178.  Note that there are differences in implementation
// between pre-C++17 and C++17.

// This is a compile-only test, and only test on C++17.

template class bsl::optional<bslma::ManagedPtr<void>>;
template class std::optional<bslma::ManagedPtr<void>>;
#endif

                                // ------------
                                // Test Case 22
                                // ------------

namespace BloombergLP {
namespace bslh {

template <class HASHALG, class RETURN, class CLASS>
void hashAppend(HASHALG& hashAlg, RETURN (CLASS:: *member)())
{
    hashAlg(&member, sizeof(member));
}

template <class HASHALG, class RETURN, class CLASS>
void hashAppend(HASHALG& hashAlg, RETURN (CLASS:: *member)() const)
{
    hashAlg(&member, sizeof(member));
}

}  // close namespace bslh
}  // close enterprise namespace

// helper functions to determine the type of a reference
bool isConstRef(const MyClass1&)
{
    return true;
}
bool isConstRef(MyClass1&)
{
    return false;
}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
bool isConstRef(MyClass1&&)
{
    return false;
}
bool isConstRef(const MyClass1&&)
{
    return true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
bool isRvalueRef(const MyClass1&)
{
    return false;
}
bool isRvalueRef(MyClass1&)
{
    return false;
}
bool isRvalueRef(MyClass1&&)
{
    return true;
}
bool isRvalueRef(const MyClass1&&)
{
    return true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

bool isConstRef(const MyClass2&)
{
    return true;
}
bool isConstRef(MyClass2&)
{
    return false;
}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
bool isConstRef(MyClass2&&)
{
    return false;
}
bool isConstRef(const MyClass2&&)
{
    return true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
bool isRvalueRef(const MyClass2&)
{
    return false;
}
bool isRvalueRef(MyClass2&)
{
    return false;
}
bool isRvalueRef(MyClass2&&)
{
    return true;
}
bool isRvalueRef(const MyClass2&&)
{
    return true;
}
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

// helper functions to determine the constness of a pointer
template <typename T>
bool isConstPtr(T *)
{
    return false;
}
template <typename T>
bool isConstPtr(const T *)
{
    return true;
}

#define TEST_HELPER_SET_EXP_COUNTS                                            \
    int              expCopy   = ValueType::s_copyConstructorInvocations;     \
    int              expMove   = ValueType::s_moveConstructorInvocations;     \
    int              expLvalue = ValueType::s_lvalueConstructorInvocations;   \
    int              expRvalue = ValueType::s_rvalueConstructorInvocations;   \
    int              expCopyA  = ValueType::s_copyAssignmentInvocations;      \
    int              expMoveA  = ValueType::s_moveAssignmentInvocations;      \

#define TEST_HELPER_UPDATE_EXP_COUNTS                                         \
    expCopy   = ValueType::s_copyConstructorInvocations   - expCopy;          \
    expMove   = ValueType::s_moveConstructorInvocations   - expMove;          \
    expLvalue = ValueType::s_lvalueConstructorInvocations - expLvalue;        \
    expRvalue = ValueType::s_rvalueConstructorInvocations - expRvalue;        \
    expCopyA  = ValueType::s_copyAssignmentInvocations    - expCopyA;         \
    expMoveA  = ValueType::s_moveAssignmentInvocations    - expMoveA;         \

#define TEST_HELPER_SET_NUM_COUNTS                                            \
    int          numCopy   = ValueType::s_copyConstructorInvocations;         \
    int          numMove   = ValueType::s_moveConstructorInvocations;         \
    int          numLvalue = ValueType::s_lvalueConstructorInvocations;       \
    int          numRvalue = ValueType::s_rvalueConstructorInvocations;       \
    int          numCopyA  = ValueType::s_copyAssignmentInvocations;          \
    int          numMoveA  = ValueType::s_moveAssignmentInvocations;          \

#define TEST_HELPER_UPDATE_NUM_COUNTS                                         \
    numCopy   = ValueType::s_copyConstructorInvocations   - numCopy;          \
    numMove   = ValueType::s_moveConstructorInvocations   - numMove;          \
    numLvalue = ValueType::s_lvalueConstructorInvocations - numLvalue;        \
    numRvalue = ValueType::s_rvalueConstructorInvocations - numRvalue;        \
    numCopyA  = ValueType::s_copyAssignmentInvocations    - numCopyA;         \
    numMoveA  = ValueType::s_moveAssignmentInvocations    - numMoveA;         \

#define TEST_HELPER_CHECK_COUNTS                                              \
    ASSERTV(expMove,   numMove,   expMove   == numMove);                      \
    ASSERTV(expCopy,   numCopy,   expCopy   == numCopy);                      \
    ASSERTV(expLvalue, numLvalue, expLvalue == numLvalue);                    \
    ASSERTV(expRvalue, numRvalue, expRvalue == numRvalue);                    \
    ASSERTV(expMoveA,  numMoveA,  expMoveA  == numMoveA);                     \
    ASSERTV(expCopyA,  numCopyA,  expCopyA  == numCopyA);                     \

#define TEST_EMPLACE(expArgs, emplaceArgs)                                    \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        ValWithAllocator valBuffer expArgs;                                   \
        ValueType&                 EXP = valBuffer.object();                  \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj     = objBuffer.object();                        \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        ValueType& retVal        = obj.emplace emplaceArgs;                   \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(EXP == obj.value());                                           \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
        ASSERT(createdAlike(EXP, obj.value()) == true);                       \
        ASSERT(BSLS_UTIL_ADDRESSOF(obj.value()) ==                            \
               BSLS_UTIL_ADDRESSOF(retVal));                                  \
    }

#define TEST_ASSIGN_VAL_EMPTY(source)                                         \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        ValueType expVal(source);                                             \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = source;                                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_MOVE_ASSIGN_VAL_EMPTY(source)                                    \
    {                                                                         \
        ValueType srcCopy(source);                                            \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        ValueType expVal(MoveUtil::move(srcCopy));                            \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = MoveUtil::move(source);                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_ASSIGN_VAL_ENGAGED(source)                                       \
    {                                                                         \
        ValueType expVal;                                                     \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        expVal            = source;                                           \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        obj.emplace();                                                        \
        ASSERT(obj.has_value());                                              \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = source;                                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }

#define TEST_MOVE_ASSIGN_VAL_ENGAGED(SourceType, source)                      \
    {                                                                         \
        SourceType srcCopy(source);                                           \
        ValueType expVal;                                                     \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        expVal            = MoveUtil::move(srcCopy);                          \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        obj.emplace();                                                        \
        ASSERT(obj.has_value());                                              \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = MoveUtil::move(source);                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(source)                              \
    {                                                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        obj = source;                                                         \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &oa));                                     \
    }
#define TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(source)                            \
    {                                                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        obj.emplace(ValueType());                                             \
        ASSERT(obj.has_value());                                              \
        int numDest = ValueType::s_destructorInvocations;                     \
        obj         = source;                                                 \
        numDest     = ValueType::s_destructorInvocations - numDest;           \
        ASSERT(1 == numDest);                                                 \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &oa));                                     \
    }

#define TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source)                            \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        ValueType expVal(source.value());                                    \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = source;                                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }

#define TEST_MOVE_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source)                       \
    {                                                                         \
        OPT_TYPE  srcCopy(source);                                            \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        ValueType expVal(MoveUtil::move(srcCopy.value()));                    \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = MoveUtil::move(source);                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(source.value() == srcCopy.value());                            \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source)                          \
    {                                                                         \
        ValueType expVal;                                                     \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        expVal            = source.value();                                   \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        obj.emplace();                                                        \
        ASSERT(obj.has_value());                                              \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = source;                                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_MOVE_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source)                     \
    {                                                                         \
        OPT_TYPE  srcCopy(source);                                            \
        ValueType expVal;                                                     \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        expVal            = MoveUtil::move(srcCopy.value());                  \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        obj.emplace();                                                        \
        ASSERT(obj.has_value());                                              \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        obj         = MoveUtil::move(source);                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
        ASSERT(source.value() == srcCopy.value());                            \
    }

#define TEST_COPY_FROM_EMPTY_OPT(source)                                      \
    {                                                                         \
        bslma::TestAllocatorMonitor dam(&da);                                 \
        ASSERT(!source.has_value());                                          \
        bsl::optional<DEST_TYPE> obj(source);                                 \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_MOVE_FROM_EMPTY_OPT(source, propagate)                           \
    {                                                                         \
        bslma::TestAllocator&       expAlloc = (propagate ? oa : da);         \
        bslma::TestAllocatorMonitor dam(&expAlloc);                           \
        ASSERT(!source.has_value());                                          \
        bsl::optional<DEST_TYPE> obj(MoveUtil::move(source));                 \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &expAlloc));                               \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_COPY_FROM_ENGAGED_OPT(source)                                    \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(source.value());                                     \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<DEST_TYPE> obj(source);                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }
#define TEST_MOVE_FROM_ENGAGED_OPT(source, propagate)                         \
    {                                                                         \
        SRC_OPT_TYPE          srcCopy(source);                                \
        bslma::TestAllocator& expAlloc = (propagate ? oa : da);               \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(MoveUtil::move(srcCopy.value()));                    \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<DEST_TYPE> obj(MoveUtil::move(source));                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &expAlloc));                               \
        ASSERT(checkAllocator(obj.value(), &expAlloc));                       \
    }

#define TEST_EXT_COPY_FROM_EMPTY_OPT(source)                                  \
    {                                                                         \
        bslma::TestAllocatorMonitor dam(&da);                                 \
        ASSERT(!source.has_value());                                          \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, source);                   \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_EXT_MOVE_FROM_EMPTY_OPT(source)                                  \
    {                                                                         \
        bslma::TestAllocatorMonitor dam(&da);                                 \
        ASSERT(!source.has_value());                                          \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));   \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_EXT_COPY_FROM_ENGAGED_OPT(source)                                \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(source.value());                                     \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, source);                   \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }
#define TEST_EXT_MOVE_FROM_ENGAGED_OPT(source)                                \
    {                                                                         \
        SRC_OPT_TYPE srcCopy(source);                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE    expVal(MoveUtil::move(srcCopy.value()));                 \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));   \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }

#define TEST_COPY_FROM_VALUE(source)                                          \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(source);                                             \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<DEST_TYPE> obj(source);                                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }
#define TEST_MOVE_FROM_VALUE(source)                                          \
    {                                                                         \
        SRC_TYPE  srcCopy(source);                                            \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(MoveUtil::move(srcCopy));                            \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<DEST_TYPE> obj(MoveUtil::move(source));                 \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }

#define TEST_EXT_COPY_FROM_VALUE(source)                                      \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(source);                                             \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, source);                   \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }
#define TEST_EXT_MOVE_FROM_VALUE(source)                                      \
    {                                                                         \
        SRC_TYPE  srcCopy(source);                                            \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        DEST_TYPE expVal(MoveUtil::move(srcCopy));                            \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));   \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }

#define TEST_IN_PLACE_CONSTRUCT(init, expArgs, expAlloc)                      \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        ValWithAllocator valBuffer expArgs;                                   \
        ValueType&                 EXP = valBuffer.object();                  \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        Obj obj init;                                                         \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(EXP == obj.value());                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(checkAllocator(obj, expAlloc));                                \
        ASSERT(checkAllocator(obj.value(), expAlloc));                        \
    }

#define TEST_MAKE_OPTIONAL_DEDUCED(source)                                    \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        bsl::optional<ValueType> expObj(bsl::in_place, source);               \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<ValueType> obj = bsl::make_optional(source);            \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == source);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }

#define TEST_MAKE_OPTIONAL(expArgs, args)                                     \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        bsl::optional<ValueType> expObj expArgs;                              \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<ValueType> obj = bsl::make_optional<ValueType> args;    \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
        ASSERT(createdAlike(expObj.value(), obj.value()) == true);            \
    }

#define TEST_MAKE_OPTIONAL_WITH_ALLOC_DEDUCED(source)                         \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        bsl::optional<ValueType> expObj(bsl::in_place, source);               \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<ValueType> obj =                                        \
            bsl::make_optional(bsl::allocator_arg, &ta, source);              \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == source);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }

#define TEST_MAKE_OPTIONAL_WITH_ALLOC(expArgs, args, alloc)                   \
    {                                                                         \
        TEST_HELPER_SET_EXP_COUNTS                                            \
        bsl::optional<ValueType> expObj expArgs;                              \
        TEST_HELPER_UPDATE_EXP_COUNTS                                         \
        TEST_HELPER_SET_NUM_COUNTS                                            \
        bsl::optional<ValueType> obj = bsl::make_optional<ValueType> args;    \
        TEST_HELPER_UPDATE_NUM_COUNTS                                         \
        TEST_HELPER_CHECK_COUNTS                                              \
        ASSERT(checkAllocator(obj, alloc));                                   \
        ASSERT(checkAllocator(obj.value(), alloc));                           \
        ASSERT(createdAlike(expObj.value(), obj.value()) == true);            \
    }

template <class TYPE,
          bool USES_BSLMA_ALLOC =
              bslma::UsesBslmaAllocator<TYPE>::value>
class Test_Util {
    // This class provided test utilities which have different behaviour
    // depending on whether 'TYPE is allocator-aware or not.  The main template
    // is for allocator-aware types.

  public:
    static bool checkAllocator(const TYPE&                 obj,
                               const bsl::allocator<char>& expected);
        // Check if, for the specified 'obj', 'obj.get_allocator()' returns the
        // specified 'expected' allocator.

    static bool hasSameAllocator(const TYPE& obj, const TYPE& other);
        // Check if, for the specified 'obj' and specified 'other',
        // 'obj.get_allocator() == other.get_allocator()';
};

template <class TYPE>
class Test_Util<TYPE, false> {
    // This class provided test utilities which have different behaviour
    // depending on whether 'TYPE is allocator-aware or not.  This
    // specialization is for non allocator-aware types.

  public:
    static bool checkAllocator(const TYPE&, const bsl::allocator<char>&);
        // return 'true'.

    static bool hasSameAllocator(const TYPE&, const TYPE&);
        // return 'true'.
};

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool Test_Util<TYPE, USES_BSLMA_ALLOC>::checkAllocator(
                                          const TYPE&                 obj,
                                          const bsl::allocator<char>& expected)
{
    return (expected == obj.get_allocator());
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool Test_Util<TYPE, USES_BSLMA_ALLOC>::hasSameAllocator(const TYPE& obj,
                                                         const TYPE& other)
{
    return (obj.get_allocator() == other.get_allocator());
}

template <class TYPE>
inline bool
Test_Util<TYPE, false>::checkAllocator(const TYPE&,
                                       const bsl::allocator<char>&)
{
    return true;
}

template <class TYPE>
inline bool
Test_Util<TYPE, false>::hasSameAllocator(const TYPE&, const TYPE&)
{
    return true;
}

template <class TYPE>
bool checkAllocator(const TYPE& obj, const bsl::allocator<char>& allocator)
{
    return Test_Util<TYPE>::checkAllocator(obj, allocator);
}

template <class TYPE>
bool hasSameAllocator(const TYPE& obj1, const TYPE& obj2)
{
    return Test_Util<TYPE>::hasSameAllocator(obj1, obj2);
}

                                // ------------
                                // Test Case 25
                                // ------------

namespace TEST_CASE_25 {

struct Src {};

struct ImplicitDst {
    // Non-allocating class implicitly constructible and move-constructible
    // from 'Src'.

    ImplicitDst(const Src&) {}
    ImplicitDst(bslmf::MovableRef<Src>) {}
};

struct ImplicitDstAlloc {
    // Allocating class implicitly constructible and move-constructible from
    // 'Src'.

    BSLMF_NESTED_TRAIT_DECLARATION(ImplicitDstAlloc,
                                   bslma::UsesBslmaAllocator);

    ImplicitDstAlloc(const Src&) {}
    ImplicitDstAlloc(bslmf::MovableRef<Src>) {}
};

struct ExplicitDst {
    // Non-allocating class explicitly constructible and move-constructible
    // from 'Src'.

    explicit ExplicitDst(const Src&) {}
    explicit ExplicitDst(bslmf::MovableRef<Src>) {}
};

struct ExplicitDstAlloc {
    // Allocating class explicitly constructible and move-constructible from
    // 'Src'.

    BSLMF_NESTED_TRAIT_DECLARATION(ExplicitDstAlloc,
                                   bslma::UsesBslmaAllocator);

    explicit ExplicitDstAlloc(const Src&) {}
    explicit ExplicitDstAlloc(bslmf::MovableRef<Src>) {}
};

void testCase25()
{
    ASSERT(( bsl::is_convertible<Src, ImplicitDst>::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<Src>, ImplicitDst>::value));

    ASSERT(( bsl::is_convertible<Src, ImplicitDstAlloc>::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<Src>,
                                 ImplicitDstAlloc>::value));

    ASSERT((!bsl::is_convertible<Src, ExplicitDst>::value));
    ASSERT((!bsl::is_convertible<bslmf::MovableRef<Src>, ExplicitDst>::value));

    ASSERT((!bsl::is_convertible<Src, ExplicitDstAlloc>::value));
    ASSERT((!bsl::is_convertible<bslmf::MovableRef<Src>,
                                 ExplicitDstAlloc>::value));

    using bsl::optional;

    ASSERT(( bsl::is_convertible<Src, optional<Src> >::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<Src>,
                                 optional<Src> >::value));

    ASSERT(( bsl::is_convertible<Src, optional<ImplicitDst> >::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<Src>,
                                 optional<ImplicitDst> >::value));

    ASSERT(( bsl::is_convertible<Src, optional<ImplicitDstAlloc> >::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<Src>,
                                 optional<ImplicitDstAlloc> >::value));

    ASSERT((!bsl::is_convertible<Src, optional<ExplicitDst> >::value));
    ASSERT((!bsl::is_convertible<bslmf::MovableRef<Src>,
                                 optional<ExplicitDst> >::value));

    ASSERT((!bsl::is_convertible<Src, optional<ExplicitDstAlloc> >::value));
    ASSERT((!bsl::is_convertible<bslmf::MovableRef<Src>,
                                 optional<ExplicitDstAlloc> >::value));

    ASSERT(( bsl::is_convertible<optional<Src>,
                                 optional<ImplicitDst> >::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<optional<Src> >,
                                 optional<ImplicitDst> >::value));

    ASSERT(( bsl::is_convertible<optional<Src>,
                                 optional<ImplicitDstAlloc> >::value));
    ASSERT(( bsl::is_convertible<bslmf::MovableRef<optional<Src> >,
                                 optional<ImplicitDstAlloc> >::value));

    ASSERT((!bsl::is_convertible<optional<Src>,
                                 optional<ExplicitDst> >::value));
    ASSERT((!bsl::is_convertible<bslmf::MovableRef<optional<Src> >,
                                 optional<ExplicitDst> >::value));

    ASSERT((!bsl::is_convertible<optional<Src>,
                                 optional<ExplicitDstAlloc> >::value));
    ASSERT((!bsl::is_convertible<bslmf::MovableRef<optional<Src> >,
                                 optional<ExplicitDstAlloc> >::value));

#if 201103L <= BSLS_COMPILERFEATURES_CPLUSPLUS
    ASSERT(( std::is_constructible<optional<Src>, Src>::value));
    ASSERT(( std::is_constructible<optional<Src>,
                                   bslmf::MovableRef<Src> >::value));

    ASSERT(( std::is_constructible<optional<ImplicitDst>, Src>::value));
    ASSERT(( std::is_constructible<optional<ImplicitDst>,
                                   bslmf::MovableRef<Src> >::value));

    ASSERT(( std::is_constructible<optional<ImplicitDstAlloc>, Src>::value));
    ASSERT(( std::is_constructible<optional<ImplicitDstAlloc>,
                                   bslmf::MovableRef<Src> >::value));

    ASSERT(( std::is_constructible<optional<ExplicitDst>, Src>::value));
    ASSERT(( std::is_constructible<optional<ExplicitDst>,
                                   bslmf::MovableRef<Src> >::value));

    ASSERT(( std::is_constructible<optional<ExplicitDstAlloc>, Src>::value));
    ASSERT(( std::is_constructible<optional<ExplicitDstAlloc>,
                                   bslmf::MovableRef<Src> >::value));

    ASSERT(( std::is_constructible<optional<ImplicitDst>,
                                   optional<Src> >::value));
    ASSERT(( std::is_constructible<optional<ImplicitDst>,
                                   bslmf::MovableRef<optional<Src> > >
                                                                     ::value));

    ASSERT(( std::is_constructible<optional<ImplicitDstAlloc>,
                                   optional<Src> >::value));
    ASSERT(( std::is_constructible<optional<ImplicitDstAlloc>,
                                   bslmf::MovableRef<optional<Src> > >
                                                                     ::value));

    ASSERT(( std::is_constructible<optional<ExplicitDst>,
                                   optional<Src> >::value));
    ASSERT(( std::is_constructible<optional<ExplicitDst>,
                                   bslmf::MovableRef<optional<Src> > >
                                                                     ::value));

    ASSERT(( std::is_constructible<optional<ExplicitDstAlloc>,
                                   optional<Src> >::value));
    ASSERT(( std::is_constructible<optional<ExplicitDstAlloc>,
                                   bslmf::MovableRef<optional<Src> > >
                                                                     ::value));
#endif
}

}  // close namespace TEST_CASE_25



                              // ---------------
                              // Test Case 23-24
                              // ---------------

template <class TYPE,
          bool  USES_BSLMA_ALLOC =
                           BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
struct Derived;

template <class TYPE>
struct Derived<TYPE, true> : bsl::optional<TYPE, true> {
    // CREATORS
    Derived(bsl::allocator_arg_t aarg,
            bsl::allocator<char> alloc,
            const TYPE&          value)
    : bsl::optional<TYPE, true>(aarg, alloc, value)
    {
    }
};

template <class TYPE>
struct Derived<TYPE, false> : bsl::optional<TYPE, false> {
    // CREATORS
    Derived(const TYPE& value)
    : bsl::optional<TYPE, false>(value)
    {
    }
};

                              // ------------
                              // Test Case 22
                              // ------------

struct EasyConvert {
    // This 'struct' is intended to mimic the problematic aspects of
    // 'bdef_Function'.  It can be:
    //: o default-constructed
    //:
    //: o copy-constructed
    //:
    //: o copy-assigned
    //:
    //: o constructed from other types, but fails when this happens
    //:
    //: o assigned from other types, but fails when this happens

    // CREATORS
    EasyConvert() {}

    template <class ANY_TYPE>
    EasyConvert(const ANY_TYPE& a) { a.nonExistent(); }

    EasyConvert(const EasyConvert&) {}

    // MANIPULATORS
    EasyConvert& operator=(const EasyConvert&) { return *this; }

    template <class ANY_TYPE>
    EasyConvert& operator=(const ANY_TYPE& a)
    {
        a.nonExistent();

        return *this;
    }
};

                              // ------------
                              // Test Case 14
                              // ------------
struct CustomHashAlgorithm {
    // A custom hash algorithm class type used for verifying that custom
    // (non-'bslh') hash types compile.

    typedef bsls::Types::Int64 result_type;

    void operator()(const void *, size_t)
        // Do nothing.
    {
    }

    result_type computeHash()
        // Unconditionally return 0.
    {
        return 0;
    }
};

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TYPE>
class TestDriver {
    // This class template provides a namespace for testing the 'optional'
    // type.

  private:
    // PRIVATE TYPES
    typedef TYPE ValueType;
        // ValueType under test.

    typedef bsl::optional<ValueType>             Obj;
    typedef bsl::optional<const ValueType>       ObjC;
    typedef const bsl::optional<ValueType>       CObj;
    typedef const bsl::optional<const ValueType> CObjC;
        // Type under test.

    typedef bslalg::ConstructorProxy<Obj> ObjWithAllocator;
        // Wrapper for 'Obj' whose constructor takes an allocator.

    typedef bslalg::ConstructorProxy<ValueType> ValWithAllocator;
        // Wrapper for 'ValueType' whose constructor takes an allocator.

    typedef bsltf::TestValuesArray<TYPE> TestValues;
        // Array of test values of 'TYPE'.

  public:

    static void testCase24();
        // TESTING DERIVED -- 'TYPE' DOES NOT ALLOCATE

    static void testCase23();
        // TESTING DERIVED -- 'TYPE' ALLOCATES

    static void testCase20();
        // TESTING NOEXCEPT

    static void testCase19();
        // TESTING DRQS 165776192

    static void testCase18();
        // TESTING type deduction

    static void testCase17();
    static void testCase17b();
        // TESTING 'make_optional' ALLOCATOR EXTENDED FACILITY

    static void testCase16();
    static void testCase16b();
        // TESTING 'make_optional' FACILITY


    static void testCase15();
    static void testCase15b();
        // TESTING TRAITS AND TYPEDEFS

    static void testCase14();
        // TESTING 'hashAppend'


    static void testCase13();
    static void testCase13_imp_a();
    static void testCase13_imp_b();
        // TESTING 'emplace' METHOD.  Note that this test can only be executed
        // with types that are constructible from an integer, have a 'value()'
        // method, and which provide a 's_copyConstructorInvocations' and
        // 's_moveConstructorInvocations' static variable that counts the
        // number of times copy/move constructor has been invoked.

    static void testCase10d();
        // TESTING 'operator=' OVERLOAD RESOLUTION

    static void testCase10c();
        // TESTING 'operator=(non_optional_type)'

    template <class OPT_TYPE>
    static void testCase10b_imp();
    static void testCase10b();
        // TESTING 'operator=(optional_type)'

    static void testCase10a_imp();
    static void testCase10a();
        // TESTING operator=(nullopt_t)

    template <class DEST_TYPE, class SRC_TYPE>
    static void testCase7b_imp();
    static void testCase7b();
        // TESTING ALLOCATOR EXTENDED CONSTRUCTION FROM OPTIONAL

    template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
    static void testCase7a_imp();
    template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
    static void testCase7a_imp_constmovebug();
    static void testCase7a();
        // TESTING CONSTRUCTION FROM OPTIONAL

    static void testCase4f();
        // TESTING 'operator*' MEMBER FUNCTION.  Note that this test can only
        // be executed with types that are constructible from an integer, have
        // a 'value()' method, and for which there exists an overload of
        // 'isConstRef'.

    static void testCase4e();
        // TESTING 'operator->' MEMBER FUNCTION.  Note that this test can only
        // be executed with types that are constructible from an integer and
        // have a 'value()' method.

    static void testCase4d();
        // TESTING ALLOCATOR EXTENDED 'value_or' METHOD.  Note that this test
        // can only be executed with types that are constructible from an
        // integer, have a 'value()' method, are allocator-aware and have a
        // 'get_allocator' method.

    static void testCase4c();
        // TESTING 'value_or' METHOD.  Note that this test can only be executed
        // with types that are constructible from an integer, have a 'value()'
        // method, and, if TYPE is allocator-aware, has a 'get_allocator'
        // method

    static void testCase4b();
        // TESTING 'value' METHOD.  Note that this test can only be executed
        // with types that are constructible from an integer, have a 'value()'
        // method, and for which there exists an overload of 'isRvalueRef' and
        // 'isConstRef'.

    static void testCase4a();
        // TESTING CONVERSION TO BOOL

    static void testCase3d();
        // TESTING 'initializer_list' 'in_place_t' CONSTRUCTOR

    static void testCase3c();
        // TESTING 'in_place_t' CONSTRUCTOR

    template <class DEST_TYPE, class SRC_TYPE>
    static void testCase3b_imp();
    static void testCase3b();
        // TESTING ALLOCATOR EXTENDED CONSTRUCTION FROM VALUE

    template <class DEST_TYPE, class SRC_TYPE>
    static void testCase3a_imp();
    static void testCase3a();
        // TESTING CONSTRUCTION FROM VALUE

    static void testCase2c_imp();
    static void testCase2c();
        // TESTING 'reset' MEMBER FUNCTION.  Note that this test requires
        // <TYPE> to provide a 's_destructorInvocations' static variable which
        // counts the number of times destructor has been invoked.

    static void testCase2b();
        // ALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS.  Note that this test
        // requires 'TYPE' to be allocator-aware.

    static void testCase2a();
        // DISENGAGED CONSTRUCTORS AND DESTRUCTOR.  Note that this test
        // requires <TYPE> to provide a 's_destructorInvocations' static
        // variable which counts the number of times destructor has been
        // invoked.

    static void testCase1();
        // BREATHING TEST
};

template <class TYPE>
void bslstl_optional_value_type_deduce(const bsl::optional<TYPE>&)
{
}

template <class TYPE>
void bslstl_optional_optional_type_deduce(const TYPE&)
{
}

template <class TYPE>
void TestDriver<TYPE>::testCase24()
{
    // ------------------------------------------------------------------------
    // TESTING ASSIGNABILITY/CONSTRUCTIBILITY FROM DERIVED CLASS
    //
    // The 'class' 'Derived<TYPE>' (defined in this file above) is derived from
    // 'bsl::optional<TYPE>'.  In this test case, we only consider 'TYPE's
    // which don't allocate memory.
    //
    // Concerns:
    //: 1 'optional' can be assigned from a const 'Derived' object.
    //:
    //: 2 'optional' can be constructed from a const 'Derived' object.
    //:
    //: 3 'optional' can be assigned from a moved 'Derived' object.
    //:
    //: 4 'optional' can be constructed from a moved 'Derived' object.
    //
    // Plan:
    //: 1 assign to an 'optional' from a const 'Derived'.
    //:
    //: 2 construct an 'optional' from a const 'Derived'.
    //:
    //: 3 assign to an 'optional' from a moved 'Derived'.
    //:
    //: 4 construct an 'optional' from a moved 'Derived'.
    //
    // TESTING DERIVED -- 'TYPE' DOES NOT ALLOCATE
    // ------------------------------------------------------------------------

    BSLMF_ASSERT(!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value);

    const TestValues     tvs;

    ASSERT(tvs[0] != tvs[1]);

    Obj            ov(tvs[0]);
    Derived<TYPE>  dv(tvs[1]);    const Derived<TYPE>& DV = dv;

    ASSERT(tvs[0] == *ov);
    ASSERT(tvs[1] == *DV);

    // assign from const 'Derived'

    ov = DV;

    ASSERT(tvs[1] == *ov);
    ASSERT(tvs[1] == *DV);

    ov = tvs[0];

    // construct from const 'Derived'

    Obj odv(DV);

    ASSERT(*odv == tvs[1]);

    // assign from moved 'Derived'

    ov = MoveUtil::move(dv);

    ASSERT_IS_NOT_MOVED_FROM(*ov);
    ASSERT_IS_MOVED_INTO(*ov);
    ASSERT_IS_MOVED_FROM(*dv);
    ASSERT_IS_NOT_MOVED_INTO(*dv);

    ASSERT(tvs[1] == *ov);

    *ov = tvs[0];
    *dv = tvs[1];

    ASSERT_IS_NOT_MOVED_FROM(*ov);
    ASSERT_IS_NOT_MOVED_INTO(*ov);
    ASSERT_IS_NOT_MOVED_FROM(*dv);
    ASSERT_IS_NOT_MOVED_INTO(*dv);

    ASSERT(tvs[0] == *ov);
    ASSERT(tvs[1] == *dv);

    // construct from moved 'Derived'

    Obj oev(MoveUtil::move(dv));

    ASSERT_IS_NOT_MOVED_FROM(*oev);
    ASSERT_IS_MOVED_INTO(*oev);
    ASSERT_IS_MOVED_FROM(*dv);
    ASSERT_IS_NOT_MOVED_INTO(*dv);

    ASSERT(tvs[1] == *oev);
}

template <class TYPE>
void TestDriver<TYPE>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING ASSIGNABILITY/CONSTRUCTIBILITY FROM DERIVED CLASS
    //
    // The 'class' 'Derived<TYPE>' (defined in this file above) is derived from
    // 'bsl::optional<TYPE>'.  In this test case, we only consider 'TYPE's
    // which allocate memory.
    //
    // Concerns:
    //: 1 'optional' can be assigned from a const 'Derived' object.
    //:   o allocators match
    //:
    //:   o allocators don't match
    //:
    //: 2 'optional' can be constructed from a const 'Derived' object.
    //:   o No allocator passed
    //:
    //:   o non-matching allocator passed
    //:
    //:   o matching allocator passed
    //:
    //: 3 'optional' can be assigned from a moved 'Derived' object.
    //:
    //: 4 'optional' can be constructed from a moved 'Derived' object.
    //:   o No allocator passed
    //:
    //:   o non-matching allocator passed
    //:
    //:   o matching allocator passed
    //
    // Plan:
    //: 1 Create 3 'bsl::allocator' objects, 'aa', 'ab', and 'ad', where 'ad'
    //:   is the default allocator.
    //:
    //: 2 Iterate a loop through 'c' equals 'a', 'b', and 'c', where a
    //:   reference 'aExp' is set to allocators 'aa', 'ab', and 'ad' depending
    //:   on the value of 'c'.  We will do our tests, both assignments and
    //:   constructions, so that the expected allocator of the result matches
    //:   'aExp'.
    //:   o Create an 'optional' object 'ov' which uses 'aExp'.  'ov' will be
    //:     used as the destination for assignments.
    //:
    //:   o Create a 'Derived' object 'dv' which uses 'ab', and 'DV', a const
    //:     ref to that.
    //;
    //:   o Assign to 'ov' from 'DV' (C-1).
    //:
    //:   o Check that the value and allocator of 'ov' are as expected.
    //:
    //:   o Reset 'ov' to its original value.
    //:
    //:   o Create a 'bsls::ObjectBuffer<optional>' 'oBuf', and 'odv', an
    //:     'optional' reference to it.
    //:
    //:   o Go into a switch and call placement constructors to construct 'odv'
    //:     from DV.  If 'aExp' is 'aa' or 'ab' pass 'aExp' to the constructor,
    //:     otherwise pass no allocator to the constructor (in which case we
    //:     expect the result to use the default allocator 'ad', which will
    //:     match 'aExp').  (C-2)
    //:
    //:   o Check that the value and allocator of 'odv' are as expected.
    //:
    //:   o Destroy 'odv'
    //:
    //:   o Assign to 'ov' from moved 'dv'.  (C-3)
    //:
    //:   o Check that the value and allocator of 'ov' are as expected.
    //:
    //:   o Reset 'ov' to its original value.
    //:
    //:   o Go into a switch and call placement constructors to construct 'odv'
    //:     from moved 'dv'.  If 'aExp' is 'aa' or 'ad' pass 'aExp' to the
    //:     constructor, otherwise pass no allocator to the constructor (in
    //:     which case we expect the result to use 'dv's allocator 'ab', which
    //:     will match 'aExp').  (C-4)
    //:
    //:   o Check that the value and allocator of 'odv' are as expected.
    //:
    //:   o Destroy 'odv'
    //
    // TESTING DERIVED -- 'TYPE' ALLOCATES
    // ------------------------------------------------------------------------

    BSLMF_ASSERT(BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value);

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator ta, tb;
    bsl::allocator<char> aa(&ta), ab(&tb), ad(&da);

    const TestValues     tvs;

    ASSERT(tvs[0] != tvs[1]);

    for (char c = 'a'; c <= 'c'; ++c) {
        bsl::allocator<char>& aExp = 'a' == c ? aa    // 'aExp' is always
                                   : 'b' == c ? ab    // expected to be the
                                   : ad;              // allocator of the
                                                      // destinations, both
                                                      // 'ov' and 'odv'.

        Obj ov(bsl::allocator_arg, aExp, tvs[0]);
        Derived<TYPE>        dv(bsl::allocator_arg, ab, tvs[1]);
        const Derived<TYPE>& DV = dv;

        ASSERT(tvs[0] == *ov);
        ASSERT(tvs[1] == *DV);

        ASSERT(aExp == ov.get_allocator());
        ASSERT(ab   == DV.get_allocator());

        // assign from const

        ov = DV;

        ASSERT_IS_NOT_MOVED_FROM(*ov);
        ASSERT_IS_NOT_MOVED_INTO(*ov);
        ASSERT_IS_NOT_MOVED_FROM(*DV);
        ASSERT_IS_NOT_MOVED_INTO(*DV);

        ASSERT(tvs[1] == *ov);
        ASSERT(tvs[1] == *dv);

        ASSERT(aExp == ov.get_allocator());
        ASSERT(ab == DV.get_allocator());

        ov = tvs[0];

        // construct from const

        bsls::ObjectBuffer<Obj> oBuf;
        Obj& odv = oBuf.object();
        switch (c) {
          case 'a':
          case 'b': {
            new (oBuf.address()) Obj(bsl::allocator_arg, aExp, DV);
          } break;
          case 'c': {
            new (oBuf.address()) Obj(DV);
          } break;
          default: {
            ASSERT(0);
          }
        }

        ASSERT(tvs[1] == *odv);
        ASSERT(tvs[1] == *DV);

        ASSERT_IS_NOT_MOVED_FROM(*odv);
        ASSERT_IS_NOT_MOVED_INTO(*odv);
        ASSERT_IS_NOT_MOVED_FROM(*DV);
        ASSERT_IS_NOT_MOVED_INTO(*DV);

        ASSERT(aExp == odv.get_allocator());

        odv.~Obj();

        // assign from moved

        ov = MoveUtil::move(dv);

        ASSERT_IS_NOT_MOVED_FROM(*ov);
        ASSERT_IS_MOVED_INTO(*ov);
        ASSERT_IS_MOVED_FROM(*dv);
        ASSERT_IS_NOT_MOVED_INTO(*dv);

        ASSERT(tvs[1] == *ov);

        ASSERT(aExp == ov.get_allocator());
        ASSERT(ab == DV.get_allocator());

        *ov = tvs[0];
        *dv = tvs[1];

        ASSERT_IS_NOT_MOVED_FROM(*ov);
        ASSERT_IS_NOT_MOVED_INTO(*ov);
        ASSERT_IS_NOT_MOVED_FROM(*dv);
        ASSERT_IS_NOT_MOVED_INTO(*dv);

        ASSERT(tvs[0] == *ov);
        ASSERT(tvs[1] == *dv);

        // construct from moved

        switch (c) {
          case 'a':
          case 'c': {
            new (oBuf.address()) Obj(bsl::allocator_arg,
                                     aExp,
                                     MoveUtil::move(dv));
          } break;
          case 'b': {
            // No allocator passed, will get allocator from 'dv', which is
            // 'ab', which matched 'aExp' if 'c == 'b''.

            new (oBuf.address()) Obj(MoveUtil::move(dv));
          } break;
        }

        ASSERT(tvs[1] == *odv);

        ASSERT_IS_NOT_MOVED_FROM(*odv);
        ASSERT_IS_MOVED_INTO(*odv);
        ASSERT_IS_MOVED_FROM(*dv);
        ASSERT_IS_NOT_MOVED_INTO(*dv);

        ASSERT(aExp == odv.get_allocator());

        odv.~Obj();
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING NOEXCEPT
    //
    // Concerns:
    //: 1 That the default, and 'nullopt_t', are noexcept.
    //:
    //: 2 That the move c'tor is noexcept if the contained type is nothrow move
    //:   constructible.
    //
    // Plan:
    //: 1 Use the 'noexcept' operator on c'tor calls to see if the three
    //:   respective c'tors are noexcept when we expect them to be.
    //
    // Testing:
    //   NOEXCEPT
    // ------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    const char *type = bsls::NameOf<TYPE>().name();

    Obj                  mX;

    ASSERTV(type, noexcept(Obj()));
    ASSERTV(type, noexcept(Obj(bsl::nullopt)));
    ASSERTV(type, bsl::is_nothrow_move_constructible<TYPE>::value ==
                                                 noexcept(Obj(std::move(mX))));
#endif
}

template <class TYPE>
void TestDriver<TYPE>::testCase19()
{
    // ------------------------------------------------------------------------
    // REPRODUCE BUG FROM DRQS 165776192
    //
    // Concerns:
    //: 1 The copy constructor is selected when constructing a copy of nested
    //:   'bsl::optional', i.e., 'bsl::optional<bsl::optional<TYPE>>'.
    //
    // Plan:
    //: 1 Create an 'optional' object of the
    //:   'bsl::optional<bsl::optional<TYPE>>' type.
    //:
    //: 2 Create a copy the object created in (P-1) and ensure that compiler
    //:   can compile it.
    //
    // Testing:
    //   DRQS 165776192
    // ------------------------------------------------------------------------

    bsl::optional<bsl::optional<TYPE> > s;
    bsl::optional<bsl::optional<TYPE> > ss(s);
    (void) ss;
}

template <class TYPE>
void TestDriver<TYPE>::testCase18()
{
    // --------------------------------------------------------------------
    // TESTING type deduction
    //
    //  In this test, we verify that type deduction with 'bsl::optional'
    //  works.
    //
    // Concerns:
    //: 1 Invoking a template function which takes 'bsl::optional<TYPE>'
    //:   deduces the TYPE
    //:
    //: 2 Invoking a template function which takes 'TYPE' with 'bsl::optional'
    //:   will be able to deduce 'TYPE' as 'bsl::optional'
    //
    // Plan:
    //: 1 Call a template function which takes 'bsl::optional<TYPE>' with an
    //:   object of type 'bsl::optional<TestType>'.  [C-1]
    //:
    //: 2 Call a template function which takes 'TYPE' with an object of type
    //:   'bsl::optional<TestType>'.  [C-2]
    //
    //
    // Testing:
    //    Type deduction
    //

    Obj x;

    bslstl_optional_value_type_deduce(x);
    bslstl_optional_optional_type_deduce(x);
}

template <class TYPE>
void TestDriver<TYPE>::testCase17()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  allocator extended 'make_optional' facility is created using the given
    //  arguments and without unnecessary copies.  We are not worried about the
    //  allocator policy of the allocator-aware type, as choosing the allocator
    //  policy when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of allocator extended 'make_optional'
    //  where the type is deduced.
    //
    //
    // Concerns:
    //: 1 Invoking allocator extended 'make_optional' creates an 'optional'
    //:   with the value of the argument.
    //:
    //: 2 In a single non-allocator argument of allocator extended
    //:   'make_optional' invocation, the value type of the optional is deduced
    //:   from the non-allocator argument.
    //:
    //: 3 Argument is perfectly forwarded.
    //:
    //: 4 The specified allocator is used as the allocator for the returned
    //:   'optional' object.
    //:
    //: 5 No unnecessary objects of 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Call allocator extended 'make_optional' to create an 'optional'.  As
    //:   an argument use an lvalue of the desired 'value_type'.  Verify that
    //:   the resulting 'optional' object has the expected value. [C-1]
    //:
    //: 2 Repeat step 1 without specifying the template argument when invoking
    //:   'make_optional'. [C-2]
    //:
    //: 3 Repeat steps 1-2 with an rvalue argument, with a const lvalue
    //:   reference and with a const rvalue reference. Verify the number of
    //:   invocations of copy and move value type constructors is the same as
    //:   when the 'optional''s 'in_place' constructor has been called.
    //:   [C-3][C-5]
    //:
    //: 4 In steps 1-3, verify that the resulting optional is using the allocator
    //:   specified in the allocator extended 'make_optional' call. [C-4]
    //
    // Testing:
    //   optional make_optional(bsl::allocator_arg_t,
    //                          const allocator_type&,
    //                          TYPE&&);

    if (verbose)
        printf("\nTESTING ALLOCATOR EXTENDED 'make_optional' FACILITY"
               "\n===================================================\n");

    if (veryVerbose)
        printf("\tDeduced type make_optional.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        ValWithAllocator sourceBuf(2, &oa);
        ValueType&       source      = sourceBuf.object();
        const ValueType& constSource = sourceBuf.object();

        TEST_MAKE_OPTIONAL_WITH_ALLOC_DEDUCED(source);
        TEST_MAKE_OPTIONAL_WITH_ALLOC_DEDUCED(constSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't deduction friendly when it comes to deducing
        // the allocator type
        TEST_MAKE_OPTIONAL_WITH_ALLOC_DEDUCED(MoveUtil::move(source));
        TEST_MAKE_OPTIONAL_WITH_ALLOC_DEDUCED(MoveUtil::move(constSource));
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase17b()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  allocator extended 'make_optional' facility is created using the given
    //  arguments and without unnecessary copies.  We are not worried about the
    //  allocator policy of the allocator-aware type, as choosing the allocator
    //  policy when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of allocator extended 'make_optional'
    //  with var args.
    //
    //
    // Concerns:
    //: 1 Invoking allocator extended 'make_optional' creates an 'optional'
    //:   with the value of the arguments converted to the specified value type.
    //:
    //: 2 Arguments are perfectly forwarded.
    //:
    //: 3 No unnecessary objects of 'value_type' are created.
    //:
    //: 4 The specified allocator is used as the allocator for the returned
    //:   'optional' object.
    //:
    //: 5 If the first argument is a 'braced-init-list', allocator extended
    //:   'make_optional' will deduce an 'std::intializer_list'
    //
    //
    // Plan:
    //: 1 Call allocator extended 'make_optional' to create an 'optional'.  As
    //:   an argument use an lvalue of the desired 'value_type'.  Verify that
    //:   the resulting 'optional' object has the expected value. [C-1]
    //:
    //: 2 Repeat step 1 with different number of arguments and with a
    //:   combination of lvalue and rvalue for each argument. Verify that that
    //:   the resulting object was created with the same combination of
    //:   lvalue and rvalue arguments. [C-2]
    //:
    //: 3 In steps 1-2, verify that there were no more copies of value type
    //:   created than when calling an in_place constructor. [C-3]
    //:
    //: 4 In steps 1-3, verify that the resulting optional is using the
    //:   allocator specified in the allocator extended 'make_optional' call.
    //:   [C-4]
    //:
    //: 5 Repeat steps 1-4 with an additional 'braced-init-list' as the first
    //:   non-allocator argument to allocator extended 'make_optional'. [C-5]
    //
    // Testing:
    //    optional make_optional(alloc_arg, const alloc&, ARGS&&...);
    //    optional make_optional(alloc_arg, const alloc&, init_list, ARGS&&...);

    if (verbose)
        printf("\nTESTING ALLOCATOR EXTENDED 'make_optional' FACILITY"
               "\n===================================================\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place), (bsl::allocator_arg, &oa), &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, MoveUtil::move(VA1)),
            (bsl::allocator_arg, &oa, MoveUtil::move(VA1)),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, VA1), (bsl::allocator_arg, &oa, VA1), &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, MoveUtil::move(VA1), VA2),
            (bsl::allocator_arg, &oa, MoveUtil::move(VA1), VA2),
            &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, VA1, MoveUtil::move(VA2)),
            (bsl::allocator_arg, &oa, VA1, MoveUtil::move(VA2)),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
            (bsl::allocator_arg,
             &oa,
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, VA1, MoveUtil::move(VA2), VA3),
            (bsl::allocator_arg, &oa, VA1, MoveUtil::move(VA2), VA3),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7)),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9)),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9),
                                       VA10),
                                      (bsl::allocator_arg,
                                       &oa,
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9),
                                       VA10),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9,
                                       MoveUtil::move(VA10)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9,
                                       MoveUtil::move(VA10)),
                                      &oa);
     }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&        \
    (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION >= 1900))
    if (veryVerbose)
        printf("\tUsing 'initializer_list' argument.\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place, {1, 2, 3}),
                                      (bsl::allocator_arg, &oa, {1, 2, 3}),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1)),
            (bsl::allocator_arg, &oa, {1, 2, 3}, MoveUtil::move(VA1)),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, {1, 2, 3}, VA1),
            (bsl::allocator_arg, &oa, {1, 2, 3}, VA1),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1), VA2),
            (bsl::allocator_arg, &oa, {1, 2, 3}, MoveUtil::move(VA1), VA2),
            &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
            (bsl::allocator_arg, &oa, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC(
            (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
            (bsl::allocator_arg,
             &oa,
             {1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3),
            &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7)),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8)),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9)),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9),
                                      &oa);

        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9),
                                       VA10),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       MoveUtil::move(VA1),
                                       VA2,
                                       MoveUtil::move(VA3),
                                       VA4,
                                       MoveUtil::move(VA5),
                                       VA6,
                                       MoveUtil::move(VA7),
                                       VA8,
                                       MoveUtil::move(VA9),
                                       VA10),
                                      &oa);
        TEST_MAKE_OPTIONAL_WITH_ALLOC((bsl::in_place,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9,
                                       MoveUtil::move(VA10)),
                                      (bsl::allocator_arg,
                                       &oa,
                                       {1, 2, 3},
                                       VA1,
                                       MoveUtil::move(VA2),
                                       VA3,
                                       MoveUtil::move(VA4),
                                       VA5,
                                       MoveUtil::move(VA6),
                                       VA7,
                                       MoveUtil::move(VA8),
                                       VA9,
                                       MoveUtil::move(VA10)),
                                      &oa);
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

template <class TYPE>
void TestDriver<TYPE>::testCase16()
{
    // --------------------------------------------------------------------
    // TESTING 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'make_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator-aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of 'make_optional' where the type
    //  is deduced.
    //
    //
    // Concerns:
    //: 1 Invoking 'make_optional' creates an 'optional' with the value of the
    //:   argument.
    //:
    //: 2 In a single argument 'make_optional' invocation, the value type of
    //:   the optional is deduced from the argument.
    //:
    //: 3 Argument is perfectly forwarded.
    //:
    //: 4 If the 'value_type' is allocator-aware, default allocator is used to
    //:   construct the 'optional' object.
    //:
    //: 5 No unnecessary objects of 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Call 'make_optional' to create an 'optional'.  As an argument to
    //:   'make_optional' use an lvalue of the desired 'value_type'.  Verify that the
    //:   resulting 'optional' object has the expected value. [C-1]
    //:
    //: 2 Repeat step 1 without specifying the template argument when invoking
    //:   'make_optional'. [C-2]
    //:
    //: 3 Repeat steps 1-2 with an rvalue argument, with a const lvalue
    //:   reference and with a const rvalue reference. Verify the number of
    //:   invocations of copy and move value type constructors is the same as
    //:   when the optional's in_place constructor has been called. [C-3][C-5]
    //:
    //: 4 In steps 1-3, if type is allocator-aware, verify that the resulting
    //:   'optional' is using the default allocator. [C-4]
    //
    // Testing:
    //      make_optional(T&&);

    if (verbose)
        printf("\nTESTING 'make_optional' FACILITY"
               "\n================================\n");

    if (veryVerbose)
        printf("\tDeduced type make_optional.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        ValWithAllocator sourceBuf(2, &oa);
        ValueType&       source      = sourceBuf.object();
        const ValueType& constSource = sourceBuf.object();

        TEST_MAKE_OPTIONAL_DEDUCED(source);
        TEST_MAKE_OPTIONAL_DEDUCED(constSource);
        TEST_MAKE_OPTIONAL_DEDUCED(MoveUtil::move(source));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make this test fail
        TEST_MAKE_OPTIONAL_DEDUCED(MoveUtil::move(constSource));
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase16b()
{
    // --------------------------------------------------------------------
    // TESTING 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'make_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator-aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of 'make_optional' with var args.
    //
    //
    // Concerns:
    //: 1 Invoking 'make_optional' creates an 'optional' with the value of the
    //:   arguments converted to the specified value type.
    //:
    //: 2 Arguments are perfectly forwarded.
    //:
    //: 3 No unnecessary objects of 'value_type' are created.
    //:
    //: 4 If the 'value_type' is allocator-aware, default allocator is used to
    //:   construct the 'optional' object.
    //:
    //: 5 If the first argument is a 'braced-init-list', 'make_optional' will
    //:   deduce an 'std::initializer list'
    //
    //
    // Plan:
    //: 1 Call 'make_optional' to create an 'optional'.  As an argument to
    //:   'make_optional' use an lvalue of the desired 'value_type'.  Verify that the
    //:   resulting 'optional' object has the expected value. [C-1]
    //:
    //: 2 Repeat step 1 with different number of arguments and with a
    //:   combination of lvalue and rvalue for each argument. Verify that that
    //:   the resulting object was created with the same combination of
    //:   lvalue and rvalue arguments. [C-2]
    //:
    //: 3 in steps 1-2, verify that there were no more copies of value type created
    //:   than when calling an in_place constructor. [C-3]
    //:
    //: 4 In steps 1-3, if type is allocator-aware, verify that the resulting
    //:   'optional' is using the default allocator. [C-4]
    //:
    //: 5 Repeat steps 1-4 with an additional 'braced-init-list' as the first
    //:   argument to 'make_optional'. [C-5]
    //
    // Testing:
    //      make_optional();
    //      make_optional(ARG &&, Args&&... args);
    //      make_optional(initializer_list<U> il, Args&&... args);

    if (verbose)
        printf("\nTESTING 'make_optional' FACILITY"
               "\n================================\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_MAKE_OPTIONAL((bsl::in_place), ());

        TEST_MAKE_OPTIONAL((bsl::in_place, MoveUtil::move(VA1)),
                           (MoveUtil::move(VA1)));

        TEST_MAKE_OPTIONAL((bsl::in_place, VA1), (VA1));

        TEST_MAKE_OPTIONAL((bsl::in_place, MoveUtil::move(VA1), VA2),
                           (MoveUtil::move(VA1), VA2));
        TEST_MAKE_OPTIONAL((bsl::in_place, VA1, MoveUtil::move(VA2)),
                           (VA1, MoveUtil::move(VA2)));

        TEST_MAKE_OPTIONAL(
               (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
               (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));

        TEST_MAKE_OPTIONAL((bsl::in_place, VA1, MoveUtil::move(VA2), VA3),
                           (VA1, MoveUtil::move(VA2), VA3));

        TEST_MAKE_OPTIONAL(
          (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4),
          (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_MAKE_OPTIONAL(
          (bsl::in_place, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)),
          (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)));

        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)));
     }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&        \
    (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION >= 1900))

    if (veryVerbose)
        printf("\tUsing 'initializer_list' argument.\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_MAKE_OPTIONAL((bsl::in_place, {1, 2, 3}), ({1, 2, 3}));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1)),
                           ({1, 2, 3}, MoveUtil::move(VA1)));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1, 2, 3}, VA1), ({1, 2, 3}, VA1));

        TEST_MAKE_OPTIONAL(
                         (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1), VA2),
                         ({1, 2, 3}, MoveUtil::move(VA1), VA2));
        TEST_MAKE_OPTIONAL(
                         (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
                         ({1, 2, 3}, VA1, MoveUtil::move(VA2)));

        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));

        TEST_MAKE_OPTIONAL(
                    (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
                    ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3));

        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)));
     }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

template <class TYPE>
void TestDriver<TYPE>::testCase15()
{
    // --------------------------------------------------------------------
    // TESTING TRAITS AND TYPEDEFS
    //
    //
    // Concerns:
    //: 1 That 'optional<TYPE>::value_type' is 'TYPE'
    //:
    //: 2 That 'optional<const TYPE>::value_type' is 'const TYPE'
    //:
    //: 3 That 'bslma::UsesBslmaAllocator<optional<TYPE>>' and
    //:   'bslmf::UsesAllocatorArgT<optional<TYPE>>' are 'false'
    //:   if 'TYPE' is a non allocator-aware type.
    //:
    //: 4 That 'bslma::UsesBslmaAllocator<optional<TYPE>>' and
    //:   'bslmf::UsesAllocatorArgT<optional<TYPE>>' are 'true'
    //:   if 'TYPE' is an allocator-aware type.
    //:
    //: 5 That 'optional<TYPE>' is trivially destructible if 'TYPE' is
    //:   trivially destructible
    //
    // Plan:
    //: 1 Verify that 'optional<TYPE>::value_type' matches 'TYPE'. [C-1]
    //:
    //: 2 Repeat step 1 using a 'const' 'value_type'. [C-2]
    //:
    //: 3 Verify that both 'UsesBslmaAllocator' and
    //:   'UsesAllocatorArgT' traits for 'optional<TYPE'> are true if 'TYPE'
    //:   is allocator-aware. [C-3]
    //:
    //: 4 Verify that both 'UsesBslmaAllocator' and
    //:   'UsesAllocatorArgT' traits for 'optional<TYPE'> are false if 'TYPE'
    //:   is not allocator-aware. [C-4]
    //:
    //: 5 Verify that 'optional<TYPE'> is trivially destructible if 'TYPE' is
    //:   trivially destructible. [C-5]
    //
    // Testing:
    //
    //      optional<TYPE>::value_type
    //      bslma::UsesBslmaAllocator<optional<TYPE>>
    //      bslma::UsesAllocatorArgT<optional<TYPE>>
    //      std::is_trivially_destructible<optional<TYPE>>
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING TRAITS AND TYPEDEFS"
               "\n===========================\n");

    {
        const char *valueTypeName = bsls::NameOf<ValueType>().name();
        const char *objName       = bsls::NameOf<Obj>().name();

        ASSERT((bsl::is_same<typename Obj::value_type, ValueType>::value));
        ASSERT(
            (bsl::is_same<typename ObjC::value_type, const ValueType>::value));

        const bool usesAllocatorObj = bslma::UsesBslmaAllocator<Obj>::value;
        const bool usesAllocatorValueType =
                                   bslma::UsesBslmaAllocator<ValueType>::value;
        const bool usesArgTObj = bslmf::UsesAllocatorArgT<Obj>::value;

        const bool convObj = bsl::is_convertible<bslma::Allocator *,
                                                 Obj>::value;
        const bool convValueType = bsl::is_convertible<bslma::Allocator *,
                                                       ValueType>::value;
        if (veryVerbose) {
            P_(valueTypeName);    P_(convObj);    P(convValueType);
        }

        ASSERTV(valueTypeName, objName, usesAllocatorObj,
                                                        usesAllocatorValueType,
                                   usesAllocatorObj == usesAllocatorValueType);
        ASSERTV(valueTypeName, objName, usesArgTObj, usesAllocatorValueType,
                                        usesArgTObj == usesAllocatorValueType);
        ASSERTV(valueTypeName, objName, usesArgTObj, usesAllocatorObj,
                                              usesArgTObj == usesAllocatorObj);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(std::is_trivially_destructible<Obj>::value ==
               std::is_trivially_destructible<ValueType>::value);
#else
        ASSERT(bsl::is_trivially_copyable<Obj>::value ==
               bsl::is_trivially_copyable<ValueType>::value);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase15b()
{
    // --------------------------------------------------------------------
    // TESTING TRAITS AND TYPEDEFS
    //
    //
    // Concerns:
    //: 1 That 'optional<TYPE>::allocator_type' is 'bsl::allocator<char>' if
    //:   'TYPE' is allocator aware.
    //
    // Plan:
    //: 1 Verify that 'optional<TYPE>::allocator_type' is
    //:   'bsl::allocator<char>'. [C-1]
    //
    // Testing:
    //
    //      optional<TYPE>::allocator_type
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING TRAITS AND TYPEDEFS"
               "\n===========================\n");

    {
        ASSERT((bsl::is_same<typename ObjC::allocator_type,
                             bsl::allocator<char> >::value));
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //: 1 Hashing a value with a null value is equivalent to appending 'false'
    //:   to the hash.
    //:
    //: 2 Hashing a value with a nullable value is equivalent to appending
    //:   'true' to the hash followed by the value.
    //:
    //: 3 Invoking 'hashAppend' with a custom hash algorithm as the first
    //:   argument correctly finds 'bslh::hashAppend' from the 'bslh_hash'
    //:   component.
    //
    // Plan:
    //: 1 Create a null nullable value and verify that hashing it yields the
    //:   same value as hashing 'false'. [C-1]
    //:
    //: 2 Create a non-null nullable value for a series of test values and
    //:   verify that hashing it produces the same result as hashing 'true' and
    //:   then the test values themselves. [C-2]
    //:
    //: 3 Create a mock hash algorithm 'class' 'CustomHashAlgorithm' (not in
    //:   the bslh` namespace) then pass an instance of this hash 'class' to
    //:   'hashAppend'.
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const optional<TYPE>& input);
    //   DRQS 169300521
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'hashAppend'"
               "\n====================\n");

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    {
        ObjWithAllocator object(&oa);

        Obj&       x = object.object();
        const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        const size_t hashValue1 = bslh::Hash<>()(X);
        const size_t hashValue2 = bslh::Hash<>()(false);
        ASSERTV(hashValue1, hashValue2, hashValue1 == hashValue2);
    }

    {
        ObjWithAllocator object(&oa);

        Obj&       x = object.object();
        const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            x = VALUES[i];

            bslma::TestAllocatorMonitor oam(&oa);

            bslh::DefaultHashAlgorithm hasher;

            const size_t hashValue1 = bslh::Hash<>()(X);
            hashAppend(hasher, true);
            hashAppend(hasher, VALUES[i]);
            const size_t hashValue2 =
                static_cast<size_t>(hasher.computeHash());

            ASSERTV(hashValue1, hashValue2, hashValue2 == hashValue1);

            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }
        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }
    {
        if (veryVerbose)
            printf("Testing custom hash type.\n");

        CustomHashAlgorithm customHashAlgo ;
        hashAppend(customHashAlgo, bsl::optional<int>());
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase13()
{
    TestDriver<TYPE>::testCase13_imp_a();
    TestDriver<const TYPE>::testCase13_imp_a();
    TestDriver<TYPE>::testCase13_imp_b();
    TestDriver<const TYPE>::testCase13_imp_b();
}

template <class TYPE>
void TestDriver<TYPE>::testCase13_imp_a()
{
    // --------------------------------------------------------------------
    // TESTING 'emplace' METHOD
    //   This test will ensure that the 'emplace' method works as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' with no arguments creates a default constructed
    //:   'value_type' object.
    //:
    //: 2 If 'value_type' is allocator-aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //:
    //: 3 There are no unnecessary 'value_type' copies created
    //:
    //: 4 Variadic arguments to 'emplace' method are correctly forwarded to the
    //:   constructor arguments.
    //:
    //: 5 'emplace' returns a reference offering modifiable access to the
    //:   'optional''s value type object.
    //
    // Plan:
    //: 1 Call 'emplace' method that takes no arguments and verify the value
    //:   type object is default constructed. [C-1]
    //:
    //: 2 In step 1, if the 'value_type' is allocator-aware, verify the
    //:   allocator used for the constructed value is the allocator of the
    //:   'optional' object. [C-2]
    //:
    //: 3 Invoke 'emplace' method with varying number of arguments, some of
    //:   which are to be moved from. Verify the arguments are perfect
    //:   forwarded to the constructor in the correct order. [C-4]
    //:
    //: 4 In steps 1-3, verify that no additional copies of 'value_type' have
    //:   been created. [C-3]
    //:
    //: 5 Verify that the returned reference refers to the 'value_type' object.
    //:   [C-5]
    //
    // Testing:
    //
    //   T& emplace(Args&&...);
    //
    // --------------------------------------------------------------------
    if (verbose)
        printf("\nTESTING 'emplace' METHOD"
               "\n========================\n");
    {
        bslma::TestAllocator da("other", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        TEST_EMPLACE((&da), ());
        TEST_EMPLACE((MoveUtil::move(VA1), &da), (MoveUtil::move(VA1)));
        TEST_EMPLACE((VA1, &da), (VA1));
        TEST_EMPLACE((MoveUtil::move(VA1), VA2, &da),
                     (MoveUtil::move(VA1), VA2));
        TEST_EMPLACE((VA1, MoveUtil::move(VA2), &da),
                     (VA1, MoveUtil::move(VA2)));

        TEST_EMPLACE((MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &da),
                     (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
        TEST_EMPLACE((VA1, MoveUtil::move(VA2), VA3, &da),
                     (VA1, MoveUtil::move(VA2), VA3));

        TEST_EMPLACE(
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &da),
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_EMPLACE(
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &da),
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5)));
        TEST_EMPLACE(
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &da),
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6)));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7)));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8)));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9)));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10,
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10),
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10)));
    }
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EMPLACE
    {
        CObj bad1;
        bad1.emplace();               // this should not compile 1
        bad1.emplace(VA1);            // this should not compile 2
        bad1.emplace(VA1, VA2);       // this should not compile 3
        bad1.emplace(VA1, VA2, VA3);  // this should not compile 4
        bad1.emplace(VA1, VA2, VA3, VA4);
        // this should not compile 5

        CObjC bad2;
        bad2.emplace();               // this should not compile 6
        bad2.emplace(VA1);            // this should not compile 7
        bad2.emplace(VA1, VA2);       // this should not compile 8
        bad2.emplace(VA1, VA2, VA3);  // this should not compile 9
        bad2.emplace(VA1, VA2, VA3, VA4);
        // this should not compile 10
    }
#endif
}
template <class TYPE>
void TestDriver<TYPE>::testCase13_imp_b()
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    // --------------------------------------------------------------------
    // TESTING TESTING INITIALIZER LIST 'emplace' METHOD
    //   This test will ensure that the initializer list 'emplace' method works
    //   as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' with only an initializer list detects the
    //:   initializer list.
    //:
    //: 2 Variadic arguments to 'emplace' method are correctly fowarded to the
    //:   constructor arguments.
    //:
    //: 3 If 'value_type' is allocator-aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //:
    //: 4 There are no unnecessary argument type and 'value_type' copies
    //:   created
    //:
    //: 5 'emplace' returns a reference offering modifiable access to the
    //:   'optional''s value type object.
    //
    // Plan:
    //: 1 Create an 'optional' object of non allocator-aware 'value_type'.
    //:   Call 'emplace' method that takes just an initializer list, and verify
    //:   the object was constructed using an initializer list constructor.
    //:   [C-1]
    //:
    //: 2 Repeat step 1 using varying number of arguments. [C-2]
    //:
    //: 3 Repeat steps 1-2 with an allocator-aware 'value_type' and verify the
    //:   value in optional was constructed using the correct allocator. [C-3]
    //:
    //: 4 In steps 1-3, verify no unnecessary copies of the arguments and the
    //:   'value_type' have been created. [C-4]
    //:
    //: 5 Verify that the returned reference refers to the 'value_type' object.
    //:   [C-5]
    //
    // Testing:
    //
    //   T& emplace(std::initializer_list<U>, Args&&...);
    //
    // --------------------------------------------------------------------
    if (verbose)
        printf("\nTESTING INITIALIZER LIST 'emplace' METHOD"
               "\n=========================================\n");
    {
        bslma::TestAllocator da("other", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3}, &da), ({1, 2, 3}));
        TEST_EMPLACE(
              (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &da),
              ({1, 2, 3}, MoveUtil::move(VA1)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3}, VA1, &da),
                     ({1, 2, 3}, VA1));
        TEST_EMPLACE(
         (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), VA2, &da),
         ({1, 2, 3}, MoveUtil::move(VA1), VA2));
        TEST_EMPLACE(
         (std::initializer_list<int>{1, 2, 3}, VA1, MoveUtil::move(VA2), &da),
         ({1, 2, 3}, VA1, MoveUtil::move(VA2)));

        TEST_EMPLACE(
            (std::initializer_list<int>{1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             &da),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      &da),
                     ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3));

        TEST_EMPLACE(
            (std::initializer_list<int>{1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4,
             &da),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_EMPLACE(
            (std::initializer_list<int>{1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             &da),
            ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6)));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8)));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10,
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10),
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10)));
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

void testCase10f()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(optional_type)'
    //   This test will ensure that the 'operator=(rhs)', where 'rhs' is an
    //   optional type, member function works as expected.  These test require
    //   compilation failures and will not run by default.
    //
    // Concerns:
    //: 1 'operator=(rhs)', where 'rhs' is an 'optional' type whose
    //:   'value_type' is not assignable to the 'value_type' of the target
    //:    'optional', can not be called.
    //:
    //: 2 'operator=(rhs)', where 'rhs' is an 'optional' type whose
    //:   'value_type' is not convertible to target 'optional''s 'value_type',
    //:    can not be called.
    //
    // Plan:
    //: 1 Create an 'optional' object of non allocator-aware 'value_type' as
    //:   the target object.  Verify that an 'optional' object of 'value_type'
    //:   that is convertible, but not assignable to target object 'value_type'
    //:   can not be assigned to the target object.  Note that this test
    //:   requires compilation errors and needs to be enabled and checked
    //:   manually. [C-1]
    //:
    //: 2 Verify that an 'optional' object of 'value_type' that is assignable,
    //:   but not convertible to target object's 'value_type' can not be
    //:   assigned to the target object.  Note that this test requires
    //:   compilation errors and needs to be enabled and checked manually.
    //:   [C-2]
    //
    //
    // Testing:
    //
    // optional& operator=(const optional&);
    // optional& operator=(optional&&);
    // optional& operator=(const optional<ANY_TYPE>&);
    // optional& operator=(optional<ANY_TYPE>&&);
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=(optional_type)'"
               "\n==================================\n");
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_OPT

    if (veryVerbose)
        printf("\tUsing 'MyClass1b'.\n");
    {
        typedef MyClass1b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX  = ValueType(0);
            bsl::optional<MyClass1> mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass1c'.\n");
    {
        typedef MyClass1c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX  = ValueType(0);
            bsl::optional<MyClass1> mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2b'.\n");
    {
        typedef MyClass2b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX;
            bsl::optional<MyClass2> mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2c'.\n");
    {
        typedef MyClass2c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX;
            bsl::optional<MyClass2> mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
#endif
}

void testCase10e()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(non_optional_type)'
    //   This test will ensure that the 'operator=(non_optional_type)' function
    //   works as expected.
    //
    // Concerns:
    //: 1 'operator=(rhs)', where 'rhs' is not an 'optional' object, can not be
    //:    called if 'rhs' is of a type which is not assignable to
    //:    'value_type'.
    //:
    //: 2  'operator=(rhs)', where 'rhs' is not an 'optional' object, can not be
    //:    called if 'value_type' is not constructable from 'rhs'.
    //
    // Plan:
    //
    //: 1 Create an 'optional' object. Verify that lvalue and rvalue of type
    //:   convertible, but not assignable to 'value_type' can not be assigned
    //:   to the 'optional' object. Note that this test requires compilation
    //:   errors and needs to be enabled and checked manually. [C-1]
    //:
    //: 2 Create an 'optional' object. Verify that lvalue and rvalue of type
    //:   assignable, but not convertible to 'value_type' can not be assigned
    //:   to the 'optional' object. Note that this test requires compilation
    //:   errors and needs to be enabled and checked manually. [C-2]
    //
    //
    // Testing:
    //
    // optional& operator=(const TYPE&);
    // optional& operator=(TYPE&&);
    // optional& operator=(const ANY_TYPE&);
    // optional& operator=(ANY_TYPE&&);
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=(non_optional_type)'"
               "\n======================================"
               "\n");
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT

    if (veryVerbose)
        printf("\tUsing 'MyClass1b'.\n");
    {
        typedef MyClass1b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX  = ValueType(0);
            MyClass1 mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass1c'.\n");
    {
        typedef MyClass1c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX  = ValueType(0);
            MyClass1 mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2b'.\n");
    {
        typedef MyClass2b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX;
            MyClass2 mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2c'.\n");
    {
        typedef MyClass2c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX;
            MyClass2 mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
#endif
}

template <class TYPE>
void TestDriver<TYPE>::testCase10d()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=' OVERLOAD RESOLUTION
    //   This test will ensure that the 'operator=' overload
    //   resolution works as expected in the presence of constructors and
    //   assignment operators of 'value_type' taking an 'optional'.
    //   In these tests we do not care about the different allocation argument
    //   policies because the allocator propagation is tested in the
    //   functionality tests for 'operator=. Here, we only care that the
    //   correct overload is selected.
    //   Similarly, we do not care about the allocator state after assignment,
    //   nor do we care about the difference in behaviour when the source and
    //   destination 'optional's are engaged/disengaged. We deliberately always
    //   use engaged 'optional's as that directly forwards to the underlying
    //   'value_type' assignment.
    //
    // Concerns:
    //: 1 If 'value_type' 'TYPE' is assignable and constructible from an
    //:   'optional' type 'OPT_TYPE', the assignment of 'OPT_TYPE' to an
    //:   'optional<TYPE>' assigns the 'OPT_TYPE' object to the 'value_type'
    //:   object of 'optional<TYPE>'. The resulting 'optional' is always
    //:   engaged.
    //:
    //: 2 If 'value_type' 'TYPE' is not assignable and constructible from an
    //:   'optional' type 'OPT_TYPE', the assignment of an engaged 'OPT_TYPE'
    //:   to an 'optional<TYPE>' assigns the 'value_type' object of the
    //:   'OPT_TYPE' object to the 'value_type' object of 'optional<TYPE>'.
    //:
    //: 3 Move assign and copy assign have the same overload resolution.
    //:
    //: 4 Assigning '{}' to an 'optional' is interpreted as assigning a default
    //:   constructed 'optional' of same type. The resulting 'optional' is
    //:   disengaged.
    //:
    //: 5 Assigning a 'nullopt_t' object to an 'optional' object is always
    //:   interpreted as assigning a disengaged 'optional' of same type.
    //:
    //: 6 'optional' types of non allocator-aware 'value_type' and
    //:   allocator-aware type have the same overload resolution.
    //:
    //: Plan:
    //: 1 Create a source object of 'OPT_TYPE', where 'OPT_TYPE' is
    //:   'optional<TYPE>'.  Assign the source object to an object of
    //:   'optional<OPT_TYPE>'.  Verify that the resulting object is engaged and that
    //:   its 'value_type' object matches the source object. [C-1]
    //:
    //: 2 Assign an engaged object of 'optional<OPT_TYPE>' to another object
    //:   of 'optional<OPT_TYPE>'.  Verify that the 'value_type' of the destination
    //:    object matches the 'value_type' of the source object. [C-2]
    //:
    //: 3 Repeat steps 1 and 2 using rvalues. [C-3]
    //:
    //: 4 Assign '{}' to an object of 'OPT_TYPE' and verify that the resulting
    //:   'OPT_TYPE' object is disengaged. [C-4]
    //:
    //: 5 Assign '{}' to an object of 'optional<OPT_TYPE>' and verify that the
    //:   resulting 'optional<OPT_TYPE>' object is disengaged. [C-4]
    //:
    //: 6 Assign 'nullopt_t' object to an object of 'OPT_TYPE' and verify that
    //:  the resulting 'OPT_TYPE' object is disengaged. [C-5]
    //:
    //: 7 Assign 'nullopt_t' object to an object of 'optional<OPT_TYPE>' and
    //:   verify that the resulting 'optional<OPT_TYPE>' object is disengaged.
    //:   [C-5]
    //:
    //: 8 Execute the test with an allocator-aware TYPE. [C-6]
    //
    // Testing:
    //
    //   optional& operator= overload set
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=' OVERLOAD RESOLUTION"
               "\n=======================================\n");

    if (veryVerbose)
        printf("\tUsing non allocator-aware 'value_type'.\n");
    {
        typedef bsl::optional<Obj> OPT_TYPE;

        ValueType val = ValueType();
        Obj       source;
        OPT_TYPE  destination;
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());
        ASSERT(!source.has_value());

        destination.reset();
        source.emplace(val);
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == source.value());

        CObj constSource;
        destination.reset();
        destination = constSource;
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        source = {};
        ASSERT(!source.has_value());
        destination = {};
        ASSERT(!destination.has_value());
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

        source.emplace(val);
        destination = MoveUtil::move(source);
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == val);

        source = val;
        ASSERT(source.has_value());
        ASSERT(source.value() == val);

        destination.reset();
        destination = val;
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == source.value());

        source = bsl::nullopt;
        ASSERT(!source.has_value());
        destination = bsl::nullopt;
        ASSERT(!destination.has_value());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase10c()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(non_optional_type)'
    //   This test will ensure that the 'operator=(rhs)' member function, where
    //   'rhs' is not an 'optional' type, works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator=(rhs)', where 'rhs' is not of an 'optional' type,
    //:   or an engaged 'optional' assigns 'rhs' to the 'value_type' object.
    //:
    //: 2 Calling 'operator=(rhs)', where 'rhs' is not of an 'optional' type,
    //:   or a disengaged 'optional' creates a 'value_type' object initialized
    //:   with 'rhs'.
    //:
    //: 3 For allocator-aware types, the assignment to a disengaged 'optional'
    //:   uses the stored allocator.
    //:
    //: 4 Assignment of rvalues uses move assignment/construction where
    //:   available.
    //:
    //: 5 'optional' object of const qualified 'value_type' can not be assigned
    //:   to.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object. Verify that assignment from
    //:   'value_type', from 'const' qualified 'value_type', and from a type
    //:   assignable to 'value_type' results in  an 'optional' object having
    //:   the (possibly converted) value of 'rhs'. [C-1]
    //:
    //: 2 Repeat step 1 using rvalue 'rhs' and verify that 'rhs' was moved from.
    //:   [C-4]
    //:
    //: 3 Create a disengaged 'optional' object. Verify that assignment from
    //:   'value_type', from 'const' qualified 'value_type', and from type
    //:   assignable to 'value_type' results in an 'optional' object having the
    //:   (possibly converted) value of 'rhs'. [C-2]
    //:
    //: 4 Repeat step 4 using rvalue 'rhs' and verify that the 'rhs' was moved from.
    //:   [C-4]
    //:
    //: 5 If 'TYPE' is allocator-aware, verify that the value of the 'optional'
    //:   object after assignment is the allocator used at construction. [C-3]
    //:
    //: 6 Verify that an 'optional' object of 'const' qualified 'value_type'
    //:   can not be assigned to. Note that this test requires compilation
    //:   errors and needs to be enabled and checked manually. [C-5]
    //
    // Testing:
    //
    // optional& operator=(const TYPE&);
    // optional& operator=(TYPE&&);
    // optional& operator=(const ANY_TYPE&);
    // optional& operator=(ANY_TYPE&&);
    //
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (veryVeryVerbose)
            printf("\t\tChecking assignment to an engaged "
                   "'optional'.\n");
        {
            ValWithAllocator valBuffer(1, &da);
            ValueType&       vi = valBuffer.object();
            TEST_ASSIGN_VAL_ENGAGED(vi);

            ValWithAllocator valBuffer2(2, &da);
            ValueType&       vi2 = valBuffer2.object();
            TEST_MOVE_ASSIGN_VAL_ENGAGED(ValueType, vi2);
            ASSERT(vi2.value() == k_MOVED_FROM_VAL);

            const ValueType& cvi = valBuffer.object();
            TEST_ASSIGN_VAL_ENGAGED(cvi);
            TEST_ASSIGN_VAL_ENGAGED(MoveUtil::move(cvi));

            int i = 3;
            TEST_ASSIGN_VAL_ENGAGED(i);
            TEST_MOVE_ASSIGN_VAL_ENGAGED(int, i);

            const int ci = 6;
            TEST_ASSIGN_VAL_ENGAGED(ci);
            TEST_ASSIGN_VAL_ENGAGED(MoveUtil::move(ci));
        }
        if (veryVeryVerbose)
            printf("\t\tChecking assignment to a disengaged"
                   " 'optional'.\n");
        {
            ValWithAllocator valBuffer(1, &da);
            ValueType&       vi = valBuffer.object();
            TEST_ASSIGN_VAL_EMPTY(vi);

            ValWithAllocator valBuffer2(2, &da);
            ValueType&       vi2 = valBuffer2.object();
            TEST_MOVE_ASSIGN_VAL_EMPTY(vi2);
            ASSERT(vi2.value() == k_MOVED_FROM_VAL);

            const ValueType& cvi = valBuffer.object();
            TEST_ASSIGN_VAL_EMPTY(cvi);
            TEST_ASSIGN_VAL_EMPTY(MoveUtil::move(cvi));

            int i = 3;
            TEST_ASSIGN_VAL_EMPTY(i);

            const int ci = 6;
            TEST_ASSIGN_VAL_EMPTY(ci);
            TEST_ASSIGN_VAL_EMPTY(MoveUtil::move(ci));
        }
        {
            ObjC      mX = ValueType(0);
            ValueType vi = ValueType(1);
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
            mX = vi;  // this should not compile
#endif                //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        }
    }
}

template <class TYPE>
template <class OPT_TYPE>
void TestDriver<TYPE>::testCase10b_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(optional_type)'
    //   This test will ensure that the 'operator=(rhs)', where 'rhs' is an
    //   'optional' type, works as expected.
    //
    // Concerns:
    //: 1 That 'operator=(rhs)', where 'rhs' is a disengaged 'optional' object,
    //:   makes the target 'optional' object disengaged.
    //:
    //: 2 That 'operator=(rhs)', invoked on an engaged 'optional' object and
    //:   where 'rhs' is an engaged 'optional' object, assigns the value of the
    //:   'rhs''optional' to the value of the target 'optional' object.
    //:
    //: 3 That 'operator=(rhs)', invoked on a disengaged 'optional' object and
    //:   where 'rhs' is an engaged 'optional' object, constructs a
    //:   'value_type' object from the value of 'rhs''optional' object.
    //:
    //: 4 The behaviour is the same if 'rhs' is a const-qualified 'optional',
    //:   or an 'optional' of a const-qualified 'value_type'.
    //:
    //: 5 For allocator-aware types, the assignment from an 'optional' object
    //:   does not modify the allocator.
    //:
    //: 6 Assignment from rvalues uses move assignment where available.
    //:
    //: 7 Assignment to an 'optional' of const qualified 'value_type' is not
    //:   possible.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of 'value_type'.  Assign a disengaged
    //:   'optional' of the same type to it.  Verify that the destination object
    //:   is disengaged. [C-1]
    //:
    //: 2 Emplace a value into the test object.  Assign an engaged 'optional'
    //:   of the same type to it.  Verify thatthe value of the test object is the
    //:   same as that of the object assigned to it. [C-2]
    //:
    //: 3 Assign a disengaged 'optional' of a 'value_type' convertible to test
    //:   object's 'value_type'.  Verify that the destination object is
    //:   disengaged. [C-1]
    //:
    //: 4 Emplace a value into the test object.  Assign an engaged 'optional'
    //:   of a 'value_type' convertible to test object's 'value_type'.  Check
    //:   that the value of the test object is the same as that of the object
    //:   assigned to it. [C-2]
    //:
    //: 5 Repeat steps 1-4 using a const-qualified 'optional' object as 'rhs',
    //:   and using an 'optional' of const-qualified value type. [C-4]
    //:
    //: 6 Repeat steps 1-5 using an rvalue as 'rhs'.  Verify the object was
    //:   moved from if 'rhs' is a non const-qualified 'optional' of a non
    //:   const-qualified 'value_type', and copied otherwise. [C-6]
    //:
    //: 7 Repeat steps 1-6 using a disengaged 'optional' as the test object
    //:   in each step by calling 'reset' before each assignment. [C-3]
    //:
    //: 8 In steps 1-7, if 'value_type' is allocator-aware, verify that the test
    //:   object's allocator has not been modified. [C-5]
    //:
    //: 9 Verify that a const qualified 'optional' can not be assigned to.
    //:   Note that this test requires compilation failures and needs to be
    //:   enabled and checked manually. [C-7]
    //
    // Testing:
    //
    // optional& operator=(const optional&);
    // optional& operator=(optional&&);
    // optional& operator=(const optional<ANY_TYPE>&);
    // optional& operator=(optional<ANY_TYPE>&&);
    // optional& operator=(const std::optional<ANY_TYPE>&);
    // optional& operator=(std::optional<ANY_TYPE>&&);
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bslalg::ConstructorProxy<OPT_TYPE> SourceWithAllocator;
        SourceWithAllocator                        X(&da);
        OPT_TYPE&                                  source  = X.object();
        const OPT_TYPE&                            cSource = X.object();

        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(source);
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(source);
        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(MoveUtil::move(source));
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(MoveUtil::move(source));

        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(cSource);
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make these tests
        // fail
        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(MoveUtil::move(cSource));
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        source.emplace(3);
        TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source);
        TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source);
        TEST_MOVE_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source);
        source.emplace(3);
        TEST_MOVE_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source);
        source.emplace(3);

        TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(cSource);
        TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make these tests
        // fail
        TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(MoveUtil::move(cSource));
        TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
    {
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        typedef int OtherType;

        ObjC      mX = ValueType(0);
        OtherType i  = OtherType(3);
        mX           = i;
#endif  //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase10b()
{
    testCase10b_imp<bsl::optional<TYPE> >();
    testCase10b_imp<bsl::optional<int> >();
    testCase10b_imp<bsl::optional<const TYPE> >();
    testCase10b_imp<bsl::optional<const int> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    testCase10b_imp<std::optional<TYPE> >();
    testCase10b_imp<std::optional<int> >();
    testCase10b_imp<std::optional<TYPE> >();
    testCase10b_imp<std::optional<int> >();
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
}

template <class DEST_TYPE, class SRC_TYPE>
void testCase10b_udts_imp()
    // Perform the 10b test between optionals of UDTs
{
    TestDriver<DEST_TYPE>::template
        testCase10b_imp<bsl::optional<SRC_TYPE> >();
    TestDriver<DEST_TYPE>::template
        testCase10b_imp<bsl::optional<const SRC_TYPE> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    TestDriver<DEST_TYPE>::template
        testCase10b_imp<std::optional<SRC_TYPE> >();
    TestDriver<DEST_TYPE>::template
        testCase10b_imp<std::optional<const SRC_TYPE> >();
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
}

void testCase10b_udts()
    // Perform the 10b test between optionals of UDTs. Note that, unlike the
    // copy/move test (7a) we do not attempt to run this test with types
    // '<MyClass2a, Myclass1>', because 'MyClass2a' is constructible from
    // 'MyClass1' (via 'MyClass2&&'), but is not assignable from 'MyClass1'.
{
    testCase10b_udts_imp<MyClass1a, MyClass1>();
    testCase10b_udts_imp<MyClass2,  MyClass1>();
    testCase10b_udts_imp<MyClass2a, MyClass2>();
}

template <class TYPE>
void TestDriver<TYPE>::testCase10a_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(nullopt_t)'
    //   This test will ensure that the operator=(nullopt_t) member function
    //   works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator=(nullopt_t)' on a disengaged 'optional' leaves the
    //:   'optional' disengaged.
    //:
    //: 2 Calling 'operator=(nullopt_t)' on an engaged 'optional' makes the
    //:   'optional' disengaged.
    //:
    //: 3 For an allocator-aware 'value_type', calling 'operator=(nullopt_t)'
    //:   does not modify the allocator.
    //:
    //: 4 'operator=(nullopt_t)' can be called on a non const qualified
    //:   'optional' of a const qualified 'value_type'.
    //
    //
    // Plan:
    //: 1 Create a disengaged 'optional' of a non allocator-aware 'value_type'.
    //:   Call operator=(nullopt_t) and verify that the 'optional' object is
    //:   still disengaged [C-1]
    //:
    //: 2 Emplace a value in the 'optional'. Call operator=(nullopt_t) and
    //:   verify that the 'optional' object has been disengaged. [C-2]
    //:
    //: 3 In steps 1 and 2, if 'TYPE' is allocator-aware, verify the allocator
    //:   has not changed. [C-3]
    //:
    //: 4 Repeat the test using a const qualified 'TYPE'. [C-4]
    //
    // Testing:
    //
    //   operator=(nullopt_t)
    //
    // --------------------------------------------------------------------

    {
        Obj mX;
        mX = bsl::nullopt;
        ASSERT(!mX.has_value());

        mX.emplace(ValueType());
        mX = bsl::nullopt;
        ASSERT(!mX.has_value());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase10a()
{
    TestDriver<TYPE>::testCase10a_imp();
    TestDriver<const TYPE>::testCase10a_imp();
}

template <class OPT_TYPE1, class OPT_TYPE2>
void testCase9_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'swap' METHOD
    //
    // Concerns:
    //: 1 Swap of two disengaged objects is a no-op,
    //:
    //: 2 Swap of an engaged and a disengaged 'optional' moves the value
    //:   from the engaged object to another without calling swap for the
    //:   'value_type'.
    //:
    //: 3 Swap of two engaged objects calls swap for the 'value_type'.
    //:
    //: 4 Behaviour is the same for 'swap' member function, and 'swap' free
    //:   function
    //:
    //: 5 Behaviour is the same for allocator-aware and non allocator-aware
    //:   'value_type's.
    //
    // Plan:
    //: 1 Call 'swap' free function with two disengaged 'optional' objects and
    //:   verify 'value_type' 'swap' has not been called.  [C-1]
    //:
    //: 2 Call 'swap' free function with an engaged 'optional' object and a
    //:   disengaged 'optional' object.  Verify the disengaged 'optional' is
    //:   now engaged with a value from the other object, and that the other
    //:   object is now disengaged.  Verify that the 'value_type' 'swap' has
    //:   not been called.  [C-2]
    //:
    //: 3 Call swap free function with two engaged 'optional' objects and
    //:   verify 'value_type' 'swap' has been called.  [C-3]
    //:
    //: 4 Repeat steps 1-3 using the 'swap' member function.  [C-4]
    //:
    //: 5 Invoke the test using an allocator-aware 'TYPE' and a non
    //:   allocator-aware 'TYPE'.  [C-5]
    //
    //
    // Testing:
    //    void swap(optional& other);
    //    void swap(bsl::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
    //    void swap(std::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
    //    void swap(bsl::optional<TYPE>& lhs, std::optional<TYPE>& rhs);
    // --------------------------------------------------------------------

    using bsl::swap;

    typedef typename OPT_TYPE1::value_type VALUE_TYPE;

    {
        OPT_TYPE1 a;
        OPT_TYPE2 b;

        VALUE_TYPE::swapReset();
        swap(a, b);

        ASSERT(!VALUE_TYPE::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        VALUE_TYPE::swapReset();
        a.swap(b);

        ASSERT(!VALUE_TYPE::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        VALUE_TYPE obj1(1);
        VALUE_TYPE obj2(2);

        const VALUE_TYPE Zobj1(obj1);
        const VALUE_TYPE Zobj2(obj2);

        OPT_TYPE1 a = obj1;
        OPT_TYPE2 b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        VALUE_TYPE::swapReset();
        ASSERT(!VALUE_TYPE::swapCalled());
        swap(a, b);
        ASSERT(VALUE_TYPE::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        VALUE_TYPE::swapReset();
        ASSERT(!VALUE_TYPE::swapCalled());
        a.swap(b);
        ASSERT(VALUE_TYPE::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        OPT_TYPE1 nonNullObj(VALUE_TYPE(10));
        OPT_TYPE1 nonNullObjCopy(nonNullObj);
        OPT_TYPE2 nullObj;

        VALUE_TYPE::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!VALUE_TYPE::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        VALUE_TYPE::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!VALUE_TYPE::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
}

template <class TYPE>
void testCase9_noexcept()
    // Verify that noexcept specification of the member 'swap' function is
    // correct.
{
    bsl::optional<TYPE> a;
    bsl::optional<TYPE> b;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isNoexcept = bsl::is_nothrow_move_constructible<TYPE>::value &&
                            bsl::is_nothrow_swappable<TYPE>::value;
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
#endif
}

void testCase9()
{

    testCase9_imp<bsl::optional<Swappable>,
                   bsl::optional<Swappable> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // swap between bsl::optional and std::optional works only for non-AA
    // types

    testCase9_imp<bsl::optional<Swappable>,
                   std::optional<Swappable> >();


    testCase9_imp<std::optional<Swappable>,
                   bsl::optional<Swappable> >();
#endif

    testCase9_imp<bsl::optional<SwappableAA>,
                   bsl::optional<SwappableAA> >();

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    // Test 'noexcept'
    ASSERT( bsl::is_nothrow_move_constructible<Swappable>::value);
    ASSERT(!bsl::is_nothrow_swappable<Swappable>::value);
    testCase9_noexcept<Swappable>();

#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
    ASSERT( bsl::is_nothrow_move_constructible<int>::value);
    ASSERT( bsl::is_nothrow_swappable<int>::value);
    testCase9_noexcept<int>();

    ASSERT(!bsl::is_nothrow_move_constructible<
                                        ThrowMoveConstructible<true> >::value);
    ASSERT( bsl::is_nothrow_swappable<ThrowMoveConstructible<true> >::value);
    testCase9_noexcept<ThrowMoveConstructible<true> >();
#endif

    ASSERT(!bsl::is_nothrow_move_constructible<
                                       ThrowMoveConstructible<false> >::value);
    ASSERT(!bsl::is_nothrow_swappable<ThrowMoveConstructible<false> >::value);
    testCase9_noexcept<ThrowMoveConstructible<false> >();
#endif
}


template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE>
void TestDriver<TYPE>::testCase7b_imp()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION
    //   This test will ensure that the copy construction works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an engaged 'optional' of the same
    //:    type creates an engaged 'optional' where the 'value_type' object is
    //:   copy constructed from the 'value_type' object of the original
    //:   'optional' object.
    //:
    //: 2 Constructing an 'optional' from a disengaged 'optional' of the same
    //:    type creates a disengaged 'optional'.
    //:
    //: 3 The 'value_type' object in 'optional' is move constructed when
    //:   possible.
    //:
    //: 4 If allocator extended version of copy constructor is used, the
    //:   allocator passed to the constructor is the allocator of the newly
    //:   created 'optional'.
    //:
    //: 5 No unnecessary copies of the 'value_type' are created.
    //:
    //: 6 All of the above holds if the source object is an 'std::optional'.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of a non allocator-aware 'value_type'.
    //:   Use the created object to copy initialize another 'optional'
    //:   object of the same type.  Verify thatthe value of the new object and the
    //:   value of the original object match.  [C-1]
    //:
    //: 2 Repeat step 1 using a disengaged 'optional' object as the source
    //:   object.  Verify that the new 'optional' object is disengaged.  [C-2]
    //:
    //: 3 Repeat step 1 using an rvalue 'optional' as source object.  Check
    //:   that the move constructor is invoked where appropriate.  [C-3]
    //:
    //: 4 In steps 1-3, if 'value_type' is allocator-aware, verify that the
    //:   allocator of the newly created 'optional' object is the allocator
    //:   provided to the copy constructor.  [C-4]
    //:
    //: 5 In steps 1-4, for concern 4, verify that no unnecessary copies of the
    //:   'value_type' have been created.  [C-5]
    //:
    //: 6 Repeat steps 1-5 using an 'std::optional' as the source object.
    //:   [C-6]
    //
    // Testing:
    //
    //    optional(alloc_arg, alloc, const optional&);
    //    optional(alloc_arg, alloc, optional&&);
    //    optional(alloc_arg, alloc, const optional<ANY_TYPE> &);
    //    optional(alloc_arg, alloc, optional<ANY_TYPE>&&);
    //    optional(alloc_arg, alloc, const std::optional<ANY_TYPE> &);
    //    optional(alloc_arg, alloc, std::optional<ANY_TYPE>&&);
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::optional<DEST_TYPE> DEST_OPT_TYPE;
        {
            typedef bsl::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_EXT_COPY_FROM_EMPTY_OPT(source);
            TEST_EXT_COPY_FROM_EMPTY_OPT(csource);
            TEST_EXT_MOVE_FROM_EMPTY_OPT(source);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_EXT_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            source.emplace(3);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(source);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(csource);
            TEST_EXT_MOVE_FROM_ENGAGED_OPT(source);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            source.emplace(3);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            typedef std::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_EXT_COPY_FROM_EMPTY_OPT(source);
            TEST_EXT_COPY_FROM_EMPTY_OPT(csource);
            TEST_EXT_COPY_FROM_EMPTY_OPT(MoveUtil::move(source));
            TEST_EXT_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));

            source.emplace(3);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(source);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(csource);
            TEST_EXT_MOVE_FROM_ENGAGED_OPT(source);
            source.emplace(3);
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            TEST_EXT_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase7b()
{
    testCase7b_imp<TYPE, TYPE>();
    testCase7b_imp<TYPE, int>();
    testCase7b_imp<TYPE, const TYPE>();
    testCase7b_imp<TYPE, const int>();
    testCase7b_imp<const TYPE, TYPE>();
    testCase7b_imp<const TYPE, int>();
    testCase7b_imp<const TYPE, const TYPE>();
    testCase7b_imp<const TYPE, const int>();
}


template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
void TestDriver<TYPE>::testCase7a_imp()
{
    // --------------------------------------------------------------------
    // TESTING COPY/MOVE CONSTRUCTION
    //   This test will ensure that the copy construction works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an engaged 'optional' creates an
    //:   engaged 'optional' where the 'value_type' object is constructed
    //:   from the 'value_type' object of the original 'optional' object.
    //:
    //: 2 Constructing an 'optional' from a disengaged 'optional' of the same
    //:   type creates a disengaged 'optional'.
    //:
    //: 3 If 'value_type' is allocator-aware, and the source is an rvalue
    //:   'optional' of same 'value_type', the allocator is propagated.
    //:   Otherwise, the default allocator is used.
    //:
    //: 4 The 'value_type' object in 'optional' is move constructed when
    //:   possible.
    //:
    //: 5 No unnecessary copies of the 'value_type' are created.
    //:
    //: 6 All of the above holds if the source object is an 'std::optional'.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of 'SRC_TYPE' and use it to create an
    //:   'optional' of 'DEST_TYPE'.  Verify that the value of the new object
    //:   and the value of the original object match. [C-1]
    //:
    //: 2 Repeat step 1 using a disengaged 'optional' object as the source
    //:   object.  Verify that the created 'optional' object is disengaged.
    //:   [C-2]
    //:
    //: 3 If the 'value_type' is allocator-aware, verify that the allocator of
    //:   the newly created 'optional' object is the default allocator. [C-3]
    //:
    //: 4 Repeat steps 1-3 using an rvalue for source. Verify that the move
    //:   constructor is invoked where necessary.  Note that a part of this
    //:   test is extracted to a separate function to account for a bug in
    //:   older versions of cpplib. [C-4]
    //:
    //: 5 In step 4, if the 'value_type' is allocator-aware, verify that the
    //:   allocator is propagated as needed. [C-3]
    //:
    //: 6 In steps 1-5, verify that no unnecessary copies of the 'value_type'
    //:   have been created. [C-5]
    //:
    //: 8 Repeat steps 1-5 using an 'std::optional' as the source object.
    //
    // Testing:
    //
    //    optional(const optional&);
    //    optional(optional&&);
    //    optional(const optional<ANY_TYPE> &);
    //    optional(optional<ANY_TYPE>&&);
    //    optional(const std::optional<ANY_TYPE> &);
    //    optional(std::optional<ANY_TYPE>&&);
    // --------------------------------------------------------------------

    if (veryVerbose) printf("testCase7a_imp<%s><%s, %s, %d>\n",
                 bsls::NameOf<TYPE>().name(), bsls::NameOf<DEST_TYPE>().name(),
                 bsls::NameOf<SRC_TYPE>().name(), PROPAGATE_ON_MOVE);
    {

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bsl::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_COPY_FROM_EMPTY_OPT(source);
            TEST_COPY_FROM_EMPTY_OPT(csource);
            TEST_MOVE_FROM_EMPTY_OPT(source, PROPAGATE_ON_MOVE);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            source.emplace(3);
            TEST_COPY_FROM_ENGAGED_OPT(source);
            TEST_COPY_FROM_ENGAGED_OPT(csource);
                // extracted to testCase7a_imp_constmovebug
                // TEST_MOVE_FROM_ENGAGED_OPT(source, PROPAGATE_ON_MOVE);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            source.emplace(3);
            TEST_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            typedef std::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_COPY_FROM_EMPTY_OPT(source);
            TEST_COPY_FROM_EMPTY_OPT(csource);
            TEST_MOVE_FROM_EMPTY_OPT(source, false);
            TEST_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));

            source.emplace(3);
            TEST_COPY_FROM_ENGAGED_OPT(source);
            TEST_COPY_FROM_ENGAGED_OPT(csource);
                // extracted to testCase7a_imp_constmovebug
                //TEST_MOVE_FROM_ENGAGED_OPT(source, false);
            source.emplace(3);
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            TEST_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
}
template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
void TestDriver<TYPE>::testCase7a_imp_constmovebug()
{
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bsl::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE& source = sourceBuffer.object();

            source.emplace(3);
            TEST_MOVE_FROM_ENGAGED_OPT(source, PROPAGATE_ON_MOVE);
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            typedef std::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE& source = sourceBuffer.object();

            source.emplace(3);
            TEST_MOVE_FROM_ENGAGED_OPT(source, false);
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
}

#if BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if (BSLS_LIBRARYFEATURES_STDCPP_GNU &&                                       \
     (!defined(_GLIBCXX_RELEASE) ||                                           \
      (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 8)))
#define STD_OPTIONAL_CONST_MOVE_BUG 1
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1929
// Note that a bug report has been raised with Microsoft, see DRQS 168145193
#define STD_OPTIONAL_CONST_MOVE_BUG 1
#endif

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class TYPE>
void TestDriver<TYPE>::testCase7a()
{
    if (verbose) printf("testCase7a<%s>\n", bsls::NameOf<TYPE>().name());

    testCase7a_imp<TYPE, TYPE, true>();
    testCase7a_imp<TYPE, int, false>();
    testCase7a_imp<TYPE, const TYPE, false>();
    testCase7a_imp<TYPE, const int, false>();
    testCase7a_imp<const TYPE, TYPE, true>();
    testCase7a_imp<const TYPE, int, false>();
    testCase7a_imp<const TYPE, const TYPE, true>();
    testCase7a_imp<const TYPE, const int, false>();

    testCase7a_imp_constmovebug<TYPE, TYPE, true>();
    testCase7a_imp_constmovebug<TYPE, int, false>();
    testCase7a_imp_constmovebug<TYPE, const TYPE, false>();
    testCase7a_imp_constmovebug<TYPE, const int, false>();
    testCase7a_imp_constmovebug<const TYPE, TYPE, true>();
    testCase7a_imp_constmovebug<const TYPE, int, false>();
#ifndef STD_OPTIONAL_CONST_MOVE_BUG
    // In MSVC (see DRQS 168145193) and in older version of CPPLIB, the
    // constness of 'value_type' is not preserved when attempting to move
    // construct from an 'optional' of 'const' 'value_type'.  This results in a
    // move being invoked, despite the fact that the 'value_type' of the source
    // 'optional' object should not be modifiable.
    testCase7a_imp_constmovebug<const TYPE, const TYPE, true>();
#endif
    testCase7a_imp_constmovebug<const TYPE, const int, false>();
}

template <class DEST_TYPE, class SRC_TYPE>
void testCase7a_udts_imp()
    // Perform the 7a test between optionals of UDTs
{
    TestDriver<DEST_TYPE>::template
        testCase7a_imp<DEST_TYPE, SRC_TYPE, false>();
    TestDriver<DEST_TYPE>::template
        testCase7a_imp<const DEST_TYPE, SRC_TYPE, false>();
    TestDriver<DEST_TYPE>::template
        testCase7a_imp<DEST_TYPE, const SRC_TYPE, false>();
    TestDriver<DEST_TYPE>::template
        testCase7a_imp<const DEST_TYPE, const SRC_TYPE, false>();

    TestDriver<DEST_TYPE>::template
        testCase7a_imp_constmovebug<DEST_TYPE, SRC_TYPE, false>();
    TestDriver<DEST_TYPE>::template
        testCase7a_imp_constmovebug<const DEST_TYPE, SRC_TYPE, false>();
    TestDriver<DEST_TYPE>::template
        testCase7a_imp_constmovebug<DEST_TYPE, const SRC_TYPE, false>();
    TestDriver<DEST_TYPE>::template
        testCase7a_imp_constmovebug<
            const DEST_TYPE, const SRC_TYPE, false>();
}

void testCase7a_udts()
    // Perform the 7a test between optionals of UDTs
{
    testCase7a_udts_imp<MyClass1a, MyClass1>();
    testCase7a_udts_imp<MyClass2,  MyClass1>();
    testCase7a_udts_imp<MyClass2a, MyClass1>();
    testCase7a_udts_imp<MyClass2a, MyClass2>();
}


template <class OPT_TYPE1, class OPT_TYPE2>
void testCase6_imp_a()
{
    OPT_TYPE1 X;
    OPT_TYPE2 Y;
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    constexpr bool threeWayComparable = bsl::three_way_comparable_with<
                                               typename OPT_TYPE1::value_type,
                                               typename OPT_TYPE2::value_type>;
#endif

    //comparing two disengaged optionals
    ASSERT(X == Y);     // If bool(x) != bool(y), false;
    ASSERT(!(X != Y));  // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X < Y));   // If !y, false;
    ASSERT(!(X > Y));   // If !x, false;
    ASSERT(X <= Y);     // If !x, true;
    ASSERT(X >= Y);     // If !y, true;

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    if constexpr (threeWayComparable) {
        ASSERT(  X <=> Y == 0 );
        ASSERT(!(X <=> Y != 0));
        ASSERT(!(X <=> Y <  0));
        ASSERT(!(X <=> Y >  0));
        ASSERT(  X <=> Y <= 0 );
        ASSERT(  X <=> Y >= 0 );
    }
#endif

    //'rhs' disengaged, 'lhs' engaged
    Y.emplace(3);
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT((X < Y));    // If !y, false; otherwise, if !x, true;
    ASSERT(!(X > Y));   // If !x, false;
    ASSERT(X <= Y);     // If !x, true;
    ASSERT(!(X >= Y));  // If !y, true; otherwise, if !x, false;

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    if constexpr (threeWayComparable) {
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(  X <=> Y <  0 );
        ASSERT(!(X <=> Y >  0));
        ASSERT(  X <=> Y <= 0 );
        ASSERT(!(X <=> Y >= 0));
    }
#endif

    //'rhs' engaged, 'lhs' disengaged
    X.emplace(5);
    Y.reset();
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X < Y));   // If !y, false; otherwise, if !x, true;
    ASSERT((X > Y));    // If !x, false; otherwise, if !y, true;
    ASSERT(!(X <= Y));  // If !x, true; otherwise, if !y, false;
    ASSERT((X >= Y));   // If !y, true; otherwise, if !x, false;

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    if constexpr (threeWayComparable) {
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(!(X <=> Y <  0));
        ASSERT(  X <=> Y >  0 );
        ASSERT(!(X <=> Y <= 0));
        ASSERT(  X <=> Y >= 0 );
    }
#endif

    //both engaged, compare the values
    X.emplace(1);
    Y.emplace(3);
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT((X < Y));    // If !y, false; otherwise, if !x, true;
    ASSERT(!(X > Y));   // If !x, false; otherwise, if !y, true;
    ASSERT((X <= Y));   // If !x, true; otherwise, if !y, false;
    ASSERT(!(X >= Y));  // If !y, true; otherwise, if !x, false;

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    if constexpr (threeWayComparable) {
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(  X <=> Y <  0 );
        ASSERT(!(X <=> Y >  0));
        ASSERT(  X <=> Y <= 0 );
        ASSERT(!(X <=> Y >= 0));
    }
#endif
}

template <class OPT_TYPE, class VAL_TYPE>
void testCase6_imp_b()
{
    OPT_TYPE X;
    VAL_TYPE Y = 3;
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    constexpr bool threeWayComparable = bsl::three_way_comparable_with<
                                                 typename OPT_TYPE::value_type,
                                                 VAL_TYPE>;
#endif

    //comparison with a disengaged optional on 'rhs'
    ASSERT(!(X == Y));  // return bool(x) ? *x == v : false;
    ASSERT((X != Y));   // return bool(x) ? *x != v : true;
    ASSERT((X < Y));    // return bool(x) ? *x < v : true;
    ASSERT(!(X > Y));   // return bool(x) ? *x > v : false;
    ASSERT((X <= Y));   // return bool(x) ? *x <= v : true;
    ASSERT(!(X >= Y));  // return bool(x) ? *x >= v : false;

    //comparison with a disengaged optional on 'lhs'
    ASSERT(!(Y == X));  // return bool(x) ? v == *x : false;
    ASSERT((Y != X));   // return bool(x) ? v != *x : true;
    ASSERT(!(Y < X));   // return bool(x) ? v < *x : false;
    ASSERT((Y > X));    // return bool(x) ? v > *x : true;
    ASSERT(!(Y <= X));  // return bool(x) ? v <= *x : false;
    ASSERT((Y >= X));   // return bool(x) ? v >= *x : true;

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    if constexpr (threeWayComparable) {
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(  X <=> Y <  0 );
        ASSERT(!(X <=> Y >  0));
        ASSERT(  X <=> Y <= 0 );
        ASSERT(!(X <=> Y >= 0));

        ASSERT(!(Y <=> X == 0));
        ASSERT(  Y <=> X != 0 );
        ASSERT(!(Y <=> X <  0));
        ASSERT(  Y <=> X >  0 );
        ASSERT(!(Y <=> X <= 0));
        ASSERT(  Y <=> X >= 0 );
    }
#endif

    //comparison with an engaged optional on 'rhs'
    X.emplace(7);
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X < Y));   // If !y, false; otherwise, if !x, true;
    ASSERT((X > Y));    // If !x, false;
    ASSERT(!(X <= Y));  // If !x, true;
    ASSERT((X >= Y));   // If !y, true; otherwise, if !x, false;

    //comparison with an engaged optional on 'lhs'
    ASSERT(!(Y == X));  // If bool(x) != bool(y), false;
    ASSERT((Y != X));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT((Y < X));    // If !y, false; otherwise, if !x, true;
    ASSERT(!(Y > X));   // If !x, false;
    ASSERT((Y <= X));   // If !x, true;
    ASSERT(!(Y >= X));  // If !y, true; otherwise, if !x, false;

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    if constexpr (threeWayComparable) {
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(!(X <=> Y <  0));
        ASSERT(  X <=> Y >  0 );
        ASSERT(!(X <=> Y <= 0));
        ASSERT(  X <=> Y >= 0 );

        ASSERT(!(Y <=> X == 0));
        ASSERT(  Y <=> X != 0 );
        ASSERT(  Y <=> X <  0 );
        ASSERT(!(Y <=> X >  0));
        ASSERT(  Y <=> X <= 0 );
        ASSERT(!(Y <=> X >= 0));
    }
#endif
}

template <class TYPE>
void testCase6_imp_c()
{
    bsl::optional<TYPE> X;

    //comparison with a disengaged optional on 'rhs'
    ASSERT((X == bsl::nullopt));   // !x
    ASSERT(!(X != bsl::nullopt));  // bool(x)
    ASSERT(!(X < bsl::nullopt));   // false
    ASSERT(!(X > bsl::nullopt));   // bool(x)
    ASSERT((X <= bsl::nullopt));   // !x
    ASSERT((X >= bsl::nullopt));   // true

    //comparison with a disengaged optional on 'lhs'
    ASSERT((bsl::nullopt == X));   // !x
    ASSERT(!(bsl::nullopt != X));  // bool(x)
    ASSERT(!(bsl::nullopt < X));   // bool(x)
    ASSERT(!(bsl::nullopt > X));   // false
    ASSERT((bsl::nullopt <= X));   // true
    ASSERT((bsl::nullopt >= X));   // !x

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    ASSERT(  X <=> bsl::nullopt == 0 );
    ASSERT(!(X <=> bsl::nullopt != 0));
    ASSERT(!(X <=> bsl::nullopt <  0));
    ASSERT(!(X <=> bsl::nullopt >  0));
    ASSERT(  X <=> bsl::nullopt <= 0 );
    ASSERT(  X <=> bsl::nullopt >= 0 );

    ASSERT(  bsl::nullopt <=> X == 0);
    ASSERT(!(bsl::nullopt <=> X != 0));
    ASSERT(!(bsl::nullopt <=> X <  0));
    ASSERT(!(bsl::nullopt <=> X >  0));
    ASSERT(  bsl::nullopt <=> X <= 0);
    ASSERT(  bsl::nullopt <=> X >= 0);
#endif

    //comparison with an engaged optional on 'rhs'
    X.emplace(7);
    ASSERT(!(X == bsl::nullopt));  // !x
    ASSERT((X != bsl::nullopt));   // bool(x)
    ASSERT(!(X < bsl::nullopt));   // false
    ASSERT((X > bsl::nullopt));    // bool(x)
    ASSERT(!(X <= bsl::nullopt));  // !x
    ASSERT((X >= bsl::nullopt));   // true

    //comparison with an engaged optional on'lhs'
    ASSERT(!(bsl::nullopt == X));  // !x
    ASSERT((bsl::nullopt != X));   // bool(x)
    ASSERT((bsl::nullopt < X));    // bool(x)
    ASSERT(!(bsl::nullopt > X));   // false
    ASSERT((bsl::nullopt <= X));   // true
    ASSERT(!(bsl::nullopt >= X));  // !x

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    ASSERT(!(X <=> bsl::nullopt == 0));
    ASSERT(  X <=> bsl::nullopt != 0 );
    ASSERT(!(X <=> bsl::nullopt <  0));
    ASSERT(  X <=> bsl::nullopt >  0 );
    ASSERT(!(X <=> bsl::nullopt <= 0));
    ASSERT(  X <=> bsl::nullopt >= 0 );

    ASSERT(!(bsl::nullopt <=> X == 0));
    ASSERT(  bsl::nullopt <=> X != 0 );
    ASSERT(  bsl::nullopt <=> X <  0 );
    ASSERT(!(bsl::nullopt <=> X >  0));
    ASSERT(  bsl::nullopt <=> X <= 0 );
    ASSERT(!(bsl::nullopt <=> X >= 0));
#endif
}
void testCase6()
{
    // --------------------------------------------------------------------
    // TESTING RELATIONAL OPERATORS
    //
    // Concerns:
    //: 1 Two 'optional' objects can be compared if their 'value_type's are
    //:   comparable.  The result depends on whether the objects are engaged
    //:   or not.
    //:
    //: 2 We can compare an 'optional' object of 'value_type' V and a non
    //:   'optional' object of type U if U and V are comparable types.  The
    //:   result depends on whether the 'optional' object is engaged or not.
    //:
    //: 3 We can compare any 'optional' object with 'nulllopt_t'.  The result
    //:   depends on whether the 'optional' object is engaged or not.
    //:
    //: 4 'operator<=>' is consistent with '<', '>', '<=', '>='.
    //
    // Plan:
    //: 1 For each relation operator, compare two 'optional' objects of
    //:   comparable value types. Execute tests for a combination of engaged
    //:   and disengaged 'optional' objects.  [C-1]
    //:
    //: 2 For each relation operator, compare an 'optional' object of 'TYPE',
    //:   and an object of type comparable to TYPE.  Execute tests for an
    //:   engaged and disengaged 'optional' object.  [C-2]
    //:
    //: 3 For each relation operator, compare an 'optional' object and a
    //:   'nullopt_t' object.  Execute tests for an engaged and disengaged
    //:   'optional' object.  [C-3]
    //
    // Testing:
    //    bool operator==(const optional<LHS>&, nullopt_t&);
    //    bool operator!=(const optional<LHS>&, nullopt_t&);
    //    bool operator< (const optional<LHS>&, nullopt_t&);
    //    bool operator<=(const optional<LHS>&, nullopt_t&);
    //    bool operator> (const optional<LHS>&, nullopt_t&);
    //    bool operator>=(const optional<LHS>&, nullopt_t&);
    //    bool operator==(const optional<LHS>&, const RHS&);
    //    bool operator!=(const optional<LHS>&, const RHS&);
    //    bool operator< (const optional<LHS>&, const RHS&);
    //    bool operator<=(const optional<LHS>&, const RHS&);
    //    bool operator> (const optional<LHS>&, const RHS&);
    //    bool operator>=(const optional<LHS>&, const RHS&);
    //    bool operator==(const optional<LHS>&, const optional<RHS>&);
    //    bool operator!=(const optional<LHS>&, const optional<RHS>&);
    //    bool operator< (const optional<LHS>&, const optional<RHS>&);
    //    bool operator<=(const optional<LHS>&, const optional<RHS>&);
    //    bool operator>=(const optional<LHS>&, const optional<RHS>&);
    //    bool operator> (const optional<LHS>&, const optional<RHS>&);
    //    bool operator==(const nullopt_t&, const optional<RHS>&);
    //    bool operator!=(const nullopt_t&, const optional<RHS>&);
    //    bool operator< (const nullopt_t&, const optional<RHS>&);
    //    bool operator<=(const nullopt_t&, const optional<RHS>&);
    //    bool operator> (const nullopt_t&, const optional<RHS>&);
    //    bool operator>=(const nullopt_t&, const optional<RHS>&);
    //    bool operator==(const LHS&, const optional<RHS>&);
    //    bool operator!=(const LHS&, const optional<RHS>&);
    //    bool operator< (const LHS&, const optional<RHS>&);
    //    bool operator<=(const LHS&, const optional<RHS>&);
    //    bool operator> (const LHS&, const optional<RHS>&);
    //    bool operator>=(const LHS&, const optional<RHS>&);
    //    bool operator==(const std::optional<LHS>&, const optional<RHS>&);
    //    bool operator!=(const std::optional<LHS>&, const optional<RHS>&);
    //    bool operator< (const std::optional<LHS>&, const optional<RHS>&);
    //    bool operator<=(const std::optional<LHS>&, const optional<RHS>&);
    //    bool operator>=(const std::optional<LHS>&, const optional<RHS>&);
    //    bool operator> (const std::optional<LHS>&, const optional<RHS>&);
    //    bool operator==(const optional<LHS>&, const std::optional<RHS>&);
    //    bool operator!=(const optional<LHS>&, const std::optional<RHS>&);
    //    bool operator< (const optional<LHS>&, const std::optional<RHS>&);
    //    bool operator<=(const optional<LHS>&, const std::optional<RHS>&);
    //    bool operator>=(const optional<LHS>&, const std::optional<RHS>&);
    //    bool operator> (const optional<LHS>&, const std::optional<RHS>&);
    //    auto operator<=>(const optional<LHS>&, const optional<RHS>&);
    //    auto operator<=>(const optional<LHS>&, const RHS&);
    //    auto operator<=>(const optional<LHS>&, nullopt_t);
    //    auto operator<=>(const optional<LHS>&, const std::optional<RHS>&);

    if (veryVerbose)
        printf("\tComparison with an 'optional'.\n");
    {
        testCase6_imp_a<bsl::optional<int>, bsl::optional<MyClass2> >();

        testCase6_imp_a<bsl::optional<MyClass2>, bsl::optional<int> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        testCase6_imp_a<std::optional<int>, bsl::optional<int> >();

        testCase6_imp_a<bsl::optional<int>, std::optional<int> >();

        testCase6_imp_a<std::optional<MyClass2>, bsl::optional<MyClass2> >();

        testCase6_imp_a<bsl::optional<MyClass2>, std::optional<MyClass2> >();

        testCase6_imp_a<std::optional<int>, bsl::optional<MyClass2> >();

        testCase6_imp_a<bsl::optional<MyClass2>, std::optional<int> >();

        testCase6_imp_a<std::optional<MyClass2>, bsl::optional<int> >();

        testCase6_imp_a<bsl::optional<int>, std::optional<MyClass2> >();
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
    if (veryVerbose)
        printf("\tComparison with a non 'optional' .\n");
    {
        testCase6_imp_b<bsl::optional<int>, MyClass2>();

        testCase6_imp_b<bsl::optional<MyClass2>, int>();
    }
    if (veryVerbose)
        printf("\tComparison with a nullopt_t .\n");
    {
        testCase6_imp_c<int>();
        testCase6_imp_c<MyClass2>();
    }
}


template <class TYPE>
void TestDriver<TYPE>::testCase4f()
{
    // --------------------------------------------------------------------
    // TESTING 'operator*'
    //   This test will ensure that the 'operator*' works as expected.
    //
    //   MSVC-2015 has a bug which removes constness from temporaries in
    //   certain situations. For example :
    //
    //       bsl::string = CObj("s").value();
    //
    //   invokes non const qualified overload of 'value', despite the fact the
    //   temporary is of a const qualified 'bsl::optional'. In the following
    //   example, the constness is preserved:
    //
    //       Cobj temp = Cobj("s");
    //       bsl::string = MoveUtil::move(temp).value();
    //
    //    The tests have been written to take this issue into account.
    //
    //
    // Concerns:
    //: 1 Calling 'operator* 'on an engaged 'optional' returns a reference to
    //:   the contained value.
    //:
    //: 2 Returned reference is 'const' qualified if the 'optional' object
    //:   is 'const' qualified, or if the 'value_type' is 'const' qualified.
    //:
    //: 3 All of the above concerns apply whether or not the 'value_type' is
    //:   allocator-aware.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object.  Using 'operator*', verify that the
    //:   returned value of 'optional' object. [C-1]
    //:
    //: 2 Modify the value of the object obtained using 'operator*'. Verify thatthe
    //:   value of the 'optional' has been modified. [C-1]
    //:
    //: 3 Verify that the pointer returned from 'operator*' is not 'const'
    //:   qualified if neither the 'optional' object, nor the 'value_type' are
    //:   'const' qualified. [C-2]
    //:
    //: 4 Verify that the pointer returned from 'operator*' is 'const' qualified
    //:   if the 'optional' object is 'const' qualified. [C-2]
    //:
    //: 5 Verify that the pointer returned from 'operator*' is 'const' qualified
    //:   if the 'value_type' is 'const' qualified. [C-2]
    //:
    //: 6 Execute the test with allocator-aware and non allocator-aware 'TYPE'.
    //:   [C-3]
    //
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    // --------------------------------------------------------------------

    {
        Obj   mX(ValueType(4));
        CObj  cObjX(ValueType(8));
        ObjC  objcX(ValueType(9));
        CObjC cObjcX(ValueType(10));

        ASSERT((*mX).value() == 4);
        ASSERT((*cObjX).value() == 8);
        ASSERT((*objcX).value() == 9);
        ASSERT((*cObjcX).value() == 10);

        *mX = 6;
        ASSERT((*mX).value() == 6);

        ASSERT(!isConstRef(mX. operator*()));
        ASSERT(isConstRef(cObjX. operator*()));
        ASSERT(isConstRef(objcX. operator*()));
    }
    {
        CObj cObjX(ValueType(8));
        ObjC objcX(ValueType(9));

        ASSERT((*(Obj(ValueType(4)))).value() == 4);
        ASSERT((*(CObj(ValueType(8)))).value() == 8);
        ASSERT((*(ObjC(ValueType(9)))).value() == 9);
        ASSERT((*(CObjC(ValueType(10)))).value() == 10);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT(!isConstRef(Obj(ValueType(4)). operator*()));
        ASSERT(isConstRef((std::move(cObjX)). operator*()));
        ASSERT(isConstRef((std::move(objcX)). operator*()));
#endif
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase4e()
{
    // --------------------------------------------------------------------
    // TESTING 'operator->'
    //   This test will ensure that the 'operator->' works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator->' on an engaged 'optional' object returns a
    //:   pointer to the contained value object.
    //:
    //: 2 Returned pointer is 'const' qualified if the 'optional' object
    //:   is 'const' qualified, or if the 'value_type' is 'const' qualified.
    //:
    //: 3 All of the above concerns apply whether or not the 'value_type' is
    //:   allocator-aware.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object.  Using 'operator->', verify that the
    //:   returned value matches the value in the 'optional' object. [C-1]
    //:
    //: 2 Assign a value to the 'optional' object through a call to
    //:   'operator->'.  Verify thatthe value of the 'optional' object is as
    //:   expected. [C-1]
    //:
    //: 3 Verify that the pointer returned from 'operator->' is not 'const'
    //:   qualified if neither the 'optional' object, nor the 'value_type' are
    //:   'const' qualified. [C-2]
    //:
    //: 4 Verify that the pointer returned from 'operator->' is 'const'
    //:   qualified if the 'optional' object is 'const' qualified. [C-2]
    //:
    //: 5 Verify that the pointer returned from 'operator->' is 'const'
    //:   qualified if the 'value_type' is 'const' qualified. [C-2]
    //:
    //: 6 Run the test using both allocator-aware and non allocator-aware
    //:   'TYPE' [C-3]
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    // --------------------------------------------------------------------

    {
        Obj   mX(2);
        CObj  cObjX(5);
        ObjC  objcX(ValueType(9));
        CObjC cObjcX(ValueType(10));

        ASSERT(mX->value() == 2);
        ASSERT(cObjX->value() == 5);
        ASSERT(objcX->value() == 9);
        ASSERT(cObjcX->value() == 10);

        *(mX. operator->()) = 6;
        ASSERT(mX->value() == 6);

        ASSERT(!isConstPtr(mX. operator->()));
        ASSERT(isConstPtr(cObjX. operator->()));
        ASSERT(isConstPtr(objcX. operator->()));
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase4d()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED 'value_or' METHOD
    //   This test will ensure that the allocator extended 'value_or' method
    //   works as expected. We will also verify that single argument 'value_or'
    //   method with an optional created using an allocator extended
    //   constructor.
    //
    // Concerns:
    //: 1 Calling 'value_or' on a disengaged 'optional' object returns the
    //:   argument value converted to the 'value_type' of the 'optional'
    //:   object.
    //:
    //: 2 Calling 'value_or' on an engaged 'optional' object returns the value
    //:   in the 'optional' object.
    //:
    //: 3 In C++11, when calling 'value_or' on an engaged 'optional' rvalue, the
    //:   returned object is created by moving from the 'value_type' object in
    //:   'optional'.
    //:
    //: 4 It is possible to call 'value_or' on a constant 'optional' object.
    //:
    //: 5 The allocator of the object returned from the allocator extended
    //:   'value_or' method is the allocator specified in the call to
    //:   'value_or'.
    //
    // Plan:
    //
    //: 1 Create a disengaged 'optional' object.  Call 'value_or' and check
    //:   that the return object matches the argument given to 'value_or'.
    //:   [C-1]
    //:
    //: 2 Emplace a value in the 'optional' object. Call 'value_or' and check
    //:   that the returned value matches the value in the 'optional' object.
    //:   [C-2]
    //:
    //: 3 If ref qualifieres are supported, call 'value_or' on an engaged
    //:   'optional' rvalue. Verify the resulting objects has been created by
    //:   move construction. [C-3]
    //:
    //: 4 Repeat step 1 using a 'const' reference to the 'optional' object.
    //:   [C-4]
    //:
    //: 5 In steps 1-4 verify that the allocator of the returned object is the
    //:   one specified in the call to 'value_or'. [C-5]
    //
    // Testing:
    //  TYPE value_or(alloc_arg, alloc, ANY_TYPE&& ) const;
    //
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

#ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
        if (veryVerbose)
            printf("\tallocator tests of allocator extended "
                   "value_or'.\n");
        {
            Obj        mX(bsl::allocator_arg, &oa);
            const Obj& X = mX;

            bsls::ObjectBuffer<TYPE> valBuffer;
            bslma::ConstructionUtil::construct(
                valBuffer.address(), &da, 5);
            const TYPE& val = valBuffer.object();

            const ValueType& i1 = X.value_or(bsl::allocator_arg, &ta, val);
            ASSERT(i1.value() == 5);
            ASSERT(i1.get_allocator() == &ta);

            mX.emplace(8);
            const ValueType& i2 = mX.value_or(bsl::allocator_arg, &ta, val);
            ASSERT(i2.value() == 8);
            ASSERT(i2.get_allocator() == &ta);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            Obj              source(4);
            const ValueType& i3 =
                MoveUtil::move(source).value_or(bsl::allocator_arg, &ta, val);
            ASSERT(i3 == 4);
            ASSERT(source.value() == k_MOVED_FROM_VAL);
            ASSERT(i3.get_allocator() == &ta);
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        }
#endif  //BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase4c()
{
    // --------------------------------------------------------------------
    // TESTING 'value_or' METHOD
    //   This test will ensure that the 'value_or' method works as expected.
    //
    // Concerns:
    //: 1 Calling 'value_or' on a disengaged 'optional' object returns the
    //:   argument value converted to the 'value_type' of the 'optional'
    //:   object.
    //:
    //: 2 Calling 'value_or' on an engaged 'optional' object returns the value
    //:   in the 'optional' object.
    //:
    //: 3 In C++11, when calling 'value_or' on an engaged 'optional' rvalue, the
    //:   returned object is created by moving from the 'value_type' object in
    //:   'optional'.
    //:
    //: 4 It is possible to call 'value_or' on a constant 'optional' object.
    //:
    //: 5 If 'value_type' is allocator-aware, the allocator of the object
    //:   returned from a single argument 'value_or' method is determined by
    //:   the copy/move/conversion constructor of the 'value_type'.
    //
    // Plan:
    //
    //: 1 Create a disengaged 'optional' object.  Call 'value_or' and check
    //:   that the return object matches the argument given to 'value_or'.
    //:   [C-1]
    //:
    //: 2 Emplace a value in the 'optional' object. Call 'value_or' and check
    //:   that the returned value matches the value in the 'optional' object.
    //:   [C-2]
    //:
    //: 3 If ref qualifieres are supported, call 'value_or' on an engaged
    //:   'optional' rvalue. Verify the resulting objects has been created by
    //:   move construction. [C-3]
    //:
    //: 4 Repeat step 1 using a 'const' reference to the 'optional' object.
    //:   [C-4]
    //:
    //: 5 If the 'value_type' is allocator-aware, verify that the allocator of
    //:   the returned object is correct. [C-5]
    //
    // Testing:
    //  TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const;
    //
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose)
        printf("\tlvalue disengaged optional and lvalue\n");
    {
        ObjWithAllocator objBuf(&ta);
        Obj&             obj = objBuf.object();
        const Obj&       X   = obj;
        ValWithAllocator valBuffer(5, &oa);
        const ValueType& val      = valBuffer.object();
        ValueType        expected = val;
        const ValueType& dest     = X.value_or(val);
        ASSERT(dest == val);
        ASSERT(hasSameAllocator(dest, expected));
    }
    if (veryVerbose)
        printf("\tlvalue disengaged optional and rvalue\n");
    {
        ObjWithAllocator objBuf(&ta);
        Obj&             obj = objBuf.object();
        ValWithAllocator valBuffer(5, &oa);
        ValueType&       val  = valBuffer.object();
        const ValueType& dest = obj.value_or(MoveUtil::move(val));
        ValWithAllocator valCopyBuffer(5, &oa);
        ValueType&       valCopy  = valCopyBuffer.object();
        ValueType        expected(MoveUtil::move(valCopy));
        ASSERT(val.value() == k_MOVED_FROM_VAL);
        ASSERT(dest == expected);
        ASSERT(hasSameAllocator(dest, expected));
    }
    if (veryVerbose)
        printf("\tlvalue engaged optional\n");
    {
        ObjWithAllocator objBuf(4, &ta);
        Obj&             obj = objBuf.object();
        ValWithAllocator valBuffer(5, &oa);
        const ValueType& val      = valBuffer.object();
        const ValueType& dest     = obj.value_or(val);
        ValueType        expected = obj.value();
        ASSERT(dest == expected);
        ASSERT(hasSameAllocator(dest, expected));
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    if (veryVerbose)
        printf("\trvalue disengaged optional and lvalue\n");
    {
        ObjWithAllocator objBuf(&ta);
        Obj&             obj = objBuf.object();
        ValWithAllocator valBuffer(5, &oa);
        ValueType&       val = valBuffer.object();
        const ValueType& dest =
            MoveUtil::move(obj).value_or(MoveUtil::move(val));
        ValWithAllocator valCopyBuffer(5, &oa);
        ValueType&       valCopy  = valCopyBuffer.object();
        ValueType        expected(MoveUtil::move(valCopy));
        ASSERT(dest == expected);
        ASSERT(val.value() == k_MOVED_FROM_VAL);
        ASSERT(hasSameAllocator(dest, expected));
    }
    if (veryVerbose)
        printf("\trvalue engaged optional\n");
    {
        ObjWithAllocator objBuf(4, &ta);
        Obj&             obj  = objBuf.object();
        const ValueType& dest = MoveUtil::move(obj).value_or(77);
        obj.emplace(4);
        ValueType expected = (MoveUtil::move(obj.value()));
        ASSERT(dest == expected);
        ASSERT(obj.value().value() == k_MOVED_FROM_VAL);
        ASSERT(hasSameAllocator(dest, expected));
    }
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
}

template <class TYPE>
void TestDriver<TYPE>::testCase4b()
{
    // --------------------------------------------------------------------
    // TESTING 'value' METHOD
    //   This test will ensure that the 'value' method works as expected.
    //   The test relies on constructors and 'emplace' method.
    //
    //   MSVC-2015 has a bug which removes constness from temporaries in
    //   certain situations. For example :
    //
    //       bsl::string = CObj("s").value();
    //
    //   invokes non const qualified overload of 'value', despite the fact the
    //   temporary is of a const qualified 'bsl::optional'. In the following
    //   example, the constness is preserved:
    //
    //       Cobj temp = Cobj("s");
    //       bsl::string = MoveUtil::move(temp).value();
    //
    //    The tests have been written to take this issue into account.
    //
    // Concerns:
    //: 1 Calling 'value()' on a disengaged 'optional' throws
    //:   'bad_optional_access' exception.
    //:
    //: 2 Calling 'value()' on a engaged 'optional' returns the reference
    //:   to the 'value_type' object.
    //:
    //: 3 The reference returned from 'value()' is 'const' qualified if the
    //:   'optional' object is 'const' qualified, or if the 'value_type' is
    //:   'const' qualified.
    //:
    //: 4 In C++11 and onwards, the returned reference is an rvalue reference
    //:   if the 'optional' object is an rvalue.
    //:
    //: 5 It is possible to modify non constant 'optional' of non constant
    //:   'value_type' through the reference returned by 'value' method.
    //:
    //: 6 That 'value' method behaves the same for allocator-aware types and
    //:   non allocator-aware types.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' object. Call the 'value' method and
    //:   verify that the 'bad_optional_access' exception is thrown. [C-1]
    //:
    //: 2 Emplace a value in the 'optional' object. Call the 'value' method
    //:   and verify that the returned object has the expected value. [C-2]
    //:
    //: 3 Using 'isConstRef' verify the 'const' qualification of a reference
    //:   returned from 'value' on a non 'const' 'optional' of non 'const'
    //:   'value_type', 'const' 'optional' of non 'const' 'value_type', and
    //:   'const' 'optional' of 'const' 'value_type'. [C-3]
    //:
    //: 4 If ref qualifiers are supported using 'isRvalueRef' verify that the
    //:   reference returned from 'value' is an rvalue reference if the
    //:   'optional' object is an rvalue. [C-4]
    //:
    //: 5 Modify the value of the 'optional' object through the reference
    //:   returned from the 'value' method. Call 'value' method and verify that
    //:   the value of the 'optional' object has been modified. [C-5]
    //:
    //: 6 Call 'reset' on the 'optional' object. Call 'value' method and check
    //:   that the 'bad_optional_access' exception is thrown. [C-1]
    //:
    //: 7 In steps 1-6, verify that no unexpected exception is thrown. [C-1]
    //:
    //: 8 Run the test with an allocator-aware TYPE and a non allocator-aware
    //:   type. [C-6]
    //
    // Testing:
    //   TYPE& value() &;
    //   const TYPE& value() & const;
    //   TYPE&& value() &&;
    //   const TYPE&& value() && const;
    //
    // --------------------------------------------------------------------


#ifdef BDE_BUILD_TARGET_EXC
    bool unexpected_exception_thrown = false;
    try {
#endif
        Obj        mX;
        const Obj& X = mX;

#ifdef BDE_BUILD_TARGET_EXC
        bool bad_optional_exception_caught = false;
        try {
            (void) mX.value();
        }
        catch (const bsl::bad_optional_access&) {
            bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;
#endif

        mX.emplace(3);
        ASSERT(mX.value().value() == 3);

        TYPE& ri         = mX.value();
        ri.d_def.d_value = 7;
        ASSERT(mX.value().value() == 7);

        ObjC cmX(3);
        ASSERT(!isConstRef(mX.value()));
        ASSERT(isConstRef(X.value()));
        ASSERT(isConstRef(cmX.value()));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        ASSERT(!isRvalueRef(mX.value()));
        ASSERT(isRvalueRef(Obj(3).value()));
        ASSERT(isRvalueRef(ObjC(4).value()));
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

#ifdef BDE_BUILD_TARGET_EXC
        mX.reset();
        try {
            (void) mX.value();
        }
        catch (bsl::bad_optional_access&) {
            bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;
    }
    catch (...) {
        unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);
    unexpected_exception_thrown = false;
#endif  // defined(BDE_BUILD_TARGET_EXC)
}

template <class TYPE>
void TestDriver<TYPE>::testCase4a()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSION TO BOOL
    //   This test will ensure that the conversion to 'bool' works as expected.
    //   The test relies on reset and emplace member functions, as well as
    //   construction from a 'value_type'.
    //
    // Concerns:
    //: 1 A disengaged 'optional' when converted to 'bool' evaluates to 'false'
    //:   and returns 'false' from its 'has_value' method.
    //:
    //: 2 An engaged 'optional' when converted to 'bool' evaluates to 'true'
    //:   and returns 'true' from its 'has_value' method.
    //:
    //: 3 Both conversion to 'bool' and 'has_value' method can be performed/
    //:   invoked on a 'const' qualified 'optional' object.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' and verify that it evaluates to
    //:   'false' when converted to 'bool' and that 'has_value'
    //:   method returns 'false'. [C-1]
    //:
    //: 2 Emplace a value in the 'optional' object and verify that it evaluates
    //:   to 'true' when converted to 'bool' and that the 'has_value' method
    //:   returns 'true'. [C-2]
    //:
    //: 3 Call 'reset' method and verify that the 'optional' object evaluates
    //:   to 'false' when converted to 'bool' and that 'has_value' method
    //:   returns 'false'. [C-1]
    //:
    //: 4 Create an engaged 'optional' and verify that it evaluates to 'true'
    //:   when converted to 'bool' and that 'has_value' method returns 'true'.
    //:   [C-2]
    //:
    //: 5 Repeat step 1 using a 'const' qualified reference to test conversion
    //:  to 'bool' and to invoke the 'has_value' method. [C-3]
    //
    // Testing:
    //   operator bool() const;
    //   bool has_value() const;
    //
    // --------------------------------------------------------------------

    {
        Obj        mX;
        const Obj& X = mX;
        ASSERT(!X);
        ASSERT(false == X.has_value());

        mX.emplace(ValueType());
        ASSERT(mX);
        ASSERT(true == mX.has_value());

        mX.reset();
        ASSERT(!mX);
        ASSERT(false == mX.has_value());
    }
    {
        Obj mX(bsl::in_place);
        ASSERT(mX);
        ASSERT(true == mX.has_value());
    }
}


template <class TYPE>
void TestDriver<TYPE>::testCase3d()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED 'in_place_t' CONSTRUCTOR
    //   This test will ensure that the allocator extended 'initializer_list'
    //   'in_place_t' constructor works as expected.
    //
    // Concerns:
    //: 1 Calling 'in_place_t' constructor creates an engaged 'optional' whose
    //:   'value_type' object is created using the constructors arguments.
    //:
    //: 2 Arguments to the 'in_place_t' constructor are forwarded in correct
    //:   order to the 'value_type' object's constructor.
    //:
    //: 3 Arguments to the 'in_place_t' constructor are perfect forwarded to
    //:   the 'value_type' object's constructor, that is, rvalue argument is
    //:   forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //:   to the 'value_type' object's constructor
    //:
    //: 4 The newly created 'optional' object will use the allocator
    //:   specified in the constructor call.
    //:
    //: 5 If a braced init list is used for the first argument after the
    //:   'in_place' tag, 'optional' will deduce an initializer list and
    //:   forward it to the 'value_type'.
    //:
    //: 6 No unnecessary copies of the 'value_type' are created
    //
    //
    // Plan:
    //: 1 Create an object of 'TYPE' using a set of arguments 'ARGS'.  Create
    //:   an 'optional' object of 'TYPE' by invoking the 'in_place_t'
    //:   constructor with the same arguments.  Verify that the value of the
    //:   'value_type' object in the 'optional' object matches the value of the
    //:   'TYPE' object. [C-1]
    //:
    //: 2 Repeat step 1 using different number of constructor arguments [C-2].
    //:
    //: 3 Repeat step 1 and 2 using a mixture of rvalue and lvalue arguments such
    //:   that each argument position is tested with an rvalue and an lvalue.
    //:   Verify that the rvalue arguments have been moved from. [C-3]
    //:
    //: 4 Verify that the allocator specified in the constructor call was used
    //:   to construct the 'value_type' object. [C-4]
    //:
    //: 5 Repeat steps 1-4 with an additional initializer_list argument. [C-5]
    //:
    //: 6 In steps 1-5, verify that no unnecessary copies of the TYPE object have
    //:   been created. [C-6]
    //
    // Testing:
    //
    //   void optional(alloc_arg, alloc, in_place_t, Args&&...);
    //   void optional(alloc_arg, alloc, in_place_t, init_list, Args&&...);
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose)
        printf("\tUsing 'ValueType' argument.\n");
    {
        ValWithAllocator srcBuffer(&da);
        ValueType&       source  = srcBuffer.object();
        const ValueType& csource = srcBuffer.object();

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, source),
            (source, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, csource),
            (csource, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(source)),
            (MoveUtil::move(source), &ta),
            &oa);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make this test fail
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(csource)),
            (MoveUtil::move(csource), &ta),
            &oa);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }

    if (veryVerbose)
        printf("\tUsing variadic arguments.\n");

    {
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place), (&ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(VA1)),
            (MoveUtil::move(VA1), &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, VA1), (VA1, &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(VA1), VA2),
            (MoveUtil::move(VA1), VA2, &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, VA1, MoveUtil::move(VA2)),
            (VA1, MoveUtil::move(VA2), &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3),
                                (VA1, MoveUtil::move(VA2), VA3, &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6)),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8)),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10)),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10),
                                 &ta),
                                &oa);
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
        printf("\tUsing 'initializer_list' argument.\n");

    {
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, {1, 2, 3}),
            (std::initializer_list<int>{1, 2, 3}, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1)),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, {1, 2, 3}, VA1),
            (std::initializer_list<int>{1, 2, 3}, VA1, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10),
                                 &ta),
                                &oa);
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

}

template <class TYPE>
void TestDriver<TYPE>::testCase3c()
{
    // --------------------------------------------------------------------
    // TESTING 'in_place_t' CONSTRUCTOR
    //   This test will ensure that the 'in_place_t' constructor works
    //   as expected.
    //
    // Concerns:
    //: 1 Calling 'in_place_t' constructor creates an engaged 'optional' whose
    //:   'value_type' object is created using the constructors arguments.
    //:
    //: 2 Arguments to the 'in_place_t' constructor are forwarded in correct
    //:   order to the 'value_type' object's constructor.
    //:
    //: 3 Arguments to the 'in_place_t' constructor are perfect forwarded to
    //:   the 'value_type' object's constructor, that is, rvalue argument is
    //:   forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //:   to the 'value_type' object's constructor
    //:
    //: 4 If the type uses an allocator and no allocator is provided, the
    //:   'optional' object will use the default allocator.
    //:
    //: 5 If a braced init list is used for the first argument after the
    //:   'in_place' tag, 'optional' will deduce an initializer list and
    //:   forward it to the 'value_type'.
    //:
    //: 6 No unnecessary copies of the 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Create an object of 'TYPE' using a set of arguments 'ARGS'.  Create
    //:   an 'optional' object of 'TYPE' by invoking the 'in_place_t'
    //:   constructor with the same arguments.  Verify that the value of the
    //:   'value_type' object in the 'optional' object matches the value of the
    //:   'TYPE' object.  [C-1]
    //:
    //: 2 Repeat step 1 using different number of constructor arguments [C-2].
    //:
    //: 3 Repeat steps 1 and 2 using a mixture of rvalue and lvalue  arguments such
    //:   that each argument position is tested with an rvalue and an lvalue.
    //:   Verify that the rvalue arguments have been moved from.  [C-3]
    //:
    //: 4 If 'value_type' is allocator-aware, verify that the default allocator
    //:   was used to construct the 'value_type' object.  [C-4]
    //:
    //: 5 Repeat steps 1-4 with an additional braced init list argument.  [C-5]
    //:
    //: 6 In steps 1-5, verify that no unnecessary copies of the TYPE object
    //:   have been created.  [C-6]
    //
    // Testing:
    //
    //    optional(in_place_t, ARGS&&...);
    //    optional(in_place_t, std::initializer_list, ARGS&&...);
    //
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose)
        printf("\tUsing 'ValueType' argument.\n");
    {
        ValWithAllocator srcBuffer(&oa);
        ValueType&       source  = srcBuffer.object();
        const ValueType& csource = srcBuffer.object();

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, source), (source, &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, csource), (csource, &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, MoveUtil::move(source)),
                                (MoveUtil::move(source), &oa),
                                &da);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make this test fail
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, MoveUtil::move(csource)),
                                (MoveUtil::move(csource), &oa),
                                &da);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }

    if (veryVerbose)
        printf("\tUsing variadic arguments.\n");

    {
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place), (&oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, MoveUtil::move(VA1)),
                                (MoveUtil::move(VA1), &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, VA1), (VA1, &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, MoveUtil::move(VA1), VA2),
                                (MoveUtil::move(VA1), VA2, &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, VA1, MoveUtil::move(VA2)),
                                (VA1, MoveUtil::move(VA2), &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, VA1, MoveUtil::move(VA2), VA3),
                                (VA1, MoveUtil::move(VA2), VA3, &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place,
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &oa),
            &da);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6)),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8)),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10)),
                                (VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10),
                                 &oa),
                                &da);
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
        printf("\tUsing 'initializer_list' argument.\n");

    {
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1, 2, 3}),
                                (std::initializer_list<int>{1, 2, 3}, &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1)),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, VA1),
            (std::initializer_list<int>{1, 2, 3}, VA1, &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1), VA2),
            (std::initializer_list<int>{1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             &oa),
            &da);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
            (std::initializer_list<int>{1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
            (std::initializer_list<int>{1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 VA6,
                                 MoveUtil::move(VA7),
                                 VA8,
                                 MoveUtil::move(VA9),
                                 VA10,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 MoveUtil::move(VA6),
                                 VA7,
                                 MoveUtil::move(VA8),
                                 VA9,
                                 MoveUtil::move(VA10),
                                 &oa),
                                &da);
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE>
void TestDriver<TYPE>::testCase3b_imp()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED CONSTRUCTION FROM A VALUE
    //   This test will ensure that the allocator extended construction from
    //   a value works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from a 'value_type' object creates an
    //:   engaged 'optional' with the value of the source object.
    //:
    //: 2 The allocator provided in the constructor is the allocator used for
    //:   the newly created 'optional'.
    //:
    //: 3 Move construction of the 'value_type' is used when possible.
    //:
    //: 4 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //
    //: 1 Create a value object and use it as the source object for an
    //:   'optional' by invoking an allocator extended version of the
    //:   constructor taking a value.  Verify thatthe constructed 'optional' object
    //:   is engaged and contains the value of the source object.  [C-1]
    //:
    //: 2 Repeat step 1 using a 'value_type' object and an object of type convertible
    //:   to 'value_type' as the source object.  [C-1]
    //:
    //: 3 If 'value_type' is allocator-aware, verify that the allocator of the new
    //:   'optional' object is the allocator provided in the constructor call.
    //:   [C-2]
    //:
    //: 4 In steps 1-2, verify that no unnecessary copies of the 'value_type'
    //:   are created by comparing the number of copy/move constructors invoked
    //:   to creating an instance of 'value_type' object from the source object
    //:   [C-4]
    //:
    //: 5 Repeat steps 1-4 using a const source object, an rvalue source
    //:   object, and a const rvalue source object. [C-4]
    //
    // Testing:
    //
    //   optional(allocator_arg_t, allocator_type, const TYPE&);
    //   optional(allocator_arg_t, allocator_type, TYPE&&);
    //   optional(allocator_arg_t, allocator_type, const ANY_TYPE&);
    //   optional(allocator_arg_t, allocator_type, ANY_TYPE&&);
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::optional<DEST_TYPE> DEST_OPT_TYPE;
        {
            typedef bslalg::ConstructorProxy<SRC_TYPE> SourceWithAllocator;
            SourceWithAllocator                        sourceBuffer(22, &oa);
            SRC_TYPE&       source  = sourceBuffer.object();
            const SRC_TYPE& csource = sourceBuffer.object();

            TEST_EXT_COPY_FROM_VALUE(source);
            TEST_EXT_COPY_FROM_VALUE(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_EXT_COPY_FROM_VALUE(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_EXT_MOVE_FROM_VALUE(source);
        }
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase3b()
{
    testCase3b_imp<TYPE, TYPE>();
    testCase3b_imp<TYPE, int>();
    testCase3b_imp<const TYPE, TYPE>();
    testCase3b_imp<const TYPE, int>();
}


template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE>
void TestDriver<TYPE>::testCase3a_imp()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTION FROM A VALUE
    //   This test will ensure that the copy construction from a value
    //   works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from a 'value_type' object creates an
    //:   engaged 'optional' with the value of the source object.
    //:
    //: 2 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //:
    //: 3 Move construction of the 'value_type' is used when possible.
    //:
    //: 4 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //
    //: 1 Create a value object and use it as the source object for an
    //:   'optional'.  Verify that the constructed 'optional' object is engaged and
    //:   contains the value of the object used to initialise it.  [C-1]
    //:
    //: 2 Repeat step 1 using a 'value_type' object and an object of type convertible
    //:   to 'value_type' as the source object.  [C-1]
    //:
    //: 3 If 'value_type' is allocator-aware, verify that the allocator of the new
    //:   'optional' object is the default allocator [C-2]
    //:
    //: 4 In steps 1-3, verify that no unnecessary copies of the 'value_type'
    //:   are created by comparing the number of copy/move constructors invoked
    //:   to creating an instance of 'value_type' object from the source object
    //:   [C-4]
    //:
    //: 5 Repeat steps 1-4 using a const source object, an rvalue source
    //:   object, and a const rvalue source object.  [C-4]
    //
    // Testing:
    //
    //    optional(const TYPE&);
    //    optional(TYPE&&);
    //    optional(const ANY_TYPE&);
    //    optional(ANY_TYPE&&);
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bslalg::ConstructorProxy<SRC_TYPE> SourceWithAllocator;
            SourceWithAllocator                        sourceBuffer(21, &oa);
            SRC_TYPE&       source  = sourceBuffer.object();
            const SRC_TYPE& csource = sourceBuffer.object();

            TEST_COPY_FROM_VALUE(source);
            TEST_COPY_FROM_VALUE(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_COPY_FROM_VALUE(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_MOVE_FROM_VALUE(source);
        }
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase3a()
{
    testCase3a_imp<TYPE, TYPE>();
    testCase3a_imp<TYPE, int>();
    testCase3a_imp<const TYPE, TYPE>();
    testCase3a_imp<const TYPE, int>();
}

template <class TYPE>
void TestDriver<TYPE>::testCase2c_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'reset' MEMBER FUNCTION
    //   This test will ensure that the 'reset 'function works as expected.
    //   The test relies on constructors, 'has_value' and 'emplace' methods.
    //
    // Concerns:
    //: 1 Calling 'reset' on an disengaged 'optional' leaves it disengaged.
    //:
    //: 2 Calling 'reset' on an engaged 'optional' makes it disengaged and the
    //:   destructor of the 'value_type' is invoked.
    //:
    //: 3 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is 'const'.
    //:
    //: 4 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is allocator-aware.
    //:
    //: 5 For an allocator-aware 'value_type', calling 'reset' does not modify
    //:   the allocator.
    //
    // Plan:
    //: 1 Create a disengaged 'optional'object.  Call 'reset' on the created
    //:   object and verify that it is still disengaged. [C-1]
    //:
    //: 2 Emplace a value in the 'optional' object. Call 'reset' on the test
    //:   object and verify that it has been disengaged and that the destructor
    //:   of the 'value_type' has been invoked. [C-2]
    //:
    //: 3 In steps 1-2, if the 'value_type' is allocator-aware, verify that
    //:   the 'get_allocator' method returns the allocator used to construct
    //:   the 'optional' type after each call to 'reset. [C-5]
    //:
    //: 4 Run the test using a const qualified TYPE. [C-3]
    //:
    //: 5 Run the test using an allocator-aware TYPE. [C-4]
    //
    // Testing:
    //   void reset();
    //
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    {
        Obj mX;

        mX.reset();
        ASSERT(false == mX.has_value());
        ASSERT(checkAllocator(mX, &da));

        mX.reset();
        ASSERT(false == mX.has_value());
        ASSERT(checkAllocator(mX, &da));

        mX.emplace(TYPE());
        ASSERT(true == mX.has_value());

        int dI = ValueType::s_destructorInvocations;
        mX.reset();
        ASSERT(false == mX.has_value());
        ASSERT(checkAllocator(mX, &da));
        ASSERT(dI == ValueType::s_destructorInvocations - 1);
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase2c()
{
    TestDriver<TYPE>::testCase2c_imp();
    TestDriver<const TYPE>::testCase2c_imp();
}

template <class TYPE>
void TestDriver<TYPE>::testCase2b()
{
    // --------------------------------------------------------------------
    //  ALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS
    //   This test will ensure that the allocator extended construction of a
    //   disengaged 'optional' is working as expected.
    //
    // Concerns:
    //: 1 That the allocator extended default constructor creates a disengaged
    //:   object, as determined by 'has_value' returning 'false'.
    //:
    //: 2 That the allocator extended constructor taking 'nullopt_t' object
    //:   creates a disengaged object determined by 'has_value' returning
    //:   'false'.
    //:
    //: 3 That the 'get_allocator' method returns the allocator passed in to
    //:   the constructors.
    //:
    //: 4 These constructors do not allocate any memory.
    //
    // Plan:
    //: 1 Construct an 'optional' object using allocator extended default
    //:   construction and verify that the 'optional' object is disengaged.
    //:   [C-1]
    //:
    //: 2 Construct an 'optional' object using allocator extended constructor
    //:   that takes 'nullopt_t' argument and verify that the 'optional' object
    //:   is disengaged. [C-2]
    //:
    //: 3 In step 1 and 2, verify that the 'get_allocator' method returns the
    //:   allocator used in 'optional' construction. [C-3]
    //:
    //: 4 In steps 1 and 2, verify no memory has been allocated. [C-4]
    //
    // Testing:
    //   optional(bsl::allocator_arg_t, allocator_type);
    //   optional(bsl::allocator_arg_t, allocator_type, nullopt_t);
    //   allocator_type get_allocator() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::DefaultAllocatorGuard oag(&oa);

    {
        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor oam(&oa);

        Obj X(bsl::allocator_arg, &oa);
        ASSERT(!X.has_value());
        ASSERT(X.get_allocator() == &oa);
        ASSERT(dam.isTotalSame());
        ASSERT(oam.isTotalSame());
    }
    {
        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor oam(&oa);

        Obj X = Obj(bsl::allocator_arg, &oa, nullopt);
        ASSERT(!X.has_value());
        ASSERT(X.get_allocator() == &oa);
        ASSERT(dam.isTotalSame());
        ASSERT(oam.isTotalSame());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase2a()
{
    // --------------------------------------------------------------------
    // DISENGAGED CONSTRUCTORS AND DESTRUCTOR
    //   This test will ensure that the construction of a disengaged 'optional'
    //   is working as expected.
    //
    // Concerns:
    //: 1 That the default constructor creates a disengaged object, as
    //:   determined by 'has_value' returning 'false'.
    //:
    //: 2 That the constructor taking 'nullopt_t' object creates a disengaged
    //:   object determined by 'has_value' returning 'false'.
    //:
    //: 3 If the 'value_type' of the 'optional' is allocator-aware (AA), then
    //:   the 'get_allocator' method returns the default allocator when using
    //:   these constructors.
    //:
    //: 4 These constructors do not allocate any memory, whether or not the
    //:   'value_type' is AA.
    //:
    //: 5 If an engaged 'optional' object is destroyed, the destructor of the
    //:   'value_type' is invoked.
    //
    // Plan:
    //: 1 Construct an 'optional' object using default construction and verify
    //:   that the 'optional' object is disengaged. [C-1]
    //:
    //: 2 Construct an 'optional' object using the constructor that takes
    //:   'nullopt_t' argument and verify that the 'optional' object is
    //:   disengaged. [C-2]
    //:
    //: 3 In steps 1 and 2, if the 'value_type' is allocator-aware, verify that
    //:   the 'get_allocator' method returns the default allocator [C-3]
    //:
    //: 4 In steps 1 and 2, verify no memory has been allocated. [C-4]
    //:
    //: 5 in steps 1 and 2, verify that the 'value_type' destructor is not
    //:   invoked when the optional object is destroyed. [C-5]
    //:
    //: 6 Emplace a value into the disengaged optional. Verify that the
    //:   'value_type' destructor is invoked when the optional object is
    //:   destroyed. [C-5]
    //
    // Testing:
    //   optional();
    //   optional(nullopt_t);
    //   ~optional();
    // --------------------------------------------------------------------

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        int                          dI = ValueType::s_destructorInvocations;

        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj        mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(!X);
            ASSERT(checkAllocator(X, &da));
            ASSERT(dam.isTotalSame());
            dI = ValueType::s_destructorInvocations;
        }
        ASSERT(dI == ValueType::s_destructorInvocations);
        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj        mX = Obj(nullopt);
            const Obj& X  = mX;
            ASSERT(!X.has_value());
            ASSERT(!X);
            ASSERT(checkAllocator(X, &da));
            ASSERT(dam.isTotalSame());
            dI = ValueType::s_destructorInvocations;
        }
        ASSERT(dI == ValueType::s_destructorInvocations);
        {
            Obj mX = ValueType(5);
            ASSERT(mX.has_value());
            ASSERT(mX);
            dI = ValueType::s_destructorInvocations;
        }
        ASSERT(dI == ValueType::s_destructorInvocations - 1);
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Create a disengaged 'optional'
    //:
    //: 2 For allocator-aware 'value_type, verify that 'get_allocator' method
    //:   returns the allocator used to construct the 'optional' object.
    //:
    //: 3 Test that 'has_value' on a disengaged 'optional' returns 'false'
    //:
    //: 4 Test that disengaged 'optional' evaluates to 'false'
    //:
    //: 5 Emplace a value into the 'optional' object and verify that
    //:   'value()' method returns the emplaced value.
    //:
    //: 6 Verify reset() method disengages an engaged 'optional'.
    //:
    //: 7 Create an engaged 'optional' and verify that 'value()' returns the
    //:   expected value
    //:
    //: 8 Test that an engaged 'optional' evaluates to 'true'
    //
    // Testing:
    //   BREATHING TEST
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    Obj X;
    ASSERT(checkAllocator(X, &da));
    ASSERT(!X.has_value());
    ASSERT(!X);

    TYPE val = TYPE();
    X.emplace(val);
    ASSERT(checkAllocator(X, &da));
    ASSERT(X.has_value());
    ASSERT(X.value() == val);

    X.reset();
    ASSERT(!X.has_value());

    Obj Y = val;
    ASSERT(Y.has_value());
    ASSERT(checkAllocator(X, &da));
    ASSERT(Y.value() == val);
    ASSERT(Y);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::optional' cannot be deduced from the constructor parameters.
    //..
    // optional()
    // optional(bsl::nullopt_t)
    // optional(bsl::in_place_t, ...)
    // optional(bsl::in_place_t, initializer_list, ...)
    // optional(bsl::allocator_arg_t, allocator_type);
    // optional(bsl::allocator_arg_t, allocator_type, bsl::nullopt_t);
    // optional(bsl::allocator_arg_t, allocator_type, bsl::in_place_t, ...);
    // optional(bsl::allocator_arg_t, allocator_type, bsl::in_place_t,
    //                                                  initializer_list, ...);
    // All the converting constructors
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void AllocatorAwareConstructors ()
        // Test that constructing a 'bsl::optional' from 'allocator_arg_t', an
        // allocator and either a 'T' or an 'optional<T>' deduces the correct
        // type.
        //..
        // optional(bsl::allocator_arg_t, allocator_type, T) -> optional<T>
        // optional(bsl::allocator_arg_t, allocator_type, const optional&  o)
        //    -> decltype(o)
        // optional(bsl::allocator_arg_t, allocator_type,       optional&& o)
        //    -> decltype(o)
        //..
    {
        bsl::allocator <char> a;
        bsl::allocator_arg_t  allocator_arg;
        bslma::Allocator     *pa = nullptr;
        bslma::TestAllocator *ta = nullptr;
        bsl::string           s;

        typedef bsl::optional<bsl::string> o1_t;
        bsl::optional o1a(allocator_arg, a,  s);
        bsl::optional o1b(allocator_arg, pa, s);
        bsl::optional o1c(allocator_arg, ta, s);
        ASSERT_SAME_TYPE(decltype(o1a), o1_t);
        ASSERT_SAME_TYPE(decltype(o1b), o1_t);
        ASSERT_SAME_TYPE(decltype(o1c), o1_t);

        typedef bsl::optional<bsl::string> o2_t;
        o2_t          o2(s);
        bsl::optional o2a(allocator_arg, a,  o2);
        bsl::optional o2b(allocator_arg, pa, o2);
        bsl::optional o2c(allocator_arg, ta, o2);
        ASSERT_SAME_TYPE(decltype(o2a), o2_t);
        ASSERT_SAME_TYPE(decltype(o2b), o2_t);
        ASSERT_SAME_TYPE(decltype(o2c), o2_t);

        typedef bsl::optional<bsl::string> o3_t;
        o3_t          o3(s);
        bsl::optional o3a(allocator_arg, a,  std::move(o3));
        bsl::optional o3b(allocator_arg, pa, std::move(o3));
        bsl::optional o3c(allocator_arg, ta, std::move(o3));
        ASSERT_SAME_TYPE(decltype(o3a), o3_t);
        ASSERT_SAME_TYPE(decltype(o3b), o3_t);
        ASSERT_SAME_TYPE(decltype(o3c), o3_t);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests

//#define BSLSTL_OPTIONAL_COMPILE_FAIL_NOT_AN_ALLOCATOR_AWARE_TYPE
#if defined(BSLSTL_OPTIONAL_COMPILE_FAIL_NOT_AN_ALLOCATOR_AWARE_TYPE)
        bsl::optional  o97(allocator_arg, pa, 42);
        // this should fail to compile ('int' is not an allocator-aware type)
#endif

//#define BSLSTL_OPTIONAL_COMPILE_FAIL_NOT_AN_ALLOCATOR
#if defined(BSLSTL_OPTIONAL_COMPILE_FAIL_NOT_AN_ALLOCATOR)
        bsl::string *ps = nullptr;
        bsl::optional  o98(allocator_arg, ps, s);
        // this should fail to compile ('bsl::string *' is not an allocator)
#endif
        }

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::optional' from a single argument
        // deduces the correct type.
        //..
        // optional(T) -> optional<T>
        // optional(const optional&  o) -> decltype(o)
        // optional(      optional&& o) -> decltype(o)
        //..
    {
        bsl::optional o1(1);
        ASSERT_SAME_TYPE(decltype(o1), bsl::optional<int>);

        bsl::optional<double> o2(2.0);
        bsl::optional         o2a(o2);
        ASSERT_SAME_TYPE(decltype(o2a), bsl::optional<double>);

        bsl::optional<float>  o3(3.0f);
        bsl::optional         o3a(std::move(o3));
        ASSERT_SAME_TYPE(decltype(o3a), bsl::optional<float>);
    }

#undef ASSERT_SAME_TYPE
};
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int test      = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST  %s CASE %d \n", __FILE__, test);

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) {  case 0:
      case 27: {
        //---------------------------------------------------------------------
        // TESTING CONCEPTS
        //
        // Concern:
        //: 1 'Optional_ConvertibleToBool' is 'true' only for types convertible
        //:   to 'bool'.
        //:
        //: 2 'Optional_DerivedFromOptional' is 'true' only for types that are
        //:   either 'bsl::optional' or 'std::optional'  or derived from one of
        //:   them.
        //
        // Plan:
        //: 1 Apply the concepts to different types and verify the result.
        //
        // Testing:
        //   CONCEPTS
        //---------------------------------------------------------------------
        if (verbose) printf("\nTESTING CONCEPTS"
                            "\n================\n");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        BSLMF_ASSERT(( Optional_ConvertibleToBool<bool>));
        BSLMF_ASSERT(( Optional_ConvertibleToBool<int>));
        BSLMF_ASSERT((!Optional_ConvertibleToBool<bsl::weak_ordering>));
        BSLMF_ASSERT((!Optional_ConvertibleToBool<bsl::nullopt_t>));

        class Derived : public bsl::optional<int> {};
        class C {};

        BSLMF_ASSERT(( Optional_DerivedFromOptional<bsl::optional<int>>));
        BSLMF_ASSERT(( Optional_DerivedFromOptional<std::optional<int>>));
        BSLMF_ASSERT(( Optional_DerivedFromOptional<Derived>));
        BSLMF_ASSERT((!Optional_DerivedFromOptional<int>));
        BSLMF_ASSERT((!Optional_DerivedFromOptional<C>));
#endif
      } break;
      case 26: {
        //---------------------------------------------------------------------
        // bsl::optional<bslma::ManagedPtr<void>>
        //
        // Concern:
        //: 1 That 'bsl::optional<bslma::ManagedPtr<void>>' can be explicitly
        //:   instantiated on C++17 when implementation of 'bsl::optional' is
        //:   delegated to 'std::optional' for non-allocator-aware types (see
        //:   DRQS 168171178).  Note that we omit this test for clang with
        //:   libstdc++ where the explicit instantiation of
        //:   'std::optional<bslma::ManagedPtr<void>>' fails to compile.
        //
        // Plan:
        //: 1 Do 'template class ...' declarations at file scope and see if
        //:   they compile on C++17.
        //:
        //: 2 Use 'BSLMF_ASSERT' to examine type traits to verify that
        //:   'optional<bslma::ManagedPtr>' is neither copy-constructible nor
        //:   copy-assignable (due to the standard maintaining that the
        //:   corresponding functions having a const-reference argument).
        //
        // Testing:
        //   bsl::optional<bslma::ManagedPtr<void>>
        //---------------------------------------------------------------------

        if (verbose) printf("TEST bsl::optional<bslma::ManagedPtr<void>>\n"
                            "===========================================\n");

        if (veryVerbose) {
#if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU)
            printf("stdcpp_gnu\n");
#endif
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
            printf("stdcpp_llvm\n");
#endif
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) &&               \
   !(defined(BSLS_PLATFORM_CMP_CLANG) &&                                      \
     defined(BSLS_LIBRARYFEATURES_STDCPP_GNU))               &&               \
   !(defined(BSLS_PLATFORM_CMP_CLANG) &&                                      \
     defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM))
        typedef bsl::optional<bslma::ManagedPtr<void>> Obj;

        BSLMF_ASSERT(!bsl::is_copy_constructible<Obj>::value);
        BSLMF_ASSERT(!std::is_copy_assignable<Obj>::value);

        typedef std::optional<bslma::ManagedPtr<void>> SObj;

        BSLMF_ASSERT(!bsl::is_copy_constructible<SObj>::value);
        BSLMF_ASSERT(!std::is_copy_assignable<SObj>::value);
#endif
      } break;
      case 25: {
        //---------------------------------------------------------------------
        // IMPLICIT/EXPLICIT C'TORS TEST
        //
        // Concern:
        //: 1 That constructors that should be explicit are explicit, and those
        //:   that should be implicit are implicit.
        //:
        // Plan:
        //: 1 Declare a source type 'Src'.
        //:
        //: 2 Declare a destination type 'ImplicitDst', a non-allocating type
        //:   which can be implicitly constructed from 'Src'.
        //:
        //: 3 Declare a destination type 'ImplicitDstAlloc', an allocating type
        //:   which can be implicitly constructed from 'Src'.
        //:
        //: 4 Declare a destination type 'ExplicitDst', a non-allocating type
        //:   which can be explicitly constructed from 'Src'.
        //:
        //: 5 Declare a destination type 'ExplicitDstAlloc', an allocating type
        //:   which can be implicitly constructed from 'Src'.
        //:
        //: 6 Use 'bsl::is_convertible' to verify that 'optional<ImplicitDst>'
        //:   and 'optional<ImplicitDstAlloc>' are implicitly constructible and
        //:   move-constructible from 'Src', and that 'optional<ExplicitDst>'
        //:   and 'optional<ExplicitDstAlloc>' are not.
        //:
        //: 7 Use 'bsl::is_convertible' to verify that 'optional<ImplicitDst>'
        //:   and 'optional<ImplicitDstAlloc>' are implicitly constructible and
        //:   move-constructible from 'optional<Src>', and that
        //:   'optional<ExplicitDst>' and 'optional<ExplicitDstAlloc>' are not.
        //:
        //: 8 Use 'std::is_constructible' to verify that all of the above
        //:   conversions in '6' and '7' are either implicitly or explicitly
        //:   possible.
        //
        // Testing:
        //   IMPLICIT/EXPLICIT C'TORS TEST
        //---------------------------------------------------------------------

        if (verbose) printf("IMPLICIT/EXPLICIT C'TORS TEST\n"
                            "=============================\n");

        TEST_CASE_25::testCase25();
      } break;
      case 24: {
        RUN_EACH_TYPE(TestDriver,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::SimpleTestType,
                      bsltf::BitwiseCopyableTestType,
                      bsltf::BitwiseMoveableTestType,
                      bsltf::MovableTestType);
      } break;
      case 23: {
        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      bsltf::AllocTestType,
                      bsltf::AllocBitwiseMoveableTestType,
                      bsltf::MovableAllocTestType);
      } break;
      case 22: {
        //---------------------------------------------------------------------
        // REPRODUCE DRQS 168615744 bsl::optional<bdef_Function>
        //
        // Formerly, assignment got confused with 'TYPE' == 'bdef_Function'
        // because 'bdef_Function' was constructible and assignable from any
        // type, including 'bsl::optional<bdef_Function>'.  But when
        // non-callable types are actually assigned to 'bdef_Function', a
        // very complex compile failure occurs.
        //
        // Concern:
        //: 1 Copy-assignment is a better match than a perfect-forwarding
        //:   assignment when assigning 'optional<T>' to 'optional<T>' even
        //:   when 'T' is convertible / assignable from 'optional<T>'.
        //
        // Plan:
        //: 1 We don't have access to 'bdef_Function' from bslstl, so create a
        //:   type 'EasyConvert' that is
        //:   o default constructible.
        //:
        //:   o convertible / assignable from itself with no errors
        //:
        //:   o convertible / assignable from any other type, but which
        //:     generates compile errors if such a conversion or assignment is
        //:     attempted.
        //:
        //: 2 Create an instance 'a' of 'optional<EasyConvert>' containing a
        //:   default-constructed 'EasyConvert' object.
        //:
        //: 3 Copy construct an instance 'b' of the same type from 'a'.
        //:
        //: 4 Construct empty values 'c', 'd', and 'e'.
        //:
        //: 5 Assign between all possibilities of 'empty' and 'full' values.
        //:
        //: 6 Assign 'full' and 'empty' values from an 'EasyConvert' object.
        //
        // Testing:
        //   operator=(optional<bdef_Function>);
        //---------------------------------------------------------------------

        if (verbose) printf("Testing operator=(optional<bdef_Function>);\n"
                            "===========================================\n");

        const EasyConvert          ec;
        bsl::optional<EasyConvert> a(ec);
        bsl::optional<EasyConvert> b(a), c, d, e;

        a = b;    // full  <- full
        b = c;    // full  <- empty
        d = a;    // empty <- full
        c = e;    // empty <- empty

        ASSERT( a.has_value());
        ASSERT(!c.has_value());

        a = ec;    // full  <- TYPE
        c = ec;    // empty <- TYPE
      } break;
      case 21: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Simple one argument constructors deduce the template argument.
        //:
        //: 2 Constructing an 'optional' from 'allocator_arg_t', an allocator
        //:   and a value deduces the template argument.
        //
        // Plan:
        //: 1 Create an optional by invoking the constructor without supplying
        //:   the template argument explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        BSLA_MAYBE_UNUSED
        TestDeductionGuides test;
#endif
      } break;
      case 20: {
        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 19: {
        if (verbose)
            printf("\nTESTING DRQS 165776192"
                   "\n======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 18:
        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLSTL_OPTIONAL_TEST_NESTED_TYPES);
        break;
      case 17:
        RUN_EACH_TYPE(TestDriver, testCase17, MyClass2, MyClass2a);
        RUN_EACH_TYPE(TestDriver,
                      testCase17b,
                      ConstructTestTypeAlloc,
                      ConstructTestTypeAllocArgT);
        break;
      case 16:
        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        RUN_EACH_TYPE(TestDriver,
                      testCase16b,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);
        break;
      case 15:
        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR)
        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLSTL_OPTIONAL_TEST_NESTED_TYPES);
        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      bsl::allocator<char>);
        RUN_EACH_TYPE(TestDriver, testCase15b, MyClass2, MyClass2a);
        RUN_EACH_TYPE(TestDriver, testCase15b,
                      BSLSTL_OPTIONAL_TEST_NESTED_TYPE(MyClass2),
                      BSLSTL_OPTIONAL_TEST_NESTED_TYPE(MyClass2a));
        break;
      case 14:
        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
        break;
      case 13:
        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);
        break;
      case 12: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBSLX STREAMING"
                   "\n==============\n");

        if (verbose)
            printf("Not implemented.\n");

      } break;

      case 11:
      case 10:

        if (verbose)
            printf("\nTESTING 'operator=(nullopt_t)'"
                   "\n==============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase10a,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);


        if (verbose)
            printf("\nTESTING 'operator=(optional_type)'"
                   "\n==================================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase10b,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        if (verbose)
            printf("\nTESTING UDTS 'operator=(optional_type)'"
                   "\n==================================\n");
        testCase10b_udts();

        if (verbose)
            printf("\nTESTING 'operator=(non_optional_type)'"
                   "\n======================================"
                   "\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase10c,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        RUN_EACH_TYPE(TestDriver,
                      testCase10d,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        testCase10e();
        testCase10f();
        break;

      case 9:
        if (verbose)
            printf("\nTESTING 'swap' METHOD"
                   "\n=====================\n");
        testCase9();

        break;

      case 8:
      case 7:

        if (verbose)
            printf("\nTESTING COPY/MOVE CONSTRUCTION "
                   "\n==============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase7a,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        if (verbose)
            printf("\nTESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION"
                   "\n=================================================\n");
        RUN_EACH_TYPE(TestDriver, testCase7b, MyClass2, MyClass2a);

        if (verbose)
            printf("\nTESTING UDTS MOVE CONSTRUCTION"
                   "\n==============================\n");
        testCase7a_udts();

        break;
      case 6:
        if (verbose)
            printf("\nTESTING RELATIONAL OPERATORS"
                   "\n============================\n");
        testCase6();
        break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nPRINT AND OUTPUT OPERATOR"
                   "\n=========================\n");

        if (verbose)
            printf("Not implemented.\n");

      } break;
      case 4:
        if (verbose)
            printf("\nTESTING CONVERSION TO BOOL"
                   "\n==========================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase4a,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(TestDriver,
                      testCase4a,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        if (verbose)
            printf("\nTESTING 'value' METHOD"
                   "\n======================\n");
        RUN_EACH_TYPE(TestDriver, testCase4b, MyClass1, MyClass2);

        if (verbose)
            printf("\nTESTING 'value_or' METHOD"
                   "\n=========================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase4c,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        if (verbose)
            printf("\nTESTING ALLOCATOR EXTENDED 'value_or' METHOD"
                   "\n============================================\n");
        RUN_EACH_TYPE(TestDriver, testCase4d, MyClass2, MyClass2a);


        if (verbose)
            printf("\nTESTING 'operator->'"
                   "\n====================\n");
        RUN_EACH_TYPE(TestDriver, testCase4e, MyClass1, MyClass2, MyClass2a)

        if (verbose)
            printf("\nTESTING 'operator*'"
                   "\n===================\n");
        RUN_EACH_TYPE(TestDriver, testCase4f, MyClass1, MyClass2);

        break;

      case 3:
        if (verbose)
            printf("\nTESTING CONSTRUCTION FROM A VALUE"
                   "\n=================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3a,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        if (verbose)
            printf(
                "\nTESTING ALLOCATOR EXTENDED CONSTRUCTION FROM A VALUE"
                "\n===================================================="
                "\n");
        RUN_EACH_TYPE(TestDriver, testCase3b, MyClass2, MyClass2a);


        if (verbose)
            printf("\nTESTING 'in_place_t' CONSTRUCTOR "
                   "\n================================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase3c,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);


        if (verbose)
            printf("\nTESTING ALLOCATOR EXTENDED 'in_place_t' CONSTRUCTOR "
                   "\n===================================================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase3d,
                      ConstructTestTypeAlloc,
                      ConstructTestTypeAllocArgT);
        break;
      case 2:
        if (verbose)
            printf("\nDISENGAGED CONSTRUCTORS AND DESTRUCTOR"
                   "\n======================================"
                   "\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2a,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);


        if (verbose)
            printf("\nALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS"
                   "\n==========================================\n");

        RUN_EACH_TYPE(TestDriver, testCase2b, MyClass2, MyClass2a);


        if (verbose)
            printf("\nTESTING 'reset' MEMBER FUNCTION "
                   "\n===============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase2c,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        break;
      case 1:

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase1,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase1,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;

      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

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
