// bslstl_isconvertibletostringview.h                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_ISCONVERTIBLETOSTRINGVIEW
#define INCLUDED_BSLSTL_ISCONVERTIBLETOSTRINGVIEW

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for types convertible to string_view.
//
//@CLASSES:
//  bslstl::IsConvertibleToStringView: meta-function for string_view conversion
//
//@SEE_ALSO: bslmf_isconvertible, bslstl_isconvertibletocstring,
//           bslstl_stringview
//
//@DESCRIPTION: This component provides a meta-function,
// `bslstl::IsConvertibleToStringView`, that may be used to query (at
// compile-time) whether a type is convertible to a
// `bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>`.
//
// `bslstl::IsConvertibleToStringView` meets the requirements of the
// `UnaryTypeTrait` concept defined in the C++ standard [meta.rqmts] with a
// base characteristic of `bsl::true_type` if the (template parameter) `TYPE`
// is convertible to `bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>`, and a
// base characteristic of `bsl::false_type` otherwise.
//
// This trait is particularly useful for implementing functions that accept
// "string-view-like" types, as specified in C++17 and later standards.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Determine If a Type Is Convertible to a String View
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine whether various types are convertible to
// `bsl::string_view`.
//
// First, we note that `bsl::string_view` itself is convertible:
// ```
// assert((bslstl::IsConvertibleToStringView<char,
//                                           std::char_traits<char>,
//                                           bsl::string_view>::value));
// ```
// Next, we verify that `const char *` is convertible to string_view:
// ```
// assert((bslstl::IsConvertibleToStringView<char,
//                                           std::char_traits<char>,
//                                           const char *>::value));
// ```
// Finally, we verify that an unrelated type is not convertible:
// ```
// assert((!bslstl::IsConvertibleToStringView<char,
//                                            std::char_traits<char>,
//                                            int>::value));
// ```

#include <bslscm_version.h>

#include <bslstl_stringview.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>

namespace BloombergLP {
namespace bslstl {

                    // ==================================
                    // struct IsConvertibleToStringView
                    // ==================================

/// This `struct` template implements a meta-function to determine if the
/// (template parameter) `t_TYPE` is convertible to
/// `bsl::basic_string_view<t_CHAR_TYPE, t_CHAR_TRAITS>`.
template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_TYPE>
struct IsConvertibleToStringView
: bsl::is_convertible<t_TYPE,
                      bsl::basic_string_view<t_CHAR_TYPE, t_CHAR_TRAITS> > {
};

/// This partial specialization of `IsConvertibleToStringView` is
/// instantiated when `const t_CHAR_TYPE (&)[]` (an unbounded array
/// reference) is tested for convertibility to string view.  This handles
/// edge cases with certain compilers where `bsl::is_convertible` may not
/// correctly handle incomplete array types.
template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
struct IsConvertibleToStringView<t_CHAR_TYPE,
                                 t_CHAR_TRAITS,
                                 const t_CHAR_TYPE (&)[]>
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
