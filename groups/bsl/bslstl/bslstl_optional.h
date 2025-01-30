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
// `bsl::optional<TYPE>`, that implements a notion of object that may or may
// not contain a `TYPE` value.  This template class also provides an interface
// to check if the optional object contains a value or not, as well as a
// contextual conversion to `bool`.  If an optional object is engaged, i.e.
// contains a value, it will evaluate to `true` when converted to `bool`;
// otherwise, it will evaluate to `false` when converted to `bool`.
//
// An optional object is engaged if it has been initialized with, or assigned
// from an object of `TYPE` or another engaged optional object, or if the value
// was created using the `emplace` method.  Other types of assignment and
// initialization (including default initialization), as well as calling
// `reset` method will result in the optional object being disengaged.
//
// If the underlying `TYPE` has value-semantics, then so will the type
// `bsl::optional<TYPE>`.  Two homogeneous optional objects have the same value
// if their underlying (non-null) `TYPE` values are the same, or both are null.
//
// Note that the object of template parameter `TYPE` that is managed by a
// `bsl::optional<TYPE>` object is created *in*-*place*.  Consequently, the
// template parameter `TYPE` must be a complete type when the class is
// instantiated.
//
// In addition to the standard homogeneous, value-semantic, operations such as
// copy/move construction, copy/move assignment, equality comparison, and
// relational operators, `bsl::optional` also supports conversion between
// optional types for which the underlying types are convertible, i.e., for
// heterogeneous copy construction, copy assignment, and equality comparison
// (e.g., between `int` and `double`); attempts at conversion between
// incompatible types, such as `int` and `bsl::string`, will fail to compile.
//
// For allocator-aware types, bsl::optional uses the same allocator for all
// `value_type` objects it manages during its lifetime.
//
///Sections:
///--------
// This file is very complex, a navigation map of this file can be obtained by:
// `$ grep -n Section: bslstl_optional.h`
//
///Usage
///-----
// The following snippets of code illustrate use of this component:
//
// First, create a `optional` `int` object:
// ```
// bsl::optional<int> optionalInt;
// assert(!optionalInt.has_value());
// ```
// Next, give the `int` object the value 123 (making it non-null):
// ```
// optionalInt.emplace(123);
// assert( optionalInt.has_value());
// assert(123 == optionalInt.value());
// ```
// Finally, reset the object to its default constructed state (i.e., null):
// ```
// optionalInt.reset();
// assert(!optionalInt.has_value());
// ```
//
///Known limitations
///-----------------
// * For assignment/construction constraints, we use `is_constructible` but
//   the exact creation will be done using allocation construction that will
//   invoke an allocator-extended constructor for allocator-aware types.
//   If the `value_type` is constructible from the assignment/constructor
//   argument, but doesn't have a corresponding allocator-extended
//   constructor, the overload selection may not be be correct.
// * `optional<const TYPE>` is fully supported in C++11 and onwards. However,
//   due to limitations of `MovableRef<const TYPE>`, C++03 support for const
//   `value_type`s is limited and move semantics of such an `optional` in
//   C++03 will not work.

#include <bslscm_version.h>

#include <bslstl_badoptionalaccess.h>
#include <bslstl_compare.h>
#include <bslstl_hash.h>
#include <bslstl_inplace.h>

#include <bslalg_swaputil.h>

#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_bslallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_decay.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isaccessiblebaseof.h>
#include <bslmf_isbitwisecopyable.h>
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
// clang-format off
// Include version that can be compiled with C++03
// Generated on Mon Jan 13 08:31:39 2025
// Command line: sim_cpp11_features.pl bslstl_optional.h

# define COMPILING_BSLSTL_OPTIONAL_H
# include <bslstl_optional_cpp03.h>
# undef COMPILING_BSLSTL_OPTIONAL_H

// clang-format on
#else

// ============================================================================
//                      Section: BSLSTL_OPTIONAL_* Macros
// ============================================================================

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) && \
   !(defined(BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED) &&          \
    (BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED < 17))

/// The following macro `BSLSTL_OPTIONAL_USES_STD_ALIASES` controls whether
/// platform standard library is used as a basis for `bsl` types in the
/// following ways:
///  - `bsl::nullopt` is an alias to `std::nullopt`
///
///  - `bsl::optional<T, false>` inherits from `std::optional<T>`, i.e.,
///    `std::optional` is used for types that are *not* allocator-aware.
///
/// This macro does *not* control whether there are conversions and comparisons
/// between `std::optional` and `bsl::optional`, that is conditioned only on
/// whether `std::optional` is available (i.e.,
/// `BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY` is defined).
    #define BSLSTL_OPTIONAL_USES_STD_ALIASES
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY & not disabled

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

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
#define BSLSTL_OPTIONAL_CONVERTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE)            \
    BSLSTL_OPTIONAL_CONVERTS_FROM(                                            \
        TYPE,                                                                 \
        std::optional<typename bsl::remove_cvref<ANY_TYPE>::type>)

#define BSLSTL_OPTIONAL_ASSIGNS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE)             \
    BSLSTL_OPTIONAL_ASSIGNS_FROM(                                             \
        TYPE,                                                                 \
        std::optional<typename bsl::remove_cvref<ANY_TYPE>::type>)
# else  // BSLSTL_OPTIONAL_USES_STD_ALIASES
// The value of these macros is chosen to not affect the constraints these
// macros appears in.
#define BSLSTL_OPTIONAL_CONVERTS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) false
#define BSLSTL_OPTIONAL_ASSIGNS_FROM_STD_OPTIONAL(TYPE, ANY_TYPE) false

# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES else

// Macros to define common constraints that enable a constructor or assignment
// operator.
#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(TYPE,          \
                                                               ANY_TYPE)      \
    typename bsl::enable_if<                                                  \
        !bsl::is_same<TYPE, ANY_TYPE>::value &&                               \
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

#define BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(TYPE, ANY_TYPE)             \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Optional_ConstructsFromType<TYPE,                \
                                                         ANY_TYPE>::value,    \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(TYPE, ANY_TYPE)            \
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(                                \
                     TYPE,                                                    \
                     ANY_TYPE) = BloombergLP::bslstl::Optional_OptNoSuchType(0)

#define BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED)              \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslmf::IsAccessibleBaseOf<optional, DERIVED>::value &&   \
        !bsl::is_const<DERIVED>::value,                                       \
        BloombergLP::bslstl::Optional_OptNoSuchType>::type

#define BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(DERIVED)             \
    BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(DERIVED) =                \
                                 BloombergLP::bslstl::Optional_OptNoSuchType(0)

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
        !bsl::is_same<bsl::optional<TYPE>,                                    \
                      typename bsl::remove_cvref<ANY_TYPE>::type>::value &&   \
            !(bsl::is_same<TYPE,                                              \
                           typename bsl::decay<ANY_TYPE>::type>::value &&     \
              std::is_scalar<TYPE>::value) &&                                 \
            std::is_constructible<TYPE, ANY_TYPE>::value &&                   \
            std::is_assignable<TYPE, ANY_TYPE>::value,                        \
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
        optional<t_TYPE> >::type

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
  #define BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG(ARG) = ARG
#else
  #define BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG(ARG)
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
  #define BSLSTL_OPTIONAL_REQUIRES(EXPR) requires(EXPR)
#else
  #define BSLSTL_OPTIONAL_REQUIRES(EXPR)
#endif

namespace BloombergLP {
namespace bslstl {
/// This trivial tag type is used to create `nullopt_t` objects prior to
/// C++17.
struct Optional_NulloptConstructToken {
};
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES

using nullopt_t = std::nullopt_t;
using std::nullopt;

# else  // BSLSTL_OPTIONAL_USES_STD_ALIASES

                              // ================
                              // struct nullopt_t
                              // ================

/// This trivial tag type is used to create `optional` objects in a disengaged
/// state.  It uses a constructor template to avoid being constructible from
/// any braced list.  See implementation notes for more information.
struct nullopt_t {

    // CREATORS

    /// Create a `nullopt_t` object.  Note that the argument is not used.
    template <class t_TYPE>
    explicit BSLS_KEYWORD_CONSTEXPR nullopt_t(
         t_TYPE,
         typename enable_if<is_same<t_TYPE,
                                    BloombergLP::bslstl::
                                        Optional_NulloptConstructToken>::value,
                            int>::type = 0) BSLS_KEYWORD_NOEXCEPT
    {
    }
};

/// Value of type `nullopt_t` used as an argument to functions that take a
/// `nullopt_t` argument.
#  if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr
nullopt_t nullopt(BloombergLP::bslstl::Optional_NulloptConstructToken{});
#  else
extern const nullopt_t nullopt;
#  endif

# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

template <class t_TYPE>
class optional;

}  // close namespace bsl

// ============================================================================
//                Section: bslstl::Optional_* Utility Classes
// ============================================================================

namespace BloombergLP {
namespace bslstl {

                // =======================================
                // struct Optional_IsTriviallyDestructible
                // =======================================

# ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

/// This metafunction is derived from
/// `std::is_trivially_destructible<t_TYPE>` in C++11 and later.  In C++03,
/// the metafunction is derived from `bsl::is_trivially_copyable`, a trait
/// that implies the type is also trivially destructible.
template <class t_TYPE>
struct Optional_IsTriviallyDestructible
: std::is_trivially_destructible<t_TYPE> {
};

# else  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

/// C++03 does not provide a trivially destructible trait.  Instead we use
/// `bslmf::IsBitwiseCopyable` which implies the type is also trivially
/// destructible.
///
/// Note that we use `bslmf::IsBitwiseCopyable` here and not
/// `bsl::is_trivially_copyable` because on some platforms, the native
/// `std::is_trivially_copyable<Optional_Data<t_TYPE, true>>` is `false` even
/// though it has no dtor and `TYPE` is trivally copyable.
template <class t_TYPE>
struct Optional_IsTriviallyDestructible : bslmf::IsBitwiseCopyable<t_TYPE> {
};

# endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY else

                        // ============================
                        // class Optional_OptNoSuchType
                        // ============================

/// This component-private trivial tag type is used to distinguish between
/// arguments passed by a user, and an `enable_if` argument.  It is not default
/// constructible so the following construction never invokes a constrained
/// single parameter constructor:
/// ```
///  optional<SomeType> o(int, {});
/// ```
struct Optional_OptNoSuchType {

    // CREATORS

    /// Create an `Optional_OptNoSuchType` object.  Note that the argument is
    /// not used.
    explicit BSLS_KEYWORD_CONSTEXPR Optional_OptNoSuchType(
                                                    int) BSLS_KEYWORD_NOEXCEPT;
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

/// This metafunction is derived from `bsl::true_type` if `t_TYPE` is an
/// allocator-aware const type, and if `t_ANY_TYPE` is the same as `t_TYPE`,
/// minus the cv qualification.  This trait is used to enable a constructor
/// overload for a const qualified allocator-aware `ValueType` taking an rvalue
/// of Optional_Base of the non-const qualified `ValueType`.  Such an overload
/// needs to propagate the allocator.
template <class t_TYPE, class t_ANY_TYPE>
struct Optional_PropagatesAllocator
: bsl::integral_constant<
      bool,
      bslma::UsesBslmaAllocator<t_TYPE>::value &&
          bsl::is_const<t_TYPE>::value &&
          bsl::is_same<t_ANY_TYPE,
                       typename bsl::remove_cv<t_TYPE>::type>::value> {
};

/// This metafunction is derived from `bsl::true_type` if `t_ANY_TYPE` is not
/// derived from `bsl::optional<t_TYPE>`, `t_ANY_TYPE` is not a tag type, and
/// `t_TYPE` is constructible from `t_ANY_TYPE`.
template <class t_TYPE, class t_ANY_TYPE>
struct Optional_ConstructsFromType
: bsl::integral_constant<
      bool,
      !bslmf::IsAccessibleBaseOf<
          bsl::optional<t_TYPE>,
          typename bsl::remove_cvref<t_ANY_TYPE>::type>::value &&
      !bsl::is_same<typename bsl::remove_cvref<t_ANY_TYPE>::type,
                    bsl::nullopt_t>::value &&
      !bsl::is_same<typename bsl::remove_cvref<t_ANY_TYPE>::type,
                    bsl::in_place_t>::value &&
      !bsl::is_same<typename bsl::remove_cvref<t_ANY_TYPE>::type,
                    bsl::allocator_arg_t>::value &&
      BSLSTL_OPTIONAL_IS_CONSTRUCTIBLE(t_TYPE, t_ANY_TYPE, true)> {
};

                        // ===========================
                        // Component-Private Tag Types
                        // ===========================

// The types defined in this section are used to select particular templated
// constructors of 'class Optional_Base'.  This avoids the need to duplicate
// constraints between 'optional' and 'Optional_Base'.

struct Optional_ConstructFromForwardRef {};
struct Optional_CopyConstructFromOtherOptional {};
struct Optional_MoveConstructFromOtherOptional {};
struct Optional_CopyConstructFromStdOptional {};
struct Optional_MoveConstructFromStdOptional {};

                           // ======================
                           // class Optional_DataImp
                           // ======================

/// This component-private `struct` manages a `value_type` object in an
/// `Optional_Base` object.  This class provides an abstraction for `const`
/// value type.  An `Optional_Base` object may contain an object of `const`
/// type.  An assignment to such an `Optional_Base` object should not succeed.
/// However, unless the `Optional_Base` object itself is `const`, it should be
/// possible to change the value of the `Optional_Base` object using `emplace`.
/// In order to allow for that, this class manages a non-const object of
/// `value_type`, but all the accessors return a `const` adjusted reference to
/// the managed object.
template <class t_TYPE>
struct Optional_DataImp {

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<t_TYPE>::type StoredType;

    // DATA

    // in-place `TYPE` object
    bsls::ObjectBuffer<StoredType> d_buffer;

    // `true` if object has a value, and `false` otherwise
    bool                           d_hasValue;

  public:
    // CREATORS

    /// Create an empty `Optional_DataImp` object.
    Optional_DataImp() BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an object of `StoredType` in `d_buffer` using the specified
    /// `allocator` and `args` and return a reference providing modifiable
    /// access to the underlying `t_TYPE` object.
    template <class... t_ARGS>
    t_TYPE& emplace(bslma::Allocator                            *allocator,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create an object of `StoredType` in `d_buffer` using the specified
    /// `allocator`, `initializer_list`, and `args`, and return a reference
    /// providing modifiable access to the underlying `t_TYPE` object.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    t_TYPE& emplace(
                 bslma::Allocator                            *allocator,
                 std::initializer_list<t_INIT_LIST_TYPE>      initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    /// Destroy the `value_type` object in `d_buffer`, if any.
    void reset() BSLS_KEYWORD_NOEXCEPT;

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return the `value_type` object in `d_buffer` with const
    /// qualification adjusted to match that of `t_TYPE`.  The behavior is
    /// undefined unless `this->hasValue() == true`.
    t_TYPE&  value() &;
    t_TYPE&& value() &&;
# else
    /// Return the `value_type` object in `d_buffer` with const
    /// qualification adjusted to match that of `t_TYPE`.  The behavior is
    /// undefined unless `this->hasValue() == true`.
    t_TYPE& value();
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    // ACCESSORS

    /// Return `true` if this objects has a value, and `false` otherwise.
    bool hasValue() const BSLS_KEYWORD_NOEXCEPT;

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return the `value_type` object in `d_buffer` with const
    /// qualification adjusted to match that of `t_TYPE`.  The behavior is
    /// undefined unless `this->hasValue() == true`.
    const t_TYPE&  value() const &;
    const t_TYPE&& value() const &&;

# else
    /// Return the `value_type` object in `d_buffer` with const qualification
    /// adjusted to match that of `t_TYPE`.  The behavior is undefined unless
    /// `this->hasValue() == true`.
    const t_TYPE& value() const;
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else
};

