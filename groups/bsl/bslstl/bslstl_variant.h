// bslstl_variant.h                                                   -*-C++-*-

#ifndef INCLUDED_BSLSTL_VARIANT
#define INCLUDED_BSLSTL_VARIANT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant allocator aware variant type.
//
//@CLASSES:
//  bsl::variant: allocator-aware implementation of `std::variant`
//  bsl::variant_alternative: metafunction to return a variant alternative
//  bsl::variant_alternative_t: alias to return type of `variant_alternative`
//  bsl::variant_size: metafunction to return number of variant alternatives
//  bsl::variant_size_v: the result value of the `variant_size` metafunction
//
//@CANONICAL_HEADER: bsl_variant.h
//
//@DESCRIPTION: This component provides a class template,
// `bsl::variant<TYPES...>`, that is a not-yet-standardised allocator-aware
// version of `std::variant`.  For functionality common to `std::variant`,
// C++23 was used as the reference specification, modulo limitations listed
// below.  `bsl::variant` may hold and manage the lifetime of an object, known
// as the *contained value*, which is stored within the footprint of the
// `bsl::variant` object, and must be one of the template arguments `TYPES`.
// These template arguments are called *alternatives*, and the alternative
// corresponding to the contained value is said to be *active*.  A
// `bsl::variant` object may also hold no value under exceptional
//  circumstances.
//
// A program that instantiates the definition of `variant` with no template
// arguments is ill-formed.
//
// A reference or pointer to the contained value of a `bsl::variant` can be
// obtained using the free functions `get` or `get_if`, respectively.  Such a
// reference or pointer that does not have top-level `const` may be used to
// modify the contained value directly, if desired.
//
// `bsl::variant` is copy/move constructible when all alternatives are
// copy/move constructible; the resulting object holds the alternative that the
// source object held.  `bsl::variant` can also be constructed from a value of
// one of the alternatives, or from an expression for which there is an
// unambiguous best match conversion to one of the alternatives.  In addition,
// `bsl::variant` supports construction of an explicitly specified alternative
// from a variadic number of arguments.
//
// If at least one alternative is allocator-aware, `bsl::variant` is
// allocator-aware.  For an allocator-aware `bsl::variant`, each constructor
// has a matching allocator-extended version that specifies the allocator that
// will be used during the lifetime of the `bsl::variant` object to construct
// any allocator-aware alternative that the `bsl::variant` object holds.  Note
// that the `bsl::variant` object itself does not allocate any memory; its
// footprint is large enough to hold any of its alternatives.
//
// `bsl::variant` is copy/move assignable when all alternatives are copy/move
// assignable and copy/move constructible; if the LHS has a different active
// alternative than the RHS, the contained value of the LHS will be destroyed
// before the contained value of the new alternative is created.
// `bsl::variant` may also be assigned to from an expression for which there is
// an unambiguous best match conversion to one of the alternatives.
//
// The `bsl::variant::emplace` methods, which take an explicitly specified
// alternative, can also be used to construct a contained value after a
// `bsl::variant` object has already been constructed.  If the `bsl::variant`
// object already holds a contained value, that object will be destroyed before
// the specified alternative is created.
//
// If an exception is thrown during an operation that changes the active
// alternative in a `bsl::variant` object, the `bsl::variant` object might be
// left in a state that holds no value, referred to as the *valueless by
// exception* state, indicated by the `valueless_by_exception()` method
// returning `true` and the `index()` method returning `bsl::variant_npos`.
//
// Two `bsl::variant`s of the same type compare equal if they hold the same
// alternative and their contained values compare equal, or they both hold no
// value.
//
// The `index()` method returns the zero-based index of the current
// alternative.  Additionally, the free function `holds_alternative` can be
// used to check whether an explicitly specified type is the currently active
// alternative.
//
// Free function `bsl::visit` is provided that implements the visitor design
// pattern as specified by the C++ standard, modulo limitations listed below.
//
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
// bsl::variant<int, char> v1;
// bsl::variant<int, char> v2(bsl::in_place_type_t<char>(), 'c');
//
// assert(bsl::holds_alternative<int>(v1));
// assert(bsl::holds_alternative<char>(v2));
// ```
// Next, we create a visitor that can be called with a value of any of the
// alternatives:
// ```
// class MyVisitor {
//   public:
//     template <class t_TYPE>
//     void operator()(const t_TYPE& value) const
//     {
//         bsl::cout << value << bsl::endl;
//     }
// };
// ```
//  We can now use `bsl::visit` to apply the visitor to our variant objects:
// ```
// MyVisitor visitor;
// bsl::visit(visitor, v1);  // prints integer 0
// bsl::visit(visitor, v2);  // prints char 'c'
// ```
//  To retrieve a contained value, we can use the `get` free functions.  If the
//  requested alternative is not the currently active alternative, an exception
//  of type `bsl::bad_variant_access` will be thrown.
// ```
// assert(0 == bsl::get<int>(v1));
// assert('c' == bsl::get<1>(v2));
// try {
//     bsl::get<int>(v2);
// } catch (const bsl::bad_variant_access& ex) {
//     bsl::cout << "non-active alternative requested" << bsl::endl;
// }
// ```
//
///Example 2: Variant Default Construction
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to default construct a `bsl::variant` which can hold an
// alternative of type `S`.  Type `S` is not default constructible so we use
// `bsl::monostate`as the first alternative to allow for default construction
// of the variant object.
// ```
// struct S {
//     S(int i) : d_i(i) {}
//     int d_i;
// };
//
// bsl::variant<bsl::monostate, S> v3;
// ```
//  To create an alternative of type `S`. we can use the emplace method.
// ```
// v3.emplace<S>(3);
// assert(bsl::holds_alternative<S>(v3));
// ```
//
///Known limitations
///-----------------
// * The variadic constructors and emplace methods in C++03 are limited to one
//   parameter.
// * In C++03, constructors and assignment operators that determine the
//   best-matching alternative (instead of taking an explicitly specified
//   alternative) require the argument type to exactly match one of the
//   alternatives, modulo cv-qualification.
// * In C++03, the majority of functions do not have constraints due to
//   language limitations. The documentation for each specific function lists
//   any constraints that are implemented.
// * Visitation functionality is limited to one variant.  Before C++17,
//   visitation only supports the `VISITOR(ALTERNATIVE)` form of invocation;
//   cases where the visitor is a pointer to member are not supported.
// * Constexpr, triviality, and exception specifications are not implemented.
// * In `operator=(const variant& rhs)` and `operator=(T&& t)`, only direct
//   copy construction from the relevant alternative is tried.  This behavior
//   differs from the standard, which requires the construction of a temporary
//   alternative object if construction of the relevant alternative is not
//   `noexcept` (see [variant.assign] for details).  The standard behavior
//   causes unnecessary performance degradation in cases where the alternative
//   constructor does not throw, yet is not marked `noexcept`; this behavior
//   is therefore not implemented in `bsl::variant`.
// * Due to the C++03 limitations of the `bsl::invoke_result` facility, it is
//   not possible to explicitly specify a return type for `bsl::visit` without
//   triggering a hard error during substitution into the overload of
//   `bsl::visit` having a deduced return type on some C++03 compilers (Sun
//   for example).  For this reason, the overload of `bsl::visit` that takes
//   the return type as the first, non-deduced template argument is not
//   provided in C++03.  The non-standard free function `bsl::visitR` may be
//   used instead.  `bsl::visitR` is also provided in C++11 and later for
//   backward compatibility with the C++03 interface.

#include <bslscm_version.h>

#include <bslstl_badvariantaccess.h>
#include <bslstl_hash.h>
#include <bslstl_inplace.h>

#include <bslalg_swaputil.h>

#include <bslma_constructionutil.h>
#include <bslma_destructionutil.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_assert.h>
#include <bslmf_conjunction.h>
#include <bslmf_integersequence.h>
#include <bslmf_integralconstant.h>
#include <bslmf_invokeresult.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_iscopyconstructible.h>
#include <bslmf_isreference.h>
#include <bslmf_issame.h>
#include <bslmf_makeintegersequence.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removeconst.h>
#include <bslmf_removecvref.h>
#include <bslmf_util.h>
#include <bslmf_voidtype.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_util.h>  // 'forward<T>(V)'

#include <stddef.h>

// We provide one of two feature sets for 'bsl::variant', depending on the
// capabilities of the compiler.  These are the necessary compiler features for
// the full feature set of 'bsl::variant'.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// Note that 'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE' does not require
// a C++14 compiler.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
#define BSL_VARIANT_FULL_IMPLEMENTATION
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif  // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
#include <type_traits>
#include <functional>  // std::invoke
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#include <initializer_list>
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri Dec 22 18:51:14 2023
// Command line: sim_cpp11_features.pl bslstl_variant.h
# define COMPILING_BSLSTL_VARIANT_H
# include <bslstl_variant_cpp03.h>
# undef COMPILING_BSLSTL_VARIANT_H
#else

// When generating the expansion of a variadic template, the
// sim_cpp11_features.pl script currently expands the primary template by the
// required number of template arguments.  If there is a partial specialization
// that adds a non variadic template parameter, the generated primary template
// will no longer match the expansion of such specialization.  In the case of
// 'variant' metafunction helpers, such additional template argument is always
// a type parameter.  To work around this issue, a dummy template type
// parameter is added to the primary template and defaulted to
// 'BSLSTL_VARIANT_NOT_A_TYPE'.
typedef void BSLSTL_VARIANT_NOT_A_TYPE;

namespace bsl {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=18
template <class t_HEAD, class... t_TAIL>
class variant;
#endif

// TRAITS

