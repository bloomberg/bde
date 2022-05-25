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
// '<bsl_utility.h>' instead.  This component provides a namespace for free
// functions implementing standard utilities that are not provided by the
// underlying standard library implementation.  For example, 'as_const' is a
// C++17 utility, and it is provided here for code using C++03.
//
///Usage
///-----
// This component is for use by the 'bsl+bslhdrs' package.  Use
// 'bsl_utility.h' directly.

#include <bslscm_version.h>

#include <bslmf_addconst.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <utility>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::as_const;
#else
template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
typename bsl::add_const<TYPE>::type& as_const(TYPE& t) BSLS_KEYWORD_NOEXCEPT
    // return a reference offering non-modifiable access to the specified 't'.
{
    return t;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#  error "compiler supports rvalue references but not deleted functions"
# endif

template <class TYPE>
void as_const(const TYPE&&) = delete;
# endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

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
