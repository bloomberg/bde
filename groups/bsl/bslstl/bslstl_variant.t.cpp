// bslstl_variant.t.cpp                                               -*-C++-*-
#include <bslstl_variant.h>
#include <bslstl_monostate.h>

#include <bslh_defaulthashalgorithm.h>
#include <bslh_hash.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bslstl_string.h>
#include <bslstl_utility.h>

#include <bslalg_constructorproxy.h>
#include <bslalg_swaputil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_typeidentity.h>

#include <bsltf_nondefaultconstructibletesttype.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>  // usage example
#include <utility>

using namespace BloombergLP;
using namespace bsl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC_GCC
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

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
// ----------------------------------------------------------------------------
// TYPEDEFS
// [ 2] typename bsl::allocator<char> allocator_type;
//
// TRAITS
// [ 2] BloombergLP::bslmf::IsBitwiseMoveable
// [ 2] BloombergLP::bslma::UsesBslmaAllocator
// [ 2] BloombergLP::bslmf::UsesAllocatorArgT
//
// CREATORS
// [ 4] variant();
// [ 9] variant(const variant&);
// [ 9] variant(variant&&);
// [ 9] explicit variant(t_STD_VARIANT&&);
// [ 5] variant(TYPE&&);
// [ 5] variant(in_place_type, ARGS&&...);
// [ 5] variant(in_place_type, std::initializer_list, ARGS&&...);
// [ 5] variant(in_place_index, ARGS&&...);
// [ 5] variant(in_place_index, init_list ARGS&&...);
// [ 4] variant(alloc_arg, alloc);
// [ 9] variant(alloc_arg, alloc, const variant&);
// [ 9] variant(alloc_arg, alloc, variant&&);
// [ 9] explicit variant(alloc_arg, alloc, t_STD_VARIANT&&);
// [ 5] variant(alloc_arg, alloc, TYPE&&);
// [ 5] variant(alloc_arg, alloc, in_place_type, ARGS&&...);
// [ 5] variant(alloc_arg, alloc, in_place_type, init_list, ARGS&&...);
// [ 5] variant(alloc_arg, alloc, in_place_index, ARGS&&...);
// [ 5] variant(alloc_arg, alloc, in_place_index, init_list, ARGS&&...);
// [ 4] ~variant();
//
// MANIPULATORS
// [ 7] TYPE& emplace<TYPE>(Args&&...);
// [ 7] TYPE& emplace<TYPE>(initializer_list<U>, Args&&...);
// [ 7] TYPE& emplace<INDEX>(Args&&...);
// [ 7] TYPE& emplace<INDEX>(initializer_list<U>, Args&&...);
// [11] variant& operator=(const variant&);
// [11] variant& operator=(variant&&);
// [11] variant& operator=(T&&);
// [10] void swap(variant&);
//
// ACCESSORS
// [ 4] allocator_type get_allocator() const;
// [ 6] bool valueless_by_exception() const;
// [ 6] size_t index() const;
//
// FREE OPERATORS
// [ 6] TYPE& get<INDEX>(variant&);
// [ 6] const TYPE& get<INDEX>(const variant&);
// [ 6] TYPE&& get<INDEX>(variant&&);
// [ 6] const TYPE&& get<INDEX>(const variant&&);
// [ 6] TYPE& get<TYPE>(variant&);
// [ 6] const TYPE& get<TYPE>(const variant&);
// [ 6] TYPE&& get<TYPE>(variant&&);
// [ 6] const TYPE&& get<TYPE>(const variant&&);
// [ 6] TYPE* get_if<INDEX>(variant *);
// [ 6] const TYPE* get_if<INDEX>(const variant *);
// [ 6] TYPE*  get_if<TYPE>(variant *);
// [ 6] const TYPE* get_if<TYPE>(const variant *);
// [13] void hashAppend(HASHALG& hashAlg, const variant<TYPES...>& input);
// [ 6] bool holds_alternative(const variant&);
// [ 8] bool operator==(const variant<Types...>&, const variant<Types...>&);
// [ 8] bool operator!=(const variant<Types...>&, const variant<Types...>&);
// [ 8] bool operator<(const variant<Types...>&, const variant<Types...>&);
// [ 8] bool operator>(const variant<Types...>&, const variant<Types...>&);
// [ 8] bool operator<=(const variant<Types...>&, const variant<Types...>&);
// [ 8] bool operator>=(const variant<Types...>&, const variant<Types...>&);
// [10] void swap(variant<Types...>&, variant<Types...>&);
// [12] RET visit<RET, VISITOR, VARIANT>(VISITOR&&, VARIANT&&);
// [12] RET visitR<RET, VISITOR, VARIANT>(VISITOR&, VARIANT&);
// [12] visit<VISITOR, VARIANT>(VISITOR&&, VARIANT&&);
//
// HELPER METAFUNCTIONS
// [ 3] struct variant_size<VARIANT>;
// [ 3] size_t variant_size_v<VARIANT>;
// [ 3] struct variant_alternative<I, VARIANT>;
// [ 3] typedef variant_alternative_t<INDEX,VARIANT>;
// [ 3] size_t variant_npos;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [16] CONCERN: SFINAE for `get` works on Solaris (DRQS 175366735)

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

namespace {
enum { k_DESTROYED = 0x05c };

}  // close unnamed namespace

// ============================================================================
//               ADDITIONAL MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) &&                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
#define U_VARIANT_FULL_IMPLEMENTATION
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

struct MyClass1;
struct MyClass1a;
struct MyClass2;
struct MyClass2a;

struct ConstructTestTypeNoAlloc;
struct ConstructTestTypeAlloc;
struct ConstructTestTypeAllocArgT;

struct Swappable;
struct SwappableAA;

struct Throws;

template <class TYPE>
struct isSwapTestType : bsl::false_type {
};
template <>
struct isSwapTestType<Swappable> : bsl::true_type {
};
template <>
struct isSwapTestType<SwappableAA> : bsl::true_type {
};

typedef bsl::variant<int>                      Variant_int;
typedef bsl::variant<int, MyClass1>            Variant_int_MyClass1;
typedef bsl::variant<int, MyClass1, MyClass1a> Variant_int_MyClass1_MyClass1a;
typedef bsl::variant<int, MyClass2, MyClass1>  Variant_int_MyClass2_MyClass1;
typedef bsl::variant<int, MyClass2a>           Variant_int_MyClass2a;
typedef bsl::variant<MyClass2a>                Variant_MyClass2a;

typedef bsl::variant<MyClass1>       Variant_MyClass1;
typedef bsl::variant<MyClass1, int>  Variant_MyClass1_int;
typedef bsl::variant<MyClass1a>      Variant_MyClass1a;
typedef bsl::variant<MyClass1a, int> Variant_MyClass1a_int;
typedef bsl::variant<MyClass2>       Variant_MyClass2;
typedef bsl::variant<MyClass2, int>  Variant_MyClass2_int;
typedef bsl::variant<MyClass2a>      Variant_MyClass2a;
typedef bsl::variant<MyClass2a, int> Variant_MyClass2a_int;

typedef bsl::variant<MyClass1, MyClass1a, MyClass2, MyClass2a>
    Variant_MyClass1_MyClass1a_MyClass2_MyClass2a;
typedef bsl::variant<MyClass2, MyClass2a> Variant_MyClass2_MyClass2a;

typedef bsl::variant<ConstructTestTypeNoAlloc> Variant_CTTNoAlloc;
typedef bsl::variant<ConstructTestTypeAlloc>   Variant_CTTAlloc;
typedef bsl::variant<ConstructTestTypeNoAlloc,
                     ConstructTestTypeAlloc,
                     ConstructTestTypeAllocArgT>
    Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT;

//SWAP
typedef bsl::variant<Swappable>            Variant_Swappable;
typedef bsl::variant<SwappableAA>          Variant_SwappableAA;
typedef bsl::variant<int, char, Swappable> Variant_int_char_SwappableAA;
typedef bsl::variant<Swappable, SwappableAA, int, char>
    Variant_Swappable_SwappableAA_int_char;

//HASH
typedef bsl::variant<size_t>                   Variant_size_t;
typedef bsl::variant<size_t, char>             Variant_size_t_char;
typedef bsl::variant<size_t, signed char, int> Variant_size_t_signed_char_int;

//IDENTICAL ALTERNATIVES
typedef bsl::variant<int, int>           Variant_int_int;
typedef bsl::variant<MyClass1, MyClass1> Variant_MyClass1_MyClass1;
typedef bsl::variant<MyClass2, MyClass2, MyClass2a>
    Variant_MyClass2_MyClass2_MyClass2a;
typedef bsl::variant<ConstructTestTypeNoAlloc, ConstructTestTypeNoAlloc>
    Variant_CTTNoAlloc_CTTNoAlloc;
typedef bsl::variant<ConstructTestTypeAlloc, ConstructTestTypeAlloc>
    Variant_CTTAlloc_CTTAlloc;
typedef bsl::variant<ConstructTestTypeNoAlloc,
                     ConstructTestTypeAlloc,
                     ConstructTestTypeAllocArgT,
                     ConstructTestTypeAllocArgT>
    Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT_CTTAllocArgT;
typedef bsl::variant<int, Swappable, Swappable>
    Variant_int_Swappable_Swappable;
typedef bsl::variant<int, SwappableAA, SwappableAA>
    Variant_int_SwappableAA_SwappableAA;

//CV IDENTICAL ALTERNATIVES
typedef bsl::variant<MyClass1, const MyClass1> Variant_MyClass1_const_MyClass1;
typedef bsl::variant<const MyClass2, MyClass2, MyClass2a>
    Variant_const_MyClass2_MyClass2_MyClass2a;
typedef bsl::variant<ConstructTestTypeNoAlloc, const ConstructTestTypeNoAlloc>
    Variant_CTTNoAlloc_const_CTTNoAlloc;
typedef bsl::variant<const ConstructTestTypeAlloc, ConstructTestTypeAlloc>
    Variant_const_CTTAlloc_CTTAlloc;
typedef bsl::variant<ConstructTestTypeNoAlloc,
                     ConstructTestTypeAlloc,
                     ConstructTestTypeAllocArgT,
                     const ConstructTestTypeAllocArgT>
    Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT_const_CTTAllocArgT;

//CV qualified ALTERNATIVES
typedef bsl::variant<const MyClass1> Variant_const_MyClass1;
typedef bsl::variant<const MyClass2, MyClass2a>
    Variant_const_MyClass2_MyClass2a;

#define BSLSTL_VARIANT_TEST_TYPES                                             \
    Variant_int, Variant_int_MyClass1, Variant_int_MyClass1_MyClass1a,        \
        Variant_int_MyClass2_MyClass1, Variant_int_MyClass2a,                 \
        Variant_MyClass2a, Variant_int_int

#define BSLSTL_VARIANT_TEST_TYPES_NONUNIQUE                                   \
    Variant_MyClass1_MyClass1, Variant_MyClass2_MyClass2_MyClass2a,           \
        Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT_CTTAllocArgT

#define BSLSTL_VARIANT_TEST_TYPES_CVNONUNIQUE                                 \
    Variant_MyClass1_const_MyClass1,                                          \
        Variant_const_MyClass2_MyClass2_MyClass2a,                            \
        Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT_const_CTTAllocArgT

#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_CONSTRUCTOR                         \
    Variant_MyClass1, Variant_MyClass1_int, Variant_MyClass1a,                \
        Variant_MyClass1a_int,                                                \
        BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR

#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_CONSTRUCTOR_NONUNIQUE               \
    Variant_MyClass1_MyClass1,                                                \
        BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR_NONUNIQUE

#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_CONSTRUCTOR_CVNONUNIQUE             \
    Variant_MyClass1_const_MyClass1,                                          \
        BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR_CVNONUNIQUE

#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR                      \
    Variant_MyClass2, Variant_MyClass2_int, Variant_MyClass2a,                \
        Variant_MyClass2a_int

#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR_NONUNIQUE            \
    Variant_MyClass2_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR_CVNONUNIQUE          \
    Variant_const_MyClass2_MyClass2_MyClass2a

typedef bsl::variant<Throws, MyClass2> VARIANT_TYPE_TC1;
#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_THROW_AE_CONSTRUCTOR VARIANT_TYPE_TC1

typedef bsl::variant<Throws>         VARIANT_TYPE_TC2;
typedef bsl::variant<Throws, size_t> VARIANT_TYPE_TC3;
#define BSLSTL_VARIANT_TEST_TYPES_DEFAULT_THROW_CONSTRUCTOR                   \
    VARIANT_TYPE_TC2, VARIANT_TYPE_TC3,                                       \
        BSLSTL_VARIANT_TEST_TYPES_DEFAULT_THROW_AE_CONSTRUCTOR

#define BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING                           \
    Variant_MyClass1, Variant_MyClass1a,                                      \
        Variant_MyClass1_MyClass1a_MyClass2_MyClass2a, Variant_MyClass2,      \
        Variant_MyClass2a, Variant_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_NONUNIQUE                 \
    Variant_MyClass1_MyClass1, Variant_MyClass2_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE               \
    Variant_MyClass1_const_MyClass1, Variant_const_MyClass2_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVQUALIFIED               \
    Variant_const_MyClass1, Variant_const_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING                        \
    Variant_MyClass2, Variant_MyClass2a,                                      \
        Variant_MyClass1_MyClass1a_MyClass2_MyClass2a,                        \
        Variant_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_NONUNIQUE              \
    Variant_MyClass2_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_CVNONUNIQUE            \
    Variant_const_MyClass2_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_CVQUALIFIED            \
    Variant_const_MyClass2_MyClass2a

#define BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS                               \
    Variant_CTTNoAlloc, BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS

#define BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_NONUNIQUE                     \
    Variant_CTTNoAlloc_CTTNoAlloc,                                            \
        BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_NONUNIQUE

#define BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_CVNONUNIQUE                   \
    Variant_CTTNoAlloc_const_CTTNoAlloc,                                      \
        BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_CVNONUNIQUE

#define BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS                            \
    Variant_CTTAlloc, Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT

#define BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_NONUNIQUE                  \
    Variant_CTTAlloc_CTTAlloc,                                                \
        Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT_CTTAllocArgT

#define BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_CVNONUNIQUE                \
    Variant_const_CTTAlloc_CTTAlloc,                                          \
        Variant_CTTNoAlloc_CTTAlloc_CTTAllocArgT_const_CTTAllocArgT

#define BSLSTL_VARIANT_TEST_TYPES_SWAP                                        \
    Variant_Swappable, Variant_SwappableAA, Variant_int_char_SwappableAA,     \
        Variant_Swappable_SwappableAA_int_char

#define BSLSTL_VARIANT_TEST_TYPES_SWAP_NONUNIQUE                              \
    Variant_int_Swappable_Swappable, Variant_int_SwappableAA_SwappableAA

#define BSLSTL_VARIANT_TEST_TYPES_HASH                                        \
    Variant_size_t, Variant_size_t_char, Variant_size_t_signed_char_int

#ifdef BDE_BUILD_TARGET_EXC
#define EXC_ASSERT(X) ASSERT(X)
#else
#define EXC_ASSERT(X)
#endif

#define TEST_COPY_CONSTRUCT(source)                                           \
    {                                                                         \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(get<INDEX>(source));                                   \
        expCopy           = AltType::s_copyConstructorInvocations - expCopy;  \
        expMove           = AltType::s_moveConstructorInvocations - expMove;  \
        int       numCopy = AltType::s_copyConstructorInvocations;            \
        int       numMove = AltType::s_moveConstructorInvocations;            \
        DEST_TYPE obj(source);                                                \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERTV(obj.index(), INDEX, obj.index() == INDEX);                    \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &da));                         \
    }
#define TEST_MOVE_CONSTRUCT(source, propagate)                                \
    {                                                                         \
        SRC_TYPE              srcCopy(source);                                \
        bslma::TestAllocator& expAlloc = (propagate ? oa : da);               \
                                                                              \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(MoveUtil::move(get<INDEX>(srcCopy)));                  \
                                                                              \
        expCopy = AltType::s_copyConstructorInvocations - expCopy;            \
        expMove = AltType::s_moveConstructorInvocations - expMove;            \
        int numCopy = AltType::s_copyConstructorInvocations;                  \
        int numMove = AltType::s_moveConstructorInvocations;                  \
        ASSERT(checkAllocator(source, &oa));                                  \
        ASSERT(checkAllocator(get<INDEX>(source), &oa));                      \
        DEST_TYPE obj(MoveUtil::move(source));                                \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERTV(obj.index(), INDEX, obj.index() == INDEX);                    \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &expAlloc));                               \
        ASSERT(checkAllocator(get<INDEX>(obj), &expAlloc));                   \
    }

#define TEST_EXT_COPY_CONSTRUCT(source)                                       \
    {                                                                         \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(get<INDEX>(source));                                   \
        expCopy           = AltType::s_copyConstructorInvocations - expCopy;  \
        expMove           = AltType::s_moveConstructorInvocations - expMove;  \
        int       numCopy = AltType::s_copyConstructorInvocations;            \
        int       numMove = AltType::s_moveConstructorInvocations;            \
        DEST_TYPE obj(bsl::allocator_arg, &ta, source);                       \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &ta));                         \
    }
#define TEST_EXT_MOVE_CONSTRUCT(source)                                       \
    {                                                                         \
        SRC_TYPE srcCopy(source);                                             \
        int      expCopy = AltType::s_copyConstructorInvocations;             \
        int      expMove = AltType::s_moveConstructorInvocations;             \
        AltType  expVal(MoveUtil::move(get<INDEX>(srcCopy)));                 \
        expCopy           = AltType::s_copyConstructorInvocations - expCopy;  \
        expMove           = AltType::s_moveConstructorInvocations - expMove;  \
        int       numCopy = AltType::s_copyConstructorInvocations;            \
        int       numMove = AltType::s_moveConstructorInvocations;            \
        DEST_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));       \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &ta));                         \
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#define TEST_CONSTRUCT_FROM_STD_LVALUE(source)                                \
    {                                                                         \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(std::get<INDEX>(source));                              \
        expCopy           = AltType::s_copyConstructorInvocations - expCopy;  \
        expMove           = AltType::s_moveConstructorInvocations - expMove;  \
        int       numCopy = AltType::s_copyConstructorInvocations;            \
        int       numMove = AltType::s_moveConstructorInvocations;            \
        DEST_TYPE obj(source);                                                \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERTV(obj.index(), INDEX, obj.index() == INDEX);                    \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &da));                         \
    }

#define TEST_CONSTRUCT_FROM_STD_RVALUE(source)                                \
    {                                                                         \
        SRC_TYPE srcCopy(source);                                             \
                                                                              \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(std::move(std::get<INDEX>(srcCopy)));                  \
                                                                              \
        expCopy = AltType::s_copyConstructorInvocations - expCopy;            \
        expMove = AltType::s_moveConstructorInvocations - expMove;            \
        int numCopy = AltType::s_copyConstructorInvocations;                  \
        int numMove = AltType::s_moveConstructorInvocations;                  \
        ASSERT(checkAllocator(std::get<INDEX>(source), &oa));                 \
        DEST_TYPE obj(std::move(source));                                     \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERTV(obj.index(), INDEX, obj.index() == INDEX);                    \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &da));                         \
    }

#define TEST_EXT_CONSTRUCT_FROM_STD_LVALUE(source)                            \
    {                                                                         \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(std::get<INDEX>(source));                              \
        expCopy           = AltType::s_copyConstructorInvocations - expCopy;  \
        expMove           = AltType::s_moveConstructorInvocations - expMove;  \
        int       numCopy = AltType::s_copyConstructorInvocations;            \
        int       numMove = AltType::s_moveConstructorInvocations;            \
        DEST_TYPE obj(bsl::allocator_arg, &ta, source);                       \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &ta));                         \
    }

#define TEST_EXT_CONSTRUCT_FROM_STD_RVALUE(source)                            \
    {                                                                         \
        SRC_TYPE srcCopy(source);                                             \
        int      expCopy = AltType::s_copyConstructorInvocations;             \
        int      expMove = AltType::s_moveConstructorInvocations;             \
        AltType  expVal(std::move(std::get<INDEX>(srcCopy)));                 \
        expCopy           = AltType::s_copyConstructorInvocations - expCopy;  \
        expMove           = AltType::s_moveConstructorInvocations - expMove;  \
        int       numCopy = AltType::s_copyConstructorInvocations;            \
        int       numMove = AltType::s_moveConstructorInvocations;            \
        DEST_TYPE obj(bsl::allocator_arg, &ta, std::move(source));            \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(get<INDEX>(obj), &ta));                         \
    }

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#define TEST_COPY_FROM_VALUE(source)                                          \
    {                                                                         \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(source);                                               \
        expCopy         = AltType::s_copyConstructorInvocations - expCopy;    \
        expMove         = AltType::s_moveConstructorInvocations - expMove;    \
        int     numCopy = AltType::s_copyConstructorInvocations;              \
        int     numMove = AltType::s_moveConstructorInvocations;              \
        VARIANT obj(source);                                                  \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(bsl::get<INDEX>(obj), &da));                    \
    }
#define TEST_MOVE_FROM_VALUE(source)                                          \
    {                                                                         \
        AltType srcCopy(source);                                              \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(MoveUtil::move(srcCopy));                              \
        expCopy         = AltType::s_copyConstructorInvocations - expCopy;    \
        expMove         = AltType::s_moveConstructorInvocations - expMove;    \
        int     numCopy = AltType::s_copyConstructorInvocations;              \
        int     numMove = AltType::s_moveConstructorInvocations;              \
        VARIANT obj(MoveUtil::move(source));                                  \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(bsl::get<INDEX>(obj) == expVal);                               \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(bsl::get<INDEX>(obj), &da));                    \
    }

#define TEST_AE_COPY_FROM_VALUE(source)                                       \
    {                                                                         \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(source);                                               \
        expCopy         = AltType::s_copyConstructorInvocations - expCopy;    \
        expMove         = AltType::s_moveConstructorInvocations - expMove;    \
        int     numCopy = AltType::s_copyConstructorInvocations;              \
        int     numMove = AltType::s_moveConstructorInvocations;              \
        VARIANT obj(bsl::allocator_arg, &oa, source);                         \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(get<INDEX>(obj) == expVal);                                    \
        ASSERT(checkAllocator(obj, &oa));                                     \
        ASSERT(checkAllocator(bsl::get<INDEX>(obj), &oa));                    \
    }
#define TEST_AE_MOVE_FROM_VALUE(source)                                       \
    {                                                                         \
        AltType srcCopy(source);                                              \
        int     expCopy = AltType::s_copyConstructorInvocations;              \
        int     expMove = AltType::s_moveConstructorInvocations;              \
        AltType expVal(MoveUtil::move(srcCopy));                              \
        expCopy         = AltType::s_copyConstructorInvocations - expCopy;    \
        expMove         = AltType::s_moveConstructorInvocations - expMove;    \
        int     numCopy = AltType::s_copyConstructorInvocations;              \
        int     numMove = AltType::s_moveConstructorInvocations;              \
        VARIANT obj(bsl::allocator_arg, &oa, MoveUtil::move(source));         \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(bsl::get<INDEX>(obj) == expVal);                               \
        ASSERT(checkAllocator(obj, &oa));                                     \
        ASSERT(checkAllocator(bsl::get<INDEX>(obj), &oa));                    \
    }

#define TEST_TAGGED_CONSTRUCT(init, expArgs, expAlloc)                        \
    {                                                                         \
        int              expCopy = AltType::s_copyConstructorInvocations;     \
        int              expMove = AltType::s_moveConstructorInvocations;     \
        ValWithAllocator valBuffer expArgs;                                   \
        AltType&                   expVal = valBuffer.object();               \
        expCopy         = AltType::s_copyConstructorInvocations - expCopy;    \
        expMove         = AltType::s_moveConstructorInvocations - expMove;    \
        int     numCopy = AltType::s_copyConstructorInvocations;              \
        int     numMove = AltType::s_moveConstructorInvocations;              \
        VARIANT obj     = VARIANT init;                                       \
        numCopy         = AltType::s_copyConstructorInvocations - numCopy;    \
        numMove         = AltType::s_moveConstructorInvocations - numMove;    \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        ASSERT(bsl::get<INDEX>(obj) == expVal);                               \
        ASSERT(checkAllocator(obj, expAlloc));                                \
        ASSERT(checkAllocator(bsl::get<INDEX>(obj), expAlloc));               \
    }

#define TEST_EMPLACE(TAG, expArgs, emplaceArgs)                               \
    {                                                                         \
        int              expCopy = AltType::s_copyConstructorInvocations;     \
        int              expMove = AltType::s_moveConstructorInvocations;     \
        ValWithAllocator valBuffer expArgs;                                   \
        AltType&          expVal = valBuffer.object();                        \
        expCopy = AltType::s_copyConstructorInvocations - expCopy;            \
        expMove = AltType::s_moveConstructorInvocations - expMove;            \
                                                                              \
        VariantWithAllocator objBuffer(&oa);                                  \
        VARIANT&             obj = objBuffer.object();                        \
                                                                              \
        int      numCopy = AltType::s_copyConstructorInvocations;             \
        int      numMove = AltType::s_moveConstructorInvocations;             \
        int      expDest = FirstType::s_destructorInvocations + 1;            \
        AltType& retVal = obj.template emplace<TAG> emplaceArgs;              \
        numCopy = AltType::s_copyConstructorInvocations - numCopy;            \
        numMove = AltType::s_moveConstructorInvocations - numMove;            \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(expDest == FirstType::s_destructorInvocations);                \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == INDEX);                                         \
        AltType& ObjAlt = bsl::get<INDEX>(obj);                               \
        ASSERT(bsl::get<INDEX>(obj) == expVal);                               \
        ASSERT(checkAllocator(ObjAlt, &oa));                                  \
        ASSERT(checkAllocator(ObjAlt, &oa));                                  \
        ASSERT(createdAlike(expVal, ObjAlt) == true);                         \
        ASSERT(BSLS_UTIL_ADDRESSOF(ObjAlt) == BSLS_UTIL_ADDRESSOF(retVal));   \
    }

#define TEST_EMPLACE_THROWS(emplaceArgs)                                      \
    {                                                                         \
        bsl::variant<TYPE, Throws> obj;                                       \
        Throws::s_should_throw = true;                                        \
        int expDest            = TYPE::s_destructorInvocations + 1;           \
        BSLS_TRY { obj.template emplace<Throws> emplaceArgs; }                \
        BSLS_CATCH(...) {}                                                    \
        ASSERT(expDest == TYPE::s_destructorInvocations);                     \
        ASSERT(obj.valueless_by_exception());                                 \
        ASSERT(obj.index() == bsl::variant_npos);                             \
    }                                                                         \
    {                                                                         \
        bsl::variant<TYPE, Throws> obj;                                       \
        Throws::s_should_throw = true;                                        \
        int expDest            = TYPE::s_destructorInvocations + 1;           \
        BSLS_TRY { obj.template emplace<1> emplaceArgs; }                     \
        BSLS_CATCH(...) {}                                                    \
        ASSERT(expDest == TYPE::s_destructorInvocations);                     \
        ASSERT(obj.valueless_by_exception());                                 \
        ASSERT(obj.index() == bsl::variant_npos);                             \
    }

#define TEST_ASSIGN_FROM_VARIANT_SAME_INDEX(source)                           \
    {                                                                         \
        SrcAltType expVal;                                                    \
        int        expCopy       = SrcAltType::s_copyConstructorInvocations;  \
        int        expMove       = SrcAltType::s_moveConstructorInvocations;  \
        int        expCopyAssign = SrcAltType::s_copyAssignmentInvocations;   \
        int        expMoveAssign = SrcAltType::s_moveAssignmentInvocations;   \
        expVal                   = bsl::get<SRC_INDEX>(source);               \
        expCopy = SrcAltType::s_copyConstructorInvocations - expCopy;         \
        expMove = SrcAltType::s_moveConstructorInvocations - expMove;         \
        expCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - expCopyAssign;          \
        expMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - expMoveAssign;          \
        VariantWithAllocator objBuffer(bsl::in_place_index_t<DEST_INDEX>(),   \
                                       &oa);                                  \
        VARIANT&             obj = objBuffer.object();                        \
        int numCopy              = SrcAltType::s_copyConstructorInvocations;  \
        int numMove              = SrcAltType::s_moveConstructorInvocations;  \
        int numCopyAssign        = SrcAltType::s_copyAssignmentInvocations;   \
        int numMoveAssign        = SrcAltType::s_moveAssignmentInvocations;   \
        int numDest              = DestAltType::s_destructorInvocations;      \
        obj                      = source;                                    \
        numCopy = SrcAltType::s_copyConstructorInvocations - numCopy;         \
        numMove = SrcAltType::s_moveConstructorInvocations - numMove;         \
        numCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - numCopyAssign;          \
        numMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - numMoveAssign;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(expCopyAssign == numCopyAssign);                               \
        ASSERT(expMoveAssign == numMoveAssign);                               \
        ASSERT(numDest == DestAltType::s_destructorInvocations);              \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == SRC_INDEX);                                     \
        SrcAltType& ObjAlt = bsl::get<SRC_INDEX>(obj);                        \
        ASSERT(ObjAlt == expVal);                                             \
        ASSERT(checkAllocator(ObjAlt, &oa));                                  \
    }
#define TEST_ASSIGN_FROM_VARIANT_DIFF_INDEX(source)                           \
    {                                                                         \
        int        expCopy       = SrcAltType::s_copyConstructorInvocations;  \
        int        expMove       = SrcAltType::s_moveConstructorInvocations;  \
        int        expCopyAssign = SrcAltType::s_copyAssignmentInvocations;   \
        int        expMoveAssign = SrcAltType::s_moveAssignmentInvocations;   \
        SrcAltType expVal(bsl::get<SRC_INDEX>(source));                       \
        expCopy = SrcAltType::s_copyConstructorInvocations - expCopy;         \
        expMove = SrcAltType::s_moveConstructorInvocations - expMove;         \
        expCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - expCopyAssign;          \
        expMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - expMoveAssign;          \
        VariantWithAllocator objBuffer(bsl::in_place_index_t<DEST_INDEX>(),   \
                                       &oa);                                  \
        VARIANT&             obj = objBuffer.object();                        \
        int numCopy              = SrcAltType::s_copyConstructorInvocations;  \
        int numMove              = SrcAltType::s_moveConstructorInvocations;  \
        int numCopyAssign        = SrcAltType::s_copyAssignmentInvocations;   \
        int numMoveAssign        = SrcAltType::s_moveAssignmentInvocations;   \
        int numDest              = DestAltType::s_destructorInvocations;      \
        obj                      = source;                                    \
        numCopy = SrcAltType::s_copyConstructorInvocations - numCopy;         \
        numMove = SrcAltType::s_moveConstructorInvocations - numMove;         \
        numCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - numCopyAssign;          \
        numMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - numMoveAssign;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(expCopyAssign == numCopyAssign);                               \
        ASSERT(expMoveAssign == numMoveAssign);                               \
        ASSERT(numDest == DestAltType::s_destructorInvocations - 1);          \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == SRC_INDEX);                                     \
        SrcAltType& ObjAlt = bsl::get<SRC_INDEX>(obj);                        \
        ASSERT(ObjAlt == expVal);                                             \
        ASSERT(checkAllocator(ObjAlt, &oa));                                  \
    }

#define TEST_ASSIGN_FROM_ALT_SAME_INDEX(source)                               \
    {                                                                         \
        SrcAltType expVal;                                                    \
        int        expCopy       = SrcAltType::s_copyConstructorInvocations;  \
        int        expMove       = SrcAltType::s_moveConstructorInvocations;  \
        int        expCopyAssign = SrcAltType::s_copyAssignmentInvocations;   \
        int        expMoveAssign = SrcAltType::s_moveAssignmentInvocations;   \
        expVal                   = source;                                    \
        expCopy = SrcAltType::s_copyConstructorInvocations - expCopy;         \
        expMove = SrcAltType::s_moveConstructorInvocations - expMove;         \
        expCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - expCopyAssign;          \
        expMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - expMoveAssign;          \
        VariantWithAllocator objBuffer(bsl::in_place_index_t<DEST_INDEX>(),   \
                                       &oa);                                  \
        VARIANT&             obj = objBuffer.object();                        \
        int numCopy              = SrcAltType::s_copyConstructorInvocations;  \
        int numMove              = SrcAltType::s_moveConstructorInvocations;  \
        int numCopyAssign        = SrcAltType::s_copyAssignmentInvocations;   \
        int numMoveAssign        = SrcAltType::s_moveAssignmentInvocations;   \
        int numDest              = DestAltType::s_destructorInvocations;      \
        obj                      = source;                                    \
        numCopy = SrcAltType::s_copyConstructorInvocations - numCopy;         \
        numMove = SrcAltType::s_moveConstructorInvocations - numMove;         \
        numCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - numCopyAssign;          \
        numMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - numMoveAssign;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(expCopyAssign == numCopyAssign);                               \
        ASSERT(expMoveAssign == numMoveAssign);                               \
        ASSERT(numDest == DestAltType::s_destructorInvocations);              \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == SRC_INDEX);                                     \
        SrcAltType& ObjAlt = bsl::get<SRC_INDEX>(obj);                        \
        ASSERT(ObjAlt == expVal);                                             \
        ASSERT(checkAllocator(ObjAlt, &oa));                                  \
    }
