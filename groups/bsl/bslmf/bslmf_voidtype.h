// bslmf_voidtype.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_VOIDTYPE
#define INCLUDED_BSLMF_VOIDTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a helper for implementing SFINAE-based metafunctions.
//
//@CLASSES:
//  bsl::void_t:     alias template to help create SFINAE contexts in C++11
//  bslmf::VoidType: class template to emulate `bsl::void_t` in C++03
//
//@MACROS:
//  BSLMF_VOIDTYPE:  helper macro for SFINAE-based metafunctions
//  BSLMF_VOIDTYPE2: helper macro for SFINAE-based metafunctions
//  BSLMF_VOIDTYPES: helper macro for SFINAE-based metafunctions
//
//@SEE_ALSO: bslmf_resulttype
//
//@DESCRIPTION: This component provides the alias template `bsl::void_t`, as
// specified by the C++14 standard, and a metafunction, `bslmf::VoidType`, to
// emulate the same functionality for older compilers that do not support alias
// templates, which are first specified by the C++11 standard.  It further
// provides 3 macros, `BSLMF_VOIDTYPE/2/S`, that provide a consistent way to
// use the alias template where supported and the class template otherwise.
//
// All forms of the metafunction, however it is written, produce the result
// type `void`.  The usefulness of this do-nothing metafunction is that, when
// it is instantiated, all of its template arguments must be valid.  By putting
// the template instantiation in a SFINAE context, any use of template
// parameters that name invalid dependent types are discarded by the compiler
// as non-viable.  Thus, `VoidType` is most commonly used to build
// metafunctions that test for the existence of a specific nested data type
// (see {Usage}).
//
// The `bslmf::VoidType` class template is intended to provide functionality
// identical to the C++14 metafunction `std::void_t`, but without using C++11
// alias templates.  A use, in C++14-compliant code, of:
// ```
// std::void_t<T1, T2, ...>
// ```
// can be replaced, in BDE-compliant code using any version of standard C++,
// by:
// ```
// typename bslmf::VoidType<T1, T2, ...>::type
// ```
//
///Macro Reference
///---------------
// This section documents the preprocessor macros defined in this component.
//
///Macros for type-dependant SFINAE checks in any C++ dialect
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following macros are for use only in a type-dependent context.  They all
// expand to a type expression that uses either `bsl::void_t<ARGS>` if alias
// templates are supported by the current compiler, and to
// `typename BloombergLP::bslmf::VoidType<ARGS>::type` otherwise.  The alias
// template form is preferred, as it consumes fewer resources while compiling
// code; the compiled code will be equivalent, whichever implementation is
// chosen.  These macros support writing simple code that uses the preferred
// idiom supported by the current tool chain.  Note that code targeting only
// C++11 or later can use `bsl::void_t` directly with no loss of efficiency or
// generality.  These macros are needed only to support older C++03 tool
// chains.
//
// The three macros support one, two, or many arguments, and are otherwise
// identical.  The reason for three macros is that the overwhelmingly common
// use cases need only one, rarely two, type expressions, so for legacy
// compilers that do not support variadic templates, we can use a simpler class
// template that has fewer type parameters.
//
//: `BSLMF_VOIDTYPE( TYPE_EXPRESSION )`:
//:     This macro will expand into a type expression that aliases `void` if
//:     the "TYPE EXPRESSION" is valid, and will fail to expand in a SFINAE
//:     friendly manner if the type expressions is not valid.
//
//: `BSLMF_VOIDTYPE2( TYPE_EXPRESSION_1, TYPE_EXPRESSION_2 )`:
//:     This macro will expand into a type expression that aliases `void` if
//:     each "TYPE EXPRESSION" is valid, and will fail to expand in a SFINAE
//:     friendly manner if either of the type expressions is not valid.
//
//: `BSLMF_VOIDTYPES( TYPE_EXPRESSIONS... )`:
//:     This macro will expand into a type expression that aliases `void` if
//:     each "TYPE EXPRESSION" is valid, and will fail to expand in a SFINAE
//:     friendly manner if any of the type expressions is not valid.
//
///Additional concerns when using old compilers
///--------------------------------------------
// When compiling with a C++03 tool chain, the `typename` keyword in the macro
// expansion, used to extract the nested `type` from the class template, is not
// valid syntax unless the whole type expression is type (or value) dependent.
// Such use will produce an error, even if the supplied type expression is
// valid.  E.g., `BSLMF_VOIDTYPE(void)` will always be a (non-SFINAE) error in
// C++03, but is perfectly valid in C++11.  Be sure to test your code with a
// C++03 build when deploying these macros.
//
// The templates and macros in this component aid in creating SFINAE conditions
// in a conforming C++03 manner.  However, several of the compilers still in
// production have significant issues in their handling of SFINAE.  For
// example, the Solaris CC compiler (prior to the 12.4 release) is very
// forgiving and will accept most invalid code without creating a SFINAE
// failure.  Idiomatic use of this component for compile-time reflection to
// detect named members of a class is known to work.  Other uses should be
// carefully tested before deployment.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Usage Example 1
///- - - - - - - -
// In this example, we demonstrate the use of `VoidType` to determine whether a
// given type `T` has a member type `T::iterator`.  Our goal is to create a
// metafunction, `HasIteratorType`, such that `HasIteratorType<T>::k_VALUE` is
// `true` if `T::iterator` is a valid type and `false` otherwise.  This example
// is adapted from the paper proposing `std::void_t` for the C++ Standard,
// N3911.
//
// First, we define the base-case metafunction that returns `false`:
// ```
// template <class TYPE, class = void>
// struct HasIteratorType {
//     enum { k_VALUE = false };
// };
// ```
// Then, we create a partial specialization that uses `VoidType` to probe for
// `T::iterator`:
// ```
// template <class TYPE>
// struct HasIteratorType<TYPE, BSLMF_VOIDTYPE(typename TYPE::iterator)> {
//     enum { k_VALUE = true };
// };
// ```
// Now, we define a class that has an `iterator` member and apply
// `HasIteratorType` to it:
// ```
// struct WithIterator {
//     typedef short *iterator;
// };
//
// void usageExample1()
// {
//     assert(true == HasIteratorType<WithIterator>::k_VALUE);
// ```
// As `WithIterator::iterator` is a valid type,
// `BSLMF_VOIDTYPE(typename TYPE::iterator)` will be `void` and the second
// `HasIteratorType` template will be more specialized than the primary
// template, thus yielding a `k_VALUE` of `true`.
//
// Finally, we try to instantiate `HasIteratorType<int>`.  Any use of
// `BSLMF_VOIDTYPE(TYPE::iterator)` will result in a substitution failure.
// Fortunately, the Substitution Failure Is Not An Error (SFINAE) rule applies
// and the partial specialization is eliminated from consideration, resulting
// in the primary template being instantiated and yielding a `k_VALUE` of
// `false`:
// ```
//     assert(false == HasIteratorType<int>::k_VALUE);
// }
// ```
//
///Usage Example 2
///- - - - - - - -
// This example demonstrates the use of `VoidType` to probe for more than one
// type at once.  As in the previous example, we are defining a metafunction.
// We'll define `IsTraversable<T>::k_VALUE` to be `true` if `T::iterator` and
// `T::value_type` both exist.  First, we define a primary template that always
// yields `false`:
// ```
// template <class TYPE, class = void>
// struct IsTraversable {
//     enum { k_VALUE = false };
// };
// ```
// Then, we create a partial specialization that uses `BSLMF_VOIDTYPE2` with
// two parameters:
// ```
// template <class TYPE>
// struct IsTraversable<TYPE,
//                      BSLMF_VOIDTYPE2(typename TYPE::iterator,
//                                      typename TYPE::value_type)> {
//     enum { k_VALUE = true };
// };
// ```
// Now, we define a type that meets the requirements for being traversable:
// ```
// struct MyTraversable {
//     typedef int  value_type;
//     typedef int *iterator;
// };
// ```
// Finally, the `IsTraversable` metafunction yields `true` for `Traversable`
// but not for either `WithIterator`, which lacks a `value_type` member, nor
// `int`, which lacks both `iterator` and `value_type` members:
// ```
// int usageExample2()
// {
//     assert(true  == IsTraversable<MyTraversable>::k_VALUE);
//     assert(false == IsTraversable<WithIterator>::k_VALUE);
//     assert(false == IsTraversable<int>::k_VALUE);
// }
// ```

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace BloombergLP {
namespace bslmf {

                        // =======================
                        // class template VoidType
                        // =======================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
/// Metafunction that always yields `type` `void` for any well-formed list
/// of type parameters.  This metafunction is useful when using SFINAE to
/// probe for well-formed types.
template <class ...>
#else
template <class T1  = void, class T2  = void, class T3  = void,
          class T4  = void, class T5  = void, class T6  = void,
          class T7  = void, class T8  = void, class T9  = void,
          class T10 = void, class T11 = void, class T12 = void,
          class T13 = void, class T14 = void>
#endif
struct VoidType {

    // PUBLIC TYPES
    typedef void type;
};

}  // close package namespace
}  // close enterprise namespace


