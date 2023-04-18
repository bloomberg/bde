// bslstl_optional.h                                                  -*-C++-*-

#ifndef INCLUDED_BSLSTL_OPTIONAL
#define INCLUDED_BSLSTL_OPTIONAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant allocator aware optional type.
//
//@CLASSES:
//  bsl::optional: template class for optional objects
//
//@CANONICAL_HEADER: bsl_optional.h
//
//@DESCRIPTION: This component provides a template class,
// 'bsl::optional<TYPE>', that implements a notion of object that may or may
// not contain a 'TYPE' value.  This template class also provides an interface
// to check if the optional object contains a value or not, as well as a
// contextual conversion to 'bool'.  If an optional object is engaged, i.e.
// contains a value, it will evaluate to 'true' when converted to 'bool';
// otherwise, it will evaluate to 'false' when converted to 'bool'.
//
// An optional object is engaged if it has been initialized with, or assigned
// from an object of 'TYPE' or another engaged optional object, or if the value
// was created using the 'emplace' method.  Other types of assignment and
// initialization (including default initialization), as well as calling
// 'reset' method will result in the optional object being disengaged.
//
// If the underlying 'TYPE' has value-semantics, then so will the type
// 'bsl::optional<TYPE>'.  Two homogeneous optional objects have the same value
// if their underlying (non-null) 'TYPE' values are the same, or both are null.
//
// Note that the object of template parameter 'TYPE' that is managed by a
// 'bsl::optional<TYPE>' object is created *in*-*place*.  Consequently, the
// template parameter 'TYPE' must be a complete type when the class is
// instantiated.
//
// In addition to the standard homogeneous, value-semantic, operations such as
// copy/move construction, copy/move assignment, equality comparison, and
// relational operators, 'bsl::optional' also supports conversion between
// optional types for which the underlying types are convertible, i.e., for
// heterogeneous copy construction, copy assignment, and equality comparison
// (e.g., between 'int' and 'double'); attempts at conversion between
// incompatible types, such as 'int' and 'bsl::string', will fail to compile.
//
// For allocator-aware types, bsl::optional uses the same allocator for all
// 'value_type' objects it manages during its lifetime.
//
///Sections:
///--------
// This file is very complex, a navigation map of this file can be obtained by:
// '$ grep -n Section: bslstl_optional.h'
//
///Usage
///-----
// The following snippets of code illustrate use of this component:
//
// First, create a 'optional' 'int' object:
//..
//  bsl::optional<int> optionalInt;
//  assert(!optionalInt.has_value());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
//  optionalInt.emplace(123);
//  assert( optionalInt.has_value());
//  assert(123 == optionalInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
//  optionalInt.reset();
//  assert(!optionalInt.has_value());
//..
//
///Known limitations
///-----------------
//: o For assignment/construction constraints, we use 'is_constructible' but
//:   the exact creation will be done using allocation construction that will
//:   invoke an allocator-extended constructor for allocator-aware types.
//:   If the 'value_type' is constructible from the assignment/constructor
//:   argument, but doesn't have a corresponding allocator-extended
//:   constructor, the overload selection may not be be correct.
//:
//: o 'optional<const TYPE>' is fully supported in C++11 and onwards. However,
//:   due to limitations of 'MovableRef<const TYPE>', C++03 support for const
//:   'value_type's is limited and move semantics of such an 'optional' in
//:   C++03 will not work.

#include <bslscm_version.h>

#include <bslstl_badoptionalaccess.h>
#include <bslstl_compare.h>
#include <bslstl_hash.h>
#include <bslstl_inplace.h>

#include <bslalg_swaputil.h>

#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_decay.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isaccessiblebaseof.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removeconst.h>
#include <bslmf_removecvref.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <stddef.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <bslmf_if.h>
#endif  // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
# include <type_traits>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
//In C++17, bsl::optional for non-aa types inherits from std::optional
# include <optional>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
# include <initializer_list>
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Mon Jan 30 15:27:01 2023
// Command line: sim_cpp11_features.pl bslstl_optional.h
# define COMPILING_BSLSTL_OPTIONAL_H
# include <bslstl_optional_cpp03.h>
# undef COMPILING_BSLSTL_OPTIONAL_H
#else

// ============================================================================
//                      Section: BSLSTL_OPTIONAL_* Macros
// ============================================================================

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

// This macro is defined as 'std::is_constructible<U, V>::value' in C++11 and
// later, and as 'DEFAULT' in C++03 with the value of 'DEFAULT' typically
// chosen to not affect the constraint this macro appears in.  Note that
// 'is_constructible', unlike 'is_convertible', is 'true' for both implicitly
// and explicitly constructible conversions.
#define BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(U, V, DEFAULT)                     \
    std::is_constructible<U, V>::value

// This macro is defined as 'std::is_assignable<U, V>::value' in C++11 and
// later, and as 'DEFAULT' in C++03 with the value of 'DEFAULT' typically
// chosen to not affect the constraint this macro appears in.
#define BSLSTL_OPTIONAL_IS_ASSIGNABLE(U, V, DEFAULT)                        \
    std::is_assignable<U, V>::value

# else  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#define BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(U, V, DEFAULT) DEFAULT

#define BSLSTL_OPTIONAL_IS_ASSIGNABLE(U, V, DEFAULT) DEFAULT

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY else

// As in 'std' implementation, if the 'TYPE' converts from any value category
// of an optional type 'OPT_TYPE', we consider it convertible from that
// optional type.
#define BSLSTL_OPTIONAL_CONVERTS_FROM(TYPE, OPT_TYPE)                         \
    (bsl::is_convertible<const OPT_TYPE&, TYPE>::value ||                     \
     bsl::is_convertible<OPT_TYPE&, TYPE>::value ||                           \
     bsl::is_convertible<const OPT_TYPE, TYPE>::value ||                      \
     bsl::is_convertible<OPT_TYPE, TYPE>::value ||                            \
     BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, const OPT_TYPE&, false) ||      \
     BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, OPT_TYPE&, false) ||            \
     BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, const OPT_TYPE, false) ||       \
     BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, OPT_TYPE, false))

// As in 'std' implementation, if the 'TYPE' can be assigned to from any value
// category of an optional type 'OPT_TYPE', we consider it convertible from
// that optional type.
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#define BSLSTL_OPTIONAL_ASSIGNS_FROM(TYPE, OPT_TYPE)                          \
    (std::is_assignable<TYPE&, const OPT_TYPE&>::value ||                     \
     std::is_assignable<TYPE&, OPT_TYPE&>::value ||                           \
     std::is_assignable<TYPE&, const OPT_TYPE>::value ||                      \
     std::is_assignable<TYPE&, OPT_TYPE>::value)

# else  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// The value of this macro is chosen so it does not affect the disjunction-form
// constraint this macro appears in.
#define BSLSTL_OPTIONAL_ASSIGNS_FROM(TYPE, OPT_TYPE) false
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY else

#define BSLSTL_OPTIONAL_CONVERTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE)            \
    BSLSTL_OPTIONAL_CONVERTS_FROM(                                            \
        TYPE,                                                                 \
        bsl::optional<typename bsl::remove_cvref<ANY_TYPE>::type>)

#define BSLSTL_OPTIONAL_ASSIGNS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE)             \
    BSLSTL_OPTIONAL_ASSIGNS_FROM(                                             \
        TYPE,                                                                 \
        bsl::optional<typename bsl::remove_cvref<ANY_TYPE>::type>)

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#define BSLSTL_OPTIONAL_CONVERTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE)            \
    BSLSTL_OPTIONAL_CONVERTS_FROM(                                            \
        TYPE,                                                                 \
        std::optional<typename bsl::remove_cvref<ANY_TYPE>::type>)

#define BSLSTL_OPTIONAL_ASSIGNS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE)             \
    BSLSTL_OPTIONAL_ASSIGNS_FROM(                                             \
        TYPE,                                                                 \
        std::optional<typename bsl::remove_cvref<ANY_TYPE>::type>)
# else  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
// The value of these macros is chosen to not affect the constraints these
// macros appears in.
#define BSLSTL_OPTIONAL_CONVERTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) false
#define BSLSTL_OPTIONAL_ASSIGNS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) false

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY else

// Macros to define common constraints that enable a constructor or assignment
// operator.
#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,          \
                                                               ANY_TYPE)      \
    typename bsl::enable_if<                                                  \
        !BSLSTL_OPTIONAL_CONVERTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE) &&        \
            BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, ANY_TYPE, true),           \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,         \
                                                                ANY_TYPE)     \
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(                   \
                     TYPE,                                                    \
                     ANY_TYPE) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,          \
                                                               ANY_TYPE)      \
    typename bsl::enable_if<                                                  \
        !BSLSTL_OPTIONAL_CONVERTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) &&        \
            BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, ANY_TYPE, true),           \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,         \
                                                                ANY_TYPE)     \
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(                   \
                     TYPE,                                                    \
                     ANY_TYPE) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(TYPE,        \
                                                                 ANY_TYPE)    \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Optional_PropagatesAllocator<TYPE,               \
                                                          ANY_TYPE>::value,   \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(TYPE,       \
                                                                  ANY_TYPE)   \
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(                 \
                     TYPE,                                                    \
                     ANY_TYPE) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(     \
    TYPE, ANY_TYPE)                                                           \
    typename bsl::enable_if<                                                  \
        !BloombergLP::bslstl::Optional_PropagatesAllocator<TYPE,              \
                                                           ANY_TYPE>::value,  \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(    \
    TYPE, ANY_TYPE)                                                           \
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(         \
                     TYPE,                                                    \
                     ANY_TYPE) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE)              \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Optional_ConstructsFromType<TYPE,                \
                                                         ANY_TYPE>::value,    \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE)             \
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(                                \
                      TYPE,                                                   \
                      ANYTYPE) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#   define BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED)           \
      typename bsl::enable_if<                                                \
        !bsl::is_same<optional, DERIVED>::value &&                            \
        BloombergLP::bslmf::IsAccessibleBaseOf<optional,                      \
                                               DERIVED>::value,               \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#   define BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(DERIVED)          \
       BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED) =             \
                                 BloombergLP::bslstl::Optional_OptNoSuchType(0)
# endif

#define BSLSTL_OPTIONAL_DEFINE_IF_SAME(U, V)                                  \
    typename bsl::enable_if<                                                  \
        bsl::is_same<U, V>::value,                                            \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_SAME(U, V)                                 \
    BSLSTL_OPTIONAL_DEFINE_IF_SAME(U, V) =                                    \
                                 BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(U, V)                              \
    typename bsl::enable_if<                                                  \
        !bsl::is_same<U, V>::value,                                           \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(U, V)                             \
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(                                       \
                            U,                                                \
                            V) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(U, V)                    \
    typename bsl::enable_if<                                                  \
        !bsl::is_convertible<V, U>::value,                                    \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(U, V)                   \
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(                             \
                            U,                                                \
                            V) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(U, V)                \
    typename bsl::enable_if<                                                  \
        bsl::is_convertible<V, U>::value,                                     \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(U, V)               \
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(                         \
                            U,                                                \
                            V) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE)       \
    typename bsl::enable_if<                                                  \
        !BSLSTL_OPTIONAL_CONVERTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE) &&        \
            BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, ANY_TYPE, true) &&         \
            BSLSTL_OPTIONAL_IS_ASSIGNABLE(TYPE&, ANY_TYPE, true) &&           \
            !BSLSTL_OPTIONAL_ASSIGNS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),       \
        optional<TYPE> >::type

#define BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, ANY_TYPE)       \
    typename bsl::enable_if<                                                  \
        !BSLSTL_OPTIONAL_CONVERTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) &&        \
            BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, ANY_TYPE, true) &&         \
            BSLSTL_OPTIONAL_IS_ASSIGNABLE(TYPE&, ANY_TYPE, true) &&           \
            !BSLSTL_OPTIONAL_ASSIGNS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE),       \
        optional<TYPE> >::type

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#   define BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(TYPE, DERIVED)          \
      typename bsl::enable_if<                                                \
        BloombergLP::bslmf::IsAccessibleBaseOf<bsl::optional<TYPE>,           \
                                               DERIVED>::value &&             \
        !bsl::is_same<bsl::optional<TYPE>, DERIVED>::value,                   \
        optional<TYPE> >::type
# endif

#define BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE)        \
    typename bsl::enable_if<                                                  \
        !::BloombergLP::bslmf::IsAccessibleBaseOf<                            \
            bsl::optional<TYPE>,                                              \
            typename bsl::remove_cvref<ANY_TYPE>::type>::value &&             \
            !(bsl::is_same<TYPE,                                              \
                           typename bsl::decay<ANY_TYPE>::type>::value &&     \
              std::is_scalar<TYPE>::value) &&                                 \
            BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, ANY_TYPE, true) &&         \
            BSLSTL_OPTIONAL_IS_ASSIGNABLE(TYPE&, ANY_TYPE, true),             \
        optional<TYPE> >::type

#define BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE)           \
    typename bsl::enable_if<                                                  \
        !::BloombergLP::bslmf::IsAccessibleBaseOf<                            \
            bsl::optional<TYPE>,                                              \
            typename bsl::remove_cv<                                          \
                typename ::BloombergLP::bslmf::MovableRefUtil::               \
                    RemoveReference<ANY_TYPE>::type>::type>::value,           \
        optional<TYPE> >::type

#define BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG(ARG)                      \
    typename bsl::enable_if<                                                  \
        !bsl::is_same<                                                        \
            typename bsl::remove_cvref<ARG>::type,                            \
            bsl::allocator_arg_t>::value,                                     \
        optional<TYPE> >::type

// ============================================================================
//         Section: Forward 'bsl' Declarations, incuding 'optional'
// ============================================================================

