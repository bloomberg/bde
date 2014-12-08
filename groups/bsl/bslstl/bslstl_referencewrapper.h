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
//  bsl::reference_wrapper: A wrapper for a reference
//
//@DESCRIPTION: 'bsl::reference_wrapper' simply wraps a reference into a
//  copyable, assignable object to allow it to be stored in a place that cannot
//  normally hold a reference, such as a standard container.  Because it is
//  convertible to TYPENAME&, it can be used with functions that take a
//  TYPENAME&.
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

template <class TYPENAME>
class reference_wrapper
{
  public:
    // PUBLIC TYPES

    typedef TYPENAME type;

    // CREATORS

        // BDE_VERIFY pragma: -IC01 // This really is a conversion.
    reference_wrapper(TYPENAME& t);
        // Wrap a reference to specified argument 't'.

    reference_wrapper(const reference_wrapper& ref);
        // Copy specified wrapper 'ref'

    // ASSIGNMENT

    reference_wrapper& operator=(const reference_wrapper& ref);
        // Return a copy of the specified 'ref'.

    // ACCESSORS

    operator TYPENAME&() const;
        // implicitly convert '*this' to a reference to TYPENAME.

    TYPENAME& get() const;
        // Return the reference contructed or assigned to '*this'.


  private:
    TYPENAME* d_p;
};

    // The following helper functions 'ref()' and 'cref()' each return a
    // wrapped reference to the specified argument 't'.  They are intended as
    // abbreviations, solely for convenience in writing and reading code that
    // uses reference wrappers.

    // REF

template <class TYPENAME>
reference_wrapper<TYPENAME> ref(TYPENAME& t);
    // Return a wrapper of the specified 't'.

template <class TYPENAME>
reference_wrapper<TYPENAME> ref(reference_wrapper<TYPENAME> t);
    // Return a wrapper of the specified 't'.

    // CREF

template <class TYPENAME>
reference_wrapper<const TYPENAME> cref(const TYPENAME& t);
    // Return a wrapper of the specified 't'.

template <class TYPENAME>
reference_wrapper<const TYPENAME> cref(reference_wrapper<TYPENAME> t);
    // Return a wrapper of the specified 't'.

   // preparing to close namespace bsl, get ready...

}  // close namespace bsl

   // successfully closed namespace bsl, success!

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

namespace bsl {

    // reference_wrapper<TYPENAME> members

template <class TYPENAME>
inline reference_wrapper<TYPENAME>::reference_wrapper(TYPENAME& t)
: d_p(BSLS_UTIL_ADDRESSOF(t)) {}

template <class TYPENAME>
inline reference_wrapper<TYPENAME>::reference_wrapper(
                                        const reference_wrapper<TYPENAME>& ref)
: d_p(ref.d_p) {}

template <class TYPENAME>
inline reference_wrapper<TYPENAME>& reference_wrapper<TYPENAME>::operator=(
                                        const reference_wrapper<TYPENAME>& ref)
{
    d_p = ref.d_p;
    return *this;
}

template <class TYPENAME>
inline reference_wrapper<TYPENAME>::operator TYPENAME&() const
{ return *d_p; }

template <class TYPENAME>
inline TYPENAME& reference_wrapper<TYPENAME>::get() const
{ return *d_p; }

    // REF()

template <class TYPENAME>
inline reference_wrapper<TYPENAME> ref(TYPENAME& t)
{ return reference_wrapper<TYPENAME>(t); }

template <class TYPENAME>
inline reference_wrapper<TYPENAME> ref(reference_wrapper<TYPENAME> t)
{ return reference_wrapper<TYPENAME>(t); }

    // CREF()

template <class TYPENAME>
inline reference_wrapper<const TYPENAME> cref(const TYPENAME& t)
{ return reference_wrapper<const TYPENAME>(t); }

template <class TYPENAME>
inline reference_wrapper<const TYPENAME> cref(reference_wrapper<TYPENAME> t)
{ return reference_wrapper<const TYPENAME>(*t.get()); }

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
