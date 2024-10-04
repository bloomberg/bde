// bslfmt_formatterbase.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERBOOL
#define INCLUDED_BSLFMT_FORMATTERBOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for boolean types
//
//@CLASSES:
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for boolean types.
//

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
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterintegral.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'


namespace bsl {
// FORMATTER SPECIALIZATIONS

template <class t_CHAR>
struct formatter<bool, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

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
    typename t_FORMAT_CONTEXT::iterator format(bool            v,
                                               t_FORMAT_CONTEXT& fc) const
    {
        const char *buf = v ? "1" : "0";

        return BloombergLP::bslfmt::Formatter_CharUtils<
            t_CHAR>::outputFromChar(buf, buf + 1, fc.out());
    }
};

}


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
