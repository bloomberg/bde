// bslstl_referencewrapper.h                                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_REFERENCEWRAPPER
#define INCLUDED_BSLSTL_REFERENCEWRAPPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Wrap a reference in a copyable, assignable object.
//
//@CLASSES:
//  bsl::reference_wrapper: A class object to wrap a reference
//
//@DESCRIPTION: This component provides 'bsl::reference_wrapper', a reduced
//  implementation of the standard C++2011 template of the same name, which
//  simply wraps a reference into a copyable, assignable object to allow it to
//  be stored in a place that cannot normally hold a reference, such as a
//  standard container.  Because it is convertible to its contained reference
//  type, it can be passed to functions that take such a reference.
//
//  This component also provides helper functions 'bsl::ref' and 'bsl::cref'
//  that may be used to generate reference_wrapper objects more concisely than
//  with the constructor.
//
//  NOTE: This component is a partial implementation that does not support its
//  use as a function object, and is in any case of limited usefulness in a
//  pure C++98 environment.
//
///Usage
///-----
// This is a section meant to illustrate a conceivable use of this component.
//
///Example 1: Sorted references
/// - - - - - - - - - - - - - -
// Let us suppose that we wish to handle objects that will be passed to a
// comparison function expecting references to the objects.  Let us suppose
// further that these objects are large enough that we would not wish to move
// them around bodily as they are sorted. (Please note that plausible examples
// of uses for this component are limited in C++98, particularly so when given
// that the example may not depend on other library components.)
//
// First, let us define the large object:
//..
//  struct Canary {
//      int d_values[1000];
//
//      explicit Canary(int values);
//  };
//
//  Canary::Canary(int values)
//  {
//       for (int i = 0; i < 1000; ++i) {
//           d_values[i] = values;
//       }
//  }
//..
// the comparison function:
//..
//  bool operator<(Canary const& a, Canary const& b)
//  {
//     return a.d_values[0] < b.d_values[0];
//  }
//..
// and a function to sort two items:
//..
//  template <typename T>
//  void sort_two_things(T& a, T& b)
//  {
//      if (b < a) {
//          T tmp(a);
//          a = b;
//          b = tmp;
//      }
//  }
//..
// We can call 'sort_two_things()' on wrappers representing Canary objects
// without need to move actual, large 'Canary' objects about. In the call to
// 'sort_two_things()', below, the 'operator=' used in it is that of
// 'bsl::reference_wrapper<Canary>', but the 'operator<' used is the one
// declared for 'Canary&' objects.  All of the conversions needed are applied
// implicitly.
//..
//  Canary two(2);
//  Canary one(1);
//  bsl::reference_wrapper<Canary> canaryA = bsd::ref(two);
//  bsl::reference_wrapper<Canary> canaryA = bsd::ref(one);
//  sort_two_things(canaryA, canaryB);
//
//  assert(&canaryA.get() == &one);
//  assert(&canaryB.get() == &two);
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_referencewrapper.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>                               // for BSLS_UTIL_ADDRESSOF
#endif

namespace bsl {

template <typename T>
class reference_wrapper {

  private:
    T* d_pointer;

  public:
    // PUBLIC TYPES
    typedef T type;

    // CREATORS
    reference_wrapper(T& object);
        // Create a reference wrapper capturing a pointer to the object
        // presumed to be referred to by the specified reference 'object'.
        // Note that this is an implicitly converting constructor.

    reference_wrapper(const reference_wrapper& original);
        // Create a reference wrapper that refers to the same presumed object
        // as the specified 'original'.

    reference_wrapper& operator=(const reference_wrapper& rhs);
        // Assign '*this' to refer to the same object as the specified 'rhs'.
        // Return '*this'.

    // ACCESSORS
    T& get() const;
        // Return a reference to the presumed object previously passed to the
        // constructor or assignment operator of this object.

    operator T&() const;
        // Return 'this->get()', so that objects of this type may be passed to
        // arguments declared as 'T&'.
};

// FREE FUNCTIONS

template <typename T>
reference_wrapper<const T> cref(const T& object);

template <typename T>
reference_wrapper<const T> cref(reference_wrapper<T> original);
    // Return a wrapper representing the const T object presumed to be referred
    // to by the specified 'object' or 'original'.

template <typename T>
reference_wrapper<T> ref(T& object);

template <typename T>
reference_wrapper<T> ref(reference_wrapper<T> original);
    // Return a wrapper representing the T object presumed to be referred to by
    // the specified 'object' or 'original'.

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

    // reference_wrapper<T> members

template <typename T>
inline reference_wrapper<T>::reference_wrapper(T& object)
: d_pointer(BSLS_UTIL_ADDRESSOF(object))
{
}

template <typename T>
inline reference_wrapper<T>::reference_wrapper(
                                          const reference_wrapper<T>& original)
: d_pointer(original.d_pointer)
{
}

template <typename T>
inline reference_wrapper<T>& reference_wrapper<T>::operator=(
                                               const reference_wrapper<T>& rhs)
{
    d_pointer = rhs.d_pointer;
    return *this;
}

template <typename T>
inline reference_wrapper<T>::operator T&() const
{
    return *d_pointer;
}

template <typename T>
inline T& reference_wrapper<T>::get() const
{
    return *d_pointer;
}

template <typename T>
inline reference_wrapper<const T> cref(const T& object)
{
    return reference_wrapper<const T>(object);
}

    // free functions cref, ref

template <typename T>
inline reference_wrapper<const T> cref(reference_wrapper<T> original)
{
    return reference_wrapper<const T>(*original.get());
}

template <typename T>
inline reference_wrapper<T> ref(T& object)
{
    return reference_wrapper<T>(object);
}

template <typename T>
inline reference_wrapper<T> ref(reference_wrapper<T> original)
{
    return reference_wrapper<T>(original);
}

   // Preparing to close namespace bsl...

}  // close namespace bsl

   // Successfully closed namespace bsl.

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
