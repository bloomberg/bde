// bslstl_referencewrapper.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_REFERENCE_WRAPPER
#define INCLUDED_BSLSTL_REFERENCE_WRAPPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Wrap a reference in a copyable, assignable object
//
//@CLASSES:
//  bsl::reference_wrapper<T>
//
//@DESCRIPTION:
//  reference_wrapper simply wraps a reference into a copyable, assignable
//  object to allow it to be stored in a place that cannot normally hold a
//  reference, such as a standard container.  Because it is convertible to
//  T&, it can be used with functions that take a T&.
//
//  Helper functions bsl::ref and bsl::cref may be used to generate
//  reference_wrapper objects more concisely than with the constructor.
//
//  NOTE: This component is a partial implementation that does not support its
//  use as a function object.  Do not try to use it where operator() would be
//  called on it.
//

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
#include <bsls_util.h>
#endif

namespace bsl {

template <typename T>
class reference_wrapper
{
  public:
    // PUBLIC TYPES

    typedef T type;

    // CREATORS

    reference_wrapper(T& t);
    reference_wrapper(const reference_wrapper& t);
        // Wrap a reference to specified argument 't'.

    // ASSIGNMENT

    reference_wrapper& operator=(const reference_wrapper& ref);
        // Return a copy of the specified 'ref'.

    // ACCESSORS

    operator T&() const;
        // implicitly convert '*this' to a reference to T.

    T& get() const;
        // Return the reference contructed or assigned to '*this'.


  private:
    T* d_p;

#if 201103L <= __cplusplus
    reference_wrapper(reference_wrapper&&) = delete;
#endif
};

    // The following helper functions 'ref()' and 'cref()' each return a
    // wrapped reference to the specified argument 't'.  They are intended as
    // abbreviations, solely for convenience in writing and reading code that
    // uses reference wrappers.

template <typename T>
reference_wrapper<T> ref(T& t);

template <typename T>
reference_wrapper<T> ref(reference_wrapper<T> t);

template <typename T>
reference_wrapper<const T> cref(const T& t);

template <typename T>
reference_wrapper<const T> cref(reference_wrapper<T> t);

#if 201103L <= __cplusplus
template <typename T>
void ref(T&&) = delete;
template <typename T>
void cref(const T&&) = delete;
#endif


}  // namespace bsl

//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace bsl {

    // reference_wrapper<T> members

template <typename T>
inline reference_wrapper<T>::reference_wrapper(T& t)
: d_p(BSLS_UTIL_ADDRESSOF(t)) {}

template <typename T>
inline reference_wrapper<T>::reference_wrapper(const reference_wrapper<T>& ref)
: d_p(ref.d_p) {}

template <typename T>
inline reference_wrapper<T>& reference_wrapper<T>::operator=(
                                          const reference_wrapper<T>& ref)
{
    d_p = ref.d_p;
    return *this;
}

template <typename T>
inline reference_wrapper<T>::operator T&() const
{ return *d_p; }

template <typename T>
inline T& reference_wrapper<T>::get() const
{ return *d_p; }

    // ref(), cref()

template <typename T>
inline reference_wrapper<T> ref(T& t)
{ return reference_wrapper<T>(t); }

template <typename T>
inline reference_wrapper<T> ref(reference_wrapper<T> t)
{ return reference_wrapper<T>(t); }

template <typename T>
inline reference_wrapper<const T> cref(const T& t)
{ return reference_wrapper<const T>(t); }

template <typename T>
inline reference_wrapper<const T> cref(reference_wrapper<T> t)
{ return reference_wrapper<const T>(*t.get()); }

}  // namespace bsl

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
