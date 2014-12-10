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
//  bsl::reference_wrapper: A class object to hold a reference to an object
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
// This section illustrates intended use of this component.
//
///Example 1: Sorted references
/// - - - - - - - - - - - - - -
// Let us suppose that we wish to handle objects that will be passed to a
// comparison function expecting references to the objects.  Let us suppose
// further that these objects are large enough that we would not wish to move
// them around bodily as they are sorted. Note that plausible examples of uses
// for this component are limited in freestanding C++98.
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
// Next, the comparison function:
//..
//  bool operator<(Canary const& a, Canary const& b)
//  {
//     return a.d_values[0] < b.d_values[0];
//  }
//..
// Finally, a generic function to sort two items:
//..
//  template <typename T>
//  void sortTwoItems(T& a, T& b)
//  {
//      if (b < a) {
//          T tmp(a);
//          a = b;
//          b = tmp;
//      }
//  }
//..
// We can call 'sortTwoItems()' on wrappers representing Canary objects without
// need to move actual, large 'Canary' objects about. In the call to
// 'sortTwoItems()', below, the 'operator=' used in it is that of
// 'bsl::reference_wrapper<Canary>', but the 'operator<' used is the one
// declared for 'Canary&' objects.  All of the conversions needed are applied
// implicitly.
//..
//  Canary two(2);
//  Canary one(1);
//  bsl::reference_wrapper<Canary> canaryA = bsd::ref(two);
//  bsl::reference_wrapper<Canary> canaryA = bsd::ref(one);
//  sortTwoItems(canaryA, canaryB);
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

                    // =======================
                    // class reference_wrapper
                    // =======================

template <typename REFERENCED_TYPE>
class reference_wrapper {
    // This class is a wrapper that encapsulates an object reference, enabling
    // operations not possible on actual references, including assignment,
    // copying, and storage in standard containers.  When stored in a
    // container, it enables functions defined to operate on references to the
    // type represented to be called on the container elements.

  private:
    // DATA
    REFERENCED_TYPE* d_represented_p;  // (not owned)

  public:
    // TYPES
    typedef REFERENCED_TYPE type;

    // CREATORS
    reference_wrapper(REFERENCED_TYPE& object);    // IMPLICIT
        // Create a reference wrapper representing the specified 'object'.

    reference_wrapper(const reference_wrapper& original);
        // Create a reference_wrapper object having the same value as the
        // specified 'original' object.

    // ~reference_wrapper() = default;
        // Destroy this object.

    // MANIPULATORS
    reference_wrapper& operator=(const reference_wrapper& rhs);
        // Assign this object to have the same value as the specified 'rhs'.
        // Return '*this'.

    // ACCESSORS
    REFERENCED_TYPE& get() const;
        // Return a reference to the object that '*this' represents.

    operator REFERENCED_TYPE&() const;
        // Return a reference to the object that '*this' represents.
};

// FREE FUNCTIONS

template <typename REFERENCED_TYPE>
reference_wrapper<const REFERENCED_TYPE> cref(const REFERENCED_TYPE& object);
    // Return a reference wrapper representing a 'const' view of the specified
    // 'object'.

template <typename REFERENCED_TYPE>
reference_wrapper<const REFERENCED_TYPE> cref(
                                  reference_wrapper<REFERENCED_TYPE> original);
    // Return a reference wrapper representing a 'const' view of the same
    // object as the specified 'original'.

template <typename REFERENCED_TYPE>
reference_wrapper<REFERENCED_TYPE> ref(REFERENCED_TYPE& object);
    // Return a reference wrapper that represents the specified 'object'.

template <typename REFERENCED_TYPE>
reference_wrapper<REFERENCED_TYPE> ref(
                                  reference_wrapper<REFERENCED_TYPE> original);
    // Return a reference wrapper that represents the same object as the
    // specified 'original'.

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                    // =======================
                    // class reference_wrapper
                    // =======================

    // reference_wrapper members

// CREATORS
template <typename REFERENCED_TYPE>
inline reference_wrapper<REFERENCED_TYPE>::reference_wrapper(
                                                       REFERENCED_TYPE& object)
: d_represented_p(BSLS_UTIL_ADDRESSOF(object))
{
}

template <typename REFERENCED_TYPE>
inline reference_wrapper<REFERENCED_TYPE>::reference_wrapper(
                            const reference_wrapper<REFERENCED_TYPE>& original)
: d_represented_p(original.d_represented_p)
{
}

// MANIPULATORS
template <typename REFERENCED_TYPE>
inline reference_wrapper<REFERENCED_TYPE>&
reference_wrapper<REFERENCED_TYPE>::operator=(
                                 const reference_wrapper<REFERENCED_TYPE>& rhs)
{
    d_represented_p = rhs.d_represented_p;
    return *this;
}

// ACCESSORS
template <typename REFERENCED_TYPE>
inline REFERENCED_TYPE& reference_wrapper<REFERENCED_TYPE>::get() const
{
    return *d_represented_p;
}

template <typename REFERENCED_TYPE>
inline reference_wrapper<REFERENCED_TYPE>::operator REFERENCED_TYPE&() const
{
    return *d_represented_p;
}

// FREE FUNCTIONS
template <typename REFERENCED_TYPE>
inline reference_wrapper<const REFERENCED_TYPE> cref(
                                                 const REFERENCED_TYPE& object)
{
    return reference_wrapper<const REFERENCED_TYPE>(object);
}

template <typename REFERENCED_TYPE>
inline reference_wrapper<const REFERENCED_TYPE> cref(
                                   reference_wrapper<REFERENCED_TYPE> original)
{
    return reference_wrapper<const REFERENCED_TYPE>(*original.get());
}

template <typename REFERENCED_TYPE>
inline reference_wrapper<REFERENCED_TYPE> ref(REFERENCED_TYPE& object)
{
    return reference_wrapper<REFERENCED_TYPE>(object);
}

template <typename REFERENCED_TYPE>
inline reference_wrapper<REFERENCED_TYPE> ref(
                                   reference_wrapper<REFERENCED_TYPE> original)
{
    return reference_wrapper<REFERENCED_TYPE>(original);
}

}  // close namespace bsl

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