                            // ===================
                            // struct variant_size
                            // ===================

/// This metafunction calculates the number of alternatives in the (possibly
/// cv-qualified) `bsl::variant` type of (template parameter)
/// `t_BSL_VARIANT`.  The primary template is not defined.
template <class t_BSL_VARIANT>
struct variant_size;

template <class t_BSL_VARIANT>
struct variant_size<const t_BSL_VARIANT> : variant_size<t_BSL_VARIANT> {
};

template <class t_BSL_VARIANT>
struct variant_size<volatile t_BSL_VARIANT> : variant_size<t_BSL_VARIANT> {
};

template <class t_BSL_VARIANT>
struct variant_size<const volatile t_BSL_VARIANT>
: variant_size<t_BSL_VARIANT> {
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
struct variant_size<variant<t_HEAD, t_TAIL...> >
: bsl::integral_constant<size_t, 1 + sizeof...(t_TAIL)> {
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
/// This variable template represents the result value of the
/// `bsl::variant_size` metafunction.
template <class t_BSL_VARIANT>
BSLS_KEYWORD_INLINE_VARIABLE constexpr size_t variant_size_v =
    variant_size<t_BSL_VARIANT>::value;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

/// This value is returned by `bsl::variant::index()` if
/// `valueless_by_exception()` is `true`.
BSLS_KEYWORD_INLINE_CONSTEXPR size_t variant_npos = -1;

                         // ==========================
                         // struct variant_alternative
                         // ==========================

/// This metafunction calculates the type of the alternative whose index is
/// (template parameter) `t_INDEX` in the possibly cv-qualified
/// `bsl::variant` type of (template parameter) `t_TYPE`.  If `t_TYPE` is
/// cv-qualified, its cv-qualifiers are applied to the alternative.
template <size_t t_INDEX, class t_TYPE>
struct variant_alternative;

template <size_t t_INDEX, class t_TYPE>
struct variant_alternative<t_INDEX, const t_TYPE> {
    typedef typename variant_alternative<t_INDEX, t_TYPE>::type const type;
};

template <size_t t_INDEX, class t_TYPE>
struct variant_alternative<t_INDEX, volatile t_TYPE> {
    typedef typename variant_alternative<t_INDEX, t_TYPE>::type volatile type;
};

template <size_t t_INDEX, class t_TYPE>
struct variant_alternative<t_INDEX, const volatile t_TYPE> {
    typedef
        typename variant_alternative<t_INDEX, t_TYPE>::type const volatile type;
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// 'Variant_VariantAlternativeImpl' defined to avoid 'variant<>' from the
// sim_cpp11_features.pl script

/// This component-private metafunction provides the implementation of
/// `bsl::variant_alternative`.
template <size_t t_INDEX,
          class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... t_TAIL>
struct Variant_VariantAlternativeImpl
: Variant_VariantAlternativeImpl<t_INDEX - 1, t_TAIL...> {
};

template <size_t t_INDEX>
struct Variant_VariantAlternativeImpl<t_INDEX, BSLSTL_VARIANT_NOT_A_TYPE> {
};

template <>
struct Variant_VariantAlternativeImpl<0, BSLSTL_VARIANT_NOT_A_TYPE> {
};

template <class t_HEAD, class... t_TAIL>
struct Variant_VariantAlternativeImpl<0, t_HEAD, t_TAIL...> {
    typedef t_HEAD type;
};

template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
struct variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >
: Variant_VariantAlternativeImpl<t_INDEX, t_HEAD, t_TAIL...> {
    BSLMF_ASSERT((t_INDEX <
                  bsl::variant_size<variant<t_HEAD, t_TAIL...> >::value));
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
/// `variant_alternative_t` is an alias to the return type of the
/// `variant_alternative` metafunction.
template <size_t t_INDEX, class t_TYPE>
using variant_alternative_t =
    typename variant_alternative<t_INDEX, t_TYPE>::type;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// FREE FUNCTIONS

/// Efficiently exchange the values of the specified `lhs` and `rhs`
/// objects.  This method provides the no-throw guarantee if the two variant
/// objects being swapped have the same active alternative and that
/// alternative provides that guarantee; otherwise, this method provides the
/// basic guarantee.  If `lhs` and `rhs` do not contain the same alternative
/// or they have unequal allocators, and an exception is thrown during the
/// swap, either or both variant objects may be left in a valueless state or
/// with an alternative in a moved-from state.  All alternatives shall be
/// move constructible and swappable.  For simplicity of implementation,
/// this function differs from the standard in the following :
/// * constraints are not implemented
/// * constexpr is not implemented
template <class t_HEAD, class... t_TAIL>
void swap(bsl::variant<t_HEAD, t_TAIL...>& lhs,
          bsl::variant<t_HEAD, t_TAIL...>& rhs);

// HASH SPECIALIZATIONS

/// Pass the specified `input` to the specified `hashAlg`, where `hashAlg`
/// is a hashing algorithm.
template <class t_HASHALG, class t_HEAD, class... t_TAIL>
void hashAppend(t_HASHALG& hashAlg, const variant<t_HEAD, t_TAIL...>& input);

// 20.7.5, value access

/// Return `true` if the specified `obj` currently holds the (template
/// parameter) `t_TYPE` alternative, and `false` otherwise.  `t_TYPE` shall
/// appear exactly once in the variant's list of alternatives.
template <class t_TYPE, class t_HEAD, class... t_TAIL>
bool holds_alternative(
                  const variant<t_HEAD, t_TAIL...>& obj) BSLS_KEYWORD_NOEXCEPT;
#endif
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
/// Return a reference to the alternative object at index (template
/// parameter) `t_INDEX` in the specified `obj`.  If `t_INDEX` is not the
/// index of the currently active alternative, throw an exception of type
/// `bad_variant_access`.  `t_INDEX` shall be a valid index for the variant
/// type of `obj`.
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type& get(
                                              variant<t_HEAD, t_TAIL...>& obj);
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
const typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
get(const variant<t_HEAD, t_TAIL...>& obj);
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&& get(
                                             variant<t_HEAD, t_TAIL...>&& obj);
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
const typename variant_alternative<t_INDEX,
                                   variant<t_HEAD, t_TAIL...> >::type&&
get(const variant<t_HEAD, t_TAIL...>&& obj);

/// Return a reference to the alternative object with type (template
/// parameter) `t_TYPE` in the specified `obj`.  If `t_TYPE` is not the type
/// of the currently active alternative, throw an exception of type
/// `bad_variant_access`.  `t_TYPE` shall appear exactly once in the
/// variant's list of alternatives.
template <class t_TYPE, class t_HEAD, class... t_TAIL>
t_TYPE& get(variant<t_HEAD, t_TAIL...>& obj);
template <class t_TYPE, class t_HEAD, class... t_TAIL>
const t_TYPE& get(const variant<t_HEAD, t_TAIL...>& obj);
template <class t_TYPE, class t_HEAD, class... t_TAIL>
t_TYPE&& get(variant<t_HEAD, t_TAIL...>&& obj);
template <class t_TYPE, class t_HEAD, class... t_TAIL>
const t_TYPE&& get(const variant<t_HEAD, t_TAIL...>&& obj);

template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename add_pointer<
    typename variant_alternative<t_INDEX,
                                 variant<t_HEAD, t_TAIL...> >::type>::type
get_if(variant<t_HEAD, t_TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;

/// Return a pointer to the alternative object with index (template
/// parameter) `t_INDEX` in the specified `obj`, or a null pointer if `obj`
/// itself is a null pointer or if `t_INDEX` is not the index of the
/// currently active alternative.  `t_INDEX` shall be a valid alternative
/// index.
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename add_pointer<const typename variant_alternative<
    t_INDEX,
    variant<t_HEAD, t_TAIL...> >::type>::type
get_if(const variant<t_HEAD, t_TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;

template <class t_TYPE, class t_HEAD, class... t_TAIL>
typename add_pointer<t_TYPE>::type get_if(
                        variant<t_HEAD, t_TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;

/// Return a pointer to the alternative object with type (template
/// parameter) `t_TYPE` in the specified `obj`, or a null pointer if `obj`
/// itself is a null pointer or if `t_TYPE` is not the type of the currently
/// active alternative.  `t_TYPE` shall appear exactly once in the variant's
/// list of alternatives.
template <class t_TYPE, class t_HEAD, class... t_TAIL>
typename add_pointer<const t_TYPE>::type get_if(
                  const variant<t_HEAD, t_TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;
#else  // BSL_VARIANT_FULL_IMPLEMENTATION

// See 'bslstl_variant.cpp' for implementation notes regarding 'bsl::get'.

/// This component-private metafunction computes the return types for
/// `bsl::get<t_INDEX>` and `bsl::get_if<t_INDEX>` with an argument of type
/// `t_ARG` or `t_ARG*`, respectively, where `t_VARIANT` is `t_ARG` with
/// cvref-qualifiers stripped, and shall be a specialization of `bsl::variant`.
/// `t_VALID` shall be `true` if and only if `t_INDEX` is a valid index for
/// `t_VARIANT`.  If `t_VALID` is `false`, no member typedefs are provided.
template <bool t_VALID, size_t t_INDEX, class t_ARG, class t_VARIANT>
struct Variant_GetIndexReturnTypeImpl {
};

template <size_t t_INDEX, class t_VARIANT>
struct Variant_GetIndexReturnTypeImpl<true, t_INDEX, t_VARIANT, t_VARIANT> {
    typedef typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type& type;

    typedef
        typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type *pointer;
};

template <size_t t_INDEX, class t_VARIANT>
struct Variant_GetIndexReturnTypeImpl<true,
                                      t_INDEX,
                                      const t_VARIANT,
                                      t_VARIANT> {
    typedef typename bsl::variant_alternative<t_INDEX, const t_VARIANT>::type&
        type;

    typedef typename bsl::variant_alternative<t_INDEX, const t_VARIANT>::type
        *pointer;
};

template <size_t t_INDEX, class t_VARIANT>
struct Variant_GetIndexReturnTypeImpl<
    true,
    t_INDEX,
    BloombergLP::bslmf::MovableRef<t_VARIANT>,
    t_VARIANT> {
    typedef BloombergLP::bslmf::MovableRef<
        typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type>
        type;
};

/// This component-private metafunction computes the return types for
/// `bsl::get<t_INDEX>` and `bsl::get_if<t_INDEX>` with an argument of type
/// `t_ARG` or `t_ARG*` respectively.  If `t_ARG` is not a (possibly
/// cvref-qualified) `bsl::variant`, no definition is provided.  If `t_INDEX`
/// is not a valid index for `t_ARG`, the member typedefs `type` and `pointer`
/// are not provided.
template <size_t t_INDEX,
          class  t_ARG,
          class  t_VARIANT = typename BloombergLP::bslmf::MovableRefUtil::
              Decay<t_ARG>::type>
struct Variant_GetIndexReturnType;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <size_t t_INDEX, class t_ARG, class t_HEAD, class... t_TAIL>
struct Variant_GetIndexReturnType<t_INDEX,
                                  t_ARG,
                                  bsl::variant<t_HEAD, t_TAIL...> >
: Variant_GetIndexReturnTypeImpl<(t_INDEX <= sizeof...(t_TAIL)),
                                 t_INDEX,
                                 t_ARG,
                                 bsl::variant<t_HEAD, t_TAIL...> > {
};

/// This component-private metafunction is defined only when `t_HEAD` is a
/// specialization of `bsl::variant` and `t_TAIL` is empty, in which case it
/// provides member typedefs corresponding to `t_TYPE`.  Naming these member
/// typedefs results in a substitution failure when this metafunction is not
/// defined.
template <class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_GetTypeReturnType;

/// This partial specialization provides member typedefs that are used to
/// declare the `get` and `get_if` function templates in C++03 as non-variadic
/// function templates that are constrained to accept only references or
/// pointers to `bsl::variant`.
template <class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_GetTypeReturnType<t_TYPE, bsl::variant<t_HEAD, t_TAIL...> > {
    typedef t_TYPE                             type;
    typedef typename add_pointer<t_TYPE>::type pointer;
};
#endif

/// Return a reference to the alternative object at index (template parameter)
/// `t_INDEX` in the specified `obj`.  If `t_INDEX` is not the index of the
/// currently active alternative, throw an exception of type
/// `bad_variant_access`.  `t_INDEX` shall be a valid index for the variant
/// type of `obj`.  Note that `t_VARIANT` may be const-qualified.
template <size_t t_INDEX, class t_VARIANT>
typename Variant_GetIndexReturnType<t_INDEX, t_VARIANT>::type
get(t_VARIANT&                                obj);
template <size_t t_INDEX, class t_VARIANT>
typename Variant_GetIndexReturnType<
    t_INDEX,
    BloombergLP::bslmf::MovableRef<t_VARIANT> >::type
get(BloombergLP::bslmf::MovableRef<t_VARIANT> obj);

/// Return a reference to the alternative object with type (template parameter)
/// `t_TYPE` in the specified `obj`.  If `t_TYPE` is not the type of the
/// currently active alternative, throw an exception of type
/// `bad_variant_access`.  `t_TYPE` shall appear exactly once in the variant`s
/// list of alternatives.
template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<t_TYPE&, t_VARIANT>::type get(
                                t_VARIANT&                                obj);
template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<const t_TYPE&, t_VARIANT>::type get(
                                const t_VARIANT&                          obj);
template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<BloombergLP::bslmf::MovableRef<t_TYPE>,
                                   t_VARIANT>::type get(
                                BloombergLP::bslmf::MovableRef<t_VARIANT> obj);

// Return a pointer to the alternative object with index (template parameter)
// `t_INDEX` in the specified `obj`, or a null pointer if `obj` itself is a
// null pointer or if `t_INDEX` is not the index of the currently active
// alternative.  `t_INDEX` shall be a valid alternative index.  Note that
// `t_VARIANT` may be const-qualified.
template <size_t t_INDEX, class t_VARIANT>
typename Variant_GetIndexReturnType<t_INDEX, t_VARIANT>::pointer
get_if(t_VARIANT *obj) BSLS_KEYWORD_NOEXCEPT;

/// Return a pointer to the alternative object with type (template parameter)
/// `t_TYPE` in the specified `obj`, or a null pointer if `obj` itself is a
/// null pointer or if `t_TYPE` is not the type of the currently active
/// alternative.  `t_TYPE` shall appear exactly once in the variant`s list of
/// alternatives.
template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<t_TYPE, t_VARIANT>::pointer get_if(
                                         t_VARIANT *obj) BSLS_KEYWORD_NOEXCEPT;
template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<const t_TYPE, t_VARIANT>::pointer get_if(
                                   const t_VARIANT *obj) BSLS_KEYWORD_NOEXCEPT;
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
// FREE OPERATORS

// 20.7.6, relational operators
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Return `true` if the specified `lhs` and `rhs` are both valueless by
/// exception or if they have the same active alternative and their
/// contained values compare equal; otherwise, return `false`.  All
/// alternatives shall support `operator==`.
template <class t_HEAD, class... t_TAIL>
bool operator==(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs);

/// Return `true` if the specified `lhs` and `rhs` have different active
/// alternatives or only one holds an alternative, or if they have the same
/// active alternative and the contained values compare unequal; otherwise,
/// return `false`.  All alternatives shall support `operator!=`.
template <class t_HEAD, class... t_TAIL>
bool operator!=(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs);

/// Return `true` if the index of the active alternative in the specified
/// `lhs` is less than that of the specified `rhs`, or if both have the same
/// active alternative and the contained value of `lhs` compares less than
/// that of `rhs`, or if `lhs` is valueless by exception and `rhs` is not;
/// otherwise, return `false`.  All alternatives shall support `operator<`.
template <class t_HEAD, class... t_TAIL>
bool operator<(const variant<t_HEAD, t_TAIL...>& lhs,
               const variant<t_HEAD, t_TAIL...>& rhs);

/// Return `true` if the index of the active alternative in the specified
/// `lhs` is greater than that of the specified `rhs`, or if both have the
/// same active alternative and the contained value of `lhs` compares
/// greater than that of `rhs`, or if `rhs` is valueless by exception and
/// `lhs` is not; otherwise, return `false`.  All alternatives shall support
/// `operator>`.
template <class t_HEAD, class... t_TAIL>
bool operator>(const variant<t_HEAD, t_TAIL...>& lhs,
               const variant<t_HEAD, t_TAIL...>& rhs);

/// Return `true` if the index of the active alternative in the specified
/// `lhs` is less than that of the specified `rhs`, or if both have the same
/// active alternative and the contained value of `lhs` compares less than
/// or equal to that of `rhs`, or if `lhs` is valueless by exception;
/// otherwise, return `false`.  All alternatives shall support `operator<=`.
template <class t_HEAD, class... t_TAIL>
bool operator<=(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs);

/// Return `true` if the index of the active alternative in the specified
/// `lhs` is greater than that of the specified `rhs`, or if both have the
/// same active alternative and the contained value of `lhs` compares
/// greater than or equal to that of `rhs`, or if `rhs` is valueless by
/// exception; otherwise, return `false`.  All alternatives shall support
/// `operator>=`.
template <class t_HEAD, class... t_TAIL>
bool operator>=(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs);

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON &&             \
    defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

/// If `lhs` and `rhs` are not valueless by exception and hold the same
/// alternative `i`, return `get<i>(lhs) <=> get<i>(rhs)` .  If `lhs` and
/// `rhs` are not valueless by exception and hold the aalternatives `i` and
/// `j` respectively, return `i<=>j`.  Return `strong_ordering::equal` if
/// both variants are valueless by exception.  Return
/// `strong_ordering::less` if `lhs`  is valueless by exception.  Return
/// `strong_ordering::greater` if `rhs` is valueless by exception.
template <class... t_ALTS>
    requires(std::three_way_comparable<t_ALTS> && ...)
constexpr std::common_comparison_category_t<
    std::compare_three_way_result_t<t_ALTS>...>
operator<=>(const variant<t_ALTS...>& lhs, const variant<t_ALTS...>& rhs);
#endif
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {
// COMPONENT-PRIVATE TAG TYPES

/// This component-private tag type is used as a parameter type for
/// constructors of `Variant_Base` that accept a `std::variant`.
struct Variant_ConstructFromStdTag {};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This component-private metafunction is derived from `bsl::true_type` if
/// at least one template argument uses an allocator, and from
/// `bsl::false_type` otherwise.
template <class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE, class... t_TAIL>
struct Variant_UsesBslmaAllocatorAny;

template <>
struct Variant_UsesBslmaAllocatorAny<BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::false_type {
};

template <class t_HEAD, class... t_TAIL>
struct Variant_UsesBslmaAllocatorAny
: bsl::integral_constant<
      bool,
      BloombergLP::bslma::UsesBslmaAllocator<t_HEAD>::value ||
          BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<
          t_TAIL...>::value> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// all template arguments are bitwise moveable, and from `bsl::false_type`
/// otherwise.
template <class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE, class... t_TAIL>
struct Variant_IsBitwiseMoveableAll;

template <>
struct Variant_IsBitwiseMoveableAll<BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::true_type {
};

template <class t_HEAD, class... t_TAIL>
struct Variant_IsBitwiseMoveableAll
: bsl::integral_constant<
      bool,
      BloombergLP::bslmf::IsBitwiseMoveable<t_HEAD>::value &&
          Variant_IsBitwiseMoveableAll<t_TAIL...>::value> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// (template parameter) `t_TYPE` is not a tag type.  This metafunction
/// requires any cv and ref qualifications to be removed from the queried
/// type.
template <class t_TYPE>
struct Variant_IsTag : bsl::false_type {
};

template <>
struct Variant_IsTag<bsl::allocator_arg_t> : bsl::true_type {
};

template <class t_TYPE>
struct Variant_IsTag<bsl::in_place_type_t<t_TYPE> > : bsl::true_type {
};

template <size_t t_INDEX>
struct Variant_IsTag<bsl::in_place_index_t<t_INDEX> > : bsl::true_type {
};

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
/// This component-private metafunction derives from
/// `std::is_constructible<t_TO, t_FROM>` in C++11 and later, and
/// `bsl::true_type` in C++03.
template <class t_TO, class t_FROM>
struct Variant_IsConstructible : std::is_constructible<t_TO, t_FROM> {
};

/// This component-private metafunction derives from
/// `std::is_assignable<t_LHS, t_RHS>` in C++11 and later, and
/// `bsl::true_type` in C++03.
template <class t_LHS, class t_RHS>
struct Variant_IsAssignable : std::is_assignable<t_LHS, t_RHS> {
};
#else
/// This component-private metafunction derives from
/// `std::is_constructible<t_TO, t_FROM>` in C++11 and later, and
/// `bsl::true_type` in C++03.
template <class t_TO, class t_FROM>
struct Variant_IsConstructible : bsl::true_type {
};

// This component-private metafunction derives from
// `std::is_assignable<t_LHS, t_RHS>` in C++11 and later, and
// `bsl::true_type` in C++03.
template <class t_LHS, class t_RHS>
struct Variant_IsAssignable : bsl::true_type {
};
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

/// This component-private macro is used as a constraint in function
/// definitions which require the specified `VARIANT` to be constructible
/// from the specified `TYPE`.
#define BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(VARIANT, TYPE)               \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Variant_ConstructsFromType<VARIANT,              \
                                                        TYPE>::value,         \
        BloombergLP::bslstl::Variant_NoSuchType>::type

/// This component-private macro is used as a constraint in function
/// declarations which require the specified `VARIANT` to be constructible
/// from the specified `TYPE`.
#define BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(VARIANT, TYPE)              \
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(                                 \
                        VARIANT,                                              \
                        TYPE) = BloombergLP::bslstl::Variant_NoSuchType(0)

/// This component-private macro is used as a constraint when defining
/// constructors of `variant` from the corresponding `std::variant`.
#define BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM_STD(STD_VARIANT)             \
    bsl::enable_if_t<                                                         \
        BloombergLP::bslstl::variant_constructsFromStd<variant, STD_VARIANT>, \
        BloombergLP::bslstl::Variant_NoSuchType>

/// This component-private macro is used as a constraintwhen declaring
/// constructors of `variant` from the corresponding `std::variant`.
#define BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM_STD(STD_VARIANT)            \
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM_STD(STD_VARIANT)                 \
        = BloombergLP::bslstl::Variant_NoSuchType(0)

/// This component-private macro is used as a constraint in function
/// definitions which require the specified `TYPE` to be a unique
/// alternative in `variant<t_HEAD, t_TAIL...>`.
/// Implementation note: This macro can't use
/// `BSLSTL_VARIANT_HAS_UNIQUE_TYPE` because this macro is used at points
/// where `variant<t_HEAD, t_TAIL...>` expands to an invalid construct.
#define BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(TYPE)                        \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Variant_HasUniqueType<TYPE, variant>::value,     \
        BloombergLP::bslstl::Variant_NoSuchType>::type

/// This component-private macro is used as a constraint in function
/// declarations which require the specified `TYPE` to be a unique
/// alternative in `variant<t_HEAD, t_TAIL...>`.
#define BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(TYPE)                       \
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(                                 \
                           TYPE) = BloombergLP::bslstl::Variant_NoSuchType(0)

/// This component-private macro expands to the check for whether the
/// specified `TYPE` is a unique alternative in
/// `variant<t_HEAD, t_TAIL...>`. See definition `Variant_HasUniqueType`
/// for more details.
#define BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE)                                  \
    BloombergLP::bslstl::                                                     \
        Variant_HasUniqueType<TYPE, variant<t_HEAD, t_TAIL...> >::value

/// This component-private macro expands to the type of alternative at
/// specified `INDEX` for a variant of type
/// `bsl::variant<t_HEAD, t_TAIL...>`.
#define BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX)                                   \
    typename bsl::variant_alternative<INDEX,                                  \
                                      bsl::variant<t_HEAD, t_TAIL...> >::type

/// This component-private macro expands to the index of the first
/// alternative in the specified `VARIANT` that is identical to the
/// specified `TYPE`, or `bsl::variant_npos` if no such alternative exists.
#define BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)                                \
    BloombergLP::bslstl::Variant_TypeToIndex<TYPE, VARIANT>::value

/// This component-private macro expands to the index of the first
/// alternative in the specified `VARIANT` that is a "unique" best match for
/// conversion from `std::declval<TYPE>()`.  See the documentation for
/// `Variant_ConvertIndex` for more details.
#define BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, VARIANT)                        \
    BloombergLP::bslstl::Variant_ConvertIndex<TYPE, VARIANT>::value

/// This component-private macro expands to the type of the alternative
/// in the specified `VARIANT` that is a "unique" best match for conversion
/// from `std::declval<TYPE>()`.  See the documentation for
/// `Variant_ConvertIndex` for more details.
#define BSLSTL_VARIANT_CONVERT_TYPE_OF(TYPE, VARIANT)                         \
    typename bsl::variant_alternative<BSLSTL_VARIANT_CONVERT_INDEX_OF(        \
                                          TYPE, VARIANT),                     \
                                      VARIANT>::type

/// This component-private macro expands to the invocation of
/// `Variant_ImpUtil::visitId` with specified `RET` as return type, and
/// specified `VISITOR` and specified `VAROBJ` as arguments.  See the
/// documentation of `Variant_ImpUtil::visitId` for more details.
#define BSLSTL_VARIANT_VISITID(RET, VISITOR, VAROBJ)                          \
    BloombergLP::bslstl::Variant_ImpUtil::visitId<RET>(VISITOR, VAROBJ);

/// This component-private metafunction provides implementation for
/// `Variant_TypeToIndex`.  It evaluates to `t_INDEX + i` where `i` is the
/// zero-based index of (template parameter) `t_TYPE` in (template parameters)
/// `t_HEAD, t_TAIL...`, or `bsl::variant_npos` if `t_TYPE` is not found.
template <size_t t_INDEX,
          class t_TYPE,
          class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... t_TAIL>
struct Variant_TypeToIndexImpl
: bsl::conditional<
      bsl::is_same<t_TYPE, t_HEAD>::value,
      bsl::integral_constant<size_t, t_INDEX>,
      Variant_TypeToIndexImpl<t_INDEX + 1, t_TYPE, t_TAIL...> >::type {
};

/// This partial specialization is used when the list of alternatives
/// `t_HEAD, t_TAIL...` is empty, i.e., `t_TYPE` wasn't found in the
/// originally supplied list of alternatives.
template <size_t t_INDEX, class t_TYPE>
struct Variant_TypeToIndexImpl<t_INDEX, t_TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, bsl::variant_npos> {
};

/// This component-private metafunction calculates the zero-based index of
/// (template parameter) `t_TYPE` in the list of alternatives in (template
/// parameter) `t_VARIANT`, or `bsl::variant_npos` if there is no such
/// alternative.  The primary template (used when `t_VARIANT` is not a
/// `bsl::variant`) is not defined.
template <class t_TYPE, class t_VARIANT>
struct Variant_TypeToIndex;  // primary template not defined

template <class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_TypeToIndex<t_TYPE, bsl::variant<t_HEAD, t_TAIL...> >
: Variant_TypeToIndexImpl<0, t_TYPE, t_HEAD, t_TAIL...> {
};

/// This component-private metafunction calculates the number of times
/// (template parameter) `t_TYPE` occurs in (template parameters)
/// `t_HEAD, t_TAIL...`.  An alternative must have the same cv-qualification
/// as `t_TYPE` in order to be counted.
template <class t_TYPE,
          class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... t_TAIL>
struct Variant_CountType
: bsl::integral_constant<size_t,
                         bsl::is_same<t_TYPE, t_HEAD>::value +
                             Variant_CountType<t_TYPE, t_TAIL...>::value> {
};

/// Specialization for purposes of the sim_cpp11_features.pl script.
template <class t_TYPE>
struct Variant_CountType<t_TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, 0> {
};

/// This component-private metafunction derives from `bsl::true_type` if
/// (template parameter) `t_TYPE` occurs exactly once as an alternative of
/// (template parameter) `t_VARIANT`, and `bsl::false_type` otherwise.  An
/// alternative must have the same cv-qualification as `t_TYPE` in order to
/// be counted.  The primary template (used when `t_VARIANT` is not a
/// `bsl::variant`) is not defined.
template <class t_TYPE, class t_VARIANT>
struct Variant_HasUniqueType;

template <class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_HasUniqueType<t_TYPE, bsl::variant<t_HEAD, t_TAIL...> >
: bsl::integral_constant<bool,
                         Variant_CountType<t_TYPE, t_HEAD, t_TAIL...>::value ==
                             1> {
};

/// This component-private metafunction calculates the number of times
/// (template parameter) `t_TYPE` occurs in (template parameters)
/// `t_HEAD, t_TAIL...`, where two types that differ only in top-level
/// cv-qualification are considered to be the same.
template <class t_TYPE,
          class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... t_TAIL>
struct Variant_CountCVType
: bsl::integral_constant<
      size_t,
      bsl::is_same<typename bsl::remove_cv<t_TYPE>::type,
                   typename bsl::remove_cv<t_HEAD>::type>::value +
          Variant_CountCVType<t_TYPE, t_TAIL...>::value> {
};

/// Specialization for purposes of the sim_cpp11_features.pl script.
template <class t_TYPE>
struct Variant_CountCVType<t_TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, 0> {
};

/// This component-private metafunction derives from `bsl::true_type` if
/// (template parameter) `t_TYPE` occurs exactly once as an alternative of
/// (template parameter) `t_VARIANT`, and `bsl::false_type` otherwise, where
/// two types that differ only in top-level cv-qualification are considered
/// to be the same.  The primary template (used when `t_VARIANT` is not a
/// `bsl::variant`) is not defined.
template <class t_TYPE, class t_VARIANT>
struct Variant_HasUniqueCVType;

template <class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_HasUniqueCVType<t_TYPE, bsl::variant<t_HEAD, t_TAIL...> >
: bsl::integral_constant<
      bool,
      Variant_CountCVType<t_TYPE, t_HEAD, t_TAIL...>::value == 1> {
};
#endif

/// This component-private metafunction calculates the alternative type at
/// index (template parameter) `t_INDEX` in (template parameter)
/// `t_VARIANT`, where the cv- and ref-qualifiers of `t_VARIANT` are added
/// to the alternative type.  This metafunction is used to calculate the
/// return type of `bsl::visit`.
template <class t_VARIANT, size_t t_INDEX>
struct Variant_CVQualAlt {

    /// Alternative at `t_INDEX` with combined cv-qualifiers
    typedef typename bsl::variant_alternative<
        t_INDEX,
        typename bslmf::MovableRefUtil::RemoveReference<t_VARIANT>::type>::type
        CVAlt;

    typedef typename bsl::conditional<
        bslmf::MovableRefUtil::IsReference<t_VARIANT>::value,
        typename bsl::conditional<
            bslmf::MovableRefUtil::IsMovableReference<t_VARIANT>::value,
            typename bslmf::MovableRefUtil::AddMovableReference<CVAlt>::type,
            typename bslmf::MovableRefUtil::AddLvalueReference<CVAlt>::type>::
            type,
        CVAlt>::type type;
};

/// This component-private metafunction derives from `bsl::true_type` if,
/// for each alternative `ALTi` in (template parameter) `t_VARIANT` with
/// index less than or equal to (template parameter) `t_INDEX`,
/// `decltype(std::declval<t_VISITOR>(std::declval<ALTi>()))` is `t_RET`;
/// otherwise, this metafunction derives from `bsl::false_type`.  Note that
/// `ALTi` has the cv- and ref-qualifiers from `t_VARIANT` added to it.
/// This metafunction is used to determine whether invoking the visitor
/// results in the same type and value category for all alternatives.
template <class t_RET,
          class t_VISITOR,
          class t_VARIANT,
          size_t t_INDEX =
               bsl::variant_size<typename bslmf::MovableRefUtil::
                                     RemoveReference<t_VARIANT>::type>::value -
               1>
struct Variant_IsSameReturnType
: public bsl::integral_constant<
      bool,
      bsl::is_same<t_RET,
                   typename bsl::invoke_result<
                       t_VISITOR,
                       typename Variant_CVQualAlt<t_VARIANT, t_INDEX>::type>::
                       type>::value &&
          Variant_IsSameReturnType<t_RET, t_VISITOR, t_VARIANT, t_INDEX - 1>::
              value> {
};
template <class t_RET, class t_VISITOR, class t_VARIANT>
struct Variant_IsSameReturnType<t_RET, t_VISITOR, t_VARIANT, 0>
: bsl::is_same<t_RET,
               typename bsl::invoke_result<
                   t_VISITOR,
                   typename Variant_CVQualAlt<t_VARIANT, 0>::type>::type> {
};

                           // ======================
                           // struct Variant_ImpUtil
                           // ======================

/// This `struct` provides a namespace for utility functions used implement
/// various operations on `bsl::variant`.
struct Variant_ImpUtil {

    /// Return a reference to the alternative with index (template
    /// parameter) `t_INDEX` in the specified `variant`.  If `t_INDEX` is
    /// not the index of the currently active alternative, throw an
    /// exception of type `bad_variant_access`.  Note that the return type
    /// must be explicitly specified.
    template <class t_RET, size_t t_INDEX, class t_VARIANT>
    static t_RET& get(t_VARIANT& variant);
    template <class t_RET, size_t t_INDEX, class t_VARIANT>
    static t_RET& get(const t_VARIANT& variant);

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    /// Invoke the specified `visitor` on the active alternative of the
    /// specified `variant`, implicitly converting the return type to the
    /// explicitly specified (template parameter) `t_RET`.  The behavior is
    /// undefined unless `variant` holds a value.  Note that the return type
    /// must be explicitly specified.  This function does not require friend
    /// access to `Variant` and has been added to `Variant_ImpUtil` for the
    /// purposes of avoiding free functions.
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET visit(t_VISITOR&& visitor, t_VARIANT&& variant);

    /// Invoke the specified `visitor` on the active alternative of the
    /// specified `variant`, implicitly converting the return type to the
    /// explicitly specified (template parameter) `t_RET`, and pass a tag
    /// representing the index of the selected alternative when invoking the
    /// visitor.  It is used internally for visitors that participate in the
    /// `variant` implementation.  The behavior is undefined unless
    /// `variant` holds a value.  This function does not require friend
    /// access to `Variant` and has been added to `Variant_ImpUtil` for the
    /// purposes of avoiding free functions.
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET visitId(t_VISITOR&& visitor, t_VARIANT&& variant);
#else
    // Lack of perfect forwarding in C++03 means overload set must be
    // different.

    /// Invoke the specified `visitor` on the active alternative of the
    /// specified `variant`, implicitly converting the return type to the
    /// explicitly specified (template parameter) `t_RET`.  The behavior is
    /// undefined if `variant` does not hold a value.  This function does
    /// not require friend access to `Variant` and has been added to
    /// `Variant_ImpUtil` for the purposes of avoiding free functions.
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET visit(t_VISITOR& visitor, t_VARIANT& variant);
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET visit(t_VISITOR& visitor, const t_VARIANT& variant);

    /// Invoke the specified `visitor` on a `bslmf::MovableRef` referring to
    /// the active alternative of the specified `variant`, implicitly
    /// converting the return type to the explicitly specified (template
    /// parameter) `t_RET`.  The behavior is undefined if `variant` does not
    /// hold a value.  This function does not require friend access to
    /// `Variant` and has been added to `Variant_ImpUtil` for the purposes
    /// of avoiding free functions.
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET moveVisit(t_VISITOR& visitor, t_VARIANT& variant);

    /// Invoke the specified `visitor` with a
    /// `bsl::in_place_index_t<t_INDEX>` tag (where `t_INDEX` is the index
    /// of the first alternative that has the same type as the active
    /// alternative of the specified `variant`) and the active alternative
    /// of `variant`, implicitly converting the return type to the
    /// explicitly specified (template parameter) `t_RET`.  The behavior is
    /// undefined if `variant` does not hold a value.  This function does
    /// not require friend access to `Variant` and has been added to
    /// `Variant_ImpUtil` for the purposes of avoiding free functions.
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET visitId(t_VISITOR& visitor, t_VARIANT& variant);
    template <class t_RET, class t_VISITOR, class t_VARIANT>
    static t_RET visitId(t_VISITOR& visitor, const t_VARIANT& variant);

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
    // Implementation notes
    // --------------------
    // In C++03, the internal visitation dispatch must call a type-based (not
    // index-based) getter.  This internal type-based getter will always be
    // passed the type of the currently active alternative, so there is no need
    // to check whether the requested alternative matches the currently active
    // alternative.  Moreover, in cases where there are alternatives of
    // identical type, 'bsl::get' cannot be passed such a type.  To solve the
    // problem, we implement an unsafe type-based getter that is used only for
    // internal visitation.

    /// Return a reference to type (template parameter) 't_RET' to the
    /// alternative at (template parameter) 't_INDEX' in the specified
    /// 'variant', regardless of whether that is the active alternative.
    /// The behavior is undefined unless the active alternative of 'variant'
    /// has the same type as the alternative with index 't_INDEX'.  Note
    /// that the return type must be explicitly specified.
    template <class t_RET, size_t t_INDEX, class t_VARIANT>
    static t_RET& unsafeGet(t_VARIANT& variant);
    template <class t_RET, size_t t_INDEX, class t_VARIANT>
    static t_RET& unsafeGet(const t_VARIANT& variant);

    /// Return a reference to const-qualified (template parameter) `t_TYPE`
    /// to the first alternative of the specified `obj` that has type
    /// `t_TYPE`, regardless of whether that is the active alternative.  The
    /// behavior is undefined unless the active alternative of `variant` is
    /// of type `t_TYPE`.  This function does not require friend access to
    /// `Variant` and has been added to `Variant_ImpUtil` for the purposes
    /// of avoiding free functions.
    template <class t_TYPE, class t_VARIANT>
    static t_TYPE& unsafeGet(t_VARIANT& obj);
    template <class t_TYPE, class t_VARIANT>
    static const t_TYPE& unsafeGet(const t_VARIANT& obj);

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    /// Return a reference to the object managed by the first member of the
    /// specified `variantUnion`.  It is the base case for the overload of
    /// `getAlternative` below.  This function does not require friend
    /// access to `Variant` and has been added to `Variant_ImpUtil` for the
    /// purposes of avoiding free functions.
    template <class t_RET, class t_VARIANT_UNION>
    static t_RET& getAlternative(
                  bsl::in_place_index_t<0>,
                  t_VARIANT_UNION&          variantUnion) BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference to the alternative with index (template
    /// parameter) `t_INDEX` in the specified `variantUnion` by recursively
    /// unravelling `variantUnion` until the desired alternative is at the
    /// head.  `t_INDEX` shall be a valid alternative index.  The behavior
    /// is undefined unless the alternative with index `t_INDEX` has the
    /// same type as the active alternative of `variantUnion`.  This
    /// function does not require friend access to `Variant` and has been
    /// added to `Variant_ImpUtil` for the purposes of avoiding free
    /// functions.
    template <class t_RET, size_t t_INDEX, class t_VARIANT_UNION>
    static t_RET& getAlternative(
            bsl::in_place_index_t<t_INDEX>,
            t_VARIANT_UNION&               variantUnion) BSLS_KEYWORD_NOEXCEPT;

    template <class t_VARIANT>
    static bool Equal(const t_VARIANT& lhs, const t_VARIANT& rhs);
    template <class t_VARIANT>
    static bool NotEqual(const t_VARIANT& lhs, const t_VARIANT& rhs);
    template <class t_VARIANT>
    static bool LessThan(const t_VARIANT& lhs, const t_VARIANT& rhs);
    template <class t_VARIANT>
    static bool GreaterThan(const t_VARIANT& lhs, const t_VARIANT& rhs);
    template <class t_VARIANT>
    static bool LessOrEqual(const t_VARIANT& lhs, const t_VARIANT& rhs);

    /// Return the result of comparing the specified `lhs` with the
    /// specified `rhs`.  The behavior is undefined unless both `lhs` and
    /// `rhs` hold the same alternative.  Note that the capitalization of
    /// the names of these methods has been chosen so that their definitions
    /// can be generated using a macro.
    template <class t_VARIANT>
    static bool GreaterOrEqual(const t_VARIANT& lhs, const t_VARIANT& rhs);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// This visitor is used to implement the explicit constructor for
    /// `bsl::variant` from `std::variant`.
    template <class t_VARIANT, class t_STD_VARIANT>
    class ConstructFromStdVisitor;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

               // ==============================================
               // class Variant_ImpUtil::ConstructFromStdVisitor
               // ==============================================

template <class t_VARIANT, class t_STD_VARIANT>
class Variant_ImpUtil::ConstructFromStdVisitor {
  private:
    // DATA
    t_VARIANT&     d_target;
    t_STD_VARIANT& d_original;

  public:
    // CREATORS

    /// Create a `ConstructFromStdVisitor` object that, when invoked, will
    /// construct the alternative of the specified `target` corresponding to
    /// the active alternative of the specified `original`.  `target` shall
    /// have the same sequence of alternative types as `original`.
    explicit ConstructFromStdVisitor(t_VARIANT&     target,
                                     t_STD_VARIANT& original);

    // ACCESSORS

    /// Construct the alternative at index (template parameter) `t_INDEX`
    /// in `d_target` from the contained value of `d_original`.  If
    /// `t_STD_VARIANT` is an lvalue reference type, the alternative will be
    /// created by copy construction, otherwise it will be created by move
    /// construction.  The behavior is undefined unless `d_original` holds a
    /// value and `d_target.index() == d_original.index()`.  Note that the
    /// unused parameter of type `t_TYPE&` refers to the alternative that
    /// will be constructed, but we do not construct directly into that
    /// object because we need to make sure that the allocator of `d_target`
    /// (if any) is used.
    template <size_t t_INDEX, class t_TYPE>
    void operator()(bsl::in_place_index_t<t_INDEX>, t_TYPE&) const;
};
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
/// This component-private struct is used to check whether an alternative
/// given by (template parameter) `t_TYPE` is a potential match for an
/// argument of a `bsl::variant` constructor or assignment operator that
/// does not take an explicitly specified alternative type.  The standard
/// allows such conversion only when the declaration `t_TYPE d_x[] = {expr};`
/// is valid, where `expr` is the (forwarded) argument expression.
template <class t_TYPE>
struct Variant_ArrayHelper {
    t_TYPE d_x[1];
};

/// This component-private metafunction checks whether a conversion from a
/// pointer type to (template parameter) `t_TYPE` is narrowing.  It is
/// instantiated only for `bool`, and its behavior depends on whether the
/// compiler has implemented P1957R2.
template <class t_TYPE, class = void>
struct Variant_CheckForP1957R2 : bsl::true_type {
};

template <class t_TYPE>
struct Variant_CheckForP1957R2<
    t_TYPE,
    bsl::void_t<decltype(Variant_ArrayHelper<t_TYPE>{{"bc"}})> >
: bsl::false_type {
};

template <class t_DEST, class t_SOURCE, class = void>
struct Variant_ConvertsWithoutNarrowing : bsl::false_type {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// (template parameter) `t_SOURCE` can be converted to (template parameter)
/// `t_DEST` without narrowing, and `bsl::false_type` otherwise.  A
/// conversion from pointer or pointer-to-member type to cv `bool` is
/// considered narrowing even if the compiler does not implement P1957R2
/// (which was adopted as a DR); however, on compilers that do not implement
/// P1957R2, we do not have the ability to check whether a user-defined
/// conversion sequence to cv `bool` would use a narrowing standard
/// conversion, so on those compilers, we permit conversion to a `t_DEST`
/// that is cv `bool` only if `t_SOURCE` is also cv `bool`, and not when
/// `t_SOURCE` is a class type.  This behavior is not expected to pose a
/// problem for users migrating from `bdlb::Variant`, because that class
/// does not support implicit conversions from an argument type to an
/// alternative type.
template <class t_DEST, class t_SOURCE>
struct Variant_ConvertsWithoutNarrowing<
    t_DEST,
    t_SOURCE,
    bsl::void_t<decltype(
        Variant_ArrayHelper<t_DEST>{{std::declval<t_SOURCE>()}})> >
: bsl::integral_constant<
      bool,
      !(!Variant_CheckForP1957R2<bool>::value &&
        bsl::is_same<bool, typename bsl::remove_cvref<t_DEST>::type>::value &&
        !bsl::is_same<bool,
                      typename bsl::remove_cvref<t_SOURCE>::type>::value)> {
};

                       // =============================
                       // struct Variant_OverloadSetImp
                       // =============================

/// This component-private metafunction computes an overload set consisting
/// of one function, named `candidate`, for each type in (template
/// parameters) `t_HEAD, t_TAIL...`, having one parameter of that type.
/// Each such function participates in overload resolution only when
/// `std::declval<t_SRC>()` is convertible to the alternative without
/// narrowing, and returns `bsl::integral_constant<t_INDEX + i>`, where `i`
/// is the zero-based index of the corresponding alternative.  Note that a
/// type that occurs multiple times in `t_HEAD, t_TAIL...` (possibly with
/// varying cv-qualifications) will only result in the generation of a
/// single candidate.  This implementation relies on expression SFINAE,
/// `decltype`, `std::declval`, and P1957R2; since these features are not
/// available in C++03, the C++03 version requires an exact match modulo
/// cv-qualification.
template <class t_SRC, size_t t_INDEX, class t_HEAD, class... t_TAIL>
struct Variant_OverloadSetImp
: Variant_OverloadSetImp<t_SRC, t_INDEX + 1, t_TAIL...> {

    using Variant_OverloadSetImp<t_SRC, t_INDEX + 1, t_TAIL...>::candidate;

    template <class t_DEST = t_HEAD>
    static typename bsl::enable_if<
        Variant_ConvertsWithoutNarrowing<t_DEST, t_SRC>::value,
        bsl::integral_constant<size_t, t_INDEX> >::type candidate(t_HEAD);
};
template <class t_SRC, size_t t_INDEX, class t_HEAD>
struct Variant_OverloadSetImp<t_SRC, t_INDEX, t_HEAD> {
    template <class t_DEST = t_HEAD>
    static typename bsl::enable_if<
        Variant_ConvertsWithoutNarrowing<t_DEST, t_SRC>::value,
        bsl::integral_constant<size_t, t_INDEX> >::type candidate(t_HEAD);
};

/// This component-private metafunction provides a member typedef `Index`
/// representing the value that should be computed by
/// `Variant_ConvertIndex`.  The primary template is instantiated when the
/// partial specialization below is not viable because no viable alternative
/// exists for the conversion or because the best match is not "unique" (see
/// the documentation of `Variant_ConvertIndex` for an explanation).
template <class t_SRC, class t_VARIANT, class = void>
struct Variant_OverloadHelper {

    typedef bsl::integral_constant<size_t, bsl::variant_npos> Index;
};

/// This partial specialization is used when a "unique" best match is found
/// for converting `std::declval<t_SRC>()` to one of (template parameters)
/// `t_HEAD, t_TAIL...`.
template <class t_SRC, class t_HEAD, class... t_TAIL>
struct Variant_OverloadHelper<
    t_SRC,
    bsl::variant<t_HEAD, t_TAIL...>,
    bsl::void_t<decltype(Variant_OverloadSetImp<t_SRC, 0, t_HEAD, t_TAIL...>::
                             candidate(std::declval<t_SRC>()))> > {
    typedef decltype(
        Variant_OverloadSetImp<t_SRC, 0, t_HEAD, t_TAIL...>::candidate(
            std::declval<t_SRC>())) Index;
};

/// This component-private metafunction computes the index of the
/// alternative in (template parameter) `t_VARIANT` that is the "unique"
/// best match for conversion from `std::declval<t_TYPE>()`, or
/// `bsl::variant_npos` if there is no such alternative.  An alternative
/// that occurs multiple times in `t_VARIANT` (possibly with varying
/// cv-qualifications) is considered to occur only once; thus, if all
/// alternatives that are tied for the best match are the same type
/// (possibly with varying cv-qualifications), the result is the lowest
/// index at which that type (with any cv-qualification) occurs.
template <class t_TYPE, class t_VARIANT>
struct Variant_ConvertIndex
: Variant_OverloadHelper<t_TYPE, t_VARIANT>::Index {
};

#else  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This component-private metafunction computes `t_INDEX + i`, where `i` is
/// the zero-based index of the first occurrence of (template parameter)
/// `t_TYPE` in (template parameters) `t_HEAD, t_TAIL...`, or
/// `bsl::variant_npos` if not found.  An alternative is considered to match
/// `t_TYPE` if the two types are the same modulo cv-qualification.  This
/// metafunction is used to implement `Variant_ConvertIndex`.
template <size_t t_INDEX,
          class t_TYPE,
          class t_HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... t_TAIL>
struct Variant_ConvertToIndexImpl;

/// Specialization for the case when t_ARGS is empty, i.e. the type wasn't
/// found in the given list of template type arguments.
template <size_t t_INDEX, class t_TYPE>
struct Variant_ConvertToIndexImpl<t_INDEX, t_TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, bsl::variant_npos> {
};
template <size_t t_INDEX, class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_ConvertToIndexImpl<t_INDEX, t_TYPE, t_HEAD, t_TAIL...>
: bsl::conditional<
      bsl::is_same<typename bsl::remove_cv<t_TYPE>::type,
                   typename bsl::remove_cv<t_HEAD>::type>::value,
      bsl::integral_constant<size_t, t_INDEX>,
      Variant_ConvertToIndexImpl<t_INDEX + 1, t_TYPE, t_TAIL...> >::type {
};

/// This component-private metafunction computes the zero-based index of the
/// first occurrence of (template parameter) `t_TYPE` in the list of
/// alternatives of (template parameter) `t_VARIANT`, or `bsl::variant_npos`
/// if not found.  An alternative is considered to match `t_TYPE` if the two
/// types are the same modulo cv-qualification.  The primary template (used
/// when `t_VARIANT` is not a `bsl::variant`) is not defined.  This
/// metafunction is used to determine the alternative to be constructed or
/// assigned to from a given argument type when the alternative type is not
/// explicitly specified.  Note that in C++11 and later,
/// `Variant_ConvertIndex` allows implicit conversions, but this behavior
/// cannot be implemented in C++03.
template <class t_TYPE, class t_VARIANT>
struct Variant_ConvertIndex;

template <class t_TYPE, class t_HEAD, class... t_TAIL>
struct Variant_ConvertIndex<t_TYPE, bsl::variant<t_HEAD, t_TAIL...> >
: Variant_ConvertToIndexImpl<
      0,
      typename bslmf::MovableRefUtil::RemoveReference<t_TYPE>::type,
      t_HEAD,
      t_TAIL...> {
};

#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

/// This component-private metafunction is derived from `bsl::true_type` if
/// there is a unique best match alternative in (template parameter)
/// `t_VARIANT` for 'std::declval<t_TYPE>() and that alternative is
/// constructible from `std::declval<t_TYPE>(), and `bsl::false_type'
/// otherwise.
template <class t_VARIANT,
          class t_TYPE,
          size_t t_INDEX = BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE, t_VARIANT)>
struct Variant_IsAlternativeConstructibleFrom
: bsl::integral_constant<
      bool,
      Variant_IsConstructible<
          typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type,
          t_TYPE>::value &&
          Variant_HasUniqueCVType<BSLSTL_VARIANT_CONVERT_TYPE_OF(t_TYPE,
                                                                 t_VARIANT),
                                  t_VARIANT>::value> {
};

template <class t_VARIANT, class t_TYPE>
struct Variant_IsAlternativeConstructibleFrom<t_VARIANT,
                                              t_TYPE,
                                              bsl::variant_npos>
: bsl::false_type {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// there is a unique best match alternative in (template parameter)
/// `t_VARIANT` for `std::declval<t_TYPE>()` and that alternative is both
/// constructible and assignable from `std::declval<t_TYPE>()`, and
/// `bsl::false_type` otherwise.
template <class t_VARIANT,
          class t_TYPE,
          size_t t_INDEX = BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE, t_VARIANT)>
struct Variant_isAlternativeAssignableFrom
: bsl::integral_constant<
      bool,
      Variant_IsAssignable<
          typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type&,
          t_TYPE>::value &&
          Variant_HasUniqueCVType<BSLSTL_VARIANT_CONVERT_TYPE_OF(t_TYPE,
                                                                 t_VARIANT),
                                  t_VARIANT>::value> {
};

template <class t_VARIANT, class t_TYPE>
struct Variant_isAlternativeAssignableFrom<t_VARIANT,
                                           t_TYPE,
                                           bsl::variant_npos>
: bsl::false_type {
};

/// This component-private metafunction declares a member `type` that is an
/// alias to the `std::variant` type corresponding to `t_TYPE` if one
/// exists, and `void` otherwise.
template <class t_TYPE>
struct Variant_CorrespondingStdVariant {
    typedef void type;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_HEAD, class... t_TAIL>
struct Variant_CorrespondingStdVariant<bsl::variant<t_HEAD, t_TAIL...>> {
    typedef std::variant<t_HEAD, t_TAIL...> type;
};
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

/// This component-private metafunction is derived from `bsl::true_type` if
/// (template parameter) `t_TYPE` is neither a tag type, nor the type
/// (template parameter) `t_VARIANT` (modulo cv-qualification), nor the
/// corresponding (possibly cv-qualified) `std::variant` type, there is a
/// unique best match alternative in `t_VARIANT` for
/// `std::declval<t_TYPE>()`, and that alternative is constructible from
/// `std::declval<t_TYPE>()`; otherwise, this metafunction is derived from
/// `bsl::false_type`.
template <class t_VARIANT, class t_TYPE>
struct Variant_ConstructsFromType
: bsl::integral_constant<
      bool,
      !bsl::is_same<typename bsl::remove_cvref<t_TYPE>::type,
                    t_VARIANT>::value &&
          !Variant_IsTag<typename bsl::remove_cvref<t_TYPE>::type>::value &&
          !bsl::is_same<
              typename Variant_CorrespondingStdVariant<t_VARIANT>::type,
              typename bsl::remove_cvref<t_TYPE>::type>::value &&
          Variant_IsAlternativeConstructibleFrom<t_VARIANT, t_TYPE>::value> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// (template parameter) `t_TYPE` is neither a tag type nor the type
/// (template parameter) `t_VARIANT` (modulo cv-qualification), there is a
/// unique best match alternative in `t_VARIANT` for
/// `std::declval<t_TYPE>()`, and that alternative is constructible and
/// assignable from `std::declval<t_TYPE>()`; otherwise, this metafunction
/// is derived from `bsl::false_type`.
template <class t_VARIANT, class t_TYPE>
struct Variant_AssignsFromType
: bsl::integral_constant<
      bool,
      !bsl::is_same<typename bsl::remove_cvref<t_TYPE>::type,
                    t_VARIANT>::value &&
          Variant_IsAlternativeConstructibleFrom<t_VARIANT, t_TYPE>::value &&
          Variant_isAlternativeAssignableFrom<t_VARIANT, t_TYPE>::value> {
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
/// This component-private constexpr variable template is `true` if
/// `t_VARIANT` is a specialization of `bsl::variant`, and `t_STD_VARIANT`
/// is the `std::variant` type (possibly with added cvref-qualification)
/// that has the same sequence of alternatives, and each alternative of
/// `t_VARIANT` is constructible from the corresponding alternative of
/// `t_STD_VARIANT` (with cv-qualification and value category matching that
/// of `t_STD_VARIANT`).  Note that we use `bsl::conjunction_v` for its
/// short-circuiting properties: if `t_STD_VARIANT` isn`t a `std::variant`
/// (for example, if it`s a `bsl::variant`) we avoid recursing into the
/// constraints of `bsl::variant`.
template <class t_VARIANT, class t_STD_VARIANT>
constexpr bool variant_constructsFromStd =
  bsl::conjunction_v<
      bsl::is_same<typename Variant_CorrespondingStdVariant<t_VARIANT>::type,
                   bsl::remove_cvref_t<t_STD_VARIANT>>,
      std::is_constructible<bsl::remove_cvref_t<t_STD_VARIANT>,
                            t_STD_VARIANT>>;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
// The following component-private machinery allows for conditionally deleting
// special member functions.

/// This component-private class has deleted copy constructor if (template
/// parameter) `t_ISCOPYCONSTRUCTIBLE` is `false`.  All other special member
/// functions are defaulted.
template <bool t_ISCOPYCONSTRUCTIBLE>
struct Variant_CopyConstructorBase {
};
template <>
struct Variant_CopyConstructorBase<false> {
    Variant_CopyConstructorBase()                                   = default;
    Variant_CopyConstructorBase(const Variant_CopyConstructorBase&) = delete;
    Variant_CopyConstructorBase(Variant_CopyConstructorBase&&)      = default;
    Variant_CopyConstructorBase&
    operator=(const Variant_CopyConstructorBase&) = default;
    Variant_CopyConstructorBase&
    operator=(Variant_CopyConstructorBase&&) = default;
};

/// This component-private class has deleted move constructor if (template
/// parameter) `t_ISMOVECONSTRUCTIBLE` is `false`.  All other special member
/// functions are defaulted.
template <bool t_ISCOPYCONSTRUCTIBLE, bool t_ISMOVECONSTRUCTIBLE>
struct Variant_MoveConstructorBase
: Variant_CopyConstructorBase<t_ISCOPYCONSTRUCTIBLE> {
};
template <bool t_ISCOPYCONSTRUCTIBLE>
struct Variant_MoveConstructorBase<t_ISCOPYCONSTRUCTIBLE, false>
: Variant_CopyConstructorBase<t_ISCOPYCONSTRUCTIBLE> {
    Variant_MoveConstructorBase()                                   = default;
    Variant_MoveConstructorBase(const Variant_MoveConstructorBase&) = default;
    Variant_MoveConstructorBase(Variant_MoveConstructorBase&&)      = delete;
    Variant_MoveConstructorBase&
    operator=(const Variant_MoveConstructorBase&) = default;
    Variant_MoveConstructorBase&
    operator=(Variant_MoveConstructorBase&&) = default;
};

/// This component-private class has deleted copy assignment operator if
/// (template parameter) `t_ISCOPYASSIGNABLE` is `false`.  All other special
/// member functions are defaulted.
template <bool t_ISCOPYCONSTRUCTIBLE,
          bool t_ISMOVECONSTRUCTIBLE,
          bool t_ISCOPYASSIGNABLE>
struct Variant_CopyAssignBase
: Variant_MoveConstructorBase<t_ISCOPYCONSTRUCTIBLE, t_ISMOVECONSTRUCTIBLE> {
};
template <bool t_ISCOPYCONSTRUCTIBLE, bool t_ISMOVECONSTRUCTIBLE>
struct Variant_CopyAssignBase<t_ISCOPYCONSTRUCTIBLE,
                              t_ISMOVECONSTRUCTIBLE,
                              false>
: Variant_MoveConstructorBase<t_ISCOPYCONSTRUCTIBLE, t_ISMOVECONSTRUCTIBLE> {
    Variant_CopyAssignBase()                                         = default;
    Variant_CopyAssignBase(const Variant_CopyAssignBase&)            = default;
    Variant_CopyAssignBase(Variant_CopyAssignBase&&)                 = default;
    Variant_CopyAssignBase& operator=(const Variant_CopyAssignBase&) = delete;
    Variant_CopyAssignBase& operator=(Variant_CopyAssignBase&&)      = default;
};

/// This component-private class has deleted move assignment operator if
/// (template parameter) `t_ISMOVEASSIGNABLE` is `false`.  All other special
/// member functions are defaulted.
template <bool t_ISCOPYCONSTRUCTIBLE,
          bool t_ISMOVECONSTRUCTIBLE,
          bool t_ISCOPYASSIGNABLE,
          bool t_ISMOVEASSIGNABLE>
struct Variant_MoveAssignBase : Variant_CopyAssignBase<t_ISCOPYCONSTRUCTIBLE,
                                                       t_ISMOVECONSTRUCTIBLE,
                                                       t_ISCOPYASSIGNABLE> {
};
template <bool t_ISCOPYCONSTRUCTIBLE,
          bool t_ISMOVECONSTRUCTIBLE,
          bool t_ISCOPYASSIGNABLE>
struct Variant_MoveAssignBase<t_ISCOPYCONSTRUCTIBLE,
                              t_ISMOVECONSTRUCTIBLE,
                              t_ISCOPYASSIGNABLE,
                              false>
: Variant_CopyAssignBase<t_ISCOPYCONSTRUCTIBLE,
                         t_ISMOVECONSTRUCTIBLE,
                         t_ISCOPYASSIGNABLE> {
    Variant_MoveAssignBase()                                         = default;
    Variant_MoveAssignBase(const Variant_MoveAssignBase&)            = default;
    Variant_MoveAssignBase(Variant_MoveAssignBase&&)                 = default;
    Variant_MoveAssignBase& operator=(const Variant_MoveAssignBase&) = default;
    Variant_MoveAssignBase& operator=(Variant_MoveAssignBase&&)      = delete;
};

/// This component-private class has special member functions that are
/// either deleted or defaulted according to the specified template
/// parameters.
template <bool t_ISCOPYCONSTRUCTIBLE,
          bool t_ISMOVECONSTRUCTIBLE,
          bool t_ISCOPYASSIGNABLE,
          bool t_ISMOVEASSIGNABLE>
struct Variant_SMFBase : Variant_MoveAssignBase<t_ISCOPYCONSTRUCTIBLE,
                                                t_ISMOVECONSTRUCTIBLE,
                                                t_ISCOPYASSIGNABLE,
                                                t_ISMOVEASSIGNABLE> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// all template parameters are copy constructible, and `bsl::false_type`
/// otherwise.
template <class t_HEAD, class... t_TAIL>
struct Variant_IsCopyConstructibleAll
: bsl::integral_constant<
      bool,
      std::is_copy_constructible<t_HEAD>::value &&
          Variant_IsCopyConstructibleAll<t_TAIL...>::value> {
};

template <class t_HEAD>
struct Variant_IsCopyConstructibleAll<t_HEAD>
: std::is_copy_constructible<t_HEAD> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// all template parameters are move constructible, and `bsl::false_type`
/// otherwise.
template <class t_HEAD, class... t_TAIL>
struct Variant_IsMoveConstructibleAll
: bsl::integral_constant<
      bool,
      std::is_move_constructible<t_HEAD>::value &&
          Variant_IsMoveConstructibleAll<t_TAIL...>::value> {
};

template <class t_HEAD>
struct Variant_IsMoveConstructibleAll<t_HEAD>
: std::is_move_constructible<t_HEAD> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// all template parameters are copy assignable, and `bsl::false_type`
/// otherwise.
template <class t_HEAD, class... t_TAIL>
struct Variant_IsCopyAssignableAll
: bsl::integral_constant<bool,
                         std::is_copy_assignable<t_HEAD>::value &&
                             Variant_IsCopyAssignableAll<t_TAIL...>::value> {
};

template <class t_HEAD>
struct Variant_IsCopyAssignableAll<t_HEAD> : std::is_copy_assignable<t_HEAD> {
};

/// This component-private metafunction is derived from `bsl::true_type` if
/// all template parameters are move assignable, and `bsl::false_type`
/// otherwise.
template <class t_HEAD, class... t_TAIL>
struct Variant_IsMoveAssignableAll
: bsl::integral_constant<bool,
                         std::is_move_assignable<t_HEAD>::value &&
                             Variant_IsMoveAssignableAll<t_TAIL...>::value> {
};

template <class t_HEAD>
struct Variant_IsMoveAssignableAll<t_HEAD> : std::is_move_assignable<t_HEAD> {
};
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_RET, class t_VISITOR, class t_VARIANT, size_t t_INDEX>
struct Variant_Function {
    /// This component-private function invokes the specified `visitor` with
    /// the active alternative object of the specified `variant` and
    /// implicitly converts the return type to (template parameter) `t_RET`.
    /// The pre-C++17 implementation is limited to invocations of the form
    /// `visitor(variant)`.  The behavior is undefined unless (template
    /// parameter) `t_INDEX` is the index of the active alternative.
    static t_RET functionImpl(t_VISITOR&& visitor, t_VARIANT&& variant)
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        return std::invoke(
            std::forward<t_VISITOR>(visitor),
            bsl::get<t_INDEX>(std::forward<t_VARIANT>(variant)));
#else
        return std::forward<t_VISITOR>(visitor)(
            bsl::get<t_INDEX>(std::forward<t_VARIANT>(variant)));
#endif
    }
};

/// This partial specialization is used when `t_RET` is `void`.  The visitor
/// is invoked and its return value, if any, is ignored.  This partial
/// specialization is necessary because a `void`-returning function cannot
/// contain a `return` statement with a non-`void` operand.
template <class t_VISITOR, class t_VARIANT, size_t t_INDEX>
struct Variant_Function<void, t_VISITOR, t_VARIANT, t_INDEX> {

    static void functionImpl(t_VISITOR&& visitor, t_VARIANT&& variant)
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        std::invoke(std::forward<t_VISITOR>(visitor),
                    bsl::get<t_INDEX>(std::forward<t_VARIANT>(variant)));
#else
        std::forward<t_VISITOR>(visitor)(
            bsl::get<t_INDEX>(std::forward<t_VARIANT>(variant)));
#endif
    }
};

template <class t_RET, class t_VISITOR, class t_VARIANT, size_t t_INDEX>
struct Variant_FunctionId {
    /// This component-private function invokes the specified `visitor` with
    /// a `bsl::in_place_index_t` tag representing the value of (template
    /// parameter) `t_INDEX` and the active alternative object of the
    /// specified `variant`, implicitly converting the return type to
    /// (template parameter) `t_RET`.  The behavior is undefined unless
    /// `t_INDEX` is the index of the active alternative.
    static t_RET functionImpl(t_VISITOR&& visitor, t_VARIANT&& variant)
    {
        return visitor(bsl::in_place_index_t<t_INDEX>(),
                       bsl::get<t_INDEX>(std::forward<t_VARIANT>(variant)));
    }
};

/// This component-private struct computes an array in which element `i` is
/// a pointer to
/// `Variant_Function<t_RET, t_VISITOR, t_VARIANT, i>::functionImpl`,
/// defined above as a function that invokes a `t_VISITOR` with alternative
/// `i` of `t_VARIANT` and implicitly converts the return type to `t_RET`.
template <class t_RET, class t_VISITOR, class t_VARIANT, class t_DUMMY>
struct Variant_VTable;

/// In order to allow for perfect forwarding, both t_VISITOR and t_VARIANT
/// must be of reference type.  If they are not, something went wrong.
template <class t_RET, class t_VISITOR, class t_VARIANT, size_t... t_INDICES>
struct Variant_VTable<t_RET,
                      t_VISITOR,
                      t_VARIANT,
                      bslmf::IntegerSequence<std::size_t, t_INDICES...> > {
    BSLMF_ASSERT(bsl::is_reference<t_VISITOR>::value);
    BSLMF_ASSERT(bsl::is_reference<t_VARIANT>::value);

    typedef t_RET (*FuncPtr)(t_VISITOR, t_VARIANT);

    static BSLS_KEYWORD_CONSTEXPR FuncPtr s_map[sizeof...(t_INDICES)] = {
        &(Variant_Function<t_RET, t_VISITOR, t_VARIANT, t_INDICES>::
              functionImpl)...};
};

/// This component-private struct computes an array in which element `i` is
/// a pointer to
/// `Variant_FunctionId<t_RET, t_VISITOR, t_VARIANT, i>::functionImpl`,
/// defined above as a function that invokes a `t_VISITOR` with a tag
/// representing `i` and alternative `i` of `t_VARIANT`, implicitly
/// converting the return type to `t_RET`.  Implementation note: The
/// constexpr static maps of pointers defined by `Variant_VTable` and this
/// class have deliberately been defined in two different classes as having
/// them in the same class caused issues with Clang and Microsoft Visual
/// C++.
template <class t_RET, class t_VISITOR, class t_VARIANT, class t_DUMMY>
struct Variant_VTableId;

/// In order to allow for perfect forwarding, both t_VISITOR and t_VARIANT
/// must be of reference type.  If they are not, something went wrong.
template <class t_RET, class t_VISITOR, class t_VARIANT, size_t... t_INDICES>
struct Variant_VTableId<t_RET,
                        t_VISITOR,
                        t_VARIANT,
                        bslmf::IntegerSequence<std::size_t, t_INDICES...> > {
    BSLMF_ASSERT(bsl::is_reference<t_VISITOR>::value);
    BSLMF_ASSERT(bsl::is_reference<t_VARIANT>::value);

    typedef t_RET (*FuncPtr)(t_VISITOR, t_VARIANT);

    static BSLS_KEYWORD_CONSTEXPR FuncPtr s_mapId[sizeof...(t_INDICES)] = {
        &(Variant_FunctionId<t_RET, t_VISITOR, t_VARIANT, t_INDICES>::
              functionImpl)...};
};

// inline definitions of component-private function pointer maps
template <class t_RET, class t_VISITOR, class t_VARIANT, size_t... t_INDICES>
BSLS_KEYWORD_CONSTEXPR typename Variant_VTable<
    t_RET,
    t_VISITOR,
    t_VARIANT,
    bslmf::IntegerSequence<std::size_t, t_INDICES...> >::FuncPtr
    Variant_VTable<t_RET,
                   t_VISITOR,
                   t_VARIANT,
                   bslmf::IntegerSequence<std::size_t, t_INDICES...> >::s_map
        [sizeof...(t_INDICES)];

template <class t_RET, class t_VISITOR, class t_VARIANT, size_t... t_INDICES>
BSLS_KEYWORD_CONSTEXPR typename Variant_VTableId<
    t_RET,
    t_VISITOR,
    t_VARIANT,
    bslmf::IntegerSequence<std::size_t, t_INDICES...> >::FuncPtr
    Variant_VTableId<t_RET,
                     t_VISITOR,
                     t_VARIANT,
                     bslmf::IntegerSequence<std::size_t, t_INDICES...> >::
        s_mapId[sizeof...(t_INDICES)];

#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class t_RET, class t_VISITOR, class t_VARIANT, class t_ALT_TYPE>
struct Variant_Function {
    /// This component-private function invokes the specified `visitor` on
    /// the contained value of the specified `variant`, implicitly
    /// converting the return type to (template parameter) `t_RET`.  The
    /// behavior is undefined if (template parameter) `t_ALT_TYPE` is not
    /// the type of the active alternative.
    static t_RET functionImpl(t_VISITOR& visitor, t_VARIANT& variant)
    {
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
        return visitor(ImpUtil::unsafeGet<t_ALT_TYPE>(variant));
    }
};

/// This partial specialization is used when `t_RET` is `void`.  The visitor
/// is invoked and its return value, if any, is ignored.  This partial
/// specialization is necessary because a `void`-returning function cannot
/// contain a `return` statement with a non-`void` operand.
template <class t_VISITOR, class t_VARIANT, class t_ALT_TYPE>
struct Variant_Function<void, t_VISITOR, t_VARIANT, t_ALT_TYPE> {

    static void functionImpl(t_VISITOR& visitor, t_VARIANT& variant)
    {
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
        visitor(ImpUtil::unsafeGet<t_ALT_TYPE>(variant));
    }
};

template <class t_RET, class t_VISITOR, class t_VARIANT, class t_ALT_TYPE>
struct Variant_MoveFunction {
    /// This component-private function invokes the specified `visitor` with
    /// a `bslmf::MovableRef` referring to the active alternative object of
    /// the specified `variant`, implicitly converting the return type to
    /// (template parameter) `t_RET`.  The behavior is undefined unless
    /// `variant` has an active alternative of type (template parameter)
    /// `t_ALT_TYPE`.
    static t_RET functionImpl(t_VISITOR& visitor, t_VARIANT& variant)
    {
        typedef BloombergLP::bslmf::MovableRefUtil   MoveUtil;
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

        return visitor(
                      MoveUtil::move(ImpUtil::unsafeGet<t_ALT_TYPE>(variant)));
    }
};

/// This partial specialization is used when `t_RET` is `void`.  The visitor
/// is invoked and its return value, if any, is ignored.  This partial
/// specialization is necessary because a `void`-returning function cannot
/// contain a `return` statement with a non-`void` operand.
template <class t_VISITOR, class t_VARIANT, class t_ALT_TYPE>
struct Variant_MoveFunction<void, t_VISITOR, t_VARIANT, t_ALT_TYPE> {

    static void functionImpl(t_VISITOR& visitor, t_VARIANT& variant)
    {
        typedef BloombergLP::bslmf::MovableRefUtil   MoveUtil;
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

        visitor(MoveUtil::move(ImpUtil::unsafeGet<t_ALT_TYPE>(variant)));
    }
};

template <class t_RET, class t_VISITOR, class t_VARIANT, class t_ALT_TYPE>
struct Variant_FunctionId {
    /// This component-private function invokes the specified `visitor` with
    /// a `bsl::in_place_index_t` tag representing the index of the first
    /// occurrence of (template parameter) `t_ALT_TYPE` in the specified
    /// `variant` and the active alternative object of `variant`, implicitly
    /// converting the return type to (template parameter) `t_RET`.  The
    /// behavior is undefined unless `variant` has an active alternative of
    /// type `t_ALT_TYPE`.  Note that the index passed to the visitor might
    /// not be the index of the active alternative, if that alternative
    /// occurs multiple times in `t_VARIANT`.
    static t_RET functionImpl(t_VISITOR& visitor, t_VARIANT& variant)
    {
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
        static const size_t t_INDEX = BSLSTL_VARIANT_INDEX_OF(
            t_ALT_TYPE, typename bsl::remove_cv<t_VARIANT>::type);

        return visitor(bsl::in_place_index_t<t_INDEX>(),
                       ImpUtil::unsafeGet<t_ALT_TYPE>(variant));
    }
};

/// This component-private struct computes arrays in which element `i` is a
/// pointer to one of the `functionImpl` functions defined above, invoking
/// an lvalue of type (template parameter) `t_VISITOR` with alternative `i`
/// of (template parameter) `t_VARIANT` and implicitly converting the return
/// type to `t_RET`.  The primary template (used when `t_VARIANT` is not a
/// `bsl::variant`) is not defined.  Note that the `t_UNUSED` template
/// parameter is required due to limitations of `sim_cpp11_features.pl`.
template <class t_RET, class t_VISITOR, class t_VARIANT, class... t_UNUSED>
struct Variant_VTable;

/// This partial specialization is used when the `bsl::variant` is not `const`.
template <class t_RET, class t_VISITOR, class t_HEAD, class... t_TAIL>
struct Variant_VTable<t_RET, t_VISITOR, bsl::variant<t_HEAD, t_TAIL...> > {

    typedef t_RET (*FuncPtr)(t_VISITOR&, bsl::variant<t_HEAD, t_TAIL...>&);

    typedef bsl::variant<t_HEAD, t_TAIL...> Variant;

    /// Return a pointer to the first element of an array of function pointers
    /// that invoke the visitor with an lvalue referring to the corresponding
    /// alternative.
    static const FuncPtr *map()
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_Function<t_RET, t_VISITOR, Variant, t_HEAD>::
                  functionImpl),
            &(Variant_Function<t_RET, t_VISITOR, Variant, t_TAIL>::
                  functionImpl)...};
        return s_mapArray;
    }

    /// Return a pointer to the first element of an array of function pointers
    /// that invoke the visitor with a `bslmf::MovableRef` referring to the
    /// corresponding alternative.
    static const FuncPtr *moveMap()
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_MoveFunction<t_RET, t_VISITOR, Variant, t_HEAD>::
                  functionImpl),
            &(Variant_MoveFunction<t_RET, t_VISITOR, Variant, t_TAIL>::
                  functionImpl)...};
        return s_mapArray;
    }

    /// Return a pointer to the first element of an array of function pointers
    /// where element `i` invokes the visitor with a tag representing the index
    /// of the first alternative whose type is the same as that of alternative
    /// `i`, and an lvalue referring to alternative `i`.
    static const FuncPtr *mapId()
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_FunctionId<t_RET, t_VISITOR, Variant, t_HEAD>::
                  functionImpl),
            &(Variant_FunctionId<t_RET, t_VISITOR, Variant, t_TAIL>::
                  functionImpl)...};
        return s_mapArray;
    }
};

/// This partial specialization is used when the 'bsl::variant' is 'const'.
template <class t_RET, class t_VISITOR, class t_HEAD, class... t_TAIL>
struct Variant_VTable<t_RET,
                      t_VISITOR,
                      const bsl::variant<t_HEAD, t_TAIL...> > {

    typedef t_RET (*FuncPtr)(t_VISITOR&,
                             const bsl::variant<t_HEAD, t_TAIL...>&);

    typedef const bsl::variant<t_HEAD, t_TAIL...> Variant;

    /// Return a pointer to the first element of an array of function pointers
    /// that invoke the visitor with an lvalue referring to the corresponding
    /// alternative.
    static const FuncPtr *map()
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_Function<t_RET, t_VISITOR, Variant, t_HEAD>::
                  functionImpl),
            &(Variant_Function<t_RET, t_VISITOR, Variant, t_TAIL>::
                  functionImpl)...};
        return s_mapArray;
    }