                            // ===================
                            // class Optional_Data
                            // ===================

/// This component-private `struct` manages a `value_type` object in
/// `Optional_Base` by inheriting from `Optional_DataImp`.  In addition,
/// this primary template properly destroys the owned instance of `t_TYPE`
/// in its destructor.
template <
    class t_TYPE,
    bool  t_IS_TRIVIALLY_DESTRUCTIBLE =
        bslstl::Optional_IsTriviallyDestructible<t_TYPE>::value>
struct Optional_Data : public Optional_DataImp<t_TYPE> {

  public:
    // CREATORS

    /// Destroy the managed `value_type` object, if it exists.
    ~Optional_Data();
};

                            // ===================
                            // class Optional_Data
                            // ===================

/// This partial specialization manages a trivially destructible
/// `value_type` in Optional_Base.  It does not have a user-provided
/// destructor, which makes it `is_trivially_destructible` itself.
template <class t_TYPE>
struct Optional_Data<t_TYPE, true> : public Optional_DataImp<t_TYPE> {

  public:
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Optional_Data,
                                      bslmf::IsBitwiseCopyable,
                                      bslmf::IsBitwiseCopyable<t_TYPE>::value);
};

                         // ==========================
                         // Component-private concepts
                         // ==========================

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
template <class t_TYPE>
void optional_acceptsBslOptional(const bsl::optional<t_TYPE>&);

template <class t_TYPE>
void optional_acceptsStdOptional(const std::optional<t_TYPE>&);

// This component-private concept models the Standard's exposition-only
// `boolean-testable` concept.
template <class t_TYPE>
concept Optional_ConvertibleToBool =
    bsl::is_convertible_v<t_TYPE, bool>;

// This component-private concept is used in the subsequent implementation
// of the component-private concept `Optional_DerivedFromOptional`.
template <class t_TYPE>
concept Optional_DerivedFromBslOptional =
    requires (const t_TYPE& t) { optional_acceptsBslOptional(t); };

// This component-private concept is used in the subsequent implementation
// of the component-private concept `Optional_DerivedFromOptional`.
template <class t_TYPE>
concept Optional_DerivedFromStdOptional =
    requires (const t_TYPE& t) { optional_acceptsStdOptional(t); };

/// This component-private concept models whether a type is derived from one
/// of `std::optional` or `bsl::optional`.
template <class t_TYPE>
concept Optional_DerivedFromOptional =
    Optional_DerivedFromBslOptional<t_TYPE> ||
    Optional_DerivedFromStdOptional<t_TYPE>;
#endif

// ============================================================================
//           Section: Definition of Allocator-Aware 'Optional_Base'
// ============================================================================

                            // ===================
                            // class Optional_Base
                            // ===================

/// This component-private class template implements the functionality of
/// `bsl::optional`.  The primary template is instantiated when `TYPE` is
/// allocator-aware, and holds the allocator that is used to create the
/// stored object.
template <class t_TYPE,
          bool  t_USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value>
class Optional_Base {

  public:
    // TYPES

    /// `value_type` is an alias for the underlying `TYPE` upon which this
    /// template class is instantiated, and represents the type of the
    /// managed object.  The name is chosen so it is compatible with the
    /// `std::optional` implementation.
    typedef t_TYPE value_type;

    typedef bsl::allocator<char> allocator_type;

  protected:
    // PROTECTED TYPES
    typedef allocator_type AllocType;

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

# ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    /// This type is needed only in C++03 mode, where `explicit` conversion
    /// operators are not supported.  An `optional` object is contextually
    /// converted to `UnspecifiedBool` when used in `if` statements, but is not
    /// implicitly convertible to `bool`.
    typedef BloombergLP::bsls::UnspecifiedBool<Optional_Base>
                                                   UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType UnspecifiedBool;
# endif

    // DATA

    // in-place `TYPE` object
    BloombergLP::bslstl::Optional_Data<t_TYPE> d_value;

    // allocator to be used for all in-place `TYPE` objects
    allocator_type                             d_allocator;

  protected:
    // PROTECTED CREATORS

    /// Create a disengaged `Optional_Base` object.  Use the currently
    /// installed default allocator to supply memory.
    Optional_Base();

    /// Create a disengaged `Optional_Base` object.  Use the currently
    /// installed default allocator to supply memory.
    Optional_Base(bsl::nullopt_t);                                  // IMPLICIT

    /// Create an `Optional_Base` object having the value of the specified
    /// `original` object.  Use the currently installed default allocator to
    /// supply memory.
    Optional_Base(const Optional_Base& original);

    /// Create an `Optional_Base` object having the same value as the
    /// specified `original` object by moving the contents of `original` to
    /// the newly-created object.  The allocator associated with `original`
    /// is propagated for use in the newly-created object.  `original` is
    /// left in a valid, but unspecified state.
    Optional_Base(BloombergLP::bslmf::MovableRef<Optional_Base> original)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                            bsl::is_nothrow_move_constructible<t_TYPE>::value);

    /// Create an `Optional_Base` object whose contained value is
    /// initialized by forwarding from the specified `value`.  Use the
    /// currently installed default allocator to supply memory.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value);

    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` converted to `t_TYPE` otherwise.
    /// Use the currently installed default allocator to supply memory.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original);

    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` converted to `t_TYPE` otherwise.
    /// Use the currently installed default allocator to supply memory.
    /// `original` is left in a valid but unspecified state.
    template <class t_ANY_TYPE>
    Optional_Base(
             BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
             BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(
                 t_TYPE,
                 t_ANY_TYPE));

    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` otherwise.  This is a special case
    /// constructor where `t_ANY_TYPE` is a non-const version of `t_TYPE`
    /// and we use the allocator from `original` to supply memory.
    /// `original` is left in a valid but unspecified state.
    template <class t_ANY_TYPE>
    Optional_Base(
        BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
        BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(t_TYPE,
                                                                  t_ANY_TYPE));

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` converted to `t_TYPE` otherwise.
    /// Use the currently installed default allocator to supply memory.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                  const std::optional<t_ANY_TYPE>& original);

    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` converted to `t_TYPE` otherwise.
    /// Use the currently installed default allocator to supply memory.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                  std::optional<t_ANY_TYPE>&& original);
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an `Optional_Base` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `args`.
    /// Use the currently installed default allocator to supply memory.
    template <class... t_ARGS>
    explicit Optional_Base(bsl::in_place_t,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create an `Optional_Base` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `il` and
    /// specified `args`.  Use the currently installed default allocator to
    /// supply memory.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    Optional_Base(bsl::in_place_t,
                  std::initializer_list<t_INIT_LIST_TYPE>      il,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // allocator-extended constructors

    /// Create a disengaged `Optional_Base` object.  Use the specified
    /// `allocator` to supply memory.
    Optional_Base(bsl::allocator_arg_t, allocator_type allocator);

    /// Create a disengaged `Optional_Base` object.  Use the specified
    /// `allocator` to supply memory.
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type allocator,
                  bsl::nullopt_t);

    /// If the specified `original` contains a value, create an
    /// `Optional_Base` object whose contained value is initialized from
    /// `*original`.  Otherwise, create a disengaged `Optional_Base` object.
    /// Use the specified `allocator` to supply memory.
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type       allocator,
                  const Optional_Base& original);

    /// If the specified `original` contains a value, create an
    /// `Optional_Base` object whose contained value is initialized by
    /// moving `*original`.  Otherwise, create a disengaged `Optional_Base`
    /// object.  Use the specified `allocator` to supply memory.  `original`
    /// is left in a valid but unspecified state.
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                                allocator,
                  BloombergLP::bslmf::MovableRef<Optional_Base> original);

    /// Create an `Optional_Base` object whose contained value is
    /// initialized by forwarding from the specified `value`.  Use the
    /// specified `allocator` to supply memory.
    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                                allocator,
                  BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value);

    /// If the specified `original` contains a value, create an
    /// `Optional_Base` object whose contained value is initialized from
    /// `*original`, converted to `t_TYPE`.  Otherwise, create a disengaged
    /// `Optional_Base`.  Use the specified `allocator` to supply memory.
    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                   allocator,
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original);

    /// If the specified `original` contains a value, create an
    /// `Optional_Base` object whose contained value is initialized by
    /// moving from `*original` and converting to `t_TYPE`.  Otherwise,
    /// create a disengaged `Optional_Base`.  Use the specified `allocator`
    /// to supply memory.  `original` is left in a valid but unspecified
    /// state.
    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                                     allocator,
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original);

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
    /// If the specified `original` contains a value, create an
    /// `Optional_Base` object whose contained value is initialized from
    /// `*original`, converted to `t_TYPE`.  Otherwise, create a disengaged
    /// `Optional_Base` object.  Use the specified `allocator` to supply
    /// memory.
    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                   allocator,
                  BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                  const std::optional<t_ANY_TYPE>& original);

    /// If the specified `original` contains a value, create an
    /// `Optional_Base` object whose contained value is initialized from
    /// moving from `*original` and converting to `t_TYPE`.  Otherwise,
    /// create a disengaged `Optional_Base`.  Use the specified `allocator`
    /// to supply memory.  `original` is left in a valid but unspecified
    /// state.
    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type              allocator,
                  BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                  std::optional<t_ANY_TYPE>&& original);
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an `Optional_Base` object whose contained value is
    /// initialized from the specified `args`.  Use the specified
    /// `allocator` to supply memory.
    template <class... t_ARGS>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                               allocator,
                  bsl::in_place_t,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create an `Optional_Base` object whose contained value is
    /// initialized from the specified `il` and `args`.  Use the specified
    /// `allocator` to supply memory.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    Optional_Base(bsl::allocator_arg_t,
                  allocator_type                               allocator,
                  bsl::in_place_t,
                  std::initializer_list<t_INIT_LIST_TYPE>      il,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // PROTECTED MANIPULATORS

    /// If `*this` holds an object, assign to that object the value of the
    /// specified `rhs`, converted to `t_TYPE`.  Otherwise, construct a held
    /// object from `rhs`, converted to `t_TYPE`.  The allocators of `*this`
    /// and `rhs` remain unchanged.
    template <class t_ANY_TYPE>
    void assignOrEmplace(BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) rhs);

# ifndef BDE_OMIT_INTERNAL_DEPRECATED
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.  Note that this function is only intended to
    /// be called by `bdlb::NullableValue::value` during transition of its
    /// implementation to use 'bsl::Optional_Base.  Note that ref-qualified
    /// versions of `value()` are not provided because `NullableValue` does
    /// not require them.
    t_TYPE& dereferenceRaw();

    // PROTECTED ACCESSORS

    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.  Note that this function is only intended to
    /// be called by `bdlb::NullableValue::value` during transition of its
    /// implementation to use 'bsl::Optional_Base.  Note that ref-qualified
    /// versions of `value()` are not provided because `NullableValue` does
    /// not require them.
    const t_TYPE& dereferenceRaw() const;
# endif  // BDE_OMIT_INTERNAL_DEPRECATED

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Optional_Base,
                                   BloombergLP::bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(Optional_Base,
                                   BloombergLP::bslmf::UsesAllocatorArgT);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         Optional_Base,
                         BloombergLP::bslmf::IsBitwiseMoveable,
                         BloombergLP::bslmf::IsBitwiseMoveable<t_TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         Optional_Base,
                         BloombergLP::bslmf::IsBitwiseCopyable,
                         BloombergLP::bslmf::IsBitwiseCopyable<t_TYPE>::value);

    // MANIPULATORS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Assign to this `Optional_Base` object the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `args` and
    /// return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  If this `Optional_Base` object already contains an
    /// object (`true == hasValue()`), that object is destroyed before the
    /// new object is created.  The allocator specified at the construction
    /// of this `Optional_Base` object is used to supply memory to the value
    /// object.  Attempts to explicitly specify via `args` another allocator
    /// to supply memory to the created (value) object are disallowed by the
    /// compiler.  Note that if the constructor of `t_TYPE` throws an
    /// exception this object is left in a disengaged state.
    template <class... t_ARGS>
    t_TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Assign to this `Optional_Base` object the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `il` and
    /// specified `args` and return a reference providing modifiable access
    /// to the underlying `t_TYPE` object.  If this `Optional_Base` object
    /// already contains an object (`true == hasValue()`), that object is
    /// destroyed before the new object is created.  The allocator specified
    /// at the construction of this `Optional_Base` object is used to supply
    /// memory to the value object.  Attempts to explicitly specify via
    /// `args` another allocator to supply memory to the created (value)
    /// object are disallowed by the compiler.  Note that if the constructor
    /// of `t_TYPE` throws an exception this object is left in a disengaged
    /// state.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    t_TYPE& emplace(std::initializer_list<t_INIT_LIST_TYPE>      il,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif
    /// Reset this object to the default constructed state (i.e., to a
    /// disengaged state).
    void reset() BSLS_KEYWORD_NOEXCEPT;

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee if the template parameter `t_TYPE` provides
    /// that guarantee and the result of the `hasValue` method for the two
    /// objects being swapped is the same.  The behavior is undefined unless
    /// this object was created with the same allocator as `other`.
    void swap(Optional_Base& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           bsl::is_nothrow_move_constructible<t_TYPE>::value &&
                           bsl::is_nothrow_swappable<t_TYPE>::value);

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object if `true == has_value()` and throw
    /// `bsl::bad_optional_access` otherwise.
    t_TYPE&  value() &;
    t_TYPE&& value() &&;
# else
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  Throws a `bsl::bad_optional_access` if the
    /// `Optional_Base` object is disengaged.
    t_TYPE& value();
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a copy of the underlying object of a (template parameter)
    /// `t_TYPE` if this object is non-null, and the specified `value`
    /// converted to `t_TYPE` otherwise.  Note that this method returns *by*
    /// *value*, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(t_ANY_TYPE&& value) &&;
#  ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    /// If this object is non-null, return a copy of the underlying object
    /// of a (template parameter) `t_TYPE` created using the provided
    /// allocator, and the specified `value` converted to `t_TYPE` using the
    /// specified `allocator` otherwise.  Note that this method returns *by*
    /// *value*, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(bsl::allocator_arg_t,
                    allocator_type,
                    t_ANY_TYPE&& value) &&;
#  endif  // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a non-`const` reference to this object.
    Optional_Base& operator=(const Optional_Base& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a non-`const` reference to this object.  The allocators of this
    /// object and `rhs` both remain unchanged.  The contents of `rhs` are
    /// either move-constructed into or move-assigned to this object.  `rhs` is
    /// left in a valid but unspecified state.
    Optional_Base& operator=(
                            BloombergLP::bslmf::MovableRef<Optional_Base> rhs);

    /// Return a pointer providing modifiable access to the underlying `t_TYPE`
    /// object.  The behavior is undefined if the `Optional_Base` object is
    /// disengaged.
    t_TYPE *operator->();

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.
    t_TYPE&  operator*() &;
    t_TYPE&& operator*() &&;
# else
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.
    t_TYPE& operator*();
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    // ACCESSORS

    /// Return allocator used for construction of `value_type`.
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `false` if this object is disengaged, and `true` otherwise.
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object if `true == has_value()` and throw
    /// `bsl::bad_optional_access` otherwise.
    const t_TYPE&  value() const &;
    const t_TYPE&& value() const &&;
# else
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  Throws a `bsl::bad_optional_access` if the
    /// `Optional_Base` object is disengaged.
    const t_TYPE& value() const;
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    /// Return a pointer providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.
    const t_TYPE *operator->() const;

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.
    const t_TYPE&  operator*() const &;
    const t_TYPE&& operator*() const &&;
# else
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.
    const t_TYPE& operator*() const;
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a copy of the underlying object of a (template parameter)
    /// `t_TYPE` if this object is non-null, and the specified `value`
    /// converted to `t_TYPE` otherwise.  Note that this method returns *by*
    /// *value*, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const&;
#  ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    /// If this object is non-null, return a copy of the underlying object
    /// of a (template parameter) `t_TYPE` created using the provided
    /// allocator, and the specified `value` converted to `t_TYPE` using the
    /// specified `allocator` otherwise.  Note that this method returns *by*
    /// *value*, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(
                   bsl::allocator_arg_t,
                   allocator_type,
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const&;
#  endif  // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# else
    /// Return a copy of the underlying object of a (template parameter)
    /// `t_TYPE` if this object is non-null, and the specified `value`
    /// converted to `t_TYPE` otherwise.  Note that this method returns **by
    /// value**, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const;
#  ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    /// If this object is non-null, return a copy of the underlying object of a
    /// (template parameter) `t_TYPE` created using the provided allocator, and
    /// the specified `value` converted to `t_TYPE` using the specified
    /// `allocator` otherwise.  Note that this method returns **by value**, so
    /// may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(bsl::allocator_arg_t,
                   allocator_type,
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const;
#  endif  // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    /// Return `false` if this object is disengaged, and `true` otherwise.
    explicit operator bool() const BSLS_KEYWORD_NOEXCEPT;
#else
    /// Simulation of explicit conversion to bool.  Inlined to work around xlC
    /// bug when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT else
};

// ============================================================================
//      Section: Definition of C++17 Allocator-Unaware 'Optional_Base'
// ============================================================================

                     // ==================================
                     // class Optional_Base<t_TYPE, false>
                     // ==================================

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
/// Specialization of `Optional_Base` for `value_type` that is not
/// allocator-aware when `std::optional` is available.
template <class t_TYPE>
class Optional_Base<t_TYPE, false> : public std::optional<t_TYPE> {

  private:
    // PRIVATE TYPES
    typedef std::optional<t_TYPE> StdOptionalBase;

  protected:
    // PROTECTED TYPES
    struct AllocType {
      private:
        // NOT IMPLEMENTED

        /// This constructor prevents `AllocType` from being an aggregate.
        explicit AllocType(int) = delete;
    };

    // PROTECTED CREATORS

    /// Create a disengaged `Optional_Base` object.
    Optional_Base();

    /// Create a disengaged `Optional_Base` object.
    Optional_Base(bsl::nullopt_t);                                  // IMPLICIT

    /// Create an `Optional_Base` object whose contained value is initialized
    /// by forwarding from the specified `value`.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                  t_ANY_TYPE&& value);

    /// Create a disengaged `Optional_Base` object if the specified `original`
    /// object is disengaged, and an `Optional_Base` object with the value of
    /// `original.value()` (of `t_ANY_TYPE`) converted to `t_TYPE` otherwise.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original);

    /// Create a disengaged `Optional_Base` object if the specified `original`
    /// object is disengaged, and an `Optional_Base` object with the value of
    /// `original.value()` (of `t_ANY_TYPE`) converted to `t_TYPE` otherwise.
    /// `original` is left in a valid but unspecified state.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  Optional_Base<t_ANY_TYPE>&& original);

    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                  const std::optional<t_ANY_TYPE>& original);

    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                  std::optional<t_ANY_TYPE>&& original);

    /// Create an `Optional_Base` object whose contained value is initialized
    /// from the specified `args`.
    template <class... t_ARGS>
    explicit Optional_Base(bsl::in_place_t, t_ARGS&&... args);

    /// Create an `Optional_Base` object whose contained value is initialized
    /// from the specified `il` and `args`.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    Optional_Base(bsl::in_place_t,
                  std::initializer_list<t_INIT_LIST_TYPE> il,
                  t_ARGS&&...                             args);

    /// These allocator-extended constructors cannot be called, and are
    /// provided only to prevent compilation errors when 'optional' is
    /// explicitly instantiated.
    Optional_Base(bsl::allocator_arg_t, AllocType);

    Optional_Base(bsl::allocator_arg_t, AllocType, bsl::nullopt_t);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                  t_ANY_TYPE&&);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>&);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  Optional_Base<t_ANY_TYPE>&&);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                  const std::optional<t_ANY_TYPE>&);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                  std::optional<t_ANY_TYPE>&&);

    template <class... t_ARGS>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  bsl::in_place_t,
                  t_ARGS&&...);

    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  bsl::in_place_t,
                  std::initializer_list<t_INIT_LIST_TYPE>,
                  t_ARGS&&...);

    // PROTECTED MANIPULATORS

    /// If `*this` holds an object, assign to that object the value of the
    /// specified `rhs`, converted to `t_TYPE`.  Otherwise, construct a held
    /// object from `rhs`, converted to `t_TYPE`.
    template <class t_ANY_TYPE>
    void assignOrEmplace(t_ANY_TYPE&& rhs);

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.  Note that this function is only intended to
    /// be called by `bdlb::NullableValue::value` during transition of its
    /// implementation to use 'bsl::Optional_Base.  Note that ref-qualified
    /// versions of `value()` are not provided because `NullableValue` does
    /// not require them.
    t_TYPE& dereferenceRaw();

    // PROTECTED ACCESSORS

    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.  Note that this function is only intended to
    /// be called by `bdlb::NullableValue::value` during transition of its
    /// implementation to use 'bsl::Optional_Base.  Note that ref-qualified
    /// versions of `value()` are not provided because `NullableValue` does
    /// not require them.
    const t_TYPE& dereferenceRaw() const;
#  endif  // BDE_OMIT_INTERNAL_DEPRECATED

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         Optional_Base,
                         BloombergLP::bslmf::IsBitwiseMoveable,
                         BloombergLP::bslmf::IsBitwiseMoveable<t_TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         Optional_Base,
                         BloombergLP::bslmf::IsBitwiseCopyable,
                         BloombergLP::bslmf::IsBitwiseCopyable<t_TYPE>::value);
};
# else  // BSLSTL_OPTIONAL_USES_STD_ALIASES

// ============================================================================
//     Section: Definition of Pre-C++17 Allocator-Unaware 'Optional_Base'
// ============================================================================

                     // ==================================
                     // class Optional_Base<t_TYPE, false>
                     // ==================================

/// Specialization of `Optional_Base` for `value_type` that is not
/// allocator-aware.
template <class t_TYPE>
class Optional_Base<t_TYPE, false> {

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

# ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    /// This type is needed only in C++03 mode, where 'explicit' conversion
    /// operators are not supported.  An 'optional' object is contextually
    /// converted to 'UnspecifiedBool' when used in 'if' statements, but is not
    /// implicitly convertible to 'bool'.
    typedef BloombergLP::bsls::UnspecifiedBool<Optional_Base>
                                                   UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType UnspecifiedBool;
# endif

    // DATA

    /// in-place `TYPE` object
    BloombergLP::bslstl::Optional_Data<t_TYPE> d_value;

  protected:
    // PROTECTED TYPES
    struct AllocType {
      private:
        // NOT IMPLEMENTED

        /// This constructor prevents `AllocType` from being an aggregate.
        explicit AllocType(int) BSLS_KEYWORD_DELETED;
    };

    // PROTECTED CREATORS

    /// Create a disengaged `Optional_Base` object.
    Optional_Base();

    /// Create a disengaged `Optional_Base` object.
    Optional_Base(bsl::nullopt_t);                                  // IMPLICIT

    /// Create an `Optional_Base` object having the value of the specified
    /// `original` object.
    Optional_Base(const Optional_Base& original);

    /// Create an `Optional_Base` object having the same value as the
    /// specified `original` object by moving the contents of `original` to
    /// the newly-created object.  `original` is left in a valid, but
    /// unspecified state.
    Optional_Base(BloombergLP::bslmf::MovableRef<Optional_Base> original)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                            bsl::is_nothrow_move_constructible<t_TYPE>::value);

    /// Create an `Optional_Base` object whose contained value is initialized
    /// by forwarding from the specified `value`.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value);

    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` converted to `t_TYPE` otherwise.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original);

