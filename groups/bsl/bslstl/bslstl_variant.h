// bslstl_variant.h                                                   -*-C++-*-

#ifndef INCLUDED_BSLSTL_VARIANT
#define INCLUDED_BSLSTL_VARIANT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant allocator aware variant type.
//
//@CLASSES:
//  bsl::variant: allocator-aware implementation of 'std::variant'
//
//@CANONICAL_HEADER: bsl_variant.h
//
//@DESCRIPTION: This component provides a class template,
// 'bsl::variant<TYPES...>', that is a not-yet-standardised allocator-aware
// version of 'std::variant'.  For functionality common to 'std::variant',
// C++23 was used as the reference specification, modulo limitations listed
// below.  'bsl::variant' may hold and manage the lifetime of an object, known
// as the *contained value*, which is stored within the footprint of the
// 'bsl::variant' object, and must be one of the template arguments 'TYPES'.
// These template arguments are called *alternatives*, and the alternative
// corresponding to the contained value is said to be *active*.  A
// 'bsl::variant' object may also hold no value.
//
// A program that instantiates the definition of 'variant' with no template
// arguments is ill-formed.
//
// A reference or pointer to the contained value of a 'bsl::variant' can be
// obtained using the free functions 'get' or 'get_if', respectively.  Such a
// reference or pointer that does not have top-level 'const' may be used to
// modify the contained value directly, if desired.
//
// 'bsl::variant' is copy/move constructible when all alternatives are
// copy/move constructible; the resulting object holds the alternative that the
// source object held.  'bsl::variant' can also be constructed from a value of
// one of the alternatives, or from an expression for which there is an
// unambiguous best match conversion to one of the alternatives.  In addition,
// 'bsl::variant' supports construction of an explicitly specified alternative
// from a variadic number of arguments.
//
// If at least one alternative is allocator-aware, 'bsl::variant' is
// allocator-aware.  For an allocator-aware 'bsl::variant', each constructor
// has a matching allocator-extended version that specifies the allocator that
// will be used during the lifetime of the 'bsl::variant' object to construct
// any allocator-aware alternative that the 'bsl::variant' object holds.  Note
// that the 'bsl::variant' object itself does not allocate any memory; its
// footprint is large enough to hold any of its alternatives.
//
// 'bsl::variant' is copy/move assignable when all alternatives are copy/move
// assignable and copy/move constructible; if the LHS has a different active
// alternative than the RHS, the contained value of the LHS will be destroyed
// before the contained value of the new alternative is created.
// 'bsl::variant' may also be assigned to from an expression for which there is
// an unambiguous best match conversion to one of the alternatives.
//
// The 'bsl::variant::emplace' methods, which take an explicitly specified
// alternative, can also be used to construct a contained value after a
// 'bsl::variant' object has already been constructed.  If the 'bsl::variant'
// object already holds a contained value, that object will be destroyed before
// the specified alternative is created.
//
// If an exception is thrown during an operation that changes the active
// alternative in a 'bsl::variant' object, the 'bsl::variant' object might be
// left in a state that holds no value, referred to as the *valueless by
// exception* state, indicated by the 'valueless_by_exception()' method
// returning 'true' and the 'index()' method returning 'bsl::variant_npos'.
//
// Two 'bsl::variant's of the same type compare equal if they hold the same
// alternative and their contained values compare equal, or they both hold no
// value.
//
// The 'index()' method returns the index of the current alternative.
// Additionally, the free function 'holds_alternative' can be used to check
// whether an explicitly specified type is the currently active alternative.
//
// Free function 'bsl::visit' is provided that implements the visitor design
// pattern as specified by the C++ standard, modulo limitations listed below.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// First, create a 'variant' object that can hold an integer, char, or string.
// The default constructor of 'bsl::variant<TYPES...>' creates the first
// alternative in 'TYPES...'; to create a different alternative, we can provide
// the index or the type of the alternative to create.
//..
//  bsl::variant<int, char, std::string> v1;
//  bsl::variant<int, char, std::string> v2(bsl::in_place_type_t<char>(), 'c');
//  bsl::variant<int, char, std::string> v3(bsl::in_place_index_t<2>(),
//                                          "string");
//
//  assert(bsl::holds_alternative<int>(v1));
//  assert(bsl::holds_alternative<char>(v2));
//  assert(v3.index() == 2);
//..
// Next, we create a visitor that can be called with a value of any of the
// alternatives:
//..
//  class MyVisitor {
//    public:
//      template <class TYPE>
//      void operator()(const TYPE& value) const
//      {
//          bsl::cout << value << bsl::endl;
//      }
//  };
//..
//  We can now use 'bsl::visit' to apply the visitor to our variant objects:
//..
//  MyVisitor visitor;
//  bsl::visit(visitor, v1);  // prints integer 0
//  bsl::visit(visitor, v2);  // prints char 'c'
//  bsl::visit(visitor, v3);  // prints string "string"
//..
//  To retrieve a contained value, we can use the 'get' free functions.  If the
//  requested alternative is not the currently active alternative, an exception
//  of type 'bsl::bad_variant_access' will be thrown.
//..
//  int i = bsl::get<int>(v1);
//  char c = bsl::get<1>(v2);
//  try {
//      bsl::get<std::string>(v2);
//  } catch (bsl::bad_variant_access const& ex) {
//      bsl::cout << "non-active alternative requested" << bsl::endl;
//  }
//..
//
///Known limitations
///-----------------
//: o The variadic constructors and emplace methods in C++03 are limited to one
//:   parameter.
//:
//: o In C++03, constructors and assignment operators that determine the
//:   best-matching alternative (instead of taking an explicitly specified
//:   alternative) require the argument type to exactly match one of the
//:   alternatives, modulo cv-qualification.
//:
//: o In C++03, the majority of functions do not have constraints due to
//:   language limitations. The documentation for each specific function lists
//:   any constraints that are implemented.
//:
//: o Visitation functionality is limited to one variant.  Before C++17,
//:   visitation only supports the 'VISITOR(ALTERNATIVE)' form of invocation;
//:   cases where the visitor is a pointer to member are not supported.
//:
//: o Constexpr, triviality, and exception specifications are not implemented.
//:
//: o In 'operator=(const variant& rhs)' and 'operator=(T&& t)', only direct
//:   copy construction from the relevant alternative is tried.  This behavior
//:   differs from the standard, which requires the construction of a temporary
//:   alternative object if construction of the relevant alternative is not
//:   'noexcept' (see [variant.assign] for details).  The standard behavior
//:   causes unnecessary performance degradation in cases where the alternative
//:   constructor does not throw, yet is not marked 'noexcept'; this behavior
//:   is therefore not implemented in 'bsl::variant'.
//:
//: o Due to the C++03 limitations of the 'bsl::invoke_result' facility, it is
//:   not possible to explicitly specify a return type for 'bsl::visit' without
//:   triggering a hard error during substitution into the overload of
//:   'bsl::visit' having a deduced return type on some C++03 compilers (Sun
//:   for example).  For this reason, the overload of 'bsl::visit' that takes
//:   the return type as the first, non-deduced template argument is not
//:   provided in C++03.  The non-standard free function 'bsl::visitR' may be
//:   used instead.  'bsl::visitR' is also provided in C++11 and later for
//:   backward compatibility with the C++03 interface.

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

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Tue Sep 19 17:05:58 2023
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
#define BSLSTL_VARIANT_NOT_A_TYPE void

namespace bsl {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
class variant;
#endif

// TRAITS

