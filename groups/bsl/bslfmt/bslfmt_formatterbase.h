// bslfmt_formatterbase.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERBASE
#define INCLUDED_BSLFMT_FORMATTERBASE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base template for formatter specializations
//
//@CLASSES:
//  bsl::formatter: standard-compliant formatter base template.
//
//@DESCRIPTION: This component provides an base template of the C++20 Standard
// Library's `formatter`, which provides a customization point for use defined
// types seeking to use the formatting library.
//
// It also provides a mechanism, when the standard library `<format>` header is
// available, to enable those partial specializations to be forwarded to the
// `std` namespace to enable use of `std::format` as well as `bsl::format`
//
///User-provided formatters
///------------------------
//
// User-provided formatters are supported by the BSL implementation, just as
// they are by the standard library implementation. However, in order for them
// to be compatible with both implementations, there are specific requirements,
// notably:
//
// - If you will define a formatter for your type 'T', do so in the same
//   component header that defines 'T' itself.  This avoids issues due to
//   users forgetting to include the header for the formatter.
// - Define `bsl::formatter<T>` - *DO NOT* define `std::formatter<T>` - Use
//   template arguments for the format context and parse context
//   parameters. This is essential as the parameter type passed in will
//   depend upon underlying implementation.
// - The `parse` function should be constexpr in C++20, but this is not
//   required (and may not be possible) for earlier C++ standards.
//
// An example of a user defined formatter is as follows:
//
// ```
// namespace bsl {
//
// template <class t_CHAR> struct formatter<UserDefinedType, t_CHAR> {
//     template <class t_PARSE_CONTEXT>
//     BSLS_KEYWORD_CONSTEXPR_CPP20
//     t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& pc)
//     {
//         // implementation goes here
//     }
//
//     template <class t_FORMAT_CONTEXT>
//     t_FORMAT_CONTEXT::iterator format(UserDefinedType   s,
//                                       t_FORMAT_CONTEXT& ctx) const
//     {
//         // implementation goes here
//     }
// };
//
// }  // close namespace bsl
// ```

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20                      \
    typedef void Formatter_PreventStdPromotion
#else
// On earlier C++ compilers we use a dummy typedef to avoid the compiler
// warning about extra semicolons.
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20                      \
    typedef void Formatter_DoNotPreventStdPromotion_DummyTypedef
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP23                      \
    typedef void Formatter_PreventStdPromotion
#else
// On earlier C++ compilers we use a dummy typedef to avoid the compiler
// warning about extra semicolons.
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP23                      \
    typedef void Formatter_DoNotPreventStdPromotion_DummyTypedef
#endif


namespace bsl {
template <class t_ARG, class t_CHAR = char>
struct formatter {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};
}  // close namespace bsl

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace BloombergLP {
namespace bslfmt {

template <class t_FORMATTER, class = void>
struct Formatter_IsStdAliasingEnabled : bsl::true_type {
};

template <class t_FORMATTER>
struct Formatter_IsStdAliasingEnabled<
    t_FORMATTER,
    typename t_FORMATTER::Formatter_PreventStdPromotion> : bsl::false_type {
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace std {

template <class t_ARG, class t_CHAR>
struct formatter;

template <class t_ARG, class t_CHAR>
requires(
    BloombergLP::bslfmt::Formatter_IsStdAliasingEnabled<
        bsl::formatter<t_ARG, t_CHAR> >::value
)
struct formatter<t_ARG, t_CHAR>
: bsl::formatter<t_ARG, t_CHAR> {};

}  // close namespace std
#endif


#endif  // INCLUDED_BSLFMT_FORMATTERBASE

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