    /// Create a disengaged `Optional_Base` object if the specified
    /// `original` object is disengaged, and an `Optional_Base` object with
    /// the value of `original.value()` converted to `t_TYPE` otherwise.
    /// `original` is left in a valid but unspecified state.
    template <class t_ANY_TYPE>
    Optional_Base(BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an `Optional_Base` object holding an object of type
    /// Create an `Optional_Base` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `args`.
    template <class... t_ARGS>
    explicit Optional_Base(bsl::in_place_t,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create an `Optional_Base` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `il` and
    /// specified `args`.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    explicit Optional_Base(bsl::in_place_t,
                           std::initializer_list<t_INIT_LIST_TYPE>      il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    /// These allocator-extended constructors cannot be called, and are provided
    /// only to prevent compilation errors when `optional` is explicitly
    /// instantiated.
    Optional_Base(bsl::allocator_arg_t, AllocType);

    Optional_Base(bsl::allocator_arg_t, AllocType, bsl::nullopt_t);

    Optional_Base(bsl::allocator_arg_t, AllocType, const Optional_Base&);

    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslmf::MovableRef<Optional_Base>);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE));

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>&);

    template <class t_ANY_TYPE>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>));

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... t_ARGS>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  bsl::in_place_t,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...);

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    Optional_Base(bsl::allocator_arg_t,
                  AllocType,
                  bsl::in_place_t,
                  std::initializer_list<t_INIT_LIST_TYPE>,
                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...);
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // PROTECTED MANIPULATORS

    /// If `*this` holds an object, assign to that object the value of
    /// `rhs`, converted to `t_TYPE`.  Otherwise, construct a held object
    /// from `rhs`, converted to `t_TYPE`.
    template <class t_ANY_TYPE>
    void assignOrEmplace(BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) rhs);

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.  Note that this function is only intended to
    /// be called by `bdlb::NullableValue::value` during transition of its
    /// implementation to use `bsl::Optional_Base`.  Note that ref-qualified
    /// versions of `value()` are not provided because `NullableValue` does
    /// not require them.
    t_TYPE& dereferenceRaw();

    // PROTECTED ACCESSORS

    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if the `Optional_Base`
    /// object is disengaged.  Note that this function is only intended to
    /// be called by `bdlb::NullableValue::value` during transition of its
    /// implementation to use `bsl::Optional_Base`.  Note that ref-qualified
    /// versions of `value()` are not provided because `NullableValue` does
    /// not require them.
    const t_TYPE& dereferenceRaw() const;
#  endif  // !BDE_OMIT_INTERNAL_DEPRECATED

  public:
    // TYPES

    /// `value_type` is an alias for the underlying `t_TYPE` upon which this
    /// template class is instantiated, and represents the type of the
    /// managed object.  The name is chosen so it is compatible with the
    /// `std::optional` implementation.
    typedef t_TYPE value_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         Optional_Base,
                         BloombergLP::bslmf::IsBitwiseMoveable,
                         BloombergLP::bslmf::IsBitwiseMoveable<t_TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         Optional_Base,
                         BloombergLP::bslmf::IsBitwiseCopyable,
                         BloombergLP::bslmf::IsBitwiseCopyable<t_TYPE>::value);

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Assign to this `Optional_Base` object the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `args` and
    /// return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  If this `Optional_Base` object already contains an
    /// object (`true == hasValue()`), that object is destroyed before the
    /// new object is created.  Note that if the constructor of `t_TYPE`
    /// throws an exception this object is left in a disengaged state.
    template <class... t_ARGS>
    t_TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Assign to this `Optional_Base` object the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `il` and
    /// specified `args` and return a reference providing modifiable access
    /// to the underlying `t_TYPE` object.  If this `Optional_Base` object
    /// already contains an object (`true == hasValue()`), that object is
    /// destroyed before the new object is created.  Note that if the
    /// constructor of `t_TYPE` throws an exception this object is left in a
    /// disengaged state.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    t_TYPE& emplace(std::initializer_list<t_INIT_LIST_TYPE>      il,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    /// Reset this object to the default constructed state (i.e., to be in a
    /// disengaged state).
    void reset() BSLS_KEYWORD_NOEXCEPT;

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee if the template parameter `t_TYPE`
    /// provides that guarantee and the result of the `hasValue` method for
    /// the two objects being swapped is the same.
    void swap(Optional_Base& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           bsl::is_nothrow_move_constructible<t_TYPE>::value &&
                           bsl::is_nothrow_swappable<t_TYPE>::value);

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object if `true == has_value()` and throw
    /// `bsl::bad_optional_access` otherwise.
    t_TYPE&  value() &;
    t_TYPE&& value() &&;
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  Throws a `bsl::bad_optional_access` if the
    /// `Optional_Base` object is disengaged.
    t_TYPE& value();
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a copy of the underlying object of a (template parameter)
    /// 't_TYPE' if this object is non-null, and the specified 'value'
    /// converted to 't_TYPE' otherwise.  Note that this method returns **by
    /// value**, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(t_ANY_TYPE&& value) &&;
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a non-`const` reference to this object.
    Optional_Base& operator=(const Optional_Base& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a non-`const` reference to this object.  The allocators of this
    /// object and `rhs` both remain unchanged.  The contents of `rhs` are
    /// either move-constructed into or move-assigned to this object.  `rhs` is
    /// left in a valid but unspecified state.
    Optional_Base& operator=(
                            BloombergLP::bslmf::MovableRef<Optional_Base> rhs);

    /// Return a pointer providing modifiable access to the underlying `t_TYPE`
    /// object.  The behavior is undefined if this object is disengaged.
    t_TYPE *operator->();

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if this object is
    /// disengaged.
    t_TYPE&  operator*() &;
    t_TYPE&& operator*() &&;
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if this object is
    /// disengaged.
    t_TYPE& operator*();
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    // ACCESSORS

    /// Return `false` if this object is disengaged, and `true` otherwise.
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object if `true == has_value()` and throw
    /// `bsl::bad_optional_access` otherwise.
    const t_TYPE&  value() const &;
    const t_TYPE&& value() const &&;
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing non-modifiable access to the underlying
    /// 't_TYPE' object.  Throws a 'bsl::bad_optional_access' if the
    /// 'Optional_Base' object is disengaged.
    const t_TYPE& value() const;
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a copy of the underlying object of a (template parameter)
    /// `t_TYPE` if this object is non-null, and the specified `value`
    /// converted to `t_TYPE` otherwise.  Note that this method returns **by
    /// value**, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const&;
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a copy of the underlying object of a (template parameter)
    /// `t_TYPE` if this object is non-null, and the specified `value`
    /// converted to `t_TYPE` otherwise.  Note that this method returns **by
    /// value**, so may be inefficient in some contexts.
    template <class t_ANY_TYPE>
    t_TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const;
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

    /// Return a pointer providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if this object is
    /// disengaged.
    const t_TYPE *operator->() const;

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if this object is
    /// disengaged.
    const t_TYPE&  operator*() const&;
    const t_TYPE&& operator*() const&&;
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    /// Return a reference providing non-modifiable access to the underlying
    /// `t_TYPE` object.  The behavior is undefined if this object is
    /// disengaged.
    const t_TYPE& operator*() const;
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    /// Return `false` if this object is disengaged, and `true` otherwise.
    explicit operator bool() const BSLS_KEYWORD_NOEXCEPT;
#else
    /// Simulation of explicit conversion to bool.  Inlined to work around xlC
    /// bug when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT else
};
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES else

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

