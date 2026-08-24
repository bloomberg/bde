// bslstl_utility.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_UTILITY
#define INCLUDED_BSLSTL_UTILITY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations for utilities not in the system library.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_utility.h
//
//@SEE_ALSO: bsl+bslhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// `<bsl_utility.h>` instead.  This component provides a namespace for free
// functions implementing standard utilities that are not provided by the
// underlying standard library implementation.  For example, `as_const` is a
// C++17 utility, and it is provided here for code using C++03.
//
///Usage
///-----
// This component is for use by the `bsl+bslhdrs` package.  Use
// `bsl_utility.h` directly.

#include <bslscm_version.h>

#include <bslmf_addconst.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <utility>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
using std::as_const;
#else
/// Return a reference offering non-modifiable access to the specified `t`.
template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
typename bsl::add_const<TYPE>::type& as_const(TYPE& t) BSLS_KEYWORD_NOEXCEPT;

# ifdef BSLS_COMPILERFEATURES_FULL_CPP11
template <class TYPE>
void as_const(const TYPE&&) = delete;
# endif  //  C++11
#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
using std::exchange;
#else
# ifdef BSLS_COMPILERFEATURES_FULL_CPP11
/// Assign to the specified `obj` the specified `newValue`, forwarded to the
/// assignment operator of `obj`, and return the value held by `obj` prior to
/// that assignment.
template <class t_TYPE, class t_OTHER_TYPE = t_TYPE>
t_TYPE exchange(t_TYPE& obj, t_OTHER_TYPE&& newValue);
# else
/// Assign to the specified `obj` the specified `newValue`, and return the
/// value held by `obj` prior to that assignment.  Note that these functions
/// support move semantics, simulated using `bslmf::MovableRef`:
/// * The return value is move-constructed from `obj`.
/// * If `t_OTHER_TYPE` is a specialization of `bslmf::MovableRef`, then move
///   assignment to `obj` will occur.
template <class t_TYPE, class t_OTHER_TYPE>
t_TYPE exchange(t_TYPE& obj, const t_OTHER_TYPE& newValue);
template <class t_TYPE, class t_OTHER_TYPE>
t_TYPE exchange(t_TYPE& obj, t_OTHER_TYPE&       newValue);
# endif  // C++11
#endif  // C++14

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
typename bsl::add_const<TYPE>::type& as_const(TYPE& t) BSLS_KEYWORD_NOEXCEPT
{
    return t;
}
#endif

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
# ifdef BSLS_COMPILERFEATURES_FULL_CPP11
template <class t_TYPE, class t_OTHER_TYPE>
t_TYPE exchange(t_TYPE& obj, t_OTHER_TYPE&& newValue)
{
    t_TYPE oldValue = static_cast<t_TYPE&&>(obj);
    obj = static_cast<t_OTHER_TYPE&&>(newValue);
    return oldValue;
}
# else
}  // close namespace bsl
namespace BloombergLP {
namespace bslstl {
/// This component-private class template is used to implement `bsl::exchange`
/// in C++03.  It performs an assignment in its destructor.  See the
/// implementation notes in the .cpp file for more information.
template <class t_TYPE, class t_OTHER_TYPE>
struct Utility_ExchangeAssignOnDestruction {
    // DATA

    t_TYPE&       d_obj;       // left side for assignment
    t_OTHER_TYPE& d_newValue;  // right side for assignment

    // CREATORS

    /// Create a `Utility_ExchangeAssignOnDestruction` object that, on
    /// destruction, will assign the specified `newValue` to the specified
    /// `obj`.
    Utility_ExchangeAssignOnDestruction(t_TYPE& obj, t_OTHER_TYPE& newValue)
    : d_obj(obj), d_newValue(newValue) {}

    /// Destroy this object, performing the promised assignment.
    ~Utility_ExchangeAssignOnDestruction()
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    {
        d_obj = d_newValue;
    }
};
}  // close package bslstl
}  // close enterprise namespace
namespace bsl {
template <class t_TYPE, class t_OTHER_TYPE>
t_TYPE exchange(t_TYPE& obj, const t_OTHER_TYPE& newValue)
{
    BloombergLP::bslstl::
        Utility_ExchangeAssignOnDestruction<t_TYPE, const t_OTHER_TYPE>
            a(obj, newValue);
    return t_TYPE(BloombergLP::bslmf::MovableRefUtil::move(obj));
}

template <class t_TYPE, class t_OTHER_TYPE>
t_TYPE exchange(t_TYPE& obj, t_OTHER_TYPE& newValue)
{
    BloombergLP::bslstl::Utility_ExchangeAssignOnDestruction<t_TYPE,
                                                             t_OTHER_TYPE>
        a(obj, newValue);
    return t_TYPE(BloombergLP::bslmf::MovableRefUtil::move(obj));
}
# endif  // C++11
#endif  // C++14
}  // close namespace bsl

#endif  // INCLUDED_BSLSTL_UTILITY

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
