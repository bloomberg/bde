// balxml_formatterwhitespacetype.h                                   -*-C++-*-
#ifndef INCLUDED_BALXML_FORMATTERWHITESPACETYPE
#define INCLUDED_BALXML_FORMATTERWHITESPACETYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of whitespace options for 'balxml_formatter'.
//
//@CLASSES:
//  balxml::FormatterWhitespaceType: options when outputting textual data
//
//@SEE_ALSO: balxml_formatter
//
//@DESCRIPTION: This component provides a namespace,
// 'balxml::FormatterWhitespaceType', for the 'enum' type
// 'FormatterWhitespaceType::Enum', which enumerates the set of options
// available when outputting textual data of an XML element using the XML
// formatter provided by the 'balxml' package.

#include <balscm_version.h>

namespace BloombergLP {
namespace balxml {

                       // ==============================
                       // struct FormatterWhitespaceType
                       // ==============================

struct FormatterWhitespaceType {
    // This 'struct' describes options available when outputting textual data
    // of an element between its pair of opening and closing tags.

    // TYPES
    enum Enum {
        e_PRESERVE_WHITESPACE,  // data is output as is

        e_WORDWRAP,  // data may be wrapped if output otherwise exceeds the
                     // wrap column

        e_WORDWRAP_INDENT,  // in addition to allowing wrapping, indent
                            // properly before continuing to output on the next
                            // line after wrapping

        e_NEWLINE_INDENT  // in addition to allowing wrapping and indentation,
                          // the tags do not share their respective lines with
                          // data
    };
};

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_BALXML_FORMATTERWHITESPACETYPE

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