                            // ===================
                            // struct variant_size
                            // ===================

template <class BSL_VARIANT>
struct variant_size;
    // This metafunction calculates the number of alternatives in the (possibly
    // cv-qualified) 'bsl::variant' type of (template parameter)
    // 'BSL_VARIANT'.  The primary template is not defined.

template <class BSL_VARIANT>
struct variant_size<const BSL_VARIANT> : variant_size<BSL_VARIANT> {
};

template <class BSL_VARIANT>
struct variant_size<volatile BSL_VARIANT> : variant_size<BSL_VARIANT> {
};

template <class BSL_VARIANT>
struct variant_size<const volatile BSL_VARIANT> : variant_size<BSL_VARIANT> {
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
struct variant_size<variant<HEAD, TAIL...> >
: bsl::integral_constant<size_t, 1 + sizeof...(TAIL)> {
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class BSL_VARIANT>
BSLS_KEYWORD_INLINE_VARIABLE constexpr size_t variant_size_v =
    variant_size<BSL_VARIANT>::value;
    // This variable template represents the result value of the
    // 'bsl::variant_size' metafunction.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

BSLS_KEYWORD_INLINE_CONSTEXPR size_t variant_npos = -1;
    // This value is returned by 'bsl::variant::index()' if
    // 'valueless_by_exception()' is 'true'.

                         // ==========================
                         // struct variant_alternative
                         // ==========================

template <size_t INDEX, class TYPE>
struct variant_alternative;
    // This metafunction calculates the type of the alternative whose index is
    // (template parameter) 'INDEX' in the possibly cv-qualified
    // 'bsl::variant' type of (template parameter) 'TYPE'.  If 'TYPE' is
    // cv-qualified, its cv-qualifiers are applied to the alternative.

template <size_t INDEX, class TYPE>
struct variant_alternative<INDEX, const TYPE> {
    typedef typename variant_alternative<INDEX, TYPE>::type const type;
};

template <size_t INDEX, class TYPE>
struct variant_alternative<INDEX, volatile TYPE> {
    typedef typename variant_alternative<INDEX, TYPE>::type volatile type;
};

template <size_t INDEX, class TYPE>
struct variant_alternative<INDEX, const volatile TYPE> {
    typedef
        typename variant_alternative<INDEX, TYPE>::type const volatile type;
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// 'Variant_VariantAlternativeImpl' defined to avoid 'variant<>' from the
// sim_cpp11_features.pl script
template <size_t INDEX, class HEAD = BSLSTL_VARIANT_NOT_A_TYPE, class... TAIL>
struct Variant_VariantAlternativeImpl
: Variant_VariantAlternativeImpl<INDEX - 1, TAIL...> {
    // This component-private metafunction provides the implementation of
    // 'bsl::variant_alternative'.
};

template <size_t INDEX>
struct Variant_VariantAlternativeImpl<INDEX, BSLSTL_VARIANT_NOT_A_TYPE> {
};

template <>
struct Variant_VariantAlternativeImpl<0, BSLSTL_VARIANT_NOT_A_TYPE> {
};

template <class HEAD, class... TAIL>
struct Variant_VariantAlternativeImpl<0, HEAD, TAIL...> {
    typedef HEAD type;
};

template <size_t INDEX, class HEAD, class... TAIL>
struct variant_alternative<INDEX, variant<HEAD, TAIL...> >
: Variant_VariantAlternativeImpl<INDEX, HEAD, TAIL...> {
    BSLMF_ASSERT((INDEX < bsl::variant_size<variant<HEAD, TAIL...> >::value));
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <size_t INDEX, class TYPE>
using variant_alternative_t = typename variant_alternative<INDEX, TYPE>::type;
    // 'variant_alternative_t' is an alias to the return type of the
    // 'variant_alternative' metafunction.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// FREE FUNCTIONS
template <class HEAD, class... TAIL>
void swap(bsl::variant<HEAD, TAIL...>& lhs, bsl::variant<HEAD, TAIL...>& rhs);
    // Efficiently exchange the values of the specified 'lhs' and 'rhs'
    // objects.  This method provides the no-throw guarantee if the two variant
    // objects being swapped have the same active alternative and that
    // alternative provides that guarantee; otherwise, this method provides the
    // basic guarantee.  If 'lhs' and 'rhs' do not contain the same alternative
    // or they have unequal allocators, and an exception is thrown during the
    // swap, either or both variant objects may be left in a valueless state or
    // with an alternative in a moved-from state.  All alternatives shall be
    // move constructible and swappable.  For simplicity of implementation,
    // this function differs from the standard in the following :
    //: o constraints are not implemented
    //: o constexpr is not implemented

// HASH SPECIALIZATIONS
template <class HASHALG, class HEAD, class... TAIL>
void hashAppend(HASHALG& hashAlg, const variant<HEAD, TAIL...>& input);
    // Pass the specified 'input' to the specified 'hashAlg', where 'hashAlg'
    // is a hashing algorithm.

// 20.7.5, value access
template <class TYPE, class HEAD, class... TAIL>
bool holds_alternative(
                      const variant<HEAD, TAIL...>& obj) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'obj' currently holds the (template
    // parameter) 'TYPE' alternative, and 'false' otherwise.  'TYPE' shall
    // appear exactly once in the variant's list of alternatives.
#endif
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <size_t INDEX, class HEAD, class... TAIL>
typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type& get(
                                                  variant<HEAD, TAIL...>& obj);
template <size_t INDEX, class HEAD, class... TAIL>
const typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type& get(
                                            const variant<HEAD, TAIL...>& obj);
template <size_t INDEX, class HEAD, class... TAIL>
typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&& get(
                                                 variant<HEAD, TAIL...>&& obj);
template <size_t INDEX, class HEAD, class... TAIL>
const typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&& get(
                                           const variant<HEAD, TAIL...>&& obj);
    // Return a reference to the alternative object at index (template
    // parameter) 'INDEX' in the specified 'obj'.  If 'INDEX' is not the index
    // of the currently active alternative, an exception of type
    // 'bad_variant_access' is thrown.  'INDEX' shall be a valid index for the
    // variant type of 'obj'.

template <class TYPE, class HEAD, class... TAIL>
TYPE& get(variant<HEAD, TAIL...>& obj);
template <class TYPE, class HEAD, class... TAIL>
const TYPE& get(const variant<HEAD, TAIL...>& obj);
template <class TYPE, class HEAD, class... TAIL>
TYPE&& get(variant<HEAD, TAIL...>&& obj);
template <class TYPE, class HEAD, class... TAIL>
const TYPE&& get(const variant<HEAD, TAIL...>&& obj);
    // Return a reference to the alternative object with type (template
    // parameter) 'TYPE' in the specified 'obj'.  If 'TYPE' is not the type of
    // the currently active alternative, an exception of type
    // 'bad_variant_access' is thrown.  'TYPE' shall appear exactly once in the
    // variant's list of alternatives.

template <size_t INDEX, class HEAD, class... TAIL>
typename add_pointer<
    typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type>::type
get_if(variant<HEAD, TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;
template <size_t INDEX, class HEAD, class... TAIL>
typename add_pointer<
    const typename variant_alternative<INDEX,
                                       variant<HEAD, TAIL...> >::type>::type
get_if(const variant<HEAD, TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;
    // Return a pointer to the alternative object with index (template
    // parameter) 'INDEX' in the specified 'obj', or a null pointer if 'obj' is
    // a null pointer or if 'INDEX' is not the index of the currently active
    // alternative.  'INDEX' shall be a valid alternative index.

template <class TYPE, class HEAD, class... TAIL>
typename add_pointer<TYPE>::type get_if(
                            variant<HEAD, TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;
template <class TYPE, class HEAD, class... TAIL>
typename add_pointer<const TYPE>::type get_if(
                      const variant<HEAD, TAIL...> *obj) BSLS_KEYWORD_NOEXCEPT;
    // Return a pointer to the alternative object with type (template
    // parameter) 'TYPE' in the specified 'obj', or a null pointer if 'obj' is
    // a null pointer or if 'TYPE' is not the type of the currently active
    // alternative.  'TYPE' shall appear exactly once in the variant's list of
    // alternatives.
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Implementation note: The C++03 versions of the 'get' and 'get_if' functions
// are declared with non-variadic signatures because the IBM compiler does not
// like the generated code for functions that take an argument of template
// class with variadic template arguments.  The problem was only observed with
// 'get' and 'get_if' functions, and the exact issue is not known.

template <class TYPE, class HEAD, class... TAIL>
struct Variant_ReturnType;
    // This component-private metafunction is defined only when 'HEAD' is a
    // specialization of 'bsl::variant' and 'TAIL' is empty, in which case it
    // provides member typedefs corresponding to 'TYPE'.  Naming these member
    // typedefs results in a substitution failure when this metafunction is not
    // defined.

template <class TYPE, class HEAD, class... TAIL>
struct Variant_ReturnType<TYPE, bsl::variant<HEAD, TAIL...> > {
    // This partial specialization provides member typedefs that are used to
    // declare the 'get' and 'get_if' function templates in C++03 as
    // non-variadic function templates that are constrained to accept only
    // references or pointers to 'bsl::variant'.
    typedef TYPE                             type;
    typedef typename add_pointer<TYPE>::type pointer;
};

template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    typename variant_alternative<INDEX, VARIANT>::type&,
    VARIANT>::type
get(VARIANT&                                obj);
template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    const typename variant_alternative<INDEX, VARIANT>::type&,
    VARIANT>::type
get(const VARIANT&                          obj);
template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    BloombergLP::bslmf::MovableRef<
        typename variant_alternative<INDEX, VARIANT>::type>,
    VARIANT>::type
get(BloombergLP::bslmf::MovableRef<VARIANT> obj);
    // Return a reference to the alternative object at index (template
    // parameter) 'INDEX' in the specified 'obj'.  If 'INDEX' is not the index
    // of the currently active alternative, an exception of type
    // 'bad_variant_access' is thrown.  'INDEX' shall be a valid index for the
    // variant type of 'obj'.

template <class TYPE, class VARIANT>
typename Variant_ReturnType<TYPE&, VARIANT>::type
get(VARIANT&                                obj);
template <class TYPE, class VARIANT>
typename Variant_ReturnType<const TYPE&, VARIANT>::type
get(const VARIANT&                          obj);
template <class TYPE, class VARIANT>
typename Variant_ReturnType<BloombergLP::bslmf::MovableRef<TYPE>,
                            VARIANT>::type
get(BloombergLP::bslmf::MovableRef<VARIANT> obj);
    // Return a reference to the alternative object with type (template
    // parameter) 'TYPE' in the specified 'obj'.  If 'TYPE' is not the type of
    // the currently active alternative, an exception of type
    // 'bad_variant_access' is thrown.  'TYPE' shall appear exactly once in the
    // variant's list of alternatives.

template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<typename variant_alternative<INDEX, VARIANT>::type,
                            VARIANT>::pointer
get_if(VARIANT       *obj) BSLS_KEYWORD_NOEXCEPT;
template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    const typename variant_alternative<INDEX, VARIANT>::type,
    VARIANT>::pointer
get_if(const VARIANT *obj) BSLS_KEYWORD_NOEXCEPT;
    // Return a pointer to the alternative object with index (template
    // parameter) 'INDEX' in the specified 'obj', or a null pointer if 'obj' is
    // a null pointer or if 'INDEX' is not the index of the currently active
    // alternative.  'INDEX' shall be a valid alternative index.

template <class TYPE, class VARIANT>
typename Variant_ReturnType<TYPE, VARIANT>::pointer       get_if(
                                                            VARIANT       *obj)
                                                         BSLS_KEYWORD_NOEXCEPT;
template <class TYPE, class VARIANT>
typename Variant_ReturnType<const TYPE, VARIANT>::pointer get_if(
                                                            const VARIANT *obj)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return a pointer to the alternative object with type (template
    // parameter) 'TYPE' in the specified 'obj', or a null pointer if 'obj' is
    // a null pointer or if 'TYPE' is not the type of the currently active
    // alternative.  'TYPE' shall appear exactly once in the variant's list of
    // alternatives.
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
// FREE OPERATORS

// 20.7.6, relational operators
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
bool operator==(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' are both valueless by
    // exception or if they have the same active alternative and their
    // contained values compare equal; otherwise, return 'false'.  All
    // alternatives shall support `operator==`.

template <class HEAD, class... TAIL>
bool operator!=(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different active
    // alternatives or only one holds an alternative, or if they have the same
    // active alternative and the contained values compare unequal; otherwise,
    // return 'false'.  All alternatives shall support 'operator!='.

template <class HEAD, class... TAIL>
bool operator<(const variant<HEAD, TAIL...>& lhs,
               const variant<HEAD, TAIL...>& rhs);
    // Return 'true' if the index of the active alternative in the specified
    // 'lhs' is less than that of the specified 'rhs', or if both have the same
    // active alternative and the contained value of 'lhs' compares less than
    // that of 'rhs', or if 'lhs' is valueless by exception and 'rhs' is not;
    // otherwise, return 'false'.  All alternatives shall support 'operator<'.

template <class HEAD, class... TAIL>
bool operator>(const variant<HEAD, TAIL...>& lhs,
               const variant<HEAD, TAIL...>& rhs);
    // Return 'true' if the index of the active alternative in the specified
    // 'lhs' is greater than that of the specified 'rhs', or if both have the
    // same active alternative and the contained value of 'lhs' compares
    // greater than that of 'rhs', or if 'rhs' is valueless by exception and
    // 'lhs' is not; otherwise, return 'false'.  All alternatives shall support
    // 'operator>'.

template <class HEAD, class... TAIL>
bool operator<=(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs);
    // Return 'true' if the index of the active alternative in the specified
    // 'lhs' is less than that of the specified 'rhs', or if both have the same
    // active alternative and the contained value of 'lhs' compares less than
    // or equal to that of 'rhs', or if 'lhs' is valueless by exception;
    // otherwise, return 'false'.  All alternatives shall support 'operator<='.

template <class HEAD, class... TAIL>
bool operator>=(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs);
    // Return 'true' if the index of the active alternative in the specified
    // 'lhs' is greater than that of the specified 'rhs', or if both have the
    // same active alternative and the contained value of 'lhs' compares
    // greater than or equal to that of 'rhs', or if 'rhs' is valueless by
    // exception; otherwise, return 'false'.  All alternatives shall support
    // 'operator>='.
}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

template <class TYPE = BSLSTL_VARIANT_NOT_A_TYPE, class... OTHER>
struct Variant_UsesBslmaAllocatorAny;
    // This component-private metafunction is derived from 'bsl::true_type' if
    // at least one template argument uses an allocator, and from
    // 'bsl::false_type' otherwise.

template <>
struct Variant_UsesBslmaAllocatorAny<BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::false_type {
};

template <class TYPE, class... OTHER>
struct Variant_UsesBslmaAllocatorAny
: bsl::integral_constant<
      bool,
      BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value ||
          BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<
              OTHER...>::value> {
};

template <class TYPE = BSLSTL_VARIANT_NOT_A_TYPE, class... OTHER>
struct Variant_IsBitwiseMoveableAll;
    // This component-private metafunction is derived from 'bsl::true_type' if
    // all template arguments are bitwise moveable, and from 'bsl::false_type'
    // otherwise.

template <>
struct Variant_IsBitwiseMoveableAll<BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::true_type {
};

template <class TYPE, class... OTHER>
struct Variant_IsBitwiseMoveableAll
: bsl::integral_constant<bool,
                         BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value &&
                             Variant_IsBitwiseMoveableAll<OTHER...>::value> {
};

template <class TYPE>
struct Variant_IsTag : bsl::false_type {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // (template parameter) 'TYPE' is not a tag type.  This metafunction
    // requires any cv and ref qualifications to be removed from the queried
    // type.
};

template <>
struct Variant_IsTag<bsl::allocator_arg_t> : bsl::true_type {
};

template <class TYPE>
struct Variant_IsTag<bsl::in_place_type_t<TYPE> > : bsl::true_type {
};

template <size_t INDEX>
struct Variant_IsTag<bsl::in_place_index_t<INDEX> > : bsl::true_type {
};

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_TO, class t_FROM>
struct Variant_IsConstructible : std::is_constructible<t_TO, t_FROM> {
    // This component-private metafunction derives from
    // 'std::is_constructible<t_TO, t_FROM>' in C++11 and later, and
    // 'bsl::true_type' in C++03.
};

template <class t_LHS, class t_RHS>
struct Variant_IsAssignable : std::is_assignable<t_LHS, t_RHS> {
    // This component-private metafunction derives from
    // 'std::is_assignable<t_LHS, t_RHS>' in C++11 and later, and
    // 'bsl::true_type' in C++03.
};
#else
template <class t_TO, class t_FROM>
struct Variant_IsConstructible : bsl::true_type {
    // This component-private metafunction derives from
    // 'std::is_constructible<t_TO, t_FROM>' in C++11 and later, and
    // 'bsl::true_type' in C++03.
};

template <class t_LHS, class t_RHS>
struct Variant_IsAssignable : bsl::true_type {
    // This component-private metafunction derives from
    // 'std::is_assignable<t_LHS, t_RHS>' in C++11 and later, and
    // 'bsl::true_type' in C++03.
};
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#define BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(VARIANT, TYPE)               \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Variant_ConstructsFromType<VARIANT,              \
                                                        TYPE>::value,         \
        BloombergLP::bslstl::Variant_NoSuchType>::type

#define BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(VARIANT, TYPE)              \
    BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(                                 \
                        VARIANT,                                              \
                        TYPE) = BloombergLP::bslstl::Variant_NoSuchType(0)

#define BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(TYPE)                        \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::Variant_HasUniqueType<TYPE, variant>::value,     \
        BloombergLP::bslstl::Variant_NoSuchType>::type
    // Implementation note: This macro can't use
    // 'BSLSTL_VARIANT_HAS_UNIQUE_TYPE' because this macro is used at points
    // where 'variant<HEAD, TAIL...>' expands to an invalid construct.

#define BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(TYPE)                       \
    BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(                                 \
                           TYPE) = BloombergLP::bslstl::Variant_NoSuchType(0)

#define BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE)                                  \
    BloombergLP::bslstl::                                                     \
        Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value

#define BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX)                                   \
    typename bsl::variant_alternative<INDEX,                                  \
                                      bsl::variant<HEAD, TAIL...> >::type

#define BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)                                \
    BloombergLP::bslstl::Variant_TypeToIndex<TYPE, VARIANT>::value
    // This component-private macro expands to the index of the first
    // alternative in the specified 'VARIANT' that is identical to the
    // specified 'TYPE', or 'bsl::variant_npos' if no such alternative exists.

#define BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, VARIANT)                        \
    BloombergLP::bslstl::Variant_ConvertIndex<TYPE, VARIANT>::value
    // This component-private macro expands to the index of the first
    // alternative in the specified 'VARIANT' that is a "unique" best match for
    // conversion from 'std::declval<TYPE>()'.  See the documentation for
    // 'Variant_ConvertIndex' for more details.

#define BSLSTL_VARIANT_CONVERT_TYPE_OF(TYPE, VARIANT)                         \
    typename bsl::variant_alternative<BSLSTL_VARIANT_CONVERT_INDEX_OF(        \
                                          TYPE, VARIANT),                     \
                                      VARIANT>::type

#define BSLSTL_VARIANT_VISITID(RET, VISITOR, VAROBJ)                          \
    BloombergLP::bslstl::Variant_VisitImplId<RET>(VISITOR, VAROBJ);

template <size_t   INDEX,
          class    TYPE,
          class    HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... TAIL>
struct Variant_TypeToIndexImpl
: bsl::conditional<bsl::is_same<TYPE, HEAD>::value,
                   bsl::integral_constant<size_t, INDEX>,
                   Variant_TypeToIndexImpl<INDEX + 1, TYPE, TAIL...> >::type {
    // This component-private metafunction provides implementation for
    // 'Variant_TypeToIndex'.  It evaluates to 'INDEX + i' where 'i' is the
    // zero-based index of (template parameter) 'TYPE' in (template parameters)
    // 'HEAD, TAIL...', or 'bsl::variant_npos' if 'TYPE' is not found.
};

template <size_t INDEX, class TYPE>
struct Variant_TypeToIndexImpl<INDEX, TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, bsl::variant_npos> {
    // This partial specialization is used when the list of alternatives
    // 'HEAD, TAIL...' is empty, i.e., 'TYPE' wasn't found in the originally
    // supplied list of alternatives.
};

template <class TYPE, class VARIANT>
struct Variant_TypeToIndex;  // primary template not defined
    // This component-private metafunction calculates the zero-based index of
    // (template parameter) 'TYPE' in the list of alternatives in (template
    // parameter) 'VARIANT', or 'bsl::variant_npos' if there is no such
    // alternative.  The primary template (used when 'VARIANT' is not a
    // 'bsl::variant') is not defined.

template <class TYPE, class HEAD, class... TAIL>
struct Variant_TypeToIndex<TYPE, bsl::variant<HEAD, TAIL...> >
: Variant_TypeToIndexImpl<0, TYPE, HEAD, TAIL...> {
};

template <class TYPE, class HEAD = BSLSTL_VARIANT_NOT_A_TYPE, class... TAIL>
struct Variant_CountType
: bsl::integral_constant<size_t,
                         bsl::is_same<TYPE, HEAD>::value +
                             Variant_CountType<TYPE, TAIL...>::value> {
    // This component-private metafunction calculates the number of times
    // (template parameter) 'TYPE' occurs in (template parameters)
    // 'HEAD, TAIL...'.  An alternative must have the same cv-qualification as
    // 'TYPE' in order to be counted.
};

template <class TYPE>
struct Variant_CountType<TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, 0> {
    // Specialization for purposes of the sim_cpp11_features.pl script.
};

template <class TYPE, class VARIANT>
struct Variant_HasUniqueType;
    // This component-private metafunction derives from 'bsl::true_type' if
    // (template parameter) 'TYPE' occurs exactly once as an alternative of
    // (template parameter) 'VARIANT', and 'bsl::false_type' otherwise.  An
    // alternative must have the same cv-qualification as 'TYPE' in order to be
    // counted.  The primary template (used when 'VARIANT' is not a
    // 'bsl::variant') is not defined.

template <class TYPE, class HEAD, class... TAIL>
struct Variant_HasUniqueType<TYPE, bsl::variant<HEAD, TAIL...> >
: bsl::integral_constant<bool,
                         Variant_CountType<TYPE, HEAD, TAIL...>::value == 1> {
};

template <class TYPE, class HEAD = BSLSTL_VARIANT_NOT_A_TYPE, class... TAIL>
struct Variant_CountCVType
: bsl::integral_constant<
      size_t,
      bsl::is_same<typename bsl::remove_cv<TYPE>::type,
                   typename bsl::remove_cv<HEAD>::type>::value +
          Variant_CountCVType<TYPE, TAIL...>::value> {
    // This component-private metafunction calculates the number of times
    // (template parameter) 'TYPE' occurs in (template parameters)
    // 'HEAD, TAIL...', where two types that differ only in top-level
    // cv-qualification are considered to be the same.
};

template <class TYPE>
struct Variant_CountCVType<TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, 0> {
    // Specialization for purposes of the sim_cpp11_features.pl script.
};

template <class TYPE, class VARIANT>
struct Variant_HasUniqueCVType;
    // This component-private metafunction derives from 'bsl::true_type' if
    // (template parameter) 'TYPE' occurs exactly once as an alternative of
    // (template parameter) 'VARIANT', and 'bsl::false_type' otherwise, where
    // two types that differ only in top-level cv-qualification are considered
    // to be the same.  The primary template (used when 'VARIANT' is not a
    // 'bsl::variant') is not defined.

template <class TYPE, class HEAD, class... TAIL>
struct Variant_HasUniqueCVType<TYPE, bsl::variant<HEAD, TAIL...> >
: bsl::integral_constant<bool,
                         Variant_CountCVType<TYPE, HEAD, TAIL...>::value ==
                             1> {
};
#endif
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class VARIANT, size_t INDEX>
struct Variant_CVQualAlt {
    // This component-private metafunction calculates the alternative type at
    // index (template parameter) 'INDEX' in (template parameter) 'VARIANT',
    // where the cv- and ref-qualifiers of 'VARIANT' are added to the
    // alternative type.  This metafunction is used to calculate the return
    // type of 'bsl::visit'.

    typedef typename bsl::variant_alternative<
        INDEX,
        bsl::remove_reference_t<VARIANT> >::type CVAlt;
        // Alternative at 'INDEX' with combined cv-qualifiers

    typedef typename bsl::conditional<
        bsl::is_reference<VARIANT>::value,
        typename bsl::conditional<
            bsl::is_rvalue_reference<VARIANT>::value,
            typename bsl::add_rvalue_reference<CVAlt>::type,
            typename bsl::add_lvalue_reference<CVAlt>::type>::type,
        CVAlt>::type type;
};

template <class  RET,
          class  VISITOR,
          class  VARIANT,
          size_t INDEX =
              bsl::variant_size<bsl::remove_reference_t<VARIANT> >::value - 1>
struct Variant_IsSameReturnType
: public bsl::integral_constant<
      bool,
      bsl::is_same<RET,
                   typename bsl::invoke_result<
                       VISITOR,
                       typename Variant_CVQualAlt<VARIANT, INDEX>::type>::
                       type>::value &&
          Variant_IsSameReturnType<RET, VISITOR, VARIANT, INDEX - 1>::value> {
    // This component-private metafunction derives from 'bsl::true_type' if,
    // for each alternative 'ALTi' in (template parameter) 'VARIANT' with index
    // less than or equal to (template parameter) 'INDEX',
    // 'decltype(std::declval<VISITOR>(std::declval<ALTi>()))' is 'RET';
    // otherwise, this metafunction derives from 'bsl::false_type'.  Note that
    // 'ALTi' has the cv- and ref-qualifiers from 'VARIANT' added to it.  This
    // metafunction is used to determine whether invoking the visitor results
    // in the same type and value category for all alternatives.
};
template <class RET, class VISITOR, class VARIANT>
struct Variant_IsSameReturnType<RET, VISITOR, VARIANT, 0>
: bsl::is_same<RET,
               typename bsl::invoke_result<
                   VISITOR,
                   typename Variant_CVQualAlt<VARIANT, 0>::type>::type> {
};
#else   // BSL_VARIANT_FULL_IMPLEMENTATION
template <class  RET,
          class  VISITOR,
          class  VARIANT,
          size_t INDEX = bsl::variant_size<VARIANT>::value - 1>
struct Variant_IsSameReturnType
: public bsl::integral_constant<
      bool,
      bsl::is_same<RET,
                   typename bsl::invoke_result<
                       VISITOR,
                       typename bsl::variant_alternative<INDEX, VARIANT>::
                           type&>::type>::value &&
          Variant_IsSameReturnType<RET, VISITOR, VARIANT, INDEX - 1>::value> {
    // This component-private metafunction derives from 'bsl::true_type' if,
    // for each alternative 'ALTi' in (template parameter) 'VARIANT' with index
    // less than or equal to (template parameter) 'INDEX', invoking an object
    // of type (template parameter) 'VISITOR' on an lvalue of type 'ALTi' would
    // yield the type and value category given by (template parameter) 'RET';
    // otherwise, this metafunction derives from 'bsl::false_type'.  Note that
    // 'ALTi' has any cv-qualifiers from 'VARIANT' added to it.  This
    // metafunction is used to determine whether invoking the visitor results
    // in the same type and value category for all alternatives.
};

template <class RET, class VISITOR, class VARIANT>
struct Variant_IsSameReturnType<RET, VISITOR, VARIANT, 0>
: bsl::is_same<
      RET,
      typename bsl::invoke_result<
          VISITOR,
          typename bsl::variant_alternative<0, VARIANT>::type&>::type> {
};
template <class  RET,
          class  VISITOR,
          class  VARIANT,
          size_t INDEX = bsl::variant_size<VARIANT>::value - 1>
struct Variant_IsSameMoveReturnType
: public bsl::integral_constant<
      bool,
      bsl::is_same<RET,
                   typename bsl::invoke_result<
                       VISITOR,
                       BloombergLP::bslmf::MovableRef<
                           typename bsl::variant_alternative<INDEX, VARIANT>::
                               type> >::type>::value &&
          Variant_IsSameMoveReturnType<RET, VISITOR, VARIANT, INDEX - 1>::
              value> {
    // This component-private metafunction derives from 'bsl::true_type' if,
    // for each alternative 'ALTi' in (template parameter) 'VARIANT' with index
    // less than or equal to (template parameter) 'INDEX', invoking an object
    // of type (template parameter) 'VISITOR' on a 'bslmf::MovableRef<ALTi>'
    // would yield the type and value category given by (template parameter)
    // 'RET'; otherwise, this metafunction derives from 'bsl::false_type'.
    // This metafunction is used to determine whether invoking the visitor
    // results in the same type and value category for all alternatives.
};

template <class RET, class VISITOR, class VARIANT>
struct Variant_IsSameMoveReturnType<RET, VISITOR, VARIANT, 0>
: bsl::is_same<
      RET,
      typename bsl::invoke_result<
          VISITOR,
          BloombergLP::bslmf::MovableRef<
              typename bsl::variant_alternative<0, VARIANT>::type> >::type> {
};
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

                           // ======================
                           // struct Variant_ImpUtil
                           // ======================

template <class VARIANT>
struct Variant_ImpUtil {
    // This component-private class holds all functions that need to have
    // friendship access to the 'bsl::variant' type (template parameter)
    // 'VARIANT'.

    template <class RET, size_t INDEX>
    static RET& getImpl(bsl::in_place_index_t<INDEX>, VARIANT&       variant);
    template <class RET, size_t INDEX>
    static RET& getImpl(bsl::in_place_index_t<INDEX>, const VARIANT& variant);
        // Return a reference to the alternative with index (template
        // parameter) 'INDEX' in the specified 'variant'.  If 'INDEX' is not
        // the index of the currently active alternative, an exception of type
        // 'bad_variant_access' is thrown.  Note that the return type must be
        // explicitly specified.

#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
    template <class RET, size_t INDEX>
    static RET& Variant_UnsafeGetImpl(bsl::in_place_index_t<INDEX>,
                                      VARIANT&       variant);
    template <class RET, size_t INDEX>
    static RET& Variant_UnsafeGetImpl(bsl::in_place_index_t<INDEX>,
                                      const VARIANT& variant);
        // These component-private functions return a reference to type
        // template parameter) 'RET' to the alternative at (template parameter)
        // 'INDEX' in the specified 'variant', regardless of whether that is
        // the active alternative.  The behavior is undefined unless the active
        // alternative of 'variant' has the same type as the alternative with
        // index 'INDEX'.  Note that the return type must be explicitly
        // specified.
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    static bool EqualImpl(const VARIANT& lhs, const VARIANT& rhs);
    static bool NotEqualImpl(const VARIANT& lhs, const VARIANT& rhs);
    static bool LessThanImpl(const VARIANT& lhs, const VARIANT& rhs);
    static bool GreaterThanImpl(const VARIANT& lhs, const VARIANT& rhs);
    static bool LessOrEqualImpl(const VARIANT& lhs, const VARIANT& rhs);
    static bool GreaterOrEqualImpl(const VARIANT& lhs, const VARIANT& rhs);
        // Return the result of comparing the specified 'lhs' with the
        // specified 'rhs'.  The behavior is undefined unless both 'lhs' and
        // 'rhs' hold the same alternative.  Note that the capitalization of
        // the names of these methods has been chosen so that their definitions
        // can be generated using a macro.
};

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
struct Variant_ArrayHelper {
    // This component-private struct is used to check whether an alternative
    // given by (template parameter) 'TYPE' is a potential match for an
    // argument of a 'bsl::variant' constructor or assignment operator that
    // does not take an explicitly specified alternative type.  The standard
    // allows such conversion only when the declaration 'TYPE d_x[] = {expr};'
    // is valid, where 'expr' is the (forwarded) argument expression.
    TYPE d_x[1];
};

template <class TYPE, class = void>
struct Variant_CheckForP1957R2 : bsl::true_type {
    // This component-private metafunction checks whether a conversion from a
    // pointer type to (template parameter) 'TYPE' is narrowing.  It is
    // instantiated only for 'bool', and its behavior depends on whether the
    // compiler has implemented P1957R2.
};

template <class TYPE>
struct Variant_CheckForP1957R2<
    TYPE,
    bsl::void_t<decltype(Variant_ArrayHelper<TYPE>{{"bc"}})> >
: bsl::false_type {
};

template <class DEST, class SOURCE, class = void>
struct Variant_ConvertsWithoutNarrowing : bsl::false_type {
};

template <class DEST, class SOURCE>
struct Variant_ConvertsWithoutNarrowing<
    DEST,
    SOURCE,
    bsl::void_t<decltype(
        Variant_ArrayHelper<DEST>{{std::declval<SOURCE>()}})> >
: bsl::integral_constant<
      bool,
      !(!Variant_CheckForP1957R2<bool>::value &&
        bsl::is_same<bool, typename bsl::remove_cvref<DEST>::type>::value &&
        !bsl::is_same<bool,
                      typename bsl::remove_cvref<SOURCE>::type>::value)> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // (template parameter) 'SOURCE' can be converted to (template parameter)
    // 'DEST' without narrowing, and 'bsl::false_type' otherwise.  A conversion
    // from pointer or pointer-to-member type to cv 'bool' is considered
    // narrowing even if the compiler does not implement P1957R2 (which was
    // adopted as a DR); however, on compilers that do not implement P1957R2,
    // we do not have the ability to check whether a user-defined conversion
    // sequence to cv 'bool' would use a narrowing standard conversion, so on
    // those compilers, we permit conversion to a 'DEST' that is cv 'bool' only
    // if 'SOURCE' is also cv 'bool', and not when 'SOURCE' is a class type.
    // This behavior is not expected to pose a problem for users migrating from
    // 'bdlb::Variant', because that class does not support implicit
    // conversions from an argument type to an alternative type.
};

                       // =============================
                       // struct Variant_OverloadSetImp
                       // =============================

template <class SRC, size_t INDEX, class HEAD, class... TAIL>
struct Variant_OverloadSetImp
: Variant_OverloadSetImp<SRC, INDEX + 1, TAIL...> {
    // This component-private metafunction computes an overload set consisting
    // of one function, named 'candidate', for each type in (template
    // parameters) 'HEAD, TAIL...', having one parameter of that type.  Each
    // such function participates in overload resolution only when
    // 'std::declval<SRC>()' is convertible to the alternative without
    // narrowing, and returns 'bsl::integral_constant<INDEX + i>', where 'i' is
    // the zero-based index of the corresponding alternative.  Note that a type
    // that occurs multiple times in 'HEAD, TAIL...' (possibly with varying
    // cv-qualifications) will only result in the generation of a single
    // candidate.  This implementation relies on expression SFINAE, 'decltype',
    // 'std::declval', and P1957R2; since these features are not available in
    // C++03, the C++03 version requires an exact match modulo
    // cv-qualification.

    using Variant_OverloadSetImp<SRC, INDEX + 1, TAIL...>::candidate;

    template <class DEST = HEAD>
    static typename bsl::enable_if<
        Variant_ConvertsWithoutNarrowing<DEST, SRC>::value,
        bsl::integral_constant<size_t, INDEX> >::type candidate(HEAD);
};
template <class SRC, size_t INDEX, class HEAD>
struct Variant_OverloadSetImp<SRC, INDEX, HEAD> {
    template <class DEST = HEAD>
    static typename bsl::enable_if<
        Variant_ConvertsWithoutNarrowing<DEST, SRC>::value,
        bsl::integral_constant<size_t, INDEX> >::type candidate(HEAD);
};

template <class SRC, class VARIANT, class = void>
struct Variant_OverLoadHelper {
    // This component-private metafunction provides a member typedef 'Index'
    // representing the value that should be computed by
    // 'Variant_ConvertIndex'.  The primary template is instantiated when the
    // partial specialization below is not viable because no viable alternative
    // exists for the conversion or because the best match is not "unique" (see
    // the documentation of 'Variant_ConvertIndex' for an explanation).

    typedef bsl::integral_constant<size_t, bsl::variant_npos> Index;
};

template <class SRC, class HEAD, class... TAIL>
struct Variant_OverLoadHelper<
    SRC,
    bsl::variant<HEAD, TAIL...>,
    bsl::void_t<decltype(Variant_OverloadSetImp<SRC, 0, HEAD, TAIL...>::
                             candidate(std::declval<SRC>()))> > {
    // This partial specialization is used when a "unique" best match is found
    // for converting 'std::declval<SRC>()' to one of (template parameters)
    // 'HEAD, TAIL...'.
    typedef decltype(Variant_OverloadSetImp<SRC, 0, HEAD, TAIL...>::candidate(
        std::declval<SRC>())) Index;
};

template <class TYPE, class VARIANT>
struct Variant_ConvertIndex : Variant_OverLoadHelper<TYPE, VARIANT>::Index {
    // This component-private metafunction computes the index of the
    // alternative in (template parameter) 'VARIANT' that is the "unique" best
    // match for conversion from 'std::declval<TYPE>()', or 'bsl::variant_npos'
    // if there is no such alternative.  An alternative that occurs multiple
    // times in 'VARIANT' (possibly with varying cv-qualifications) is
    // considered to occur only once; thus, if all alternatives that are tied
    // for the best match are the same type (possibly with varying
    // cv-qualifications), the result is the lowest index at which that type
    // (with any cv-qualification) occurs.
};

#else  //BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <size_t   INDEX,
          class    TYPE,
          class    HEAD = BSLSTL_VARIANT_NOT_A_TYPE,
          class... TAIL>
struct Variant_ConvertToIndexImpl;
    // This component-private metafunction computes 'INDEX + i', where 'i' is
    // the zero-based index of the first occurrence of (template parameter)
    // 'TYPE' in (template parameters) 'HEAD, TAIL...', or 'bsl::variant_npos'
    // if not found.  An alternative is considered to match 'TYPE' if the two
    // types are the same modulo cv-qualification.  This metafunction is used
    // to implement 'Variant_ConvertIndex'.

template <size_t INDEX, class TYPE>
struct Variant_ConvertToIndexImpl<INDEX, TYPE, BSLSTL_VARIANT_NOT_A_TYPE>
: bsl::integral_constant<size_t, bsl::variant_npos> {
    // Specialization for the case when ARGS is empty, i.e. the type wasn't
    // found in the given list of template type arguments.
};
template <size_t INDEX, class TYPE, class HEAD, class... TAIL>
struct Variant_ConvertToIndexImpl<INDEX, TYPE, HEAD, TAIL...>
: bsl::conditional<
      bsl::is_same<typename bsl::remove_cv<TYPE>::type,
                   typename bsl::remove_cv<HEAD>::type>::value,
      bsl::integral_constant<size_t, INDEX>,
      Variant_ConvertToIndexImpl<INDEX + 1, TYPE, TAIL...> >::type {
};

template <class TYPE, class VARIANT>
struct Variant_ConvertIndex;
    // This component-private metafunction computes the zero-based index of the
    // first occurrence of (template parameter) 'TYPE' in the list of
    // alternatives of (template parameter) 'VARIANT', or 'bsl::variant_npos'
    // if not found.  An alternative is considered to match 'TYPE' if the two
    // types are the same modulo cv-qualification.  The primary template (used
    // when 'VARIANT' is not a 'bsl::variant') is not defined.  This
    // metafunction is used to determine the alternative to be constructed or
    // assigned to from a given argument type when the alternative type is not
    // explicitly specified.  Note that in C++11 and later,
    // 'Variant_ConvertIndex' allows implicit conversions, but this behavior
    // cannot be implemented in C++03.

template <class TYPE, class HEAD, class... TAIL>
struct Variant_ConvertIndex<TYPE, bsl::variant<HEAD, TAIL...> >
: Variant_ConvertToIndexImpl<
      0,
      typename bslmf::MovableRefUtil::RemoveReference<TYPE>::type,
      HEAD,
      TAIL...> {
};

#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

template <class  VARIANT,
          class  TYPE,
          size_t INDEX = BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, VARIANT)>
struct Variant_IsAlternativeConstructibleFrom
: bsl::integral_constant<
      bool,
      Variant_IsConstructible<
          typename bsl::variant_alternative<INDEX, VARIANT>::type,
          TYPE>::value &&
          Variant_HasUniqueCVType<BSLSTL_VARIANT_CONVERT_TYPE_OF(TYPE,
                                                                 VARIANT),
                                  VARIANT>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // there is a unique best match alternative in (template parameter)
    // 'VARIANT' for 'std::declval<TYPE>() and that alternative is
    // constructible from 'std::declval<TYPE>(), and 'bsl::false_type'
    // otherwise.
};

template <class VARIANT, class TYPE>
struct Variant_IsAlternativeConstructibleFrom<VARIANT, TYPE, bsl::variant_npos>
: bsl::false_type {
};

template <class  VARIANT,
          class  TYPE,
          size_t INDEX = BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, VARIANT)>
struct Variant_isAlternativeAssignableFrom
: bsl::integral_constant<
      bool,
      Variant_IsAssignable<
          typename bsl::variant_alternative<INDEX, VARIANT>::type&,
          TYPE>::value &&
          Variant_HasUniqueCVType<BSLSTL_VARIANT_CONVERT_TYPE_OF(TYPE,
                                                                 VARIANT),
                                  VARIANT>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // there is a unique best match alternative in (template parameter)
    // 'VARIANT' for 'std::declval<TYPE>()' and that alternative is both
    // constructible and assignable from 'std::declval<TYPE>()', and
    // 'bsl::false_type' otherwise.
};

template <class VARIANT, class TYPE>
struct Variant_isAlternativeAssignableFrom<VARIANT, TYPE, bsl::variant_npos>
: bsl::false_type {
};

template <class VARIANT, class TYPE>
struct Variant_ConstructsFromType
: bsl::integral_constant<
      bool,
      !bsl::is_same<typename bsl::remove_cvref<TYPE>::type, VARIANT>::value &&
          !Variant_IsTag<typename bsl::remove_cvref<TYPE>::type>::value &&
          Variant_IsAlternativeConstructibleFrom<VARIANT, TYPE>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // (template parameter) 'TYPE' is neither a tag type nor the type
    // (template parameter) 'VARIANT' (modulo cv-qualification), there is a
    // unique best match alternative in 'VARIANT' for 'std::declval<TYPE>()',
    // and that alternative is constructible from 'std::declval<TYPE>()';
    // otherwise, this metafunction is derived from 'bsl::false_type'.
};

template <class VARIANT, class TYPE>
struct Variant_AssignsFromType
: bsl::integral_constant<
      bool,
      !bsl::is_same<typename bsl::remove_cvref<TYPE>::type, VARIANT>::value &&
          Variant_IsAlternativeConstructibleFrom<VARIANT, TYPE>::value &&
          Variant_isAlternativeAssignableFrom<VARIANT, TYPE>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // (template parameter) 'TYPE' is neither a tag type nor the type
    // (template parameter) 'VARIANT' (modulo cv-qualification), there is a
    // unique best match alternative in 'VARIANT' for 'std::declval<TYPE>()',
    // and that alternative is constructible and assignable from
    // 'std::declval<TYPE>()'; otherwise, this metafunction is derived from
    // 'bsl::false_type'.
};

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
// The following component-private machinery allows for conditionally deleting
// special member functions.

template <bool ISCOPYCONSTRUCTIBLE>
struct Variant_CopyConstructorBase {
    // This component-private class has deleted copy constructor if (template
    // parameter) 'ISCOPYCONSTRUCTIBLE' is 'false'.  All other special member
    // functions are defaulted.
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

template <bool ISCOPYCONSTRUCTIBLE, bool ISMOVECONSTRUCTIBLE>
struct Variant_MoveConstructorBase
: Variant_CopyConstructorBase<ISCOPYCONSTRUCTIBLE> {
    // This component-private class has deleted move constructor if (template
    // parameter) 'ISMOVECONSTRUCTIBLE' is 'false'.  All other special member
    // functions are defaulted.
};
template <bool ISCOPYCONSTRUCTIBLE>
struct Variant_MoveConstructorBase<ISCOPYCONSTRUCTIBLE, false>
: Variant_CopyConstructorBase<ISCOPYCONSTRUCTIBLE> {
    Variant_MoveConstructorBase()                                   = default;
    Variant_MoveConstructorBase(const Variant_MoveConstructorBase&) = default;
    Variant_MoveConstructorBase(Variant_MoveConstructorBase&&)      = delete;
    Variant_MoveConstructorBase&
    operator=(const Variant_MoveConstructorBase&) = default;
    Variant_MoveConstructorBase&
    operator=(Variant_MoveConstructorBase&&) = default;
};

template <bool ISCOPYCONSTRUCTIBLE,
          bool ISMOVECONSTRUCTIBLE,
          bool ISCOPYASSIGNABLE>
struct Variant_CopyAssignBase
: Variant_MoveConstructorBase<ISCOPYCONSTRUCTIBLE, ISMOVECONSTRUCTIBLE> {
    // This component-private class has deleted copy assignment operator if
    // (template parameter) 'ISCOPYASSIGNABLE' is 'false'.  All other special
    // member functions are defaulted.
};
template <bool ISCOPYCONSTRUCTIBLE, bool ISMOVECONSTRUCTIBLE>
struct Variant_CopyAssignBase<ISCOPYCONSTRUCTIBLE, ISMOVECONSTRUCTIBLE, false>
: Variant_MoveConstructorBase<ISCOPYCONSTRUCTIBLE, ISMOVECONSTRUCTIBLE> {
    Variant_CopyAssignBase()                                         = default;
    Variant_CopyAssignBase(const Variant_CopyAssignBase&)            = default;
    Variant_CopyAssignBase(Variant_CopyAssignBase&&)                 = default;
    Variant_CopyAssignBase& operator=(const Variant_CopyAssignBase&) = delete;
    Variant_CopyAssignBase& operator=(Variant_CopyAssignBase&&)      = default;
};

template <bool ISCOPYCONSTRUCTIBLE,
          bool ISMOVECONSTRUCTIBLE,
          bool ISCOPYASSIGNABLE,
          bool ISMOVEASSIGNABLE>
struct Variant_MoveAssignBase : Variant_CopyAssignBase<ISCOPYCONSTRUCTIBLE,
                                                       ISMOVECONSTRUCTIBLE,
                                                       ISCOPYASSIGNABLE> {
    // This component-private class has deleted move assignment operator if
    // (template parameter) 'ISMOVEASSIGNABLE' is 'false'.  All other special
    // member functions are defaulted.
};
template <bool ISCOPYCONSTRUCTIBLE,
          bool ISMOVECONSTRUCTIBLE,
          bool ISCOPYASSIGNABLE>
struct Variant_MoveAssignBase<ISCOPYCONSTRUCTIBLE,
                              ISMOVECONSTRUCTIBLE,
                              ISCOPYASSIGNABLE,
                              false>
: Variant_CopyAssignBase<ISCOPYCONSTRUCTIBLE,
                         ISMOVECONSTRUCTIBLE,
                         ISCOPYASSIGNABLE> {
    Variant_MoveAssignBase()                                         = default;
    Variant_MoveAssignBase(const Variant_MoveAssignBase&)            = default;
    Variant_MoveAssignBase(Variant_MoveAssignBase&&)                 = default;
    Variant_MoveAssignBase& operator=(const Variant_MoveAssignBase&) = default;
    Variant_MoveAssignBase& operator=(Variant_MoveAssignBase&&)      = delete;
};

template <bool ISCOPYCONSTRUCTIBLE,
          bool ISMOVECONSTRUCTIBLE,
          bool ISCOPYASSIGNABLE,
          bool ISMOVEASSIGNABLE>
struct Variant_SMFBase : Variant_MoveAssignBase<ISCOPYCONSTRUCTIBLE,
                                                ISMOVECONSTRUCTIBLE,
                                                ISCOPYASSIGNABLE,
                                                ISMOVEASSIGNABLE> {
    // This component-private class has special member functions that are
    // either deleted or defaulted according to the specified template
    // parameters.
};

template <class TYPE, class... OTHER>
struct Variant_IsCopyConstructibleAll
: bsl::integral_constant<bool,
                         std::is_copy_constructible<TYPE>::value &&
                             Variant_IsCopyConstructibleAll<OTHER...>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // all template parameters are copy constructible, and 'bsl::false_type'
    // otherwise.
};

template <class TYPE>
struct Variant_IsCopyConstructibleAll<TYPE>
: std::is_copy_constructible<TYPE> {
};

template <class TYPE, class... OTHER>
struct Variant_IsMoveConstructibleAll
: bsl::integral_constant<bool,
                         std::is_move_constructible<TYPE>::value &&
                             Variant_IsMoveConstructibleAll<OTHER...>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // all template parameters are move constructible, and 'bsl::false_type'
    // otherwise.
};

template <class TYPE>
struct Variant_IsMoveConstructibleAll<TYPE>
: std::is_move_constructible<TYPE> {
};

template <class TYPE, class... OTHER>
struct Variant_IsCopyAssignableAll
: bsl::integral_constant<bool,
                         std::is_copy_assignable<TYPE>::value &&
                             Variant_IsCopyAssignableAll<OTHER...>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // all template parameters are copy assignable, and 'bsl::false_type'
    // otherwise.
};

template <class TYPE>
struct Variant_IsCopyAssignableAll<TYPE> : std::is_copy_assignable<TYPE> {
};

template <class TYPE, class... OTHER>
struct Variant_IsMoveAssignableAll
: bsl::integral_constant<bool,
                         std::is_move_assignable<TYPE>::value &&
                             Variant_IsMoveAssignableAll<OTHER...>::value> {
    // This component-private metafunction is derived from 'bsl::true_type' if
    // all template parameters are move assignable, and 'bsl::false_type'
    // otherwise.
};

template <class TYPE>
struct Variant_IsMoveAssignableAll<TYPE> : std::is_move_assignable<TYPE> {
};
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

template <class RET, class VARIANTUNION>
RET& variant_getAlternativeImpl(bsl::in_place_index_t<0>,
                                VARIANTUNION& variantUnion)
                                                          BSLS_KEYWORD_NOEXCEPT
    // This component-private function returns a reference to the object
    // managed by the first member of the specified 'variantUnion'.  It is the
    // base case for the overload of 'getAlternativeImpl' below.
{
    return variantUnion.d_head.value();
}

template <class RET, size_t INDEX, class VARIANTUNION>
RET& variant_getAlternativeImpl(bsl::in_place_index_t<INDEX>,
                                VARIANTUNION& variantUnion)
                                                          BSLS_KEYWORD_NOEXCEPT
    // This component-private function returns a reference to the alternative
    // with index (template parameter) 'INDEX' in the specified 'variantUnion'
    // by recursively unravelling 'variantUnion' until the desired alternative
    // is at the head.  'INDEX' shall be a valid alternative index.  The
    // behavior is undefined unless the alternative with index 'INDEX' has the
    // same type as the active alternative of 'variantUnion'.
{
    return variant_getAlternativeImpl<RET>(bsl::in_place_index_t<INDEX - 1>(),
                                           variantUnion.d_tail);
}

template <class VARIANT>
template <class RET, size_t INDEX>
RET& Variant_ImpUtil<VARIANT>::getImpl(bsl::in_place_index_t<INDEX>,
                                       VARIANT& variant)
{
    if (variant.index() != INDEX) {
        BSLS_THROW(bsl::bad_variant_access());
    }

    return variant_getAlternativeImpl<RET>(bsl::in_place_index_t<INDEX>(),
                                           variant.d_union);
}

template <class VARIANT>
template <class RET, size_t INDEX>
RET& Variant_ImpUtil<VARIANT>::getImpl(bsl::in_place_index_t<INDEX>,
                                       const VARIANT& variant)
{
    if (variant.index() != INDEX) {
        BSLS_THROW(bsl::bad_variant_access());
    }

    return variant_getAlternativeImpl<RET>(bsl::in_place_index_t<INDEX>(),
                                           variant.d_union);
}

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_RET, class VISITOR, class VARIANT, size_t INDEX>
struct Variant_Function {
    static t_RET functionImpl(VISITOR&& visitor, VARIANT&& variant)
        // This component-private function invokes the specified 'visitor' with
        // the active alternative object of the specified 'variant' and
        // implicitly converts the return type to (template parameter) 't_RET'.
        // The pre-C++17 implementation is limited to invocations of the form
        // 'visitor(variant)'.  The behavior is undefined unless (template
        // parameter) 'INDEX' is the index of the active alternative.
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        return std::invoke(std::forward<VISITOR>(visitor),
                           bsl::get<INDEX>(std::forward<VARIANT>(variant)));
#else
        return visitor(bsl::get<INDEX>(std::forward<VARIANT>(variant)));
#endif
    }
};

template <class VISITOR, class VARIANT, size_t INDEX>
struct Variant_Function<void, VISITOR, VARIANT, INDEX> {
    // This partial specialization is used when 't_RET' is 'void'.  The visitor
    // is invoked and its return value, if any, is ignored.  This partial
    // specialization is necessary because a 'void'-returning function cannot
    // contain a 'return' statement with a non-'void' operand.

