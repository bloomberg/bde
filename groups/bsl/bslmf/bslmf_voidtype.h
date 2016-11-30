// bslmf_voidtype.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_VOIDTYPE
#define INCLUDED_BSLMF_VOIDTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a helper for implementing SFINAE-based metafunctions.
//
//@CLASSES: bslmf::VoidType<T>:  Helper class for SFINAE-based metafunctions
//
//@SEE_ALSO: bslmf::ResultType
//
//@DESCRIPTION:  This component provides a trivial metafunction, 'VoidType',
// that takes up to 14 type parameters. The template always yields a
// member type, 'type', which is always 'void'. The usefulness of this
// do-nothing metafunction is that, when it is instantiated, all of its
// template parameters must be valid. By putting the template instantiation in
// a SFINAE context, any use of template parameters that name invalid
// dependent types are discareded by the compiler as non-viable. Thus,
// 'VoidType' is most commonly used to build metafunctions that test for the
// existance of a specific nested data type (see usage example, below).
//
// The 'bslmf::VoidType' class template is intended to provide identical
// functionality as the C++14 metafunction 'std::void_t', but without using
// C++11 alias templates. A use, in C++14-compliant code, of
//..
//  'std::void_t<T1, T2, ...>'
//..
// can be replaced, in BSL-compliant code using any version of standard C++,
// by
//..
//  'typename bslmf::VoidType<T1, T2, ...>::type'
//..
//
///Usage
///-----
//
///Usage Example 1
///- - - - - - - -
// In this example, we demonstrate the use of 'VoidType' to determine whether
// a given type 'T' as a member type 'T::iterator'. Our goal is to create a
// metafunction, 'HasIteratorType' such that 'HasIteratorType<T>::VALUE' is
// 'true' if 'T::iterator' is a valid type and 'false' otherwise.  This
// example is adapted from the paper proposing 'std::void_t' for the C++
// Standard, N3911.
//
// First, we define the base-case metafunction, which returns 'false':
//..
//  template <class T, class = void>
//  struct HasIteratorType {
//      enum { VALUE = false };
//  };
//..
// Now we create a partial specialization that uses 'VoidType' to probe for
// 'T::iterator'.
//..
//  template <class T>
//  struct HasIteratorType<T,
//                      typename bslmf::VoidType<typename T::iterator>::type> {
//      enum { VALUE = true };
//  };
//..
// To see how this works, we define a class that has a 'iterator' member and
// apply 'HasIteratorType' to it:
//..
//  struct WithIterator {
//      typedef short *iterator;
//  };
//
//  int main() {
//      assert(true == HasIteratorType<WithIterator>::VALUE);
//..
// Since 'WithIterator::iterator' is a valid type,
// 'VoidType<WithIterator::iterator>::type' will be 'void' and the second
// 'HasIteratorType' template will be more specialized than the primary
// template and will thus get instantiated, yielding a 'VALUE' of 'true'.
//
// Conversely, if we try to instantiate 'HasIteratorType<int>', any use of
// 'VoidType<int::iterator>::type' will result in a substitution
// failure. Fortunately, the Substitution Failure Is Not An Error (SFINAE)
// rule applies, so the code will compile, but the specialization is
// eliminated from consideration, resulting in the primary template being
// instantiated and yielding a 'VALUE' of 'FALSE':
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
// type at once.  As in the previous example, we are defining a
// metafunction. We'll define 'IsTraversable<T>::VALUE' to be true if
// 'T::iterator' and 'T::value_type' both exist. As before, we start with a
// primary template that always yields 'false':
//..
//  template <class T, class = void>
//  struct IsTraversable {
//      enum { VALUE = false };
//  };
//..
// This time, we create a partial specialization that uses 'VoidType' with two
// parameters:
//..
//  template <class T>
//  struct IsTraversable<T,
//                       typename bslmf::VoidType<typename T::iterator,
//                                                typename T::value_type
//                                               >::type> {
//      enum { VALUE = true };
//  };
//..
// Next we define a type that meets the requirement for being traversable:
//..
//  struct MyTraversable {
//      typedef int  value_type;
//      typedef int *iterator;
//  };
//..
// The 'IsTraversable' metafunction yields 'true' for 'Traversable' but not
// for 'WithIterator', which lacks 'value_type' or 'int', which lacks both
// 'iterator' and 'value_type'.
//..
//  int main() {
//      assert(true  == IsTraversable<MyTraversable>::VALUE);
//      assert(false == IsTraversable<WithIterator>::VALUE);
//      assert(false == IsTraversable<int>::VALUE);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // =======================
                        // class template VoidType
                        // =======================

template <class T1  = void, class T2  = void, class T3  = void,
          class T4  = void, class T5  = void, class T6  = void,
          class T7  = void, class T8  = void, class T9  = void,
          class T10 = void, class T11 = void, class T12 = void,
          class T13 = void, class T14 = void>
struct VoidType {
    // Metafunction that allways yields 'type' 'void' for any well-formed list
    // of type parameters. This metafunction is useful when using SFINAE to
    // probe for well-formed types.

    typedef void type;
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_VOIDTYPE)

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