    /// Return a pointer to the first element of an array of function pointers
    /// where element `i` invokes the visitor with a tag representing the index
    /// of the first alternative whose type is the same as that of alternative
    /// `i`, and an lvalue referring to alternative `i`.
    static const FuncPtr *mapId()
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_FunctionId<t_RET, t_VISITOR, Variant, t_HEAD>::
                  functionImpl),
            &(Variant_FunctionId<t_RET, t_VISITOR, Variant, t_TAIL>::
                  functionImpl)...};
        return s_mapArray;
    }
};

#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
/// Return the result of invoking the specified `visitor` with the currently
/// active alternative of the specified `variant`, implicitly converting the
/// result to the explicitly specified (template parameter) `t_RET`.  If
/// `variant` does not hold a value, throw an exception of type
/// `bsl::bad_variant_access`.  Note that unlike the `visit` overload below
/// that deduces its return type, this overload does not require the visitor
/// to yield the exact same type for each alternative, but only requires
/// each such type to be implicitly convertible to `t_RET`.  This function
/// differs from the standard in the following ways:
/// * only one variant is supported
/// * constexpr is not implemented
/// * before C++17, only `t_VISITOR(ALTERNATIVE)` form of visitation is
///   supported; cases where the visitor is a pointer to member are not
///   supported.
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET visit(t_VISITOR&& visitor, t_VARIANT&& variant)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return ImpUtil::visit<t_RET>(std::forward<t_VISITOR>(visitor),
                                 std::forward<t_VARIANT>(variant));
}

