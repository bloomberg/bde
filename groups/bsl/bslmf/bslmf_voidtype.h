// bslmf_voidtype.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_VOIDTYPE
#define INCLUDED_BSLMF_VOIDTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a helper for implementing SFINAE-based metafunctions.
//
//@CLASSES:
//  bslmf::VoidType: helper class template for SFINAE-based metafunctions
//
//@MACROS:
//  BSLMF_VOIDTYPE: helper macro for SFINAE-based metafunctions
//
//@SEE_ALSO: bslmf_resulttype
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:  This component provides a trivial metafunction,
// 'bslmf::VoidType', that takes up to 14 type parameters.  The template always
// yields a member type, 'type', that is always 'void'.  The usefulness of this
// do-nothing metafunction is that, when it is instantiated, all of its
// template parameters must be valid.  By putting the template instantiation in
// a SFINAE context, any use of template parameters that name invalid dependent
// types are discarded by the compiler as non-viable.  Thus, 'VoidType' is most
// commonly used to build metafunctions that test for the existence of a
// specific nested data type (see {Usage}).
//
// The 'bslmf::VoidType' class template is intended to provide functionality
// identical to the C++14 metafunction 'std::void_t', but without using C++11
// alias templates.  A use, in C++14-compliant code, of:
//..
//  std::void_t<T1, T2, ...>
//..
// can be replaced, in BDE-compliant code using any version of standard C++,
// by:
//..
//  typename bslmf::VoidType<T1, T2, ...>::type
//..
//
///Macro Reference
///---------------
// This section documents the preprocessor macros defined in this component.
//
///Macros for type-dependant SFINAE checks in any C++ dialect
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following macro is for use in a type-dependent context, to enable the
// most appropriate idiom for SFINAE checks in the C++ language version being
// compiled:
//
//: 'BSLMF_VOIDTYPE( TYPE_EXPRESSIONS... )':
//:     This macro will expand into a type expression that aliases 'void' if
//:     each "TYPE EXPRESSION" is valid, and will fail to expand in a SFINAE
//:     friendly manner if any of the type expressions is not valid.  Note that
//:     this macro will likely expand to an error on a C++03 compiler if used
//:     in a non-dependent expression, where the 'typename' keyword is not
//:     permitted.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Usage Example 1
///- - - - - - - -
// In this example, we demonstrate the use of 'VoidType' to determine whether a
// given type 'T' has a member type 'T::iterator'.  Our goal is to create a
// metafunction, 'HasIteratorType', such that 'HasIteratorType<T>::VALUE' is
// 'true' if 'T::iterator' is a valid type and 'false' otherwise.  This example
// is adapted from the paper proposing 'std::void_t' for the C++ Standard,
// N3911.
//
// First, we define the base-case metafunction that returns 'false':
//..
//  template <class TYPE, class = void>
//  struct HasIteratorType {
//      enum { VALUE = false };
//  };
//..
// Then, we create a partial specialization that uses 'VoidType' to probe for
// 'T::iterator':
//..
//  template <class TYPE>
//  struct HasIteratorType<TYPE, BSLMF_VOIDTYPE(typename TYPE::iterator)> {
//      enum { VALUE = true };
//  };
//..
// Now, we define a class that has a 'iterator' member and apply
// 'HasIteratorType' to it:
//..
//  struct WithIterator {
//      typedef short *iterator;
//  };
//..
// Finally, we define a class that has an 'iterator' member and apply
// 'HasIteratorType' to it:
//..
//  int usageExample1()
//  {
//      assert(true == HasIteratorType<WithIterator>::VALUE);
//..
// Since 'WithIterator::iterator' is a valid type,
// 'BSLMF_VOIDTYPE(TYPE::iterator)' will be 'void' and the second
// 'HasIteratorType' template will be more specialized than the primary
// template and will thus get instantiated, yielding a 'VALUE' of 'true'.
//
// Conversely, if we try to instantiate 'HasIteratorType<int>', any use of
// 'BSLMF_VOIDTYPE(TYPE::iterator)' will result in a substitution failure.
// Fortunately, the Substitution Failure Is Not An Error (SFINAE) rule applies,
// so the code will compile, but the specialization is eliminated from
// consideration, resulting in the primary template being instantiated and
// yielding a 'VALUE' of 'false':
//..
//      assert(false == HasIteratorType<int>::VALUE);
//
//      return 0;
//  }
//..
//
///Usage Example 2
///- - - - - - - -
// This example demonstrates the use of 'VoidType' to probe for more than one
// type at once.  As in the previous example, we are defining a metafunction.
// We'll define 'IsTraversable<T>::VALUE' to be 'true' if 'T::iterator' and
// 'T::value_type' both exist.  First, we define a primary template that always
// yields 'false':
//..
//  template <class TYPE, class = void>
//  struct IsTraversable {
//      enum { VALUE = false };
//  };
//..
// Then, we create a partial specialization that uses 'BSLMF_VOIDTYPE' with two
// parameters:
//..
//  template <class TYPE>
//  struct IsTraversable<TYPE,
//                       BSLMF_VOIDTYPE(typename TYPE::iterator,
//                                      typename TYPE::value_type)> {
//      enum { VALUE = true };
//  };
//..
// Now, we define a type that meets the requirement for being traversable:
//..
//  struct MyTraversable {
//      typedef int  value_type;
//      typedef int *iterator;
//  };
//..
// Finally, the 'IsTraversable' metafunction yields 'true' for 'Traversable'
// but not for either 'WithIterator', which lacks a 'value_type' member, nor
// 'int', which lacks both 'iterator' and 'value_type' members:
//..
//  int usageExample2()
//  {
//      assert(true  == IsTraversable<MyTraversable>::VALUE);
//      assert(false == IsTraversable<WithIterator>::VALUE);
//      assert(false == IsTraversable<int>::VALUE);
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace BloombergLP {
namespace bslmf {

                        // =======================
                        // class template VoidType
                        // =======================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
template <class ...>
#else
template <class T1  = void, class T2  = void, class T3  = void,
          class T4  = void, class T5  = void, class T6  = void,
          class T7  = void, class T8  = void, class T9  = void,
          class T10 = void, class T11 = void, class T12 = void,
          class T13 = void, class T14 = void>
#endif
struct VoidType {
    // Metafunction that always yields 'type' 'void' for any well-formed list
    // of type parameters.  This metafunction is useful when using SFINAE to
    // probe for well-formed types.

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
}

# define BSLMF_VOIDTYPE(...) bsl::void_t<__VA_ARGS__>
#endif

#if !defined(BSLMF_VOIDTYPE)
# define BSLMF_VOIDTYPE(...)                                    \
    typename BloombergLP::bslmf::VoidType<__VA_ARGS__>::type
#endif

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
