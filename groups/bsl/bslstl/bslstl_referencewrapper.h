// bslstl_referencewrapper.h                                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_REFERENCEWRAPPER
#define INCLUDED_BSLSTL_REFERENCEWRAPPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide copyable, assignable object wrapper for references.
//
//@CLASSES:
//  bsl::reference_wrapper: class object to hold a reference to an object
//
//@DESCRIPTION: This component provides 'bsl::reference_wrapper', a reduced
//  implementation of the standard C++2011 template of the same name, which
//  simply wraps a reference into a copyable, assignable object to allow it to
//  be stored in a place that cannot normally hold a reference, such as a
//  standard container.  Because a reference wrapper is convertible to its
//  contained reference type, it can be passed to functions that take such a
//  reference.
//
//  This component also provides the (free) helper functions 'bsl::ref' and
//  'bsl::cref' that may be used to generate 'reference_wrapper' objects more
//  concisely than with the constructor.
//
//  NOTE: This component is a partial implementation of the standard class,
//  omitting support for use as a function object, and is in any case of
//  limited usefulness in a pure C++98 environment.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Sorted References
/// - - - - - - - - - - - - - -
// Let us suppose that we wish to handle objects that will be passed to a
// comparison function expecting references to the objects.  Let us suppose
// further that these objects are large enough that we would not wish to move
// them around bodily as they are sorted.  Note that plausible examples of uses
// for this component are limited in freestanding C++98.
//
// First, let us define the large-object type:
//..
//  struct Canary {
//      static const int s_size = 1000;
//      Canary *d_values[s_size];
//      Canary();
//  };
//
//  Canary::Canary()
//  {
//       for (int i = 0; i < s_size; ++i) {
//           d_values[i] = this;
//       }
//  }
//..
// Next, we define the comparison function:
//..
//  bool operator<(Canary const& a, Canary const& b)
//  {
//      return a.d_values[0] < b.d_values[0];
//  }
//..
// Finally, we define a generic function to sort two items:
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
// We can call 'sortTwoItems' on wrappers representing 'Canary' objects
// without need to move actual, large 'Canary' objects about.  In the call to
// 'sortTwoItems', below, the 'operator=' used in it is that of
// 'bsl::reference_wrapper<Canary>', but the 'operator<' used is the one
// declared for 'Canary&' arguments.  All of the conversions needed are
// applied implicitly:
//..
//  Canary canaries[2];
//  bsl::reference_wrapper<Canary> canaryA = bsl::ref(canaries[1]);
//  bsl::reference_wrapper<Canary> canaryB = bsl::ref(canaries[0]);
//  sortTwoItems(canaryA, canaryB);
//
//  assert(&canaryA.get() == canaries);
//  assert(&canaryB.get() == canaries + 1);
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
#include <bsls_util.h>  // for BloombergLP::bsls::Util::addressOf
#endif

namespace bsl {

                    // =======================
                    // class reference_wrapper
                    // =======================

template <class T>
class reference_wrapper {
    // This class is a wrapper that encapsulates an object reference, enabling
    // operations not possible on actual references, including assignment,
    // copying, and storage in standard containers.  When stored in a
    // container, it enables functions defined to operate on references to the
    // type represented to be called on the container elements.

  private:
    // DATA
    T *d_represented_p;  // the represented object (not owned)

  public:
    // TYPES
    typedef T type;

    // CREATORS
    reference_wrapper(T& object);    // IMPLICIT
        // Create a reference wrapper representing the specified 'object'.

    // reference_wrapper(const reference_wrapper& original) = default;
        // Create a reference wrapper referring to the same object as the
        // specified 'original'.

    // ~reference_wrapper() = default;
        // Destroy this object.

    // MANIPULATORS
    // reference_wrapper& operator=(const reference_wrapper& rhs) = default;
        // Assign this object to refer to the same object as the specified
        // 'rhs', and return '*this'.

    // ACCESSORS
    T& get() const;
        // Return a reference to the object that '*this' represents.

    operator T&() const;
        // Return a reference to the object that '*this' represents.
};

// FREE FUNCTIONS
template <class T>
reference_wrapper<const T> cref(const T& object);
    // Return a reference wrapper representing a 'const' view of the specified
    // 'object'.

template <class T>
reference_wrapper<const T> cref(reference_wrapper<T> original);
    // Return a reference wrapper representing a 'const' view of the same
    // object as the specified 'original'.

template <class T>
reference_wrapper<T> ref(T& object);
    // Return a reference wrapper that represents the specified 'object'.

template <class T>
reference_wrapper<T> ref(reference_wrapper<T> original);
    // Return a reference wrapper that represents the same object as the
    // specified 'original'.

}  // close namespace bsl

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                    // -----------------------
                    // class reference_wrapper
                    // -----------------------

// CREATORS
template <class T>
inline
bsl::reference_wrapper<T>::reference_wrapper(T& object)
  : d_represented_p(BloombergLP::bsls::Util::addressOf(object))
{
}

// ACCESSORS
template <class T>
inline
T& bsl::reference_wrapper<T>::get() const
{
    return *d_represented_p;
}

template <class T>
inline
bsl::reference_wrapper<T>::operator T&() const
{
    return *d_represented_p;
}

// FREE FUNCTIONS
template <class T>
inline
bsl::reference_wrapper<const T> bsl::cref(const T& object)
{
    return reference_wrapper<const T>(object);
}

template <class T>
inline
bsl::reference_wrapper<const T> bsl::cref(bsl::reference_wrapper<T> original)
{
    return cref(original.get());
}

template <class T>
inline
bsl::reference_wrapper<T> bsl::ref(T& object)
{
    return reference_wrapper<T>(object);
}

template <class T>
inline
bsl::reference_wrapper<T> bsl::ref(bsl::reference_wrapper<T> original)
{
    return ref(original.get());
}

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