/// Return the result of invoking the specified `visitor` with the currently
/// active alternative of the specified `variant`.  If `variant` does not
/// hold a value, throw an exception of type `bsl::bad_variant_access`. For
/// each alternative, invocation of the visitor with that alternative shall
/// be of the same type and value category.  This function differs from the
/// standard in the following ways:
/// * only one variant is supported
/// * constexpr is not implemented
/// * before C++17, only `t_VISITOR(ALTERNATIVE)` form of visitation is
///   supported; cases where the visitor is a pointer to member are not
///   supported.
template <class t_VISITOR, class t_VARIANT>
typename bsl::invoke_result<
    t_VISITOR,
    typename BloombergLP::bslstl::Variant_CVQualAlt<t_VARIANT, 0>::type>::type
visit(t_VISITOR&& visitor, t_VARIANT&& variant)
{
    typedef typename bsl::invoke_result<
        t_VISITOR&&,
        typename BloombergLP::bslstl::Variant_CVQualAlt<t_VARIANT&&,
                                                        0>::type>::type RET;
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    static_assert(
        BloombergLP::bslstl::
            Variant_IsSameReturnType<RET, t_VISITOR&&, t_VARIANT&&>::value,
        "The value type and category of invoking the visitor with "
        "every alternative is not the same");
    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return ImpUtil::visit<RET>(std::forward<t_VISITOR>(visitor),
                               std::forward<t_VARIANT>(variant));
}

/// Return the result of invoking the specified `visitor` with the currently
/// active alternative of the specified `variant`, implicitly converting the
/// result to the explicitly specified (template parameter) `t_RET`.  If
/// `variant` does not hold a value, throw an exception of type
/// `bsl::bad_variant_access`.  This function is provided in all language
/// versions for compatibility with the C++03 version (see below), but in new
/// code in C++11 and later, `bsl::visit` should be used instead of this
/// function.
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET visitR(t_VISITOR& visitor, t_VARIANT&& variant)
{
    return visit<t_RET>(visitor, std::forward<t_VARIANT>(variant));
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION

/// Return the result of invoking the specified `visitor` with the currently
/// active alternative of the specified `variant`, implicitly converting the
/// result to the explicitly specified (template parameter) `t_RET`.  If
/// `variant` does not hold a value, throw an exception of type
/// `bsl::bad_variant_access`.  Unlike `visit`, which deduces its return type,
/// this function does not require the visitor to yield the exact same type for
/// each alternative, but only requires each such type to be implicitly
/// convertible to `t_RET`.  Implementation note: Due to limitations of the
/// type deduction facility in C++03, this function can not share the same name
/// as the deduced type `visit` because it is not possible to discern between
/// invocation of non deduced return type `visit` where the return type
/// parameter is explicitly specified and invocation of deduced return type
/// `visit` where the visitor type parameter is explicitly specified.
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET visitR(t_VISITOR& visitor, t_VARIANT& variant)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return ImpUtil::visit<t_RET>(visitor, variant);
}

/// Return the result of invoking the specified `visitor` with a
/// `bslmf::MovableRef` referring to the currently active alternative of the
/// specified `variant`, implicitly converting the result to the explicitly
/// specified (template parameter) `t_RET`.  If `variant` does not hold a
/// value, throw an exception of type `bsl::bad_variant_access`.  Unlike
/// `visit`, which deduces its return type, this function does not require the
/// visitor to yield the exact same type for each alternative, but only
/// requires each such type to be implicitly convertible to `t_RET`.
/// Implementation note: Due to limitations of the type deduction facility in
/// C++03, this function can not share the same name as the deduced type
/// `visit` because it is not possible to discern between invocation of non
/// deduced return type `visit` where the return type parameter is explicitly
/// specified and invocation of deduced return type `visit` where the visitor
/// type parameter is explicitly specified.
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET visitR(t_VISITOR&                                visitor,
             BloombergLP::bslmf::MovableRef<t_VARIANT> variant)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    t_VARIANT& lvalue = variant;
    if (lvalue.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return ImpUtil::moveVisit<t_RET>(visitor, lvalue);
}

/// Return the result of invoking the specified `visitor` with the currently
/// active alternative of the specified `variant`.  If `variant` does not hold
/// a value, throw an exception of type `bsl::bad_variant_access`.  For each
/// alternative, invocation of the visitor with that alternative shall be of
/// the same type and value category.
template <class t_VISITOR, class t_VARIANT>
typename bsl::invoke_result<
    t_VISITOR&,
    typename bsl::variant_alternative<0, t_VARIANT>::type&>::type
visit(t_VISITOR& visitor, t_VARIANT& variant)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    typedef typename bsl::invoke_result<
        t_VISITOR&,
        typename bsl::variant_alternative<0, t_VARIANT>::type&>::type Ret;

    BSLMF_ASSERT(
        (BloombergLP::bslstl::
             Variant_IsSameReturnType<Ret, t_VISITOR, t_VARIANT&>::value));

    return ImpUtil::visit<Ret>(visitor, variant);
}

/// Return the result of invoking the specified `visitor` with a
/// `bslmf::MovableRef` referring to the currently active alternative of the
/// specified `variant`.  If `variant` does not hold a value, throw an
/// exception of type `bsl::bad_variant_access`.  For each alternative,
/// invocation of the visitor with that alternative shall be of the same type
/// and value category.
template <class t_VISITOR, class t_VARIANT>
typename bsl::invoke_result<
    t_VISITOR&,
    BloombergLP::bslmf::MovableRef<
        typename bsl::variant_alternative<0, t_VARIANT>::type> >::type
visit(t_VISITOR& visitor, BloombergLP::bslmf::MovableRef<t_VARIANT> variant)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    t_VARIANT& lvalue = variant;
    if (lvalue.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    typedef typename bsl::invoke_result<
        t_VISITOR&,
        BloombergLP::bslmf::MovableRef<
            typename bsl::variant_alternative<0, t_VARIANT>::type> >::type Ret;

    BSLMF_ASSERT((BloombergLP::bslstl::Variant_IsSameReturnType<
                  Ret,
                  t_VISITOR,
                  BloombergLP::bslmf::MovableRef<t_VARIANT> >::value));

    return ImpUtil::moveVisit<Ret>(visitor, lvalue);
}

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                          // ========================
                          // class Variant_NoSuchType
                          // ========================

/// This component-private trivial tag type is used to distinguish between
/// arguments passed by a user, and an `enable_if` argument.  It is not default
/// constructible so the following construction never invokes a constrained
/// single parameter constructor:
/// ```
///  struct SomeType
///  {
///      SomeType(int, const std::string &s = "default"){}
///  };
///
///  variant<SomeType> o(1, {});
/// ```
struct Variant_NoSuchType {

    // CREATORS

    /// Create a `Variant_NoSuchType` object.
    explicit BSLS_KEYWORD_CONSTEXPR Variant_NoSuchType(
                                                    int) BSLS_KEYWORD_NOEXCEPT;
};

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
/// This component-private class manages an object of type (template parameter)
/// `t_TYPE` that is stored in a `variant` object.  Note that `t_TYPE` may be
/// const-qualified, in which case the internally stored object is not `const`,
/// but the interface of this class prevents modification thereof.
template <class t_TYPE>
class Variant_DataImp {

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<t_TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;  // in-place 't_TYPE' object

  public:
    // CREATORS
    Variant_DataImp() = default;
    // Create a 'Variant_DataImp' object that holds an empty buffer.

    // MANIPULATORS

    /// Create a `Variant_DataImp` object whose stored `t_TYPE` object is
    /// constructed by forwarding the specified `args` to
    /// `ConstructionUtil::construct` directly.  This means the first
    /// argument in `args` must be a pointer to `bslma::Allocator`, which
    /// will be ignored if `t_TYPE` is not allocator-aware.
    template <class... t_ARGS>
    Variant_DataImp(t_ARGS&&... args);

    /// Return a reference to the stored `t_TYPE` object.  The behavior is
    /// undefined unless this object contains a `t_TYPE` object (i.e., this
    /// object is currently managing the active alternative of a `bsl::variant`
    /// object).
    BSLS_KEYWORD_CONSTEXPR_CPP14 t_TYPE& value() &;
    BSLS_KEYWORD_CONSTEXPR_CPP14 t_TYPE&& value() &&;

    // ACCESSORS

    /// Return a const reference to the stored `t_TYPE` object.  The behavior
    /// is undefined unless this object contains a `t_TYPE` object (i.e., this
    /// object is currently managing the active alternative of a `bsl::variant`
    /// object).
    BSLS_KEYWORD_CONSTEXPR_CPP14 const t_TYPE& value() const&;
    BSLS_KEYWORD_CONSTEXPR_CPP14 const t_TYPE&& value() const&&;
};

/// This component-private union can hold a `Variant_DataImp` object for any
/// alternative in (template parameters) `t_TYPES...`.  The primary template
/// is used when `t_TYPES...` is empty.
template <class... t_TYPES>
union Variant_Union {
};

/// This partial specialization uses template recursion because C++ does not
/// support member packs.
template <class t_HEAD, class... t_TAIL>
union Variant_Union<t_HEAD, t_TAIL...> {

    // PUBLIC DATA
    Variant_DataImp<t_HEAD>  d_head;
    Variant_Union<t_TAIL...> d_tail;

    // CREATORS

    /// Create a `Variant_Union` object having no active alternative.
    Variant_Union() = default;

    /// Create a `Variant_Union` object holding an object of type `t_HEAD`
    /// direct-initialized from the specified `args`.
    template <class... t_ARGS>
    Variant_Union(bsl::in_place_index_t<0>, t_ARGS&&... args)
    : d_head(std::forward<t_ARGS>(args)...)
    {
    }

    /// Create a `Variant_Union` object holding the alternative at index
    /// (template parameter) `t_INDEX` in (template parameters) 't_HEAD,
    /// t_TAIL...`, direct-initialized from the specified `args'.  Note that
    /// `t_INDEX` is not necessarily the absolute index of the desired
    /// alternative in the `bsl::variant` object.  We use the tag to
    /// "unravel" the union until we get to the desired alternative type.
    template <size_t t_INDEX, class... t_ARGS>
    Variant_Union(bsl::in_place_index_t<t_INDEX>, t_ARGS&&... args)
    : d_tail(bsl::in_place_index_t<t_INDEX - 1>(),
             std::forward<t_ARGS>(args)...)
    {
    }
};
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
/// This component-private class manages an object of type (template parameter)
/// `t_TYPE` that is stored in a `variant` object.  Note that `t_TYPE` may be
/// const-qualified, in which case the internally stored object is not `const`,
/// but the interface of this class prevents modification thereof.
template <class t_TYPE>
class Variant_DataImp {

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<t_TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;  // in-place 't_TYPE' object

  public:
    // CREATORS

    /// Create a `Variant_DataImp` object that holds an empty buffer.
    //! Variant_DataImp() = default;

    // MANIPULATOR

    /// Return a reference to the stored `t_TYPE` object.  The behavior is
    /// undefined unless the enclosing union contains a `t_TYPE` object (i.e.,
    /// the enclosing `bsl::variant` has an active alternative of type
    /// `t_TYPE`).
    t_TYPE& value();

    // ACCESSORS

    /// Return a const reference to the stored `t_TYPE` object.  The behavior
    /// is undefined unless the enclosing union contains a `t_TYPE` object
    /// (i.e., the enclosing `bsl::variant` has an active alternative of type
    /// `t_TYPE`).
    const t_TYPE& value() const;
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This component-private union can hold a `Variant_DataImp` object for any
/// alternative in (template parameters) `t_HEAD, t_TAIL0, t_TAIL...`.  Note
/// that the declaration and definition of the primary template must be kept
/// separate in order for `sim_cpp11_features.pl` to work properly.  The
/// purpose of the `t_TAIL0` template parameter is explained below.
template <class t_HEAD = void, class... t_TAIL0 = void, class... t_TAIL>
union Variant_Union;

/// This specialization is used when the type list is empty.
template <>
union Variant_Union<void, void> {
};

template <>
union Variant_Union<BloombergLP::bsls::CompilerFeaturesNilT> {
};

/// The implementation of this template uses template recursion because C++
/// does not support member packs.  The `t_TAIL0` template parameter is
/// needed in order to avoid an empty template argument list in the type of
/// the `d_tail` member, which causes an issue with the IBM compiler.
template <class t_HEAD, class t_TAIL0, class... t_TAIL>
union Variant_Union<t_HEAD, t_TAIL0, t_TAIL...> {

    // PUBLIC DATA
    Variant_DataImp<t_HEAD>           d_head;
    Variant_Union<t_TAIL0, t_TAIL...> d_tail;
};
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

                     // ==================================
                     // class Variant_CopyConstructVisitor
                     // ==================================

/// This component-private class is a visitor that is used to implement the
/// copy constructor for `bsl::variant`.
template <class t_VARIANT_BASE>
class Variant_CopyConstructVisitor {

  private:
    // DATA
    t_VARIANT_BASE *d_variant_p;

  public:
    // CREATORS

    /// Create a `Variant_CopyConstructVisitor` object that, when invoked, will
    /// copy-construct an alternative in the specified `variant`.
    explicit Variant_CopyConstructVisitor(t_VARIANT_BASE *variant)
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

    // ACCESSORS

    /// Copy-construct the alternative at index (template parameter)
    /// `t_INDEX` from the specified `other` (i.e. the alternative that is
    /// being visited).  Note that the allocator specified on construction
    /// of `*d_variant_p` will be used.
    template <size_t t_INDEX, class t_TYPE>
    void operator()(bsl::in_place_index_t<t_INDEX>, const t_TYPE& other) const
    {
        d_variant_p->template baseEmplace<t_INDEX>(other);
    }
};

                     // ==================================
                     // class Variant_MoveConstructVisitor
                     // ==================================

/// This component-private class is a visitor that is used to implement the
/// move constructor for `bsl::variant`.
template <class t_VARIANT_BASE>
class Variant_MoveConstructVisitor {

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    // DATA
    t_VARIANT_BASE *d_variant_p;

  public:
    // CREATORS

    /// Create a `Variant_MoveConstructVisitor` object that, when invoked,
    /// will move-construct an alternative in the specified `variant`.
    explicit Variant_MoveConstructVisitor(t_VARIANT_BASE *variant)
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

    // ACCESSORS

    /// Move-construct the alternative at index (template parameter)
    /// `t_INDEX` from the specified `other` (i.e. the alternative that is
    /// being visited).  Note that the allocator specified on construction
    /// of `*d_variant_p` will be used.
    template <size_t t_INDEX, class t_TYPE>
    void operator()(bsl::in_place_index_t<t_INDEX>, t_TYPE& other) const
    {
        d_variant_p->template baseEmplace<t_INDEX>(MoveUtil::move(other));
    }
};

                      // ===============================
                      // class Variant_CopyAssignVisitor
                      // ===============================

/// This component-private class is a visitor that is used to implement the
/// copy assignment operator for `bsl::variant`.
template <class t_VARIANT>
class Variant_CopyAssignVisitor {

  private:
    // DATA
    t_VARIANT *d_variant_p;

  public:
    // CREATORS

    /// Create a `Variant_CopyAssignVisitor` object that, when invoked, will
    /// copy-assign to the active alternative of the specified `variant`.
    explicit Variant_CopyAssignVisitor(t_VARIANT *variant)
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }


  public:
    // ACCESSORS

    /// Copy-assign to the active alternative of `*d_variant_p` from the
    /// specified `value` (i.e. the alternative that is being visited).  The
    /// behavior is undefined unless (template parameter) `t_INDEX` is the
    /// index of the active alternative of `*d_variant_p` (or, in C++03, the
    /// index of an alternative that has the same type as the active
    /// alternative).
    template <size_t t_INDEX, class t_TYPE>
    void operator()(bsl::in_place_index_t<t_INDEX>, const t_TYPE& value) const
    {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
        bsl::get<t_INDEX>(*d_variant_p) = value;
#else
        // When invoking this visitor in C++03, `t_INDEX` may not be the index
        // of the active alternative, but it will be an index of an alternative
        // of same type.  However, because `t_TYPE` is deduced, it may be
        // missing cv-qualifications.  `t_ALT_TYPE` will give the correct
        // cv-qualification.
        typedef typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type
                                                     Alt_Type;
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

        ImpUtil::unsafeGet<Alt_Type>(*d_variant_p) = value;
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    }
};

                      // ===============================
                      // class Variant_MoveAssignVisitor
                      // ===============================

/// This component-private class is a visitor that is used to implement the
/// move assignment operator for `bsl::variant`.
template <class t_VARIANT>
class Variant_MoveAssignVisitor {

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    // DATA
    t_VARIANT *d_variant_p;

  public:
    // CREATORS

    /// Create a `Variant_MoveAssignVisitor` object that, when invoked, will
    /// move-assign to the active alternative of the specified `variant`.
    explicit Variant_MoveAssignVisitor(t_VARIANT *variant)
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

  public:
    // ACCESSORS

    /// Move-assign to the active alternative of `*d_variant_p` from the
    /// specified `value` (i.e. the alternative that is being visited).  The
    /// behavior is undefined unless (template parameter) `t_INDEX` is the
    /// index of the active alternative of `*d_variant_p` (or, in C++03, the
    /// index of an alternative that has the same type as the active
    /// alternative).
    template <size_t t_INDEX, class t_TYPE>
    void operator()(bsl::in_place_index_t<t_INDEX>, t_TYPE& value) const
    {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
        bsl::get<t_INDEX>(*d_variant_p) = MoveUtil::move(value);
#else
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
        ImpUtil::unsafeGet<t_TYPE>(*d_variant_p) = MoveUtil::move(value);
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    }
};

                      // ================================
                      // struct Variant_DestructorVisitor
                      // ================================

/// This component-private struct is a visitor that destroys the active
/// alternative of the visited `variant` object.
struct Variant_DestructorVisitor {

  public:
    // ACCESSORS

    /// Destroy the specified `object`, which is the alternative that is
    /// being visited.
    template <class t_TYPE>
    void operator()(t_TYPE& object) const
    {
        bslma::DestructionUtil::destroy(&object);
    }
};

                       // ==============================
                       // struct Variant_##NAME##Visitor
                       // ==============================

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
#define BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(NAME, OP)                      \
    template <class t_VARIANT>                                                \
    struct Variant_##NAME##Visitor {                                          \
        /*  When using this visitor, construct the visitor using the left  */ \
        /*  operand, and call visitation on the right operand.             */ \
        explicit Variant_##NAME##Visitor(const t_VARIANT *variant)            \
        : d_variant_p(variant)                                                \
        {                                                                     \
            BSLS_ASSERT_SAFE(d_variant_p);                                    \
        }                                                                     \
                                                                              \
      private:                                                                \
        const t_VARIANT *d_variant_p;                                         \
                                                                              \
      public:                                                                 \
        template <size_t t_INDEX, class t_TYPE>                               \
        bool operator()(bsl::in_place_index_t<t_INDEX>,                       \
                        const t_TYPE&                  value) const           \
        {                                                                     \
            return (bsl::get<t_INDEX>(*d_variant_p) OP value);                \
        }                                                                     \
    };                                                                        \
                                                                              \
    template <class t_VARIANT>                                                \
    bool Variant_ImpUtil::NAME(const t_VARIANT& lhs, const t_VARIANT& rhs)    \
    {                                                                         \
        BSLS_ASSERT(lhs.index() == rhs.index() &&                             \
                    !lhs.valueless_by_exception());                           \
                                                                              \
        Variant_##NAME##Visitor<t_VARIANT> NAME##Visitor(                     \
                                                   BSLS_UTIL_ADDRESSOF(lhs)); \
        return BSLSTL_VARIANT_VISITID(bool, NAME##Visitor, rhs);              \
    }
