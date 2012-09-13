// bdeat_formattingmode.h                                             -*-C++-*-
#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#define INCLUDED_BDEAT_FORMATTINGMODE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide formatting mode constants.
//
//@CLASSES:
//  bdeat_FormattingMode: scope for formatting mode constants
//
//@SEE_ALSO: baexml_typesprintutil baexml_typesparserutil
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdeat_FormattingMode' struct provided in this component
// defines the symbolic constants for all the formatting modes supported by the
// 'bdeat' framework.  The formatting modes are separated into two categories:
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
//  BDEAT_DEFAULT           Use a default formatting mode.
//  BDEAT_DEC               Use the decimal format.
//  BDEAT_HEX               Use the hexadecimal format.
//  BDEAT_BASE64            Use the base64 format.
//  BDEAT_TEXT              Use the text format.
//..
//
///Formatting Flags
///----------------
// The following bitwise flags can be applied to a schema element:
//..
//  Formatting Mode         Description
//  ---------------         -----------
//  BDEAT_UNTAGGED          Use untagged formatting (for anonymous choices).
//  BDEAT_ATTRIBUTE         Use attribute formatting (for XSD attributes).
//  BDEAT_SIMPLE_CONTENT    Use simple content formatting (for XSD simple
//                          content types).
//  BDEAT_NILLABLE          Use nillable formatting (for XSD 'nillable'
//                          option).
//  BDEAT_LIST              Use the list format (this is used for arrays).
//..
//
///Usage
///-----
// Due to the low-level nature of this component, a usage example is not
// necessary.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

struct bdeat_FormattingMode {
    // This struct contains the symbolic constants for the formatting modes
    // supported by the 'bdeat' framework.

    // CONSTANTS
    enum {
        // bit-field: original type (i.e., schema type)
        BDEAT_DEFAULT   = 0x0,  // default formatting mode
        BDEAT_DEC       = 0x1,  // use decimal format
        BDEAT_HEX       = 0x2,  // use hexadecimal format
        BDEAT_BASE64    = 0x3,  // use base64 format
        BDEAT_TEXT      = 0x4,  // use text format
        BDEAT_TYPE_MASK = 0x7,  // mask for type bit-field

        // formatting flags
        BDEAT_UNTAGGED       = 0x00010000,  // use untagged formatting
        BDEAT_ATTRIBUTE      = 0x00020000,  // use attribute formatting
        BDEAT_SIMPLE_CONTENT = 0x00040000,  // use simple content formatting
        BDEAT_NILLABLE       = 0x00080000,  // use nillable formatting
        BDEAT_LIST           = 0x00100000,  // use list format (for arrays)
        BDEAT_FLAGS_MASK     = 0x001F0000   // mask for formatting flags

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , DEFAULT           = BDEAT_DEFAULT
      , DEC               = BDEAT_DEC
      , HEX               = BDEAT_HEX
      , BASE64            = BDEAT_BASE64
      , TEXT              = BDEAT_TEXT
      , TYPE_MASK         = BDEAT_TYPE_MASK
      , IS_UNTAGGED       = BDEAT_UNTAGGED
      , IS_ATTRIBUTE      = BDEAT_ATTRIBUTE
      , IS_SIMPLE_CONTENT = BDEAT_SIMPLE_CONTENT
      , IS_NILLABLE       = BDEAT_NILLABLE
      , IS_LIST           = BDEAT_LIST
      , FLAGS_MASK        = BDEAT_FLAGS_MASK

      , BDEAT_IS_UNTAGGED       = BDEAT_UNTAGGED
      , BDEAT_IS_ATTRIBUTE      = BDEAT_ATTRIBUTE
      , BDEAT_IS_SIMPLE_CONTENT = BDEAT_SIMPLE_CONTENT
      , BDEAT_IS_NILLABLE       = BDEAT_NILLABLE
      , BDEAT_IS_LIST           = BDEAT_LIST
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
};

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
