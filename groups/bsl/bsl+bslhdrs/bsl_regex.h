// bsl_regex.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_REGEX
#define INCLUDED_BSL_REGEX

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <regex>

namespace bsl {

    using native_std::regex_match;
    using native_std::regex_search;
    using native_std::regex_replace;
    using native_std::regex_iterator;
    using native_std::regex_token_iterator;
    using native_std::basic_regex;
    using native_std::match_results;
    using native_std::sub_match;
    using native_std::regex_traits;
    using native_std::regex_error;
    using native_std::regex;
    using native_std::wregex;
    using native_std::cmatch;
    using native_std::wcmatch;
    using native_std::smatch;
    using native_std::wsmatch;
    using native_std::csub_match;
    using native_std::wcsub_match;
    using native_std::ssub_match;
    using native_std::wssub_match;
    using native_std::begin;
    using native_std::end;
    namespace regex_constants {
        using native_std::regex_constants::icase;
        using native_std::regex_constants::nosubs;
        using native_std::regex_constants::optimize;
        using native_std::regex_constants::collate;
        using native_std::regex_constants::ECMAScript;
        using native_std::regex_constants::basic;
        using native_std::regex_constants::extended;
        using native_std::regex_constants::awk;
        using native_std::regex_constants::grep;
        using native_std::regex_constants::egrep;
        using native_std::regex_constants::match_default;
        using native_std::regex_constants::match_not_bol;
        using native_std::regex_constants::match_not_eol;
        using native_std::regex_constants::match_not_bow;
        using native_std::regex_constants::match_not_eow;
        using native_std::regex_constants::match_any;
        using native_std::regex_constants::match_not_null;
        using native_std::regex_constants::match_continuous;
        using native_std::regex_constants::match_prev_avail;
        using native_std::regex_constants::format_default;
        using native_std::regex_constants::format_sed;
        using native_std::regex_constants::format_no_copy;
        using native_std::regex_constants::format_first_only;
        using native_std::regex_constants::error_collate;
        using native_std::regex_constants::error_ctype;
        using native_std::regex_constants::error_escape;
        using native_std::regex_constants::error_backref;
        using native_std::regex_constants::error_brack;
        using native_std::regex_constants::error_paren;
        using native_std::regex_constants::error_brace;
        using native_std::regex_constants::error_badbrace;
        using native_std::regex_constants::error_range;
        using native_std::regex_constants::error_space;
        using native_std::regex_constants::error_badrepeat;
        using native_std::regex_constants::error_complexity;
        using native_std::regex_constants::error_stack;
    }  // close namespace regex_constants

}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