    static void functionImpl(VISITOR&& visitor, VARIANT&& variant)
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        std::invoke(std::forward<VISITOR>(visitor),
                    bsl::get<INDEX>(std::forward<VARIANT>(variant)));
#else
        visitor(bsl::get<INDEX>(std::forward<VARIANT>(variant)));
#endif
    }
};

template <class t_RET, class VISITOR, class VARIANT, size_t INDEX>
struct Variant_FunctionId {
    static t_RET functionImpl(VISITOR&& visitor, VARIANT&& variant)
        // This component-private function invokes the specified 'visitor' with
        // a 'bsl::in_place_index_t' tag representing the value of (template
        // parameter) 'INDEX' and the active alternative object of the
        // specified 'variant', implicitly converting the return type to
        // (template parameter) 't_RET'.  The behavior is undefined unless
        // 'INDEX' is the index of the active alternative.
    {
        return visitor(bsl::in_place_index_t<INDEX>(),
                       bsl::get<INDEX>(std::forward<VARIANT>(variant)));
    }
};

template <class t_RET, class VISITOR, class VARIANT, class DUMMY>
struct Variant_VTable;
    // This component-private struct computes an array in which element 'i' is
    // a pointer to 'Variant_Function<R, VISITOR, VARIANT, i>::functionImpl',
    // defined above as a function that invokes a 'VISITOR' with alternative
    // 'i' of 'VARIANT' and implicitly converts the return type to 't_RET'.

template <class t_RET, class VISITOR, class VARIANT, size_t... INDICES>
struct Variant_VTable<t_RET,
                      VISITOR,
                      VARIANT,
                      bslmf::IntegerSequence<std::size_t, INDICES...> > {
    // In order to allow for perfect forwarding, both VISITOR and VARIANT must
    // be of reference type.  If they are not, something went wrong.
    BSLMF_ASSERT(bsl::is_reference<VISITOR>::value);
    BSLMF_ASSERT(bsl::is_reference<VARIANT>::value);

    typedef t_RET (*FuncPtr)(VISITOR, VARIANT);

    static BSLS_KEYWORD_CONSTEXPR FuncPtr s_map[sizeof...(INDICES)] = {&(
         Variant_Function<t_RET, VISITOR, VARIANT, INDICES>::functionImpl)...};
};

template <class t_RET, class VISITOR, class VARIANT, class DUMMY>
struct Variant_VTableId;
    // This component-private struct computes an array in which element 'i' is
    // a pointer to
    // 'Variant_FunctionId<t_RET, VISITOR, VARIANT, i>::functionImpl', defined
    // above as a function that invokes a 'VISITOR' with a tag representing 'i'
    // and alternative 'i' of 'VARIANT', implicitly converting the return type
    // to 't_RET'.  Implementation note: The constexpr static maps of pointers
    // defined by 'Variant_VTable' and this class have deliberately been
    // defined in two different classes as having them in the same class caused
    // issues with Clang and Microsoft Visual C++.

template <class t_RET, class VISITOR, class VARIANT, size_t... INDICES>
struct Variant_VTableId<t_RET,
                        VISITOR,
                        VARIANT,
                        bslmf::IntegerSequence<std::size_t, INDICES...> > {
    // In order to allow for perfect forwarding, both VISITOR and VARIANT must
    // be of reference type.  If they are not, something went wrong.
    BSLMF_ASSERT(bsl::is_reference<VISITOR>::value);
    BSLMF_ASSERT(bsl::is_reference<VARIANT>::value);

    typedef t_RET (*FuncPtr)(VISITOR, VARIANT);

    static BSLS_KEYWORD_CONSTEXPR FuncPtr s_mapId[sizeof...(INDICES)] = {
        &(Variant_FunctionId<t_RET, VISITOR, VARIANT, INDICES>::
              functionImpl)...};
};

// inline definitions of component-private function pointer maps
template <class t_RET, class VISITOR, class VARIANT, size_t... INDICES>
BSLS_KEYWORD_CONSTEXPR typename Variant_VTable<
    t_RET,
    VISITOR,
    VARIANT,
    bslmf::IntegerSequence<std::size_t, INDICES...> >::FuncPtr
    Variant_VTable<t_RET,
                   VISITOR,
                   VARIANT,
                   bslmf::IntegerSequence<std::size_t, INDICES...> >::s_map
        [sizeof...(INDICES)];

template <class t_RET, class VISITOR, class VARIANT, size_t... INDICES>
BSLS_KEYWORD_CONSTEXPR typename Variant_VTableId<
    t_RET,
    VISITOR,
    VARIANT,
    bslmf::IntegerSequence<std::size_t, INDICES...> >::FuncPtr
    Variant_VTableId<t_RET,
                     VISITOR,
                     VARIANT,
                     bslmf::IntegerSequence<std::size_t, INDICES...> >::s_mapId
        [sizeof...(INDICES)];

template <class t_RET, class VISITOR, class VARIANT>
t_RET Variant_VisitImpl(VISITOR&& visitor, VARIANT&& variant)
    // This component-private function invokes the specified 'visitor' on the
    // active alternative of the specified 'variant', implicitly converting the
    // return type to the explicitly specified (template parameter) 't_RET'.
    // The behavior is undefined if 'variant' does not hold a value.
{
    static constexpr size_t varSize =
        bsl::variant_size<bsl::remove_reference_t<VARIANT> >::value;

    typedef typename Variant_VTable<
        t_RET,
        VISITOR&&,
        VARIANT&&,
        bslmf::MakeIntegerSequence<std::size_t, varSize> >::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 'VISITOR' and 'VARIANT',
    // then invoke the one corresponding to the active index.
    FuncPtr func_ptr =
        Variant_VTable<t_RET,
                       VISITOR&&,
                       VARIANT&&,
                       bslmf::MakeIntegerSequence<std::size_t, varSize> >::
            s_map[variant.index()];

    return (*func_ptr)(std::forward<VISITOR>(visitor),
                       std::forward<VARIANT>(variant));
}
template <class t_RET, class VISITOR, class VARIANT>
t_RET Variant_VisitImplId(VISITOR&& visitor, VARIANT&& variant)
    // This component-private function is like 'Variant_VisitImpl', but it also
    // passes a tag representing the index of the selected alternative when
    // invoking the visitor.  It is used internally for visitors that
    // participate in the 'variant' implementation.
{
    static constexpr size_t varSize =
        bsl::variant_size<bsl::remove_reference_t<VARIANT> >::value;

    typedef typename Variant_VTableId<
        t_RET,
        VISITOR&&,
        VARIANT&&,
        bslmf::MakeIntegerSequence<std::size_t, varSize> >::FuncPtr FuncPtr;

    FuncPtr func_ptr =
        Variant_VTableId<t_RET,
                         VISITOR&&,
                         VARIANT&&,
                         bslmf::MakeIntegerSequence<std::size_t, varSize> >::
            s_mapId[variant.index()];

    return (*func_ptr)(std::forward<VISITOR>(visitor),
                       std::forward<VARIANT>(variant));
}

#else  //BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// In C++03, the internal visitation dispatch must call a type-based (not
// index-based) getter.  This internal type-based getter will always be passed
// the type of the currently active alternative, so there is no need to check
// whether the requested alternative matches the currently active alternative.
// Moreover, in cases where there are alternatives of identical type,
// 'bsl::get' cannot be passed such a type.  To solve the problem, we implement
// a component-private, unsafe type-based getter that is used only for internal
// visitation.
template <class VARIANT>
template <class RET, size_t INDEX>
RET& Variant_ImpUtil<VARIANT>::Variant_UnsafeGetImpl(
                                                  bsl::in_place_index_t<INDEX>,
                                                  VARIANT& variant)
{
    return variant_getAlternativeImpl<RET>(bsl::in_place_index_t<INDEX>(),
                                           variant.d_union);
}

template <class VARIANT>
template <class RET, size_t INDEX>
RET& Variant_ImpUtil<VARIANT>::Variant_UnsafeGetImpl(
                                                  bsl::in_place_index_t<INDEX>,
                                                  const VARIANT& variant)
{
    return variant_getAlternativeImpl<RET>(bsl::in_place_index_t<INDEX>(),
                                           variant.d_union);
}

