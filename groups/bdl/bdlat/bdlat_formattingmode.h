// bdlat_formattingmode.h                                             -*-C++-*-
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
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdlat_FormattingMode' struct provided in this component
// defines the symbolic constants for all the formatting modes supported by the
// 'bdlat' framework.  The formatting modes are separated into two categories:
// a bit-field for the original type (i.e., schema type), and a number of
// flags.
//
///Original Type (schema type)
///---------------------------
// This bit-field defines the formatting modes that are derived based on the
// type of an element, as defined in the schema.  The available modes are:
//..
//  Formatting Mode         Description
//  ---------------         -----------
//  BDLAT_DEFAULT           Use a default formatting mode.
//  BDLAT_DEC               Use the decimal format.
//  BDLAT_HEX               Use the hexadecimal format.
//  BDLAT_BASE64            Use the base64 format.
//  BDLAT_TEXT              Use the text format.
//..
//
///Formatting Flags
///----------------
// The following bitwise flags can be applied to a schema element:
//..
//  Formatting Mode         Description
//  ---------------         -----------
//  BDLAT_UNTAGGED          Use untagged formatting (for anonymous choices).
//  BDLAT_ATTRIBUTE         Use attribute formatting (for XSD attributes).
//  BDLAT_SIMPLE_CONTENT    Use simple content formatting (for XSD simple
//                          content types).
//  BDLAT_NILLABLE          Use nillable formatting (for XSD 'nillable'
//                          option).
//  BDLAT_LIST              Use the list format (this is used for arrays).
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
//ARB:ENUM 78
        BDLAT_DEFAULT   = 0x0,  // default formatting mode
//ARB:ENUM 79
        BDLAT_DEC       = 0x1,  // use decimal format
//ARB:ENUM 80
        BDLAT_HEX       = 0x2,  // use hexadecimal format
//ARB:ENUM 81
        BDLAT_BASE64    = 0x3,  // use base64 format
//ARB:ENUM 82
        BDLAT_TEXT      = 0x4,  // use text format
//ARB:ENUM 83
        BDLAT_TYPE_MASK = 0x7,  // mask for type bit-field

        // formatting flags
//ARB:ENUM 86
        BDLAT_UNTAGGED       = 0x00010000,  // use untagged formatting
//ARB:ENUM 87
        BDLAT_ATTRIBUTE      = 0x00020000,  // use attribute formatting
//ARB:ENUM 88
        BDLAT_SIMPLE_CONTENT = 0x00040000,  // use simple content formatting
//ARB:ENUM 89
        BDLAT_NILLABLE       = 0x00080000,  // use nillable formatting
//ARB:ENUM 90
        BDLAT_LIST           = 0x00100000,  // use list format (for arrays)
//ARB:ENUM 91
        BDLAT_FLAGS_MASK     = 0x001F0000   // mask for formatting flags

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , DEFAULT           = BDLAT_DEFAULT
      , DEC               = BDLAT_DEC
      , HEX               = BDLAT_HEX
      , BASE64            = BDLAT_BASE64
      , TEXT              = BDLAT_TEXT
      , TYPE_MASK         = BDLAT_TYPE_MASK
      , IS_UNTAGGED       = BDLAT_UNTAGGED
      , IS_ATTRIBUTE      = BDLAT_ATTRIBUTE
      , IS_SIMPLE_CONTENT = BDLAT_SIMPLE_CONTENT
      , IS_NILLABLE       = BDLAT_NILLABLE
      , IS_LIST           = BDLAT_LIST
      , FLAGS_MASK        = BDLAT_FLAGS_MASK

      , BDLAT_IS_UNTAGGED       = BDLAT_UNTAGGED
      , BDLAT_IS_ATTRIBUTE      = BDLAT_ATTRIBUTE
      , BDLAT_IS_SIMPLE_CONTENT = BDLAT_SIMPLE_CONTENT
      , BDLAT_IS_NILLABLE       = BDLAT_NILLABLE
      , BDLAT_IS_LIST           = BDLAT_LIST
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#ifndef BDE_OPENSOURCE_PUBLICATION
      , BDEAT_DEFAULT   = BDLAT_DEFAULT
      , BDEAT_DEC       = BDLAT_DEC
      , BDEAT_HEX       = BDLAT_HEX
      , BDEAT_BASE64    = BDLAT_BASE64
      , BDEAT_TEXT      = BDLAT_TEXT
      , BDEAT_TYPE_MASK = BDLAT_TYPE_MASK
      , BDEAT_UNTAGGED       = BDLAT_UNTAGGED
      , BDEAT_ATTRIBUTE      = BDLAT_ATTRIBUTE
      , BDEAT_SIMPLE_CONTENT = BDLAT_SIMPLE_CONTENT
      , BDEAT_NILLABLE       = BDLAT_NILLABLE
      , BDEAT_LIST           = BDLAT_LIST
      , BDEAT_FLAGS_MASK     = BDLAT_FLAGS_MASK
      , BDEAT_IS_UNTAGGED       = BDLAT_UNTAGGED
      , BDEAT_IS_ATTRIBUTE      = BDLAT_ATTRIBUTE
      , BDEAT_IS_SIMPLE_CONTENT = BDLAT_SIMPLE_CONTENT
      , BDEAT_IS_NILLABLE       = BDLAT_NILLABLE
      , BDEAT_IS_LIST           = BDLAT_LIST
#endif  // BDE_OPENSOURCE_PUBLICATION
    };
};

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
