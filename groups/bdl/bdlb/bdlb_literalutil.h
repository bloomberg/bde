// bdlb_literalutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_LITERALUTIL
#define INCLUDED_BDLB_LITERALUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility routines for programming language literals.
//
//@CLASSES:
//  bdlb::LiteralUtil: namespace for utilies for programming language literals
//
//@SEE_ALSO: hslc_lexer, hslc_parser
//
//@DESCRIPTION: This component provides a namespace, 'bdlb::LiteralUtil', for a
// set of utility routines that operate on (programming language) literals.

#include <bdlscm_version.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdlb {

                             // ==================
                             // struct LiteralUtil
                             // ==================

struct LiteralUtil {
    // CLASS METHODS
    static void createQuotedEscapedCString(bsl::string              *result,
                                           const bslstl::StringRef&  input);
        // Load into the specified 'result' string the '"' delimited and
        // escaped C/C++ string literal equivalent representing the same value
        // as that of the specified 'input' string.  When the C string literal
        // equivalent is translated by a compiler having C-compatible string
        // literals, it will result in a string identical to the 'input'
        // string.  Note that this code uses the (ASCII) '\' character, rather
        // than Unicode code points for escapes.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