#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&   \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
namespace bsl {

template <class...>
using void_t = void;
}  // close namespace bsl

# define BSLMF_VOIDTYPE(ARG)  bsl::void_t<     ARG   >
# define BSLMF_VOIDTYPE2(...) bsl::void_t<__VA_ARGS__>
# define BSLMF_VOIDTYPES(...) bsl::void_t<__VA_ARGS__>

#else
// Define a couple of implementation-private classes with smaller template
// parameter lists, for efficient use on C++03 compilers.  Note that the
// preferred interface for portable C++03/11 code is to use the three
// 'VOIDTYPE' macros, rather than the class templates directly.

namespace BloombergLP {
namespace bslmf {

template <class T1>
struct VoidType_1 {
    // Metafunction that always yields 'type' 'void' for any well-formed type
    // parameter.  This metafunction is useful when using SFINAE to probe for
    // well-formed types.  Note that this metafunction is not intended for
    // direct user consumption, but rather as an implementation detail for the
    // 'BSLMF_VOIDTYPE' macro.

    // PUBLIC TYPES
    typedef void type;
};

template <class T1, class T2>
struct VoidType_2 {
    // Metafunction that always yields 'type' 'void' for any well-formed list
    // of type parameters.  This metafunction is useful when using SFINAE to
    // probe for well-formed types.  Note that this metafunction is not
    // intended for direct user consumption, but rather as an implementation
    // detail for the 'BSLMF_VOIDTYPE2' macro.

    // PUBLIC TYPES
    typedef void type;
};

}  // close package namespace
}  // close enterprise namespace

# define BSLMF_VOIDTYPE(ARG)                                                  \
                   typename BloombergLP::bslmf::VoidType_1<     ARG   >::type
# define BSLMF_VOIDTYPE2(...)                                                 \
                   typename BloombergLP::bslmf::VoidType_2<__VA_ARGS__>::type
# define BSLMF_VOIDTYPES(...)                                                 \
                   typename BloombergLP::bslmf::VoidType<__VA_ARGS__>::type
#endif  // supports C++11 API directly

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