// ============================================================================
//             Section: Definition of Class Template 'optional'
// ============================================================================

                               // ==============
                               // class optional
                               // ==============

template <class t_TYPE>
class optional : public BloombergLP::bslstl::Optional_Base<t_TYPE> {
  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslstl::Optional_Base<t_TYPE> BaseType;

    typedef typename BaseType::AllocType AllocType;

    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                        optional,
                        BloombergLP::bslma::UsesBslmaAllocator,
                        BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                        optional,
                        BloombergLP::bslmf::UsesAllocatorArgT,
                        BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         optional,
                         BloombergLP::bslmf::IsBitwiseMoveable,
                         BloombergLP::bslmf::IsBitwiseMoveable<t_TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         optional,
                         BloombergLP::bslmf::IsBitwiseCopyable,
                         BloombergLP::bslmf::IsBitwiseCopyable<t_TYPE>::value);

    // CREATORS

    /// Create a disengaged `optional` object.  Use the currently installed
    /// default allocator to supply memory.
    optional() BSLS_KEYWORD_NOEXCEPT;

    /// Create a disengaged `optional` object.  Use the currently installed
    /// default allocator to supply memory.
    optional(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;                 // IMPLICIT

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    /// If the `optional` base class of the specified `original` holds a
    /// value, create an `optional` object whose contained value is
    /// initialized by moving from that value; otherwise, create a
    /// disengaged `optional` object.  The allocator associated with the
    /// `optional` base class of `original` is propagated for use in the
    /// newly-created object.  `original` is left in a valid, but
    /// unspecified state.  Note that this constructor does not participate
    /// in overload resolution unless `optional` is an accessible base class
    /// of `t_DERIVED` or `t_DERIVED` is `optional` itself (in which case
    /// the instantiation provides the move constructor in C++03; a move
    /// constructor is implicitly declared in C++11).
    template <class t_DERIVED>
    optional(BloombergLP::bslmf::MovableRef<t_DERIVED> original,
             BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(t_DERIVED))
             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
               bsl::is_nothrow_move_constructible<t_TYPE>::value);  // IMPLICIT
#endif

    template <class t_ANY_TYPE BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG(t_TYPE)>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                               t_ANY_TYPE));
                                                                    // IMPLICIT

    /// Create an `optional` object whose contained value is initialized by
    /// forwarding from the specified `value`.  Use the currently installed
    /// default allocator to supply memory.  Note that this constructor
    /// participates in overload resolution only when `t_TYPE` is
    /// constructible from `t_ANY_TYPE` and is implicit only when
    /// `t_ANY_TYPE` is implicitly convertible to `t_TYPE`.
    template <class t_ANY_TYPE BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG(t_TYPE)>
    explicit optional(
            BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
            BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
            BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));

    template <class t_ANY_TYPE>
    optional(
         const optional<t_ANY_TYPE>& original,
         BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
             t_TYPE,
             const t_ANY_TYPE&),
         BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                           const t_ANY_TYPE&));
                                                                    // IMPLICIT

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` converted to `t_TYPE` otherwise.  Use the
    /// currently installed default allocator to supply memory.  Note that
    /// this constructor participates in overload resolution only when
    /// `t_TYPE` is constructible from an lvalue of `const t_ANY_TYPE`, and
    /// is implicit only when an lvalue of `const t_ANY_TYPE` is implicitly
    /// convertible to `t_TYPE`.
    template <class t_ANY_TYPE>
    explicit optional(
         const optional<t_ANY_TYPE>& original,
         BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
             t_TYPE,
             const t_ANY_TYPE&),
         BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE,
                                                       const t_ANY_TYPE&));

    template <class t_ANY_TYPE>
    optional(
        BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));
                                                                    // IMPLICIT

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` moved and converted to `t_TYPE` otherwise.  Use
    /// the allocator from `original` to supply memory if `t_ANY_TYPE` is a
    /// non-const version of `t_TYPE`; otherwise, use the currently
    /// installed default allocator.  `original` is left in a valid but
    /// unspecified state.  Note that this constructor participates in
    /// overload resolution only when `t_TYPE` is constructible from an
    /// xvalue of `t_ANY_TYPE`, and is implicit only when an xvalue of
    /// `t_ANY_TYPE` is implicitly convertible to `t_TYPE`.
    template <class t_ANY_TYPE>
    explicit optional(
        BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
    template <class t_ANY_TYPE>
    optional(
            const std::optional<t_ANY_TYPE>& original,
            BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                t_TYPE,
                const t_ANY_TYPE&),
            BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                              t_ANY_TYPE));
                                                                    // IMPLICIT

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` converted to `t_TYPE` otherwise.  Use the
    /// currently installed default allocator to supply memory.  Note that
    /// this constructor participates in overload resolution only when
    /// `t_TYPE` is constructible from an lvalue of `const t_ANY_TYPE`, and
    /// is implicit only when an lvalue of `const t_ANY_TYPE` is implicitly
    /// convertible to `t_TYPE`.
    template <class t_ANY_TYPE>
    explicit optional(
            const std::optional<t_ANY_TYPE>& original,
            BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
                t_TYPE,
                const t_ANY_TYPE&),
            BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));

    template <class t_ANY_TYPE>
    optional(
        std::optional<t_ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));
                                                                    // IMPLICIT

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` converted to `t_TYPE` otherwise.  Use the
    /// currently installed default allocator to supply memory.  Note that
    /// this constructor participates in overload resolution only when
    /// `t_TYPE` is constructible from an xvalue of `t_ANY_TYPE`, and is
    /// implicit only when an xvalue of `t_ANY_TYPE` is implicitly
    /// convertible to `t_TYPE`.
    template <class t_ANY_TYPE>
    explicit optional(
        std::optional<t_ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an `optional` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `args`.
    /// Use the currently installed default allocator to supply memory.
    template <class... t_ARGS>
    explicit optional(bsl::in_place_t,
                      BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create an `optional` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `il` and
    /// specified `args`.  Use the currently installed default allocator to
    /// supply memory.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    explicit optional(bsl::in_place_t,
                      std::initializer_list<t_INIT_LIST_TYPE>      il,
                      BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // These allocator-extended constructors can be called only when 't_TYPE'
    // is allocator-aware.

    /// Create a disengaged `optional` object.  Use the specified
    /// `allocator` to supply memory.
    optional(bsl::allocator_arg_t, AllocType allocator);

    /// Create a disengaged `optional` object.  Use the specified
    /// `allocator` to supply memory.
    optional(bsl::allocator_arg_t, AllocType allocator, bsl::nullopt_t);

    /// If the specified `original` contains a value, create an `optional`
    /// object whose contained value is initialized from `*original`;
    /// otherwise, create a disengaged `optional` object.  Use the specified
    /// `allocator` to supply memory.
    optional(bsl::allocator_arg_t,
             AllocType       allocator,
             const optional& original);

    /// If the `optional` base class of the specified `original` holds a
    /// value, create an `optional` object whose contained value is
    /// initialized by moving from that value; otherwise, create a
    /// disengaged `optional` object.  Use the specified `allocator` to
    /// supply memory.  `original` is left in a valid, but unspecified
    /// state.  Note that this constructor does not participate in overload
    /// resolution unless `optional` is an accessible base class of
    /// `t_DERIVED` or `t_DERIVED` is `optional` itself (in which case the
    /// instantiation provides the allocator-extended move constructor).
    template <class t_DERIVED>
    optional(bsl::allocator_arg_t,
             AllocType                          allocator,
             BSLMF_MOVABLEREF_DEDUCE(t_DERIVED) original,
             BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL(
                             typename bsl::remove_reference<t_DERIVED>::type));

    /// Create an `optional` object whose contained value is initialized by
    /// forwarding from the specified `value`.  Use the specified
    /// `allocator` to supply memory.  Note that this constructor
    /// participates in overload resolution only when `t_TYPE` is
    /// constructible from `t_ANY_TYPE`, and is implicit only when
    /// `t_ANY_TYPE` is implicitly convertible to `t_TYPE`.
    template <class t_ANY_TYPE BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG(t_TYPE)>
    optional(bsl::allocator_arg_t,
             AllocType                                     allocator,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
             BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
             BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                               t_ANY_TYPE));
    template <class t_ANY_TYPE BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG(t_TYPE)>
    explicit optional(
            bsl::allocator_arg_t,
            AllocType                                     allocator,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
            BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
            BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` converted to `t_TYPE` otherwise.  Use the
    /// specified `allocator` to supply memory.  Note that this constructor
    /// participates in overload resolution only when `t_TYPE` is
    /// constructible from an lvalue of `const t_ANY_TYPE`, and is implicit
    /// only when an lvalue of `const t_ANY_TYPE` is implicitly convertible
    /// to `t_TYPE`.
    template <class t_ANY_TYPE>
    optional(
         bsl::allocator_arg_t,
         AllocType                   allocator,
         const optional<t_ANY_TYPE>& original,
         BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
             t_TYPE,
             const t_ANY_TYPE&),
         BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                           const t_ANY_TYPE&));
    template <class t_ANY_TYPE>
    explicit optional(
         bsl::allocator_arg_t,
         AllocType                   allocator,
         const optional<t_ANY_TYPE>& original,
         BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(
             t_TYPE,
             const t_ANY_TYPE&),
         BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE,
                                                       const t_ANY_TYPE&));

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` moved and converted to `t_TYPE` otherwise.  Use
    /// the specified `allocator` to supply memory.  `original` is left in a
    /// valid but unspecified state.  Note that this constructor
    /// participates in overload resolution only when `t_TYPE` is
    /// constructible from an xvalue of `t_ANY_TYPE`, and is implicit only
    /// when an xvalue of `t_ANY_TYPE` is implicitly convertible to
    /// `t_TYPE`.
    template <class t_ANY_TYPE>
    optional(
        bsl::allocator_arg_t,
        AllocType                                     allocator,
        BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));
    template <class t_ANY_TYPE>
    explicit optional(
        bsl::allocator_arg_t,
        AllocType                                     allocator,
        BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` converted to `t_TYPE` otherwise.  Use the
    /// specified `allocator` to supply memory.  Note that this constructor
    /// participates in overload resolution only when `t_TYPE` is
    /// constructible from an lvalue of `const t_ANY_TYPE`, and is implicit
    /// only when an lvalue of `const t_ANY_TYPE` is implicitly convertible
    /// to `t_TYPE`.
    template <class t_ANY_TYPE>
    optional(
         bsl::allocator_arg_t,
         AllocType                        allocator,
         const std::optional<t_ANY_TYPE>& original,
         BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
             t_TYPE,
             const t_ANY_TYPE&),
         BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                           const t_ANY_TYPE&));
    template <class t_ANY_TYPE>
    explicit optional(
         bsl::allocator_arg_t,
         AllocType                        allocator,
         const std::optional<t_ANY_TYPE>& original,
         BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(
             t_TYPE,
             const t_ANY_TYPE&),
         BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE,
                                                       const t_ANY_TYPE));

    /// Create a disengaged `optional` object if the specified `original`
    /// object is disengaged, and an `optional` object with the value of
    /// `original.value()` moved and converted to `t_TYPE` otherwise.  Use
    /// the specified `allocator` to supply memory.  `original` is left in a
    /// valid but unspecified state.  Note that this constructor
    /// participates in overload resolution only when `t_TYPE` is
    /// constructible from an xvalue of `t_ANY_TYPE`, and is implicit only
    /// when an xvalue of `t_ANY_TYPE` is implicitly convertible to
    /// `t_TYPE`.
    template <class t_ANY_TYPE>
    optional(
        bsl::allocator_arg_t,
        AllocType                   allocator,
        std::optional<t_ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));
    template <class t_ANY_TYPE>
    explicit optional(
        bsl::allocator_arg_t,
        AllocType                   allocator,
        std::optional<t_ANY_TYPE>&& original,
        BSLSTL_OPTIONAL_DECLARE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                                t_ANY_TYPE),
        BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE));
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an `optional` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `args`.
    /// Use the specified `allocator` to supply memory.
    template <class... t_ARGS>
    explicit optional(bsl::allocator_arg_t,
                      AllocType                                    allocator,
                      bsl::in_place_t,
                      BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create an `optional` object having the value of the (template
    /// parameter) `t_TYPE` created in place using the specified `il` and
    /// `args`.  Use the specified `allocator` to supply memory.
    template <class t_INIT_LIST_TYPE, class... t_ARGS>
    explicit optional(bsl::allocator_arg_t,
                      AllocType                                    allocator,
                      bsl::in_place_t,
                      std::initializer_list<t_INIT_LIST_TYPE>      il,
                      BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    // MANIPULATORS

    // We implement 'operator=' in the most derived class to avoid having to
    // repeat constraints in each of the three implementations of
    // 'Optional_Base'.

    /// Reset this object to be disengaged and return a reference providing
    /// modifiable access to this object.
    optional& operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    /// If the `optional` base class of the specified `rhs` holds a value,
    /// move-construct or move-assign the contained value of `*this` from
    /// that value.  Otherwise, set `*this` to be disengaged.  Return a
    /// reference providing modifiable access to this object.  `rhs` is left
    /// in a valid but unspecified state.  This method does not participate
    /// in overload resolution unless `optional` is an accessible base class
    /// of `t_DERIVED` or `t_DERIVED` is `optional` itself (in which case
    /// the instantiation provides the move assignment operator in C++03; a
    /// move assignment operator is implicitly declared in C++11).
    template <class t_DERIVED>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(t_TYPE, t_DERIVED)&
    operator=(BloombergLP::bslmf::MovableRef<t_DERIVED> rhs);
#endif

    /// Disengage this object if the specified `rhs` object is disengaged,
    /// and assign to this object the value of `rhs.value()` (of
    /// `t_ANY_TYPE`) converted to `t_TYPE` otherwise.  Return a reference
    /// providing modifiable access to this object.  Note that this method
    /// does not participate in overload resolution unless `t_TYPE` and
    /// `t_ANY_TYPE` are compatible.
    template <class t_ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(t_TYPE, const t_ANY_TYPE&)&
    operator=(const optional<t_ANY_TYPE>& rhs);

    /// Disengage this object if the specified `rhs` object is disengaged,
    /// and move assign to this object the value of `rhs.value()` (of
    /// `t_ANY_TYPE`) converted to `t_TYPE` otherwise.  Return a reference
    /// providing modifiable access to this object.  Note that this method
    /// does not participate in overload resolution unless `t_TYPE` and
    /// `t_ANY_TYPE` are compatible.
    template <class t_ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(t_TYPE, t_ANY_TYPE)&
    operator=(BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    /// Assign to this object the value of the specified `rhs` object
    /// converted to `t_TYPE`, and return a reference providing modifiable
    /// access to this object.  Note that this method may invoke assignment
    /// from `rhs`, or construction from `rhs`, depending on whether this
    /// object is engaged.  This assignment operator does not participate in
    /// overload resolution if `t_ANY_TYPE` is possibly cv-qualified
    /// `bsl::optional<t_TYPE>`.  Also note that an assignment of the form
    /// `o = {};`, where `o` is of type `optional`, will always assign to
    /// `o` the empty state, and never call this overload (see
    /// implementation notes).
    template <class t_ANY_TYPE = t_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(t_TYPE, t_ANY_TYPE)&
    operator=(t_ANY_TYPE&& rhs);
#else
    // The existence of MovableRef in C++11 affects the above functions, and
    // they need to be defined in terms of rvalue references and perfect
    // forwarding.  For C++03, the MovableRef overloads are provided below.

    /// Assign to this object the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.
    optional& operator=(const t_TYPE& rhs);

    /// Assign to this object the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.  The contents
    /// of `rhs` are either move-constructed into or move-assigned to this
    /// object.  `rhs` is left in a valid but unspecified state.
    optional& operator=(BloombergLP::bslmf::MovableRef<t_TYPE> rhs);

    /// Assign to this object the value of the specified `rhs` object (of
    /// `ANY_TYPE`) converted to `TYPE`, and return a reference providing
    /// modifiable access to this object.  Note that this method may invoke
    /// assignment from `rhs`, or construction from `rhs`, depending on
    /// whether this `optional` object is engaged.
    template <class t_ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(t_TYPE, t_ANY_TYPE)&
    operator=(const t_ANY_TYPE& rhs);

    template <class t_ANY_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(t_TYPE, t_ANY_TYPE)&
    operator=(BloombergLP::bslmf::MovableRef<t_ANY_TYPE> rhs);
#endif  // RVALUES else

#ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
    /// Disengage this object if the specified `rhs` object is disengaged,
    /// and assign to this object the value of `rhs.value()` (of
    /// `t_ANY_TYPE`) converted to `t_TYPE` otherwise.  Return a reference
    /// providing modifiable access to this object.  Note that this method
    /// does not participate in overload resolution unless `t_TYPE` and
    /// `t_ANY_TYPE` are compatible.
    template <class t_ANY_TYPE = t_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(t_TYPE, const t_ANY_TYPE&)&
    operator=(const std::optional<t_ANY_TYPE>& rhs);

    /// Disengage this object if the specified `rhs` object is disengaged,
    /// and move assign to this object the value of `rhs.value()` (of
    /// `t_ANY_TYPE`) converted to `t_TYPE` otherwise.  Return a reference
    /// providing modifiable access to this object.  Note that this method
    /// does not participate in overload resolution unless `t_TYPE` and
    /// `t_ANY_TYPE` are compatible.
    template <class t_ANY_TYPE = t_TYPE>
    BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(t_TYPE, t_ANY_TYPE)&
    operator=(std::optional<t_ANY_TYPE>&& rhs);
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES
};

}  // close namespace bsl

// ============================================================================
//         Section: Type Traits on Allocator-Unaware 'Optional_Base'
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class t_TYPE>
struct UsesBslmaAllocator<bslstl::Optional_Base<t_TYPE, false> >
: bsl::false_type {
};

template <class t_TYPE>
struct UsesBslmaAllocator<bsl::optional<t_TYPE> >
: UsesBslmaAllocator<t_TYPE> {
};

}  // close namespace bslma
}  // close enterprise namespace

namespace bsl {

// ============================================================================
//                          Section: CTAD Constructors
// ============================================================================

# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

/// Deduce the specified type `t_TYPE` from the corresponding type supplied
/// to the constructor of `optional`.
template <class t_TYPE>
optional(t_TYPE) -> optional<t_TYPE>;

/// Deduce the specified type `t_TYPE` from the corresponding type supplied
/// to the constructor of `optional`.   This guide does not participate in
/// deduction unless the deduced type `t_TYPE` supports the bslma allocator
/// model, and the specified `t_ALLOC` can be implicitly converted to
/// `bsl::allocator<char>`.
template <class t_TYPE,
          class t_ALLOC,
          class = typename bsl::enable_if_t<
                    BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value>,
          class = typename bsl::enable_if_t<
                bsl::is_convertible_v<t_ALLOC, bsl::allocator<char>>>
          >
optional(bsl::allocator_arg_t, t_ALLOC, t_TYPE)
-> optional<t_TYPE>;

/// Deduce the specified type `t_TYPE` from the corresponding template
/// parameter type supplied to the constructor of `optional`.   This guide
/// does not participate in deduction unless the deduced type `t_TYPE`
/// supports the bslma allocator model, and the specified `t_ALLOC` can be
/// implicitly converted to `bsl::allocator<char>`.
template <class t_TYPE,
          class t_ALLOC,
          class = typename bsl::enable_if_t<
                    BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value>,
          class = typename bsl::enable_if_t<
                bsl::is_convertible_v<t_ALLOC, bsl::allocator<char>>>
          >
optional(bsl::allocator_arg_t, t_ALLOC, optional<t_TYPE>)
-> optional<t_TYPE>;
# endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                      Section: Free Function Declarations
// ============================================================================

// FREE FUNCTIONS

/// Efficiently exchange the values of the specified `lhs` and `rhs`
/// objects.  This method provides the no-throw exception-safety guarantee
/// if the template parameter `t_TYPE` provides that guarantee, `lhs` and
/// `rhs` have equal allocators, and `lhs.hasValue() == rhs.hasValue()`.
template <class t_TYPE>
typename bsl::enable_if<BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(bsl::optional<t_TYPE>& lhs, bsl::optional<t_TYPE>& rhs);

/// Efficiently exchange the values of the specified `lhs` and `rhs`
/// objects.  This method provides the no-throw exception-safety guarantee
/// if the template parameter `t_TYPE` provides that guarantee and the
/// result of the `hasValue` method for `lhs` and `rhs` is the same.
template <class t_TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(bsl::optional<t_TYPE>& lhs, bsl::optional<t_TYPE>& rhs);

// HASH SPECIALIZATIONS

/// Pass the specified `input` to the specified `hashAlg`, where `hashAlg`
/// is a hashing algorithm.
template <class t_HASHALG, class t_TYPE>
void hashAppend(t_HASHALG& hashAlg, const optional<t_TYPE>& input);

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` `optional` objects have the
/// same value, and `false` otherwise.  Two `optional` objects have the same
/// value if both are disengaged, or if both are engaged and the values of
/// their underlying objects compare equal.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs == *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    });

/// Return `true` if the specified `lhs` and `rhs` `optional` objects do not
/// have the same value, and `false` otherwise.  Two `optional` objects do
/// not have the same value if one is disengaged and the other is engaged,
/// or if both are engaged and the values of their underlying objects do not
/// compare equal.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs != *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    });

/// Return `true` if the specified `lhs` `optional` object is ordered before
/// the specified `rhs` `optional` object, and `false` otherwise.  `lhs` is
/// ordered before `rhs` if `lhs` is disengaged and `rhs` is engaged or if
/// both are engaged and `lhs.value()` is ordered before `rhs.value()`.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const bsl::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs < *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    });

/// Return `true` if the specified `lhs` `optional` object is ordered after
/// the specified `rhs` `optional` object, and `false` otherwise.  `lhs` is
/// ordered after `rhs` if `lhs` is engaged and `rhs` is disengaged or if
/// both are engaged and `lhs.value()` is ordered after `rhs.value()`.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const bsl::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs > *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    });

/// Return `true` if the specified `lhs` `optional` object is ordered before
/// the specified `rhs` `optional` object or if `lhs` and `rhs` have the
/// same value, and `false` otherwise.  (See `operator<` and `operator==`.)
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs <= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    });