#else
#define BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(NAME, OP)                      \
    template <class t_VARIANT>                                                \
    struct Variant_##NAME##Visitor {                                          \
        Variant_##NAME##Visitor(const t_VARIANT *variant)                     \
        : d_variant_p(variant)                                                \
        {                                                                     \
            BSLS_ASSERT_SAFE(d_variant_p);                                    \
        }                                                                     \
                                                                              \
      private:                                                                \
        const t_VARIANT *d_variant_p;                                         \
                                                                              \
      public:                                                                 \
        template <size_t t_INDEX, class t_TYPE>                               \
        bool operator()(bsl::in_place_index_t<t_INDEX>,                       \
                        t_TYPE&                  value) const                 \
        {                                                                     \
            /* When invoking this visitor in C++03, `t_INDEX` may not be   */ \
            /* the index of the active alternative, but it will be an      */ \
            /* index of an alternative of same type.  However, because     */ \
            /* `t_TYPE` is deduced, it may have incorrect                  */ \
            /* cv-qualifications.  `t_ALT_TYPE` will give the correct      */ \
            /* cv-qualification.                                           */ \
            typedef                                                           \
                typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type   \
                                                                    Alt_Type; \
            typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;             \
            return (ImpUtil::unsafeGet<Alt_Type>(*d_variant_p) OP value);     \
        }                                                                     \
    };                                                                        \
                                                                              \
    template <class t_VARIANT>                                                \
    bool Variant_ImpUtil::NAME(const t_VARIANT& lhs, const t_VARIANT& rhs)    \
    {                                                                         \
        BSLS_ASSERT(lhs.index() == rhs.index() &&                             \
                    !lhs.valueless_by_exception());                           \
                                                                              \
        Variant_##NAME##Visitor<t_VARIANT> NAME##Visitor(                     \
                                                   BSLS_UTIL_ADDRESSOF(lhs)); \
        return BSLSTL_VARIANT_VISITID(bool, NAME##Visitor, rhs);              \
    }
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    // This macro is used to define the 'Variant_ImpUtil::*Impl' methods.

BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(Equal, ==)
BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(NotEqual, !=)
BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(LessThan, <)
BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(GreaterThan, >)
BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(LessOrEqual, <=)
BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(GreaterOrEqual, >=)

                         // =========================
                         // class Variant_SwapVisitor
                         // =========================

/// This component-private class is a visitor that is used to implement
/// `bsl::variant::swap`.
template <class t_VARIANT>
class Variant_SwapVisitor {

  private:
    // DATA
    t_VARIANT *d_variant_p;

  public:
    // CREATORS

    /// Create a `Variant_SwapVisitor` object that, when invoked, will swap
    /// the visited alternative with the active alternative of the specified
    /// `variant`.
    explicit Variant_SwapVisitor(t_VARIANT *variant)
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

    // ACCESSORS

    /// Swap the alternative having index (template parameter) `t_INDEX` in
    /// `*d_variant_p` with the specified `value` (i.e. the alternative that
    /// is being visited).  The behavior is undefined unless `t_INDEX` is the
    /// index of the active alternative of `*d_variant_p` (or, in C++03, the
    /// index of an alternative that has the same type as the active
    /// alternative).
    template <size_t t_INDEX, class t_TYPE>
    void operator()(bsl::in_place_index_t<t_INDEX>, t_TYPE& value) const
    {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
        BloombergLP::bslalg::SwapUtil::swap(
                         BSLS_UTIL_ADDRESSOF(value),
                         BSLS_UTIL_ADDRESSOF(bsl::get<t_INDEX>(*d_variant_p)));
#else
        typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
            // When invoking this visitor in C++03, 't_INDEX' may not be the
            // index of the active alternative, but it will be an index of an
            // alternative of same type. Swap can not be invoked on a constant
            // variant, so the cv qualifications will match.
        BloombergLP::bslalg::SwapUtil::swap(
              BSLS_UTIL_ADDRESSOF(value),
              BSLS_UTIL_ADDRESSOF((ImpUtil::unsafeGet<t_TYPE>(*d_variant_p))));
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    }
};

                         // =========================
                         // class Variant_HashVisitor
                         // =========================

/// This component-private class is a visitor that is used to implement
/// `hashAppend` for `bsl::variant`.
template <class t_HASHALG>
class Variant_HashVisitor {

  private:
    // DATA
    t_HASHALG& d_hashAlg;

  public:
    // CREATORS

    /// Create a `Variant_HashVisitor` object that, when invoked, appends to
    /// the specified `hashAlg`.
    explicit Variant_HashVisitor(t_HASHALG& hashAlg)
    : d_hashAlg(hashAlg)
    {
    }

    // ACCESSORS

    /// Append the specified `value` (i.e., the alternative that is being
    /// visited) to `d_hashAlg`.
    template <class t_TYPE>
    void operator()(t_TYPE& value) const
    {
        using BloombergLP::bslh::hashAppend;
        hashAppend(d_hashAlg, value);
    }
};

/// This component-private struct keeps track of the allocator for a
/// `bsl::variant` object.  The primary template is used when the `variant`
/// is not allocator-aware (because it has no allocator-aware alternatives).
template <bool t_AA>
struct Variant_AllocatorBase {

    // ACCESSORS

    /// Return a null pointer.  Note that this method has the same return
    /// type as the allocator-aware version so that the remainder of the
    /// implementation of `bsl::variant` is abstracted with respect to
    /// whether or not the specialization is allocator-aware; this is why
    /// both methods return `bslma::Allocator*` instead of
    /// `bsl::allocator<char>`.  The returned pointer will be ignored (and
    /// not dereferenced) when passed to
    /// `bslma::ConstructionUtil::construct`.
    BloombergLP::bslma::Allocator *mechanism() const
    {
        return NULL;
    }
};

/// This specialization is used when the `variant` is allocator-aware.
template <>
struct Variant_AllocatorBase<true> {

    // TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
    typedef bsl::allocator<char>               allocator_type;

    // PUBLIC DATA
    allocator_type d_allocator;

    // CREATORS

    /// Create a `Variant_AllocatorBase` object that holds the currently
    /// installed default allocator.
    Variant_AllocatorBase() {}

    /// Create a `Variant_AllocatorBase` object that holds a copy of the
    /// allocator held by the specified `original`.
    Variant_AllocatorBase(const Variant_AllocatorBase& original)
    : d_allocator(original.d_allocator)
    {
    }

    /// Create a `Variant_AllocatorBase` object that holds a copy of the
    /// specified `allocator`.
    Variant_AllocatorBase(allocator_type allocator)
    : d_allocator(allocator)
    {
    }

    // ACCESSORS

    /// Return the mechanism of the stored allocator.
    BloombergLP::bslma::Allocator *mechanism() const
    {
        return d_allocator.mechanism();
    }
};

                            // ===================
                            // struct variant_base
                            // ===================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This component-private struct defines the data representation of
/// `bsl::variant` and contains the implementations of its special member
/// functions (which must be defaulted in `bsl::variant` itself).  This
/// class also contains implementations of additional constructors and
/// methods needed for the correct functionality of the `variant` class
/// hierarchy.
template <class t_HEAD, class... t_TAIL>
struct Variant_Base
: public BloombergLP::bslstl::Variant_AllocatorBase<
      BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<t_HEAD,
                                                         t_TAIL...>::value> {

    // TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
    typedef BloombergLP::bslstl::Variant_AllocatorBase<
        BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<t_HEAD,
                                                           t_TAIL...>::value>
        AllocBase;
    typedef BloombergLP::bslstl::Variant_Union<t_HEAD, t_TAIL...>
                                            Variant_Union;
    typedef bsl::variant<t_HEAD, t_TAIL...> Variant;

    /// This trivial tag type is used as a dummy when `Variant_Base` wraps a
    /// non-allocator-aware type.
    struct NoAlloc {
    };

    /// Type alias to the allocator type used by `variant`.
    typedef typename bsl::conditional<
        BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<t_HEAD,
                                                           t_TAIL...>::value,
        bsl::allocator<char>,
        NoAlloc>::type allocator_type;

    // PUBLIC DATA

    /// Index of the currently active alternative in this variant object, or
    /// `bsl::variant_npos` if the variant is valueless by exception.
    size_t        d_type;

    /// Union holding the alternative object.
    Variant_Union d_union;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        Variant_Base,
        BloombergLP::bslma::UsesBslmaAllocator,
        (BloombergLP::bslstl::
             Variant_UsesBslmaAllocatorAny<t_HEAD, t_TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        Variant_Base,
        BloombergLP::bslmf::UsesAllocatorArgT,
        (BloombergLP::bslstl::
             Variant_UsesBslmaAllocatorAny<t_HEAD, t_TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        Variant_Base,
        BloombergLP::bslmf::IsBitwiseMoveable,
        (BloombergLP::bslstl::Variant_IsBitwiseMoveableAll<t_HEAD,
                                                           t_TAIL...>::value));

    // CREATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    /// Create a `Variant_Base` object holding the 0th alternative, which is
    /// value-initialized.  If this `Variant_Base` is allocator-aware, the
    /// currently installed default allocator is used to supply memory.
    Variant_Base();

    /// Create a `Variant_Base` object holding the same alternative (if any)
    /// as the specified `original` object.  If this `Variant_Base` is
    /// allocator-aware, the currently installed default allocator is used
    /// to supply memory.  If the `original` object is not valueless by
    /// exception', the contained value is copy-constructed from the
    /// contained value of `original`.  All alternatives shall be copy
    /// constructible.
    Variant_Base(const Variant_Base& original);

    /// Create a `Variant_Base` object holding the same alternative (if any)
    /// held by the specified `original`.  If this `Variant_Base` is
    /// allocator-aware, the allocator of `original` is used to supply
    /// memory.  If the `original` object is not valueless by exception, the
    /// contained value is move-constructed from the contained value of
    /// `original`.  All alternatives shall be move constructible.
    Variant_Base(Variant_Base&& original);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// Create a `Variant_Base` object whose index is the specified `index`
    /// without constructing an alternative object.  If this `Variant_Base`
    /// is allocator-aware, the default allocator is used to supply memory.
    Variant_Base(Variant_ConstructFromStdTag, size_t index);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    /// Create a `Variant_Base` object holding the alternative with index
    /// (template parameter) `t_INDEX`, direct-initialized from the
    /// specified `args`.  If this `Variant_Base` is allocator-aware, the
    /// currently installed default allocator is used to supply memory.
    template <size_t t_INDEX, class... t_ARGS>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>, t_ARGS&&... args);

    // allocator-extended constructors

    /// Create a `Variant_Base` object holding the 0th alternative, which is
    /// value-initialized.  If this `Variant_Base` is allocator-aware, the
    /// specified `allocator` is used to supply memory.
    Variant_Base(bsl::allocator_arg_t, allocator_type allocator);

    /// Create a `Variant_Base` object holding the same alternative (if any)
    /// held by the specified `original` object.  If this `Variant_Base` is
    /// allocator-aware, the specified `allocator` is used to supply memory.
    /// If the `original` object is not valueless by exception, the
    /// contained value is copy/move constructed from the contained value of
    /// `original`.  All alternatives shall be copy/move constructible.
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type       allocator,
                 const Variant&       original);
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type       allocator,
                 Variant&&            original);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// Create a `Variant_Base` object whose index is the specified `index`
    /// without constructing an alternative object.  If this `Variant_Base`
    /// is allocator-aware, the specified `allocator` is used to supply
    /// memory.
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type allocator,
                 Variant_ConstructFromStdTag,
                 size_t         index);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    /// Create a `Variant_Base` object holding the alternative with index
    /// (template parameter) `t_INDEX`, direct-initialized from the
    /// specified `args`.  If this `Variant_Base` is allocator-aware, the
    /// specified `allocator` is used to supply memory.
    template <size_t t_INDEX, class... t_ARGS>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type                 allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          t_ARGS&&...                    args);

#else  // BSL_VARIANT_FULL_IMPLEMENTATION

    /// Create a `Variant_Base` object holding the 0th alternative, which is
    /// value-initialized.  If this `Variant_Base` is allocator-aware, the
    /// currently installed default constructor is used to supply memory.
    Variant_Base();

    /// Create a `Variant_Base` object holding the same alternative (if any) as
    /// the specified `original` object.  If this `Variant_Base` is
    /// allocator-aware, the currently installed default allocator is used to
    /// supply memory.  If the `original` object is not valueless by
    /// exception, the contained value is copy-constructed from the contained
    /// value of `original`.  All alternatives shall be copy constructible.
    Variant_Base(const Variant& original);

    /// Create a `Variant_Base` object holding the same alternative (if any)
    /// held by the specified `original`.  If this `Variant_Base` is
    /// allocator-aware, the allocator of `original` is used to supply memory.
    /// If the `original` object is not valueless by exception, the contained
    /// value is move-constructed from the contained value of `original`.  All
    /// alternatives shall be move constructible.
    Variant_Base(BloombergLP::bslmf::MovableRef<Variant> original);

    /// Create a `Variant_Base` object holding the alternative with index
    /// (template parameter) `t_INDEX`, which is value-initialized.  If this
    /// `Variant_Base` is allocator-aware, the currently installed default
    /// constructor is used to supply memory.
    template <size_t t_INDEX>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>);

    /// Create a `Variant_Base` object holding the alternative with index
    /// (template parameter) `t_INDEX`, which is direct-initialized from the
    /// specified `arg`.  If this `Variant_Base` is allocator-aware, the
    /// currently installed default constructor is used to supply memory.
    template <size_t t_INDEX, class t_ARG_01>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01&                arg_01);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01&                arg_01,
                          const t_ARG_02&                arg_02);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01&                arg_01,
                          const t_ARG_02&                arg_02,
                          const t_ARG_03&                arg_03);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07,
                          const t_ARG_08& arg_08);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07,
                          const t_ARG_08& arg_08,
                          const t_ARG_09& arg_09);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    explicit Variant_Base(bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07,
                          const t_ARG_08& arg_08,
                          const t_ARG_09& arg_09,
                          const t_ARG_10& arg_10);

    // allocator-extended constructors

    /// Create a `Variant_Base` object holding the 0th alternative, which is
    /// value-initialized.  If this `Variant_Base` is allocator-aware, the
    /// specified `allocator` is used to supply memory.
    Variant_Base(bsl::allocator_arg_t, allocator_type allocator);

    /// Create a `Variant_Base` object holding the same alternative (if any)
    /// held by the specified `original` object.  If this `Variant_Base` is
    /// allocator-aware, the specified `allocator` is used to supply memory.
    /// If the `original` object is not valueless by exception, the
    /// contained value is copy/move constructed from the contained value of
    /// `original`.  All alternatives shall be copy/move constructible.
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type       allocator,
                 const Variant&       original);
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type                          allocator,
                 BloombergLP::bslmf::MovableRef<Variant> original);

    /// Create a `Variant_Base` object holding the alternative with index
    /// (template parameter) `t_INDEX`, which is value-initialized.  If this
    /// `Variant_Base` is allocator-aware, the specified `allocator` is used to
    /// supply memory.
    template <size_t t_INDEX>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type                 allocator,
                          bsl::in_place_index_t<t_INDEX>);

    /// Create a `Variant_Base` object holding the alternative with index
    /// (template parameter) `t_INDEX`, direct-initialized from `arg`.  If this
    /// `Variant_Base` is allocator-aware, the specified `allocator` is used to
    /// supply memory.
    template <size_t t_INDEX, class t_ARG_01>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type                 allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01&                arg_01);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type                 allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01&                arg_01,
                          const t_ARG_02&                arg_02);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type                 allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01&                arg_01,
                          const t_ARG_02&                arg_02,
                          const t_ARG_03&                arg_03);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07,
                          const t_ARG_08& arg_08);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07,
                          const t_ARG_08& arg_08,
                          const t_ARG_09& arg_09);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<t_INDEX>,
                          const t_ARG_01& arg_01,
                          const t_ARG_02& arg_02,
                          const t_ARG_03& arg_03,
                          const t_ARG_04& arg_04,
                          const t_ARG_05& arg_05,
                          const t_ARG_06& arg_06,
                          const t_ARG_07& arg_07,
                          const t_ARG_08& arg_08,
                          const t_ARG_09& arg_09,
                          const t_ARG_10& arg_10);

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    /// Destroy this object.  The contained value, if any, is destroyed.
    ~Variant_Base();

    // MANIPULATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, direct-initialized from the specified `args`.  If this
    /// `Variant_Base` object already holds a value, that contained value is
    /// destroyed before the new object is created.  If the alternative is
    /// allocator-aware, it uses the allocator specified upon the
    /// construction of this `Variant_Base` object to supply memory; passing
    /// an allocator argument to this method results in two allocators being
    /// passed to the alternative constructor, resulting in a likely
    /// compilation error.  Note that if the constructor of the alternative
    /// exits via an exception, this object is left in the valueless by
    /// exception state.
    template <size_t t_INDEX, class... t_ARGS>
    void baseEmplace(t_ARGS&&...);
