// bsl_regex.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_REGEX
#define INCLUDED_BSL_REGEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bslstl_iterator.h> // std::begin/end/rbegin/rend, etc

#include <regex>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
//@SEE_ALSO: package bos+stdhdrs in the bos package group
    using std::regex_match;
    using std::regex_search;
    using std::regex_replace;
    using std::regex_iterator;
    using std::cregex_iterator;
    using std::wcregex_iterator;
    using std::sregex_iterator;
    using std::wsregex_iterator;
    using std::regex_token_iterator;
    using std::basic_regex;
    using std::match_results;
    using std::sub_match;
    using std::regex_traits;
    using std::regex_error;
    using std::regex;
    using std::wregex;
    using std::cmatch;
    using std::wcmatch;
    using std::smatch;
    using std::wsmatch;
    using std::csub_match;
    using std::wcsub_match;
    using std::ssub_match;
    using std::wssub_match;

    namespace regex_constants {
        using std::regex_constants::icase;
        using std::regex_constants::nosubs;
        using std::regex_constants::optimize;
        using std::regex_constants::collate;
        using std::regex_constants::ECMAScript;
        using std::regex_constants::basic;
        using std::regex_constants::extended;
        using std::regex_constants::awk;
        using std::regex_constants::grep;
        using std::regex_constants::egrep;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
// As of 4/2022:
//  MSVC has not yet implemented multiline
//  libstdc++ has implemented it, but not shipped it in any release.
//  libc++ is shipping it.
//
//        using std::regex_constants::multiline;
#endif
        using std::regex_constants::match_default;
        using std::regex_constants::match_not_bol;
        using std::regex_constants::match_not_eol;
        using std::regex_constants::match_not_bow;
        using std::regex_constants::match_not_eow;
        using std::regex_constants::match_any;
        using std::regex_constants::match_not_null;
        using std::regex_constants::match_continuous;
        using std::regex_constants::match_prev_avail;
        using std::regex_constants::format_default;
        using std::regex_constants::format_sed;
        using std::regex_constants::format_no_copy;
        using std::regex_constants::format_first_only;
        using std::regex_constants::error_collate;
        using std::regex_constants::error_ctype;
        using std::regex_constants::error_escape;
        using std::regex_constants::error_backref;
        using std::regex_constants::error_brack;
        using std::regex_constants::error_paren;
        using std::regex_constants::error_brace;
        using std::regex_constants::error_badbrace;
        using std::regex_constants::error_range;
        using std::regex_constants::error_space;
        using std::regex_constants::error_badrepeat;
        using std::regex_constants::error_complexity;
        using std::regex_constants::error_stack;
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
