// bslmf_referencewrapper.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMF_REFERENCEWRAPPER
#define INCLUDED_BSLMF_REFERENCEWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a copyable, assignable object wrapper for references.
//
//@CLASSES:
//  bsl::reference_wrapper: copyable, rebindable reference proxy
//
//@CANONICAL_HEADER: bsl_functional.h
//
//@DESCRIPTION: This component provides `bsl::reference_wrapper`, a copyable,
// rebindable proxy for a reference to an object or function.  As a
// `reference_wrapper` is an object it can be stored in a place that cannot
// normally hold a reference, such as an array or a Standard Library container.
// A `reference_wrapper` is implicitly convertible to its contained reference
// type so that it can be passed to functions that take such a reference.
// Unlike a native C++ reference, which is immutably bound to its target at
// initialization, a `reference_wrapper` may be rebound to refer to a different
// target by assignment from another `reference_wrapper` object.
//
// This component also provides the (free) helper functions `bsl::ref` and
// `bsl::cref` that may be used to generate `reference_wrapper` objects more
// concisely than with the constructor.
//
// NOTE: This component provides only a partial implementation of the standard
// class template before C++11, omitting support for use as a function object.
// Further, the C++ Standard Library uses `std::reference_wrapper` as a special
// vocabulary type for several factory functions such as `std::bind`,
// `std::make_pair`, and `std::make_tuple` to embed true references in the
// created objects.  The `bsl` library does not implement those functions, but
// uses the `std` implementations directly.  Hence, those features are not
// available with our C++03 implementation.
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
// them around bodily as they are sorted.
//
// First, let us define the large-object type:
// ```
// struct Canary {
//     static const int s_size = 1000;
//     Canary *d_values[s_size];
//     Canary();
// };
//
// Canary::Canary()
// {
//      for (int i = 0; i < s_size; ++i) {
//          d_values[i] = this;
//      }
// }
// ```
// Next, we define the comparison function:
// ```
// bool operator<(Canary const& a, Canary const& b)
// {
//     return a.d_values[0] < b.d_values[0];
// }
// ```
// Finally, we define a generic function to sort two items:
// ```
// template <class T>
// void sortTwoItems(T& a, T& b)
// {
//     if (b < a) {
//         T tmp(a);
//         a = b;
//         b = tmp;
//     }
// }
// ```
// We can call `sortTwoItems` on wrappers representing `Canary` objects
// without need to move actual, large `Canary` objects about.  In the call to
// `sortTwoItems`, below, the `operator=` used in it is that of
// `bsl::reference_wrapper<Canary>`, but the `operator<` used is the one
// declared for `Canary&` arguments.  All of the conversions needed are
// applied implicitly:
// ```
// Canary canaries[2];
// bsl::reference_wrapper<Canary> canaryA = bsl::ref(canaries[1]);
// bsl::reference_wrapper<Canary> canaryB = bsl::ref(canaries[0]);
// sortTwoItems(canaryA, canaryB);
//
// assert(&canaryA.get() == canaries);
// assert(&canaryB.get() == canaries + 1);
// ```

#include <bslscm_version.h>

#include <bslmf_isreferencewrapper.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_compilerfeatures.h>
#include <bsls_util.h>  // for BloombergLP::bsls::Util::addressOf

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_isbitwisemoveable.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_FULL_CPP11
# include <functional>
# define BSLMF_REFERENCEWRAPPER_IS_ALIASED

namespace bsl {
using std::cref;
using std::ref;
using std::reference_wrapper;
}  // close enterprise namespace

#else   // C++03 implementation

namespace bsl {

                    // =======================
                    // class reference_wrapper
                    // =======================

/// This class is a copyable, rebindable proxy for a reference to an object
/// or function.  It encapsulates a reference into a value that can be
/// reassigned to refer to a different target and can be stored where a
/// native reference cannot, such as in a Standard Library container.  A
/// `reference_wrapper` is implicitly convertible to a reference to the
/// represented type, so it can be passed to functions expecting such a
/// reference.
template <class T>
class reference_wrapper {

  private:
    // DATA
    T *d_represented_p;  // the represented object (not owned)

  public:
    // TYPES
    typedef T type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(reference_wrapper,
                                   bsl::is_trivially_copyable);

    // CREATORS

    /// Create a reference wrapper representing the specified `object`.
    reference_wrapper(T& object);                                   // IMPLICIT

    /// Create a reference wrapper referring to the same object as the
    /// specified `original`.
    //! reference_wrapper(const reference_wrapper& original) = default;

    /// Destroy this object.
    //! ~reference_wrapper() = default;

    // MANIPULATORS

    /// Assign this object to refer to the same object as the specified
    /// `rhs`, and return `*this`.
    //! reference_wrapper& operator=(const reference_wrapper& rhs) = default;

    // ACCESSORS

    /// Return a reference to the object that `*this` represents.
    T& get() const;

    /// Return a reference to the object that `*this` represents.
    operator T&() const;
};

// FREE FUNCTIONS

/// Return a reference wrapper representing a `const` view of the specified
/// `object`.
template <class T>
reference_wrapper<const T> cref(const T& object);

/// Return a reference wrapper representing a `const` view of the same
/// object as the specified `original`.
template <class T>
reference_wrapper<const T> cref(reference_wrapper<T> original);

/// Return a reference wrapper that represents the specified `object`.
template <class T>
reference_wrapper<T> ref(T& object);

/// Return a reference wrapper that represents the same object as the
/// specified `original`.
template <class T>
reference_wrapper<T> ref(reference_wrapper<T> original);

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

#endif // BSLMF_REFERENCEWRAPPER_IS_ALIASED

namespace BloombergLP {
namespace bslmf {

template <class T>
struct IsReferenceWrapper<bsl::reference_wrapper<T> > : bsl::true_type { };

}  // close namespace bslmf
}  // close enterprise namespace

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