namespace bsl {

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

using nullopt_t = std::nullopt_t;
using std::nullopt;

# else  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

                              // ================
                              // struct nullopt_t
                              // ================
struct nullopt_t {
    // This trivial tag type is used to create 'optional' objects in a
    // disengaged state.  It is not default constructible so the following
    // assignment is unambiguous:
    //..
    //   optional<SomeType> o;
    //   o = {};
    //..
    // where 'o' is an 'optional' object.

    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR nullopt_t(int) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'nullopt_t' object.  Note that the argument is not used.
};

// CREATORS
inline
BSLS_KEYWORD_CONSTEXPR nullopt_t::nullopt_t(int) BSLS_KEYWORD_NOEXCEPT
{
    // This 'constexpr' function has to be defined before initializing the
    // 'constexpr' value, 'nullopt', below.
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr nullopt_t nullopt = nullopt_t(0);
#  else
extern const nullopt_t nullopt;
#  endif
    // Value of type 'nullopt_t' used as an argument to functions that take a
    // 'nullopt_t' argument.

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class TYPE,
          bool  USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class optional;

}  // close namespace bsl

// ============================================================================
//                     Sections: bslstl::Optional_* types
// ============================================================================

namespace BloombergLP {
namespace bslstl {

                // =======================================
                // struct Optional_IsTriviallyDestructible
                // =======================================

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

template <class TYPE>
struct Optional_IsTriviallyDestructible
: std::is_trivially_destructible<TYPE> {
    // This metafunction is derived from 'std::is_trivially_destructible<TYPE>'
    // in C++11 and later.  In C++03, the metafunction is derived from
    // 'bsl::is_trivially_copyable', a trait that implies the type is also
    // trivially destructible.
};

# else  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

template <class TYPE>
struct Optional_IsTriviallyDestructible : bsl::is_trivially_copyable<TYPE> {
    // C++03 does not provide a trivially destructible trait.  Instead we use
    // 'bsl::is_trivially_copyable' which implies the type is also trivially
    // destructible.
};

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY else

                        // ============================
                        // class Optional_OptNoSuchType
                        // ============================

struct Optional_OptNoSuchType {
    // This component-private trivial tag type is used to distinguish between
    // arguments passed by a user, and an 'enable_if' argument.  It is not
    // default constructible so the following construction never invokes a
    // constrained single parameter constructor:
    //..
    //   optional<SomeType> o(int, {});
    //..

    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR Optional_OptNoSuchType(
                                                    int) BSLS_KEYWORD_NOEXCEPT;
        // Create an 'Optional_OptNoSuchType' object.  Note that the argument
        // is not used.
};

// CREATORS
inline
BSLS_KEYWORD_CONSTEXPR Optional_OptNoSuchType::Optional_OptNoSuchType(
                                                     int) BSLS_KEYWORD_NOEXCEPT
{
}

                     // ==================================
                     // class Optional_PropagatesAllocator
                     // ==================================

template <class TYPE, class ANY_TYPE>
struct Optional_PropagatesAllocator
: bsl::integral_constant<
      bool,
      bslma::UsesBslmaAllocator<TYPE>::value && bsl::is_const<TYPE>::value &&
          bsl::is_same<ANY_TYPE, typename bsl::remove_cv<TYPE>::type>::value> {
    // This metafunction is derived from 'bsl::true_type' if 'TYPE' is an
    // allocator-aware const type, and if 'ANY_TYPE' is the same as 'TYPE',
    // minus the cv qualification.  This trait is used to enable a constructor
    // overload for a const qualified allocator-aware 'ValueType' taking an
    // rvalue of optional of the non-const qualified 'ValueType'.  Such an
    // overload needs to propagate the allocator.
};

template <class TYPE, class ANY_TYPE>
struct Optional_ConstructsFromType
: bsl::integral_constant<
      bool,
      !bsl::is_same<ANY_TYPE, TYPE>::value &&
          !bslmf::IsAccessibleBaseOf<
              bsl::optional<TYPE>,
              typename bsl::remove_cvref<ANY_TYPE>::type>::value &&
          !bsl::is_same<typename bsl::remove_cvref<ANY_TYPE>::type,
                        bsl::nullopt_t>::value &&
          !bsl::is_same<typename bsl::remove_cvref<ANY_TYPE>::type,
                        bsl::in_place_t>::value &&
          !bsl::is_same<typename bsl::remove_cvref<ANY_TYPE>::type,
                        bsl::allocator_arg_t>::value &&
          BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(TYPE, ANY_TYPE, true)> {
    // This metafunction is derived from 'bsl::true_type' if 'ANY_TYPE' is not
    // a tag type, if 'ANY_TYPE' and 'TYPE' are not a same type, and if 'TYPE'
    // is constructible from 'ANY_TYPE'.
};

                           // ======================
                           // class Optional_DataImp
                           // ======================

template <class TYPE>
struct Optional_DataImp {
    // This component-private 'struct' manages a 'value_type' object in an
    // 'optional' object.  This class provides an abstraction for 'const' value
    // type.  An 'optional' object may contain an object of 'const' type.  An
    // assignment to such an 'optional' object should not succeed.  However,
    // unless the 'optional' object itself is 'const', it should be possible to
    // change the value of the 'optional' object using 'emplace'.  In order to
    // allow for that, this class manages a non-const object of 'value_type',
    // but all the accessors return a 'const' adjusted reference to the managed
    // object.

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;
        // in-place 'TYPE' object

    bool                                        d_hasValue;
        // 'true' if object has a value, and 'false' otherwise
  public:
    // CREATORS
    Optional_DataImp() BSLS_KEYWORD_NOEXCEPT;
        // Create an empty 'Optional_DataImp' object.

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    TYPE& emplace(bslma::Allocator                           *allocator,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create an object of 'StoredType' in 'd_buffer' using the specified
        // 'allocator' and arguments and return a reference providing
        // modifiable access to the underlying 'TYPE' object.

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    TYPE& emplace(bslma::Allocator                           *allocator,
                  std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create an object of 'StoredType' in 'd_buffer' using the specified
        // 'allocator', 'initializer_list'  and arguments, and return a
        // reference providing modifiable access to the underlying 'TYPE'
        // object.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Destroy the 'value_type' object in 'd_buffer', if any.

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    TYPE&  value() &;
    TYPE&& value() &&;
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'.
# else
    TYPE& value();
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'.
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    // ACCESSORS
    bool hasValue() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this objects has a value, and 'false' otherwise.

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    const TYPE&  value() const &;
    const TYPE&& value() const &&;
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'.

# else
    const TYPE& value() const;
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'.
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else
};

                            // ===================
                            // class Optional_Data
                            // ===================

template <
    class TYPE,
    bool  IS_TRIVIALLY_DESTRUCTIBLE =
        bslstl::Optional_IsTriviallyDestructible<TYPE>::value>
struct Optional_Data : public Optional_DataImp<TYPE> {
    // This component-private 'struct' manages a 'value_type' object in
    // 'optional' by inheriting from `Optional_DataImp`.  In addition, this
    // primary template properly destroys the owned instance of 'TYPE' in its
    // destructor.

  public:
    // CREATORS
    ~Optional_Data();
        // Destroy the managed 'value_type' object, if it exists.
};

                            // ===================
                            // class Optional_Data
                            // ===================

template <class TYPE>
struct Optional_Data<TYPE, true> : public Optional_DataImp<TYPE> {
    // This partial specialization manages a trivially destructible
    // 'value_type' in optional.  It does not have a user-provided destructor,
    // which makes it 'is_trivially_destructible' itself.

  public:
# ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Optional_Data,
                                      bsl::is_trivially_copyable,
                                      bsl::is_trivially_copyable<TYPE>::value);
        // Workaround for C++03 'bsl::is_trivially_copyable' trait.  Note that,
        // whether 'Optional_Data<TYPE>' satisfies 'bsl::is_trivially_copyable'
        // doesn't affect 'Optional<TYPE>' 'bsl::is_trivially_copyable' trait.
        // We only add this nested trait for the tests to be able to check the
        // C++03 implementation of 'Optional_Data'.  For correct C++03
        // functionality, 'bsl::optional' has to add a nested trait as well.
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
template <class t_TYPE>
concept Optional_ConvertibleToBool =
    is_convertible_v<t_TYPE, bool>;

template <class t_TYPE>
concept Optional_DerivedFromBslOptional =
    requires(const t_TYPE &t) {
        []<class U>(const bsl::optional<U>&){}(t);
    };
template <class t_TYPE>
concept Optional_DerivedFromStdOptional =
    requires(const t_TYPE &t) {
        []<class U>(const std::optional<U>&){}(t);
    };
template <class t_TYPE>
concept Optional_DerivedFromOptional =
    Optional_DerivedFromBslOptional<t_TYPE> ||
    Optional_DerivedFromStdOptional<t_TYPE>;
#endif

// ============================================================================
//                 Section: Allocator-Aware 'optional' Declaration
// ============================================================================

                            // ====================
                            // class optional<TYPE>
                            // ====================

template <class TYPE, bool USES_BSLMA_ALLOC>
class optional {
    // This class template provides an STL-compliant implementation of
    // 'optional' type.  The main template is instantiated for allocator-aware
    // types and holds an allocator used to create all objects of 'value_type'
    // managed by the 'optional' object.

  public:
    // PUBLIC TYPES
    typedef TYPE value_type;
        // 'value_type' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object.  The name is chosen so it is compatible with the
        // 'std::optional' implementation.

    typedef typename bsl::allocator<char> allocator_type;

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

# ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    // This type is needed only in C++03 mode, where 'explicit' conversion
    // operators are not supported.  An 'optional' object is implicitly
    // converted to 'UnspecifiedBool' when used in 'if' statements, but is not
    // implicitly convertible to 'bool'.
    typedef BloombergLP::bsls::UnspecifiedBool<optional> UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType       UnspecifiedBool;

# endif

    // DATA
    BloombergLP::bslstl::Optional_Data<TYPE> d_value;
        // in-place 'TYPE' object

    allocator_type                           d_allocator;
        // allocator to be used for all in-place 'TYPE' objects

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(optional,
                                   BloombergLP::bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(optional,
                                   BloombergLP::bslmf::UsesAllocatorArgT);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        optional,
        BloombergLP::bslmf::IsBitwiseMoveable,
        BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value);

# ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional,
                                      bsl::is_trivially_copyable,
                                      bsl::is_trivially_copyable<TYPE>::value);
        // Workaround for C++03 'bsl::is_trivially_copyable' trait.
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

    // CREATORS
    optional() BSLS_KEYWORD_NOEXCEPT;
        // Create a disengaged 'optional' object.  Use the currently installed
        // default allocator to supply memory.

    optional(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;                 // IMPLICIT
        // Create a disengaged 'optional' object.  Use the currently installed
        // default allocator to supply memory.

    optional(const optional& original);                             // IMPLICIT
        // Create an 'optional' object having the value of the specified
        // 'original' object.  Use the currently installed default allocator to
        // supply memory.

    optional(BloombergLP::bslmf::MovableRef<optional> original)
             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                 bsl::is_nothrow_move_constructible<TYPE>::value);  // IMPLICIT
        // Create an 'optional' object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' is
        // propagated for use in the newly-created object.  'original' is left
        // in a valid, but unspecified state.

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class DERIVED>
    optional(BloombergLP::bslmf::MovableRef<DERIVED> original,
             BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(DERIVED),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(optional, DERIVED))
             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                 bsl::is_nothrow_move_constructible<TYPE>::value);  // IMPLICIT
        // Create an 'optional' object having the same value as the base of the
        // specified 'original' object by moving the contents of 'original' to
        // the newly-created object.  The allocator associated with 'original'
        // is propagated for use in the newly-created object.  'original' is
        // left in a valid, but unspecified state.  Note that this constructor
        // does not participate in overload resolution unless 'optional' is an
        // accessible base class of 'DERIVED'.
#endif

    // Because there are no default template arguments in C++03, the case of
    // 'ANYTYPE==TYPE' is written out separately.
    template <class ANY_TYPE>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
        BSLSTL_OPTIONAL_DECLARE_IF_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the specified 'value'.  Use the
        // currently installed default allocator to supply memory.

    template <class ANY_TYPE>
    explicit optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
        BSLSTL_OPTIONAL_DECLARE_IF_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the specified 'value'.  Use the
        // currently installed default allocator to supply memory.

    template <class ANY_TYPE>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the specified 'value' (of
        // 'ANY_TYPE') converted to 'TYPE'.  Use the currently installed
        // default allocator to supply memory.

    template <class ANY_TYPE>
    explicit optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the specified 'value' (of
        // 'ANY_TYPE') converted to 'TYPE'.  Use the currently installed
        // default allocator to supply memory.

    template <class ANY_TYPE>
    optional(
        const optional<ANY_TYPE>& original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
            TYPE,
            const ANY_TYPE&),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                          const ANY_TYPE&))
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
            emplace(original.value());
        }
    }