#else   //  BSL_VARIANT_FULL_IMPLEMENTATION
    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, which is value-initialized.  If this `Variant_Base` object
    /// already holds a value, that contained value is destroyed before the new
    /// object is created.  If the alternative is allocator-aware, it uses the
    /// allocator specified upon the construction of this `Variant_Base` object
    /// to supply memory; passing an allocator argument to this method results
    /// in two allocators being passed to the alternative constructor,
    /// resulting in a likely compilation error.  Note that if the constructor
    /// of the alternative exits via an exception, this object is left in the
    /// valueless by exception state.
    template <size_t t_INDEX>
    void baseEmplace();

    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, direct-initialized from the specified `arg`.  If this
    /// `Variant_Base` object already holds a value, that contained value is
    /// destroyed before the new object is created.  If the alternative is
    /// allocator-aware, it uses the allocator specified upon the construction
    /// of this `Variant_Base` object to supply memory; passing an allocator
    /// argument to this method results in two allocators being passed to the
    /// alternative constructor, resulting in a likely compilation error.  Note
    /// that if the constructor of the alternative exits via an exception, this
    /// object is left in the valueless by exception state.
    template <size_t t_INDEX, class t_ARG_01>
    void baseEmplace(const t_ARG_01& arg_01);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
    void baseEmplace(const t_ARG_01& arg_01, const t_ARG_02& arg_02);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    void baseEmplace(const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     const t_ARG_10& arg_10);
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    /// If the specified `rhs` holds the same alternative type as this
    /// object, copy assign the contained value of `rhs` to the contained
    /// value of this object.  Otherwise, destroy the contained value of
    /// this object (if any) and, if `rhs` holds a value, copy-construct the
    /// corresponding alternative of this object from the contained value of
    /// `rhs`.  The allocators of this object and `rhs` both remain
    /// unchanged.  If the construction of a new alternative object exits
    /// via an exception, this `Variant_Base` object is left in a valueless
    /// by exception state.  This behavior differs from the standard, for
    /// reasons that are explained in the documentation for
    /// `variant::operator=`.  All alternatives shall be copy constructible
    /// and copy assignable.
    Variant_Base& operator=(const Variant_Base& rhs);

    /// If the specified `rhs` holds the same alternative type as this
    /// object, move assign the contained value of `rhs` to the contained
    /// value of this object.  Otherwise, destroy the currently held
    /// alternative object (if any) and, if `rhs` holds a value,
    /// move-construct the corresponding alternative of this object from the
    /// contained value of `rhs`.  The allocators of this object and `rhs`
    /// both remain unchanged.  If the construction of a new alternative
    /// object exits via an exception, this `Variant_Base` object is left in
    /// a valueless by exception state.  All alternatives shall be move
    /// constructible and move assignable.
    Variant_Base& operator=(BloombergLP::bslmf::MovableRef<Variant_Base> rhs);

    /// Destroy the contained value, if any.
    void reset() BSLS_KEYWORD_NOEXCEPT;
};
#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                               // =============
                               // class variant
                               // =============

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
class variant
: private BloombergLP::bslstl::Variant_Base<t_HEAD, t_TAIL...>
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
,
  private BloombergLP::bslstl::Variant_SMFBase<
      BloombergLP::bslstl::Variant_IsCopyConstructibleAll<t_HEAD,
                                                          t_TAIL...>::value,
      BloombergLP::bslstl::Variant_IsMoveConstructibleAll<t_HEAD,
                                                          t_TAIL...>::value,
      BloombergLP::bslstl::Variant_IsCopyConstructibleAll<t_HEAD,
                                                          t_TAIL...>::value &&
          BloombergLP::bslstl::Variant_IsCopyAssignableAll<t_HEAD,
                                                           t_TAIL...>::value,
      BloombergLP::bslstl::Variant_IsMoveConstructibleAll<t_HEAD,
                                                          t_TAIL...>::value &&
          BloombergLP::bslstl::Variant_IsMoveAssignableAll<t_HEAD,
                                                           t_TAIL...>::value>
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
{
  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                   MoveUtil;
    typedef BloombergLP::bslstl::Variant_Base<t_HEAD, t_TAIL...> Variant_Base;
    typedef BloombergLP::bslstl::Variant_Union<t_HEAD, t_TAIL...>
        Variant_Union;

    // FRIENDS
    friend struct BloombergLP::bslstl::Variant_ImpUtil;
        // This struct contains functions that require access to the
        // 'Variant_Union' subobject.  Note that because 'variant' is not
        // standard-layout in the allocator-aware case, we cannot easily cast
        // between the variant object and its 'Variant_Union' subobject.

    friend struct BloombergLP::bslstl::Variant_Base<t_HEAD, t_TAIL...>;
        // 'Variant_Base' must be able to downcast from 'Variant_Base' to
        // 'variant' (which is privately derived from 'Variant_Base').

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        variant,
        BloombergLP::bslma::UsesBslmaAllocator,
        (BloombergLP::bslstl::
             Variant_UsesBslmaAllocatorAny<t_HEAD, t_TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        variant,
        BloombergLP::bslmf::UsesAllocatorArgT,
        (BloombergLP::bslstl::
             Variant_UsesBslmaAllocatorAny<t_HEAD, t_TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        variant,
        BloombergLP::bslmf::IsBitwiseMoveable,
        (BloombergLP::bslstl::Variant_IsBitwiseMoveableAll<t_HEAD,
                                                           t_TAIL...>::value));

    // TYPES

    /// Type alias to the allocator type used by `variant`.
    typedef typename Variant_Base::allocator_type allocator_type;

    // CREATORS

    // 20.7.3.1, constructors
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    /// Create a `variant` object holding the 0th alternative, which is
    /// value-initialized.  If this `variant` is allocator-aware, the
    /// currently installed default allocator is used to supply memory.
    /// This constructor participates in overload resolution only if the 0th
    /// alternative is default constructible.  For simplicity of
    /// implementation, this method differs from the standard in the
    /// following ways:
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    template <class t_FIRST_ALT = t_HEAD,
              class             = typename bsl::enable_if_t<
                  std::is_default_constructible<t_FIRST_ALT>::value> >
    variant()
    : Variant_Base()
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    /// Create a `variant` object holding the same alternative (if any) as
    /// the specified `original` object.  If this `variant` is
    /// allocator-aware, the currently installed default allocator is used
    /// to supply memory.  If the `original` object is not valueless by
    /// exception', the contained value is copy-constructed from the
    /// contained value of `original`.  This constructor is deleted unless
    /// all alternatives are copy constructible.  For simplicity of
    /// implementation, this method differs from the standard in the
    /// following ways:
    /// * conditional triviality is not implemented
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    variant(const variant& original) = default;

    /// Create a `variant` object holding the same alternative (if any) held
    /// by the specified `original`.  If this `variant` is allocator-aware,
    /// the allocator of `original` is used to supply memory.  If `original`
    /// is not valueless by exception, the contained value is
    /// move-constructed from the contained value of `original`.  This
    /// constructor participates in overload resolution only if all
    /// alternatives are move constructible.  For simplicity of
    /// implementation, this method differs from the standard in the
    /// following ways:
    /// * conditional triviality is not implemented
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    variant(variant&& original) = default;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// Create a `variant` object holding the same alternative (if any) held
    /// by the specified `original`.  If this `variant` is allocator-aware,
    /// the specified `allocator` is used to supply memory.  If `original`
    /// is not valueless by exception, the contained value is
    /// copy/move-constructed from the contained value of `original`.  This
    /// constructor participates in overload resolution only if all
    /// alternatives are copy/move-constructible.
    template <class t_STD_VARIANT>
    explicit variant(
                  t_STD_VARIANT&& original,
                  BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM_STD(t_STD_VARIANT))
    : Variant_Base(BloombergLP::bslstl::Variant_ConstructFromStdTag(),
                   original.index())
    {
        // MSVC 19.22 and later has a bug where if this constructor is defined
        // out of line, it becomes non-explicit in copy-list-initialization.
        if (!valueless_by_exception()) {
            BloombergLP::bslstl::Variant_ImpUtil::
                ConstructFromStdVisitor<variant, decltype(original)>
                    visitor(*this, original);
            BSLSTL_VARIANT_VISITID(void, visitor, *this);
        }
    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    /// Create a `variant` object whose contained value is
    /// direct-initialized from the specified `t`.  The alternative selected
    /// is the best match among all alternatives for which the expression
    /// `t_ALT_TYPE x[] = {std::forward<t_TYPE>(t)};` is well formed, and
    /// this constructor participates in overload resolution only if there
    /// is a unique best matching alternative and that alternative is
    /// constructible from `t`.  If this `variant` is allocator-aware, the
    /// currently installed default allocator is used to supply memory.
    /// Note that the cv-qualification of an alternative type does not
    /// affect how well the alternative type matches an argument type.  For
    /// simplicity of implementation, this method differs from the standard
    /// in the following ways:
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    template <class t_TYPE>
    variant(t_TYPE&& t,
            BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, t_TYPE));

    /// Create a `variant` object holding a contained value of type
    /// (template parameter) `t_TYPE`, direct-initialized from the specified
    /// `args`.  If this `variant` is allocator-aware, the currently
    /// installed default allocator is used to supply memory.  This
    /// constructor participates in overload resolutionly if `t_TYPE`
    /// designates a unique alternative and is constructible from `args`.
    /// For simplicity of implementation, this method differs from the
    /// standard in the following way:
    /// * `constexpr` is not implemented
    template <class t_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(t_TYPE) &&
                  std::is_constructible<t_TYPE, t_ARGS...>::value> >
    explicit variant(bsl::in_place_type_t<t_TYPE>, t_ARGS&&... args)
    : Variant_Base(
          bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
          std::forward<t_ARGS>(args)...)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    template <class t_TYPE,
              class INIT_LIST_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(t_TYPE) &&
                  std::is_constructible<t_TYPE,
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        t_ARGS...>::value> >
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     t_ARGS&&...                           args)
    // Create a 'variant' object holding a contained value of type
    // (template parameter) 't_TYPE', direct-initialized from the specified
    // 'il' and 'args'.  If this 'variant' is allocator-aware, the
    // currently installed default allocator is used to supply memory.
    // This constructor participates in overload resolutionly if 't_TYPE'
    // designates a unique alternative and is constructible from 'il' and
    // 'args'.  For simplicity of implementation, this method differs from
    // the standard in the following way:
    //: o 'constexpr' is not implemented
    : Variant_Base(
          bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
          il,
          std::forward<t_ARGS>(args)...)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    /// Create a `variant` object holding the alternative with index
    /// (template parameter) `t_INDEX`, direct-initialized from the
    /// specified `args`.  If this `variant` is allocator-aware, the
    /// currently installed default allocator is used to supply memory.
    /// This constructor participates in overload resolution only if
    /// `t_INDEX` is a valid alternative index and the designated
    /// alternative is constructible from `args`.  For simplicity of
    /// implementation, this method differs from the standard in the
    /// following ways:
    /// * `constexpr` is not implemented
    template <size_t   t_INDEX,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  (t_INDEX < 1 + sizeof...(t_TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(t_INDEX),
                                        t_ARGS...>::value> >
    explicit variant(bsl::in_place_index_t<t_INDEX>, t_ARGS&&... args)
    : Variant_Base(bsl::in_place_index_t<t_INDEX>(),
                   std::forward<t_ARGS>(args)...)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, direct-initialized from the specified `il` and
    /// `args`.  If this `variant` is allocator-aware, the currently installed
    /// default allocator is used to supply memory.  This constructor
    /// participates in overload resolution only if `t_INDEX` is a valid
    /// alternative index and the designated alternative is constructible from
    /// `il` and `args`.  For simplicity of implementation, this method differs
    /// from the standard in the following way:
    /// * `constexpr` is not implemented
    template <size_t t_INDEX,
              class INIT_LIST_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  (t_INDEX < 1 + sizeof...(t_TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(t_INDEX),
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        t_ARGS...>::value> >
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     t_ARGS&&...                           args)
    : Variant_Base(bsl::in_place_index_t<t_INDEX>(),
                   il,
                   std::forward<t_ARGS>(args)...)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    // allocator-extended constructors

    /// Create a `variant` object holding the 0th alternative, which is
    /// value-initialized.  If this `variant` is allocator-aware, the specified
    /// `allocator` is used to supply memory.  This constructor participates in
    /// overload resolution only if the 0th alternative is default
    /// constructible.
    template <class FIRST = t_HEAD,
              class       = typename bsl::enable_if_t<
                  std::is_default_constructible<FIRST>::value> >
    variant(bsl::allocator_arg_t, allocator_type allocator)
    : Variant_Base(bsl::allocator_arg_t(), allocator)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    /// Create a `variant` object holding the same alternative (if any) as the
    /// specified `original` object.  If this `variant` is allocator-aware, the
    /// specified `allocator` is used to supply memory.  If the `original`
    /// object is not valueless by exception`, the contained value is
    /// copy-constructed from the contained value of `original`.  This
    /// constructor is deleted unless all alternatives are copy constructible.
    template <
        class FIRST = t_HEAD,
        class       = typename bsl::enable_if_t<
            BloombergLP::bslstl::
                Variant_IsCopyConstructibleAll<FIRST, t_TAIL...>::value> >
    variant(bsl::allocator_arg_t,
            allocator_type allocator,
            const variant& original)
    : Variant_Base(bsl::allocator_arg_t(), allocator, original)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    /// Create a `variant` object holding the same alternative (if any) held
    /// by the specified `original`.  If this `variant` is allocator-aware,
    /// the specified `allocator` is used to supply memory.  If `original`
    /// is not valueless by exception, the contained value is
    /// move-constructed from the contained value of `original`.  This
    /// constructor participates in overload resolution only if all
    /// alternatives are move constructible.
    template <
        class FIRST = t_HEAD,
        class       = typename bsl::enable_if_t<
            BloombergLP::bslstl::
                Variant_IsMoveConstructibleAll<FIRST, t_TAIL...>::value> >
    variant(bsl::allocator_arg_t, allocator_type allocator, variant&& original)
    : Variant_Base(bsl::allocator_arg_t(), allocator, std::move(original))
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// Create a `variant` object holding the same alternative (if any) held
    /// by the specified `original`.  If this `variant` is allocator-aware,
    /// the specified `allocator` is used to supply memory.  If `original`
    /// is not valueless by exception, the contained value is
    /// copy/move-constructed from the contained value of `original`.  This
    /// constructor participates in overload resolution only if all
    /// alternatives are copy/move-constructible.
    template <class t_STD_VARIANT>
    explicit variant(
                  bsl::allocator_arg_t,
                  allocator_type  allocator,
                  t_STD_VARIANT&& original,
                  BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM_STD(t_STD_VARIANT))
    : Variant_Base(bsl::allocator_arg_t(),
                   allocator,
                   BloombergLP::bslstl::Variant_ConstructFromStdTag(),
                   original.index())
    {
        // MSVC 19.22 and later has a bug where if this constructor is defined
        // out of line, it becomes non-explicit in copy-list-initialization.
        if (!valueless_by_exception()) {
            BloombergLP::bslstl::Variant_ImpUtil::
                ConstructFromStdVisitor<variant, decltype(original)>
                    visitor(*this, original);
            BSLSTL_VARIANT_VISITID(void, visitor, *this);
        }
    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    /// Create a `variant` object whose contained value is
    /// direct-initialized from the specified `t`.  The alternative selected
    /// is the best match among all alternatives for which the expression
    /// `t_ALT_TYPE x[] = {std::forward<t_TYPE>(t)};` is well formed, and
    /// this constructor participates in overload resolution only if there
    /// is a unique best matching alternative and that alternative is
    /// constructible from `t`.  If this `variant` is allocator-aware, the
    /// specified `allocator` is used to supply memory.  Note that the
    /// cv-qualification of an alternative type does not affect how well the
    /// alternative type matches an argument type.
    template <class t_TYPE>
    variant(
          bsl::allocator_arg_t,
          allocator_type                                             allocator,
          t_TYPE&&                                                   t,
          BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, t_TYPE));

    /// Create a `variant` object holding a contained value of type
    /// (template parameter) `t_TYPE`, direct-initialized from the specified
    /// `args`.  If this `variant` is allocator-aware, the specified
    /// `allocator `is used to supply memory.  This constructor participates
    /// in overload resolutionly if `t_TYPE` designates a unique alternative
    /// and is constructible from `args`.
    template <class t_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(t_TYPE) &&
                  std::is_constructible<t_TYPE, t_ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     t_ARGS&&... args)
    : Variant_Base(
          bsl::allocator_arg_t{},
          allocator,
          bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
          std::forward<t_ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    /// Create a `variant` object holding a contained value of type (template
    /// parameter) `t_TYPE`, direct-initialized from the specified `il` and
    /// `args`.  If this `variant` is allocator-aware, the specified `allocator
    /// `is used to supply memory.  This constructor participates in overload
    /// resolutionly if `t_TYPE` designates a unique alternative and is
    /// constructible from `il` and `args`.
    template <class t_TYPE,
              class INIT_LIST_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(t_TYPE) &&
                  std::is_constructible<t_TYPE,
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        t_ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     t_ARGS&&...                           args)
    : Variant_Base(
          bsl::allocator_arg_t{},
          allocator,
          bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
          il,
          std::forward<t_ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, direct-initialized from the specified `args`.  If
    /// this `variant` is allocator-aware, the specified `allocator` is used to
    /// supply memory.  This constructor participates in overload resolution
    /// only if `t_INDEX` is a valid alternative index and the designated
    /// alternative is constructible from `args`.
    template <size_t   t_INDEX,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  (t_INDEX < 1 + sizeof...(t_TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(t_INDEX),
                                        t_ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     t_ARGS&&... args)
    : Variant_Base(bsl::allocator_arg_t{},
                   allocator,
                   bsl::in_place_index_t<t_INDEX>(),
                   std::forward<t_ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, direct-initialized from the specified `il` and
    /// `args`.  If this `variant` is allocator-aware, the specified
    /// `allocator` is used to supply memory.  This constructor participates in
    /// overload resolution only if `t_INDEX` is a valid alternative index and
    /// the designated alternative is constructible from `il` and `args`.
    template <size_t t_INDEX,
              class INIT_LIST_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  (t_INDEX < 1 + sizeof...(t_TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(t_INDEX),
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        t_ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     t_ARGS&&...                           args)
    : Variant_Base(bsl::allocator_arg_t{},
                   allocator,
                   bsl::in_place_index_t<t_INDEX>(),
                   il,
                   std::forward<t_ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

#else  // BSL_VARIANT_FULL_IMPLEMENTATION

    /// Create a `variant` object holding the 0th alternative, which is
    /// value-initialized.  If this `variant` is allocator-aware, the
    /// currently installed default allocator is used to supply memory.
    variant();

    // Create a `variant` object holding the same alternative (if any) as the
    // specified `original` object.  If this `variant` is allocator-aware, the
    // currently installed default allocator is used to supply memory.  If the
    // `original` object is not valueless by exception, the contained value is
    // copy-constructed from the contained value of `original`.  All
    // alternatives shall be copy constructible.
    variant(const variant& original);

    /// Create a `variant` object holding the same alternative (if any) held by
    /// the specified `original`.  If this `variant` is allocator-aware, the
    /// allocator of `original` is used to supply memory.  If `original` is not
    /// valueless by exception, the contained value is move-constructed from
    /// the contained value of `original`.  All alternatives shall be move
    /// constructible.
    variant(BloombergLP::bslmf::MovableRef<variant> original);

    /// Create a `variant` object whose contained value is copy or move
    /// constructed from the specified `value`.  If (template parameter)
    /// `t_TYPE` is `bslmf::MovableRef<T>`, then the selected alternative is
    /// the one whose type is the same as `T` (modulo cv-qualification);
    /// otherwise, the selected alternative is the one that is the same as
    /// `t_TYPE` (modulo cv-qualification).  This constructor participates
    /// in overload resolution only if there is a unique such alternative.
    /// If this `variant` is allocator-aware, the current installed default
    /// allocator is used to supply memory.
    template <class t_TYPE>
    variant(const t_TYPE&                                              value,
            BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, t_TYPE));

    /// Create a `variant` object holding a contained value of type (template
    /// parameter) `t_TYPE`, which is value-initialized.  If this `variant` is
    /// allocator-aware, the currently installed default allocator is used to
    /// supply memory.  This constructor participates in overload resolutionly
    /// if `t_TYPE` designates a unique alternative.
    template <class t_TYPE>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    /// Create a `variant` object holding a contained value of type (template
    /// parameter) `t_TYPE`, which is direct-initialized from the specified
    /// `arg`.  If this `variant` is allocator-aware, the currently installed
    /// default allocator is used to supply memory.  This constructor
    /// participates in overload resolution only if `t_TYPE` designates a
    /// unique alternative.
    template <class t_TYPE, class t_ARG_01>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01&                                   arg_01,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE, class t_ARG_01, class t_ARG_02>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01&                                   arg_01,
                     const t_ARG_02&                                   arg_02,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01&                                   arg_01,
                     const t_ARG_02&                                   arg_02,
                     const t_ARG_03&                                   arg_03,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    explicit variant(bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     const t_ARG_10& arg_10,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, which is value-initialized.  If this `variant` is
    /// allocator-aware, the currently installed default allocator is used to
    /// supply memory.  `t_INDEX` shall be a valid alternative index.
    template <size_t t_INDEX>
    explicit variant(bsl::in_place_index_t<t_INDEX>);

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, which is direct-initialized from the specified
    /// `arg`.  If this `variant` is allocator-aware, the currently installed
    /// default allocator is used to supply memory.  `t_INDEX` shall be a valid
    /// alternative index.
    template <size_t t_INDEX, class t_ARG_01>
    explicit variant(bsl::in_place_index_t<t_INDEX>, const t_ARG_01& arg_01);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01&                arg_01,
                     const t_ARG_02&                arg_02);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01&                arg_01,
                     const t_ARG_02&                arg_02,
                     const t_ARG_03&                arg_03);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    explicit variant(bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     const t_ARG_10& arg_10);

    // allocator-extended constructors

    /// Create a `variant` object holding the 0th alternative, which is
    /// value-initialized.  If this `variant` is allocator-aware, the
    /// specified `allocator` is used to supply memory.
    variant(bsl::allocator_arg_t, allocator_type allocator);

    /// Create a `variant` object holding the same alternative as the
    /// specified `original` object.  If this `variant` is allocator-aware,
    /// the specified `allocator` is used to supply memory.  If the
    /// `original` object is not valueless by exception, the contained value
    /// is copy/move constructed from the contained value of `original`.
    /// All alternatives shall be copy/move constructible.
    variant(bsl::allocator_arg_t,
            allocator_type       allocator,
            const variant&       original);
    variant(bsl::allocator_arg_t,
            allocator_type                          allocator,
            BloombergLP::bslmf::MovableRef<variant> original);

    /// Create a `variant` object whose contained value is copy or move
    /// constructed from the specified `value`.  If (template parameter)
    /// `t_TYPE` is `bslmf::MovableRef<T>`, then the selected alternative is
    /// the one whose type is the same as `T` (modulo cv-qualification);
    /// otherwise, the selected alternative is the one that is the same as
    /// `t_TYPE` (modulo cv-qualification).  This constructor participates in
    /// overload resolution only if there is a unique such alternative.  If
    /// this `variant` is allocator-aware, the specified `allocator` is used to
    /// supply memory.
    template <class t_TYPE>
    variant(
          bsl::allocator_arg_t,
          allocator_type                                             allocator,
          const t_TYPE&                                              value,
          BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, t_TYPE));

    /// Create a `variant` object holding a contained value of type
    /// (template parameter) `t_TYPE`, which is value-initialized.  If this
    /// `variant` is allocator-aware, the currently installed default
    /// allocator is used to supply memory.  If this `variant` is
    /// allocator-aware, the specified `allocator` is used to supply memory.
    /// This constructor participates in overload resolution only if
    /// `t_TYPE` designates a unique alternative.
    template <class t_TYPE>
    explicit variant(
                   bsl::allocator_arg_t,
                   allocator_type                                    allocator,
                   bsl::in_place_type_t<t_TYPE>,
                   BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    /// Create a `variant` object holding a contained value of type (template
    /// parameter) `t_TYPE`, which is direct-initialized from the specified
    /// `arg`.  If this `variant` is allocator-aware, the specified `allocator`
    /// is used to supply memory.  This constructor participates in overload
    /// resolution only if `t_TYPE` designates a unique alternative.
    template <class t_TYPE, class t_ARG_01>
    explicit variant(
                   bsl::allocator_arg_t,
                   allocator_type                                    allocator,
                   bsl::in_place_type_t<t_TYPE>,
                   const t_ARG_01&                                   arg_01,
                   BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE, class t_ARG_01, class t_ARG_02>
    explicit variant(
                   bsl::allocator_arg_t,
                   allocator_type                                    allocator,
                   bsl::in_place_type_t<t_TYPE>,
                   const t_ARG_01&                                   arg_01,
                   const t_ARG_02&                                   arg_02,
                   BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    explicit variant(
                   bsl::allocator_arg_t,
                   allocator_type                                    allocator,
                   bsl::in_place_type_t<t_TYPE>,
                   const t_ARG_01&                                   arg_01,
                   const t_ARG_02&                                   arg_02,
                   const t_ARG_03&                                   arg_03,
                   BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<t_TYPE>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     const t_ARG_10& arg_10,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(t_TYPE));

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, which is value-initialized.  If this `variant` is
    /// allocator-aware, the specified `allocator` is used to supply memory.
    /// `t_INDEX` shall be a valid alternative index.
    template <size_t t_INDEX>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type                 allocator,
                     bsl::in_place_index_t<t_INDEX>);

    /// Create a `variant` object holding the alternative with index (template
    /// parameter) `t_INDEX`, which is direct-initialized from the specified
    /// `arg`.  If this `variant` is allocator-aware, the specified `allocator`
    /// is used to supply memory.  `t_INDEX` shall be a valid alternative
    /// index.
    template <size_t t_INDEX, class t_ARG_01>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type                 allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01&                arg_01);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type                 allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01&                arg_01,
                     const t_ARG_02&                arg_02);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type                 allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01&                arg_01,
                     const t_ARG_02&                arg_02,
                     const t_ARG_03&                arg_03);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<t_INDEX>,
                     const t_ARG_01& arg_01,
                     const t_ARG_02& arg_02,
                     const t_ARG_03& arg_03,
                     const t_ARG_04& arg_04,
                     const t_ARG_05& arg_05,
                     const t_ARG_06& arg_06,
                     const t_ARG_07& arg_07,
                     const t_ARG_08& arg_08,
                     const t_ARG_09& arg_09,
                     const t_ARG_10& arg_10);

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    // 20.7.3.2, destructor
    /// Destroy this object.  The contained value (if any) is destroyed.
    /// For simplicity of implementation, this method differs from the
    /// standard in the following ways:
    /// * conditional triviality is not implemented
    /// * 'constexpr' is not implemented
    //! ~variant() = default;

    // MANIPULATORS
    // 20.7.3.4, modifiers
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    /// Create an object of alternative (template parameter) `t_TYPE` in
    /// place, direct-initialized from the specified `args`, destroying any
    /// previously contained value first.  Return a reference to the newly
    /// created `t_TYPE` object.  If `t_TYPE` is allocator-aware, it uses
    /// the allocator specified upon the construction of this `variant`
    /// object to supply memory; passing an allocator argument to this
    /// method results in two allocators being passed to the alternative
    /// constructor, resulting in a likely compilation error.  If the
    /// constructor of `t_TYPE` exits via an exception, this object may be
    /// left in the valueless by exception state.  This method participates
    /// in overload resolution only if `t_TYPE` designates a unique
    /// alternative and is constructible from `args`.  For simplicity of
    /// implementation, this method differs from the standard in the
    /// following way:
    /// * `constexpr` is not implemented
    template <class t_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(t_TYPE) &&
                  std::is_constructible<t_TYPE, t_ARGS...>::value> >
    t_TYPE& emplace(t_ARGS&&... args)
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);
        Variant_Base::template baseEmplace<index>(
            std::forward<t_ARGS>(args)...);

        return bsl::get<index>(*this);
    }

    /// Create an object of alternative (template parameter) `t_TYPE` in
    /// place, direct-initialized from the specified `il` and `args`,
    /// destroying any previously contained value first.  Return a reference
    /// to the newly created `t_TYPE` object.  If `t_TYPE` is
    /// allocator-aware, it uses the allocator specified upon the
    /// construction of this `variant` object to supply memory; passing an
    /// allocator argument to this method results in two allocators being
    /// passed to the alternative constructor, resulting in a likely
    /// compilation error.  If the constructor of `t_TYPE` exits via an
    /// exception, this object may be left in the valueless by exception
    /// state.  This method participates in overload resolution only if
    /// `t_TYPE` designates a unique alternative and is constructible from
    /// `il` and `args`.  For simplicity of implementation, this method
    /// differs from the standard in the following way:
    /// * `constexpr` is not implemented
    template <class t_TYPE,
              class INIT_LIST_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(t_TYPE) &&
                  std::is_constructible<t_TYPE,
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        t_ARGS...>::value> >
    t_TYPE& emplace(std::initializer_list<INIT_LIST_TYPE> il, t_ARGS&&... args)
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);
        Variant_Base::template baseEmplace<index>(
            il, std::forward<t_ARGS>(args)...);

        return bsl::get<index>(*this);
    }

    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, direct-initialized from the specified `args`, destroying any
    /// previously contained value first.  Return a reference to the newly
    /// created contained value.  If the alternative is allocator-aware, it
    /// uses the allocator specified upon the construction of this `variant`
    /// object to supply memory; passing an allocator argument to this
    /// method results in two allocators being passed to the alternative
    /// constructor, resulting in a likely compilation error.  If the
    /// alternative constructor exits via an exception, this object may be
    /// left in the valueless by exception state.  This method participates
    /// in overload resolution only if `t_INDEX` is a valid alternative
    /// index and the designated alternative is constructible from `args`.
    /// For simplicity of implementation, this method differs from the
    /// standard in the following way:
    /// * `constexpr` is not implemented
    template <size_t   t_INDEX,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  (t_INDEX < 1 + sizeof...(t_TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(t_INDEX),
                                        t_ARGS...>::value> >
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(t_ARGS&&... args)
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        Variant_Base::template baseEmplace<t_INDEX>(
            std::forward<t_ARGS>(args)...);

        return bsl::get<t_INDEX>(*this);
    }

    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, direct-initialized from the specified `il` and args',
    /// destroying any previously contained value first.  Return a reference
    /// to the newly created contained value.  If the alternative is
    /// allocator-aware, it uses the allocator specified upon the
    /// construction of this `variant` object to supply memory; passing an
    /// allocator argument to this method results in two allocators being
    /// passed to the alternative constructor, resulting in a likely
    /// compilation error.  If the alternative constructor exits via an
    /// exception, this object may be left in the valueless by exception
    /// state.  This method participates in overload resolution only if
    /// `t_INDEX` is a valid alternative index and the designated
    /// alternative is constructible from `il` and `args`.  For simplicity
    /// of implementation, this method differs from the standard in the
    /// following way:
    /// * `constexpr` is not implemented
    template <size_t t_INDEX,
              class INIT_LIST_TYPE,
              class... t_ARGS,
              class = typename bsl::enable_if_t<
                  (t_INDEX < 1 + sizeof...(t_TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(t_INDEX),
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        t_ARGS...>::value> >
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(std::initializer_list<INIT_LIST_TYPE> il, t_ARGS&&... args)
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        Variant_Base::template baseEmplace<t_INDEX>(
            il, std::forward<t_ARGS>(args)...);

        return bsl::get<t_INDEX>(*this);
    }

#else   //  BSL_VARIANT_FULL_IMPLEMENTATION

    /// Create an object of alternative (template parameter) `t_TYPE` in
    /// place, which is value-initialized; any previously contained value is
    /// destroyed first.  Return a reference to the newly created `t_TYPE`
    /// object.  If `t_TYPE` is allocator-aware, it uses the allocator
    /// specified upon the construction of this `variant` object to supply
    /// memory; passing an allocator argument to this method results in two
    /// allocators being passed to the alternative constructor, resulting in
    /// a likely compilation error.  Note that if the constructor of
    /// `t_TYPE` exits via an exception, this object is left in a valueless
    /// by exception state.  This function participates in overload
    /// resolution only if `t_TYPE` is a unique alternative.
    template <class t_TYPE>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace();

    /// Create an object of alternative (template parameter) `t_TYPE` in
    /// place, which is direct-initialized from `arg`; any previously
    /// contained value is destroyed first.  Return a reference to the newly
    /// created `t_TYPE` object.  If `t_TYPE` is allocator-aware, it uses
    /// the allocator specified upon the construction of this `variant`
    /// object to supply memory; passing an allocator argument to this
    /// method results in two allocators being passed to the alternative
    /// constructor, resulting in a likely compilation error.  Note that if
    /// the constructor of `t_TYPE` exits via an exception, this object is
    /// left in a valueless by exception state.  This function participates
    /// in overload resolution only if `t_TYPE` is a unique alternative.
    template <class t_TYPE, class t_ARG_01>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01);

    template <class t_TYPE, class t_ARG_01, class t_ARG_02>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01, const t_ARG_02& arg_02);

    template <class t_TYPE, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07,
            const t_ARG_08& arg_08);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07,
            const t_ARG_08& arg_08,
            const t_ARG_09& arg_09);

    template <class t_TYPE,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, variant>::value,
        t_TYPE&>::type
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07,
            const t_ARG_08& arg_08,
            const t_ARG_09& arg_09,
            const t_ARG_10& arg_10);

    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, which is value-initialized; any previously contained value is
    /// destroyed first.  Return a reference to the newly created contained
    /// value.  If the alternative is allocator-aware, it uses the allocator
    /// specified upon the construction of this `variant` object to supply
    /// memory; passing an allocator argument to this method results in two
    /// allocators being passed to the alternative constructor, resulting in
    /// a likely compilation error.  Note that if the alternative
    /// constructor exits via an exception, this object is left in the
    /// valueless by exception state.  `t_INDEX` shall be a valid
    /// alternative index.
    template <size_t t_INDEX>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace();

    /// Create the alternative with index (template parameter) `t_INDEX` in
    /// place, direct-initialized from the specified `arg`; any previously
    /// contained value is destroyed first.  Return a reference to the newly
    /// created contained value.  If the alternative is allocator-aware, it
    /// uses the allocator specified upon the construction of this `variant`
    /// object to supply memory; passing an allocator argument to this
    /// method results in two allocators being passed to the alternative
    /// constructor, resulting in a likely compilation error.  Note that if
    /// the alternative constructor exits via an exception, this object is
    /// left in the valueless by exception state.  `t_INDEX` shall be a
    /// valid alternative index.
    template <size_t t_INDEX, class t_ARG_01>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01, const t_ARG_02& arg_02);

    template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07,
            const t_ARG_08& arg_08);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07,
            const t_ARG_08& arg_08,
            const t_ARG_09& arg_09);

    template <size_t t_INDEX,
              class t_ARG_01,
              class t_ARG_02,
              class t_ARG_03,
              class t_ARG_04,
              class t_ARG_05,
              class t_ARG_06,
              class t_ARG_07,
              class t_ARG_08,
              class t_ARG_09,
              class t_ARG_10>
    typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
    emplace(const t_ARG_01& arg_01,
            const t_ARG_02& arg_02,
            const t_ARG_03& arg_03,
            const t_ARG_04& arg_04,
            const t_ARG_05& arg_05,
            const t_ARG_06& arg_06,
            const t_ARG_07& arg_07,
            const t_ARG_08& arg_08,
            const t_ARG_09& arg_09,
            const t_ARG_10& arg_10);
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    // MANIPULATORS
    // 20.7.3.3, assignment
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    /// If `rhs` holds the same alternative type as this object, copy assign
    /// the contained value of `rhs` to the contained value of this object.
    /// Otherwise, destroy the contained value of this object (if any) and, if
    /// `rhs` holds a value, copy-construct the corresponding alternative of
    /// this object from the contained value of `rhs`.  The allocators of this
    /// object and `rhs` both remain unchanged.  This operator is deleted
    /// unless all alternatives are copy constructible and copy assignable.  If
    /// the construction of a new alternative object exits via an exception,
    /// this `variant` object is left in a valueless by exception state.  This
    /// is different from the standard, which requires a temporary copy of
    /// `rhs` to be made if copy construction of the active alternative is not
    /// `noexcept` (see [variant.assign] for details).  The standard behavior
    /// causes unnecessary performance degradation in cases where the
    /// alternative constructor does not throw, yet is not marked `noexcept`;
    /// this behavior is therefore not implemented in `bsl::variant`.  For
    /// simplicity of implementation, this method also differs from the
    /// standard in the following ways:
    /// * conditional triviality is not implemented
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    variant& operator=(const variant& rhs) = default;

    /// If `rhs` holds the same alternative type as this object, move assign
    /// the contained value of `rhs` to the contained value of this object.
    /// Otherwise, destroy the contained value of this object (if any) and, if
    /// `rhs` holds a value, move-construct the corresponding alternative of
    /// this object from the contained value of `rhs`.  The allocators of this
    /// object and `rhs` both remain unchanged.  If the construction of a new
    /// alternative object throws an exception, this `variant` object is left
    /// in a valueless by exception state.  This operator participes in
    /// overload resolution only if all alternatives are move constructible and
    /// mvoe assignable.  For simplicity of implementation, this method differs
    /// from the standard in the following ways:
    /// * conditional triviality is not implemented
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    variant& operator=(variant&& rhs) = default;

    /// Assign to this object the specified `value`.  The alternative
    /// corresponding to `value` is the best match among all alternatives
    /// for which the expression
    /// `t_ALT_TYPE x[] = {std::forward<t_TYPE>(value)};` is well formed,
    /// and this operator participates in overload resolution only if there
    /// is a unique best matching alternative and that alternative is both
    /// constructible and assignable from `value`.  If this `variant`
    /// already holds the alternative corresponding to `value`, the
    /// contained value is assigned to from `value`; otherwise, any
    /// contained value is destroyed and the alternative corresponding to
    /// `value` is direct-initialized from `value`.  Note that if the
    /// construction of a new alternative object exits via an exception,
    /// this `variant` object may be left in a valueless by exception state.
    /// This is different from the standard, which requires a temporary
    /// alternative object to be constructed if such construction is not
    /// `noexcept` (see [variant.assign] for details).  The standard
    /// behavior causes unnecessary performance degradation in cases where
    /// the alternative constructor does not throw, yet is not marked
    /// `noexcept`; this behavior is therefore not implemented in
    /// `bsl::variant`.  For simplicity of implementation, this method also
    /// differs from the standard in the following ways:
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    template <class t_TYPE>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_AssignsFromType<variant, t_TYPE>::value,
        variant&>::type
    operator=(t_TYPE&& value)
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        const size_t altIndex =
            BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE, variant);

        if (index() == altIndex) {
            bsl::get<altIndex>(*this) = std::forward<t_TYPE>(value);
        }
        else {
            // 'altIndex' can not be 'variant_npos' if
            // 'Variant_AssignsFromType' is satisfied
            Variant_Base::template baseEmplace<altIndex>(
                std::forward<t_TYPE>(value));
        }

        return *this;
    }
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
    /// If 'rhs' holds the same alternative type as this object, copy assign
    /// the contained value of 'rhs' to the contained value of this object.
    /// Otherwise, destroy the contained value of this object (if any) and, if
    /// 'rhs' holds a value, copy-construct the corresponding alternative of
    /// this object from the contained value of 'rhs'.  The allocators of this
    /// object and 'rhs' both remain unchanged.  If the construction of a new
    /// alternative object exits via an exception, this 'variant' object is
    /// left in a valueless by exception state.  All alternatives shall be copy
    /// constructible and copy assignable.
    variant& operator=(const variant& rhs);

    /// If `rhs` holds the same alternative type as this object, move assign
    /// the contained value of `rhs` to the contained value of this object.
    /// Otherwise, destroy the contained value of this object (if any) and, if
    /// `rhs` holds a value, move-construct the corresponding alternative of
    /// this object from the contained value of `rhs`.  The allocators of this
    /// object and `rhs` both remain unchanged.  If the construction of a new
    /// alternative object throws an exception, this `variant` object is left
    /// in a valueless by exception state.  All alternatives shall be move
    /// constructible and move assignable.
    variant& operator=(BloombergLP::bslmf::MovableRef<variant> rhs);

    /// Assign to this object the specified `value`.  If (template parameter)
    /// `t_TYPE` is `bslmf::MovableRef<T>`, then the alternative corresponding
    /// to `value` is the one whose type is the same as `T` (modulo
    /// cv-qualification); otherwise, the selected alternative is the one that
    /// is the same as `t_TYPE` (modulo cv-qualification).  This operator
    /// participates in overload resolution only if there is a unique such
    /// alternative.  If this `variant` object already holds that alternative,
    /// then the contained value is assigned from `value`; otherwise, any
    /// contained value is destroyed and the selected alternative is
    /// direct-initialized from `value`.  The selected alternative shall be
    /// constructible and assignable from `value`.  If the construction of a
    /// new alternative object exits via an exception, this `variant` object is
    /// left in the valueless by exception state.
    template <class t_TYPE>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_AssignsFromType<variant, t_TYPE>::value,
        variant&>::type
    operator=(const t_TYPE& value);

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    // 20.7.3.6, swap

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee if the two swapped objects contain the
    /// same alternative and if that alternative `t_TYPE` provides that
    /// guarantee.  If `*this` and `other` do not have the same active
    /// alternative and this method exits via an exception, either or both
    /// `variant` objects may be left in a valueless state or with an
    /// contained value in a moved-from state.  The behavior is undefined
    /// unless `*this` has the same allocator as `other`.  All alternatives
    /// shall be move constructible and swappable.  For simplicity of
    /// implementation, this method differs from the standard in the
    /// following ways:
    /// * `constexpr` is not implemented
    /// * `noexcept` specification is not implemented
    void swap(variant& other);

    // ACCESSORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    template <bool t_USES_BSLMA_ALLOC = BloombergLP::bslstl::
                  Variant_UsesBslmaAllocatorAny<t_HEAD, t_TAIL...>::value,
              class = typename bsl::enable_if_t<t_USES_BSLMA_ALLOC> >
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    /// Return a copy of this object's allocator.  At least one alternative of
    /// this `variant` object shall be allocator-aware; in C++03, due to
    /// language limitations, this method can be called even if no alternative
    /// is allocator-aware, but returns a non-allocator type in that case.
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        return allocator_type(Variant_Base::AllocBase::mechanism());
    }

    // 20.7.3.5, value status

    /// Return the index of the alternative currently managed by this
    /// `variant` object, or `bsl::variant_npos` if this object is valueless
    /// by exception.  This method differs from the standard in the
    /// following way:
    /// * `constexpr` is not implemented
    /// This is because no constructors are currently constexpr and there is no
    /// way to test the constexpr property of this function.
    size_t index() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `false` if there is an alternative object currently managed
    /// by this `variant` object, and `true` otherwise.  A `variant` object
    /// can become valueless by exception if the creation of an alternative
    /// object exits via an exception, or if it is copied or assigned from
    /// another `variant` object that is valueless by exception.  This
    /// method differs from the standard in the following way:
    /// * `constexpr` is not implemented
    /// This is because no constructors are currently constexpr and there is no
    /// way to test the constexpr property of this function.
    bool valueless_by_exception() const BSLS_KEYWORD_NOEXCEPT;
};

