// bdlat_formattingmode.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#define INCLUDED_BDLAT_FORMATTINGMODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide formatting mode constants.
//
//@CLASSES:
//  bdlat_FormattingMode: scope for formatting mode constants
//
//@SEE_ALSO: balxml_typesprintutil balxml_typesparserutil
//
//@DESCRIPTION: The 'bdlat_FormattingMode' struct provided in this component
// defines the symbolic constants for all the formatting modes supported by the
// 'bdlat' framework.  The formatting modes are separated into two categories:
// a bit-field for the original type (i.e., schema type), and a number of
// flags.
//
///Original Type (Schema Type)
///---------------------------
// This bit-field defines the formatting modes that are derived based on the
// type of an element, as defined in the schema.  The available modes are:
//..
//  Formatting Mode         Description
//  ---------------         -----------
//  e_DEFAULT               Use a default formatting mode.
//  e_DEC                   Use the decimal format.
//  e_HEX                   Use the hexadecimal format.
//  e_BASE64                Use the base64 format.
//  e_TEXT                  Use the text format.
//..
//
///Formatting Flags
///----------------
// The following bitwise flags can be applied to a schema element:
//..
//  Formatting Mode         Description
//  ---------------         -----------
//  e_UNTAGGED              Use untagged formatting (for anonymous choices).
//  e_ATTRIBUTE             Use attribute formatting (for XSD attributes).
//  e_SIMPLE_CONTENT        Use simple content formatting (for XSD simple
//                          content types).
//  e_NILLABLE              Use nillable formatting (for XSD 'nillable'
//                          option).
//  e_LIST                  Use the list format (this is used for arrays).
//..
//
///Usage
///-----
// Due to the low-level nature of this component, a usage example is not
// necessary.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

namespace BloombergLP {

struct bdlat_FormattingMode {
    // This struct contains the symbolic constants for the formatting modes
    // supported by the 'bdlat' framework.

    // CONSTANTS
    enum {
        // bit-field: original type (i.e., schema type)
        e_DEFAULT   = 0x0,  // default formatting mode
        e_DEC       = 0x1,  // use decimal format
        e_HEX       = 0x2,  // use hexadecimal format
        e_BASE64    = 0x3,  // use base64 format
        e_TEXT      = 0x4,  // use text format
        e_TYPE_MASK = 0x7,  // mask for type bit-field

        // formatting flags
        e_UNTAGGED       = 0x00010000,  // use untagged formatting
        e_ATTRIBUTE      = 0x00020000,  // use attribute formatting
        e_SIMPLE_CONTENT = 0x00040000,  // use simple content formatting
        e_NILLABLE       = 0x00080000,  // use nillable formatting
        e_LIST           = 0x00100000,  // use list format (for arrays)
        e_FLAGS_MASK     = 0x001F0000   // mask for formatting flags

    };
};

}  // close enterprise namespace

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
