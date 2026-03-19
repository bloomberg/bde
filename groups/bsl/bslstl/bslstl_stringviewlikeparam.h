// bslstl_stringviewlikeparam.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGVIEWLIKEPARAM
#define INCLUDED_BSLSTL_STRINGVIEWLIKEPARAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros for SFINAE constraints on string-view-like params.
//
//@MACROS:
//  BSLSTL_STRINGVIEWLIKEPARAM_TYPE_IF_COMPLETE: completeness check
//  BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T: SV-convertible enable_if
//  BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T: SV-conv, not C-string
//
//@SEE_ALSO: bslstl_isconvertibletostringview, bslstl_isconvertibletocstring
//
//@DESCRIPTION: This component provides a set of macros that simplify the
// declaration of function template parameters constrained to accept
// "string-view-like" types.  A type is considered "string-view-like" if it is
// convertible to `bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>`.
//
// The "ONLY" variants additionally exclude types that are convertible to
// `const CHAR_TYPE *` (C-strings), which is useful for overload resolution
// when separate overloads exist for C-string arguments.
//
// These macros are designed to be used in function declarations and
// definitions where SFINAE is needed to constrain template parameters.
//
/// Macro Summary
///-------------
// This section provides a brief description of each macro:
//
// - `BSLSTL_STRINGVIEWLIKEPARAM_TYPE_IF_COMPLETE`: Helper macro that
//   provides a completeness check for the `STRING_VIEW_LIKE_TYPE` template
//   parameter, working around SFINAE deficiencies in certain compilers.
//
// - `BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType)`: SFINAE
//   `enable_if` type alias for types convertible to string_view.  The result
//   is `ResultType` if enabled.  Use as:
//   `BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(void) * = 0` for declaration,
//   or `BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(const ALLOCATOR&) alloc =`
//                                                              `ALLOCATOR()`
//
// - `BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)`: SFINAE
//   `enable_if` type alias for types convertible to string_view but NOT
//   convertible to C-string (`const CHAR_TYPE *`).  The result is
//   `ResultType` if enabled.  Use as:
//   `BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(void) * = 0` for
//   declaration, or
//   `BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(const ALLOCATOR&) alloc =`
//                                                              `ALLOCATOR()`
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Declaring a String-View-Like Constructor
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to declare a constructor that accepts any type convertible
// to `bsl::string_view` but not `const char *`.
//
// First, we declare the constructor in the class definition:
// ```
// template <class STRING_VIEW_LIKE_TYPE>
// explicit MyClass(const STRING_VIEW_LIKE_TYPE& value,
//                  BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(void) *= 0);
// ```
// Then, we define the constructor outside the class:
// ```
// template <class STRING_VIEW_LIKE_TYPE>
// MyClass::MyClass(const STRING_VIEW_LIKE_TYPE& value,
//                  BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(void) *)
// {
//     bsl::string_view sv = value;
//     // use sv...
// }
// ```

#include <bslscm_version.h>

#include <bslstl_isconvertibletocstring.h>
#include <bslstl_isconvertibletostringview.h>

#include <bslmf_enableif.h>

#include <bsls_platform.h>

/// We need to use an intermediate completeness test to work around
/// deficiencies with SFINAE in the Sun and AIX compilers.
#define BSLSTL_STRINGVIEWLIKEPARAM_TYPE_IF_COMPLETE                           \
    typename bsl::enable_if<0 != sizeof(STRING_VIEW_LIKE_TYPE),               \
                            const STRING_VIEW_LIKE_TYPE&>::type

/// Expand to an `enable_if` type alias for `ResultType` that is defined
/// (i.e., SFINAE-friendly) only when the `STRING_VIEW_LIKE_TYPE` template
/// parameter is convertible to
/// `bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>`.  The `CHAR_TYPE` and
/// `CHAR_TRAITS` template parameters must be in scope.  The specified
/// `ResultType` (passed as a variadic argument) is the resulting type when
/// the condition is satisfied.
#define BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(...)                           \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::IsConvertibleToStringView<                       \
            CHAR_TYPE,                                                        \
            CHAR_TRAITS,                                                      \
            BSLSTL_STRINGVIEWLIKEPARAM_TYPE_IF_COMPLETE>::value,              \
        __VA_ARGS__>::type

/// Expand to an `enable_if` type alias for `ResultType` that is defined
/// (i.e., SFINAE-friendly) only when the `STRING_VIEW_LIKE_TYPE` template
/// parameter is convertible to
/// `bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>` but is *not*
/// convertible to `const CHAR_TYPE *`.  The `CHAR_TYPE` and `CHAR_TRAITS`
/// template parameters must be in scope.  The specified `ResultType`
/// (passed as a variadic argument) is the resulting type when both
/// conditions are satisfied.
#define BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(...)                      \
    typename bsl::enable_if<                                                  \
        BloombergLP::bslstl::IsConvertibleToStringView<                       \
            CHAR_TYPE,                                                        \
            CHAR_TRAITS,                                                      \
            BSLSTL_STRINGVIEWLIKEPARAM_TYPE_IF_COMPLETE>::value               \
     && !BloombergLP::bslstl::IsConvertibleToCString<                         \
            CHAR_TYPE,                                                        \
            BSLSTL_STRINGVIEWLIKEPARAM_TYPE_IF_COMPLETE>::value,              \
        __VA_ARGS__>::type

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