    template <class ANY_TYPE>
    explicit optional(
        const optional<ANY_TYPE>& original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
            TYPE,
            const ANY_TYPE&),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&))
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
            emplace(original.value());
        }
    }

    template <class ANY_TYPE>
    optional(BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(
                                                                     TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.
        // 'original' is left in a valid but unspecified state.

    template <class ANY_TYPE>
    explicit
    optional(BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(
                                                                     TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE,
                                                           ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.
        // 'original' is left in a valid but unspecified state.

    template <class ANY_TYPE>
    optional(
        BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(TYPE,
                                                                  ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' otherwise.  This is a special case constructor
        // where 'ANY_TYPE' is a non-const version of 'TYPE' and we use the
        // allocator from 'original' to supply memory.  'original' is left in a
        // valid but unspecified state.

    template <class ANY_TYPE>
    explicit
    optional(
        BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(TYPE,
                                                                  ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' otherwise.  This is a special case constructor
        // where 'ANY_TYPE' is a non-const version of 'TYPE' and we use the
        // allocator from 'original' to supply memory.  'original' is left in a
        // valid but unspecified state.

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    template <class ANY_TYPE = TYPE>
    optional(const std::optional<ANY_TYPE>& original,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                 TYPE,
                 const ANY_TYPE&),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.

    template <class ANY_TYPE = TYPE>
    explicit optional(const std::optional<ANY_TYPE>& original,
                      BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                          TYPE,
                          const ANY_TYPE&),
                      BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE,
                                                                    ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.

    template <class ANY_TYPE = TYPE>
    optional(std::optional<ANY_TYPE>&& original,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.

    template <class ANY_TYPE = TYPE>
    explicit optional(
        std::optional<ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                                ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the currently installed default allocator to supply memory.

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    explicit optional(bsl::in_place_t,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create an 'optional' object having the value of the (template
        // parameter) 'TYPE' created in place using the specified 'args'.  Use
        // the currently installed default allocator to supply memory.
#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    explicit optional(bsl::in_place_t,
                      std::initializer_list<INIT_LIST_TYPE>      il,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create an 'optional' object having the value of the (template
        // parameter) 'TYPE' created in place using the specified 'il' and
        // specified 'args'.  Use the currently installed default allocator to
        // supply memory.
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    optional(bsl::allocator_arg_t, allocator_type allocator);
        // Create a disengaged 'optional' object.  Use the specified
        // 'allocator' to supply memory.

    optional(bsl::allocator_arg_t, allocator_type allocator, bsl::nullopt_t);
        // Create a disengaged 'optional' object.  Use the specified
        // 'allocator' to supply memory.

    optional(bsl::allocator_arg_t,
             allocator_type       allocator,
             const optional&      original);
        // If specified 'original' contains a value, initialize the contained
        // 'value_type' object with '*original'.  Otherwise, create a
        // disengaged 'optional' object.  Use the specified 'allocator' (e.g.,
        // the address of a 'bslma::Allocator' object) to supply memory.

    optional(bsl::allocator_arg_t,
             allocator_type                           allocator,
             BloombergLP::bslmf::MovableRef<optional> original);
        // Create a 'optional' object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  Use the specified 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class DERIVED>
    optional(bsl::allocator_arg_t,
             allocator_type                          allocator,
             BloombergLP::bslmf::MovableRef<DERIVED> original,
             BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(DERIVED),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(optional, DERIVED));
        // Create a 'optional' object having the same value as the base class
        // of the specified 'original' object by moving the contents of
        // 'original' to the newly-created object.  Use the specified
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory.  Note that this constructor does not participate in
        // overload resolution unless 'optional' is an accessible base class of
        // 'DERIVED'.
# endif

    template <class ANY_TYPE>
    explicit optional(
                     bsl::allocator_arg_t,
                     allocator_type                                  allocator,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)     value,
                     BSLSTL_OPTIONAL_DECLARE_IF_SAME(ANY_TYPE, TYPE));
        // Create an 'optional' object having the specified 'value'.  Use the
        // specified 'allocator' to supply memory.  Note that this overload is
        // selected if 'ANY_TYPE == TYPE'.

    template <class ANY_TYPE>
    explicit optional(
           bsl::allocator_arg_t,
           allocator_type                                            allocator,
           BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)               value,
           BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE));
        // Create an 'optional' object having the specified 'value' (of
        // 'ANY_TYPE') converted to 'TYPE'.   Use the specified 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory.
        // 'value' is left in a valid but unspecified state.  Note that this
        // constructor does not participate in overload resolution unless
        // 'ANY_TYPE' is convertible to 'TYPE'.

    template <class ANY_TYPE>
    explicit optional(bsl::allocator_arg_t,
                      allocator_type            allocator,
                      const optional<ANY_TYPE>& original,
                      BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
                      BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
                          TYPE,
                          const ANY_TYPE&));
        // Create an 'optional' object having the value of the specified
        // 'original' object.  Use the specified 'allocator' (e.g., the address
        // of a 'bslma::Allocator' object) to supply memory.

    template <class ANY_TYPE>
    explicit optional(
        bsl::allocator_arg_t,
        allocator_type       allocator,
        BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // Use the specified 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory.  'original' is left in
        // a valid but unspecified state.

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    template <class ANY_TYPE = TYPE>
    explicit optional(bsl::allocator_arg_t,
                      allocator_type                 allocator,
                      const std::optional<ANY_TYPE>& original,
                      BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                          TYPE,
                          const ANY_TYPE&));
        // If specified 'original' contains a value, initialize the
        // 'value_type' object with '*original'.  Otherwise, create a
        // disengaged 'optional' object.  Use the specified 'allocator'(e.g.,
        // the address of a 'bslma::Allocator' object) to supply memory.

    template <class ANY_TYPE = TYPE>
    explicit optional(
        bsl::allocator_arg_t,
        allocator_type            allocator,
        std::optional<ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                                ANY_TYPE));
        // If specified 'original' contains a value, initialize the
        // 'value_type' object by move construction from '*original'.
        // Otherwise, create a disengaged 'optional' object.  Use the specified
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory.

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    explicit optional(bsl::allocator_arg_t,
                      allocator_type                             allocator,
                      bsl::in_place_t,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create the 'value_type' object using the specified arguments.  Use
        // the specified 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory for this and any future 'value_type'
        // objects.

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    explicit optional(
                   bsl::allocator_arg_t,
                   allocator_type                             allocator,
                   bsl::in_place_t,
                   std::initializer_list<INIT_LIST_TYPE>      initializer_list,
                   BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create the 'value_type' object using the specified
        // 'initializer_list' and arguments.  Use the specified 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory.
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Assign to this 'optional' object the value of the (template
        // parameter) 'TYPE' created in place using the specified 'args' and
        // return a reference providing modifiable access to the underlying
        // 'TYPE' object.  If this 'optional' object already contains an object
        // ('true == hasValue()'), that object is destroyed before the new
        // object is created.  The allocator specified at the construction of
        // this 'optional' object is used to supply memory to the value object.
        // Attempts to explicitly specify via 'args' another allocator to
        // supply memory to the created (value) object are disallowed by the
        // compiler.  Note that if the constructor of 'TYPE' throws an
        // exception this object is left in a disengaged state.

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    TYPE& emplace(std::initializer_list<INIT_LIST_TYPE>      il,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Assign to this 'optional' object the value of the (template
        // parameter) 'TYPE' created in place using the specified 'il' and
        // specified 'args' and return a reference providing modifiable access
        // to the underlying 'TYPE' object.  If this 'optional' object already
        // contains an object ('true == hasValue()'), that object is destroyed
        // before the new object is created.  The allocator specified at the
        // construction of this 'optional' object is used to supply memory to
        // the value object.  Attempts to explicitly specify via 'args' another
        // allocator to supply memory to the created (value) object are
        // disallowed by the compiler.  Note that if the constructor of 'TYPE'
        // throws an exception this object is left in a disengaged state.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif
    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to the default constructed state (i.e., to a
        // disengaged state).

    void swap(optional& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<TYPE>::value &&
                             bsl::is_nothrow_swappable<TYPE>::value);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the template parameter 'TYPE' provides
        // that guarantee and the result of the 'hasValue' method for the two
        // objects being swapped is the same.  The behavior is undefined unless
        // this object was created with the same allocator as 'other'.

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    TYPE&  value() &;
    TYPE&& value() &&;
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object if 'true == has_value()' and throw
        // 'bsl::bad_optional_access' otherwise.
# else
    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  Throws a 'bsl::bad_optional_access' if the
        // 'optional' object is disengaged.
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    template <class ANY_TYPE>
    TYPE value_or(ANY_TYPE&& value) &&;
        // Return a copy of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // converted to 'TYPE' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    template <class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t, allocator_type, ANY_TYPE&& value) &&;
        // If this object is non-null, return a copy of the underlying object
        // of a (template parameter) 'TYPE' created using the provided
        // allocator, and the specified 'value' converted to 'TYPE' using the
        // specified 'allocator' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  endif  // BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    optional& operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to be disengaged and return a reference providing
        // modifiable access to this object.

    optional& operator=(const optional& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    optional& operator=(BloombergLP::bslmf::MovableRef<optional> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  The contents of 'rhs'
        // are either move-constructed into or move-assigned to this object.
        // 'rhs' is left in a valid but unspecified state.

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class DERIVED>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(TYPE, DERIVED)&
    operator=(BloombergLP::bslmf::MovableRef<DERIVED> rhs);
        // Assign to this object the value of the base of the specified 'rhs'
        // object, and return a non-'const' reference to this object.  The
        // allocators of this object and 'rhs' both remain unchanged.  The
        // contents of 'rhs' are either move-constructed into or move-assigned
        // to this object.  'rhs' is left in a valid but unspecified state.
        // Note that this manipulator does not participate in overload
        // resolution unless 'optional' is an accessible base class of
        // 'DERIVED'.
# endif

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, const ANY_TYPE&) &
    operator=(const optional<ANY_TYPE>& rhs)
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and assign to this object the value of 'rhs.value()' (of 'ANY_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method does not
        // participate in overload resolution unless 'TYPE' and 'ANY_TYPE' are
        // compatible.
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.

        if (rhs.has_value()) {
            if (this->has_value()) {
                d_value.value() = *rhs;
            }
            else {
                emplace(*rhs);
            }
        }
        else {
            reset();
        }
        return *this;
    }

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE) &
    operator=(BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and move assign to this object the value of 'rhs.value()' (of
        // 'ANY_TYPE') converted to 'TYPE' otherwise.  Return a reference
        // providing modifiable access to this object.  Note that this method
        // does not participate in overload resolution unless 'TYPE' and
        // 'ANY_TYPE' are compatible.

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE) &
    operator=(ANY_TYPE&& rhs);
        // Assign to this object the value of the specified 'rhs' object
        // converted to 'TYPE', and return a reference providing modifiable
        // access to this object.  Note that this method may invoke assignment
        // from 'rhs', or construction from 'rhs', depending on whether this
        // object is engaged.
        // BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE)
        // contains a check that disables this overload if 'ANY_TYPE' is
        // 'optional<TYPE>'.  This is needed to prevent this assignment
        // operator being a better match for non const 'optional<TYPE>' lvalues
        // than 'operator=(const optional& rhs)'.  This function needs to be a
        // worse match than 'operator=(optional)' so cases like :
        //..
        //      bsl::optional<int> oi;
        //      oi = {};
        //..
        // represent assignment from a default constructed 'optional', as
        // opposed to assignment from default constructed 'value_type'.  Note
        // that in C++03, where there is no concept of perfect forwarding,
        // this is not a concern.

# else  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // The existence of MovableRef in C++11 affects the above functions, and
    // they need to be defined in terms of rvalue references and perfect
    // forwarding.  For C++03, the MovableRef overloads are provided below.

    optional& operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    optional& operator=(BloombergLP::bslmf::MovableRef<TYPE> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are either move-constructed into or move-assigned to this
        // object.  'rhs' is left in a valid but unspecified state.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE) &
    operator=(const ANY_TYPE& rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'ANY_TYPE') converted to 'TYPE', and return a reference providing
        // modifiable access to this object.  Note that this method may invoke
        // assignment from 'rhs', or construction from 'rhs', depending on
        // whether this 'optional' object is engaged.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE) &
    operator=(BloombergLP::bslmf::MovableRef<ANY_TYPE> rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'ANY_TYPE') converted to 'TYPE', and return a reference providing
        // modifiable access to this object.  The contents of 'rhs' are either
        // move-constructed into or move-assigned to this object.  'rhs' is
        // left in a valid but unspecified state.  This overload needs to exist
        // in C++03 because the perfect forwarding 'operator=' can not to be
        // specified in terms of 'MovableRef'.
# endif  // RVALUES else

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, const ANY_TYPE&) &
    operator=(const std::optional<ANY_TYPE>& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and assign to this object the value of 'rhs.value()' (of 'ANY_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method does not
        // participate in overload resolution unless 'TYPE' and 'ANY_TYPE' are
        // compatible.

    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) &
    operator=(std::optional<ANY_TYPE>&& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and move assign to this object the value of 'rhs.value()' (of
        // 'ANY_TYPE') converted to 'TYPE' otherwise.  Return a reference
        // providing modifiable access to this object.  Note that this method
        // does not participate in overload resolution unless 'TYPE' and
        // 'ANY_TYPE' are compatible.

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    TYPE *operator->();
        // Return a pointer providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    TYPE&  operator*() &;
    TYPE&& operator*() &&;
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.
# else
    TYPE& operator*();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return allocator used for construction of 'value_type'.

    bool has_value() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'false' if this object is disengaged, and 'true' otherwise.

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    const TYPE&  value() const &;
    const TYPE&& value() const &&;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object if 'true == has_value()' and throw
        // 'bsl::bad_optional_access' otherwise.
# else

    const TYPE& value() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  Throws a 'bsl::bad_optional_access' if the
        // 'optional' object is disengaged.

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    const TYPE *operator->() const;
        // Return a pointer providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    const TYPE&  operator*() const &;
    const TYPE&& operator*() const &&;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.

# else

    const TYPE& operator*() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    template <class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const&;
        // Return a copy of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // converted to 'TYPE' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    template <class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t,
                  allocator_type,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const&;
        // If this object is non-null, return a copy of the underlying object
        // of a (template parameter) 'TYPE' created using the provided
        // allocator, and the specified 'value' converted to 'TYPE' using the
        // specified 'allocator' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  endif  // BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# else
    template <class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const;
        // Return a copy of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // converted to 'TYPE' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    template <class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t,
                  allocator_type,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const;
        // If this object is non-null, return a copy of the underlying object
        // of a (template parameter) 'TYPE' created using the provided
        // allocator, and the specified 'value' converted to 'TYPE' using the
        // specified 'allocator' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  endif  // BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    explicit operator bool() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'false' if this object is disengaged, and 'true' otherwise.
# else
    // Simulation of explicit conversion to bool.  Inlined to work around xlC
    // bug when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
# endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT else

  protected:

# ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // PROTECTED MANIPULATORS
    TYPE& dereferenceRaw();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.  Note that this function is only intended to be
        // called by 'bdlb::NullableValue::value' during transition of its
        // implementation to use 'bsl::optional.  Note that ref-qualified
        // versions of 'value()' are not provided because 'NullableValue' does
        // not require them.

    // PROTECTED ACCESSORS
    const TYPE& dereferenceRaw() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.  Note that this function is only intended to be
        // called by 'bdlb::NullableValue::value' during transition of its
        // implementation to use 'bsl::optional.  Note that ref-qualified
        // versions of 'value()' are not provided because 'NullableValue' does
        // not require them.
# endif  // BDE_OMIT_INTERNAL_DEPRECATED

};

// ============================================================================
//            Section: C++17 Allocator-Unaware 'optional' Declaration
// ============================================================================

                            // ====================
                            // class optional<TYPE>
                            // ====================

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE>
class optional<TYPE, false> : public std::optional<TYPE> {
    // Specialization of 'optional' for 'value_type' that is not
    // allocator-aware when 'std::optional' is available.
  private:
    // PRIVATE TYPES
    typedef std::optional<TYPE> OptionalBase;

  public:
    // CREATORS
   optional() noexcept;
        // Create a disengaged 'optional' object.

    optional(bsl::nullopt_t) noexcept;                              // IMPLICIT
        // Create a disengaged 'optional' object.

    template <class ANY_TYPE = TYPE>
    optional(
       ANY_TYPE&&                                                        value,
       BSLSTL_OPTIONAL_DECLARE_IF_SAME(ANY_TYPE, TYPE),
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE = TYPE>
    explicit optional(
           ANY_TYPE&&                                                    value,
           BSLSTL_OPTIONAL_DECLARE_IF_SAME(ANY_TYPE, TYPE),
           BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE = TYPE>
    optional(
       ANY_TYPE&&                                                        value,
       BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE = TYPE>
    explicit optional(
           ANY_TYPE&&                                                    value,
           BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
           BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE>
    optional(
       const optional<ANY_TYPE>& original,
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
       BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
           TYPE,
           const ANY_TYPE&),
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                         const ANY_TYPE&));
       // Create a disengaged 'optional' object if the specified 'original'
       // object is disengaged, and an 'optional' object with the value of
       // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.

    template <class ANY_TYPE>
    explicit optional(
       const optional<ANY_TYPE>& original,
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
       BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
           TYPE,
           const ANY_TYPE&),
       BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&));
       // Create a disengaged 'optional' object if the specified 'original'
       // object is disengaged, and an 'optional' object with the value of
       // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.

    template <class ANY_TYPE>
    optional(optional<ANY_TYPE>&& original,
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // 'original' is left in a valid but unspecified state.

    template <class ANY_TYPE>
    explicit optional(
        optional<ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // 'original' is left in a valid but unspecified state.

    template <class ANY_TYPE = TYPE>
    optional(const std::optional<ANY_TYPE>& original,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                 TYPE,
                 const ANY_TYPE&),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.

    template <class ANY_TYPE = TYPE>
    explicit optional(const std::optional<ANY_TYPE>& original,
                      BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                          TYPE,
                          const ANY_TYPE&),
                      BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE,
                                                                    ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.

    template <class ANY_TYPE = TYPE>
    optional(std::optional<ANY_TYPE>&& original,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // 'original' is left in a valid but unspecified state.

    template <class ANY_TYPE = TYPE>
    explicit optional(
        std::optional<ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                                ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // 'original' is left in a valid but unspecified state.

    template <class... ARGS>
    explicit optional(bsl::in_place_t,
                      ARGS&&... args);
        // Create an 'optional' object having the value of the (template
        // parameter) 'TYPE' created in place using the specified 'args'.

    template <class INIT_LIST_TYPE, class... ARGS>
    explicit optional(bsl::in_place_t,
                      std::initializer_list<INIT_LIST_TYPE> il,
                      ARGS&&... args);
        // Create an 'optional' object having the value of the (template
        // parameter) 'TYPE' created in place using the specified 'il' and
        // the specified 'args'.

    // MANIPULATORS
    optional& operator=(bsl::nullopt_t) noexcept;
        // reset the optional to a disengaged state.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, const ANY_TYPE&) &
    operator=(const optional<ANY_TYPE>& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and assign to this object the value of 'rhs.value()' (of 'ANY_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method does not
        // participate in overload resolution unless 'TYPE' and 'ANY_TYPE' are
        // compatible.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE) &
    operator=(optional<ANY_TYPE>&& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and move assign to this object the value of 'rhs.value()' (of
        // 'ANY_TYPE') converted to 'TYPE' otherwise.  Return a reference
        // providing modifiable access to this object.  Note that this method
        // does not participate in overload resolution unless 'TYPE' and
        // 'ANY_TYPE' are compatible.

    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, const ANY_TYPE&) &
    operator=(const std::optional<ANY_TYPE>& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and assign to this object the value of 'rhs.value()' (of 'ANY_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method does not
        // participate in overload resolution unless 'TYPE' and 'ANY_TYPE' are
        // compatible.

    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) &
    operator=(std::optional<ANY_TYPE>&& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and move assign to this object the value of 'rhs.value()' (of
        // 'ANY_TYPE') converted to 'TYPE' otherwise.  Return a reference
        // providing modifiable access to this object.  Note that this method
        // does not participate in overload resolution unless 'TYPE' and
        // 'ANY_TYPE' are compatible.

    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE) &
    operator=(ANY_TYPE&& rhs);
      // Assign to this object the value of the specified 'rhs' object
      // converted to 'TYPE', and return a reference providing modifiable
      // access to this object.  Note that this method may invoke assignment
      // from 'rhs', or construction from 'rhs', depending on whether this
      // object is engaged.
      // BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE)
      // contains a check that disables this overload if 'ANY_TYPE' is
      // 'optional<TYPE>'.  This is needed to prevent this assignment operator
      // being a better match for non const 'optional<TYPE>' lvalues than
      // 'operator=(const optional& rhs)'.  This function needs to be a worse
      // match than 'operator=(optional)' so cases like :
      //..
      //      bsl::optional<int> oi;
      //      oi = {};
      //..
      // represent assignment from a default constructed 'optional', as opposed
      // to assignment from default constructed 'value_type'.  Note that in
      // C++03, where there is no concept of perfect forwarding, this is not a
      // concern.

  protected:

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // PROTECTED MANIPULATORS
    TYPE& dereferenceRaw();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.  Note that this function is only intended to be
        // called by 'bdlb::NullableValue::value' during transition of its
        // implementation to use 'bsl::optional.  Note that ref-qualified
        // versions of 'value()' are not provided because 'NullableValue' does
        // not require them.

    // PROTECTED ACCESSORS
    const TYPE& dereferenceRaw() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.  Note that this function is only intended to be
        // called by 'bdlb::NullableValue::value' during transition of its
        // implementation to use 'bsl::optional.  Note that ref-qualified
        // versions of 'value()' are not provided because 'NullableValue' does
        // not require them.
#  endif  // BDE_OMIT_INTERNAL_DEPRECATED
};
# else  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// ============================================================================
//          Section: Pre-C++17 Allocator-Unaware 'optional' Declaration
// ============================================================================

template <class TYPE>
class optional<TYPE, false> {
    // Specialization of 'optional' for 'value_type' that is not
    // allocator-aware.
  public:
    // PUBLIC TYPES
    typedef TYPE value_type;
        // 'value_type' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object.  The name is chosen so it is compatible with the
        // 'std::optional' implementation.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

#  ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    // This type is needed only in C++03 mode, where 'explicit' conversion
    // operators are not supported.  A 'function' is implicitly converted to
    // 'UnspecifiedBool' when used in 'if' statements, but is not implicitly
    // convertible to 'bool'.
    typedef BloombergLP::bsls::UnspecifiedBool<optional> UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType       UnspecifiedBool;

#  endif

    // DATA
    BloombergLP::bslstl::Optional_Data<TYPE> d_value;
        // in-place 'TYPE' object

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        optional,
        BloombergLP::bslmf::IsBitwiseMoveable,
        BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value);

#  ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional,
                                      bsl::is_trivially_copyable,
                                      bsl::is_trivially_copyable<TYPE>::value);
        // Workaround for C++03 'bsl::is_trivially_copyable' trait.
#  endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

    // CREATORS
    optional() BSLS_KEYWORD_NOEXCEPT;
        // Create a disengaged 'optional' object.

    optional(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;                 // IMPLICIT
        // Create a disengaged 'optional' object.

    optional(const optional& original);                             // IMPLICIT
        // Create an 'optional' object having the value of the specified
        // 'original' object.

    optional(BloombergLP::bslmf::MovableRef<optional> original)
             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                 bsl::is_nothrow_move_constructible<TYPE>::value);  // IMPLICIT
        // Create an 'optional' object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  'original' is left in a valid, but
        // unspecified state.

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class DERIVED>
    optional(
        BloombergLP::bslmf::MovableRef<DERIVED> original,
        BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(DERIVED),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(optional, DERIVED))
             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                 bsl::is_nothrow_move_constructible<TYPE>::value);  // IMPLICIT
        // Create an 'optional' object having the same value as the base of the
        // specified 'original' object by moving the contents of 'original' to
        // the newly-created object.  'original' is left in a valid, but
        // unspecified state.  Note that this constructor does not participate
        // in overload resolution unless 'optional' is an accessible base class
        // of 'DERIVED'.
# endif

    template <class ANY_TYPE>
    optional(
       BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                       value,
       BSLSTL_OPTIONAL_DECLARE_IF_SAME(ANY_TYPE, TYPE),
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE>
    explicit optional(
           BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                   value,
           BSLSTL_OPTIONAL_DECLARE_IF_SAME(ANY_TYPE, TYPE),
           BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE>
    optional(
       BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                       value,
       BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
       BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE>
    explicit optional(
           BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                   value,
           BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
           BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create an 'optional' object having the same value as the specified
        // 'value' object by forwarding the contents of 'value' to the
        // newly-created object.

    template <class ANY_TYPE>
    optional(
        const optional<ANY_TYPE>& original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
            TYPE,
            const ANY_TYPE&),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                          const ANY_TYPE&));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.

    template <class ANY_TYPE>
    explicit optional(
        const optional<ANY_TYPE>& original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
            TYPE,
            const ANY_TYPE&),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.

    template <class ANY_TYPE>
    optional(BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                     ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE,
                                                               ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // 'original' is left in a valid but unspecified state.

    template <class ANY_TYPE>
    explicit optional(
        BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                                ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE));
        // Create a disengaged 'optional' object if the specified 'original'
        // object is disengaged, and an 'optional' object with the value of
        // 'original.value()' (of 'ANY_TYPE') converted to 'TYPE' otherwise.
        // 'original' is left in a valid but unspecified state.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    explicit optional(bsl::in_place_t,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    explicit optional(bsl::in_place_t,
                      std::initializer_list<INIT_LIST_TYPE>,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Assign to this 'optional' object the value of the (template
        // parameter) 'TYPE' created in place using the specified 'args' and
        // return a reference providing modifiable access to the underlying
        // 'TYPE' object.  If this 'optional' object already contains an object
        // ('true == hasValue()'), that object is destroyed before the new
        // object is created.  Note that if the constructor of 'TYPE' throws an
        // exception this object is left in a disengaged state.

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    TYPE& emplace(std::initializer_list<INIT_LIST_TYPE>      il,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Assign to this 'optional' object the value of the (template
        // parameter) 'TYPE' created in place using the specified 'il' and
        // specified 'args' and return a reference providing modifiable access
        // to the underlying 'TYPE' object.  If this 'optional' object already
        // contains an object ('true == hasValue()'), that object is destroyed
        // before the new object is created.  Note that if the constructor of
        // 'TYPE' throws an exception this object is left in a disengaged
        // state.

#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to the default constructed state (i.e., to be in a
        // disengaged state).

    void swap(optional& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<TYPE>::value &&
                             bsl::is_nothrow_swappable<TYPE>::value);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the template parameter 'TYPE' provides
        // that guarantee and the result of the 'hasValue' method for the two
        // objects being swapped is the same.

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    TYPE&  value() &;
    TYPE&& value() &&;
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object if 'true == has_value()' and throw
        // 'bsl::bad_optional_access' otherwise.

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  Throws a 'bsl::bad_optional_access' if the
        // 'optional' object is disengaged.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    template <class ANY_TYPE>
    TYPE value_or(ANY_TYPE&& value) &&;
        // Return a copy of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // converted to 'TYPE' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    optional& operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;
        // reset the 'optional' object to a disengaged state.

    optional& operator=(const optional& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    optional& operator=(BloombergLP::bslmf::MovableRef<optional> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  The contents of 'rhs'
        // are either move-constructed into or move-assigned to this object.
        // 'rhs' is left in a valid but unspecified state.

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class DERIVED>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(TYPE, DERIVED)&
    operator=(BloombergLP::bslmf::MovableRef<DERIVED> rhs);
        // Assign to this object the value of the base class of the specified
        // 'rhs' object, and return a non-'const' reference to this object.
        // The allocators of this object and 'rhs' both remain unchanged.  The
        // contents of 'rhs' are either move-constructed into or move-assigned
        // to this object.  'rhs' is left in a valid but unspecified state.
        // Note that this manipulator does not participate in overload
        // resolution unless 'optional' is an accessible base class of
        // 'DERIVED'.
# endif

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, const ANY_TYPE&) &
    operator=(const optional<ANY_TYPE>& rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and assign to this object the value of 'rhs.value()' (of 'ANY_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method does not
        // participate in overload resolution unless 'TYPE' and 'ANY_TYPE' are
        // compatible.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE)& operator=(
                              BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) rhs);
        // Disengage this object if the specified 'rhs' object is disengaged,
        // and move assign to this object the value of 'rhs.value()' (of
        // 'ANY_TYPE') converted to 'TYPE' otherwise.  Return a reference
        // providing modifiable access to this object.  Note that this method
        // does not participate in overload resolution unless 'TYPE' and
        // 'ANY_TYPE' are compatible.

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class ANY_TYPE = TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE) &
    operator=(ANY_TYPE&& rhs);
        // Assign to this object the value of the specified 'rhs' object
        // converted to 'TYPE', and return a reference providing modifiable
        // access to this object.  Note that this method may invoke assignment
        // from 'rhs', or construction from 'rhs', depending on whether this
        // object is engaged.  BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF
        // contains a check that disables this overload if 'ANY_TYPE' is
        // 'optional<TYPE>'.  This is needed to prevent this assignment
        // operator being a better match for non const 'optional<TYPE>' lvalues
        // than 'operator=(const optional& rhs)'.  This function needs to be a
        // worse match than 'operator=(optional)' so cases like :
        //..
        //      bsl::optional<int> oi;
        //      oi = {};
        //..
        // represent assignment from a default constructed 'optional', as
        // opposed to assignment from default constructed 'value_type'.  Note
        // that in C++03, where there is no concept of perfect forwarding, this
        // is not a concern.

#  else  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // MovableRef and rvalue give different semantics in template functions.
    // For C++03, we need to specify different overloads.

    optional& operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    optional& operator=(BloombergLP::bslmf::MovableRef<TYPE> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are either move-constructed into or move-assigned to this
        // object.  'rhs' is left in a valid but unspecified state.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE)&
    operator=(const ANY_TYPE& rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'ANY_TYPE') converted to 'TYPE', and return a reference providing
        // modifiable access to this object.  Note that this method may invoke
        // assignment from 'rhs', or construction from 'rhs', depending on
        // whether this 'optional' object is engaged.

    template <class ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE)&
    operator=(BloombergLP::bslmf::MovableRef<ANY_TYPE> rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'ANY_TYPE') converted to 'TYPE', and return a reference providing
        // modifiable access to this object.  The contents of 'rhs' are either
        // move-constructed into or move-assigned to this object.  'rhs' is
        // left in a valid but unspecified state.  This overload needs to exist
        // in C++03 because the perfect forwarding 'operator=' can not to be
        // specified in terms of 'MovableRef'.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES else

    TYPE *operator->();
        // Return a pointer providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if this object is
        // disengaged.

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    TYPE&  operator*() &;
    TYPE&& operator*() &&;
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if this object is
        // disengaged.

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    TYPE& operator*();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if this object is
        // disengaged.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    // ACCESSORS
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'false' if this object is disengaged, and 'true' otherwise.

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    const TYPE&  value() const &;
    const TYPE&& value() const &&;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object if 'true == has_value()' and throw
        // 'bsl::bad_optional_access' otherwise.
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    const TYPE& value() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  Throws a 'bsl::bad_optional_access' if the
        // 'optional' object is disengaged.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    template <class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const&;
        // Return a copy of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // converted to 'TYPE' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    template <class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const;
        // Return a copy of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // converted to 'TYPE' otherwise.  Note that this method returns *by*
        // *value*, so may be inefficient in some contexts.
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    const TYPE *operator->() const;
        // Return a pointer providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if this object is
        // disengaged.

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    const TYPE&  operator*() const&;
    const TYPE&& operator*() const&&;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if this object is
        // disengaged.
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    const TYPE& operator*() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if this object is
        // disengaged.

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    BSLS_KEYWORD_EXPLICIT operator bool() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'false' if this object is disengaged, and 'true' otherwise.
#  else  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // Simulation of explicit conversion to bool.  Inlined to work around xlC
    // bug when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT else

  protected:

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // PROTECTED MANIPULATORS
    TYPE& dereferenceRaw();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.  Note that this function is only intended to be
        // called by 'bdlb::NullableValue::value' during transition of its
        // implementation to use 'bsl::optional.  Note that ref-qualified
        // versions of 'value()' are not provided because 'NullableValue' does
        // not require them.

    // PROTECTED ACCESSORS
    const TYPE& dereferenceRaw() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined if the 'optional' object
        // is disengaged.  Note that this function is only intended to be
        // called by 'bdlb::NullableValue::value' during transition of its
        // implementation to use 'bsl::optional.  Note that ref-qualified
        // versions of 'value()' are not provided because 'NullableValue' does
        // not require them.
#  endif  // !BDE_OMIT_INTERNAL_DEPRECATED
};
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY else

}  // close namespace bsl

// ============================================================================
//              Section: Type Traits on Allocator-Unaware 'optional'
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator<bsl::optional<TYPE, false> > : bsl::false_type {};

}  // close namespace bslma
}  // close enterprise namespace

namespace bsl {

// ============================================================================
//                          Section: CTAD Constructors
// ============================================================================

# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <class TYPE>
optional(TYPE) -> optional<TYPE>;
    // Deduce the specified type 'TYPE' from the corresponding type supplied to
    // the constructor of 'optional'.

template <class TYPE,
          class ALLOC,
          class = typename bsl::enable_if_t<
                    BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>,
          class = typename bsl::enable_if_t<
                bsl::is_convertible_v<ALLOC, bsl::allocator<char>>>
          >
optional(bsl::allocator_arg_t, ALLOC, TYPE)
-> optional<TYPE>;
    // Deduce the specified type 'TYPE' from the corresponding type supplied to
    // the constructor of 'optional'.   This guide does not participate in
    // deduction unless the deduced type 'TYPE' supports the bslma allocator
    // model, and specified 'ALLOC' can be implicitly converted to
    // 'bsl::allocator<char>'.

template <class TYPE,
          class ALLOC,
          class = typename bsl::enable_if_t<
                    BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>,
          class = typename bsl::enable_if_t<
                bsl::is_convertible_v<ALLOC, bsl::allocator<char>>>
          >
optional(bsl::allocator_arg_t, ALLOC, optional<TYPE>)
-> optional<TYPE>;
    // Deduce the specified type 'TYPE' from the corresponding template
    // parameter type supplied to the constructor of 'optional'.   This guide
    // does not participate in deduction unless the deduced type 'TYPE'
    // supports the bslma allocator model, and specified 'ALLOC' can be
    // implicitly converted to 'bsl::allocator<char>'.

# endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                      Section: Free Function Declarations
// ============================================================================

// FREE FUNCTIONS
template <class TYPE>
typename
bsl::enable_if<BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value, void>::type
swap(bsl::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
    // Efficiently exchange the values of the specified 'lhs' and 'rhs'
    // objects.  This method provides the no-throw exception-safety guarantee
    // if the template parameter 'TYPE' provides that guarantee and the result
    // of the 'hasValue' method for 'lhs' and 'rhs' is the same.

template <class TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(bsl::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
    // Efficiently exchange the values of the specified 'lhs' and 'rhs'
    // objects.  This method provides the no-throw exception-safety guarantee
    // if the template parameter 'TYPE' provides that guarantee and the result
    // of the 'hasValue' method for 'lhs' and 'rhs' is the same.


// HASH SPECIALIZATIONS
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const optional<TYPE>& input);
    // Pass the specified 'input' to the specified 'hashAlg', where 'hashAlg'
    // is a hashing algorithm.

// FREE OPERATORS

// comparison with optional
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs == *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' and 'rhs' optional objects have the
    // same value, and 'false' otherwise.  Two optional objects have the same
    // value if both are disengaged, or if both are engaged and the values of
    // their underlying objects compare equal.  Note that this function will
    // fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs != *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' and 'rhs' optional objects do not
    // have the same value, and 'false' otherwise.  Two optional objects do not
    // have the same value if one is disengaged and the other is engaged, or if
    // both are engaged and the values of their underlying objects do not
    // compare equal.  Note that this function will fail to compile if
    // 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const bsl::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs < *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs' optional object, and 'false' otherwise.  'lhs' is
    // ordered before 'rhs' if 'lhs' is disengaged and 'rhs' is engaged or if
    // both are engaged and 'lhs.value()' is ordered before 'rhs.value()'.
    // Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const bsl::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs > *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' optional object, and 'false' otherwise.  'lhs' is
    // ordered after 'rhs' if 'lhs' is engaged and 'rhs' is disengaged or if
    // both are engaged and 'lhs.value()' is ordered after 'rhs.value()'.  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs <= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs' optional object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator<' and 'operator=='.)  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs >= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' optional object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

// comparison with nullopt_t
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator==(
                            const bsl::optional<TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator==(
                       const bsl::nullopt_t&,
                       const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if specified 'value' is disengaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                            const bsl::optional<TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                       const bsl::nullopt_t&,
                       const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if specified 'value' is engaged, and 'false' otherwise.

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<(
                             const bsl::optional<TYPE>&,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false'.  Note that 'bsl::nullopt_t' never orders after a
    // 'bsl::optional'.


template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<(
                       const bsl::nullopt_t&,
                       const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if specified 'value' is engaged, and 'false' otherwise.
    // Note that 'bsl::nullopt_t' is ordered before any 'bsl::optional' that is
    // engaged.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>(
                            const bsl::optional<TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if specified 'value' is engaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>(
                             const bsl::nullopt_t&,
                             const bsl::optional<TYPE>&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false'.  Note that 'bsl::nullopt_t' never orders after a
    // 'bsl::optional'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                            const bsl::optional<TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if specified 'value' is disengaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                             const bsl::nullopt_t&,
                             const bsl::optional<TYPE>&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                             const bsl::optional<TYPE>&,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                       const bsl::nullopt_t&,
                       const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if specified 'value' is disengaged, and 'false' otherwise.

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

// comparison with T
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs != rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs != *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  An 'optional' object and a value of
    // some type do not have the same value if either the optional object is
    // null, or its underlying value does not compare equal to the other value.
    // Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs == rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs == *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  An 'optional' object and a value of some
    // type have the same value if the optional object is non-null and its
    // underlying value compares equal to the other value.  Note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs < rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs', and 'false' otherwise.  'lhs' is ordered before
    // 'rhs' if 'lhs' is disengaged or 'lhs.value()' is ordered before 'rhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs < *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' optional object, and 'false' otherwise.  'lhs' is ordered before
    // 'rhs' if 'rhs' is engaged and 'lhs' is ordered before 'rhs.value()'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs > rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs', and 'false' otherwise.  'lhs' is ordered after
    // 'rhs' if 'lhs' is engaged and 'lhs.value()' is ordered after 'rhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs > *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' optional object, and 'false' otherwise.  'lhs' is ordered after
    // 'rhs' if 'rhs' is disengaged or 'lhs' is ordered after 'rhs.value()'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs <= rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs' or 'lhs' and 'rhs' have the same value, and 'false'
    // otherwise.  (See 'operator<' and 'operator=='.)

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs <= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' optional object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator<' and 'operator=='.)

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' optional object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs >= rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' or 'lhs' and 'rhs' have the same value, and 'false'
    // otherwise.  (See 'operator>' and 'operator=='.)

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs >= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
;
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' optional object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator>' and 'operator=='.)

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                              const bsl::optional<t_LHS>& lhs,
                                              const bsl::optional<t_RHS>& rhs);
    // Perform a three-way comparison of the specified 'lhs' and the specified
    // 'rhs' objects by using the comparison operators of 't_LHS' and 't_RHS';
    // return the result of that comparison.

template <class t_LHS, class t_RHS>
requires (!Optional_DerivedFromOptional<t_RHS>) &&
         three_way_comparable_with<t_LHS, t_RHS>
compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                              const bsl::optional<t_LHS>& lhs,
                                              const t_RHS&                rhs);
    // Perform a three-way comparison of the specified 'lhs' and the specified
    // 'rhs' objects by using the comparison operators of 't_LHS' and 't_RHS';
    // return the result of that comparison.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR strong_ordering operator<=>(
                                  const bsl::optional<TYPE>& value,
                                  bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;
    // Perform a three-way comparison of the specified 'value' and 'nullopt';
    // return the result of that comparison.

template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                              const bsl::optional<t_LHS>& lhs,
                                              const std::optional<t_RHS>& rhs);
    // Perform a three-way comparison of the specified 'lhs' and the specified
    // 'rhs' objects by using the comparison operators of 't_LHS' and 't_RHS';
    // return the result of that comparison.

#endif

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(bsl::optional<TYPE>& lhs, std::optional<TYPE>& rhs);
template <class TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(std::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs);
    // Efficiently exchange the values of the specified 'lhs' and 'rhs'
    // objects.  This method provides the no-throw exception-safety guarantee
    // if the template parameter 'TYPE' provides that guarantee and the result
    // of the 'hasValue' method for 'lhs' and 'rhs' is the same.


// comparison with optional
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' optional objects have the
    // same value, and 'false' otherwise.  Two optional objects have the same
    // value if both are disengaged, or if both are engaged and the values of
    // their underlying objects compare equal.  Note that this function will
    // fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' optional objects do not
    // have the same value, and 'false' otherwise.  Two optional objects do not
    // have the same value if one is disengaged and the other is engaged, or if
    // both are engaged and the values of their underlying objects do not
    // compare equal.  Note that this function will fail to compile if
    // 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const bsl::optional<LHS_TYPE>& lhs,
               const std::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const std::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs' optional object, and 'false' otherwise.  'lhs' is
    // ordered before 'rhs' if 'lhs' is disengaged and 'rhs' is engaged or if
    // both are engaged and 'lhs.value()' is ordered before 'rhs.value()'.
    // Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const bsl::optional<LHS_TYPE>& lhs,
               const std::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const std::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' optional object, and 'false' otherwise.  'lhs' is
    // ordered after 'rhs' if 'lhs' is engaged and 'rhs' is disengaged or if
    // both are engaged and 'lhs.value()' is ordered after 'rhs.value()'.  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' optional object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator<' and 'operator=='.)  Note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' optional object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<TYPE>::type>
make_optional(bsl::allocator_arg_t,
              typename bsl::optional<
                  typename bsl::decay<TYPE>::type>::allocator_type const&,
              BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs);
    // Return an 'optional' object containing a 'TYPE' object created by
    // invoking a 'bsl::optional' allocator-extended 'in_place_t' constructor
    // with the specified 'alloc' as the allocator argument, and specified
    // 'rhs' as the constructor argument.  Note that this function will fail to
    // compile if TYPE doesn't use allocators.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional(
                     bsl::allocator_arg_t,
                     typename bsl::optional<TYPE>::allocator_type const& alloc,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...          args);
    // Return an 'optional' object containing a 'TYPE' object created by
    // invoking a 'bsl::optional' allocator-extended 'in_place_t' constructor
    // with the specified 'alloc' as the allocator argument, and specified
    // 'args' as constructor arguments.  Note that this function will fail to
    // compile if TYPE doesn't use allocators.

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION >= 1900))
// MSVC2013 has a bug that causes deduction issues in free template functions
// that have an 'std::initializer_list' argument where the
// 'std::initializer_list' element type is deduced.

template <class TYPE, class INIT_LIST_TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional(
                     bsl::allocator_arg_t,
                     typename bsl::optional<TYPE>::allocator_type const& alloc,
                     std::initializer_list<INIT_LIST_TYPE>               il,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...          args);
    // Return an 'optional' object containing a 'TYPE' object created by
    // invoking a 'bsl::optional' allocator-extended 'in_place_t' constructor
    // with the specified 'alloc' as the allocator argument, and specified 'il'
    // and 'args' as the constructor arguments.  Note that this function will
    // fail to compile if TYPE doesn't use allocators.
#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<TYPE>::type>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs);
    // Return an 'optional' object containing a 'TYPE' object created by
    // invoking a 'bsl::optional' constructor with the specified 'rhs' as the
    // constructor argument.  If TYPE uses an allocator, the default allocator
    // will be used for the 'optional' object.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional();
    // Return an 'optional' object containing a default constructed 'TYPE'
    // object.  If TYPE uses an allocator, the default allocator will be used
    // for the 'optional' object.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, class ARG, class... ARGS>
BSLS_KEYWORD_CONSTEXPR BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG(ARG)
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(ARG),
              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
    // Return an 'optional' object containing a 'TYPE' object created by
    // invoking a 'bsl::optional' 'in_place_t' constructor with the specified
    // arguments as the constructor arguments.  If TYPE uses an allocator, the
    // default allocator will be used for the 'optional' object.

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION >= 1900))
// MSVC2013 has a bug that causes deduction issues in free template functions
// that have an 'std::initializer_list' argument where the
// 'std::initializer_list' element type is deduced.

template <class TYPE, class INIT_LIST_TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional(
                              std::initializer_list<INIT_LIST_TYPE>      il,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Return an 'optional' object containing a 'TYPE' object created by
    // invoking a 'bsl::optional' 'in_place_t' constructor with the specified
    // 'il' and 'args' as the constructor arguments.  If TYPE uses an
    // allocator, the default allocator will be used for the 'optional' object.

#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslstl {

// ============================================================================
//                  Section: bslstl::Optional_* Definitions
// ============================================================================

                           // ======================
                           // class Optional_DataImp
                           // ======================

// CREATORS
template <class TYPE>
Optional_DataImp<TYPE>::Optional_DataImp() BSLS_KEYWORD_NOEXCEPT
: d_hasValue(false)
{
}

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
TYPE& Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
    return d_buffer.object();
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
TYPE& Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
    return d_buffer.object();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class TYPE>
void Optional_DataImp<TYPE>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue) {
      d_hasValue = false;
      bslma::DestructionUtil::destroy(d_buffer.address());
    }
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
TYPE& Optional_DataImp<TYPE>::value() &
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return d_buffer.object();
}

template <class TYPE>
inline
TYPE&& Optional_DataImp<TYPE>::value() &&
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return std::move(d_buffer.object());
}
# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
TYPE& Optional_DataImp<TYPE>::value()
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return d_buffer.object();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

// ACCESSORS
template <class TYPE>
inline
bool Optional_DataImp<TYPE>::hasValue() const BSLS_KEYWORD_NOEXCEPT
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return d_hasValue;
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
const TYPE& Optional_DataImp<TYPE>::value() const&
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return d_buffer.object();
}

template <class TYPE>
inline
const TYPE&& Optional_DataImp<TYPE>::value() const&&
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return std::move(d_buffer.object());
}
# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
const TYPE& Optional_DataImp<TYPE>::value() const
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'optional' method invoking 'value()'

    return d_buffer.object();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

                            // ===================
                            // class Optional_Data
                            // ===================

// CREATORS
template <class TYPE, bool IS_TRIVIALLY_DESTRUCTIBLE>
Optional_Data<TYPE, IS_TRIVIALLY_DESTRUCTIBLE>::~Optional_Data()
{
    this->reset();
}
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

// ============================================================================
//               Section: Allocator-Aware 'optional' Definitions
// ============================================================================

                            // ====================
                            // class optional<TYPE>
                            // ====================

// CREATORS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional() BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(bsl::nullopt_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(const optional& original)
{
    if (original.has_value()) {
        emplace(*original);
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                             BloombergLP::bslmf::MovableRef<optional> original)
                       BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                               bsl::is_nothrow_move_constructible<TYPE>::value)
: d_allocator(MoveUtil::access(original).get_allocator())
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class DERIVED>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                BloombergLP::bslmf::MovableRef<DERIVED> original,
                BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED),
                BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(optional, DERIVED))
            BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                               bsl::is_nothrow_move_constructible<TYPE>::value)
: d_allocator(MoveUtil::access(original).get_allocator())
{
    DERIVED&  dvalue = original;
    optional& lvalue = dvalue;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}
# endif

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                      value,
        BSLSTL_OPTIONAL_DEFINE_IF_SAME(TYPE, ANY_TYPE),
        BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
            BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                  value,
            BSLSTL_OPTIONAL_DEFINE_IF_SAME(TYPE, ANY_TYPE),
            BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                      value,
        BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
        BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
            BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)                  value,
            BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
            BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(TYPE,
                                                                     ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(TYPE,
                                                                     ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
   BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
   BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
   BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
   BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(TYPE, ANY_TYPE),
   BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: d_allocator(MoveUtil::access(original).get_allocator())
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
   BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
   BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
   BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
   BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(TYPE, ANY_TYPE),
   BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: d_allocator(MoveUtil::access(original).get_allocator())
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    const std::optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    const std::optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    std::optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    if (original.has_value()) {
        emplace(std::move(original.value()));
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    std::optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    if (original.has_value()) {
        emplace(std::move(original.value()));
    }
}
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class... ARGS>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                               bsl::in_place_t,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class INIT_LIST_TYPE, class... ARGS>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                               bsl::in_place_t,
                               std::initializer_list<INIT_LIST_TYPE>      il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(il, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(bsl::allocator_arg_t,
                                           allocator_type       allocator)
: d_allocator(allocator)
{
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(bsl::allocator_arg_t,
                                           allocator_type       allocator,
                                           bsl::nullopt_t)
: d_allocator(allocator)
{
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(bsl::allocator_arg_t,
                                           allocator_type       allocator,
                                           const optional&      original)
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(*original);
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                            bsl::allocator_arg_t,
                            allocator_type                           allocator,
                            BloombergLP::bslmf::MovableRef<optional> original)
: d_allocator(allocator)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class DERIVED>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                bsl::allocator_arg_t,
                allocator_type                          allocator,
                BloombergLP::bslmf::MovableRef<DERIVED> original,
                BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED),
                BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(optional, DERIVED))
: d_allocator(allocator)
{
    DERIVED&  dvalue = original;
    optional& lvalue = dvalue;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}
# endif

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                      bsl::allocator_arg_t,
                      allocator_type                                 allocator,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)    value,
                      BSLSTL_OPTIONAL_DEFINE_IF_SAME(ANY_TYPE, TYPE))
: d_allocator(allocator)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                    bsl::allocator_arg_t,
                    allocator_type                                   allocator,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)      value,
                    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE))
: d_allocator(allocator)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    bsl::allocator_arg_t,
    allocator_type            allocator,
    const optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                           const ANY_TYPE&))
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
  bsl::allocator_arg_t,
  allocator_type                                                     allocator,
  BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>)                        original,
  BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
  BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE))
: d_allocator(allocator)
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    bsl::allocator_arg_t,
    allocator_type                 allocator,
    const std::optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                           const ANY_TYPE&))
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
    bsl::allocator_arg_t,
    allocator_type            allocator,
    std::optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE))
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(std::move(original.value()));
    }
}

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class... ARGS>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                              bsl::allocator_arg_t,
                              allocator_type                             alloc,
                              bsl::in_place_t,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