template <class TYPE, class HEAD, class... TAIL>
TYPE& Variant_UnsafeGet(bsl::variant<HEAD, TAIL...>& obj)
    // This component-private function returns a reference to type (template
    // parameter) 'TYPE' to the first alternative of the specified 'obj' that
    // has type 'TYPE', regardless of whether that is the active alternative.
    // The behavior is undefined unless the active alternative of 'variant' is
    // of type 'TYPE'.
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    return Var_ImpUtil::template Variant_UnsafeGetImpl<TYPE>(
        bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(), obj);
}

template <class TYPE, class HEAD, class... TAIL>
const TYPE& Variant_UnsafeGet(const bsl::variant<HEAD, TAIL...>& obj)
    // This component-private function returns a reference to const-qualified
    // (template parameter) 'TYPE' to the first alternative of the specified
    // 'obj' that has type 'TYPE', regardless of whether that is the active
    // alternative.  The behavior is undefined unless the active alternative of
    // 'variant' is of type 'TYPE'.
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    return Var_ImpUtil::template Variant_UnsafeGetImpl<const TYPE>(
        bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(), obj);
}

template <class R, class VISITOR, class VARIANT, class ALT_TYPE>
struct Variant_Function {
    static R functionImpl(VISITOR& visitor, VARIANT& variant)
        // This component-private function invokes the specified 'visitor' on
        // the contained value of the specified 'variant', implicitly
        // converting the return type to (template parameter) 'R'.  The
        // behavior is undefined if (template parameter) 'ALT_TYPE' is not the
        // type of the active alternative.
    {
        return visitor(Variant_UnsafeGet<ALT_TYPE>(variant));
    }
};

template <class VISITOR, class VARIANT, class ALT_TYPE>
struct Variant_Function<void, VISITOR, VARIANT, ALT_TYPE> {
    // This partial specialization is used when 'R' is 'void'.  The visitor is
    // invoked and its return value, if any, is ignored.  This partial
    // specialization is necessary because a 'void'-returning function cannot
    // contain a 'return' statement with a non-'void' operand.
    static void functionImpl(VISITOR& visitor, VARIANT& variant)
    {
        visitor(Variant_UnsafeGet<ALT_TYPE>(variant));
    }
};

template <class R, class VISITOR, class VARIANT, class ALT_TYPE>
struct Variant_MoveFunction {
    static R functionImpl(VISITOR& visitor, VARIANT& variant)
        // This component-private function invokes the specified 'visitor' with
        // a 'bslmf::MovableRef' referring to the active alternative object of
        // the specified 'variant', implicitly converting the return type to
        // (template parameter) 'R'.  The behavior is undefined unless
        // 'variant' has an active alternative of type (template parameter)
        // 'ALT_TYPE'.
    {
        typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

        return visitor(MoveUtil::move(Variant_UnsafeGet<ALT_TYPE>(variant)));
    }
};

template <class VISITOR, class VARIANT, class ALT_TYPE>
struct Variant_MoveFunction<void, VISITOR, VARIANT, ALT_TYPE> {
    // This partial specialization is used when 'R' is 'void'.  The visitor is
    // invoked and its return value, if any, is ignored.  This partial
    // specialization is necessary because a 'void'-returning function cannot
    // contain a 'return' statement with a non-'void' operand.
    static void functionImpl(VISITOR& visitor, VARIANT& variant)
    {
        typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

        visitor(MoveUtil::move(Variant_UnsafeGet<ALT_TYPE>(variant)));
    }
};

template <class R, class VISITOR, class VARIANT, class ALT_TYPE>
struct Variant_FunctionId {
    static R functionImpl(VISITOR& visitor, VARIANT& variant)
        // This component-private function invokes the specified 'visitor' with
        // a 'bsl::in_place_index_t' tag representing the index of the first
        // occurrence of (template parameter) 'ALT_TYPE' in the specified
        // 'variant' and the active alternative object of 'variant', implicitly
        // converting the return type to (template parameter) 'R'.  The
        // behavior is undefined unless 'variant' has an active alternative of
        // type 'ALT_TYPE'.  Note that the index passed to the visitor might
        // not be the index of the active alternative, if that alternative
        // occurs multiple times in 'VARIANT'.
    {
        static const size_t INDEX = BSLSTL_VARIANT_INDEX_OF(
            ALT_TYPE, typename bsl::remove_cv<VARIANT>::type);

        return visitor(bsl::in_place_index_t<INDEX>(),
                       Variant_UnsafeGet<ALT_TYPE>(variant));
    }
};

template <class R, class VISITOR, class VARIANT, class... UNUSED>
struct Variant_VTable;
    // This component-private struct computes arrays in which element 'i' is a
    // pointer to one of the 'functionImpl' functions defined above, invoking
    // an lvalue of type (template parameter) 'VISITOR' with alternative 'i' of
    // (template parameter) 'VARIANT' and implicitly converting the return type
    // to 'R'.  The primary template (used when 'VARIANT' is not a
    // 'bsl::variant') is not defined.  Note that the 'UNUSED' template
    // parameter is required due to limitations of 'sim_cpp11_features.pl'.

template <class R, class VISITOR, class HEAD, class... TAIL>
struct Variant_VTable<R, VISITOR, bsl::variant<HEAD, TAIL...> > {
    // This partial specialization is used when the 'bsl::variant' is not
    // 'const'.

    typedef R (*FuncPtr)(VISITOR&, bsl::variant<HEAD, TAIL...>&);

    typedef bsl::variant<HEAD, TAIL...> VARIANT;

    static const FuncPtr *map()
        // Return a pointer to the first element of an array of function
        // pointers that invoke the visitor with an lvalue referring to the
        // corresponding alternative.
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_Function<R, VISITOR, VARIANT, HEAD>::functionImpl),
            &(Variant_Function<R, VISITOR, VARIANT, TAIL>::functionImpl)...};
        return s_mapArray;
    }

    static const FuncPtr *moveMap()
        // Return a pointer to the first element of an array of function
        // pointers that invoke the visitor with a 'bslmf::MovableRef'
        // referring to the corresponding alternative.
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_MoveFunction<R, VISITOR, VARIANT, HEAD>::functionImpl),
            &(Variant_MoveFunction<R, VISITOR, VARIANT, TAIL>::
                  functionImpl)...};
        return s_mapArray;
    }

    static const FuncPtr *mapId()
        // Return a pointer to the first element of an array of function
        // pointers where element 'i' invokes the visitor with a tag
        // representing the index of the first alternative whose type is the
        // same as that of alternative 'i', and an lvalue referring to
        // alternative 'i'.
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_FunctionId<R, VISITOR, VARIANT, HEAD>::functionImpl),
            &(Variant_FunctionId<R, VISITOR, VARIANT, TAIL>::functionImpl)...};
        return s_mapArray;
    }
};

template <class R, class VISITOR, class HEAD, class... TAIL>
struct Variant_VTable<R, VISITOR, const bsl::variant<HEAD, TAIL...> > {
    // This partial specialization is used when the 'bsl::variant' is 'const'.

    typedef R (*FuncPtr)(VISITOR&, const bsl::variant<HEAD, TAIL...>&);

    typedef const bsl::variant<HEAD, TAIL...> VARIANT;

    static const FuncPtr *map()
        // Return a pointer to the first element of an array of function
        // pointers that invoke the visitor with an lvalue referring to the
        // corresponding alternative.
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_Function<R, VISITOR, VARIANT, HEAD>::functionImpl),
            &(Variant_Function<R, VISITOR, VARIANT, TAIL>::functionImpl)...};
        return s_mapArray;
    }

    static const FuncPtr *mapId()
        // Return a pointer to the first element of an array of function
        // pointers where element 'i' invokes the visitor with a tag
        // representing the index of the first alternative whose type is the
        // same as that of alternative 'i', and an lvalue referring to
        // alternative 'i'.
    {
        static const FuncPtr s_mapArray[] = {
            &(Variant_FunctionId<R, VISITOR, VARIANT, HEAD>::functionImpl),
            &(Variant_FunctionId<R, VISITOR, VARIANT, TAIL>::functionImpl)...};
        return s_mapArray;
    }
};

template <class R, class VISITOR, class HEAD, class... TAIL>
R Variant_VisitImpl(VISITOR& visitor, bsl::variant<HEAD, TAIL...>& variant)
    // This component-private function invokes the specified 'visitor' on the
    // active alternative of the specified 'variant', implicitly converting the
    // return type to the explicitly specified (template parameter) 'R'.  The
    // behavior is undefined if 'variant' does not hold a value.
{
    typedef bsl::variant<HEAD, TAIL...>                           VARIANT;
    typedef typename Variant_VTable<R, VISITOR, VARIANT>::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 'VISITOR' and 'VARIANT',
    // then invoke the one corresponding to the active index.
    FuncPtr func_ptr =
        Variant_VTable<R, VISITOR, VARIANT>::map()[variant.index()];

    return (*func_ptr)(visitor, variant);
}

template <class R, class VISITOR, class HEAD, class... TAIL>
R Variant_VisitImpl(VISITOR&                           visitor,
                    const bsl::variant<HEAD, TAIL...>& variant)
    // This component-private function invokes the specified 'visitor' on the
    // active alternative of the specified 'variant', implicitly converting the
    // return type to the explicitly specified (template parameter) 'R'.  The
    // behavior is undefined if 'variant' does not hold a value.
{
    typedef const bsl::variant<HEAD, TAIL...>                     VARIANT;
    typedef typename Variant_VTable<R, VISITOR, VARIANT>::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 'VISITOR' and 'VARIANT',
    // then invoke the one corresponding to the active index.
    FuncPtr func_ptr =
        Variant_VTable<R, VISITOR, VARIANT>::map()[variant.index()];

    return (*func_ptr)(visitor, variant);
}

template <class R, class VISITOR, class HEAD, class... TAIL>
R Variant_MoveVisitImpl(VISITOR& visitor, bsl::variant<HEAD, TAIL...>& variant)
    // This component-private function invokes the specified 'visitor' on a
    // 'bslmf::MovableRef' referring to the active alternative of the specified
    // 'variant', implicitly converting the return type to the explicitly
    // specified (template parameter) 'R'.  The behavior is undefined if
    // 'variant' does not hold a value.
{
    typedef bsl::variant<HEAD, TAIL...>                           VARIANT;
    typedef typename Variant_VTable<R, VISITOR, VARIANT>::FuncPtr FuncPtr;

    // Generate the table of all function pointers for 'VISITOR' and 'VARIANT',
    // then invoke the one corresponding to the active index.
    FuncPtr func_ptr =
        Variant_VTable<R, VISITOR, VARIANT>::moveMap()[variant.index()];

    return (*func_ptr)(visitor, variant);
}

// The visitation functions below are the same as the ones above except that
// the visitor is invoked with an additional argument of type
// 'bsl::in_place_index_t'.  They are used internally for visitors that
// participate in the 'variant' implementation.
template <class R, class VISITOR, class HEAD, class... TAIL>
R Variant_VisitImplId(VISITOR& visitor, bsl::variant<HEAD, TAIL...>& variant)
    // This component-private function invokes the specified 'visitor' with a
    // 'bsl::in_place_index_t<INDEX>' tag (where 'INDEX' is the index of the
    // first alternative that has the same type as the active alternative of
    // the specified 'variant') and the active alternative of 'variant',
    // implicitly converting the return type to the explicitly specified
    // (template parameter) 'R'.  The behavior is undefined if 'variant' does
    // not hold a value.
{
    typedef bsl::variant<HEAD, TAIL...>                           VARIANT;
    typedef typename Variant_VTable<R, VISITOR, VARIANT>::FuncPtr FuncPtr;

    FuncPtr func_ptr =
        Variant_VTable<R, VISITOR, VARIANT>::mapId()[variant.index()];

    return (*func_ptr)(visitor, variant);
}

template <class R, class VISITOR, class HEAD, class... TAIL>
R Variant_VisitImplId(VISITOR&                           visitor,
                      const bsl::variant<HEAD, TAIL...>& variant)
    // This component-private function invokes the specified 'visitor' with a
    // 'bsl::in_place_index_t<INDEX>' tag (where 'INDEX' is the index of the
    // first alternative that has the same type as the active alternative of
    // the specified 'variant') and the active alternative of 'variant',
    // implicitly converting the return type to the explicitly specified
    // (template parameter) 'R'.  The behavior is undefined if 'variant' does
    // not hold a value.
{
    typedef const bsl::variant<HEAD, TAIL...>                     VARIANT;
    typedef typename Variant_VTable<R, VISITOR, VARIANT>::FuncPtr FuncPtr;

    FuncPtr func_ptr =
        Variant_VTable<R, VISITOR, VARIANT>::mapId()[variant.index()];

    return (*func_ptr)(visitor, variant);
}

#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_RET, class VISITOR, class VARIANT>
t_RET visit(VISITOR&& visitor, VARIANT&& variant)
    // Return the result of invoking the specified 'visitor' with the currently
    // active alternative of the specified 'variant', implicitly converting the
    // result to the explicitly specified (template parameter) 't_RET'.  If
    // 'variant' does not hold a value, an exception of type
    // 'bsl::bad_variant_access' is thrown.  Note that unlike the 'visit'
    // overload below that deduces its return type, this overload does not
    // require the visitor to yield the exact same type for each alternative,
    // but only requires each such type to be implicitly convertible to 'R'.
    // This function differs from the standard in the following ways:
    //: o only one variant is supported
    //:
    //: o constexpr is not implemented
    //:
    //: o before C++17, only 'VISITOR(ALTERNATIVE)' form of visitation is
    //:   supported; cases where the visitor is a pointer to member are not
    //:   supported.
{
    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return BloombergLP::bslstl::Variant_VisitImpl<t_RET>(
        std::forward<VISITOR>(visitor), std::forward<VARIANT>(variant));
}

template <class VISITOR, class VARIANT>
typename bsl::invoke_result<
    VISITOR,
    typename BloombergLP::bslstl::Variant_CVQualAlt<VARIANT, 0>::type>::type
visit(VISITOR&& visitor, VARIANT&& variant)
    // Return the result of invoking the specified 'visitor' with the currently
    // active alternative of the specified 'variant'.  If 'variant' does not
    // hold a value, an exception of type 'bsl::bad_variant_access' is thrown.
    // For each alternative, invocation of the visitor with that alternative
    // shall be of the same type and value category.  This function differs
    // from the standard in the following ways:
    //: o only one variant is supported
    //:
    //: o constexpr is not implemented
    //:
    //: o before C++17, only 'VISITOR(ALTERNATIVE)' form of visitation is
    //:   supported; cases where the visitor is a pointer to member are not
    //:   supported.
{
    typedef typename bsl::invoke_result<
        VISITOR,
        typename BloombergLP::bslstl::Variant_CVQualAlt<VARIANT,
                                                        0>::type>::type R;
    static_assert(BloombergLP::bslstl::
                      Variant_IsSameReturnType<R, VISITOR, VARIANT>::value,
                  "The value type and category of invoking the visitor with "
                  "every alternative is not the same");
    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return BloombergLP::bslstl::Variant_VisitImpl<R>(
        std::forward<VISITOR>(visitor), std::forward<VARIANT>(variant));
}

template <class t_RET, class VISITOR, class VARIANT>
t_RET visitR(VISITOR& visitor, VARIANT&& variant)
    // Return the result of invoking the specified 'visitor' with the currently
    // active alternative of the specified 'variant', implicitly converting the
    // result to the explicitly specified (template parameter) 't_RET'.  If
    // 'variant' does not hold a value, an exception of type
    // 'bsl::bad_variant_access' is thrown.  This function is provided in all
    // language versions for compatibility with the C++03 version (see below),
    // but in new code in C++11 and later, 'bsl::visit' should be used instead
    // of this function.
{
    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return BloombergLP::bslstl::Variant_VisitImpl<t_RET>(
        visitor, std::forward<VARIANT>(variant));
}
#else  //BSL_VARIANT_FULL_IMPLEMENTATION

template <class t_RET, class VISITOR, class VARIANT>
t_RET visitR(VISITOR& visitor, VARIANT& variant)
    // Return the result of invoking the specified 'visitor' with the currently
    // active alternative of the specified 'variant', implicitly converting the
    // result to the explicitly specified (template parameter) 't_RET'.  If
    // 'variant' does not hold a value, an exception of type
    // 'bsl::bad_variant_access' is thrown.  Unlike 'visit', which deduces its
    // return type, this function does not require the visitor to yield the
    // exact same type for each alternative, but only requires each such type
    // to be implicitly convertible to 't_RET'.  Implementation note: Due to
    // limitations of the type deduction facility in C++03, this function can
    // not share the same name as the deduced type visit because it is not
    // possible to discern between invocation of non deduced return type visit
    // where the return type parameter is explicitly specified and invocation
    // of deduced return type visit where the visitor type parameter is
    // explicitly specified.
{
    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return BloombergLP::bslstl::Variant_VisitImpl<t_RET>(visitor, variant);
}