/// Return `true` if the specified `lhs` `optional` object is ordered after
/// the specified `rhs` `optional` object or if `lhs` and `rhs` have the
/// same value, and `false` otherwise.  (See `operator>` and `operator==`.)
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs >= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    });

// comparison with 'nullopt_t'

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator==(
                            const bsl::optional<t_TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
#if !(defined(BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON) &&          \
      defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS))
/// Return `true` if the specified `value` is disengaged, and `false`
/// otherwise.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator==(
                            const bsl::nullopt_t&,
                            const bsl::optional<t_TYPE>& value)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `value` is engaged, and `false` otherwise.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                            const bsl::optional<t_TYPE>& value,
                            const bsl::nullopt_t&)       BSLS_KEYWORD_NOEXCEPT;
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                            const bsl::nullopt_t&,
                            const bsl::optional<t_TYPE>& value)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  Note that `bsl::nullopt_t` never orders after a
/// `bsl::optional`.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<(
                             const bsl::optional<t_TYPE>&,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `value` is engaged, and `false` otherwise.
/// Note that `bsl::nullopt_t` is ordered before any `bsl::optional` that is
/// engaged.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<(
                     const bsl::nullopt_t&,
                     const bsl::optional<t_TYPE>& value) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `value` is engaged, and `false` otherwise.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>(
                            const bsl::optional<t_TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  Note that `bsl::nullopt_t` never orders after a
/// `bsl::optional`.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>(
                           const bsl::nullopt_t&,
                           const bsl::optional<t_TYPE>&) BSLS_KEYWORD_NOEXCEPT;

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                            const bsl::optional<t_TYPE>& value,
                            const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is disengaged, and 'false'
    // otherwise.

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                           const bsl::nullopt_t&,
                           const bsl::optional<t_TYPE>&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.  Note that 'bsl::nullopt_t' is ordered before any
    // 'bsl::optional' that is engaged.

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                             const bsl::optional<t_TYPE>&,
                             const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.  Note that 'bsl::nullopt_t' is ordered before any
    // 'bsl::optional' that is engaged.

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                     const bsl::nullopt_t&,
                     const bsl::optional<t_TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is disengaged, and 'false'
    // otherwise.
#endif  // !(defined(BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON) &&
        //   defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS))

// comparison with 'value_type'

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  An `optional` object and a value of some
/// type have the same value if the optional object is engaged and its
/// underlying value compares equal to the other value.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const bsl::optional<t_LHS_TYPE>& lhs,
                const t_RHS_TYPE&                rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs == rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const t_LHS_TYPE&                lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs == *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` and `rhs` objects do not have the same
/// value, and `false` otherwise.  An `optional` object and a value of some
/// type do not have the same value if either the optional object is
/// disengaged, or its underlying value does not compare equal to the other
/// value.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const bsl::optional<t_LHS_TYPE>& lhs,
                const t_RHS_TYPE&                rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs != rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const t_LHS_TYPE&                lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs != *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` `optional` object is ordered before
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `lhs` is disengaged or `lhs.value()` is ordered before `rhs`.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs < rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` is ordered before the specified `rhs`
/// `optional` object, and `false` otherwise.  `lhs` is ordered before `rhs` if
/// `rhs` is engaged and `lhs` is ordered before `rhs.value()`.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs < *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` `optional` object is ordered after the
/// specified `rhs`, and `false` otherwise.  `lhs` is ordered after `rhs` if
/// `lhs` is engaged and `lhs.value()` is ordered after `rhs`.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs > rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` is ordered after the specified `rhs`
/// `optional` object, and `false` otherwise.  `lhs` is ordered after `rhs` if
/// `rhs` is disengaged or `lhs` is ordered after `rhs.value()`.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs > *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` `optional` object is ordered before
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator<` and `operator==`.)
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs <= rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` is ordered before the specified `rhs`
/// `optional` object or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator<` and `operator==`.)
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs <= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

/// Return `true` if the specified `lhs` `optional` object is ordered after the
/// specified `rhs` or if `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator>` and `operator==`.)
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs >= rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

// Return `true` if the specified `lhs` is ordered after the specified `rhs`
// `optional` object or if `lhs` and `rhs` have the same value, or `false`
// otherwise.  (See `operator>` and `operator==`.)
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs >= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        });

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
/// Perform a three-way comparison of the specified `lhs` and the specified
/// `rhs` objects by using the comparison operators of `t_LHS` and `t_RHS`;
/// return the result of that comparison.
template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
constexpr compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                              const bsl::optional<t_LHS>& lhs,
                                              const bsl::optional<t_RHS>& rhs);

/// Perform a three-way comparison of the specified `lhs` and the specified
/// `rhs` objects by using the comparison operators of `t_LHS` and `t_RHS`;
/// return the result of that comparison.
template <class t_LHS, class t_RHS>
requires (!BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS>) &&
         three_way_comparable_with<t_LHS, t_RHS>
constexpr compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                              const bsl::optional<t_LHS>& lhs,
                                              const t_RHS&                rhs);

/// Perform a three-way comparison of the specified `value` and `nullopt`;
/// return the result of that comparison.
template <class t_TYPE>
constexpr strong_ordering operator<=>(
                                  const bsl::optional<t_TYPE>& value,
                                  bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;

/// Perform a three-way comparison of the specified `lhs` and the specified
/// `rhs` objects by using the comparison operators of `t_LHS` and `t_RHS`;
/// return the result of that comparison.
template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
constexpr compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                              const bsl::optional<t_LHS>& lhs,
                                              const std::optional<t_RHS>& rhs);
#endif

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
/// Efficiently exchange the values of the specified `lhs` and `rhs`
/// objects.  This method provides the no-throw exception-safety guarantee
/// if the template parameter `t_TYPE` provides that guarantee and the
/// result of the `hasValue` method for `lhs` and `rhs` is the same.
template <class t_TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(bsl::optional<t_TYPE>& lhs, std::optional<t_TYPE>& rhs);
template <class t_TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(std::optional<t_TYPE>& lhs, bsl::optional<t_TYPE>& rhs);

// comparison with 'std::optional'

