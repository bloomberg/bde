// bslstl_isconvertibletocstring.h                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_ISCONVERTIBLETOCSTRING
#define INCLUDED_BSLSTL_ISCONVERTIBLETOCSTRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for types convertible to C-string.
//
//@CLASSES:
//  bslstl::IsConvertibleToCString: meta-function for C-string convertibility
//
//@SEE_ALSO: bslmf_isconvertible, bslstl_isconvertibletostringview
//
//@DESCRIPTION: This component provides a meta-function,
// `bslstl::IsConvertibleToCString`, that may be used to query (at
// compile-time) whether a type is convertible to a C-string (i.e.,
// `const CHAR_TYPE *`).
//
// `bslstl::IsConvertibleToCString` meets the requirements of the
// `UnaryTypeTrait` concept defined in the C++ standard [meta.rqmts] with a
// base characteristic of `bsl::true_type` if the (template parameter) `TYPE`
// is convertible to `const CHAR_TYPE *`, and a base characteristic of
// `bsl::false_type` otherwise.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Determine If a Type Is Convertible to a C-String
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine whether various types are convertible to
// C-strings (null-terminated character arrays).
//
// First, we create a type that is convertible to `const char *`:
// ```
// struct MyCStringWrapper {
//     const char *d_str;
//     operator const char *() const { return d_str; }
// };
// ```
// Now, we instantiate the `bslstl::IsConvertibleToCString` template for
// various types and verify the results:
// ```
// assert( (bslstl::IsConvertibleToCString<char, const char *>::value));
// assert( (bslstl::IsConvertibleToCString<char, MyCStringWrapper>::value));
// assert(!(bslstl::IsConvertibleToCString<char, int>::value));
// assert(!(bslstl::IsConvertibleToCString<char, void *>::value));
// ```

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>

namespace BloombergLP {
namespace bslstl {

                      // ==============================
                      // struct IsConvertibleToCString
                      // ==============================

/// This `struct` template implements a meta-function to determine if the
/// (template parameter) `t_TYPE` is convertible to `const t_CHAR_TYPE *`.
template <class t_CHAR_TYPE, class t_TYPE>
struct IsConvertibleToCString
: bsl::is_convertible<t_TYPE, const t_CHAR_TYPE *> {
};

/// This partial specialization of `IsConvertibleToCString` is instantiated
/// when `const t_CHAR_TYPE (&)[]` (an unbounded array reference) is tested
/// for convertibility to `const t_CHAR_TYPE *`.  This handles edge cases
/// with certain compilers where `bsl::is_convertible` may not correctly
/// handle incomplete array types.
template <class t_CHAR_TYPE>
struct IsConvertibleToCString<t_CHAR_TYPE, const t_CHAR_TYPE (&)[]>
: bsl::true_type {
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