template <class t_RET, class VISITOR, class VARIANT>
t_RET visitR(VISITOR& visitor, BloombergLP::bslmf::MovableRef<VARIANT> variant)
    // Return the result of invoking the specified 'visitor' with a
    // 'bslmf::MovableRef' referring to the currently active alternative of the
    // specified 'variant', implicitly converting the result to the explicitly
    // specified (template parameter) 't_RET'.  If 'variant' does not hold a
    // value, an exception of type 'bsl::bad_variant_access' is thrown.  Unlike
    // 'visit', which deduces its return type, this function does not require
    // the visitor to yield the exact same type for each alternative, but only
    // requires each such type to be implicitly convertible to 't_RET'.
    // Implementation note: Due to limitations of the type deduction facility
    // in C++03, this function can not share the same name as the deduced type
    // visit because it is not possible to discern between invocation of non
    // deduced return type visit where the return type parameter is explicitly
    // specified and invocation of deduced return type visit where the visitor
    // type parameter is explicitly specified.
{
    VARIANT& lvalue = variant;
    if (lvalue.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    return BloombergLP::bslstl::Variant_MoveVisitImpl<t_RET>(visitor, lvalue);
}

template <class VISITOR, class VARIANT>
typename bsl::invoke_result<
    VISITOR,
    typename bsl::variant_alternative<0, VARIANT>::type&>::type
visit(VISITOR& visitor, VARIANT& variant)
    // Return the result of invoking the specified 'visitor' with the currently
    // active alternative of the specified 'variant'.  If 'variant' does not
    // hold a value, an exception of type 'bsl::bad_variant_access' is thrown.
    // For each alternative, invocation of the visitor with that alternative
    // shall be of the same type and value category.
{
    if (variant.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    typedef typename bsl::invoke_result<
        VISITOR,
        typename bsl::variant_alternative<0, VARIANT>::type&>::type R;

    BSLMF_ASSERT((BloombergLP::bslstl::
                      Variant_IsSameReturnType<R, VISITOR, VARIANT>::value));

    return BloombergLP::bslstl::Variant_VisitImpl<R>(visitor, variant);
}

template <class VISITOR, class VARIANT>
typename bsl::invoke_result<
    VISITOR,
    BloombergLP::bslmf::MovableRef<
        typename bsl::variant_alternative<0, VARIANT>::type> >::type
visit(VISITOR& visitor, BloombergLP::bslmf::MovableRef<VARIANT> variant)
    // Return the result of invoking the specified 'visitor' with a
    // 'bslmf::MovableRef' referring to the currently active alternative of the
    // specified 'variant'.  If 'variant' does not hold a value, an exception
    // of type 'bsl::bad_variant_access' is thrown.  For each alternative,
    // invocation of the visitor with that alternative shall be of the same
    // type and value category.
{
    VARIANT& lvalue = variant;
    if (lvalue.valueless_by_exception()) {
        BSLS_THROW(bsl::bad_variant_access());
    }
    typedef typename bsl::invoke_result<
        VISITOR,
        BloombergLP::bslmf::MovableRef<
            typename bsl::variant_alternative<0, VARIANT>::type> >::type R;

    BSLMF_ASSERT(
        (BloombergLP::bslstl::
             Variant_IsSameMoveReturnType<R, VISITOR, VARIANT>::value));

    return BloombergLP::bslstl::Variant_MoveVisitImpl<R>(visitor, lvalue);
}

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                          // ========================
                          // class Variant_NoSuchType
                          // ========================

struct Variant_NoSuchType {
    // This component-private trivial tag type is used to distinguish between
    // arguments passed by a user, and an 'enable_if' argument.  It is not
    // default constructible so the following construction never invokes a
    // constrained single parameter constructor:
    //..
    //   struct SomeType
    //   {
    //       SomeType(int, const std::string &s = "default"){}
    //   };
    //
    //   variant<SomeType> o(1, {});
    //..

    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR Variant_NoSuchType(int)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'Variant_NoSuchType' object.
};

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
class Variant_DataImp {
    // This component-private class manages an object of type (template
    // parameter) 'TYPE' that is stored in a 'variant' object.  Note that
    // 'TYPE' may be const-qualified, in which case the internally stored
    // object is not 'const', but the interface of this class prevents
    // modification thereof.

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;  // in-place 'TYPE' object

  public:
    // CREATORS
    Variant_DataImp() = default;
        // Create a 'Variant_DataImp' object that holds an empty buffer.

    // MANIPULATORS
    template <class... ARGS>
    Variant_DataImp(ARGS&&... args);
        // Create a 'Variant_DataImp' object whose stored 'TYPE' object is
        // constructed by forwarding the specified 'args' to
        // 'ConstructionUtil::construct directly.  This means the first
        // argument in 'args' must be a pointer to 'bslma::Allocator', which
        // will be ignored if 'TYPE' is not allocator-aware.

    BSLS_KEYWORD_CONSTEXPR_CPP14 TYPE&  value() &;
    BSLS_KEYWORD_CONSTEXPR_CPP14 TYPE&& value() &&;
        // Return a reference to the stored 'TYPE' object.  The behavior is
        // undefined unless this object contains a 'TYPE' object (i.e., this
        // object is currently managing the active alternative of a
        // 'bsl::variant' object).

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 const TYPE& value() const&;
    BSLS_KEYWORD_CONSTEXPR_CPP14 const TYPE&& value() const&&;
        // Return a const reference to the stored 'TYPE' object.  The behavior
        // is undefined unless this object contains a 'TYPE' object (i.e., this
        // object is currently managing the active alternative of a
        // 'bsl::variant' object).
};

template <class... TYPES>
union Variant_Union {
    // This component-private union can hold a 'Variant_DataImp' object for
    // any alternative in (template parameters) 'TYPES...'.  The primary
    // template is used when 'TYPES...' is empty.
};

template <class HEAD, class... TAIL>
union Variant_Union<HEAD, TAIL...> {
    // This partial specialization uses template recursion because C++ does not
    // support member packs.

    // PUBLIC DATA
    Variant_DataImp<HEAD>  d_head;
    Variant_Union<TAIL...> d_tail;

    // CREATORS
    Variant_Union() = default;
        // Create a 'Variant_Union' object having no active alternative.

    template <class... ARGS>
    Variant_Union(bsl::in_place_index_t<0>, ARGS&&... args)
    : d_head(std::forward<ARGS>(args)...)
        // Create a 'Variant_Union' object holding an object of type 'HEAD'
        // direct-initialized from the specified 'args'.
    {
    }

    template <size_t t_INDEX, class... ARGS>
    Variant_Union(bsl::in_place_index_t<t_INDEX>, ARGS&&... args)
    : d_tail(bsl::in_place_index_t<t_INDEX - 1>(), std::forward<ARGS>(args)...)
        // Create a 'Variant_Union' object holding the alternative at index
        // (template parameter) 't_INDEX' in (template parameters) 'HEAD,
        // TAIL...', direct-initialized from the specified 'args'.  Note that
        // 't_INDEX' is not necessarily the absolute index of the desired
        // alternative in the 'bsl::variant' object.  We use the tag to
        // "unravel" the union until we get to the desired alternative type.
    {
    }
};
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
class Variant_DataImp {
    // This component-private class manages an object of type (template
    // parameter) 'TYPE' that is stored in a 'variant' object.  Note that
    // 'TYPE' may be const-qualified, in which case the internally stored
    // object is not 'const', but the interface of this class prevents
    // modification thereof.

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;  // in-place 'TYPE' object

  public:
    // CREATORS
    //! Variant_DataImp() = default;
        // Create a 'Variant_DataImp' object that holds an empty buffer.

    // MANIPULATORS
    TYPE& value();
        // Return a reference to the stored 'TYPE' object.  The behavior is
        // undefined unless the enclosing union contains a 'TYPE' object (i.e.,
        // the enclosing 'bsl::variant' has an active alternative of type
        // 'TYPE').

    // ACCESSORS
    const TYPE& value() const;
        // Return a const reference to the stored 'TYPE' object.  The behavior
        // is undefined unless the enclosing union contains a 'TYPE' object
        // (i.e., the enclosing 'bsl::variant' has an active alternative of
        // type 'TYPE').
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD = void, class TAIL0 = void, class... TAIL>
union Variant_Union;
    // This component-private union can hold a 'Variant_DataImp' object for
    // any alternative in (template parameters) 'HEAD, TAIL0, TAIL...'.  Note
    // that the declaration and definition of the primary template must be kept
    // separate in order for 'sim_cpp11_features.pl' to work properly.  The
    // purpose of the 'TAIL0' template parameter is explained below.

template <>
union Variant_Union<void, void> {
    // This specialization is used when the type list is empty.
};

template <class HEAD, class TAIL0, class... TAIL>
union Variant_Union<HEAD, TAIL0, TAIL...> {
    // The implementation of this template uses template recursion because C++
    // does not support member packs.  The 'TAIL0' template parameter is needed
    // in order to avoid an empty template argument list in the type of the
    // 'd_tail' member, which causes an issue with the IBM compiler.

    // PUBLIC DATA
    Variant_DataImp<HEAD>         d_head;
    Variant_Union<TAIL0, TAIL...> d_tail;
};
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

                     // ==================================
                     // class Variant_CopyConstructVisitor
                     // ==================================

template <class VARIANT_BASE>
class Variant_CopyConstructVisitor {
    // This component-private class is a visitor that is used to implement the
    // copy constructor for 'bsl::variant'.

  private:
    // DATA
    VARIANT_BASE *d_variant_p;

  public:
    // CREATORS
    explicit Variant_CopyConstructVisitor(VARIANT_BASE *variant)
        // Create a 'Variant_CopyConstructVisitor' object that, when invoked,
        // will copy-construct an alternative in the specified 'variant'.
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

    // ACCESSORS
    template <size_t INDEX, class TYPE>
    void operator()(bsl::in_place_index_t<INDEX>, const TYPE& other) const
        // Copy-construct the alternative at index (template parameter) 'INDEX'
        // from the specified 'other' (i.e. the alternative that is being
        // visited).  Note that the allocator specified on construction of
        // '*d_variant_p' will be used.
    {
        d_variant_p->template baseEmplace<INDEX>(other);
    }
};

                     // ==================================
                     // class Variant_MoveConstructVisitor
                     // ==================================

template <class VARIANT_BASE>
class Variant_MoveConstructVisitor {
    // This component-private class is a visitor that is used to implement the
    // move constructor for 'bsl::variant'.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    // DATA
    VARIANT_BASE *d_variant_p;

  public:
    // CREATORS
    explicit Variant_MoveConstructVisitor(VARIANT_BASE *variant)
        // Create a 'Variant_MoveConstructVisitor' object that, when invoked,
        // will move-construct an alternative in the specified 'variant'.
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

    // ACCESSORS
    template <size_t INDEX, class TYPE>
    void operator()(bsl::in_place_index_t<INDEX>, TYPE& other) const
        // Move-construct the alternative at index (template parameter) 'INDEX'
        // from the specified 'other' (i.e. the alternative that is being
        // visited).  Note that the allocator specified on construction of
        // '*d_variant_p' will be used.
    {
        d_variant_p->template baseEmplace<INDEX>(MoveUtil::move(other));
    }
};

                      // ===============================
                      // class Variant_CopyAssignVisitor
                      // ===============================

template <class VARIANT>
class Variant_CopyAssignVisitor {
    // This component-private class is a visitor that is used to implement the
    // copy assignment operator for 'bsl::variant'.

  private:
    // DATA
    VARIANT *d_variant_p;

  public:
    // CREATORS
    explicit Variant_CopyAssignVisitor(VARIANT *variant)
        // Create a 'Variant_CopyAssignVisitor' object that, when invoked, will
        // copy-assign to the active alternative of the specified 'variant'.
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }


  public:
    // ACCESSORS
    template <size_t INDEX, class TYPE>
    void operator()(bsl::in_place_index_t<INDEX>, const TYPE& value) const
        // Copy-assign to the active alternative of '*d_variant_p' from the
        // specified 'value' (i.e. the alternative that is being visited).  The
        // behavior is undefined unless (template parameter) 'INDEX' is the
        // index of the active alternative of '*d_variant_p' (or, in C++03, the
        // index of an alternative that has the same type as the active
        // alternative).
    {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
        bsl::get<INDEX>(*d_variant_p) = value;
#else
        // When invoking this visitor in C++03, 'INDEX' may not be the index of
        // the active alternative, but it will be an index of an alternative of
        // same type.  However, because 'TYPE' is deduced, it may be missing
        // cv-qualifications.  'ALT_TYPE' will give the correct
        // cv-qualification.
        typedef
            typename bsl::variant_alternative<INDEX, VARIANT>::type ALT_TYPE;
        Variant_UnsafeGet<ALT_TYPE>(*d_variant_p) = value;
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    }
};

                      // ===============================
                      // class Variant_MoveAssignVisitor
                      // ===============================

template <class VARIANT>
class Variant_MoveAssignVisitor {
    // This component-private class is a visitor that is used to implement the
    // move assignment operator for 'bsl::variant'.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    // DATA
    VARIANT *d_variant_p;

  public:
    // CREATORS
    explicit Variant_MoveAssignVisitor(VARIANT *variant)
        // Create a 'Variant_MoveAssignVisitor' object that, when invoked, will
        // move-assign to the active alternative of the specified 'variant'.
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

  public:
    // ACCESSORS
    template <size_t INDEX, class TYPE>
    void operator()(bsl::in_place_index_t<INDEX>, TYPE& value) const
        // Move-assign to the active alternative of '*d_variant_p' from the
        // specified 'value' (i.e. the alternative that is being visited).  The
        // behavior is undefined unless (template parameter) 'INDEX' is the
        // index of the active alternative of '*d_variant_p' (or, in C++03, the
        // index of an alternative that has the same type as the active
        // alternative).
    {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
        bsl::get<INDEX>(*d_variant_p) = MoveUtil::move(value);
#else
        Variant_UnsafeGet<TYPE>(*d_variant_p)     = MoveUtil::move(value);
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    }
};

                      // ================================
                      // struct Variant_DestructorVisitor
                      // ================================

struct Variant_DestructorVisitor {
    // This component-private struct is a visitor that destroys the active
    // alternative of the visited 'variant' object.

  public:
    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& object) const
        // Destroy the specified 'object', which is the alternative that is
        // being visited.
    {
        bslma::DestructionUtil::destroy(&object);
    }
};
                       // ==============================
                       // struct Variant_##NAME##Visitor
                       // ==============================

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
#define BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(NAME, OP)                      \
    template <class VARIANT>                                                  \
    struct Variant_##NAME##Visitor {                                          \
        /*  When using this visitor, construct the visitor using the left  */ \
        /*  operand, and call visitation on the right operand.             */ \
        explicit Variant_##NAME##Visitor(const VARIANT *variant)              \
        : d_variant_p(variant)                                                \
        {                                                                     \
            BSLS_ASSERT_SAFE(d_variant_p);                                    \
        }                                                                     \
                                                                              \
      private:                                                                \
        const VARIANT *d_variant_p;                                           \
                                                                              \
      public:                                                                 \
        template <size_t INDEX, class TYPE>                                   \
        bool operator()(bsl::in_place_index_t<INDEX>,                         \
                        const TYPE&                  value) const             \
        {                                                                     \
            return (bsl::get<INDEX>(*d_variant_p) OP value);                  \
        }                                                                     \
    };                                                                        \
                                                                              \
    template <class VARIANT>                                                  \
    bool Variant_ImpUtil<VARIANT>::NAME##Impl(const VARIANT& lhs,             \
                                              const VARIANT& rhs)             \
    {                                                                         \
        BSLS_ASSERT(lhs.index() == rhs.index() &&                             \
                    !lhs.valueless_by_exception());                           \
                                                                              \
        Variant_##NAME##Visitor<VARIANT> NAME##Visitor(                       \
                                                   BSLS_UTIL_ADDRESSOF(lhs)); \
        return BSLSTL_VARIANT_VISITID(bool, NAME##Visitor, rhs);              \
    }
#else
#define BSLSTL_VARIANT_RELOP_VISITOR_DEFINITON(NAME, OP)                      \
    template <class VARIANT>                                                  \
    struct Variant_##NAME##Visitor {                                          \
        Variant_##NAME##Visitor(const VARIANT *variant)                       \
        : d_variant_p(variant)                                                \
        {                                                                     \
            BSLS_ASSERT_SAFE(d_variant_p);                                    \
        }                                                                     \
                                                                              \
      private:                                                                \
        const VARIANT *d_variant_p;                                           \
                                                                              \
      public:                                                                 \
        template <size_t INDEX, class TYPE>                                   \
        bool operator()(bsl::in_place_index_t<INDEX>,                         \
                        TYPE&                  value) const                   \
        {                                                                     \
            /* When invoking this visitor in C++03, 'INDEX' may not be the */ \
            /* index of the active alternative, but it will be an index of */ \
            /* an alternative of same type.  However, because 'TYPE' is    */ \
            /* deduced, it may have incorrect cv-qualifications.           */ \
            /* 'ALT_TYPE' will give the correct cv-qualification.          */ \
            typedef typename bsl::variant_alternative<INDEX, VARIANT>::type   \
                                                                    ALT_TYPE; \
            return (Variant_UnsafeGet<ALT_TYPE>(*d_variant_p) OP value);      \
        }                                                                     \
    };                                                                        \
                                                                              \
    template <class VARIANT>                                                  \
    bool Variant_ImpUtil<VARIANT>::NAME##Impl(const VARIANT& lhs,             \
                                              const VARIANT& rhs)             \
    {                                                                         \
        BSLS_ASSERT(lhs.index() == rhs.index() &&                             \
                    !lhs.valueless_by_exception());                           \
                                                                              \
        Variant_##NAME##Visitor<VARIANT> NAME##Visitor(                       \
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

template <class VARIANT>
class Variant_SwapVisitor {
    // This component-private class is a visitor that is used to implement
    // 'bsl::variant::swap'.

  private:
    // DATA
    VARIANT *d_variant_p;

  public:
    // CREATORS
    explicit Variant_SwapVisitor(VARIANT *variant)
        // Create a 'Variant_SwapVisitor' object that, when invoked, will swap
        // the visited alternative with the active alternative of the specified
        // 'variant'.
    : d_variant_p(variant)
    {
        BSLS_ASSERT_SAFE(d_variant_p);
    }

    // ACCESSORS
    template <size_t INDEX, class TYPE>
    void operator()(bsl::in_place_index_t<INDEX>, TYPE& value) const
        // Swap the alternative having index (template parameter) 'INDEX' in
        // '*d_variant_p' with the specified 'value' (i.e. the alternative that
        // is being visited).  The behavior is undefined unless 'INDEX' is the
        // index of the active alternative of '*d_variant_p' (or, in C++03, the
        // index of an alternative that has the same type as the active
        // alternative).
    {
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
        BloombergLP::bslalg::SwapUtil::swap(
                           BSLS_UTIL_ADDRESSOF(value),
                           BSLS_UTIL_ADDRESSOF(bsl::get<INDEX>(*d_variant_p)));
#else
        // When invoking this visitor in C++03, 'INDEX' may not be the index of
        // the active alternative, but it will be an index of an alternative of
        // same type. Swap can not be invoked on a constant variant, so the cv
        // qualifications will match.
        BloombergLP::bslalg::SwapUtil::swap(
                 BSLS_UTIL_ADDRESSOF(value),
                 BSLS_UTIL_ADDRESSOF((Variant_UnsafeGet<TYPE>(*d_variant_p))));
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    }
};

                         // =========================
                         // class Variant_HashVisitor
                         // =========================

template <class HASHALG>
class Variant_HashVisitor {
    // This component-private class is a visitor that is used to implement
    // 'hashAppend' for 'bsl::variant'.

  private:
    // DATA
    HASHALG& d_hashAlg;

  public:
    // CREATORS
    explicit Variant_HashVisitor(HASHALG& hashAlg)
        // Create a 'Variant_HashVisitor' object that, when invoked, appends to
        // the specified 'hashAlg'.
    : d_hashAlg(hashAlg)
    {
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& value) const
        // Append the specified 'value' (i.e., the alternative that is being
        // visited) to 'd_hashAlg'.
    {
        using BloombergLP::bslh::hashAppend;
        hashAppend(d_hashAlg, value);
    }
};

template <bool AA>
struct Variant_AllocatorBase {
    // This component-private struct keeps track of the allocator for a
    // 'bsl::variant' object.  The primary template is used when the 'variant'
    // is not allocator-aware (because it has no allocator-aware alternatives).

    // ACCESSORS
    BloombergLP::bslma::Allocator *mechanism() const
        // Return a null pointer.  Note that this method has the same return
        // type as the allocator-aware version so that the remainder of the
        // implementation of 'bsl::variant' is abstracted with respect to
        // whether or not the specialization is allocator-aware; this is why
        // both methods return 'bslma::Allocator*' instead of
        // 'bsl::allocator<char>'.  The returned pointer will be ignored (and
        // not dereferenced) when passed to
        // 'bslma::ConstructionUtil::construct'.
    {
        return NULL;
    }
};

template <>
struct Variant_AllocatorBase<true> {
    // This specialization is used when the 'variant' is allocator-aware.

    // TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
    typedef bsl::allocator<char>               allocator_type;

    // PUBLIC DATA
    allocator_type d_allocator;

    // CREATORS
    Variant_AllocatorBase() {}
        // Create a 'Variant_AllocatorBase' object that holds the currently
        // installed default allocator.

    Variant_AllocatorBase(const Variant_AllocatorBase& original)
        // Create a 'Variant_AllocatorBase' object that holds a copy of the
        // allocator held by the specified 'original'.
    : d_allocator(original.d_allocator)
    {
    }

    Variant_AllocatorBase(allocator_type allocator)
        // Create a 'Variant_AllocatorBase' object that holds a copy of the
        // specified 'allocator'.
    : d_allocator(allocator)
    {
    }

    // ACCESSORS
    BloombergLP::bslma::Allocator *mechanism() const
        // Return the mechanism of the stored allocator.
    {
        return d_allocator.mechanism();
    }
};

                            // ===================
                            // struct variant_base
                            // ===================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
struct Variant_Base
: public BloombergLP::bslstl::Variant_AllocatorBase<
      BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                         TAIL...>::value> {
    // This component-private struct defines the data representation of
    // 'bsl::variant' and contains the implementations of its special member
    // functions (which must be defaulted in 'bsl::variant' itself).  This
    // class also contains implementations of additional constructors and
    // methods needed for the correct functionality of the 'variant' class
    // hierarchy.

    // TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                MoveUtil;
    typedef BloombergLP::bslstl::Variant_AllocatorBase<
        BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                           TAIL...>::value>
                                                              AllocBase;
    typedef BloombergLP::bslstl::Variant_Union<HEAD, TAIL...> Variant_Union;
    typedef bsl::variant<HEAD, TAIL...>                       Variant;

    struct NoAlloc {
        // This trivial tag type is used as a dummy when 'Variant_Base' wraps a
        // non-allocator-aware type.
    };

    typedef typename bsl::conditional<
        BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                           TAIL...>::value,
        bsl::allocator<char>,
        NoAlloc>::type allocator_type;
        // Type alias to the allocator type used by 'variant'.

    // PUBLIC DATA
    size_t d_type;
        // Index of the currently active alternative in this variant object, or
        // 'bsl::variant_npos' if the variant is valueless by exception.

    Variant_Union d_union;
        // Union holding the alternative object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        Variant_Base,
        BloombergLP::bslma::UsesBslmaAllocator,
        (BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                            TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        Variant_Base,
        BloombergLP::bslmf::UsesAllocatorArgT,
        (BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                            TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        Variant_Base,
        BloombergLP::bslmf::IsBitwiseMoveable,
        (BloombergLP::bslstl::Variant_IsBitwiseMoveableAll<HEAD,
                                                           TAIL...>::value));

    // CREATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    Variant_Base();
        // Create a 'Variant_Base' object holding the 0th alternative, which is
        // value-initialized.  If this 'Variant_Base' is allocator-aware, the
        // currently installed default allocator is used to supply memory.

    Variant_Base(const Variant_Base& original);
        // Create a 'Variant_Base' object holding the same alternative (if any)
        // as the specified 'original' object.  If this 'Variant_Base' is
        // allocator-aware, the currently installed default allocator is used
        // to supply memory.  If the 'original' object is not valueless by
        // exception', the contained value is copy-constructed from the
        // contained value of 'original'.  All alternatives shall be copy
        // constructible.

    Variant_Base(Variant_Base&& original);
        // Create a 'Variant_Base' object holding the same alternative (if any)
        // held by the specified 'original'.  If this 'Variant_Base' is
        // allocator-aware, the allocator of 'original' is used to supply
        // memory.  If the 'original' object is not valueless by exception, the
        // contained value is move-constructed from the contained value of
        // 'original'.  All alternatives shall be move constructible.

    template <size_t INDEX, class... ARGS>
    explicit Variant_Base(bsl::in_place_index_t<INDEX>, ARGS&&... args);
        // Create a 'Variant_Base' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from the specified
        // 'args'.  If this 'Variant_Base' is allocator-aware, the currently
        // installed default allocator is used to supply memory.

    // allocator-extended constructors
    Variant_Base(bsl::allocator_arg_t, allocator_type allocator);
        // Create a 'Variant_Base' object holding the 0th alternative, which is
        // value-initialized.  If this 'Variant_Base' is allocator-aware, the
        // specified 'allocator' is used to supply memory.

    Variant_Base(bsl::allocator_arg_t,
                 allocator_type allocator,
                 const Variant& original);
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type allocator,
                 Variant&&      original);
        // Create a 'Variant_Base' object holding the same alternative (if any)
        // held by the specified 'original' object.  If this 'Variant_Base' is
        // allocator-aware, the specified 'allocator' is used to supply memory.
        // If the 'original' object is not valueless by exception, the
        // contained value is copy/move constructed from the contained value of
        // 'original'.  All alternatives shall be copy/move constructible.

    template <size_t INDEX, class... ARGS>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<INDEX>,
                          ARGS&&...      args);
        // Create a 'Variant_Base' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from the specified
        // 'args'.  If this 'Variant_Base' is allocator-aware, the specified
        // 'allocator' is used to supply memory.

#else  // BSL_VARIANT_FULL_IMPLEMENTATION

    Variant_Base();
        // Create a 'Variant_Base' object holding the 0th alternative, which is
        // value-initialized.  If this 'Variant_Base' is allocator-aware, the
        // currently installed default constructor is used to supply memory.

    Variant_Base(const Variant& original);
        // Create a 'Variant_Base' object holding the same alternative (if any)
        // as the specified 'original' object.  If this 'Variant_Base' is
        // allocator-aware, the currently installed default allocator is used
        // to supply memory.  If the 'original' object is not valueless by
        // exception', the contained value is copy-constructed from the
        // contained value of 'original'.  All alternatives shall be copy
        // constructible.

    Variant_Base(BloombergLP::bslmf::MovableRef<Variant> original);
        // Create a 'Variant_Base' object holding the same alternative (if any)
        // held by the specified 'original'.  If this 'Variant_Base' is
        // allocator-aware, the allocator of 'original' is used to supply
        // memory.  If the 'original' object is not valueless by exception, the
        // contained value is move-constructed from the contained value of
        // 'original'.  All alternatives shall be move constructible.

    template <size_t INDEX>
    explicit Variant_Base(bsl::in_place_index_t<INDEX>);
        // Create a 'Variant_Base' object holding the alternative with index
        // (template parameter) 'INDEX', which is value-initialized.  If this
        // 'Variant_Base' is allocator-aware, the currently installed default
        // constructor is used to supply memory.

    template <size_t INDEX, class ARG>
    explicit Variant_Base(bsl::in_place_index_t<INDEX>, const ARG& arg);
        // Create a 'Variant_Base' object holding the alternative with index
        // (template parameter) 'INDEX', which is direct-initialized from the
        // specified 'arg'.  If this 'Variant_Base' is allocator-aware, the
        // currently installed default constructor is used to supply memory.

    // allocator-extended constructors
    Variant_Base(bsl::allocator_arg_t, allocator_type allocator);
        // Create a 'Variant_Base' object holding the 0th alternative, which is
        // value-initialized.  If this 'Variant_Base' is allocator-aware, the
        // specified 'allocator' is used to supply memory.

    Variant_Base(bsl::allocator_arg_t,
                 allocator_type                          allocator,
                 const Variant&                          original);
    Variant_Base(bsl::allocator_arg_t,
                 allocator_type                          allocator,
                 BloombergLP::bslmf::MovableRef<Variant> original);
        // Create a 'Variant_Base' object holding the same alternative (if any)
        // held by the specified 'original' object.  If this 'Variant_Base' is
        // allocator-aware, the specified 'allocator' is used to supply memory.
        // If the 'original' object is not valueless by exception, the
        // contained value is copy/move constructed from the contained value of
        // 'original'.  All alternatives shall be copy/move constructible.

    template <size_t INDEX>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<INDEX>);
        // Create a 'Variant_Base' object holding the alternative with index
        // (template parameter) 'INDEX', which is value-initialized.  If this
        // 'Variant_Base' is allocator-aware, the specified 'allocator' is used
        // to supply memory.

    template <size_t INDEX, class ARG>
    explicit Variant_Base(bsl::allocator_arg_t,
                          allocator_type allocator,
                          bsl::in_place_index_t<INDEX>,
                          const ARG&     arg);
        // Create a 'Variant_Base' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from 'arg'.  If
        // this 'Variant_Base' is allocator-aware, the specified 'allocator' is
        // used to supply memory.

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    ~Variant_Base();
        // Destroy this object.  The contained value, if any, is destroyed.

    // MANIPULATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    template <size_t INDEX, class... ARGS>
    void baseEmplace(ARGS&&...);
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, direct-initialized from the specified 'args'.  If this
        // 'Variant_Base' object already holds a value, that contained value is
        // destroyed before the new object is created.  If the alternative is
        // allocator-aware, it uses the allocator specified upon the
        // construction of this 'Variant_Base' object to supply memory; passing
        // an allocator argument to this method results in two allocators being
        // passed to the alternative constructor, resulting in a likely
        // compilation error.  Note that if the constructor of the alternative
        // exits via an exception, this object is left in the valueless by
        // exception state.