/// Return `true` if the specified `lhs` and `rhs` optional objects have the
/// same value, and `false` otherwise.  Two optional objects have the same
/// value if both are disengaged, or if both are engaged and the values of
/// their underlying objects compare equal.  Note that this function will
/// fail to compile if `t_LHS_TYPE` and `t_RHS_TYPE` are not compatible.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs);
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` optional objects do not
/// have the same value, and `false` otherwise.  Two optional objects do not
/// have the same value if one is disengaged and the other is engaged, or if
/// both are engaged and the values of their underlying objects do not
/// compare equal.  Note that this function will fail to compile if
/// `t_LHS_TYPE` and `t_RHS_TYPE` are not compatible.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs);
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` optional object is ordered before
/// the specified `rhs` optional object, and `false` otherwise.  `lhs` is
/// ordered before `rhs` if `lhs` is disengaged and `rhs` is engaged or if
/// both are engaged and `lhs.value()` is ordered before `rhs.value()`.
/// Note that this function will fail to compile if `t_LHS_TYPE` and
/// `t_RHS_TYPE` are not compatible.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const bsl::optional<t_LHS_TYPE>& lhs,
               const std::optional<t_RHS_TYPE>& rhs);
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const std::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` optional object is ordered after
/// the specified `rhs` optional object, and `false` otherwise.  `lhs` is
/// ordered after `rhs` if `lhs` is engaged and `rhs` is disengaged or if
/// both are engaged and `lhs.value()` is ordered after `rhs.value()`.  Note
/// that this function will fail to compile if `t_LHS_TYPE` and `t_RHS_TYPE`
/// are not compatible.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const bsl::optional<t_LHS_TYPE>& lhs,
               const std::optional<t_RHS_TYPE>& rhs);
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const std::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` optional object or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator<` and `operator==`.)  Note that this
/// function will fail to compile if `t_LHS_TYPE` and `t_RHS_TYPE` are not
/// compatible.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs);
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` optional object is ordered after
/// the specified `rhs` optional object or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator>` and `operator==`.)  Note
/// that this function will fail to compile if `t_LHS_TYPE` and `t_RHS_TYPE`
/// are not compatible.
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs);
template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs);
#endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

/// Return an `optional` object containing a `t_TYPE` object created by
/// invoking a `bsl::optional` allocator-extended `in_place_t` constructor
/// with the specified `alloc` as the allocator argument, and specified
/// `rhs` as the constructor argument.  Note that this function will fail to
/// compile if `t_TYPE` doesn't use allocators.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<t_TYPE>::type>
make_optional(bsl::allocator_arg_t,
              const typename bsl::optional<
                  typename bsl::decay<t_TYPE>::type>::allocator_type& alloc,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)               rhs);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Return an `optional` object containing a `t_TYPE` object created by
/// invoking a `bsl::optional` allocator-extended `in_place_t` constructor
/// with the specified `alloc` as the allocator argument, and specified
/// `args` as constructor arguments.  Note that this function will fail to
/// compile if `t_TYPE` doesn't use allocators.
template <class t_TYPE, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional(
                   bsl::allocator_arg_t,
                   typename bsl::optional<t_TYPE>::allocator_type const& alloc,
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...          args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
// MSVC2013 has a bug that causes deduction issues in free template functions
// that have an 'std::initializer_list' argument where the
// 'std::initializer_list' element type is deduced.

/// Return an `optional` object containing a `t_TYPE` object created by
/// invoking a `bsl::optional` allocator-extended `in_place_t` constructor
/// with the specified `alloc` as the allocator argument, and specified `il`
/// and `args` as the constructor arguments.  Note that this function will
/// fail to compile if `t_TYPE` doesn't use allocators.
template <class t_TYPE, class t_INIT_LIST_TYPE, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional(
                   bsl::allocator_arg_t,
                   typename bsl::optional<t_TYPE>::allocator_type const& alloc,
                   std::initializer_list<t_INIT_LIST_TYPE>               il,
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...          args);
#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif

/// Return an `optional` object containing a `t_TYPE` object created by
/// invoking a `bsl::optional` constructor with the specified `rhs` as the
/// constructor argument.  If `t_TYPE` uses an allocator, the default
/// allocator will be used for the `optional` object.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<t_TYPE>::type>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE) rhs);

/// Return an `optional` object containing a value-initialized `t_TYPE`
/// object.  If `t_TYPE` uses an allocator, the default allocator will be
/// used for the `optional` object.
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional();

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Return an `optional` object containing a `TYPE` object created by
/// invoking a `bsl::optional` `in_place_t` constructor with the specified
/// `arg` and `args` as the constructor arguments.  If `t_TYPE` uses an
/// allocator, the default allocator will be used for the `optional` object.
template <class t_TYPE, class t_ARG, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG(t_ARG)
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)     arg,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
// MSVC2013 has a bug that causes deduction issues in free template functions
// that have an 'std::initializer_list' argument where the
// 'std::initializer_list' element type is deduced.

/// Return an `optional` object containing a `t_TYPE` object created by
/// invoking a `bsl::optional` `in_place_t` constructor with the specified
/// `il` and `args` as the constructor arguments.  If `t_TYPE` uses an
/// allocator, the default allocator will be used for the `optional` object.
template <class t_TYPE, class t_INIT_LIST_TYPE, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional(
                            std::initializer_list<t_INIT_LIST_TYPE>      il,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args);

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
template <class t_TYPE>
Optional_DataImp<t_TYPE>::Optional_DataImp() BSLS_KEYWORD_NOEXCEPT
: d_hasValue(false)
{
}

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE>
template <class... t_ARGS>
inline
t_TYPE& Optional_DataImp<t_TYPE>::emplace(
                       bslma::Allocator                             *allocator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
    d_hasValue = true;
    return d_buffer.object();
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
t_TYPE& Optional_DataImp<t_TYPE>::emplace(
                       bslma::Allocator                             *allocator,
                       std::initializer_list<t_INIT_LIST_TYPE>       il,
                       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
    d_hasValue = true;
    return d_buffer.object();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class t_TYPE>
void Optional_DataImp<t_TYPE>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue) {
      d_hasValue = false;
      bslma::DestructionUtil::destroy(d_buffer.address());
    }
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
t_TYPE& Optional_DataImp<t_TYPE>::value() &
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'Optional_Base' method invoking 'value()'

    return d_buffer.object();
}

template <class t_TYPE>
inline
t_TYPE&& Optional_DataImp<t_TYPE>::value() &&
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'Optional_Base' method invoking 'value()'

    return std::move(d_buffer.object());
}
# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
t_TYPE& Optional_DataImp<t_TYPE>::value()
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'Optional_Base' method invoking 'value()'

    return d_buffer.object();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

// ACCESSORS
template <class t_TYPE>
inline
bool Optional_DataImp<t_TYPE>::hasValue() const BSLS_KEYWORD_NOEXCEPT
{
    return d_hasValue;
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
const t_TYPE& Optional_DataImp<t_TYPE>::value() const&
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'Optional_Base' method invoking 'value()'

    return d_buffer.object();
}

template <class t_TYPE>
inline
const t_TYPE&& Optional_DataImp<t_TYPE>::value() const&&
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'Optional_Base' method invoking 'value()'

    return std::move(d_buffer.object());
}
# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
const t_TYPE& Optional_DataImp<t_TYPE>::value() const
{
    // We do not assert on an empty object in this function as the assert level
    // is determined by the 'Optional_Base' method invoking 'value()'

    return d_buffer.object();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

                            // ===================
                            // class Optional_Data
                            // ===================

// CREATORS
template <class t_TYPE, bool t_IS_TRIVIALLY_DESTRUCTIBLE>
Optional_Data<t_TYPE, t_IS_TRIVIALLY_DESTRUCTIBLE>::~Optional_Data()
{
    this->reset();
}

// ============================================================================
//        Section: Allocator-Aware 'Optional_Base' Method Definitions
// ============================================================================

                            // ===================
                            // class Optional_Base
                            // ===================

// PROTECTED CREATORS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base()
{
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(bsl::nullopt_t)
{
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                                                 const Optional_Base& original)
{
    if (original.has_value()) {
        emplace(*original);
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                        BloombergLP::bslmf::MovableRef<Optional_Base> original)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<t_TYPE>::value)
: d_allocator(MoveUtil::access(original).get_allocator())
{
    Optional_Base& lvalue = original;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                         BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                         BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value));
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original,
  BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_DOES_NOT_PROPAGATE_ALLOCATOR(t_TYPE,
                                                                   t_ANY_TYPE))
{
    Optional_Base<t_ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
          BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
          BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original,
          BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCT_PROPAGATES_ALLOCATOR(t_TYPE,
                                                                   t_ANY_TYPE))
: d_allocator(MoveUtil::access(original).get_allocator())
{
    Optional_Base<t_ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                    BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                    const std::optional<t_ANY_TYPE>& original)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                    BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                    std::optional<t_ANY_TYPE>&& original)
{
    if (original.has_value()) {
        emplace(std::move(original.value()));
    }
}
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class... t_ARGS>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                             bsl::in_place_t,
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                             bsl::in_place_t,
                             std::initializer_list<t_INIT_LIST_TYPE>      il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    emplace(il, BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                                                      bsl::allocator_arg_t,
                                                      allocator_type allocator)
: d_allocator(allocator)
{
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                                                      bsl::allocator_arg_t,
                                                      allocator_type allocator,
                                                      bsl::nullopt_t)
: d_allocator(allocator)
{
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                                                bsl::allocator_arg_t,
                                                allocator_type       allocator,
                                                const Optional_Base& original)
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(*original);
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                       bsl::allocator_arg_t,
                       allocator_type                                allocator,
                       BloombergLP::bslmf::MovableRef<Optional_Base> original)
: d_allocator(allocator)
{
    Optional_Base& lvalue = original;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                       bsl::allocator_arg_t,
                       allocator_type                                allocator,
                       BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                       BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value)
: d_allocator(allocator)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value));
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                  bsl::allocator_arg_t,
                  allocator_type                   allocator,
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original)
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                  bsl::allocator_arg_t,
                  allocator_type                                     allocator,
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original)
: d_allocator(allocator)
{
    Optional_Base<t_ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                    bsl::allocator_arg_t,
                    allocator_type                   allocator,
                    BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                    const std::optional<t_ANY_TYPE>& original)
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                    bsl::allocator_arg_t,
                    allocator_type              allocator,
                    BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                    std::optional<t_ANY_TYPE>&& original)
: d_allocator(allocator)
{
    if (original.has_value()) {
        emplace(std::move(original.value()));
    }
}
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class... t_ARGS>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                            bsl::allocator_arg_t,
                            allocator_type                               alloc,
                            bsl::in_place_t,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
: d_allocator(alloc)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::Optional_Base(
                            bsl::allocator_arg_t,
                            allocator_type                               alloc,
                            bsl::in_place_t,
                            std::initializer_list<t_INIT_LIST_TYPE>      il,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
: d_allocator(alloc)
{
    emplace(il, BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

// PROTECTED MANIPULATORS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
void Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::assignOrEmplace(
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) rhs)
{
    if (has_value()) {
        d_value.value() = BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, rhs);
    } else {
        emplace(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, rhs));
    }
}

# ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::dereferenceRaw()
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();
}

// PROTECTED ACCESORS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
const t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::dereferenceRaw() const
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();
}
# endif  // BDE_OMIT_INTERNAL_DEPRECATED

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class... t_ARGS>
inline
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::emplace(
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    return d_value.emplace(d_allocator.mechanism(),
                           BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::emplace(
                             std::initializer_list<t_INIT_LIST_TYPE>      il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    return d_value.emplace(d_allocator.mechanism(),
                           il,
                           BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
void Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::reset() BSLS_KEYWORD_NOEXCEPT
{
    d_value.reset();
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
void Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::swap(Optional_Base& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           bsl::is_nothrow_move_constructible<t_TYPE>::value &&
                           bsl::is_nothrow_swappable<t_TYPE>::value)
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
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value() &
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE&& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value() &&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value()
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
t_TYPE Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value_or(
                                                         t_ANY_TYPE&& value) &&
{
    if (has_value()) {
        return t_TYPE(std::move(d_value.value()));                    // RETURN
    }
    else {
        return t_TYPE(std::forward<t_ANY_TYPE>(value));               // RETURN
    }
}

#  ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
t_TYPE Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value_or(
                                                      bsl::allocator_arg_t,
                                                      allocator_type allocator,
                                                      t_ANY_TYPE&&   value) &&
{
    if (has_value()) {
        return BloombergLP::bslma::ConstructionUtil::make<t_TYPE>(
                                        allocator,
                                        std::move(d_value.value()));  // RETURN
    }
    else {
        return BloombergLP::bslma::ConstructionUtil::make<t_TYPE>(
                                   allocator,
                                   std::forward<t_ANY_TYPE>(value));  // RETURN
    }
}
#  endif  // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>&
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator=(const Optional_Base& rhs)
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

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>&
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator=(
                             BloombergLP::bslmf::MovableRef<Optional_Base> rhs)
{
    Optional_Base& lvalue = rhs;

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

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE *Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator->()
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator*() &
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE&& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator*() &&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator*()
{
   BSLS_ASSERT(has_value());

   return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

// ACCESSORS

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
typename Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::allocator_type
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_allocator;
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
bool
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::has_value() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_value.hasValue();
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value() const&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE&& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value() const&&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value() const
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
t_TYPE Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value_or(
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const&
{
    if (has_value()) {
        return t_TYPE(d_value.value());                               // RETURN
    }
    else {
        return t_TYPE(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE,
                                                    value));          // RETURN
    }
}

#  ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
t_TYPE Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value_or(
                    bsl::allocator_arg_t,
                    allocator_type                                allocator,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const&
{
    if (has_value()) {
        return BloombergLP::bslma::ConstructionUtil::make<t_TYPE>(
            allocator, d_value.value());                              // RETURN
    }
    else {
        return BloombergLP::bslma::ConstructionUtil::make<t_TYPE>(
            allocator,
            BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value));        // RETURN
    }
}
#  endif  // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
t_TYPE Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value_or(
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const
{
    if (has_value()) {
        return t_TYPE(d_value.value());                               // RETURN
    }
    else {
        return t_TYPE(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE,
                                                    value));          // RETURN
    }
}

#  ifdef BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
template <class t_ANY_TYPE>
inline
t_TYPE Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::value_or(
                     bsl::allocator_arg_t,
                     allocator_type                                allocator,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value) const
{
    if (has_value()) {
        return BloombergLP::bslma::ConstructionUtil::make<t_TYPE>(
            allocator, d_value.value());                              // RETURN
    }
    else {
        return BloombergLP::bslma::ConstructionUtil::make<t_TYPE>(
            allocator,
            BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, value));          // RETURN
    }
}
#  endif  // BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE *Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator->() const
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator*() const&
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE&& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator*() const&&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}

# else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
inline
const t_TYPE& Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator*() const
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class t_TYPE, bool t_USES_BSLMA_ALLOC>
Optional_Base<t_TYPE, t_USES_BSLMA_ALLOC>::operator bool() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT

// ============================================================================
//    Section: C++17 Allocator-Unaware 'Optional_Base' Method Definitions
// ============================================================================

                     // ==================================
                     // class Optional_Base<t_TYPE, false>
                     // ==================================

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
// PROTECTED CREATORS
template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base()
{
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::nullopt_t)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                         BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                         t_ANY_TYPE&& value)
: StdOptionalBase(std::forward<t_ANY_TYPE>(value))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original)
{
    if (original.has_value()) {
        this->emplace(original.value());
    }
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  Optional_Base<t_ANY_TYPE>&& original)
{
    if (original.has_value()) {
        this->emplace(std::move(original.value()));
    }
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                    BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                    const std::optional<t_ANY_TYPE>& original)
: StdOptionalBase(original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                    BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                    std::optional<t_ANY_TYPE>&& original)
: StdOptionalBase(std::move(original))
{
}

template <class t_TYPE>
template <class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::in_place_t, t_ARGS&&... args)
: StdOptionalBase(bsl::in_place, std::forward<t_ARGS>(args)...)
{
}

template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                                  bsl::in_place_t,
                                  std::initializer_list<t_INIT_LIST_TYPE> il,
                                  t_ARGS&&...                             args)
: StdOptionalBase(bsl::in_place, il, std::forward<t_ARGS>(args)...)
{
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::allocator_arg_t, AllocType)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::allocator_arg_t,
                                            AllocType,
                                            bsl::nullopt_t)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                         bsl::allocator_arg_t,
                         AllocType,
                         BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                         t_ANY_TYPE&&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  Optional_Base<t_ANY_TYPE>&&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                    bsl::allocator_arg_t,
                    AllocType,
                    BloombergLP::bslstl::Optional_CopyConstructFromStdOptional,
                    const std::optional<t_ANY_TYPE>&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                    bsl::allocator_arg_t,
                    AllocType,
                    BloombergLP::bslstl::Optional_MoveConstructFromStdOptional,
                    std::optional<t_ANY_TYPE>&&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::allocator_arg_t,
                                            AllocType,
                                            bsl::in_place_t,
                                            t_ARGS&&...)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                                  bsl::allocator_arg_t,
                                  AllocType,
                                  bsl::in_place_t,
                                  std::initializer_list<t_INIT_LIST_TYPE>,
                                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