: d_allocator(alloc)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class INIT_LIST_TYPE, class... ARGS>
inline
optional<TYPE, USES_BSLMA_ALLOC>::optional(
                              bsl::allocator_arg_t,
                              allocator_type                             alloc,
                              bsl::in_place_t,
                              std::initializer_list<INIT_LIST_TYPE>      il,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
: d_allocator(alloc)
{
    emplace(il, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class... ARGS>
inline
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return d_value.emplace(d_allocator.mechanism(),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class INIT_LIST_TYPE, class... ARGS>
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::emplace(
                               std::initializer_list<INIT_LIST_TYPE>      il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return d_value.emplace(d_allocator.mechanism(),
                           il,
                           BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
void optional<TYPE, USES_BSLMA_ALLOC>::reset() BSLS_KEYWORD_NOEXCEPT
{
    d_value.reset();
}

template <class TYPE, bool USES_BSLMA_ALLOC>
void optional<TYPE, USES_BSLMA_ALLOC>::swap(optional& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<TYPE>::value &&
                             bsl::is_nothrow_swappable<TYPE>::value)
{
    BSLS_ASSERT(d_allocator == other.d_allocator);

    if (this->has_value() && other.has_value()) {
        BloombergLP::bslalg::SwapUtil::swap(
                                         BSLS_UTIL_ADDRESSOF(d_value.value()),
                                         BSLS_UTIL_ADDRESSOF(*other));
    }
    else if (this->has_value()) {
        other.emplace(MoveUtil::move(d_value.value()));
        this->reset();
    }
    else if (other.has_value()) {
        this->emplace(MoveUtil::move(*other));
        other.reset();
    }
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::value() &
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE&& optional<TYPE, USES_BSLMA_ALLOC>::value() &&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::value()
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
TYPE optional<TYPE, USES_BSLMA_ALLOC>::value_or(ANY_TYPE&& value) &&
{
    if (has_value()) {
        return TYPE(std::move(d_value.value()));                      // RETURN
    }
    else {
        return TYPE(std::forward<ANY_TYPE>(value));                   // RETURN
    }
}

#  ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
TYPE optional<TYPE, USES_BSLMA_ALLOC>::value_or(bsl::allocator_arg_t,
                                                allocator_type       allocator,
                                                ANY_TYPE&&           value) &&
{
    if (has_value()) {
        return BloombergLP::bslma::ConstructionUtil::make<TYPE>(
            allocator.mechanism(), std::move(d_value.value()));
    }
    else {
        return BloombergLP::bslma::ConstructionUtil::make<TYPE>(
            allocator.mechanism(), std::forward<ANY_TYPE>(value));
    }
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
# endif  // BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>& optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                          bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    reset();
    return *this;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>& optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                                           const optional& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            d_value.value() = *rhs;
        }
        else {
            emplace(*rhs);
        }
    }
    else {
        reset();
    }
    return *this;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>& optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                  BloombergLP::bslmf::MovableRef<optional> rhs)
{
    optional& lvalue = rhs;

    if (lvalue.has_value()) {
        if (this->has_value()) {
            d_value.value() = MoveUtil::move(*lvalue);
        }
        else {
            emplace(MoveUtil::move(*lvalue));
        }
    }
    else {
        reset();
    }
    return *this;
}

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class DERIVED>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(TYPE, DERIVED)&
optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                   BloombergLP::bslmf::MovableRef<DERIVED> rhs)
{
    DERIVED&  dvalue = rhs;
    optional& lvalue = dvalue;

    if (lvalue.has_value()) {
        if (this->has_value()) {
            d_value.value() = MoveUtil::move(*lvalue);
        }
        else {
            emplace(MoveUtil::move(*lvalue));
        }
    }
    else {
        reset();
    }
    return *this;
}
# endif

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE) &
optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                               BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) rhs)
{
    optional<ANY_TYPE>& lvalue = rhs;

    if (lvalue.has_value()) {
        if (this->has_value()) {
            d_value.value() = MoveUtil::move(*lvalue);
        }
        else {
            emplace(MoveUtil::move(*lvalue));
        }
    }
    else {
        reset();
    }
    return *this;
}

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE)&
optional<TYPE, USES_BSLMA_ALLOC>::operator=(ANY_TYPE&& rhs)
{
    if (has_value()) {
        d_value.value() = std::forward<ANY_TYPE>(rhs);
    }
    else {
        emplace(std::forward<ANY_TYPE>(rhs));
    }
    return *this;
}
# else  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>& optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                                               const TYPE& rhs)
{
    if (has_value()) {
        d_value.value() = rhs;
    }
    else {
        emplace(rhs);
    }
    return *this;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
optional<TYPE, USES_BSLMA_ALLOC>& optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                      BloombergLP::bslmf::MovableRef<TYPE> rhs)
{
    TYPE& lvalue = rhs;

    if (has_value()) {
        d_value.value() = MoveUtil::move(lvalue);
    }
    else {
        emplace(MoveUtil::move(lvalue));
    }
    return *this;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE)&