#else   //  BSL_VARIANT_FULL_IMPLEMENTATION
    template <size_t INDEX>
    void baseEmplace();
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, which is value-initialized.  If this 'Variant_Base' object
        // already holds a value, that contained value is destroyed before the
        // new object is created.  If the alternative is allocator-aware, it
        // uses the allocator specified upon the construction of this
        // 'Variant_Base' object to supply memory; passing an allocator
        // argument to this method results in two allocators being passed to
        // the alternative constructor, resulting in a likely compilation
        // error.  Note that if the constructor of the alternative exits via an
        // exception, this object is left in the valueless by exception state.

    template <size_t INDEX, class ARG>
    void baseEmplace(const ARG& arg);
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, direct-initialized from the specified 'arg'.  If this
        // 'Variant_Base' object already holds a value, that contained value is
        // destroyed before the new object is created.  If the alternative is
        // allocator-aware, it uses the allocator specified upon the
        // construction of this 'Variant_Base' object to supply memory; passing
        // an allocator argument to this method results in two allocators being
        // passed to the alternative constructor, resulting in a likely
        // compilation error.  Note that if the constructor of the alternative
        // exits via an exception, this object is left in the valueless by
        // exception state.
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    Variant_Base& operator=(const Variant_Base& rhs);
        // If the specified 'rhs' holds the same alternative type as this
        // object, copy assign the contained value of 'rhs' to the contained
        // value of this object.  Otherwise, destroy the contained value of
        // this object (if any) and, if 'rhs' holds a value, copy-construct the
        // corresponding alternative of this object from the contained value of
        // 'rhs'.  The allocators of this object and 'rhs' both remain
        // unchanged.  If the construction of a new alternative object exits
        // via an exception, this 'Variant_Base' object is left in a valueless
        // by exception state.  This behavior differs from the standard, for
        // reasons that are explained in the documentation for
        // 'variant::operator='.  All alternatives shall be copy constructible
        // and copy assignable.

    Variant_Base& operator=(BloombergLP::bslmf::MovableRef<Variant_Base> rhs);
        // If the specified 'rhs' holds the same alternative type as this
        // object, move assign the contained value of 'rhs' to the contained
        // value of this object.  Otherwise, destroy the currently held
        // alternative object (if any) and, if 'rhs' holds a value,
        // move-construct the corresponding alternative of this object from the
        // contained value of 'rhs'.  The allocators of this object and 'rhs'
        // both remain unchanged.  If the construction of a new alternative
        // object exits via an exception, this 'Variant_Base' object is left in
        // a valueless by exception state.  All alternatives shall be move
        // constructible and move assignable.

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Destroy the contained value, if any.
};
#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                               // =============
                               // class variant
                               // =============

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
class variant
: private BloombergLP::bslstl::Variant_Base<HEAD, TAIL...>
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
,
  private BloombergLP::bslstl::Variant_SMFBase<
      BloombergLP::bslstl::Variant_IsCopyConstructibleAll<HEAD,
                                                          TAIL...>::value,
      BloombergLP::bslstl::Variant_IsMoveConstructibleAll<HEAD,
                                                          TAIL...>::value,
      BloombergLP::bslstl::Variant_IsCopyConstructibleAll<HEAD,
                                                          TAIL...>::value &&
          BloombergLP::bslstl::Variant_IsCopyAssignableAll<HEAD,
                                                           TAIL...>::value,
      BloombergLP::bslstl::Variant_IsMoveConstructibleAll<HEAD,
                                                          TAIL...>::value &&
          BloombergLP::bslstl::Variant_IsMoveAssignableAll<HEAD,
                                                           TAIL...>::value>
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
{
  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                MoveUtil;
    typedef BloombergLP::bslstl::Variant_Base<HEAD, TAIL...>  Variant_Base;
    typedef BloombergLP::bslstl::Variant_Union<HEAD, TAIL...> Variant_Union;

    // FRIENDS
    friend struct BloombergLP::bslstl::Variant_ImpUtil<variant>;
        // This struct contains functions that require access to the
        // 'Variant_Union' subobject.  Note that because 'variant' is not
        // standard-layout in the allocator-aware case, we cannot easily cast
        // between the variant object and its 'Variant_Union' subobject.

    friend struct BloombergLP::bslstl::Variant_Base<HEAD, TAIL...>;
        // 'Variant_Base' must be able to downcast from 'Variant_Base' to
        // 'variant' (which is privately derived from 'Variant_Base').

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        variant,
        BloombergLP::bslma::UsesBslmaAllocator,
        (BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                            TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        variant,
        BloombergLP::bslmf::UsesAllocatorArgT,
        (BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD,
                                                            TAIL...>::value));
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        variant,
        BloombergLP::bslmf::IsBitwiseMoveable,
        (BloombergLP::bslstl::Variant_IsBitwiseMoveableAll<HEAD,
                                                           TAIL...>::value));

    // TYPES
    typedef typename Variant_Base::allocator_type allocator_type;
        // Type alias to the allocator type used by 'variant'.

    // CREATORS

    // 20.7.3.1, constructors
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    template <class t_FIRST_ALT = HEAD,
              class             = typename bsl::enable_if_t<
                  std::is_default_constructible<t_FIRST_ALT>::value> >
    variant()
        // Create a 'variant' object holding the 0th alternative, which is
        // value-initialized.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.
        // This constructor participates in overload resolution only if the 0th
        // alternative is default constructible.  For simplicity of
        // implementation, this method differs from the standard in the
        // following ways:
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented
    : Variant_Base()
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    variant(const variant& original) = default;
        // Create a 'variant' object holding the same alternative (if any) as
        // the specified 'original' object.  If this 'variant' is
        // allocator-aware, the currently installed default allocator is used
        // to supply memory.  If the 'original' object is not valueless by
        // exception', the contained value is copy-constructed from the
        // contained value of 'original'.  This constructor is deleted unless
        // all alternatives are copy constructible.  For simplicity of
        // implementation, this method differs from the standard in the
        // following ways:
        //: o conditional triviality is not implemented
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented

    variant(variant&& original) = default;
        // Create a 'variant' object holding the same alternative (if any) held
        // by the specified 'original'.  If this 'variant' is allocator-aware,
        // the allocator of 'original' is used to supply memory.  If 'original'
        // is not valueless by exception, the contained value is
        // move-constructed from the contained value of 'original'.  This
        // constructor participates in overload resolution only if all
        // alternatives are move constructible.  For simplicity of
        // implementation, this method differs from the standard in the
        // following ways:
        //: o conditional triviality is not implemented
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented

    template <class TYPE>
    variant(TYPE&& t,
            BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, TYPE));
        // Create a 'variant' object whose contained value is
        // direct-initialized from the specified 't'.  The alternative selected
        // is the best match among all alternatives for which the expression
        // 'ALT_TYPE x[] = {std::forward<TYPE>(t)};' is well formed, and this
        // constructor participates in overload resolution only if there is a
        // unique best matching alternative and that alternative is
        // constructible from 't'.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.
        // Note that the cv-qualification of an alternative type does not
        // affect how well the alternative type matches an argument type.  For
        // simplicity of implementation, this method differs from the standard
        // in the following ways:
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented

    template <class    TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE) &&
                  std::is_constructible<TYPE, ARGS...>::value> >
    explicit variant(bsl::in_place_type_t<TYPE>, ARGS&&... args)
    : Variant_Base(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>(),
               std::forward<ARGS>(args)...)
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', direct-initialized from the specified
        // 'args'.  If this 'variant' is allocator-aware, the currently
        // installed default allocator is used to supply memory.  This
        // constructor participates in overload resolutionly if 'TYPE'
        // designates a unique alternative and is constructible from 'args'.
        // For simplicity of implementation, this method differs from the
        // standard in the following way:
        //: o 'constexpr' is not implemented
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    template <class    TYPE,
              class    INIT_LIST_TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE) &&
                  std::is_constructible<TYPE,
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        ARGS...>::value> >
    explicit variant(bsl::in_place_type_t<TYPE>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     ARGS&&...                             args)
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', direct-initialized from the specified
        // 'il' and 'args'.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.
        // This constructor participates in overload resolutionly if 'TYPE'
        // designates a unique alternative and is constructible from 'il' and
        // 'args'.  For simplicity of implementation, this method differs from
        // the standard in the following way:
        //: o 'constexpr' is not implemented
    : Variant_Base(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>(),
               il,
               std::forward<ARGS>(args)...)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    template <size_t   INDEX,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  (INDEX < 1 + sizeof...(TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX),
                                        ARGS...>::value> >
    explicit variant(bsl::in_place_index_t<INDEX>, ARGS&&... args)
    : Variant_Base(bsl::in_place_index_t<INDEX>(), std::forward<ARGS>(args)...)
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from the specified
        // 'args'.  If this 'variant' is allocator-aware, the currently
        // installed default allocator is used to supply memory.  This
        // constructor participates in overload resolution only if 'INDEX' is a
        // valid alternative index and the designated alternative is
        // constructible from 'args'.  For simplicity of implementation, this
        // method differs from the standard in the following ways:
        //: o 'constexpr' is not implemented
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    template <size_t   INDEX,
              class    INIT_LIST_TYPE,
              class... ARGS,
              class  = typename bsl::enable_if_t<
                  (INDEX < 1 + sizeof...(TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX),
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        ARGS...>::value> >
    explicit variant(bsl::in_place_index_t<INDEX>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     ARGS&&...                             args)
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from the specified
        // 'il' and 'args'.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.
        // This constructor participates in overload resolution only if 'INDEX'
        // is a valid alternative index and the designated alternative is
        // constructible from 'il' and 'args'.  For simplicity of
        // implementation, this method differs from the standard in the
        // following way:
        //: o 'constexpr' is not implemented
    : Variant_Base(bsl::in_place_index_t<INDEX>(),
                   il,
                   std::forward<ARGS>(args)...)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }

    // allocator-extended constructors
    template <class FIRST = HEAD,
              class       = typename bsl::enable_if_t<
                  std::is_default_constructible<FIRST>::value> >
    variant(bsl::allocator_arg_t, allocator_type allocator)
        // Create a 'variant' object holding the 0th alternative, which is
        // value-initialized.  If this 'variant' is allocator-aware, the
        // specified 'allocator' is used to supply memory.  This constructor
        // participates in overload resolution only if the 0th alternative is
        // default constructible.
    : Variant_Base(bsl::allocator_arg_t(), allocator)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    template <class FIRST = HEAD,
              class       = typename bsl::enable_if_t<
                  BloombergLP::bslstl::
                      Variant_IsCopyConstructibleAll<FIRST, TAIL...>::value> >
    variant(bsl::allocator_arg_t,
            allocator_type allocator,
            const variant& original)
        // Create a 'variant' object holding the same alternative (if any) as
        // the specified 'original' object.  If this 'variant' is
        // allocator-aware, the specified 'allocator' is used to supply memory.
        // If the 'original' object is not valueless by exception', the
        // contained value is copy-constructed from the contained value of
        // 'original'.  This constructor is deleted unless all alternatives are
        // copy constructible.
    : Variant_Base(bsl::allocator_arg_t(), allocator, original)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }
    template <class FIRST = HEAD,
              class       = typename bsl::enable_if_t<
                  BloombergLP::bslstl::
                      Variant_IsMoveConstructibleAll<FIRST, TAIL...>::value> >
    variant(bsl::allocator_arg_t, allocator_type allocator, variant&& original)
    : Variant_Base(bsl::allocator_arg_t(), allocator, std::move(original))
        // Create a 'variant' object holding the same alternative (if any) held
        // by the specified 'original'.  If this 'variant' is allocator-aware,
        // the specified 'allocator' is used to supply memory.  If 'original'
        // is not valueless by exception, the contained value is
        // move-constructed from the contained value of 'original'.  This
        // constructor participates in overload resolution only if all
        // alternatives are move constructible.
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    template <class TYPE>
    variant(bsl::allocator_arg_t,
            allocator_type allocator,
            TYPE&&         t,
            BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, TYPE));
        // Create a 'variant' object whose contained value is
        // direct-initialized from the specified 't'.  The alternative selected
        // is the best match among all alternatives for which the expression
        // 'ALT_TYPE x[] = {std::forward<TYPE>(t)};' is well formed, and this
        // constructor participates in overload resolution only if there is a
        // unique best matching alternative and that alternative is
        // constructible from 't'.  If this 'variant' is allocator-aware, the
        // specified 'allocator' is used to supply memory.  Note that the
        // cv-qualification of an alternative type does not affect how well the
        // alternative type matches an argument type.

    template <class    TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE) &&
                  std::is_constructible<TYPE, ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<TYPE>,
                     ARGS&&...      args)
    : Variant_Base(
          bsl::allocator_arg_t{},
          allocator,
          bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>(),
          std::forward<ARGS>(args)...)
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', direct-initialized from the specified
        // 'args'.  If this 'variant' is allocator-aware, the specified
        // 'allocator 'is used to supply memory.  This constructor participates
        // in overload resolutionly if 'TYPE' designates a unique alternative
        // and is constructible from 'args'.
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    template <class    TYPE,
              class    INIT_LIST_TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE) &&
                  std::is_constructible<TYPE,
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type                        allocator,
                     bsl::in_place_type_t<TYPE>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     ARGS&&...                             args)
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', direct-initialized from the specified
        // 'il' and 'args'.  If this 'variant' is allocator-aware, the
        // specified 'allocator 'is used to supply memory.  This constructor
        // participates in overload resolutionly if 'TYPE' designates a unique
        // alternative and is constructible from 'il' and 'args'.
    : Variant_Base(
          bsl::allocator_arg_t{},
          allocator,
          bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>(),
          il,
          std::forward<ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    template <size_t   INDEX,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  (INDEX < 1 + sizeof...(TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX),
                                        ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<INDEX>,
                     ARGS&&...      args)
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from the specified
        // 'args'.  If this 'variant' is allocator-aware, the specified
        // 'allocator' is used to supply memory.  This constructor participates
        // in overload resolution only if 'INDEX' is a valid alternative index
        // and the designated alternative is constructible from 'args'.
    : Variant_Base(bsl::allocator_arg_t{},
                   allocator,
                   bsl::in_place_index_t<INDEX>(),
                   std::forward<ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

    template <size_t   INDEX,
              class    INIT_LIST_TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  (INDEX < 1 + sizeof...(TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX),
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        ARGS...>::value> >
    explicit variant(bsl::allocator_arg_t,
                     allocator_type                        allocator,
                     bsl::in_place_index_t<INDEX>,
                     std::initializer_list<INIT_LIST_TYPE> il,
                     ARGS&&...                             args)
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', direct-initialized from the specified
        // 'il' and 'args'.  If this 'variant' is allocator-aware, the
        // specified 'allocator' is used to supply memory.  This constructor
        // participates in overload resolution only if 'INDEX' is a valid
        // alternative index and the designated alternative is constructible
        // from 'il' and 'args'.
    : Variant_Base(bsl::allocator_arg_t{},
                   allocator,
                   bsl::in_place_index_t<INDEX>(),
                   il,
                   std::forward<ARGS>(args)...)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }

#else  // BSL_VARIANT_FULL_IMPLEMENTATION

    variant();
        // Create a 'variant' object holding the 0th alternative, which is
        // value-initialized.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.

    variant(const variant& original);
        // Create a 'variant' object holding the same alternative (if any) as
        // the specified 'original' object.  If this 'variant' is
        // allocator-aware, the currently installed default allocator is used
        // to supply memory.  If the 'original' object is not valueless by
        // exception', the contained value is copy-constructed from the
        // contained value of 'original'.  All alternatives shall be copy
        // constructible.

    variant(BloombergLP::bslmf::MovableRef<variant> original);
        // Create a 'variant' object holding the same alternative (if any) held
        // by the specified 'original'.  If this 'variant' is allocator-aware,
        // the allocator of 'original' is used to supply memory.  If 'original'
        // is not valueless by exception, the contained value is
        // move-constructed from the contained value of 'original'.  All
        // alternatives shall be move constructible.

    template <class TYPE>
    variant(const TYPE& value,
            BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, TYPE));
        // Create a 'variant' object whose contained value is copy or move
        // constructed from the specified 'value'.  If (template parameter)
        // 'TYPE' is 'bslmf::MovableRef<T>', then the selected alternative is
        // the one whose type is the same as 'T' (modulo cv-qualification);
        // otherwise, the selected alternative is the one that is the same as
        // 'TYPE' (modulo cv-qualification).  This constructor participates in
        // overload resolution only if there is a unique such alternative.  If
        // this 'variant' is allocator-aware, the current installed default
        // allocator is used to supply memory.

    template <class TYPE>
    explicit variant(bsl::in_place_type_t<TYPE>,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(TYPE));
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', which is value-initialized.  If this
        // 'variant' is allocator-aware, the currently installed default
        // allocator is used to supply memory.  This constructor participates
        // in overload resolutionly if 'TYPE' designates a unique alternative.

    template <class TYPE, class ARG>
    explicit variant(bsl::in_place_type_t<TYPE>,
                     const ARG& arg,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(TYPE));
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', which is direct-initialized from the
        // specified 'arg'.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.
        // This constructor participates in overload resolution only if 'TYPE'
        // designates a unique alternative.

    template <size_t INDEX>
    explicit variant(bsl::in_place_index_t<INDEX>);
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', which is value-initialized.  If this
        // 'variant' is allocator-aware, the currently installed default
        // allocator is used to supply memory.  'INDEX' shall be a valid
        // alternative index.

    template <size_t INDEX, class ARG>
    explicit variant(bsl::in_place_index_t<INDEX>, const ARG& arg);
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', which is direct-initialized from the
        // specified 'arg'.  If this 'variant' is allocator-aware, the
        // currently installed default allocator is used to supply memory.
        // 'INDEX' shall be a valid alternative index.

    // allocator-extended constructors
    variant(bsl::allocator_arg_t, allocator_type allocator);
        // Create a 'variant' object holding the 0th alternative, which is
        // value-initialized.  If this 'variant' is allocator-aware, the
        // specified 'allocator' is used to supply memory.

    variant(bsl::allocator_arg_t,
            allocator_type                          allocator,
            const variant&                          original);
    variant(bsl::allocator_arg_t,
            allocator_type                          allocator,
            BloombergLP::bslmf::MovableRef<variant> original);
        // Create a 'variant' object holding the same alternative as the
        // specified 'original' object.  If this 'variant' is allocator-aware,
        // the specified 'allocator' is used to supply memory.  If the
        // 'original' object is not valueless by exception, the contained value
        // is copy/move constructed from the contained value of 'original'.
        // All alternatives shall be copy/move constructible.

    template <class TYPE>
    variant(bsl::allocator_arg_t,
            allocator_type allocator,
            const TYPE&    value,
            BSLSTL_VARIANT_DECLARE_IF_CONSTRUCTS_FROM(variant, TYPE));
        // Create a 'variant' object whose contained value is copy or move
        // constructed from the specified 'value'.  If (template parameter)
        // 'TYPE' is 'bslmf::MovableRef<T>', then the selected alternative is
        // the one whose type is the same as 'T' (modulo cv-qualification);
        // otherwise, the selected alternative is the one that is the same as
        // 'TYPE' (modulo cv-qualification).  This constructor participates in
        // overload resolution only if there is a unique such alternative.  If
        // this 'variant' is allocator-aware, the specified 'allocator' is used
        // to supply memory.

    template <class TYPE>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<TYPE>,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(TYPE));
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', which is value-initialized.  If this
        // 'variant' is allocator-aware, the currently installed default
        // allocator is used to supply memory.  If this 'variant' is
        // allocator-aware, the specified 'allocator' is used to supply memory.
        // This constructor participates in overload resolution only if 'TYPE'
        // designates a unique alternative.

    template <class TYPE, class ARG>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_type_t<TYPE>,
                     const ARG&     arg,
                     BSLSTL_VARIANT_DECLARE_IF_HAS_UNIQUE_TYPE(TYPE));
        // Create a 'variant' object holding a contained value of type
        // (template parameter) 'TYPE', which is direct-initialized from the
        // specified 'arg'.  If this 'variant' is allocator-aware, the
        // specified 'allocator' is used to supply memory.  This constructor
        // participates in overload resolution only if 'TYPE' designates a
        // unique alternative.

    template <size_t INDEX>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<INDEX>);
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', which is value-initialized.  If this
        // 'variant' is allocator-aware, the specified 'allocator' is used to
        // supply memory.  'INDEX' shall be a valid alternative index.

    template <size_t INDEX, class ARG>
    explicit variant(bsl::allocator_arg_t,
                     allocator_type allocator,
                     bsl::in_place_index_t<INDEX>,
                     const ARG&     arg);
        // Create a 'variant' object holding the alternative with index
        // (template parameter) 'INDEX', which is direct-initialized from the
        // specified 'arg'.  If this 'variant' is allocator-aware, the
        // specified 'allocator' is used to supply memory.  'INDEX' shall be a
        // valid alternative index.

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    // 20.7.3.2, destructor
    //! ~variant() = default;
        // Destroy this object.  The contained value (if any) is destroyed.
        // For simplicity of implementation, this method differs from the
        // standard in the following ways:
        //: o conditional triviality is not implemented
        //: o 'constexpr' is not implemented

    // MANIPULATORS
    // 20.7.3.4, modifiers
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    template <class    TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE) &&
                  std::is_constructible<TYPE, ARGS...>::value> >
    TYPE& emplace(ARGS&&... args)
        // Create an object of alternative (template parameter) 'TYPE' in
        // place, direct-initialized from the specified 'args', destroying any
        // previously contained value first.  Return a reference to the newly
        // created 'TYPE' object.  If 'TYPE' is allocator-aware, it uses the
        // allocator specified upon the construction of this 'variant' object
        // to supply memory; passing an allocator argument to this method
        // results in two allocators being passed to the alternative
        // constructor, resulting in a likely compilation error.  Note that if
        // the constructor of 'TYPE' exits via an exception, this object is
        // left in the valueless by exception state.  This method participates
        // in overload resolution only if 'TYPE' designates a unique
        // alternative and is constructible from 'args'.  For simplicity of
        // implementation, this method differs from the standard in the
        // following way:
        //: o 'constexpr' is not implemented
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        const size_t index = BSLSTL_VARIANT_INDEX_OF(TYPE, variant);
        Variant_Base::template baseEmplace<index>(std::forward<ARGS>(args)...);

        return bsl::get<index>(*this);
    }

    template <class    TYPE,
              class    INIT_LIST_TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  BSLSTL_VARIANT_HAS_UNIQUE_TYPE(TYPE) &&
                  std::is_constructible<TYPE,
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        ARGS...>::value> >
    TYPE& emplace(std::initializer_list<INIT_LIST_TYPE> il, ARGS&&... args)
        // Create an object of alternative (template parameter) 'TYPE' in
        // place, direct-initialized from the specified 'il' and 'args',
        // destroying any previously contained value first.  Return a reference
        // to the newly created 'TYPE' object.  If 'TYPE' is allocator-aware,
        // it uses the allocator specified upon the construction of this
        // 'variant' object to supply memory; passing an allocator argument to
        // this method results in two allocators being passed to the
        // alternative constructor, resulting in a likely compilation error.
        // Note that if the constructor of 'TYPE' exits via an exception, this
        // object is left in the valueless by exception state.  This method
        // participates in overload resolution only if 'TYPE' designates a
        // unique alternative and is constructible from 'il' and 'args'.  For
        // simplicity of implementation, this method differs from the standard
        // in the following way:
        //: o 'constexpr' is not implemented
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        const size_t index = BSLSTL_VARIANT_INDEX_OF(TYPE, variant);
        Variant_Base::template baseEmplace<index>(il,
                                                  std::forward<ARGS>(args)...);

        return bsl::get<index>(*this);
    }

    template <size_t   INDEX,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  (INDEX < 1 + sizeof...(TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX),
                                        ARGS...>::value> >
    typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&
    emplace(ARGS&&... args)
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, direct-initialized from the specified 'args', destroying any
        // previously contained value first.  Return a reference to the newly
        // created contained value.  If the alternative is allocator-aware, it
        // uses the allocator specified upon the construction of this 'variant'
        // object to supply memory; passing an allocator argument to this
        // method results in two allocators being passed to the alternative
        // constructor, resulting in a likely compilation error.  Note that if
        // the alternative constructor exits via an exception, this object is
        // left in the valueless by exception state.  This method participates
        // in overload resolution only if 'INDEX' is a valid alternative index
        // and the designated alternative is constructible from 'args'.  For
        // simplicity of implementation, this method differs from the standard
        // in the following way:
        //: o 'constexpr' is not implemented
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        Variant_Base::template baseEmplace<INDEX>(std::forward<ARGS>(args)...);

        return bsl::get<INDEX>(*this);
    }

    template <size_t   INDEX,
              class    INIT_LIST_TYPE,
              class... ARGS,
              class =  typename bsl::enable_if_t<
                  (INDEX < 1 + sizeof...(TAIL)) &&
                  std::is_constructible<BSLSTL_VARIANT_TYPE_AT_INDEX(INDEX),
                                        std::initializer_list<INIT_LIST_TYPE>&,
                                        ARGS...>::value> >
    typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&
    emplace(std::initializer_list<INIT_LIST_TYPE> il, ARGS&&... args)
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, direct-initialized from the specified 'il' and args',
        // destroying any previously contained value first.  Return a reference
        // to the newly created contained value.  If the alternative is
        // allocator-aware, it uses the allocator specified upon the
        // construction of this 'variant' object to supply memory; passing an
        // allocator argument to this method results in two allocators being
        // passed to the alternative constructor, resulting in a likely
        // compilation error.  Note that if the alternative constructor exits
        // via an exception, this object is left in the valueless by exception
        // state.  This method participates in overload resolution only if
        // 'INDEX' is a valid alternative index and the designated alternative
        // is constructible from 'il' and 'args'.  For simplicity of
        // implementation, this method differs from the standard in the
        // following way:
        //: o 'constexpr' is not implemented
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        Variant_Base::template baseEmplace<INDEX>(il,
                                                  std::forward<ARGS>(args)...);

        return bsl::get<INDEX>(*this);
    }