// PROTECTED MANIPULATORS
template <class t_TYPE>
template <class t_ANY_TYPE>
void Optional_Base<t_TYPE, false>::assignOrEmplace(t_ANY_TYPE&& rhs)
{
    StdOptionalBase::operator=(std::forward<t_ANY_TYPE>(rhs));
}

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class t_TYPE>
t_TYPE& Optional_Base<t_TYPE, false>::dereferenceRaw()
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return this->operator*();

}

// PROTECTED ACCESORS
template <class t_TYPE>
const t_TYPE& Optional_Base<t_TYPE, false>::dereferenceRaw() const
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return this->operator*();

}
#  endif  // BDE_OMIT_INTERNAL_DEPRECATED
# else  // BSLSTL_OPTIONAL_USES_STD_ALIASES

// ============================================================================
//  Section: Pre-C++17 Allocator-Unaware 'Optional_Base' Method Definitions
// ============================================================================

// PROTECTED CREATORS
template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base()
{
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::nullopt_t)
{
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(const Optional_Base& original)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                        BloombergLP::bslmf::MovableRef<Optional_Base> original)
         BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<t_TYPE>::value)
{
    Optional_Base& lvalue = original;

    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                         BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                         BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value));
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>& original)
{
    if (original.has_value()) {
        emplace(original.value());
    }
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>) original)
{
    Optional_Base<t_ANY_TYPE>& lvalue = original;
    if (lvalue.has_value()) {
        emplace(MoveUtil::move(*lvalue));
    }
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE>
template <class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                             bsl::in_place_t,
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    emplace(BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                             bsl::in_place_t,
                             std::initializer_list<t_INIT_LIST_TYPE>      il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    emplace(il, BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::allocator_arg_t, AllocType)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::allocator_arg_t,
                                            AllocType,
                                            bsl::nullopt_t)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(bsl::allocator_arg_t,
                                            AllocType,
                                            const Optional_Base&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                                 bsl::allocator_arg_t,
                                 AllocType,
                                 BloombergLP::bslmf::MovableRef<Optional_Base>)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                         bsl::allocator_arg_t,
                         AllocType,
                         BloombergLP::bslstl::Optional_ConstructFromForwardRef,
                         BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE))
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional,
                  const Optional_Base<t_ANY_TYPE>&)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                  bsl::allocator_arg_t,
                  AllocType,
                  BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional,
                  BSLMF_MOVABLEREF_DEDUCE(Optional_Base<t_ANY_TYPE>))
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE>
template <class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                                  bsl::allocator_arg_t,
                                  AllocType,
                                  bsl::in_place_t,
                                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
inline
Optional_Base<t_TYPE, false>::Optional_Base(
                                  bsl::allocator_arg_t,
                                  AllocType,
                                  bsl::in_place_t,
                                  std::initializer_list<t_INIT_LIST_TYPE>,
                                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...)
{
    BSLS_ASSERT_INVOKE_NORETURN("Unreachable");
}
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

// PROTECTED MANIPULATORS
template <class t_TYPE>
template <class t_ANY_TYPE>
void Optional_Base<t_TYPE, false>::assignOrEmplace(
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) rhs)
{
    if (has_value()) {
        d_value.value() = BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, rhs);
    } else {
        emplace(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, rhs));
    }
}

#  ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class t_TYPE>
t_TYPE& Optional_Base<t_TYPE, false>::dereferenceRaw()
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();

}

// PROTECTED ACCESORS
template <class t_TYPE>
const t_TYPE& Optional_Base<t_TYPE, false>::dereferenceRaw() const
{
    // This method is provided for the purpose of allowing 'NullableValue' to
    // determine the assert level in its value() method.  Do not assert here.

    return d_value.value();

}
#  endif  // BDE_OMIT_INTERNAL_DEPRECATED

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE>
template <class... t_ARGS>
inline
t_TYPE&
Optional_Base<t_TYPE, false>::emplace(
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    return d_value.emplace(NULL,
                           BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
t_TYPE& Optional_Base<t_TYPE, false>::emplace(
    std::initializer_list<t_INIT_LIST_TYPE>      il,
    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    return d_value.emplace(
        NULL, il, BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

template <class t_TYPE>
inline
void Optional_Base<t_TYPE, false>::reset() BSLS_KEYWORD_NOEXCEPT
{
    d_value.reset();
}

template <class t_TYPE>
void Optional_Base<t_TYPE, false>::swap(Optional_Base& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           bsl::is_nothrow_move_constructible<t_TYPE>::value &&
                           bsl::is_nothrow_swappable<t_TYPE>::value)
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
template <class t_TYPE>
inline
t_TYPE& Optional_Base<t_TYPE, false>::value() &
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
template <class t_TYPE>
inline
t_TYPE&&
Optional_Base<t_TYPE, false>::value() &&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
t_TYPE& Optional_Base<t_TYPE, false>::value()
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
template <class t_ANY_TYPE>
inline
t_TYPE
Optional_Base<t_TYPE, false>::value_or(t_ANY_TYPE&& value) &&
{
    if (has_value()) {
        return t_TYPE(std::move(d_value.value()));                    // RETURN
    }
    else {
        return t_TYPE(std::forward<t_ANY_TYPE>(value));               // RETURN
    }
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>&
Optional_Base<t_TYPE, false>::operator=(const Optional_Base& rhs)
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

template <class t_TYPE>
inline
Optional_Base<t_TYPE, false>&
Optional_Base<t_TYPE, false>::operator=(
                             BloombergLP::bslmf::MovableRef<Optional_Base> rhs)
{
    Optional_Base& lvalue = rhs;
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

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
t_TYPE& Optional_Base<t_TYPE, false>::operator*() &
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class t_TYPE>
inline
t_TYPE&& Optional_Base<t_TYPE, false>::operator*() &&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}
#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
t_TYPE& Optional_Base<t_TYPE, false>::operator*()
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class t_TYPE>
inline
t_TYPE *Optional_Base<t_TYPE, false>::operator->()
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

// ACCESSORS

template <class t_TYPE>
inline
bool Optional_Base<t_TYPE, false>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.hasValue();
}

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
const t_TYPE&
Optional_Base<t_TYPE, false>::value() const&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class t_TYPE>
inline
const t_TYPE&&
Optional_Base<t_TYPE, false>::value() const&&
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
const t_TYPE&
Optional_Base<t_TYPE, false>::value() const
{
    if (!has_value())
        BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

template <class t_TYPE>
template <class t_ANY_TYPE>
inline
t_TYPE
Optional_Base<t_TYPE, false>::value_or(
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value)
const &
{
    if (has_value()) {
        return t_TYPE(d_value.value());                               // RETURN
    }
    else {
        return t_TYPE(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE,
                                                    value));          // RETURN
    }
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
template <class t_ANY_TYPE>
inline
t_TYPE
Optional_Base<t_TYPE, false>::value_or(
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value)
const
{
    if (has_value()) {
        return t_TYPE(d_value.value());                               // RETURN
    }
    else {
        return t_TYPE(BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE,
                                                    value));          // RETURN
    }
}

#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

template <class t_TYPE>
inline
const t_TYPE *Optional_Base<t_TYPE, false>::operator->() const
{
    BSLS_ASSERT(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

#  ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
const t_TYPE& Optional_Base<t_TYPE, false>::operator*() const&
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}

template <class t_TYPE>
inline
const t_TYPE&& Optional_Base<t_TYPE, false>::operator*() const&&
{
    BSLS_ASSERT(has_value());

    return std::move(d_value.value());
}

#  else  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
template <class t_TYPE>
inline
const t_TYPE& Optional_Base<t_TYPE, false>::operator*() const
{
    BSLS_ASSERT(has_value());

    return d_value.value();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS else

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class t_TYPE>
Optional_Base<t_TYPE, false>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
# endif  // else BSLSTL_OPTIONAL_USES_STD_ALIASES

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                   Section: 'optional' Method Definitions
// ============================================================================

                               // ==============
                               // class optional
                               // ==============

namespace bsl {

// CREATORS
template <class t_TYPE>
optional<t_TYPE>::optional() BSLS_KEYWORD_NOEXCEPT
: BaseType()
{
}

template <class t_TYPE>
optional<t_TYPE>::optional(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
: BaseType()
{
}

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
template <class t_DERIVED>
optional<t_TYPE>::optional(
                    BloombergLP::bslmf::MovableRef<t_DERIVED> original,
                    BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(t_DERIVED))
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                             bsl::is_nothrow_move_constructible<t_TYPE>::value)
: BaseType(MoveUtil::move(static_cast<BaseType&>(original)))
{
}
#endif

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
          BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
          BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_ConstructFromForwardRef(),
           BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
              BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
              BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
              BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_ConstructFromForwardRef(),
           BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     const optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                      const t_ANY_TYPE&))
: BaseType(BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     const optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, const t_ANY_TYPE&))
: BaseType(BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional(),
           MoveUtil::move(
                  static_cast<BloombergLP::bslstl::Optional_Base<t_ANY_TYPE>&>(
                      original)))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional(),
           MoveUtil::move(
                  static_cast<BloombergLP::bslstl::Optional_Base<t_ANY_TYPE>&>(
                      original)))
{
}

#ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     const std::optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_CopyConstructFromStdOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     const std::optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_CopyConstructFromStdOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    std::optional<t_ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_MoveConstructFromStdOptional(),
           std::move(original))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    std::optional<t_ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(BloombergLP::bslstl::Optional_MoveConstructFromStdOptional(),
           std::move(original))
{
}
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE>
template <class... t_ARGS>
optional<t_TYPE>::optional(bsl::in_place_t,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
: BaseType(bsl::in_place, BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...)
{
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
optional<t_TYPE>::optional(bsl::in_place_t,
                           std::initializer_list<t_INIT_LIST_TYPE>      il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
: BaseType(bsl::in_place, il, BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...)
{
}
#endif
#endif

template <class t_TYPE>
optional<t_TYPE>::optional(bsl::allocator_arg_t, AllocType allocator)
: BaseType(bsl::allocator_arg, allocator)
{
}

template <class t_TYPE>
optional<t_TYPE>::optional(bsl::allocator_arg_t,
                           AllocType allocator,
                           bsl::nullopt_t)
: BaseType(bsl::allocator_arg, allocator)
{
}

template <class t_TYPE>
optional<t_TYPE>::optional(bsl::allocator_arg_t,
                           AllocType       allocator,
                           const optional& original)
: BaseType(bsl::allocator_arg, allocator, original)
{
}

template <class t_TYPE>
template <class t_DERIVED>
optional<t_TYPE>::optional(bsl::allocator_arg_t,
                           AllocType                          allocator,
                           BSLMF_MOVABLEREF_DEDUCE(t_DERIVED) original,
                           BSLSTL_OPTIONAL_DEFINE_IF_DERIVED_FROM_OPTIONAL(
                              typename bsl::remove_reference<t_DERIVED>::type))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           MoveUtil::move(static_cast<BaseType&>(original)))
{
    // Implementation Note: The 'remove_reference' in the signature is needed
    // to work around a bug in GCC 10.  (It has not been determined whether
    // other versions are affected.)
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
          bsl::allocator_arg_t,
          AllocType                                     allocator,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
          BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
          BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_ConstructFromForwardRef(),
           BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
             bsl::allocator_arg_t,
             AllocType                                     allocator,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value,
             BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM(t_TYPE, t_ANY_TYPE),
             BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_ConstructFromForwardRef(),
           BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     bsl::allocator_arg_t,
     AllocType                   allocator,
     const optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                      const t_ANY_TYPE&))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     bsl::allocator_arg_t,
     AllocType                   allocator,
     const optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, const t_ANY_TYPE&))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_CopyConstructFromOtherOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    bsl::allocator_arg_t,
    AllocType                                     allocator,
    BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional(),
           MoveUtil::move(
                  static_cast<BloombergLP::bslstl::Optional_Base<t_ANY_TYPE>&>(
                      original)))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    bsl::allocator_arg_t,
    AllocType                                     allocator,
    BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_BSL_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_MoveConstructFromOtherOptional(),
           MoveUtil::move(
                  static_cast<BloombergLP::bslstl::Optional_Base<t_ANY_TYPE>&>(
                      original)))
{
}

#ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     bsl::allocator_arg_t,
     AllocType                        allocator,
     const std::optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE,
                                                      const t_ANY_TYPE&))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_CopyConstructFromStdOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
     bsl::allocator_arg_t,
     AllocType                        allocator,
     const std::optional<t_ANY_TYPE>& original,
     BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE,
                                                            const t_ANY_TYPE&),
     BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, const t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_CopyConstructFromStdOptional(),
           original)
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    bsl::allocator_arg_t,
    AllocType                   allocator,
    std::optional<t_ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_MoveConstructFromStdOptional(),
           std::move(original))
{
}

template <class t_TYPE>
template <class t_ANY_TYPE>
optional<t_TYPE>::optional(
    bsl::allocator_arg_t,
    AllocType                   allocator,
    std::optional<t_ANY_TYPE>&& original,
    BSLSTL_OPTIONAL_DEFINE_IF_CONSTRUCTS_FROM_STD_OPTIONAL(t_TYPE, t_ANY_TYPE),
    BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT(t_TYPE, t_ANY_TYPE))