optional<TYPE, USES_BSLMA_ALLOC>::operator=(const ANY_TYPE& rhs)
{
    if (has_value()) {
        d_value.value() = rhs;
    }
    else {
        emplace(rhs);
    }
    return *this;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE) &
optional<TYPE, USES_BSLMA_ALLOC>::operator=(
                                  BloombergLP::bslmf::MovableRef<ANY_TYPE> rhs)
{
    ANY_TYPE& lvalue = rhs;
    if (has_value()) {
        d_value.value() = MoveUtil::move(lvalue);
    }
    else {
        emplace(MoveUtil::move(lvalue));
    }
    return *this;
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES else

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, const ANY_TYPE&) &
optional<TYPE, USES_BSLMA_ALLOC>::operator=(const std::optional<ANY_TYPE>& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            d_value.value() = *rhs;
        }
        else {
            emplace(*rhs);
        }
    }
    else {
        reset();
    }
    return *this;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) &
optional<TYPE, USES_BSLMA_ALLOC>::operator=(std::optional<ANY_TYPE>&& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            d_value.value() = std::move(*rhs);
        }
        else {
            emplace(std::move(*rhs));
        }
    }
    else {
        reset();
    }
    return *this;
}
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE *optional<TYPE, USES_BSLMA_ALLOC>::operator->()
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::operator*() &
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE&& optional<TYPE, USES_BSLMA_ALLOC>::operator*() &&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::operator*()
{
   BSLS_ASSERT(has_value());

   return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

// ACCESSORS

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
typename optional<TYPE, USES_BSLMA_ALLOC>::allocator_type
optional<TYPE, USES_BSLMA_ALLOC>::get_allocator()
const BSLS_KEYWORD_NOEXCEPT
{
    return d_allocator;
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool optional<TYPE, USES_BSLMA_ALLOC>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.hasValue();
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE& optional<TYPE, USES_BSLMA_ALLOC>::value() const&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE&& optional<TYPE, USES_BSLMA_ALLOC>::value() const&&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE& optional<TYPE, USES_BSLMA_ALLOC>::value() const
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
TYPE optional<TYPE, USES_BSLMA_ALLOC>::value_or(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const&
{
    if (has_value()) {
        return TYPE(d_value.value());                                 // RETURN
    }
    else {
        return TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));  // RETURN
    }
}

#  ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
TYPE optional<TYPE, USES_BSLMA_ALLOC>::value_or(
                      bsl::allocator_arg_t,
                      allocator_type                              allocator,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const&
{
    if (has_value()) {
        return BloombergLP::bslma::ConstructionUtil::make<TYPE>(
            allocator.mechanism(), d_value.value());
    }
    else {
        return BloombergLP::bslma::ConstructionUtil::make<TYPE>(
            allocator.mechanism(),
            BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
    }
}
#  endif  // BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
TYPE optional<TYPE, USES_BSLMA_ALLOC>::value_or(
                       BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const
{
    if (has_value()) {
        return TYPE(d_value.value());                                 // RETURN
    }
    else {
        return TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));  // RETURN
    }
}

#  ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
template <class TYPE, bool USES_BSLMA_ALLOC>
template <class ANY_TYPE>
inline
TYPE optional<TYPE, USES_BSLMA_ALLOC>::value_or(
                       bsl::allocator_arg_t,
                       allocator_type                              allocator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value) const
{
    if (has_value()) {
        return BloombergLP::bslma::ConstructionUtil::make<TYPE>(
            allocator.mechanism(), d_value.value());
    }
    else {
        return BloombergLP::bslma::ConstructionUtil::make<TYPE>(
            allocator.mechanism(),
            BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
    }
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
# endif  // BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION else

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE *optional<TYPE, USES_BSLMA_ALLOC>::operator->() const
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE& optional<TYPE, USES_BSLMA_ALLOC>::operator*() const&
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE&& optional<TYPE, USES_BSLMA_ALLOC>::operator*() const&&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE, bool USES_BSLMA_ALLOC>
inline
const TYPE& optional<TYPE, USES_BSLMA_ALLOC>::operator*() const
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class TYPE, bool USES_BSLMA_ALLOC>
optional<TYPE, USES_BSLMA_ALLOC>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT

# ifndef BDE_OMIT_INTERNAL_DEPRECATED
// PROTECTED MANIPULATORS
template <class TYPE, bool USES_BSLMA_ALLOC>
TYPE& optional<TYPE, USES_BSLMA_ALLOC>::dereferenceRaw()
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();

}

// PROTECTED ACCESORS
template <class TYPE, bool USES_BSLMA_ALLOC>
const TYPE& optional<TYPE, USES_BSLMA_ALLOC>::dereferenceRaw() const
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();

}
# endif  // BDE_OMIT_INTERNAL_DEPRECATED

// ============================================================================
//            Section: C++17 Allocator-Unaware 'optional' Definitions
// ============================================================================

                        // ===========================
                        // class optional<TYPE, false>
                        // ===========================

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
// CREATORS
template <class TYPE>
inline
optional<TYPE, false>::optional() BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE>
inline
optional<TYPE, false>::optional(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    ANY_TYPE&& value,
    BSLSTL_OPTIONAL_DEFINE_IF_SAME(ANY_TYPE, TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(std::forward<ANY_TYPE>(value))
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    ANY_TYPE&& value,
    BSLSTL_OPTIONAL_DEFINE_IF_SAME(ANY_TYPE, TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(std::forward<ANY_TYPE>(value))
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    ANY_TYPE&& value,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(std::forward<ANY_TYPE>(value))
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    ANY_TYPE&& value,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(std::forward<ANY_TYPE>(value))
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&))
{
    if (original.has_value()) {
        this->emplace(original.value());
    }
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&))
{
    if (original.has_value()) {
        this->emplace(original.value());
    }
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    if (original.has_value()) {
        this->emplace(std::move(original.value()));
    }
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    if (original.has_value()) {
        this->emplace(std::move(original.value()));
    }
}

template <class TYPE>
template <class... ARGS>
inline
optional<TYPE, false>::optional(
    bsl::in_place_t,
    ARGS&&... args)
: OptionalBase(bsl::in_place, std::forward<ARGS>(args)...)
{
}

template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
inline
optional<TYPE, false>::optional(
    bsl::in_place_t,
    std::initializer_list<INIT_LIST_TYPE>      il,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
: OptionalBase(bsl::in_place, il, std::forward<ARGS>(args)...)
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const std::optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(original)
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const std::optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(original)
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    std::optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(std::move(original))
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    std::optional<ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
: OptionalBase(std::move(original))
{
}

template <class TYPE>
inline
optional<TYPE, false>& optional<TYPE, false>::
operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    this->reset();
    return *this;
}

// MANIPULATORS
template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, const ANY_TYPE&)&
optional<TYPE, false>::operator=(const optional<ANY_TYPE>& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            this->operator*() = *rhs;
        }
        else {
            this->emplace(*rhs);
        }
    }
    else {
        this->reset();
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE)&
optional<TYPE, false>::operator=(optional<ANY_TYPE>&& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            this->operator*() = std::move(*rhs);
        }
        else {
            this->emplace(std::move(*rhs));
        }
    }
    else {
        this->reset();
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, const ANY_TYPE&)&
optional<TYPE, false>::operator=(const std::optional<ANY_TYPE>& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            this->operator*() = *rhs;
        }
        else {
            this->emplace(*rhs);
        }
    }
    else {
        this->reset();
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(TYPE, ANY_TYPE)&
optional<TYPE, false>::operator=(std::optional<ANY_TYPE>&& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            this->operator*() = std::move(*rhs);
        }
        else {
            this->emplace(std::move(*rhs));
        }
    }
    else {
        this->reset();
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE)&
optional<TYPE, false>::operator=(ANY_TYPE&& rhs)
{
    if (this->has_value()) {
        this->operator*() = std::forward<ANY_TYPE>(rhs);
    }
    else {
        this->emplace(std::forward<ANY_TYPE>(rhs));
    }
    return *this;
}

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
// PROTECTED MANIPULATORS
template <class TYPE>
TYPE& optional<TYPE, false>::dereferenceRaw()
{
    // This method is provided for the purpose of allowing 'NullableValue'
    // to determine the assert level in its value() method.  Do not assert
    // here.

    return this->operator*();

}