#endif

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslstl {

                           // ---------------------
                           // class Variant_ImpUtil
                           // ---------------------

template <class t_RET, class t_VARIANT_UNION>
t_RET& Variant_ImpUtil::getAlternative(
                   bsl::in_place_index_t<0>,
                   t_VARIANT_UNION&         variantUnion) BSLS_KEYWORD_NOEXCEPT
{
    return variantUnion.d_head.value();
}

template <class t_RET, size_t t_INDEX, class t_VARIANT_UNION>
t_RET& Variant_ImpUtil::getAlternative(
             bsl::in_place_index_t<t_INDEX>,
             t_VARIANT_UNION&               variantUnion) BSLS_KEYWORD_NOEXCEPT
{
    return getAlternative<t_RET>(bsl::in_place_index_t<t_INDEX - 1>(),
                                 variantUnion.d_tail);
}

template <class t_RET, size_t t_INDEX, class t_VARIANT>
t_RET& Variant_ImpUtil::get(t_VARIANT& variant)
{
    if (variant.index() != t_INDEX) {
        BSLS_THROW(bsl::bad_variant_access());
    }

    return getAlternative<t_RET>(bsl::in_place_index_t<t_INDEX>(),
                                 variant.d_union);
}

template <class t_RET, size_t t_INDEX, class t_VARIANT>
t_RET& Variant_ImpUtil::get(const t_VARIANT& variant)
{
    if (variant.index() != t_INDEX) {
        BSLS_THROW(bsl::bad_variant_access());
    }

    return getAlternative<t_RET>(bsl::in_place_index_t<t_INDEX>(),
                                 variant.d_union);
}

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::visit(t_VISITOR&& visitor, t_VARIANT&& variant)
{
    static constexpr size_t varSize =
        bsl::variant_size<bsl::remove_reference_t<t_VARIANT> >::value;

    typedef typename Variant_VTable<
        t_RET,
        t_VISITOR&&,
        t_VARIANT&&,
        bslmf::MakeIntegerSequence<std::size_t, varSize> >::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 't_VISITOR' and
    // 't_VARIANT', then invoke the one corresponding to the active index.
    FuncPtr funcPtr =
        Variant_VTable<t_RET,
                       t_VISITOR&&,
                       t_VARIANT&&,
                       bslmf::MakeIntegerSequence<std::size_t, varSize> >::
            s_map[variant.index()];

    return (*funcPtr)(std::forward<t_VISITOR>(visitor),
                      std::forward<t_VARIANT>(variant));
}
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::visitId(t_VISITOR&& visitor, t_VARIANT&& variant)
{
    static constexpr size_t varSize =
        bsl::variant_size<bsl::remove_reference_t<t_VARIANT> >::value;

    typedef typename Variant_VTableId<
        t_RET,
        t_VISITOR&&,
        t_VARIANT&&,
        bslmf::MakeIntegerSequence<std::size_t, varSize> >::FuncPtr FuncPtr;

    FuncPtr funcPtr =
        Variant_VTableId<t_RET,
                         t_VISITOR&&,
                         t_VARIANT&&,
                         bslmf::MakeIntegerSequence<std::size_t, varSize> >::
            s_mapId[variant.index()];

    return (*funcPtr)(std::forward<t_VISITOR>(visitor),
                      std::forward<t_VARIANT>(variant));
}

#else
template <class t_RET, size_t t_INDEX, class t_VARIANT>
t_RET& Variant_ImpUtil::unsafeGet(t_VARIANT& variant)
{
    return getAlternative<t_RET>(bsl::in_place_index_t<t_INDEX>(),
                                 variant.d_union);
}

template <class t_RET, size_t t_INDEX, class t_VARIANT>
t_RET& Variant_ImpUtil::unsafeGet(const t_VARIANT& variant)
{
    return getAlternative<t_RET>(bsl::in_place_index_t<t_INDEX>(),
                                 variant.d_union);
}

template <class t_TYPE, class t_VARIANT>
t_TYPE& Variant_ImpUtil::unsafeGet(t_VARIANT& obj)
{
    return unsafeGet<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(
                                                                          obj);
}

template <class t_TYPE, class t_VARIANT>
const t_TYPE& Variant_ImpUtil::unsafeGet(const t_VARIANT& obj)
{
    return unsafeGet<const t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(
                                                                          obj);
}

template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::visit(t_VISITOR& visitor, t_VARIANT& variant)
{
    typedef
        typename Variant_VTable<t_RET, t_VISITOR, t_VARIANT>::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 't_VISITOR' and
    // 't_VARIANT', then invoke the one corresponding to the active index.
    FuncPtr funcPtr =
        Variant_VTable<t_RET, t_VISITOR, t_VARIANT>::map()[variant.index()];

    return (*funcPtr)(visitor, variant);
}

template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::visit(t_VISITOR& visitor, const t_VARIANT& variant)
{
    typedef typename Variant_VTable<t_RET, t_VISITOR, const t_VARIANT>::FuncPtr
        FuncPtr;

    // Generate the table of all function pointers for 't_VISITOR' and
    // 't_VARIANT', then invoke the one corresponding to the active index.
    FuncPtr funcPtr =
        Variant_VTable<t_RET, t_VISITOR, const t_VARIANT>::map()[variant
                                                                     .index()];

    return (*funcPtr)(visitor, variant);
}

template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::moveVisit(t_VISITOR& visitor, t_VARIANT& variant)
{
    typedef
        typename Variant_VTable<t_RET, t_VISITOR, t_VARIANT>::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 't_VISITOR' and
    // 't_VARIANT', then invoke the one corresponding to the active index.
    FuncPtr funcPtr =
        Variant_VTable<t_RET, t_VISITOR, t_VARIANT>::moveMap()[variant
                                                                   .index()];

    return (*funcPtr)(visitor, variant);
}

// The visitation functions below are the same as the ones above except that
// the visitor is invoked with an additional argument of type
// 'bsl::in_place_index_t'.  They are used internally for visitors that
// participate in the 'variant' implementation.
template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::visitId(t_VISITOR& visitor, t_VARIANT& variant)
{
    typedef
        typename Variant_VTable<t_RET, t_VISITOR, t_VARIANT>::FuncPtr FuncPtr;

    FuncPtr funcPtr =
        Variant_VTable<t_RET, t_VISITOR, t_VARIANT>::mapId()[variant.index()];

    return (*funcPtr)(visitor, variant);
}

template <class t_RET, class t_VISITOR, class t_VARIANT>
t_RET Variant_ImpUtil::visitId(t_VISITOR& visitor, const t_VARIANT& variant)
{
    typedef typename Variant_VTable<t_RET, t_VISITOR, const t_VARIANT>::FuncPtr
        FuncPtr;

    FuncPtr funcPtr = Variant_VTable<t_RET, t_VISITOR, const t_VARIANT>::
        mapId()[variant.index()];

    return (*funcPtr)(visitor, variant);
}

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
               // ----------------------------------------------
               // class Variant_ImpUtil::ConstructFromStdVisitor
               // ----------------------------------------------

template <class t_VARIANT, class t_STD_VARIANT>
Variant_ImpUtil::ConstructFromStdVisitor<t_VARIANT, t_STD_VARIANT>::
    ConstructFromStdVisitor(t_VARIANT& target, t_STD_VARIANT& original)
: d_target(target)
, d_original(original)
{
}

template <class t_VARIANT, class t_STD_VARIANT>
template <size_t t_INDEX, class t_TYPE>
void
Variant_ImpUtil::ConstructFromStdVisitor<t_VARIANT, t_STD_VARIANT>::
    operator()(bsl::in_place_index_t<t_INDEX>, t_TYPE&) const
{
    // Implementation note: calling the correct overload of 'operator()' is
    // made possible only by the fact that we have set 'd_target.d_type' to
    // 'd_original.index()' prior to invoking the visitor.  But we must now use
    // private access to 'd_target' to set the index to -1 before we call
    // 'baseEmplace' (otherwise, 'baseEmplace' would attempt to destroy the
    // alternative first before constructing a new one).

    d_target.d_type = bsl::variant_npos;
    d_target.template baseEmplace<t_INDEX>(
               std::get<t_INDEX>(std::forward<t_STD_VARIANT>(d_original)));
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

                          // ------------------------
                          // class Variant_NoSuchType
                          // ------------------------

// CREATORS
inline
BSLS_KEYWORD_CONSTEXPR Variant_NoSuchType::Variant_NoSuchType(
                                                     int) BSLS_KEYWORD_NOEXCEPT
{
}

                           // ---------------------
                           // class Variant_DataImp
                           // ---------------------

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
// CREATORS
template <class t_TYPE>
template <class... t_ARGS>
inline
Variant_DataImp<t_TYPE>::Variant_DataImp(t_ARGS&&... args)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(), std::forward<t_ARGS>(args)...);
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

// MANIPULATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 t_TYPE& Variant_DataImp<t_TYPE>::value() &
{
    return d_buffer.object();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 t_TYPE&& Variant_DataImp<t_TYPE>::value() &&
{
    return std::move(d_buffer.object());
}
#else   // BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_TYPE>
inline
t_TYPE& Variant_DataImp<t_TYPE>::value()
{
    return d_buffer.object();
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

// ACCESSORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 const t_TYPE& Variant_DataImp<t_TYPE>::value()
const&
{
    return d_buffer.object();
}

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 const t_TYPE&& Variant_DataImp<t_TYPE>::value()
const&&
{
    return std::move(d_buffer.object());
}
#else   // BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_TYPE>
inline
const t_TYPE& Variant_DataImp<t_TYPE>::value() const
{
    return d_buffer.object();
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

/// This component-private function swaps the values of the specified `lhs`
/// and `rhs` when the type (template parameter) `t_VARIANT` is an
/// allocator-aware variant.
template <class t_VARIANT>
void variant_swapImpl(bsl::true_type, t_VARIANT& lhs, t_VARIANT& rhs)
{
    if (lhs.get_allocator() == rhs.get_allocator()) {
        lhs.swap(rhs);
        return;                                                       // RETURN
    }

    t_VARIANT futureLhs(bsl::allocator_arg, lhs.get_allocator(), rhs);
    t_VARIANT futureRhs(bsl::allocator_arg, rhs.get_allocator(), lhs);

    futureLhs.swap(lhs);
    futureRhs.swap(rhs);
}

/// This component-private function swaps the values of the specified `lhs`
/// and `rhs` when the type (template parameter) `t_VARIANT` is a
/// non-allocator-aware variant.
template <class t_VARIANT>
void variant_swapImpl(bsl::false_type, t_VARIANT& lhs, t_VARIANT& rhs)
{
    lhs.swap(rhs);
}

                            // -------------------
                            // struct Variant_Base
                            // -------------------

// CREATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base()
: AllocBase()
, d_type(0)
, d_union(bsl::in_place_index_t<0>(), AllocBase::mechanism())
{
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(const Variant_Base& original)
: AllocBase()
, d_type(bsl::variant_npos)
{
    if (original.d_type != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
            copyConstructor(this);
        BSLSTL_VARIANT_VISITID(
            void, copyConstructor, static_cast<const Variant&>(original));
    }
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(Variant_Base&& original)
: AllocBase(original)
, d_type(bsl::variant_npos)
{
    if (original.d_type != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(this);
        BSLSTL_VARIANT_VISITID(
            void, moveConstructor, static_cast<Variant&>(original));
    }
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_HEAD, class... t_TAIL>
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(Variant_ConstructFromStdTag,
                                              size_t index)
: AllocBase()
, d_type(index)
{
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type       allocator)
: AllocBase(allocator)
, d_type(0)
, d_union(bsl::in_place_index_t<0>(), AllocBase::mechanism())
{
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type       allocator,
                                              const Variant&       original)
: AllocBase(allocator)
, d_type(bsl::variant_npos)
{
    if (original.index() != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
            copyConstructor(this);
        BSLSTL_VARIANT_VISITID(void, copyConstructor, original);
    }
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type       allocator,
                                              Variant&&            original)
: AllocBase(allocator)
, d_type(bsl::variant_npos)
{
    if (original.index() != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(this);
        BSLSTL_VARIANT_VISITID(void, moveConstructor, original);
    }
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_HEAD, class... t_TAIL>
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              Variant_ConstructFromStdTag,
                                              size_t         index)
: AllocBase(allocator)
, d_type(index)
{
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class... t_ARGS>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              t_ARGS&&... args)
: d_type(t_INDEX)
, d_union(bsl::in_place_index_t<t_INDEX>(),
          AllocBase::mechanism(),
          std::forward<t_ARGS>(args)...)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class... t_ARGS>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              t_ARGS&&... args)
: AllocBase(allocator)
, d_type(t_INDEX)
, d_union(bsl::in_place_index_t<t_INDEX>(),
          AllocBase::mechanism(),
          std::forward<t_ARGS>(args)...)
{
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base()
: AllocBase()
, d_type(0)
{
    typedef typename bsl::remove_cv<t_HEAD>::type Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism());
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(const Variant& original)
: AllocBase()
, d_type(bsl::variant_npos)
{
    if (!original.valueless_by_exception()) {
        BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
            copyConstructor(this);
        BSLSTL_VARIANT_VISITID(void, copyConstructor, original);
            // In C++03, if there are nonunique alternatives,
            // 'BSLSTL_VARIANT_VISITID' will always use the first nonunique
            // alternative, which may result in the wrong 'd_type' being set.
        d_type = original.d_type;
    }
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                              BloombergLP::bslmf::MovableRef<Variant> original)
: AllocBase(MoveUtil::access(original))
, d_type(bsl::variant_npos)
{
    Variant& lvalue = original;
    if (!lvalue.valueless_by_exception()) {
        BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(this);
        BSLSTL_VARIANT_VISITID(void, moveConstructor, lvalue);
            // In C++03, if there are nonunique alternatives,
            // 'BSLSTL_VARIANT_VISITID' will always use the first nonunique
            // alternative, which may result in the wrong 'd_type' being set.
        d_type = lvalue.d_type;
    }
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism());
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                         bsl::in_place_index_t<t_INDEX>,
                                         const t_ARG_01&                arg_01)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                         bsl::in_place_index_t<t_INDEX>,
                                         const t_ARG_01&                arg_01,
                                         const t_ARG_02&                arg_02)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                         bsl::in_place_index_t<t_INDEX>,
                                         const t_ARG_01&                arg_01,
                                         const t_ARG_02&                arg_02,
                                         const t_ARG_03&                arg_03)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07,
                                              const t_ARG_08& arg_08)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07,
                                              const t_ARG_08& arg_08,
                                              const t_ARG_09& arg_09)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08,
        arg_09);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07,
                                              const t_ARG_08& arg_08,
                                              const t_ARG_09& arg_09,
                                              const t_ARG_10& arg_10)
: AllocBase()
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08,
        arg_09,
        arg_10);
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type       allocator)
: AllocBase(allocator)
, d_type(0)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<t_HEAD *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism());
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type       allocator,
                                              const Variant&       original)
: AllocBase(allocator)
, d_type(bsl::variant_npos)
{
    if (!original.valueless_by_exception()) {
        BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
            copyConstructor(this);
        BSLSTL_VARIANT_VISITID(void, copyConstructor, original);

        // In C++03, if there are nonunique alternatives,
        // 'BSLSTL_VARIANT_VISITID' will always use the first nonunique
        // alternative, which may result in the wrong 'd_type' being set.
        d_type = original.d_type;
    }
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                             bsl::allocator_arg_t,
                             allocator_type                          allocator,
                             BloombergLP::bslmf::MovableRef<Variant> original)
: AllocBase(allocator)
, d_type(bsl::variant_npos)
{
    Variant& lvalue = original;
    if (!lvalue.valueless_by_exception()) {
        BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(this);
        BSLSTL_VARIANT_VISITID(void, moveConstructor, lvalue);

        // In C++03, if there are nonunique alternatives,
        // 'BSLSTL_VARIANT_VISITID' will always use the first nonunique
        // alternative, which may result in the wrong 'd_type' being set.
        d_type = lvalue.d_type;
    }
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                      bsl::allocator_arg_t,
                                      allocator_type                 allocator,
                                      bsl::in_place_index_t<t_INDEX>)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism());
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                      bsl::allocator_arg_t,
                                      allocator_type                 allocator,
                                      bsl::in_place_index_t<t_INDEX>,
                                      const t_ARG_01&                arg_01)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                      bsl::allocator_arg_t,
                                      allocator_type                 allocator,
                                      bsl::in_place_index_t<t_INDEX>,
                                      const t_ARG_01&                arg_01,
                                      const t_ARG_02&                arg_02)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(
                                      bsl::allocator_arg_t,
                                      allocator_type                 allocator,
                                      bsl::in_place_index_t<t_INDEX>,
                                      const t_ARG_01&                arg_01,
                                      const t_ARG_02&                arg_02,
                                      const t_ARG_03&                arg_03)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07,
                                              const t_ARG_08& arg_08)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07,
                                              const t_ARG_08& arg_08,
                                              const t_ARG_09& arg_09)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08,
        arg_09);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
inline
Variant_Base<t_HEAD, t_TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                              allocator_type allocator,
                                              bsl::in_place_index_t<t_INDEX>,
                                              const t_ARG_01& arg_01,
                                              const t_ARG_02& arg_02,
                                              const t_ARG_03& arg_03,
                                              const t_ARG_04& arg_04,
                                              const t_ARG_05& arg_05,
                                              const t_ARG_06& arg_06,
                                              const t_ARG_07& arg_07,
                                              const t_ARG_08& arg_08,
                                              const t_ARG_09& arg_09,
                                              const t_ARG_10& arg_10)
: AllocBase(allocator)
, d_type(t_INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *>(BSLS_UTIL_ADDRESSOF(d_union)),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08,
        arg_09,
        arg_10);
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
Variant_Base<t_HEAD, t_TAIL...>::~Variant_Base()
{
    reset();
}
#endif
// MANIPULATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class... t_ARGS>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(t_ARGS&&... args)
{
    reset();

    // We need to assemble the construction arguments for the underlying
    //'Variant_DataImp' object here.  They require that allocator pointer is
    // the leading argument after 'Variant_Union' strips the 't_INDEX'.  To be
    // able to get such argument set, we say that 'Variant_Union' is not
    // allocator aware.  If we were to have 'Variant_Union' be allocator aware
    // and use trailing allocator construction, we wouldn't be able to extract
    // the allocator to move it to the right position for the 'Variant_DataImp'
    // construct invocation.  If we were to have either 'Variant_Union' or
    // 'Variant_DataImp' use 'allocator_arg_t' semantics, it wouldn't work for
    // the non allocator aware variant where there is no allocator object.
    BloombergLP::bslma::ConstructionUtil::construct(
        BSLS_UTIL_ADDRESSOF(d_union),
        (void *)0,
        bsl::in_place_index_t<t_INDEX>{},
        AllocBase::mechanism(),
        std::forward<t_ARGS>(args)...);
    d_type = t_INDEX;
}