: BaseType(bsl::allocator_arg_t(),
           allocator,
           BloombergLP::bslstl::Optional_MoveConstructFromStdOptional(),
           std::move(original))
{
}
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE>
template <class... t_ARGS>
optional<t_TYPE>::optional(
                        bsl::allocator_arg_t,
                        AllocType                                    allocator,
                        bsl::in_place_t,
                        BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
: BaseType(bsl::allocator_arg,
           allocator,
           bsl::in_place,
           BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...)
{
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE>
template <class t_INIT_LIST_TYPE, class... t_ARGS>
optional<t_TYPE>::optional(
                        bsl::allocator_arg_t,
                        AllocType                                    allocator,
                        bsl::in_place_t,
                        std::initializer_list<t_INIT_LIST_TYPE>      il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
: BaseType(bsl::allocator_arg,
           allocator,
           bsl::in_place,
           il,
           BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...)
{
}
#endif
#endif

// MANIPULATORS
template <class t_TYPE>
optional<t_TYPE>&
optional<t_TYPE>::operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    this->reset();
    return *this;
}

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
template <class t_DERIVED>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED(t_TYPE, t_DERIVED) &
optional<t_TYPE>::operator=(BloombergLP::bslmf::MovableRef<t_DERIVED> rhs)
{
    BaseType& lvalue = rhs;
    BaseType::operator=(MoveUtil::move(lvalue));
    return *this;
}
#endif

template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(t_TYPE, const t_ANY_TYPE&)&
optional<t_TYPE>::operator=(const optional<t_ANY_TYPE>& rhs)
{
    if (rhs.has_value()) {
        this->assignOrEmplace(*rhs);
    } else {
        this->reset();
    }
    return *this;
}

template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL(t_TYPE, t_ANY_TYPE)&
optional<t_TYPE>::operator=(BSLMF_MOVABLEREF_DEDUCE(optional<t_ANY_TYPE>) rhs)
{
    BloombergLP::bslstl::Optional_Base<t_ANY_TYPE>& lvalue = rhs;
    if (lvalue.has_value()) {
        this->assignOrEmplace(MoveUtil::move(*lvalue));
    } else {
        this->reset();
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_FORWARD_REF(t_TYPE, t_ANY_TYPE)&
optional<t_TYPE>::operator=(t_ANY_TYPE&& rhs)
{
    this->assignOrEmplace(std::forward<t_ANY_TYPE>(rhs));
    return *this;
}
#else
template <class t_TYPE>
optional<t_TYPE>& optional<t_TYPE>::operator=(const t_TYPE& rhs)
{
    this->assignOrEmplace(rhs);
    return *this;
}

template <class t_TYPE>
optional<t_TYPE>&
optional<t_TYPE>::operator=(BloombergLP::bslmf::MovableRef<t_TYPE> rhs)
{
    this->assignOrEmplace(MoveUtil::move(rhs));
    return *this;
}

template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(t_TYPE, t_ANY_TYPE)&
optional<t_TYPE>::operator=(const t_ANY_TYPE& rhs)
{
    this->assignOrEmplace(rhs);
    return *this;
}

template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_ANY_TYPE(t_TYPE, t_ANY_TYPE)&
optional<t_TYPE>::operator=(BloombergLP::bslmf::MovableRef<t_ANY_TYPE> rhs)
{
    this->assignOrEmplace(MoveUtil::move(rhs));
    return *this;
}
#endif  // RVALUES else

#ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(t_TYPE, const t_ANY_TYPE&)&
optional<t_TYPE>::operator=(const std::optional<t_ANY_TYPE>& rhs)
{
    if (rhs.has_value()) {
        this->assignOrEmplace(*rhs);
    } else {
        this->reset();
    }
    return *this;
}

template <class t_TYPE>
template <class t_ANY_TYPE>
BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL(t_TYPE, t_ANY_TYPE)&
optional<t_TYPE>::operator=(std::optional<t_ANY_TYPE>&& rhs)
{
    if (rhs.has_value()) {
        this->assignOrEmplace(std::move(*rhs));
    } else {
        this->reset();
    }
    return *this;
}
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

// ============================================================================
//                      Section: Free Function Definitions
// ============================================================================

template <class t_TYPE>
inline
typename bsl::enable_if<BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(bsl::optional<t_TYPE>& lhs, bsl::optional<t_TYPE>& rhs)
{
    if (lhs.get_allocator() == rhs.get_allocator()) {
        lhs.swap(rhs);

        return;                                                       // RETURN
    }

    bsl::optional<t_TYPE> futureLhs(
        bsl::allocator_arg, lhs.get_allocator(), rhs);
    bsl::optional<t_TYPE> futureRhs(
        bsl::allocator_arg, rhs.get_allocator(), lhs);

    futureLhs.swap(lhs);
    futureRhs.swap(rhs);
}

template <class t_TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(bsl::optional<t_TYPE>& lhs, bsl::optional<t_TYPE>& rhs)
{
    lhs.swap(rhs);
}

// HASH SPECIALIZATIONS
template <class t_HASHALG, class t_TYPE>
void hashAppend(t_HASHALG& hashAlg, const bsl::optional<t_TYPE>& input)
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

// FREE OPERATORS

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs == *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    })
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs == *rhs;                                          // RETURN
    }
    return lhs.has_value() == rhs.has_value();
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs != *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    })
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs != *rhs;                                          // RETURN
    }

    return lhs.has_value() != rhs.has_value();
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const bsl::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs < *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    })
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !lhs.has_value() || *lhs < *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const bsl::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs > *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    })
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !rhs.has_value() || *lhs > *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs <= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    })
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() && *lhs <= *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const bsl::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(requires {
        { *lhs >= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
    })
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }
    return lhs.has_value() && *lhs >= *rhs;
}

// comparison with 'nullopt_t'

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator==(
                                   const bsl::optional<t_TYPE>& value,
                                   const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

#if !(defined(BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON) &&          \
      defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS))
template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator==(
                      const bsl::nullopt_t&,
                      const bsl::optional<t_TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                                   const bsl::optional<t_TYPE>& value,
                                   const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator!=(
                      const bsl::nullopt_t&,
                      const bsl::optional<t_TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<(
                                   const bsl::optional<t_TYPE>&,
                                   const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<(
                      const bsl::nullopt_t&,
                      const bsl::optional<t_TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>(
                                   const bsl::optional<t_TYPE>& value,
                                   const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>(
                            const bsl::nullopt_t&,
                            const bsl::optional<t_TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                                   const bsl::optional<t_TYPE>& value,
                                   const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator<=(
                            const bsl::nullopt_t&,
                            const bsl::optional<t_TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                                   const bsl::optional<t_TYPE>&,
                                   const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool operator>=(
                      const bsl::nullopt_t&,
                      const bsl::optional<t_TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}
#endif  // !(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS &&
        //   BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON)

// comparison with 'value_type'

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs == rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return lhs.has_value() && *lhs == rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator==(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs == *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return rhs.has_value() && lhs == *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs != rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return !lhs.has_value() || *lhs != rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator!=(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs != *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return !rhs.has_value() || lhs != *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs < rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return !lhs.has_value() || *lhs < rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs < *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return rhs.has_value() && lhs < *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs > rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return lhs.has_value() && *lhs > rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs > *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return !rhs.has_value() || lhs > *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs <= rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return !lhs.has_value() || *lhs <= rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator<=(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs <= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return rhs.has_value() && lhs <= *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const bsl::optional<t_LHS_TYPE>& lhs, const t_RHS_TYPE& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS_TYPE>
        && requires {
            { *lhs >= rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return lhs.has_value() && *lhs >= rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
bool operator>=(const t_LHS_TYPE& lhs, const bsl::optional<t_RHS_TYPE>& rhs)
    BSLSTL_OPTIONAL_REQUIRES(
        !BloombergLP::bslstl::Optional_DerivedFromOptional<t_LHS_TYPE>
        && requires {
            { lhs >= *rhs } -> BloombergLP::bslstl::Optional_ConvertibleToBool;
        })
{
    return !rhs.has_value() || lhs >= *rhs;
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
constexpr compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
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
requires (!BloombergLP::bslstl::Optional_DerivedFromOptional<t_RHS>) &&
         three_way_comparable_with<t_LHS, t_RHS>
constexpr compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
                                               const bsl::optional<t_LHS>& lhs,
                                               const t_RHS&                rhs)
{
    if (lhs) {
        return *lhs <=> rhs;
    }
    return strong_ordering::less;
}

template <class t_TYPE>
constexpr strong_ordering operator<=>(const bsl::optional<t_TYPE>& value,
                                      bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value() <=> false;
}

template <class t_LHS, three_way_comparable_with<t_LHS> t_RHS>
constexpr compare_three_way_result_t<t_LHS, t_RHS> operator<=>(
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

# ifdef BSLSTL_OPTIONAL_USES_STD_ALIASES
template <class t_TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(bsl::optional<t_TYPE>& lhs, std::optional<t_TYPE>& rhs)
{
    lhs.swap(rhs);
}

template <class t_TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<t_TYPE>::value,
                        void>::type
swap(std::optional<t_TYPE>& lhs, bsl::optional<t_TYPE>& rhs)
{
    lhs.swap(rhs);
}

// comparison with 'std::optional'

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator==(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs == *rhs;
    }
    return lhs.has_value() == rhs.has_value();
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator==(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs == *rhs;
    }
    return lhs.has_value() == rhs.has_value();
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator!=(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs != *rhs;
    }

    return lhs.has_value() != rhs.has_value();
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator!=(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
{
    if (lhs.has_value() && rhs.has_value()) {
        return *lhs != *rhs;
    }

    return lhs.has_value() != rhs.has_value();
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator<(const bsl::optional<t_LHS_TYPE>& lhs,
               const std::optional<t_RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !lhs.has_value() || *lhs < *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator<(const std::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !lhs.has_value() || *lhs < *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator>(const bsl::optional<t_LHS_TYPE>& lhs,
               const std::optional<t_RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !rhs.has_value() || *lhs > *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator>(const std::optional<t_LHS_TYPE>& lhs,
               const bsl::optional<t_RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return !rhs.has_value() || *lhs > *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator<=(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() && *lhs <= *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator<=(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() && *lhs <= *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator>=(const bsl::optional<t_LHS_TYPE>& lhs,
                const std::optional<t_RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }
    return lhs.has_value() && *lhs >= *rhs;
}

template <class t_LHS_TYPE, class t_RHS_TYPE>
inline
bool operator>=(const std::optional<t_LHS_TYPE>& lhs,
                const bsl::optional<t_RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }
    return lhs.has_value() && *lhs >= *rhs;
}
# endif  // BSLSTL_OPTIONAL_USES_STD_ALIASES

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<t_TYPE>::type>
make_optional(bsl::allocator_arg_t,
              const typename bsl::optional<
                  typename bsl::decay<t_TYPE>::type>::allocator_type& alloc,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)               rhs)
{
    return bsl::optional<typename bsl::decay<t_TYPE>::type>(
                                   bsl::allocator_arg,
                                   alloc,
                                   bsl::in_place,
                                   BSLS_COMPILERFEATURES_FORWARD(t_TYPE, rhs));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional(
                 bsl::allocator_arg_t,
                 typename bsl::optional<t_TYPE>::allocator_type const&   alloc,
                 BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...            args)
{
    return bsl::optional<t_TYPE>(
                               bsl::allocator_arg,
                               alloc,
                               bsl::in_place,
                               BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
template <class t_TYPE, class t_INIT_LIST_TYPE, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional(
                   bsl::allocator_arg_t,
                   typename bsl::optional<t_TYPE>::allocator_type const& alloc,
                   std::initializer_list<t_INIT_LIST_TYPE>               il,
                   BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)...          args)
{
    return bsl::optional<t_TYPE>(
                               bsl::allocator_arg,
                               alloc,
                               bsl::in_place,
                               il,
                               BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<typename bsl::decay<t_TYPE>::type>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE) rhs)
{
    return bsl::optional<typename bsl::decay<t_TYPE>::type>(
        BSLS_COMPILERFEATURES_FORWARD(t_TYPE, rhs));
}

template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional()
{
    return bsl::optional<t_TYPE>(bsl::in_place);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, class t_ARG, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG(t_ARG)
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)     arg,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    return bsl::optional<t_TYPE>(
                               bsl::in_place,
                               BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg),
                               BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) &&      \
    !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
template <class t_TYPE, class t_INIT_LIST_TYPE, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::optional<t_TYPE> make_optional(
                             std::initializer_list<t_INIT_LIST_TYPE>      il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARGS)... args)
{
    return bsl::optional<t_TYPE>(
                               bsl::in_place,
                               il,
                               BSLS_COMPILERFEATURES_FORWARD(t_ARGS, args)...);
}
#  endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif
}  // close namespace bsl

// There is a problem in the standard definition of the is-optional concept
// that results in types inheriting from std::optional not being identified
// correctly as optional types.  The end result is endless recursion evaluating
// the requires clause for the spaceship operator when it is implemented
// according to the C++20 specification, which happens for GCC 11-14 and
// MSVC-2022 prior to MSVC 19.36 when building with C++20 and later. In MSVC
// 19.36 Microsoft implemented the solution suggested in LWG-3746, so the
// workaround is not required for that and subsequent versions; similarly, the
// issue will be resolved in GCC 15 (to be released).
//
// The issue with the standard is tracked here:
// https://cplusplus.github.io/LWG/issue3746
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

# if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L && \
     defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) && \
     (11 <= _GLIBCXX_RELEASE && _GLIBCXX_RELEASE <= 14)

#define BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED

namespace std {
template<typename _Tp>
inline constexpr bool __is_optional_v<bsl::optional<_Tp>> = true;
}

# endif // BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED

# if BSLS_COMPILERFEATURES_CPLUSPLUS == 202002L &&                            \
     defined(BSLS_PLATFORM_CMP_MSVC) &&                                       \
     (BSLS_PLATFORM_CMP_VERSION >= 1930 && BSLS_PLATFORM_CMP_VERSION < 1936)
    // This workaround is not required from MSVC 19.36 onwards.

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
#undef BSLSTL_OPTIONAL_DECLARE_IF_DERIVED_FROM_OPTIONAL
#undef BSLSTL_OPTIONAL_DECLARE_IF_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_DEFINE_IF_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_DECLARE_IF_NOT_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_DEFINE_IF_NOT_EXPLICIT_CONSTRUCT
#undef BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_BSL_OPTIONAL
#undef BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_STD_OPTIONAL
#undef BSLSTL_OPTIONAL_ENABLE_ASSIGN_FROM_DERIVED
#undef BSLSTL_OPTIONAL_ENABLE_IF_NOT_ALLOCATOR_TAG
#undef BSLSTL_OPTIONAL_DEFAULT_TEMPLATE_ARG
#undef BSLSTL_OPTIONAL_REQUIRES

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