#define TEST_ASSIGN_FROM_ALT_DIFF_INDEX(source)                               \
    {                                                                         \
        int        expCopy       = SrcAltType::s_copyConstructorInvocations;  \
        int        expMove       = SrcAltType::s_moveConstructorInvocations;  \
        int        expCopyAssign = SrcAltType::s_copyAssignmentInvocations;   \
        int        expMoveAssign = SrcAltType::s_moveAssignmentInvocations;   \
        SrcAltType expVal(source);                                            \
        expCopy = SrcAltType::s_copyConstructorInvocations - expCopy;         \
        expMove = SrcAltType::s_moveConstructorInvocations - expMove;         \
        expCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - expCopyAssign;          \
        expMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - expMoveAssign;          \
        VariantWithAllocator objBuffer(bsl::in_place_index_t<DEST_INDEX>(),   \
                                       &oa);                                  \
        VARIANT&             obj = objBuffer.object();                        \
        int numCopy              = SrcAltType::s_copyConstructorInvocations;  \
        int numMove              = SrcAltType::s_moveConstructorInvocations;  \
        int numCopyAssign        = SrcAltType::s_copyAssignmentInvocations;   \
        int numMoveAssign        = SrcAltType::s_moveAssignmentInvocations;   \
        int numDest              = DestAltType::s_destructorInvocations;      \
        obj                      = source;                                    \
        numCopy = SrcAltType::s_copyConstructorInvocations - numCopy;         \
        numMove = SrcAltType::s_moveConstructorInvocations - numMove;         \
        numCopyAssign =                                                       \
            SrcAltType::s_copyAssignmentInvocations - numCopyAssign;          \
        numMoveAssign =                                                       \
            SrcAltType::s_moveAssignmentInvocations - numMoveAssign;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(expCopyAssign == numCopyAssign);                               \
        ASSERT(expMoveAssign == numMoveAssign);                               \
        ASSERT(numDest == DestAltType::s_destructorInvocations - 1);          \
        ASSERT(!obj.valueless_by_exception());                                \
        ASSERT(obj.index() == SRC_INDEX);                                     \
        SrcAltType& ObjAlt = bsl::get<SRC_INDEX>(obj);                        \
        ASSERT(ObjAlt == expVal);                                             \
        ASSERT(checkAllocator(ObjAlt, &oa));                                  \
    }

#define TEST_GET_NON_ACTIVE_ALT(tag, obj)                                     \
    {                                                                         \
        bool bad_variant_access_exception_caught = false;                     \
        BSLS_TRY { bsl::get<tag>(obj); }                                      \
        BSLS_CATCH(const bsl::bad_variant_access&)                            \
        {                                                                     \
            bad_variant_access_exception_caught = true;                       \
        }                                                                     \
        EXC_ASSERT(bad_variant_access_exception_caught);                      \
    }

#define TEST_GET_THROWS_ALT(tag, obj)                                         \
    {                                                                         \
        bool bad_variant_access_thrown = false;                               \
        BSLS_TRY { get<tag>(obj); }                                           \
        BSLS_CATCH(const bsl::bad_variant_access&)                            \
        {                                                                     \
            bad_variant_access_thrown = true;                                 \
        }                                                                     \
        EXC_ASSERT(bad_variant_access_thrown);                                \
    }

#define RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(testNo)                    \
    template <class VARIANT, size_t N, size_t M>                              \
    struct TestCase##testNo##_imp;                                            \
                                                                              \
    template <class VARIANT, bool USES_BSLMA_ALLOC>                           \
    void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase##testNo()            \
    {                                                                         \
        TestCase##testNo##_imp<VARIANT,                                       \
                               bsl::variant_size<VARIANT>::value,             \
                               bsl::variant_size<VARIANT>::value>             \
            test##testNo;                                                     \
    }                                                                         \
    template <class VARIANT>                                                  \
    struct TestCase##testNo##_imp<VARIANT, 1, 0> {                            \
    };                                                                        \
    template <class VARIANT, size_t N>                                        \
    struct TestCase##testNo##_imp<VARIANT, N, 0>                              \
    : TestCase##testNo##_imp<VARIANT,                                         \
                             N - 1,                                           \
                             bsl::variant_size<VARIANT>::value> {             \
    };                                                                        \
    template <class VARIANT, size_t N, size_t M>                              \
    struct TestCase##testNo##_imp                                             \
    : TestCase##testNo##_imp<VARIANT, N, M - 1> {                             \
        TestCase##testNo##_imp()

#define RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END                              \
    }                                                                         \
    ;

                              // ================
                              // class MyClassDef
                              // ================

/// Data members that give MyClassX size and alignment.  This class is a
/// simple aggregate, use to provide a common data layout to subsequent test
/// types.  There are no semantics associated with any of the members, in
/// particular the allocator pointer is not used directly by this aggregate
/// to allocate storage owned by this class.
struct MyClassDef {

    // DATA (exceptionally public, only in test driver)
    int               d_value;
    int              *d_data_p;
    bslma::Allocator *d_allocator_p;

    // In optimized builds, some compilers will elide some of the operations in
    // the destructors of the test classes defined below.  In order to force
    // the compiler to retain all of the code in the destructors, we provide
    // the following function that can be used to (conditionally) print out the
    // state of a `MyClassDef` data member.  If the destructor calls this
    // function as its last operation, then all values set in the destructor
    // have visible side-effects, but non-verbose test runs do not have to be
    // burdened with additional output.
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

/// This `class` is a simple type that does not take allocators.  Its
/// implementation owns a `MyClassDef` aggregate, but uses only the
/// `d_value` data member, to support the `value` attribute.  The
/// `d_allocator_p` pointer is always initialized to a null pointer, while
/// the `d_data_p` pointer is never initialized.  This class supports move,
/// copy, and destructor counters and can be used in tests that check for
/// unnecessary copies.
struct MyClass1 {

    // DATA
    MyClassDef d_def;

    static int s_defaultConstructorInvocations;
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;
    static int s_destructorInvocations;