// PROTECTED ACCESORS
template <class TYPE>
const TYPE& optional<TYPE, false>::dereferenceRaw() const
{
    // This method is provided for the purpose of allowing 'NullableValue'
    // to determine the assert level in its value() method.  Do not assert
    // here.

    return this->operator*();

}
#  endif  // BDE_OMIT_INTERNAL_DEPRECATED
# else  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// ============================================================================
//          Section: Pre-C++17 Allocator-Unaware 'optional' Definitions
// ============================================================================

// CREATORS
template <class TYPE>
inline
optional<TYPE, false>::optional() BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE>
inline
optional<TYPE, false>::optional(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE>
inline
optional<TYPE, false>::optional(const optional& original)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE>
inline
optional<TYPE, false>::optional(
    BloombergLP::bslmf::MovableRef<optional> original)
                  BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                               bsl::is_nothrow_move_constructible<TYPE>::value)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class DERIVED>
inline
optional<TYPE, false>::optional(
                BloombergLP::bslmf::MovableRef<DERIVED> original,
                BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED),
                BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(optional, DERIVED))
                BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                               bsl::is_nothrow_move_constructible<TYPE>::value)
{
    DERIVED&  dvalue = original;
    optional& lvalue = dvalue;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}
# endif

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
    BSLSTL_OPTIONAL_DEFINE_IF_SAME(ANY_TYPE, TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
    BSLSTL_OPTIONAL_DEFINE_IF_SAME(ANY_TYPE, TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANYTYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&))
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const optional<ANY_TYPE>& original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,
                                                           const ANY_TYPE&),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, const ANY_TYPE&))
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class TYPE>
template <class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(TYPE, ANY_TYPE))
{
    optional<ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
optional<TYPE, false>::optional(
    bsl::in_place_t,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
inline
optional<TYPE, false>::optional(
    bsl::in_place_t,
    std::initializer_list<INIT_LIST_TYPE>      il,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(il, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
TYPE& optional<TYPE, false>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return d_value.emplace(NULL, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
TYPE& optional<TYPE, false>::emplace(
    std::initializer_list<INIT_LIST_TYPE>      il,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return d_value.emplace(
        NULL, il, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class TYPE>
inline
void optional<TYPE, false>::reset() BSLS_KEYWORD_NOEXCEPT
{
    d_value.reset();
}

template <class TYPE>
void optional<TYPE, false>::swap(optional& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<TYPE>::value &&
                             bsl::is_nothrow_swappable<TYPE>::value)
{
    if (this->has_value() && other.has_value()) {
        BloombergLP::bslalg::SwapUtil::swap(
                                         BSLS_UTIL_ADDRESSOF(d_value.value()),
                                         BSLS_UTIL_ADDRESSOF(*other));
    }
    else if (this->has_value()) {
        other.emplace(MoveUtil::move(d_value.value()));
        this->reset();
    }
    else if (other.has_value()) {
        this->emplace(MoveUtil::move(*other));
        other.reset();
    }
}

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
TYPE& optional<TYPE, false>::value() &
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
template <class TYPE>
inline
TYPE&&
optional<TYPE, false>::value() &&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
TYPE& optional<TYPE, false>::value()
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, false>::value_or(ANY_TYPE&& value) &&
{
    if (has_value()) {
        return TYPE(std::move(d_value.value()));                      // RETURN
    }
    else {
        return TYPE(std::forward<ANY_TYPE>(value));                   // RETURN
    }
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class TYPE>
inline
optional<TYPE, false>& optional<TYPE, false>::
operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    reset();
    return *this;
}

template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(const optional& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            d_value.value() = *rhs;
        }
        else {
            emplace(*rhs);
        }
    }
    else {
        reset();
    }
    return *this;
}

template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(BloombergLP::bslmf::MovableRef<optional> rhs)
{
    optional& lvalue = rhs;
    if (lvalue.has_value()) {
        if (this->has_value()) {
            d_value.value() = MoveUtil::move(*lvalue);
        }
        else {
            emplace(MoveUtil::move(*lvalue));
        }
    }
    else {
        reset();
    }
    return *this;
}

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class DERIVED>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(TYPE, DERIVED)&
optional<TYPE, false>::operator=(BloombergLP::bslmf::MovableRef<DERIVED> rhs)
{
    DERIVED&  dvalue = rhs;
    optional& lvalue = dvalue;;
    if (lvalue.has_value()) {
        if (this->has_value()) {
            d_value.value() = MoveUtil::move(*lvalue);
        }
        else {
            emplace(MoveUtil::move(*lvalue));
        }
    }
    else {
        reset();
    }
    return *this;
}
# endif

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, const ANY_TYPE&)&
optional<TYPE, false>::operator=(const optional<ANY_TYPE>& rhs)
{
    if (rhs.has_value()) {
        if (this->has_value()) {
            d_value.value() = *rhs;
        }
        else {
            emplace(*rhs);
        }
    }
    else {
        reset();
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(TYPE, ANY_TYPE)&
optional<TYPE, false>::
operator=(BSLMF_MOVABLEREF_DEDUCE(optional<ANY_TYPE>) rhs)
{
    optional<ANY_TYPE>& lvalue = rhs;
    if (lvalue.has_value()) {
        if (this->has_value()) {
            d_value.value() = MoveUtil::move(*lvalue);
        }
        else {
            emplace(MoveUtil::move(*lvalue));
        }
    }
    else {
        reset();
    }
    return *this;
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(TYPE, ANY_TYPE)&
optional<TYPE, false>::operator=(ANY_TYPE&& rhs)
{
    if (has_value()) {
        d_value.value() = std::forward<ANY_TYPE>(rhs);
    }
    else {
        emplace(std::forward<ANY_TYPE>(rhs));
    }
    return *this;
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(const TYPE& rhs)
{
    if (has_value()) {
        d_value.value() = rhs;
    }
    else {
        emplace(rhs);
    }
    return *this;
}

template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(BloombergLP::bslmf::MovableRef<TYPE> rhs)
{
    TYPE& lvalue = rhs;

    if (has_value()) {
        d_value.value() = MoveUtil::move(lvalue);
    }
    else {
        emplace(MoveUtil::move(lvalue));
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE)&
optional<TYPE, false>::operator=(const ANY_TYPE& rhs)
{
    if (has_value()) {
        d_value.value() = rhs;
    }
    else {
        emplace(rhs);
    }
    return *this;
}

template <class TYPE>
template <class ANY_TYPE>
inline
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(TYPE, ANY_TYPE)&
optional<TYPE, false>::operator=(BloombergLP::bslmf::MovableRef<ANY_TYPE> rhs)
{
    ANY_TYPE& lvalue = rhs;
    if (has_value()) {
        d_value.value() = MoveUtil::move(lvalue);
    }
    else {
        emplace(MoveUtil::move(lvalue));
    }
    return *this;
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
TYPE& optional<TYPE, false>::operator*() &
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class TYPE>
inline
TYPE&& optional<TYPE, false>::operator*() &&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
TYPE& optional<TYPE, false>::operator*()
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class TYPE>
inline
TYPE *optional<TYPE, false>::operator->()
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

// ACCESSORS

template <class TYPE>
inline
bool optional<TYPE, false>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.hasValue();
}

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
const TYPE&
optional<TYPE, false>::value() const&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class TYPE>
inline
const TYPE&&
optional<TYPE, false>::value() const&&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
const TYPE&
optional<TYPE, false>::value() const
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class TYPE>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, false>::value_or(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value)
const &
{
    if (has_value()) {
        return TYPE(d_value.value());                                 // RETURN
    }
    else {
        return TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));  // RETURN
    }
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, false>::value_or(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) value)
const
{
    if (has_value()) {
        return TYPE(d_value.value());                                 // RETURN
    }
    else {
        return TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));  // RETURN
    }
}

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

template <class TYPE>
inline
const TYPE *optional<TYPE, false>::operator->() const
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::operator*() const&
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class TYPE>
inline
const TYPE&& optional<TYPE, false>::operator*() const&&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::operator*() const
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class TYPE>
optional<TYPE, false>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
// PROTECTED MANIPULATORS
template <class TYPE>
TYPE& optional<TYPE, false>::dereferenceRaw()
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();

}

// PROTECTED ACCESORS
template <class TYPE>
const TYPE& optional<TYPE, false>::dereferenceRaw() const
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();

}
#  endif  // BDE_OMIT_INTERNAL_DEPRECATED
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// FREE FUNCTIONS

template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const optional<TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;

    if (input.has_value()) {
        hashAppend(hashAlg, true);
        hashAppend(hashAlg, *input);
    }
    else {
        hashAppend(hashAlg, false);
    }
}

// ============================================================================
//                      Section: Free Function Definitions
// ============================================================================

template <class TYPE>
inline
typename bsl::enable_if<BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(bsl::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs)
{
    if (lhs.get_allocator() == rhs.get_allocator()) {
        lhs.swap(rhs);

        return;                                                       // RETURN
    }

    bsl::optional<TYPE> futureLhs(
        bsl::allocator_arg, lhs.get_allocator(), rhs);
    bsl::optional<TYPE> futureRhs(
        bsl::allocator_arg, rhs.get_allocator(), lhs);

    futureLhs.swap(lhs);
    futureRhs.swap(rhs);
}

template <class TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(bsl::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs)
{
    lhs.swap(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs == *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs == *rhs;                                          // RETURN
    }
    return lhs.has_value() == rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs != *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs != *rhs;                                          // RETURN
    }

    return lhs.has_value() != rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs == rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return lhs.has_value() && *lhs == rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs == *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return rhs.has_value() && lhs == *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs != rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return !lhs.has_value() || *lhs != rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs != *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return !rhs.has_value() || lhs != *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const bsl::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs < *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !lhs.has_value() || *lhs < *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs < rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return !lhs.has_value() || *lhs < rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs < *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return rhs.has_value() && lhs < *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const bsl::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs > *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !rhs.has_value() || *lhs > *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs > rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return lhs.has_value() && *lhs > rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs > *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return !rhs.has_value() || lhs > *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs <= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() && *lhs <= *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs <= rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return !lhs.has_value() || *lhs <= rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs <= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return rhs.has_value() && lhs <= *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const bsl::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires requires {
        { *lhs >= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }
    return lhs.has_value() && *lhs >= *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const bsl::optional<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<RHS_TYPE>) && requires {
        { *lhs >= rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return lhs.has_value() && *lhs >= rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const LHS_TYPE& lhs, const bsl::optional<RHS_TYPE>& rhs)
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    requires (!Optional_DerivedFromOptional<LHS_TYPE>) && requires {
        { lhs >= *rhs } -> Optional_ConvertibleToBool;
    }
#endif
{
    return !rhs.has_value() || lhs >= *rhs;
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
inline
compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                               const bsl::optional<t_LHS>& lhs,
                                               const bsl::optional<t_RHS>& rhs)
{
    const bool lhs_has_value = lhs.has_value(),
               rhs_has_value = rhs.has_value();
    if (lhs_has_value && rhs_has_value) {
        return *lhs <=> *rhs;
    }
    return lhs_has_value <=> rhs_has_value;
}

template <class t_LHS, class t_RHS>
requires (!Optional_DerivedFromOptional<t_RHS>) &&
         three_way_comparable_with<t_LHS, t_RHS>
inline
compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                               const bsl::optional<t_LHS>& lhs,
                                               const t_RHS&                rhs)
{
    if (lhs) {
        return *lhs <=> rhs;
    }
    return strong_ordering::less;
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR strong_ordering operator<=>(
                                   const bsl::optional<TYPE>& value,
                                   bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value() <=> false;
}

template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
inline
compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                               const bsl::optional<t_LHS>& lhs,
                                               const std::optional<t_RHS>& rhs)
{
    const bool lhs_has_value = lhs.has_value(),
               rhs_has_value = rhs.has_value();
    if (lhs_has_value && rhs_has_value) {
        return *lhs <=> *rhs;
    }
    return lhs_has_value <=> rhs_has_value;
}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON &&
        // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator==(
                             const bsl::optional<TYPE>& value,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator==(
                        const bsl::nullopt_t&,
                        const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                             const bsl::optional<TYPE>& value,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                        const bsl::nullopt_t&,
                        const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<(
                              const bsl::optional<TYPE>&,
                              const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<(
                        const bsl::nullopt_t&,
                        const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>(
                             const bsl::optional<TYPE>& value,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>(
                              const bsl::nullopt_t&,
                              const bsl::optional<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                             const bsl::optional<TYPE>& value,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                              const bsl::nullopt_t&,
                              const bsl::optional<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                              const bsl::optional<TYPE>&,
                              const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                        const bsl::nullopt_t&,
                        const bsl::optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<TYPE>::type>
make_optional(bsl::allocator_arg_t,
              typename bsl::optional<typename bsl::decay<TYPE>::type>::
                  allocator_type const&               alloc,
              BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs)
{
    return bsl::optional<typename bsl::decay<TYPE>::type>(
        bsl::allocator_arg,
        alloc,
        bsl::in_place,
        BSLS_COMPILERFEATURES_FORWARD(TYPE, rhs));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional(
                     bsl::allocator_arg_t,
                     typename bsl::optional<TYPE>::allocator_type const& alloc,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...          args)
{
    return bsl::optional<TYPE>(bsl::allocator_arg,
                               alloc,
                               bsl::in_place,
                               BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION >= 1900))

template <class TYPE, class INIT_LIST_TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional(
                     bsl::allocator_arg_t,
                     typename bsl::optional<TYPE>::allocator_type const& alloc,
                     std::initializer_list<INIT_LIST_TYPE>               il,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...          args)
{
    return bsl::optional<TYPE>(bsl::allocator_arg,
                               alloc,
                               bsl::in_place,
                               il,
                               BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<TYPE>::type>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs)
{
    return bsl::optional<typename bsl::decay<TYPE>::type>(
        BSLS_COMPILERFEATURES_FORWARD(TYPE, rhs));
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional()
{
    return bsl::optional<TYPE>(bsl::in_place);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, class ARG, class... ARGS>
BSLS_KEYWORD_CONSTEXPR BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG(ARG)
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(ARG)     arg,
              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return bsl::optional<TYPE>(bsl::in_place,
                               BSLS_COMPILERFEATURES_FORWARD(ARG, arg),
                               BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    (!defined(BSLS_PLATFORM_CMP_MSVC) || (BSLS_PLATFORM_CMP_VERSION >= 1900))

template <class TYPE, class INIT_LIST_TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<TYPE> make_optional(
                               std::initializer_list<INIT_LIST_TYPE>      il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return bsl::optional<TYPE>(
        bsl::in_place, il, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(bsl::optional<TYPE>& lhs, std::optional<TYPE>& rhs)
{
    lhs.swap(rhs);
}

template <class TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(std::optional<TYPE>& lhs, bsl::optional<TYPE>& rhs)
{
    lhs.swap(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs == *rhs;
    }
    return lhs.has_value() == rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs == *rhs;
    }
    return lhs.has_value() == rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs != *rhs;
    }

    return lhs.has_value() != rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs != *rhs;
    }

    return lhs.has_value() != rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const bsl::optional<LHS_TYPE>& lhs,
               const std::optional<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;
    }

    return !lhs.has_value() || *lhs < *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const std::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;
    }

    return !lhs.has_value() || *lhs < *rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const bsl::optional<LHS_TYPE>& lhs,
               const std::optional<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const std::optional<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const bsl::optional<LHS_TYPE>& lhs,
                const std::optional<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const std::optional<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
}  // close namespace bsl

// There is a problem in the standard definition of the is-optional concept
// that results in types inheriting from std::optional not being identified
// correctly as optional types.  The end result is endless recursion
// evaluating the requires clause for the spaceship operator when it is
// implemented according to the C++20 specification, which currently happens
// for gcc-11, gcc-12 and MSVC-2022 when building with C++20.
//
// The issue with the standard is tracked here:
// https://cplusplus.github.io/LWG/lwg-active.html#3746
//
// See DRQS 170388558 for more information
//
// BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED and
// BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_MSVC_WORKAROUND_NEEDED are deliberately
// exposed to allow their use in higher level optional types (i.e.,
// bdlb::NullableValue).

// The following hacks prevent compiler crashes, and should only be applied on
// compiler versions where the problem is not fixed.  The hacks required for
// gcc and for MSVC are different due to different standard library
// implementations, hence the two distinct macros and implementations.

# if BSLS_COMPILERFEATURES_CPLUSPLUS==202002L && \
     defined(BSLS_PLATFORM_CMP_GNU) && \
     defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) && \
     (BSLS_PLATFORM_CMP_VERSION > 110000 &&      \
      BSLS_PLATFORM_CMP_VERSION < 130000)

#define BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED

namespace std {
template<typename _Tp>
inline constexpr bool __is_optional_v<bsl::optional<_Tp>> = true;
}

# endif // BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED

# if BSLS_COMPILERFEATURES_CPLUSPLUS == 202002L &&                            \
     defined(BSLS_PLATFORM_CMP_MSVC) &&                                       \
     (BSLS_PLATFORM_CMP_VERSION >= 1930 && BSLS_PLATFORM_CMP_VERSION <= 1934)

#define BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_MSVC_WORKAROUND_NEEDED

namespace std {
template <typename _Tp>
inline constexpr bool _Is_specialization_v<bsl::optional<_Tp>, std::optional> =
    true;
}

# endif // BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_MSVC_WORKAROUND_NEEDED

#undef BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL
#undef BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL
#undef BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL
#undef BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL
#undef BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR
#undef BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR
#undef BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR
#undef BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR
#undef BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM
#undef BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM
#undef BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL
#undef BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL
#undef BSLSTL_OPTIONAL_DECLARE_IF_SAME
#undef BSLSTL_OPTIONAL_DEFINE_IF_SAME
#undef BSLSTL_OPTIONAL_DECLARE_IF_NOT_SAME
#undef BSLSTL_OPTIONAL_DEFINE_IF_NOT_SAME
#undef BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL
#undef BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL
#undef BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED
#undef BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG

#endif // End C++11 code

#endif // INCLUDED_BSLSTL_OPTIONAL

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