#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace()
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism());
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04,
                                                  const t_ARG_05& arg_05)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04,
                                                  const t_ARG_05& arg_05,
                                                  const t_ARG_06& arg_06)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04,
                                                  const t_ARG_05& arg_05,
                                                  const t_ARG_06& arg_06,
                                                  const t_ARG_07& arg_07)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04,
                                                  const t_ARG_05& arg_05,
                                                  const t_ARG_06& arg_06,
                                                  const t_ARG_07& arg_07,
                                                  const t_ARG_08& arg_08)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04,
                                                  const t_ARG_05& arg_05,
                                                  const t_ARG_06& arg_06,
                                                  const t_ARG_07& arg_07,
                                                  const t_ARG_08& arg_08,
                                                  const t_ARG_09& arg_09)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08,
        arg_09);
    d_type = t_INDEX;
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
void Variant_Base<t_HEAD, t_TAIL...>::baseEmplace(const t_ARG_01& arg_01,
                                                  const t_ARG_02& arg_02,
                                                  const t_ARG_03& arg_03,
                                                  const t_ARG_04& arg_04,
                                                  const t_ARG_05& arg_05,
                                                  const t_ARG_06& arg_06,
                                                  const t_ARG_07& arg_07,
                                                  const t_ARG_08& arg_08,
                                                  const t_ARG_09& arg_09,
                                                  const t_ARG_10& arg_10)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<t_INDEX, Variant>::type>::type
        Alt_Type;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
        reinterpret_cast<Alt_Type *> BSLS_UTIL_ADDRESSOF(d_union),
        AllocBase::mechanism(),
        arg_01,
        arg_02,
        arg_03,
        arg_04,
        arg_05,
        arg_06,
        arg_07,
        arg_08,
        arg_09,
        arg_10);
    d_type = t_INDEX;
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>& Variant_Base<t_HEAD, t_TAIL...>::operator=(
                                                       const Variant_Base& rhs)
{
    if (&rhs != this) {
        if (this->d_type == rhs.d_type) {
            if (d_type != bsl::variant_npos) {
                Variant& self = static_cast<Variant&>(*this);
                BloombergLP::bslstl::Variant_CopyAssignVisitor<Variant>
                copyAssign(BSLS_UTIL_ADDRESSOF(self));
                BSLSTL_VARIANT_VISITID(
                    void, copyAssign, static_cast<const Variant&>(rhs));
            }
        }
        else {
            reset();
            if (rhs.d_type != bsl::variant_npos) {
                BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
                    copyConstructor(this);
                BSLSTL_VARIANT_VISITID(
                    void, copyConstructor, static_cast<const Variant&>(rhs));
            }
            d_type = rhs.d_type;
        }
    }
    return *this;
}

template <class t_HEAD, class... t_TAIL>
inline
Variant_Base<t_HEAD, t_TAIL...>& Variant_Base<t_HEAD, t_TAIL...>::operator=(
                              BloombergLP::bslmf::MovableRef<Variant_Base> rhs)
{
    Variant_Base& lvalue = rhs;
    if (&lvalue != this) {
        if (this->d_type == lvalue.d_type) {
            if (d_type != bsl::variant_npos) {
                Variant& self = static_cast<Variant&>(*this);
                BloombergLP::bslstl::Variant_MoveAssignVisitor<Variant>
                moveAssign(BSLS_UTIL_ADDRESSOF(self));
                BSLSTL_VARIANT_VISITID(
                    void, moveAssign, static_cast<Variant&>(lvalue));
            }
        }
        else {
            reset();
            if (lvalue.d_type != bsl::variant_npos) {
                BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
                    moveConstructor(this);
                BSLSTL_VARIANT_VISITID(
                    void, moveConstructor, static_cast<Variant&>(lvalue));
            }
            d_type = lvalue.d_type;
        }
    }
    return *this;
}
template <class t_HEAD, class... t_TAIL>
void Variant_Base<t_HEAD, t_TAIL...>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_type != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_DestructorVisitor destructor;
        bsl::visit(destructor, static_cast<Variant&>(*this));
        d_type = bsl::variant_npos;
    }
}
#endif
}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                               // -------------
                               // class variant
                               // -------------

// CREATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
variant<t_HEAD, t_TAIL...>::variant(
    t_TYPE&& value,
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE,
                                                            variant)>(),
      std::forward<t_TYPE>(value))
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    t_TYPE&&       value,
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t{},
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE,
                                                            variant)>(),
      std::forward<t_TYPE>(value))
{
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>::variant()
: Variant_Base()
{
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>::variant(const variant& original)
: Variant_Base(original)
{
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>::variant(
                              BloombergLP::bslmf::MovableRef<variant> original)
: Variant_Base(original)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
variant<t_HEAD, t_TAIL...>::variant(
    const t_TYPE& value,
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE,
                                                            variant)>(),
      value)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>())
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01, class t_ARG_02>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01, class t_ARG_02, class t_ARG_03>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    const t_ARG_08& arg_08,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07,
      arg_08)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    const t_ARG_08& arg_08,
    const t_ARG_09& arg_09,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07,
      arg_08,
      arg_09)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    const t_ARG_08& arg_08,
    const t_ARG_09& arg_09,
    const t_ARG_10& arg_10,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07,
      arg_08,
      arg_09,
      arg_10)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>)
: Variant_Base(bsl::in_place_index_t<t_INDEX>())
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01&                arg_01)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(), arg_01)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01&                arg_01,
                                    const t_ARG_02&                arg_02)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(), arg_01, arg_02)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01&                arg_01,
                                    const t_ARG_02&                arg_02,
                                    const t_ARG_03&                arg_03)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(), arg_01, arg_02, arg_03)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07,
               arg_08)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07,
               arg_08,
               arg_09)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09,
                                    const t_ARG_10& arg_10)
: Variant_Base(bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07,
               arg_08,
               arg_09,
               arg_10)
{
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type       allocator)
: Variant_Base(bsl::allocator_arg_t(), allocator)
{
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type       allocator,
                                    const variant&       original)
: Variant_Base(bsl::allocator_arg_t(), allocator, original)
{
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>::variant(
                             bsl::allocator_arg_t,
                             allocator_type                          allocator,
                             BloombergLP::bslmf::MovableRef<variant> original)
: Variant_Base(bsl::allocator_arg_t(), allocator, original)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    const t_TYPE&  value,
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE,
                                                            variant)>(),
      value)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>())
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01, class t_ARG_02>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01, class t_ARG_02, class t_ARG_03>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    const t_ARG_08& arg_08,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07,
      arg_08)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    const t_ARG_08& arg_08,
    const t_ARG_09& arg_09,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07,
      arg_08,
      arg_09)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
inline
variant<t_HEAD, t_TAIL...>::variant(
    bsl::allocator_arg_t,
    allocator_type allocator,
    bsl::in_place_type_t<t_TYPE>,
    const t_ARG_01& arg_01,
    const t_ARG_02& arg_02,
    const t_ARG_03& arg_03,
    const t_ARG_04& arg_04,
    const t_ARG_05& arg_05,
    const t_ARG_06& arg_06,
    const t_ARG_07& arg_07,
    const t_ARG_08& arg_08,
    const t_ARG_09& arg_09,
    const t_ARG_10& arg_10,
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(t_TYPE))
: Variant_Base(
      bsl::allocator_arg_t(),
      allocator,
      bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant)>(),
      arg_01,
      arg_02,
      arg_03,
      arg_04,
      arg_05,
      arg_06,
      arg_07,
      arg_08,
      arg_09,
      arg_10)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>())
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07,
               arg_08)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07,
               arg_08,
               arg_09)
{
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
inline
variant<t_HEAD, t_TAIL...>::variant(bsl::allocator_arg_t,
                                    allocator_type allocator,
                                    bsl::in_place_index_t<t_INDEX>,
                                    const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09,
                                    const t_ARG_10& arg_10)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<t_INDEX>(),
               arg_01,
               arg_02,
               arg_03,
               arg_04,
               arg_05,
               arg_06,
               arg_07,
               arg_08,
               arg_09,
               arg_10)
{
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace()
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>();

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg_01);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01, class t_ARG_02>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg_01, arg_02);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE, class t_ARG_01, class t_ARG_02, class t_ARG_03>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg_01, arg_02, arg_03);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg_01, arg_02, arg_03, arg_04);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(
        arg_01, arg_02, arg_03, arg_04, arg_05);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg_01,
                                              arg_02,
                                              arg_03,
                                              arg_04,
                                              arg_05,
                                              arg_06,
                                              arg_07,
                                              arg_08,
                                              arg_09);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <class t_TYPE,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
typename bsl::enable_if<BloombergLP::bslstl::Variant_HasUniqueType<
                            t_TYPE,
                            variant<t_HEAD, t_TAIL...> >::value,
                        t_TYPE&>::type
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09,
                                    const t_ARG_10& arg_10)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(t_TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg_01,
                                              arg_02,
                                              arg_03,
                                              arg_04,
                                              arg_05,
                                              arg_06,
                                              arg_07,
                                              arg_08,
                                              arg_09,
                                              arg_10);

    return bsl::get<index>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace()
{
    Variant_Base::template baseEmplace<t_INDEX>();

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01)
{
    Variant_Base::template baseEmplace<t_INDEX>(arg_01);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02)
{
    Variant_Base::template baseEmplace<t_INDEX>(arg_01, arg_02);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX, class t_ARG_01, class t_ARG_02, class t_ARG_03>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03)
{
    Variant_Base::template baseEmplace<t_INDEX>(arg_01, arg_02, arg_03);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04)
{
    Variant_Base::template baseEmplace<t_INDEX>(
        arg_01, arg_02, arg_03, arg_04);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05)
{
    Variant_Base::template baseEmplace<t_INDEX>(
        arg_01, arg_02, arg_03, arg_04, arg_05);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06)
{
    Variant_Base::template baseEmplace<t_INDEX>(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07)
{
    Variant_Base::template baseEmplace<t_INDEX>(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08)
{
    Variant_Base::template baseEmplace<t_INDEX>(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09)
{
    Variant_Base::template baseEmplace<t_INDEX>(arg_01,
                                                arg_02,
                                                arg_03,
                                                arg_04,
                                                arg_05,
                                                arg_06,
                                                arg_07,
                                                arg_08,
                                                arg_09);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
template <size_t t_INDEX,
          class t_ARG_01,
          class t_ARG_02,
          class t_ARG_03,
          class t_ARG_04,
          class t_ARG_05,
          class t_ARG_06,
          class t_ARG_07,
          class t_ARG_08,
          class t_ARG_09,
          class t_ARG_10>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
variant<t_HEAD, t_TAIL...>::emplace(const t_ARG_01& arg_01,
                                    const t_ARG_02& arg_02,
                                    const t_ARG_03& arg_03,
                                    const t_ARG_04& arg_04,
                                    const t_ARG_05& arg_05,
                                    const t_ARG_06& arg_06,
                                    const t_ARG_07& arg_07,
                                    const t_ARG_08& arg_08,
                                    const t_ARG_09& arg_09,
                                    const t_ARG_10& arg_10)
{
    Variant_Base::template baseEmplace<t_INDEX>(arg_01,
                                                arg_02,
                                                arg_03,
                                                arg_04,
                                                arg_05,
                                                arg_06,
                                                arg_07,
                                                arg_08,
                                                arg_09,
                                                arg_10);

    return bsl::get<t_INDEX>(*this);
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>& variant<t_HEAD, t_TAIL...>::operator=(
                                                            const variant& rhs)
{
    Variant_Base::operator=(static_cast<const Variant_Base&>(rhs));
    return *this;
}

template <class t_HEAD, class... t_TAIL>
inline
variant<t_HEAD, t_TAIL...>& variant<t_HEAD, t_TAIL...>::operator=(
                                   BloombergLP::bslmf::MovableRef<variant> rhs)
{
    variant<t_HEAD, t_TAIL...>& lvalue = rhs;
    Variant_Base::operator=(
                           MoveUtil::move(static_cast<Variant_Base&>(lvalue)));
    return *this;
}
template <class t_HEAD, class... t_TAIL>
template <class t_TYPE>
inline
typename bsl::enable_if<
    BloombergLP::bslstl::Variant_AssignsFromType<variant<t_HEAD, t_TAIL...>,
                                                 t_TYPE>::value,
    variant<t_HEAD, t_TAIL...>&>::type
variant<t_HEAD, t_TAIL...>::operator=(const t_TYPE& rhs)
{
    const size_t altIndex = BSLSTL_VARIANT_CONVERT_INDEX_OF(t_TYPE, variant);

    if (index() == altIndex) {
        bsl::get<altIndex>(*this) = rhs;
    }
    else {
        // 'altIndex' can not be 'variant_npos' if 'Variant_AssignsFromType' is
        // satisfied
        emplace<altIndex>(rhs);
    }

    return *this;
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_HEAD, class... t_TAIL>
inline
void variant<t_HEAD, t_TAIL...>::swap(variant& other)
{
    if (!valueless_by_exception()) {
        if (index() == other.index()) {
            BloombergLP::bslstl::Variant_SwapVisitor<variant> swapper(this);
            BSLSTL_VARIANT_VISITID(void, swapper, other);
        }
        else {
            variant tmpThis(MoveUtil::move(*this));
            this->reset();
            if (!other.valueless_by_exception()) {
                BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
                    moveConstructor(this);
                BSLSTL_VARIANT_VISITID(void, moveConstructor, other);
            }
#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
            // In C++03, if there are nonunique alternatives,
            // 'BSLSTL_VARIANT_VISITID' will always use the first nonunique
            // alternative, which may result in the wrong 'd_type' being set.
            this->d_type = other.index();
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
            other.reset();
            BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(BSLS_UTIL_ADDRESSOF(other));
            BSLSTL_VARIANT_VISITID(void, moveConstructor, tmpThis);
#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
            other.d_type = tmpThis.index();
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
        }
    }
    else {
        if (!other.valueless_by_exception()) {
            BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
                moveConstructor(this);
            BSLSTL_VARIANT_VISITID(void, moveConstructor, other);
#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
            this->d_type = other.index();
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
            other.reset();
        }
    }
}

// ACCESSORS
template <class t_HEAD, class... t_TAIL>
inline
size_t variant<t_HEAD, t_TAIL...>::index() const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_type;
}

template <class t_HEAD, class... t_TAIL>
inline
bool variant<t_HEAD, t_TAIL...>::valueless_by_exception()
const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_type == bsl::variant_npos;
}

#endif

// FREE FUNCTIONS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type& get(
                                               variant<t_HEAD, t_TAIL...>& obj)
{
    BSLMF_ASSERT((t_INDEX < variant_size<variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...>                  Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil                      ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, Variant>::type Ret;

    return ImpUtil::get<Ret, t_INDEX>(obj);
}

template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
const typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&
get(const variant<t_HEAD, t_TAIL...>& obj)
{
    BSLMF_ASSERT((t_INDEX < variant_size<variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...>                  Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil                      ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, Variant>::type Ret;

    return ImpUtil::get<const Ret, t_INDEX>(obj);
}

template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename variant_alternative<t_INDEX, variant<t_HEAD, t_TAIL...> >::type&& get(
                                              variant<t_HEAD, t_TAIL...>&& obj)
{
    BSLMF_ASSERT((t_INDEX < variant_size<variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...>                  Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil                      ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, Variant>::type Ret;

    return std::move(ImpUtil::get<Ret, t_INDEX>(obj));
}

template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
const typename variant_alternative<t_INDEX,
                                   variant<t_HEAD, t_TAIL...> >::type&&
get(const variant<t_HEAD, t_TAIL...>&& obj)
{
    BSLMF_ASSERT((t_INDEX < variant_size<variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...>                  Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil                      ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, Variant>::type Ret;

    return std::move(ImpUtil::get<const Ret, t_INDEX>(obj));
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <size_t t_INDEX, class t_VARIANT>
typename Variant_GetIndexReturnType<t_INDEX, t_VARIANT>::type
get(t_VARIANT& obj)
{
    BSLMF_ASSERT((t_INDEX < variant_size<t_VARIANT>::value));

    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type Ret;

    return ImpUtil::get<Ret, t_INDEX>(obj);
}

template <size_t t_INDEX, class t_VARIANT>
typename Variant_GetIndexReturnType<
    t_INDEX,
    BloombergLP::bslmf::MovableRef<t_VARIANT> >::type
get(BloombergLP::bslmf::MovableRef<t_VARIANT> obj)
{
    typedef BloombergLP::bslmf::MovableRefUtil   MoveUtil;
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type Ret;

    t_VARIANT& lvalue = obj;

    return MoveUtil::move(ImpUtil::get<Ret, t_INDEX>(lvalue));
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_TYPE, class t_HEAD, class... t_TAIL>
t_TYPE& get(variant<t_HEAD, t_TAIL...>& obj)
{
    BSLMF_ASSERT((BloombergLP::bslstl::Variant_HasUniqueType<
                  t_TYPE,
                  variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil     ImpUtil;

    return ImpUtil::get<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)>(obj);
}

template <class t_TYPE, class t_HEAD, class... t_TAIL>
const t_TYPE& get(const variant<t_HEAD, t_TAIL...>& obj)
{
    BSLMF_ASSERT((BloombergLP::bslstl::Variant_HasUniqueType<
                  t_TYPE,
                  variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil     ImpUtil;

    return ImpUtil::get<const t_TYPE,
                        BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)>(obj);
}

template <class t_TYPE, class t_HEAD, class... t_TAIL>
t_TYPE&& get(variant<t_HEAD, t_TAIL...>&& obj)
{
    static_assert(
        BloombergLP::bslstl::
            Variant_HasUniqueType<t_TYPE, variant<t_HEAD, t_TAIL...> >::value,
        "Type is not unique in variant");

    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil     ImpUtil;

    return std::move(
          ImpUtil::get<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)>(obj));
}

template <class t_TYPE, class t_HEAD, class... t_TAIL>
const t_TYPE&& get(const variant<t_HEAD, t_TAIL...>&& obj)
{
    static_assert(
        BloombergLP::bslstl::
            Variant_HasUniqueType<t_TYPE, variant<t_HEAD, t_TAIL...> >::value,
        "Type is not unique in variant");

    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil     ImpUtil;

    return std::move(
          ImpUtil::get<const t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)>(
              obj));
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<t_TYPE&, t_VARIANT>::type
get(t_VARIANT& obj)
{
    BSLMF_ASSERT((
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, t_VARIANT>::value));

    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    return ImpUtil::get<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(
                                                                          obj);
}

template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<const t_TYPE&, t_VARIANT>::type
get(const t_VARIANT& obj)
{
    BSLMF_ASSERT((
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, t_VARIANT>::value));

    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    return ImpUtil::get<const t_TYPE,
                        BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(obj);
}

template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<BloombergLP::bslmf::MovableRef<t_TYPE>,
                                   t_VARIANT>::type
get(BloombergLP::bslmf::MovableRef<t_VARIANT> obj)
{
    BSLMF_ASSERT((
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, t_VARIANT>::value));

    typedef BloombergLP::bslmf::MovableRefUtil            MoveUtil;
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    t_VARIANT& lvalue = obj;
    return MoveUtil::move(
              ImpUtil::get<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(
                  lvalue));
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename add_pointer<
    typename variant_alternative<t_INDEX,
                                 variant<t_HEAD, t_TAIL...> >::type>::type
get_if(variant<t_HEAD, t_TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((t_INDEX < variant_size<variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...>                  Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil                      ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, Variant>::type Ret;

    if (ptr == 0 || ptr->index() != t_INDEX) {
        return NULL;                                                  // RETURN
    }

    return BSLS_UTIL_ADDRESSOF((ImpUtil::get<Ret, t_INDEX>(*ptr)));
}

template <size_t t_INDEX, class t_HEAD, class... t_TAIL>
typename add_pointer<const typename variant_alternative<
    t_INDEX,
    variant<t_HEAD, t_TAIL...> >::type>::type
get_if(const variant<t_HEAD, t_TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((t_INDEX < variant_size<variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...>                  Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil                      ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, Variant>::type Ret;

    if (ptr == 0 || ptr->index() != t_INDEX) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF((ImpUtil::get<const Ret, t_INDEX>(*ptr)));
}

template <class t_TYPE, class t_HEAD, class... t_TAIL>
typename add_pointer<t_TYPE>::type get_if(
                         variant<t_HEAD, t_TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((BloombergLP::bslstl::Variant_HasUniqueType<
                  t_TYPE,
                  variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil     ImpUtil;

    if (ptr == 0 || ptr->index() != BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF((
        ImpUtil::get<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)>(*ptr)));
}

template <class t_TYPE, class t_HEAD, class... t_TAIL>
typename add_pointer<const t_TYPE>::type get_if(
                   const variant<t_HEAD, t_TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((BloombergLP::bslstl::Variant_HasUniqueType<
                  t_TYPE,
                  variant<t_HEAD, t_TAIL...> >::value));

    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    typedef BloombergLP::bslstl::Variant_ImpUtil     ImpUtil;

    if (ptr == 0 || ptr->index() != BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(
         (ImpUtil::get<const t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant)>(
             *ptr)));
}
#else
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <size_t t_INDEX, class t_VARIANT>
typename Variant_GetIndexReturnType<t_INDEX, t_VARIANT>::pointer
get_if(t_VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((t_INDEX < variant_size<t_VARIANT>::value));

    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;
    typedef typename bsl::variant_alternative<t_INDEX, t_VARIANT>::type Ret;

    if (ptr == 0 || ptr->index() != t_INDEX) {
        return NULL;                                                  // RETURN
    }

    return BSLS_UTIL_ADDRESSOF((ImpUtil::get<Ret, t_INDEX>(*ptr)));
}

template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<t_TYPE, t_VARIANT>::pointer
get_if(t_VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, t_VARIANT>::value));

    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (ptr == 0 ||
        ptr->index() != BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(
             (ImpUtil::get<t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(
                 *ptr)));
}

template <class t_TYPE, class t_VARIANT>
typename Variant_GetTypeReturnType<const t_TYPE, t_VARIANT>::pointer
get_if(const t_VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((
        BloombergLP::bslstl::Variant_HasUniqueType<t_TYPE, t_VARIANT>::value));

    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (ptr == 0 ||
        ptr->index() != BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF((
        ImpUtil::get<const t_TYPE, BSLSTL_VARIANT_INDEX_OF(t_TYPE, t_VARIANT)>(
            *ptr)));
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// HASH SPECIALIZATIONS
template <class t_HASHALG, class t_HEAD, class... t_TAIL>
void hashAppend(t_HASHALG& hashAlg, const variant<t_HEAD, t_TAIL...>& input)
{
    if (!input.valueless_by_exception()) {
        hashAppend(hashAlg, input.index());
        BloombergLP::bslstl::Variant_HashVisitor<t_HASHALG> hashVisitor(
                                                                      hashAlg);
        visit(hashVisitor, input);
    }
    else {
        hashAppend(hashAlg, false);
    }
}

template <class t_TYPE, class t_HEAD, class... t_TAIL>
bool holds_alternative(
                   const variant<t_HEAD, t_TAIL...>& obj) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((BloombergLP::bslstl::Variant_HasUniqueType<
                  t_TYPE,
                  variant<t_HEAD, t_TAIL...> >::value));
    typedef typename bsl::variant<t_HEAD, t_TAIL...> Variant;
    return obj.index() == BSLSTL_VARIANT_INDEX_OF(t_TYPE, Variant);
}

template <class t_HEAD, class... t_TAIL>
void swap(bsl::variant<t_HEAD, t_TAIL...>& lhs,
          bsl::variant<t_HEAD, t_TAIL...>& rhs)
{
    BloombergLP::bslstl::variant_swapImpl(
        bsl::integral_constant<
            bool,
            BloombergLP::bslstl::
                Variant_UsesBslmaAllocatorAny<t_HEAD, t_TAIL...>::value>(),
        lhs,
        rhs);
}

// FREE OPERATORS
template <class t_HEAD, class... t_TAIL>
bool operator==(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (lhs.index() != rhs.index()) {
        return false;                                                 // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    return ImpUtil::Equal(lhs, rhs);
}

template <class t_HEAD, class... t_TAIL>
bool operator!=(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (lhs.index() != rhs.index()) {
        return true;                                                  // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    return ImpUtil::NotEqual(lhs, rhs);
}

template <class t_HEAD, class... t_TAIL>
bool operator<(const variant<t_HEAD, t_TAIL...>& lhs,
               const variant<t_HEAD, t_TAIL...>& rhs)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (rhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() < rhs.index();                             // RETURN
    }
    return ImpUtil::LessThan(lhs, rhs);
}

template <class t_HEAD, class... t_TAIL>
bool operator>(const variant<t_HEAD, t_TAIL...>& lhs,
               const variant<t_HEAD, t_TAIL...>& rhs)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (lhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (rhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() > rhs.index();                             // RETURN
    }
    return ImpUtil::GreaterThan(lhs, rhs);
}

template <class t_HEAD, class... t_TAIL>
bool operator<=(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (lhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (rhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() < rhs.index();                             // RETURN
    }
    return ImpUtil::LessOrEqual(lhs, rhs);
}

template <class t_HEAD, class... t_TAIL>
bool operator>=(const variant<t_HEAD, t_TAIL...>& lhs,
                const variant<t_HEAD, t_TAIL...>& rhs)
{
    typedef BloombergLP::bslstl::Variant_ImpUtil ImpUtil;

    if (rhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() > rhs.index();                             // RETURN
    }

    return ImpUtil::GreaterOrEqual(lhs, rhs);
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON &&             \
    defined BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

template <class... t_ALTS>
    requires(std::three_way_comparable<t_ALTS> && ...)
constexpr std::common_comparison_category_t<
    std::compare_three_way_result_t<t_ALTS>...>
operator<=>(const variant<t_ALTS...>& lhs, const variant<t_ALTS...>& rhs)
{
    using RET = std::common_comparison_category_t<
        std::compare_three_way_result_t<t_ALTS>...>;

    const size_t lhs_index = lhs.index();
    const size_t rhs_index = rhs.index();
    if ((lhs_index != variant_npos) && (rhs_index != variant_npos)) {
        if (lhs_index == rhs_index) {
            return BloombergLP::bslstl::Variant_ImpUtil::visitId<RET>(
                    [&lhs]<size_t t_INDEX>(bsl::in_place_index_t<t_INDEX>,
                                           auto&& rhs_value) {
                        return bsl::get<t_INDEX>(lhs) <=> rhs_value;  // RETURN
                    },
                    rhs);
        }
        else {
            return lhs_index <=> rhs_index;                           // RETURN
        }
    }
    else {
        if (lhs_index == variant_npos) {
            if (rhs_index == variant_npos)
                return std::strong_ordering::equal;                   // RETURN
            else
                return std::strong_ordering::less;                    // RETURN
        }
        else
            return std::strong_ordering::greater;                     // RETURN
    }
}
#endif

#endif
}  // close namespace bsl

#endif // End C++11 code

#undef BSL_VARIANT_FULL_IMPLEMENTATION
#undef BSLSTL_VARIANT_NOT_A_TYPE
#undef BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM
#undef BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM
#undef BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE
#undef BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE
#undef BSLSTL_VARIANT_HAS_UNIQUE_TYPE
#undef BSLSTL_VARIANT_TYPE_AT_INDEX
#undef BSLSTL_VARIANT_INDEX_OF
#undef BSLSTL_VARIANT_CONVERT_INDEX_OF
#undef BSLSTL_VARIANT_CONVERT_TYPE_OF
#undef BSLSTL_VARIANT_VISITID
#undef BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON

#endif  // End C++11 code

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