    // CREATORS
    MyClass1(int v = 0)  // IMPLICIT
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
        ++s_defaultConstructorInvocations;
    }
    MyClass1(const MyClass1& rhs)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++s_copyConstructorInvocations;
    }

    MyClass1(bslmf::MovableRef<MyClass1> other)
    {
        MyClass1& otherRef  = MoveUtil::access(other);
        d_def.d_value       = otherRef.d_def.d_value;
        d_def.d_allocator_p = 0;
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
        MyClass1& otherRef = MoveUtil::access(rhs);
        d_def.d_value      = otherRef.d_def.d_value;
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
int MyClass1::s_defaultConstructorInvocations = 0;
int MyClass1::s_copyConstructorInvocations    = 0;
int MyClass1::s_moveConstructorInvocations    = 0;
int MyClass1::s_copyAssignmentInvocations     = 0;
int MyClass1::s_moveAssignmentInvocations     = 0;
int MyClass1::s_destructorInvocations         = 0;

                              // ===============
                              // class MyClass1a
                              // ===============

/// This `class` is the same as MyClass1, except it also supports conversion
/// from MyClass1. This allows for testing of converting constructors and
/// assignment from a type convertible to value type.
struct MyClass1a {

    MyClass1   d_data;
    static int s_defaultConstructorInvocations;
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_copyAssignmentInvocations;
    static int s_moveAssignmentInvocations;
    static int s_destructorInvocations;

    // CREATORS

    MyClass1a(int v = 0)  // IMPLICIT
    : d_data(v)
    {
        ++s_defaultConstructorInvocations;
    }

    MyClass1a(const MyClass1& v)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass1a(bslmf::MovableRef<MyClass1> v)  // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(v)))
    {
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1> v)
    : d_data(MoveUtil::access(v))
    {
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
        ++s_moveAssignmentInvocations;
        return *this;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass1a& operator=(const MyClass1& rhs)
    {
        d_data = rhs;
        return *this;
    }

    MyClass1a& operator=(bslmf::MovableRef<MyClass1> rhs)
    {
        d_data = MoveUtil::move(MoveUtil::access(rhs));
        return *this;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a& operator=(bslmf::MovableRef<const MyClass1> rhs)
    {
        d_data = MoveUtil::access(rhs);
        return *this;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~MyClass1a() { ++s_destructorInvocations; }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
// CLASS DATA
int MyClass1a::s_defaultConstructorInvocations = 0;
int MyClass1a::s_copyConstructorInvocations    = 0;
int MyClass1a::s_moveConstructorInvocations    = 0;
int MyClass1a::s_copyAssignmentInvocations     = 0;
int MyClass1a::s_moveAssignmentInvocations     = 0;
int MyClass1a::s_destructorInvocations         = 0;

bool operator==(const MyClass1a& lhs, const MyClass1a& rhs)
{
    return (lhs.value() == rhs.value());
}

                               // ==============
                               // class MyClass2
                               // ==============

/// This `class` supports the `bslma::UsesBslmaAllocator` trait, providing
/// an allocator-aware version of every constructor.  While it holds an
/// allocator and has the expected allocator propagation properties of a
/// `bslma::Allocator`-aware type, it does not actually allocate any memory.
/// This class supports move, copy, and destructor counters and can be used
/// in tests that check for unnecessary copies and correct destructor
/// invocation.  This class is convertable and assignable from an object of
/// type `MyClass1`, which allows testing of converting constructors and
/// assignment from a type convertible to value type.
struct MyClass2 {

    // DATA
    MyClassDef d_def;

    static int s_defaultConstructorInvocations;
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
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
        s_defaultConstructorInvocations++;
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
        MyClass2& otherRef  = MoveUtil::access(other);
        d_def.d_value       = otherRef.d_def.d_value;
        d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        s_moveConstructorInvocations++;
    }
    MyClass2(bslmf::MovableRef<MyClass2> other, bslma::Allocator *a)
    {
        // IMPLICIT
        MyClass2& otherRef  = MoveUtil::access(other);
        d_def.d_value       = otherRef.d_def.d_value;
        d_def.d_allocator_p = a;

        s_moveConstructorInvocations++;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass2> other)
    {  // IMPLICIT

        const MyClass2& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p      = otherRef.d_def.d_allocator_p;
        // a move from a const object is a copy
        s_copyConstructorInvocations++;
    }
    MyClass2(bslmf::MovableRef<const MyClass2> other, bslma::Allocator *a)
    {  // IMPLICIT

        const MyClass2& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p      = a;

        // a move from a const object is a copy
        s_copyConstructorInvocations++;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2(const MyClass1& rhs, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    MyClass2(bslmf::MovableRef<MyClass1> other, bslma::Allocator *a = 0)
        // IMPLICIT
    {
        MyClass1& otherRef  = MoveUtil::access(other);
        d_def.d_value       = otherRef.d_def.d_value;
        d_def.d_allocator_p = a;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass1> other, bslma::Allocator *a = 0)
    {  // IMPLICIT

        const MyClass1& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p      = a;
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
        ++s_copyAssignmentInvocations;
        // do not touch allocator!
        return *this;
    }

    MyClass2& operator=(bslmf::MovableRef<MyClass2> rhs)
    {
        MyClass2& otherRef = MoveUtil::access(rhs);
        d_def.d_value      = otherRef.d_def.d_value;
        ++s_moveAssignmentInvocations;
        // do not touch allocator!
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2& operator=(bslmf::MovableRef<const MyClass2> rhs)
    {
        const MyClass2& otherRef = MoveUtil::access(rhs);
        d_def.d_value            = otherRef.d_def.d_value;
        ++s_moveAssignmentInvocations;
        // do not touch allocator!
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
int MyClass2::s_defaultConstructorInvocations = 0;
int MyClass2::s_copyConstructorInvocations    = 0;
int MyClass2::s_moveConstructorInvocations    = 0;
int MyClass2::s_copyAssignmentInvocations     = 0;
int MyClass2::s_moveAssignmentInvocations     = 0;
int MyClass2::s_destructorInvocations         = 0;

bool operator==(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() == rhs.value());
}
bool operator==(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() == rhs);
}

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

                                 // =========
                                 // MyClass2a
                                 // =========

/// This `class` behaves the same as `MyClass2` (allocator-aware type that
/// never actually allocates memory) except that it uses the
/// `allocator_arg_t` idiom for passing an allocator to constructors.  This
/// class is constructible and assignable from MyClass2
struct MyClass2a {

    MyClass2   d_data;

    static int s_defaultConstructorInvocations;
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
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
        ++s_defaultConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
        ++s_defaultConstructorInvocations;
    }

    MyClass2a(int v)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass2a(const MyClass2& rhs)  // IMPLICIT
    : d_data(rhs)
    {
    }

    MyClass2a(bslmf::MovableRef<MyClass2> rhs)  // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(rhs)))
    {
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2> rhs)  // IMPLICIT
    : d_data(MoveUtil::access(rhs))
    {
        ++s_copyConstructorInvocations;
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
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator            *a,
              bslmf::MovableRef<MyClass2>  v)
    : d_data(MoveUtil::move(MoveUtil::access(v)), a)
    {
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator                  *a,
              bslmf::MovableRef<const MyClass2>  v)
    : d_data(MoveUtil::access(v), a)
    {
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

    ~MyClass2a() { ++s_destructorInvocations; }

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
        ++s_moveAssignmentInvocations;
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
int MyClass2a::s_defaultConstructorInvocations = 0;
int MyClass2a::s_copyConstructorInvocations    = 0;
int MyClass2a::s_moveConstructorInvocations    = 0;
int MyClass2a::s_copyAssignmentInvocations     = 0;
int MyClass2a::s_moveAssignmentInvocations     = 0;
int MyClass2a::s_destructorInvocations         = 0;

                                 // =========
                                 // MyClass2b
                                 // =========

/// This struct behaves the same as `MyClass2` (allocator-aware type that
/// never actually allocates memory) except that it uses the
/// `allocator_arg_t` idiom for passing an allocator to constructors.  This
/// struct is assignable from MyClass2, but not constructible from MyClass2.
struct MyClass2b {

    // DATA
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

/// This struct behaves the same as `MyClass2` (allocator-aware type that
/// never actually allocates memory) except that it uses the
/// `allocator_arg_t` idiom for passing an allocator to constructors.  This
/// struct is constructable from `MyClass2`, but not assignable from
/// `MyClass2`.
struct MyClass2c {

    // DATA
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

/// This very simple `struct` is used purely to disambiguate types in
/// passing parameters to `construct` due to the fact that
/// `ConstructTestArg<ID1>` is a different type than `ConstructTestArg<ID2>`
/// if `ID1 != ID2`.
template <int ID>
struct ConstructTestArg {

    // PUBLIC DATA
    const int d_value;

    /// A counter tracking the number of copy constructions leading to this
    /// particular instance.
    int       d_copyCount;

    // CREATORS

    /// Create an object having the specified `value`.
    ConstructTestArg(int value = -1);  // IMPLICIT

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
                      // ===============================
                      // struct ConstructTestTypeNoAlloc
                      // ===============================

/// This `struct` provides a test class capable of holding up to 14
/// parameters of types `ConstructTestArg[1--14]`.  By default, a
/// `ConstructTestTypeNoAlloc` is constructed with nil (`N1`) values, but
/// instances can be constructed with actual values (e.g., for creating
/// expected values).  A `ConstructTestTypeNoAlloc` can be invoked with up
/// to 14 parameters, via member functions `testFunc[1--14]`.  These
/// functions are also called by the overloaded member `operator()` of the
/// same signatures, and similar global functions `testFunc[1--14]`.  All
/// invocations support the above `ConstructTestSlotsNoAlloc` mechanism.
///
/// This `struct` intentionally does *not* take an allocator.
struct ConstructTestTypeNoAlloc {

    // TYPES
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

    /// Argument types for shortcut.
    typedef ConstructTestArg<14> Arg14;

    enum {
        N1 = -1  // default value for all private data
    };

    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // DATA
    int d_ilsum;  // sum of initializer_list argument values

    Arg1  d_a1;
    Arg2  d_a2;
    Arg3  d_a3;
    Arg4  d_a4;
    Arg5  d_a5;
    Arg6  d_a6;
    Arg7  d_a7;
    Arg8  d_a8;
    Arg9  d_a9;
    Arg10 d_a10;
    Arg11 d_a11;
    Arg12 d_a12;
    Arg13 d_a13;
    Arg14 d_a14;

    // CREATORS
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
    ConstructTestTypeNoAlloc(std::initializer_list<int> il)  // IMPLICIT
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
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    ~ConstructTestTypeNoAlloc() { ++s_destructorInvocations; }
};
int ConstructTestTypeNoAlloc::s_copyConstructorInvocations = 0;
int ConstructTestTypeNoAlloc::s_moveConstructorInvocations = 0;
int ConstructTestTypeNoAlloc::s_destructorInvocations      = 0;
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
                       // =============================
                       // struct ConstructTestTypeAlloc
                       // =============================

/// This struct provides a test class capable of holding up to 14 parameters
/// of types `ConstructTestArg[1--14]`.  By default, a
/// `ConstructTestTypeAlloc` is constructed with nil (`N1`) values, but
/// instances can be constructed with actual values (e.g., for creating
/// expected values).  This struct intentionally *does* take an allocator.
struct ConstructTestTypeAlloc {

    // TYPES
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

    /// Argument types for shortcut.
    typedef ConstructTestArg<14> Arg14;

    enum {
        N1 = -1  // default value for all private data
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructTestTypeAlloc,
                                   bslma::UsesBslmaAllocator);

    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // DATA
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

    // CREATORS
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
                                                                    // IMPLICIT
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

    // In order to distinguish between an variant allocator parameter and a
    // non-variant `Arg` parameter, we need to explicitly call out the type of
    // the parameter before the variant allocator.
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
    ConstructTestTypeAlloc(
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         const Arg7&                             a7,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         bslmf::MovableRef<Arg7>                 a7,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         const Arg8&                             a8,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         bslmf::MovableRef<Arg8>                 a8,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                         const Arg9&                             a9,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                         bslmf::MovableRef<Arg9>                 a9,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         const Arg7&                             a7,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         bslmf::MovableRef<Arg7>                 a7,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         const Arg8&                             a8,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         bslmf::MovableRef<Arg8>                 a8,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                         const Arg9&                             a9,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                         bslmf::MovableRef<Arg9>                 a9,
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
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
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
                         std::initializer_list<int>              il,
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
                         bslma::Allocator                       *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        const Arg11&                             a11,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslmf::MovableRef<Arg11>                 a11,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        const Arg12&                             a12,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslmf::MovableRef<Arg12>                 a12,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        const Arg13&                             a13,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslmf::MovableRef<Arg13>                 a13,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        const Arg14&                             a14,
                        bslma::Allocator                        *allocator = 0)
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
    ConstructTestTypeAlloc(
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
                        bslmf::MovableRef<Arg14>                 a14,
                        bslma::Allocator                        *allocator = 0)
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
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    bsl::allocator<char> get_allocator() const { return d_allocator_p; }

    ~ConstructTestTypeAlloc() { ++s_destructorInvocations; }
};

int ConstructTestTypeAlloc::s_copyConstructorInvocations = 0;
int ConstructTestTypeAlloc::s_moveConstructorInvocations = 0;
int ConstructTestTypeAlloc::s_destructorInvocations      = 0;
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

                     // =================================
                     // struct ConstructTestTypeAllocArgT
                     // =================================

/// This struct provides a test class capable of holding up to 14 parameters
/// of types `ConstructTestArg[1--14]`.  By default, a
/// `ConstructTestTypeAllocArgT` is constructed with nil (`N1`) values, but
/// instances can be constructed with actual values (e.g., for creating
/// expected values).  This struct takes an allocator using the
/// `allocator_arg_t` protocol.
struct ConstructTestTypeAllocArgT {

    // TYPES
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

    /// Argument types for shortcut.
    typedef ConstructTestArg<14> Arg14;

    enum {
        N1 = -1  // default value for all private data
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructTestTypeAllocArgT,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(ConstructTestTypeAllocArgT,
                                   bslmf::UsesAllocatorArgT);

    // CLASS DATA
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // DATA
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

    // CREATORS
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

    ConstructTestTypeAllocArgT(const Arg1& a1)  // IMPLICIT
    : d_ilsum(0)
    , d_allocator_p(0)
    , d_a1(a1)
    {
    }

    ConstructTestTypeAllocArgT(bslmf::MovableRef<Arg1> a1)  // IMPLICIT
    : d_ilsum(0)
    , d_allocator_p(0)
    , d_a1(MoveUtil::move(a1))
    {
    }

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    /// This constructor is not defined and exists solely to make a
    /// corresponding `is_constructible` check succeed.
    template <class ARG2  = ConstructTestArg<2>,
              class ARG3  = ConstructTestArg<3>,
              class ARG4  = ConstructTestArg<4>,
              class ARG5  = ConstructTestArg<5>,
              class ARG6  = ConstructTestArg<6>,
              class ARG7  = ConstructTestArg<7>,
              class ARG8  = ConstructTestArg<8>,
              class ARG9  = ConstructTestArg<9>,
              class ARG10 = ConstructTestArg<10>,
              class ARG11 = ConstructTestArg<11>,
              class ARG12 = ConstructTestArg<12>,
              class ARG13 = ConstructTestArg<13>,
              class ARG14 = ConstructTestArg<14> >
    ConstructTestTypeAllocArgT(const ConstructTestArg<1>&,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) = 1);

    /// This constructor is not defined and exists solely to make a
    /// corresponding `is_constructible` check succeed.
    template <class ARG2  = ConstructTestArg<2>,
              class ARG3  = ConstructTestArg<3>,
              class ARG4  = ConstructTestArg<4>,
              class ARG5  = ConstructTestArg<5>,
              class ARG6  = ConstructTestArg<6>,
              class ARG7  = ConstructTestArg<7>,
              class ARG8  = ConstructTestArg<8>,
              class ARG9  = ConstructTestArg<9>,
              class ARG10 = ConstructTestArg<10>,
              class ARG11 = ConstructTestArg<11>,
              class ARG12 = ConstructTestArg<12>,
              class ARG13 = ConstructTestArg<13>,
              class ARG14 = ConstructTestArg<14> >
    ConstructTestTypeAllocArgT(ConstructTestArg<1>&,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) = 1);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    /// This constructor is not defined and exists solely to make a
    /// corresponding `is_constructible` check succeed.
    template <class ARG1  = ConstructTestArg<1>,
              class ARG2  = ConstructTestArg<2>,
              class ARG3  = ConstructTestArg<3>,
              class ARG4  = ConstructTestArg<4>,
              class ARG5  = ConstructTestArg<5>,
              class ARG6  = ConstructTestArg<6>,
              class ARG7  = ConstructTestArg<7>,
              class ARG8  = ConstructTestArg<8>,
              class ARG9  = ConstructTestArg<9>,
              class ARG10 = ConstructTestArg<10>,
              class ARG11 = ConstructTestArg<11>,
              class ARG12 = ConstructTestArg<12>,
              class ARG13 = ConstructTestArg<13>,
              class ARG14 = ConstructTestArg<14> >
    ConstructTestTypeAllocArgT(std::initializer_list<int>,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) = 1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) = 1);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif  // U_VARIANT_FULL_IMPLEMENTATION

    // allocator extended constructors

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

    ~ConstructTestTypeAllocArgT() { ++s_destructorInvocations; }
};

int ConstructTestTypeAllocArgT::s_copyConstructorInvocations = 0;
int ConstructTestTypeAllocArgT::s_moveConstructorInvocations = 0;
int ConstructTestTypeAllocArgT::s_destructorInvocations      = 0;

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
        SwappableAA& otherRef = MoveUtil::access(other);
        d_def.d_value         = otherRef.d_def.d_value;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }

    bsl::allocator<char> get_allocator() const { return d_def.d_allocator_p; }
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

struct Throws {
    static bool s_should_throw;
    static int  s_destructorInvocations;
    Throws()
    {
#ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
#endif  // BDE_BUILD_TARGET_EXC
    }

    Throws(const Throws&)
    {
#ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
#endif  // BDE_BUILD_TARGET_EXC
    }


#ifdef U_VARIANT_FULL_IMPLEMENTATION
    template <class... ARGS>
    Throws(ARGS&&...)  // IMPLICIT
    {
#ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
#endif  // BDE_BUILD_TARGET_EXC
    }

    template <class INIT_LIST_TYPE, class... ARGS>
    Throws(std::initializer_list<INIT_LIST_TYPE>, ARGS&&...)  // IMPLICIT
    {
#ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
#endif  // BDE_BUILD_TARGET_EXC
    }
#else
    template <class ARGS_01>
    explicit Throws(const ARGS_01&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&,
                    const ARGS_05&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&,
                    const ARGS_05&,
                    const ARGS_06&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&,
                    const ARGS_05&,
                    const ARGS_06&,
                    const ARGS_07&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&,
                    const ARGS_05&,
                    const ARGS_06&,
                    const ARGS_07&,
                    const ARGS_08&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&,
                    const ARGS_05&,
                    const ARGS_06&,
                    const ARGS_07&,
                    const ARGS_08&,
                    const ARGS_09&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09,
              class ARGS_10>
    explicit Throws(const ARGS_01&,
                    const ARGS_02&,
                    const ARGS_03&,
                    const ARGS_04&,
                    const ARGS_05&,
                    const ARGS_06&,
                    const ARGS_07&,
                    const ARGS_08&,
                    const ARGS_09&,
                    const ARGS_10&)
    {
    #ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
    #endif
    }
#endif

    Throws& operator=(const Throws&)
    {
#ifdef BDE_BUILD_TARGET_EXC
        if (s_should_throw)
            BSLS_THROW(1);
#endif  // BDE_BUILD_TARGET_EXC
        return *this;
    }

    ~Throws() { ++s_destructorInvocations; }
};
bool Throws::s_should_throw          = false;
int  Throws::s_destructorInvocations = 0;

bool operator==(const Throws&, const Throws&)
{
    return true;
}
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON               \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
std::strong_ordering operator<=>(const Throws&, const Throws&)
{
    return std::strong_ordering::equal;
}
#else

bool operator!=(const Throws&, const Throws&)
{
    return true;
}
bool operator<(const Throws&, const Throws&)
{
    return true;
}
bool operator>(const Throws&, const Throws&)
{
    return true;
}
bool operator<=(const Throws&, const Throws&)
{
    return true;
}
bool operator>=(const Throws&, const Throws&)
{
    return true;
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#define DEFINE_COMP_HELPER(Name, OP)                                          \
    struct Name##TestType {                                                   \
        Name##TestType(){};                                                   \
    };                                                                        \
    bool operator OP(const Name##TestType&, const Name##TestType&)            \
    {                                                                         \
        return true;                                                          \
    }

DEFINE_COMP_HELPER(Equal, ==)
DEFINE_COMP_HELPER(Unequal, !=)
DEFINE_COMP_HELPER(LessThan, <)
DEFINE_COMP_HELPER(GreaterThan, >)
DEFINE_COMP_HELPER(LessOrEqual, <=)
DEFINE_COMP_HELPER(GreaterOrEqual, >=)

template <class HASHALG>
void hashAppend(HASHALG&, const Throws&){};

enum {
    INT_TYPE         = 0,
    LONG_TYPE        = 1,
    CHAR_TYPE        = 2,
    CONST_INT_TYPE   = 3,
    INT_TYPE_R       = 4,
    LONG_TYPE_R      = 5,
    CHAR_TYPE_R      = 6,
    CONST_INT_TYPE_R = 7,
    THROWS           = 8
};

template <class t_TYPE>
struct CallablePassByLRef {
    t_TYPE operator()(int&)
    {
        static int i = INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(long&)
    {
        static int i = LONG_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(char&)
    {
        static int i = CHAR_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const int&)
    {
        static int i = CONST_INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const Throws&)
    {
        static int i = THROWS;
        return static_cast<t_TYPE>(i);
    }
};
template <class t_TYPE>
struct CallablePassByConstLRef {
    t_TYPE operator()(const int&)
    {
        static int i = INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const long&)
    {
        static int i = LONG_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const char&)
    {
        static int i = CHAR_TYPE;
        return static_cast<t_TYPE>(i);
    }
};
#ifdef U_VARIANT_FULL_IMPLEMENTATION
template <class t_TYPE>
struct CallablePassByRRef {
    t_TYPE operator()(int&&) &
    {
        static int i = INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(long&&) &
    {
        static int i = LONG_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(char&&) &
    {
        static int i = CHAR_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const int&&) &
    {
        static int i = CONST_INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(int&&) &&
    {
        static int i = INT_TYPE_R;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(long&&) &&
    {
        static int i = LONG_TYPE_R;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(char&&) &&
    {
        static int i = CHAR_TYPE_R;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const int&&) &&
    {
        static int i = CONST_INT_TYPE_R;
        return static_cast<t_TYPE>(i);
    }
};

template <class t_TYPE>
struct CallablePassByConstRRef {
    t_TYPE operator()(const int&)
    {
        static int i = INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const long&)
    {
        static int i = LONG_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const char&)
    {
        static int i = CHAR_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const int&&)
    {
        static int i = INT_TYPE_R;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const long&&)
    {
        static int i = LONG_TYPE_R;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const char&&)
    {
        static int i = CHAR_TYPE_R;
        return static_cast<t_TYPE>(i);
    }
};

struct VoidCallable {
    int i = 99;
    void operator()(const int&) &
    {
        i = INT_TYPE;
    }

    void operator()(const long&) &
    {
        i = LONG_TYPE;
    }

    void operator()(const char&) &
    {
        i = CHAR_TYPE;
    }

    void operator()(const int&) &&
    {
        i = INT_TYPE_R;
    }

    void operator()(const long&) &&
    {
        i = LONG_TYPE_R;
    }

    void operator()(const char&) &&
    {
        i = CHAR_TYPE_R;
    }
};
#else
template <class t_TYPE>
struct CallablePassByConstRRef {
    t_TYPE operator()(const int&)
    {
        static int i = INT_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const long&)
    {
        static int i = LONG_TYPE;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(const char&)
    {
        static int i = CHAR_TYPE;
        return static_cast<t_TYPE>(i);
    }
    t_TYPE operator()(BloombergLP::bslmf::MovableRef<int>)
    {
        static int i = INT_TYPE_R;
        return static_cast<t_TYPE>(i);
    }

    t_TYPE operator()(BloombergLP::bslmf::MovableRef<long>)
    {
        static int i = LONG_TYPE_R;
        return static_cast<t_TYPE>(i);
    }
    t_TYPE operator()(BloombergLP::bslmf::MovableRef<char>)
    {
        static int i = CHAR_TYPE_R;
        return static_cast<t_TYPE>(i);
    }
    t_TYPE operator()(BloombergLP::bslmf::MovableRef<const int>)
    {
        static int i = CONST_INT_TYPE_R;
        return static_cast<t_TYPE>(i);
    }
};

#endif
/// This class provides test utilities that have different behaviour
/// depending on whether `TYPE` is allocator-aware or not.  The primary
/// template is for allocator-aware types.
template <class TYPE,
          bool  USES_BSLMA_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value>
class Test_Util {

  public:
    /// Check if, for the specified `obj`, `obj.get_allocator()` returns the
    /// specified `expected` allocator.
    static bool checkAllocator(const TYPE&                 obj,
                               const bsl::allocator<char>& expected);

    /// Check if, for the specified `obj` and specified `other`,
    /// `obj.get_allocator() == other.get_allocator()`;
    static bool hasSameAllocator(const TYPE& obj, const TYPE& other);
};

/// This class provides test utilities that have different behaviour
/// depending on whether `TYPE` is allocator-aware or not.  This
/// specialization is for non allocator-aware types.
template <class TYPE>
class Test_Util<TYPE, false> {

  public:
    /// return `true`.
    static bool checkAllocator(const TYPE&, const bsl::allocator<char>&);

    /// return `true`.
    static bool hasSameAllocator(const TYPE&, const TYPE&);
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

template <class TYPE, bool = isSwapTestType<TYPE>::value>
struct SwapUtil {
    static void swapReset() {}
    static bool swapCalled() { return true; }
};

template <class TYPE>
struct SwapUtil<TYPE, true> {
    static void swapReset() { TYPE::swapReset(); }
    static bool swapCalled() { return TYPE::swapCalled(); }
};

enum { LVALUE_REF = 0, LVALUE_CONST_REF, RVALUE_REF, RVALUE_CONST_REF };

int checkQualification(int&)
{
    return LVALUE_REF;
}
int checkQualification(const int&)
{
    return LVALUE_CONST_REF;
}
#ifdef U_VARIANT_FULL_IMPLEMENTATION
int checkQualification(int&&)
{
    return RVALUE_REF;
}
int checkQualification(const int&&)
{
    return RVALUE_CONST_REF;
}
#else
int checkQualification(BloombergLP::bslmf::MovableRef<int>)
{
    return RVALUE_REF;
}
#endif

template <class t_TYPE>
bool isConst(t_TYPE)
{
    return bsl::is_same<t_TYPE, const t_TYPE>::value;
}
template <class t_TYPE>
bool isConstPtr(t_TYPE *)
{
    return bsl::is_same<t_TYPE, const t_TYPE>::value;
}
#ifdef U_VARIANT_FULL_IMPLEMENTATION

template <class VARIANT, class TYPE, class... ARGS>
struct EmplaceHelperType {
    template <class ALT_TYPE = TYPE,
              class          = bsl::void_t<
                  decltype(std::declval<VARIANT>().template emplace<ALT_TYPE>(
                               std::declval<ARGS>()...))> >
    static bool check(int)
    {
        return true;
    };

    static bool check(char) { return false; };
};

template <class VARIANT1, class VARIANT2, class = void>
struct FreeSwapHelper : std::false_type {
};

template <class VARIANT1, class VARIANT2>
struct FreeSwapHelper<VARIANT1,
                      VARIANT2,
                      bsl::void_t<decltype(swap(std::declval<VARIANT1 &>(),
                                                std::declval<VARIANT2 &>()))> >
: std::true_type {
};

template <class VARIANT1, class VARIANT2, class = void>
struct SwapHelper : std::false_type {
};

template <class VARIANT1, class VARIANT2>
struct SwapHelper<VARIANT1,
                  VARIANT2,
                  bsl::void_t<decltype(std::declval<VARIANT1 &>().swap(
                      std::declval<VARIANT2 &>()))> > : std::true_type {
};

#endif  // U_VARIANT_FULL_IMPLEMENTATION

bool checkVariantNpos(const size_t&)
{
    return true;
}
template <class t_TYPE>
bool checkVariantNpos(t_TYPE&)
{
    return false;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
namespace {
/// If the specified `t_VARIANT` is a specialization of `bsl::variant`,
/// provide the member `type`, which is an alias to the corresponding
/// specialization of `std::variant`.
template <class t_VARIANT>
struct BslVariantToStdVariant;

template <class t_HEAD, class... t_TAIL>
struct BslVariantToStdVariant<bsl::variant<t_HEAD, t_TAIL...>> {
    using type = std::variant<t_HEAD, t_TAIL...>;
};

template <class t_TYPE>
void tryCopyInitialize(t_TYPE)
{
    // This function is only called in an unevaluated operand, but needs a
    // definition in order to avoid the `Wunused-function` warning.
}

template <class... t_ARGS,
          class    t_TYPE,
          class = decltype(
                       tryCopyInitialize<t_TYPE>({std::declval<t_ARGS>()...}))>
constexpr bool canCopyListInitializeFrom(t_TYPE*)
{
    return true;
}

template <class... t_ARGS>
constexpr bool canCopyListInitializeFrom(void*)
{
    return false;
}

/// This variable template is `true` if the specified object type `t_TYPE`
/// is explicitly constructible from the specified `t_ARGS...` but not
/// implicitly constructible from a braced-init-list of `t_ARGS...`.  Note
/// that we can't put the braced-init-list argument directly in the
/// definition of `isOnlyExplicitlyConstructible` because the explicit
/// constructor can win and make the initialization ill formed here (see
/// CWG2525); we have to push the braced-init-list into an SFINAE context,
/// necessitating the `canCopyListInitializeFrom` helper.
template <class t_TYPE, class... t_ARGS>
constexpr bool isOnlyExplicitlyConstructible =
                      std::is_constructible_v<t_TYPE, t_ARGS...> &&
                      !canCopyListInitializeFrom<t_ARGS...>((t_TYPE*)nullptr);
}  // close unnamed namespace
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// ============================================================================
//                              TEST CASE 16
// ----------------------------------------------------------------------------

namespace test_case_16 {
template <class t_TYPE>
struct T { };

template <size_t t_INDEX, class t_TYPE>
bool get(const T<t_TYPE>&)
{
    return true;
}

template <class t_ELEM_TYPE, class t_TYPE>
bool get(const T<t_TYPE>&)
{
    return true;
}

template <class t_TYPE>
bool m(const t_TYPE& t) {
    return get<0>(t) && get<int>(t);
}
}  // close namespace test_case_16

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

/// This class template provides a namespace for testing the `variant` type.
template <class TYPE,
          bool  USES_BSLMA_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value>
class TestDriver {

  private:
    // PRIVATE TYPES

    /// VariantType under test.
    typedef TYPE VariantType;

  public:
    /// TESTING `hashAppend`
    static void testCase13();

    static void testCase7Index();

    /// TESTING `emplace` METHOD.  Note that this test can only be executed
    /// with types that are constructible from an integer, have a `value()`
    /// method, and which provide a `s_copyConstructorInvocations` and
    /// `s_moveConstructorInvocations` static variable that counts the
    /// number of times copy/move constructor has been invoked.
    static void testCase7Type();

    /// TESTING operator=(alternative type)
    static void testCase11b();

    /// TESTING operator=(variant)
    static void testCase11a();

    /// TESTING swap
    static void testCase10();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// TESTING ALLOCATOR-EXTENDED CONSTRUCTION FROM `std::variant`
    static void testCase9f();

    /// TESTING CONSTRUCTION FROM `std::variant`
    static void testCase9e();
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    static void testCase9b();
        // TESTING ALLOCATOR EXTENDED CONSTRUCTION FROM VARIANT
    static void testCase9a();
        // TESTING CONSTRUCTION FROM VARIANT

    /// TESTING `holds_alternative` FREE FUNCTION
    static void testCase6c();

    static void testCase6bIndex();

    /// TESTING `get_if` FREE FUNCTION
    static void testCase6bType();

    static void testCase6aIndex();

    /// TESTING `get` FREE FUNCTION
    static void testCase6aType();

    static void testCase5dIndex();

    /// TESTING ALLOCATOR EXTENDED `in_place_t` CONSTRUCTOR
    static void testCase5dType();

    static void testCase5cIndex();

    /// TESTING `in_place_t` CONSTRUCTOR
    static void testCase5cType();

    static void testCase5b();
        // TESTING ALLOCATOR EXTENDED CONSTRUCTION FROM VALUE

    static void testCase5a();
        // TESTING CONSTRUCTION FROM VALUE

    /// ALLOCATOR EXTENDED DEFAULT CONSTRUCTORS.  Note that this test
    /// requires the designated alternative to be allocator-aware and to
    /// provide static data members named `s_copyConstructorInvocations` and
    /// `s_destructorInvocations` that count the number of times the default
    /// constructor and the destructor have been invoked, respectively.
    static void testCase4b();

    /// DEFAULT CONSTRUCTOR AND DESTRUCTOR.  Note that this test requires
    /// the designated alternative to provide static data members named
    /// `s_copyConstructorInvocations` and `s_destructorInvocations` that
    /// count the number of times the default constructor and the destructor
    /// have been invoked, respectively.
    static void testCase4a();

    static void testCase2();
        // TESTING TRAITS AND TYPEDEFS

    /*    static void testCase1();
        // BREATHING TEST
*/
};

void testCase13a()
{
    // ------------------------------------------------------------------------
    // TESTING `hashAppend` of EMPTY VARIANT
    //
    // Concerns:
    // 1. Hashing a value of empty `variant` object is equivalent to appending
    //    `false` to the hash.
    //
    //
    // Plan:
    // 1. Create an empty `variant` object and verify that hashing it yields
    // the
    //    same value as hashing `false`. [C-1]
    //
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const variant<TYPES...>& input);
    // ------------------------------------------------------------------------
#ifdef BDE_BUILD_TARGET_EXC
    if (verbose)
        printf("\nTESTING `hashAppend` of EMPTY VARIANT"
               "\n====================================\n");

    {
        bsl::variant<int, Throws> x;

        Throws::s_should_throw = true;
        BSLS_TRY { x.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        const size_t hashValue1 = bslh::Hash<>()(x);

        bslh::DefaultHashAlgorithm hasher;
        hashAppend(hasher, false);
        const size_t hashValue2 = static_cast<size_t>(hasher.computeHash());

        ASSERTV(hashValue1, hashValue2, hashValue2 == hashValue1);
    }
#endif  // BDE_BUILD_TARGET_EXC
}

template <class VARIANT, size_t t_NUM>
struct testCase13_imp;

template <class VARIANT>
struct testCase13_imp<VARIANT, 0> {
};

template <class VARIANT, size_t t_NUM>
struct testCase13_imp : testCase13_imp<VARIANT, t_NUM - 1> {
    testCase13_imp()
    {
        // --------------------------------------------------------------------
        // TESTING `hashAppend`
        //
        // Concerns:
        // 1. Hashing a non empty variant object is equivalent to appending the
        //    index of the currently active alternative type, followed by the
        // value
        //    of the managed object.
        //
        // Plan:
        //
        // 1. Create a `variant` object and for a series of alternative types
        // and
        //    their test values verify that hashing it produces the same result
        // as
        //    hashing the index of the tested type and the test value. [C-1]
        //
        // Testing:
        //   void hashAppend(HASHALG& hashAlg, const variant<TYPES...>& input);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING `hashAppend`"
                   "\n====================\n");

        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;

        /// Array of test values of `TYPE`.
        typedef bsltf::TestValuesArray<AltType>                    TestValues;
        const TestValues VALUES;
        const int        NUM_VALUES = static_cast<int>(VALUES.size());

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            VARIANT x;

            for (int i = 0; i < NUM_VALUES; ++i) {
                x.template emplace<AltType>(VALUES[i]);

                bslma::TestAllocatorMonitor oam(&oa);

                bslh::DefaultHashAlgorithm hasher;

                const size_t hashValue1 = bslh::Hash<>()(x);
                hashAppend(hasher, INDEX);
                hashAppend(hasher, VALUES[i]);
                const size_t hashValue2 =
                    static_cast<size_t>(hasher.computeHash());

                ASSERTV(hashValue1, hashValue2, hashValue2 == hashValue1);

                ASSERT(oam.isInUseSame());
                ASSERT(0 == da.numBlocksInUse());
            }
        }
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase13()
{
    testCase13_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

void testCase12b()
{
    // --------------------------------------------------------------------
    // TESTING VISIT WITH SPECIFIED RETURN TYPE
    //
    //   This test will ensure that visit method with specified return type
    //   behaves correctly.
    //
    // Concerns:
    // 1. That visit invokes the specified visitor with the currently active
    //    alternative.
    //
    // 2. That value category of the variant is preserved, i.e. that invoking
    //    visit with an lvalue object invokes the visitor with  an lvalue, and
    //    that invoking visit with an rvalue object invokes the visitor with an
    //    rvalue.  Additionally, in n C++17, that the value category of the
    //    visitor is preserved.
    //
    // 3. That the constness of the variant is preserved, i.e. that invoking
    //    visit with an lvalue object invokes the visitor with a const
    //    qualified alternative.  Additionally, In C++17, that the constness of
    //    the visitor is preserved.
    //
    // 4. That the constness of the active alternative is preserved when
    //    invoking the visitor.  Additionally, that constness of the active
    //    alternative is preserved in presence of another alternative of same
    //    type, but different cv-qualification.
    //
    // 5. That visitation works in presence of identical alternatives,
    //
    // 7. That it is possible to specify void as the return type when the
    //    visitor returns a non-void type.
    //
    //
    // Plan:
    // 1. For the concerns above, using the helper visitors, check that `visit`
    //    can be invoked and that it results in the correct value category and
    //    cv qualification of the alternative, and in C++17, the visitor.
    //
    // Testing:
    //
    //     RET visit<RET, VISITOR, VARIANT>(VISITOR&&, VARIANT&&);
    //     RET visitR<RET, VISITOR, VARIANT>(VISITOR&, VARIANT&);

    bsl::variant<int, long, char, int, const int>        var;
    const bsl::variant<int, long, char, int, const int>& cvar = var;
    CallablePassByLRef<int>                              LRfunctor;
    CallablePassByLRef<void>                             VoidLRfunctor;
    CallablePassByLRef<const int&>                       Reffunctor;
    CallablePassByConstLRef<int>                         CLRfunctor;
    CallablePassByConstRRef<int>                         CRRfunctor;
#ifdef U_VARIANT_FULL_IMPLEMENTATION
    CallablePassByRRef<int> RRfunctor;

    ASSERT(bsl::visit<int>(LRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit<int>(CLRfunctor, var) == INT_TYPE);
#ifdef MANUAL_TEST9
    ASSERT(bsl::visit<int>(RRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit<int>(CRRfunctor, var) == INT_TYPE);
#endif

    ASSERT(bsl::visit<int>(CLRfunctor, cvar) == INT_TYPE);
#ifdef MANUAL_TEST10
    ASSERT(bsl::visit<int>(LRfunctor, cvar) == INT_TYPE);
    ASSERT(bsl::visit<int>(RRfunctor, cvar) == INT_TYPE);
    ASSERT(bsl::visit<int>(CRRfunctor, cvar) == INT_TYPE);
#endif

#ifdef MANUAL_TEST11
    ASSERT(bsl::visit<int>(LRfunctor, MoveUtil::move(var)) == INT_TYPE);
#endif
    ASSERT(bsl::visit<int>(CLRfunctor, MoveUtil::move(var)) == INT_TYPE);
    ASSERT(bsl::visit<int>(RRfunctor, MoveUtil::move(var)) == INT_TYPE);
    ASSERT(bsl::visit<int>(MoveUtil::move(RRfunctor), MoveUtil::move(var)) ==
           INT_TYPE_R);
    ASSERT(bsl::visit<int>(CRRfunctor, MoveUtil::move(var)) == INT_TYPE_R);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#ifdef MANUAL_TEST12
    ASSERT(bsl::visit<int>(LRfunctor, MoveUtil::move(cvar)) == INT_TYPE);
#endif
    ASSERT(bsl::visit<int>(CLRfunctor, MoveUtil::move(cvar)) == INT_TYPE);
#ifdef MANUAL_TEST13
    ASSERT(bsl::visit<int>(RRfunctor, MoveUtil::move(cvar)) == INT_TYPE);
#endif
    ASSERT(bsl::visit<int>(CRRfunctor, MoveUtil::move(cvar)) == INT_TYPE_R);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    var.emplace<1>();
    ASSERT(bsl::visit<int>(LRfunctor, var) == LONG_TYPE);
    ASSERT(bsl::visit<int>(CLRfunctor, var) == LONG_TYPE);

    var.emplace<2>();
    ASSERT(bsl::visit<int>(LRfunctor, var) == CHAR_TYPE);
    ASSERT(bsl::visit<int>(CLRfunctor, var) == CHAR_TYPE);

    var.emplace<3>();
    ASSERT(bsl::visit<int>(LRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit<int>(CLRfunctor, var) == INT_TYPE);

    var.emplace<4>();
    ASSERT(bsl::visit<int>(LRfunctor, var) == CONST_INT_TYPE);
    ASSERT(bsl::visit<int>(MoveUtil::move(RRfunctor), MoveUtil::move(var)) ==
           CONST_INT_TYPE_R);

    VoidCallable Voidfunctor;
    bsl::visit<void>(Voidfunctor, var);
    ASSERT(Voidfunctor.i == INT_TYPE);

    bsl::visit<void>(MoveUtil::move(Voidfunctor), MoveUtil::move(var));
    ASSERT(Voidfunctor.i == INT_TYPE_R);

    // check the return type deduction and value category
    bsl::visit<void>(LRfunctor, var);
    bsl::visit<void>(Voidfunctor, var);
    bsl::visit<const int&>(Reffunctor, var);

#endif  // U_VARIANT_FULL_IMPLEMENTATION
    // visitR tests
    var.emplace<0>();
    ASSERT(bsl::visitR<int>(LRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visitR<int>(CLRfunctor, var) == INT_TYPE);

    ASSERT(bsl::visitR<int>(CLRfunctor, cvar) == INT_TYPE);
#ifdef MANUAL_TEST15
    ASSERT(bsl::visitR<int>(LRfunctor, cvar) == INT_TYPE);
#endif

#ifdef MANUAL_TEST16
    ASSERT(bsl::visitR<int>(LRfunctor, MoveUtil::move(var)) == INT_TYPE);
#endif
    ASSERT(bsl::visitR<int>(CLRfunctor, MoveUtil::move(var)) == INT_TYPE);
    ASSERT(bsl::visitR<int>(CRRfunctor, MoveUtil::move(var)) == INT_TYPE_R);

    var.emplace<1>();
    ASSERT(bsl::visitR<int>(LRfunctor, var) == LONG_TYPE);

    var.emplace<2>();
    ASSERT(bsl::visitR<int>(LRfunctor, var) == CHAR_TYPE);

    var.emplace<3>();
    ASSERT(bsl::visitR<int>(LRfunctor, var) == INT_TYPE);

    // check the return type deduction and value category
    bsl::visitR<void>(LRfunctor, var);
    bsl::visitR<void>(VoidLRfunctor, var);

    bsl::visitR<const int&>(Reffunctor, var);
}

void testCase12a()
{
    // --------------------------------------------------------------------
    // TESTING VISIT WITHOUT SPECIFIED RETURN TYPE
    //
    //   This test will ensure that visit method without specified return type
    //   behaves correctly.
    //
    // Concerns:
    // 1. That visit invokes the specified visitor with the currently active
    //    alternative.
    //
    // 2. That value category of the variant is preserved, i.e. that invoking
    //    visit with an lvalue object invokes the visitor with  an lvalue, and
    //    that invoking visit with an rvalue object invokes the visitor with an
    //    rvalue.  Additionally, in n C++17, that the value category of the
    //    visitor is preserved.
    //
    // 3. That the constness of the variant is preserved, i.e. that invoking
    //    visit with an lvalue object invokes the visitor with a const
    //    qualified alternative.  Additionally, In C++17, that the constness of
    //    the visitor is preserved.
    //
    // 4. That the constness of the active alternative is preserved when
    //    invoking the visitor.  Additionally, that constness of the active
    //    alternative is preserved in presence of another alternative of same
    //    type, but different cv-qualification.
    //
    // 5. That visitation works in presence of identical alternatives,
    //
    // 6. That it is possible to work with visitors returning void and
    //    reference types.
    //
    // 7. That visitation of an valueless variant throws a `bad_variant_access`
    //    exception.
    //
    // Plan:
    // 1. For the concerns above, using the helper visitors, check that `visit`
    //    can be invoked, that the correct alternative is passed to the
    //    visitor, and that value category and cv qualification of the
    //    alternative, and in C++17, the visitor are preserved.
    //
    // 2. Using `Throws` type, check that a visitation of the valueless
    //    variant throws a `bad_variant_access` exception.
    //
    // Testing:
    //
    //     RET visit<RET, VISITOR, VARIANT>(VISITOR&&, VARIANT&&);

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    // check the type, value category, and constness of the alternative

    bsl::variant<int, long, char, int, const int>        var;
    const bsl::variant<int, long, char, int, const int>& cvar = var;
    CallablePassByLRef<int>                              LRfunctor;
    CallablePassByConstLRef<int>                         CLRfunctor;
    CallablePassByRRef<int>                              RRfunctor;
    CallablePassByConstRRef<int>                         CRRfunctor;

    ASSERT(bsl::visit(LRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit(CLRfunctor, var) == INT_TYPE);
#ifdef MANUAL_TEST1
    ASSERT(bsl::visit(RRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit(CRRfunctor, var) == INT_TYPE);
#endif

    ASSERT(bsl::visit(CLRfunctor, cvar) == INT_TYPE);
#ifdef MANUAL_TEST2
    ASSERT(bsl::visit(LRfunctor, cvar) == INT_TYPE);
    ASSERT(bsl::visit(RRfunctor, cvar) == INT_TYPE);
    ASSERT(bsl::visit(CRRfunctor, cvar) == INT_TYPE);
#endif

#ifdef MANUAL_TEST3
    ASSERT(bsl::visit(LRfunctor, MoveUtil::move(var)) == INT_TYPE);
#endif
    ASSERT(bsl::visit(CLRfunctor, MoveUtil::move(var)) == INT_TYPE);
    ASSERT(bsl::visit(RRfunctor, MoveUtil::move(var)) == INT_TYPE);
    ASSERT(bsl::visit(MoveUtil::move(RRfunctor), MoveUtil::move(var)) ==
           INT_TYPE_R);
    ASSERT(bsl::visit(CRRfunctor, MoveUtil::move(var)) == INT_TYPE_R);

#ifdef MANUAL_TEST4
    ASSERT(bsl::visit(LRfunctor, MoveUtil::move(cvar)) == INT_TYPE);
#endif
    ASSERT(bsl::visit(CLRfunctor, MoveUtil::move(cvar)) == INT_TYPE);
#ifdef MANUAL_TEST5
    ASSERT(bsl::visit(RRfunctor, MoveUtil::move(cvar)) == INT_TYPE);
#endif
    ASSERT(bsl::visit(CRRfunctor, MoveUtil::move(cvar)) == INT_TYPE_R);

    var.emplace<1>();
    ASSERT(bsl::visit(LRfunctor, var) == LONG_TYPE);
    ASSERT(bsl::visit(CLRfunctor, var) == LONG_TYPE);

    var.emplace<2>();
    ASSERT(bsl::visit(LRfunctor, var) == CHAR_TYPE);
    ASSERT(bsl::visit(CLRfunctor, var) == CHAR_TYPE);

    var.emplace<3>();
    ASSERT(bsl::visit(LRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit(CLRfunctor, var) == INT_TYPE);

    var.emplace<4>();
    ASSERT(bsl::visit(LRfunctor, var) == CONST_INT_TYPE);

    // check the return type deduction and value category
    VoidCallable Voidfunctor;
    bsl::visit(Voidfunctor, var);
    ASSERT(Voidfunctor.i == INT_TYPE);

    bsl::visit(MoveUtil::move(Voidfunctor), var);
    ASSERT(Voidfunctor.i == INT_TYPE_R);


    CallablePassByLRef<const int&> Reffunctor;
    ASSERT(
     (std::is_same<decltype(bsl::visit(Reffunctor, var)), const int&>::value));

    ASSERT(bsl::visit(MoveUtil::move(RRfunctor), MoveUtil::move(var)) ==
           CONST_INT_TYPE_R);


#else   // U_VARIANT_FULL_IMPLEMENTATION

    bsl::variant<int, long, char, int> var;
    const bsl::variant<int, long, char, int>& cvar = var;
    CallablePassByLRef<int> LRfunctor;
    CallablePassByConstLRef<int> CLRfunctor;
    CallablePassByConstRRef<int> CRRfunctor;

    ASSERT(bsl::visit(LRfunctor, var) == INT_TYPE);
    ASSERT(bsl::visit(CLRfunctor, var) == INT_TYPE);

    ASSERT(bsl::visit(CLRfunctor, cvar) == INT_TYPE);

    ASSERT(bsl::visit(CLRfunctor, MoveUtil::move(var)) == INT_TYPE);
    ASSERT(bsl::visit(CRRfunctor, MoveUtil::move(var)) == INT_TYPE_R);

    var.emplace<1>();
    ASSERT(bsl::visit(LRfunctor, var) == LONG_TYPE);

    var.emplace<2>();
    ASSERT(bsl::visit(LRfunctor, var) == CHAR_TYPE);

    var.emplace<3>();
    ASSERT(bsl::visit(LRfunctor, var) == INT_TYPE);

    CallablePassByLRef<void> Voidfunctor;
    bsl::visit(Voidfunctor, var);

    CallablePassByLRef<const int&> Reffunctor;
    bsl::visit(Reffunctor, var);
#endif  // U_VARIANT_FULL_IMPLEMENTATION

    // testing empty variant
    bsl::variant<int, Throws> empty;
    Throws::s_should_throw = true;
    BSLS_TRY { empty.emplace<Throws>(); }
    BSLS_CATCH(...) {}
    ASSERT(empty.valueless_by_exception());
    bool bvaExceptionThrown = false;
    BSLS_TRY { bsl::visit(LRfunctor, empty); }
    BSLS_CATCH(const bsl::bad_variant_access&) { bvaExceptionThrown = true; }
    BSLS_CATCH(...) {}
#ifdef BDE_BUILD_TARGET_EXC
    ASSERT(bvaExceptionThrown);
#endif  // BDE_BUILD_TARGET_EXC
}

void testCase11e()
{
    // --------------------------------------------------------------------
    // TESTING ASSIGNABLE TRAITS
    //
    //   This test will ensure that the copy/move assignment is disabled
    //   under certain conditions.
    //
    // Concerns:
    // 1. That the variant with an alternative that is not copyable is also
    //    not asignable.  Similarly, if any alternative is moveable, but not
    //    copyable, the variant is move assignable, but not copy assignable.
    //
    // 1. That the variant with an alternative that is not copyable is also
    //    not asignable.  Similarly, if any alternative is move assignable, but
    //    not copy assignable, the variant is move assignable, but not copy
    //    assignable.
    //
    // 3. That the variant with non unique alternative TYPE can be copy/move
    //    assigned from another variant.
    //
    // 4. That the variant with two alternatives of same type, but different
    //    cv-qualification can be copy/move assigned from another variant.
    //
    // 5. If the type of `rhs` does not exactly match any alternative type,
    //    and is not convertible to any alternative type, the assignment
    //    from `rhs` is not possible.
    //
    // 6. If the type of `rhs` does not exactly match any alternative type,
    //    and has an equally good conversion to more than one alternative type,
    //    the assignment from `rhs` is not possible.
    //
    // 7. If the type of `rhs` does not exactly match any alternative type,
    //    and has a deleted conversion to one alternative type, the
    //    assignment from `rhs` is not possible.
    //
    // 8. If the type of `rhs` does not exactly match any alternative type,
    //    and has a narrowing conversion to one alternative type, the
    //    assignment from `rhs` is not possible.
    //
    //
    // Plan:
    // 1. For the concerns above, check `is_assignable` trait with the
    //    appropriate set of arguments.
    //
    //
    // Testing:
    //
    //      variant& operator=(const variant&);
    //      variant& operator=(variant&&);
    //      variant& operator=(T&&);

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    struct NonCopyable {
        NonCopyable(const NonCopyable&) = delete;
    };

    struct MoveOnly {
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&)      = default;

        MoveOnly& operator=(const MoveOnly&) = delete;
    };

    struct NonAssignable {
        NonAssignable(const NonAssignable&)            = default;
        NonAssignable& operator=(const NonAssignable&) = delete;
    };

    struct MoveAssignable {
        MoveAssignable(const MoveAssignable&) = default;
        MoveAssignable(MoveAssignable&&)      = default;

        MoveAssignable& operator=(const MoveAssignable&) = delete;
        MoveAssignable& operator=(MoveAssignable&&)      = default;
    };

    typedef bsl::variant<MyClass2, NonCopyable>    NonCopy_Variant;
    typedef bsl::variant<MyClass2, MoveOnly>       MoveOnly_Variant;
    typedef bsl::variant<MyClass2, NonAssignable>  NoAssign_Variant;
    typedef bsl::variant<MyClass2, MoveAssignable> MoveAssign_Variant;
    typedef bsl::variant<MyClass2, MyClass2>       Unique_Variant;
    typedef bsl::variant<MyClass2, const MyClass2> CVUnique_Variant;

    ASSERT(!(std::is_copy_assignable<NonCopy_Variant>::value));
    ASSERT(!(std::is_copy_assignable<MoveOnly_Variant>::value));
    ASSERT(!(std::is_copy_assignable<NonCopy_Variant>::value));
    ASSERT(!(std::is_copy_assignable<NoAssign_Variant>::value));
    ASSERT(!(std::is_copy_assignable<MoveAssign_Variant>::value));

    ASSERT(!(std::is_move_assignable<NonCopy_Variant>::value));
    ASSERT(!(std::is_move_assignable<MoveOnly_Variant>::value));
    ASSERT(!(std::is_move_assignable<NonCopy_Variant>::value));
    ASSERT(!(std::is_move_assignable<NoAssign_Variant>::value));
    ASSERT((std::is_move_assignable<MoveAssign_Variant>::value));

    ASSERT(!(std::is_assignable<Unique_Variant, MyClass2>::value));
    ASSERT(!(std::is_assignable<Unique_Variant, MyClass2&>::value));
    ASSERT(!(std::is_assignable<CVUnique_Variant, MyClass2>::value));
    ASSERT(!(std::is_assignable<CVUnique_Variant, MyClass2&>::value));

    // no best conversion to an alternative
    struct FromInt1 {
        FromInt1(int){}  // IMPLICIT
        FromInt1& operator=(int) { return *this; };
    };

    struct FromInt2 {
        FromInt2(int){}  // IMPLICIT
        FromInt2& operator=(int) { return *this; };
    };

    typedef bsl::variant<FromInt1, FromInt2> Variant_FI1_FI2;
    ASSERT(!(std::is_assignable<Variant_FI1_FI2, int>::value));

    //deleted conversion
    struct FromInt3 {
        FromInt3(int) = delete;  // IMPLICIT
        FromInt3& operator=(int) { return *this; };
    };
    typedef bsl::variant<FromInt3> Variant_FI3;

    ASSERT(!(std::is_assignable<Variant_FI3, int>::value));

    // narrowing conversion
    struct AA_Type {
        BSLMF_NESTED_TRAIT_DECLARATION(AA_Type, bslma::UsesBslmaAllocator);
    };

    typedef bsl::variant<int, AA_Type> Variant_int_AA;

    ASSERT(!(std::is_assignable<Variant_int_AA, double>::value));

    struct FromDouble {
        FromDouble(double){}  // IMPLICIT
        FromDouble& operator=(double) { return *this; }
    };
    bsl::variant<int, FromDouble> obj{};
    obj = 0.0;
    ASSERT(obj.index() == 1);

#endif  // U_VARIANT_FULL_IMPLEMENTATION
}

void testCase11d()
{
    // --------------------------------------------------------------------
    // TESTING `operator=(ANY_TYPE)`
    //   This test will ensure that the `operator=(rhs)`, where either `rhs`
    //   or `*this` is a `valueluess_by_exception` object, works as expected.
    //
    // Concerns:
    // 1. That `operator=(rhs)`, where `rhs` is a `valueluess_by_exception`
    //    `variant` object, results in a `valueluess_by_exception` target
    //    `variant` object .
    //
    // 2. That `operator=(rhs)`, invoked on `valueluess_by_exception` `variant`
    //    object with a non `valueluess_by_exception` `rhs` object, constructs
    //    a managed object of the same type and value as the managed object of
    //    the `rhs`'variant' object.
    //
    // 3. The behaviour is the same for copy and move assignment.
    //
    // 4. If assignment to a non empty `variant` object throws, the destination
    //    object is left in `valueluess_by_exception` state only if the
    //    assignment would have resulted in a initialization of a new
    //    managed object.
    //
    // 5. Assignment to a const `variant` object or a variant for which one
    //    of the alternative types is not assignable is not possible.
    //
    //
    // Plan:
    // 1. Create a `valueluess_by_exception` `variant` object  and assign it
    //    to a `variant` object with an active alternative `TYPE`.  Verify the
    //    destination object is `valueluess_by_exception`. [C-1]
    //
    // 2. Create a `variant` object with an active alternative `TYPE` and
    //    assign it to a `valueluess_by_exception` `variant` object.  Verify
    //    that the destination object's active type is TYPE' and that its
    //    value matches that of the `rhs` managed object [C-2]
    //
    // 3. Repeat steps 1-3 using an lvalue and rvalue.
    //    [C-3]
    //
    // 4. Using `Throws` helper class, check that an assignment that exits via
    //    an exception results in a valueless variant if the assignment
    //    requires a new managed object, and in a variant holding a value if
    //    the assignment calls the assigment operator of the managed type.
    //    [C-4]
    //
    // 5. Check that assignment to a const variant object and an assignment to
    //    a const alternative type is not possible.  Note that this test is
    //    disabled by default and needs to be manually run. [C-5]
    //
    // Testing:
    //
    // variant& operator=(ANY_TYPE&&);
    // --------------------------------------------------------------------
#ifdef BDE_BUILD_TARGET_EXC
    using bsl::swap;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("other", veryVeryVeryVerbose);

    if (veryVerbose)
        printf("\tTesting assignment to empty variant.\n");

    {
        Throws::s_should_throw = false;
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, bsl::in_place_index_t<0>());
        MyClass2 rhs(45, &ta);

        Throws::s_should_throw = true;
        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}
        Throws::s_should_throw = false;
        lhs                    = rhs;

        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(get<MyClass2>(lhs) == 45);

        Throws::s_should_throw = true;
        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}
        Throws::s_should_throw = false;

        lhs = MoveUtil::move(rhs);
        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(get<MyClass2>(lhs) == 45);
    }
    if (veryVerbose)
        printf("\tTesting assignment that throws. Different alternative type. "
               "\n");

    {
        Throws::s_should_throw = false;
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, bsl::in_place_index_t<0>());
        Throws rhs;

        Throws::s_should_throw = true;
        BSLS_TRY { lhs = rhs; }
        BSLS_CATCH(...) {}

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.get_allocator() == &oa);

        // reset the lhs
        lhs = MyClass2();
        BSLS_TRY { lhs = MoveUtil::move(rhs); }
        BSLS_CATCH(...) {}

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.get_allocator() == &oa);
    }
    if (veryVerbose)
        printf("\tTesting assignment that throws. Same alternative type. \n");
    {
        Throws::s_should_throw = false;
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, bsl::in_place_index_t<1>());
        bsl::variant<MyClass2, Throws> rhs(
            bsl::allocator_arg, &ta, bsl::in_place_index_t<1>());

        Throws::s_should_throw = true;
        BSLS_TRY { lhs = rhs; }
        BSLS_CATCH(...) {}
            // Assignment that throws does not cause the variant to become
            // empty.
        ASSERT(!lhs.valueless_by_exception());

        Throws::s_should_throw = true;
        BSLS_TRY { lhs = MoveUtil::move(rhs); }
        BSLS_CATCH(...) {}
            // Assignment that throws does not cause the variant to become
            // empty.
        ASSERT(!lhs.valueless_by_exception());
    }

#ifdef MANUAL_TEST19
    const bsl::variant<int> a;
        // check that we can't assign to a const variant a = 4;

    bsl::variant<MyClass2, const int> b;
        // check that we can't assign to an alternative type which is not
        // assignable.
    b = 3;
#endif

#endif  // BDE_BUILD_TARGET_EXC
}

void testCase11c()
{
    // --------------------------------------------------------------------
    // TESTING `operator=(variant_type)`
    //   This test will ensure that the `operator=(rhs)`, where either `rhs`
    //   or `*this` is a `valueluess_by_exception` object, works as expected.
    //
    // Concerns:
    // 1. That `operator=(rhs)`, where `rhs` is a `valueluess_by_exception`
    //    `variant` object, results in a `valueluess_by_exception` target
    //    `variant` object .
    //
    // 2. That `operator=(rhs)`, invoked on `valueluess_by_exception` `variant`
    //    object with a non `valueluess_by_exception` `rhs` object, constructs
    //    a managed object of the same type and value as the managed object of
    //    the `rhs`'variant' object.
    //
    // 3. The behaviour is the same for copy and move construction.
    //
    // 4. If assignment to a non empty `variant` object throws, the destination
    //    object is left in `valueluess_by_exception` state only if the
    //    assignment would have resulted in a initialization of a new
    //    managed object.
    //
    // 5. Assignment to a const `variant` object or a variant for which one
    //    of the alternative types is not assignable is not possible.
    //
    //
    // Plan:
    // 1. Create a `valueluess_by_exception` `variant` object  and assign it
    //    to a `variant` object with an active alternative `TYPE`.  Verify the
    //    destination object is `valueluess_by_exception`. [C-1]
    //
    // 2. Create a `variant` object with an active alternative `TYPE` and
    //    assign it to a `valueluess_by_exception` `variant` object.  Verify
    //    that the destination object's active type is TYPE' and that its
    //    value matches that of the `rhs` managed object [C-2]
    //
    // 3. Repeat steps 1-2 using an rvalue. [C-3]
    //
    // 5. Using `Throws` helper class, check that an assignment that exits via
    //    an exception results in a valueless variant if the assignment
    //    requires a new managed object, and in a variant holding a value if
    //    the assignment calls the assigment operator of the managed type.
    //    [C-4]
    //
    // 6. Check that assignment to a const variant object and an assignment to
    //    a variant with a const alternative type is not possible.  Note that
    //    this test is disabled by default and needs to be manually run. [C-5]
    //
    // Testing:
    //
    // variant& operator=(const variant&); variant& operator=(variant&&);
    // --------------------------------------------------------------------
#ifdef BDE_BUILD_TARGET_EXC

    using bsl::swap;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("other", veryVeryVeryVerbose);

    if (veryVerbose)
        printf("\tTesting assignment from empty variant.\n");

    {
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, MyClass2(45));
        bsl::variant<MyClass2, Throws> rhs(bsl::allocator_arg, &ta);

        Throws::s_should_throw = true;
        BSLS_TRY { rhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        int numDestructorInvocations = MyClass2::s_destructorInvocations;
        lhs                          = rhs;

        ASSERT(MyClass2::s_destructorInvocations ==
               numDestructorInvocations + 1);
        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(lhs.get_allocator() == &oa);

        lhs.emplace<MyClass2>();
        numDestructorInvocations = MyClass2::s_destructorInvocations;
        lhs                      = MoveUtil::move(rhs);

        ASSERT(MyClass2::s_destructorInvocations ==
               numDestructorInvocations + 1);
        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(lhs.get_allocator() == &oa);
    }
    if (veryVerbose)
        printf("\tTesting assignment to empty variant.\n");

    {
        bsl::variant<MyClass2, Throws> lhs(bsl::allocator_arg, &oa);
        bsl::variant<MyClass2, Throws> rhs(
            bsl::allocator_arg, &ta, MyClass2(45));

        Throws::s_should_throw = true;
        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        lhs = rhs;

        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(get<MyClass2>(lhs) == 45);
        ASSERT(lhs.get_allocator() == &oa);

        Throws::s_should_throw = true;
        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        lhs = MoveUtil::move(rhs);
        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(get<MyClass2>(lhs) == 45);
        ASSERT(lhs.get_allocator() == &oa);
    }
    if (veryVerbose)
        printf("\tTesting assignment that throws. Different alternative "
               "types. \n");

    {
        Throws::s_should_throw = false;
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, bsl::in_place_index_t<0>());
        bsl::variant<MyClass2, Throws> rhs(
            bsl::allocator_arg, &ta, bsl::in_place_index_t<1>());

        Throws::s_should_throw = true;
        BSLS_TRY { lhs = rhs; }
        BSLS_CATCH(...) {}

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.get_allocator() == &oa);

        // reset the lhs
        lhs = MyClass2();
        BSLS_TRY { lhs = MoveUtil::move(rhs); }
        BSLS_CATCH(...) {}

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.get_allocator() == &oa);
    }
    if (veryVerbose)
        printf("\tTesting assignment that throws. Same alternative type. \n");
    {
        Throws::s_should_throw = false;
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, bsl::in_place_index_t<1>());
        bsl::variant<MyClass2, Throws> rhs(
            bsl::allocator_arg, &ta, bsl::in_place_index_t<1>());

        Throws::s_should_throw = true;
        BSLS_TRY { lhs = rhs; }
        BSLS_CATCH(...) {}
            // Assignment that throws does not cause the variant to become
            // empty.
        ASSERT(!lhs.valueless_by_exception());

        BSLS_TRY { lhs = MoveUtil::move(rhs); }
        BSLS_CATCH(...) {}
            // Assignment that throws does not cause the variant to become
            // empty.
        ASSERT(!lhs.valueless_by_exception());
    }

#ifdef MANUAL_TEST20
    const bsl::variant<int> a, b;
        // check that we can't assign to a const variant
    a = b;

    bsl::variant<long, const int> c, d;
        // check that we can't assign to a variant that has an alternative type
        // which is not assignable, even if it isn't the active alternative.
    c = d;
#endif

#endif  // BDE_BUILD_TARGET_EXC
}

template <class VARIANT, size_t t_NUM1, size_t t_NUM2>
struct testCase11b_imp;

template <class VARIANT>
struct testCase11b_imp<VARIANT, 1, 0> {
};

template <class VARIANT, size_t t_NUM1>
struct testCase11b_imp<VARIANT, t_NUM1, 0>
: testCase11b_imp<VARIANT, t_NUM1 - 1, bsl::variant_size<VARIANT>::value> {
};
template <class VARIANT, size_t t_NUM1, size_t t_NUM2>
struct testCase11b_imp : testCase11b_imp<VARIANT, t_NUM1, t_NUM2 - 1> {
    testCase11b_imp()
        // --------------------------------------------------------------------
        // TESTING `operator=(ANY_TYPE)`
        //   This test will ensure that the `operator=(rhs)`, where `rhs` is
        //   one
        //   of the alternative types, works as expected.
        //
        // Concerns:
        // 1. Calling `operator=(rhs)`, where `rhs` is of an alternative type
        //    `AltType`, results in variant object whose active alternative is
        //    `AltType` and whose value matches `rhs`.
        //
        // 2. If `variant`'s active alternative type changes, previous
        //    alternative object is destroyed.
        //
        // 3. For allocator-aware  variant types, the assignment does not
        //    change the allocator.
        //
        // 4. Assignment of rvalues uses move assignment/construction where
        //    available.
        //
        //
        // Plan:
        // 1. Create a `variant` object with the active alternative
        //    `DestAltType`. Verify that assignment from an object of
        //    alternative `SrcAltType` and from `const` qualified `SrcAltType`,
        //    results in a `variant` object having an active alternative of
        //    `SrcAltType`. [C-1]
        //
        // 2. In step 1, if `DestAltType` is the same as `SrcAltType`, verify
        //    that the relevant `DestAltType` assignment operators have been
        //    called as indicated by the type's internal counters. [C-1]
        //
        // 3. In step 1, if `DestAltType` is not the same as `SrcAltType`,
        //    verify that the `SrcAltType` alternative object was destroyed and
        //    a new alternative object of type `DestAltType` has been copy
        //    constructed as indicated by the internal counters of `SrcAltType`
        //    and `DestAltType`. [C-1][C-2]
        //
        // 4. If the `variant` type is allocator aware, check that the
        //    allocator of the variant and its (possibly new) alternative has
        //    not changed.
        //
        // 3. Repeat steps 1-4 using rvalue `rhs` and verify that `rhs` was
        //    moved from. [C-4]
        //
        // Testing:
        //
        // variant& operator=(ANY_TYPE&&);
        // --------------------------------------------------------------------

    {
        const size_t SRC_INDEX = t_NUM1 - 1;
        typedef
            typename variant_alternative<SRC_INDEX, VARIANT>::type SrcAltType;
        const size_t DEST_INDEX = t_NUM2 - 1;
        typedef typename variant_alternative<DEST_INDEX, VARIANT>::type
            DestAltType;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        typedef bslalg::ConstructorProxy<VARIANT>    VariantWithAllocator;
        typedef bslalg::ConstructorProxy<SrcAltType> AltWithAllocator;
        AltWithAllocator                             X(3, &da);
        SrcAltType&                                  source  = X.object();
        const SrcAltType&                            cSource = X.object();

        if (DEST_INDEX == SRC_INDEX) {
            TEST_ASSIGN_FROM_ALT_SAME_INDEX(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly, which will make these
            // tests fail
            TEST_ASSIGN_FROM_ALT_SAME_INDEX(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            TEST_ASSIGN_FROM_ALT_SAME_INDEX(source);
            TEST_ASSIGN_FROM_ALT_SAME_INDEX(MoveUtil::move(source));
        }
        else {
            TEST_ASSIGN_FROM_ALT_DIFF_INDEX(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly, which will make these
            // tests fail
            TEST_ASSIGN_FROM_ALT_DIFF_INDEX(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            TEST_ASSIGN_FROM_ALT_DIFF_INDEX(source);
            TEST_ASSIGN_FROM_ALT_DIFF_INDEX(MoveUtil::move(source));
        }
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase11b()
{
    testCase11b_imp<VARIANT,
                    bsl::variant_size<VARIANT>::value,
                    bsl::variant_size<VARIANT>::value>
        test;
}

template <class VARIANT, size_t t_NUM1, size_t t_NUM2>
struct testCase11a_imp;

template <class VARIANT>
struct testCase11a_imp<VARIANT, 1, 0> {
};

template <class VARIANT, size_t t_NUM1>
struct testCase11a_imp<VARIANT, t_NUM1, 0>
: testCase11a_imp<VARIANT, t_NUM1 - 1, bsl::variant_size<VARIANT>::value> {
};
template <class VARIANT, size_t t_NUM1, size_t t_NUM2>
struct testCase11a_imp : testCase11a_imp<VARIANT, t_NUM1, t_NUM2 - 1> {
    testCase11a_imp()
        // --------------------------------------------------------------------
        // TESTING `operator=(variant_type)`
        //   This test will ensure that the `operator=(rhs)`, where `rhs` is a
        //   non-empty `variant` type, works as expected.
        //
        // Concerns:
        // 1. That `operator=(rhs)`, where `rhs` is a non-empty `variant`
        // object,
        //    results in the target `variant` object holding the same
        // alternative
        //    type having the same value as that managed by the `rhs` object.
        //
        // 2. That `operator=(rhs)`, invoked on `variant` having the same
        // active
        //    alternative type as `rhs` object, assigns the value of the
        //    `rhs` managed object to the value of the target's managed object.
        //
        // 3. That `operator=(rhs)`, on `variant` having a different
        // alternative
        //    type to the `rhs` object, constructs a managed object from the
        //    managed object of the `rhs`'variant' object.
        //
        // 4. For allocator-aware types, the assignment from a `variant` object
        //    does not modify the allocator.
        //
        // 6. Assignment uses move assignment/construction where available.
        //
        //
        // Plan:
        // 1. Create a `variant` object with active alternative `A` and assign
        // it
        //    to a `variant` with active alternative B.  Verify the destination
        //    object has active alternative A of the same value as the source
        //    object.  [C-1];
        //
        // 2. If A and B are the same type, check the assignment operator of
        //    type A was called. [C-2]
        //
        // 3. If A and B are not the same type, check the destructor of B was
        //    called, and copy constructor operator of type A was called. [C-3]
        //
        // 4. In steps 1-3, if `variant` is allocator-aware, verify that the
        // test
        //    object's allocator has not been modified and that the resulting
        //    managed object uses the correct allocator. [C-5]
        //
        // 5. Repeat steps 1-4 using an const lvalue, rvalue, and const rvalue.
        //    Verify that move assignment/constructor was called as
        // appropriate.
        //    [C-6]
        //
        // Testing:
        //
        // variant& operator=(const variant&); variant& operator=(variant&&);
        // --------------------------------------------------------------------

    {
        const size_t SRC_INDEX = t_NUM1 - 1;
        typedef
            typename variant_alternative<SRC_INDEX, VARIANT>::type SrcAltType;
        const size_t DEST_INDEX = t_NUM2 - 1;
        typedef typename variant_alternative<DEST_INDEX, VARIANT>::type
            DestAltType;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;
        VariantWithAllocator X(bsl::in_place_index_t<SRC_INDEX>(), 3, &da);
        VARIANT&             source  = X.object();
        const VARIANT&       cSource = X.object();

        if (DEST_INDEX == SRC_INDEX) {
            TEST_ASSIGN_FROM_VARIANT_SAME_INDEX(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly, which will make these
            // tests fail
            TEST_ASSIGN_FROM_VARIANT_SAME_INDEX(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            TEST_ASSIGN_FROM_VARIANT_SAME_INDEX(source);
            TEST_ASSIGN_FROM_VARIANT_SAME_INDEX(MoveUtil::move(source));
        }
        else {
            TEST_ASSIGN_FROM_VARIANT_DIFF_INDEX(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly, which will make these
            // tests fail
            TEST_ASSIGN_FROM_VARIANT_DIFF_INDEX(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            TEST_ASSIGN_FROM_VARIANT_DIFF_INDEX(source);
            TEST_ASSIGN_FROM_VARIANT_DIFF_INDEX(MoveUtil::move(source));
        }
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase11a()
{
    testCase11a_imp<VARIANT,
                    bsl::variant_size<VARIANT>::value,
                    bsl::variant_size<VARIANT>::value>
        test;
}

void testCase10a()
{
    // --------------------------------------------------------------------
    // TESTING `swap` METHOD
    //
    //   This test will ensure that `swap` of two variant object of which at
    //   least one is `valueless_by_exception` works as expected.
    //
    // Concerns:
    //
    // 1. Swap of two variant objects where at least one is
    //    `valueless_by_exception` swaps the states of the variant objects.
    //
    // 2. Concerns 1 applies to member function and free function `swap`.
    //
    // 3. Free function `swap` allows for swapping AA objects that do not use
    //     the same allocator. Swap will preserve the allocators of each
    //     `variant` object.
    //
    // 4. Swap does not compile if a variant object is a constant object.
    //
    // Plan:
    // 1. Call `swap` free function with combinations of two `variant` objects
    //    where at least one is `valueless_by_exception`.  Verify the objects
    //    have swapped the states, and that values of any managed object has
    //    been preserved. [C-1]
    //
    // 2. Repeat step 1 using the `swap` member function.  [C-2]
    //
    // 3. Repeat step 1 using two AA `variant` types using different
    //    allocators. Check the allocators have not been swapped. [C-3]
    //
    // 4. Using `SwapHelper` and `SwapHelperFree` check concern 4.
    //
    //
    // Testing:
    //    void swap(variant& other);
    //    void swap(variant<Types...>&, variant<Types...>&);
    // --------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
    using bsl::swap;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("other", veryVeryVeryVerbose);

    if (veryVerbose)
        printf("\tTesting member `swap`.\n");

    {
        bsl::variant<int, Throws> lhs(45);
        bsl::variant<int, Throws> rhs;

        Throws::s_should_throw = true;
        BSLS_TRY { rhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        lhs.swap(rhs);

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(!rhs.valueless_by_exception());
        ASSERT(rhs.index() == 0);
        ASSERT(bsl::get<int>(rhs) == 45);

        lhs.swap(rhs);

        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(bsl::get<int>(lhs) == 45);
        ASSERT(rhs.valueless_by_exception());
        ASSERT(rhs.index() == bsl::variant_npos);

        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        lhs.swap(rhs);

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(rhs.valueless_by_exception());
        ASSERT(rhs.index() == bsl::variant_npos);
    }

    if (veryVerbose)
        printf("\tTesting free `swap` with same allocators .\n");
    {
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, MyClass2(45));
        bsl::variant<MyClass2, Throws> rhs(bsl::allocator_arg, &oa);

        Throws::s_should_throw = true;
        BSLS_TRY { rhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        swap(lhs, rhs);

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(!rhs.valueless_by_exception());
        ASSERT(rhs.index() == 0);
        ASSERT(bsl::get<MyClass2>(rhs) == 45);

        swap(rhs, lhs);

        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(bsl::get<MyClass2>(lhs) == 45);
        ASSERT(rhs.valueless_by_exception());
        ASSERT(rhs.index() == bsl::variant_npos);

        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        swap(lhs, rhs);

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(rhs.valueless_by_exception());
        ASSERT(rhs.index() == bsl::variant_npos);
    }
    if (veryVerbose)
        printf("\tFree `swap` with different allocators .\n");
    {
        bsl::variant<MyClass2, Throws> lhs(
            bsl::allocator_arg, &oa, MyClass2(45));
        bsl::variant<MyClass2, Throws> rhs(bsl::allocator_arg, &ta);

        Throws::s_should_throw = true;
        BSLS_TRY { rhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        swap(lhs, rhs);

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(!rhs.valueless_by_exception());
        ASSERT(lhs.get_allocator() == &oa);
        ASSERT(rhs.index() == 0);
        ASSERT(bsl::get<MyClass2>(rhs) == 45);
        ASSERT(rhs.get_allocator() == &ta);
        ASSERT(bsl::get<MyClass2>(rhs).get_allocator() == &ta);

        swap(rhs, lhs);

        ASSERT(!lhs.valueless_by_exception());
        ASSERT(lhs.index() == 0);
        ASSERT(bsl::get<MyClass2>(lhs) == 45);
        ASSERT(lhs.get_allocator() == &oa);
        ASSERT(bsl::get<MyClass2>(lhs).get_allocator() == &oa);
        ASSERT(rhs.valueless_by_exception());
        ASSERT(rhs.index() == bsl::variant_npos);
        ASSERT(rhs.get_allocator() == &ta);

        BSLS_TRY { lhs.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        swap(lhs, rhs);

        ASSERT(lhs.valueless_by_exception());
        ASSERT(lhs.index() == bsl::variant_npos);
        ASSERT(lhs.get_allocator() == &oa);
        ASSERT(rhs.valueless_by_exception());
        ASSERT(rhs.index() == bsl::variant_npos);
        ASSERT(rhs.get_allocator() == &ta);
    }

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    // negative tests for swap member function const variant object
    ASSERT( (SwapHelper<Variant_int, Variant_int>::value));
    ASSERT(!(SwapHelper<Variant_int, const Variant_int>::value));
    ASSERT(!(SwapHelper<const Variant_int, Variant_int>::value));

    // negative tests for free swap function const variant object
    ASSERT( (FreeSwapHelper<Variant_int, Variant_int>::value));
    ASSERT(!(FreeSwapHelper<Variant_int, const Variant_int>::value));
    ASSERT(!(FreeSwapHelper<const Variant_int, Variant_int>::value));

#endif  // U_VARIANT_FULL_IMPLEMENTATION
#endif  // BDE_BUILD_TARGET_EXC
}

RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(10)
{
    // --------------------------------------------------------------------
    // TESTING `swap` METHOD
    //
    //   This test will ensure that `swap` of two variant object that are not
    //   `valueless_by_exception` works as expected.
    //
    // Concerns:
    //
    // 1. Swap of two variant objects with the same alternative type swaps the
    //    the values of the alternatives
    //
    // 2. Swap of two variant objects that do not have the same alternative
    //    swaps the alternative objects.
    //
    // 3. Concerns 1 and 2 apply to member function and free function `swap`.
    //
    // 4. Free function `swap` allows for swapping AA objects that do not use
    //    the same allocator. Swap will preserve the allocators of each
    //    `variant` object.
    //
    //
    // Plan:
    // 1. Call `swap` free function with two identical alternative types and
    //    verify `value_type` `swap` has been called.  [C-1]
    //
    // 2. Call `swap` free function with two `variant` object having a
    //    different alternative type.  Verify the objects have swapped the
    //    active alternatives types, and that values of each object has been
    //    preserved. [C-2]
    //
    // 3. Repeat steps 1-2 using the `swap` member function.  [C-3]
    //
    // 4. Repeat step 1-2 using two AA `variant` objects using different
    //    allocators. Check the allocators have not been swapped. [C-4]
    //
    //
    //
    // Testing:
    //    void swap(variant& other);
    //    void swap(variant<Types...>&, variant<Types...>&);
    // --------------------------------------------------------------------

    using bsl::swap;

    const size_t LHS_INDEX = N - 1;
    typedef typename variant_alternative<LHS_INDEX, VARIANT>::type LhsAltType;
    const size_t RHS_INDEX = M - 1;
    typedef typename variant_alternative<RHS_INDEX, VARIANT>::type RhsAltType;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("other", veryVeryVeryVerbose);

    typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;
    if (veryVerbose)
        printf("\tTesting member `swap`.\n");

    {
        LhsAltType lhsAlt(15);
        RhsAltType rhsAlt(34);

        VARIANT lhsVar(bsl::in_place_index_t<LHS_INDEX>(), lhsAlt);
        VARIANT rhsVar(bsl::in_place_index_t<RHS_INDEX>(), rhsAlt);

        if (LHS_INDEX == RHS_INDEX) {
            SwapUtil<LhsAltType>::swapReset();
        }

        lhsVar.swap(rhsVar);

        ASSERT(!lhsVar.valueless_by_exception());
        ASSERT(lhsVar.index() == RHS_INDEX);
        ASSERT(bsl::get<RHS_INDEX>(lhsVar) == rhsAlt);

        ASSERT(!rhsVar.valueless_by_exception());
        ASSERT(rhsVar.index() == LHS_INDEX);
        ASSERT(bsl::get<LHS_INDEX>(rhsVar) == lhsAlt);

        if (LHS_INDEX == RHS_INDEX) {
            ASSERT(SwapUtil<LhsAltType>::swapCalled());
        }
    }
    if (veryVerbose)
        printf("\tTesting free `swap` with same allocators .\n");
    {
        LhsAltType lhsAlt(15);
        RhsAltType rhsAlt(34);

        VARIANT lhsVar(bsl::in_place_index_t<LHS_INDEX>(), lhsAlt);
        VARIANT rhsVar(bsl::in_place_index_t<RHS_INDEX>(), rhsAlt);

        if (LHS_INDEX == RHS_INDEX) {
            SwapUtil<LhsAltType>::swapReset();
        }
        swap(lhsVar, rhsVar);

        ASSERT(!lhsVar.valueless_by_exception());
        ASSERT(lhsVar.index() == RHS_INDEX);
        ASSERT(bsl::get<RHS_INDEX>(lhsVar) == rhsAlt);

        ASSERT(!rhsVar.valueless_by_exception());
        ASSERT(rhsVar.index() == LHS_INDEX);
        ASSERT(bsl::get<LHS_INDEX>(rhsVar) == lhsAlt);

        if (LHS_INDEX == RHS_INDEX) {
            ASSERT(SwapUtil<LhsAltType>::swapCalled());
        }
    }
    if (veryVerbose)
        printf("\tFree `swap` with different allocators .\n");
    {
        LhsAltType lhsAlt(15);
        RhsAltType rhsAlt(34);

        VariantWithAllocator lhsVarBuf(
            bsl::in_place_index_t<LHS_INDEX>(), lhsAlt, &oa);
        VARIANT& lhsVar = lhsVarBuf.object();

        VariantWithAllocator rhsVarBuf(
            bsl::in_place_index_t<RHS_INDEX>(), rhsAlt, &ta);
        VARIANT& rhsVar = rhsVarBuf.object();

        swap(lhsVar, rhsVar);

        ASSERT(!lhsVar.valueless_by_exception());
        ASSERT(lhsVar.index() == RHS_INDEX);
        RhsAltType& newLhsObj = bsl::get<RHS_INDEX>(lhsVar);
        ASSERT(newLhsObj == rhsAlt);
        ASSERT(checkAllocator(lhsVar, &oa));
        ASSERT(checkAllocator(newLhsObj, &oa));

        ASSERT(!rhsVar.valueless_by_exception());
        ASSERT(rhsVar.index() == LHS_INDEX);
        LhsAltType& newRhsObj = bsl::get<LHS_INDEX>(rhsVar);
        ASSERT(newRhsObj == lhsAlt);
        ASSERT(checkAllocator(rhsVar, &ta));
        ASSERT(checkAllocator(newRhsObj, &ta));
    }
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_DEST_TYPE, size_t t_NUM>
struct testCase9f_imp;

template <class t_DEST_TYPE>
struct testCase9f_imp<t_DEST_TYPE, 0> {
};

template <class t_DEST_TYPE, size_t t_NUM>
struct testCase9f_imp
: testCase9f_imp<t_DEST_TYPE, t_NUM - 1> {
    testCase9f_imp()
    {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR-EXTENDED CONSTRUCTION FROM `std::variant`
        //   This test will ensure that allocator-extended construction from an
        //   lvalue or rvalue of `std::variant` twith the same sequence of
        //   alternatives that is not `valueless_by_exception` works as
        //   expected.
        //
        // Concerns:
        // 1. Constructing a `variant` from a `std::variant` object with the
        //    same sequence of alternatives that is not
        //    `valueluess_by_exception` creates a `variant` where the active
        //    alternative type matches that of the source object, and whose
        //    managed object is constructed from the managed object of the
        //    source variant.
        //
        // 2. The supplied allocator is used as the allocator of the variant
        //    and of the constructed alternative type object.
        //
        // 3. The `value_type` object in `variant` is move constructed when the
        //    source `std::variant` is an rvalue.
        //
        // 4. No unnecessary copies of the `value_type` are created.
        //
        // Plan:
        // 1. Create a `std::variant` holding alternative object of some type
        //    `TYPE` and construct a `variant` object from an lvalue of the
        //    `std::variant` using the allocator-extended constructor.  Verify
        //    that the active alternative of the new object is `TYPE`, and that
        //    the value of its managed object matches that of the source
        //    object. [C-1]
        //
        // 2. In step 1, verify that the supplied allocator is used for the
        //    variant object and for the managed object. [C-2]
        //
        // 3. Repeat steps 1-2 using an rvalue for source.  Verify that the
        //    move constructor is invoked for alternatives of class type.
        //    [C-3]
        //
        // 4. In steps 1-3, verify that no unnecessary copies of the
        //    `value_type` have been created. [C-4]
        //
        // Testing:
        //   explicit variant(alloc_arg, alloc, t_STD_VARIANT&&);
        // --------------------------------------------------------------------

        // These typedefs are needed by the `TEST_` macros.
        using DEST_TYPE = t_DEST_TYPE;
        using SRC_TYPE  = typename BslVariantToStdVariant<t_DEST_TYPE>::type;

        const size_t INDEX = t_NUM - 1;
        using AltType      = variant_alternative_t<INDEX, t_DEST_TYPE>;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        SRC_TYPE                     source(std::in_place_index<INDEX>);

        TEST_EXT_COPY_CONSTRUCT(source);
        TEST_EXT_COPY_CONSTRUCT(bsl::as_const(source));
        TEST_EXT_MOVE_CONSTRUCT(source);
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase9f()
{
    testCase9f_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class t_DEST_TYPE, size_t t_NUM>
struct testCase9e_imp;

template <class t_DEST_TYPE>
struct testCase9e_imp<t_DEST_TYPE, 0> {
};
template <class t_DEST_TYPE, size_t t_NUM>
struct testCase9e_imp
: testCase9e_imp<t_DEST_TYPE, t_NUM - 1> {
    testCase9e_imp()
    {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTION FROM `std::variant`
        //   This test will ensure that construction from an lvalue or rvalue
        //   of `std::variant` with the same sequence of alternatives that is
        //   not `valueluess_by_exception` works as expected.
        //
        // Concerns:
        // 1. Constructing a `variant` from a `std::variant` object with the
        //    same sequence of alternatives that is not
        //    `valueluess_by_exception` creates a `variant` where the active
        //    alternative type matches that of the source object, and whose
        //    managed object is constructed from the managed object of the
        //    source variant.
        //
        // 2. If `variant` type is allocator-aware, the default allocator is
        //    used.
        //
        // 3. The `value_type` object in `variant` is move constructed when the
        //    source `std::variant` is an rvalue.
        //
        // 4. No unnecessary copies of the `value_type` are created.
        //
        // Plan:
        // 1. Create a `std::variant` holding alternative object of some type
        //    `TYPE` and construct a `variant` object from an lvalue of the
        //    `std::variant`.  Verify that the active alternative of the new
        //    object is `TYPE`, and that the value of its managed object
        //    matches that of the source object.  [C-1]
        //
        // 2. In step 1, if the `variant` is allocator-aware, verify that the
        //    currently installed default allocator is used.  [C-2]
        //
        // 3. Repeat steps 1-2 using an rvalue for source.  Verify that the
        //    move constructor is invoked for alternatives of class type.
        //    [C-3]
        //
        // 4. In steps 1-3, verify that no unnecessary copies of the
        //    `value_type` have been created.  [C-4]
        //
        // Testing:
        //   explicit variant(t_STD_VARIANT&&);
        // --------------------------------------------------------------------

        // These typedefs are needed by the `TEST_` macros.
        using DEST_TYPE = t_DEST_TYPE;
        using SRC_TYPE  = typename BslVariantToStdVariant<t_DEST_TYPE>::type;

        const size_t INDEX = t_NUM - 1;
        using AltType      = variant_alternative_t<INDEX, t_DEST_TYPE>;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        SRC_TYPE                     source(std::in_place_index<INDEX>);

        // Modify the source `std::variant` such that its active alternative
        // has `&oa` as its allocator if allocator-aware (to ensure that the
        // `bsl::variant` doesn't get its allocator from the source
        // alternative).
        AltType& sourceAlternative = std::get<INDEX>(source);
        sourceAlternative.~AltType();
        bslma::ConstructionUtil::construct(&sourceAlternative, &oa);

        TEST_COPY_CONSTRUCT(source);
        TEST_COPY_CONSTRUCT(bsl::as_const(source));
        TEST_MOVE_CONSTRUCT(source, false);
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase9e()
{
    testCase9e_imp<VARIANT,
                   bsl::variant_size<VARIANT>::value> test;
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

void testCase9d()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTIBLE AND CONVERTIBLE TRAITS
    //   This test will ensure that the copy/move construction is disabled
    //   under certain conditions.
    //
    // Concerns:
    // 1. That the variant with an alternative that is not copyable is also
    //    not copyable.  Similarly, if any alternative is moveable, but not
    //    copyable, the variant is movable, but not copyable.
    //
    // 2. That the same rules apply to allocator extended constructors.
    //
    // 3. That the same rules apply to direct-initialization of `bsl::variant`
    //    from `std::variant`.
    //
    // 4. That `bsl::variant` is not convertible from `std::variant` (the
    //    constructor is explicit).
    //
    // Plan:
    // 1. For the concerns above, check `is_constructible` and `is_convertible`
    //    traits with the appropriate set of arguments.  (C-1..4)
    //
    // Testing:
    //   variant(const variant&);
    //   variant(variant&&);
    //   explicit variant(t_STD_VARIANT&&);
    //   variant(alloc_arg, alloc, const variant&);
    //   variant(alloc_arg, alloc, variant&&);
    //   explicit variant(alloc_arg, alloc, t_STD_VARIANT&&);

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    struct NonCopyable {
        NonCopyable(const NonCopyable&) = delete;
    };

    struct MoveOnly {
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&)      = default;

        MoveOnly& operator=(const MoveOnly&) = delete;
    };

    typedef bsl::variant<MyClass2, NonCopyable> NonCopy_Variant;
    typedef bsl::variant<MyClass2, MoveOnly>    MoveOnly_Variant;
    typedef bsl::variant<MyClass2, int>         CopyMove_Variant;

    ASSERT(!(std::is_copy_constructible<NonCopy_Variant>::value));
    ASSERT(!(std::is_copy_constructible<MoveOnly_Variant>::value));
    ASSERT( (std::is_copy_constructible<CopyMove_Variant>::value));

    ASSERT(!(std::is_move_constructible<NonCopy_Variant>::value));
    ASSERT( (std::is_move_constructible<MoveOnly_Variant>::value));
    ASSERT( (std::is_move_constructible<CopyMove_Variant>::value));

    // allocator extended copy/move
    ASSERT(!(std::is_constructible<NonCopy_Variant,
                                   allocator_arg_t,
                                   NonCopy_Variant::allocator_type,
                                   const NonCopy_Variant&>::value));
    ASSERT(!(std::is_constructible<NonCopy_Variant,
                                   allocator_arg_t,
                                   NonCopy_Variant::allocator_type,
                                   NonCopy_Variant>::value));
    ASSERT(!(std::is_constructible<MoveOnly_Variant,
                                   allocator_arg_t,
                                   MoveOnly_Variant::allocator_type,
                                   const MoveOnly_Variant&>::value));
    ASSERT( (std::is_constructible<MoveOnly_Variant,
                                   allocator_arg_t,
                                   MoveOnly_Variant::allocator_type,
                                   MoveOnly_Variant>::value));
    ASSERT( (std::is_constructible<CopyMove_Variant,
                                   allocator_arg_t,
                                   CopyMove_Variant::allocator_type,
                                   const CopyMove_Variant&>::value));
    ASSERT( (std::is_constructible<CopyMove_Variant,
                                   allocator_arg_t,
                                   CopyMove_Variant::allocator_type,
                                   CopyMove_Variant>::value));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // copy/move from `std::variant`
    using NonCopy_StdVariant =
                             BslVariantToStdVariant<NonCopy_Variant>::type;
    using MoveOnly_StdVariant =
                             BslVariantToStdVariant<MoveOnly_Variant>::type;
    using CopyMove_StdVariant =
                             BslVariantToStdVariant<CopyMove_Variant>::type;

    ASSERT(!(std::is_constructible_v<NonCopy_Variant,
                                     const NonCopy_StdVariant&>));
    ASSERT(!(std::is_constructible_v<NonCopy_Variant,
                                     NonCopy_StdVariant>));

    ASSERT(!(std::is_constructible_v<     MoveOnly_Variant,
                                          const MoveOnly_StdVariant&>));
    ASSERT((isOnlyExplicitlyConstructible<MoveOnly_Variant,
                                          MoveOnly_StdVariant>));

    ASSERT((isOnlyExplicitlyConstructible<CopyMove_Variant,
                                          const CopyMove_StdVariant&>));
    ASSERT((isOnlyExplicitlyConstructible<CopyMove_Variant,
                                          CopyMove_StdVariant>));

    ASSERT(!(std::is_constructible_v<NonCopy_Variant,
                                     allocator_arg_t,
                                     NonCopy_Variant::allocator_type,
                                     const NonCopy_StdVariant&>));
    ASSERT(!(std::is_constructible_v<NonCopy_Variant,
                                     allocator_arg_t,
                                     NonCopy_Variant::allocator_type,
                                     NonCopy_StdVariant>));

    ASSERT(!(std::is_constructible_v<     MoveOnly_Variant,
                                          allocator_arg_t,
                                          MoveOnly_Variant::allocator_type,
                                          const MoveOnly_StdVariant&>));
    ASSERT((isOnlyExplicitlyConstructible<MoveOnly_Variant,
                                          allocator_arg_t,
                                          MoveOnly_Variant::allocator_type,
                                          MoveOnly_StdVariant>));

    ASSERT((isOnlyExplicitlyConstructible<CopyMove_Variant,
                                          allocator_arg_t,
                                          CopyMove_Variant::allocator_type,
                                          const CopyMove_StdVariant&>));
    ASSERT((isOnlyExplicitlyConstructible<CopyMove_Variant,
                                          allocator_arg_t,
                                          CopyMove_Variant::allocator_type,
                                          CopyMove_StdVariant>));
    // allocator-extended copy/move from `std::variant`
#endif
#endif  // U_VARIANT_FULL_IMPLEMENTATION
}

void testCase9c()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTION FROM AN EMPTY VARIANT
    //
    //   This test will ensure that the construction from a
    //   `valueless_by_exception` variant works as expected. This test only
    //   makes sense if exceptions are enabled.  In C++17, this test also
    //   includes a case where the source object is a `std::variant` with the
    //   same sequence of alternatives.
    //
    // Concerns:
    // 1. Constructing a `variant` from a `variant` object that is
    //    `valueluess_by_exception` creates a `variant`  object that is
    //    `valueluess_by_exception`.
    //
    // 2. If `variant` is allocator aware, and no allocator is provided, the
    //    allocator is propagated if the source is a non-const rvalue.
    // Otherwise
    //    default allocator is used for the new `variant` object.
    //
    // 3. If allocator extended constructor is used, the supplied allocator is
    //    used as the allocator for the new `variant` object.
    //
    // 4. In C++17 and later, constructing a `variant` from a `std::variant`
    //    object with the same sequence of alternatives that is valueless by
    //    exception produces an object that is valueless by exception and uses
    //    the default allocator or the supplied allocator.
    //
    // Plan:
    // 1. Create a `variant` source object, and using `Throws`
    //    helper class put it in the `valueless_by_exception` state.  Copy
    //    construct a new `variant` object from the source object and verify
    // that
    //    the new object is `valueless_by_exception`. [C-1]
    //
    // 2. In step1, if the `variant` is allocator aware, check the correct
    //    allocator is used for the new `variant` object.  Repeat for rvalue
    // and
    //    const rvalue source object.  [C-2]
    //
    // 3. Repeat step 1 for an allocator aware variant using an allocator
    //    extended constructor.  Verify the supplied allocator is used for the
    //    new `variant` object.  [C-3]
    //
    // 4. Repeat steps 1-3 with a `std::variant` as the source object.  [C-4]
    //
    // Testing:
    //   variant(const variant&);
    //   variant(variant&&);
    //   explicit variant(t_STD_VARIANT&&);
    //   variant(alloc_arg, alloc, const variant&);
    //   variant(alloc_arg, alloc, variant&&);
    //   explicit variant(alloc_arg, alloc, t_STD_VARIANT&&);

#ifdef BDE_BUILD_TARGET_EXC
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    {
        bsl::variant<int, Throws> source;
        Throws::s_should_throw = true;
        BSLS_TRY { source.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        bsl::variant<int, Throws> obj(source);
        ASSERT(obj.valueless_by_exception());
        ASSERT(obj.index() == bsl::variant_npos);

        bsl::variant<int, Throws> obj2(MoveUtil::move(source));
        ASSERT(obj2.valueless_by_exception());
        ASSERT(obj2.index() == bsl::variant_npos);
    }
    {
        bsl::variant<MyClass2, Throws> source(bsl::allocator_arg, &oa);
        Throws::s_should_throw = true;
        BSLS_TRY { source.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        bsl::variant<MyClass2, Throws> obj(source);
        ASSERT(obj.valueless_by_exception());
        ASSERT(obj.index() == bsl::variant_npos);
        ASSERT(obj.get_allocator() == &da);

        bsl::variant<MyClass2, Throws> obj2(MoveUtil::move(source));
        ASSERT(obj2.valueless_by_exception());
        ASSERT(obj2.index() == bsl::variant_npos);
        ASSERT(obj2.get_allocator() == &oa);

        bsl::variant<MyClass2, Throws> obj3(bsl::allocator_arg, &ta, source);
        ASSERT(obj3.valueless_by_exception());
        ASSERT(obj3.index() == bsl::variant_npos);
        ASSERT(obj3.get_allocator() == &ta);

        bsl::variant<MyClass2, Throws> obj4(
            bsl::allocator_arg, &ta, MoveUtil::move(source));
        ASSERT(obj4.valueless_by_exception());
        ASSERT(obj4.index() == bsl::variant_npos);
        ASSERT(obj4.get_allocator() == &ta);
    }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    {
        std::variant<int, Throws> source;
        Throws::s_should_throw = true;
        BSLS_TRY { source.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        bsl::variant<int, Throws> obj(source);
        ASSERT(obj.valueless_by_exception());
        ASSERT(obj.index() == bsl::variant_npos);

        bsl::variant<int, Throws> obj2(std::move(source));
        ASSERT(obj2.valueless_by_exception());
        ASSERT(obj2.index() == bsl::variant_npos);
    }
    {
        std::variant<MyClass2, Throws> source(std::in_place_index<0>, &oa);
        Throws::s_should_throw = true;
        BSLS_TRY { source.emplace<Throws>(); }
        BSLS_CATCH(...) {}

        bsl::variant<MyClass2, Throws> obj(source);
        ASSERT(obj.valueless_by_exception());
        ASSERT(obj.index() == bsl::variant_npos);
        ASSERT(obj.get_allocator() == &da);

        bsl::variant<MyClass2, Throws> obj2(std::move(source));
        ASSERT(obj2.valueless_by_exception());
        ASSERT(obj2.index() == bsl::variant_npos);
        ASSERT(obj2.get_allocator() == &da);

        bsl::variant<MyClass2, Throws> obj3(bsl::allocator_arg, &ta, source);
        ASSERT(obj3.valueless_by_exception());
        ASSERT(obj3.index() == bsl::variant_npos);
        ASSERT(obj3.get_allocator() == &ta);

        bsl::variant<MyClass2, Throws> obj4(bsl::allocator_arg,
                                            &ta,
                                            std::move(source));
        ASSERT(obj4.valueless_by_exception());
        ASSERT(obj4.index() == bsl::variant_npos);
        ASSERT(obj4.get_allocator() == &ta);
    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#endif  // BDE_BUILD_TARGET_EXC
}

template <class  DEST_TYPE,
          class  SRC_TYPE,
          bool   PROPAGATE_ON_MOVE,
          size_t t_NUM>
struct testCase9b_imp;

template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
struct testCase9b_imp<DEST_TYPE, SRC_TYPE, PROPAGATE_ON_MOVE, 0> {
};

template <class  DEST_TYPE,
          class  SRC_TYPE,
          bool   PROPAGATE_ON_MOVE,
          size_t t_NUM>
struct testCase9b_imp
: testCase9b_imp<DEST_TYPE, SRC_TYPE, PROPAGATE_ON_MOVE, t_NUM - 1> {
    testCase9b_imp()
    {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION
        //
        //   This test will ensure that the allocator extended copy
        //   construction
        //   from a `variant` object that is not `valueluess_by_exception`
        //   works as
        //   expected.
        //
        // Concerns:
        // 1. Constructing a `variant` from a `variant` object that is not
        //    `valueluess_by_exception` creates a `variant` where the active
        //    alternative type matches that of the
        //    source object, and whose managed object is constructed from
        //    the managed object of the source variant.
        //
        // 2. The supplied allocator is used as the allocator of the variant
        // and
        //    of the constructed alternative type object.
        //
        // 3. The `value_type` object in `variant` is move constructed when
        //    possible.
        //
        // 4. No unnecessary copies of the `value_type` are created.
        //
        //
        // Plan:
        // 1. Create an `variant` holding alternative object of some type
        // `TYPE`
        //    and use it to create a `variant` object.  Verify that the active
        //    alternative of the new object is `TYPE`, and that the value of
        //    its
        //    managed object matches that of the source object. [C-1]
        //
        // 2. In step 1, verify that the supplied allocator is used for the
        //    variant object and for the managed object. [C-2]
        //
        // 3. Repeat steps 1-2 using an rvalue for source. Verify that the move
        //    constructor is invoked where necessary.  [C-3]
        //
        // 4. In steps 1-3, verify that no unnecessary copies of the
        // `value_type`
        //    have been created. [C-4]
        //
        //
        // Testing:
        //
        //      variant(alloc_arg, alloc, const variant&);
        //      variant(alloc_arg, alloc, variant&&);
        // --------------------------------------------------------------------

        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, DEST_TYPE>::type AltType;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bslalg::ConstructorProxy<SRC_TYPE> SourceWithAllocator;
            SourceWithAllocator sourceBuffer(bsl::in_place_index_t<INDEX>(),
                                             &oa);
            SRC_TYPE&           source  = sourceBuffer.object();
            const SRC_TYPE&     csource = sourceBuffer.object();

            TEST_EXT_COPY_CONSTRUCT(source);
            TEST_EXT_COPY_CONSTRUCT(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly, which will make these
            // tests fail
            TEST_EXT_COPY_CONSTRUCT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_EXT_MOVE_CONSTRUCT(source);
        }
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase9b()
{
    testCase9b_imp<VARIANT, VARIANT, true, bsl::variant_size<VARIANT>::value>
        test1;
    testCase9b_imp<VARIANT,
                   const VARIANT,
                   false,
                   bsl::variant_size<VARIANT>::value>
        test2;
}

template <class  DEST_TYPE,
          class  SRC_TYPE,
          bool   PROPAGATE_ON_MOVE,
          size_t t_NUM>
struct testCase9a_imp;

template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
struct testCase9a_imp<DEST_TYPE, SRC_TYPE, PROPAGATE_ON_MOVE, 0> {
};
template <class  DEST_TYPE,
          class  SRC_TYPE,
          bool   PROPAGATE_ON_MOVE,
          size_t t_NUM>
struct testCase9a_imp
: testCase9a_imp<DEST_TYPE, SRC_TYPE, PROPAGATE_ON_MOVE, t_NUM - 1> {
    testCase9a_imp()
    {
        // --------------------------------------------------------------------
        // TESTING COPY/MOVE CONSTRUCTION
        //   This test will ensure that the copy construction from a
        //   `variant` object that is not `valueluess_by_exception` works as
        //   expected.
        //
        // Concerns:
        // 1. Constructing a `variant` from a `variant` object that is not
        //    `valueluess_by_exception` creates a `variant` where the active
        //    alternative type matches that of the
        //    source object, and whose managed object is constructed from
        //    the managed object of the source variant.
        //
        // 2. If `variant` type is allocator-aware, and the source is a
        // non-const
        //    rvalue, the allocator is propagated.  Otherwise, the default
        //    allocator is used.
        //
        // 3. The `value_type` object in `variant` is move constructed when
        //    possible.
        //
        // 4. No unnecessary copies of the `value_type` are created.
        //
        //
        // Plan:
        // 1. Create an `variant` holding alternative object of some type
        // `TYPE`
        //    and use it to create a `variant` object.  Verify that the active
        //    alternative of the new object is `TYPE`, and that the value of
        //    its
        //    managed object matches that of the source object. [C-1]
        //
        // 2. In step 1, if the `variant` is allocator-aware, verify that the
        //    allocator is propagated as needed. [C-2]
        //
        // 3. Repeat steps 1-2 using an rvalue for source. Verify that the move
        //    constructor is invoked where necessary.  [C-3]
        //
        // 4. In steps 1-3, verify that no unnecessary copies of the
        // `value_type`
        //    have been created. [C-4]
        //
        //
        // Testing:
        //
        //    variant(const variant&);
        //    variant(variant&&);
        // --------------------------------------------------------------------

        {
            const size_t INDEX = t_NUM - 1;
            // DEST_TYPE and SRC_TYPE are same types, but DEST_TYPE may be
            // const qualified to test for construction from a const object.
            typedef
                typename variant_alternative<INDEX, DEST_TYPE>::type AltType;
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            {
                typedef bslalg::ConstructorProxy<SRC_TYPE> SourceWithAllocator;
                SourceWithAllocator sourceBuffer(
                    bsl::in_place_index_t<INDEX>(), &oa);
                SRC_TYPE&       source  = sourceBuffer.object();
                const SRC_TYPE& csource = sourceBuffer.object();

                TEST_COPY_CONSTRUCT(source);
                TEST_COPY_CONSTRUCT(csource);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                // C++03 MovableRef isn't const friendly, which will make these
                // tests fail
                TEST_COPY_CONSTRUCT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

                TEST_MOVE_CONSTRUCT(source, PROPAGATE_ON_MOVE);
            }
        }
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase9a()
{
    testCase9a_imp<VARIANT, VARIANT, true, bsl::variant_size<VARIANT>::value>
        test1;
    testCase9a_imp<VARIANT,
                   const VARIANT,
                   false,
                   bsl::variant_size<VARIANT>::value>
        test2;
}

void testCase8()
{
    // Concerns:
    // 1. Two `variant` objects can be compared if every one of their
    //    alternative types is comparable. The result of two `variant` objects
    //    having the same active alternative type is the result of comparing
    //    the alternative objects. Otherwise, the result of the comparison
    //    depends on the value of `index()` of the two objects.
    //
    // 2. That each comparsion operation requires only that specific comparison
    //    to exist for each alternative type.
    //
    //
    // Plan:
    // 1. For each relation operator, compare two `variant` objects containing
    //    the same alternative type. [C-1]
    //
    // 2. For each relation operator, compare two `variant` objects containing
    //    a different alternative type. [C-1]
    //
    // 3. For each relation operator, compare a `variant` object containing
    //    an alternative type object and a `valueless_by_exception` variant
    //    object. [C-1]
    //
    // 4. For each relation operator, compare two `valueless_by_exception`
    //    `variant` objects. [C-1]
    //
    // 5. For each relation operator, check that the relevant comparison is
    //    possible for a variant containing an alternative that only supports
    //    that specific relation operator. [C-2]
    //
    // Testing:
    //
    //  bool operator==(const variant<Types...>&, const variant<Types...>&);
    //  bool operator!=(const variant<Types...>&, const variant<Types...>&);
    //  bool operator<(const variant<Types...>&, const variant<Types...>&);
    //  bool operator>(const variant<Types...>&, const variant<Types...>&);
    //  bool operator<=(const variant<Types...>&, const variant<Types...>&);
    //  bool operator>=(const variant<Types...>&, const variant<Types...>&);
    // --------------------------------------------------------------------

    if (veryVerbose)
        printf("\tSame alternative.\n");
    {
        bsl::variant<int, char, const int> X(bsl::in_place_index_t<2>(), 6);
        bsl::variant<int, char, const int> Y(bsl::in_place_index_t<2>(), 7);

        ASSERT(!(X == Y));
        ASSERT((X != Y));
        ASSERT((X < Y));
        ASSERT(!(X > Y));
        ASSERT((X <= Y));
        ASSERT(!(X >= Y));

        ASSERT(!(Y == X));
        ASSERT((Y != X));
        ASSERT(!(Y < X));
        ASSERT((Y > X));
        ASSERT(!(Y <= X));
        ASSERT((Y >= X));
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(  X <=> Y <  0 );
        ASSERT(!(X <=> Y >  0));
        ASSERT(  X <=> Y <= 0 );
        ASSERT(!(X <=> Y >= 0));
#endif

        X.emplace<0>(5);
        Y.emplace<0>(5);

        ASSERT((X == Y));
        ASSERT(!(X != Y));
        ASSERT(!(X < Y));
        ASSERT(!(X > Y));
        ASSERT((X <= Y));
        ASSERT((X >= Y));

        ASSERT((Y == X));
        ASSERT(!(Y != X));
        ASSERT(!(Y < X));
        ASSERT(!(Y > X));
        ASSERT((Y <= X));
        ASSERT((Y >= X));
    }
    if (veryVerbose)
        printf("\tdifferent alternative.\n");
    {
        bsl::variant<int, char, const int> X(bsl::in_place_index_t<2>(), 6);
        bsl::variant<int, char, const int> Y(bsl::in_place_index_t<0>(), 7);

        ASSERT(!(X == Y));
        ASSERT((X != Y));
        ASSERT(!(X < Y));
        ASSERT((X > Y));
        ASSERT(!(X <= Y));
        ASSERT((X >= Y));

        ASSERT(!(Y == X));
        ASSERT((Y != X));
        ASSERT((Y < X));
        ASSERT(!(Y > X));
        ASSERT((Y <= X));
        ASSERT(!(Y >= X));
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(!(X <=> Y <  0));
        ASSERT(  X <=> Y >  0 );
        ASSERT(!(X <=> Y <= 0));
        ASSERT(  X <=> Y >= 0 );
#endif
    }

    if (veryVerbose)
        printf("\tdifferent alternative, but same value \n");
    {
        bsl::variant<int, char, const int> X(bsl::in_place_index_t<2>(), 6);
        bsl::variant<int, char, const int> Y(bsl::in_place_index_t<0>(), 6);

        ASSERT(!(X == Y));
        ASSERT((X != Y));
        ASSERT(!(X < Y));
        ASSERT((X > Y));
        ASSERT(!(X <= Y));
        ASSERT((X >= Y));

        ASSERT(!(Y == X));
        ASSERT((Y != X));
        ASSERT((Y < X));
        ASSERT(!(Y > X));
        ASSERT((Y <= X));
        ASSERT(!(Y >= X));
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT(!(X <=> Y == 0));
        ASSERT(  X <=> Y != 0 );
        ASSERT(!(X <=> Y <  0));
        ASSERT(  X <=> Y >  0 );
        ASSERT(!(X <=> Y <= 0));
        ASSERT(  X <=> Y >= 0 );
#endif
    }
#ifdef BDE_BUILD_TARGET_EXC
    if (veryVerbose)
        printf("\tone variant is empty.\n");
    {
        Throws::s_should_throw = false;
        bsl::variant<Throws, int, int> X;
        bsl::variant<Throws, int, int> Y;

        Throws::s_should_throw = true;
        BSLS_TRY { X.emplace<0>(1); }
        BSLS_CATCH(...) {}

        ASSERT(!(X == Y));
        ASSERT((X != Y));
        ASSERT((X < Y));
        ASSERT(!(X > Y));
        ASSERT((X <= Y));
        ASSERT(!(X >= Y));

        ASSERT(!(Y == X));
        ASSERT((Y != X));
        ASSERT(!(Y < X));
        ASSERT((Y > X));
        ASSERT(!(Y <= X));
        ASSERT((Y >= X));
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT((X <=> Y == std::strong_ordering::less));
        ASSERT((Y <=> X == std::strong_ordering::greater));
#endif
    }
    if (veryVerbose)
        printf("\tboth variants are empty.\n");
    {
        Throws::s_should_throw = false;
        bsl::variant<Throws, int, int> X;
        bsl::variant<Throws, int, int> Y;

        Throws::s_should_throw = true;
        BSLS_TRY { X.emplace<0>(1); }
        BSLS_CATCH(...) {}
        BSLS_TRY { Y.emplace<0>(1); }
        BSLS_CATCH(...) {}

        ASSERT((X == Y));
        ASSERT(!(X != Y));
        ASSERT(!(X < Y));
        ASSERT(!(X > Y));
        ASSERT((X <= Y));
        ASSERT((X >= Y));
#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT(((X <=> Y) == std::strong_ordering::equal));
#endif

    }
#endif  // BDE_BUILD_TARGET_EXC

    {
        bsl::variant<EqualTestType> a, b;
        ASSERT((a == b));
    }
    {
        bsl::variant<UnequalTestType> a, b;
        ASSERT((a != b));
    }
    {
        bsl::variant<LessThanTestType> a, b;
        ASSERT((a < b));
    }
    {
        bsl::variant<GreaterThanTestType> a, b;
        ASSERT((a > b));
    }
    {
        bsl::variant<LessOrEqualTestType> a, b;
        ASSERT((a <= b));
    }
    {
        bsl::variant<int, GreaterOrEqualTestType> a, b;
        ASSERT((a >= b));
    }
}

void testCase7d()
{
    // --------------------------------------------------------------------
    // TESTING `emplace` CONSTRAINS
    //   This test will ensure that the `emplace` method is constrained as
    //   expected.
    //
    // Concerns:
    // 1. That `emplace` can not be called with an invalid alternative.
    //
    // 2. That `emplace` can not be called with an alternative type if that
    //    type is not unique alternative type.
    //
    // 3. That `emplace` can be called with an alternative type if that type is
    //    a unique alternative type, but there exists another alternative of
    //    same type with differentcv-qualifications.
    //
    // 4. That `emplace` can not be called with arguments from which the
    //     specified alternative is not constructible.
    //
    // Plan:
    // 1. Create a `variant` object and using the emplace helper class
    //    call `emplace` method with a type and arguments to test the specified
    //    concerns

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    // invalid type or id
    ASSERT(!(EmplaceHelperType<Variant_size_t_char, MyClass1>::check(1)));
    ASSERT((EmplaceHelperType<Variant_size_t_char, size_t>::check(1)));
    ASSERT(!(EmplaceHelperType<Variant_MyClass1_MyClass1, int>::check(1)));

    // non unique alternative
    ASSERT(
        !(EmplaceHelperType<Variant_MyClass1_MyClass1, MyClass1>::check(1)));
    ASSERT(
     (EmplaceHelperType<Variant_MyClass1_const_MyClass1, MyClass1>::check(1)));
    ASSERT((EmplaceHelperType<Variant_MyClass1_const_MyClass1,
                              const MyClass1>::check(1)));

    // not constructible from
    ASSERT(
        !(EmplaceHelperType<Variant_size_t_char, char, MyClass1>::check(1)));
    ASSERT(!(EmplaceHelperType<Variant_MyClass1_const_MyClass1,
                               MyClass1,
                               MyClass1,
                               MyClass1>::check(1)));

    // initializer list emplace overload
    ASSERT(!(EmplaceHelperType<Variant_CTTAlloc,
                               MyClass1,
                               std::initializer_list<int> >::check(1)));

    // non unique alternative
    ASSERT(!(EmplaceHelperType<Variant_CTTAlloc_CTTAlloc,
                               ConstructTestTypeAlloc,
                               std::initializer_list<int> >::check(1)));
    ASSERT((EmplaceHelperType<Variant_const_CTTAlloc_CTTAlloc,
                              ConstructTestTypeAlloc,
                              std::initializer_list<int> >::check(1)));
    ASSERT((EmplaceHelperType<Variant_const_CTTAlloc_CTTAlloc,
                              const ConstructTestTypeAlloc,
                              std::initializer_list<int> >::check(1)));

    // not constructible from
    ASSERT(!(EmplaceHelperType<Variant_CTTAlloc_CTTAlloc,
                               ConstructTestTypeAlloc,
                               std::initializer_list<int>,
                               MyClass1>::check(1)));
#endif  // U_VARIANT_FULL_IMPLEMENTATION
}

template <class TYPE>
void testCase7c()
{
    // --------------------------------------------------------------------
    // TESTING `emplace` METHOD
    //   This test will ensure that the `emplace` method works as expected.
    //
    // Concerns:
    // 1. If the constructor called from any `emplace` method throws, the
    //    `variant` object is left in a `valueluess by exception` state.
    //
    // Plan:
    // 1. Create a `variant` object and using the `Throws` helper class
    //    call `emplace` method specifying the type to create and no arguments.
    //    Verify the `variant` object is left in a `valueluess by exception`
    //    state after the exception is thrown. [C-1]
    //
    // 2. Repeat step 1 using varying number of arguments [C-1]
    //
    // 3. Repeat steps 1-2 with the additional `initializer_list` argument.
    // [C-1]
    //
    // 4. Repeat steps 1-3 with index of the `Throws` type as the
    //    first template argument to `emplace`.
    //
    // Testing:
    //
    //   TYPE& emplace<TYPE>(Args&&...);
    //   TYPE& emplace<INDEX>(Args&&...);
    //   TYPE& emplace<TYPE>(initializer_list<U>, Args&&...);
    //   TYPE& emplace<INDEX>(initializer_list<U>, Args&&...);
#ifdef BDE_BUILD_TARGET_EXC
    bslma::TestAllocator da("other", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    TEST_EMPLACE_THROWS(());
    TEST_EMPLACE_THROWS((VA1));
    TEST_EMPLACE_THROWS((VA1, VA2));
    TEST_EMPLACE_THROWS((VA1, VA2, VA3, VA4, VA5));
    TEST_EMPLACE_THROWS((VA1, VA2, VA3, VA4, VA5, VA6));
    TEST_EMPLACE_THROWS((VA1, VA2, VA3, VA4, VA5, VA6, VA7));
    TEST_EMPLACE_THROWS((VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8));
    TEST_EMPLACE_THROWS((VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8, VA9));
    TEST_EMPLACE_THROWS((VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8, VA9, VA10));
#ifdef U_VARIANT_FULL_IMPLEMENTATION
    TEST_EMPLACE_THROWS((std::initializer_list<int>{1, 2, 3}));
    TEST_EMPLACE_THROWS((std::initializer_list<int>{1, 2, 3}, VA1));
    TEST_EMPLACE_THROWS((std::initializer_list<int>{1, 2, 3}, VA1, VA2));
    TEST_EMPLACE_THROWS(
               (std::initializer_list<int>{1, 2, 3}, VA1, VA2, VA3, VA4, VA5));
    TEST_EMPLACE_THROWS(
          (std::initializer_list<int>{1, 2, 3}, VA1, VA2, VA3, VA4, VA5, VA6));
    TEST_EMPLACE_THROWS(
     (std::initializer_list<int>{1, 2, 3}, VA1, VA2, VA3, VA4, VA5, VA6, VA7));
    TEST_EMPLACE_THROWS((std::initializer_list<int>{1, 2, 3},
                         VA1,
                         VA2,
                         VA3,
                         VA4,
                         VA5,
                         VA6,
                         VA7,
                         VA8));
    TEST_EMPLACE_THROWS((std::initializer_list<int>{1, 2, 3},
                         VA1,
                         VA2,
                         VA3,
                         VA4,
                         VA5,
                         VA6,
                         VA7,
                         VA8,
                         VA9));
    TEST_EMPLACE_THROWS((std::initializer_list<int>{1, 2, 3},
                         VA1,
                         VA2,
                         VA3,
                         VA4,
                         VA5,
                         VA6,
                         VA7,
                         VA8,
                         VA9,
                         VA10));
#endif  // U_VARIANT_FULL_IMPLEMENTATION
#endif  // BDE_BUILD_TARGET_EXC
}

template <class VARIANT, size_t t_NUM>
struct testCase7aIndex_imp;

template <class VARIANT>
struct testCase7aIndex_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase7aIndex_imp : testCase7aIndex_imp<VARIANT, t_NUM - 1> {
    testCase7aIndex_imp()
    {
        // --------------------------------------------------------------------
        // TESTING `emplace` METHOD
        //   This test will ensure that the `emplace` method works as expected.
        //
        // Concerns:
        // 1. Calling `emplace` creates an alternative type determined by the
        //    first template argument specifying the index of the alternative
        //    type.
        //
        // 2. Variadic arguments to `emplace` method are correctly forwarded to
        //    the constructor of the alternative type.
        //
        // 3. If `value_type` is allocator-aware, `emplace` invokes the
        //    allocator extended constructor using the `variant`'s allocator.
        //
        // 4. There are no unnecessary argument type and `value_type` copies
        //    created
        //
        // 5. `emplace` returns a reference offering modifiable access to the
        //    `variant`'s value type object.
        //
        // Plan:
        // 1. Create a `variant` object and call `emplace` method specifying
        //    the index of the alternative type to create and providing no
        //    arguments.  Verify the `variant` object contains the correct
        //    alternative type object [C-1]
        //
        // 2. In step 1, verify the alternative object was default constructed.
        //    [C-2]
        //
        // 3. Repeat step 1 using varying number of arguments and verify the
        //    arguments to the emplace method have been perfect forwarded to
        //    the constructor of the alternative type. [C-2]
        //
        // 4. Repeat steps 1-3 with an allocator-aware alternative type and
        //    verify the alternative object in variant was constructed using
        //    the correct allocator. [C-3]
        //
        // 5. In steps 1-4, verify no unnecessary copies of the arguments and
        //    the alternative type have been created. [C-4]
        //
        // 6. Verify that the returned reference refers to the alternative type
        //    object.  [C-5]
        //
        //
        // Testing:
        //
        //   TYPE& emplace<INDEX>(Args&&...);

        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
        typedef typename variant_alternative<0, VARIANT>::type     FirstType;
        typedef bslalg::ConstructorProxy<AltType> ValWithAllocator;
        typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;

        if (verbose)
            printf("\nTESTING `emplace` METHOD"
                   "\n========================\n");
        {
            bslma::TestAllocator da("other", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);

            TEST_EMPLACE(INDEX, (&da), ());
            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1), &da),
                         (MoveUtil::move(VA1)));
            TEST_EMPLACE(INDEX, (VA1, &da), (VA1));
            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1), VA2, &da),
                         (MoveUtil::move(VA1), VA2));
            TEST_EMPLACE(INDEX,
                         (VA1, MoveUtil::move(VA2), &da),
                         (VA1, MoveUtil::move(VA2)));

            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &da),
                         (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
            TEST_EMPLACE(INDEX,
                         (VA1, MoveUtil::move(VA2), VA3, &da),
                         (VA1, MoveUtil::move(VA2), VA3));

            TEST_EMPLACE(
                    INDEX,
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &da),
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
            TEST_EMPLACE(
                    INDEX,
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &da),
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1),
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
               INDEX,
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &da),
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5));

            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(INDEX,
                         (VA1,
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

            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(INDEX,
                         (VA1,
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

            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(INDEX,
                         (VA1,
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

            TEST_EMPLACE(INDEX,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(INDEX,
                         (VA1,
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

            TEST_EMPLACE(INDEX,
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
            TEST_EMPLACE(INDEX,
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
#ifdef BSLSTL_variant_TEST_BAD_EMPLACE
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
};

template <class VARIANT, size_t t_NUM>
struct testCase7aType_imp;

template <class VARIANT>
struct testCase7aType_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase7aType_imp : testCase7aType_imp<VARIANT, t_NUM - 1> {
    testCase7aType_imp()
    {
        // --------------------------------------------------------------------
        // TESTING `emplace` METHOD
        //   This test will ensure that the `emplace` method works as expected.
        //
        // Concerns:
        // 1. Calling `emplace` creates an alternative type determined by the
        //    first template argument specifying the type of the alternative.
        //
        // 2. Variadic arguments to `emplace` method are correctly forwarded to
        //    the constructor of the alternative type.
        //
        // 3. If `value_type` is allocator-aware, `emplace` invokes the
        //    allocator extended constructor using the `variant`'s allocator.
        //
        // 4. There are no unnecessary argument type and `value_type` copies
        //    created
        //
        // 5. `emplace` returns a reference offering modifiable access to the
        //    `variant`'s value type object.
        //
        // Plan:
        // 1. Create a `variant` object and call `emplace` method specifying
        //    the type of the alternative to create and providing no arguments.
        //    Verify the `variant` object contains the correct alternative type
        //    object [C-1]
        //
        // 2. In step 1, verify the alternative object was default constructed.
        //    [C-2]
        //
        // 3. Repeat step 1 using varying number of arguments and verify the
        //    arguments to the emplace method have been perfect forwarded to
        //    the constructor of the alternative type. [C-2]
        //
        // 4. Repeat steps 1-3 with an allocator-aware alternative type and
        //    verify the alternative object in variant was constructed using
        //    the correct allocator. [C-3]
        //
        // 5. In steps 1-4, verify no unnecessary copies of the arguments and
        //    the alternative type have been created. [C-4]
        //
        // 6. Verify that the returned reference refers to the alternative type
        //    object.  [C-5]
        //
        //
        // Testing:
        //
        //   TYPE& emplace<TYPE>(Args&&...);

        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
        typedef typename variant_alternative<0, VARIANT>::type     FirstType;
        typedef bslalg::ConstructorProxy<AltType> ValWithAllocator;
        typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;

        if (verbose)
            printf("\nTESTING `emplace` METHOD"
                   "\n========================\n");
        {
            bslma::TestAllocator da("other", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);

            TEST_EMPLACE(AltType, (&da), ());
            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1), &da),
                         (MoveUtil::move(VA1)));
            TEST_EMPLACE(AltType, (VA1, &da), (VA1));
            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1), VA2, &da),
                         (MoveUtil::move(VA1), VA2));
            TEST_EMPLACE(AltType,
                         (VA1, MoveUtil::move(VA2), &da),
                         (VA1, MoveUtil::move(VA2)));

            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &da),
                         (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
            TEST_EMPLACE(AltType,
                         (VA1, MoveUtil::move(VA2), VA3, &da),
                         (VA1, MoveUtil::move(VA2), VA3));

            TEST_EMPLACE(
                    AltType,
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &da),
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
            TEST_EMPLACE(
                    AltType,
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &da),
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1),
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
               AltType,
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &da),
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5));

            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(AltType,
                         (VA1,
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

            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(AltType,
                         (VA1,
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

            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(AltType,
                         (VA1,
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

            TEST_EMPLACE(AltType,
                         (MoveUtil::move(VA1),
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
            TEST_EMPLACE(AltType,
                         (VA1,
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

            TEST_EMPLACE(AltType,
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
            TEST_EMPLACE(AltType,
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
#ifdef BSLSTL_variant_TEST_BAD_EMPLACE
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
};

template <class VARIANT, size_t t_NUM>
struct testCase7bIndex_imp;

template <class VARIANT>
struct testCase7bIndex_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase7bIndex_imp : testCase7bIndex_imp<VARIANT, t_NUM - 1> {
    testCase7bIndex_imp()
    {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        // --------------------------------------------------------------------
        // TESTING TESTING INITIALIZER LIST `emplace` METHOD
        //   This test will ensure that the initializer list `emplace` method
        //   works as expected.
        //
        // Concerns:
        // 1. Calling `emplace` creates an alternative type determined by the
        //    first template argument specifying the index of the alternative
        //    type.
        //
        // 2. Variadic arguments to `emplace` method are correctly forwarded to
        //    the  constructor arguments in the presence of an initializer list
        //    argument.
        //
        // 3. If `value_type` is allocator-aware, `emplace` invokes the
        //    allocator extended constructor using the `variant`'s allocator.
        //
        // 4. There are no unnecessary argument type and `value_type` copies
        //    created
        //
        // 5. `emplace` returns a reference offering modifiable access to the
        //    `variant`'s value type object.
        //
        // Plan:
        // 1. Create a `variant` object and call `emplace` method specifying
        //    the index of the alternative type to create and providing an
        //    initializer list.  Verify the `variant` object contains the
        //    correct alternative type object [C-1]
        //
        // 2. In step 1, verify the alternative object was created using the
        //    specified initializer list. [C-2]
        //
        // 3. Repeat step 1 using varying number of arguments. [C-2]
        //
        // 4. Repeat steps 1-3 with an allocator-aware alternative type and
        //    verify the alternative object in variant was constructed using
        //    the correct allocator. [C-3]
        //
        // 5. In steps 1-4, verify no unnecessary copies of the arguments and
        //    the alternative type have been created. [C-4]
        //
        // 6. Verify that the returned reference refers to the alternative type
        //    object.  [C-5]
        //
        // Testing:
        //
        //   TYPE& emplace<INDEX>(initializer_list<U>, Args&&...);
        //
        // --------------------------------------------------------------------

#ifdef U_VARIANT_FULL_IMPLEMENTATION
        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
        typedef typename variant_alternative<0, VARIANT>::type     FirstType;
        typedef bslalg::ConstructorProxy<AltType> ValWithAllocator;
        typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;

        if (verbose)
            printf("\nTESTING INITIALIZER LIST `emplace` METHOD"
                   "\n=========================================\n");
        {
            bslma::TestAllocator da("other", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3}, &da),
                         ({1, 2, 3}));
            TEST_EMPLACE(
              INDEX,
              (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &da),
              ({1, 2, 3}, MoveUtil::move(VA1)));
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3}, VA1, &da),
                         ({1, 2, 3}, VA1));
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
                          MoveUtil::move(VA1),
                          VA2,
                          &da),
                         ({1, 2, 3}, MoveUtil::move(VA1), VA2));
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          &da),
                         ({1, 2, 3}, VA1, MoveUtil::move(VA2)));

            TEST_EMPLACE(
                INDEX,
                (std::initializer_list<int>{1, 2, 3},
                 MoveUtil::move(VA1),
                 VA2,
                 MoveUtil::move(VA3),
                 &da),
                ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          VA3,
                          &da),
                         ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3));

            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
                          MoveUtil::move(VA1),
                          VA2,
                          MoveUtil::move(VA3),
                          VA4,
                          &da),
                         ({1, 2, 3},
                          MoveUtil::move(VA1),
                          VA2,
                          MoveUtil::move(VA3),
                          VA4));
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          VA3,
                          MoveUtil::move(VA4),
                          &da),
                         ({1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          VA3,
                          MoveUtil::move(VA4)));

            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(INDEX,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(INDEX,
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
            TEST_EMPLACE(INDEX,
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

            TEST_EMPLACE(INDEX,
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
            TEST_EMPLACE(INDEX,
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
#endif  // U_VARIANT_FULL_IMPLEMENTATION
    }
};

template <class VARIANT, size_t t_NUM>
struct testCase7bType_imp;

template <class VARIANT>
struct testCase7bType_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase7bType_imp : testCase7bType_imp<VARIANT, t_NUM - 1> {
    testCase7bType_imp()
    {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        // --------------------------------------------------------------------
        // TESTING TESTING INITIALIZER LIST `emplace` METHOD
        //   This test will ensure that the initializer list `emplace` method
        //   works as expected.
        //
        // Concerns:
        // 1. Calling `emplace` creates an alternative type determined by the
        //    first template argument specifying the type of the alternative.
        //
        // 2. Variadic arguments to `emplace` method are correctly forwarded to
        //    the  constructor arguments in the presence of an initializer list
        //    argument.
        //
        // 3. If `value_type` is allocator-aware, `emplace` invokes the
        //    allocator extended constructor using the `variant`'s allocator.
        //
        // 4. There are no unnecessary argument type and `value_type` copies
        //    created
        //
        // 5. `emplace` returns a reference offering modifiable access to the
        //    `variant`'s value type object.
        //
        // Plan:
        // 1. Create a `variant` object and call `emplace` method specifying
        //    the alternative type to create and providing an initializer list.
        //    Verify the `variant` object contains the correct alternative type
        //    object [C-1]
        //
        // 2. In step 1, verify the alternative object was created using the
        //    specified initializer list. [C-2]
        //
        // 3. Repeat step 1 using varying number of arguments. [C-2]
        //
        // 4. Repeat steps 1-3 with an allocator-aware alternative type and
        //    verify the alternative object in variant was constructed using
        //    the correct allocator. [C-3]
        //
        // 5. In steps 1-4, verify no unnecessary copies of the arguments and
        //    the alternative type have been created. [C-4]
        //
        // 6. Verify that the returned reference refers to the alternative type
        //    object.  [C-5]
        //
        // Testing:
        //
        //   TYPE& emplace<TYPE>(initializer_list<U>, Args&&...);
        //
        // --------------------------------------------------------------------

#ifdef U_VARIANT_FULL_IMPLEMENTATION
        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
        typedef typename variant_alternative<0, VARIANT>::type     FirstType;
        typedef bslalg::ConstructorProxy<AltType> ValWithAllocator;
        typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;

        if (verbose)
            printf("\nTESTING INITIALIZER LIST `emplace` METHOD"
                   "\n=========================================\n");
        {
            bslma::TestAllocator da("other", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3}, &da),
                         ({1, 2, 3}));
            TEST_EMPLACE(
              AltType,
              (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &da),
              ({1, 2, 3}, MoveUtil::move(VA1)));
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3}, VA1, &da),
                         ({1, 2, 3}, VA1));
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
                          MoveUtil::move(VA1),
                          VA2,
                          &da),
                         ({1, 2, 3}, MoveUtil::move(VA1), VA2));
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          &da),
                         ({1, 2, 3}, VA1, MoveUtil::move(VA2)));

            TEST_EMPLACE(
                AltType,
                (std::initializer_list<int>{1, 2, 3},
                 MoveUtil::move(VA1),
                 VA2,
                 MoveUtil::move(VA3),
                 &da),
                ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          VA3,
                          &da),
                         ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3));

            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
                          MoveUtil::move(VA1),
                          VA2,
                          MoveUtil::move(VA3),
                          VA4,
                          &da),
                         ({1, 2, 3},
                          MoveUtil::move(VA1),
                          VA2,
                          MoveUtil::move(VA3),
                          VA4));
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          VA3,
                          MoveUtil::move(VA4),
                          &da),
                         ({1, 2, 3},
                          VA1,
                          MoveUtil::move(VA2),
                          VA3,
                          MoveUtil::move(VA4)));

            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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
            TEST_EMPLACE(AltType,
                         (std::initializer_list<int>{1, 2, 3},
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

            TEST_EMPLACE(AltType,
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
            TEST_EMPLACE(AltType,
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

            TEST_EMPLACE(AltType,
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
            TEST_EMPLACE(AltType,
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
#endif  // U_VARIANT_FULL_IMPLEMENTATION
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase7Index()
{
    testCase7aIndex_imp<VARIANT, bsl::variant_size<VARIANT>::value> testA;
    testCase7bIndex_imp<VARIANT, bsl::variant_size<VARIANT>::value> testB;
}

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase7Type()
{
    testCase7aType_imp<VARIANT, bsl::variant_size<VARIANT>::value> testA;
    testCase7bType_imp<VARIANT, bsl::variant_size<VARIANT>::value> testB;
}

void testCase6e()
{
    //
    // TESTING `index` FREE FUNCTION
    //
    //   This test will ensure that `index` member function works as expected.
    //
    // Concerns:
    // 1. Calling `index` returns the index of the current active alternative.
    //
    // 2. `index` can be invoked with a `const` `variant`
    //     object.
    //
    // Plan:
    //
    // 1. Create a variant object.  Check that `index` returns the correct
    //    index for the current active altenative. [C-1]
    //
    // 2. Modify the currently active alternative type and verify `index`
    //    returns the correct value each time. [C-1]
    //
    // 2. Repeat steps 1-2 with a const `variant` object. [C-3]
    //
    //
    // Testing:
    //
    //    size_t index() const;
    //
    // --------------------------------------------------------------------

    if (veryVerbose)
        printf("\tTesting unique alternatives.\n");
    {
        bsl::variant<int, char, long, MyClass1>        obj;
        const bsl::variant<int, char, long, MyClass1>& cobj = obj;

        ASSERT(cobj.index() == 0);

        obj.emplace<1>('a');

        ASSERT(cobj.index() == 1);

        obj = MyClass1();

        ASSERT(cobj.index() == 3);

        obj.emplace<long>(3);

        ASSERT(cobj.index() == 2);
    }
    if (veryVerbose)
        printf("\tTesting nonunique alternatives.\n");
    {
        bsl::variant<int, char, int, MyClass1>        obj;
        const bsl::variant<int, char, int, MyClass1>& cobj = obj;

        ASSERT(cobj.index() == 0);

        obj.emplace<0>(1);

        ASSERT(cobj.index() == 0);

        obj.emplace<2>(1);

        ASSERT(cobj.index() == 2);
    }
}

void testCase6d()
{
    //
    // TESTING `valueless_by_exception` FREE FUNCTION
    //
    //   This test will ensure that `valueless_by_exception` free function
    //   works as expected.  This test only makes sense when exceptions are
    //   enabled.
    //
    // Concerns:
    // 1. Calling `valueless_by_exception` returns `false` if the `variant`
    //    object holds an active alternative object.
    //
    // 2. If `variant` object is empty, `valueless_by_exception` returns `true`
    //    and `index` returns `bsl::variant_npos`.
    //
    // 3. `valueless_by_exception` can be invoked with a `const` `variant`
    //     object.
    //
    // Plan:
    //
    // 1. Create a variant object.  Check that `valueless_by_exception`
    //    returns `false`. [C-1]
    //
    // 2. Using `emplace` and a helper class that throws on construction,
    //    put the variant object in an empty state.  Check that
    //    `valueless_by_exception` returns `true` and that `index` returns
    //    `bsl::variant_npos`. [C-2]
    //
    // 3. Using `emplace` and a helper class that doesn't throws on
    // construction,
    //    put the variant object in an engaged state.  Check that
    //    `valueless_by_exception` returns `false` and that `index` returns
    //    the corect alternative index. [C-2]
    //
    // 4. Repeat steps 1-2 with a const `variant` object. [C-3]
    //
    //
    // Testing:
    //    bool valueless_by_exception() const;
    //    size_t index() const;
    //
    // --------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
    Throws::s_should_throw = false;
    bsl::variant<Throws> obj;
    ASSERT(obj.valueless_by_exception() == false);
    ASSERT(obj.index() == 0);

    Throws::s_should_throw = true;
    BSLS_TRY { obj.emplace<Throws>(1); }
    BSLS_CATCH(...){};
    ASSERT(obj.valueless_by_exception() == true);
    ASSERT(obj.index() == bsl::variant_npos);

    Throws::s_should_throw = false;
    obj.emplace<Throws>(1);
    ASSERT(obj.valueless_by_exception() == false);
    ASSERT(obj.index() == 0);
#endif  // BDE_BUILD_TARGET_EXC
}

RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(6c)
{
    //
    // TESTING `holds_alternative` FREE FUNCTION
    //
    //   This test will ensure that `holds_alternative` free function works
    //   as expected.
    //
    // Concerns:
    // 1. Calling `holds_alternative` with type `TYPE` as the template argument
    //    and a `variant` object with `TYPE ` as one of its alternative types
    //    returns `true` if `TYPE` is the currently active alternative, and
    //    `false` otherwise.
    //
    // 2. It's not possible to invoke `holds_alternative` with type `TYPE` as
    //    the template argument if the specified  `variant` object doesn't have
    //    `TYPE ` as one of its alternative types.
    //
    // 3. `holds_alternative` can be invoked with a `const` `variant` object.
    //
    // Plan:
    //
    // 1. Create an object of alternative type `A` and use it to create a
    //    `variant` object holding alternative type `A`.  Check that
    //   invoking `holds_alternative` with type `A` returns `true`. [C-1]
    //
    // 2. In step1, invoke `holds_alternative` with a different alternative
    //   type and check that the function returns `false`. [C-1]
    //
    // 3. In step1, invoke `holds_alternative` with a type that is not an
    //   alternative, and observe the compilation error.  This test is disabled
    //   by default and needs to be run manually. [C-2]
    //
    // 4. Repeat steps 1-2 with a const `variant` object. [C-3]
    //
    //
    // Testing:
    //    bool holds_alternative(const variant&);
    //
    // --------------------------------------------------------------------

    const size_t                                                IndexA = N - 1;
    const size_t                                                IndexB = M - 1;
    typedef typename variant_alternative<IndexB, VARIANT>::type AltTypeB;

    {
        VARIANT        obj  = VARIANT(bsl::in_place_index_t<IndexA>());
        const VARIANT& cobj = obj;
        ASSERT(obj.index() == IndexA);

        if (IndexA == IndexB) {
            ASSERT(holds_alternative<AltTypeB>(obj));
            ASSERT(holds_alternative<AltTypeB>(cobj));
        }
        else {
            ASSERT(!holds_alternative<AltTypeB>(obj));
            ASSERT(!holds_alternative<AltTypeB>(cobj));
        }
#ifdef false
        struct NonAlternative {
        };
        holds_alternative<NonAlternative>(obj);
#endif
    }
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END
void testCase6b1()
{
    //
    // TESTING `get_if` FREE FUNCTION
    //
    //   This test will ensure that the `get_if` function works as expected.
    //
    // Concerns:
    // 1. Calling `get_if` free function with an alternative type as a template
    //    argument and a nullptr as the function argument
    //    returns a nullptr.
    //
    // 2. Calling `get_if` free function with an alternative type as a template
    //    argument and a `valueless_by_exception`
    //    variant object as the function argument returns a `nullptr`.
    //
    // 3. The behaviour is the same when the template arguments specifies
    //    an index of an alternative type.
    //
    // 4. `get_if` free function only accepts type template argument
    //    that are valid alternative types.
    //
    // 5. get_if' free function only accepts `size_t` template argument
    //    that are indeces of alternative types.
    //
    // Plan:
    //
    // 1. Invoke `get_if` with a valid alternative type as the template
    //    argument and `nullptr` as the function argument, and verify that it
    //    returns `nullptr`. [C-1]
    //
    // 2. Create a variant object in a `valueless_by_exceptions` state
    //    using the `Throws` helper class and verify that invoking `get_if`
    //    with `Throws` as template argument and the pointer to the said
    //    variant object returns `nullptr`. [C-2]
    //
    // 3. Repeat step 1-2 using an index of an alternative type
    //    as the template argumen. [C-3]
    //
    // 4. Verify that specifying an type that is not an alternative type as a
    //    template argument to `get_if` function results in an error.  Note
    //    that this test is disabled by default and needs to be manually
    //    enabled. [C-4]
    //
    // 5. In step 4, use an invalid alternative type index as the template
    //    argument. [C-5]
    //
    // Testing:
    //      TYPE* get_if<INDEX>(variant *);
    //      const TYPE* get_if<INDEX>(const variant *);
    //      TYPE*  get_if<TYPE>(variant *);
    //      const TYPE* get_if<TYPE>(const variant *);
    // --------------------------------------------------------------------

    bsl::variant<Throws> *pobj = 0;
    ASSERT(get_if<0>(pobj) == NULL);
    ASSERT(get_if<Throws>(pobj) == NULL);

#ifdef BDE_BUILD_TARGET_EXC
    Throws::s_should_throw = false;
    bsl::variant<Throws> obj;
    ASSERT(obj.valueless_by_exception() == false);

    Throws::s_should_throw = true;
    BSLS_TRY { obj.emplace<Throws>(1); }
    BSLS_CATCH(...){};
    ASSERT(obj.valueless_by_exception() == true);

    ASSERT(get_if<0>(&obj) == NULL);
    ASSERT(get_if<Throws>(&obj) == NULL);

#ifdef MANUAL_TEST17
    // compilation fails for invalid indices
    get_if<bsl::variant_npos>(&obj);
    get_if<1>(&obj);
        // compilation fails for invalid types
    get_if<int>(&obj);
    get_if<const Throws>(&obj);
#endif
#endif  // BDE_BUILD_TARGET_EXC
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(6bIndex)
{
    //
    // TESTING `get_if` FREE FUNCTION
    //
    //   This test will ensure that the `get_if` function works as expected.
    //
    // Concerns:
    // 1. Calling `get_if` free function with an index as a template
    //    argument and a pointer to a variant object as the function argument
    //    returns a pointer to the object managed by the variant when the
    //    specified index is the index of the currently active alternative.
    //
    // 2. Calling `get_if` free function with an index as a template
    //    argument and a pointer to a variant object as the function argument
    //    returns a `nullptr` when the index is not the index of the currently
    //    active alternative.
    //
    // 3. `get_if` free function can be invoked with a pointer to a const
    //     variant object.
    //
    // 4.  When invoked with a pointer to a `const variant` object, `get_if`
    //     function returns a pointer to a `const` alternative type object.
    //
    // Plan:
    //
    // 1. Create an object of alternative type `A` and use it to create a
    //    `variant` object holding alternative type `A`.  Using `get_if` method
    //    with index of type `A` as the template argument, verify that the
    //    returned object is identical to the source object.  If `A` is
    //    allocator aware, verify that the object returned from the `get_if`
    //    method uses the allocator of the `variant` object. [C-1]
    //
    // 2. In step 1, call `get_if` method using a different index as the
    //    template argument and verify that it returns `nullptr`. [C-2]
    //
    // 3. Repeat steps 1-2 using a pointer to a const `variant` object. [C-3]
    //
    // 4. In step 4, verify the return pointer is to a const alternative
    //    type object.  Note that this test results in a compilation error
    //    and needs to be manually enabled. [C-4]
    //
    // Testing:
    //      TYPE* get_if<INDEX>(variant *);
    //      const TYPE* get_if<INDEX>(const variant *);
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    const size_t                                                IndexA = N - 1;
    typedef typename variant_alternative<IndexA, VARIANT>::type AltTypeA;
    const size_t                                                IndexB = M - 1;
    typedef typename variant_alternative<IndexB, VARIANT>::type AltTypeB;

    typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;

    {
        AltTypeA             source(23);
        VariantWithAllocator VarBuf(
            bsl::in_place_index_t<IndexA>(), source, &oa);
        VARIANT       *obj  = BSLS_UTIL_ADDRESSOF(VarBuf.object());
        const VARIANT *cobj = BSLS_UTIL_ADDRESSOF(VarBuf.object());
        ASSERT(obj->index() == IndexA);
        ASSERT(checkAllocator(*obj, &oa));

        if (IndexA == IndexB) {
            AltTypeA *res = bsl::get_if<IndexA>(obj);
            ASSERT(res);
            ASSERT(*res == source);
            ASSERT(checkAllocator(*res, &oa));

            ASSERT(isConstPtr(bsl::get_if<IndexA>(cobj)));
            const AltTypeA *cres = bsl::get_if<IndexA>(cobj);
            ASSERT(cres);
            ASSERT(*cres == source);
            ASSERT(checkAllocator(*cres, &oa));
        }
        else {
            AltTypeB *res = bsl::get_if<IndexB>(obj);
            ASSERT(!res);

            ASSERT(isConstPtr(bsl::get_if<IndexB>(cobj)));
            const AltTypeB *cres = bsl::get_if<IndexB>(cobj);
            ASSERT(!cres);
        }
    }
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END

RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(6bType)
{
    //
    // TESTING `get_if` FREE FUNCTION
    //
    //   This test will ensure that the `get_if` function works as expected.
    //
    // Concerns:
    // 1. Calling `get_if` free function with an alternative type as a template
    //    argument and a pointer to a variant object as the function argument
    //    returns a pointer to the object managed by the variant when the
    //    specified alternative type is the currently active type.
    //
    // 2. Calling `get_if` free function with an alternative type as a template
    //    argument and a pointer to a variant object as the function argument
    //    returns a `nullptr` when the specified alternative type is not the
    //   currently active type.
    //
    // 3. `get_if` free function can be invoked with a pointer to a const
    //     variant object.
    //
    // 4.  When invoked with a pointer to a `const variant` object, `get_if`
    //     function returns a pointer to a `const` alternative type object.
    //
    // Plan:
    //
    // 1. Create an object of alternative type `A` and use it to create a
    //    `variant` object holding alternative type `A`.  Using `get_if` method
    //    with type `A` as the template argument, verify that the returned
    //    object is identical to the source object.  If `A` is allocator aware,
    //    verify that the object returned from the `get_if` method uses the
    //    allocator of the `variant` object. [C-1]
    //
    // 2. In step 1, call `get_if` method using an alternative type `B` as the
    //    template argument and verify that it returns `nullptr`. [C-2]
    //
    // 3. Repeat steps 1-2 using a pointer to a const `variant` object. [C-3]
    //
    // 4. In step 3, verify the return pointer is to a const alternative
    //    type object.  Note that this test results in a compilation error
    //    and needs to be manually enabled. [C-4]
    //
    // Testing:
    //      TYPE*  get_if<TYPE>(variant *);
    //      const TYPE* get_if<TYPE>(const variant *);
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    const size_t                                                IndexA = N - 1;
    typedef typename variant_alternative<IndexA, VARIANT>::type AltTypeA;
    const size_t                                                IndexB = M - 1;
    typedef typename variant_alternative<IndexB, VARIANT>::type AltTypeB;

    typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;
    {
        AltTypeA             source(23);
        VariantWithAllocator VarBuf(
            bsl::in_place_index_t<IndexA>(), source, &oa);
        VARIANT       *obj  = BSLS_UTIL_ADDRESSOF(VarBuf.object());
        const VARIANT *cobj = BSLS_UTIL_ADDRESSOF(VarBuf.object());
        ASSERT(obj->index() == IndexA);
        ASSERT(checkAllocator(*obj, &oa));

        if (IndexA == IndexB) {
            AltTypeA *res = bsl::get_if<AltTypeA>(obj);
            ASSERT(res);
            ASSERT(*res == source);
            ASSERT(checkAllocator(*res, &oa));

            ASSERT(isConstPtr(bsl::get_if<AltTypeA>(cobj)));
            const AltTypeA *cres = bsl::get_if<AltTypeA>(cobj);
            ASSERT(*cres == source);
            ASSERT(checkAllocator(*cres, &oa));
        }
        else {
            AltTypeB *res = bsl::get_if<AltTypeB>(obj);
            ASSERT(!res);

            ASSERT(isConstPtr(bsl::get_if<AltTypeB>(cobj)));
            const AltTypeB *cres = bsl::get_if<AltTypeB>(cobj);
            ASSERT(!cres);
        }
    }
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END

void testCase6a2()
{
    //   TESTING `get` FREE FUNCTION
    //
    //   This test will ensure that the `get` free function checks for
    //   valid alternative type and index and handles `valueless_by_exception`
    //   `variant` object correctly
    //
    // Concerns:
    // 1. Calling `get` free function with an alternative type as a template
    //    argument and `valueless_by_exception` variant object
    //    results in `bad_variant_access` exception.
    //
    // 2. Calling `get` free function with an index of an alternative type as
    //    a template argument argument and `valueless_by_exception` variant
    // object
    //    results in `bad_variant_access` exception.
    //
    // 3. The behaviour is the same regarldes of the const qualification or
    //    value category of the vraint function argument.
    //
    // 4. `get` free function requires the specified type template argument
    //    to be a valid alternative type, and for the specified `size_t`
    //    template argument to be an index of an alterantive type.
    //
    //
    // Plan:
    //
    // 1. Using `Throws` helper class, create a `valueless_by_exception`
    //    variant object.  Verify that `get` function with `Throws` as the
    // template
    //    argument throws `bad_variant_access` exception. [C-1]
    //
    // 2. Repeat step 1 with `0` as the template argument. [C-2]
    //
    // 3. Repeat step 1-2 using a const lvalue, rvalue and const rvalue variant
    //   function argument. [C-3]
    //
    // 4. Verify that specifying an type that is not an alternative type as a
    //    template argument to `get_if` function results in an error.  Note
    //    that this test is disabled by default and needs to be manually
    //    enabled. [C-4]
    //
    // 5. In step 4, use an invalid alternative type index as the template
    //    argument. [C-4]
    //
    // Testing:
    //  TYPE& get<INDEX>(variant&);
    //  const TYPE& get<INDEX>(const variant&);
    //  TYPE&& get<INDEX>(variant&&);
    //  const TYPE&& get<INDEX>(const variant&&);
    //  TYPE& get<TYPE>(variant&);
    //  const TYPE& get<TYPE>(const variant&);
    //  TYPE&& get<TYPE>(variant&&);
    //  const TYPE&& get<TYPE>(const variant&&);
    //
    // --------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
    Throws::s_should_throw = false;
    bsl::variant<Throws>        obj;
    const bsl::variant<Throws>& cobj = obj;
    ASSERT(obj.valueless_by_exception() == false);

    Throws::s_should_throw = true;
    BSLS_TRY { obj.emplace<Throws>(1); }
    BSLS_CATCH(...){};
    ASSERT(obj.valueless_by_exception() == true);

    TEST_GET_THROWS_ALT(Throws, obj);
    TEST_GET_THROWS_ALT(Throws, cobj);
    TEST_GET_THROWS_ALT(Throws, MoveUtil::move(obj));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    TEST_GET_THROWS_ALT(Throws, MoveUtil::move(cobj));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    TEST_GET_THROWS_ALT(0, obj);
    TEST_GET_THROWS_ALT(0, cobj);
    TEST_GET_THROWS_ALT(0, MoveUtil::move(obj));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    TEST_GET_THROWS_ALT(0, MoveUtil::move(cobj));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef MANUAL_TEST18
    // compilation fails for invalid indices
    get<bsl::variant_npos>(obj);
    get<1>(obj);
        // compilation fails for invalid types
    get<int>(obj);
    get<const Throws>(obj);
#endif
#endif  // BDE_BUILD_TARGET_EXC
}

void testCase6a1()
{
    //   TESTING `get` FREE FUNCTION
    //
    //   This test will ensure that the `get` free function returns a correctly
    //   const qualified object of appropriate value category.
    //
    // Concerns:
    // 1. Calling `get` free function with an alternative type as a template
    //    argument and with a const qualified variant object
    //    returns a reference to a const qualified alternative object.
    //
    // 1. Calling `get` free function  with an alternative type as a template
    //    argument and an lvalue returns an lvalue reference, and
    //    rvalue reference otherwise.
    //
    // 3. The behaviour is the same for `get` free function invoked with an
    //    index as a template argument.
    //
    //
    // Plan:
    //
    // 1. Using `checkQualification` helper function,
    //    verify that invoking `get` function with a currently active
    // alternative
    //    type as the type template argument and an lvalue of non const variant
    //    returns an lvalue reference to a non-const alternative object.
    // [C-1][C-2]
    //
    // 2. In step 1, repeat the test using a const qualified lvalue and verify
    //    that the `get` function returns a const qualified lvalue reference.
    // [c-2][C-2]
    //
    // 3. Repeat step 1-2 using an rvalue of variant type and verify the
    // returned
    //    reference is correctly const qualified rvalue reference [C-1][C-22]
    //
    // 4. Repeat steps 1-3 using athe index of the currently active alternative
    //    as the value of the size_t template argument.
    //
    // Testing:
    //  TYPE& get<INDEX>(variant&);
    //  const TYPE& get<INDEX>(const variant&);
    //  TYPE&& get<INDEX>(variant&&);
    //  const TYPE&& get<INDEX>(const variant&&);
    //  TYPE& get<TYPE>(variant&);
    //  const TYPE& get<TYPE>(const variant&);
    //  TYPE&& get<TYPE>(variant&&);
    //  const TYPE&& get<TYPE>(const variant&&);
    //
    // --------------------------------------------------------------------

    {
        bsl::variant<int>        obj;
        const bsl::variant<int>& cobj = obj;

        ASSERT(checkQualification(get<int>(obj)) == LVALUE_REF);
        ASSERT(checkQualification(get<int>(cobj)) == LVALUE_CONST_REF);
        ASSERT(checkQualification(get<int>(MoveUtil::move(obj))) ==
               RVALUE_REF);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT(checkQualification(get<int>(MoveUtil::move(cobj))) ==
               RVALUE_CONST_REF);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        ASSERT(checkQualification(get<0>(obj)) == LVALUE_REF);
        ASSERT(checkQualification(get<0>(cobj)) == LVALUE_CONST_REF);
        ASSERT(checkQualification(get<0>(MoveUtil::move(obj))) == RVALUE_REF);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT(checkQualification(get<0>(MoveUtil::move(cobj))) ==
               RVALUE_CONST_REF);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
}

RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(6aIndex)
{
    //
    // TESTING `get` FREE FUNCTION
    //
    //   This test will ensure that the `get` free function works as expected.
    //
    // Concerns:
    // 1. Calling get free function with an index as a template
    //    argument and a variant object as the function argument
    //    returns a reference to the object managed by the variant when the
    //    specified index is the index of the currently active alternative
    //    type.
    //
    // 2. Calling get free function with an index as a template
    //    argument and a variant object as the function argument
    //    throws a `bad_variant_access` exception when the specified
    //    index is not the index of the currently active alternative type.
    //
    // 3. Get free function can be invoked on const `variant` objects and on
    //    rvalues of `variant` type.
    //
    // Plan:
    //
    // 1. Create an object of alternative type `A` and use it to create a
    //    `variant` object holding alternative type `A`.  Using get method with
    //    index of `A` as the template argument, verify that the returned
    // object
    //    is identical to the source object.  If `A` is allocator aware, verify
    //    that the object returned from the get method uses the allocator of
    //    the `variant` object. [C-1]
    //
    // 2. In step 1, call get method using an alternative type `B` as the
    //    template argument and verify that an exception of type
    //    `bad_variant_access` is thrown. [C-2]
    //
    // 3. Repeat steps 1-3 using a reference to a const `variant` object,
    //    rvalue of `variant` type, and const rvalue of `variant` type. [C-3]
    //
    // Testing:
    //  TYPE& get<INDEX>(variant&);
    //  const TYPE& get<INDEX>(const variant&);
    //  TYPE&& get<INDEX>(variant&&);
    //  const TYPE&& get<INDEX>(const variant&&);
    //
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    const size_t                                                IndexA = N - 1;
    typedef typename variant_alternative<IndexA, VARIANT>::type AltTypeA;
    const size_t                                                IndexB = M - 1;

    typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;
    if (veryVerbose)
        printf("\tUsing index template argument.\n");
    {
        AltTypeA             source(23);
        VariantWithAllocator VarBuf(
            bsl::in_place_index_t<IndexA>(), source, &oa);
        VARIANT&       obj  = VarBuf.object();
        const VARIANT& cobj = VarBuf.object();
        ASSERT(obj.index() == IndexA);
        ASSERT(checkAllocator(obj, &oa));

        if (IndexA == IndexB) {
            AltTypeA& res = bsl::get<IndexA>(obj);
            ASSERT(res == source);
            ASSERT(checkAllocator(res, &oa));

            const AltTypeA& cres = bsl::get<IndexA>(cobj);
            ASSERT(cres == source);
            ASSERT(checkAllocator(cres, &oa));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            const AltTypeA&& crres = bsl::get<IndexA>(MoveUtil::move(cobj));
            ASSERT(crres == source);
            ASSERT(checkAllocator(crres, &oa));

            AltTypeA&& rres = bsl::get<IndexA>(MoveUtil::move(obj));
            ASSERT(rres == source);
            ASSERT(checkAllocator(rres, &oa));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
        else {
#ifdef BDE_BUILD_TARGET_EXC
            TEST_GET_NON_ACTIVE_ALT(IndexB, obj);
            TEST_GET_NON_ACTIVE_ALT(IndexB, cobj);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_GET_NON_ACTIVE_ALT(IndexB, MoveUtil::move(obj));
            TEST_GET_NON_ACTIVE_ALT(IndexB, MoveUtil::move(cobj));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif  // BDE_BUILD_TARGET_EXC
        }
    }
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END

RUN_FOR_EACH_ALTERNATIVE_COMBINATION_START(6aType)
{
    //
    // TESTING `get` FREE FUNCTION
    //
    //   This test will ensure that the `get` free function works as expected.
    //
    // Concerns:
    // 1. Calling get free function with an alternative type as a template
    //    argument and a variant object as the function argument
    //    returns a reference to the object managed by the variant when the
    //    specified alternative type is the currently active type.
    //
    // 2. Calling get free function with an alternative type as a template
    //    argument and a variant object as the function argument
    //    throws a `bad_variant_access` exception when the
    //    specified alternative type is not the currently active type.
    //
    // 3. Get free function can be invoked on const `variant` objects and on
    //    rvalues of `variant` type.
    //
    // Plan:
    //
    // 1. Create an object of alternative type `A` and use it to create a
    //    `variant` object holding alternative type `A`.  Using get method with
    //    type `A` as the template argument, verify that the returned object
    //    is identical to the source object.  If `A` is allocator aware, verify
    //    that the object returned from the get method uses the allocator of
    //    the `variant` object. [C-1]
    //
    // 2. In step 1, call get method using an alternative type `B` as the
    //    template argument and verify that an exception of type
    //    `bad_variant_access` is thrown. [C-2]
    //
    // 3. Repeat steps 1-2 using a reference to a const `variant` object,
    //    rvalue of `variant` type, and const rvalue of `variant` type. [C-3]
    //
    // Testing:
    //  TYPE& get<TYPE>(variant&);
    //  const TYPE& get<TYPE>(const variant&);
    //  TYPE&& get<TYPE>(variant&&);
    //  const TYPE&& get<TYPE>(const variant&&);
    //
    // --------------------------------------------------------------------

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    const size_t                                                IndexA = N - 1;
    typedef typename variant_alternative<IndexA, VARIANT>::type AltTypeA;
    const size_t                                                IndexB = M - 1;
    typedef typename variant_alternative<IndexB, VARIANT>::type AltTypeB;

    typedef bslalg::ConstructorProxy<VARIANT> VariantWithAllocator;
    if (veryVerbose)
        printf("\tUsing type template argument.\n");
    {
        AltTypeA             source(32);
        VariantWithAllocator VarBuf(
            bsl::in_place_index_t<IndexA>(), source, &oa);
        VARIANT&       obj  = VarBuf.object();
        const VARIANT& cobj = VarBuf.object();
        ASSERT(obj.index() == IndexA);
        ASSERT(checkAllocator(obj, &oa));

        if (IndexA == IndexB) {
            AltTypeA& res = bsl::get<AltTypeA>(obj);
            ASSERT(res == source);
            ASSERT(checkAllocator(res, &oa));

            const AltTypeA& cres = bsl::get<AltTypeA>(cobj);
            ASSERT(cres == source);
            ASSERT(checkAllocator(cres, &oa));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            const AltTypeA&& crres = bsl::get<AltTypeA>(MoveUtil::move(cobj));
            ASSERT(crres == source);
            ASSERT(checkAllocator(crres, &oa));

            AltTypeA&& rres = bsl::get<AltTypeA>(MoveUtil::move(obj));
            ASSERT(rres == source);
            ASSERT(checkAllocator(rres, &oa));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
        else {
            TEST_GET_NON_ACTIVE_ALT(AltTypeB, obj);
            TEST_GET_NON_ACTIVE_ALT(AltTypeB, cobj);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_GET_NON_ACTIVE_ALT(AltTypeB, MoveUtil::move(obj));
            TEST_GET_NON_ACTIVE_ALT(AltTypeB, MoveUtil::move(cobj));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
    }
}
RUN_FOR_EACH_ALTERNATIVE_COMBINATION_END

void testCase5e()
{
    // --------------------------------------------------------------------
    // CONSTRUCTIBILITY FROM A VALUE
    //   This test will ensure that constructibility from a value is
    //   implemented as specified in the standard.
    //
    // Concerns:
    // 1. That the variant with non unique alternative TYPE can not be
    //    constructed from the value of TYPE.  The same applies to
    //    a variant with two alternatives of the same type, but different
    //    cv qualification.
    //
    // 2. That the variant with non unique alternative TYPE can not be
    //    constructed with a constructor taking a `in_place_type<TYPE>`.
    //
    // 3. That the variant with two alternatives of same type, but different
    //    cv-qualification can be constructed with a constructor taking an
    //    `in_place_type` tag
    //
    // 4. If the argument type does not exactly match any alternative type,
    //    and is not convertible to any alternative type, the construction
    //    from that argument is not possible.
    //
    // 5. If the argument type does not exactly match any alternative type,
    //    and has an equally good conversion to more than one alternative type,
    //    the construction from that argument is not possible.
    //
    // 6. If the argument type does not exactly match any alternative type,
    //    and has a deleted conversion to one alternative type, the
    //    construction from that argument is not possible.
    //
    // 7. If the argument type does not exactly match any alternative type,
    //    and has a narrowing conversion to one alternative type, the
    //    construction from that argument is not possible.
    //
    // 8. If the `in_place_type` argument does not exactly match any
    //    alternative type, or the specified altenative is not constructible
    //    from the remaining arguments, the construction from specified
    //    argument set is not possible.
    //
    // 9. That the same rules apply to allocator extended default constructor.
    //
    // Plan:
    // 1. For the concerns above, check `is_constructible` trait with the
    //    appropriate set of arguments.
    //
    // Testing:
    //    variant(TYPE&&);
    //    variant(alloc_arg, alloc, TYPE&&);
    //    variant(in_place_type);
    //    variant(alloc_arg, alloc, in_place_type);
    // --------------------------------------------------------------------

    typedef bsl::variant<MyClass2, MyClass2>       Unique_Variant;
    typedef bsl::variant<MyClass2, const MyClass2> CVUnique_Variant;
#ifdef U_VARIANT_FULL_IMPLEMENTATION
    ASSERT(!(std::is_constructible<Unique_Variant, MyClass2>::value));
    ASSERT(!(std::is_constructible<CVUnique_Variant, MyClass2>::value));

    ASSERT(!(std::is_constructible<Unique_Variant,
                                   bsl::in_place_type_t<MyClass2> >::value));
    ASSERT((std::is_constructible<CVUnique_Variant,
                                  bsl::in_place_type_t<MyClass2> >::value));
    ASSERT(
        (std::is_constructible<CVUnique_Variant,
                               bsl::in_place_type_t<const MyClass2> >::value));

    ASSERT(!(std::is_constructible<Unique_Variant,
                                   bsl::allocator_arg_t,
                                   Unique_Variant::allocator_type,
                                   MyClass2>::value));
    ASSERT(!(std::is_constructible<CVUnique_Variant,
                                   bsl::allocator_arg_t,
                                   CVUnique_Variant::allocator_type,
                                   MyClass2>::value));

    ASSERT(!(std::is_constructible<Unique_Variant,
                                   bsl::allocator_arg_t,
                                   Unique_Variant::allocator_type,
                                   bsl::in_place_type_t<MyClass2> >::value));
    ASSERT((std::is_constructible<CVUnique_Variant,
                                  bsl::allocator_arg_t,
                                  Unique_Variant::allocator_type,
                                  bsl::in_place_type_t<MyClass2> >::value));
    ASSERT(
        (std::is_constructible<CVUnique_Variant,
                               bsl::allocator_arg_t,
                               Unique_Variant::allocator_type,
                               bsl::in_place_type_t<const MyClass2> >::value));

    ///nonexisting alternative
    typedef bsl::variant<int, long> Variant_int_long;
    ASSERT(!(std::is_constructible<Variant_int_long,
                                   bsl::in_place_type_t<MyClass2> >::value));
    ASSERT(!(std::is_constructible<Variant_int_long,
                                   bsl::in_place_type_t<const int> >::value));
    ASSERT(!(std::is_constructible<Variant_int_long,
                                   allocator_arg_t,
                                   Variant_int_long::allocator_type,
                                   bsl::in_place_type_t<const int> >::value));

    //no conversion to an alternative
    ASSERT(!(std::is_constructible<Variant_int_long, MyClass2>::value));
    ASSERT(!(std::is_constructible<Variant_int_long,
                                   allocator_arg_t,
                                   Variant_int_long::allocator_type,
                                   MyClass2>::value));

    ASSERT(!(std::is_constructible<Variant_int_long,
                                   bsl::in_place_type_t<int>,
                                   MyClass2>::value));
    ASSERT(!(std::is_constructible<Variant_int_long,
                                   allocator_arg_t,
                                   Variant_int_long::allocator_type,
                                   bsl::in_place_type_t<int>,
                                   MyClass2>::value));

    ASSERT(!(std::is_constructible<Variant_int_long,
                                   bsl::in_place_index_t<0>,
                                   MyClass2>::value));
    ASSERT(!(std::is_constructible<Variant_int_long,
                                   allocator_arg_t,
                                   Variant_int_long::allocator_type,
                                   bsl::in_place_index_t<0>,
                                   MyClass2>::value));

    // no best conversion to an alternative
    struct FromInt1 {
        FromInt1(const int&){}  // IMPLICIT
    };

    struct FromInt2 {
        FromInt2(const int&){}  // IMPLICIT

        BSLMF_NESTED_TRAIT_DECLARATION(FromInt2, bslma::UsesBslmaAllocator);
    };

    typedef bsl::variant<FromInt1, FromInt2> Variant_FI1_FI2;
    ASSERT(!(std::is_constructible<Variant_FI1_FI2, int>::value));
    ASSERT(!(std::is_constructible<Variant_FI1_FI2,
                                   allocator_arg_t,
                                   Variant_FI1_FI2::allocator_type,
                                   int>::value));

    // deleted converting constructor
    struct FromInt3 {
        FromInt3(const int&) = delete;  // IMPLICIT
        BSLMF_NESTED_TRAIT_DECLARATION(FromInt3, bslma::UsesBslmaAllocator);
    };

    typedef bsl::variant<FromInt3> Variant_FI3;
    ASSERT(!(std::is_constructible<Variant_FI3,
                                   bsl::in_place_type_t<FromInt3>,
                                   int>::value));
    ASSERT(!(std::is_constructible<Variant_FI3,
                                   allocator_arg_t,
                                   Variant_FI3::allocator_type,
                                   bsl::in_place_type_t<FromInt3>,
                                   int>::value));

    ASSERT(!(std::is_constructible<Variant_FI3,
                                   bsl::in_place_index_t<0>,
                                   int>::value));
    ASSERT(!(std::is_constructible<Variant_FI3,
                                   allocator_arg_t,
                                   Variant_FI3::allocator_type,
                                   bsl::in_place_index_t<0>,
                                   int>::value));

    ASSERT(!(std::is_constructible<Variant_FI3, int>::value));
    ASSERT(!(std::is_constructible<Variant_FI3,
                                   allocator_arg_t,
                                   Variant_FI3::allocator_type,
                                   int>::value));

    bslma::TestAllocator             da("default", veryVeryVeryVerbose);
    bsl::variant<FromInt3, FromInt1> obj1{1};
    ASSERT(obj1.index() == 1);
    bsl::variant<FromInt3, FromInt1> obj2{bsl::allocator_arg, &da, 1};
    ASSERT(obj2.index() == 1);

    // narrowing conversion
    struct AA_Type {
        BSLMF_NESTED_TRAIT_DECLARATION(AA_Type, bslma::UsesBslmaAllocator);
    };

    typedef bsl::variant<int, AA_Type> Variant_int_AA;

    ASSERT(!(std::is_constructible<Variant_int_AA, double>::value));
    ASSERT(!(std::is_constructible<Variant_int_AA,
                                   allocator_arg_t,
                                   Variant_int_AA::allocator_type,
                                   double>::value));

    struct FromDouble {
        FromDouble(double, BloombergLP::bslma::Allocator * = nullptr) {}
                                                                    // IMPLICIT

        BSLMF_NESTED_TRAIT_DECLARATION(FromDouble, bslma::UsesBslmaAllocator);
    };
    bsl::variant<int, FromDouble> obj3{0.0};
    ASSERT(obj3.index() == 1);

    bsl::variant<int, FromDouble> obj4{bsl::allocator_arg, &da, 0.0};
    ASSERT(obj4.index() == 1);

#endif  // U_VARIANT_FULL_IMPLEMENTATION

    ASSERT(!(bsl::is_convertible<Unique_Variant, MyClass2>::value));
    ASSERT(!(bsl::is_convertible<CVUnique_Variant, MyClass2>::value));
    ASSERT(!(bsl::is_convertible<bsl::in_place_type_t<MyClass2>,
                                 Unique_Variant>::value));
}

template <class VARIANT, size_t t_NUM, class TAG>
void testCase5d_runTest()
{  // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED TAGGED INPLACE CONSTRUCTORS
    //   This test will ensure that the allocator extended tagged inplace
    //   constructors work as expected.
    //
    // Concerns:
    // 1. Calling an allocator extended  in_place_type_t<TYPE> tagged
    // constructor creates a
    //    `variant` with alternative type `TYPE` whose alternative
    //    object is created using the constructors arguments.
    //
    // 2. Arguments to the tagged constructor are forwarded in correct
    //    order to the alternative object's constructor.
    //
    // 3. Arguments to the tagged constructor are perfect forwarded to
    //    the alternative object's constructor, that is, rvalue argument is
    //    forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //    to the alternative object's constructor
    //
    // 4. If the alternative type uses an allocator, the `variant` object and
    //    the contained alternative object will use the specified allocator.
    //
    // 5. If a braced init list is used for the first argument after the
    //    tag, `variant` will deduce an initializer list and
    //    forward it to the `value_type`.
    //
    // 6. No unnecessary copies of the `value_type` are created.
    //
    // 7. The behaviour is the same when using bsl::in_place_index_t<INDEX>
    //    tagged constructor, except the alternative type is determined by
    //    the INDEX, representing the zero based index of the alternative
    //    type to create.
    //
    // Plan:
    // 1. Create an object of `TYPE` using a set of arguments `ARGS`.  Create
    //    an `variant` object containg `TYPE` as one of the alternatives by
    //    invoking the allocator extnded `in_place_type_t<TYPE>`
    //    constructor with the same arguments, but specifying a different
    //    allocator.  Verify that the resulting
    //    variant object has `TYPE` as the active alternative by calling
    //    `index()`, and check that the value of the managed alternative
    //    object matches the value of the reference `TYPE` object.  [C-1]
    //
    // 2. Repeat step 1 using different number of constructor arguments
    // [C-2].
    //
    // 3. Repeat steps 1 and 2 using a mixture of rvalue and lvalue arguments
    // such
    //    that each argument position is tested with an rvalue and an lvalue.
    //    Verify that the rvalue arguments have been moved from.  [C-3]
    //
    // 4. If `TYPE` is allocator-aware, verify that the specified allocator
    //    was used to construct the `variant` object and the managed
    //    alternative object.  [C-4]
    //
    // 5. Repeat steps 1-4 with an additional braced init list argument.
    // [C-5]
    //
    // 6. In steps 1-5, verify that no unnecessary copies of the TYPE object
    //    have been created.  [C-6]
    //
    // 7. Repeat steps 1-6 using `bsl::in_place_index_t<INDEX>` tag.
    //
    // Testing:
    //
    //    variant(alloc_arg, alloc, in_place_type, ARGS&&...);
    //    variant(alloc_arg, alloc, in_place_type, init_list, ARGS&&...);
    //    variant(alloc_arg, alloc, in_place_index, ARGS&&...);
    //    variant(alloc_arg, alloc, in_place_index, init_list, ARGS&&...);
    //
    // --------------------------------------------------------------------
    const size_t INDEX = t_NUM - 1;

    typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
    typedef bslalg::ConstructorProxy<AltType> ValWithAllocator;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose)
        printf("\tUsing variadic arguments.\n");

    {
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg, &oa, TAG()), (&ta), &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg, &oa, TAG(), MoveUtil::move(VA1)),
            (MoveUtil::move(VA1), &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg, &oa, TAG(), VA1), (VA1, &ta), &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg, &oa, TAG(), MoveUtil::move(VA1), VA2),
            (MoveUtil::move(VA1), VA2, &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg, &oa, TAG(), VA1, MoveUtil::move(VA2)),
            (VA1, MoveUtil::move(VA2), &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             TAG(),
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg, &oa, TAG(), VA1, MoveUtil::move(VA2), VA3),
            (VA1, MoveUtil::move(VA2), VA3, &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             TAG(),
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &ta),
            &oa);
        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             TAG(),
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             TAG(),
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
                               MoveUtil::move(VA1),
                               VA2,
                               MoveUtil::move(VA3),
                               VA4,
                               VA5,
                               VA6),
                              (MoveUtil::move(VA1),
                               VA2,
                               MoveUtil::move(VA3),
                               VA4,
                               MoveUtil::move(VA5),
                               VA6,
                               &ta),
                              &oa);
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
#ifdef U_VARIANT_FULL_IMPLEMENTATION
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
        printf("\tUsing `initializer_list` argument.\n");

    {
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg, &oa, TAG(), {1, 2, 3}),
                              (std::initializer_list<int>{1, 2, 3}, &ta),
                              &oa);

        TEST_TAGGED_CONSTRUCT(
            (bsl::allocator_arg, &oa, TAG(), {1, 2, 3}, MoveUtil::move(VA1)),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &ta),
            &oa);

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg, &oa, TAG(), {1, 2, 3}, VA1),
                              (std::initializer_list<int>{1, 2, 3}, VA1, &ta),
                              &oa);

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
                               {1, 2, 3},
                               MoveUtil::move(VA1),
                               VA2),
                              (std::initializer_list<int>{1, 2, 3},
                               MoveUtil::move(VA1),
                               VA2,
                               &ta),
                              &oa);
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
                               {1, 2, 3},
                               VA1,
                               MoveUtil::move(VA2)),
                              (std::initializer_list<int>{1, 2, 3},
                               VA1,
                               MoveUtil::move(VA2),
                               &ta),
                              &oa);

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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

        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
        TEST_TAGGED_CONSTRUCT((bsl::allocator_arg,
                               &oa,
                               TAG(),
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
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif  // U_VARIANT_FULL_IMPLEMENTATION
}

template <class VARIANT, size_t t_NUM>
struct testCase5dIndex_imp;

template <class VARIANT>
struct testCase5dIndex_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase5dIndex_imp : testCase5dIndex_imp<VARIANT, t_NUM - 1> {
    testCase5dIndex_imp()
    {
        const size_t INDEX = t_NUM - 1;
        testCase5d_runTest<VARIANT, t_NUM, bsl::in_place_index_t<INDEX> >();
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase5dIndex()
{
    testCase5dIndex_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class VARIANT, size_t t_NUM>
struct testCase5dType_imp;

template <class VARIANT>
struct testCase5dType_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase5dType_imp : testCase5dType_imp<VARIANT, t_NUM - 1> {
    testCase5dType_imp()
    {
        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
        testCase5d_runTest<VARIANT, t_NUM, bsl::in_place_type_t<AltType> >();
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase5dType()
{
    testCase5dType_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class VARIANT, class TAG, size_t t_NUM>
void testCase5c_runTest()
{
    // --------------------------------------------------------------------
    // TESTING TAGGED INPLACE CONSTRUCTORS
    //   This test will ensure that the tagged inplace constructors works
    //   as expected.
    //
    // Concerns:
    // 1. Calling a in_place_type_t<TYPE> tagged constructor creates a
    //    `variant` with alternative type `TYPE`, and whose alternative
    //    object is created using the constructors arguments.
    //
    // 2. Arguments to the tagged constructor are forwarded in correct
    //    order to the alternative object's constructor.
    //
    // 3. Arguments to the tagged constructor are perfect forwarded to
    //    the alternative object's constructor, that is, rvalue argument is
    //    forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //    to the alternative object's constructor
    //
    // 4. If the alternative type uses an allocator, the `variant` object and
    //    the contained alternative object will use the default allocator.
    //
    // 5. If a braced init list is used for the first argument after the
    //    tag, `variant` will deduce an initializer list and
    //    forward it to the `value_type`.
    //
    // 6. No unnecessary copies of the `value_type` are created.
    //
    // 7. The behaviour is the same when using bsl::in_place_index_t<INDEX>
    //    tagged constructor, except the alternative type is determined by
    //    the INDEX, representing the zero based index of the alternative
    //    type to create.
    //
    // Plan:
    // 1. Create an object of `TYPE` using a set of arguments `ARGS`.  Create
    //    an `variant` object containg `TYPE` as one of the alternatives by
    //    invoking the `in_place_type_t<TYPE>`
    //    constructor with the same arguments.  Verify that the resulting
    //    variant object has `TYPE` as the active alternative by calling
    //    `index()`, and check that the value of the managed alternative
    //    object matches the value of the reference `TYPE` object.  [C-1]
    //
    // 2. Repeat step 1 using different number of constructor arguments
    // [C-2].
    //
    // 3. Repeat steps 1 and 2 using a mixture of rvalue and lvalue arguments
    // such
    //    that each argument position is tested with an rvalue and an lvalue.
    //    Verify that the rvalue arguments have been moved from.  [C-3]
    //
    // 4. If `TYPE` is allocator-aware, verify that the default allocator
    //    was used to construct the `variant` object and the managed
    //    alternative object.  [C-4]
    //
    // 5. Repeat steps 1-4 with an additional braced init list argument.
    // [C-5]
    //
    // 6. In steps 1-5, verify that no unnecessary copies of the TYPE object
    //    have been created.  [C-6]
    //
    // 7. Repeat steps 1-6 using `bsl::in_place_index_t<INDEX>` tag.
    //
    // Testing:
    //
    //    variant(in_place_type, ARGS&&...);
    //    variant(in_place_type, init_list, ARGS&&...);
    //    variant(in_place_index, ARGS&&...);
    //    variant(in_place_index, init_list, ARGS&&...);
    //
    // --------------------------------------------------------------------

    const size_t INDEX = t_NUM - 1;

    typedef typename variant_alternative<INDEX, VARIANT>::type AltType;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose)
        printf("\tUsing variadic arguments.\n");
    /// Wrapper for `AltType` whose constructor takes an allocator.
    typedef bslalg::ConstructorProxy<AltType> ValWithAllocator;

    {
        TEST_TAGGED_CONSTRUCT((TAG()), (&oa), &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), MoveUtil::move(VA1)), (MoveUtil::move(VA1), &oa), &da);

        TEST_TAGGED_CONSTRUCT((TAG(), VA1), (VA1, &oa), &da);

        TEST_TAGGED_CONSTRUCT((TAG(), MoveUtil::move(VA1), VA2),
                              (MoveUtil::move(VA1), VA2, &oa),
                              &da);
        TEST_TAGGED_CONSTRUCT((TAG(), VA1, MoveUtil::move(VA2)),
                              (VA1, MoveUtil::move(VA2), &oa),
                              &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &oa),
            &da);

        TEST_TAGGED_CONSTRUCT((TAG(), VA1, MoveUtil::move(VA2), VA3),
                              (VA1, MoveUtil::move(VA2), VA3, &oa),
                              &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &oa),
            &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &oa),
            &da);

        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT(
            (TAG(), VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &oa),
            &da);

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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
#ifdef U_VARIANT_FULL_IMPLEMENTATION
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
        printf("\tUsing `initializer_list` argument.\n");

    {
        TEST_TAGGED_CONSTRUCT((TAG(), {1, 2, 3}),
                              (std::initializer_list<int>{1, 2, 3}, &oa),
                              &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), {1, 2, 3}, MoveUtil::move(VA1)),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &oa),
            &da);

        TEST_TAGGED_CONSTRUCT((TAG(), {1, 2, 3}, VA1),
                              (std::initializer_list<int>{1, 2, 3}, VA1, &oa),
                              &da);

        TEST_TAGGED_CONSTRUCT((TAG(), {1, 2, 3}, MoveUtil::move(VA1), VA2),
                              (std::initializer_list<int>{1, 2, 3},
                               MoveUtil::move(VA1),
                               VA2,
                               &oa),
                              &da);
        TEST_TAGGED_CONSTRUCT((TAG(), {1, 2, 3}, VA1, MoveUtil::move(VA2)),
                              (std::initializer_list<int>{1, 2, 3},
                               VA1,
                               MoveUtil::move(VA2),
                               &oa),
                              &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), {1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
            (std::initializer_list<int>{1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             &oa),
            &da);

        TEST_TAGGED_CONSTRUCT(
            (TAG(), {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
            (std::initializer_list<int>{1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             &oa),
            &da);

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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

        TEST_TAGGED_CONSTRUCT((TAG(),
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
        TEST_TAGGED_CONSTRUCT((TAG(),
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
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif  // U_VARIANT_FULL_IMPLEMENTATION
}

template <class VARIANT, size_t t_NUM>
struct testCase5cIndex_imp;

template <class VARIANT>
struct testCase5cIndex_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase5cIndex_imp : testCase5cIndex_imp<VARIANT, t_NUM - 1> {
    testCase5cIndex_imp()
    {
        const size_t INDEX = t_NUM - 1;
        testCase5c_runTest<VARIANT, bsl::in_place_index_t<INDEX>, t_NUM>();
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase5cIndex()
{
    testCase5cIndex_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class VARIANT, size_t t_NUM>
struct testCase5cType_imp;

template <class VARIANT>
struct testCase5cType_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase5cType_imp : testCase5cType_imp<VARIANT, t_NUM - 1> {
    testCase5cType_imp()
    {
        const size_t INDEX = t_NUM - 1;
        typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
        testCase5c_runTest<VARIANT, bsl::in_place_type_t<AltType>, t_NUM>();
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase5cType()
{
    testCase5cType_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class VARIANT, size_t t_NUM>
struct testCase5b_imp;
template <class VARIANT>
struct testCase5b_imp<VARIANT, 0> {
};

template <class VARIANT, size_t t_NUM>
struct testCase5b_imp : testCase5b_imp<VARIANT, t_NUM - 1> {
    testCase5b_imp()
    {
        // TESTING ALLOCATOR EXTENDED CONSTRUCTION FROM A VALUE
        //   This test will ensure that the allocator extended construction
        //   from a
        //   value of alternative type works as expected.
        //
        // Concerns:
        // 1. Constructing a `variant` from an object of alternative type
        // `AltType`
        //   creates a `variant` with the 'AltType as an active alternative.
        //
        // 2. The alternative object has been copy constructed from the source
        //    object, move construction of the source object is used when
        // possible,
        //    and no unnecessary copies of the `AltType` are created.
        //
        // 3. The newly created `variant` object will use the allocator
        //    specified in the constructor call.
        //
        // Plan:
        //
        // 1. Create an `AltType` object and use it as the source object for a
        //    `variant` containing `AltType`.  Verify that the constructed
        // `variant`
        //    object has `AltType` alternative active. [C-1]
        //
        // 2. Verify that the allocator specified in the constructor call was
        // used
        //    to construct the `variant` and its `AltType` object. [C-3]
        //
        // 3. In steps 1-2, verify that the alternative object has been
        // copy/move
        //    constructed from the source object and that no unnecessary copies
        // of
        //    the `AltType` are created by comparing the number of copy/move
        //    constructors invoked to creating an instance of `AltType` object
        //    from the source object [C-2]
        //
        // 4. Repeat steps 1-3 using a const source object, an rvalue source
        //    object, and a const rvalue source object.  [C-2]
        //
        // Testing:
        //
        //    variant(alloc_arg, alloc, TYPE&&);
        // --------------------------------------------------------------------

        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            const size_t INDEX = t_NUM - 1;
            typedef typename variant_alternative<INDEX, VARIANT>::type AltType;

            {
                typedef bslalg::ConstructorProxy<AltType> SourceWithAllocator;
                SourceWithAllocator sourceBuffer(21, &oa);
                AltType&            source  = sourceBuffer.object();
                const AltType&      csource = sourceBuffer.object();

                TEST_AE_COPY_FROM_VALUE(source);
                TEST_AE_COPY_FROM_VALUE(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                // C++03 MovableRef isn't const friendly, which will make these
                // tests fail
                TEST_AE_COPY_FROM_VALUE(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                TEST_AE_MOVE_FROM_VALUE(source);
            }
        }
    }
};

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase5b()
{
    testCase5b_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class VARIANT, size_t t_NUM>
struct testCase5a_imp;

template <class VARIANT, bool USES_BSLMA_ALLOC>
void TestDriver<VARIANT, USES_BSLMA_ALLOC>::testCase5a()
{
    // testing construction from an alternative type only.
    testCase5a_imp<VARIANT, bsl::variant_size<VARIANT>::value> test;
}

template <class VARIANT>
struct testCase5a_imp<VARIANT, 0> {
};
template <class VARIANT, size_t t_NUM>
struct testCase5a_imp : testCase5a_imp<VARIANT, t_NUM - 1> {
    testCase5a_imp()
    {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTION FROM A VALUE
        //   This test will ensure that the construction from a value of an
        //   alternative type works as expected.
        //
        // Concerns:
        // 1. Constructing a `variant` from an object of alternative type
        // `AltType`
        //    creates a `variant` with the 'AltType as an active alternative.
        //
        // 2. The alternative object has been copy constructed from the source
        //    object, move construction of the source object is used when
        // possible,
        //    and no unnecessary copies of the `AltType` are created..
        //
        // 3. If no allocator is provided and the variant type uses allocator,
        // the
        //    default allocator is used for the newly created `variant`.
        //
        // Plan:
        //
        // 1. Create an `AltType` object and use it as the source object for a
        //    `variant` containing `AltType`.  Verify that the constructed
        // `variant`
        //    object has `AltType` alternative active. [C-1]
        //
        // 2. If `value_type` is allocator-aware, verify that the allocator of
        // the new
        //    `variant` object is the default allocator [C-3]
        //
        // 3. In steps 1-2, verify that the alternative object has been
        // copy/move
        //    constructed from the source object and that no unnecessary copies
        // of
        //    the `AltType` are created by comparing the number of copy/move
        //    constructors invoked to creating an instance of `AltType` object
        //    from the source object [C-2]
        //
        // 4. Repeat steps 1-3 using a const source object, an rvalue source
        //    object, and a const rvalue source object.  [C-2]
        //
        // Testing:
        //
        //    variant(TYPE&&);
        // --------------------------------------------------------------------

        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            const size_t INDEX = t_NUM - 1;
            typedef typename variant_alternative<INDEX, VARIANT>::type AltType;
            {
                typedef bslalg::ConstructorProxy<AltType> SourceWithAllocator;
                SourceWithAllocator sourceBuffer(21, &oa);
                AltType&            source  = sourceBuffer.object();
                const AltType&      csource = sourceBuffer.object();

                TEST_COPY_FROM_VALUE(source);
                TEST_COPY_FROM_VALUE(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                // C++03 MovableRef isn't const friendly, which will make these
                // tests fail
                TEST_COPY_FROM_VALUE(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                TEST_MOVE_FROM_VALUE(source);
            }
        }
    }
};

void testCase4c()
{
    // --------------------------------------------------------------------
    // DEFAULT CONSTRUCTIBILITY
    //   This test will ensure that default constructibility is implemented
    //   as specified in the standar.
    //
    // Concerns:
    // 1. That the variant with default constructible alternative at index 0
    //    is default constructible.
    //
    // 2. That the variant with an alternative at index 0 that is not default
    //    constructible alternative, is not default constructible.
    //
    // 3. That the same rules apply to allocator extended default constructor.
    //
    // Plan:
    // 1. Check `is_default_constructible` trait for a variant with a default
    //    constructible alternative at index 0. [C-1]
    //
    // 2. Check `is_default_constructible` trait for a variant with a
    //    non default constructible alternative at index 0. [C-2]
    //
    // 3. Check that a variant with a default constructible alternative at
    //    index 0 can be constructed from `allocator_arg_t` and an allocator
    //    type. [C-3]
    //
    // 4. Check that a variant with a non default constructible alternative at
    //    index 0 can not be constructed from `allocator_arg_t` and an
    //    allocator type. [C-3]
    //
    // Testing:
    //   variant();
    //   variant(bsl::allocator_arg_t, allocator_type);
    // --------------------------------------------------------------------

#ifdef U_VARIANT_FULL_IMPLEMENTATION
    ASSERT((std::is_default_constructible<
            bsl::variant<int,
                         bsltf::NonDefaultConstructibleTestType> >::value));
    ASSERT(!(std::is_default_constructible<
             bsl::variant<bsltf::NonDefaultConstructibleTestType,
                          int> >::value));

    ASSERT((std::is_constructible<
            bsl::variant<MyClass2, bsltf::NonDefaultConstructibleTestType>,
            bsl::allocator_arg_t,
            bsl::variant<MyClass2, bsltf::NonDefaultConstructibleTestType>::
                allocator_type>::value));
    ASSERT(!(std::is_constructible<
             bsl::variant<bsltf::NonDefaultConstructibleTestType, MyClass2>,
             bsl::allocator_arg_t,
             bsl::variant<MyClass2, bsltf::NonDefaultConstructibleTestType>::
                 allocator_type>::value));
#else
    bsl::variant<MyClass2, bsltf::NonDefaultConstructibleTestType> obj1;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bsl::variant<MyClass2, bsltf::NonDefaultConstructibleTestType> obj2(
        bsl::allocator_arg, &da);

#endif  // U_VARIANT_FULL_IMPLEMENTATION
}

template <class TYPE, bool USES_BSLMA_ALLOC>
void TestDriver<TYPE, USES_BSLMA_ALLOC>::testCase4b()
{
    // --------------------------------------------------------------------
    // ALLOCATOR EXTENDED DEFAULT CONSTRUCTORS AND DESTRUCTOR
    //   This test will ensure that the allocator extended default construction
    //   of a `variant` is working as expected.
    //
    // Concerns:
    // 1. That the allocator extended  default constructor creates the
    //    alternative at index `0`, as determined by `index()` returning `0`.
    //
    // 2. That the `get_allocator` method returns the allocator passed in to
    //    the constructors.
    //
    // 3. When the variant object is destroyed, the destructor of the active
    //    alternative type is invoked.
    //
    // Plan:
    // 1. Construct a `variant` object using default construction and verify
    //    that the alternative at index `0` has been created. [C-1]
    //
    // 2. In step 1, verify that the `get_allocator` method returns the
    //    allocator used in `variant` construction. [C-2]
    //
    // 3. in steps 1 and 2, verify that the selected alternative type
    //    destructor is invoked when the variant object is destroyed. [C-3]
    //
    // Testing:
    //   variant(bsl::allocator_arg_t, allocator_type);
    //   allocator_type get_allocator() const;
    // --------------------------------------------------------------------

    {
        typedef typename variant_alternative<0, VariantType>::type AltType;
        int                  dI = AltType::s_destructorInvocations;
        int                  cI = AltType::s_defaultConstructorInvocations;
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor oam(&oa);

            VariantType X(bsl::allocator_arg, &oa);

            ASSERT(!X.valueless_by_exception());
            ASSERT(X.index() == 0);
            ASSERT(checkAllocator(X, &oa));
            ASSERT(checkAllocator(bsl::get<0>(X), &oa));
            ASSERT(cI + 1 == AltType::s_defaultConstructorInvocations);
            dI = AltType::s_destructorInvocations;
        }
        ASSERT(dI + 1 == AltType::s_destructorInvocations);
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
void TestDriver<TYPE, USES_BSLMA_ALLOC>::testCase4a()
{
    // --------------------------------------------------------------------
    // DEFAULT CONSTRUCTORS AND DESTRUCTOR
    //   This test will ensure that the default construction of a `variant`
    //   is working as expected.
    //
    // Concerns:
    // 1. That the default constructor creates the alternative at index `0`, as
    //    determined by `index()` returning `0`.
    //
    // 2. If the selected alternative type is allocator-aware (AA), then
    //    the `get_allocator` method returns the default allocator when using
    //    these constructors.
    //
    // 3. When the variant object is destroyed, the destructor of the active
    //    alternative type is invoked.
    //
    // Plan:
    // 1. Construct a `variant` object using default construction and verify
    //    that the alternative at index `0` has been created. [C-1]
    //
    // 2. In steps 1, if the selected alternative type is allocator-aware,
    //    verify that the `get_allocator` method returns the default allocator
    // [C-2]
    //
    // 3. in steps 1 and 2, verify that the selected alternative type
    //    destructor is invoked when the variant object is destroyed. [C-3]
    //
    //
    // Testing:
    //   variant();
    //   ~variant();
    // --------------------------------------------------------------------

    {
        typedef typename variant_alternative<0, VariantType>::type AltType;
        int                  dI = AltType::s_destructorInvocations;
        int                  cI = AltType::s_defaultConstructorInvocations;
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        {
            bslma::TestAllocatorMonitor dam(&da);

            VariantType        mX;
            const VariantType& X = mX;

            ASSERT(!X.valueless_by_exception());
            ASSERT(!mX.valueless_by_exception());
            ASSERT(mX.index() == 0);
            ASSERT(checkAllocator(X, &da));
            ASSERT(checkAllocator(bsl::get<0>(X), &da));
            ASSERT(cI + 1 == AltType::s_defaultConstructorInvocations);
            dI = AltType::s_destructorInvocations;
        }
        ASSERT(dI + 1 == AltType::s_destructorInvocations);
    }
}

void testCase3()
{
    // --------------------------------------------------------------------
    // TESTING HELPER METAFUNCTIONS
    //
    //
    // Concerns:
    // 1. That `variant_size<VARIANT>::value` and `variant_size_v<VARIANT>`
    //    reflect the number of alternative types in a specified `variant`
    //    type
    //
    // 2. That `variant_alternative` and `variant_alternative_t` give the
    //    alternative type of the specified `variant` type at the specified
    //    index.
    //
    // 3. That all above metafunctions work for a const qualified variant, and
    //    that the returned alternative type is appropriately qualified for
    //    the given `variant` type.
    //
    // 4. That the type `bsl::variant_npos` is `size_t` and the value is `-1`.
    //
    // Plan:
    // 1. For various `variant` types verify the `variant_size<VARIANT>::value`
    //    and `variant_size_v<VARIANT>` reflect the number of alternative
    //    types. [C-1]
    //
    // 2. For various combination of `variant` types and indices verify that
    //    the  `variant_alternative` and `variant_alternative_t` give the
    //    alternative type of the specified `variant` type at the specified
    //    index. [C-2]
    //
    // 3. Repeat steps 1-2 with const qualified `variant` type. [C-3]
    //
    // 4. Verify the type and value of `bsl::variant_npos`. [C-4]
    //
    // Testing:
    //
    //     struct variant_size<VARIANT>;
    //     size_t variant_size_v<VARIANT>;
    //     struct variant_alternative<I, VARIANT>;
    //     typedef variant_alternative_t<INDEX,VARIANT>;
    //     size_t variant_npos;
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING HELPER METAFUNCTIONS"
               "n=============================\n");

    {
        typedef bsl::variant<int>                             Var1;
        typedef bsl::variant<int, char, long>                 Var2;
        typedef bsl::variant<int, char, long, const MyClass1> Var3;

        typedef const Var1 CVar1;
        typedef const Var2 CVar2;
        typedef const Var3 CVar3;

        ASSERT(bsl::variant_size<Var1>::value == 1);
        ASSERT(bsl::variant_size<Var2>::value == 3);
        ASSERT(bsl::variant_size<Var3>::value == 4);

        ASSERT(bsl::variant_size<CVar1>::value == 1);
        ASSERT(bsl::variant_size<CVar2>::value == 3);
        ASSERT(bsl::variant_size<CVar3>::value == 4);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
        ASSERT(bsl::variant_size_v<Var1> == 1);
        ASSERT(bsl::variant_size_v<Var2> == 3);
        ASSERT(bsl::variant_size_v<Var3> == 4);

        ASSERT(bsl::variant_size_v<CVar1> == 1);
        ASSERT(bsl::variant_size_v<CVar2> == 3);
        ASSERT(bsl::variant_size_v<CVar3> == 4);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

        ASSERT(
          (bsl::is_same<bsl::variant_alternative<0, Var1>::type, int>::value));
        ASSERT(
          (bsl::is_same<bsl::variant_alternative<0, Var2>::type, int>::value));
        ASSERT(
         (bsl::is_same<bsl::variant_alternative<1, Var2>::type, char>::value));
        ASSERT(
         (bsl::is_same<bsl::variant_alternative<2, Var2>::type, long>::value));
        ASSERT(
          (bsl::is_same<bsl::variant_alternative<0, Var3>::type, int>::value));
        ASSERT(
         (bsl::is_same<bsl::variant_alternative<1, Var3>::type, char>::value));
        ASSERT(
         (bsl::is_same<bsl::variant_alternative<2, Var3>::type, long>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<3, Var3>::type,
                             const MyClass1>::value));

        ASSERT((bsl::is_same<bsl::variant_alternative<0, CVar1>::type,
                             const int>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<0, CVar2>::type,
                             const int>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<1, CVar2>::type,
                             const char>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<2, CVar2>::type,
                             const long>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<0, CVar3>::type,
                             const int>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<1, CVar3>::type,
                             const char>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<2, CVar3>::type,
                             const long>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative<3, CVar3>::type,
                             const MyClass1>::value));

#ifdef U_VARIANT_FULL_IMPLEMENTATION
        ASSERT(
              (bsl::is_same<bsl::variant_alternative_t<0, Var1>, int>::value));
        ASSERT(
              (bsl::is_same<bsl::variant_alternative_t<0, Var2>, int>::value));
        ASSERT(
             (bsl::is_same<bsl::variant_alternative_t<1, Var2>, char>::value));
        ASSERT(
             (bsl::is_same<bsl::variant_alternative_t<2, Var2>, long>::value));
        ASSERT(
              (bsl::is_same<bsl::variant_alternative_t<0, Var3>, int>::value));
        ASSERT(
             (bsl::is_same<bsl::variant_alternative_t<1, Var3>, char>::value));
        ASSERT(
             (bsl::is_same<bsl::variant_alternative_t<2, Var3>, long>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<3, Var3>,
                             const MyClass1>::value));

        ASSERT((bsl::is_same<bsl::variant_alternative_t<0, CVar1>,
                             const int>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<0, CVar2>,
                             const int>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<1, CVar2>,
                             const char>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<2, CVar2>,
                             const long>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<0, CVar3>,
                             const int>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<1, CVar3>,
                             const char>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<2, CVar3>,
                             const long>::value));
        ASSERT((bsl::is_same<bsl::variant_alternative_t<3, CVar3>,
                             const MyClass1>::value));

#endif  // U_VARIANT_FULL_IMPLEMENTATION

        ASSERT(checkVariantNpos(bsl::variant_npos));
        ASSERT(int(bsl::variant_npos) == -1);
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
void TestDriver<TYPE, USES_BSLMA_ALLOC>::testCase2()
{
    // --------------------------------------------------------------------
    // TESTING TRAITS AND TYPEDEFS
    //
    //
    // Concerns:
    // 1. That `bslma::UsesBslmaAllocator<variant<TYPE>>` and
    //    `bslmf::UsesAllocatorArgT<variant<TYPE>>` are `false`
    //    if all alternatives are non allocator-aware.
    //
    // 2. That `bslma::UsesBslmaAllocator<variant<TYPE>>` and
    //    `bslmf::UsesAllocatorArgT<variant<TYPE>>` are `true`
    //    if at least one alternative is an allocator-aware type.
    //
    // 3. That `variant<ARGS...>::allocator_type` is `bsl::allocator<char>` if
    //    if at least one alternative is an allocator-aware type.
    //
    // Plan:
    // 1. Verify that both `UsesBslmaAllocator` and
    //    `UsesAllocatorArgT` traits for `TYPE` are true if `USES_BSLMA_ALLOC`
    //    is `true`. [C-1]
    //
    // 2. Verify that both `UsesBslmaAllocator` and
    //    `UsesAllocatorArgT` traits for `TYPE` are false if `USES_BSLMA_ALLOC`
    //    is "false". [C-4]
    //
    // 3. Verify that `variant<ARGS...>::allocator_type` is
    //    `bsl::allocator<char>` if `USES_BSLMA_ALLOC` is "true". [C-3]
    //
    // Testing:
    //
    //      bslma::UsesBslmaAllocator<variant>
    //      bslma::UsesAllocatorArgT<variant>
    //      typename bsl::allocator<char> allocator_type
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING TRAITS AND TYPEDEFS"
               "n===========================\n");

    ASSERT(bslma::UsesBslmaAllocator<TYPE>::value == USES_BSLMA_ALLOC);
    ASSERT(bslmf::UsesAllocatorArgT<TYPE>::value == USES_BSLMA_ALLOC);
    ASSERT((bsl::is_same<typename TYPE::allocator_type,
                         bsl::allocator<char> >::value == USES_BSLMA_ALLOC));
}

void testCase2a()
{
    // --------------------------------------------------------------------
    // TESTING `IsBitwiseMoveable` TRAIT
    //
    //
    // Concerns:
    // 1. That `bslmf::IsBitwiseMoveable<variant<TYPE>>` is `true`
    //    if all alternatives are bitwise moveable, and `false` otherwise.
    //
    // Plan:
    // 1. For different combination of bitwise moveable and non bitwise
    //    moveableVerify alternatives, verify that
    //    `bslmf::IsBitwiseMoveable<variant<TYPE>>` is as expected. [C-1]
    //
    // Testing:
    //
    //      bslmf::IsBitwiseMoveable<variant>
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING `IsBitwiseMoveable` TRAIT"
               "n==================================\n");

    ASSERT((bslmf::IsBitwiseMoveable<variant<int> >::value));
    ASSERT((bslmf::IsBitwiseMoveable<variant<int, long, char> >::value));

    ASSERT(!(bslmf::IsBitwiseMoveable<MyClass1>::value));
    ASSERT(!(bslmf::IsBitwiseMoveable<variant<MyClass1> >::value));
    ASSERT(!(bslmf::IsBitwiseMoveable<variant<int, MyClass1, char> >::value));
}

namespace usage_example_1 {

class MyVisitor {
  public:
    template <class t_TYPE>
    void operator()(const t_TYPE& value) const
    {
        if (verbose)  // ADDED
        std::cout << value << std::endl;
    }
};

}  // namespace usage_example_1

namespace usage_example_2 {

struct S {
    S(int i) : d_i(i) {}
    int d_i;
};

}  // namespace usage_example_2

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

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nUSAGE EXAMPLE"
                   "\n=============\n");

        using namespace usage_example_1;
        using namespace usage_example_2;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Variant Use
/// - - - - - - - - - - - - - -
// First, we create a `variant` object that can hold an integer, a char, or
// a string.  The default constructor of `bsl::variant<TYPES...>` creates the
// first alternative in `TYPES...`; to create a different alternative, we can
// provide the index or the type of the alternative to create:
// ```
    bsl::variant<int, char> v1;
    bsl::variant<int, char> v2(bsl::in_place_type_t<char>(), 'c');

    ASSERT(bsl::holds_alternative<int>(v1));
    ASSERT(bsl::holds_alternative<char>(v2));
// ```
// Next, we create a visitor that can be called with a value of any of the
// alternatives:
// ```
//  class MyVisitor {
//    public:
//      template <class t_TYPE>
//      void operator()(const t_TYPE& value) const
//      {
//          bsl::cout << value << bsl::endl;
//      }
//  };
// ```
//  We can now use `bsl::visit` to apply the visitor to our variant objects:
// ```
    MyVisitor visitor;
    bsl::visit(visitor, v1);  // prints integer 0
    bsl::visit(visitor, v2);  // prints char 'c'
// ```
//  To retrieve a contained value, we can use the `get` free functions.  If the
//  requested alternative is not the currently active alternative, an exception
//  of type `bsl::bad_variant_access` will be thrown.
// ```
    ASSERT(0 == bsl::get<int>(v1));
    ASSERT('c' == bsl::get<1>(v2));
    try {
        bsl::get<int>(v2);
    } catch (const bsl::bad_variant_access& ex) {
        if (verbose)  // ADDED
        std::cout << "non-active alternative requested" << std::endl;
    }
// ```
//
///Example 2: Variant Default Construction
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to default construct a `bsl::variant` which can hold an
// alternative of type `S`.  Type `S` is not default constructible so we use
// `bsl::monostate`as the first alternative to allow for default construction
// of the variant object.
// ```
//  struct S
//  {
//      S(int i) : d_i(i) {}
//      int d_i;
//  };
//
    bsl::variant<bsl::monostate, S> v3;
// ```
//  To create an alternative of type `S`. we can use the emplace method.
// ```
    v3.emplace<S>(3);
    ASSERT(bsl::holds_alternative<S>(v3));
// ```

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // SFINAE FOR `get`
        //
        // Concerns:
        // 1. `bsl::get` for `bsl::variant` does not participate in overload
        //    resolution unless the argument's type is (possibly cv-qualified)
        //    `bsl::variant` or derived therefrom.  In particular, SunCC has a
        //    bug in which the call `get<N>(t)`, where `t` has a dependent
        //    type, triggers argument-dependent lookup (ADL).  (Note that ADL
        //    should not take place in this call until C++20.)  This bug then
        //    triggers another bug in which certain invalid types are not
        //    treated as substitution failures, resulting in a hard error when
        //    `bsl::get` is in the overload set.
        //
        // Plan:
        // 1. Define a class template, `T`, that can take a `bsl::variant` type
        //    as a template argument.  (This is needed to trigger ADL that will
        //    look in the `bsl` namespace.)
        //
        // 2. Define two function templates named `get` that can accept a
        //    function argument of any specialization of `T`.  One `get`
        //    template takes a template argument of type `size_t`, and the
        //    other takes a type template argument.
        //
        // 3. Define another function template, `m`, that calls `get<0>` and
        //    `get<int>` on its argument.
        //
        // 4. Create an object `t` of type `T<bsl::variant<int> >`.  (This will
        //    make `bsl` an associated namespace for ADL calls that have `t` as
        //    an argument.)
        //
        // 5. Call `m(t)` and verify that the function templates defined in P-2
        //    were called.  (C-1)
        //
        // Testing:
        //   CONCERN: SFINAE for `get` works on Solaris (DRQS 175366735)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nSFINAE FOR `get`"
                   "\n================\n");

        using namespace test_case_16;

        T<bsl::variant<int> > t;
        ASSERT(m(t));
      } break;
      case 15: {
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
        //   Reserved for `bslx` streaming.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBSLX STREAMING"
                   "\n==============\n");

        if (verbose)
            printf("Not implemented.\n");
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an `ostream` in some standard, human-readable form.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for `bslx` streaming.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nPRINT AND OUTPUT OPERATOR"
                   "\n=========================\n");

        if (verbose)
            printf("Not implemented.\n");

      } break;
      case 13: {
        RUN_EACH_TYPE(TestDriver, testCase13, BSLSTL_VARIANT_TEST_TYPES_HASH);

        if (verbose)
            printf("\nTESTING `hashAppend` OF EMPTY VARIANT"
                   "\n=====================================\n");
        testCase13a();
      } break;
      case 12: {
        if (verbose)
            printf("\nTESTING VISIT "
                   "\n=============\n");

        if (verbose)
            printf("\nTESTING VISIT WITHOUT SPECIFIED RETURN TYPE"
                   "\n===========================================\n");
        testCase12a();

        if (verbose)
            printf("\nTESTING VISIT WITH SPECIFIED RETURN TYPE"
                   "\n===========================================\n");
        testCase12b();

      } break;
      case 11: {
        if (verbose)
            printf("\nTESTING `operator=(variant)`"
                   "\n============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase11a,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_NONUNIQUE);

        if (verbose)
            printf("\nTESTING `operator=(ANY_TYPE)`"
                   "\n==============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase11b,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING);

        if (verbose)
            printf("\nTESTING `operator=` with empty variant"
                   "\n======================================\n");
        testCase11c();
        testCase11d();
        testCase11e();
      } break;
      case 10: {
        if (verbose)
            printf("\nTESTING `swap`"
                   "\n==============\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase10,
                      BSLSTL_VARIANT_TEST_TYPES_SWAP,
                      BSLSTL_VARIANT_TEST_TYPES_SWAP_NONUNIQUE);

        if (verbose)
            printf("\nTESTING `swap` OF EMPTY VARIANT"
                   "\n================================\n");
        testCase10a();
      } break;
      case 9: {
        if (verbose)
            printf("\nTESTING COPY/MOVE CONSTRUCTION "
                   "\n==============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase9a,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);
        if (verbose)
            printf("\nTESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION"
                   "\n=================================================\n");
        RUN_EACH_TYPE(
                   TestDriver,
                   testCase9b,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_NONUNIQUE,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_CVNONUNIQUE);

        if (verbose)
            printf("\nTESTING CONSTRUCTION FROM EMPTY VARIANT"
                   "\n=======================================\n");
        testCase9c();

        if (verbose)
            printf("\nTESTING CONSTRUCTIBLE AND CONVERTIBLE TRAITS"
                   "\n============================================\n");
        testCase9d();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        if (verbose)
            printf("\nTESTING CONSTRUCTION FROM `std::variant`"
                   "\n========================================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase9e,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);
        if (verbose)
            printf(
                "\nTESTING ALLOCATOR-EXTENDED CONSTRUCTION FROM `std::variant`"
                "\n==========================================================="
                "\n");
        RUN_EACH_TYPE(
                   TestDriver,
                   testCase9f,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_NONUNIQUE,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_CVNONUNIQUE);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      } break;
      case 8: {
        if (verbose)
            printf("\nTESTING RELATIONAL OPERATORS"
                   "\n============================\n");
        testCase8();
      } break;
      case 7: {
        if (verbose)
            printf("\nTESTING `emplace` METHOD"
                   "\n========================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase7Index,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_CVNONUNIQUE);
        RUN_EACH_TYPE(TestDriver,
                      testCase7Type,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_CVNONUNIQUE);

        if (verbose)
            printf("\nTESTING `emplace` METHOD IN PRESENCE OF EXCEPTION"
                   "\n================================================\n");
        testCase7c<MyClass1>();
        testCase7c<MyClass2>();

        if (verbose)
            printf("\nTESTING `emplace` CONSTRAINTS"
                   "\n================================================\n");

        testCase7d();

      } break;
      case 6: {
        if (verbose)
            printf("\nTESTING `index`"
                   "\n===============\n");
        testCase6e();

        if (verbose)
            printf("\nTESTING `valueless_by_exception`"
                   "\n================================\n");
        testCase6d();

        if (verbose)
            printf("\nTESTING `holds_alternative` FREE FUNCTION"
                   "\n=========================================\n");
        {
            RUN_EACH_TYPE(
                      TestDriver,
                      testCase6c,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);
        }
        if (verbose)
            printf("\nTESTING `get_if` FREE FUNCTION"
                   "\n==============================\n");
        {
            RUN_EACH_TYPE(
                      TestDriver,
                      testCase6bIndex,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);
            RUN_EACH_TYPE(
                      TestDriver,
                      testCase6bType,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);

            testCase6b1();
        }

        if (verbose)
            printf("\nTESTING `get` FREE FUNCTION"
                   "\n===========================\n");
        {
            RUN_EACH_TYPE(
                      TestDriver,
                      testCase6aIndex,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);
            RUN_EACH_TYPE(
                      TestDriver,
                      testCase6aType,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVNONUNIQUE);

            testCase6a1();
            testCase6a2();
        }
      } break;
      case 5: {
        if (verbose)
            printf("\nTESTING CONSTRUCTION FROM A VALUE"
                   "\n=================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase5a,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING,
                      BSLSTL_VARIANT_TEST_TYPES_INSTANCE_COUNTING_CVQUALIFIED);

        if (verbose)
            printf("\nTESTING ALLOCATOR EXTENDED CONSTRUCTION FROM A VALUE"
                   "\n===================================================="
                   "\n");
        RUN_EACH_TYPE(
                   TestDriver,
                   testCase5b,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING,
                   BSLSTL_VARIANT_TEST_TYPES_AA_INSTANCE_COUNTING_CVQUALIFIED);

        if (verbose)
            printf("\nTESTING INPLACE CONSTRUCTORS "
                   "\n============================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase5cIndex,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_CVNONUNIQUE);
        RUN_EACH_TYPE(TestDriver,
                      testCase5cType,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS,
                      BSLSTL_VARIANT_TEST_TYPES_VARIADIC_ARGS_CVNONUNIQUE);

        if (verbose)
            printf("\nTESTING ALLOCATOR EXTENDED INPLACE CONSTRUCTORS"
                   "\n===============================================\n");
        RUN_EACH_TYPE(TestDriver,
                      testCase5dIndex,
                      BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS,
                      BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_CVNONUNIQUE);
        RUN_EACH_TYPE(TestDriver,
                      testCase5dType,
                      BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS,
                      BSLSTL_VARIANT_TEST_TYPES_AA_VARIADIC_ARGS_CVNONUNIQUE);

        testCase5e();
      } break;
      case 4: {
        if (verbose)
            printf("\nTESTING DEFAULT CONSTRUCTORS AND DESTRUCTOR"
                   "\n==========================================="
                   "\n");

        RUN_EACH_TYPE(
                    TestDriver,
                    testCase4a,
                    BSLSTL_VARIANT_TEST_TYPES_DEFAULT_CONSTRUCTOR,
                    BSLSTL_VARIANT_TEST_TYPES_DEFAULT_CONSTRUCTOR_NONUNIQUE,
                    BSLSTL_VARIANT_TEST_TYPES_DEFAULT_CONSTRUCTOR_CVNONUNIQUE);

        RUN_EACH_TYPE(
                 TestDriver,
                 testCase4b,
                 BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR,
                 BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR_NONUNIQUE,
                 BSLSTL_VARIANT_TEST_TYPES_DEFAULT_AE_CONSTRUCTOR_CVNONUNIQUE);

        testCase4c();
      } break;
      case 3: {
        if (verbose)
            printf("\nTESTING HELPER METAFUNCTIONS"
                   "\n===========================\n");

        testCase3();
      } break;
      case 2: {
        if (verbose)
            printf("\nTESTING TYPE TRAITS"
                   "\n===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLSTL_VARIANT_TEST_TYPES,
                      BSLSTL_VARIANT_TEST_TYPES_NONUNIQUE,
                      BSLSTL_VARIANT_TEST_TYPES_CVNONUNIQUE);
        testCase2a();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create a `bsl::variant<int, char>` object.
        //
        // 2. Verify that the alternative with index 0 has the value 0.
        //
        // 3. Assign the value `x` to the variant object.
        //
        // 4. Verify that the alternative with index 1 has the value `x`.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        variant<int, char> v;
        ASSERT(0 == get<0>(v));
        v = 'x';
        ASSERT('x' == get<1>(v));
      } break;
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
// Copyright 2023 Bloomberg Finance L.P.
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
