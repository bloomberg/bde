// bslfmt_formatterstring.h                                           -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSTRING
#define INCLUDED_BSLFMT_FORMATTERSTRING

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

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
#include <bslfmt_formatterbase.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

namespace BloombergLP {
namespace bslfmt {

template <class t_VALUE, class t_CHAR>
struct Formatter_StringBase {
  public:
    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bsl::format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(t_VALUE           v,
                                               t_FORMAT_CONTEXT& fc) const
    {
        bsl::basic_string_view<t_CHAR> sv(v);
        return bsl::copy(sv.begin(), sv.end(), fc.out());
    }
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

template <class t_CHAR>
struct formatter<const t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::Formatter_StringBase<const t_CHAR *, t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_CHAR>
struct formatter<std::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::Formatter_StringBase<std::basic_string_view<t_CHAR>,
                                            t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a `std`
    // string view, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;
};
#endif

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

template <class t_CHAR>
struct formatter<bsl::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::Formatter_StringBase<bsl::basic_string_view<t_CHAR>,
                                            t_CHAR> {
  public:
    // TRAITS

    // If we do not alias string view then we should alias its formatter from
    // the `std` namespace, so DO NOT DEFINE the trait
    // BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20.
};

#endif

}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace std {
// FORMATTER SPECIALIZATIONS

/// This is a customisation of `std::formatter` for `bsl::string` and
/// `bsl::wstring`. We cannot rely on the automatic promotion mechanism because
/// we do not define bsl formatters for `bsl::string` or `bsl::wstring` (on the
/// basis that the `string_view` formatter is used instead).
template <class t_CHAR>
struct formatter<bsl::basic_string<t_CHAR>, t_CHAR>
: bsl::formatter<bsl::basic_string_view<t_CHAR>, t_CHAR> {
};
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