#else   //  BSL_VARIANT_FULL_IMPLEMENTATION

    template <class TYPE>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<TYPE, variant>::value,
        TYPE&>::type
    emplace();
        // Create an object of alternative (template parameter) 'TYPE' in
        // place, which is value-initialized; any previously contained value is
        // destroyed first.  Return a reference to the newly created 'TYPE'
        // object.  If 'TYPE' is allocator-aware, it uses the allocator
        // specified upon the construction of this 'variant' object to supply
        // memory; passing an allocator argument to this method results in two
        // allocators being passed to the alternative constructor, resulting in
        // a likely compilation error.  Note that if the constructor of 'TYPE'
        // exits via an exception, this object is left in a valueless by
        // exception state.  This function participates in overload resolution
        // only if 'TYPE' is a unique alternative.

    template <class TYPE, class ARG>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_HasUniqueType<TYPE, variant>::value,
        TYPE&>::type
    emplace(const ARG& arg);
        // Create an object of alternative (template parameter) 'TYPE' in
        // place, which is direct-initialized from 'arg'; any previously
        // contained value is destroyed first.  Return a reference to the newly
        // created 'TYPE' object.  If 'TYPE' is allocator-aware, it uses the
        // allocator specified upon the construction of this 'variant' object
        // to supply memory; passing an allocator argument to this method
        // results in two allocators being passed to the alternative
        // constructor, resulting in a likely compilation error.  Note that if
        // the constructor of 'TYPE' exits via an exception, this object is
        // left in a valueless by exception state.  This function participates
        // in overload resolution only if 'TYPE' is a unique alternative.

    template <size_t INDEX>
    typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&
    emplace();
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, which is value-initialized; any previously contained value is
        // destroyed first.  Return a reference to the newly
        // created contained value.  If the alternative is allocator-aware, it
        // uses the allocator specified upon the construction of this 'variant'
        // object to supply memory; passing an allocator argument to this
        // method results in two allocators being passed to the alternative
        // constructor, resulting in a likely compilation error.  Note that if
        // the alternative constructor exits via an exception, this object is
        // left in the valueless by exception state.  'INDEX' shall be a valid
        // alternative index.

    template <size_t INDEX, class ARG>
    typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&
    emplace(const ARG& arg);
        // Create the alternative with index (template parameter) 'INDEX' in
        // place, direct-initialized from the specified 'arg'; any previously
        // contained value is destroyed first.  Return a reference to the newly
        // created contained value.  If the alternative is allocator-aware, it
        // uses the allocator specified upon the construction of this 'variant'
        // object to supply memory; passing an allocator argument to this
        // method results in two allocators being passed to the alternative
        // constructor, resulting in a likely compilation error.  Note that if
        // the alternative constructor exits via an exception, this object is
        // left in the valueless by exception state.  'INDEX' shall be a valid
        // alternative index.
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

    // MANIPULATORS
    // 20.7.3.3, assignment
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    variant& operator=(const variant& rhs) = default;
        // If 'rhs' holds the same alternative type as this object, copy assign
        // the contained value of 'rhs' to the contained value of this object.
        // Otherwise, destroy the contained value of this object (if any) and,
        // if 'rhs' holds a value, copy-construct the corresponding alternative
        // of this object from the contained value of 'rhs'.  The allocators of
        // this object and 'rhs' both remain unchanged.  This operator is
        // deleted unless all alternatives are copy constructible and copy
        // assignable.  If the construction of a new alternative object exits
        // via an exception, this 'variant' object is left in a valueless by
        // exception state.  This is different from the standard, which
        // requires a temporary copy of 'rhs' to be made if copy construction
        // of the active alternative is not 'noexcept' (see [variant.assign]
        // for details).  The standard behavior causes unnecessary performance
        // degradation in cases where the alternative constructor does not
        // throw, yet is not marked 'noexcept'; this behavior is therefore not
        // implemented in 'bsl::variant'.  For simplicity of implementation,
        // this method also differs from the standard in the following ways:
        //: o conditional triviality is not implemented
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented

    variant& operator=(variant&& rhs) = default;
        // If 'rhs' holds the same alternative type as this object, move assign
        // the contained value of 'rhs' to the contained value of this object.
        // Otherwise, destroy the contained value of this object (if any) and,
        // if 'rhs' holds a value, move-construct the corresponding alternative
        // of this object from the contained value of 'rhs'.  The allocators of
        // this object and 'rhs' both remain unchanged.  If the construction of
        // a new alternative object throws an exception, this 'variant' object
        // is left in a valueless by exception state.  This operator
        // participes in overload resolution only if all alternatives are move
        // constructible and mvoe assignable.  For simplicity of
        // implementation, this method differs from the standard in the
        // following ways:
        //: o conditional triviality is not implemented
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented

    template <class TYPE>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_AssignsFromType<variant, TYPE>::value,
        variant&>::type
    operator=(TYPE&& value)
        // Assign to this object the specified 'value'.  The alternative
        // corresponding to 'value' is the best match among all alternatives
        // for which the expression
        // 'ALT_TYPE x[] = {std::forward<TYPE>(value)};' is well formed, and
        // this operator participates in overload resolution only if there is a
        // unique best matching alternative and that alternative is both
        // constructible and assignable from 'value'.  If this 'variant'
        // already holds the alternative corresponding to 'value', the
        // contained value is assigned to from 'value'; otherwise, any
        // contained value is destroyed and the alternative corresponding to
        // 'value' is direct-initialized from 'value'.  Note that if the
        // construction of a new alternative object exits via an exception,
        // this 'variant' object is left in a valueless by exception state.
        // This is different from the standard, which requires a temporary
        // alternative object to be constructed if such construction is not
        // 'noexcept' (see [variant.assign] for details).  The standard
        // behavior causes unnecessary performance degradation in cases where
        // the alternative constructor does not throw, yet is not marked
        // 'noexcept'; this behavior is therefore not implemented in
        // 'bsl::variant'.  For simplicity of implementation, this method also
        // differs from the standard in the following ways:
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        const size_t altIndex = BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, variant);

        if (index() == altIndex) {
            bsl::get<altIndex>(*this) = std::forward<TYPE>(value);
        }
        else {
            // 'altIndex' can not be 'variant_npos' if
            // 'Variant_AssignsFromType' is satisfied
            Variant_Base::template baseEmplace<altIndex>(
                std::forward<TYPE>(value));
        }

        return *this;
    }
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
    variant& operator=(const variant& rhs);
        // If 'rhs' holds the same alternative type as this object, copy assign
        // the contained value of 'rhs' to the contained value of this object.
        // Otherwise, destroy the contained value of this object (if any) and,
        // if 'rhs' holds a value, copy-construct the corresponding alternative
        // of this object from the contained value of 'rhs'.  The allocators of
        // this object and 'rhs' both remain unchanged.  If the construction of
        // a new alternative object exits via an exception, this 'variant'
        // object is left in a valueless by exception state.  All alternatives
        // shall be copy constructible and copy assignable.

    variant& operator=(BloombergLP::bslmf::MovableRef<variant> rhs);
        // If 'rhs' holds the same alternative type as this object, move assign
        // the contained value of 'rhs' to the contained value of this object.
        // Otherwise, destroy the contained value of this object (if any) and,
        // if 'rhs' holds a value, move-construct the corresponding alternative
        // of this object from the contained value of 'rhs'.  The allocators of
        // this object and 'rhs' both remain unchanged.  If the construction of
        // a new alternative object throws an exception, this 'variant' object
        // is left in a valueless by exception state.  All alternatives shall
        // be move constructible and move assignable.

    template <class TYPE>
    typename bsl::enable_if<
        BloombergLP::bslstl::Variant_AssignsFromType<variant, TYPE>::value,
        variant&>::type
    operator=(const TYPE& value);
        // Assign to this object the specified 'value'.  If (template
        // parameter)
        // 'TYPE' is 'bslmf::MovableRef<T>', then the alternative corresponding
        // to 'value' is
        // the one whose type is the same as 'T' (modulo cv-qualification);
        // otherwise, the selected alternative is the one that is the same as
        // 'TYPE' (modulo cv-qualification).  This operator participates in
        // overload resolution only if there is a unique such alternative.
        // If this 'variant' object already holds that alternative, then the
        // contained value is assigned from 'value'; otherwise, any contained
        // value is destroyed and the selected alternative is
        // direct-initialized from 'value'.  The selected alternative shall be
        // constructible and assignable from 'value'.  If the construction of
        // a new alternative object exits via an exception, this 'variant'
        // object is left in the valueless by exception state.

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    // 20.7.3.6, swap
    void swap(variant& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the two swapped objects contain the
        // same alternative and if that alternative 'TYPE' provides that
        // guarantee.  If '*this' and 'other' do not have the same active
        // alternative and this method exits via an exception, either or both
        // 'variant' objects may be left in a valueless state or with an
        // contained value in a moved-from state.  The behavior is undefined
        // unless '*this' has the same allocator as 'other'.  All alternatives
        // shall be move constructible and swappable.  For simplicity of
        // implementation, this method differs from the standard in the
        // following ways:
        //: o 'constexpr' is not implemented
        //: o 'noexcept' specification is not implemented

    // ACCESSORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    template <bool USES_BSLMA_ALLOC = BloombergLP::bslstl::
                  Variant_UsesBslmaAllocatorAny<HEAD, TAIL...>::value,
              class = typename bsl::enable_if_t<USES_BSLMA_ALLOC> >
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT
        // Return a copy of this object's allocator.  At least one alternative
        // of this 'variant' object shall be allocator-aware; in C++03, due to
        // language limitations, this method can be called even if no
        // alternative is allocator-aware, but returns a non-allocator type in
        // that case.
    {
        // This function template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
        return allocator_type(Variant_Base::AllocBase::mechanism());
    }

    // 20.7.3.5, value status
    size_t index() const BSLS_KEYWORD_NOEXCEPT;
        // Return the index of the alternative currently managed by this
        // 'variant' object, or 'bsl::variant_npos' if this object is valueless
        // by exception.  This method differs from the standard in the
        // following way:
        //: o 'constexpr' is not implemented
        // This is because no constructors are currently constexpr and there is
        // no way to test the constexpr property of this function.

    bool valueless_by_exception() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'false' if there is an alternative object currently managed
        // by this 'variant' object, and 'true' otherwise.  A 'variant' object
        // can become valueless by exception if the creation of an alternative
        // object exits via an exception, or if it is copied or assigned from
        // another 'variant' object that is valueless by exception.  This
        // method differs from the standard in the following way:
        //: o 'constexpr' is not implemented
        // This is because no constructors are currently constexpr and there is
        // no way to test the constexpr property of this function.
};

#endif

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslstl {

                          // ------------------------
                          // class Variant_NoSuchType
                          // ------------------------

// CREATORS
inline
BSLS_KEYWORD_CONSTEXPR Variant_NoSuchType::Variant_NoSuchType(int)
                                                          BSLS_KEYWORD_NOEXCEPT
{
}

                           // ---------------------
                           // class Variant_DataImp
                           // ---------------------

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
// CREATORS
template <class TYPE>
template <class... ARGS>
inline
Variant_DataImp<TYPE>::Variant_DataImp(ARGS&&... args)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(), std::forward<ARGS>(args)...);
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

// MANIPULATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 TYPE& Variant_DataImp<TYPE>::value() &
{
    return d_buffer.object();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 TYPE&& Variant_DataImp<TYPE>::value() &&
{
    return std::move(d_buffer.object());
}
#else   // BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
inline
TYPE& Variant_DataImp<TYPE>::value()
{
    return d_buffer.object();
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

// ACCESSORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 const TYPE& Variant_DataImp<TYPE>::value() const&
{
    return d_buffer.object();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 const TYPE&&
Variant_DataImp<TYPE>::value() const&&
{
    return std::move(d_buffer.object());
}
#else   // BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE>
inline
const TYPE& Variant_DataImp<TYPE>::value() const
{
    return d_buffer.object();
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

template <class VARIANT>
void variant_swapImpl(bsl::true_type, VARIANT& lhs, VARIANT& rhs)
    // This component-private function swaps the values of the specified 'lhs'
    // and 'rhs' when the type (template parameter) 'VARIANT' is an
    // allocator-aware variant.
{
    if (lhs.get_allocator() == rhs.get_allocator()) {
        lhs.swap(rhs);
        return;                                                       // RETURN
    }

    VARIANT futureLhs(bsl::allocator_arg, lhs.get_allocator(), rhs);
    VARIANT futureRhs(bsl::allocator_arg, rhs.get_allocator(), lhs);

    futureLhs.swap(lhs);
    futureRhs.swap(rhs);
}

template <class VARIANT>
void variant_swapImpl(bsl::false_type, VARIANT& lhs, VARIANT& rhs)
    // This component-private function swaps the values of the specified 'lhs'
    // and 'rhs' when the type (template parameter) 'VARIANT' is a
    // non-allocator-aware variant.
{
    lhs.swap(rhs);
}

                            // -------------------
                            // struct Variant_Base
                            // -------------------

// CREATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base()
: AllocBase()
, d_type(0)
, d_union(bsl::in_place_index_t<0>(), AllocBase::mechanism())
{
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(const Variant_Base& original)
: AllocBase()
, d_type(bsl::variant_npos)
{
    if (original.d_type != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
            copyConstructor(this);
        BSLSTL_VARIANT_VISITID(void,
                               copyConstructor,
                               static_cast<const Variant&>(original));
    }
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(Variant_Base&& original)
: AllocBase(original)
, d_type(bsl::variant_npos)
{
    if (original.d_type != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(this);
        BSLSTL_VARIANT_VISITID(void,
                               moveConstructor,
                               static_cast<Variant&>(original));
    }
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                          allocator_type allocator)
: AllocBase(allocator)
, d_type(0)
, d_union(bsl::in_place_index_t<0>(), AllocBase::mechanism())
{
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                          allocator_type allocator,
                                          const Variant& original)
: AllocBase(allocator)
, d_type(bsl::variant_npos)
{
    if (original.index() != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
            copyConstructor(this);
        BSLSTL_VARIANT_VISITID(void, copyConstructor, original);
    }
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                          allocator_type allocator,
                                          Variant&&      original)
: AllocBase(allocator)
, d_type(bsl::variant_npos)
{
    if (original.index() != bsl::variant_npos) {
        BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
            moveConstructor(this);
        BSLSTL_VARIANT_VISITID(void, moveConstructor, original);
    }
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class... ARGS>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::in_place_index_t<INDEX>,
                                          ARGS&&... args)
: d_type(INDEX)
, d_union(bsl::in_place_index_t<INDEX>(),
          AllocBase::mechanism(),
          std::forward<ARGS>(args)...)
{
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class... ARGS>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                          allocator_type allocator,
                                          bsl::in_place_index_t<INDEX>,
                                          ARGS&&...      args)
: AllocBase(allocator)
, d_type(INDEX)
, d_union(bsl::in_place_index_t<INDEX>(),
          AllocBase::mechanism(),
          std::forward<ARGS>(args)...)
{
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base()
: AllocBase()
, d_type(0)
{
    typedef typename bsl::remove_cv<HEAD>::type ALT_TYPE;

    BloombergLP::bslma::ConstructionUtil::construct(
                    reinterpret_cast<ALT_TYPE *>(BSLS_UTIL_ADDRESSOF(d_union)),
                    AllocBase::mechanism());
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(const Variant& original)
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

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(
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

template <class HEAD, class... TAIL>
template <size_t INDEX>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::in_place_index_t<INDEX>)
: AllocBase()
, d_type(INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<INDEX, Variant>::type>::type TYPE;

    BloombergLP::bslma::ConstructionUtil::construct(
                        reinterpret_cast<TYPE *>(BSLS_UTIL_ADDRESSOF(d_union)),
                        AllocBase::mechanism());
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class ARG>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::in_place_index_t<INDEX>,
                                          const ARG& arg)
: AllocBase()
, d_type(INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<INDEX, Variant>::type>::type TYPE;

    BloombergLP::bslma::ConstructionUtil::construct(
                        reinterpret_cast<TYPE *>(BSLS_UTIL_ADDRESSOF(d_union)),
                        AllocBase::mechanism(),
                        arg);
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                          allocator_type allocator)
: AllocBase(allocator)
, d_type(0)
{
    BloombergLP::bslma::ConstructionUtil::construct(
                        reinterpret_cast<HEAD *>(BSLS_UTIL_ADDRESSOF(d_union)),
                        AllocBase::mechanism());
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(bsl::allocator_arg_t,
                                          allocator_type allocator,
                                          const Variant& original)
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

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(
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

template <class HEAD, class... TAIL>
template <size_t INDEX>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(
                                        bsl::allocator_arg_t,
                                        allocator_type allocator,
                                        bsl::in_place_index_t<INDEX>)
: AllocBase(allocator)
, d_type(INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<INDEX, Variant>::type>::type TYPE;

    BloombergLP::bslma::ConstructionUtil::construct(
                        reinterpret_cast<TYPE *>(BSLS_UTIL_ADDRESSOF(d_union)),
                        AllocBase::mechanism());
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class ARG>
inline
Variant_Base<HEAD, TAIL...>::Variant_Base(
                                        bsl::allocator_arg_t,
                                        allocator_type allocator,
                                        bsl::in_place_index_t<INDEX>,
                                        const ARG&     arg)
: AllocBase(allocator)
, d_type(INDEX)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<INDEX, Variant>::type>::type TYPE;

    BloombergLP::bslma::ConstructionUtil::construct(
                        reinterpret_cast<TYPE *>(BSLS_UTIL_ADDRESSOF(d_union)),
                        AllocBase::mechanism(),
                        arg);
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
Variant_Base<HEAD, TAIL...>::~Variant_Base()
{
    reset();
}
#endif
// MANIPULATORS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class HEAD, class... TAIL>
template <size_t INDEX, class... ARGS>
void Variant_Base<HEAD, TAIL...>::baseEmplace(ARGS&&... args)
{
    reset();

    // We need to assemble the construction arguments for the underlying
    //'Variant_DataImp' object here.  They require that allocator pointer is
    // the leading argument after 'Variant_Union' strips the 'INDEX'.  To be
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
                                                bsl::in_place_index_t<INDEX>{},
                                                AllocBase::mechanism(),
                                                std::forward<ARGS>(args)...);
    d_type = INDEX;
}

#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
template <size_t INDEX>
void Variant_Base<HEAD, TAIL...>::baseEmplace()
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<INDEX, Variant>::type>::type TYPE;

    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
                         reinterpret_cast<TYPE *> BSLS_UTIL_ADDRESSOF(d_union),
                         AllocBase::mechanism());
    d_type = INDEX;
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class ARG>
void Variant_Base<HEAD, TAIL...>::baseEmplace(const ARG& arg)
{
    typedef typename bsl::remove_cv<
        typename bsl::variant_alternative<INDEX, Variant>::type>::type TYPE;

    reset();

    BloombergLP::bslma::ConstructionUtil::construct(
                         reinterpret_cast<TYPE *> BSLS_UTIL_ADDRESSOF(d_union),
                         AllocBase::mechanism(),
                         arg);
    d_type = INDEX;
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>& Variant_Base<HEAD, TAIL...>::operator=(
                                                       const Variant_Base& rhs)
{
    if (&rhs != this) {
        if (this->d_type == rhs.d_type) {
            if (d_type != bsl::variant_npos) {
                Variant& self = static_cast<Variant&>(*this);
                BloombergLP::bslstl::Variant_CopyAssignVisitor<Variant>
                copyAssign(BSLS_UTIL_ADDRESSOF(self));
                BSLSTL_VARIANT_VISITID(void,
                                       copyAssign,
                                       static_cast<const Variant&>(rhs));
            }
        }
        else {
            reset();
            if (rhs.d_type != bsl::variant_npos) {
                BloombergLP::bslstl::Variant_CopyConstructVisitor<Variant_Base>
                    copyConstructor(this);
                BSLSTL_VARIANT_VISITID(void,
                                       copyConstructor,
                                       static_cast<const Variant&>(rhs));
            }
            d_type = rhs.d_type;
        }
    }
    return *this;
}

template <class HEAD, class... TAIL>
inline
Variant_Base<HEAD, TAIL...>& Variant_Base<HEAD, TAIL...>::operator=(
                              BloombergLP::bslmf::MovableRef<Variant_Base> rhs)
{
    Variant_Base& lvalue = rhs;
    if (&lvalue != this) {
        if (this->d_type == lvalue.d_type) {
            if (d_type != bsl::variant_npos) {
                Variant& self = static_cast<Variant&>(*this);
                BloombergLP::bslstl::Variant_MoveAssignVisitor<Variant>
                moveAssign(BSLS_UTIL_ADDRESSOF(self));
                BSLSTL_VARIANT_VISITID(void,
                                       moveAssign,
                                       static_cast<Variant&>(lvalue));
            }
        }
        else {
            reset();
            if (lvalue.d_type != bsl::variant_npos) {
                BloombergLP::bslstl::Variant_MoveConstructVisitor<Variant_Base>
                    moveConstructor(this);
                BSLSTL_VARIANT_VISITID(void,
                                       moveConstructor,
                                       static_cast<Variant&>(lvalue));
            }
            d_type = lvalue.d_type;
        }
    }
    return *this;
}
template <class HEAD, class... TAIL>
void Variant_Base<HEAD, TAIL...>::reset() BSLS_KEYWORD_NOEXCEPT
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
template <class HEAD, class... TAIL>
template <class TYPE>
inline
variant<HEAD, TAIL...>::variant(
                       TYPE&& value,
                       BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, TYPE))
: Variant_Base(
       bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, variant)>(),
       std::forward<TYPE>(value))
{
}

template <class HEAD, class... TAIL>
template <class TYPE>
inline
variant<HEAD, TAIL...>::variant(
                       bsl::allocator_arg_t,
                       allocator_type allocator,
                       TYPE&&         value,
                       BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, TYPE))
: Variant_Base(
       bsl::allocator_arg_t{},
       allocator,
       bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, variant)>(),
       std::forward<TYPE>(value))
{
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>::variant()
: Variant_Base()
{
}

template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>::variant(const variant& original)
: Variant_Base(original)
{
}

template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>::variant(
                              BloombergLP::bslmf::MovableRef<variant> original)
: Variant_Base(original)
{
}

template <class HEAD, class... TAIL>
template <class TYPE>
inline
variant<HEAD, TAIL...>::variant(
                       const TYPE& value,
                       BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, TYPE))
: Variant_Base(
       bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, variant)>(),
       value)
{
}

template <class HEAD, class... TAIL>
template <class TYPE>
inline
variant<HEAD, TAIL...>::variant(bsl::in_place_type_t<TYPE>,
                                BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(TYPE))
: Variant_Base(bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>())
{
}

template <class HEAD, class... TAIL>
template <class TYPE, class ARG>
inline
variant<HEAD, TAIL...>::variant(bsl::in_place_type_t<TYPE>,
                                const ARG& arg,
                                BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(TYPE))
: Variant_Base(bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>(),
               arg)
{
}

template <class HEAD, class... TAIL>
template <size_t INDEX>
inline
variant<HEAD, TAIL...>::variant(bsl::in_place_index_t<INDEX>)
: Variant_Base(bsl::in_place_index_t<INDEX>())
{
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class ARG>
inline
variant<HEAD, TAIL...>::variant(bsl::in_place_index_t<INDEX>, const ARG& arg)
: Variant_Base(bsl::in_place_index_t<INDEX>(), arg)
{
}

template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>::variant(bsl::allocator_arg_t, allocator_type allocator)
: Variant_Base(bsl::allocator_arg_t(), allocator)
{
}

template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>::variant(bsl::allocator_arg_t,
                                allocator_type allocator,
                                const variant& original)
: Variant_Base(bsl::allocator_arg_t(), allocator, original)
{
}

template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>::variant(
                             bsl::allocator_arg_t,
                             allocator_type                          allocator,
                             BloombergLP::bslmf::MovableRef<variant> original)
: Variant_Base(bsl::allocator_arg_t(), allocator, original)
{
}

template <class HEAD, class... TAIL>
template <class TYPE>
inline
variant<HEAD, TAIL...>::variant(
                       bsl::allocator_arg_t,
                       allocator_type allocator,
                       const TYPE&    value,
                       BSLSTL_VARIANT_DEFINE_IF_CONSTRUCTS_FROM(variant, TYPE))
: Variant_Base(
       bsl::allocator_arg_t(),
       allocator,
       bsl::in_place_index_t<BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, variant)>(),
       value)
{
}

template <class HEAD, class... TAIL>
template <class TYPE>
inline
variant<HEAD, TAIL...>::variant(bsl::allocator_arg_t,
                                allocator_type allocator,
                                bsl::in_place_type_t<TYPE>,
                                BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(TYPE))
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>())
{
}

template <class HEAD, class... TAIL>
template <class TYPE, class ARG>
inline
variant<HEAD, TAIL...>::variant(bsl::allocator_arg_t,
                                allocator_type allocator,
                                bsl::in_place_type_t<TYPE>,
                                const ARG&     arg,
                                BSLSTL_VARIANT_DEFINE_IF_HAS_UNIQUE_TYPE(TYPE))
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, variant)>(),
               arg)
{
}

template <class HEAD, class... TAIL>
template <size_t INDEX>
inline
variant<HEAD, TAIL...>::variant(bsl::allocator_arg_t,
                                allocator_type allocator,
                                bsl::in_place_index_t<INDEX>)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<INDEX>())
{
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class ARG>
inline
variant<HEAD, TAIL...>::variant(bsl::allocator_arg_t,
                                allocator_type allocator,
                                bsl::in_place_index_t<INDEX>,
                                const ARG& arg)
: Variant_Base(bsl::allocator_arg_t(),
               allocator,
               bsl::in_place_index_t<INDEX>(),
               arg)
{
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
template <class HEAD, class... TAIL>
template <class TYPE>
typename bsl::enable_if<
    BloombergLP::bslstl::Variant_HasUniqueType<TYPE,
                                               variant<HEAD, TAIL...> >::value,
    TYPE&>::type
variant<HEAD, TAIL...>::emplace()
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(TYPE, variant);

    Variant_Base::template baseEmplace<index>();

    return bsl::get<index>(*this);
}

template <class HEAD, class... TAIL>
template <class TYPE, class ARG>
typename bsl::enable_if<
    BloombergLP::bslstl::Variant_HasUniqueType<TYPE,
                                               variant<HEAD, TAIL...> >::value,
    TYPE&>::type
variant<HEAD, TAIL...>::emplace(const ARG& arg)
{
    const size_t index = BSLSTL_VARIANT_INDEX_OF(TYPE, variant);

    Variant_Base::template baseEmplace<index>(arg);

    return bsl::get<index>(*this);
}

template <class HEAD, class... TAIL>
template <size_t INDEX>
typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&
variant<HEAD, TAIL...>::emplace()
{
    Variant_Base::template baseEmplace<INDEX>();

    return bsl::get<INDEX>(*this);
}

template <class HEAD, class... TAIL>
template <size_t INDEX, class ARG>
typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&
variant<HEAD, TAIL...>::emplace(const ARG& arg)
{
    Variant_Base::template baseEmplace<INDEX>(arg);

    return bsl::get<INDEX>(*this);
}
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifndef BSL_VARIANT_FULL_IMPLEMENTATION
template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>& variant<HEAD, TAIL...>::operator=(const variant& rhs)
{
    Variant_Base::operator=(static_cast<const Variant_Base&>(rhs));
    return *this;
}

template <class HEAD, class... TAIL>
inline
variant<HEAD, TAIL...>& variant<HEAD, TAIL...>::operator=(
                                   BloombergLP::bslmf::MovableRef<variant> rhs)
{
    variant<HEAD, TAIL...>& lvalue = rhs;
    Variant_Base::operator=(
                           MoveUtil::move(static_cast<Variant_Base&>(lvalue)));
    return *this;
}
template <class HEAD, class... TAIL>
template <class TYPE>
inline
typename bsl::enable_if<
    BloombergLP::bslstl::Variant_AssignsFromType<variant<HEAD, TAIL...>,
                                                 TYPE>::value,
    variant<HEAD, TAIL...>&>::type
variant<HEAD, TAIL...>::operator=(const TYPE& rhs)
{
    const size_t altIndex = BSLSTL_VARIANT_CONVERT_INDEX_OF(TYPE, variant);

    if (index() == altIndex) {
        bsl::get<altIndex>(*this) = rhs;
    } else {
        // 'altIndex' can not be 'variant_npos' if 'Variant_AssignsFromType' is
        // satisfied
        emplace<altIndex>(rhs);
    }

    return *this;
}

#endif  // BSL_VARIANT_FULL_IMPLEMENTATION
template <class HEAD, class... TAIL>
inline
void variant<HEAD, TAIL...>::swap(variant& other)
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
template <class HEAD, class... TAIL>
inline
size_t variant<HEAD, TAIL...>::index() const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_type;
}

template <class HEAD, class... TAIL>
inline
bool
variant<HEAD, TAIL...>::valueless_by_exception() const BSLS_KEYWORD_NOEXCEPT
{
    return this->d_type == bsl::variant_npos;
}

#endif
// FREE FUNCTIONS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <size_t INDEX, class HEAD, class... TAIL>
typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type& get(
                                                   variant<HEAD, TAIL...>& obj)
{
    BSLMF_ASSERT((INDEX < variant_size<variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, Variant>::type Ret;

    return Var_ImpUtil::template getImpl<Ret>(bsl::in_place_index_t<INDEX>(),
                                              obj);
}

template <size_t INDEX, class HEAD, class... TAIL>
const typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type& get(
                                             const variant<HEAD, TAIL...>& obj)
{
    BSLMF_ASSERT((INDEX < variant_size<variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, Variant>::type Ret;

    return Var_ImpUtil::template getImpl<const Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                obj);
}

template <size_t INDEX, class HEAD, class... TAIL>
typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&& get(
                                                  variant<HEAD, TAIL...>&& obj)
{
    BSLMF_ASSERT((INDEX < variant_size<variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, Variant>::type Ret;

    return std::move(Var_ImpUtil::template getImpl<Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                obj));
}

template <size_t INDEX, class HEAD, class... TAIL>
const typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type&& get(
                                            const variant<HEAD, TAIL...>&& obj)
{
    BSLMF_ASSERT((INDEX < variant_size<variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, Variant>::type Ret;

    return std::move(Var_ImpUtil::template getImpl<const Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                obj));
}
#else  // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    typename variant_alternative<INDEX, VARIANT>::type&,
    VARIANT>::type
get(VARIANT& obj)
{
    BSLMF_ASSERT((INDEX < variant_size<VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, VARIANT>::type Ret;

    return Var_ImpUtil::template getImpl<Ret>(bsl::in_place_index_t<INDEX>(),
                                              obj);
}

template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    const typename variant_alternative<INDEX, VARIANT>::type&,
    VARIANT>::type
get(const VARIANT& obj)
{
    BSLMF_ASSERT((INDEX < variant_size<VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, VARIANT>::type Ret;

    return Var_ImpUtil::template getImpl<const Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                obj);
}

template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    BloombergLP::bslmf::MovableRef<
        typename variant_alternative<INDEX, VARIANT>::type>,
    VARIANT>::type
get(BloombergLP::bslmf::MovableRef<VARIANT> obj)
{
    typedef BloombergLP::bslmf::MovableRefUtil                     MoveUtil;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, VARIANT>::type Ret;

    VARIANT& lvalue = obj;

    return MoveUtil::move(Var_ImpUtil::template getImpl<Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                lvalue));
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class TYPE, class HEAD, class... TAIL>
TYPE& get(variant<HEAD, TAIL...>& obj)
{
    BSLMF_ASSERT(
            (BloombergLP::bslstl::
                 Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    return Var_ImpUtil::template getImpl<TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(),
               obj);
}

template <class TYPE, class HEAD, class... TAIL>
const TYPE& get(const variant<HEAD, TAIL...>& obj)
{
    BSLMF_ASSERT(
            (BloombergLP::bslstl::
                 Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    return Var_ImpUtil::template getImpl<const TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(),
               obj);
}

template <class TYPE, class HEAD, class... TAIL>
TYPE&& get(variant<HEAD, TAIL...>&& obj)
{
    static_assert(
               BloombergLP::bslstl::
                   Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value,
               "Type is not unique in variant");

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    return std::move(Var_ImpUtil::template getImpl<TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(),
               obj));
}

template <class TYPE, class HEAD, class... TAIL>
const TYPE&& get(const variant<HEAD, TAIL...>&& obj)
{
    static_assert(
               BloombergLP::bslstl::
                   Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value,
               "Type is not unique in variant");

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    return std::move(Var_ImpUtil::template getImpl<const TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(),
               obj));
}
#else   // BSL_VARIANT_FULL_IMPLEMENTATION
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, class VARIANT>
typename Variant_ReturnType<TYPE&, VARIANT>::type get(VARIANT& obj)
{
    BSLMF_ASSERT(
           (BloombergLP::bslstl::Variant_HasUniqueType<TYPE, VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    return Var_ImpUtil::template getImpl<TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)>(),
               obj);
}

template <class TYPE, class VARIANT>
typename Variant_ReturnType<const TYPE&, VARIANT>::type get(const VARIANT& obj)
{
    BSLMF_ASSERT(
           (BloombergLP::bslstl::Variant_HasUniqueType<TYPE, VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    return Var_ImpUtil::template getImpl<const TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)>(),
               obj);
}

template <class TYPE, class VARIANT>
typename Variant_ReturnType<BloombergLP::bslmf::MovableRef<TYPE>,
                            VARIANT>::type
get(BloombergLP::bslmf::MovableRef<VARIANT> obj)
{
    BSLMF_ASSERT(
           (BloombergLP::bslstl::Variant_HasUniqueType<TYPE, VARIANT>::value));

    typedef BloombergLP::bslmf::MovableRefUtil                     MoveUtil;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    VARIANT& lvalue = obj;
    return MoveUtil::move(Var_ImpUtil::template getImpl<TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)>(),
               lvalue));
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <size_t INDEX, class HEAD, class... TAIL>
typename add_pointer<
    typename variant_alternative<INDEX, variant<HEAD, TAIL...> >::type>::type
get_if(variant<HEAD, TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((INDEX < variant_size<variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, Variant>::type Ret;

    if (ptr == 0 || ptr->index() != INDEX) {
        return NULL;                                                  // RETURN
    }

    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                *ptr));
}

template <size_t INDEX, class HEAD, class... TAIL>
typename add_pointer<
    const typename variant_alternative<INDEX,
                                       variant<HEAD, TAIL...> >::type>::type
get_if(const variant<HEAD, TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((INDEX < variant_size<variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, Variant>::type Ret;

    if (ptr == 0 || ptr->index() != INDEX) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<const Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                *ptr));
}

template <class TYPE, class HEAD, class... TAIL>
typename add_pointer<TYPE>::type
get_if(variant<HEAD, TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT(
            (BloombergLP::bslstl::
                 Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (ptr == 0 || ptr->index() != BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(),
               *ptr));
}

template <class TYPE, class HEAD, class... TAIL>
typename add_pointer<const TYPE>::type
get_if(const variant<HEAD, TAIL...> *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT(
            (BloombergLP::bslstl::
                 Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value));

    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (ptr == 0 || ptr->index() != BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<const TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, Variant)>(),
               *ptr));
}
#else
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<typename variant_alternative<INDEX, VARIANT>::type,
                            VARIANT>::pointer
get_if(VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((INDEX < variant_size<VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, VARIANT>::type Ret;

    if (ptr == 0 || ptr->index() != INDEX) {
        return NULL;                                                  // RETURN
    }

    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                *ptr));
}
template <size_t INDEX, class VARIANT>
typename Variant_ReturnType<
    const typename variant_alternative<INDEX, VARIANT>::type,
    VARIANT>::pointer
get_if(const VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT((INDEX < variant_size<VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    typedef typename bsl::variant_alternative<INDEX, VARIANT>::type Ret;

    if (ptr == 0 || ptr->index() != INDEX) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<const Ret>(
                                                bsl::in_place_index_t<INDEX>(),
                                                *ptr));
}

template <class TYPE, class VARIANT>
typename Variant_ReturnType<TYPE, VARIANT>::pointer
get_if(VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT(
           (BloombergLP::bslstl::Variant_HasUniqueType<TYPE, VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    if (ptr == 0 || ptr->index() != BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)>(),
               *ptr));
}

template <class TYPE, class VARIANT>
typename Variant_ReturnType<const TYPE, VARIANT>::pointer
get_if(const VARIANT *ptr) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT(
           (BloombergLP::bslstl::Variant_HasUniqueType<TYPE, VARIANT>::value));

    typedef typename BloombergLP::bslstl::Variant_ImpUtil<VARIANT> Var_ImpUtil;

    if (ptr == 0 || ptr->index() != BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)) {
        return NULL;                                                  // RETURN
    }
    return BSLS_UTIL_ADDRESSOF(Var_ImpUtil::template getImpl<const TYPE>(
               bsl::in_place_index_t<BSLSTL_VARIANT_INDEX_OF(TYPE, VARIANT)>(),
               *ptr));
}
#endif
#endif  // BSL_VARIANT_FULL_IMPLEMENTATION

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// HASH SPECIALIZATIONS
template <class HASHALG, class HEAD, class... TAIL>
void hashAppend(HASHALG& hashAlg, const variant<HEAD, TAIL...>& input)
{
    if (!input.valueless_by_exception()) {
        hashAppend(hashAlg, input.index());
        BloombergLP::bslstl::Variant_HashVisitor<HASHALG> hashVisitor(hashAlg);
        visit(hashVisitor, input);
    } else {
        hashAppend(hashAlg, false);
    }
}

template <class TYPE, class HEAD, class... TAIL>
bool holds_alternative(const variant<HEAD, TAIL...>& obj) BSLS_KEYWORD_NOEXCEPT
{
    BSLMF_ASSERT(
            (BloombergLP::bslstl::
                 Variant_HasUniqueType<TYPE, variant<HEAD, TAIL...> >::value));
    typedef typename bsl::variant<HEAD, TAIL...> Variant;
    return obj.index() == BSLSTL_VARIANT_INDEX_OF(TYPE, Variant);
}

template <class HEAD, class... TAIL>
void swap(bsl::variant<HEAD, TAIL...>& lhs, bsl::variant<HEAD, TAIL...>& rhs)
{
    BloombergLP::bslstl::variant_swapImpl(
        bsl::integral_constant<
            bool,
            BloombergLP::bslstl::Variant_UsesBslmaAllocatorAny<HEAD, TAIL...>::
                value>(),
        lhs,
        rhs);
}

// FREE OPERATORS
template <class HEAD, class... TAIL>
bool operator==(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs)
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (lhs.index() != rhs.index()) {
        return false;                                                 // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    return Var_ImpUtil::EqualImpl(lhs, rhs);
}

template <class HEAD, class... TAIL>
bool operator!=(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs)
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (lhs.index() != rhs.index()) {
        return true;                                                  // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    return Var_ImpUtil::NotEqualImpl(lhs, rhs);
}

template <class HEAD, class... TAIL>
bool operator<(const variant<HEAD, TAIL...>& lhs,
               const variant<HEAD, TAIL...>& rhs)
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (rhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() < rhs.index();                             // RETURN
    }
    return Var_ImpUtil::LessThanImpl(lhs, rhs);
}

template <class HEAD, class... TAIL>
bool operator>(const variant<HEAD, TAIL...>& lhs,
               const variant<HEAD, TAIL...>& rhs)
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (lhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (rhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() > rhs.index();                             // RETURN
    }
    return Var_ImpUtil::GreaterThanImpl(lhs, rhs);
}

template <class HEAD, class... TAIL>
bool operator<=(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs)
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (lhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (rhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() < rhs.index();                             // RETURN
    }
    return Var_ImpUtil::LessOrEqualImpl(lhs, rhs);
}

template <class HEAD, class... TAIL>
bool operator>=(const variant<HEAD, TAIL...>& lhs,
                const variant<HEAD, TAIL...>& rhs)
{
    typedef typename bsl::variant<HEAD, TAIL...>                   Variant;
    typedef typename BloombergLP::bslstl::Variant_ImpUtil<Variant> Var_ImpUtil;

    if (rhs.valueless_by_exception()) {
        return true;                                                  // RETURN
    }
    else if (lhs.valueless_by_exception()) {
        return false;                                                 // RETURN
    }
    else if (lhs.index() != rhs.index()) {
        return lhs.index() > rhs.index();                             // RETURN
    }

    return Var_ImpUtil::GreaterOrEqualImpl(lhs, rhs);
}
#endif
}  // close namespace bsl

#endif // End C++11 code

// #undef BSL_VARIANT_FULL_IMPLEMENTATION
// This macro is needed by the test driver.
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
#endif // INCLUDED_BSLSTL_VARIANT

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
